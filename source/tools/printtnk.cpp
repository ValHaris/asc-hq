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
#include <string.h>

#include "..\tpascal.inc"
#include "..\typen.h"
#include "..\basestrm.h"
#include "..\misc.h"
#include "..\sgstream.h"
#include "..\vesa.h"

const int maxvehicles = 1000;

char header[2][500];
char text[maxvehicles][500];
char buf[500];

int cat[100][maxvehicles];

main(int argc, char *argv[] )
{
   t_carefor_containerstream cfcst;

   try {

      #ifdef HEXAGON
      loadpalette();
      #endif

      memset ( text, 0, sizeof(text));
      memset ( header, 0, sizeof(header));
      int num = 0;
   
      int quantity=0;   
   
      char* wildcard;
   
      if ( argc == 2 ) {
         wildcard = argv[1];
         // sprintf( buf, " %s   %c%c%c\n", argv[1], 27,33,5 );
         // strcat ( header[0], buf);
      } else {
         wildcard =  "*.veh";
         // sprintf( buf, " *.tnk %c%c%c\n",27,33,5 );
         // strcat ( header[0], buf );
      }
             
      sprintf ( buf, " %12.12s %5.5s %6.6s %6.6s %3.3s %7.7s ",
               "description", "armor", "p_energy", "p_material", "id", "fuel" );
   
      strcat ( header[1], buf );                                                                       
   
      for ( int i = 0; i < 8; i++ ) {
         sprintf ( buf, "%3.3s ",choehenstufen[i] );
         strcat ( header[1], buf );
      }
   
      sprintf(buf,"|");
      strcat ( header[1], buf );
   
      for ( i = 0; i < 8; i++ ) {
         sprintf ( buf, "%3.3s ",choehenstufen[i] );
         strcat ( header[1], buf );
      }
   
      sprintf(buf, "%4.4s %4.4s %2.2s %3.3s %3.3s %3.3s %2.2s %4.4s %4.4s\n\n", "dist", "f_consumpt", "ascent", "view", "jamming", "weight", "wait", "load", "maxunitweight");
      strcat ( header[1], buf );
   
    
      tfindfile ff ( wildcard );
    
      char* cn = ff.getnextname();
    
      while( cn ) { 
   
         pvehicletype   ft;
         ft = loadvehicletype( cn );
   
         sprintf ( buf, " %12.12s %5d %6d %6d %3d %7d ",
                  ft->description, ft->armor, ft->production.energy, ft->production.material, ft->id, ft->tank );
         strcat ( text[num], buf );
   
         int max = 0;
         for ( int i = 0; i < 8; i++ ) {
            if ( ft->height & ( 1 << i ) )
               sprintf ( buf, "%3d ",ft->movement[i] );
            else
               sprintf ( buf, "%3d ",0 );
            strcat ( text[num], buf );
            if ( ft->movement[i]  > max )
               max = ft->movement[i] ;
         }
         cat[3][num] = max;
   
         sprintf( buf,"|");
         strcat ( text[num], buf );
   
         int maxattack = 0;
         for ( i = 0; i < 8; i++ ) {
            max = 0;
            for ( int j = 0; j < ft->weapons->count; j++)
               if ( ft->weapons->weapon[j].targ & ( 1 << i ) )
                  if ( max < ft->weapons->weapon[j].maxstrength )
                     max = ft->weapons->weapon[j].maxstrength;
               
            sprintf ( buf, "%3d ",max );
            strcat ( text[num], buf );
            if ( max > maxattack )
               maxattack = max;
         }
   
   
         cat[0][num] = ft->jamming;
         cat[1][num] = ft->view;
         cat[2][num] = ft->armor;
         // 3 : movement
         cat[4][num] = ft->production.energy;
         cat[5][num] = ft->production.material;
         cat[6][num] = ft->fuelconsumption;
         cat[7][num] = ft->tank;
         cat[8][num] = maxattack;
         cat[9][num] = ft->weight;
   
         int dist = 0;
         if ( ft->fuelconsumption )
            dist = ft->tank / ft->fuelconsumption;
   
         cat[10][num] = dist;
   
         sprintf( buf, "%4d %4d %2d %3d %3d %3d %2d %4d %4d\n", dist, ft->fuelconsumption, ft->steigung, ft->view, ft->jamming, ft->weight, ft->wait, ft->loadcapacity, ft->maxunitweight );
         strcat ( text[num], buf );
   
         printf("#%d %s\n", num, cn );

         cn = ff.getnextname();
   
         num++;
      }
   
      for ( int c = 0; c < 11; c++ ) {
   
         int srt[maxvehicles];
         for ( i = 0; i < maxvehicles; i++ )
            srt[i] = i;
      
         for ( i = 0; i < num; ) {
            if ( cat[c][srt[i]] < cat[c][srt[i+1]] ) {
                 int t = srt[i];
                 srt[i] = srt[i+1];
                 srt[i+1] = t;
                 if ( i > 0 )
                    i--;
            } else
                 i++;
         }
      
      
         char filename[20];
         strcpy ( filename, "tanks" );
         strcat ( filename, digit[c] );
         strcat ( filename, ".txt" );
      
         FILE* fp = fopen ( filename, "wt" );
      
      
         for ( i = 0; i < 2; i++ )
             fprintf( fp, header[i] );
      
         for ( i = 0; i < num; i++ )
             fprintf( fp, text[srt[i]]);
      
         fclose ( fp );
      }
   
      FILE* fp = fopen ( "tanks.txt", "wt" );
   
   
      for ( i = 0; i < 2; i++ )
          fprintf( fp, header[i] );
   
      for ( i = 0; i < num; i++ )
          fprintf( fp, text[i]);
   
      fclose ( fp );
   
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
};



