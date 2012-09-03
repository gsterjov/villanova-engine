/***************************************************************************
 *            modules.c
 *
 *  Sat Dec 13 20:47:23 2008
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
 
 

#include "modules.h"
#include "list.h"
#include "mem.h"
#include <stdbool.h>
#include <string.h>  /* strdup, strcmp */




#define REGISTER_INPUT(protocol,module) \
			extern VInput *v_input_##module##_new (void); \
			v_modules_register_input (protocol, v_input_##module##_new)


#define REGISTER_OUTPUT(type,module) \
			extern VOutput *v_output_##module##_new (void); \
			v_modules_register_output (type, v_output_##module##_new)


#define REGISTER_DEMUXER(codec,module) \
			extern VDemuxer *v_demuxer_##module##_new (VCodecID codec_id); \
			v_modules_register_demuxer (codec, v_demuxer_##module##_new)


#define REGISTER_CODEC(codec,module) \
			extern VCodec *v_codec_##module##_new (VCodecID codec_id); \
			v_modules_register_codec (codec, v_codec_##module##_new)




typedef struct _VInputModule   VInputModule;
typedef struct _VOutputModule   VOutputModule;
typedef struct _VDemuxerModule VDemuxerModule;
typedef struct _VCodecModule   VCodecModule;



/*
 * VInputModule:
 * @protocol: the protocol this input module can handle.
 * @create: the callback to create the input module.
 *
 * Simple structure to associate a protocol with an input module callback.
 */
struct _VInputModule
{
	char *protocol;
	CreateInput *create;
};



/*
 * VOutputModule:
 * @protocol: the protocol this input module can handle.
 * @create: the callback to create the input module.
 *
 * Simple structure to associate a protocol with an input module callback.
 */
struct _VOutputModule
{
	VOutputType output_type;
	CreateOutput *create;
};



/*
 * VDemuxerModule:
 * @codec_id: the codec this demuxer module can handle.
 * @create: the callback to create the demuxer module.
 *
 * Simple structure to associate a codec with a demuxer module callback.
 */
struct _VDemuxerModule
{
	VCodecID codec_id;
	CreateDemuxer *create;
};



/*
 * VCodecModule:
 * @codec_id: the codec this parser module can handle.
 * @create: the callback to create the parser module.
 *
 * Simple structure to associate a codec with a parser module callback.
 */
struct _VCodecModule
{
	VCodecID codec_id;
	CreateCodec *create;
};





/* module lists */
static VList *input_modules;
static VList *output_modules;
static VList *demuxer_modules;
static VList *codec_modules;






/**
 * v_modules_init:
 *
 * Initiates all available modules.
 */
void
v_modules_init (void)
{
	/* only initialise modules once */
	static bool initialised = false;
	
	if (initialised)
		return;
	
	
	/* create module lists */
	input_modules   = v_list_new ();
	output_modules  = v_list_new ();
	demuxer_modules = v_list_new ();
	codec_modules   = v_list_new ();
	
	
	/* register inputs */
	REGISTER_INPUT ("file", file);
	
	
	/* register outputs */
	REGISTER_OUTPUT (V_OUTPUT_TYPE_AUDIO, alsa);
	REGISTER_OUTPUT (V_OUTPUT_TYPE_VIDEO, xv);
	
	
	/* register demuxers */
	REGISTER_DEMUXER (V_CODEC_ID_MPEG2, mpeg);
	
	
	/* register codecs */
	REGISTER_CODEC (V_CODEC_ID_MPEG2, libavcodec);
	REGISTER_CODEC (V_CODEC_ID_MP3, libavcodec);
	REGISTER_CODEC (V_CODEC_ID_AC3, libavcodec);
	REGISTER_CODEC (V_CODEC_ID_DTS, libavcodec);

	REGISTER_CODEC (V_CODEC_ID_SUBPIC, libavcodec);
}




/**
 * v_modules_register_input:
 * @protocol: the protocol to handle.
 * @create: the module creation callback.
 *
 * Registers an input module to handle @protocol.
 */
void
v_modules_register_input (const char *protocol, CreateInput *create)
{
	VInputModule *module = v_new (VInputModule);
	
	/* default values */
	module->protocol = strdup (protocol);
	module->create = create;
	
	/* add to list */
	v_list_append (input_modules, module);
};



/**
 * v_modules_register_output:
 * @protocol: the protocol to handle.
 * @create: the module creation callback.
 *
 * Registers an input module to handle @protocol.
 */
void
v_modules_register_output (VOutputType output_type, CreateOutput *create)
{
	VOutputModule *module = v_new (VOutputModule);
	
	/* default values */
	module->output_type = output_type;
	module->create = create;
	
	/* add to list */
	v_list_append (output_modules, module);
};



/**
 * v_modules_register_demuxer:
 * @codec_id: the codec type to handle.
 * @create: the module creation callback.
 *
 * Registers a demuxer module to handle @codec_id.
 */
void
v_modules_register_demuxer (VCodecID codec_id, CreateDemuxer *create)
{
	VDemuxerModule *module = v_new (VDemuxerModule);
	
	/* default values */
	module->codec_id = codec_id;
	module->create = create;
	
	/* add to list */
	v_list_append (demuxer_modules, module);
};



/**
 * v_modules_register_parser:
 * @codec_id: the codec type to handle.
 * @create: the module creation callback.
 *
 * Registers a parser module to handle @codec_id.
 */
void
v_modules_register_codec (VCodecID codec_id, CreateCodec *create)
{
	VCodecModule *module = v_new (VCodecModule);
	
	/* default values */
	module->codec_id = codec_id;
	module->create = create;
	
	/* add to list */
	v_list_append (codec_modules, module);
};





/**
 * v_modules_find_input:
 * @protocol: the protocol which input must support.
 * @error: a #VError, or %NULL.
 *
 * Finds an appropriate input module which can handle @protocol sources.
 *
 * Returns: a #VInput structure if successful, %NULL otherwise.
 */
VInput *
v_modules_find_input (const char *protocol, VError *error)
{
	VListNode *node;
	
	
	/* look for an input which supports the specified protocol */
	for (node = input_modules->first; node; node = node->next)
	{
		VInputModule *module = (VInputModule *) node->data;
		
		/* found one */
		if (strcmp (module->protocol, protocol) == 0)
			return module->create ();
	}
	
	
	/* cannot find an appropriate module */
	v_error_set (error,
				 V_ERROR_DOMAIN_MODULES,
				 V_MODULES_ERROR_NOPROTOCOL,
				 "modules-manager",
				 "Cannot find a module which supports the protocol '%s'",
				 protocol);
	
	return NULL;
}





/**
 * v_modules_find_output:
 * @protocol: the protocol which input must support.
 * @error: a #VError, or %NULL.
 *
 * Finds an appropriate input module which can handle @protocol sources.
 *
 * Returns: a #VInput structure if successful, %NULL otherwise.
 */
VOutput *
v_modules_find_output (VOutputType output_type, VError *error)
{
	VListNode *node;
	
	
	/* look for an output of the specified type */
	for (node = output_modules->first; node; node = node->next)
	{
		VOutputModule *module = (VOutputModule *) node->data;
		
		/* found one */
		if (module->output_type == output_type)
			return module->create ();
	}
	
	
	/* cannot find an appropriate module */
	v_error_set (error,
				 V_ERROR_DOMAIN_MODULES,
				 V_MODULES_ERROR_NOOUTPUT,
				 "modules-manager",
				 "Cannot find a module which supports the output type '%s'",
				 v_output_type_string (output_type));
	
	return NULL;
}





/**
 * v_modules_find_demuxer:
 * @codec_id: the codec type to handle.
 * @error: a #VError, or %NULL.
 *
 * Finds an appropriate demuxer module which can handle @codec_id data.
 *
 * Returns: a #VDemuxer structure if successful, %NULL otherwise.
 */
VDemuxer *
v_modules_find_demuxer (VCodecID codec_id, VError *error)
{
	VListNode *node;
	
	
	/* look for a demuxer which supports the specified codec */
	for (node = demuxer_modules->first; node; node = node->next)
	{
		VDemuxerModule *module = (VDemuxerModule *) node->data;
		
		/* found one */
		if (module->codec_id == codec_id)
			return module->create (codec_id);
	}
	
	
	/* cannot find an appropriate module */
	v_error_set (error,
				 V_ERROR_DOMAIN_MODULES,
				 V_MODULES_ERROR_NODEMUXER,
				 "modules-manager",
				 "Cannot find a module which can demux '%s' data",
				 v_codec_id_string (codec_id));
	
	return NULL;
}




/**
 * v_modules_find_parser:
 * @codec_id: the codec type to handle.
 * @error: a #VError, or %NULL.
 *
 * Finds an appropriate parser module which can handle @codec_id data.
 *
 * Returns: a #VParser structure if successful, %NULL otherwise.
 */
VCodec *
v_modules_find_codec (VCodecID codec_id, VError *error)
{
	VListNode *node;
	
	
	/* look for a parser which supports the specified codec */
	for (node = codec_modules->first; node; node = node->next)
	{
		VCodecModule *module = (VCodecModule *) node->data;
		
		/* found one */
		if (module->codec_id == codec_id)
			return module->create (codec_id);
	}
	
	
	/* cannot find an appropriate module */
	v_error_set (error,
				 V_ERROR_DOMAIN_MODULES,
				 V_MODULES_ERROR_NOCODEC,
				 "modules-manager",
				 "Cannot find a module which supports the codec '%s'",
				 v_codec_id_string (codec_id));
	
	return NULL;
}


