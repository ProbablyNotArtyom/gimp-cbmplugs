/* cbmplugs -- Plugins to enable handling of Commodore 64 images in The GIMP
 *
 * Copyright © 2002-2007 David Weinehall <tao@acc.umu.se>
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
#ifndef __C64PLUGS_H__
#define __C64PLUGS_H__

#ifdef GIMP12
#define gimp_image_get_active_drawable	gimp_image_active_drawable
#define	GimpRunMode	GimpRunModeType
#define _runheader	static void run(gchar *name, gint nparams, \
					GimpParam *param, \
					gint *nreturn_vals, \
					GimpParam **return_vals)
#else
#define _runheader	static void run(const gchar *name, gint nparams, \
					const GimpParam *param, \
					gint *nreturn_vals, \
					GimpParam **return_vals)
#endif

enum {
	KOALA = 0x0,
	ADVOCP,
	OCP,
	SUXX,
	SUXXV2,
	SUXXV3,
	VIDCOM,
	ARTIST,
	BLAZE,
	IMSYS,
	FLI,
	AFLI,
	SPRITE,
	TLIGHT,
	RAIN,
	EAFLI,
	IP64H,
	IP64L,
	DRAZ,
	SHFLI,
	SHFXL,
	HWS,
} gfxformat;

gint32 load_image(char *filename, guint8 type);
gint save_image(char *filename, gint32 image_ID, guint8 type);

#endif /* __C64PLUGS_H__ */
