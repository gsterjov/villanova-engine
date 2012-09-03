/***************************************************************************
 *            stream.h
 *
 *  Sat Dec 20 19:05:42 2008
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
 
#ifndef V_STREAM_H_
#define V_STREAM_H_


#include <villanova-engine/error.h>
#include <villanova-engine/codec.h>



typedef struct _VStream VStream;


/**
 * VStream:
 * @id: a unique stream id determined by the media format.
 * @codec: a #VCodec capable of decoding the stream's frames.
 *
 * Contains details of a specific media stream and all the components required
 * to parse and decode it.
 */
struct _VStream
{
	int id;
	VCodec *codec;
	
	
	/* audio specific */
	int channels;
	int sample_rate;
	VSampleFormat sample_format;
	
	
	/* video specific */
	int width;
	int height;
	VPixelFormat pixel_format;
};



VStream *v_stream_new  (int id, VCodecID codec_id, VError *error);
void     v_stream_free (VStream *stream);



#endif /* V_STREAM_H_ */
 
