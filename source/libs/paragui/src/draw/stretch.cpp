/*
    parastretch - image stretching functions
    Copyright (C) 2000,2001,2002  Alexander Pipelka
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
 
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
    Alexander Pipelka
    pipelka@teleweb.at
 
    Last Update:      $Author: mbickel $
    Update Date:      $Date: 2006-02-15 21:30:16 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/draw/stretch.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

#include "pgdraw.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>

#if ! defined( macintosh ) && ! defined( __MWERKS__ )
// malloc.h should not be included on Mac with CodeWarrior (pro 4)
// Masahiro Minami<elsur@aaa.letter.co.jp>
// 01/05/05
//#include <malloc.h>
#endif // not defined macintosh nor __MWERKS__

#define sign(x) ((x)>0 ? 1:-1)

/*void SDL_FillRectEx(SDL_Surface *dst, Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, Uint32 color) {
	SDL_Rect r;
	r.x = x0;
	r.y = y0;
	r.w = abs(x1 - x0) + 1;
	r.h = abs(y1 - y0) + 1;
 
	SDL_FillRect(dst, &r, color);
}*/


inline void LockSurface(SDL_Surface* s) {
	if ( SDL_MUSTLOCK(s) ) {
		if ( SDL_LockSurface(s) < 0 )
			return ;
	}
}

inline void UnlockSurface(SDL_Surface* s) {
	if ( SDL_MUSTLOCK(s) ) {
		SDL_UnlockSurface(s);
		return ;
	}
}

template < class ST, class DT >
inline void StretchTemplateLUT(int x1, int x2, int y1, int y2, int yr, int yw, ST src_pixels, DT dst_pixels, Uint32* lut) {
	int dx, dy, e, d, dx2;

	register Uint32 p;

	dx = (x2 - x1);
	dy = (y2 - y1);

	dy <<= 1;
	e = dy - dx;
	dx2 = dx << 1;

	for (d = 0; d < dx; d++) {
		p = lut[*src_pixels];
		*dst_pixels++ = p;

		while (e >= 0) {
			src_pixels ++;
			e -= dx2;
		}

		e += dy;
	}
}

template < class ST, class DT >
inline void StretchTemplate(int x1, int x2, int y1, int y2, int yr, int yw, ST src_pixels, DT dst_pixels) {
	int dx, dy, e, d, dx2;
	/*int cx0 = 0;
	int cx1 = 0;
	DT dst_cx0 = (dst_pixels += cx0);
	DT dst_cx1 = (dst_pixels += cx1);*/

	dx = (x2 - x1);
	dy = (y2 - y1);

	dy <<= 1;
	e = dy - dx;
	dx2 = dx << 1;

	for (d = 0; d < dx; d++) {
		// hmm, ...
		// i know this is a bit tricky (and maybe ugly)
		//if((dst_pixels >= dst_cx0) && (dst_pixels <= dst_cx1)) {
		*dst_pixels++ = *((DT)src_pixels);
		//}
		//else {
		//	dst_pixels++;
		//}

		while (e >= 0) {
			src_pixels ++;
			e -= dx2;
		}

		e += dy;
	}
}

template < class ST, class DT >
inline void RectStretchTemplate(SDL_Surface* src_surface, ST src, int xs1, int ys1, int xs2, int ys2, SDL_Surface* dst_surface, DT dst, int xd1, int yd1, int xd2, int yd2, Uint32* lutVOI) {
	int dx, dy, e, d, dx2;
	int sx, sy;
	dx = abs((int)(yd2 - yd1));
	dy = abs((int)(ys2 - ys1));
	sx = sign(yd2 - yd1);
	sy = sign(ys2 - ys1);
	e = (dy << 1)-dx;
	dx2 = dx << 1;
	dy <<= 1;

	Uint16 src_pitch = src_surface->pitch;
	Uint16 dst_pitch = dst_surface->pitch;

	int src_bpp = src_surface->format->BytesPerPixel;
	int dst_bpp = dst_surface->format->BytesPerPixel;

	SDL_Rect clip;
	SDL_GetClipRect(dst_surface, &clip);

	register long src_pixels = ((long)src + ys1 * src_pitch + xs1 * src_bpp);
	register long dst_pixels = ((long)dst + yd1 * dst_pitch + xd1 * dst_bpp);
	register Uint32* lut = lutVOI;

	if (lut == NULL) {					// Stretch without lookup table
		for (d = 0; (d <= dx) && (yd1 < dst_surface->h) && (ys1 < src_surface->h); d++) {

			if(yd1 < clip.y || yd1 > clip.y+clip.h-1) {
				continue;
			}

			StretchTemplate(xd1, xd2, xs1, xs2, ys1, yd1, (ST)src_pixels, (DT)dst_pixels);

			while (e >= 0) {
				src_pixels += src_pitch;
				ys1++;
				e -= dx2;
			}
			dst_pixels += dst_pitch;
			yd1++;
			e += dy;
		}
	} else {									// Stretch with lookup table
		for (d = 0; (d <= dx) && (yd1 < dst_surface->h) && (ys1 < src_surface->h); d++) {
			StretchTemplateLUT(xd1, xd2, xs1, xs2, ys1, yd1, (ST)src_pixels, (DT)dst_pixels, lut);

			while (e >= 0) {
				src_pixels += src_pitch;
				ys1++;
				e -= dx2;
			}
			dst_pixels += dst_pitch;
			yd1++;
			e += dy;
		}
	}

}

