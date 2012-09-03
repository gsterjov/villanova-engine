/***************************************************************************
 *            mpeg.c
 *
 *  Wed Dec 17 16:35:13 2008
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
#include "mem.h"
#include <stdbool.h>


#define PACK_START_PREFIX   0x00000100
#define PACK_START_MASK     0xffffff00

#define PACK_HEADER_CODE    0x000001ba
#define SYSTEM_HEADER_CODE  0x000001bb

#define PROGRAM_STREAM_MAP  0x000001bc
#define PRIVATE_STREAM_1    0x000001bd
#define PRIVATE_STREAM_2    0x000001bf
#define PADDING_STREAM      0x000001be




typedef struct _VDemuxerMpeg VDemuxerMpeg;



/*
 * VDemuxerMpeg:
 *
 * Private structure for #VDemuxerMpeg inheriting #VDemuxer.
 */
struct _VDemuxerMpeg
{
	VDemuxer parent;
};





/*
 * read_timestamp:
 * @demuxer: a #VDemuxer.
 * @c: the first 16 bits.
 *
 * Reads a timestamp from an MPEG stream.
 *
 * Returns: a 64 bit integer.
 */
static int64_t
read_timestamp (VDemuxer *demuxer, uint8_t c)
{
	uint8_t d = v_buffer_read_bits16 (demuxer->buffer);
	uint8_t e = v_buffer_read_bits16 (demuxer->buffer);
	
	return (c & 0x0e) << 29 | (d >> 1) << 15 | (e >> 1);
}



/*
 * find_start_code:
 * @demuxer: a #VDemuxer.
 *
 * Finds the next startcode on an MPEG stream.
 *
 * Returns: the start code.
 */
static uint32_t
find_start_code (VDemuxer *demuxer)
{
	uint32_t code = 0xff;
	uint8_t b;
	
	
	
	/* loop until we find a prefix of 0x000001 */
	while ((code & PACK_START_MASK) != PACK_START_PREFIX)
	{
		/* reached EOS */
		if (demuxer->buffer->eos)
			return 0;
		
		
		/* read the next byte */
		b = v_buffer_read_bits8 (demuxer->buffer);
		code = ((code << 8) | b) & 0xffffff;
	}
	
	
	return code;
}




/*
 * read_pes_header:
 * @demuxer: a #VDemuxer.
 * @id: sets to the ID of the PES packet stream.
 * @pts: sets to the PTS of the PES packet.
 * @dts: sets to the DTS of the PES packet.
 * @error: a #VError, or %NULL.
 *
 * Finds the next PES packet in the stream.
 *
 * Returns: the PES packet length.
 */
static int
read_pes_header (VDemuxer *demuxer,
				 uint32_t *id,
				 int64_t  *pts,
				 int64_t  *dts,
				 VError   *error)
{
	
	uint32_t code;
	int len;
	
	
	/* look for a PES packet and handle */
	/* any headers that we come across */
	while (true)
	{
		
		/* find the next start code */
		code = find_start_code (demuxer);
		
		
		/* reached EOS */
		if (demuxer->buffer->eos)
			return 0;
		
		
		
		/* ignore these headers */
		if (code == PACK_HEADER_CODE ||
			code == SYSTEM_HEADER_CODE)
			continue;
		
		
		/* we have a PES packet */
		if ((code >= 0x1c0 && code <= 0x1df) ||
			(code >= 0x1e0 && code <= 0x1ef) ||
			code == PRIVATE_STREAM_1)
			break;
		
		
		/* skip PSM headers and unsupported PES packets */
		len = v_buffer_read_bits16 (demuxer->buffer);
		v_buffer_skip (demuxer->buffer, len);
		
	}
	
	
	
	
	/* read PES header */
	len = v_buffer_read_bits16 (demuxer->buffer);
	uint8_t c = v_buffer_read_bits8 (demuxer->buffer);
	
	len--;
	
	
	
	/* MPEG2 packet */
	if ((c >> 6) == 2)
	{
		uint8_t flags = v_buffer_read_bits8 (demuxer->buffer);
		int hlen = v_buffer_read_bits8 (demuxer->buffer);
		len -= 2;
		
		
		/* sanity check */
		if (hlen > len)
		{
			v_error_set (error,
						 V_ERROR_DOMAIN_DEMUXER,
						 V_DEMUXER_ERROR_CORRUPTED,
						 "demuxer-mpeg",
						 "Corrupted media source. The header length is larger "
						 "than the PES packet length.");
			
			return -1;
		}
		
		
		len -= hlen;
		
		
		/* only has PTS */
		if ((flags >> 6) == 2)
		{
			c = v_buffer_read_bits8 (demuxer->buffer);
			*pts = *dts = read_timestamp (demuxer, c);
			
			hlen -= 5;
		}
		
		/* has both PTS and DTS */
		else if ((flags >> 6) == 3)
		{
			c    = v_buffer_read_bits8 (demuxer->buffer);
			*pts = read_timestamp (demuxer, c);
			
			c    = v_buffer_read_bits8 (demuxer->buffer);
			*dts = read_timestamp (demuxer, c);
			
			hlen -= 10;
		}
		
		
		
		/* skip the rest of the header */
		v_buffer_skip (demuxer->buffer, hlen);
		
	}
	
	
	/* MPEG1 packet */
	else
	{
		/* skip stuffing */
		while (c == 0xff)
		{
			c = v_buffer_read_bits8 (demuxer->buffer);
			len--;
		}
		
		
		/* skip std scale and size */
		if ((c >> 6) == 1)
		{
			v_buffer_read_bits8 (demuxer->buffer);
			c = v_buffer_read_bits8 (demuxer->buffer);
			len -= 2;
		}
		
		
		/* only has PTS */
		if ((c >> 4) == 2)
		{
			*pts = *dts = read_timestamp (demuxer, c);
			len -= 4;
		}
		
		/* has both PTS and DTS */
		else if ((c >> 4) == 3)
		{
			*pts = read_timestamp      (demuxer, c);
			c    = v_buffer_read_bits8 (demuxer->buffer);
			*dts = read_timestamp      (demuxer, c);
			
			len -= 9;
		}
		
	}
	
	
	
	/* DVD: non-Mpeg audio and subpictures */
	if (code == PRIVATE_STREAM_1)
	{
		/* get the sub-stream index */
		code = v_buffer_read_bits8 (demuxer->buffer);
		len -= 1;
		
		
		/* skip audio header */
		if (code >= 0x80 && code <= 0xcf)
		{
			v_buffer_read_bits8 (demuxer->buffer);
			v_buffer_read_bits8 (demuxer->buffer);
			v_buffer_read_bits8 (demuxer->buffer);
			len -= 3;
			
			
			/* MLP/TrueHD has a 4-byte header (from libavformat)*/
			if (code >= 0xb0 && code <= 0xbf)
			{
				v_buffer_read_bits8 (demuxer->buffer);
				len -= 1;
			}
		}
		
	}
	
	
	/* reached packet data */
	*id = code;
	return len;
}





