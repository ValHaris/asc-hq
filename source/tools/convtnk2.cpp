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
#include <ctype.h>

#include "..\tpascal.inc"
#include "..\typen.h"
#include "..\basestrm.h"
#include "..\misc.h"
#include "dos.h"
#include "..\sgstream.h"
#include "..\vesa.h"


main(int argc, char *argv[] )
{ 
   t_carefor_containerstream cfcs;
   int quantity=0;   

   struct find_t  fileinfo;
   unsigned rc;        /* return code */ 

   if ( argc < 2 ) {
      printf("syntax: convtank filename\n");
      return 1;
   }

   rc = _dos_findfirst( argv[1], _A_NORMAL, &fileinfo );
          
   tterrainbits bts ( 0, 1 );


   tterrainbits packeis ( 1 << 30, 0 );

   while (rc == 0) {
      quantity++;
      pvehicletype   ft;
      ft = loadvehicletype(fileinfo.name);
      printf (" converting <%15.15s>   ", fileinfo.name);

      if ( toupper ( fileinfo.name[0] ) == 'G' )
        if ( ft->terrainaccess->terrain & cbsnow2 ) {
            ft->terrainaccess->terrain |= bts;
            printf(" G " );
         }

      if ( toupper ( fileinfo.name[0] ) == 'S' || toupper ( fileinfo.name[0] ) == 'W') {
        if ( ft->terrainaccess->terrainnot & cbsnow2 ) {
            ft->terrainaccess->terrainnot |= bts;
            printf(" S2 " );
         }
        if ( ft->terrainaccess->terrainnot & cbsnow1 ) {
            ft->terrainaccess->terrainnot |= packeis;
            ft->terrainaccess->terrainnot |= bts;
            printf(" S1 " );
         }
      }


      // converting .....

//      ft->tank = ft->tank * 14 / 10;
//      ft->movement[3] = ft->movement[3] * (11 + 4 * ( 100 - ft->movement[3]) / 100 ) / 10;
//      for ( int i = 0; i < 8; i++ )
//         ft->movement[i] = ft->movement[i] * 15 / 10;

      // ft->production.material = ft->production.material * 11 / 10;
      // ft->weight = ft->armor / 10 + ft->production.material / 20;
/*
     for ( int i = 0; i < ft->attack.weaponcount; i++ )
         if ( ft->attack.waffe[i].maxdistance < 16 ) {
            ft->attack.waffe[i].minstrength = ft->attack.waffe[i].maxstrength;
         }
*/ 

      printf("\n");


      // saving .....

      {
         tn_file_buf_stream stream ( fileinfo.name, 2 );
         writevehicle ( ft, &stream );
      
      }
      rc = _dos_findnext( &fileinfo ); 
   }

   printf ("\n %i files converted ! \n\n", quantity);
   return 0;
};



