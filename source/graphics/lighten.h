/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2004  Martin Bickel  and  Marc Schellenberger
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING. If not, write to the
    Free Software Foundation, Inc., 59 Temple Place, Suite 330,
    Boston, MA  02111-1307  USA
*/

#ifndef lightenH
 #define lightenH

#include <cmath>
 #include <map>
 #include "loki/static_check.h"
 #include "../libs/sdlmm/src/sdlmm.h"
 #include "surface.h"

#include "../misc.h"
 #include "../palette.h"
 // #include "../basegfx.h"

extern Uint8 saturationTranslationTable[256][256];


inline SDLmm::Color lighten_Color( SDLmm::Color color, int factor16 )
{
   return saturationTranslationTable[color & 0xff][factor16] |
          (saturationTranslationTable[(color >> 8) & 0xff][factor16] << 8 ) |
          (saturationTranslationTable[(color >> 16) & 0xff][factor16] << 16 ) |
          (color & 0xff000000);
}

inline void lighten_Color( SDLmm::Color* color, int factor16 )
{
   *color = lighten_Color( *color, factor16 );
};

inline SDL_Color lighten_Color( const SDL_Color& color, int factor16 )
{
   SDL_Color c  = color;
   c.r =  saturationTranslationTable[color.r & 0xff][factor16];
   c.g =  saturationTranslationTable[color.g & 0xff][factor16];
   c.b =  saturationTranslationTable[color.b & 0xff][factor16];
   return c;
}

#endif

