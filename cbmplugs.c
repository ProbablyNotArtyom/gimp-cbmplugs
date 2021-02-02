/* cbmplugs -- Plugins to enable handling of Commodore 64 images in The GIMP
 *
 * Copyright (C) 2002-2006 David Weinehall <tao@acc.umu.se>
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

#include <stdio.h>
#include <libgimp/gimp.h>

#include "cbmplugs.h"

enum {
	LORES = 0x0,
	HIRES,
} gfxmode;

enum {
	NORMAL = 0x0,
	FLI8,
} colmode;

/* Standard C64 and C128 VIC */
static guchar vicpalette[] = {
	0x00, 0x00, 0x00,	/* Black */
	0xff, 0xff, 0xff,	/* White */
	0x68, 0x37, 0x2b,	/* Red */
	0x70, 0xa4, 0xb2,	/* Cyan */
	0x6f, 0x3d, 0x86,	/* Purple */
	0x58, 0x8d, 0x43,	/* Green */
	0x35, 0x28, 0x79,	/* Blue */
	0xb8, 0xc7, 0x6f,	/* Yellow */
	0x6f, 0x4f, 0x25,	/* Orange */
	0x43, 0x39, 0x00,	/* Brown */
	0x9a, 0x67, 0x59,	/* Light Red */
	0x44, 0x44, 0x44,	/* Dark Gray */
	0x6c, 0x6c, 0x6c,	/* Medium Gray */
	0x9a, 0xd2, 0x84,	/* Light Green */
	0x6c, 0x5e, 0xb5,	/* Light Blue */
	0x95, 0x95, 0x95	/* Light Gray */
};

#define KOALA_BASE	0x6000
#define KOALA_BITMAP	(0x6000 - KOALA_BASE + 2)
#define KOALA_MCOLOUR	(0x7f40 - KOALA_BASE + 2)
#define KOALA_COLOUR	(0x8328 - KOALA_BASE + 2)
#define KOALA_BG	(0x8710 - KOALA_BASE + 2)
#define KOALA_SIZE	(0x8711 - KOALA_BASE + 2)

#define ADVOCP_BASE	0x2000
#define ADVOCP_BITMAP	(0x2000 - ADVOCP_BASE + 2)
#define ADVOCP_MCOLOUR	(0x3f40 - ADVOCP_BASE + 2)
#define ADVOCP_BG	(0x4328 - ADVOCP_BASE + 2)
#define ADVOCP_COLOUR	(0x4338 - ADVOCP_BASE + 2)
#define ADVOCP_SIZE	(0x4720 - ADVOCP_BASE + 2)

#define OCP_BASE	0x2000
#define OCP_BITMAP	(0x2000 - OCP_BASE + 2)
#define OCP_MCOLOUR	(0x3f40 - OCP_BASE + 2)
#define OCP_SIZE	(0x4328 - OCP_BASE + 2)

#define SUXXV2_BASE	0x5a24
#define SUXXV2_COLOUR	(0x5a24 - SUXXV2_BASE + 2)
#define SUXXV2_MCOLOUR	(0x5c18 - SUXXV2_BASE + 2)
#define SUXXV2_BITMAP	(0x6000 - SUXXV2_BASE + 2)
#define SUXXV2_SIZE	(0x7f40 - SUXXV2_BASE + 2)

#define SUXXV3_BASE	0x5a23
#define SUXXV3_BG	(0x5a23 - SUXXV3_BASE + 2)
#define SUXXV3_COLOUR	(0x5a24 - SUXXV3_BASE + 2)
#define SUXXV3_MCOLOUR	(0x5c18 - SUXXV3_BASE + 2)
#define SUXXV3_BITMAP	(0x6000 - SUXXV3_BASE + 2)
#define SUXXV3_SIZE	(0x7f40 - SUXXV3_BASE + 2)

#define VIDCOM_BASE	0x5800
#define VIDCOM_COLOUR	(0x5800 - VIDCOM_BASE + 2)
#define VIDCOM_MCOLOUR	(0x5c00 - VIDCOM_BASE + 2)
#define VIDCOM_BITMAP	(0x6000 - VIDCOM_BASE + 2)
#define VIDCOM_SIZE	(0x7f40 - VIDCOM_BASE + 2)

#define ARTIST_BASE	0x4000
#define ARTIST_BITMAP	(0x4000 - ARTIST_BASE + 2)
#define ARTIST_MCOLOUR	(0x6000 - ARTIST_BASE + 2)
#define ARTIST_COLOUR	(0x6400 - ARTIST_BASE + 2)
#define ARTIST_BG	(0x67ff - ARTIST_BASE + 2)
#define ARTIST_SIZE	(0x6800 - ARTIST_BASE + 2)

#define BLAZE_BASE	0xa000
#define BLAZE_BITMAP	(0xa000 - BLAZE_BASE + 2)
#define BLAZE_MCOLOUR	(0xc000 - BLAZE_BASE + 2)
#define BLAZE_COLOUR	(0xc400 - BLAZE_BASE + 2)
#define BLAZE_SIZE	(0xc800 - BLAZE_BASE + 2)

#define IMSYS_BASE	0x3c00
#define IMSYS_COLOUR	(0x3c00 - IMSYS_BASE + 2)
#define IMSYS_BITMAP	(0x4000 - IMSYS_BASE + 2)
#define IMSYS_MCOLOUR	(0x6000 - IMSYS_BASE + 2)
#define IMSYS_SIZE	(0x63e8 - IMSYS_BASE + 2)

#define FLI_BASE	0x3c00
#define FLI_COLOUR	(0x3c00 - FLI_BASE + 2)
#define FLI_MCOLOUR0	(0x4000 - FLI_BASE + 2)
#define FLI_MCOLOUR1	(0x4400 - FLI_BASE + 2)
#define FLI_MCOLOUR2	(0x4800 - FLI_BASE + 2)
#define FLI_MCOLOUR3	(0x4c00 - FLI_BASE + 2)
#define FLI_MCOLOUR4	(0x5000 - FLI_BASE + 2)
#define FLI_MCOLOUR5	(0x5400 - FLI_BASE + 2)
#define FLI_MCOLOUR6	(0x5800 - FLI_BASE + 2)
#define FLI_MCOLOUR7	(0x5c00 - FLI_BASE + 2)
#define FLI_BITMAP	(0x6000 - FLI_BASE + 2)
#define FLI_SIZE	(0x8000 - FLI_BASE + 2)

#define FLIV2_BASE	0x3b00
#define FLIV2_BG	(0x3b00 - FLIV2_BASE + 2)
#define FLIV2_COLOUR	(0x3c00 - FLIV2_BASE + 2)
#define FLIV2_MCOLOUR0	(0x4000 - FLIV2_BASE + 2)
#define FLIV2_MCOLOUR1	(0x4400 - FLIV2_BASE + 2)
#define FLIV2_MCOLOUR2	(0x4800 - FLIV2_BASE + 2)
#define FLIV2_MCOLOUR3	(0x4c00 - FLIV2_BASE + 2)
#define FLIV2_MCOLOUR4	(0x5000 - FLIV2_BASE + 2)
#define FLIV2_MCOLOUR5	(0x5400 - FLIV2_BASE + 2)
#define FLIV2_MCOLOUR6	(0x5800 - FLIV2_BASE + 2)
#define FLIV2_MCOLOUR7	(0x5c00 - FLIV2_BASE + 2)
#define FLIV2_BITMAP	(0x6000 - FLIV2_BASE + 2)
#define FLIV2_SIZE	(0x7f40 - FLIV2_BASE + 2)

#define AFLI_BASE	0x4000
#define AFLI_MCOLOUR0	(0x4000 - AFLI_BASE + 2)
#define AFLI_MCOLOUR1	(0x4400 - AFLI_BASE + 2)
#define AFLI_MCOLOUR2	(0x4800 - AFLI_BASE + 2)
#define AFLI_MCOLOUR3	(0x4c00 - AFLI_BASE + 2)
#define AFLI_MCOLOUR4	(0x5000 - AFLI_BASE + 2)
#define AFLI_MCOLOUR5	(0x5400 - AFLI_BASE + 2)
#define AFLI_MCOLOUR6	(0x5800 - AFLI_BASE + 2)
#define AFLI_MCOLOUR7	(0x5c00 - AFLI_BASE + 2)
#define AFLI_BITMAP	(0x6000 - AFLI_BASE + 2)
#define AFLI_SIZE	(0x8000 - AFLI_BASE + 2)

#define SPRITE_BASE	0x2000
#define SPRITE_BITMAP	(0x2000 - SPRITE_BASE + 2)

