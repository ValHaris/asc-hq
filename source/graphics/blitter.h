/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2003  Martin Bickel  and  Marc Schellenberger

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
 
 typedef SDLmm::Color Color;

 template<int BytesPerPixel> class PixelSize2Type;

 template<> class PixelSize2Type<1> { public: typedef Uint8  PixelType; };
 template<> class PixelSize2Type<2> { public: typedef Uint16 PixelType; };
 template<> class PixelSize2Type<4> { public: typedef Uint32 PixelType; };


   
 class ColorTransform_None {
     public:
        Color transform( Color col) { return col; };
 };

  class ColorTransform_PlayerCol {
        int shift;
     public:
        ColorTransform_PlayerCol() : shift(0) {};
        
        void setPlayer( int player )
        {
           shift = player*8;
        };
        
        Color transform( Color col) 
        { 
           if ( col >= 16 && col < 24 ) 
              return col + shift;
           else  
              return col; 
        };
 };

 class ColorTransform_XLAT {
        const char* table;
     public:
        ColorTransform_XLAT() : table(NULL) {};
        
        void setTranslationTable( const char* translationTable )
        {
           table = translationTable;
        };
        
        Color transform( Color col) 
        { 
           if ( table )
              return table[col];
           else
              return col;   
        };
 };
 
 template<int pixelsize>
 class ColorMerger_PlainOverwrite {
         typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      public:
         void init( const Surface& src ) {};
         void assign ( PixelType src, PixelType* dest ) { *dest = src; };
 };

 
 template<int pixelsize>
 class ColorMerger_AlphaOverwrite {
         typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
         int colorKey;
      public:
         void init( const Surface& src ) 
         {
            if ( src.flags() & SDL_SRCCOLORKEY )
               colorKey = src.GetPixelFormat().colorkey();
            else 
               colorKey = 0xfefefefe;    
         }
         
         void assign ( PixelType src, PixelType* dest ) { if ( src != colorKey) *dest = src; };
 };

  template<int pixelsize>
  class ColorMerger_AlphaShadow {
         typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
         int colorKey;
      public:
         void init( const Surface& src ) 
         {
            if ( src.flags() & SDL_SRCCOLORKEY )
               colorKey = src.GetPixelFormat().colorkey();
            else 
               colorKey = 0xfefefefe;    
         }
        
         void assign ( PixelType src, PixelType* dest ) { if ( src != colorKey) *dest = xlattables.a.dark2[*dest]; };
 };

 template<int pixelsize>
 class ColorMerger_AlphaMixer {
         typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
         int colorKey;
      public:
         void init( const Surface& src ) 
         {
            if ( src.flags() & SDL_SRCCOLORKEY )
               colorKey = src.GetPixelFormat().colorkey();
            else 
               colorKey = 0xfefefefe;    
         }
        
         void assign ( PixelType src, PixelType* dest ) { if ( src != colorKey) *dest = colormixbufchar[*dest + src*256 ]; };
 };

 
   
 template<int pixelsize>
 class SourcePixelSelector_Plain {
       typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
       const PixelType* pointer;
       int pitch;
    public:
       void init ( const Surface& srv )
       {
          pointer = (const PixelType*)(srv.pixels());
          pitch = srv.pitch()/sizeof(PixelType) - srv.w();
       }
       PixelType getPixel(int x, int y) { return *pointer; };
       void nextPixel() { ++pointer; };
       void nextLine() { pointer += pitch; };
 };
 
 template<int pixelsize>
 class SourcePixelSelector_Rotation {
       typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
       const Surface* surface;
       int degrees;
    public:
       SourcePixelSelector_Rotation() : surface(NULL), degrees(0) {};
       
       void init ( const Surface& srv )
       {
          surface = &srv;
       }
       
       void setAngle( int degrees )
       {
          this->degress = degrees;
       }    
       
       PixelType getPixel(int x, int y) 
       {
         SPoint newpos = getPixelRotationLocation( SPoint(x,y), surface->w(),surface->h(), degrees );

         if ( newpos.x >= 0 && newpos.y >= 0 && newpos.x < surface->w() && newpos.y < surface->h() )
            return surface->GetPixel ( newpos );
         else
            return surface->GetPixelFormat().colorkey();  

       };
       
       void nextPixel() {};
       void nextLine() {};
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
    public:
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
       
       PixelType getPixel(int x, int y) 
       {
          if ( useCache && degrees != 0 ) {
             assert ( cacheIndex );
             int index = cacheIndex[tableIndex];
             if ( index >= 0 )
                return pixelStart[index];
             else 
                return surface->GetPixelFormat().colorkey();  
          } else
             return *currentPixel;
       };
       
       void nextPixel() 
       { 
          ++tableIndex;
          ++currentPixel;
       };
       
       void nextLine()
       {
          currentPixel += pitch;
       };
 };

  
 
 template<int BytesPerTargetPixel, class SourceColorTransform, template<int> class ColorMerger, template<int> class SourcePixelSelector>
 class MegaBlitter : public SourceColorTransform, 
                     public ColorMerger<BytesPerTargetPixel>, 
                     public SourcePixelSelector<BytesPerTargetPixel> {
        typedef typename PixelSize2Type<BytesPerTargetPixel>::PixelType PixelType;
    public:
        MegaBlitter() { };

        void blit( const Surface& src, Surface& dst, SPoint dstPos )
        {
           //   SurfaceLock sl1( src );
           SurfaceLock sl2( dst );

           ColorMerger<BytesPerTargetPixel>::init( src );
           SourcePixelSelector<BytesPerTargetPixel>::init( src );

           int h = src.h();
           int w = src.w();

           PixelType* pix = (PixelType*)( dst.pixels() );

           pix += dstPos.y * dst.pitch()/BytesPerTargetPixel + dstPos.x;

           int pitch = dst.pitch()/BytesPerTargetPixel - src.w();

           for ( int y = 0; y < h; ++y ) {
              for ( int x = 0; x < w; ++x ) {
                  ColorMerger<BytesPerTargetPixel>::assign ( SourceColorTransform::transform( SourcePixelSelector<BytesPerTargetPixel>::getPixel(x,y)), pix );
                  SourcePixelSelector<BytesPerTargetPixel>::nextPixel();    
                  ++pix;
               }
               SourcePixelSelector<BytesPerTargetPixel>::nextLine();
               pix += pitch;
           }
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

