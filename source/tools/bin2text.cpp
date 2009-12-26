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

#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <map>

#include "../tpascal.inc"
#include "../typen.h"
#include "../sgstream.h"
#include "../misc.h"
#include "../basestrm.h"
#include "../basegfx.h"
#include "../buildingtype.h"
#include "../vehicletype.h"
#include "../graphicset.h"
#include "../graphicselector.h"
#include "../strtmesg.h"
#include "../textfileparser.h"
#include "../itemrepository.h"
#include "../textfile_evaluation.h"

#ifdef WIN32
#undef main
#endif


// including the command line parser, which is generated by genparse
#include "../clparser/bin2txt.cpp"

int main(int argc, char *argv[] )
{
   Cmdline cl ( argc, argv );

   if ( cl.v() ) {
      cout << argv[0] << " " << getVersionString() << endl;
      exit(0);
   }

   verbosity = cl.r();

   initFileIO( cl.c().c_str() );  // passing the filename from the command line options

   if ( cl.next_param() >= argc) {
      cl.usage();
      exit(0);
   }


   try {
      loadpalette();
      printf(".");
      loadbi3graphics();
      printf(".");

      loadAllData( false );
      printf(".");
      loadUnitSets();
      printf(".");

      for ( int i = cl.next_param(); i < argc; i++ ) {
         for ( int j = 0; j < vehicletypenum; j++ )
            if ( patimat ( argv[i], getvehicletype_forpos ( j )->filename.c_str() )) {
               VehicleType* vt = getvehicletype_forpos ( j );

               tn_file_buf_stream stream ( extractFileName_withoutSuffix ( vt->filename ) + "_generated.asctxt", tnstream::writing );
               PropertyWritingContainer pc ( "VehicleType", stream );
               cout << "Writing file " << pc.getFileName() << "... ";
               vt->runTextIO ( pc );
               pc.run();

               cout << "done \n";
            }

         for ( int j = 0; j < objecttypenum; j++ )
            if ( patimat ( argv[i], getobjecttype_forpos ( j )->filename.c_str() )) {
               ObjectType* ot = getobjecttype_forpos ( j );

               tn_file_buf_stream stream ( extractFileName_withoutSuffix ( ot->filename ) + "_generated.asctxt", tnstream::writing );
               PropertyWritingContainer pc ( "ObjectType", stream );
               cout << "Writing file " << pc.getFileName() << "... ";
               ot->runTextIO ( pc );
               pc.run();

               cout << "done \n";
            }

         for ( int j = 0; j < terraintypenum; j++ )
            if ( patimat ( argv[i], getterraintype_forpos ( j )->filename.c_str() )) {
               TerrainType* tt = getterraintype_forpos ( j );

               tn_file_buf_stream stream ( extractFileName_withoutSuffix ( tt->filename ) + "_generated.asctxt", tnstream::writing );
               PropertyWritingContainer pc ( "TerrainType", stream );
               cout << "Writing file " << pc.getFileName() << "... ";
               tt->runTextIO ( pc );
               pc.run();

               cout << "done \n";
            }

         for ( int j = 0; j < buildingtypenum; j++ )
            if ( patimat ( argv[i], getbuildingtype_forpos ( j )->filename.c_str() )) {
               BuildingType* bt = getbuildingtype_forpos ( j );

               tn_file_buf_stream stream ( extractFileName_withoutSuffix ( bt->filename ) + "_generated.asctxt", tnstream::writing );
               PropertyWritingContainer pc ( "BuildingType", stream );
               cout << "Writing file " << pc.getFileName() << "... ";
               bt->runTextIO ( pc );
               pc.run();

               cout << "done \n";
            }
      }

   } /* endtry */
   catch ( tfileerror err ) {
      printf("\nfatal error accessing file %s \n", err.getFileName().c_str() );
      return 1;
   } /* endcatch */
   catch ( ASCexception ) {
      printf("\na fatal exception occured\n" );
      return 2;
   } /* endcatch */

   return 0;
}


