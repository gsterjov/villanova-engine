/***************************************************************************
 *            frame.c
 *
 *  Sun Dec 14 18:52:55 2008
 *  Copyright  2008  Goran Sterjov
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
 
 

#include "frame.h"
#include "mem.h"



/**
 * v_raw_frame_new:
 *
 * Creates a new #VFrameRaw with default values.
 *
 * Returns: a #VFrameRaw structure.
 */
VFrameRaw *
v_frame_raw_new (int size)
{
	VFrameRaw *ret = v_new (VFrameRaw);

	ret->parent.type = V_FRAME_TYPE_RAW;
	ret->length = size;
	ret->data = v_mallocz (size);

	return ret;
}




/**
 * v_audio_frame_new:
 *
 * Creates a new #VFrameAudio with default values.
 *
 * Returns: a #VFrameAudio structure.
 */
VFrameAudio *
v_frame_audio_new (int size)
{
	VFrameAudio *ret = v_new (VFrameAudio);
	
	ret->parent.type = V_FRAME_TYPE_AUDIO;
	ret->length = size;
	ret->samples = v_mallocz (size);
	
	return ret;
}




/**
 * v_video_frame_new:
 *
 * Creates a new #VFrameVideo with default values.
 *
 * Returns: a #VFrameVideo structure.
 */
VFrameVideo *
v_frame_video_new (void)
{
	VFrameVideo *ret = v_new (VFrameVideo);
	
	ret->parent.type = V_FRAME_TYPE_VIDEO;
	ret->length = 0;
	//ret->data = NULL;
	
	return ret;
}




/**
 * v_video_frame_new:
 *
 * Creates a new #VFrameVideo with default values.
 *
 * Returns: a #VFrameVideo structure.
 */
VFrameSubtitle *
v_frame_subtitle_new (void)
{
	VFrameSubtitle *ret = v_new (VFrameSubtitle);
	
	ret->parent.type = V_FRAME_TYPE_SUBTITLE;
	//ret->data = NULL;
	
	return ret;
}






/**
 * v_frame_free:
 * @frame: a #VFrame to free.
 *
 * Free's @frame and its contents.
 */
void
v_frame_free (VFrame *frame)
{
	
	/* free frame data */
	switch (frame->type)
	{
		case V_FRAME_TYPE_RAW:
			v_free (V_FRAME_RAW (frame)->data);
			break;
		
		case V_FRAME_TYPE_AUDIO:
			v_free (V_FRAME_AUDIO (frame)->samples);
			break;
			
		case V_FRAME_TYPE_VIDEO:
			//v_free (V_FRAME_VIDEO (frame)->data);
			break;
			
		case V_FRAME_TYPE_SUBTITLE:
			break;
	}
	
	
	/* destroy frame */
	v_free (frame);
}

