
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

#include <SDL_image.h>
#include <cmath>
#include "surface.h"
#include "../basegfx.h"
#include "../basestrm.h"
#include "../sdl/SDLStretch.h"

 SDLmm::PixelFormat* Surface::default8bit  = NULL;
 SDLmm::PixelFormat* Surface::default32bit = NULL;


 void writeDefaultPixelFormat ( SDLmm::PixelFormat pf, tnstream& stream )
 {
    stream.writeInt( 1 );
    stream.writeInt(pf.BitsPerPixel()) ;
    stream.writeInt(pf.BytesPerPixel()) ;
    stream.writeInt(pf.Rmask()) ;
    stream.writeInt(pf.Gmask()) ;
    stream.writeInt(pf.Bmask()) ;
    stream.writeInt(pf.Amask()) ;
    stream.writeInt(pf.Rshift()) ;
    stream.writeInt(pf.Gshift()) ;
    stream.writeInt(pf.Bshift()) ;
    stream.writeInt(0) ; // pf.Ashift()
    stream.writeInt(pf.Rloss()) ;
    stream.writeInt(pf.Gloss()) ;
    stream.writeInt(pf.Bloss()) ;
    stream.writeInt(pf.Aloss()) ;
    stream.writeInt(pf.colorkey()) ;
    stream.writeInt(pf.alpha()) ;

 }

 SDL_PixelFormat* readSDLPixelFormat( tnstream& stream )
 {
    SDL_PixelFormat* pf = new SDL_PixelFormat;
    int version = stream.readInt();
    pf->BitsPerPixel = stream.readInt();
    pf->BytesPerPixel = stream.readInt();
    pf->Rmask = stream.readInt();
    pf->Gmask = stream.readInt();
    pf->Bmask = stream.readInt();
    pf->Amask = stream.readInt();
    pf->Rshift = stream.readInt();
    pf->Gshift = stream.readInt();
    pf->Bshift = stream.readInt();
    pf->Ashift = stream.readInt();
    pf->Rloss = stream.readInt();
    pf->Gloss = stream.readInt();
    pf->Bloss = stream.readInt();
    pf->Aloss = stream.readInt();
    pf->colorkey = stream.readInt();
    pf->alpha = stream.readInt();
    return pf;
 }

 void Surface::readDefaultPixelFormat ( tnstream& stream )
 {
     default8bit = new SDLmm::PixelFormat( readSDLPixelFormat( stream ) );
     default32bit = new SDLmm::PixelFormat( readSDLPixelFormat( stream ) );
 }

 void Surface::writeDefaultPixelFormat ( tnstream& stream )
 {
     ::writeDefaultPixelFormat( GetPixelFormat(),stream );
 }



void Surface::read ( tnstream& stream )
{
  trleheader   hd;

  hd.id = stream.readWord();
  hd.size = stream.readWord();
  hd.rle = stream.readChar();
  hd.x = stream.readWord();
  hd.y = stream.readWord();

   if (hd.id == 16973) {
      char *pnter = new char [ hd.size + sizeof(hd) ];
      memcpy( pnter, &hd, sizeof(hd));
      char* q = pnter + sizeof(hd);

      stream.readdata( q, hd.size);  // endian ok ?

      void* uncomp = uncompress_rlepict ( pnter );
      delete[] pnter;

      SetSurface( SDL_CreateRGBSurfaceFrom(uncomp, hd.x, hd.y, 8, hd.x, 0, 0, 0, 0 ));
      SetColorKey( SDL_SRCCOLORKEY, 255 );
   }
   else {
      if (hd.id == 16974) {
         int bitsPerPixel = stream.readChar();
         int bytesPerPixel = stream.readChar();
         int colorkey = stream.readInt();
         if ( bytesPerPixel == 1 ) {
            SDL_Surface* s = SDL_CreateRGBSurface ( SDL_SWSURFACE, hd.x, hd.y, 8, 0xff, 0xff, 0xff, 0xff );
            Uint8* p = (Uint8*)( s->pixels );
            for ( int y = 0; y < hd.y; ++y )
               for ( int x = 0; x< hd.x; ++x )
                  *(p++) = stream.readChar();
/*
            for ( int i = 0; i < 256; ++i) {
               s->format->palette->colors[i].r = default8bit->palette()->colors[i].r;
               s->format->palette->colors[i].g = default8bit->palette()->colors[i].g;
               s->format->palette->colors[i].b = default8bit->palette()->colors[i].b;
            }
*/
            assignDefaultPalette();
            SetSurface( s );

         } else {
            int Rmask = stream.readInt();
            int Gmask = stream.readInt();
            int Bmask = stream.readInt();
            int Amask = stream.readInt();

            SDL_Surface* s = SDL_CreateRGBSurface ( SDL_SWSURFACE, hd.x, hd.y, 32, Rmask, Gmask, Bmask, Amask );
            Uint32* p = (Uint32*)( s->pixels );
            for ( int y = 0; y < hd.y; ++y )
               for ( int x = 0; x< hd.x; ++x )
                  *(p++) = stream.readInt();
            SetSurface( s );
         }
         SetColorKey( SDL_SRCCOLORKEY, colorkey );
      } else {
         int w =  (hd.id + 1) * (hd.size + 1) + 4 ;
         char  *pnter = new char [ w ];
         memcpy ( pnter, &hd, sizeof ( hd ));
         char* q = pnter + sizeof(hd);
         stream.readdata ( q, w - sizeof(hd) ); // endian ok ?

         SetSurface( SDL_CreateRGBSurfaceFrom(q, hd.id+1, hd.size+1, 8, hd.id+1, 0, 0, 0, 0 ));
         SetColorKey( SDL_SRCCOLORKEY, 255 );
      }
   }

}

