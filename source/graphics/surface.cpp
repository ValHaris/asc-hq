
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

#include "surface.h"
#include "../basegfx.h"

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

void Surface::write ( tnstream& stream ) const
{
/*
   stream.writeWord( 16974 );
   stream.writeInt ( 1 );
   stream.writeInt ( w() );
   stream.writeInt ( h() );

   SDLmm::PixelFormat pf = GetPixelFormat();

   stream.writeChar ( pf.BitsPerPixel() );
	stream.writeChar ( BytesPerPixel() );
	stream.writeChar ( Rloss() );
	stream.writeChar ( Gloss() );
	stream.writeChar ( Bloss() );
	stream.writeChar ( Aloss() );
	stream.writeChar ( Rshift() );
	stream.writeChar ( Gshift() );
	stream.writeChar ( Bshift() );
	stream.writeChar ( Ashift() );
	stream.writeInt ( Rmask() );
	stream.writeInt ( Gmask() );
	stream.writeInt ( Bmask() );
	stream.writeInt ( Amask() );
	stream.writeInt ( colorkey() );
	stream.writeInt ( alpha() );

   if ( BytesPerPixel() == 1 )
   */
}