inline void StretchTemplate24to32(int x1, int x2, int y1, int y2, int yr, int yw, Uint8* src_pixels, Uint32* dst_pixels, Uint32* lut) {
	int dx, dy, e, d, dx2;

	register Uint8 pr;
	register Uint8 pg;
	register Uint8 pb;
	register Uint32 r;

	dx = (x2 - x1);
	dy = (y2 - y1);

	dy <<= 1;
	e = dy - dx;
	dx2 = dx << 1;

	for (d = 0; d < dx; d++) {
		pr = lut[*(src_pixels)];
		pg = lut[*(src_pixels+1)];
		pb = lut[*(src_pixels+2)];

		r = pr << 16 | pg << 8 | pb;
		*dst_pixels++ = r;

		while (e >= 0) {
			src_pixels+=3;
			e -= dx2;
		}

		e += dy;
	}
}

inline void RectStretch24to32(SDL_Surface* src_surface, Uint8* src, int xs1, int ys1, int xs2, int ys2, SDL_Surface* dst_surface, Uint32* dst, int xd1, int yd1, int xd2, int yd2, Uint32* lutVOI) {
	int dx, dy, e, d, dx2;
	int sx, sy;
	dx = abs((int)(yd2 - yd1));
	dy = abs((int)(ys2 - ys1));
	sx = sign(yd2 - yd1);
	sy = sign(ys2 - ys1);
	e = (dy << 1)-dx;
	dx2 = dx << 1;
	dy <<= 1;

	Uint16 src_pitch = src_surface->pitch;
	Uint16 dst_pitch = dst_surface->pitch;

	int src_bpp = src_surface->format->BytesPerPixel;
	int dst_bpp = dst_surface->format->BytesPerPixel;

	SDL_Rect clip;
	SDL_GetClipRect(dst_surface, &clip);

	register long src_pixels = ((long)src + ys1 * src_pitch + xs1 * src_bpp);
	register long dst_pixels = ((long)dst + yd1 * dst_pitch + xd1 * dst_bpp);
	register Uint32* lut = lutVOI;

	for (d = 0; (d <= dx) && (yd1 < dst_surface->h) && (ys1 < src_surface->h); d++) {
		StretchTemplate24to32(xd1, xd2, xs1, xs2, ys1, yd1, (Uint8*)src_pixels, (Uint32*)dst_pixels, lut);

		while (e >= 0) {
			src_pixels += src_pitch;
			ys1++;
			e -= dx2;
		}
		dst_pixels += dst_pitch;
		yd1++;
		e += dy;
	}

}

