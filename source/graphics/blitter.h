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
 #include "../libs/loki/static_check.h"
 #include "../libs/sdlmm/src/sdlmm.h"
 #include "surface.h"

#include "../misc.h"
 #include "../palette.h"
 #include "../basegfx.h"

typedef SDLmm::Color Color;

class NullParamType
   {}
;
extern NullParamType nullParam;


template<int BytesPerPixel>
class PixelSize2Type;

template<>
class PixelSize2Type<1>
{
   public:
      typedef Uint8  PixelType;
};
template<>
class PixelSize2Type<2>
{
   public:
      typedef Uint16 PixelType;
};
template<>
class PixelSize2Type<4>
{
   public:
      typedef Uint32 PixelType;
};



class TargetPixelSelector_All
{
   protected:
      int skipTarget( int x, int y )
      {
         return 0;
      };
      void init( const Surface& srv, const SPoint& pos, int xrange, int yrange )
      {}
      ;
   public:
      TargetPixelSelector_All ( NullParamType npt = nullParam )
      {}
      ;
};

class TargetPixelSelector_Valid
{
      int w,h;
      int xrange;
      SPoint dstPos;
   protected:
      int skipTarget( int x, int y )
      {
         x += dstPos.x;
         y += dstPos.y;

         if ( x >= 0 && y >= 0 && x < w && y < h )
            return 0;
         else
            if ( x < 0 )
               return -x;
            else
               return xrange - x + dstPos.x;
      };
      void init( const Surface& srv, const SPoint& pos, int xrange, int yrange )
      {
         w = srv.w();
         h = srv.h();
         dstPos = pos;
         this->xrange = xrange;
      };
   public:
      TargetPixelSelector_Valid ( NullParamType npt = nullParam ) :w(0xffffff),h(0xffffff),xrange(1)
      {}
      ;
};







template<int pixelsize>
class SourcePixelSelector_Plain
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      const PixelType* pointer;
      int pitch;
      int linelength;
      const Surface* surface;
   protected:
      SourcePixelSelector_Plain() : pointer(NULL), surface(NULL)
      {}
      ;

      void init ( const Surface& srv )
      {
         surface = &srv;
         pointer = (const PixelType*)(srv.pixels());
         linelength = srv.pitch()/sizeof(PixelType);
         pitch = linelength - srv.w();
      };

      PixelType getPixel(int x, int y)
      {
         if ( x >= 0 && y >= 0 && x < surface->w() && y < surface->h() )
            return surface->GetPixel(SPoint(x,y));
         else
            return surface->GetPixelFormat().colorkey();
      };

      PixelType nextPixel()
      {
         return *(pointer++);
      };
      void nextLine()
      {
         pointer += pitch;
      };
      void skipPixels( int pixNum )
      {
         pointer += pixNum;
      };

      int getSourcePixelSkip()
      {
         return 0;
      };
      
      void skipWholeLine()
      {
         pointer += linelength;
      };

      int getWidth()
      {
         return surface->w();
      };
      int getHeight()
      {
         return surface->h();
      };
   public:
      SourcePixelSelector_Plain ( NullParamType npt )
      {}
      ;
};


template<
int BytesPerSourcePixel,
int BytesPerTargetPixel
>
class ColorConverter
{
   public:
      typedef typename PixelSize2Type<BytesPerTargetPixel>::PixelType SourcePixelType;
      typedef typename PixelSize2Type<BytesPerTargetPixel>::PixelType TargetPixelType;
   private:
      SourcePixelType srcColorKey;
      TargetPixelType destColorKey;
   public:
      ColorConverter( const Surface& sourceSurface, Surface& targetSurface )
      {}
      ;
      TargetPixelType convert ( SourcePixelType sp )
      {
         return sp;
      };
};


template<>
class ColorConverter<4,1>
{
      int rshift, gshift, bshift;
   public:
      typedef  PixelSize2Type<4>::PixelType SourcePixelType;
      typedef  PixelSize2Type<1>::PixelType TargetPixelType;
   private:
      SourcePixelType srcColorKey;
      TargetPixelType destColorKey;
      bool srcHasColorKey;
      int srcColorMask;
   public:

      ColorConverter( const Surface& sourceSurface, Surface& targetSurface )
      {
         rshift = sourceSurface.GetPixelFormat().Rshift() + 2;
         gshift = sourceSurface.GetPixelFormat().Gshift() + 2;
         bshift = sourceSurface.GetPixelFormat().Bshift() + 2;
         srcColorMask = ~sourceSurface.GetPixelFormat().Amask();
         srcColorKey = sourceSurface.GetPixelFormat().colorkey() & srcColorMask;

         srcHasColorKey = sourceSurface.flags() & SDL_SRCCOLORKEY;
         if ( targetSurface.flags() & SDL_SRCCOLORKEY )
            destColorKey = targetSurface.GetPixelFormat().colorkey();
         else
            destColorKey = 0xff;


      };

      TargetPixelType convert ( SourcePixelType sp )
      {
         if ( srcHasColorKey && (sp & srcColorMask) == srcColorKey )
            return destColorKey;
         else
            return truecolor2pal_table[ ((sp >> rshift) & 0x3f) + (((sp >> gshift) & 0x3f) << 6) + (((sp >> bshift) & 0x3f) << 12)];
      };

};

template<>
class ColorConverter<1,4>
{
   public:
      typedef  PixelSize2Type<1>::PixelType SourcePixelType;
      typedef  PixelSize2Type<4>::PixelType TargetPixelType;
   private:
      SDL_Color* palette;
      int rshift, gshift, bshift,ashift;
      bool hasColorKey;
      TargetPixelType colorKey;
   public:

