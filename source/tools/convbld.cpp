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
#include <dos.h> 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream.h>


#include "..\tpascal.inc"
#include "..\typen.h"
#include "..\sgstream.h"
#include "..\misc.h"
#include "..\vesa.h"


#define cpy(u) bldn-> u = bldo-> u

void main ( void ) {

  find_t  fileinfo;
  unsigned rc;        /* return code */
  tbuildingtype      *bldn;

  int i,j;
  char* nm,*nn;
  char *pi,*pj;

  pj = (char*) malloc ( 800 );
  nm = (char*) malloc ( 200 );

  rc = _dos_findfirst( "*.bld", _A_NORMAL, &fileinfo );
  while( rc == 0 ) { 

       bldn = loadbuildingtype ( fileinfo.name );
/*
       for ( int i = 0; i < cwettertypennum; i++ )
          for ( int j = 0; j < maxbuildingpicnum; j++ )
             for ( int k = 0; k < 4; k++ )
                for ( int l = 0; l < 6; l++ ) {
                   cpy ( w_picture[i][j][k][l] );
                   cpy ( bi_picture[i][j][k][l] );
                }

       cpy ( entry.x );
       cpy ( entry.y );
       cpy ( powerlineconnect.x );
       cpy ( powerlineconnect.y );
       cpy ( pipelineconnect.x );
       cpy ( pipelineconnect.y );

       cpy ( id );
       cpy ( name );

       cpy ( armor );
       cpy ( jamming );
       cpy ( view );
       cpy ( loadcapacity );
       cpy ( loadcapability );
       cpy ( height );
       cpy ( produktionskosten.material );
       cpy ( produktionskosten.sprit );
       cpy ( special );
       cpy ( technologylevel );
       cpy ( researchid );

       cpy ( construction_steps );
       cpy ( maxresearchpoints );

       cpy ( tank );
       cpy ( maxplus );

       cpy ( efficiencyfuel );
       cpy ( efficiencymaterial );

       bldn->terrainaccess.terrain.set ( 0 );
       bldn->terrainaccess.terrain.terrain1 = bldo->terrain;
*/       
       bldn->buildingheight = chfahrend;
       tn_file_buf_stream stream ( fileinfo.name, 2 );
       writebuildingtype ( bldn, &stream );

       printf( "%s written \n",fileinfo.name );

      
      rc = _dos_findnext( &fileinfo );
  }

}
