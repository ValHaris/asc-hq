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
#include <string.h>
#include <map>

#include "..\tpascal.inc"
#include "..\typen.h"
#include "krkr.h"
#include "..\sgstream.h"
#include "..\misc.h"
#include "..\vesa.h"
#include "..\basestrm.h"


#define vid(wildcard,structure) printf("%s\n", wildcard );  maxid=0; for ( int j = 0; j < 60; j++ ) printf("="); tfindfile ff ( wildcard ); char* c = ff.getnextname(); structure data; while( c ) { { tnfilestream mainstream ( c, 1 ); int c; mainstream.readdata2 ( c ); mainstream.readdata ( &data, sizeof ( data )); } if ( data.id > maxid ) maxid = data.id; int found = 0; for(int i = 0; i <= id.getlength(); i++ ) if ( id[i].id == data.id ) { id[i].count++; found = 1; } if( !found ) { int ps = id.getlength()+1; id[ps].id = data.id; id[ps].count = 1; } c = ff.getnextname(); }

// The above line has > 550 characters !! Be sure that you don't use some primitive editor that truncates lines !

struct IdCount {
       int id;
       int count;
     };

typedef dynamic_array<IdCount> didcounta;


int maxid = 0;

void printresults ( didcounta& id  )
{
   // sort
   for( int a = 0; a < id.getlength(); ) {
      if ( id[a].id > id[a+1].id ) {
         IdCount b = id[a];
         id[a] = id[a+1];
         id[a+1] = b;
         if ( a > 0 )
            a--;
      } else
         a++;
   }


   int start = -1;
   int act   = -1;
   int i;

   printf("\n\n duplicate IDs (have to be corrected) : \n");
   for ( i = 0; i <= id.getlength(); ) 
      if ( id[i].count > 1  && ( act == -1 || id[i].id == act+1 ) ) {
         if ( start < 0 ) 
            start = i;
         act = id[i].id;
         i++;
      } else
         if ( start >= 0 ) {
            if ( i-1 == start )
               printf("  %8d", id[start].id );
            else
               printf(" %4d-%4d", id[start].id, id[i-1].id );
            start = -1;
            act = -1;
         } else
            i++;


   if ( start >= 0 ) {
      if ( i-1 == start )
         printf("  %8d", id[start].id );
      else
         printf(" %4d-%4d", id[start].id, id[i-1].id );
      start = -1;
   }

   start = -1;
   act   = -1;

   printf("\n\n unique IDs : \n");
   for (i = 0; i <= id.getlength(); ) 
      if ( id[i].count == 1  && ( act == -1 || id[i].id == act+1 ) ) {
         if ( start < 0 ) 
            start = i;
         act = id[i].id;
         i++;
      } else
         if ( start >= 0 ) {
            if ( i-1 == start )
               printf("  %8d", id[start].id );
            else
               printf(" %4d-%4d", id[start].id, id[i-1].id );
            start = -1;
            act = -1;
         } else
            i++;

   if ( start >= 0 ) {
      if ( i-1 == start )
         printf("  %8d", id[start].id );
      else
         printf(" %4d-%4d", id[start].id, id[i-1].id );
      start = -1;
   }


   int last = 0;

   printf("\n\n IDs not used : \n");
   for ( i = 0; i <= id.getlength(); i++ ) {
      if ( id[i].id > last+1 )
         if ( id[i].id == last+2 )
            printf("  %8d", last+1 );
         else
            printf(" %4d-%4d", last+1, id[i].id-1 );
      last = id[i].id;
   }

   printf("\n     and all above %d \n\n", maxid);
}


int main ( void )
{ 
   t_carefor_containerstream cfcst;

   try {
     #ifdef HEXAGON
   
      printf("\n\n\n buildings  " );
      {
         didcounta id;
         vid ("*.bld",tbuildingtype);
         printresults( id );
      }
    
      printf("\n\n\n vehicles  " );
      {
         didcounta id;
         vid ("*.veh",tvehicletype);
         printresults( id );
      }
   
      printf("\n\n\n terrain  " );
      {
         didcounta id;
         vid ( "*.trr", tterraintype );
         printresults( id );
      }
        
      printf("\n\n\n technologies  " );
      {
         didcounta id;
         vid ( "*.tec", ttechnology );
         printresults( id );
      }
   
      printf("\n\n\n objects  " );
      {
         didcounta id;
         vid ( "*.obl", tobjecttype );
         printresults( id );
         printf("\n PLEASE DO NOT USE IDs BELOW 100 FOR NEW OBJECTS !!\n");
      }
   
   #endif
   } /* endtry */
   catch ( tfileerror err ) {
      printf("\nfatal error accessing file %s \n", err.filename );
      return 1;
   } /* endcatch */
   catch ( terror ) {
      printf("\na fatal exception occured\n" );
      return 2;
   } /* endcatch */
   
   return 0;
}