void PG_Draw::RectStretch(SDL_Surface* src_surface, int xs1, int ys1, int xs2, int ys2, SDL_Surface* dst_surface, int xd1, int yd1, int xd2, int yd2, Uint32* lutVOI) {
	int src_bpp = src_surface->format->BytesPerPixel;
	int dst_bpp = dst_surface->format->BytesPerPixel;

	switch (dst_bpp) {

		case 1:
			switch (src_bpp) {

				case 1:
					if(src_surface->format->palette != NULL) {
						SDL_SetColors(dst_surface, src_surface->format->palette->colors, 0, 256);
					}
					RectStretchTemplate(src_surface, (Uint8*)src_surface->pixels, xs1, ys1, xs2, ys2, dst_surface, (Uint8*)dst_surface->pixels, xd1, yd1, xd2, yd2, lutVOI);
					break;

				case 2:
					RectStretchTemplate(src_surface, (Uint16*)src_surface->pixels, xs1, ys1, xs2, ys2, dst_surface, (Uint8*)dst_surface->pixels, xd1, yd1, xd2, yd2, lutVOI);
					break;
			}
			break;

		case 2:
			switch (src_bpp) {

				case 2:
					RectStretchTemplate(src_surface, (Uint16*)src_surface->pixels, xs1, ys1, xs2, ys2, dst_surface, (Uint16*)dst_surface->pixels, xd1, yd1, xd2, yd2, NULL);
					break;
			}
			break;

		case 4:
			switch (src_bpp) {

				case 1:
					RectStretchTemplate(src_surface, (Uint8*)src_surface->pixels, xs1, ys1, xs2, ys2, dst_surface, (Uint32*)dst_surface->pixels, xd1, yd1, xd2, yd2, lutVOI);
					break;

				case 3:
					RectStretch24to32(src_surface, (Uint8*)src_surface->pixels, xs1, ys1, xs2, ys2, dst_surface, (Uint32*)dst_surface->pixels, xd1, yd1, xd2, yd2, lutVOI);
					break;

				case 2:
					RectStretchTemplate(src_surface, (Uint16*)src_surface->pixels, xs1, ys1, xs2, ys2, dst_surface, (Uint32*)dst_surface->pixels, xd1, yd1, xd2, yd2, lutVOI);
					break;

				case 4:
					RectStretchTemplate(src_surface, (Uint32*)src_surface->pixels, xs1, ys1, xs2, ys2, dst_surface, (Uint32*)dst_surface->pixels, xd1, yd1, xd2, yd2, NULL);
					break;

			}
			break;
	}

	// copy the colorkey
	//SDL_SetColorKey(dst_surface, SDL_SRCCOLORKEY, src_surface->format->colorkey);
}


typedef	Uint8 Pixel;

#define	WHITE_PIXEL	(255)
#define	BLACK_PIXEL	(0)


inline SDL_Surface* new_image(int xsize, int ysize)	/* create a blank image */
{
	return SDL_CreateRGBSurface(
	           SDL_SWSURFACE,
	           xsize,
	           ysize,
	           8,
	           0, 0, 0, 0);
}

inline void free_image(SDL_Surface* image) {
	SDL_FreeSurface(image);
}

inline void get_row(Pixel* row, SDL_Surface* image, int y) {
	memcpy(row, (Uint8*)image->pixels + (y * image->pitch), (sizeof(Pixel) * image->w));
}

inline void get_column(Pixel* column, SDL_Surface* image, int x) {
	register int i, d;
	register Pixel *p;
	d = image->pitch;

	register Uint8* pixels = (Uint8*)image->pixels + x;

	for (i = image->h, p = pixels; i-- > 0; p += d) {
		*column++ = *p;
	}
}

/*
 *	filter function definitions
 */

#define	filter_support		(1.0)

inline double filter(double t) {
	/* f(t) = 2|t|^3 - 3|t|^2 + 1, -1 <= t <= 1 */
	if (t < 0.0)
		t = -t;
	if (t < 1.0)
		return ((2.0 * t - 3.0) * t * t + 1.0);
	return (0.0);
}

#define	box_support		(0.5)

inline double box_filter(double t) {
	if ((t > -0.5) && (t <= 0.5))
		return (1.0);
	return (0.0);
}

#define	triangle_support	(1.0)

inline double triangle_filter(double t) {
	if (t < 0.0)
		t = -t;
	if (t < 1.0)
		return (1.0 - t);
	return (0.0);
}

#define	bell_support		(1.5)

inline double bell_filter(double t)		/* box (*) box (*) box */
{
	if (t < 0)
		t = -t;
	if (t < .5)
		return (.75 - (t * t));
	if (t < 1.5) {
		t = (t - 1.5);
		return (.5 * (t * t));
	}
	return (0.0);
}

#define	B_spline_support	(2.0)

inline double B_spline_filter(double t)	/* box (*) box (*) box (*) box */
{
	static double tt;

	if (t < 0)
		t = -t;
	if (t < 1) {
		tt = t * t;
		return ((.5 * tt * t) - tt + (2.0 / 3.0));
	} else if (t < 2) {
		t = 2 - t;
		return ((1.0 / 6.0) * (t * t * t));
	}
	return (0.0);
}

inline double sinc(double x) {
	x *= M_PI;

	if (x != 0)
		return (sin(x) / x);
	return (1.0);
}

#define	Lanczos3_support	(3.0)

