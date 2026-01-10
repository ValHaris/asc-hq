/*
    ParaGUI - crossplatform widgetset
    surface - surface creation and manipulation functions
 
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
    Update Date:      $Date: 2009-04-18 13:48:40 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/draw/surface.cpp,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

#include "pgdraw.h"
#include "pglog.h"

SDL_Surface* PG_Draw::CreateRGBSurface(Uint16 w, Uint16 h, int flags) {
	return SDL_CreateRGBSurface (
	           flags,
	           w, h,
	           32,
			   0x00FF0000,
			   0x0000FF00,
			   0x000000FF,
			   0xFF000000);
}

static void Draw3TileH(SDL_Surface* src, const PG_Rect& r, SDL_Surface* dst, Uint8 blend = 0) {
	PG_Rect srcrect;
	PG_Rect dstrect;
	SDL_Surface* temp;
	int w,h;

	// source rectangles
	h = r.h;
	w = (int)( ( (double)h / (double)src->h) * (double)src->w );
	srcrect.SetRect(0,0,w,h);
	w /= 3;

	if (!w) {
		return;
	}

	// rescale surface

	if(h == src->h) {
		temp = src;
	} else {
		temp = PG_Draw::ScaleSurface(src, srcrect);
	}

	// set per surface alpha
	if(blend > 0) {
		SDL_SetSurfaceAlphaMod(temp, 255-blend);
	} else {
		SDL_SetSurfaceAlphaMod(temp, 0);
	}

	// blit part 1 (left)

	SDL_SetClipRect(dst, NULL);
	dstrect.SetRect(r.x,r.y,w,h);
	srcrect.SetRect(0, 0, w, h);
	PG_Draw::BlitSurface(temp, srcrect, dst, dstrect);

	// blit part 2 (middle)

	dstrect.SetRect(r.x+w, r.y, r.w-w*2, h);
	SDL_SetClipRect(dst, &dstrect);

	// blit it
	srcrect.SetRect(w, 0, w, h);
	for(int i = 1; i<(r.w/w); i++) {
		dstrect.SetRect(r.x+w*i, r.y, w, h);
		PG_Draw::BlitSurface(temp, srcrect, dst, dstrect);
	}
	SDL_SetClipRect(dst, NULL);

	// blit part 3 (right)

	dstrect.SetRect(r.x+r.w-w, r.y,w,h);
	srcrect.SetRect(w*2, 0, w, h);
	PG_Draw::BlitSurface(temp, srcrect, dst, dstrect);

	// free temp surface
	if(h != src->h) {
		SDL_FreeSurface(temp);
	}

}

static void Draw3TileV(SDL_Surface* src, const PG_Rect& r, SDL_Surface* dst, Uint8 blend = 0) {
	PG_Rect srcrect;
	PG_Rect dstrect;
	SDL_Surface* temp;
	int w,h;

	// source rectangles
	w = r.w;
	h = (int)( ( (double)w / (double)src->w) * (double)src->h );
	srcrect.SetRect(0,0,w,h);
	h /= 3;

	if (!h) {
		return;
	}

	// rescale surface

	if(w == src->w) {
		temp = src;
	} else {
		temp = PG_Draw::ScaleSurface(src, srcrect);
	}

	// set per surface alpha
	if(blend > 0) {
		SDL_SetSurfaceAlphaMod(temp, 255-blend);
	} else {
		SDL_SetSurfaceAlphaMod(temp, 0);
	}

	// blit part 1 (top)

	SDL_SetClipRect(dst, NULL);
	dstrect.SetRect(r.x,r.y,w,h);
	srcrect.SetRect(0, 0, w, h);
	PG_Draw::BlitSurface(temp, srcrect, dst, dstrect);

	// blit part 2 (middle)

	// set cliprect
	dstrect.SetRect(r.x,r.y+h,w,r.h-h*2);
	SDL_SetClipRect(dst, &dstrect);

	// blit it
	srcrect.SetRect(0, h, w, h);
	for(int i = 1; i<(r.h/h); i++) {
		dstrect.SetRect(r.x, r.y+h*i, w, h);
		PG_Draw::BlitSurface(temp, srcrect, dst, dstrect);
	}
	SDL_SetClipRect(dst, NULL);

	// blit part 3 (bottom)

	dstrect.SetRect(r.x,r.y+r.h-h,w,h);
	srcrect.SetRect(0, h*2, w, h);
	PG_Draw::BlitSurface(temp, srcrect, dst, dstrect);

	// free temp surface
	if(w != src->w) {
		SDL_FreeSurface(temp);
	}

}

static void DrawTileSurface(SDL_Surface* src, const PG_Rect& r, SDL_Surface* dst, Uint8 blend = 0) {
	PG_Rect srcrect;
	PG_Rect dstrect;

	// tile the background image over the r in surface
	dstrect = r;

	srcrect.SetRect(0, 0, src->w, src->h);

	int yc = (r.my_height / src->h) +1;
	int xc = (r.my_width / src->w) +1;

	if(blend > 0) {
		SDL_SetSurfaceAlphaMod(src, 255-blend);
	} else {
		SDL_SetSurfaceAlphaMod(src, 0);
	}

	srcrect.my_width = src->w;
	srcrect.my_height = src->h;
	dstrect.my_width = src->w;
	dstrect.my_height = src->h;

	SDL_SetClipRect(dst, &r);
	for(int y=0; y<yc; y++) {
		for(int x=0; x<xc; x++) {
			dstrect.x = r.my_xpos + src->w * x;
			dstrect.y = r.my_ypos + src->h * y;

			PG_Draw::BlitSurface(src, srcrect, dst, dstrect);
		}
	}
	SDL_SetClipRect(dst, NULL);
}

/*
**	9TILE
**
**	ABC
**	DEF
**	GHI
**
**	  v
**
**	ABBBBBBC
**	DEEEEEEEF
**	DEEEEEEEF
**	DEEEEEEEF
**	DEEEEEEEF
**	DEEEEEEEF
**	GHHHHHHI
*/
static void Draw9Tile(SDL_Surface* src, const PG_Rect& r, SDL_Surface* dst, Uint8 blend = 0) {
	PG_Rect srcrect;
	PG_Rect dstrect;
	int i = 0;

	// 3 source stripes (ABC, DEF, HGI)
	SDL_Surface* src_stripe[3];
	int h_src_stripe = src->h / 3;

	// copy source stripes
	dstrect.SetRect(0, 0, src->w, h_src_stripe);
	for(i=0; i<3; i++) {
		// set source rect
		srcrect.SetRect(0, i*h_src_stripe, src->w, h_src_stripe);

		// create stripe surface
		src_stripe[i] = SDL_CreateRGBSurface(
		                    SDL_SWSURFACE,
		                    srcrect.w,
		                    srcrect.h,
		                    32, //src->format->BitsPerPixel,
		                    0, //src->format->Rmask,
		                    0, //src->format->Gmask,
		                    0, //src->format->Bmask,
		                    0 //src->format->Amask
		                );

		// copy stripe
		PG_Draw::BlitSurface(src, srcrect, src_stripe[i], dstrect);
	}

	// 3 destination stripes (ABBBBBBC, DEEEEEEEF, GHHHHHHI)
	SDL_Surface* dst_stripe[3];
	int h_dst_stripe = src->h / 3;

	// create destination stripes
	dstrect.SetRect(0, 0, r.w, h_dst_stripe);
	for(i=0; i<3; i++) {
		// create dest. stripe
		dst_stripe[i] = SDL_CreateRGBSurface(
		                    SDL_SWSURFACE,
		                    dstrect.w,
		                    dstrect.h,
		                    32, //src->format->BitsPerPixel,
		                    0, //src->format->Rmask,
		                    0, //src->format->Gmask,
		                    0, //src->format->Bmask,
		                    0 //src->format->Amask
		                );

		// 3TILEH the source to the dest. stripe
		Draw3TileH(src_stripe[i], dstrect, dst_stripe[i], blend);
		// set alpha
		/*if(blend > 0) {
			SDL_SetAlpha(dst_stripe[i], SDL_SRCALPHA, 255-blend);
		} else {
			SDL_SetAlpha(dst_stripe[i], 0, 0);
		}*/
	}

	// copy stripe 0 (top)
	srcrect.SetRect(0, 0, dst_stripe[0]->w, dst_stripe[0]->h);
	dstrect.SetRect(r.x, r.y, r.w, dst_stripe[0]->h);
	PG_Draw::BlitSurface(dst_stripe[0], srcrect, dst, dstrect);

	// tile stripe 1 (middle part)
	dstrect.SetRect(r.x, r.y + dst_stripe[0]->h, r.w, r.h - 2*dst_stripe[0]->h);
	DrawTileSurface(dst_stripe[1], dstrect, dst, blend);

	// copy stripe 2 (bottom)
	srcrect.SetRect(0, 0, dst_stripe[2]->w, dst_stripe[2]->h);
	dstrect.SetRect(r.x, r.y+r.h-dst_stripe[2]->h, r.w, dst_stripe[2]->h);
	PG_Draw::BlitSurface(dst_stripe[2], srcrect, dst, dstrect);

	// cleanup
	for(i=0; i<3; i++) {
		SDL_FreeSurface(src_stripe[i]);
		SDL_FreeSurface(dst_stripe[i]);
	}

}