      ColorConverter( const Surface& sourceSurface, Surface& targetSurface )
      {
         if ( targetSurface.flags() & SDL_SRCCOLORKEY ) {
            hasColorKey = true;
            colorKey = targetSurface.GetPixelFormat().colorkey();
         } else {
            hasColorKey = false;
            colorKey = 0;
         }

         palette = sourceSurface.GetPixelFormat().palette()->colors;
         rshift = targetSurface.GetPixelFormat().Rshift();
         gshift = targetSurface.GetPixelFormat().Gshift();
         bshift = targetSurface.GetPixelFormat().Bshift();
         if ( targetSurface.GetPixelFormat().Amask() )
            ashift = targetSurface.GetPixelFormat().Ashift();
         else
            ashift = firstBit( ~( targetSurface.GetPixelFormat().Rmask() | targetSurface.GetPixelFormat().Gmask() | targetSurface.GetPixelFormat().Bmask()));

      };

      TargetPixelType convert ( SourcePixelType sp )
      {
         if ( sp == 0xff ) {
            if ( hasColorKey )
               return colorKey;
            else
               return Surface::transparent << ashift;
         } else {
            TargetPixelType a = Surface::opaque;
            a <<= ashift;
            return TargetPixelType(palette[sp].r << rshift) + TargetPixelType(palette[sp].g << gshift) + TargetPixelType(palette[sp].b << bshift) + a;
         }
      };

};



template<
int BytesPerSourcePixel,
int BytesPerTargetPixel,
template<int> class SourceColorTransform, // = ColorTransform_None,
template<int> class ColorMerger, // = ColorMerger_PlainOverwrite,
template<int> class SourcePixelSelector = SourcePixelSelector_Plain,
class TargetPixelSelector = TargetPixelSelector_All,
template<int,int> class MyColorConverter = ColorConverter
>
class MegaBlitter : public SourceColorTransform<BytesPerSourcePixel>,
         public ColorMerger<BytesPerTargetPixel>,
         public SourcePixelSelector<BytesPerSourcePixel>,
         public TargetPixelSelector
{
      typedef typename PixelSize2Type<BytesPerSourcePixel>::PixelType SourcePixelType;
      typedef typename PixelSize2Type<BytesPerTargetPixel>::PixelType TargetPixelType;
   public:
      MegaBlitter()
      { }
      ;
      MegaBlitter( const SourceColorTransform<BytesPerSourcePixel>& scm, const ColorMerger<BytesPerTargetPixel>& cm, const SourcePixelSelector<BytesPerSourcePixel>& sps, const TargetPixelSelector& tps ) :
            SourceColorTransform<BytesPerSourcePixel>( scm ),
            ColorMerger<BytesPerTargetPixel>( cm ),
            SourcePixelSelector<BytesPerSourcePixel>( sps ),
            TargetPixelSelector( tps )
      { }
      ;

      int getWidth()
      {
         return SourcePixelSelector<BytesPerTargetPixel>::getWidth();
      };
      int getHeight()
      {
         return SourcePixelSelector<BytesPerTargetPixel>::getHeight();
      };

      void initSource( const Surface& src )
      {
         SourcePixelSelector<BytesPerSourcePixel>::init( src );
      }


      void blit( const Surface& src, Surface& dst, SPoint dstPos )
      {
         assert( src.GetPixelFormat().BytesPerPixel() == BytesPerSourcePixel );
         assert( dst.GetPixelFormat().BytesPerPixel() == BytesPerTargetPixel );

         SurfaceLock sl( dst );

         ColorMerger<BytesPerTargetPixel>::init( BytesPerSourcePixel == BytesPerTargetPixel ? src : dst );
         SourcePixelSelector<BytesPerSourcePixel>::init( src );

         MyColorConverter<BytesPerSourcePixel,BytesPerTargetPixel> colorConverter( src, dst );

         SourceColorTransform<BytesPerSourcePixel>::init( src );

         int h = SourcePixelSelector<BytesPerSourcePixel>::getHeight();
         int w = SourcePixelSelector<BytesPerSourcePixel>::getWidth();

         TargetPixelSelector::init( dst, dstPos, w, h );
         
         TargetPixelType* pix = (TargetPixelType*)( dst.pixels() );

         pix += dstPos.y * dst.pitch()/BytesPerTargetPixel + dstPos.x;

         int pitch = dst.pitch()/BytesPerTargetPixel - w;

         typedef SourcePixelSelector<BytesPerTargetPixel> SPS;
         for ( int y = 0; y < h; ++y ) {
            for ( int x = 0; x < w; ++x ) {
               int s = TargetPixelSelector::skipTarget(x,y);
               if ( s >= 0 && SourcePixelSelector<BytesPerSourcePixel>::getSourcePixelSkip() > s )
                  s = SourcePixelSelector<BytesPerSourcePixel>::getSourcePixelSkip();
               
               if ( s==0 ) {
                  ColorMerger<BytesPerTargetPixel>::assign ( colorConverter.convert( SourceColorTransform<BytesPerSourcePixel>::transform( SourcePixelSelector<BytesPerSourcePixel>::nextPixel())), pix );
                  ++pix;
               } else {
                  if ( s > 0 ) {
                     SourcePixelSelector<BytesPerSourcePixel>::skipPixels( s );
                     pix += s;
                     x += s - 1 ;
                  } else
                     return;
               }
            }
            SourcePixelSelector<BytesPerSourcePixel>::nextLine();
            pix += pitch;
         }
      };

};


