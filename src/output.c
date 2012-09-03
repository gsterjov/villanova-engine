/***************************************************************************
 *            output.c
 *
 *  Feb 9, 2009 6:21:07 PM
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
#include "modules.h"




/**
 * v_output_new:
 * @error: a #VError, or %NULL.
 *
 * Creates a new #VOutput.
 *
 * Returns: a #VOutput structure if successful, %NULL otherwise.
 */
VOutput *
v_output_new (VOutputType output_type, VError *error)
{
	VOutput *ret = v_modules_find_output (output_type, error);
	
	/* no module */
	if (ret == NULL)
		return NULL;
	
	
	return ret;
}




/**
 * v_output_free:
 * @output: a #VOutput to free.
 *
 * Free's @output and its contents.
 */
void
v_output_free (VOutput *output)
{
	v_free (output);
}





/**
 * v_output_write:
 * @output: a #VOutput.
 * @frame: a #VFrame to write.
 *
 * Writes @frame to the output device.
 */
void
v_output_write (VOutput *output, VFrame *frame)
{
	output->write (output, frame);
}



/**
 * v_output_open:
 * @output: a #VOutput.
 * @frame: a #VFrame to write.
 *
 * Writes @frame to the output device.
 */
void
v_output_open (VOutput *output, VStream *stream)
{
	output->open (output, stream);
}



/**
 * v_output_close:
 * @output: a #VOutput.
 * @frame: a #VFrame to write.
 *
 * Writes @frame to the output device.
 */
void
v_output_close (VOutput *output)
{
	output->close (output);
}




void
v_output_write_sub (VOutput *output, VFrame *frame)
{
	output->write_sub (output, frame);
}





/**
 * v_output_type_string:
 * @id: the codec ID to convert.
 *
 * Converts @id into a user readable string.
 *
 * Returns: the full codec name of @id.
 */
char *
v_output_type_string (VOutputType output_type)
{
	switch (output_type)
	{
		case V_OUTPUT_TYPE_AUDIO:
			return "Audio";
			
		case V_OUTPUT_TYPE_VIDEO:
			return "Video";
	}
	
	
	return "Unknown";
}

