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

#include <dos.h>
#include <stdio.h>
#include <malloc.h>

#include "..\sgstream.h"

int main(int argc, char *argv[] )
{
   int pos;
   int num = 0;
   if ( argc < 2 ) {
      printf("usage: demount containerfile\n");
      return 2;
   }
   FILE* fp = fopen ( argv[1], "rb" );
   int magic;
   fread ( &magic, 1, 4, fp );
   if ( magic != 'MBCN' ) {
      printf("invalid containerfile\n");
      return 1;
   }
   fread ( &pos, 1, 4, fp );
   fseek ( fp, pos, SEEK_SET );

   fread ( &num, 1, 4, fp );
   tcontainerindex* index = new tcontainerindex[num];
   for ( int i = 0; i < num; i++ ) {
      fread ( &index[i], 1, sizeof ( index[i] ) , fp );
      if ( index[i].name ) {
         int p = -1;
         index[i].name = new char[100];
         do {
            fread ( &index[i].name[++p], 1, 1, fp );
         } while ( index[i].name[p] ); /* enddo */
      }
   }
   fclose ( fp );
   opencontainer ( argv[1] );

   int bufsize = 1000000;
   void* buf = malloc ( bufsize );

   for ( i = 0; i < num; i++ ) {
      try {
         tnfilestream instream ( index[i].name, 1 );
         tn_file_buf_stream outstream ( index[i].name, 2 );
         int size ;
         do {
            size = instream.readdata ( buf, bufsize, 0 );
            outstream.writedata ( buf, size );
         } while ( size == bufsize );
      } /* endtry */
      catch ( tfileerror err) {
         printf( "error writing file %s ", err.filename );
         return 1;
      } /* endcatch */
     
   } /* endfor */

   free ( buf );
   return 0;
}
