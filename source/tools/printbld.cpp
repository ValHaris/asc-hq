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

#include "..\tpascal.inc"
#include "..\typen.h"
#include "..\basestrm.h"
#include "..\misc.h"
#include "..\sgstream.h"
#include "..\loadbi3.h"
#include "../buildingtype.h"
#include "../vehicletype.h"

main(int argc, char *argv[] )
{
   t_carefor_containerstream cfcst;

   try {
      loadpalette();
      loadbi3graphics();

      FILE* fp = fopen ( "building.txt", "wt" );
   
      int quantity=0;   
   
      char* wildcard;
   
      if ( argc == 2 ) {
         wildcard = argv[1];
      } else {
         wildcard =  "*.bld";
      }
             
      fprintf ( fp, " %10.10s %5.5s %6.6s %6.6s %3.3s %7.7s %7.7s %7.7s %7.7s %7.7s %7.7s %7.7s \n\n",
   
      "name", "armor", "p_energy", "p_material", "id", "energy", "material", "fuel", "mx_energy", "mx_material", "mx_fuel", "mx_research" );
   
      tfindfile ff ( wildcard );
      char* fn = ff.getnextname();
      while ( fn ) {
         pbuildingtype   ft;
         ft = loadbuildingtype( fn );
   
         fprintf ( fp, " %10.10s %5d %6d %6d %3d %7d %7d %7d %7d %7d %7d %7d \n",
   
         ft->name, ft->_armor, ft->productionCost.fuel, ft->productionCost.material, ft->id, ft->_tank.energy, ft->_tank.material, ft->_tank.fuel, ft->maxplus.energy, ft->maxplus.material, ft->maxplus.fuel, ft->maxresearchpoints );
   
         fn = ff.getnextname();
      }
   
      fclose ( fp );

   } /* endtry */
   catch ( tfileerror err ) {
      printf("\nfatal error accessing file %s \n", err.filename );
      return 1;
   } /* endcatch */
   catch ( ASCexception ) {
      printf("\na fatal exception occured\n" );
      return 2;
   } /* endcatch */

   return 0;
};



