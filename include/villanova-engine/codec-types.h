/***************************************************************************
 *            codec-types.h
 *
 *  Tue Dec 30 16:58:37 2008
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
 
#ifndef V_CODEC_TYPES_H_
#define V_CODEC_TYPES_H_


#define V_FOURCC(a,b,c,d) ((a << 24) | (b << 16) | (c << 8) | d)



typedef enum _VSampleFormat VSampleFormat;
typedef enum _VPixelFormat  VPixelFormat;
typedef enum _VCodecID      VCodecID;
typedef enum _VCodecType    VCodecType;





/**
 * VSampleFormat:
 * @V_SAMPLE_FORMAT_UNKNOWN: unsupported sample format.
 * @V_SAMPLE_FORMAT_S16: signed 16 bits.
 *
 * The sample format of the audio codec.
 */
enum _VSampleFormat
{
	V_SAMPLE_FORMAT_UNKNOWN,
	V_SAMPLE_FORMAT_S16
};




/**
 * VPixelFormat:
 * @V_PIXEL_FORMAT_UNKNOWN: unsupported pixel format.
 * @V_PIXEL_FORMAT_YUV420: YCbCr 4:2:0.
 *
 * The pixel format of the video codec.
 */
enum _VPixelFormat
{
	V_PIXEL_FORMAT_UNKNOWN,
	V_PIXEL_FORMAT_YUV420,
	V_PIXEL_FORMAT_RGB32
};




/**
 * VCodecType:
 * @V_CODEC_TYPE_UNKNOWN: unsupported codec type.
 * @V_CODEC_TYPE_AUDIO: audio codec type.
 * @V_CODEC_TYPE_VIDEO: video codec type.
 * @V_CODEC_TYPE_SUBTITLE: subtitle codec type.
 *
 * The type of the codec.
 */
enum _VCodecType
{
	V_CODEC_TYPE_UNKNOWN,
	V_CODEC_TYPE_AUDIO,
	V_CODEC_TYPE_VIDEO,
	V_CODEC_TYPE_SUBTITLE
};



/**
 * VCodecID:
 * @V_CODEC_ID_UNKNOWN: unsupported codec.
 * @V_CODEC_ID_MPEG2: MPEG-2 video codec.
 * @V_CODEC_ID_MP3: MPEG layer 3 audio codec.
 * @V_CODEC_ID_AC3: AC3 audio codec.
 * @V_CODEC_ID_DTS: DTS audio codec.
 * @V_CODEC_ID_SUBPIC: DVD subtitle codec.
 *
 * Available codecs. Values are determined by the codec's FourCC.
 */
enum _VCodecID
{
	V_CODEC_ID_UNKNOWN = 0,
	
	/* video codecs */ 
	V_CODEC_ID_MPEG2 = V_FOURCC ('M','P','2','V'),
	
	/* audio codecs */
	V_CODEC_ID_MP3 = V_FOURCC ('M','P','3',' '),
	V_CODEC_ID_AC3 = V_FOURCC ('A','C','3',' '),
	V_CODEC_ID_DTS = V_FOURCC ('D','T','s',' '),
	
	/* subtitle codecs */
	V_CODEC_ID_SUBPIC = V_FOURCC ('S','U','B','P')
};




VCodecType v_codec_id_type (VCodecID id);

char *v_codec_id_string (VCodecID id);




#endif /* V_CODEC_TYPES_H_ */
 
