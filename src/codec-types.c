/***************************************************************************
 *            codec-types.c
 *
 *  Tue Dec 30 17:00:52 2008
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
 
 
#include "codec-types.h"
#include <stddef.h>  /* NULL */





/**
 * v_codec_id_type:
 * @id: the codec ID to convert.
 *
 * Gets the type of the codec @id.
 *
 * Returns: the full codec name of @id.
 */
VCodecType
v_codec_id_type (VCodecID id)
{
	switch (id)
	{
		/* video codecs */
		case V_CODEC_ID_MPEG2:
			return V_CODEC_TYPE_VIDEO;
			
		
		/* audio codecs */
		case V_CODEC_ID_MP3:
		case V_CODEC_ID_AC3:
		case V_CODEC_ID_DTS:
			return V_CODEC_TYPE_AUDIO;
			
			
		/* subtitle codecs */
		case V_CODEC_ID_SUBPIC:
			return V_CODEC_TYPE_SUBTITLE;
	}
	
	
	return V_CODEC_TYPE_UNKNOWN;
}



/**
 * v_codec_id_string:
 * @id: the codec ID to convert.
 *
 * Converts @id into a user readable string.
 *
 * Returns: the full codec name of @id.
 */
char *
v_codec_id_string (VCodecID id)
{
	switch (id)
	{
		/* video codecs */
		case V_CODEC_ID_MPEG2:
			return "MPEG-2 Video";
			
			
		/* audio codecs */
		case V_CODEC_ID_MP3:
			return "MPEG-1 Audio Layer 3 (MP3)";
			
		case V_CODEC_ID_AC3:
			return "ATSC A/52a (Dolby Digital AC3)";
			
		case V_CODEC_ID_DTS:
			return "DTS Coherent Acoustics";
			
			
		case V_CODEC_ID_SUBPIC:
			return "DVD Subpicture";
	}
	
	
	return "Unknown";
}

