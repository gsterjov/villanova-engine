/***************************************************************************
 *            libavcodec.c
 *
 *  Fri Dec 26 15:45:13 2008
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
 
 
#include "config.h"

#include "codec.h"
#include "mem.h"
#include <string.h>  /* memcpy */


#ifdef HAVE_LIBAVCODEC_AVCODEC_H
	#include <libavcodec/avcodec.h>
#else
	#include <ffmpeg/avcodec.h>
#endif



typedef struct _VCodecLibavcodec VCodecLibavcodec;



/*
 * VCodecLibavcodec:
 *
 * Private structure for #VCodecLibavcodec inheriting #VCodec.
 */
struct _VCodecLibavcodec
{
	VCodec parent;
	
	AVCodecContext *codec_ctx;
	AVCodecParserContext *parser_ctx;
	
	
	/* video decoding */
	AVFrame *raw;
};






static VCodecProperties *
v_codec_libavcodec_properties (VCodec *codec)
{
	VCodecLibavcodec *self = (VCodecLibavcodec *) codec;
	
	
	VCodecProperties *prop = v_new (VCodecProperties);
	
	
	if (self->codec_ctx->sample_fmt == SAMPLE_FMT_S16)
		prop->sample_format = V_SAMPLE_FORMAT_S16;

	if (self->codec_ctx->pix_fmt == PIX_FMT_YUV420P)
		prop->pixel_format = V_PIXEL_FORMAT_YUV420;
	
	
	
	prop->channels = self->codec_ctx->channels;
	prop->sample_rate = self->codec_ctx->sample_rate;
	
	prop->width = self->codec_ctx->width;
	prop->height = self->codec_ctx->height;
}






/*
 * v_codec_libavcodec_parse:
 *
 * Parses a packet using libavcodec.
 *
 * Returns: %true if successful, %false otherwise.
 */
static void
v_codec_libavcodec_parse (VCodec *codec, VPacket *packet, VQueue *frames)
{
	VCodecLibavcodec *self = (VCodecLibavcodec *) codec;
	
	
	
	int len = packet->length;
	int idx = 0;
	
	uint8_t *data;
	int size = 0;
	
	
	
	/* parse the packet */
	while (len > 0)
	{
		
		int ret = av_parser_parse (self->parser_ctx,
								   self->codec_ctx,
								   &data, &size,
								   &packet->data[idx], len,
								   packet->pts, packet->dts);
		
		
		idx += ret;
		len -= ret;
		
		
		/* we've got a complete frame */
		if (size)
		{
			VFrameRaw *frame = v_frame_raw_new (size);
			
			frame->stream_id = packet->id;
			frame->pts = self->parser_ctx->pts;
			frame->dts = self->parser_ctx->dts;
			
			
			/* copy frame data */
			//frame->length = size;
			//frame->data = v_malloc (size);
			
			memcpy (frame->data, data, size);
			
			/* by pushing frames onto a queue we can handle
			 * multiple frames in a single packet */
			v_queue_enqueue (frames, frame);
		}
		
	}
	
	
}





/*
 * v_codec_libavcodec_decode_audio:
 *
 * Creates a new parser using libavcodec.
 *
 * Returns: %true if successful, %false otherwise.
 */
static VFrame *
v_codec_libavcodec_decode_audio (VCodec *codec, VFrame *frame, VError *error)
{
	VCodecLibavcodec *self = (VCodecLibavcodec *) codec;
	

	VFrameRaw *raw = V_FRAME_RAW (frame);
	VFrameAudio *audio = v_frame_audio_new (AVCODEC_MAX_AUDIO_FRAME_SIZE);

	
	/* decode audio frame */
	avcodec_decode_audio2 (self->codec_ctx,
			audio->samples,
			&audio->length,
			raw->data,
			raw->length);


	return V_FRAME (audio);
}





/*
 * v_codec_libavcodec_decode_video:
 *
 * Creates a new parser using libavcodec.
 *
 * Returns: %true if successful, %false otherwise.
 */
