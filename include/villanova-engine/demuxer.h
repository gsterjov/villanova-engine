/***************************************************************************
 *            demuxer.h
 *
 *  Tue Dec 16 19:52:17 2008
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
 
#ifndef V_DEMUXER_H_
#define V_DEMUXER_H_


#include <villanova-engine/error.h>
#include <villanova-engine/buffer.h>
#include <villanova-engine/codec-types.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct _VPacket  VPacket;
typedef struct _VDemuxer VDemuxer;




/**
 * VDemuxerError:
 * @V_DEMUXER_ERROR_CORRUPTED: failed to read packet due to corrupted input.
 * @V_DEMUXER_ERROR_FAILED: unknown error.
 *
 * Possible error codes when demuxing.
 */
enum _VDemuxerError
{
	V_DEMUXER_ERROR_CORRUPTED,
	V_DEMUXER_ERROR_FAILED
};




/**
 * VDemuxer:
 * @buffer: a #VBuffer to read from.
 * @read_packet: interface prototype to read a packet from the input buffer.
 *
 * Demuxes raw data from an input buffer. All demuxer modules must inherit from
 * #VDemuxer and should override the interface prototypes it needs. At the very
 * least all #VDemuxer subclasses <emphasis>must</emphasis> implement 
 * @read_packet, 
 */
struct _VDemuxer
{
	VBuffer *buffer;
	
	
	/*< interface methods >*/
	VPacket *(* read_packet) (VDemuxer *demuxer, VError *error);
	bool (* open) (VDemuxer *demuxer, VError *error);
};




/**
 * VPacket:
 * @stream_id: the ID of the stream the packet originated from.
 * @codec_id: the codec type of the stream.
 * @length: the size of the packet data.
 * @data: raw demuxed packet data.
 * @pts: presentation timestamp.
 * @dts: decoding timestamp.
 *
 * A demuxed packet belonging to the stream specified by @id.
 */
struct _VPacket
{
	int id;
	VCodecID codec_id;
	
	int length;
	uint8_t *data;
	
	int64_t pts;
	int64_t dts;
};




VPacket *v_packet_new  (void);
void     v_packet_free (VPacket *packet);
void     v_packet_dump (VPacket *packet);



VDemuxer *v_demuxer_new  (VCodecID codec_id, VBuffer *buffer, VError *error);
void      v_demuxer_free (VDemuxer *demuxer);


VPacket  *v_demuxer_read_packet  (VDemuxer *demuxer, VError *error);

bool v_demuxer_open (VDemuxer *demuxer, VError *error);



#endif /* V_DEMUXER_H_ */
 
