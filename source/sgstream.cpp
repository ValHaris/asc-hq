/*! \file sgstream.cpp
    \brief The IO for many basic classes and structurs of ACS
   
    These routines are gradually being moved to become methods of their classes
*/


/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2005  Martin Bickel  and  Marc Schellenberger

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
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>

#include "global.h"
#include "tpascal.inc"
#include "typen.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "basegfx.h"
#include "misc.h"
#include "sgstream.h"
#include "stack.h"
#include "basestrm.h"
#include "palette.h"
#include "gameoptions.h"
#include "graphicset.h"
#include "itemrepository.h"
#include "graphics/blitter.h"

#ifdef _WIN32_
 #include <windows.h>
 #include <winreg.h>
#endif 


const char* asc_EnvironmentName = "ASC_CONFIGFILE";
int dataVersion = 0;

const int object_version = 1;
const int technology_version = 1;

Surface leergui;


Surface generate_gui_build_icon ( pvehicletype tnk )
{
   Surface s = leergui.Duplicate();
   s.Blit(tnk->getImage(), SPoint((s.w() - tnk->getImage().w())/2, (s.h() - tnk->getImage().h())/2));
   return s;
}



void loadguipictures( void )
{
   if ( !leergui.valid() ) {
      tnfilestream stream ( "leergui.raw", tnstream::reading );
      leergui.read( stream );
   }
}






void loadpalette ( void )
{
   if ( ! asc_paletteloaded ) {
      displayLogMessage ( 4, "loading palette ... " );

      tnfilestream stream ("palette.pal", tnstream::reading);
      stream.readdata( & pal, sizeof(pal));
      colormixbufchar = new char [ sizeof ( tmixbuf ) ];
      colormixbuf = (pmixbuf) colormixbufchar;
      stream.readdata( colormixbuf,  sizeof ( *colormixbuf ));

      for ( int i= 0; i < 8; i++ ) {
         stream.readdata( &xlattables.xl[i], sizeof ( xlattables.a.dark05 ));
         xlattables.xl[i][255] = 255;
      }
/*
      stream.readdata( &xlattables.a.dark1,  sizeof ( xlattables.a.dark1 ));
      xlattables.a.dark1[255] = 255;

      stream.readdata( &xlattables.a.dark2,  sizeof ( xlattables.a.dark2 ));
      xlattables.a.dark2[255] = 255;

      stream.readdata( &xlattables.a.dark3,  sizeof ( xlattables.a.dark3 ));
      xlattables.a.dark3[255] = 255;

      stream.readdata( &xlattables.light,  sizeof ( xlattables.light ));
      xlattables.light[255] = 255;
  */
      stream.readdata( &truecolor2pal_table,  sizeof ( truecolor2pal_table ));
      stream.readdata( &bi2asc_color_translation_table,  sizeof ( bi2asc_color_translation_table ));

      xlatpictgraytable = (ppixelxlattable) asc_malloc( sizeof(*xlatpictgraytable) );
      // xlatpictgraytable = new tpixelxlattable;
      generategrayxlattable(xlatpictgraytable,160,16,&pal);

      (*xlatpictgraytable)[255] = 255;

      asc_paletteloaded = 1;
      displayLogMessage ( 4, "done\n" );
   }

}




bool makeDirectory ( const ASCString& path )
{
   if ( path.empty() )
      return false;
      
   char tmp[10000];
   constructFileName( tmp, 0, path.c_str(), NULL );

   int existence = directoryExist ( tmp );

   if ( !existence ) {
      int res = createDirectory( tmp ); 
      if ( res ) {
         fprintf(stderr, "could neither access nor create directory %s\n", tmp );
         return false;
      }
   }

   return true;
}


ASCString configFileName;


ASCString getConfigFileName ()
{
   if ( !configFileName.empty() )
      return configFileName ;
   else
      return "-none- ; default values used";
}



