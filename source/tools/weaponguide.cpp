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

#include "..\tpascal.inc"
#include "..\typen.h"
#include "..\basestrm.h"
#include "..\misc.h"
#include "..\sgstream.h"
#include "..\loadbi3.h"
#include "../buildingtype.h"
#include "../vehicletype.h"
#include "../errors.h"


int main(int argc, char *argv[] )
{
   // mounting all container files
   opencontainer ( "*.con" );

   try {
                  
      loadpalette();
      loadbi3graphics();

   
      char* wildcard;
   
      if ( argc == 2 ) {
         wildcard = argv[1];
         // if a command line parameter is specified, use it as wildcard
         // for example: weaponguide s*.veh
      } else {
         wildcard =  "*.veh";
         // else use *.veh
      }
      
      FILE* overview = fopen ( "overview.html", "w" );
      // opens a file for writing and assigns the file pointer overview to it
      
      fprintf(overview, "this is the header of the overview file\n"
                        "this is the second line of the header\n" );
      //  \n is the sequence to start a new line

      int num = 0;   
    
      tfindfile ff ( wildcard );
      char* cn = ff.getnextname();
      while( cn ) { 
      	 // now we are cycling through all files that match wildcard

         
         string s = strupr ( cn );  
         // this is a C++ string which is much more powerful than the standard C strings ( char* )
         // cn is made uppercase with strupr
         
         s.replace ( s.find ("VEH"), 3, "html");
         // we are replacing the 3 characters "veh" of the original string by "html"
         
         FILE* detailed = fopen ( s.c_str(), "w" );
         // c_str() converts a c++ string back to a c string which fopen requires
         
         fprintf(detailed, "some header of the detailed file\n");
         
         pvehicletype  ft = loadvehicletype( cn );
         // we are loading the vehicletype with the name in cn
   
         fprintf ( overview, " <A HREF=""%s"">%s</A>\n", s.c_str(), ft->description );
         // we are adding a link to the overview file.
         // to put a singile " into a string we must use double quotes ( "" ), because a single quote is interpreted as the end of the string by C
         // %s tells C to insert a string there. The strings are appended at the end of the command
         // at the first %s the filename in s is inserted, at the second %s the unit variable 'description'
         
                 
         fprintf ( detailed, "Name of unit: %s \n"
                             "Armor of unit: %i \n"
                             "Production cost of unit: %i energy, %i material\n"
                             "ID of unit: %i\n", ft->name, ft->armor, ft->productionCost.energy, ft->productionCost.material, ft->id );
          
         // some details about the unit; %d tells C to insert a decimal number there
         // take a look at the vehicletype class in vehicletype.h for the names of all variables that make a vehicletype
         // be carefuel not to make a , at the end of the first lines, since this would seperate the string in to several independant strings
   
         fprintf ( detailed, "The unit can reach the following levels of height: ");
         // note that we didn't put a newline ( \n ) character at the end of the string
         
         for ( int i = 0; i < 8; i++ ) 
            if ( ft->height & ( 1 << i ) )
               fprintf ( detailed, " %s;", choehenstufen[i] );

         // choehenstufen is a global array that contains the names of the height levels         
   
         fprintf( detailed,"\n");
         // now we are starting a new line

         fclose ( detailed );
         // closing the file

         num++;      
         printf(" %d : processed unit %s \n", num, ft->description );
         // we are writing this not to a file, but the screen
         
         cn = ff.getnextname();
      }
      // getting the name of the next file and closing the loop
        
      fprintf(overview, "some text at the end of the overview file\n");   
         
      fclose ( overview );
   
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



