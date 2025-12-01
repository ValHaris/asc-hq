/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

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

#include "../loadpcx.h"
#include "../basestrm.h"
#include "../sgstream.h"
#include "../palette.h"
#include "../basegfx.h"



int main(int argc, char *argv[], char *envp[])
{
   if ( argc <= 1 ) {
      printf("\nmissing parameter. filename expected.\n");
      return 1;
   }
      

   initFileIO( "" );
   loadpalette();

   int w,h;
   void* pic;
   {
      tnfilestream s ( argv[1], tnstream::reading );
      s.readrlepict ( &pic, false, &w);
   }

   getpicsize( pic, w, h );
   
   tvirtualdisplay vd ( w+20, h+20,  255 );
   putimage ( 10, 10, pic );
   
   writepcx ( ASCString(argv[1])+".pcx", 10, 10, w+10, h+10, pal );
   
   return 0;

}