int readgameoptions ( const ASCString& filename )
{
   displayLogMessage ( 4, "loading game options ... " );

   bool registryKeyFound = false;

   ASCString fn;
   ASCString installDir;

   if ( !filename.empty() )
      fn = filename;
   else
      if ( getenv ( asc_EnvironmentName ))
         fn = getenv ( asc_EnvironmentName );
      else {

#ifdef _WIN32_
          HKEY key;
          if (  RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                                    "SOFTWARE\\Advanced Strategic Command\\",
                                    0,
                                    KEY_READ,
                                    &key ) == ERROR_SUCCESS) {

             DWORD type;
             const int size = 2000;
             char  buf[size];
             DWORD size2 = size;
             if ( RegQueryValueEx ( key, "InstallDir2", NULL, &type, (BYTE*)buf, &size2 ) == ERROR_SUCCESS ) {
                if ( type == REG_SZ	 ) {
                   fn = buf;
                   appendbackslash ( fn );
                   installDir = fn;
                   fn += asc_configurationfile;

                   registryKeyFound = true;
                }
             }

             RegCloseKey ( key );
         }

#endif
         if ( !registryKeyFound )
             fn = asc_configurationfile;
      }

   char completeFileName[10000];
   constructFileName ( completeFileName, -3, NULL, fn.c_str() );

   configFileName = completeFileName;

   displayLogMessage ( 6, ASCString("Path is ") + completeFileName + "; " );


   if ( exist ( completeFileName )) {
      displayLogMessage ( 6, "found, " );
      CGameOptions::Instance()->load( completeFileName );

      if ( registryKeyFound ) {
         ASCString primaryPath = CGameOptions::Instance()->getSearchPath(0);
         if ( primaryPath == "." || primaryPath == ".\\" || primaryPath == "./" ) {
            CGameOptions::Instance()->setSearchPath(0, installDir );
            displayLogMessage ( 6, "Setting path0 to " + installDir + ", " );
         }
      }

   } else {
     displayLogMessage ( 6, "not found, using defaults, " );

     if ( registryKeyFound ) {
        CGameOptions::Instance()->setSearchPath(0, installDir );
        displayLogMessage ( 6, "Registry Key HKEY_LOCAL_MACHINE\\SOFTWARE\\Advanced Strategic Command\\InstallDir found, setting path0 to " + installDir);
     }

     if ( !configFileName.empty() ) {
        CGameOptions::Instance()->setChanged();
        if ( writegameoptions( configFileName ))
           displayLogMessage ( 6, "A config file has been sucessfully written to " + configFileName + " ");
        else
           displayLogMessage ( 6, "Failed to write config file to " + configFileName + " ");
     }
   }

   displayLogMessage ( 4, "done\n" );

   makeDirectory ( CGameOptions::Instance()->getSearchPath(0) );

   return 0;
}

bool writegameoptions ( ASCString configFileName )
{
   if ( configFileName.empty() )
      configFileName = ::configFileName;

   if ( CGameOptions::Instance()->isChanged() && !configFileName.empty() ) {
      char buf[10000];
      if ( makeDirectory ( extractPath ( buf, configFileName.c_str() ))) {
         CGameOptions::Instance()->save( configFileName );
         return true;
      }
   }
   return false;
}

void checkFileLoadability ( const ASCString& filename )
{
   try {
      tnfilestream strm ( filename, tnstream::reading );
      strm.readChar();
   }
   catch ( ASCexception ) {
      ASCString msg = "Unable to access " + filename + "\n";
      msg +=           "Make sure the data files are in one of the search paths specified in your \n"
                       "config file ! ASC requires these data files to be present:\n"
                       " main.con ; mk1.con ; buildings.con ; trrobj.con ; trrobj2.con \n"
                       "If you don't have these files , get and install them from http://www.asc-hq.org\n"
                       "If you DO have these files, they are probably outdated. \n";
      msg +=           "The configuration file that is used is: " + configFileName + "\n";

      if ( !configFileName.empty() ) {
         CGameOptions::Instance()->setChanged();
         if ( writegameoptions( configFileName ))
            msg += "A configuration file has been written to " + configFileName + "\n";
      }

      msg +=           "These paths are being searched for data files:\n ";
      
      for ( int i = 0; i < CGameOptions::Instance()->getSearchPathNum(); ++i )
         if ( !CGameOptions::Instance()->getSearchPath(i).empty() ) 
            msg += CGameOptions::Instance()->getSearchPath(i) + "\n ";

     fatalError ( msg );
   }
   catch ( ... ) {
      fatalError ( "checkFileLoadability threw an unspecified exception\n" );
   }
}

