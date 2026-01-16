
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

#include <SDL_image.h>
#include <cmath>
#include "../sdl/sdlstretch.h"
#include "surface.h"
#include "blitter.h"
#include "../basegfx.h"
#include "../basestrm.h"
#include "../misc.h"
#include "../util/messaginghub.h"
#include <iostream>

DI_Color::DI_Color() {
	r = 0;
	g = 0;
	b = 0;
}

DI_Color::DI_Color(const SDL_Color& c) {
	*this = c;
}

DI_Color::DI_Color(Uint32 c) {
	*this = c;
}

DI_Color::DI_Color(Uint8 r, Uint8 g, Uint8 b) {
	*this = (Uint32)((r << 16) | (g << 8) | b);
}

DI_Color& DI_Color::operator=(const SDL_Color& c) {
	r = c.r;
	g = c.g;
	b = c.b;
	
	return *this;
}

DI_Color& DI_Color::operator=(Uint32 c) {
	r = (c >> 16) & 0xFF;
	g = (c >> 8) & 0xFF;
	b = c & 0xFF;
	
	return *this;
}

/*
DI_Color::operator Uint32() const {
	return (r << 16) | (g << 8) | b;
}
*/



 SDLmm::PixelFormat* Surface::default8bit  = NULL;
 SDLmm::PixelFormat* Surface::default32bit = NULL;

void Surface::SetScreen( SDL_Surface* screen )
{
  if ( screen && screen->format->BitsPerPixel == 32 )    
     default32bit = new SDLmm::PixelFormat ( screen->format );
}


Surface Surface::Duplicate() const 
{
   Surface new_surface(CreateSurface(*this));

   megaBlitter<ColorTransform_None, ColorMerger_PlainOverwrite, SourcePixelSelector_Plain, TargetPixelSelector_All>(*this, new_surface, SPoint(0,0), nullParam, nullParam, nullParam, nullParam); 

   new_surface.Blit(*this); 
   return new_surface;
}
 

 
Surface::Surface( SDL_Surface *surface) : SDLmm::Surface ( surface ), pixelDataPointer(NULL)
{
   if ( me )
      convert();
}

void Surface::convert()
{
   if ( GetPixelFormat().BitsPerPixel() == 24 ) {
      Surface s = Surface::createSurface(w(), h(), 32 );
      s.Blit( *this );

      Uint32 colorkey;
      if ( SDL_GetColorKey(me, &colorkey) == 0 )
    	  SDL_SetColorKey(s.getBaseSurface(), SDL_TRUE, colorkey);
      *this = s;   
   }

//   if ( default32bit && GetPixelFormat().BytesPerPixel() == 4 )  {
//      if ( default32bit->Rmask() != GetPixelFormat().Rmask() || default32bit->Gmask() != GetPixelFormat().Gmask() || default32bit->Bmask() != GetPixelFormat().Bmask() ) {
//         SDL_Surface *tmp;
//         Uint8 alpha;
//         SDL_GetSurfaceAlphaMod
//         if ( flags() & SDL_SRCALPHA )
//            tmp  = SDL_DisplayFormatAlpha(me);
//         else
//            tmp  = SDL_DisplayFormat(me);
//
//         if ( !tmp )
//            return;
//
//         SetSurface(tmp);
//      }
//   }

}

/*
SDLmm::ColorRGB  Surface::GetRGB(SDLmm::Color pixel) const
{
   if ( GetPixelFormat().BytesPerPixel() == 1 ) {
      assert( pixel < 256 );
      return SDLmm::ColorRGB( pal[pixel][0] * 4, pal[pixel][1] * 4, pal[pixel][2] * 4 );
   } else
      return GetPixelFormat().GetRGB( pixel );
}

SDLmm::ColorRGBA Surface::GetRGBA(SDLmm::Color pixel) const
{
   if ( GetPixelFormat().BytesPerPixel() == 1 ) {
      assert( pixel < 256 );
      return SDLmm::ColorRGBA( pal[pixel][0] * 4, pal[pixel][1] * 4, pal[pixel][2] * 4, opaque );
   } else
      return GetPixelFormat().GetRGBA( pixel );
}
*/

