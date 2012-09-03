/***************************************************************************
 *            error.h
 *
 *  Fri Dec 12 19:29:46 2008
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
 
  
#ifndef V_ERROR_H_
#define V_ERROR_H_



typedef struct _VError       VError;
typedef enum   _VErrorDomain VErrorDomain;




/**
 * VErrorDomain:
 * @V_ERROR_DOMAIN_UNKNOWN: an unknown domain.
 * @V_ERROR_DOMAIN_MEMORY: memory allocator domain.
 * @V_ERROR_DOMAIN_MODULES: module manager domain.
 * @V_ERROR_DOMAIN_ENGINE: the high level #VEngine domain.
 * @V_ERROR_DOMAIN_INPUT: the #VInput domain.
 * @V_ERROR_DOMAIN_DEMUXER: the #VDemuxer domain.
 *
 * Domains that an error can originate from.
 */
enum _VErrorDomain
{
	V_ERROR_DOMAIN_UNKNOWN,
	V_ERROR_DOMAIN_MEMORY,
	V_ERROR_DOMAIN_MODULES,
	V_ERROR_DOMAIN_ENGINE,
	V_ERROR_DOMAIN_INPUT,
	V_ERROR_DOMAIN_DEMUXER
};




/**
 * VError:
 * @domain: the #VErrorDomain that the #VError refers to.
 * @code: the specific error that occured within the @domain, or a standard 
 * error code defined in <errno.h>. All standard error values are negative.
 * @module: the name of the module that the #VError originated from.
 * @message: a human readable error message.
 *
 * Contains details on the specific error that occured.
 */
struct _VError
{
	VErrorDomain domain;
	signed int code;
	
	char *module;
	char *message;
};



VError *v_error_new  (void);
void    v_error_free (VError *error);


void v_error_set (VError *error,
				  VErrorDomain domain,
				  signed int code,
				  const char *module,
				  const char *message,
				  ...);



#endif /* V_ERROR_H_ */

 
