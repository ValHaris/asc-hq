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
#include <i86.h>
#include <graph.h>

#include "tpascal.inc"
#include "typen.h"
#include "basestrm.h"
#include "misc.h"
#include "dos.h"
#include "sgstream.h"
#include "vesa.h"

dacpalette256 pal;


main(int argc, char *argv[] )
{
   FILE* fp = fopen ( "terrain.txt", "wt" );

   int quantity=0;   

   struct find_t  fileinfo;
   unsigned rc;        /* return code */ 

   if ( argc == 2 ) {
      rc = _dos_findfirst( argv[1], _A_NORMAL, &fileinfo );
      fprintf( fp, " %s \n", argv[1] );
   } else {
      rc = _dos_findfirst( "*.trr", _A_NORMAL, &fileinfo );
      fprintf( fp, " *.trr \n");
   }
          
   fprintf ( fp, " %10.10s %15.15s %5.5s %5.5s %5.5s \n\n",

                   "filename", "name", "defensebonus", "attackbonus", "movemalus" );

   while (rc == 0) {
      pterraintype   ft;
      ft = loadbodentyp(fileinfo.name);

      fprintf ( fp, " %10.10s %15.15s %5.5d %5.5d %5.5d \n",

      fileinfo.name, ft->name, ft->weather[0]->defensebonus, ft->weather[0]->attackbonus, ft->weather[0]->movemalus[0] );

      rc = _dos_findnext( &fileinfo ); 
   }

   fclose ( fp );

   return 0;
};