Surface::Surface(const SDLmm::Surface& other) : SDLmm::Surface ( other ), pixelDataPointer(NULL)
{
   if ( me )
      convert();
}

 
const int surfaceVersion = 2;

void Surface::write ( tnstream& stream ) const
{
   if ( !valid() ) {
      stream.writeWord( 16974 );
      stream.writeWord ( 1 );
      stream.writeUint8 ( 0 );
      stream.writeWord ( 0 );
      stream.writeWord ( 0 );
      return;
   } 
   stream.writeWord( 16974 );
   stream.writeWord ( 1 );
   stream.writeUint8 ( 0 );
   stream.writeWord ( w() );
   stream.writeWord ( h() );
   stream.writeInt( surfaceVersion );

   SDLmm::PixelFormat pf = GetPixelFormat();

   stream.writeUint8 ( pf.BitsPerPixel() );
   stream.writeUint8 ( pf.BytesPerPixel() );
   Uint32 colorkey = 1;
   SDL_GetColorKey(const_cast<SDL_Surface*>(getBaseSurface()), &colorkey);
   stream.writeInt ( colorkey);
   stream.writeInt( flags() );
   if ( pf.BytesPerPixel() == 1 ) {
      for ( int y = 0; y < h(); ++y )
         stream.writedata( ((Uint8*)me->pixels) + y*pitch(), w() );
      /*
      for ( int y = 0; y < h(); ++y )
         for ( int x = 0; x < w(); ++x )
            stream.writeUint8( GetPixel(x,y));
      */      
   } else {
      SDLmm::PixelFormat pf = GetPixelFormat();
      stream.writeInt(pf.Rmask()) ;
      stream.writeInt(pf.Gmask()) ;
      stream.writeInt(pf.Bmask()) ;
      stream.writeInt(pf.Amask()) ;
      for ( int y = 0; y < h(); ++y ) {
         for ( int x = 0; x < w(); ++x )
            stream.writeInt( GetPixel(x,y));
            
      }
   }

}