#define TLIGHT_BASE	0x5800
#define TLIGHT_COLOUR	(0x5800 - TLIGHT_BASE + 2)
#define TLIGHT_BG	(0x5bff - TLIGHT_BASE + 2)
#define TLIGHT_MCOLOUR	(0x5c00 - TLIGHT_BASE + 2)
#define TLIGHT_BITMAP	(0x6000 - TLIGHT_BASE + 2)
#define TLIGHT_SIZE	(0x7f40 - TLIGHT_BASE + 2)

#define RAIN_BASE	0x4000
#define RAIN_SCREEN1	(0x4000 - RAIN_BASE + 2)
#define RAIN_MAGIC	(0x43f0 - RAIN_BASE + 2)
#define RAIN_COLTAB	(0x43f4 - RAIN_BASE + 2)
#define RAIN_SCREEN2	(0x4400 - RAIN_BASE + 2)
#define RAIN_CHARMAP	(0x4800 - RAIN_BASE + 2)
#define RAIN_SPRMAP1	(0x5000 - RAIN_BASE + 2)
#define RAIN_SPRMAP2	(0x5a00 - RAIN_BASE + 2)

#define EAFLI_BASE	0x8000
#define EAFLI_MCOLOUR0	(0x8000 - EAFLI_BASE + 2)
#define EAFLI_SPRMAP0	(0x81c0 - EAFLI_BASE + 2)
#define EAFLI_SPRCOL0	(0x81ff - EAFLI_BASE + 2)
#define EAFLI_SPRCOL1	(0x823f - EAFLI_BASE + 2)
#define EAFLI_SPRCOL3	(0x827f - EAFLI_BASE + 2)
#define EAFLI_SPRCOL2	(0x82bf - EAFLI_BASE + 2)
#define EAFLI_MFLAG	(0x82ff - EAFLI_BASE + 2)
#define EAFLI_MCOLOUR1	(0x8400 - EAFLI_BASE + 2)
#define EAFLI_SPRMAP1	(0x85c0 - EAFLI_BASE + 2)
#define EAFLI_MCOLOUR2	(0x8800 - EAFLI_BASE + 2)
#define EAFLI_SPRMAP2	(0x89c0 - EAFLI_BASE + 2)
#define EAFLI_MCOLOUR3	(0x8c00 - EAFLI_BASE + 2)
#define EAFLI_SPRMAP3	(0x8dc0 - EAFLI_BASE + 2)
#define EAFLI_MCOLOUR4	(0x9000 - EAFLI_BASE + 2)
#define EAFLI_MCOLOUR5	(0x9400 - EAFLI_BASE + 2)
#define EAFLI_MCOLOUR6	(0x9800 - EAFLI_BASE + 2)
#define EAFLI_MCOLOUR7	(0x9c00 - EAFLI_BASE + 2)
#define EAFLI_BITMAP	(0xa000 - EAFLI_BASE + 2)
#define EAFLI_SIZE	(0xadc0 - EAFLI_BASE + 2)

#define IP64H_BASE	0x4000
#define IP64H_BITMAP	(0x4000 - IP64H_BASE + 2)
#define IP64H_MCOLOUR	(0x5f40 - IP64H_BASE + 2)
#define IP64H_SIZE	(0x6328 - IP64H_BASE + 2)

#define IP64L_BASE	0x4000
#define IP64L_BITMAP	(0x4000 - IP64L_BASE + 2)
#define IP64L_MCOLOUR	(0x5f40 - IP64L_BASE + 2)
#define IP64L_COLOUR	(0x6328 - IP64L_BASE + 2)
#define IP64L_BG	(0x6710 - IP64L_BASE + 2)
#define IP64L_SIZE	(0x6711 - IP64L_BASE + 2)

#define DRAZ_BASE	0x5800
#define DRAZ_COLOUR	(0x5800 - DRAZ_BASE + 2)
#define DRAZ_MCOLOUR	(0x5c00 - DRAZ_BASE + 2)
#define DRAZ_BITMAP	(0x6000 - DRAZ_BASE + 2)
#define DRAZ_BG		(0x7f40 - DRAZ_BASE + 2)
#define DRAZ_SIZE	(0x7f42 - DRAZ_BASE + 2)

#define SHFLI_BASE	0x4000
#define SHFLI_BANK	(0x4000 - SHFLI_BASE + 2)
#define SHFLI_MCOLOUR0	(0x4000 - SHFLI_BASE + 2)
#define SHFLI_SCOLOUR0	(0x43e8 - SHFLI_BASE + 2)
#define SHFLI_SCOLOUR1	(0x43e9 - SHFLI_BASE + 2)
#define SHFLI_MCOLOUR1	(0x4400 - SHFLI_BASE + 2)
#define SHFLI_MCOLOUR2	(0x4800 - SHFLI_BASE + 2)
#define SHFLI_MCOLOUR3	(0x4c00 - SHFLI_BASE + 2)
#define SHFLI_MCOLOUR4	(0x5000 - SHFLI_BASE + 2)
#define SHFLI_MCOLOUR5	(0x5400 - SHFLI_BASE + 2)
#define SHFLI_MCOLOUR6	(0x5800 - SHFLI_BASE + 2)
#define SHFLI_MCOLOUR7	(0x5c00 - SHFLI_BASE + 2)
#define SHFLI_BITMAP	(0x6000 - SHFLI_BASE + 2)
#define SHFLI_SIZE	(0x7e00 - SHFLI_BASE + 2)

#define SHFXL_BASE	0x4000
#define SHFXL_BANK	(0x4000 - SHFXL_BASE + 2)
#define SHFXL_MCOLOUR0	(0x4000 - SHFXL_BASE + 2)
#define SHFXL_SCOLOUR0	(0x43e9 - SHFXL_BASE + 2)
#define SHFXL_MCOLOUR1	(0x4400 - SHFXL_BASE + 2)
#define SHFXL_MCOLOUR2	(0x4800 - SHFXL_BASE + 2)
#define SHFXL_MCOLOUR3	(0x4c00 - SHFXL_BASE + 2)
#define SHFXL_MCOLOUR4	(0x5000 - SHFXL_BASE + 2)
#define SHFXL_MCOLOUR5	(0x5400 - SHFXL_BASE + 2)
#define SHFXL_MCOLOUR6	(0x5800 - SHFXL_BASE + 2)
#define SHFXL_MCOLOUR7	(0x5c00 - SHFXL_BASE + 2)
#define SHFXL_BITMAP	(0x6000 - SHFXL_BASE + 2)
#define SHFXL_SIZE	(0x7c00 - SHFXL_BASE + 2)

#define HWS_BASE	0x4000
#define HWS_BITMAP	(0x4000 - HWS_BASE + 2)
#define HWS_SPRMAP1	(0x5f40 - HWS_BASE + 2)
#define HWS_SPRMAP2	(0x6840 - HWS_BASE + 2)
#define HWS_SIZE	(0x7140 - HWS_BASE + 2)

#define swap(_x, _y)	\
	do { gint _t; _t = _x; _x = _y; _y = _t; } while (0)

#define colsort(_max)	\
	do {								\
		gint _i, _j;						\
									\
		for (_i = 0; col[_i][0] < _max; _i++) {			\
			for (_j = _i + 1;				\
			     col[_j][0] < _max; _j++) {			\
				if (col[_i][1] < col[_j][1]) {		\
					swap(col[_i][0], col[_j][0]);	\
					swap(col[_i][1], col[_j][1]);	\
				}					\
			}						\
		}							\
	} while (0)