void PG_Draw::DrawThemedSurface(SDL_Surface* surface, const PG_Rect& r, PG_Gradient* gradient,SDL_Surface* background, BkMode bkmode, Uint8 blend) {
	static PG_Rect srcrect;
	static PG_Rect dstrect;
	//int x,y;
	bool bColorKey = false;
	PG_Color uColorKey;
	Uint32 c;
	PG_Rect oldclip;

	// check if we have anything to do
	if (!surface || !r.h || !r.w)
		return;

	// draw the gradient first
	if((background == NULL) || (background && (blend > 0))) {
		if(gradient != NULL) {
			if(SDL_MUSTLOCK(surface)) {
				SDL_LockSurface(surface);
			}
			DrawGradient(surface, r, *gradient);
			if(SDL_MUSTLOCK(surface)) {
				SDL_UnlockSurface(surface);
			}
		}
	}

	if(!background)
		return;

	if (!background->w || !background->h)
		return;

	//int yc;
	//int xc;
	SDL_Surface* temp;
	//int w,h;

	Uint32 colorkey;
	bColorKey = SDL_GetColorKey(background, &colorkey) == 0;
	Uint8 rc,gc,bc;

	SDL_GetRGB(colorkey, background->format, &rc, &gc, &bc);
	uColorKey = (Uint32)((rc << 16) | (gc << 8) | bc);

	if(((gradient == NULL) || (blend == 0)) && bColorKey) {
		SDL_SetColorKey(background, 0, 0);
	}

	SDL_GetClipRect(surface, &oldclip);

   // prevent any aliasing if the sizes match
   if ( bkmode == STRETCH && r.w==background->w && r.h == background->h )
      bkmode = TILE;
   
	switch(bkmode) {

			//
			// BKMODE_TILE
			//

		case TILE:
			DrawTileSurface(background, r, surface, blend);
			break;

			//
			// BKMODE_STRETCH
			//

		case STRETCH:
			// stretch the background to fit the surface

			// Scale the background to fit this widget, using
			// anti-aliasing
			temp = PG_Draw::ScaleSurface(background, r);

			// set per surface alpha
			if(blend > 0) {
				SDL_SetSurfaceAlphaMod(temp, 255-blend);
			} else {
				SDL_SetSurfaceAlphaMod(temp, 0);
			}

			// blit it
			SDL_BlitSurface(temp, NULL, surface, (PG_Rect*)&r);

			// free the temp surface
			SDL_FreeSurface(temp);
			break;

			//
			// BKMODE_3TILEH
			//

		case TILE3H:
			Draw3TileH(background, r, surface, blend);
			break;

			//
			// BKMODE_3TILEV
			//

		case TILE3V:
			Draw3TileV(background, r, surface, blend);
			break;

			//
			// BKMODE_9TILE
			//

		case TILE9:
			Draw9Tile(background, r, surface, blend);
			break;

	}

	SDL_SetClipRect(surface, const_cast<PG_Rect*>(&oldclip));

	if((/*(gradient == NULL) ||*/ (blend == 0)) && bColorKey) {
		c = uColorKey.MapRGB(background->format);
		SDL_SetColorKey(background, SDL_TRUE, c);
		c = uColorKey.MapRGB(surface->format);
		SDL_SetColorKey(surface, SDL_TRUE, c);
	}
}
