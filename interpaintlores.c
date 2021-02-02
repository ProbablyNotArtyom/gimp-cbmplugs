/* cbmplugs -- Plugins to enable handling of Commodore 64 images in The GIMP
 *
 * Copyright (C) 2002-2005 David Weinehall <tao@acc.umu.se>
 *
 * These plugins are based off of work
 *
 * Copyright (C) 1999 Maurits Rijk <lpeek.mrijk@consunet.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <string.h>
#include <libgimp/gimp.h>

#include "cbmplugs.h"

_runheader
{
	static GimpParam values[2];
	GimpRunMode run_mode;
	gint32 image_ID;

	*nreturn_vals = 1;
	*return_vals = values;
	values[0].type = GIMP_PDB_STATUS;
	values[0].data.d_status = GIMP_PDB_CALLING_ERROR;

	if (nparams < 2)
		return;

	run_mode = param[0].data.d_int32;

	if (strcmp(name, "file_interpaint_lores_load") == 0) {
		values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
		image_ID = load_image(param[1].data.d_string, IP64L);

		if (image_ID != -1) {
			*nreturn_vals = 2;
			values[0].data.d_status = GIMP_PDB_SUCCESS;
			values[1].type = GIMP_PDB_IMAGE;
			values[1].data.d_image = image_ID;
		}
	} else if (strcmp(name, "file_interpaint_lores_save") == 0) {
		values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;

		if (!save_image(param[3].data.d_string, param[1].data.d_image,
				IP64L)) {
			*nreturn_vals = 1;
			values[0].data.d_status = GIMP_PDB_SUCCESS;
		}
	}
}

static void query(void)
{
	static GimpParamDef load_args[] = {
		{
			GIMP_PDB_INT32,
			"run_mode",
			"Interactive, non-interactive"
		},
		{
			GIMP_PDB_STRING,
			"filename",
			"The name of the file to load"
		},
		{
			GIMP_PDB_STRING,
			"raw_filename",
			"The name of the file to load"
		},
	};

	static GimpParamDef load_return_vals[] = {
		{
			GIMP_PDB_IMAGE,
			"image",
			"Output image"
		},
	};

	static int nload_args = sizeof (load_args) / sizeof (load_args[0]);
	static int nload_return_vals = sizeof (load_return_vals) /
				       sizeof (load_return_vals[0]);

	static GimpParamDef save_args[] = {
		{
			GIMP_PDB_INT32,
			"run_mode",
			"Interactive, non-interactive"
		},
		{
			GIMP_PDB_IMAGE,
			"image",
			"Input image"
		},
		{
			GIMP_PDB_DRAWABLE,
			"drawable",
			"Drawable to save"
		},
		{
			GIMP_PDB_STRING,
			"filename",
			"The name of the file to save the image in"
		},
		{
			GIMP_PDB_STRING,
			"raw_filename",
			"The name of the file to save the image in"
		},
	};

	static int nsave_args = sizeof (save_args) / sizeof (save_args[0]);

	gimp_install_procedure("file_interpaint_lores_load",
			       "loads images in Interpaint Lores file format",
			       "This plug-in loads images in Interpaint Lores file format.",
			       "David Weinehall <tao@acc.umu.se>",
			       "David Weinehall",
			       "2002",
			       "<Load>/Interpaint Lores",
			       NULL,
			       GIMP_PLUGIN,
			       nload_args,
			       nload_return_vals,
			       load_args,
			       load_return_vals);

	gimp_register_load_handler("file_interpaint_lores_load", "ip64m", "");

	gimp_install_procedure("file_interpaint_lores_save",
			       "saves images in Interpaint Lores file format",
			       "This plug-in saves images in Interpaint Lores file format.",
			       "David Weinehall <tao@acc.umu.se>",
			       "David Weinehall",
			       "2002",
			       "<Save>/Interpaint Lores",
			       "INDEXED",
			       GIMP_PLUGIN,
			       nsave_args,
			       0,
			       save_args,
			       NULL);

	gimp_register_save_handler("file_interpaint_lores_save", "ip64m", "");
}

GimpPlugInInfo PLUG_IN_INFO = {
	NULL,			/* init_proc */
	NULL,			/* quit_proc */
	query,			/* query_proc */
	run,			/* run_proc */
};

MAIN();