template<
template<int> class SourceColorTransform,
template<int> class ColorMerger,
template<int> class SourcePixelSelector,
class TargetPixelSelector,
typename SourceColorTransformParameter,
typename ColorMergerParameter,
typename SourcePixelSelectorParameter,
typename TargetPixelSelectorParameter
>
void megaBlitter  ( const Surface& src,
                    Surface& dst,
                    const SPoint& pos,
                    const SourceColorTransformParameter& scmp = nullParam,
                    const ColorMergerParameter& cmp  = nullParam,
                    const SourcePixelSelectorParameter spsp  = nullParam,
                    const TargetPixelSelectorParameter tpsp  = nullParam )
{
   switch ( src.GetPixelFormat().BytesPerPixel() ) {
      case 1: {
            switch ( dst.GetPixelFormat().BytesPerPixel() ) {
               case 1: {
                     MegaBlitter<
                     1,1,
                     SourceColorTransform,
                     ColorMerger,
                     SourcePixelSelector,
                     TargetPixelSelector
                     >  blitter (
                        (SourceColorTransform<1>)( scmp ),
                        (ColorMerger<1>)( cmp ),
                        (SourcePixelSelector<1>)( spsp ),
                        TargetPixelSelector(tpsp)
                     );
                     blitter.blit( src, dst, pos );
                  }
                  break;
               case 3:
               case 4: {
                     MegaBlitter<
                     1,4,
                     SourceColorTransform,
                     ColorMerger,
                     SourcePixelSelector,
                     TargetPixelSelector
                     >  blitter (
                        (SourceColorTransform<1>)( scmp ),
                        (ColorMerger<4>)( cmp ),
                        (SourcePixelSelector<1>)( spsp ),
                        TargetPixelSelector(tpsp)
                     );
                     blitter.blit( src, dst, pos );
                  }
                  break;
            };
         }
         break;
      case 3:
      case 4:  {
            switch ( dst.GetPixelFormat().BytesPerPixel() ) {
               case 1: {
                     MegaBlitter<
                     4,1,
                     SourceColorTransform,
                     ColorMerger,
                     SourcePixelSelector,
                     TargetPixelSelector
                     >  blitter (
                        (SourceColorTransform<4>)( scmp ),
                        (ColorMerger<1>)( cmp ),
                        (SourcePixelSelector<4>)( spsp ),
                        TargetPixelSelector(tpsp)
                     );
                     blitter.blit( src, dst, pos );
                  }
                  break;
               case 3:
               case 4: {
                     MegaBlitter<
                     4,4,
                     SourceColorTransform,
                     ColorMerger,
                     SourcePixelSelector,
                     TargetPixelSelector
                     >  blitter (
                        (SourceColorTransform<4>)( scmp ),
                        (ColorMerger<4>)( cmp ),
                        (SourcePixelSelector<4>)( spsp ),
                        TargetPixelSelector(tpsp)
                     );
                     blitter.blit( src, dst, pos );
                  }
                  break;
            };
         }
         break;
   }
}



//////////////////// Color transformations ////////////////////////////////////


template<int pixelsize>
class ColorTransform_None
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
   protected:
      ColorTransform_None()
      {}
      ;
      PixelType transform( PixelType col)
      {
         return col;
      };
      void init( const Surface& src )
      {}
      ;
   public:
      ColorTransform_None ( NullParamType npt )
      {}
      ;
};

template<int pixelsize>
class ColorTransform_PlayerCol
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
   protected:
      ColorTransform_PlayerCol()
      {}
      ;

      PixelType transform( PixelType col)
      {
         return col;
      };

      void init( const Surface& src )
      {}
      ;

   public:
      ColorTransform_PlayerCol( int player )
      {}
      ;
      ColorTransform_PlayerCol ( NullParamType npt )
      {}
      ;

      void setPlayer( int player )
      {}
      ;
};

template<>
class ColorTransform_PlayerCol<1>
{
      int shift;
      typedef PixelSize2Type<1>::PixelType PixelType;

   protected:
      ColorTransform_PlayerCol() : shift(0)
      {}
      ;

      PixelType transform( PixelType col)
      {
         if ( col >= 16 && col < 24 )
            return col + shift;
         else
            return col;
      };

      void init( const Surface& src )
   {}
      ;

   public:
      ColorTransform_PlayerCol( int player )
      {
         setPlayer( player );
      };

      ColorTransform_PlayerCol ( NullParamType npt ) : shift(0)
      {}
      ;

      void setPlayer( int player )
      {
         shift = player*8;
      };
};
/*
   template<int pixelSize>
  class ColorTransform_PlayerCol {
        typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
    protected:
        ColorTransform_PlayerCol() : shift(0) {};
 
        PixelType transform( PixelType col) { return col; };
 
     public:
        ColorTransform_PlayerCol( int player ) {};
        ColorTransform_PlayerCol ( NullParamType npt ) : shift(0) {};
 
        void setPlayer( int player ){};
 };
 
 
  template<>
  class ColorTransform_PlayerCol<4> {
        int color;
        typedef PixelSize2Type<4>::PixelType PixelType;
 
    protected:
        ColorTransform_PlayerCol() : shift(0) {};
 
        PixelType transform( PixelType col)
        {
           if ( col >= 16 && col < 24 )
              return col + shift;
           else
              return col;
        };
 
     public:
        ColorTransform_PlayerCol( int player )
        {
           setPlayer( player );
        };
 
        ColorTransform_PlayerCol ( NullParamType npt ) : shift(0) {};
 
        void setPlayer( int player )
        {
           shift = player*8;
        };
 };
*/


