/***************************************************************************
 *            alsa.c
 *
 *  Feb 9, 2009 6:30:10 PM
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


#include "output.h"
#include "mem.h"
#include <alsa/asoundlib.h>




typedef struct _VOutputAlsa VOutputAlsa;



/*
 * VOutputAlsa:
 * @fd: the file descriptor.
 * @length: the input buffer length.
 * @buffer: the input buffer.
 *
 * Private structure for #VOutputAlsa inheriting #VOutput.
 */
struct _VOutputAlsa
{
	VOutput parent;
	
	const char *device;
	size_t bps;


	snd_pcm_uframes_t buffer_size;
	snd_pcm_uframes_t period_size;


	snd_pcm_t *pcm;
};




/*
 * v_output_alsa_write:
 * @output: a #VOutput.
 * @frame: a #VFrame to write.
 *
 * Writes @frame to the ALSA buffer.
 */
static void
v_output_alsa_write (VOutput *output, VFrame *frame)
{
	VOutputAlsa *self  = (VOutputAlsa *) output;
	VFrameAudio *audio = V_FRAME_AUDIO (frame);
	
	
	snd_pcm_sframes_t frames;

	
	/* length of samples in frames */
	int len = audio->length / self->bps;


	/* write to alsa buffer */
	frames = snd_pcm_writei (self->pcm, audio->samples, len);


	/* recover from buffer underrun */
	if (frames == -EPIPE)
		snd_pcm_prepare (self->pcm);
}




/*
 * v_output_alsa_open:
 * @output: a #VOutput.
 * @frame: a #VFrame to write.
 *
 * Writes @frame to the ALSA buffer.
 */
static void
v_output_alsa_open (VOutput *output, VStream *stream)
{
	VOutputAlsa *self = (VOutputAlsa *) output;
	
	
	/* load alsa device */
	snd_pcm_open (&self->pcm,
			self->device,
			SND_PCM_STREAM_PLAYBACK,
			SND_PCM_NONBLOCK);


	/* set pcm with a buffer time of 0.5 sec */
	snd_pcm_set_params (self->pcm,
			SND_PCM_FORMAT_S16,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			stream->channels,
			stream->sample_rate,
			1, 500000);


	/* get the bytes per sample */
	self->bps = snd_pcm_format_physical_width (SND_PCM_FORMAT_S16) / 8;
	self->bps *= stream->channels;



	snd_pcm_get_params (self->pcm, &self->buffer_size, &self->period_size);


	snd_pcm_prepare (self->pcm);
}




/*
 * v_output_alsa_close:
 * @output: a #VOutput.
 * @frame: a #VFrame to write.
 *
 * Writes @frame to the ALSA buffer.
 */
static void
v_output_alsa_close (VOutput *output)
{
	
}




/**
 * v_output_alsa_new:
 *
 * Creates a new audio output using ALSA.
 *
 * Returns: a #VOutput structure.
 */
VOutput *
v_output_alsa_new (void)
{
	VOutputAlsa *ret = v_new (VOutputAlsa);
	
	VOutput *output = (VOutput *) ret;
	
	
	/* set interface methods */
	output->write = v_output_alsa_write;
	output->open  = v_output_alsa_open;
	output->close = v_output_alsa_close;
	
	
	
	ret->device = "default";
	
	
	
	return output;
}