static VFrame *
v_codec_libavcodec_decode_video (VCodec *codec, VFrame *frame, VError *error)
{
	VCodecLibavcodec *self = (VCodecLibavcodec *) codec;

	
	VFrameRaw *raw = V_FRAME_RAW (frame);

	
	int frame_finished;
	
	
	/* decode video frame */
	avcodec_decode_video (self->codec_ctx,
			self->raw,
			&frame_finished,
			raw->data,
			raw->length);

	
	
	if (frame_finished)
	{
		VFrameVideo *video = v_frame_video_new ();
		
		
		video->data[0] = self->raw->data[0];
		video->data[1] = self->raw->data[1];
		video->data[2] = self->raw->data[2];
		video->data[3] = self->raw->data[3];
		
		video->linesize[0] = self->raw->linesize[0];
		video->linesize[1] = self->raw->linesize[1];
		video->linesize[2] = self->raw->linesize[2];
		video->linesize[3] = self->raw->linesize[3];
		
		
		return V_FRAME (video);
	}
	
	
	return NULL;
}




/*
 * v_codec_libavcodec_decode_subtitle:
 *
 * Creates a new parser using libavcodec.
 *
 * Returns: %true if successful, %false otherwise.
 */
static VFrame *
v_codec_libavcodec_decode_subtitle (VCodec *codec, VFrame *frame, VError *error)
{
	VCodecLibavcodec *self = (VCodecLibavcodec *) codec;
	

	VFrameRaw *raw = V_FRAME_RAW (frame);
	
	
	
	AVSubtitle sub;
	int got_sub;
	
	
	/* decode subtitle frame */
	avcodec_decode_subtitle (self->codec_ctx,
			&sub,
			&got_sub,
			raw->data,
			raw->length);


	
	if (got_sub != 0)
	{
		VFrameSubtitle *subpic = v_frame_subtitle_new ();
		

		subpic->x = sub.rects[0]->x;
		subpic->y = sub.rects[0]->y;
		subpic->w = sub.rects[0]->w;
		subpic->h = sub.rects[0]->h;


		subpic->data[0] = sub.rects[0]->pict.data[0];
		subpic->data[1] = sub.rects[0]->pict.data[1];
		subpic->data[2] = sub.rects[0]->pict.data[2];
		subpic->data[3] = sub.rects[0]->pict.data[3];

		subpic->linesize[0] = sub.rects[0]->pict.linesize[0];
		subpic->linesize[1] = sub.rects[0]->pict.linesize[1];
		subpic->linesize[2] = sub.rects[0]->pict.linesize[2];
		subpic->linesize[3] = sub.rects[0]->pict.linesize[3];
		
		
		return V_FRAME (subpic);
	}
	
	
	return NULL;
}




/**
 * v_codec_libavcodec_new:
 *
 * Creates a new codec using libavcodec.
 *
 * Returns: a #VCodec structure.
 */
VCodec *
v_codec_libavcodec_new (VCodecID codec_id)
{
	VCodecLibavcodec *priv = v_new (VCodecLibavcodec);
	
	VCodec *ret = (VCodec *) priv;
	
	
	
	enum CodecID id = CODEC_ID_NONE;
	
	
	switch (codec_id)
	{
		case V_CODEC_ID_MPEG2:
			id = CODEC_ID_MPEG2VIDEO;
			break;
			
		case V_CODEC_ID_MP3:
			id = CODEC_ID_MP3;
			break;
			
		case V_CODEC_ID_AC3:
			id = CODEC_ID_AC3;
			break;
			
		case V_CODEC_ID_DTS:
			id = CODEC_ID_DTS;
			break;
			
		case V_CODEC_ID_SUBPIC:
			id = CODEC_ID_DVD_SUBTITLE;
			break;
	}
	
	
	
	avcodec_register_all ();
	
	/* create contexts */
	priv->codec_ctx  = avcodec_alloc_context ();
	priv->parser_ctx = av_parser_init (id);
	
	
	/* open codec */
	AVCodec *av_codec = avcodec_find_decoder (id);
	avcodec_open (priv->codec_ctx, av_codec);
	
	
	
	/* set interface methods */
	ret->parse = v_codec_libavcodec_parse;
	ret->properties = v_codec_libavcodec_properties;
	
	
	
	switch (av_codec->type)
	{
		case CODEC_TYPE_AUDIO:
			ret->decode = v_codec_libavcodec_decode_audio;
			break;
			
		case CODEC_TYPE_VIDEO:
			ret->decode = v_codec_libavcodec_decode_video;
			priv->raw = avcodec_alloc_frame ();
			break;
			
		case CODEC_TYPE_SUBTITLE:
			ret->decode = v_codec_libavcodec_decode_subtitle;
	}
	
	
	return ret;
}

