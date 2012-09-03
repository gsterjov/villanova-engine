/***************************************************************************
 *            engine.c
 *
 *  Fri Dec 12 18:47:37 2008
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



#include "engine.h"
#include "mem.h"
#include "modules.h"
#include "async-queue.h"
#include "queue.h"
#include "clock.h"
#include "colorspace.h"
#include <pthread.h>
#include <unistd.h> /* usleep */
#include <stdio.h>  /* printf */




/*
 * VEnginePriv:
 *
 * Private structure for #VEngine.
 */
struct _VEnginePriv
{
	/* threads */
	pthread_t frames_proc;
	pthread_t audio_proc;
	pthread_t video_proc;
	pthread_t subpic_proc;


	/* queues */
	VAsyncQueue *audio_events;
	VAsyncQueue *video_events;
	VAsyncQueue *subpic_events;


	/* streams */
	VStream *audio;
	VStream *video;
	VStream *subpic;
	
	
	VClock *clock;
	VColorspace *colorspace;
};






/*
 * new_stream:
 * @stream: a #VStream.
 *
 * Handles new stream events.
 */
static void
new_stream (VInput *input, VStream *stream, void *userdata)
{
	VEngine *self = (VEngine *) userdata;
	VEnginePriv *priv = self->priv;


	/* TODO: stream changes should be handled in the appropriate thread
	 * so that we wont screw up the current decoding buffer */
	switch (stream->codec->type)
	{
	case V_CODEC_TYPE_AUDIO:
		if (priv->audio == NULL)
		{
			priv->audio = stream;
			v_output_open (self->audio_output, stream);
		}
		break;


	case V_CODEC_TYPE_VIDEO:
		if (priv->video == NULL)
		{
			priv->video = stream;
			v_output_open (self->video_output, stream);
			
			priv->colorspace = v_colorspace_new (V_PIXEL_FORMAT_YUV420,
					V_PIXEL_FORMAT_YUV420,
					stream->width,
					stream->height,
					NULL);
		}
		break;


	case V_CODEC_TYPE_SUBTITLE:
		if (priv->subpic == NULL)
			priv->subpic = stream;
		break;
	}



	printf ("-----------------------\n");
	
	printf ("NEW STREAM - %x -  %s\n", stream->id, v_codec_id_string (stream->codec->id));

	printf ("channels=%d, sample rate=%d, width=%d, height=%d\n",
			stream->channels,
			stream->sample_rate,
			stream->width,
			stream->height);
}






/*
 * process_audio:
 * @user_data: a #VEngine.
 *
 * Reads frames from an input and adds it to the appropriate
 * queue for decoding and output.
 */
static void *
process_audio (void *user_data)
{
	VEngine *self = (VEngine *) user_data;
	VEnginePriv *priv = self->priv;


	while (true)
	{
		VFrame *frame = v_async_queue_dequeue_wait (priv->audio_events);
		
		if (frame == NULL)
			continue;
		
		
		/* decode audio frame */
		VFrame *aud_frame = v_codec_decode (priv->audio->codec, frame, NULL);
		
		
		/* send frame to the output device */
		v_output_write (self->audio_output, aud_frame);
		
		
		/* clean up */
		v_frame_free (aud_frame);
		v_frame_free (frame);
	}
	
}



/*
 * process_video:
 * @user_data: a #VEngine.
 *
 * Reads frames from an input and adds it to the appropriate
 * queue for decoding and output.
 */
static void *
process_video (void *user_data)
{
	VEngine *self = (VEngine *) user_data;
	VEnginePriv *priv = self->priv;


	while (true)
	{
		VFrame *frame = v_async_queue_dequeue_wait (priv->video_events);

		if (frame == NULL)
			continue;
		

		/* decode video frame */
		VFrame *vid_frame = v_codec_decode (priv->video->codec, frame, NULL);
		
		
		if (vid_frame)
		{
			/* timer prototype */
			VFrameRaw *raw = (VFrameRaw *) frame;
			double timeout = v_clock_get_timeout (priv->clock, raw);
			
			
			VFrame *fin_frame = v_colorspace_convert (priv->colorspace, vid_frame);
			
			
			/* timer prototype */
//			if (timeout > 0.010)
//				usleep (timeout * 1000000.0);
			
			
	
			/* send frame to the output device */
			v_output_write (self->video_output, fin_frame);
			
			
			/* clean up */
			v_frame_free (fin_frame);
			v_frame_free (vid_frame);
			
		}
		
		
		v_frame_free (frame);
	}
	
}



/*
 * process_subpic:
 * @user_data: a #VEngine.
 *
 * Reads frames from an input and adds it to the appropriate
 * queue for decoding and output.
 */
static void *
process_subpic (void *user_data)
{
	VEngine *self = (VEngine *) user_data;
	VEnginePriv *priv = self->priv;


	while (true)
	{
		VFrame *frame = v_async_queue_dequeue_wait (priv->subpic_events);
		
		if (frame == NULL)
			continue;
		
		

		/* decode subtitle frame */
		VFrame *sub_frame = v_codec_decode (priv->subpic->codec, frame, NULL);

		
		if (sub_frame)
		{
			VFrameSubtitle *sub = V_FRAME_SUBTITLE (sub_frame);
			
			
			VColorspace *col = v_colorspace_new (V_PIXEL_FORMAT_RGB32,
					V_PIXEL_FORMAT_YUV420,
					sub->w,
					sub->h,
					NULL);
			
			
			VFrame *fin_frame = v_colorspace_convert (col, sub_frame);
			
			
			v_output_write_sub (self->video_output, fin_frame);
			
			
			v_frame_free (sub_frame);
		}
		
		

		/* clean up */
		v_frame_free (frame);
	}
	
}