#define SDL_LEGACY_SRCCOLORKEY	0x00001000	/**< Blit uses a source color key */
#define SDL_LEGACYSRCALPHA	0x00010000	/**< Blit uses source alpha blending */

 
void Surface::read ( tnstream& stream )
{
  trleheader   hd;

  hd.id = stream.readWord();
  hd.size = stream.readWord();
  hd.rle = stream.readUint8();
  hd.x = stream.readWord();
  hd.y = stream.readWord();
  
  if ( hd.x == 0 && hd.y == 0 ) {
      SetSurface( NULL );
      return;  
  }

   if (hd.id == 16973) {
      Uint8 *pnter = new Uint8 [ hd.size + sizeof(hd) ];
      memcpy( pnter, &hd, sizeof(hd));
      Uint8* q = pnter + sizeof(hd);

      stream.readdata( q, hd.size);  // endian ok ?

      Uint8* uncomp = (Uint8*) uncompress_rlepict ( pnter );
      pixelDataPointer = uncomp;
      
      // TODO fix memory leak: uncomp will not be deleted
      delete[] pnter;

      SDL_Surface* surface = SDL_CreateRGBSurface( SDL_SWSURFACE, hd.x+1, hd.y+1, 8,  0, 0, 0, 0 );
      for ( int y = 0; y <= hd.y; ++y ) {
         Uint8* dest = ((Uint8*)surface->pixels) + y * surface->pitch;
         memcpy( dest, uncomp+4 + y * (hd.x+1), hd.x+1);
      } 
      free ( uncomp );
      
      SetSurface( surface );
      if ( !SetColorKey( SDL_TRUE, 255 ))
    	  throw ASCmsgException(SDL_GetError());
      assignDefaultPalette();
   }
   else {
      if (hd.id == 16974) {
         int version = stream.readInt();
         if ( version > surfaceVersion )
            throw tinvalidversion( stream.getLocation(), version, surfaceVersion );
             
         stream.readUint8(); // int bitsPerPixel =
         int bytesPerPixel = stream.readUint8();
         int colorkey = stream.readInt();
         int flags = stream.readInt();
         if ( bytesPerPixel == 1 ) {
            SDL_Surface* s = SDL_CreateRGBSurface ( SDL_SWSURFACE, hd.x, hd.y, 8, 0, 0, 0, 0 );
            if ( !s )
            	throw ASCmsgException(SDL_GetError());
            Uint8* p = (Uint8*)( s->pixels );
            for ( int y = 0; y < hd.y; ++y )
               stream.readdata( p + y*s->pitch, hd.x );
            
               /*for ( int x = 0; x< hd.x; ++x )
                  *(p++) = stream.readUint8();*/
                  
            SetSurface( s );
            assignDefaultPalette();

         } else {
            int Rmask = stream.readInt();
            int Gmask = stream.readInt();
            int Bmask = stream.readInt();
            int Amask = stream.readInt();

            SDL_Surface* s = SDL_CreateRGBSurface ( SDL_SWSURFACE, hd.x, hd.y, 32, Rmask, Gmask, Bmask, Amask );
            Uint32* p = (Uint32*)( s->pixels );
            for ( int y = 0; y < hd.y; ++y ) {
               for ( int x = 0; x< hd.x; ++x )
                  *(p++) = stream.readInt();
            }
            SetSurface( s );
         }
         if ( flags & SDL_LEGACY_SRCCOLORKEY )
            SetColorKey( SDL_TRUE, colorkey );
            
//         if ( flags & SDL_LEGACYSRCALPHA )
//        	 SDL_SetSurfaceAlphaMod( getBaseSurface(), GetPixelFormat().alpha());
//         else
//            SetAlpha ( 0, SDL_ALPHA_OPAQUE);
            
      } else {
         // int w =  (hd.id + 1) * (hd.size + 1) ;
         
         SDL_Surface* s = SDL_CreateRGBSurface( SDL_SWSURFACE, hd.id+1, hd.size+1, 8, 0,0,0,0 );

         for ( int y = 0; y <= hd.size; ++y ) {
            Uint8* pixeldata = (Uint8*)(s->pixels) + y * s->pitch;
            if ( y == 0 ) {
               memcpy ( pixeldata, ((Uint8*)&hd) + 4, sizeof ( hd ) - 4);
               Uint8* q = pixeldata + sizeof(hd) - 4;
               stream.readdata ( q, s->w - sizeof(hd) + 4 ); 
            } else {
               stream.readdata ( pixeldata, s->w ); 
            }
         }
/*

         Uint8* pntr = (Uint8*) asc_malloc( w );
         stream.readdata ( q, w - sizeof(hd) + 4 ); // endian ok ?

         SDL_SWSURFACE
         SDL_Surface* s = SDL_CreateRGBSurfaceFrom(pntr, hd.id+1, hd.size+1, 8, hd.id+1, 0, 0, 0, 0 );
//         s->flags &= ~SDL_PREALLOC;
  */       
         SetSurface( s );
         SetColorKey( SDL_TRUE, 255 );
         assignDefaultPalette();
      }
   }
   convert();
}

void Surface::readImageFile( tnstream& stream )
{
   SetSurface( IMG_Load_RW( SDL_RWFromStream ( &stream ), true ));
   convert();
}


Surface Surface::createSurface( int width, int height, SDLmm::Color color )
{
   Surface s = createSurface ( width, height, 8, color );
   s.SetColorKey( SDL_TRUE, 255 );
   return s;
}