template<int pixelsize>
class ColorTransform_PlayerTrueCol
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      bool lateConversion;
      DI_Color sourceColor;

      PixelType refColor;
      int refr, refg, refb;  // this is NOT RED, GREEN, BLUE, but device dependant. Should be renamed
   protected:
      ColorTransform_PlayerTrueCol() : refColor(0),refr(0),refg(0),refb(0)
      {}
      ;

      PixelType transform( PixelType col)
      {
         int r = (col >> 16) & 0xff;
         int g = (col >> 8) & 0xff;
         int b = (col ) & 0xff;
         if ( g==0 && b==0) {
            return ((refr * r / 256) << 16) + ((refg * r / 256) << 8) + (refb * r / 256) + (col & 0xff000000);
         } else
            if ( r==255 && g==b ) {
               return ((refr + ( 255-refr) * g / 255) << 16) + ((refg + ( 255-refg) * g / 255) << 8) + (refb + ( 255-refb) * g / 255) + (col & 0xff000000);
            } else
               return col;
      };

      void init( const Surface& src )
      {
         if ( lateConversion ) {
            setColor( src.GetPixelFormat().MapRGB( sourceColor ));
            lateConversion = false;
         }
      }

   public:
      ColorTransform_PlayerTrueCol( PixelType color ) : lateConversion( false )
      {
         setColor(color);
      };
      ColorTransform_PlayerTrueCol( DI_Color color ) : lateConversion( false )
      {
         setColor(color);
      };
      ColorTransform_PlayerTrueCol ( NullParamType npt ) : lateConversion( false ) , refColor(0),refr(0),refg(0),refb(0)
      {}
      ;

      void setColor( PixelType color )
      {
         refColor = color;
         refr = (color >> 16) & 0xff;
         refg = (color >> 8) & 0xff;
         refb = (color ) & 0xff;
      };

      void setColor( DI_Color color )
      {
         lateConversion = true;
         sourceColor = color;
      };
};



template<int pixelSize>
class ColorTransform_XLAT
{
      const char* table;

   protected:
      ColorTransform_XLAT() : table(NULL)
      {}
      ;

      void init( const Surface& src )
      {}
      ;

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

      ColorTransform_XLAT ( NullParamType npt ) : table(NULL)
      {}
      ;

      ColorTransform_XLAT ( const char* t ) : table ( t )
      {}
      ;
};


template<int pixelSize>
class ColorTransform_Gray
   { }
;


template<>
class ColorTransform_Gray<4>
{
   protected:
      ColorTransform_Gray()
      {}
      ;

      Color transform( Color col)
      {
         int i = ((col & 0xff) + ((col >> 8) & 0xff) + ((col >> 16) & 0xff)) / 3;
         return i + (i<<8) + (i <<16);
      };

      void init( const Surface& src )
      {}
      ;

   public:
      ColorTransform_Gray ( NullParamType npt )
      {}
      ;
};


//////////////////////// Color Merger ////////////////////////////////////

template<int pixelsize>
class ColorMerger_PlainOverwrite
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
   protected:
      ColorMerger_PlainOverwrite()
      {}
      ;
      void init( const Surface& dst )
      {}
      ;
      void assign ( PixelType src, PixelType* dest )
      {
         *dest = src;
      };
   public:
      ColorMerger_PlainOverwrite ( NullParamType npt )
      {}
      ;
};

template<int pixelsize>
class ColorMerger_ColoredOverwrite
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      DI_Color col;
      mutable PixelType transformedColor;
   public:
      void init( const Surface& dst ) const
      {
         transformedColor =  dst.GetPixelFormat().MapRGB( col );
      };
      void assign ( PixelType src, PixelType* dest ) const
      {
         *dest = src;
      };
      void assign ( PixelType* dest ) const
      {
         *dest = transformedColor;
      };
      ColorMerger_ColoredOverwrite ( DI_Color color ) : col(color)
      {}
      ;
};


template<int pixelsize>
class ColorMerger_AlphaHandler
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      int colorKey;
      int mask;
   protected:
      ColorMerger_AlphaHandler() : mask ( -1 )
      {}
      ;

      void init( const Surface& srf )
      {
         if ( srf.flags() & SDL_SRCCOLORKEY ) {
            colorKey = srf.GetPixelFormat().colorkey();
            if ( pixelsize > 1 )
               mask = srf.GetPixelFormat().Rmask() | srf.GetPixelFormat().Gmask() | srf.GetPixelFormat().Bmask();
         } else
            if ( pixelsize == 1 )
               colorKey = 0xff;
            else
               colorKey = 0xfefefe00;
      };

      bool isOpaque( PixelType src )
      {
         return (src & mask ) != colorKey;
      }
};


template<>
class ColorMerger_AlphaHandler<1>
{
      typedef PixelSize2Type<1>::PixelType PixelType;
      int colorKey;
      int mask;
   protected:
      ColorMerger_AlphaHandler() : mask ( -1 )
      {}
      ;

      void init( const Surface& srf )
      {
         if ( srf.flags() & SDL_SRCCOLORKEY ) {
            colorKey = srf.GetPixelFormat().colorkey();
         } else
            colorKey = 0xff;
      };

      bool isOpaque( PixelType src )
      {
         return (src & mask ) != colorKey;
      }
};

template<>
class ColorMerger_AlphaHandler<4>
{
      typedef PixelSize2Type<4>::PixelType PixelType;
   protected:
      int amask, ashift;
      PixelType colorKey;
      bool hasColorKey;
      int ckmask;
   protected:
      ColorMerger_AlphaHandler() : amask ( -1 ), ashift(0), hasColorKey(false)
      {}
      ;

      void init( const Surface& srf )
      {
         if ( srf.flags() & SDL_SRCCOLORKEY ) {
            hasColorKey = true;
            colorKey = srf.GetPixelFormat().colorkey();
            ckmask = srf.GetPixelFormat().Rmask() | srf.GetPixelFormat().Gmask() | srf.GetPixelFormat().Bmask();
         } else {
            if ( srf.GetPixelFormat().Amask() ) {
               amask = srf.GetPixelFormat().Amask();
               ashift = srf.GetPixelFormat().Ashift();
            } else {
               amask = ~( srf.GetPixelFormat().Rmask() | srf.GetPixelFormat().Gmask() | srf.GetPixelFormat().Bmask() );
               ashift = firstBit(amask);
            }
         }
      };

      bool isOpaque( PixelType src )
      {
         if ( hasColorKey )
            return (src & ckmask) != colorKey;
         else
            return PixelType((src & amask ) >> ashift) >= 128;
      };
};


template<int pixelsize>
class ColorMerger_AlphaOverwrite : public ColorMerger_AlphaHandler<pixelsize>
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
   protected:

      void assign ( PixelType src, PixelType* dest )
      {
         if ( isOpaque(src ) ) {
            *dest = src;
         }
      };

   public:
      ColorMerger_AlphaOverwrite( NullParamType npt = nullParam )
      {}
      ;
};