inline double Lanczos3_filter(double t) {
	if (t < 0)
		t = -t;
	if (t < 3.0)
		return (sinc(t) * sinc(t / 3.0));
	return (0.0);
}

#define	Mitchell_support	(2.0)

#define	B	(1.0 / 3.0)
#define	C	(1.0 / 3.0)

inline double Mitchell_filter(double t) {
	double tt;

	tt = t * t;
	if (t < 0)
		t = -t;
	if (t < 1.0) {
		t = (((12.0 - 9.0 * B - 6.0 * C) * (t * tt))
		     + (( -18.0 + 12.0 * B + 6.0 * C) * tt)
		     + (6.0 - 2 * B));
		return (t / 6.0);
	} else if (t < 2.0) {
		t = ((( -1.0 * B - 6.0 * C) * (t * tt))
		     + ((6.0 * B + 30.0 * C) * tt)
		     + (( -12.0 * B - 48.0 * C) * t)
		     + (8.0 * B + 24 * C));
		return (t / 6.0);
	}
	return (0.0);
}

//	image rescaling routine

typedef struct {
	int	pixel;
	double	weight;
}
CONTRIB;

typedef struct {
	int	n; 		/* number of contributors */
	CONTRIB	*p; 		/* pointer to list of contributions */
}
CLIST;

CLIST	*contrib; 		/* array of contribution lists */
static double filterLUT[4001];

void PG_Draw::CreateFilterLUT() {
	static bool bCreated = false;
	double v;

	if(bCreated) {
		return;
	}

	for (int i = -2000; i < 2001; i++) {
		v = (double)i / 1000.0;
		filterLUT[i + 2000] = B_spline_filter(v);
	}

	bCreated = true;
}

typedef double (*filterfunc)(double);

//#define CLAMP(v,l,h)    ((v)<(l) ? (l) : (v) > (h) ? (h) : v)
#define CLAMP(v,l,h)    ((Uint32)v & 0xFF)
#define FLTIDX(v)		(int)(v*1000 + 2000)