Surface Surface::createSurface( int width, int height, int depth, SDLmm::Color color )
{
   assert ( depth == 32 || depth == 8 );

   SDL_Surface* surf = NULL;   
   if ( depth == 32 && default32bit ) {
      int rmask = default32bit->Rmask();
      int gmask = default32bit->Gmask();
      int bmask = default32bit->Bmask();
      int amask = ~(rmask | gmask | bmask );
      surf = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, depth, rmask, gmask, bmask, amask );
   } else
      surf = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, depth, 0xff, 0xff00, 0xff0000, 0xff000000 );
   
   Surface s ( surf );
   if ( depth == 32 )
      s.Fill(color);
   else {
      s.Fill(color & 0xff );
      s.assignDefaultPalette();
   }
   // s.SetColorKey( SDL_SRCCOLORKEY, 255 );
   return s;
}

void Surface::FillTransparent()
{
   if ( GetPixelFormat().BitsPerPixel() == 32 ) {
      Fill( 0 );
   } else {
	  Uint32 colorkey;
	  if ( SDL_GetColorKey(getBaseSurface(), &colorkey) ==0 ) {
		  Fill( colorkey );
	  } else {
		  Fill( 0 );
	  }
   }      
}


void Surface::assignDefaultPalette()
{
   if ( me && GetPixelFormat().BytesPerPixel() == 1 ) {
        SDL_Color spal[256];
        memset ( spal, 0, 256* sizeof(SDL_Color));
        for ( int i = 0; i < 256; i++ ) {
           spal[i].r = pal[i][0] * 4;
           spal[i].g = pal[i][1] * 4;
           spal[i].b = pal[i][2] * 4;
           spal[i].a = 255;
         }
        if ( me->format && me->format->palette)
           if ( SDL_SetPaletteColors ( me->format->palette, spal, 0, 256 ) < 0)
              throw ASCmsgException(ASCString("Surface::assignDefaultPalette : Error settings Palette") + SDL_GetError());
   }
}


void Surface::assignPalette(SDL_Color* colors, int startColor, int colorNum )
{
   if ( me->format && me->format->palette)
      if ( SDL_SetPaletteColors ( me->format->palette, colors, startColor, colorNum ) < 0 )
          throw ASCmsgException(ASCString("Surface::assignPalette : Error settings Palette") + SDL_GetError());
}


void Surface::strech ( int width, int height )
{
   if ( width != w() || height != h() ) {
      SDL_Surface* s;
      if( GetPixelFormat().BytesPerPixel() == 1 )
         s = SDL_CreateRGBSurface ( SDL_SWSURFACE, width, height, 8, 0,0,0,0 );
      else
         s = SDL_CreateRGBSurface ( SDL_SWSURFACE, width, height, 32, 0xff, 0xff00, 0xff0000, 0xff000000 );
         
      SDL_StretchSurface( me,0,0,w()-1,h()-1, s, 0,0,width-1, height-1);

      SetSurface(s);
      if( GetPixelFormat().BytesPerPixel() == 1 ) {
         assignDefaultPalette();
         detectColorKey();
      }
   }
}

bool Surface::hasAlpha() 
{
   if ( GetPixelFormat().BitsPerPixel() > 8 ) {
      for ( int y = 0; y < h(); ++y )
         for ( int x = 0; x < w(); ++x )
            if ( (GetPixel(x,y) >> GetPixelFormat().Ashift())  != SDL_ALPHA_OPAQUE ) {
               // GetSurface()->flags |= SDL_SRCALPHA;
               return true;
            }

      // GetSurface()->flags &= ~SDL_SRCALPHA; 
   }
   return false;
}



void Surface::detectColorKey ( bool RLE )
{

   // detect if image has per pixel alpha - don't use ColorKey then
   if ( GetPixelFormat().BitsPerPixel() > 8 ) 
      if ( hasAlpha() ) 
         return;

   SetAlpha ( 0, 0 );
      
   if ( GetPixelFormat().BitsPerPixel() > 8 ) {
      SetColorKey( SDL_TRUE, GetPixel(0,0) & ( GetPixelFormat().Rmask() | GetPixelFormat().Gmask() | GetPixelFormat().Bmask()));
   } else
      SetColorKey( SDL_TRUE, GetPixel(0,0));
}


