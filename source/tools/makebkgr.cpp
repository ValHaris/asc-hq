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


#include <malloc.h>
#include <stdio.h>
#include <conio.h>

#include "..\tpascal.inc"
#include "..\vesa.h"
#include "..\loadpcx.h"
#include "..\basestrm.h"
#include "..\misc.h"

#define maxint 1147483648

int* buf;
int* bufptr;

dacpalette256 pal;

main(int argc, char *argv[], char *envp[])
{
   if ( argc <= 1) {
      printf( " Syntax: MAKEBKGR <filename> \n" );
      return 1;
   }

   initsvga ( 0x103 );
   loadpcxxy ( argv[1], 1, 0, 0 );
   void* buf = malloc ( 1000000 );
   getch();
   {
//      tnfilestream stream ( "hexmap.raw", 2 );
      tn_file_buf_stream stream ( "hexmap.raw", 2 );
   
      getimage ( 10, 15, 38, 42, buf );  // Links oben
//      stream.writerlepict  ( buf );
      stream.writedata ( buf, getpicsize2 ( buf ));
   
      getimage ( 294, 15, 357, 42, buf );  // oben
  //    stream.writerlepict  ( buf );
      stream.writedata ( buf, getpicsize2 ( buf ));
   
      getimage ( 487, 15, 545, 66, buf );  // rechts oben
//      stream.writerlepict  ( buf );
      stream.writedata ( buf, getpicsize2 ( buf ));

      getimage ( 10, 43, 39, 90, buf );  // Links 
  //    stream.writerlepict  ( buf );
      stream.writedata ( buf, getpicsize2 ( buf ));
   
   
      getimage ( 525, 67, 545, 115, buf );  // rechts
//      stream.writerlepict  ( buf );
      stream.writedata ( buf, getpicsize2 ( buf ));
   
      getimage ( 10, 475, 69, 526, buf );  // Links unten
  //    stream.writerlepict  ( buf );
      stream.writedata ( buf, getpicsize2 ( buf ));
   
      getimage ( 70, 499, 133, 526, buf );  // unten
//      stream.writerlepict  ( buf );
      stream.writedata ( buf, getpicsize2 ( buf ));
   
      getimage ( 519, 499, 545, 526, buf );  // rechts unten
//      stream.writerlepict  ( buf ); 
      stream.writedata ( buf, getpicsize2 ( buf ));
   }

   getch();
   settextmode ( 3 );
   return 0;
}


