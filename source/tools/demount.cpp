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

                 struct tcontainerindex {
                    char* name;
                    int start;  // das erste Byte
                    int end;    // das letzte Byte
                 };


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
   for ( i = 0; i < num; i++ ) {
      printf("writing %s \n", index[i].name );
      FILE* dest = fopen ( index[i].name, "wb" );
      int size = index[i].end - index[i].start + 1;
      void* p = malloc ( size );
      fseek ( fp, index[i].start, SEEK_SET );
      fread ( p, 1, size, fp );
      fwrite ( p, 1, size, dest );
      fclose ( dest );
      free ( p );
   } /* endfor */

   fclose ( fp );
   return 0;
}
