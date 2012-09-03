/***************************************************************************
 *            mem.c
 *
 *  Wed Dec 24 13:51:58 2008
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
 
 
#include "mem.h"
#include <stdlib.h>  /* malloc */
#include <string.h>  /* memset */



/**
 * v_malloc:
 *
 * Creates a new memory block of size @length.
 *
 * Returns: a pointer to the new memory block.
 */
void *
v_malloc (size_t length)
{
	return malloc (length);
}



/**
 * v_mallocz:
 *
 * Creates a new zeroed memory block of size @length.
 *
 * Returns: a pointer to the new memory block.
 */
void *
v_mallocz (size_t length)
{
	return memset (malloc (length), 0, length);
}



/**
 * v_realloc:
 *
 * Resizes the memory block of size @length.
 *
 * Returns: a pointer to the resized memory block.
 */
void *
v_realloc (void *ptr, size_t length)
{
	return realloc (ptr, length);
}



/**
 * v_free:
 *
 * Frees the memory block allocated with v_malloc(), v_mallocz() or v_new().
 */
void
v_free (void *ptr)
{
	free (ptr);
}

