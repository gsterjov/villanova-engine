/***************************************************************************
 *            input.c
 *
 *  Sat Dec 13 19:38:20 2008
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
 
 

#include "input.h"
#include "modules.h"
#include "mem.h"
#include "list.h"
#include "stream.h"
#include "demuxer.h"
#include <string.h>  /* strdup */
#include <stdlib.h>  /* free */

#include <stdio.h>




/*
 * VInputPriv:
 * @demuxer: the demuxer.
 * @buffer: the input buffer.
 * @streams: a list of streams.
 * @new_streams: a list of new streams with no metadata yet.
 *
 * Private structure for #VInput.
 */
struct _VInputPriv
{
	VDemuxer *demuxer;
	VBuffer  *buffer;
	
	VList  *streams;
	VList  *new_streams;
	VQueue *frames;
	
	
	/* events */
	VNewStreamFunc *new_stream_handler;
	VEosFunc       *eos_handler;
	
	void *new_stream_userdata;
	void *eos_userdata;
};





/*
 * v_input_find_stream:
 * @input: a #VInput.
 * @packet: a #VPacket.
 * @error: a #VError, or %NULL.
 *
 * Finds the stream associated with @packet.
 *
 * Returns: a #VStream structure if successful, %NULL otherwise.
 */
static VStream *
find_stream (VInput *input, VPacket *packet, VError *error)
{
	VInputPriv *priv = input->priv;
	
	
	VListNode *node = NULL;
	VStream *stream = NULL;
	
	
	/* look for a matching stream */
	for (node = priv->streams->first; node; node = node->next)
	{
		stream = (VStream *) node->data;
		
		/* found matching stream */
		if (stream->id == packet->id)
			return stream;
	}
	
	
	
	/* no matching stream found */
	stream = v_stream_new (packet->id, packet->codec_id, error);
	
	
	/* add it to the list */
	if (stream != NULL)
	{
		v_list_append (priv->streams, stream);
		v_list_append (priv->new_streams, stream);
	}
	
	
	return stream;
}






/**
 * v_input_new:
 * @protocol: the input protocol to handle.
 * @uri: the absolute path to the media source.
 * @error: a #VError, or %NULL.
 *
 * Creates a new #VInput capable of handling @protocol.
 *
 * Returns: a #VInput structure if successful, %NULL otherwise.
 */
VInput *
v_input_new (const char *protocol, const char *uri, VError *error)
{
	VInput *ret = v_modules_find_input (protocol, error);
	
	/* no module */
	if (ret == NULL)
		return NULL;
	
	
	VInputPriv *priv = v_new (VInputPriv);
	
	
	/* default values */
	ret->protocol = strdup (protocol);
	ret->uri = strdup (uri);
	ret->priv = priv;
	
	
	return ret;
}




/**
 * v_input_free:
 * @input: a #VInput to free.
 *
 * Destroys @input and its contents.
 */
void
v_input_free (VInput *input)
{
	free (input->protocol);
	free (input->uri);
	v_free (input->priv);
	v_free (input);
}




/**
 * v_input_open:
 * @input: a #VInput.
 * @error: a #VError, or %NULL.
 *
 * Opens up an input stream and prepares it for reading.
 *
 * Returns: %true if successful, %false otherwise.
 */
bool
v_input_open (VInput *input, VError *error)
{
	VInputPriv *priv = input->priv;
	
	
	/* create buffer */
	VBuffer *buffer = input->open (input, error);
	
	/* no buffer, open failed*/
	if (buffer == NULL)
		return false;
	
	
	
	/* create input components */
	priv->buffer  = buffer;
	
	/* TODO: create a probe function to find the correct format type */
	priv->demuxer = v_demuxer_new (V_CODEC_ID_MPEG2, buffer, error);
	
	
	/* no demuxer */
	if (priv->demuxer == NULL)
	{
		input->close  (input);
		v_buffer_free (buffer);
		return false;
	}
	
	
	v_demuxer_open (priv->demuxer, error);
	
	
	
	priv->streams     = v_list_new ();
	priv->new_streams = v_list_new ();
	priv->frames      = v_queue_new (0);
	
	
	return true;
}



