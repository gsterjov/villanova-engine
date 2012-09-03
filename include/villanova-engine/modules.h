/***************************************************************************
 *            modules.h
 *
 *  Sat Dec 13 20:47:58 2008
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
 
#ifndef V_MODULES_H_
#define V_MODULES_H_


#include <villanova-engine/error.h>
#include <villanova-engine/input.h>
#include <villanova-engine/output.h>
#include <villanova-engine/demuxer.h>
#include <villanova-engine/codec.h>



/**
 * CreateInput:
 *
 * Callback prototype for creating a new module inheriting #VInput.
 */
typedef VInput *CreateInput (void);


/**
 * CreateOutput:
 *
 * Callback prototype for creating a new module inheriting #VOutput.
 */
typedef VOutput *CreateOutput (void);


/**
 * CreateDemuxer:
 *
 * Callback prototype for creating a new module inheriting #VDemuxer.
 */
typedef VDemuxer *CreateDemuxer (VCodecID codec_id);


/**
 * CreateCodec:
 *
 * Callback prototype for creating a new module inheriting #VParser.
 */
typedef VCodec *CreateCodec (VCodecID codec_id);




typedef enum _VModulesError VModulesError;



/**
 * VModulesError:
 * @V_MODULES_ERROR_NOPROTOCOL: cannot find an input module supporting the
 * specified protocol.
 * @V_MODULES_ERROR_NODEMUXER: cannot find a demuxer module supporting the 
 * specified codec type.
 * @V_MODULES_ERROR_NOCODEC: cannot find a codec module supporting the 
 * specified codec type.
 *
 * Error codes for module loading.
 */
enum _VModulesError
{
	V_MODULES_ERROR_NOPROTOCOL,
	V_MODULES_ERROR_NOOUTPUT,
	V_MODULES_ERROR_NODEMUXER,
	V_MODULES_ERROR_NOCODEC
};




void v_modules_init (void);


void v_modules_register_input   (const char *protocol, CreateInput *create);
void v_modules_register_output  (VOutputType output_type, CreateOutput *create);
void v_modules_register_demuxer (VCodecID codec_id, CreateDemuxer *create);
void v_modules_register_codec   (VCodecID codec_id, CreateCodec *create);


VInput   *v_modules_find_input   (const char *protocol, VError *error);
VOutput  *v_modules_find_output  (VOutputType output_type, VError *error);
VDemuxer *v_modules_find_demuxer (VCodecID codec_id, VError *error);
VCodec   *v_modules_find_codec   (VCodecID codec_id, VError *error);



#endif /* V_MODULES_H_ */

 
