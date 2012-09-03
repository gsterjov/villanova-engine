/***************************************************************************
 *            codec.h
 *
 *  Sat Dec 20 21:41:20 2008
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
 
#ifndef V_CODEC_H_
#define V_CODEC_H_



#include <villanova-engine/error.h>
#include <villanova-engine/frame.h>
#include <villanova-engine/codec-types.h>
#include <villanova-engine/demuxer.h>
#include <villanova-engine/queue.h>
#include <stdbool.h>



typedef struct _VCodec VCodec;
typedef struct _VCodecProperties VCodecProperties;


/**
 * VCodec:
 * @type: the type of codec.
 * @id: the codec id.
 *
 * Contains the relevant components to decode a media stream.
 */
struct _VCodec
{
	VCodecType type;
	VCodecID id;
	
	
	/*< interface methods >*/
	void    (* parse)  (VCodec *codec, VPacket *packet, VQueue *frames);
	VFrame *(* decode) (VCodec *codec, VFrame *frame, VError *error);
	
	
	VCodecProperties *(* properties) (VCodec *codec);
};




struct _VCodecProperties
{
	int channels;
	int sample_rate;
	VSampleFormat sample_format;
	
	int width;
	int height;
	VPixelFormat pixel_format;
};




VCodec *v_codec_new  (VCodecID id, VError *error);
void    v_codec_free (VCodec *codec);


void    v_codec_parse  (VCodec *codec, VPacket *packet, VQueue *frames);
VFrame *v_codec_decode (VCodec *codec, VFrame *frame, VError *error);



VCodecProperties *v_codec_properties (VCodec *codec);





#endif /* V_CODEC_H_ */
 
