/*! \file sgstream.cpp
    \brief The IO for many basic classes and structurs of ACS
   
    These routines are gradually being moved to become methods of their classes
*/


/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2003  Martin Bickel  and  Marc Schellenberger

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
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>

#ifdef _WIN32_
 #include <windows.h>
 #include <winreg.h>
#endif 


#include "global.h"
#include "CLoadable.h"
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

const char* asc_EnvironmentName = "ASC_CONFIGFILE";
int dataVersion = 0;

const int object_version = 1;
const int technology_version = 1;

void* leergui = NULL;
void* removegui = NULL;


void* generate_vehicle_gui_build_icon ( pvehicletype tnk )
{

   int wd;
   int hg;
   getpicsize ( leergui, wd, hg );

   int minx = 0;
   int miny = 0;
   int maxx = 0;
   int maxy = 0;

   tvirtualdisplay vdsp ( 500, 500 );

   bar ( 0, 0, 450, 450, 255 );

   putspriteimage ( 0, 0, tnk->picture[0] );
   maxx= fieldxsize;
   maxy= fieldysize;

   int sze = imagesize ( minx, miny, maxx, maxy );
   void* buf = new char [ sze ];
   getimage ( minx, miny, maxx, maxy, buf );

   while ( (maxx - minx + 1 > wd ) || ( maxy - miny + 1 > hg )) {
      void* temp = halfpict ( buf );

      char* dst = (char*) buf;
      char* src = (char*) temp;

      for ( int i = 0; i < sze; i++ )
         *(dst++) = *(src++);

      minx = 0;
      miny = 0;
      getpicsize ( buf, maxx, maxy );

   } /* endwhile */

   putimage ( 0, 0, leergui );
   putspriteimage ( (wd - maxx) / 2, (hg - maxy) / 2, buf );

   char* newbuildingpic = new char [ imagesize ( 0, 0, wd-1, hg-1 ) ];
   getimage ( 0, 0, wd-1, hg-1, newbuildingpic );
   asc_free ( buf );

   return newbuildingpic;
}





void* generate_building_gui_build_icon ( pbuildingtype bld )
{

   int wd;
   int hg;
   getpicsize ( leergui, wd, hg );

   int minx = 1000;
   int miny = 1000;
   int maxx = 0;
   int maxy = 0;

   tvirtualdisplay vdsp ( 500, 500 );

    bar ( 0, 0, 450, 450, 255 );
  
    for (int y = 0; y <= 5; y++)
       for (int x = 0; x <= 3; x++)
          if (bld->getpicture( BuildingType::LocalCoordinate(x,y) ) ) {
             int xp = fielddistx * x  + fielddisthalfx * ( y & 1);
             int yp = fielddisty * y ;
             if ( xp < minx )
                minx = xp;
             if ( yp < miny )
                miny = yp;
             if ( xp > maxx )
                maxx = xp;
             if ( yp > maxy )
                maxy = yp;

             putspriteimage ( xp, yp, bld->getpicture(BuildingType::LocalCoordinate(x,y)) );
          }
   maxx += fieldxsize;
   maxy += fieldysize;

   int sze = imagesize ( minx, miny, maxx, maxy );
   char* buf = new char [ sze ];
   getimage ( minx, miny, maxx, maxy, buf );

/*
   while ( (maxx - minx + 1 > wd ) || ( maxy - miny + 1 > hg )) {
      void* temp = halfpict ( buf );

      char* dst = (char*) buf;
      char* src = (char*) temp;

      for ( int i = 0; i < sze; i++ )
         *(dst++) = *(src++);

      minx = 0;
      miny = 0;
      getpicsize ( buf, maxx, maxy );

   } 
*/
   TrueColorImage* img = zoomimage ( buf, wd, hg, pal, 1 );
   delete[] buf;

   buf = convertimage ( img, pal );
   delete img;
   getpicsize ( buf, maxx, maxy );

   putimage ( 0, 0, leergui );
   putspriteimage ( (wd - maxx) / 2, (hg - maxy) / 2, buf );

   char* newbuildingpic = new char [ imagesize ( 0, 0, wd-1, hg-1 ) ];
   getimage ( 0, 0, wd-1, hg-1, newbuildingpic );
   delete[] buf;

   return newbuildingpic;


}