void zoom(SDL_Surface* dst, SDL_Surface* src, double fwidth) {
	SDL_Surface *tmp; 			/* intermediate image */
	double xscale, yscale; 		/* zoom scale factors */
	register int i, j, k; 			/* loop variables */
	double n; 				/* pixel number */
	double center, left, right; 	/* filter calculation variables */
	double width, fscale, weight; 	/* filter calculation variables */
	Pixel *raster; 			/* a row or column of pixels */
	register Uint8* tmp_pixels;
	int tmp_pitch;
	register double tmp_w, tmp_h, src_w;

	if (!src || !dst)
		return;

	if (!src->w || !src->h || !dst->w || !dst->h)
		return;

	LockSurface(src);
	LockSurface(dst);

	/* create intermediate image to hold horizontal zoom */
	tmp = new_image(dst->w, src->h);
	LockSurface(tmp);

	xscale = (double) dst->w / (double) src->w;
	yscale = (double) dst->h / (double) src->h;

	/* pre-calculate filter contributions for a row */
	contrib = (CLIST *)calloc(dst->w, sizeof(CLIST));
	src_w = (double)src->w;
	if (xscale < 1.0) {
		width = (double)(fwidth / xscale);
		fscale = (double)(1.0 / xscale);
		tmp_w = (double)dst->w;
		for (i = 0; i < tmp_w; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)calloc((int) (width * 2 + 1),
			                                 sizeof(CONTRIB));
			center = i / xscale;
			left = ceil(center - width);
			right = floor(center + width);
			for (j = (int)left; j <= (int)right; ++j) {
				weight = center - j;
				//weight = B_spline_filter(weight / fscale) / fscale;
				weight = filterLUT[FLTIDX(weight / fscale)] / fscale;
				if (j < 0) {
					n = -j;
				} else if (j >= src_w) {
					n = (src_w - j) + src_w - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = (int)n;
				contrib[i].p[k].weight = weight;
			}
		}
	} else {
		tmp_w = dst->w;
		for (i = 0; i < tmp_w; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)calloc((int) (fwidth * 2 + 1),
			                                 sizeof(CONTRIB));
			center = i / xscale;
			left = ceil(center - fwidth);
			right = floor(center + fwidth);
			for (j = (int)left; j <= (int)right; ++j) {
				weight = center - j;
				//weight = B_spline_filter(weight);
				weight = filterLUT[FLTIDX(weight)];
				if (j < 0) {
					n = -j;
				} else if (j >= src_w) {
					n = (src_w - j) + src_w - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = (int)n;
				contrib[i].p[k].weight = weight;
			}
		}
	}

	/* apply filter to zoom horizontally from src to tmp */
	tmp_pitch = tmp->pitch;
	tmp_pixels = (Uint8*)tmp->pixels;

	raster = (Pixel *)calloc(src->w, sizeof(Pixel));
	tmp_h = tmp->h;
	tmp_w = tmp->w;

	int offset = (int)(tmp_pitch - tmp_w);

	for (k = 0; k < tmp_h; ++k) {
		//get_row(raster, src, k);
		memcpy(raster, (Uint8*)src->pixels + (k * src->pitch), (sizeof(Pixel) * src->w));
		for (i = 0; i < tmp_w; ++i) {
			weight = 0.0;
			for (j = 0; j < contrib[i].n; ++j) {
				weight += raster[contrib[i].p[j].pixel]
				          * contrib[i].p[j].weight;
			}
			*((Uint8 *)tmp_pixels++) = (Pixel)CLAMP(weight, BLACK_PIXEL, WHITE_PIXEL);
		}
		// tmp_pixels -= i;
		tmp_pixels += offset;
	}

	free(raster);

	/* free the memory allocated for horizontal filter weights */
	for (i = 0; i < tmp_w; ++i) {
		free(contrib[i].p);
	}
	free(contrib);

	/* pre-calculate filter contributions for a column */
	contrib = (CLIST *)calloc(dst->h, sizeof(CLIST));
	if (yscale < 1.0) {
		width = fwidth / yscale;
		fscale = 1.0 / yscale;
		tmp_h = dst->h;
		for (i = 0; i < tmp_h; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)calloc((int) (width * 2 + 1),
			                                 sizeof(CONTRIB));
			center = i / yscale;
			left = ceil(center - width);
			right = floor(center + width);
			for (j = (int)left; j <= (int)right; ++j) {
				weight = center - j;
				//weight = B_spline_filter(weight / fscale) / fscale;
				weight = filterLUT[FLTIDX(weight / fscale)] / fscale;
				if (j < 0) {
					n = -j;
				} else if (j >= tmp->h) {
					n = (tmp->h - j) + tmp->h - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = (int)n;
				contrib[i].p[k].weight = weight;
			}
		}
	} else {
		tmp_h = dst->h;
		for (i = 0; i < tmp_h; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)calloc((int) (fwidth * 2 + 1),
			                                 sizeof(CONTRIB));
			center = i / yscale;
			left = ceil(center - fwidth);
			right = floor(center + fwidth);
			for (j = (int)left; j <= (int)right; ++j) {
				weight = center - j;
				//weight = B_spline_filter(weight);
				weight = filterLUT[FLTIDX(weight)];
				if (j < 0) {
					n = -j;
				} else if (j >= tmp->h) {
					n = (tmp->h - j) + tmp->h - 1;
				} else {
					n = j;
				}
				k = contrib[i].n++;
				contrib[i].p[k].pixel = (int)n;
				contrib[i].p[k].weight = weight;
			}
		}
	}

	/* apply filter to zoom vertically from tmp to dst */
	tmp_pitch = dst->pitch;
	tmp_pixels = (Uint8*)dst->pixels;

	raster = (Pixel *)calloc(tmp->h, sizeof(Pixel));
	tmp_w = dst->w;
	tmp_h = dst->h;

	offset = 1 - (int)(tmp_w * tmp_pitch);

	for (k = 0; k < tmp_w; ++k) {
		get_column(raster, tmp, k);
		for (i = 0; i < tmp_h; ++i) {
			weight = 0.0;
			for (j = 0; j < contrib[i].n; ++j) {
				weight += raster[contrib[i].p[j].pixel]
				          * contrib[i].p[j].weight;
			}
			*(((Uint8 *)tmp_pixels) + i*tmp_pitch + k) = (Pixel)CLAMP(weight, BLACK_PIXEL, WHITE_PIXEL);
		}
		//tmp_pixels += offset;
	}

	free(raster);

	/* free the memory allocated for vertical filter weights */
	for (i = 0; i < tmp_h; ++i) {
		free(contrib[i].p);
	}
	free(contrib);

	UnlockSurface(src);
	UnlockSurface(dst);
	UnlockSurface(tmp);
	free_image(tmp);
}


void PG_Draw::PG_SmoothFast(SDL_Surface* src, SDL_Surface* dst) {
	zoom(dst, src, B_spline_support);
}

