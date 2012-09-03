/***************************************************************************
 *            colorspace.h
 *
 *  Apr 9, 2009 2:22:48 PM
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

#ifndef V_COLORSPACE_H_
#define V_COLORSPACE_H_


#include <villanova-engine/error.h>
#include <villanova-engine/frame.h>
#include <villanova-engine/codec-types.h>



typedef struct _VColorspace     VColorspace;
typedef struct _VColorspacePriv VColorspacePriv;



/**
 * VColorspace:
 *
 * Converts video data from one colorspace/pixel format to another.
 */
struct _VColorspace
{
	/*< private >*/
	VColorspacePriv *priv;
};




VColorspace *v_colorspace_new (VPixelFormat src,
                               VPixelFormat dest,
                               int width,
                               int height,
                               VError *error);



void v_colorspace_free (VColorspace *colorspace);



VFrame *v_colorspace_convert (VColorspace *colorspace, VFrame *frame);



#endif /* V_COLORSPACE_H_ */
