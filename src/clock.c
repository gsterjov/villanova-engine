/*
 * clock.c
 *
 *  Created on: Dec 8, 2009
 *      Author: sterjov
 */


#include "config.h"

#include "clock.h"
#include "mem.h"


/* FIXME: clock shouldn't be directly using libavcodec */
#ifdef HAVE_LIBAVCODEC_AVCODEC_H
	#include <libavcodec/avcodec.h>
	#include <libswscale/swscale.h>
#else
	#include <ffmpeg/avcodec.h>
	#include <ffmpeg/swscale.h>
#endif



/*
 * VClockPriv:
 *
 * Private structure for #VClock.
 */
struct _VClockPriv
{
	double audio_clock;
	double video_clock;
	
	
	/* used to give an absolute value to incremental pts values */
	double base_clock;
	
	/* used to adjust sync for a specific clock */
	double delay;
};





/**
 * v_clock_new:
 *
 * Creates a new #VClock with default values.
 *
 * Returns: a #VClock structure.
 */
VClock *
v_clock_new (VError *error)
{
	VClock *ret = v_new (VClock);
	VClockPriv *priv = v_new (VClockPriv);
	
	
	priv->base_clock = 0;
	ret->priv = priv;
	
	return ret;
}



/**
 * v_clock_free:
 * @clock: a #VClock to free.
 *
 * Destroys @clock and its contents.
 */
void
v_clock_free (VClock *clock)
{
	VClockPriv *priv = clock->priv;
	
	v_free (clock->priv);
	v_free (clock);
}




/**
 * v_clock_get_timeout:
 * @clock: a #VClock.
 *
 * Destroys @clock and its contents.
 */
double
v_clock_get_timeout (VClock *clock, VFrameRaw *frame)
{
	VClockPriv *priv = clock->priv;
	
	
	if (priv->base_clock = 0)
		priv->base_clock = av_gettime() / 1000000.0;
	
	
	double master = (double) av_gettime () / 1000000.0;

//	printf ("%g - %jd - %g\n", priv->base_clock, frame->pts, master);
	
	return (priv->base_clock + frame->pts) - master;
}


