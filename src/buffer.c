/***************************************************************************
 *            buffer.c
 *
 *  Wed Dec 17 18:40:34 2008
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
 
 
#include "buffer.h"
#include "mem.h"
#include <string.h>  /* memcpy */



#define MIN(a,b) ((a) > (b) ? (b) : (a))



/*
 * VBufferPriv:
 * @index: the current read index.
 * @length: the length of the buffer.
 * @data: the buffer data.
 * @fill_buffer: a callback to get new buffer data.
 * @user_data: user data to send with the callback.
 *
 * Private structure for #VBuffer which tracks the internal state of buffer
 * reading.
 */
struct _VBufferPriv
{
	int index;
	int length;
	uint8_t *data;
	
	
	FillBuffer *fill_buffer;
	SkipData   *skip_data;
	
	void *user_data;
};





/*
 * fill_buffer:
 * @buffer: a #VBuffer to fill.
 *
 * Fills the buffer with new data.
 */
static void
fill_buffer (VBuffer *buffer)
{
	VBufferPriv *priv = buffer->priv;
	
	priv->index  = 0;
	priv->length = priv->fill_buffer (priv->data, priv->user_data);
	
	
	/* reached EOS */
	if (priv->length == 0)
		buffer->eos = true;
}





/**
 * v_buffer_new:
 * @buffer: the buffer data.
 * @fill_buffer: a callback to fill the input buffer with new data.
 * @skip_data: a callback to skip data on the input buffer, or %NULL.
 * @user_data: user data to pass for various buffer callbacks, or %NULL.
 *
 * Creates a new input buffer.
 *
 * Returns: a #VBuffer structure.
 */
VBuffer *
v_buffer_new (uint8_t    *buffer,
			  FillBuffer *fill_buffer,
			  SkipData   *skip_data,
			  void       *user_data)
{
	VBuffer *ret = v_new (VBuffer);
	VBufferPriv *priv = v_new (VBufferPriv);
	
	
	/* default values */
	ret->priv = priv;
	
	priv->data        = buffer;
	priv->fill_buffer = fill_buffer;
	priv->skip_data   = skip_data;
	priv->user_data   = user_data;
	
	
	return ret;
}




/**
 * v_buffer_free:
 * @buffer: a #VBuffer to free.
 *
 * Free's @buffer and its contents.
 */
void
v_buffer_free (VBuffer *buffer)
{
	v_free (buffer->priv);
	v_free (buffer);
}





/**
 * v_buffer_skip:
 * @buffer: a #VBuffer to skip on.
 * @length: the amount of bytes to skip.
 *
 * Fills the buffer with new data.
 */
void
v_buffer_skip (VBuffer *buffer, int length)
{
	VBufferPriv *priv = buffer->priv;
	
	
	int len = priv->length - priv->index;
	
	
	/* need to seek */
	if (length > len)
	{
		/* input buffer supports skip */
		if (priv->skip_data)
		{
			length -= priv->skip_data (length, priv->user_data);
			fill_buffer (buffer);
		}
		
		/* we just read the rest of the length */
		if (length > 0)
		{
			uint8_t *tmp = v_malloc (length);
			v_buffer_read_bytes (buffer, tmp, length);
			v_free (tmp);
		}
	}
	
	
	/* get new data */
	else if (length == len)
		fill_buffer (buffer);
	
	
	/* skip within the buffer */
	else
		priv->index += length;
}





/**
 * v_buffer_read_bytes:
 * @buffer: a #VBuffer to read from.
 * @data: the data buffer to read to.
 * @length: the amount of bytes to read.
 *
 * Reads the next @length bytes in @buffer and copies it to @data.
 * @data must already be allocated with a minimum size of @length.
 *
 * Returns: the amount of bytes actually read.
 */
int
v_buffer_read_bytes (VBuffer *buffer, uint8_t *data, int length)
{
	VBufferPriv *priv = buffer->priv;
	
	
	int idx = 0;
	
	
	/* read until length is reached */
	while (idx < length)
	{
		
		/* read from the buffer */
		if (priv->index < priv->length && !buffer->eos)
		{
			int len = 0;
			int avail = 0;
			
			
			/* set the position to start copying from */
			uint8_t *src = priv->data + priv->index;
			uint8_t *dst = data + idx;
			
			
			/* get the amount of bytes to copy */
			avail = priv->length - priv->index;
			len  = MIN ((length - idx), avail);
			
			
			/* copy to the appropriate index */
			memcpy (dst, src, len);
			
			
			/* update indexes */
			priv->index += len;
			idx += len;
		}
		
		
		
		/* get more data if we ran out */
		if (priv->index >= priv->length)
			fill_buffer (buffer);
		
	}
	
	
	return idx;
}





/**
 * v_buffer_read_bits8:
 * @buffer: a #VBuffer to read from.
 *
 * Reads the next 8 bits in @buffer. If EOS is reached zero will be returned.
 *
 * Returns: an 8 bit integer.
 */
uint8_t
v_buffer_read_bits8 (VBuffer *buffer)
{
	VBufferPriv *priv = buffer->priv;
	
	
	/* get some more data */
	if (priv->index >= priv->length)
		fill_buffer (buffer);
	
	
	/* get byte */
	if (priv->index < priv->length)
		return priv->data[priv->index++];
	
	else
		return 0;  /* reached EOS */
}



/**
 * v_buffer_read_bits16:
 * @buffer: a #VBuffer to read from.
 *
 * Reads the next 16 bits in @buffer.
 *
 * Returns: a 16 bit integer.
 */
uint16_t
v_buffer_read_bits16 (VBuffer *buffer)
{
	uint16_t val = v_buffer_read_bits8 (buffer) << 8;
	return val | v_buffer_read_bits8 (buffer);
}



/**
 * v_buffer_read_bits24:
 * @buffer: a #VBuffer to read from.
 *
 * Reads the next 24 bits in @buffer.
 *
 * Returns: a 32 bit integer with the 24 least significant bits set.
 */
uint32_t
v_buffer_read_bits24 (VBuffer *buffer)
{
	uint32_t val = v_buffer_read_bits16 (buffer) << 8;
	return val | v_buffer_read_bits8 (buffer);
}



/**
 * v_buffer_read_bits32:
 * @buffer: a #VBuffer to read from.
 *
 * Reads the next 32 bits in @buffer.
 *
 * Returns: a 32 bit integer.
 */
uint32_t
v_buffer_read_bits32 (VBuffer *buffer)
{
	uint32_t val = v_buffer_read_bits16 (buffer) << 16;
	return val | v_buffer_read_bits16 (buffer);
}

