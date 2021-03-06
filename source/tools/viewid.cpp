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
#include "../itemrepository.h"

#undef main

// including the command line parser, which is generated by genparse
#include "../clparser/viewid.cpp"

int main(int argc, char *argv[] )
{
   Cmdline cl ( argc, argv );

   if ( cl.v() ) {
      cout << argv[0] << " " << getVersionString() << endl;
      exit(0);
   }

   verbosity = cl.r();

   initFileIO( cl.c().c_str() );  // passing the filename from the command line options

   try {

      loadpalette();
      loadbi3graphics();

      loadalltextfiles();
      loadallobjecttypes();
      loadallbuildingtypes();
      loadallvehicletypes();
      freetextdata();

      for ( int i = 0; i < objecttypenum; i++ ) {
        Object*type s = getobjecttype_forpos ( i );
        printf(" %30s - %5d - %s\n", s->filename.c_str(), s->id, s->name.c_str() );
      }

   } /* endtry */
   catch ( tfileerror err ) {
      fatalError("fatal error accessing file " + err.getFileName() );
      return 1;
   } /* endcatch */
   catch ( ASCmsgException err ) {
      fatalError("a fatal exception occured: " + err.getMessage() );
      return 2;
   } /* endcatch */
   catch ( ASCexception ) {
      fatalError("\na fatal exception occured\n" );
      return 2;
   } /* endcatch */

   return 0;
}


