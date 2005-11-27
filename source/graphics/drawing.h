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

#ifndef drawingH
 #define drawingH

#include <cmath>
 #include <map>
 #include "../libs/loki/static_check.h"
 #include "../libs/sdlmm/src/sdlmm.h"
 #include "surface.h"
 #include "blitter.h"

#include "../misc.h"
 #include "../palette.h"
 #include "../basegfx.h"

template<int BytePerPixel, class ColorMergerUL, class ColorMergerLR>
void rectangle( const Surface& surface, const SPoint& pos, int w, int h, const ColorMergerUL& ul, const ColorMergerLR& lr )
{
   typedef typename PixelSize2Type<BytePerPixel>::PixelType TargetPixelType;

   TargetPixelType* pix = (TargetPixelType*)( surface.pixels() );
   pix += pos.y * surface.pitch()/BytePerPixel + pos.x;

   int pitch = surface.pitch()/BytePerPixel;

   h -= 1;
   w -= 1;

   for ( int x = 0; x <= w; ++x )
      ul.assign ( 1, (pix+x) );

   for ( int y = 0; y <= h; ++y )
      ul.assign ( 1, (pix+y*pitch) );

   for ( int x = 0; x <= w; ++x )
      lr.assign ( 1, (pix+x+h*pitch) );

   for ( int y = 0; y <= h; ++y )
      lr.assign ( 1, (pix+y*pitch+w) );

}

template<int BytePerPixel, class ColorMerger>
void paintFilledRectangle( const Surface& surface, const SPoint& pos, int w, int h, const ColorMerger& ul )
{
   if ( w <= 0 || h <= 0 )
      return;

   typedef typename PixelSize2Type<BytePerPixel>::PixelType TargetPixelType;
   ul.init( surface );

   TargetPixelType* pix = (TargetPixelType*)( surface.pixels() );
   pix += pos.y * surface.pitch()/BytePerPixel + pos.x;

   int pitch = surface.pitch()/BytePerPixel - w;

   h -= 1;
   w -= 1;

   for ( int y = 0; y <= h; ++y ) {
      for ( int x = 0; x <= w; ++x ) {
         ul.assign ( pix );
         ++pix;
      }
      pix += pitch;
   }
}

extern char saturationTranslationTable[256][256];


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


template< int pixelsize,
template<int> class ColorMerger >
class PutPixel: public ColorMerger<pixelsize>
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      Surface& surf;
   public:
      PutPixel( Surface& surface ) : surf ( surface )
      {
         ColorMerger<pixelsize>::init( surface );
      };

      void set
         ( SPoint pos, PixelType src )
      {
         PixelType* pix = (PixelType*)( surf.pixels() );
         pix += pos.y * surf.pitch()/pixelsize + pos.x;

         assign ( src, pix );
      };
};



template<int pixelsize>
      class ColorMerger_AlphaLighter
{}
;

template<>
      class ColorMerger_AlphaLighter<1> : public ColorMerger_AlphaHandler<1>
{
   typedef PixelSize2Type<1>::PixelType PixelType;
   protected:

      void assign ( PixelType src, PixelType* dest )
      {
         // not defined
      };
   public:
      ColorMerger_AlphaLighter ( NullParamType npt = nullParam) {} ;
      ColorMerger_AlphaLighter ( float factor ) {} ;
};


template<>
      class ColorMerger_AlphaLighter<4> : public ColorMerger_AlphaHandler<4>
{
   typedef PixelSize2Type<4>::PixelType PixelType;
   int factor16;
   protected:

      void init( const Surface& srf )
      {
         ColorMerger_AlphaHandler<4>::init(srf);
      };


      void assign ( PixelType src, PixelType* dest )
      {
         if ( isOpaque(src ) ) 
            *dest = lighten_Color( *dest, factor16 );
         
      };
   public:
      ColorMerger_AlphaLighter ( NullParamType npt = nullParam) : factor16( 16 )
      {}
      ;
      ColorMerger_AlphaLighter ( float factor ) : factor16 ( int ( factor * 16 ))
      {}
      ;
};


#endif

