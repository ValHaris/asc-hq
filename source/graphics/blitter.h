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

#ifndef blitterH
 #define blitterH

 #include <cmath>
 #include <map>
 #include "../libs/sdlmm/src/sdlmm.h"
 #include "surface.h"

 #include "../palette.h"
 #include "../basegfx.h"

 typedef SDLmm::Color Color;

 template<int BytesPerPixel> class PixelSize2Type;

 template<> class PixelSize2Type<1> { public: typedef Uint8  PixelType; };
 template<> class PixelSize2Type<2> { public: typedef Uint16 PixelType; };
 template<> class PixelSize2Type<4> { public: typedef Uint32 PixelType; };


 template<int pixelsize>
 class SourcePixelSelector_Plain {
       typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
       const PixelType* pointer;
       int pitch;
       const Surface* surface;
    protected:
       SourcePixelSelector_Plain() : pointer(NULL), surface(NULL) {};

       void init ( const Surface& srv )
       {
          surface = &srv; 
          pointer = (const PixelType*)(srv.pixels());
          pitch = srv.pitch()/sizeof(PixelType) - srv.w();
       };

       PixelType getPixel(int x, int y)
       {
          if ( x >= 0 && y >= 0 && x < surface->w() && y < surface->h() )
             return surface->GetPixel(SPoint(x,y));
          else
             return surface->GetPixelFormat().colorkey();
       };

       PixelType nextPixel() { return *(pointer++); };
       void nextLine() { pointer += pitch; };

       int getWidth()  { return surface->w(); };
       int getHeight() { return surface->h(); };
 };



  template<
     int BytesPerTargetPixel,
     class SourceColorTransform,
     template<int> class ColorMerger,
     template<int> class SourcePixelSelector = SourcePixelSelector_Plain
  >
  class MegaBlitter : public SourceColorTransform,
                      public ColorMerger<BytesPerTargetPixel>,
                      public SourcePixelSelector<BytesPerTargetPixel> {
        typedef typename PixelSize2Type<BytesPerTargetPixel>::PixelType PixelType;
    public:
        MegaBlitter() { };

        void blit( const Surface& src, Surface& dst, SPoint dstPos )
        {
           SurfaceLock sl( dst );

           ColorMerger<BytesPerTargetPixel>::init( src );
           SourcePixelSelector<BytesPerTargetPixel>::init( src );

           int h = SourcePixelSelector<BytesPerTargetPixel>::getHeight();
           int w = SourcePixelSelector<BytesPerTargetPixel>::getWidth();

           PixelType* pix = (PixelType*)( dst.pixels() );

           pix += dstPos.y * dst.pitch()/BytesPerTargetPixel + dstPos.x;

           int pitch = dst.pitch()/BytesPerTargetPixel - w;

           for ( int y = 0; y < h; ++y ) {
              for ( int x = 0; x < w; ++x ) {
                  ColorMerger<BytesPerTargetPixel>::assign ( SourceColorTransform::transform( SourcePixelSelector<BytesPerTargetPixel>::nextPixel()), pix );
                  ++pix;
               }
               SourcePixelSelector<BytesPerTargetPixel>::nextLine();
               pix += pitch;
           }
       };

};



//////////////////// Color transformations ////////////////////////////////////


 class ColorTransform_None {
     protected:
        ColorTransform_None(){};
        Color transform( Color col) { return col; };
 };

  class ColorTransform_PlayerCol {
        int shift;

    protected:
        ColorTransform_PlayerCol() : shift(0) {};

        Color transform( Color col)
        {
           if ( col >= 16 && col < 24 )
              return col + shift;
           else
              return col;
        };

     public:
        void setPlayer( int player )
        {
           shift = player*8;
        };
 };

 class ColorTransform_XLAT {
        const char* table;

     protected:
        ColorTransform_XLAT() : table(NULL) {};


        Color transform( Color col)
        {
           if ( table )
              return table[col];
           else
              return col;
        };

     public:
        void setTranslationTable( const char* translationTable )
        {
           table = translationTable;
        };

 };


//////////////////////// Color Merger ////////////////////////////////////

 template<int pixelsize>
 class ColorMerger_PlainOverwrite {
         typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      protected:
         ColorMerger_PlainOverwrite(){};
         void init( const Surface& src ) {};
         void assign ( PixelType src, PixelType* dest ) { *dest = src; };
 };


 template<int pixelsize>
 class ColorMerger_AlphaOverwrite {
         typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
         int colorKey;
      protected:
         ColorMerger_AlphaOverwrite(){};

         void init( const Surface& src )
         {
            if ( src.flags() & SDL_SRCCOLORKEY )
               colorKey = src.GetPixelFormat().colorkey();
            else
               colorKey = 0xfefefefe;
         };

         void assign ( PixelType src, PixelType* dest )
         {
            if ( src != colorKey) *dest = src;
         };
 };

  template<int pixelsize>
  class ColorMerger_AlphaShadow {
         typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
         int colorKey;
      protected:
         ColorMerger_AlphaShadow(){};

         void init( const Surface& src )
         {
            if ( src.flags() & SDL_SRCCOLORKEY )
               colorKey = src.GetPixelFormat().colorkey();
            else
               colorKey = 0xfefefefe;
         };

         void assign ( PixelType src, PixelType* dest )
         {
            if ( src != colorKey)
               *dest = xlattables.a.dark2[*dest];
         };
 };

 template<int pixelsize>
 class ColorMerger_AlphaMixer {
         typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
         int colorKey;
      protected:
         ColorMerger_AlphaMixer(){};

         void init( const Surface& src )
         {
            if ( src.flags() & SDL_SRCCOLORKEY )
               colorKey = src.GetPixelFormat().colorkey();
            else
               colorKey = 0xfefefefe;
         };

         void assign ( PixelType src, PixelType* dest )
         {
            if ( src != colorKey)
               *dest = colormixbufchar[*dest + src*256 ];
         };
 };