template<int pixelsize>
class ColorMerger_AlphaMerge : public ColorMerger_AlphaHandler<pixelsize>
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
   protected:

      void assign ( PixelType src, PixelType* dest )
      {
         if ( isOpaque(src ) ) {
            *dest = src;
         }
      };

   public:
      ColorMerger_AlphaMerge( NullParamType npt = nullParam )
      {}
      ;
};

template<>
class ColorMerger_AlphaMerge<4> : public ColorMerger_AlphaHandler<4>
{
      typedef  PixelSize2Type<4>::PixelType PixelType;
   protected:

      void assign ( PixelType src, PixelType* dest )
      {
         if ( hasColorKey ) {
            if ( (src & ckmask) != colorKey )
               *dest = src;
         } else {
            PixelType alpha = PixelType((src & amask ) >> ashift);
            if ( alpha == PixelType(Surface::opaque))
               *dest = src;
            else
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
      };

   public:
      ColorMerger_AlphaMerge( NullParamType npt = nullParam )
      {}
      ;
};




template<int pixelsize>
class ColorMerger_AlphaShadow
   {}
;


template<>
class ColorMerger_AlphaShadow<1> : public ColorMerger_AlphaHandler<1>
{
      typedef PixelSize2Type<1>::PixelType PixelType;
      const char* table;
   protected:

      void assign ( PixelType src, PixelType* dest )
      {
         // STATIC_CHECK ( pixelsize == 1, wrong_pixel_size );
         if ( isOpaque(src ) ) {
            *dest = table[*dest];
         }
      };
   public:
      ColorMerger_AlphaShadow ( NullParamType npt = nullParam) : table ( xlattables.a.dark1 )
      {}
      ;
      ColorMerger_AlphaShadow ( const char* translationTable ) : table ( translationTable )
      {}
      ;
};

template<>
class ColorMerger_AlphaShadow<4> : public ColorMerger_AlphaHandler<4>
{
      typedef PixelSize2Type<4>::PixelType PixelType;
   protected:

      void init( const Surface& srf )
      {
         ColorMerger_AlphaHandler<4>::init(srf);
      };


      void assign ( PixelType src, PixelType* dest )
      {
         if ( isOpaque(src ) ) {
            *dest = ((*dest >> 1) & 0x7f7f7f7f) | (*dest & 0xff000000 );
         }
      };
   public:
      ColorMerger_AlphaShadow ( NullParamType npt = nullParam)
      {}
      ;
      ColorMerger_AlphaShadow ( const char* translationTable )
      {}
      ;
};



template<int pixelsize>
class ColorMerger_AlphaMixer
   {}
;


template<>
class ColorMerger_AlphaMixer<1> : public ColorMerger_AlphaHandler<1>
{
      typedef  PixelSize2Type<1>::PixelType PixelType;
   protected:
      void assign ( PixelType src, PixelType* dest )
      {
         if ( isOpaque(src ) )
            *dest = colormixbufchar[*dest + src*256 ];
      };
   public:
      ColorMerger_AlphaMixer ( NullParamType npt = nullParam )
   {}
      ;
};

template<>
class ColorMerger_AlphaMixer<4> : public ColorMerger_AlphaHandler<4>
{
      typedef  PixelSize2Type<4>::PixelType PixelType;
   protected:
      void assign ( PixelType src, PixelType* dest )
      {
         // STATIC_CHECK ( pixelsize == 1, wrong_pixel_size );
         if ( isOpaque(src ) ) {
            *dest = ((*dest >> 1) & 0x7f7f7f7f) + ((src >> 1) & 0x7f7f7f7f);
         }
      };
   public:
      ColorMerger_AlphaMixer ( NullParamType npt = nullParam )
      {}
      ;
};



extern const int ColorMerger_Alpha_XLAT_Table_shadings[8]; 

template<int pixelsize>
class ColorMerger_Alpha_XLAT_TableShifter : public ColorMerger_AlphaHandler<pixelsize>
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      const char* table;
   protected:
      void assign ( PixelType src, PixelType* dest )
      {
         if ( src == 0 || src >= 8 )
            return;

         *dest = lighten_Color( *dest, ColorMerger_Alpha_XLAT_Table_shadings[src] );
         /*
                     // STATIC_CHECK ( pixelsize == 1, wrong_pixel_size );
                     if ( isOpaque(src ) ) {
                        *dest = table[ *dest + src*256 ];
                     }
                     */
      };

   public:
      void setNeutralTranslationTable( const char* translationTable )
      {
         table = translationTable;
      };

      ColorMerger_Alpha_XLAT_TableShifter( NullParamType npt = nullParam ) : table ( NULL )
      {}
      ;
      ColorMerger_Alpha_XLAT_TableShifter ( const char* translationTable ) : table ( translationTable )
      {}
      ;
};

template<>
class ColorMerger_Alpha_XLAT_TableShifter<1> : public ColorMerger_AlphaHandler<1>
{
      typedef PixelSize2Type<1>::PixelType PixelType;
      const char* table;
   protected:
      void assign ( PixelType src, PixelType* dest )
      {
         // STATIC_CHECK ( pixelsize == 1, wrong_pixel_size );
         if ( isOpaque(src ) ) {
            *dest = table[ *dest + src*256 ];
         }
      };

   public:
      void setNeutralTranslationTable( const char* translationTable )
      {
         table = translationTable;
      };

      ColorMerger_Alpha_XLAT_TableShifter( NullParamType npt = nullParam ) : table ( NULL )
      {}
      ;
      ColorMerger_Alpha_XLAT_TableShifter ( const char* translationTable ) : table ( translationTable )
      {}
      ;
};


