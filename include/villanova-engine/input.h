/***************************************************************************
 *            input.h
 *
 *  Sat Dec 13 19:32:01 2008
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
 
#ifndef V_INPUT_H_
#define V_INPUT_H_


#include <villanova-engine/error.h>
#include <villanova-engine/buffer.h>
#include <villanova-engine/frame.h>
#include <villanova-engine/stream.h>
#include <stdbool.h>


typedef struct _VInput     VInput;
typedef struct _VInputPriv VInputPriv;




/**
 * VNewStreamFunc:
 * @input: a #VInput.
 * @stream: the new #VStream.
 * @userdata: void* casted user data.
 *
 * Callback prototype for handling new stream events.
 */
typedef void VNewStreamFunc (VInput *input, VStream *stream, void *userdata);



/**
 * VEosFunc:
 * @input: a #VInput.
 * @userdata: void* casted user data.
 *
 * Callback prototype for handling end of stream events.
 */
typedef void VEosFunc (VInput *input, void *userdata);





/**
 * VInput:
 * @protocol: the input protocol.
 * @uri: the absolute path to the media source.
 * @eos: indicates whether end of stream has been reached.
 * @open: interface prototype to open a stream.
 * @close: interface prototype to close a stream.
 *
 * Handles reading from a media source. All input modules must inherit from
 * #VInput and should override the interface prototypes it needs. At the very
 * least all #VInput subclasses <emphasis>must</emphasis> implement @open 
 * and @close.
 */
struct _VInput
{
	char *protocol;
	char *uri;
	
	bool eos;
	
	
	/*< interface methods >*/
	VBuffer *(* open)  (VInput *input, VError *error);
	void     (* close) (VInput *input);
	
	
	/*< private >*/
	VInputPriv *priv;
};




VInput *v_input_new  (const char *protocol, const char *uri, VError *error);
void    v_input_free (VInput *input);


bool v_input_open  (VInput *input, VError *error);
void v_input_close (VInput *input);


VFrameRaw *v_input_read_frame (VInput *input, VError *error);




void v_input_register_new_stream (VInput *input, VNewStreamFunc *func, void *userdata);
void v_input_register_eos (VInput *input, VEosFunc *func, void *userdata);


void v_input_raise_new_stream (VInput *input, VStream *stream);
void v_input_raise_eos (VInput *input);



#endif /* V_INPUT_H_ */
 