SDL_Surface* PG_CopyRect(SDL_Surface* s, int x, int y, int w, int h) {
	SDL_Surface* image = new_image(w, h);

	for (int i = 0; i < h; i++) {
		memcpy(
		    (Uint8*)image->pixels + i*image->pitch,
		    (Uint8*)s->pixels + (y + i)*s->pitch + x,
		    w);
	}

	return image;
}


#define SCALE 1024
#define HALFSCALE 512

/*
	based on scaling algorithm of "Extended Portable Bitmap Toolkit" (pbmplus10dec91)
*/

void PG_Draw::InterpolatePixel(SDL_Surface* src, SDL_Surface* dest) {

	register Uint16 x;
	register Uint16 y;
	register const Uint8 *p;
	register Uint8 *q;
	Uint8 const *sp = NULL;                          // initialization avoids compiler warning
	Uint8 const *fp;
	Uint8 *sq;

	if (!src || !dest)
		return;

	if (!src->w || !src->h || !dest->w || !dest->h)
		return;

	int Dest_X = dest->w;
	int Dest_Y = dest->h;
	int Src_X = src->w;
	int Src_Y = src->h;

	const unsigned long sxscale = (unsigned long)(((double)Dest_X / (double)Src_X) * SCALE);
	const unsigned long syscale = (unsigned long)(((double)Dest_Y / (double)Src_Y) * SCALE);

	Uint8 *xtemp = new Uint8[Src_X];
	signed long *xvalue = new signed long[Src_X];

	fp = (Uint8*)src->pixels;
	sq = (Uint8*)dest->pixels;
	for (x = 0; x < Src_X; x++)
		xvalue[x] = HALFSCALE;

	register unsigned long yfill = SCALE;
	register unsigned long yleft = syscale;
	register int yneed = 1;
	int ysrc = 0;

	for (y = 0; y < Dest_Y; y++) {
		if (Src_Y == Dest_Y) {
			sp = fp;

			for (x = 0, p = sp, q = xtemp; x < Src_X; x++)
				*(q++) = *(p++);

			fp += src->pitch;
		} else {
			while (yleft < yfill) {
				if (yneed && (ysrc < (int)Src_Y)) {
					sp = fp;
					fp += src->pitch;
					ysrc++;
				}

				for (x = 0, p = sp; x < Src_X; x++)
					xvalue[x] += yleft * (signed long)(*(p++));

				yfill -= yleft;
				yleft = syscale;
				yneed = 1;
			}

			if (yneed && (ysrc < (int)Src_Y)) {
				sp = fp;
				fp += src->pitch;
				ysrc++;
				yneed = 0;
			}

			for (x = 0, p = sp, q = xtemp; x < Src_X; x++) {
				register signed long v = xvalue[x] + yfill * (signed long)(*(p++));
				v /= SCALE;
				*(q++) = (unsigned char)v;  //& 0xFF; //(Uint8)((v > 255) ? 255 : v);
				xvalue[x] = HALFSCALE;
			}
			yleft -= yfill;
			if (yleft == 0) {
				yleft = syscale;
				yneed = 1;
			}
			yfill = SCALE;
		}
		if (Src_X == Dest_X) {

			for (x = 0, p = xtemp, q = sq; x < Dest_X; x++)
				*(q++) = *(p++);

			sq += dest->pitch;
		} else {
			register signed long v = HALFSCALE;
			register unsigned long xfill = SCALE;
			register unsigned long xleft;
			register int xneed = 0;
			q = sq;

			for (x = 0, p = xtemp; x < Src_X; x++, p++) {
				xleft = sxscale;

				while (xleft >= xfill) {
					if (xneed) {
						q++;
						v = HALFSCALE;
					}
					v += xfill * (signed long)(*p);
					v /= SCALE;
					*q = (unsigned char)v;  // & 0xFF; //(Uint8)((v > 255) ? 255 : v);
					xleft -= xfill;
					xfill = SCALE;
					xneed = 1;
				}
				if (xleft > 0) {
					if (xneed) {
						q++;
						v = HALFSCALE;
						xneed = 0;
					}
					v += xleft * (signed long)(*p);
					xfill -= xleft;
				}
			}

			if (xfill > 0)
				v += xfill * (signed long)(*(--p));

			if (!xneed) {
				v /= SCALE;
				*q = (unsigned char)v;  // & 0xFF; //(Uint8)((v > 255) ? 255 : v);
			}

			sq += dest->pitch;
		}
	}

	delete[] xtemp;
	delete[] xvalue;
}