template<int pixelsize>
class ColorMerger_Brightness
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      float b;
   public:

      void assign ( PixelType src, PixelType* dest ) const
      {
         PixelType d = 0;
         for ( int i = 0; i < 3; ++i)
            d |= min( max( int( float(((*dest) >> (i*8)) & 0xff) * b ), 0 ), 255) << (i*8);
         d |= (*dest & 0xff000000);

         *dest = d;
      };

      ColorMerger_Brightness( float brightness )
      {
         b = brightness;
      };
};


template<int pixelsize>
class ColorMerger_Set
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      SDLmm::Color col;
   public:

      void assign ( PixelType src, PixelType* dest ) const
      {
         *dest = col;
      };

     
      ColorMerger_Set( SDLmm::Color color )
      {
         col = color;
      };
};


//////////////////// Source Pixel Selector  ///////////////////////////////////



template<int pixelsize, class SourcePixelSelector = SourcePixelSelector_Plain<pixelsize> >
class SourcePixelSelector_Rotation: public SourcePixelSelector
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      int degrees;
      int x,y;
      int w,h;
   protected:

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

      void skipPixels( int pixNum )
      {
         x += pixNum;
      };

      int getSourcePixelSkip()
      {
         return 0;
      };

      void nextLine()
      {
         x = 0;
         y +=1;
      };

   public:
      void setAngle( int degrees )
      {
         this->degrees = degrees;
      };

      SourcePixelSelector_Rotation( NullParamType npt = nullParam )  : degrees(0),x(0),y(0),w(0),h(0)
      {}

      SourcePixelSelector_Rotation( int degreesToRotate )  : degrees(degreesToRotate),x(0),y(0),w(0),h(0)
      {}
};

template<int pixelsize>
class SourcePixelSelector_DirectRotation: public SourcePixelSelector_Rotation<pixelsize>
{
   public:
      SourcePixelSelector_DirectRotation( NullParamType npt = nullParam )
      {}
      SourcePixelSelector_DirectRotation( int degreesToRotate )  : SourcePixelSelector_Rotation<pixelsize>(degreesToRotate)
      {}
}
;



class RotationCache
{
   protected:
      static map<int,int*> cache;
      static int xsize;
      static int ysize;
};

template<int pixelsize>
class SourcePixelSelector_CacheRotation : public RotationCache
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      const Surface* surface;
      int degrees;
      bool useCache;
      const PixelType* pixelStart;
      const PixelType* currentPixel;
      int tableIndex;
      int pitch;
      int* cacheIndex;
      int x,y,w,h;
   protected:

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
         w = srv.w();
         h = srv.h();
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
            if ( degrees == 0 ) {
               ++tableIndex;
               return *(currentPixel++);
            } else {
               SPoint newpos = ::getPixelRotationLocation( SPoint(x++,y), w, h, degrees );
               return getSourcePixel ( newpos.x, newpos.y );
            }
         }
      };

      void skipPixels( int pixNum )
      {
         currentPixel += pixNum;
         tableIndex += pixNum;
         x += pixNum;
      };

      int getSourcePixelSkip()
      {
         return 0;
      };

      void nextLine()
      {
         currentPixel += pitch;
         ++y;
         x = 0;
      };

      int getWidth()
      {
         return surface->w();
      };
      int getHeight()
      {
         return surface->h();
      };

   private:
      PixelType getSourcePixel(int x, int y)
      {
         if ( x >= 0 && y >= 0 && x < surface->w() && y < surface->h() )
            return surface->GetPixel(SPoint(x,y));
         else
            return surface->GetPixelFormat().colorkey();
      };


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

      SourcePixelSelector_CacheRotation( NullParamType npt = nullParam ) : surface(NULL), degrees(0), useCache(false),pixelStart(NULL), currentPixel(NULL),tableIndex(0),pitch(0), cacheIndex(NULL),x(0),y(0),w(0),h(0)
   {}
      ;

      SourcePixelSelector_CacheRotation( const Surface& srv, int degrees ) : surface(NULL), degrees(0), useCache(false),pixelStart(NULL), currentPixel(NULL),tableIndex(0),pitch(0), cacheIndex(NULL),x(0),y(0),w(0),h(0)
      {
         setAngle ( srv, degrees );
      };

      SourcePixelSelector_CacheRotation( pair<const Surface*, int> p ) : surface(NULL), degrees(0), useCache(false),pixelStart(NULL), currentPixel(NULL),tableIndex(0),pitch(0), cacheIndex(NULL),x(0),y(0),w(0),h(0)
      {
         setAngle ( *(p.first), p.second );
      };


};


template<int pixelsize, class SourcePixelSelector = SourcePixelSelector_Plain<pixelsize> >
class SourcePixelSelector_Zoom: public SourcePixelSelector
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      float zoomFactorX;
      float zoomFactorY;
      int x,y;
   protected:

      int getWidth()
      {
         return int( zoomFactorX * SourcePixelSelector::getWidth()  );
      };
      int getHeight()
      {
         return int( zoomFactorY * SourcePixelSelector::getHeight() );
      };

      PixelType getPixel(int x, int y)
      {
         return SourcePixelSelector::getPixel( int(float(x) / zoomFactorX), int(float(y) / zoomFactorY));
      };

      PixelType nextPixel()
      {
         return getPixel(x++, y);
      };

      void skipPixels( int pixNum )
      {
         x += pixNum;
      };

      int getSourcePixelSkip()
      {
         return 0;
      };
      
      void nextLine()
      {
         x= 0;
         ++y;
      };

   public:
      void setZoom( float factor )
      {
         this->zoomFactorX = factor;
         this->zoomFactorY = factor;
      };
      void setZoom( float factorX, float factorY )
      {
         this->zoomFactorX = factorX;
         this->zoomFactorY = factorY;
      };

      float getZoomX()
      {
         return zoomFactorX;
      };
      float getZoomY()
      {
         return zoomFactorY;
      };

      void setSize( int sourceWidth, int sourceHeight, int targetWidth, int targetHeight, bool forceSquare = true )
      {
         float zw = float(targetWidth) / float(sourceWidth);
         float zh = float(targetHeight)/ float(sourceHeight);
         if ( forceSquare )
            setZoom( min ( zw,zh));
         else
            setZoom( zw,zh );
      };

      SourcePixelSelector_Zoom( NullParamType npt = nullParam) : zoomFactorX(1),zoomFactorY(1),x(0),y(0)
   {}
      ;

};