void* generate_object_gui_build_icon ( pobjecttype obj, int remove )
{

   int wd;
   int hg;
   getpicsize ( leergui, wd, hg );

   int minx = 0;
   int miny = 0;
   int maxx = 0;
   int maxy = 0;

   tvirtualdisplay vdsp ( 500, 500 );

   bar ( 0, 0, 450, 450, 255 );

   obj->display( 0, 0 );
   maxx= fieldxsize;
   maxy= fieldysize;

   int sze = imagesize ( minx, miny, maxx, maxy );
   void* buf = new char [ sze ];
   getimage ( minx, miny, maxx, maxy, buf );

   while ( (maxx - minx + 1 > wd ) || ( maxy - miny + 1 > hg )) {
      void* temp = halfpict ( buf );

      char* dst = (char*) buf;
      char* src = (char*) temp;

      for ( int i = 0; i < sze; i++ )
         *(dst++) = *(src++);

      minx = 0;
      miny = 0;
      getpicsize ( buf, maxx, maxy );

   } /* endwhile */

   putimage ( 0, 0, leergui );
   putspriteimage ( (wd - maxx) / 2, (hg - maxy) / 2, buf );

   if ( remove )
     putspriteimage ( (wd-18)/2, (hg-18)/2, removegui );

   void* newbuildingpic = new char [ imagesize ( 0, 0, wd-1, hg-1 ) ];
   getimage ( 0, 0, wd-1, hg-1, newbuildingpic );
   asc_free( buf );

   return newbuildingpic;
}

void loadguipictures( void )
{
   if ( !leergui ) {
      tnfilestream stream ( "leergui.raw", tnstream::reading );
      int sze;
      stream.readrlepict ( &leergui, 0, &sze );
   }
   if ( !removegui ) {
      tnfilestream stream ( "guiremov.raw", tnstream::reading );
      int sze;
      stream.readrlepict ( &removegui, 0, &sze );
   }
}



void generatedirecpict ( void* orgpict, void* direcpict )
{
  int t, u;

   tvirtualdisplay vfb ( 100, 100 );
   putspriteimage ( 10, 10, orgpict );

   char *b = (char*) direcpict;

   for (t = 1; t < 20; t++) {
       for (u = 20-t; u < 20+t; u++) {
          *b = getpixel(  10 + u, 9 + t);
          b++;
       }
    }

    for (t = 20; t > 0; t-- ) {
       for (u =20-t; u<= 19 + t; u++) {
          *b = getpixel(  10 + u, 10 + 39 - t );
          b++;
       }
    }

}



extern dacpalette256 pal;

#define sqr(a) (a)*(a)


void generateaveragecolprt ( int x1, int y1, int x2, int y2, void* buf, char* pix1 )
{
   word *w = (word*) buf;
   // int size = ( w[0] + 1) * (w[1] + 1);


   char *c ; // = (char*) buf;
             // c+=4;

   int pixnum = 0;

   int i,j;
   int r=0, g=0, b=0;
   for (j=y1; j< y2 ; j++ ) {
       for (i=x1; i< x2 ; i++ ) {
          c = (char*) buf + j * ( w[0] + 1) + i + 4;
          if (*c < 255) {
             r+=pal[*c][0];
             g+=pal[*c][1];
             b+=pal[*c][2];
             pixnum ++;
          }
      } /* endfor */
   } /* endfor */

   if (pixnum) {
      int r1 = r / pixnum;
      int g1 = g / pixnum;
      int b1 = b / pixnum;


      int diff = 0xFFFFFFF;
      int actdif;


      for (i=0;i<256 ;i++ ) {
         actdif = sqr( pal[i][0] - r1 ) + sqr( pal[i][1] - g1 ) + sqr( pal[i][2] - b1 );
         if (actdif < diff) {
            diff = actdif;
            *pix1 = i;
         }
      }

/*
      int sml = ( r1 >> 2) + (( g1 >> 2) << 6) + (( b1 >> 2) << 12);
      *pix1 = truecolor2pal_table[sml];
*/

   } else {
      *pix1 = 255;
   }
}