static gint rainoff1[] = {
	/* sprite 0-4, lines 0-14 */
	  0x0,   0x3,   0x6,   0x9,   0xc,	/* 0-4 */
	  0xf,  0x12,  0x15,  0x18,  0x1b,	/* 5-9 */
	 0x1e,  0x21,  0x24,  0x27,  0x2a,	/* 10-14 */
	/* sprite 5-9, line 15-20 */
	0x16d, 0x170, 0x173, 0x176, 0x179,	/* 15-19 */
	0x17c,					/* 20 */
	/* sprite 5-9, line 0-9 */
	0x140, 0x143, 0x146, 0x149,		/* 21-24 */
	0x14c, 0x14f, 0x152, 0x155, 0x158,	/* 25-29 */
	0x15b,					/* 30 */
	/* sprite 10-14, line 10-20 */
	0x29e, 0x2a1, 0x2a4, 0x2a7,		/* 31-34 */
	0x2aa, 0x2ad, 0x2b0, 0x2b3, 0x2b6,	/* 35-39 */
	0x2b9, 0x2bc,				/* 40-41 */
	/* sprite 10-14, line 0-4 */
	0x280, 0x283, 0x286,			/* 42-44 */
	0x289, 0x28c,				/* 45-46 */
	/* sprite 15-19, line 5-20 */
	0x3cf, 0x3d2, 0x3d5,			/* 47-49 */
	0x3d8, 0x3db, 0x3de, 0x3e1, 0x3e4,	/* 50-54 */
	0x3e7, 0x3ea, 0x3ed, 0x3f0, 0x3f3,	/* 55-59 */
	0x3f6, 0x3f9, 0x3fc,			/* 60-62 */
	/* sprite 20-24, lines 0-15 */
	0x500, 0x503,				/* 63-64 */
	0x506, 0x509, 0x50c, 0x50f, 0x512,	/* 65-69 */
	0x515, 0x518, 0x51b, 0x51e, 0x521,	/* 70-74 */
	0x524, 0x527, 0x52a, 0x52d,		/* 75-78 */
	/* sprite 25-29, line 16-20 */
	0x670,					/* 79 */
	0x673, 0x676, 0x679, 0x67c,		/* 80-83 */
	/* sprite 25-29, line 0-10 */
	0x640,					/* 84 */
	0x643, 0x646, 0x649, 0x64c, 0x64f,	/* 85-89 */
	0x652, 0x655, 0x658, 0x65b, 0x65e,	/* 90-94 */
	/* sprite 30-34, line 11-20 */
	0x7a1, 0x7a4, 0x7a7, 0x7aa, 0x7ad,	/* 95-99 */
	0x7b0, 0x7b3, 0x7b6, 0x7b9, 0x7bc,	/* 100-104 */
	/* sprite 30-34, line 0-5 */
	0x780, 0x783, 0x786, 0x789, 0x78c,	/* 105-109 */
	0x78f,					/* 110 */
	/* sprite 35-39, line 6-20 */
	0x8d2, 0x8d5, 0x8d8, 0x8db,		/* 111-114 */
	0x8de, 0x8e1, 0x8e4, 0x8e7, 0x8ea,	/* 115-119 */
	0x8ed, 0x8f0, 0x8f3, 0x8f6, 0x8f9,	/* 120-124 */
	0x8fc,					/* 125 */
};

static gint rainoff2[] = {
	/* sprite 0-4, lines 0-14 */
	  0x0,   0x3,   0x6,   0x9,   0xc,	/* 0-4 */
	  0xf,  0x12,  0x15,  0x18,  0x1b,	/* 5-9 */
	 0x1e,  0x21,  0x24,  0x27,  0x2a,	/* 10-14 */
	/* sprite 3-5, line 15-19 */
	 0xed,  0xf0,  0xf3,  0xf6,  0xf9,	/* 15-19 */
	 0xfc,					/* 20 */
	/* sprite 3-5, line 0-8 */
	 0xc0,  0xc3,  0xc6,  0xc9,		/* 21-24 */
	 0xcc,  0xcf,  0xd2,  0xd5,  0xd8,	/* 25-29 */
	 0xdb,					/* 30 */
	/* sprite 6-8, line 10-20 */
	0x19e, 0x1a1, 0x1a4, 0x1a7,		/* 31-34 */
	0x1aa, 0x1ad, 0x1b0, 0x1b3, 0x1b6,	/* 35-39 */
	0x1b9, 0x1bc,				/* 40-41 */
	/* sprite 6-8, line 0-4 */
	0x180, 0x183, 0x186,			/* 42-44 */
	0x189, 0x18c,				/* 45-46 */
	/* sprite 9-11, line 5-20 */
	0x24f, 0x252, 0x255,			/* 47-49 */
	0x258, 0x25b, 0x25e, 0x261, 0x264,	/* 50-54 */
	0x267, 0x26a, 0x26d, 0x270, 0x273,	/* 55-59 */
	0x276, 0x279, 0x27c,			/* 60-62 */
	/* sprite 12-14, lines 0-15 */
	0x300, 0x303,				/* 63-64 */
	0x306, 0x309, 0x30c, 0x30f, 0x312,	/* 65-69 */
	0x315, 0x318, 0x31b, 0x31e, 0x321,	/* 70-74 */
	0x324, 0x327, 0x32a, 0x32d,		/* 75-78 */
	/* sprite 15-17, line 16-20 */
	0x3f0,					/* 79 */
	0x3f3, 0x3f6, 0x3f9, 0x3fc,		/* 80-83 */
	/* sprite 15-17, line 0-10 */
	0x3c0,					/* 84 */
	0x3c3, 0x3c6, 0x3c9, 0x3cc, 0x3cf,	/* 85-89 */
	0x3d2, 0x3d5, 0x3d8, 0x3db, 0x3de,	/* 90-94 */
	/* sprite 18-20, line 11-20 */
	0x4a1, 0x4a4, 0x4a7, 0x4aa, 0x4ad,	/* 95-99 */
	0x4b0, 0x4b3, 0x4b6, 0x4b9, 0x4bc,	/* 100-104 */
	/* sprite 18-20, line 0-5 */
	0x480, 0x483, 0x486, 0x489, 0x48c,	/* 105-109 */
	0x48f,					/* 110 */
	/* sprite 21-23, line 6-20 */
	0x552, 0x555, 0x558, 0x55b,		/* 111-114 */
	0x55e, 0x561, 0x564, 0x567, 0x56a,	/* 115-119 */
	0x56d, 0x570, 0x573, 0x576, 0x579,	/* 120-124 */
	0x57c,					/* 125 */
};

static guint8 tmpcol[1000];
static guint8 tmpbg[200];
static guint8 data[65536];	/* Since the Commodore 64 only has 64kB
				 * of memory, it is reasonable to assume
				 * that no image is larger than this...
				 */
static guchar *buf;

static gint32 rain_to_gimp(void)
{
	gint32 image_ID;
	gint32 layer_ID;
	GimpPixelRgn pixel_rgn;
	GimpDrawable *drawable;
	guint16 baseaddr = data[0] + data[1] * 256;
	guint8 *charmap;
	guint8 *sprmap1;
	guint8 *sprmap2;
	guint8 background;
	guint8 colour0;
	guint8 colour1;
	guint8 colour2;
	guint8 x, y;
	guchar *bufp;
	guint width = 128, height = 128;

	if (baseaddr != RAIN_BASE)
		return -1;

	charmap = data + RAIN_CHARMAP;
	sprmap1 = data + RAIN_SPRMAP1;
	sprmap2 = data + RAIN_SPRMAP2;

	/* If there's no magic signature "rain", this image
	 * does not contain any colour information; hence
	 * we default to the colours used in Rainmaker
	 */
	if (data[RAIN_MAGIC] != 0x52 || data[RAIN_MAGIC + 1] != 0x1 ||
	    data[RAIN_MAGIC + 2] != 0x9 || data[RAIN_MAGIC + 3] != 0x4e) {
		colour0 = 0xb;
		colour1 = 0xc;
		colour2 = 0xf;
		background = 0x0;
	} else {
		colour0 = data[RAIN_COLTAB + 2];
		colour1 = data[RAIN_COLTAB + 1];
		colour2 = data[RAIN_COLTAB];
		background = data[RAIN_COLTAB + 3];
	}

	image_ID = gimp_image_new(width, height, GIMP_INDEXED);
	gimp_image_set_cmap(image_ID, vicpalette, 16);

	layer_ID = gimp_layer_new(image_ID, "Background", width, height,
				  GIMP_INDEXED_IMAGE, 100, GIMP_NORMAL_MODE);
	gimp_image_add_layer(image_ID, layer_ID, 0);

	drawable = gimp_drawable_get(layer_ID);

	gimp_pixel_rgn_init(&pixel_rgn, drawable, 0, 0, width, height,
			    TRUE, FALSE);

	buf = g_new0(guchar, width * height);

	/* layer 0: sprmap2 */
	bufp = buf + 128; /* We begin at (0, 1) rather than (0, 0)
			   * for this layer
			   */

	for (y = 0; y < 126; y++) {
		for (x = 0; x < 8; x++) {
			guint i, tmp;

			tmp = sprmap2[x % 3 + (x / 3) * 6 + rainoff2[y]];

			for (i = 0; i < 8; i++) {
				*bufp++ = (tmp & 1 << (7 - i)) ? colour2 : background;
				*bufp++ = (tmp & 1 << (7 - i)) ? colour2 : background;
			}
		}
	}

	/* layer 1: sprmap1 */
	bufp = buf + 132; /* We begin at (, 1) rather than (0, 0)
			   * for this layer
			   */

	for (y = 0; y < 126; y++) {
		for (x = 0; x < 15; x++) {
			guint i, tmp;

			tmp = sprmap1[x % 3 + (x / 3) * 6 + rainoff1[y]];

			for (i = 0; i < 8; i++, bufp++)
				if (tmp & 1 << (7 - i))
					*bufp = colour1;
		}
		bufp += 8;
	}


	/* layer 2; charmap */
	bufp = buf;

	for (y = 0; y < 128; y++) {
		for (x = 0; x < 16; x++) {
			guint8 i, tmp;

			tmp = charmap[y + x * 128];

			for (i = 0; i < 8; i++, bufp++)
				if (tmp & 1 << (7 - i))
					*bufp = colour0;
		}
	}

	gimp_pixel_rgn_set_rect(&pixel_rgn, buf, 0, 0, 128, 128);
	g_free(buf);
	gimp_drawable_flush(drawable);

	return image_ID;
}

