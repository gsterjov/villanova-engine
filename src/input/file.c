/***************************************************************************
 *            file.c
 *
 *  Sat Dec 13 21:25:16 2008
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
#include "mem.h"
#include <errno.h>
#include <fcntl.h>   /* O_RDONLY */
#include <unistd.h>  /* SEEK_CUR */
#include <string.h>  /* strerror */


typedef struct _VInputFile VInputFile;



/*
 * VInputFile:
 * @fd: the file descriptor.
 * @length: the input buffer length.
 * @buffer: the input buffer.
 *
 * Private structure for #VInputFile inheriting #VInput.
 */
struct _VInputFile
{
	VInput parent;
	
	int fd;
	int length;
	uint8_t *buffer;
	
};




/*
 * v_input_file_fill_buffer:
 * @buffer: the buffer to fill.
 * @user_data: a void* casted #VInputFile.
 *
 * Fills the input buffer with new data.
 *
 * Returns: the amount of bytes read.
 */
static int
v_input_file_fill_buffer (uint8_t *buffer, void *user_data)
{
	VInputFile *self = (VInputFile *) user_data;
	return read (self->fd, buffer, self->length);
}




/*
 * v_input_file_skip_data:
 * @length: the amount of bytes to skip.
 * @user_data: a void* casted #VInputFile.
 *
 * Skips @length amount of bytes.
 *
 * Returns: the amount of bytes skipped.
 */
static int
v_input_file_skip_data (int length, void *user_data)
{
	VInputFile *self = (VInputFile *) user_data;
	return lseek (self->fd, length, SEEK_CUR);
}





/*
 * v_input_file_open:
 * @input: a #VInput.
 * @error: a #VError, or %NULL.
 *
 * Opens the input stream and creates an associated buffer.
 *
 * Returns: a #VBuffer structure if successful, %NULL otherwise.
 */
static VBuffer *
v_input_file_open (VInput *input, VError *error)
{
	VInputFile *self = (VInputFile *) input;
	
	
	
	/* open file descriptor */
	self->fd = open (input->uri, O_RDONLY, 0666);
	
	/* open failed */
	if (self->fd < 0)
	{
		int err = errno;
		
		v_error_set (error,
					 V_ERROR_DOMAIN_INPUT,
					 -err,
					 "input-file",
					 strerror (err));
		
		return NULL;
	}
	
	
	
	/* allocate buffer */
	int length = 2048;
	self->buffer = v_malloc (length);
	
	
	/* create input buffer */
	self->length = length;
	
	return v_buffer_new (self->buffer,
						 v_input_file_fill_buffer,
						 v_input_file_skip_data,
						 input);;
}




/*
 * v_input_file_close:
 * @input: a #VInput.
 *
 * Closes the input stream.
 */
static void
v_input_file_close (VInput *input)
{
	VInputFile *self = (VInputFile *) input;
	
	close  (self->fd);
	v_free (self->buffer);
}



/**
 * v_input_file_new:
 *
 * Creates a new file stream input.
 *
 * Returns: a #VInput structure.
 */
VInput *
v_input_file_new (void)
{
	VInputFile *ret = v_new (VInputFile);
	
	VInput *input = (VInput *) ret;
	
	
	/* set interface methods */
	input->open  = v_input_file_open;
	input->close = v_input_file_close;
	
	
	return input;
}

