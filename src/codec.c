/***************************************************************************
 *            codec.c
 *
 *  Sun Dec 21 14:12:47 2008
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
 
 
#include "codec.h"
#include "mem.h"
#include "modules.h"




/**
 * v_codec_new:
 * @id: the codec type to handle.
 * @error: a #VError, or %NULL.
 *
 * Creates a new #VCodec which can handle @id type codecs.
 *
 * Returns: a #VCodec structure if successful, %NULL otherwise.
 */
VCodec *
v_codec_new (VCodecID id, VError *error)
{
	VCodec *ret = v_modules_find_codec (id, error);
	
	/* no module */
	if (ret == NULL)
		return NULL;
	
	
	/* default values */
	ret->id = id;
	ret->type = v_codec_id_type (id);
	
	
	return ret;
}




/**
 * v_codec_free:
 * @codec: a #VCodec to free.
 *
 * Free's @codec and its contents.
 */
void
v_codec_free (VCodec *codec)
{
	v_free (codec);
}




/**
 * v_codec_parse:
 * @codec: a #VCodec.
 * @frame: a frame structure to output.
 * @error: a #VError, or %NULL.
 *
 * Parses a demuxed packet. If a complete frame is parsed it will be outputted
 * to @frame, otherwise @frame will remain as %NULL.
 *
 * <note>
 *   <para>
 *     @frame must be a pointer to a #VFrame pointer which in turn must equal
 *     %NULL, otherwise a memory leak will occur.
 *   </para>
 * </note>
 *
 * <example>
 *   <title>Parsing a demuxed packet.</title>
 *   <programlisting>
 *		VFrame *frame = NULL;
 *		
 *		v_codec_parse (codec, &frame, NULL);
 *		
 *		if (frame != NULL)
 *			printf ("we got a complete frame");
 *		else
 *			printf ("need to parse more packets");
 *   </programlisting>
 * </example>
 *
 *
 * Returns: %true if successful, %false otherwise.
 */
void
v_codec_parse (VCodec *codec, VPacket *packet, VQueue *frames)
{
	codec->parse (codec, packet, frames);
}




/**
 * v_codec_decode:
 * @codec: a #VCodec.
 * @frame: a #VFrame to decode.
 * @error: a #VError, or %NULL.
 *
 * Decodes @frame.
 *
 * Returns: %true if successful, %false otherwise.
 */
VFrame *
v_codec_decode (VCodec *codec, VFrame *frame, VError *error)
{
	return codec->decode (codec, frame, error);
}




VCodecProperties *
v_codec_properties (VCodec *codec)
{
	return codec->properties (codec);
}


