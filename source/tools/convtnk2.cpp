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

#include "..\typen.h"
#include "..\basestrm.h"
#include "..\sgstream.h"
#include "krkr.h"


main(int argc, char *argv[] )
{ 
   opencontainer( "*.con");

   if ( argc < 2 ) {
      printf("syntax: convtank filename\n");
      return 1;
   }

   int quantity=0;   

   int armorfaktor = 10;
   printf ("\n    Armor faktor ( in zehntel )");

   num_ed (armorfaktor, 0, maxint);

   tfindfile ff ( argv[1] );
   char* n = ff.getnextname();
          
   while ( n ) {
      quantity++;
      VehicleType* ft = loadvehicletype( n );
      printf (" converting <%15.15s>  \n ", n);

      ft->armor = ft->armor * armorfaktor / 10;

      tn_file_buf_stream stream ( n, 2 );
      writevehicle ( ft, &stream );
      
      n = ff.getnextname();
   }

   printf ("\n %i files converted ! \n\n", quantity);
   return 0;
};