/*
 * v_demuxer_mpeg_read_packet:
 * @demuxer: a #VDemuxer.
 * @error: a #VError, or %NULL.
 *
 * Reads raw input data and demuxes it into packets.
 *
 * Returns: a #VPacket if successful, %NULL otherwise.
 */
static VPacket *
v_demuxer_mpeg_read_packet (VDemuxer *demuxer, VError *error)
{
	
	uint32_t id;
	int len;
	
	uint8_t *data = NULL;
	
	int64_t pts = 0;
	int64_t dts = 0;
	
	
	
	/* find the next PES header */
	len = read_pes_header (demuxer, &id, &pts, &dts, error);
	
	/* failed to read header */
	if (len < 0)
		return NULL;
	
	
	/* EOS reached. return empty packet */
	if (len == 0)
		return v_packet_new ();
	
	
	
	
	VPacket *packet = v_packet_new ();
	
	
	
	/* determine codec type */
	/* Audio: 0xc0 - 0xdf */
	if (id >= 0x1c0 && id <= 0x1df)
		packet->codec_id = V_CODEC_ID_MP3;
	
	
	
	/* Video: 0xe0 - 0xef */
	else if (id >= 0x1e0 && id <= 0x1ef)
		packet->codec_id = V_CODEC_ID_MPEG2;
	
	
	
	/* AC3: 0x80 - 0x87  and  0xc0 - 0xcf */
	else if ((id >= 0x80 && id <= 0x87) ||
			 (id >= 0xc0 && id <= 0xcf))
		packet->codec_id = V_CODEC_ID_AC3;
	
	
	/* DTS: 0x88 - 0x8f  and  0x98 - 0x9f */
	else if ((id >= 0x88 && id <= 0x8f) ||
			 (id >= 0x98 && id <= 0x9f))
		packet->codec_id = V_CODEC_ID_DTS;
	
#if 0
	/* LPCM: 0xa0 - 0xbf */
	else if (id >= 0xa0 && id <= 0xbf)
		packet->codec_id = CODEC_ID_PCM_DVD;
	
#endif
	
	/* Subpicture: 0x20 - 0x3f */
	else if (id >= 0x20 && id <= 0x3f)
		packet->codec_id = V_CODEC_ID_SUBPIC;
	
	
	
	
	/* read PES packet data */
	packet->data = v_malloc (len);
	packet->length = v_buffer_read_bytes (demuxer->buffer, packet->data, len);
	
	
	/* set the packet values */
	packet->id     = id;
	packet->pts    = pts;
	packet->dts    = dts;
	
	
	return packet;
	
}




/**
 * v_demuxer_mpeg_new:
 *
 * Creates a new MPEG demuxer.
 *
 * Returns: a #VDemuxer structure.
 */
VDemuxer *
v_demuxer_mpeg_new (VCodecID codec_id)
{
	VDemuxerMpeg *ret = v_new (VDemuxerMpeg);
	
	VDemuxer *demuxer = (VDemuxer *) ret;
	
	
	/* set interface methods */
	demuxer->read_packet = v_demuxer_mpeg_read_packet;
	
	
	return demuxer;
}