//////////////////// Source Pixel Selector  ///////////////////////////////////



 template<int pixelsize, class SourcePixelSelector = SourcePixelSelector_Plain<pixelsize> >
 class SourcePixelSelector_Rotation: public SourcePixelSelector {
       typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
       int degrees;
       int x,y;
       int w,h;
    protected:
       SourcePixelSelector_Rotation() : degrees(0),x(0),y(0),w(0),h(0) {};

       void init ( const Surface& srv )
       {
          SourcePixelSelector::init(srv);
          w = SourcePixelSelector::getWidth();
          h = SourcePixelSelector::getHeight();
       };


       PixelType getPixel(int x, int y)
       {
         SPoint newpos = ::getPixelRotationLocation( SPoint(x,y), w, h, degrees );
         return SourcePixelSelector::getPixel ( newpos.x, newpos.y );
       };

       PixelType nextPixel()
       {
          return getPixel(x++, y);
       };

       void nextLine() { x = 0; y +=1; };

    public:
       void setAngle( int degrees )
       {
          this->degrees = degrees;
       };

 };


 class RotationCache {
    protected:
       static map<int,int*> cache;
       static int xsize;
       static int ysize;
 };

 template<int pixelsize>
 class SourcePixelSelector_CacheRotation : public RotationCache {
       typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
       const Surface* surface;
       int degrees;
       bool useCache;
       const PixelType* pixelStart;
       const PixelType* currentPixel;
       int tableIndex;
       int pitch;
       int* cacheIndex;
    protected:
       SourcePixelSelector_CacheRotation() : surface(NULL), degrees(0), useCache(false),pixelStart(NULL), currentPixel(NULL),tableIndex(0),pitch(0), cacheIndex(NULL) {};

       void init ( const Surface& srv )
       {
          if ( !surface ) {
             surface = &srv;
          } else {
             assert ( surface == &srv );
          }
          tableIndex = 0;
          pixelStart = currentPixel = (PixelType*)surface->pixels();
          pitch = srv.pitch()/sizeof(PixelType) - srv.w();
       }

       PixelType nextPixel()
       {
          if ( useCache && degrees != 0 ) {
             ++currentPixel;

             assert ( cacheIndex );
             int index = cacheIndex[tableIndex++];
             if ( index >= 0 )
                return pixelStart[index];
             else
                return surface->GetPixelFormat().colorkey();
          } else {
             ++tableIndex;
             return *(currentPixel++);
          }

       };

       void nextLine()
       {
          currentPixel += pitch;
       };

       int getWidth()  { return surface->w(); };
       int getHeight() { return surface->h(); };

    public:
       void setAngle( const Surface& srv, int degrees )
       {
          init ( srv );

          if ( xsize == -1 ) {
             xsize = surface->w();
             ysize = surface->h();
          }

          this->degrees = degrees;
          if ( degrees != 0 && surface->w() == xsize && surface->h() == ysize ) {
             useCache = true;
             if ( cache.find( degrees ) == cache.end() ) {
                int* index = new int[surface->w() * surface->h()];
                cache[degrees] = index;
                for ( int y = 0; y < surface->h(); ++y )
                    for ( int x = 0; x < surface->w(); ++x ) {
                        SPoint pnt = getPixelRotationLocation( SPoint(x,y), surface->w(),surface->h(), degrees );
                        if ( pnt.x >= 0 && pnt.y >= 0 && pnt.x < surface->w() && pnt.y < surface->h() )
                           *index++ = pnt.x + pnt.y * surface->pitch()/pixelsize;
                        else
                           *index++ = -1;
                    }
             }
             cacheIndex = cache[degrees];
          }
       }


 };


 template<int pixelsize, class SourcePixelSelector = SourcePixelSelector_Plain<pixelsize> >
 class SourcePixelSelector_Zoom: public SourcePixelSelector {
       typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
       float zoomFactor;
       int x,y;
    protected:
       SourcePixelSelector_Zoom() : zoomFactor(1),x(0),y(0) {};

       int getWidth()  { return int( zoomFactor * SourcePixelSelector::getWidth()  ); };
       int getHeight() { return int( zoomFactor * SourcePixelSelector::getHeight() ); };

       PixelType getPixel(int x, int y)
       {
         return SourcePixelSelector::getPixel( float(x) / zoomFactor, float(y) / zoomFactor);
       };

       PixelType nextPixel()
       {
          return getPixel(x++, y);
       };

       void nextLine() { x= 0; ++y;};

    public:
       void setZoom( float factor )
       {
          this->zoomFactor = factor;
       };

 };




/*
 template<class SourceColorTransform, class ColorMerger, class SourcePixelSelector>
 void megaBlit2( const Surface& src, Surface& dst, SPoint dstPos, const SourceColorTransform& sct, const ColorMerger& cm, const SourcePixelSelector& sps )
 {
    switch ( dst.GetPixelFormat().BytesPerPixel() ) {
       case 1: megaBlit<1>( src, dst, dstPos, sct, cm, sps ); break;
       case 2: megaBlit<2>( src, dst, dstPos, sct, cm, sps ); break;
       case 4: megaBlit<4>( src, dst, dstPos, sct, cm, sps ); break;
    };
 }

*/

#endif