void Surface::assignDefaultPalette()
{
   if ( me ) {
        SDL_Color spal[256];
        int col;
        for ( int i = 0; i < 256; i++ ) {
           for ( int c = 0; c < 3; c++ ) {
              col = pal[i][c];
              switch ( c ) {
                 case 0: spal[i].r = col * 4; break;
                 case 1: spal[i].g = col * 4; break;
                 case 2: spal[i].b = col * 4; break;
              };
             }
         }
         SDL_SetColors ( me, spal, 0, 256 );
   }
}

SDL_Color* getPalette( Palettes pal )
{
   static SDL_Color* grey = NULL;

   if ( pal == GreyScale ) {
      if ( !grey ) {
         grey = new SDL_Color[256];

   }
}


void Surface::assignDefaultPalette()
{
   if ( me ) {
        SDL_Color spal[256];
        int col;
        for ( int i = 0; i < 256; i++ ) {
           for ( int c = 0; c < 3; c++ ) {
              col = pal[i][c];
              switch ( c ) {
                 case 0: spal[i].r = col * 4; break;
                 case 1: spal[i].g = col * 4; break;
                 case 2: spal[i].b = col * 4; break;
              };
             }
         }
         SDL_SetColors ( me, spal, 0, 256 );
   }
}



void Surface::assignPalette(SDL_Color* colors, int startColor, int colorNum )
{
   if ( me )
      SDL_SetColors ( me, colors, startColor, colorNum );
}



void Surface::write ( tnstream& stream ) const
{

   stream.writeWord( 16974 );
   stream.writeWord ( 1 );
   stream.writeChar ( 0 );
   stream.writeWord ( w() );
   stream.writeWord ( h() );

   SDLmm::PixelFormat pf = GetPixelFormat();

   stream.writeChar ( pf.BitsPerPixel() );
   stream.writeChar ( pf.BytesPerPixel() );
   stream.writeInt ( GetPixelFormat().colorkey());
   if ( pf.BytesPerPixel() == 1 ) {
      for ( int y = 0; y < h(); ++y )
         for ( int x = 0; x < w(); ++x )
            stream.writeChar( GetPixel(x,y));
   } else {
      SDLmm::PixelFormat pf = GetPixelFormat();
      stream.writeInt(pf.Rmask()) ;
      stream.writeInt(pf.Gmask()) ;
      stream.writeInt(pf.Bmask()) ;
      stream.writeInt(pf.Amask()) ;
      for ( int y = 0; y < h(); ++y )
         for ( int x = 0; x < w(); ++x )
            stream.writeInt( GetPixel(x,y));
   }

}

void Surface::strech ( int width, int height )
{
   if ( width != w() || height != h() ) {
      SDL_Surface* s = SDL_CreateRGBSurface ( SDL_SWSURFACE, width, height, 32, 0xff, 0xff00, 0xff0000, 0xff000000 );
      SDL_StretchSurface( me,0,0,w()-1,h()-1,
                          s, 0,0,width-1, height-1);

      SetSurface(s);
   }
}


void Surface::detectColorKey (  )
{
   if ( GetPixelFormat().BitsPerPixel() > 8 )
      SetColorKey( SDL_SRCCOLORKEY, GetPixel(0,0));
}





void applyFieldMask( Surface& s, int x, int y )
{
   static Surface* mask = NULL;
   if ( !mask ) {
      tnfilestream st ( "largehex.pcx", tnstream::reading );
      mask = new Surface ( IMG_LoadPCX_RW ( SDL_RWFromStream( &st )));
      if ( mask->GetPixelFormat().BitsPerPixel() == 8)
         mask->SetColorKey( SDL_SRCCOLORKEY, 0 );
   }
   s.Blit( *mask );
   s.detectColorKey (  );
}


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

class SurfaceLock {
      Surface& surf;
   public:
      SurfaceLock( Surface& s ) : surf(s) { s.Lock(); };
      ~SurfaceLock() { surf.Unlock(); };
};

Surface rotateSurface( Surface& s, double degrees )
{
   const float pi = 3.14159265;

   float angle = degrees / 360 * 2 * pi + pi;

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
         double dx = x - s.w()/2 ;
         double dy = s.h()/2 - y;
         float nx, ny;
         if ( degrees != 0 && degrees != -180 && degrees != 180) {
            float wnk ;
            if ( dx  )
               wnk = atan2 ( dy, dx );
            else
               if ( dy > 0 )
                  wnk = pi/2;
               else
                  wnk = -pi/2;

            wnk -= angle;
            float radius = sqrt ( dx * dx + dy * dy );

            nx = -radius * cos ( wnk );
            ny = radius * sin ( wnk );
         } else
            if ( degrees == 0 ) {
               nx = dx;
               ny = -dy;
            } else
               if ( degrees == 180 || degrees == -180) {
                  nx = -dx;
                  ny = dy;
               }



         if ( nx >= 0 && ny >= 0 && nx < s.w() && ny < s.h() )
            dest.SetPixel( x, y, s.GetPixel ( int(nx), int(ny) ));
      }
   }

   return dest;
}

