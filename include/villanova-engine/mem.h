/***************************************************************************
 *            mem.h
 *
 *  Sat Dec 13 21:55:01 2008
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
 
#ifndef V_MEM_H_
#define V_MEM_H_


#include <stddef.h>



/**
 * v_new:
 * @type: the structure type to create.
 *
 * A convenience macro to create a new memory block of size @type and
 * cast it to @type. It also zeroes the memory block.
 *
 * Returns: a pointer to the new zeroed memory block casted to @type if 
 * successful, %NULL otherwise.
 */
#define v_new(type)  (type *) (v_mallocz (sizeof (type)))



void *v_malloc  (size_t length);
void *v_mallocz (size_t length);
void *v_realloc (void *ptr, size_t length);

void v_free (void *ptr);



#endif /* V_MEM_H_ */

 