static gint32 bitmap_to_gimp(guint8 type)
{
	gint32 image_ID;
	gint32 layer_ID;
	GimpPixelRgn pixel_rgn;
	GimpDrawable *drawable;
	guint row, col;
	guint xoffset = 0;
	guint height = 200;
	guint width = 320;
	guint8 *bufp;
	guint8 *background = NULL;
	guint8 *mcolour0 = NULL;
	guint8 *mcolour1 = NULL;
	guint8 *mcolour2 = NULL;
	guint8 *mcolour3 = NULL;
	guint8 *mcolour4 = NULL;
	guint8 *mcolour5 = NULL;
	guint8 *mcolour6 = NULL;
	guint8 *mcolour7 = NULL;
	guint8 *scolour0 = NULL;
	guint8 *scolour1 = NULL;
	guint8 *scolour2 = NULL;
	guint8 *scolour3 = NULL;
	guint8 *sprmap0 = NULL;
	guint8 *sprmap1 = NULL;
	guint8 *sprmap2 = NULL;
	guint8 *sprmap3 = NULL;
	guint8 *colour = NULL;
	guint8 *bitmap = NULL;
	guint8 *bank = NULL;
	guint16 baseaddr = data[0] + data[1] * 256;
	guint8 colmode = NORMAL;
	guint8 smode = HIRES;
	guint8 mode = LORES;

	if (type == SUXX) {
		if (baseaddr == SUXXV2_BASE)
			type = SUXXV2;
		else if (baseaddr == SUXXV3_BASE)
			type = SUXXV3;
		else
			return -1;
	}

	switch (type) {
	case ADVOCP:
		if (baseaddr != ADVOCP_BASE)
			return -1;

		bitmap = data + ADVOCP_BITMAP;
		mcolour0 = data + ADVOCP_MCOLOUR;
		colour = data + ADVOCP_COLOUR;
		background = data + ADVOCP_BG;
		break;

	case AFLI:
		if (baseaddr != AFLI_BASE)
			return -1;

		bitmap = data + AFLI_BITMAP;
		mcolour0 = data + AFLI_MCOLOUR0;
		mcolour1 = data + AFLI_MCOLOUR1;
		mcolour2 = data + AFLI_MCOLOUR2;
		mcolour3 = data + AFLI_MCOLOUR3;
		mcolour4 = data + AFLI_MCOLOUR4;
		mcolour5 = data + AFLI_MCOLOUR5;
		mcolour6 = data + AFLI_MCOLOUR6;
		mcolour7 = data + AFLI_MCOLOUR7;
		colmode = FLI8;
		mode = HIRES;
		break;

	case ARTIST:
		if (baseaddr != ARTIST_BASE)
			return -1;

		bitmap = data + ARTIST_BITMAP;
		mcolour0 = data + ARTIST_MCOLOUR;
		colour = data + ARTIST_COLOUR;
		background = data + ARTIST_BG;
		break;

	case BLAZE:
		if (baseaddr != BLAZE_BASE)
			return -1;

		bitmap = data + BLAZE_BITMAP;
		mcolour0 = data + BLAZE_MCOLOUR;
		colour = data + BLAZE_COLOUR;
		background = NULL;
		break;

	case DRAZ:
		if (baseaddr != DRAZ_BASE)
			return -1;

		bitmap = data + DRAZ_BITMAP;
		mcolour0 = data + DRAZ_MCOLOUR;
		colour = data + DRAZ_COLOUR;
		background = data + DRAZ_BG;
		break;

	case EAFLI:
		if (baseaddr != EAFLI_BASE)
			return -1;

		bitmap = data + EAFLI_BITMAP;
		mcolour0 = data + EAFLI_MCOLOUR0;
		mcolour1 = data + EAFLI_MCOLOUR1;
		mcolour2 = data + EAFLI_MCOLOUR2;
		mcolour3 = data + EAFLI_MCOLOUR3;
		mcolour4 = data + EAFLI_MCOLOUR4;
		mcolour5 = data + EAFLI_MCOLOUR5;
		mcolour6 = data + EAFLI_MCOLOUR6;
		mcolour7 = data + EAFLI_MCOLOUR7;
		sprmap0 = data + EAFLI_SPRMAP0;
		sprmap1 = data + EAFLI_SPRMAP1;
		sprmap2 = data + EAFLI_SPRMAP2;
		sprmap3 = data + EAFLI_SPRMAP3;
		scolour0 = data + EAFLI_SPRCOL0;
		scolour1 = data + EAFLI_SPRCOL1;
		scolour2 = data + EAFLI_SPRCOL2;
		scolour3 = data + EAFLI_SPRCOL3;
		colmode = FLI8;
		smode = data[EAFLI_MFLAG] & 1 ? LORES : HIRES;
		mode = HIRES;
		height = 84;
		break;

	case FLI:
		if (baseaddr != FLI_BASE &&
		    baseaddr != FLIV2_BASE)
			return -1;

		if (baseaddr == FLI_BASE) {
			bitmap = data + FLI_BITMAP;
			mcolour0 = data + FLI_MCOLOUR0;
			mcolour1 = data + FLI_MCOLOUR1;
			mcolour2 = data + FLI_MCOLOUR2;
			mcolour3 = data + FLI_MCOLOUR3;
			mcolour4 = data + FLI_MCOLOUR4;
			mcolour5 = data + FLI_MCOLOUR5;
			mcolour6 = data + FLI_MCOLOUR6;
			mcolour7 = data + FLI_MCOLOUR7;
			colour = data + FLI_COLOUR;
			background = NULL;
		} else {
			bitmap = data + FLIV2_BITMAP;
			mcolour0 = data + FLIV2_MCOLOUR0;
			mcolour1 = data + FLIV2_MCOLOUR1;
			mcolour2 = data + FLIV2_MCOLOUR2;
			mcolour3 = data + FLIV2_MCOLOUR3;
			mcolour4 = data + FLIV2_MCOLOUR4;
			mcolour5 = data + FLIV2_MCOLOUR5;
			mcolour6 = data + FLIV2_MCOLOUR6;
			mcolour7 = data + FLIV2_MCOLOUR7;
			colour = data + FLIV2_COLOUR;
			background = data + FLIV2_BG;
		}
		colmode = FLI8;
		break;

	case IMSYS:
		if (baseaddr != IMSYS_BASE)
			return -1;

		bitmap = data + IMSYS_BITMAP;
		mcolour0 = data + IMSYS_MCOLOUR;
		colour = data + IMSYS_COLOUR;
		background = NULL;
		break;

	case IP64H:
		if (baseaddr != IP64H_BASE)
			return -1;

		bitmap = data + IP64H_BITMAP;
		mcolour0 = data + IP64H_MCOLOUR;
		mode = HIRES;
		break;

	case IP64L:
		if (baseaddr != IP64L_BASE)
			return -1;

		bitmap = data + IP64L_BITMAP;
		mcolour0 = data + IP64L_MCOLOUR;
		colour = data + IP64L_COLOUR;
		background = data + IP64L_BG;
		break;

	case KOALA:
		if (baseaddr != KOALA_BASE)
			return -1;

		bitmap = data + KOALA_BITMAP;
		mcolour0 = data + KOALA_MCOLOUR;
		colour = data + KOALA_COLOUR;
		background = data + KOALA_BG;
		break;

	case OCP:
		if (baseaddr != OCP_BASE)
			return -1;

		bitmap = data + OCP_BITMAP;
		mcolour0 = data + OCP_MCOLOUR;
		mode = HIRES;
		break;

	case SHFLI:
		if (baseaddr != SHFLI_BASE)
			return -1;

		bitmap = data + SHFLI_BITMAP;
		mcolour0 = data + SHFLI_MCOLOUR0;
		mcolour1 = data + SHFLI_MCOLOUR1;
		mcolour2 = data + SHFLI_MCOLOUR2;
		mcolour3 = data + SHFLI_MCOLOUR3;
		mcolour4 = data + SHFLI_MCOLOUR4;
		mcolour5 = data + SHFLI_MCOLOUR5;
		mcolour6 = data + SHFLI_MCOLOUR6;
		mcolour7 = data + SHFLI_MCOLOUR7;
		scolour0 = data + SHFLI_SCOLOUR0;
		scolour1 = data + SHFLI_SCOLOUR1;
		bank = data + SHFLI_BANK;
		colmode = FLI8;
		mode = HIRES;
		xoffset = 112;
		width = 96;
		height = 168;
		break;

	case SHFXL:
		if (baseaddr != SHFXL_BASE)
			return -1;

		bitmap = data + SHFXL_BITMAP;
		mcolour0 = data + SHFXL_MCOLOUR0;
		mcolour1 = data + SHFXL_MCOLOUR1;
		mcolour2 = data + SHFXL_MCOLOUR2;
		mcolour3 = data + SHFXL_MCOLOUR3;
		mcolour4 = data + SHFXL_MCOLOUR4;
		mcolour5 = data + SHFXL_MCOLOUR5;
		mcolour6 = data + SHFXL_MCOLOUR6;
		mcolour7 = data + SHFXL_MCOLOUR7;
		scolour0 = data + SHFXL_SCOLOUR0;
		bank = data + SHFXL_BANK;
		colmode = FLI8;
		mode = HIRES;
		xoffset = 88;
		width = 144;
		height = 168;
		break;

	case SUXXV2:
		bitmap = data + SUXXV2_BITMAP;
		mcolour0 = data + SUXXV2_MCOLOUR;
		colour = data + SUXXV2_COLOUR;
		background = NULL;
		break;

	case SUXXV3:
		bitmap = data + SUXXV3_BITMAP;
		mcolour0 = data + SUXXV3_MCOLOUR;
		colour = data + SUXXV3_COLOUR;
		background = data + SUXXV3_BG;
		break;

	case TLIGHT:
		if (baseaddr != TLIGHT_BASE)
			return -1;

		bitmap = data + TLIGHT_BITMAP;
		mcolour0 = data + TLIGHT_MCOLOUR;
		colour = data + TLIGHT_COLOUR;
		background = data + TLIGHT_BG;
		break;

	case VIDCOM:
		if (baseaddr != VIDCOM_BASE)
			return -1;

		bitmap = data + VIDCOM_BITMAP;
		mcolour0 = data + VIDCOM_MCOLOUR;
		colour = data + VIDCOM_COLOUR;
		background = NULL;
		break;

	default:
		return -1;
	}

	if (type == SUXXV2 || type == SUXXV3) {
		int i;

		for (i = 0; i < 500; i++) {
			tmpcol[i] = colour[i] & 0xf;
			tmpcol[i + 500] = colour[i] >> 4 & 0xf;
		}
		colour = tmpcol;
	}

	image_ID = gimp_image_new(width, height, GIMP_INDEXED);
	gimp_image_set_cmap(image_ID, vicpalette, 16);

	layer_ID = gimp_layer_new(image_ID, "Background", width, height,
				  GIMP_INDEXED_IMAGE, 100, GIMP_NORMAL_MODE);
	gimp_image_add_layer(image_ID, layer_ID, 0);

	drawable = gimp_drawable_get(layer_ID);

	gimp_pixel_rgn_init(&pixel_rgn, drawable, 0, 0, width, height,
			    TRUE, FALSE);

	buf = g_new0(guchar, width * height);

	bufp = buf + 8;

	for (row = 0; row < height; row++) {
		static guint8 *cp;
		static gint off;

		if (type == FLI)
			off = 24 - (row / 8) + 25 * (row % 8);
		else
			off = 0;

		if (colmode == FLI8) {
			switch (row % 8) {
			default:
			case 0:
				cp = mcolour0;
				break;

			case 1:
				cp = mcolour1;
				break;

			case 2:
				cp = mcolour2;
				break;

			case 3:
				cp = mcolour3;
				break;

			case 4:
				cp = mcolour4;
				break;

			case 5:
				cp = mcolour5;
				break;

			case 6:
				cp = mcolour6;
				break;

			case 7:
				cp = mcolour7;
				break;
			}
		} else {
			cp = mcolour0;
		}

		for (col = xoffset / 8; col < (xoffset + width) / 8; col++) {
			static guint8 col1, col2, col3 = 0;
			static gint i;
			guint8 *src = bitmap + (row / 8) * 320 +
				      row % 8 + col * 8;

			if (col < 10 && type == EAFLI) {
				col1 = *(mcolour0 + col) >> 4 & 0xf;
				col2 = *(mcolour0 + col) & 0xf;
			} else {
				col1 = *(cp + (row / 8) * 40 + col) >> 4 & 0xf;
				col2 = *(cp + (row / 8) * 40 + col) & 0xf;
			}

			if (mode != HIRES)
				col3 = colour[(row >> 3) * 40 + col] & 0xf;

			for (i = 0; i < ((mode != HIRES) ? 4 : 8); i++) {
				static guint8 tmp;

				if (mode == HIRES) {
					switch (*src >> i & 1) {
					case 0:
						tmp = col2;
						break;

					case 1:
						tmp = col1;
						break;

					default:
						break;
					}

					*--bufp = tmp;
				} else {
					switch (*src >> i * 2 & 3) {
					case 0:
						tmp = (!background) ? 0 :
							*(background + off) &
							0xf;
						break;

					case 1:
						tmp = col1;
						break;

					case 2:
						tmp = col2;
						break;

					case 3:
						tmp = col3;
						break;

					default:
						break;
					}

					*--bufp = tmp;
					*--bufp = tmp;
				}
			}
			bufp += 16;
		}
	}

	if (type == SHFLI) {
		for (row = 0; row < height; row++) {
			guint8 *sb;

			switch (row % 8) {
			default:
			case 0:
				sb = mcolour0;
				break;

			case 1:
				sb = mcolour1;
				break;

			case 2:
				sb = mcolour2;
				break;

			case 3:
				sb = mcolour3;
				break;

			case 4:
				sb = mcolour4;
				break;

			case 5:
				sb = mcolour5;
				break;

			case 6:
				sb = mcolour6;
				break;

			case 7:
				sb = mcolour7;
				break;
			}

			sb += 0x3f8;

			for (col = 0; col < width / 8; col++) {
				static guint8 i;
				guint8 *dst = buf + row * width + col * 8 + width;

				for (i = 0; i < 8; i++) {
					guint8 tmp = *(dst + i);

					tmp = (*(bank + sb[4 + col / 3] *
						 64 + col % 3 +
						(row % 21) * 3) &
						 1 << (7 - i)) ? *scolour1 : tmp;
					tmp = (*(bank + sb[col / 3] *
						 64 + col % 3 +
						 (row % 21) * 3) &
						 1 << (7 - i)) ? *scolour0 : tmp;

					*(dst + i) = tmp;
				}
			}
		}
	} else if (type == SHFXL) {
		for (row = 0; row < height; row++) {
			guint8 *sb;

			switch ((row + 7) % 8) {
			default:
			case 0:
				sb = mcolour0;
				break;

			case 1:
				sb = mcolour1;
				break;

			case 2:
				sb = mcolour2;
				break;

			case 3:
				sb = mcolour3;
				break;

			case 4:
				sb = mcolour4;
				break;

			case 5:
				sb = mcolour5;
				break;

			case 6:
				sb = mcolour6;
				break;

			case 7:
				sb = mcolour7;
				break;
			}

			sb += 0x3f8;

			for (col = 0; col < width / 8; col++) {
				static guint8 i;
				guint8 *dst = buf + row * width + col * 8;

				for (i = 0; i < 8; i++) {
					guint8 tmp = *(dst + i);

					tmp = (*(bank + sb[1 + col / 3] *
						 64 + col % 3 +
						 (row % 21) * 3) &
						 1 << (7 - i)) ? *scolour0 : tmp;

					*(dst + i) = tmp;
				}
			}
		}
	} else if (type == EAFLI) {
		for (row = 0; row < height; row++) {
			guint8 *src = NULL;

			switch (row / 42) {
			case 0:
				src = (row % 2) ? sprmap0 : sprmap1;
				break;

			case 1:
				src = (row % 2) ? sprmap2 : sprmap3;
				break;

			default:
				break;
			}

			src += (row % 42) / 2 * 3;

			for (col = 0; col < 24; col++) {
				guint8 *dst = buf + row * 320 + 80 + col * 8;
				guint8 i;

				for (i = 0; i < 8; i += 2) {
					guint8 tmp;

					tmp = *(src + col % 3 + (col / 3) * 64);
					tmp = tmp >> (6 - i) & 3;

					if (col > 2) {
						if (tmp & 0x2)
							*(dst + i) = *scolour0;
						if (tmp & 0x1)
							*(dst + i + 1) = *scolour0;
					} else if (smode == LORES) {
						switch (tmp) {
						case 0x1:
							*(dst + i) = *scolour1;
							*(dst + i + 1) = *scolour1;
							break;

						case 0x2:
							*(dst + i) = *scolour2;
							*(dst + i + 1) = *scolour2;
							break;

						case 0x3:
							*(dst + i) = *scolour3;
							*(dst + i + 1) = *scolour3;
							break;

						case 0x0:
						default:
							*(dst + i) = 0xf;
							*(dst + i + 1) = 0xf;
							break;
						}
					} else {
						if (tmp & 0x1)
							*(dst + i) = *scolour1;
						else
							*(dst + i) = 0xf;
						if (tmp & 0x2)
							*(dst + i) = *scolour1;
						else
							*(dst + i + 1) = 0xf;
					}
				}
			}
		}
	}

	gimp_pixel_rgn_set_rect(&pixel_rgn, buf, 0, 0, width, height);
	g_free(buf);
	gimp_drawable_flush(drawable);

	return image_ID;
}