void initFileIO ( const ASCString& configFileName, int skipChecks )
{
   readgameoptions( configFileName );

   for ( int i = 0; i < CGameOptions::Instance()->getSearchPathNum(); i++ )
      if ( !CGameOptions::Instance()->getSearchPath(i).empty()   ) {
         displayLogMessage ( 3, "adding search patch " + CGameOptions::Instance()->getSearchPath(i) + "\n" );
         addSearchPath ( CGameOptions::Instance()->getSearchPath(i) );
      }
   try {
     opencontainer ( "*.con" );
   }
   catch ( tfileerror err ) {
      fatalError ( "a fatal IO error occured while mounting the container file %s\n"
                   "It is probably damaged, try getting a new one.\n", err.getFileName().c_str() );
   }
   catch ( tcompressionerror err ) {
      fatalError ( "a fatal error occured while decompressing a container file.\n"
                   "If you have several *.con files in your ASC directory, try removing all but main.con.\n"
                   "If the error still occurs then, get a new data package from www.asc-hq.org\n" );
   }
   catch ( ASCexception ) {
      fatalError ( "a fatal error occured while mounting the container files \n");
   }
   catch ( ... ) {
       fatalError ( "loading of game failed during pre graphic initializing" );
   }

   if ( ! (skipChecks & 1 ))
      checkFileLoadability ( "palette.pal" );

   if ( ! (skipChecks & 2 ))
      checkFileLoadability ( "data.version" );

   if ( ! (skipChecks & 4 ))
      checkFileLoadability ( "mk1.version" );

   if ( ! (skipChecks & 8 ))
      checkFileLoadability ( "trrobj.version" );

   if ( ! (skipChecks & 0x10 ))
      checkFileLoadability ( "trrobj2.version" );

   if ( ! (skipChecks & 0x20 ))
      checkFileLoadability ( "buildings.version" );
}

void versionError( const ASCString& filename, const ASCString& location )
{
   ASCString msg = "A newer version of the data file '";
   msg += filename + "' is required. \nYou can get a new data package at http://www.asc-hq.org\n";
   msg += "The old file is " + location;
   fatalError( msg );
}

void checkFileVersion( const ASCString& filename, const ASCString& containername, int version )
{
   ASCString location;
   bool dataOk = true;
   if ( exist ( filename )) {
      tnfilestream s ( filename, tnstream::reading );
      ASCString str = s.readString();
      int v = atoi ( str.c_str() );
      if ( v < version )
         dataOk = false;
      location = s.getLocation();
   } else
      dataOk = false;

   if ( !dataOk )
      versionError ( containername, location );

}

void checkDataVersion( )
{
   ASCString location;
   if ( exist ( "data.version" )) {
      tnfilestream s ( "data.version", tnstream::reading );
      dataVersion = s.readInt();
      location = s.getLocation();
   } else
      dataVersion = 0;

   if ( dataVersion < 12 || dataVersion > 0xffff )
      versionError ( "main.con", location );


   checkFileVersion( "mk1.version", "mk1.con", 13 );
   checkFileVersion( "mk3.version", "units-mk3.con", 12 );
   checkFileVersion( "buildings.version", "buildings.con", 11 );
   checkFileVersion( "trrobj.version", "trrobj.con", 13 );
   checkFileVersion( "trrobj2.version", "trrobj2.con", 2 );
}

//===================================================================================



bool SingleUnitSet :: isMember ( int id, Type type )
{
   if ( type == unit ) {
      for ( int i = 0; i < unitIds.size(); i++ )
        if ( id >= unitIds[i].from && id <= unitIds[i].to )
           return true;
   }
   if ( type == building ) {
      for ( int i = 0; i < buildingIds.size(); i++ )
        if ( id >= buildingIds[i].from && id <= buildingIds[i].to )
           return true;
   }
   return false;
}


