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

#include <stdio.h>
#include <direct.h> 

int filesize ( char* name )
{ 
   int size = -1;
   {
      DIR *dirp; 
      struct ASC_direct *direntp; 
  
      dirp = opendir( name ); 
      if( dirp != NULL ) { 
        for(;;) { 
          direntp = readdir( dirp ); 
          if ( direntp == NULL ) 
             break; 
          size = direntp -> d_size;
        } 
        closedir( dirp ); 
      } 
    }
   return size;

}                 


void main(int argc, char *argv[])
{
   if ( argc < 2 ) {
      printf(" usage: patchpcx filename \n");
      return;
   }

   for (int i = 1; i < argc; i++) {
      FILE* file = fopen ( argv[i], "rb+" );
      if ( !file ) {
         printf(" Unable to open %s for writing. \n\n", argv[i] );
         return;
      }
      fseek ( file, 124, SEEK_SET );
      int size = filesize ( argv[i] );
      fwrite ( &size, 1, 4, file );
      fclose ( file );
   } /* endfor */

}