template<int pixelsize>
class SourcePixelSelector_DirectZoom: public SourcePixelSelector_Zoom<pixelsize>
{
   public:
      SourcePixelSelector_DirectZoom( NullParamType npt = nullParam)
      {}
      ;
};


class ZoomCache
{
   protected:
      typedef map<float,int*> ZoomMap;
      static ZoomMap zoomCache;
};


template<int pixelsize, class SourcePixelSelector = SourcePixelSelector_Plain<pixelsize> >
class SourcePixelSelector_CacheZoom : private ZoomCache, public SourcePixelSelector
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      const Surface* surface;
      float zoomFactor;

      int currSrcX;
      int currSrcY;
      int* xp;
      int* yp;
      int offsetx;
      int offsety;
      ZoomMap::iterator cacheit;
   protected:

      int getWidth()
      {
         return int( zoomFactor * SourcePixelSelector::getWidth()  );
      };
      int getHeight()
      {
         return int( zoomFactor * SourcePixelSelector::getHeight() );
      };

      PixelType getPixel(int x, int y)
      {
         return SourcePixelSelector::getPixel( int(float(x) / zoomFactor), int(float(y) / zoomFactor));
      };

      PixelType nextPixel()
      {
         for ( int i = 0; i < *xp; ++i )
            SourcePixelSelector::nextPixel();
         ++xp;

         return SourcePixelSelector::nextPixel();
      };

      void skipPixels( int pixNum )
      {
         int count = pixNum;
         for ( int i = 0; i < pixNum; ++i ) {
            count += *xp;
            ++xp;
         }
         SourcePixelSelector::skipPixels( count );
      };

      int getSourcePixelSkip()
      {
         int s = SourcePixelSelector::getSourcePixelSkip();
         if ( s ) {
            s = int(floor( float(s) * zoomFactor )) -2;  // to prevent rounding errors
            if ( s < 1 )
               return 1;
            else
               return s;
         } else
            return 0;
      };

      
      void nextLine()
      {
         SourcePixelSelector::nextLine();
         for ( int i = 0; i < *yp; ++i )
            SourcePixelSelector::skipWholeLine();

         xp = &cacheit->second[offsetx];
         yp++;
      };


   public:

      
      void setZoomOffset( int x, int y )
      {
         offsetx = x;
         offsety = y;
         /*
         for ( int i = 0; i < y; ++y )
            next
         */
      }
      
      
      void setZoom( float factor )
      {
         this->zoomFactor = factor;
         assert ( factor > 0 );
         assert ( factor <= 1 );

         cacheit = zoomCache.find( factor );
         if ( cacheit == zoomCache.end() ) {
            int size  = max ( SDLmm::Display::GetDisplay().w(), SDLmm::Display::GetDisplay().h() );

            int* buf = new int[size+1];
            buf[0] = 0;

            for ( int i = 0; i < size; ++i ) {
               int a1 =  int( floor( float(i) / zoomFactor ));
               int a2 =  int( floor( float(i+1) / zoomFactor ));

               buf[i+1] = a2 - a1 - 1;
            }

            zoomCache[zoomFactor] = buf;
            cacheit = zoomCache.find( factor );
         }
         xp = &cacheit->second[offsetx];
         yp = &cacheit->second[offsety];
      }

      SourcePixelSelector_CacheZoom( NullParamType npt = nullParam ) : surface(NULL), zoomFactor(1), xp(NULL), yp(NULL), offsetx(0), offsety(0)
      {
         cacheit = zoomCache.end();
      };

      SourcePixelSelector_CacheZoom( float zoom ) : surface(NULL), zoomFactor(1), xp(NULL), yp(NULL), offsetx(0), offsety(0)
      {
         cacheit = zoomCache.end();
         setZoom ( zoom );
      };


};




template<int pixelsize, class SourcePixelSelector = SourcePixelSelector_Plain<pixelsize> >
class SourcePixelSelector_Flip: public SourcePixelSelector
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      bool hflip;
      bool vflip;
      int x,y;
      int w,h;
   protected:

      void init ( const Surface& srv )
      {
         SourcePixelSelector::init(srv);
         w = SourcePixelSelector::getWidth()-1;
         h = SourcePixelSelector::getHeight()-1;
      };



      PixelType getPixel(int x, int y)
      {
         return SourcePixelSelector::getPixel( hflip? w-x : x , vflip ? h-y : y );
      };

      PixelType nextPixel()
      {
         return getPixel(x++, y);
      };

      void skipPixels( int pixNum )
      {
         x += pixNum;
      };
      
      int getSourcePixelSkip()
      {
         return 0;
      };

      void nextLine()
      {
         x= 0;
         ++y;
      };

   public:
      void setFlipping( bool horizontal, bool vertical )
      {
         hflip = horizontal;
         vflip = vertical;
      };

      SourcePixelSelector_Flip( NullParamType npt = nullParam) : hflip(false),vflip(false),x(0),y(0),w(0),h(0)
      {}
      ;

      SourcePixelSelector_Flip( int flip) : hflip(flip&1),vflip(flip&2),x(0),y(0),w(0),h(0)
      {}
      ;

};

template<int pixelsize>
class SourcePixelSelector_DirectFlip : public SourcePixelSelector_Flip<pixelsize>
{
   public:
      SourcePixelSelector_DirectFlip( NullParamType npt = nullParam)
      {}
      ;
      SourcePixelSelector_DirectFlip( int flip) : SourcePixelSelector_Flip<pixelsize>(flip)
      {}
      ;
};