/**
 * v_input_close:
 * @input: a #VInput.
 *
 * Closes the opened input stream.
 */
void
v_input_close (VInput *input)
{
	VInputPriv *priv = input->priv;
	
	
	/* free components */
	v_demuxer_free (priv->demuxer);
	v_buffer_free  (priv->buffer);
	
	v_list_free  (priv->streams);
	v_list_free (priv->new_streams);
	v_queue_free (priv->frames);
	
	
	/* close input */
	input->close (input);
}




/**
 * v_input_read_frame:
 * @input: a #VInput.
 * @error: a #VError, or %NULL.
 *
 * Reads a complete frame from the media source.
 *
 * Returns: a #VFrame structure if successful, %NULL otherwise.
 */
VFrameRaw *
v_input_read_frame (VInput *input, VError *error)
{
	VInputPriv *priv = input->priv;
	
	
	
	/* read until we get a complete frame */
	while (v_queue_peek (priv->frames) == NULL)
	{
		
		/* reached EOS */
		if (priv->buffer->eos)
		{
			input->eos = true;
			
			/* send EOS event */
			v_input_raise_eos (input);
			
			return NULL;
		}
		
		
		/* read packet from the demuxer */
		VPacket *packet = v_demuxer_read_packet (priv->demuxer, error);
		
		/* demuxing failed */
		if (packet == NULL)
			return NULL;
		
		
		/* unknown codec type */
		if (packet->codec_id == V_CODEC_ID_UNKNOWN)
		{
			/* TODO: we should pass the packet as a frame
			 * instead of just ignoring it */
			v_packet_free (packet);
			continue;
		}
		
		
		/* find corresponding stream */
		VStream *stream = find_stream (input, packet, error);
		
		/* cannot handle the codec */
		if (stream == NULL)
		{
			v_packet_free (packet);
			continue;
		}
		
		
		
		/* parse packet to get a complete frame */
		v_codec_parse (stream->codec, packet, priv->frames);
		v_packet_free (packet);
	}
	
	
	
	/* look for new streams */
	VFrameRaw *frame = v_queue_dequeue (priv->frames);
	
	
	VListNode *node = NULL;
	VStream *st = NULL;


	/* look for a matching stream */
	for (node = priv->new_streams->first; node; node = node->next)
	{
		st = (VStream *) node->data;

		/* found matching stream */
		if (st->id == frame->stream_id)
		{
			v_list_remove (priv->new_streams, node);
			break;
		}
	}

	
	
	/* load the stream properties */
	if (st != NULL)
	{
		/* get stream info */
		VCodecProperties *prop = v_codec_properties (st->codec);

		/* FIXME: this is a really shitty way of doing this.
		 * must find another way */
		st->channels = prop->channels;
		st->sample_rate = prop->sample_rate;
		st->sample_format = prop->sample_format;

		st->width = prop->width;
		st->height = prop->height;
		st->pixel_format = prop->pixel_format;
		
		
		/* raise new stream event */
		v_input_raise_new_stream (input, st);
	}
	
	
	
	
	/* return frame */
	return frame;
}






void
v_input_register_new_stream (VInput *input, VNewStreamFunc *func, void *userdata)
{
	VInputPriv *priv = input->priv;
	
	priv->new_stream_handler = func;
	priv->new_stream_userdata = userdata;
}



void
v_input_register_eos (VInput *input, VEosFunc *func, void *userdata)
{
	VInputPriv *priv = input->priv;
	
	priv->eos_handler = func;
	priv->eos_userdata = userdata;
}




void
v_input_raise_new_stream (VInput *input, VStream *stream)
{
	VInputPriv *priv = input->priv;
	priv->new_stream_handler (input, stream, priv->new_stream_userdata);
}


void
v_input_raise_eos (VInput *input)
{
	VInputPriv *priv = input->priv;
	printf ("EOS!\n");
	//priv->eos_handler (input, priv->eos_userdata);
}


