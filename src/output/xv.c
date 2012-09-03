/***************************************************************************
 *            xv.c
 *
 *  Feb 15, 2009 2:41:44 PM
 *  Copyright  2009  Goran Sterjov
 *  <goran.sterjov@gmail.com>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with main.c; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */


#include "output.h"
#include "mem.h"
#include <string.h>  /* memcpy */

#include <sys/ipc.h>
#include <sys/shm.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xv.h>
#include <X11/extensions/Xvlib.h>
#include <X11/extensions/XShm.h>



#define IMGFMT_YV12 (('2'<<24)|('1'<<16)|('V'<<8)|'Y')
#define IMGFMT_YUY2 (('2'<<24)|('Y'<<16)|('U'<<8)|'Y')



extern XvImage *XvShmCreateImage (Display*,
		XvPortID,
		int,
		char*,
		int,
		int,
		XShmSegmentInfo*);




typedef struct _VOutputXv VOutputXv;



/*
 * VOutputXv:
 * @fd: the file descriptor.
 * @length: the input buffer length.
 * @buffer: the input buffer.
 *
 * Private structure for #VOutputXv inheriting #VOutput.
 */
struct _VOutputXv
{
	VOutput parent;
	
	
	VFrameSubtitle *subpic;
	
	
	/* X11 window stuff */
	Display *display;
	Window   window;
	GC       gc;
	
	
	/* xv stuff */
	XvPortID         port;
	XvImage         *image;
	XShmSegmentInfo  shminfo;
};





#include <stdio.h>



/*
 * create_image:
 * @output: a #VOutput.
 * @frame: a #VFrame to write.
 *
 * Writes @frame to the Xvideo image buffer.
 */
static XvImage *
create_image (Display *display,
		XvPortID port,
		int width,
		int height,
		XShmSegmentInfo *shminfo)
{
	
	/* create shared memory image */
	XvImage *image = XvShmCreateImage (display,
			port,
			IMGFMT_YV12,
			0,
			width,
			height,
			shminfo);
	
	
	
	/* allocate shared memory */
	shminfo->shmid = shmget (IPC_PRIVATE, image->data_size, IPC_CREAT | 0777);
	shminfo->shmaddr = image->data = shmat (shminfo->shmid, 0, 0);
	shminfo->readOnly = False;
	
	
	/* attach the shared memory to the display */
	XShmAttach (display, shminfo);
	XSync (display, False);
	
	
	/* delete shared memory segment since the display is now attached to it.
	   the kernel will actually delete it once all clients are detached */
	shmctl (shminfo->shmid, IPC_RMID, 0);
	shminfo->shmid = -1;
	
	
	return image;
}




/*
 * v_output_xv_write:
 * @output: a #VOutput.
 * @frame: a #VFrame to write.
 *
 * Writes @frame to the Xvideo image buffer.
 */
static void
v_output_xv_write (VOutput *output, VFrame *frame)
{
	VOutputXv *self = (VOutputXv *) output;
	VFrameVideo *video = V_FRAME_VIDEO (frame);
	
	
	
	int height = self->image->height;
	
	
	memcpy (self->image->data + self->image->offsets[0], video->data[0], video->linesize[0] * height);
	memcpy (self->image->data + self->image->offsets[1], video->data[2], video->linesize[2] * (height / 2));
	memcpy (self->image->data + self->image->offsets[2], video->data[1], video->linesize[1] * (height / 2));
	
	
	
#if 0
	if (self->subpic)
	{
		memcpy (self->image->data + self->image->offsets[0], self->subpic->data[0], self->subpic->linesize[0] * self->subpic->h);
		memcpy (self->image->data + self->image->offsets[1], self->subpic->data[2], self->subpic->linesize[2] * (self->subpic->h / 2));
		memcpy (self->image->data + self->image->offsets[2], self->subpic->data[1], self->subpic->linesize[1] * (self->subpic->h / 2));
	}
#endif
	
	
	
	Window root;
	int x, y, w, h;
	int border, depth;


	XGetGeometry(self->display,
			self->window,
			&root,
			&x, &y,
			&w, &h,
			&border, &depth);

	
	
	/* display the xv image on the overlay */
	XvShmPutImage (self->display,
			self->port,
			self->window,
			self->gc,
			self->image,
			0, 0,
			self->image->width,
			self->image->height,
			0, 0,
			w, h,
			False);
	
	

	XSync (self->display, False);
}





static void
v_output_xv_write_sub (VOutput *output, VFrame *frame)
{
	VOutputXv *self = (VOutputXv *) output;
	self->subpic = V_FRAME_SUBTITLE (frame);
}





/*
 * v_output_xv_open:
 * @output: a #VOutput.
 * @frame: a #VFrame to write.
 *
 * Writes @frame to the ALSA buffer.
 */
static void
v_output_xv_open (VOutput *output, VStream *stream)
{
	VOutputXv *self = (VOutputXv *) output;
	
	
	/* create the shared video image */
	self->image = create_image (self->display,
			self->port,
			stream->width,
			stream->height,
			&self->shminfo);



	self->window = XCreateSimpleWindow (self->display,
			DefaultRootWindow (self->display),
			0, 0,
			stream->width,
			stream->height,
			0,
			CopyFromParent,
			CopyFromParent);


	/* create the gc */
	self->gc = XCreateGC (self->display, self->window, 0, 0);


	XMapWindow (self->display, self->window);
}




/*
 * v_output_xv_close:
 * @output: a #VOutput.
 * @frame: a #VFrame to write.
 *
 * Writes @frame to the ALSA buffer.
 */
static void
v_output_xv_close (VOutput *output)
{
	
}





/**
 * v_output_xv_new:
 *
 * Creates a new video output using Xvideo.
 *
 * Returns: a #VOutput structure.
 */
VOutput *
v_output_xv_new (void)
{
	VOutputXv *ret = v_new (VOutputXv);
	
	VOutput *output = (VOutput *) ret;
	
	
	
	/* get the default display */
	ret->window = 0;
	ret->display = XOpenDisplay (NULL);
	ret->gc = NULL;


	/* get the default adaptor port */
	int num_adaptors;
	XvAdaptorInfo *adaptors;


	XvQueryAdaptors (ret->display,
			DefaultRootWindow (ret->display),
			&num_adaptors,
			&adaptors);


	ret->port = adaptors[0].base_id;
	
	XvFreeAdaptorInfo(adaptors);
	
	
	
	/* set interface methods */
	output->write = v_output_xv_write;
	output->open  = v_output_xv_open;
	output->close = v_output_xv_close;
	
	output->write_sub = v_output_xv_write_sub;
	
	
	return output;
}

