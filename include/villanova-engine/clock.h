/***************************************************************************
 *            output.h
 *
 *  Dec 8, 2009 6:15:37 PM
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

#ifndef V_CLOCK_H_
#define V_CLOCK_H_


#include <villanova-engine/error.h>
#include <villanova-engine/frame.h>


typedef struct _VClock     VClock;
typedef struct _VClockPriv VClockPriv;


/**
 * VClock:
 *
 * Internal timer for frame synchronisation.
 */
struct _VClock
{
	/*< private >*/
	VClockPriv *priv;
};



VClock *v_clock_new  (VError *error);
void    v_clock_free (VClock *clock);


double v_clock_get_timeout (VClock *clock, VFrameRaw *frame);



#endif /* V_CLOCK_H_ */