std::vector<IntRange> SingleUnitSet::parseIDs ( const char* s )
{
   char buf[10000];

   std::vector<IntRange> res;

   if ( s && s[0] ) {

      strcpy ( buf, s);

      char* piclist = strtok ( buf, ";\r\n" );

      char* pic = strtok ( piclist, "," );
      while ( pic ) {
         int from, to;
         if ( strchr ( pic, '-' )) {
            char* a = strchr ( pic, '-' );
            *a = 0;
            from = atoi ( pic );
            to = atoi ( ++a );
         } else
            from = to = atoi ( pic );

         IntRange ir;
         ir.from = from;
         ir.to = to;
         res.push_back ( ir );

         pic = strtok ( NULL, "," );
      }
   }
   return res;
}

void SingleUnitSet::TranslationTable::parseString ( const char* s )
{
   if ( s && s[0] && strchr ( s, ';' )) {
      char buf[10000];
      if ( s[0] == '#' )
         strcpy ( buf, s+1 );
      else
         strcpy ( buf, s );

      char* tname = strtok ( buf, ";\n\r");
      if ( tname )
         name = tname;

      char* xl = strtok ( NULL, ";\n\r" );
      while ( xl ) {
         if ( strchr ( xl, ',' )) {
            char* a = strchr ( xl, ',' );
            *a = 0;
            IntRange ir;
            ir.from = atoi ( xl );
            ir.to = atoi ( ++a );

            translation.push_back ( ir );

         }
         xl = strtok ( NULL, ";\n\r" );
      }

   }
}


void SingleUnitSet::read ( pnstream stream )
{
   if ( !stream )
      return;
   const char separator = '=';
   ASCString s;
   int data = stream->readTextString ( s );
   if ( s == "#V2#" ) {
      while ( data ) {
         ASCString s2;
         data = stream->readTextString ( s2 );

         size_t seppos = s2.find_first_of ( separator );
         if ( seppos >= 0 ) {
            ASCString b = s2.substr(0, seppos);
            ASCString e = s2.substr( seppos+1 );
            if ( b == "NAME" )
               name = e;

            if ( b == "ACTIVE" )
               active = atoi ( e.c_str() );


            if ( b == "TRANSFORMATION" ) {
               TranslationTable* tt = new TranslationTable;
               tt->parseString ( e.c_str() );
               transtab.push_back ( tt );
            }

            if ( b == "MAINTAINER" )
               maintainer = e;

            if ( b == "INFORMATION" )
               information = e;

            if ( b == "FILTERBUILDINGS" )
               filterBuildings = atoi ( e.c_str() );

            if ( b == "IDENTIFICATION" )
               ID = atoi ( e.c_str() );


            if ( b == "ID" )
               unitIds = parseIDs ( e.c_str() );

            if ( b == "BUILDINGID" )
               buildingIds = parseIDs ( e.c_str() );

         }
      }
   } else {
      size_t seppos = s.find_first_of ( ';' );
      if ( seppos >= 0 ) {
         ASCString b = s.substr(0, seppos);
         ASCString e = s.substr( seppos+1 );
         name = b;
         parseIDs ( e.c_str() );

         while ( data ) {
            ASCString s2;
            data = stream->readTextString ( s2 );
            if ( s2.length() ) {
               TranslationTable* tt = new TranslationTable;
               tt->parseString ( s2.c_str() );
               transtab.push_back ( tt );
            }
         }
      }
   }
}


void loadUnitSets ( void )
{
   displayLogMessage ( 4, "loading unit set definition files\n" );
   tfindfile ff ( "*.set" );
   string n = ff.getnextname();
   while ( !n.empty() ) {
      displayLogMessage ( 5, " loading unit set definition file %s ... ",n.c_str() );
      tnfilestream stream ( n.c_str(), tnstream::reading );

      SingleUnitSet* set = new SingleUnitSet;
      set->read ( &stream );
      unitSets.push_back ( set );

//      ItemFiltrationSystem::ItemFilter* itf = new ItemFiltrationSystem::ItemFilter ( set->name, set->ids, !set->active );
//      ItemFiltrationSystem::itemFilters.push_back ( itf );

      n = ff.getnextname();
      displayLogMessage ( 5, "done\n" );
   } /* endwhile */
}

std::vector<SingleUnitSet*> unitSets;