bool Surface::isTransparent( SDLmm::Color col ) const
{
   Uint32 colorkey;
   if ( SDL_GetColorKey(const_cast<SDL_Surface*>(getBaseSurface()), &colorkey) == 0 )
      return (col & (GetPixelFormat().Rmask() | GetPixelFormat().Gmask() | GetPixelFormat().Bmask())) == colorkey;
   else {
      if ( GetPixelFormat().BitsPerPixel() == 8 )
         return false;
      else {
         if ( ((col & GetPixelFormat().Amask()) >> GetPixelFormat().Ashift()) < opaque/2)
            return true;
         else
            return false;
      }
   }
}

Surface::~Surface()
{/*
   if ( pixelDataPointer ) {
      asc_free( pixelDataPointer );
      pixelDataPointer = NULL;
   }
   */
}




Surface& getFieldMask()
{
   static Surface* mask8 = NULL;
   if ( !mask8 ) {
      try {
         tnfilestream st ( "largehex.pcx", tnstream::reading );
         RWOPS_Handler rwo ( SDL_RWFromStream( &st ) );
         mask8 = new Surface ( IMG_LoadPCX_RW ( rwo.Get() ));
         rwo.Close();

         assert ( mask8->GetPixelFormat().BitsPerPixel() == 8);
         mask8->SetColorKey( SDL_TRUE, 0 );
      }
      catch ( tfileerror err ) {
         fatalError( "could not access " + err.getFileName() );
      }
      
   }
   return *mask8;

}


template<int pixelsize>
class ColorMerger_MaskApply : public ColorMerger_AlphaHandler<pixelsize>
{
      int alphamask;
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
   protected:

      void init( const Surface& srf )
      {
         alphamask = ~(srf.GetPixelFormat().Amask());
         ColorMerger_AlphaHandler<pixelsize>::init(srf);
      }


      void assign ( PixelType src, PixelType* dest )
      {
         if ( !this->isOpaque(src ) )
            *dest &= alphamask;
      };

   public:
      ColorMerger_MaskApply( NullParamType npt = nullParam )
      {}
      ;
};


void applyFieldMask( Surface& s, int x, int y, bool detecColorKey )
{
   if ( s.GetPixelFormat().BitsPerPixel() == 8 ) {
      // we don't want any transformations from one palette to another; we just assume that all 8-Bit images use the same colorspace
      MegaBlitter<1,1,ColorTransform_None,ColorMerger_AlphaOverwrite> blitter;
      blitter.blit( getFieldMask(), s, SPoint(0,0)  );
      s.detectColorKey (  );
   } else {
      if ( detecColorKey ) {
         s.Blit( getFieldMask() );
         s.detectColorKey (  );
      } else {
         MegaBlitter<1,4,ColorTransform_None,ColorMerger_MaskApply> blitter;
         blitter.blit( getFieldMask(), s, SPoint(0,0)  );
      }
   }
}

void applyLegacyFieldMask( Surface& s, int x, int y, bool detectColorKey )
{
   static Surface* mask32 = NULL;
   if ( !mask32 ) {
      Surface& mask8 = getFieldMask();
      
      mask32 = new Surface ( Surface::createSurface( mask8.w(), mask8.h(), 32 ));
      for ( int y = 0; y < mask8.h(); ++y ) {
         Uint8* s = ((Uint8*) mask8.pixels()) + mask8.pitch() * y;
         Uint32* d = (Uint32*) ((Uint8*)(mask32->pixels()) + mask32->pitch() * y);
         for ( int x = 0; x < mask8.w(); ++x, ++s, ++d)
            if ( *s == 0 )
               *d = 0;   
            else
               *d = 0xfffefefe;
      }
              
      mask32->SetColorKey( SDL_TRUE, 0 );
      
   }
   if ( s.GetPixelFormat().BitsPerPixel() == 8 ) {
      // we don't want any transformations from one palette to another; we just assume that all 8-Bit images use the same colorspace
      MegaBlitter<1,1,ColorTransform_None,ColorMerger_AlphaOverwrite> blitter;
      blitter.blit( getFieldMask(), s, SPoint(0,0)  );
      s.detectColorKey (  );
   } else {
      if ( !detectColorKey || s.hasAlpha() ) {
         MegaBlitter<1,4,ColorTransform_None,ColorMerger_MaskApply> blitter;
         blitter.blit( getFieldMask(), s, SPoint(0,0)  );
      } else {
         s.Blit( *mask32 );
         s.detectColorKey (  );
      }
   }
}

