/***************************************************************************
 *            output.h
 *
 *  Feb 9, 2009 6:15:37 PM
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

#ifndef V_OUTPUT_H_
#define V_OUTPUT_H_


#include <villanova-engine/error.h>
#include <villanova-engine/frame.h>
#include <villanova-engine/stream.h>


typedef struct _VOutput VOutput;
typedef enum   _VOutputType VOutputType;



/**
 * VCodecType:
 * @V_CODEC_TYPE_UNKNOWN: unsupported codec type.
 * @V_CODEC_TYPE_AUDIO: audio codec type.
 * @V_CODEC_TYPE_VIDEO: video codec type.
 * @V_CODEC_TYPE_SUBTITLE: subtitle codec type.
 *
 * The type of the codec.
 */
enum _VOutputType
{
	V_OUTPUT_TYPE_UNKNOWN,
	V_OUTPUT_TYPE_AUDIO,
	V_OUTPUT_TYPE_VIDEO,
};




/**
 * VOutput:
 *
 * Outputs media frames to the appropriate device.
 */
struct _VOutput
{
	/*< interface methods >*/
	void (* write) (VOutput *output, VFrame *frame);
	void (* open)  (VOutput *output, VStream *stream);
	void (* close) (VOutput *output);
	
	void (* write_sub) (VOutput *output, VFrame *frame);
};




VOutput *v_output_new  (VOutputType output_type, VError *error);
void     v_output_free (VOutput *output);


void v_output_write (VOutput *output, VFrame *frame);
void v_output_open  (VOutput *output, VStream *stream);
void v_output_close (VOutput *output);


void v_output_write_sub (VOutput *output, VFrame *frame);


char *v_output_type_string (VOutputType output_type);


#endif /* V_OUTPUT_H_ */