static gint8 get_bgcol(gint line, guint8 type)
{
	gint col[16][2];
	gint x, y;

	for (x = 0; x < 16; x++) {
		col[x][0] = x;
		col[x][1] = 0;
	}

	/* Create colour statistics:
	 * For FLI we only need to check the 24 first pixels,
	 * since those can only hold the background colour
	 * dark brown or light gray, nothing else
	 */
	for (y = (type == FLI ? line : 0); y <= (type == FLI ? line : 199); y++)
		for (x = 0; x < (type == FLI ? 24 : 320); x++)
			col[(guint8)buf[x + y * 320]][1]++;

	colsort(16);

	return col[0][0];
}

static gboolean get_fgcols(gint8 *col0, gint8 *col1, gint8 *col2,
			   guint8 type, guint x, guint y, guint8 line)
{
	gint8 tcol[8][4];
	guint8 i, j;
	guint16 cbits[8];
	guint8 tmp;

	if (line % 8 != 0 && type != FLI)
		return TRUE;

	for (i = 0; i < 8; i++) {
		tcol[i][0] = 0;
		tcol[i][1] = 0x10;
		tcol[i][2] = 0x10;
		tcol[i][3] = 0x10;
		cbits[i] = 0;
	}

	/* Create colour statistics
	 */
	for (i = 0; i < 8; i++) {
		guint8 l = 0;

		if (type == FLI || i == 0)
			l = i;

		for (j = 0; j < 8; j += 2) {
			tmp = buf[x * 8 + j + 320 * (y * 8 + i)];

			if (tmp != tmpbg[y * 8 + i])
				cbits[l] |= 1 << tmp;
		}
	}

	for (i = 0; i < 8; i++) {
		guint8 l = 0;

		if (type == FLI || i == 0)
			l = i;

		for (j = 0; j < 16; j++) {
			gint c = (cbits[l] & 1 << j) ? j : 0x10;

			if (c == 0x10 || c == tcol[l][1] ||
			    c == tcol[l][2] || c == tcol[l][3])
				continue;

			if (++tcol[l][0] > 3)
				return FALSE;

			tcol[l][tcol[l][0]] = c;
		}
	}

	for (i = 0; i < 16; i++) {
		for (j = 0; j < 8; j++) {
			guint8 l = 0;

			if (type == FLI || j == 0)
				l = j;

			if (tcol[l][0] < 3)
				continue;

			if (i != tcol[l][1] && i != tcol[l][2] &&
			    i != tcol[l][3])
				break;
		}

		if (j == 8)
			break;
	}

	if (j < 8)
		return FALSE;

	if (line % 8 == 0)
		*col2 = i & 0xf;

	if (type != FLI)
		line = 0;

	*col0 = ((tcol[line][1] != *col2) ? tcol[line][1] : tcol[line][2]);
	*col1 = ((tcol[line][2] != *col0 &&
		 (tcol[line][2] != *col2)) ? tcol[line][2] : tcol[line][3]);

	*col0 &= 0xf;
	*col1 &= 0xf;

	return TRUE;
}

