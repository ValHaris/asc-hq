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

int a[256];


int rol2 ( int b )
{
   int c = b << 2;
   c |= (  c & 768 ) >> 8;
   c &= 255;
   return c;
}

int flip1 ( int b )  // linie horiz
{
   int c = b & 68;
   c |= ( b & 1 ) << 4;
   c |= ( b & 2)  << 2;
   c |= ( b & 8)  >> 2;
   c |= ( b & 16) >> 4;
   c |= ( b & 32) << 2;
   c |= ( b & 128)>> 2;
   return c;
}

int flip2 ( int b ) // linie senk
{
   int c = b & 17;
   c |= ( b & 2)  << 6;
   c |= ( b & 4)  << 4;
   c |= ( b & 8 ) << 2;
   c |= ( b & 32) >> 2;
   c |= ( b & 64) >> 4;
   c |= ( b & 128)>> 6;
   return c;
}


int main ( void )
{
  for ( int i = 0; i < 256; i++ )
     a[i] = 0;

  for ( i = 0; i < 256; i++ ) {
     if ( !a[i] ) {
        a[i] = 2;

        int c = i;

        for ( int j = 0; j < 4; j++ ) {
           c = rol2 ( c );
           if ( c != i )
              if ( !a[c] )
                 a[c] = 1;

           int d;
           d = flip1 ( c );
           if ( d != i )
              if ( !a[d] )
                 a[d] = 1;

           d = flip2 ( c );
           if ( d != i )
              if ( !a[d] )
                 a[d] = 1;
        }

     }
  }

  for ( i = 0; i < 256; i++ )
     printf("%4d",a[i]);

  printf("\n\n");
  for ( i = 0; i < 256; i++ ) 
     if ( a[i] == 2 ) {
        printf("%4d  ",i);
        for ( int j = 7; j >=0; j-- )
           printf("%d",!!(i & ( 1 << j )));
        printf("  ");
     }
  printf("\n\n");

  return 0;
}
