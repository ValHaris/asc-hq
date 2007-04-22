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
#include "typen.h"
#include "basegfx.h"
#include "misc.h"
#include "sgstream.h"
#include "basestrm.h"
#include "palette.h"
#include "gameoptions.h"

#ifdef _WIN32_
 #include <windows.h>
 #include <winreg.h>
 #include <shlobj.h>
#endif 


const char* asc_EnvironmentName = "ASC_CONFIGFILE";
int dataVersion = 0;

const int object_version = 1;
const int technology_version = 1;




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

class ConfigurationFileLocator {
      ASCString cmdline;

   protected:
      vector<ASCString> getDefaultDirectory();

   public:
      void CommandLineParam( const ASCString& path );

      ASCString getConfig();
      ASCString getConfigForPrinting();
};


void ConfigurationFileLocator::CommandLineParam( const ASCString& path )
{
   cmdline = path;
}

vector<ASCString> ConfigurationFileLocator::getDefaultDirectory()
{
   vector<ASCString> dirs;
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
            ASCString dir = buf;
            appendbackslash ( dir );
            dirs.push_back( dir );
         }
      }

      RegCloseKey ( key );
   }

   TCHAR szPath[MAX_PATH];

   if ( SUCCEEDED(SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, szPath ))) {
      ASCString dir = szPath;
      appendbackslash ( dir );
      dirs.push_back( dir );
   }

   if ( SUCCEEDED(SHGetFolderPath( NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, szPath ))) {
      ASCString dir = szPath;
      appendbackslash ( dir );
      dirs.push_back( dir );
   }

#else

#endif

   return dirs;

}


ASCString ConfigurationFileLocator::getConfig()
{
   if ( cmdline.length() ) {
      
   }
   return "";
}

ASCString ConfigurationFileLocator::getConfigForPrinting()
{
   return "";
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
         if ( !registryKeyFound ) {
            fn = asc_configurationfile;
            if ( fn.find( pathdelimitter ) == ASCString::npos )
               fn = ASCString(".") + pathdelimitter + asc_configurationfile;
         }
      }

   char completeFileName[10000];
   constructFileName ( completeFileName, -3, NULL, fn.c_str() );

   configFileName = completeFileName;

   displayLogMessage ( 6, ASCString("Path is ") + completeFileName + "; " );


   if ( exist ( completeFileName )) {
      displayLogMessage ( 6, "found, " );
      try {
         CGameOptions::Instance()->load( completeFileName );
      } 
      catch ( ParsingError err ) {
         fatalError ( "Error parsing text file " + err.getMessage() );
      }
      catch ( tfileerror err ) {
         fatalError ( "Error loading file " + err.getFileName() );
      }
      catch ( ... ) {
         fatalError ( "caught undefined exception" );
      }

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
   try {
      if ( configFileName.empty() )
         configFileName = ::configFileName;

      if ( CGameOptions::Instance()->isChanged() && !configFileName.empty() ) {
         char buf[10000];
         if ( makeDirectory ( extractPath ( buf, configFileName.c_str() ))) {
            CGameOptions::Instance()->save( configFileName );
            return true;
         }
      }
   }
   catch ( ... ) {
      // warning("Could not save game options");
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
      msg +=           "Make sure the file main.ascdat is in one of the search paths \n"
                       "specified in your config file !\n" 
                       "If you don't have this file , get and install it from http://www.asc-hq.org\n"
                       "If you DO have the file, it is probably outdated. \n";
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
     opencontainer ( "*.ascdat" );
   }
   catch ( tfileerror err ) {
      fatalError ( "a fatal IO error occured while mounting the container file %s\n"
                   "It is probably damaged, try getting a new one.\n", err.getFileName().c_str() );
   }
   catch ( tcompressionerror err ) {
      fatalError ( "a fatal error occured while decompressing a container file.\n"
                   "If you have several *.ascdat files in your ASC directory, try removing all but main.ascdat.\n"
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

   if ( ! (skipChecks & 8 ))
      checkFileLoadability ( "trrobj.version" );

   if ( ! (skipChecks & 0x10 ))
      checkFileLoadability ( "trrobj2.version" );

   if ( ! (skipChecks & 0x20 ))
      checkFileLoadability ( "buildings.version" );

   if ( ! (skipChecks & 0x40 ))
      checkFileLoadability ( "markedfielddark.png" );
   
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
   checkFileVersion( "main.version", "main.ascdat", 2 );

   if ( exist( "pbp.ascdat" ))
      checkFileVersion( "pbp.version", "pbp.ascdat", 21 );
}

