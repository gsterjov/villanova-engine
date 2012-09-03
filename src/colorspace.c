/***************************************************************************
 *            colorspace.c
 *
 *  Apr 9, 2009 2:35:45 PM
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


#include "config.h"

#include "colorspace.h"
#include "mem.h"


/* FIXME: colorspace should be based on modules
 * instead of directly using libavcodec */
#ifdef HAVE_LIBAVCODEC_AVCODEC_H
	#include <libavcodec/avcodec.h>
	#include <libswscale/swscale.h>
#else
	#include <ffmpeg/avcodec.h>
	#include <ffmpeg/swscale.h>
#endif




/*
 * VColorspacePriv:
 * @demuxer: the demuxer.
 * @buffer: the input buffer.
 *
 * Private structure for #VColorspace.
 */
struct _VColorspacePriv
{
	VPixelFormat src;
	VPixelFormat dest;
	
	int width;
	int height;
	
	
	AVPicture *pic;
	struct SwsContext *convert_ctx;
};




/**
 * v_colorspace_new:
 * @id: the codec type to handle.
 * @error: a #VError, or %NULL.
 *
 * Creates a new #VCodec which can handle @id type codecs.
 *
 * Returns: a #VColorspace structure if successful, %NULL otherwise.
 */
VColorspace *
v_colorspace_new (VPixelFormat src,
                  VPixelFormat dest,
                  int width,
                  int height,
                  VError *error)
{
	VColorspace     *ret = v_new (VColorspace);
	VColorspacePriv *priv = v_new (VColorspacePriv);
	
	
	/* default values */
	priv->src    = src;
	priv->dest   = dest;
	priv->width  = width;
	priv->height = height;
	
	ret->priv = priv;
	
	
	
	enum PixelFormat pix_src = PIX_FMT_YUV420P;
	enum PixelFormat pix_dest = PIX_FMT_YUV420P;
	
	
	if (src == V_PIXEL_FORMAT_RGB32)
		pix_src = PIX_FMT_RGB32;
	
	if (dest == V_PIXEL_FORMAT_RGB32)
		pix_dest = PIX_FMT_RGB32;
		
	
	
	
	/* setup the conversion and scale context */
	priv->convert_ctx = sws_getContext (width, height, pix_src,
			width, height, pix_dest,
			SWS_BICUBIC, NULL, NULL, NULL);

	


	priv->pic = (AVPicture *) avcodec_alloc_frame ();

	int size = avpicture_get_size (PIX_FMT_YUV420P, width, height);

	uint8_t *buf = v_mallocz (size);

	avpicture_fill (priv->pic, buf, PIX_FMT_YUV420P, width, height);
	
	
	
	return ret;
}





/**
 * v_colorspace_free:
 * @colorspace: a #VColorspace to free.
 *
 * Free's @colorspace and its contents.
 */
void
v_colorspace_free (VColorspace *colorspace)
{
	v_free (colorspace->priv);
	v_free (colorspace);
}





/**
 * v_colorspace_convert:
 * @colorspace: a #VColorspace.
 * @frame: a #VFrame to convert.
 *
 * Converts @frame to the specified colorspace/pixel format.
 * 
 * Returns: a #VFrame if successful, %NULL otherwise.
 */
VFrame *
v_colorspace_convert (VColorspace *colorspace, VFrame *frame)
{
	VColorspacePriv *priv = colorspace->priv;
	

	if (frame->type == V_FRAME_TYPE_VIDEO)
	{
		VFrameVideo *raw   = V_FRAME_VIDEO (frame);
		VFrameVideo *video = v_frame_video_new ();


		/* convert frame and scale it */
		sws_scale (priv->convert_ctx,
				raw->data,
				raw->linesize,
				0, priv->height,
				priv->pic->data,
				priv->pic->linesize);



		video->data[0] = priv->pic->data[0];
		video->data[1] = priv->pic->data[1];
		video->data[2] = priv->pic->data[2];
		video->data[3] = priv->pic->data[3];

		video->linesize[0] = priv->pic->linesize[0];
		video->linesize[1] = priv->pic->linesize[1];
		video->linesize[2] = priv->pic->linesize[2];
		video->linesize[3] = priv->pic->linesize[3];
		

		return V_FRAME (video);
	}
	
	
	
	else
	{
		VFrameSubtitle *raw = V_FRAME_SUBTITLE (frame);
		VFrameSubtitle *sub = v_frame_subtitle_new ();


		/* convert frame and scale it */
		sws_scale (priv->convert_ctx,
				raw->data,
				raw->linesize,
				0, priv->height,
				priv->pic->data,
				priv->pic->linesize);


		
		sub->x = raw->x;
		sub->y = raw->y;
		sub->w = raw->w;
		sub->h = raw->h;
		

		sub->data[0] = priv->pic->data[0];
		sub->data[1] = priv->pic->data[1];
		sub->data[2] = priv->pic->data[2];
		sub->data[3] = priv->pic->data[3];

		sub->linesize[0] = priv->pic->linesize[0];
		sub->linesize[1] = priv->pic->linesize[1];
		sub->linesize[2] = priv->pic->linesize[2];
		sub->linesize[3] = priv->pic->linesize[3];
		
		
		return V_FRAME (sub);
	}
	
}



