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

#ifndef drawingH
 #define drawingH

#include <cmath>
 #include <map>
 #include "loki/static_check.h"
 #include "../libs/sdlmm/src/sdlmm.h"
 #include "surface.h"
 #include "blitter.h"

#include "../misc.h"
 #include "../palette.h"
 // #include "../basegfx.h"
#include "lighten.h"



template<int BytePerPixel, class ColorMergerUL, class ColorMergerLR>
void rectangle( Surface& surface, const SPoint& pos, int w, int h, const ColorMergerUL& ul, const ColorMergerLR& lr )
{
   SurfaceLock lock( surface );
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

template<int BytePerPixel, class ColorMergerUL, class ColorMergerLR >
void rectangle( Surface& surface, const SPoint& pos, int w, int h, const ColorMergerUL& ul, const ColorMergerLR& lr, const SDLmm::SRect& clip )
{
   SurfaceLock lock( surface );
   typedef typename PixelSize2Type<BytePerPixel>::PixelType TargetPixelType;

   TargetPixelType* pix = (TargetPixelType*)( surface.pixels() );
   pix += pos.y * surface.pitch()/BytePerPixel + pos.x;

   int pitch = surface.pitch()/BytePerPixel;

   h -= 1;
   w -= 1;

   for ( int x = 0; x <= w; ++x )
      if ( clip.Contains( SPoint( pos.x+x, pos.y )))
         ul.assign ( 1, (pix+x) );

   for ( int y = 0; y <= h; ++y )
      if ( clip.Contains( SPoint( pos.x, pos.y + y)))
         ul.assign ( 1, (pix+y*pitch) );

   for ( int x = 0; x <= w; ++x )
      if ( clip.Contains( SPoint( pos.x+x, pos.y + h )))
         lr.assign ( 1, (pix+x+h*pitch) );

   for ( int y = 0; y <= h; ++y )
      if ( clip.Contains( SPoint( pos.x+w, pos.y +y )))
         lr.assign ( 1, (pix+y*pitch+w) );

}





template<int BytePerPixel, class ColorMerger>
void paintFilledRectangle( Surface& surface, const SPoint& pos, int w, int h, const ColorMerger& ul )
{
   if ( w <= 0 || h <= 0 )
      return;

   SurfaceLock lock( surface );

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

extern Uint8 saturationTranslationTable[256][256];

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

      void set( SPoint pos, PixelType src )
      {
         PixelType* pix = (PixelType*)( surf.pixels() );
         pix += pos.y * surf.pitch()/pixelsize + pos.x;

         this->assign ( src, pix );
      };
};

template< int pixelsize, class ColorMerger >
class PutPixel2
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      Surface& surf;
      const ColorMerger& colormerger;
   public:
      PutPixel2( Surface& surface, const ColorMerger& cm ) : surf ( surface ), colormerger(cm)
      {
      };

      void set( SPoint pos, PixelType src )
      {
         PixelType* pix = (PixelType*)( surf.pixels() );
         pix += pos.y * surf.pitch()/pixelsize + pos.x;

         colormerger.assign ( src, pix );
      };
};


template< int pixelsize, class ColorMerger >
void drawLine ( Surface& surface, const ColorMerger& cm, const SPoint& pos, const SPoint& pos2  )
{
   PutPixel2<pixelsize, ColorMerger> pp ( surface, cm );
   int      i, deltax, deltay, numpixels, d, dinc1, dinc2, x, xinc1, xinc2, y, yinc1, yinc2;

   int x1 = pos.x;
   int y1 = pos.y;
   int x2 = pos2.x;
   int y2 = pos2.y;

   /*  calculate deltax and deltay for initialisation  */
    
   deltax = x2 - x1;
   if ( deltax < 0 )
      deltax = -deltax;

   deltay = y2 - y1;
   if ( deltay < 0 )
      deltay = -deltay;

   /*  initialize all vars based on which is the independent variable  */
   if (deltax >= deltay)
   {

      /*  x is independent variable  */
      numpixels = deltax + 1;
      d = (2 * deltay) - deltax;
      dinc1 = deltay << 1;
      dinc2 = (deltay - deltax) << 1;
      xinc1 = 1;
      xinc2 = 1;
      yinc1 = 0;
      yinc2 = 1;
   }
   else
   {

      /*  y is independent variable  */
      numpixels = deltay + 1;
      d = (2 * deltax) - deltay;
      dinc1 = deltax << 1;
      dinc2 = (deltax - deltay) << 1;
      xinc1 = 0;
      xinc2 = 1;
      yinc1 = 1;
      yinc2 = 1;
   }

   /*  make sure x and y move in the right directions  */
   if (x1 > x2)
   {
      xinc1 = -xinc1;
      xinc2 = -xinc2;
   }
   if (y1 > y2)
   {
      yinc1 = -yinc1;
      yinc2 = -yinc2;
   }

   /*  start drawing at <x1, y1>  */
   x = x1;
   y = y1;

   /*  draw the pixels  */
   for (i = 1; i <= numpixels; i++)
   {
      pp.set( SPoint(x, y), 1 );


      if (d < 0)
      {
         d = d + dinc1;
         x = x + xinc1;
         y = y + yinc1;
      }
      else
      {
         d = d + dinc2;
         x = x + xinc2;
         y = y + yinc2;
      }
   }
}



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