/*
 * process_frames:
 * @user_data: a #VmeEngine.
 *
 * Reads frames from an input and adds it to the appropriate
 * queue for decoding and output.
 */
static void *
process_frames (void *user_data)
{
	VEngine *self = (VEngine *) user_data;
	VEnginePriv *priv = self->priv;
	
	
	
	/* process input events */
	while (true)
	{
		VFrameRaw *frame = v_input_read_frame (self->input, NULL);
		
		if (frame == NULL)
			break;
		
		
		
		/* audio frame */
		if (priv->audio && frame->stream_id == priv->audio->id)
			v_async_queue_enqueue_wait (priv->audio_events, frame);

		/* video frame */
		else if (priv->video && frame->stream_id == priv->video->id)
			v_async_queue_enqueue_wait (priv->video_events, frame);

		/* subpic frame */
		else if (priv->subpic && frame->stream_id == priv->subpic->id)
			v_async_queue_enqueue_wait (priv->subpic_events, frame);

		
	}

}





/**
 * v_engine_init:
 *
 * Initiates the required components. This function <emphasis>must</emphasis>
 * be called before any other function.
 */
void
v_engine_init (void)
{
	v_modules_init ();
}



/**
 * v_engine_new:
 *
 * Creates a new #VEngine with default values.
 *
 * Returns: a #VEngine structure.
 */
VEngine *
v_engine_new (void)
{
	VEngine *ret = v_new (VEngine);
	VEnginePriv *priv = v_new (VEnginePriv);


	/* default values */
	priv->audio = NULL;
	priv->video = NULL;
	priv->subpic = NULL;
	
	priv->audio_events = v_async_queue_new (10);
	priv->video_events = v_async_queue_new (10);
	priv->subpic_events = v_async_queue_new (10);

	priv->clock = v_clock_new (NULL);
	
	
	ret->priv = priv;
	
	
	ret->audio_output = v_output_new (V_OUTPUT_TYPE_AUDIO, NULL);
	ret->video_output = v_output_new (V_OUTPUT_TYPE_VIDEO, NULL);
	

	return ret;
}



/**
 * v_engine_free:
 * @engine: a #VEngine to free.
 *
 * Destroys @engine and its contents.
 */
void
v_engine_free (VEngine *engine)
{
	VEnginePriv *priv = engine->priv;
	
	
	/* destroy queues */
	v_async_queue_free (priv->audio_events);
	v_async_queue_free (priv->video_events);
	v_async_queue_free (priv->subpic_events);

	v_free (engine->priv);
	v_free (engine);
}





/**
 * v_engine_close:
 * @engine: a #VEngine.
 *
 * Closes the engine media file and releases its resources.
 */
void
v_engine_close (VEngine *engine)
{
	/* unload components */
	v_input_close (engine->input);


	/* free components */
	v_input_free (engine->input);
}


/**
 * v_engine_open:
 * @engine: a #VEngine.
 * @uri; the absolute path to the media file, including the protocol.
 * @error: a #VError, or %NULL.
 *
 * Opens the media file located at @uri with the input module specified by the
 * protocol in @uri.
 *
 * <note>
 *   <para>
 *     All URI's must have the format 'protocol://path' where protocol is the
 *     input module type to load such as 'file' and path is the absolute path
 *     to the media file such as '/home/user/media.mpg'.
 *   </para>
 * </note>
 *
 * Returns: %true if successful, %false otherwise.
 */
bool
v_engine_open (VEngine    *engine,
			   const char *protocol,
			   const char *uri,
			   VError     *error)
{
	engine->input = v_input_new (protocol, uri, error);


	/* no input */
	if (engine->input == NULL)
		return false;
	
	
	
	/* register events */
	v_input_register_new_stream (engine->input, new_stream, engine);
	
	
	
	return v_input_open (engine->input, error);
}




/**
 * v_engine_play:
 * @engine: a #VEngine.
 * @error: a #VError, or %NULL.
 *
 * Begins playing the media file opened with v_engine_open().
 *
 * Returns: %true if successful, %false otherwise.
 */
bool
v_engine_play (VEngine *engine, VError *error)
{
	VEnginePriv *priv = engine->priv;


	pthread_create (&priv->frames_proc, NULL, process_frames, engine);
	pthread_create (&priv->audio_proc, NULL, process_audio, engine);
	pthread_create (&priv->video_proc, NULL, process_video, engine);
	pthread_create (&priv->subpic_proc, NULL, process_subpic, engine);

	return true;
}


/**
 * v_engine_pause:
 * @engine: a #VEngine.
 *
 * Pauses the media file.
 */
void
v_engine_pause (VEngine *engine)
{

}


/**
 * v_engine_stop:
 * @engine: a #VEngine.
 *
 * Stops playing the media file and resets its position to 0.
 */
void
v_engine_stop (VEngine *engine)
{

}




