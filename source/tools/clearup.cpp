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

//#include <dos.h>
#include <stdio.h>
#include <direct.h> 
#include <io.h>
#include <ctype.h>
#include <conio.h>

typedef long int int;

                 struct tcontainerindex {
                    char* name;
                    int start;  // das erste Byte
                    int end;    // das letzte Byte
                 };



int exist ( const char* s )
{
   return !access ( s, F_OK );
};


int filetime ( char* devicename )
{
   int time = -1;
   {
      DIR *dirp; 
      struct dirent *direntp; 
  
      dirp = opendir( devicename ); 
      if( dirp != NULL ) { 
        for(;;) { 
          direntp = readdir( dirp ); 
          if ( direntp == NULL ) 
             break; 
          time =  ( direntp ->d_date << 16) + direntp ->d_time;
        } 
        closedir( dirp ); 
      } 
   }
   return time;
}


char* containerfile = "MAIN.DTA";

int main ( void )
{
   if ( !exist ( containerfile ))
      printf(" %s not found. exiting.\n", containerfile );

   int maintime = filetime ( containerfile );

   int pos;
   int num = 0;
   FILE* fp = fopen ( containerfile, "rb" );
   fread ( &pos, 1, 4, fp );
   fseek ( fp, pos, SEEK_SET );

   fread ( &num, 1, 4, fp );
   tcontainerindex* index = new tcontainerindex[num];
   if ( !index ) {
      printf(" out of memory \n");
      return 1;
   }

   for ( int i = 0; i < num; i++ ) {
      fread ( &index[i], 1, sizeof ( index[i] ) , fp );
      if ( index[i].name ) {
         int p = -1;
         index[i].name = new char[100];
         if ( !index[i].name ) {
            printf(" out of memory \n");
            return 1;
         }
         do {
            fread ( &index[i].name[++p], 1, 1, fp );
         } while ( index[i].name[p] ); /* enddo */
      }
   }

   fclose ( fp );

   int u = 0;


   for ( i = 0; i < num; i++ ) 
      if ( exist ( index[i].name )) {
         if ( filetime ( index[i].name ) <= maintime ) {
            printf( " %s is older than %s ; ", index[i].name, containerfile );
            if ( u == 0 ) {
               printf(" \ndelete ( Yes / No / All ) ? ");
               fflush ( stdout );
               int k = toupper ( getch () );
               if ( k == 'A' )
                  u = 10;
               else
                  if ( k == 'Y' )
                     u = 1;
            }
            if ( u ) {
               if ( remove ( index[i].name ) == 0 )
                  printf("   %s has been deleted \n", index[i].name );
               else
                 printf("   %s has NOT been deleted \n", index[i].name );
             }
   
            if ( u < 10 )
               u = 0;
         } else
            printf( " %s is newer than %s ; \n", index[i].name, containerfile );
      }

   return 0;
}