FieldQuickView* generateAverageCol ( void* image )
{
   FieldQuickView* qv = new FieldQuickView ;

   char* c = &qv->p1 ;
   // int dx,dy;

   for ( int i=1; i<6 ;i+=2 ) {
      // dx = fieldxsize / i;
      // dy = fieldysize / i;
      for ( int k=0;k<i ; k++) {
         for ( int j=0 ; j<i ; j++) {
             generateaveragecolprt ( k * fieldxsize / i, j * fieldysize / i, (k+1) * fieldxsize / i, (j+1) * fieldysize / i, image,  c );
             c++;
         } /* endfor */
      } /* endfor */
   } /* endfor */

   return qv;
}




void loadpalette ( void )
{
   if ( ! asc_paletteloaded ) {
      displayLogMessage ( 4, "loading palette ... " );

      tnfilestream stream ("palette.pal", tnstream::reading);
      stream.readdata( & pal, sizeof(pal));
      colormixbuf = (pmixbuf) new char [ sizeof ( tmixbuf ) ];
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




bool makeDirectory ( const char* path )
{
   char tmp[10000];
   constructFileName( tmp, 0, path, NULL );

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

char* configFileNameUsed = NULL;
char* configFileNameToWrite = NULL;

char* getConfigFileName ( char* buffer )
{
   if ( buffer ) {
      if ( configFileNameUsed )
         strcpy ( buffer, configFileNameUsed );
      else
         strcpy ( buffer, "-none- ; default values used" );
   }
   return buffer;
}


CLoadableGameOptions* loadableGameOptions =     NULL;

int readgameoptions ( const char* filename )
{
   displayLogMessage ( 4, "loading game options ... " );

   bool registryKeyFound = false;

   ASCString fn;
   ASCString installDir;

   if ( filename && filename[0] )
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
             if ( RegQueryValueEx ( key, "InstallDir", NULL, &type, buf, &size2 ) == ERROR_SUCCESS ) {
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

   configFileNameToWrite = strdup ( completeFileName );

   displayLogMessage ( 6, ASCString("Path is ") + completeFileName + "; " );


   if ( exist ( completeFileName )) {
      configFileNameUsed = strdup ( completeFileName );

      if ( !loadableGameOptions )
         loadableGameOptions = new CLoadableGameOptions (CGameOptions::Instance());

      std::ifstream is( completeFileName );
      loadableGameOptions->Load(is);

      if ( registryKeyFound ) {
         ASCString primaryPath = CGameOptions::Instance()->getSearchPath(0);
         if ( primaryPath == "." || primaryPath == ".\\" || primaryPath == "./" )
            CGameOptions::Instance()->setSearchPath(0, installDir.c_str() );
      }

   } else {
      CGameOptions::Instance()->setChanged(); // to generate a configuration file
      if ( exist ( "sg.cfg" ) ) {
         tnfilestream stream ( "sg.cfg", tnstream::reading);
         int version = stream.readInt ( );
         if ( version == 102 ) {
            CGameOptions::Instance()->fastmove = stream.readInt();
            stream.readInt();
            CGameOptions::Instance()->movespeed = stream.readInt();
            CGameOptions::Instance()->endturnquestion = stream.readInt();
            CGameOptions::Instance()->smallmapactive = stream.readInt();
            CGameOptions::Instance()->units_gray_after_move = stream.readInt();
            CGameOptions::Instance()->mapzoom = stream.readInt();
            CGameOptions::Instance()->mapzoomeditor = stream.readInt();
            CGameOptions::Instance()->startupcount = stream.readInt();
            stream.readInt(); // dontMarkFieldsNotAccessible_movement
            CGameOptions::Instance()->attackspeed1 = stream.readInt();
            CGameOptions::Instance()->attackspeed2 = stream.readInt();
            CGameOptions::Instance()->attackspeed3 = stream.readInt();
            CGameOptions::Instance()->sound.muteEffects = stream.readInt();
            for ( int i = 0; i < 9; i++ )
               stream.readInt();  // dummy

            CGameOptions::Instance()->mouse.scrollbutton = stream.readInt();
            CGameOptions::Instance()->mouse.fieldmarkbutton = stream.readInt();
            CGameOptions::Instance()->mouse.smallguibutton = stream.readInt();
            CGameOptions::Instance()->mouse.largeguibutton = stream.readInt();
            CGameOptions::Instance()->mouse.smalliconundermouse = stream.readInt();
            CGameOptions::Instance()->mouse.centerbutton = stream.readInt();
            CGameOptions::Instance()->mouse.unitweaponinfo = stream.readInt();
            CGameOptions::Instance()->mouse.dragndropmovement = stream.readInt();
            for ( int j = 0; j < 7; j++ )
               stream.readInt();

            CGameOptions::Instance()->container.autoproduceammunition = stream.readInt();
            CGameOptions::Instance()->container.filleverything = stream.readInt();
            stream.readInt(); // CGameOptions::Instance()->container.emptyeverything =
            for ( int k = 0; k < 10; k++ )
               stream.readInt();

            CGameOptions::Instance()->onlinehelptime = stream.readInt();
            CGameOptions::Instance()->smallguiiconopenaftermove = stream.readInt();
            CGameOptions::Instance()->defaultPassword.setName ( strrr ( stream.readInt() ));
            CGameOptions::Instance()->replayspeed = stream.readInt();
            int bi3dir = stream.readInt();
            CGameOptions::Instance()->bi3.interpolate.terrain = stream.readInt();
            CGameOptions::Instance()->bi3.interpolate.units = stream.readInt();
            CGameOptions::Instance()->bi3.interpolate.objects = stream.readInt();
            CGameOptions::Instance()->bi3.interpolate.buildings = stream.readInt();

            if ( bi3dir ) {
               char* tmp;
               stream.readpchar ( &tmp );
               CGameOptions::Instance()->bi3.dir.setName( tmp );
               delete[] tmp;
            }

         }
      }

      if ( registryKeyFound )
         CGameOptions::Instance()->setSearchPath(0, installDir.c_str() );

   }


   #ifdef sgmain
   if ( CGameOptions::Instance()->startupcount < 10 ) {
      CGameOptions::Instance()->startupcount++;
      CGameOptions::Instance()->setChanged();
   }
   #endif

   makeDirectory ( CGameOptions::Instance()->getSearchPath(0) );

   displayLogMessage ( 4, "done\n" );
   return 0;
}

int writegameoptions ( void )
{
   if ( CGameOptions::Instance()->isChanged() && configFileNameToWrite ) {
      char buf[10000];
      if ( makeDirectory ( extractPath ( buf, configFileNameToWrite ))) {
         if ( !loadableGameOptions )
            loadableGameOptions = new CLoadableGameOptions (CGameOptions::Instance());
         std::ofstream os( configFileNameToWrite );
         loadableGameOptions->Save(os);
         return 1;
      }
   }
   return 0;
}

void checkFileLoadability ( const char* filename )
{
   try {
      tnfilestream strm ( filename, tnstream::reading );
      strm.readChar();
   }
   catch ( ASCexception ) {
      ASCString pathSearched;
      for ( int i = 0; i < 5; i++ )
         if ( CGameOptions::Instance()->getSearchPath(i) ) {
            pathSearched += " ";
            pathSearched += CGameOptions::Instance()->getSearchPath(i);
            pathSearched +=  "\n";
         }

      ASCString confName;
      char temp3[1000];
      temp3[0] = 0;
      if ( !configFileNameUsed ) {
         CGameOptions::Instance()->setChanged();
         if ( writegameoptions())
            sprintf(temp3, "A configuration file has been written to %s\n", configFileNameToWrite );
      }

      char temp5[10000];
      char temp2[1000];
      sprintf ( temp5, "Unable to access %s\n"
                       "Make sure the data files are in one of the search paths specified in your \n"
                       "config file ! ASC requires these data files to be present:\n"
                       " main.con ; mk1.con ; buildings.con ; trrobj.con ; trrobj2.con \n"
                       "If you don't have these files , get and install them from http://www.asc-hq.org\n"
                       "If you DO have these files, they are probably outdated. \n" 
                       "The configuration file that is used is: %s \n%s"
                       "These paths are being searched for data files:\n%s\n",
                       filename, getConfigFileName(temp2), temp3, pathSearched.c_str() );

     fatalError ( temp5 );
   }
   catch ( ... ) {
      fatalError ( "checkFileLoadability threw an unspecified exception\n" );
   }
}

void initFileIO ( const ASCString& configFileName, int skipChecks )
{
   readgameoptions( configFileName.c_str() );

   for ( int i = 0; i < CGameOptions::Instance()->getSearchPathNum(); i++ )
      if ( CGameOptions::Instance()->getSearchPath(i)   ) {
         displayLogMessage ( 3, "adding search patch %s\n", CGameOptions::Instance()->getSearchPath(i));
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
   msg += "The old file is located at " + location;
   fatalError( msg );
}

void checkDataVersion( )
{
   ASCString location;
   bool dataOk = true;
   if ( exist ( "data.version" )) {
      tnfilestream s ( "data.version", tnstream::reading );
      dataVersion = s.readInt();
      location = s.getLocation();
   } else
      dataVersion = 0;

   if ( dataVersion < 10 || dataVersion > 0xffff )
      versionError ( "main.con", location );


   if ( exist ( "mk1.version" )) {
      tnfilestream s ( "mk1.version", tnstream::reading );
      char v = s.readChar();
      if ( v < '4' )
         dataOk = false;
      location = s.getLocation();
   } else
      dataOk = false;

   if ( !dataOk )
      versionError ( "mk1.con", location );


   if ( exist ( "buildings.version" )) {
      tnfilestream s ( "buildings.version", tnstream::reading );
      char v = s.readChar();
      if ( v < '2' || v >= 0xffff)
         dataOk = false;
      location = s.getLocation();
   } else
      dataOk = false;

   if ( !dataOk )
      versionError ( "buildings.con", location );


   if ( exist ( "trrobj.version" )) {
      tnfilestream s ( "trrobj.version", tnstream::reading );
      char v = s.readChar();
      if ( v < '7' )
         dataOk = false;
      location = s.getLocation();
   } else
      dataOk = false;

   if ( !dataOk )
      versionError ( "trrobj.con", location );

}

//===================================================================================


int SingleUnitSet :: isMember ( int id )
{
   for ( int i = 0; i < ids.size(); i++ )
     if ( id >= ids[i].from && id <= ids[i].to )
        return 1;
   return 0;
}


void SingleUnitSet::parseIDs ( const char* s )
{
   char buf[10000];

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
         ids.push_back ( ir );

         pic = strtok ( NULL, "," );
      }
   }
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

         int seppos = s2.find_first_of ( separator );
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
               parseIDs ( e.c_str() );

         }
      }
   } else {
      int seppos = s.find_first_of ( ';' );
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



void displayLogMessage ( int msgVerbosity, char* message, ... )
{
   va_list arglist;
   va_start ( arglist, message );
   if ( msgVerbosity <= verbosity ) {
      char buf[10000];
      vsprintf ( buf, message, arglist );

      displayLogMessage( msgVerbosity, ASCString(buf) );
   }
   va_end ( arglist );
}

void displayLogMessage ( int msgVerbosity, const ASCString& message )
{
   if ( msgVerbosity <= verbosity ) {
      fprintf ( stdout, "%s", message.c_str() );
      fflush ( stdout );
   }
}
