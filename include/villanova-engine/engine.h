/***************************************************************************
 *            engine.h
 *
 *  Fri Dec 12 18:48:12 2008
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
 
#ifndef V_ENGINE_H_
#define V_ENGINE_H_


#include <stdbool.h>
#include <villanova-engine/error.h>
#include <villanova-engine/input.h>
#include <villanova-engine/output.h>



typedef struct _VEngine     VEngine;
typedef struct _VEnginePriv VEnginePriv;


/**
 * VEngine:
 * @uri: the absolute path of the opened media, including the handler.
 *
 * Contains all the various components needed for playback.
 */
struct _VEngine
{
	VInput  *input;
	VOutput *audio_output;
	VOutput *video_output;
	
	
	/*< private >*/
	VEnginePriv *priv;
};



void     v_engine_init (void);
VEngine *v_engine_new  (void);
void     v_engine_free (VEngine *engine);


void v_engine_close (VEngine *engine);
bool v_engine_open  (VEngine    *engine,
					 const char *protocol,
					 const char *uri,
					 VError     *error);

bool v_engine_play  (VEngine *engine, VError *error);
void v_engine_pause (VEngine *engine);
void v_engine_stop  (VEngine *engine);




#endif /* V_ENGINE_H_ */
