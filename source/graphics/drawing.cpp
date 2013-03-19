/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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

#include "drawing.h"

/*
SDLmm::ColorRGB lightenColor( const SDLmm::ColorRGB& color, float factor )
{
   SDLmm::ColorRGB c  = color;
   c.r =  min( max( int( float(c.r) * factor ), 0 ), 255);
   c.g =  min( max( int( float(c.g) * factor ), 0 ), 255);
   c.b =  min( max( int( float(c.b) * factor ), 0 ), 255);
   return c;
};

*/

Uint8 saturationTranslationTable[256][256];

class TableGenerator {
   public:
      TableGenerator() {
         for ( int i = 0; i < 256; ++i )
            for ( int j = 0; j < 256; ++j ) {
               int v = i * j / 16;
               if ( v > 255 )
                  v = 255;
               if ( v < 0 )
                  v = 0;
               saturationTranslationTable[i][j] = v;
            }
      };
} tableGenerator;

#if 0
SDLmm::Color lighten_Color( SDLmm::Color color, Uint8 factor16 )
{
   SDLmm::Color c = saturationTranslationTable[color & 0xff][factor16] |
         (saturationTranslationTable[(color >> 8) & 0xff][factor16] << 8 ) |
         (saturationTranslationTable[(color >> 16) & 0xff][factor16] << 16 ) |
         (color & 0xff000000);
   /*
   SDLmm::Color c = min( max( int( float(color & 0xff) * factor ), 0 ), 255) |
                    (min( max( int( float((color >> 8) & 0xff) * factor ), 0 ), 255) << 8) |
                    (min( max( int( float((color >> 16) & 0xff) * factor ), 0 ), 255) << 16) |
                    (color & 0xff000000);
   */
   return c;
}

SDL_Color lightenColor( const SDL_Color& color, float factor )
{
   SDL_Color c  = color;
   c.r =  min( max( int( float(c.r) * factor ), 0 ), 255);
   c.g =  min( max( int( float(c.g) * factor ), 0 ), 255);
   c.b =  min( max( int( float(c.b) * factor ), 0 ), 255);
   return c;
};


void lighten_Color( SDLmm::Color* color, float factor )
{
   *color = lighten_Color( *color, factor );
}




void PutPixel( Surface& s, const SPoint& pos, Uint32 src )
{
   typedef Uint32 PixelType;
   
   char* c = (char*) s.pixels();
   c += pos.y * s.pitch();
   PixelType* dest = (Uint32*) c;
   dest += pos.x;

   PixelType alpha = src >> 24;   
   
   if ( alpha != PixelType(Surface::transparent)) {
      // copied from SDL

      /*
       * take out the middle component (green), and process
       * the other two in parallel. One multiply less.
       */
      PixelType d = *dest;
      PixelType dalpha = d & 0xff000000;
      PixelType s1 = src & 0xff00ff;
      PixelType d1 = d & 0xff00ff;
      d1 = (d1 + ((s1 - d1) * alpha >> 8)) & 0xff00ff;
      src &= 0xff00;
      d &= 0xff00;
      d = (d + ((src - d) * alpha >> 8)) & 0xff00;
      *dest = d1 | d | dalpha;
   }
}
#endif

