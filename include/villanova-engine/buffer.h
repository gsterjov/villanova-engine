/***************************************************************************
 *            buffer.h
 *
 *  Wed Dec 17 18:12:06 2008
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
 
#ifndef V_BUFFER_H_
#define V_BUFFER_H_


#include <stdbool.h>
#include <stdint.h>


typedef struct _VBuffer     VBuffer;
typedef struct _VBufferPriv VBufferPriv;



/**
 * FillBuffer:
 * @buffer: the input buffer needing new data.
 * @uesr_data: void* casted user data.
 *
 * Callback prototype for filling an input buffer with new data.
 *
 * Returns: the amount of bytes read.
 */
typedef int FillBuffer (uint8_t *buffer, void *user_data);



/**
 * SkipData:
 * @length: the amount bytes to skip.
 * @uesr_data: void* casted user data.
 *
 * Callback prototype for skipping @length amount of bytes.
 *
 * Returns: the amount of bytes skipped.
 */
typedef int SkipData (int length, void *user_data);




/**
 * VBuffer:
 * @eos: the buffer has reached the end of the stream.
 * @fill_buffer: a callback to fill the input buffer with new data.
 * @skip_data: a callback to skip data on the input buffer.
 *
 * Allows reading from an input buffer in an easy and seamless way.
 */
struct _VBuffer
{
	bool eos;
	
	
	/*< callback methods >*/
	FillBuffer *fill_buffer;
	SkipData   *skip_data;
	
	
	/*< private >*/
	VBufferPriv *priv;
};



VBuffer *v_buffer_new  (uint8_t    *buffer,
						FillBuffer *fill_buffer,
						SkipData   *skip_data,
						void       *user_data);


void v_buffer_free (VBuffer *buffer);
void v_buffer_skip (VBuffer *buffer, int length);


/* buffer reading functions */
int v_buffer_read_bytes  (VBuffer *buffer, uint8_t *data, int length);

uint8_t  v_buffer_read_bits8  (VBuffer *buffer);
uint16_t v_buffer_read_bits16 (VBuffer *buffer);
uint32_t v_buffer_read_bits24 (VBuffer *buffer);
uint32_t v_buffer_read_bits32 (VBuffer *buffer);



#endif /* V_BUFFER_H_ */
 