/* FIXME */
static gint gimp_to_rain(gint32 image_ID)
{
	guint size = 0;
	(void)image_ID;

	return size;
}

/**
 * Set a pixel in a 64 bitmap picture
 *
 * @param bitmap The bitmap picture to set the pixel in
 * @param x The x-coordinate of the pixel
 * @param y The y-coordinate of the pixel
 */
static void set_bitmap_pixel(guint8 **bitmap, guint x, guint y)
{
	*(*bitmap +
	  (y / 8) * 320 +
	  (y % 8) +
	  (x / 8) * 8) |= (1 << (7 - (x % 8)));
}

/**
 * Set a pixel in a 64 spritemap
 *
 * @param bitmap The bitmap picture to set the pixel in
 * @param x The x-coordinate of the pixel
 * @param y The y-coordinate of the pixel
 * @param width The width of the sprite map (in pixels)
 */
static void set_sprmap_pixel(guint8 **sprmap, guint x, guint y, guint width)
{
	*(*sprmap +
	  (y / 21) * width / 24 * 64 +
	  (y % 21) * 3 +
	  (x / 24) * 64 +
	  (x % 24) / 8) |= (1 << (7 - (x % 8)));
}

#if 0
/**
 * Calculate colour statistics
 * Note: for multicolour images, this is going to be off by a factor of 2,
 * but since that holds equally for all colours, this does not matter
 *
 * @param cmap The image map to scan
 * @param colstats An pointer to the an array to store colour stats in
 */
static void get_colour_statistics(guchar *cmap, guint8 **colstats)
{
	int i;

	/* Empty colstats */
	for (i = 0; i < 16; i++)
		**colstats = 0;

	/* Collect the colour statistics */
	for (i = 0; i < (320 * 200); i++)
		*(*colstats + cmap[i]) += 1;
}
#endif

