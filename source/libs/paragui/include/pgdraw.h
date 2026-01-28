/*
    ParaGUI - crossplatform widgetset
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
    Update Date:      $Date: 2009-04-18 13:48:39 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgdraw.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

/** \file pgdraw.h
	Header file for the PG_Draw namespace.
	@author Alexander Pipelka
*/

#ifndef PG_DRAW_H
#define PG_DRAW_H

#include "pgrect.h"
#include "pgcolor.h"
#include "pgexception.h"

#ifndef M_PI
/**
	The definition of the number PI.
	Some platforms dont define M_PI. So we have to define it.
*/
#define M_PI 3.14159265359
#endif // M_PI

namespace PG_Draw {

// Background modes
enum BkMode {
    TILE,
    /**
    	Backgroundmode STRETCH.
    	Macro defining the background mode for stretching
    */
    STRETCH,
    /**
    	Backgroundmode 3TILEH.
    	Macro defining the background mode for horizontal 3 part tiling
        (the left part of the tile to the left, the right part to the right,
        the middle part repeatedly within the rest)
    */
    TILE3H,
    /**
    	Backgroundmode 3TILEV.
    	Macro defining the background mode for vertical 3 part tiling
    */
    TILE3V,
    /**
    	Backgroundmode 9TILE.
    	Macro defining the background mode for 9 part tiling
    */
    TILE9
};

/**
	@short ParaGUI drawing functions
 
	These functions can be used for native drawing on surfaces.
*/

//! Rotate and scale an SDL_Surface
/*!
  Rotates and scales a 32bit or 8bit SDL_Surface to newly created
  destination surface. If smooth is 1 the destination 32bit surface is
  anti-aliased. If the surface is not 8bit or 32bit RGBA/ABGR it will
  be converted into a 32bit RGBA format on the fly.
 
  \param src source surface
  \param angle the rotation in degrees
  \param zoom the scaling factor
  \param smooth whether or not to use anti-aliasing
 
  \return A new surface with the scaled, rotated original surface.
*/

DECLSPEC SDL_Surface* RotoScaleSurface(SDL_Surface *src, double angle,
                                       double zoom, bool smooth = true);

//! Scale an SDL_Surface
/*!
  Scales a 32bit or 8bit SDL_Surface to newly created destination
  surface.  If the surface is not 8bit or 32bit RGBA/ABGR it will be
  converted into a 32bit RGBA format on the fly.
 
  \param src source surface
  \param zoomx, zoomy width and height scaling factors
  \param smooth whether or not to enable anti-aliasing
 
  \return A newly created surface with the scaled surface
*/
DECLSPEC SDL_Surface* ScaleSurface(SDL_Surface *src, double zoomx, double zoomy,
                                   bool smooth = true);

//! Scale an SDL_Surface
/*!
  Scales a 32bit or 8bit SDL_Surface to newly created destination
  surface.  If the surface is not 8bit or 32bit RGBA/ABGR it will be
  converted into a 32bit RGBA format on the fly.
 
  \param src source surface
  \param rect PG_Rect specifying the width and height of the new surface
  \param smooth whether or not to enable anti-aliasing
 
  \return A newly created surface with the scaled surface
*/
static inline SDL_Surface *ScaleSurface(SDL_Surface *src, const PG_Rect &rect,
                                        bool smooth = true) {
	return ScaleSurface(src, static_cast<double>(rect.w) / src->w,
	                    static_cast<double>(rect.h) / src->h, smooth);
}

//! Scale an SDL_Surface
/*!
  Scales a 32bit or 8bit SDL_Surface to newly created destination
  surface.  If the surface is not 8bit or 32bit RGBA/ABGR it will be
  converted into a 32bit RGBA format on the fly.
 
  \param src source surface
  \param newx, newy the width and height of the new surface
  \param smooth whether or not to enable anti-aliasing
 
  \return A newly created surface with the scaled surface
*/
static inline SDL_Surface *ScaleSurface(SDL_Surface *src, Uint16 newx, Uint16 newy,
                                        bool smooth = true) {
	return ScaleSurface(src, static_cast<double>(newx) / src->w,
	                    static_cast<double>(newy) / src->h, smooth);
}

//! Scale and blit surface
/*!
  Scales a 32 bit or 8 bit SDL_Surface to the size of the destination
  surface dst and blits the result to the destination surface.  If the
  surface is not 8bit or 32bit RGBA/ABGR it will be converted into a
  32bit RGBA format on the fly.
 
  \param src Source surface
  \param dst destination surface
  \param smooth whether or not to use anti-aliasing */
DECLSPEC void BlitScale(SDL_Surface *src, SDL_Surface *dst, bool smooth = true);

/**
	Creates a surface filled with a gradient
 
	@param r		the dimensions of the surface to be created
	@param gradient the gradient colors to use (order: ul(upper left), ur, dl, dr)
	@return			a SDL_Surface pointer to the new surface
 
	This function creates a new surface filled with a given gradient defined by a set of colors
*/
DECLSPEC SDL_Surface* CreateGradient(const PG_Rect& r, PG_Gradient& gradient);

/**
	Creates a surface filled with a gradient
 
	@param r		the dimensions of the surface to be created
	@param ul		upper/left gradient color
	@param ur		upper/right gradient color
	@param dl		lower/left gradient color
	@param dr		lower/right gradient color
	@return			a SDL_Surface pointer to the new surface
 
	This function creates a new surface filled with a given gradient defined by a set of colors
*/
DECLSPEC SDL_Surface* CreateGradient(const PG_Rect& r, const PG_Color& ul, const PG_Color& ur, const PG_Color& dl, const PG_Color& dr);

/**
    Draw a gradient on a surface
 
    @param surface  the surface to draw the gradient to
    @param r        the rectangle where the gradient should be drawn
    @param gradient the gradient colors (order: ul(upper left), ur, dl, dr)
 
    \note If the surface is clipped, the gradient is only drawn within 
    the intersection of the clipping rect and r.
*/
DECLSPEC void DrawGradient(SDL_Surface* surface, const PG_Rect& r, PG_Gradient& gradient);

/**
    Draw a gradient on a surface
 
    @param surface  the surface to draw the gradient to
    @param rect        the rectangle where the gradient should be drawn
	@param ul		upper/left gradient color
	@param ur		upper/right gradient color
	@param dl		lower/left gradient color
	@param dr		lower/right gradient color
 
    This function is the same as the other one above except that the gradient
    colors are the arguments instead of a gradient.
*/
DECLSPEC void DrawGradient(SDL_Surface * surface, const PG_Rect& rect, const PG_Color& ul, const PG_Color& ur, const PG_Color& dl, const PG_Color& dr);

/**
	Create a new SDL surface
	@param w width of the new surface
	@param h height of the new surface
	@param flags surface-flags (default = SDL_SWSURFACE)
	@return pointer to the new surface
	This function creates a new SDL surface
*/
DECLSPEC SDL_Surface* CreateRGBSurface(Uint16 w, Uint16 h, int flags = SDL_SWSURFACE);

/**
	Draw a 'themed' surface
 
	@param surface			the surface to draw on
	@param r					the rectangle of the surface to draw in
	@param gradient		pointer to a gradient structure (may be NULL)
	@param background	pointer to a background surface (may be NULL)
	@param bkmode		the mode how to fill in the background surface (BKMODE_TILE | BKMODE_STRETCH | BKMODE_3TILEH | BKMODE_3TILEV | BKMODE_9TILE)
	@param blend                the blend-level between gradient an background; the
                                higher the blend level the more transparent the background is.
 
    \note This function first draws the gradient, then the (partly transparent) background.
*/
DECLSPEC void DrawThemedSurface(SDL_Surface* surface, const PG_Rect& r, PG_Gradient* gradient, SDL_Surface* background, BkMode bkmode, Uint8 blend);

/**
	Draw a line.
	@param surface destination surface
	@param x0 x startposition
	@param y0 y startposition
	@param x1 x endposition
	@param y1 y endposition
	@param color color of the line
	@param width width of the line
 
	Draws a line with given color and width onto a surface.
*/
DECLSPEC void DrawLine(SDL_Surface* surface, Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, const PG_Color& color, Uint8 width = 1);

/**
	Set a pixel.
	@param x x position
	@param y y position
	@param c color
	@param surface destination surface
 
    \note This function assumes that the surface has already been locked if
    neccessary.
*/
DECLSPEC void SetPixel(int x, int y, const PG_Color& c, SDL_Surface * surface);

#ifdef debugblits
extern void printblit(SDL_Surface* from, SDL_Surface* to, int result );
#endif

/**
	replacement for SDL_BlitSurface
	@param srf_src	source surface
	@param rect_src	PG_Rect of the source rectangle
	@param srf_dst	destination surface
	@param rect_dst	PG_Rect of the destination rectangle
 
	This function simply replaces SDL_BlitSurface and uses PG_Rect instead of SDL_Rect.
*/
inline void BlitSurface(SDL_Surface* srf_src, const PG_Rect& rect_src, SDL_Surface* srf_dst, const PG_Rect& rect_dst) {
   int result =  SDL_BlitSurface(srf_src, const_cast<PG_Rect*>(&rect_src), srf_dst, const_cast<PG_Rect*>(&rect_dst));
	if ( result < 0) {

#ifdef debugblits

       printblit(srf_src, srf_dst, result);
	   SDL_SetSurfaceBlendMode(srf_src, SDL_BLENDMODE_NONE);
	   result = SDL_BlitSurface(srf_src, const_cast<PG_Rect*>(&rect_src), srf_dst, const_cast<PG_Rect*>(&rect_dst));
	   printblit(srf_src, srf_dst, result);
	   if ( result == 0 )
	      return;

       SDL_SetSurfaceBlendMode(srf_src, SDL_BLENDMODE_BLEND);
       result = SDL_BlitSurface(srf_src, const_cast<PG_Rect*>(&rect_src), srf_dst, const_cast<PG_Rect*>(&rect_dst));
       printblit(srf_src, srf_dst, result);
       if ( result == 0 )
          return;

       SDL_SetSurfaceBlendMode(srf_src, SDL_BLENDMODE_ADD);
       result = SDL_BlitSurface(srf_src, const_cast<PG_Rect*>(&rect_src), srf_dst, const_cast<PG_Rect*>(&rect_dst));
       printblit(srf_src, srf_dst, result);

	   return;
#endif
		throw PG_Exception(SDL_GetError());
	}
}

/**
    Tiles a surface with a given image
    
    @param surface  the surface to draw to
    @param ref      unused, to be removed
    @param drawrect the area on the surface you want to draw to
    @param tilemap  the image you want to tile the surface with
   
    This function takes the tilemap and repeatedly blits it on the surface.
    If drawrect->w is not a multiple of tilemap->w (the same with the height),
    the tiles on the right or lower border are cut off appropriately.
  */
DECLSPEC void DrawTile(SDL_Surface* surface, const PG_Rect& ref, const PG_Rect& drawrect, SDL_Surface* tilemap);

#ifndef DOXYGEN_SKIP
// These will disappear (moved to another lib)
DECLSPEC void CreateFilterLUT();
DECLSPEC void PG_SmoothFast(SDL_Surface* src, SDL_Surface* dst);
DECLSPEC void InterpolatePixel(SDL_Surface* src, SDL_Surface* dest);
#endif // DOXYGEN_SKIP

} // namespace PG_Draw

#endif // PG_DRAW_H
