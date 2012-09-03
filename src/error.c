/***************************************************************************
 *            error.c
 *
 *  Fri Dec 12 19:45:01 2008
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
 


#include "error.h"
#include "mem.h"
#include <stdarg.h>
#include <stdlib.h>  /* realloc */
#include <string.h>  /* strdup */
#include <stdio.h>   /* vsnprintf */



/*
 * v_error_parse_message:
 * @message: the message format to parse.
 * @args: the variable argument list.
 *
 * Returns a string with the format expanded out with the values in @args.
 *
 * Returns: a formatted string if successful, %NULL otherwise.
 */
char *
v_error_parse_message (const char *message, va_list args)
{
	int n;
	int len = 0;
	char *str = v_mallocz (len);
	va_list args2;
	
	
	/* memory allocation failed */
	if (str == NULL)
		return NULL;
	
	
	/* get the string size */
	va_copy (args2, args);
	n = vsnprintf (str, len, message, args2);
	
	/* an error occured */
	if (n < 0)
	{
		free (str);
		return NULL;
	}
	
	
	
	/* resize to the exact size needed */
	if (n >= len)
	{
		len = n + 1;
		str = realloc (str, len);
		
		/* memory allocation failed */
		if (str == NULL)
			return NULL;
	}
	
	
	/* actually write to the string */
	if (vsnprintf (str, len, message, args) < 0)
	{
		v_free (str);
		return NULL;
	}
	
	
	return str;
}




/**
 * v_error_new:
 *
 * Creates a new #VError with default values.
 *
 * Returns: a #VError structure.
 */
VError *
v_error_new (void)
{
	return v_new (VError);
}



/**
 * v_error_free:
 * @error: a #VError to free.
 *
 * Free's @error and its contents.
 */
void
v_error_free (VError *error)
{
	v_free (error->module);
	v_free (error->message);
	v_free (error);
}




/**
 * v_error_set:
 * @error: a #VError to set.
 * @domain: a #VErrorDomain value.
 * @code: a custom error value from @domain or a standard
 * error code from <errno.h>.
 * @module: the module name.
 * @message: printf()-style error message format.
 * @Varargs: args for @message
 *
 * Sets @error with the specified parameter values. If @error is %NULL the 
 * function simply returns.
 *
 * <note>
 *   <para>
 *     If a standard error code is given it must be negated to prevent any
 *     namespace clashes with #VErrorCode values.
 *     For example, -ENOMEM is valid whereas ENOMEM is not.
 *   </para>
 * </note>
 */
void
v_error_set (VError *error,
			 VErrorDomain domain,
			 signed int code,
			 const char *module,
			 const char *message,
			 ...)
{
	va_list args;
	va_start (args, message);
	
	
	/* set error fields */
	if (error != NULL)
	{
		error->domain = domain;
		error->code = code;
		error->module = strdup (module);
		error->message = v_error_parse_message (message, args);
	}
	
	va_end (args);
}