static gint gimp_to_bitmap(gint32 image_ID, guint8 type)
{
	GimpDrawable *drawable = NULL;
	GimpPixelRgn pixel_rgn;
	guint8 mode = LORES;
	gint32 drawable_ID;
	guint8 *mcolp = NULL;
	guint8 *colp = NULL;
	guint8 *bitp;
	guint8 *sprmap1;
	guint8 *sprmap2;
	guchar *cmap;
	guint8 *bgp = NULL;
	guint height;
	guint width;
	gint ncols;
	guint size;
	guint x, y;

	width = gimp_image_width(image_ID);
	height = gimp_image_height(image_ID);

	if (type != SPRITE) {
		if (width != 320 || height != 200) {
			gimp_message("Image-size must be 320x200. Aborting.");
			return -1;
		}
	} else {
		if (width % 24 || height % 21) {
			gimp_message("Image must be evenly divisible "
				     "into 24x21 blocks. Aborting.");
			return -1;
		}
	}

	drawable_ID = gimp_image_get_active_drawable(image_ID);
	drawable = gimp_drawable_get(drawable_ID);

	if (gimp_drawable_type(drawable_ID) != GIMP_INDEXED_IMAGE) {
		gimp_message("Image must be indexed. Aborting.");
		return -1;
	}

	cmap = gimp_image_get_cmap(image_ID, &ncols);

	if (type == HWS) {
		if (ncols > 4) {
			gimp_message("Image has more than 4 colours. "
				     "Aborting.");
			return -1;
		}
	} else if (ncols > 16) {
		gimp_message("Image has more than 16 colours. Aborting.");
		return -1;
	}

	gimp_pixel_rgn_init(&pixel_rgn, drawable, 0, 0, width, height,
			    FALSE, FALSE);

	buf = g_new0(guint8, width * height);

	gimp_pixel_rgn_get_rect(&pixel_rgn, buf, 0, 0, width, height);

	/* clear any remnants of old image-data */
	for (x = 0; x < sizeof (data); x++)
		data[x] = 0;
	for (x = 0; x < sizeof (tmpbg); x++)
		tmpbg[x] = 0;
	for (x = 0; x < sizeof (tmpcol); x++)
		tmpcol[x] = 0;

	/* setup initial pointers and start address */
	switch (type) {
	case ADVOCP:
		data[0] = ADVOCP_BASE & 0xff;
		data[1] = ADVOCP_BASE >> 8 & 0xff;
		bitp = data + ADVOCP_BITMAP;
		mcolp = data + ADVOCP_MCOLOUR;
		colp = data + ADVOCP_COLOUR;
		bgp = data + ADVOCP_BG;
		size = ADVOCP_SIZE;
		break;

	case AFLI:
		data[0] = AFLI_BASE & 0xff;
		data[1] = AFLI_BASE >> 8 & 0xff;
		bitp = data + AFLI_BITMAP;
		mcolp = data + AFLI_MCOLOUR0;
		size = AFLI_SIZE;
		mode = HIRES;
		break;

	case ARTIST:
		data[0] = ARTIST_BASE & 0xff;
		data[1] = ARTIST_BASE >> 8 & 0xff;
		bitp = data + ARTIST_BITMAP;
		mcolp = data + ARTIST_MCOLOUR;
		colp = data + ARTIST_COLOUR;
		bgp = data + ARTIST_BG;
		size = ARTIST_SIZE;
		break;

	case BLAZE:
		data[0] = BLAZE_BASE & 0xff;
		data[1] = BLAZE_BASE >> 8 & 0xff;
		bitp = data + BLAZE_BITMAP;
		mcolp = data + BLAZE_MCOLOUR;
		colp = data + BLAZE_COLOUR;
		bgp = NULL;
		size = BLAZE_SIZE;
		break;

	case DRAZ:
		data[0] = DRAZ_BASE & 0xff;
		data[1] = DRAZ_BASE >> 8 & 0xff;
		bitp = data + DRAZ_BITMAP;
		mcolp = data + DRAZ_MCOLOUR;
		colp = data + DRAZ_COLOUR;
		bgp = data + DRAZ_BG;
		size = DRAZ_SIZE;
		break;

	case FLI:
		/* Only FLIv2 is supported for saving */
		data[0] = FLIV2_BASE & 0xff;
		data[1] = FLIV2_BASE >> 8 & 0xff;
		bitp = data + FLIV2_BITMAP;
		mcolp = data + FLIV2_MCOLOUR0;
		colp = data + FLIV2_COLOUR;
		bgp = data + FLIV2_BG;
		size = FLIV2_SIZE;
		break;

	case IMSYS:
		data[0] = IMSYS_BASE & 0xff;
		data[1] = IMSYS_BASE >> 8 & 0xff;
		bitp = data + IMSYS_BITMAP;
		mcolp = data + IMSYS_MCOLOUR;
		colp = data + IMSYS_COLOUR;
		bgp = NULL;
		size = IMSYS_SIZE;
		break;

	case IP64H:
		data[0] = IP64H_BASE & 0xff;
		data[1] = IP64H_BASE >> 8 & 0xff;
		bitp = data + IP64H_BITMAP;
		mcolp = data + IP64H_MCOLOUR;
		size = IP64H_SIZE;
		mode = HIRES;
		break;

	case IP64L:
		data[0] = IP64L_BASE & 0xff;
		data[1] = IP64L_BASE >> 8 & 0xff;
		bitp = data + IP64L_BITMAP;
		mcolp = data + IP64L_MCOLOUR;
		colp = data + IP64L_COLOUR;
		bgp = data + IP64L_BG;
		size = IP64L_SIZE;
		break;

	case KOALA:
		data[0] = KOALA_BASE & 0xff;
		data[1] = KOALA_BASE >> 8 & 0xff;
		bitp = data + KOALA_BITMAP;
		mcolp = data + KOALA_MCOLOUR;
		colp = data + KOALA_COLOUR;
		bgp = data + KOALA_BG;
		size = KOALA_SIZE;
		break;

	case OCP:
		data[0] = OCP_BASE & 0xff;
		data[1] = OCP_BASE >> 8 & 0xff;
		bitp = data + OCP_BITMAP;
		mcolp = data + OCP_MCOLOUR;
		size = OCP_SIZE;
		mode = HIRES;
		break;

	case SPRITE:
		data[0] = SPRITE_BASE & 0xff;
		data[1] = SPRITE_BASE >> 8 & 0xff;
		bitp = data + SPRITE_BITMAP;
		size = width / 24 * height / 21 * 64 + 2;
		break;

	case SUXX:
		/* Only SUXXv3 is supported for saving */
		data[0] = SUXXV3_BASE & 0xff;
		data[1] = SUXXV3_BASE >> 8 & 0xff;
		bitp = data + SUXXV3_BITMAP;
		mcolp = data + SUXXV3_MCOLOUR;
		colp = tmpcol;
		bgp = data + SUXXV3_BG;
		size = SUXXV3_SIZE;
		break;

	case TLIGHT:
		data[0] = TLIGHT_BASE & 0xff;
		data[1] = TLIGHT_BASE >> 8 & 0xff;
		bitp = data + TLIGHT_BITMAP;
		mcolp = data + TLIGHT_MCOLOUR;
		colp = data + TLIGHT_COLOUR;
		bgp = data + TLIGHT_BG;
		size = TLIGHT_SIZE;
		break;

	case VIDCOM:
		data[0] = VIDCOM_BASE & 0xff;
		data[1] = VIDCOM_BASE >> 8 & 0xff;
		bitp = data + VIDCOM_BITMAP;
		mcolp = data + VIDCOM_MCOLOUR;
		colp = data + VIDCOM_COLOUR;
		bgp = NULL;
		size = VIDCOM_SIZE;
		break;

	case HWS:
		data[0] = HWS_BASE & 0xff;
		data[1] = HWS_BASE >> 8 & 0xff;
		bitp = data + HWS_BITMAP;
		sprmap1 = data + HWS_SPRMAP1;
		sprmap2 = data + HWS_SPRMAP2;
		size = HWS_SIZE;
		mode = HIRES;
		break;

	default:
		gimp_message("Target format not supported. Aborting.");
		return -1;
	}

	if (mode == LORES || type == SPRITE) {
		guint i;

		for (i = 0; i < width * height; i += 2) {
			if (buf[i] != buf[i + 1]) {
				mode = HIRES;
				break;
			}
		}

		if (mode == HIRES && type != SPRITE) {
			gimp_message("Too high resolution: Aborting.");
			printf("x: %d, y: %d\n", i % 320, i / 320);
			return -1;
		}
	}

	if (type == SPRITE) {
		gint col[4][2];
		static guint8 byte[3];
		static gint8 tmp;
		static guint i, j;

		col[0][0] = 0xff;
		col[1][0] = 0xff;
		col[2][0] = 0xff;
		col[3][0] = 0xff;
		col[0][1] = 0;
		col[1][1] = 0;
		col[2][1] = 0;
		col[3][1] = 0;

		for (i = 0; i < width * height; i++) {
			tmp = buf[i];

			if (!col[0][1] || col[0][0] == tmp) {
				col[0][0] = tmp;
				col[0][1]++;
			} else if (!col[1][1] || col[1][0] == tmp) {
				col[1][0] = tmp;
				col[1][1]++;
			} else if (!col[2][1] || col[2][0] == tmp) {
				col[2][0] = tmp;
				col[2][1]++;
			} else if (!col[3][1] || col[3][0] == tmp) {
				col[3][0] = tmp;
				col[3][1]++;
			} else {
				gimp_message("Too many colours used. Aborting.");
				return -1;
			}
		}

		colsort(4);

		if (col[2][1] > 0 && mode == HIRES) {
			gimp_message("Too many colours used. Aborting.");
			return -1;
		}

		for (y = 0; y < height; y += 21) {
			for (x = 0; x < width; x += 24) {
				for (i = 0; i < 21; i++) {
					byte[0] = 0;
					byte[1] = 0;
					byte[2] = 0;

					for (j = 0; j < 24; j++) {
						tmp = buf[x + j + width *
							  (y + i)];

						if (mode == HIRES)
							byte[j / 8] <<= 1;
						else
							byte[j / 8] <<= 2;

						if (col[0][0] == tmp) {
							; /* nothing */
						} else if (col[1][0] == tmp) {
							byte[j / 8] |= 0x1;
						} else if (col[2][0] == tmp) {
							byte[j / 8] |= 0x2;
						} else {
							byte[j / 8] |= 0x3;
						}

						if (mode != HIRES)
							j++;
					}
					bitp[0] = byte[0];
					bitp[1] = byte[1];
					bitp[2] = byte[2];
					bitp += 3;
				}
				bitp++;
			}
		}
	} else if (type == HWS) {
		gint col0 = -1, col1 = -1, col2 = -1, col3 = -1;

		/* Find out what colours to use for the bitmap */
		for (y = 0; y < 200; y++) {
			for (x = 96; x < 320; x++) {
				static gint tmp;

				tmp = buf[x + 320 * y] & 0xf;

				if (col0 == tmp || col0 == -1) {
					col0 = tmp;
				} else if (col1 == tmp || col1 == -1) {
					col1 = tmp;
				} else {
					gimp_message("Columns 96-319 "
						     "cannot "
						     "contain more than 2 "
						     "colours. Aborting.");
					return -1;
				}
			}
		}

		for (y = 0; y < 200; y++) {
			for (x = 0; x < 320; x++) {
				static gint tmp;

				tmp = buf[x + 320 * y] & 0xf;

				if (col0 == tmp) {
				} else if (col1 == tmp || col1 == -1) {
					col1 = tmp;
					/* set to bitmap */
					set_bitmap_pixel(&bitp, x, y);
				} else if (col2 == tmp || col2 == -1) {
					if (y < 8 || y > 196) {
						gimp_message("Row 0-7 and "
							     "197-199 cannot "
							     "contain more "
							     "than 2 colours. "
							     "Aborting.");
						return -1;
					}

					col2 = tmp;
					set_sprmap_pixel(&sprmap1,
							 x, y - 8, 96);
				} else {
					if (y < 8 || y > 196) {
						gimp_message("Row 0-7 and "
							     "197-199 cannot "
							     "contain more "
							     "than 2 colours. "
							     "Aborting.");
						return -1;
					}

					col3 = tmp;
					set_sprmap_pixel(&sprmap2,
							 x, y - 8, 96);
				}
			}
		}
	} else if (mode == HIRES) {
		guint8 block[8];

		/* We process each block of 8x8 pixels separately,
		 * to make sure that the there only are two colours
		 * per unit
		 */
		for (y = 0; y < 25; y++) {
			static gint col0, col1;

			for (x = 0; x < 40; x++) {
				gint i, j;

				for (i = 0; i < 8; i++) {
					static gint off = 0;

					if (type == AFLI || i == 0) {
						col0 = -1;
						col1 = -1;

						switch (i) {
						case 0:
							off = 0;
							break;
						case 1:
							off = AFLI_MCOLOUR1 -
							      AFLI_MCOLOUR0;
							break;
						case 2:
							off = AFLI_MCOLOUR2 -
							      AFLI_MCOLOUR0;
							break;
						case 3:
							off = AFLI_MCOLOUR3 -
							      AFLI_MCOLOUR0;
							break;
						case 4:
							off = AFLI_MCOLOUR4 -
							      AFLI_MCOLOUR0;
							break;
						case 5:
							off = AFLI_MCOLOUR5 -
							      AFLI_MCOLOUR0;
							break;
						case 6:
							off = AFLI_MCOLOUR6 -
							      AFLI_MCOLOUR0;
							break;
						case 7:
							off = AFLI_MCOLOUR7 -
							      AFLI_MCOLOUR0;
							break;
						default:
							break;
						}
					}

					for (j = 0; j < 8; j++) {
						static gint tmp;

						tmp = buf[x * 8 + j + 320 *
							  (y * 8 + i)];
						tmp &= 0xf;

						block[i] <<= 1;

						if (col0 == tmp ||
						    col0 == -1) {
							col0 = tmp;
						} else if (col1 == tmp ||
							   col1 == -1) {
							col1 = tmp;
							block[i] |= 0x1;
						} else {
							gimp_message("Too many colours in a 8x8 block. Aborting.");
							return -1;
						}
					}

					if (col1 == -1)
						mcolp[off] = col0;
					else
						mcolp[off] = col1 << 4 | col0;
				}

				mcolp++;

				for (i = 0; i < 8; i++)
					*bitp++ = block[i];
			}
		}
	} else if (mode == LORES) {
		guint8 block[8];
		gint8 col0 = -1, col1 = -1, col2 = -1, bgcol = -1;
		gboolean is4col = TRUE;
		guint i;

		/* If the picture contains a maximum of 4 colours,
		 * always use the same bit pattern for the same colour
		 */
		for (i = 0; i < (320 * 200); i += 2) {
			guint8 tmp;

			tmp = buf[i];

			if (col0 == tmp || col0 == -1) {
				col0 = tmp;
			} else if (col1 == tmp || col1 == -1) {
				col1 = tmp;
			} else if (col2 == tmp || col2 == -1) {
				col2 = tmp;
			} else if (bgcol == tmp || bgcol == -1) {
				bgcol = tmp;
			} else {
				is4col = FALSE;
				break;
			}
		}

		if (bgp) {
			guint8 bg = 0;
			guint i;

			for (i = 0; i < 200; i++) {
				if (is4col == TRUE) {
					bg = bgcol;
				} else if (type == FLI || i == 0)
					bg = get_bgcol(i, type);

				if (type == FLI)
					*(bgp + 24 - (i / 8) +
					  25 * (i % 8)) = bg;
				else
					*bgp = bg;

				*(tmpbg + i) = bg;
			}
		}

		/* We process each block of 4x8 pixels separately,
		 * to make sure that the there only are three colours
		 * plus background per unit
		 */
		for (y = 0; y < 25; y++) {
			for (x = 0; x < 40; x++) {
				static gint j;

				for (i = 0; i < 8; i++) {
					static gint off = 0;

					/* If is4col is TRUE we already
					 * know what colours to use
					 */
					if (is4col == TRUE) {
					} else if (!get_fgcols(&col0, &col1,
							       &col2, type,
							       x, y, i)) {
						gimp_message("Too many colours in a 4x8 block. Aborting.");
						printf("x: %d, y: %d\n", x, y);
						return -1;
					}

					if (type == FLI || i == 0) {
						switch (i) {
						case 0:
							off = 0;
							break;
						case 1:
							off = FLIV2_MCOLOUR1 -
							      FLIV2_MCOLOUR0;
							break;
						case 2:
							off = FLIV2_MCOLOUR2 -
							      FLIV2_MCOLOUR0;
							break;
						case 3:
							off = FLIV2_MCOLOUR3 -
							      FLIV2_MCOLOUR0;
							break;
						case 4:
							off = FLIV2_MCOLOUR4 -
							      FLIV2_MCOLOUR0;
							break;
						case 5:
							off = FLIV2_MCOLOUR5 -
							      FLIV2_MCOLOUR0;
							break;
						case 6:
							off = FLIV2_MCOLOUR6 -
							      FLIV2_MCOLOUR0;
							break;
						case 7:
							off = FLIV2_MCOLOUR7 -
							      FLIV2_MCOLOUR0;
							break;
						default:
							break;
						}
					}

					for (j = 0; j < 8; j += 2) {
						static gint tmp;

						tmp = buf[x * 8 + j + 320 *
							  (y * 8 + i)];

						block[i] <<= 2;

						if (tmpbg[y * 8 + i] == tmp)
							; /* nothing */
						else if (col0 == tmp)
							block[i] |= 0x2;
						else if (col1 == tmp)
							block[i] |= 0x1;
						else
							block[i] |= 0x3;
					}

					mcolp[off] = col1 << 4 | col0;
				}

				mcolp++;

				*colp++ = col2;

				for (i = 0; i < 8; i++)
					*bitp++ = block[i];
			}
		}

		if (type == SUXX) {
			guint tmp = sizeof (tmpcol) / 2;

			for (x = 0; x < tmp; x++) {
				data[SUXXV3_COLOUR + x] = tmpcol[x];
				data[SUXXV3_COLOUR + x] |= tmpcol[x + tmp] << 4;
			}
		}
	} else {
		gimp_message("Target format not supported. Aborting.");
		return -1;
	}

	g_free(buf);

	return size;
}

gint32 load_image(char *filename, guint8 type)
{
	gint32 image_ID;
	FILE *in;

	if ((in = fopen(filename, "r"))) {
		fread(&data, sizeof (data), 1, in);
		fclose(in);

		if (type == RAIN)
			image_ID = rain_to_gimp();
		else
			image_ID = bitmap_to_gimp(type);

		gimp_image_set_filename(image_ID, filename);
	} else {
		gimp_message("Unable to open file for reading. Aborting.");
		return -1;
	}

	return image_ID;
}

gint save_image(char *filename, gint32 image_ID, guint8 type)
{
	FILE *out;
	gint size;

	if (type == RAIN)
		size = gimp_to_rain(image_ID);
	else
		size = gimp_to_bitmap(image_ID, type);

	if (size < 0)
		return -1;

	if ((out = fopen(filename, "wb"))) {
		fwrite(&data, size, 1, out);
		fclose(out);
	} else {
		gimp_message("Unable to open file for writing. Aborting.");
		return -1;
	}

	return 0;
}
