/*!
 * \file
 * <!--
 * Copyright 2003, 2004, 2005, 2006 Develer S.r.l. (http://www.develer.com/)
 * Copyright 1999 Bernardo Innocenti <bernie@develer.com>
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \version $Id$
 *
 * \author Bernardo Innocenti <bernie@develer.com>
 * \author Stefano Fedrigo <aleph@develer.com>
 *
 * \brief Bitmap manipulation routines.
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2006/01/24 02:17:49  bernie
 *#* Split out gfx.c into bitmap.c and line.c.
 *#*
 *#*/

#include "gfx.h"
#include "gfx_p.h"

#include <cfg/debug.h>  /* ASSERT() */
#include <cfg/cpu.h>    /* CPU_HARVARD */
#include <cfg/macros.h> /* MIN() */
#include <appconfig.h>  /* CONFIG_GFX_CLIPPING */

#include <string.h>     /* memset() */

/*!
 * Initialize a Bitmap structure with the provided parameters.
 *
 * \note The pen position is reset to the origin.
 */
void gfx_bitmapInit(Bitmap *bm, uint8_t *raster, coord_t w, coord_t h)
{
	bm->raster = raster;
	bm->width = w;
	bm->height = h;
	#if (CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_H_MSB)
		bm->stride = (w + 7) / 8;
	#elif CONFIG_BITMAP_FMT == BITMAP_FMT_PLANAR_V_LSB
		bm->stride = w;
	#else
		#error Unknown value of CONFIG_BITMAP_FMT
	#endif /* CONFIG_BITMAP_FMT */
	bm->penX = 0;
	bm->penY = 0;

#if CONFIG_GFX_CLIPPING
	bm->cr.xmin = 0;
	bm->cr.ymin = 0;
	bm->cr.xmax = w;
	bm->cr.ymax = h;
#endif /* CONFIG_GFX_CLIPPING */
}


/*!
 * Clear the whole bitmap surface to the background color.
 *
 * \note This function does \b not update the current pen position.
 */
void gfx_bitmapClear(Bitmap *bm)
{
	memset(bm->raster, 0, RASTER_SIZE(bm->width, bm->height));
}


#if CPU_HARVARD

#include <avr/pgmspace.h> /* FIXME: memcpy_P() */

/*!
 * Copy a raster picture located in program memory in the bitmap.
 * The size of the raster to copy *must* be the same of the raster bitmap.
 *
 * \note This function does \b not update the current pen position
 */
void gfx_blit_P(Bitmap *bm, const pgm_uint8_t *raster)
{
	memcpy_P(bm->raster, raster, (bm->height / 8) * bm->width);
}
#endif /* CPU_HARVARD */

/**
 * Copy a rectangular area of a bitmap on another bitmap.
 *
 * Blitting is a common copy operation involving two bitmaps.
 * A rectangular area of the source bitmap is copied bit-wise
 * to a different position in the destination bitmap.
 *
 * \note Using the same bitmap for \a src and \a dst is unsupported.
 *
 * \param dst Bitmap where the operation writes
 *
 */
void gfx_blit(Bitmap *dst, Rect *rect, Bitmap *src, coord_t srcx, coord_t srcy)
{
	coord_t dxmin, dymin, dxmax, dymax;
	coord_t dx, dy, sx, sy;

	/*
	 * Clip coordinates inside dst->cr and src->width/height.
	 */
	dxmin = rect->xmin;
	if (dxmin < dst->cr.xmin)
	{
		srcx += dst->cr.xmin - dxmin;
		dxmin = dst->cr.xmin;
	}
	dymin = rect->ymin;
	if (dymin < dst->cr.ymin)
	{
		srcy += dst->cr.ymin - dymin;
		dymin = dst->cr.ymin;
	}
	dxmax = MIN(MIN(rect->xmax, rect->xmin + src->width), dst->cr.xmax);
	dymax = MIN(MIN(rect->ymax, rect->ymin + src->height), dst->cr.ymax);

	/* TODO: make it not as dog slow as this */
	for (dx = dxmin, sx = srcx; dx < dxmax; ++dx, ++sx)
		for (dy = dymin, sy = srcy; dy < dymax; ++dy, ++sy)
			BM_DRAWPIXEL(dst, dx, dy, BM_READPIXEL(src, sx, sy));
}


/*!
 * Set the bitmap clipping rectangle to the specified coordinates.
 *
 * All drawing performed on the bitmap will be clipped inside this
 * rectangle.
 *
 * \note Following the convention used in all other operations, the
 *       top-left pixels of the rectangle are included, while the
 *       bottom-right pixels are considered outside the clipping region.
 */
void gfx_setClipRect(Bitmap *bm, coord_t minx, coord_t miny, coord_t maxx, coord_t maxy)
{
	ASSERT(minx < maxx);
	ASSERT(miny < maxy);
	ASSERT(miny >= 0);
	ASSERT(minx >= 0);
	ASSERT(maxx <= bm->width);
	ASSERT(maxy <= bm->height);

	bm->cr.xmin = minx;
	bm->cr.ymin = miny;
	bm->cr.xmax = maxx;
	bm->cr.ymax = maxy;

/*	kprintf("cr.xmin = %d, cr.ymin = %d, cr.xmax = %d, cr.ymax = %d\n",
		bm->cr.xMin, bm->cr.ymin, bm->cr.xmax, bm->cr.ymax);
*/
}
