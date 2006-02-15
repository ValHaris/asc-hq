/*
    ParaGUI - crossplatform widgetset
    gradient - gradient drawing functions
 
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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/draw/gradient.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

#include "pgdraw.h"

#include <cmath>
#include <cassert>

SDL_Surface* PG_Draw::CreateGradient(const PG_Rect& r, PG_Gradient& gradient) {
	return CreateGradient(
	           r,
	           gradient.colors[0],
	           gradient.colors[1],
	           gradient.colors[2],
	           gradient.colors[3]);
}

SDL_Surface* PG_Draw::CreateGradient(const PG_Rect& r, const PG_Color& ul, const PG_Color& ur, const PG_Color& dl, const PG_Color& dr) {
	SDL_Surface *grd = PG_Draw::CreateRGBSurface(r.w, r.h);

	r.my_xpos = 0;
	r.my_ypos = 0;
	PG_Draw::DrawGradient(grd, r, ul, ur, dl, dr);

	return grd;
}

void PG_Draw::DrawGradient(SDL_Surface* surface, const PG_Rect& r, PG_Gradient& gradient) {
	PG_Draw::DrawGradient(
	    surface,
	    r,
	    gradient.colors[0],
	    gradient.colors[1],
	    gradient.colors[2],
	    gradient.colors[3]);
}

void PG_Draw::DrawGradient(SDL_Surface * surface, const PG_Rect& rect, const PG_Color& ul, const PG_Color& ur, const PG_Color& dl, const PG_Color& dr) {
	Sint32 v00,v01,v02;
	Sint32 v10,v11,v12;

	Sint32 w = rect.my_width;
	Sint32 h = rect.my_height;

	if (!surface)
		return;

	if(w == 0 || h == 0)
		return;

	if (w > surface->w || h > surface->h)
		return;

	Uint32 c1 = ul.MapRGB(surface->format);
	Uint32 c2 = ur.MapRGB(surface->format);
	Uint32 c3 = dl.MapRGB(surface->format);
	Uint32 c4 = dr.MapRGB(surface->format);

	// solid color gradient ?
	if((c1 == c2) && (c2 == c3) && (c3 == c4)) {
		SDL_FillRect(surface, (PG_Rect*)&rect, c1);
		return;
	}

	PG_Rect clip;
	SDL_GetClipRect(surface, &clip);
	PG_Rect drawrect = rect.IntersectRect(clip);

	if(drawrect.IsNull()) {
		return;
	}

	int ox = drawrect.x - rect.x;
	int oy = drawrect.y - rect.y;

	if(SDL_MUSTLOCK(surface)) {
		SDL_LockSurface(surface);
	}

	// color space vectors
	v00 = ((ur.r - ul.r) * 256) / w;
	v01 = ((ur.g - ul.g) * 256) / w;
	v02 = ((ur.b - ul.b) * 256) / w;

	v10 = ((dr.r - dl.r) * 256) / w;
	v11 = ((dr.g - dl.g) * 256) / w;
	v12 = ((dr.b - dl.b) * 256) / w;

	Sint32 r1, g1, b1;
	Sint32 r2, g2, b2;
	Sint32 yr, yg, yb;
	Sint32 r,g,b;

	r1 = ul.r * 256;
	g1 = ul.g * 256;
	b1 = ul.b * 256;

	r2 = dl.r * 256;
	g2 = dl.g * 256;
	b2 = dl.b * 256;

	// set colors with offset (ox)
	r1 += v00 * ox;
	g1 += v01 * ox;
	b1 += v02 * ox;
	r2 += v10 * ox;
	g2 += v11 * ox;
	b2 += v12 * ox;

	SDL_PixelFormat* format = surface->format;
	Uint8 Rloss = 8+format->Rloss;
	Uint8 Gloss = 8+format->Gloss;
	Uint8 Bloss = 8+format->Bloss;
	Uint8 Rshift = format->Rshift;
	Uint8 Gshift = format->Gshift;
	Uint8 Bshift = format->Bshift;
	Uint32 Amask = format->Amask;
	Uint8 Ashift = format->Ashift;

	Uint8 bpp = format->BytesPerPixel;
	Uint32 pitch = surface->pitch;
	Uint8* bits = ((Uint8 *) surface->pixels) + (rect.y + oy)* pitch + (rect.x + ox)* bpp;
	Uint32 y_pitch = pitch*drawrect.h - bpp;
	register Uint32 pixel = 0;

	for (register Sint32 x = 0; x < drawrect.w; x++) {

		yr = (r2 - r1) / h;
		yg = (g2 - g1) / h;
		yb = (b2 - b1) / h;

		r = r1;
		g = g1;
		b = b1;
		r += yr * oy;
		g += yg * oy;
		b += yb * oy;

		for (register Sint32 y = 0; y < drawrect.h; y++) {

			/* Set the pixel */
			switch (bpp) {
				case 1:
					pixel = SDL_MapRGB ( surface->format, r>>8, g>>8, b>>8 );
					*((Uint8 *) (bits)) = (Uint8) pixel;
					break;

				case 2:
					pixel =  (r>>Rloss) << Rshift
					         | (g>>Gloss) << Gshift
					         | (b>>Bloss) << Bshift;

					*((Uint16 *) (bits)) = (Uint16) pixel;
					break;

				case 3: {
						pixel =  (r>>Rloss) << Rshift
						         | (g>>Gloss) << Gshift
						         | (b>>Bloss) << Bshift;

						Uint8 ri = (pixel >> surface->format->Rshift) & 0xFF;
						Uint8 gi = (pixel >> surface->format->Gshift) & 0xFF;
						Uint8 bi = (pixel >> surface->format->Bshift) & 0xFF;
						*((bits) + surface->format->Rshift / 8) = ri;
						*((bits) + surface->format->Gshift / 8) = gi;
						*((bits) + surface->format->Bshift / 8) = bi;
					}
					break;

				case 4:
					pixel =  (r>>Rloss) << Rshift
					         | (g>>Gloss) << Gshift
					         | (b>>Bloss) << Bshift
					         | ((SDL_ALPHA_OPAQUE << Ashift) & Amask);

					*((Uint32 *) (bits)) = (Uint32) pixel;
					break;
			}

			r += yr;
			g += yg;
			b += yb;

			// next pixel
			bits += pitch;
		}

		r1 += v00;
		g1 += v01;
		b1 += v02;
		r2 += v10;
		g2 += v11;
		b2 += v12;

		bits -= y_pitch;
	}

	if(SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
}
