/***************************************************************************
 *            demuxer.c
 *
 *  Tue Dec 16 19:54:46 2008
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
 
 
#include "demuxer.h"
#include "modules.h"
#include "mem.h"
#include <stdio.h>  /* printf */




/**
 * v_packet_new:
 *
 * Creates a new #VPacket with default values.
 *
 * Returns: a #VPacket structure.
 */
VPacket *
v_packet_new (void)
{
	return v_new (VPacket);
}




/**
 * v_packet_free:
 * @packet: a #VPacket to free.
 *
 * Free's @packet and its contents.
 */
void
v_packet_free (VPacket *packet)
{
	v_free (packet);
}





/**
 * v_packet_dump:
 * @packet: a #VPacket.
 *
 * Dumps packet data to stdout. Useful for debugging.
 */
void
v_packet_dump (VPacket *packet)
{
	/* dump info */
	printf ("id: %x\n"
			"codec: %d\n"
			"length: %d\n"
			"pts: %jd\n"
			"dts: %jd\n\n",
			packet->id,
			packet->codec_id,
			packet->length,
			packet->pts,
			packet->dts);
	
	
	int i;
	
	/* dump data in hex */
	for (i = 0; i < packet->length; i++)
		printf ("%#02x", packet->data[i]);
	
	
	printf ("\n\n\n");
}





/**
 * v_demuxer_new:
 * @buffer: a #VBuffer to read data from.
 * @error: a #VError, or %NULL.
 *
 * Creates a new #VDemuxer with default values.
 *
 * Returns: a #VDemuxer structure if successful, %NULL otherwise.
 */
VDemuxer *
v_demuxer_new (VCodecID codec_id, VBuffer *buffer, VError *error)
{
	VDemuxer *ret = v_modules_find_demuxer (codec_id, error);
	
	/* no module */
	if (ret == NULL)
		return NULL;
	
	
	/* default values */
	ret->buffer = buffer;
	
	
	return ret;
}



/**
 * v_demuxer_free:
 * @demuxer: a #VDemuxer to free.
 *
 * Free's @demuxer and its contents.
 */
void
v_demuxer_free (VDemuxer *demuxer)
{
	v_free (demuxer);
}




/**
 * v_demuxer_read_packet:
 * @demuxer: a #VDemuxer.
 * @error: a #VError, or %NULL.
 *
 * Demuxes a packet from the input stream.
 *
 * Returns: a #VPacket structure if successful, %NULL otherwise.
 */
VPacket *
v_demuxer_read_packet (VDemuxer *demuxer, VError *error)
{
	return demuxer->read_packet (demuxer, error);
}




bool
v_demuxer_open (VDemuxer *demuxer, VError *error)
{
	if (demuxer->open != NULL)
		return demuxer->open (demuxer, error);
	
	return true;
}


