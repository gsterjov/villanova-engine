/***************************************************************************
 *            player.c
 *
 *  Fri Dec 12 18:40:02 2008
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



#include <stdio.h>
#include <unistd.h>

#include <villanova-engine/error.h>
#include <villanova-engine/engine.h>




int
main (int argc, char **argv)
{
	printf ("Initiating Engine...\n");
	v_engine_init ();


	VError *err = v_error_new ();
	VEngine *engine = v_engine_new ();


	
	const char *protocol = argv[1];
	const char *uri = argv[2];

	printf ("Opening Media: %s://%s\n", protocol, uri);


	/* open the media file */
	if (v_engine_open (engine, protocol, uri, err) == false)
	{
		printf ("ERROR - %s: %s\n", err->module, err->message);

		v_error_free (err);
		v_engine_free (engine);
		return err->code;
	}



	printf ("Playing Media...\n");

	/* play the media file */
	if (v_engine_play (engine, err) == false)
	{
		printf ("ERROR - %s: %s\n", err->module, err->message);

		v_engine_close (engine);
		v_error_free (err);
		v_engine_free (engine);
		return err->code;
	}



    sleep (3);


	printf ("Closing Engine...\n");

	v_engine_close (engine);


	/* clean up */
	v_error_free (err);
	v_engine_free (engine);


	return 0;
}
