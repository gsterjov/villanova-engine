/***************************************************************************
 *            stream.c
 *
 *  Sat Dec 20 19:09:56 2008
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
 
 
#include "stream.h"
#include "mem.h"




/**
 * v_stream_new:
 * @id: a unique stream ID.
 * @codec_id: a #VCodecID value indicating the codec type of the stream.
 * @error: a #VError, or %NULL.
 *
 * Creates a new #VStream.
 *
 * Returns: a #VStream structure if successful, %NULL otherwise.
 */
VStream *
v_stream_new (int id, VCodecID codec_id, VError *error)
{
	VStream *ret = v_new (VStream);
	
	
	/* default values */
	ret->id = id;
	
	
	/* create codec */
	ret->codec = v_codec_new (codec_id, error);
	
	/* no codec */
	if (ret->codec == NULL)
	{
		v_free (ret);
		return NULL;
	}
	
	
	return ret;
}




/**
 * v_stream_free:
 * @stream: a #VStream to free.
 *
 * Free's @stream and its contents.
 */
void
v_stream_free (VStream *stream)
{
	v_codec_free  (stream->codec);
	v_free (stream);
}

