/***************************************************************************
 *            libavformat.c
 *
 *  Apr 12, 2009 2:23:05 PM
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


#include "config.h"

#include "demuxer.h"
#include "mem.h"
#include <string.h>  /* memcpy */


#ifdef HAVE_LIBAVCODEC_AVCODEC_H
	#include <libavformat/avformat.h>
#else
	#include <ffmpeg/avformat.h>
#endif



typedef struct _VDemuxerLibavformat VDemuxerLibavformat;



/*
 * VDemuxerLibavformat:
 *
 * Private structure for #VDemuxerLibavformat inheriting #VDemuxer.
 */
struct _VDemuxerLibavformat
{
	VDemuxer parent;
	
	
	/* demux components */
	AVFormatContext *format_ctx;
	AVInputFormat   *input_fmt;
	
	ByteIOContext io_ctx;
	
	
	uint8_t buffer[4096];
};





static void
probe_codec_id (VDemuxer *demuxer, AVStream *stream, AVPacket *pkt)
{

	/* get the previous probe data */
	AVProbeData *pd = &stream->probe_data;


	/* resize probe buffer to append new packet data */
	pd->buf = av_realloc (pd->buf, pd->buf_size + pkt->size + AVPROBE_PADDING_SIZE);

	/* append the new packet data */
	memcpy (pd->buf + pd->buf_size, pkt->data, pkt->size);

	
	/* set new buffer size and zero out the padding */
	pd->buf_size += pkt->size;
	memset (pd->buf + pd->buf_size, 0, AVPROBE_PADDING_SIZE);



	/* look for an input format capable of handling the probe data */
	AVInputFormat *fmt = av_probe_input_format (pd, 1);


	/* found an input format */
	if (fmt != NULL)
	{
		/* MPEG 1/2 Video Codec */
		if (strcmp (fmt->name, "mpegvideo") == 0)
		{
			stream->codec->codec_id = CODEC_ID_MPEG2VIDEO;
			stream->codec->codec_type = CODEC_TYPE_VIDEO;
		}
		
		/* unknown codec */
		else
		{
			stream->codec->codec_id = CODEC_ID_NONE;
			stream->codec->codec_type = CODEC_TYPE_UNKNOWN;
		}
		
		
		/* clean up */
		pd->buf_size = 0;
		av_freep (&pd->buf);
	}


}





static int
read_buffer (void *userdata, uint8_t *buffer, int length)
{
	VDemuxer *demuxer = (VDemuxer *) userdata;
	return v_buffer_read_bytes (demuxer->buffer, buffer, length);
}




/*
 * v_demuxer_libavformat_read_packet:
 * @demuxer: a #VDemuxer.
 * @error: a #VError, or %NULL.
 *
 * Reads raw input data and demuxes it into packets.
 *
 * Returns: a #VPacket if successful, %NULL otherwise.
 */
static VPacket *
v_demuxer_libavformat_read_packet (VDemuxer *demuxer, VError *error)
{
	VDemuxerLibavformat *priv = (VDemuxerLibavformat *) demuxer;
	
	
	AVPacket pkt;
	av_init_packet (&pkt);
	
	
	/* get the next demuxed packet */
	int ret = priv->input_fmt->read_packet (priv->format_ctx, &pkt);
	
	if (ret < 0)
		return NULL;
	
	
	
	/* get the packet stream */
	AVStream *stream = priv->format_ctx->streams[pkt.stream_index];
	
	
	/* need to probe for codec id */
	if (stream->codec->codec_id == CODEC_ID_PROBE)
		probe_codec_id (demuxer, stream, &pkt);
	
	
	
	VCodecID codec_id;
	
	/* convert codec to our codec range */
	switch (stream->codec->codec_id)
	{
		case CODEC_ID_MPEG2VIDEO:
			codec_id = V_CODEC_ID_MPEG2;
			break;
	
		case CODEC_ID_MP3:
			codec_id = V_CODEC_ID_MP3;
			break;
	
		case CODEC_ID_AC3:
			codec_id = V_CODEC_ID_AC3;
			break;
	
		case CODEC_ID_DTS:
			codec_id = V_CODEC_ID_DTS;
			break;
	
		case CODEC_ID_DVD_SUBTITLE:
			codec_id = V_CODEC_ID_SUBPIC;
			break;
			
		default:
			codec_id = V_CODEC_ID_UNKNOWN;
			break;
	}
	
	
	
	/* create packet */
	VPacket *packet = v_packet_new ();


	/* copy packet data */
	packet->data = v_mallocz (pkt.size);
	memcpy (packet->data, pkt.data, pkt.size);

	/* set packet info */
	packet->length = pkt.size;
	packet->id = stream->id;
	packet->codec_id = codec_id;

	/* set packet timestamps */
	packet->pts = pkt.pts;
	packet->dts = pkt.dts;
	
	
	/* clean up */
	av_free_packet (&pkt);
	
	return packet;
}





static bool
v_demuxer_libavformat_open (VDemuxer *demuxer, VError *error)
{
	VDemuxerLibavformat *priv = (VDemuxerLibavformat *) demuxer;
	
	
	/* open the input stream */
	av_open_input_stream (&priv->format_ctx, &priv->io_ctx, "", priv->input_fmt, NULL);
}



/**
 * v_demuxer_libavformat_new:
 *
 * Creates a new demuxer using Libavformat.
 *
 * Returns: a #VDemuxer structure.
 */
VDemuxer *
v_demuxer_libavformat_new (VCodecID codec_id)
{
	VDemuxerLibavformat *priv = v_new (VDemuxerLibavformat);
	
	VDemuxer *demuxer = (VDemuxer *) priv;
	
	
	/* set interface methods */
	demuxer->read_packet = v_demuxer_libavformat_read_packet;
	demuxer->open = v_demuxer_libavformat_open;
	
	
	/* register demuxers */
	av_register_all ();
	
	
	/* find the input format */
	priv->input_fmt = av_find_input_format ("mpeg");
	priv->input_fmt->flags |= AVFMT_NOFILE;
	
	
	
	/* create the ByteIO context */
	init_put_byte (&priv->io_ctx,
			priv->buffer,
			4096,
			0,
			priv,
			read_buffer,
			NULL,
			NULL);
	
	
	return demuxer;
}


