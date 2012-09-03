/***************************************************************************
 *            frame.h
 *
 *  Sun Dec 14 18:29:22 2008
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
 
#ifndef V_FRAME_H_
#define V_FRAME_H_


#include <stdint.h>


/* convenience casting macros */
#define V_FRAME(o)       ((VFrame *) o)
#define V_FRAME_RAW(o)   ((VFrameRaw *) o)
#define V_FRAME_AUDIO(o) ((VFrameAudio *) o)
#define V_FRAME_VIDEO(o) ((VFrameVideo *) o)
#define V_FRAME_SUBTITLE(o) ((VFrameSubtitle *) o)



typedef enum _VFrameType VFrameType;

/* frame types */
typedef struct _VFrame      VFrame;
typedef struct _VFrameRaw   VFrameRaw;
typedef struct _VFrameAudio VFrameAudio;
typedef struct _VFrameVideo VFrameVideo;
typedef struct _VFrameSubtitle VFrameSubtitle;



/**
 * VFrameType:
 * @V_FRAME_TYPE_RAW: raw frame.
 * @V_FRAME_TYPE_AUDIO: decoded audio frame.
 * @V_FRAME_TYPE_VIDEO: decoded video frame.
 * @V_FRAME_TYPE_SUBTITLE: decoded subtitle frame.
 *
 * The type of the frame.
 */
enum _VFrameType
{
	V_FRAME_TYPE_RAW,
	V_FRAME_TYPE_AUDIO,
	V_FRAME_TYPE_VIDEO,
	V_FRAME_TYPE_SUBTITLE
};



/**
 * VFrame:
 * @type: the type of the frame.
 *
 * The generic structure for all frames.
 */
struct _VFrame
{
	VFrameType type;
};



/**
 * VFrameRaw:
 * @stream_id: the ID of the stream the frame originated from.
 * @length: the size of the frame data.
 * @data: raw frame data.
 * @pts: presentation timestamp.
 * @dts: decoding timestamp.
 *
 * A complete frame belonging to the stream specified by @stream_id.
 */
struct _VFrameRaw
{
	VFrame parent;
	
	int stream_id;
	
	int length;
	uint8_t *data;
	
	int64_t pts;
	int64_t dts;
};



/**
 * VFrameAudio:
 * @length: the size of the frame data.
 * @samples: decoded frame data.
 *
 * A decoded audio frame.
 */
struct _VFrameAudio
{
	VFrame parent;
	
	int length;
	int16_t *samples;
};



/**
 * VFrameVideo:
 * @length: the size of the frame data.
 * @data: decoded frame data.
 *
 * A decoded video frame.
 */
struct _VFrameVideo
{
	VFrame parent;
	
	int length;
	int linesize[4];
	uint8_t *data[4];
};



/**
 * VFrameSubtitle:
 * @length: the size of the frame data.
 * @data: decoded frame data.
 *
 * A decoded video frame.
 */
struct _VFrameSubtitle
{
	VFrame parent;
	
	int x, y;
	int w, h;
	
	int linesize[4];
	uint8_t *data[4];
};





VFrameRaw   *v_frame_raw_new   (int size);
VFrameAudio *v_frame_audio_new (int size);
VFrameVideo *v_frame_video_new (void);
VFrameSubtitle *v_frame_subtitle_new (void);


void v_frame_free (VFrame *frame);



#endif /* V_FRAME_H_ */
 