/*
void colorShift ( Surface& s, int startcolor, int colNum, int shift )
{
   if ( s.GetPixelFormat().BitsPerPixel() != 8)
      fatalError ( "colorShift can only be done with 8 Bit surfaces");

   SDL_Color spal[256];
   int col;
   for ( int i = 0; i < 256; i++ ) {
      int src;
      if ( i >= startcolor && i < startcolor + colNum )
         src = i + shift;
      else
         src = i;
      for ( int c = 0; c < 3; c++ ) {
         col = pal[src][c];
         switch ( c ) {
            case 0: spal[i].r = col * 4; break;
            case 1: spal[i].g = col * 4; break;
            case 2: spal[i].b = col * 4; break;
         };
      }
   }
   s.assignPalette( spal, 0, 256 );
}
*/

Surface rotateSurface( Surface& s, int degrees )
{
   SurfaceLock sl1 ( s );

   Surface dest = s.Duplicate();

   SurfaceLock sl2( dest );

   if ( s.GetPixelFormat().BitsPerPixel() == 8 )
      dest.Fill ( 255 );
   else {
      dest.Fill(0xfefefe);
   }
   dest.detectColorKey();

   for ( int y = 0; y < s.h(); y++ ) {
      for ( int x = 0; x < s.w(); x++ ) {
         SPoint newpos = getPixelRotationLocation( SPoint(x,y), s.w(),s.h(), degrees );

         if ( newpos.x >= 0 && newpos.y >= 0 && newpos.x < s.w() && newpos.y < s.h() )
            dest.SetPixel( x, y, s.GetPixel ( newpos ));
      }
   }

   return dest;
}


void* Surface::toBGI() const
{
   void* p = malloc( imagesize(1,1,w(),h()) );
   Uint8* c = (Uint8*) p;
   Uint16* ww = (Uint16*) p;
   ww[0] = w()-1;
   ww[1] = h()-1;
   c += 4;
   for ( int y = 0; y < h(); ++y )
      for ( int x = 0; x < w(); ++x )
          *c++ = GetPixel(x,y);
   return p;
}

int Surface::getMemoryFootprint() const
{
   int size = sizeof(*this);
   
   const SDL_Surface* s = getBaseSurface();
   if ( s ) {
      size += sizeof( SDL_Surface );
      if ( s->format ) {
         size += sizeof( SDL_PixelFormat );
         if ( s->format->palette )
            size += sizeof ( SDL_Palette ) + s->format->palette->ncolors * sizeof(SDL_Color);
      }
      size += s->h * s->pitch;
   }
   return size;
}


void Surface::ColorKey2AlphaChannel() 
{
   Lock();
   Uint32 colorkey;
   if ( SDL_GetColorKey(getBaseSurface(), &colorkey) != 0 ) {
	   Unlock();
	   return;
   }
   for ( int y = 0; y < h(); ++y ) {
      Uint8* cp = (Uint8*) pixels();
      cp += y * pitch();
      int* ip = (int*) cp;
      for ( int x = 0; x < w(); ++x, ++ip ) 
         if ( (*ip & ~(0xff << GetPixelFormat().Ashift())) == colorkey)
            *ip &= ~(Surface::transparent << GetPixelFormat().Ashift());
   }
   SDL_SetColorKey(getBaseSurface(), SDL_FALSE, 0);
   Unlock();
}