// template<int pixelsize, class SourcePixelSelector = SourcePixelSelector_Plain<pixelsize> > // MSVC7 fails to understand that...
template<int pixelsize>
class SourcePixelSelector_Rectangle: public SourcePixelSelector_Plain<pixelsize>
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      int x,y,x1,y1;
      int w,h;
   protected:
      SourcePixelSelector_Rectangle() : x(0),y(0),x1(0),y1(0),w(0),h(0)
      {}
      ;
public:

      SourcePixelSelector_Rectangle( const SDLmm::SRect& rect ) : x(0),y(0),x1(0),y1(0),w(0),h(0)
      {
         setSrcRectangle( rect );
      }
protected:

      int getWidth()
      {
         return min(w, SourcePixelSelector_Plain<pixelsize>::getWidth() -x1 );
      };
      int getHeight()
      {
         return min(h, SourcePixelSelector_Plain<pixelsize>::getHeight()-y1 );
      };


      PixelType getPixel(int x, int y)
      {
         return SourcePixelSelector_Plain<pixelsize>::getPixel( x + x1, y + y1 );
      };

      PixelType nextPixel()
      {
         return getPixel(x++, y);
      };

      void skipWholeLine()
      {
         ++y;
      };

      void skipPixels( int pixNum )
      {
         x += pixNum;
      };

      int getSourcePixelSkip()
      {
         return 0;
      };
      
      void nextLine()
      {
         x= 0;
         ++y;
      };

   public:
      void setSrcRectangle( SPoint pos, int width, int height )
      {
         x1 = pos.x;
         y1 = pos.y;
         w = width;
         h = height;
      };
      void setSrcRectangle( const SDLmm::SRect& rect )
      {
         x1 = rect.x;
         y1 = rect.y;
         w = rect.w;
         h = rect.h;
      };

};

template<int pixelsize>
class SourcePixelSelector_DirectRectangle
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      int y,x1,y1;
      int w,h;

      const PixelType* pointer;
      const PixelType* startPointer;
      int pitch;
      int linelength;
      const Surface* surface;
   protected:
      SourcePixelSelector_DirectRectangle() : y(0),x1(0),y1(0),w(0),h(0),pointer(NULL), surface(NULL)
      {}
      ;

      int getWidth()
      {
         return min(w, surface->w() -x1 );
      };
      int getHeight()
      {
         return min(h, surface->h()-y1 );
      };


      void init ( const Surface& srv )
      {
         surface = &srv;
         startPointer = pointer = (const PixelType*)(srv.pixels());
         linelength = srv.pitch()/sizeof(PixelType);
         pitch = linelength - w -1  ;
         y = y1;
         pointer += x1 + y1 * linelength;
      };

      PixelType getPixel(int x, int y)
      {
         x += x1;
         y += y1;
         if ( x >= 0 && y >= 0 && x < surface->w() && y < surface->h() )
            return surface->GetPixel(SPoint(x,y));
         else
            return surface->GetPixelFormat().colorkey();
      };


      PixelType nextPixel()
      {
         return *(pointer++);
      };


      void skipWholeLine()
      {
         pointer += linelength;
         ++y;
      };

      void skipPixels( int pixNum )
      {
         pointer += pixNum;
      };

      int getSourcePixelSkip()
      {
         return 0;
      };
      
      void nextLine()
      {
         pointer = startPointer + x1 + (y++) * linelength;
      };

   public:
      void setSrcRectangle( SPoint pos, int width, int height )
      {
         x1 = pos.x;
         y1 = pos.y;
         w = width;
         h = height;
      };
      void setSrcRectangle( const SDLmm::SRect& rect )
      {
         x1 = rect.x;
         y1 = rect.y;
         w = rect.w;
         h = rect.h;
      };

};


class TargetPixelSelector_Rect {
        SDLmm::SRect rect; 
        int x2,y2;
        int xrange;
        int w,h;
        SPoint pos;
     protected:
        int skipTarget( int x, int y ) 
        { 
           int nx = x + pos.x;
           int ny = y + pos.y;
           if ( nx >= rect.x && ny >= rect.y && nx < x2 && ny < y2 )
              return 0; 
           else {
              if ( ny < rect.y )
                 return xrange - x;
              else
                 if ( nx < rect.x )
                    return rect.x - nx;
                 else   
                    if (ny >= y2 )
                       return -1;
                    else
                       if ( nx >= x2 )
                          return xrange - x;
                       else
                          return 1;     
           }      
        };
        void init( const Surface& srv, const SPoint& position, int xrange, int yrange ) 
        {
           w = srv.w();
           h = srv.h();
           pos = position;
           this->xrange = xrange;
        };
        
     public:
        void setTargetRect( const SDL_Rect& r  ) 
        { 
           rect = r; 
           x2 = r.x + r.w; 
           y2 = r.y + r.h;
        };
        
        void setClippingRect( SDL_Surface*  srv  ) 
        { 
           setTargetRect( srv->clip_rect );
        };
        
        TargetPixelSelector_Rect ( NullParamType npt = nullParam ) :w(0xffffff),h(0xffffff) {};   
  };


/*
 
 template<class SourceColorTransform, class ColorMerger, class SourcePixelSelector>
 void megaBlit( const Surface& src, Surface& dst, SPoint dstPos )
 {
    switch ( dst.GetPixelFormat().BytesPerPixel() ) {
       case 1: megaBlit<1,SourceColorTransform,ColorMerger,SourcePixelSelector>( src, dst, dstPos ); break;
       case 2: megaBlit<2,SourceColorTransform,ColorMerger,SourcePixelSelector>( src, dst, dstPos ); break;
       case 4: megaBlit<4,SourceColorTransform,ColorMerger,SourcePixelSelector>( src, dst, dstPos ); break;
    };
 }
 
*/

#endif

