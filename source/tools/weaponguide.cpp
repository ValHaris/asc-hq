/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2001  Martin Bickel, Marc Schellenberger and
    Steffen Froehlich
 
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
// Version v2.0 , change also GENERAL


#include <stdio.h>
#include <stdlib.h>
#include <algorithm>


#include "../typen.h"
#include "../basestrm.h"
#include "../misc.h"
#include "../sgstream.h"
#include "../buildingtype.h"
#include "../vehicletype.h"
#include "../errors.h"
#include "../graphicset.h"
#include "../ascstring.h"
#include "../itemrepository.h"
#include "../strtmesg.h"
#include "../basegfx.h"
#include "../loadpcx.h"
#include "../unitset.h"

#include "infopage.h"
#include "guidegenerator.h"

#include "../stdio-errorhandler.h"

#ifdef WIN32
#undef main
#endif


// including the command line parser, which is generated by genparse
#include "../clparser/weaponguide.cpp"


int main(int argc, char *argv[] ) 
{
   StdIoErrorHandler stdioerrorhandler;
   try {
      Cmdline cl ( argc, argv );

      if ( cl.v() ) {
         cout << argv[0] << " " << getVersionString() << endl;
         exit(0);
      }
      cout << "*******Starting guide generation*******" << endl;
      MessagingHub::Instance().setVerbosity( cl.r() );

      SDL_Init( SDL_INIT_VIDEO );
      
      ConfigurationFileLocator::Instance().setExecutableLocation( argv[0] );
      initFileIO( cl.c(), 4 );  // passing the filename from the command line options
      ASCString prefixDir = cl.d();
      if ( !prefixDir.empty() )
         appendbackslash ( prefixDir );

      #ifdef _WIN32_
      ASCString tempPath = getenv("temp");
      appendbackslash(tempPath);
      #else
      ASCString tempPath = "/tmp/";
      #endif


      printf("Loading data for generating unit documentation. Please wait...");
      loadpalette();
      printf(".");

      GraphicSetManager::Instance().loadData();
        
      loadAllData( false );
      printf(".");
      loadUnitSets();
      printf(".");
        

      char* wildcard;

      if ( cl.next_param() < argc ) {
         wildcard = argv[cl.next_param()];
         // if a command line parameter is specified, use it as wildcard
         // for example: weaponguide s*.veh
      } else {
         wildcard =  "*";
         // else use all verhicles
      }



      BuildingGuideGen gen(prefixDir, cl.m(), cl.s(),  cl.t(), cl.roottech(), cl.i(), cl.l(), !cl.b(), !cl.writeall());
      gen.processSubjects();
      cout << "*******Buildings done*******" << endl;
      UnitGuideGen unitGen(prefixDir, cl.m(), cl.s(),  cl.t(), cl.roottech(), cl.i(), cl.l(), !cl.writeall());
      unitGen.processSubjects();
      cout << "*******Units done*******" << endl; 
      cout << "******Guide generated*******" << endl;
      return 0;
   }
   catch( ... ) {
      cerr << "Exception catched\n";
      return 1;
   }
};








