/*! \file sgstream.cpp
    \brief The IO for many basic classes and structurs of ACS
   
    These routines are gradually being moved to become methods of their classes
*/


//     $Id: sgstream.cpp,v 1.82 2002-04-10 21:12:13 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.81  2002/03/25 18:48:15  mbickel
//      Applications can now specify which data checks to perform
//
//     Revision 1.80  2002/03/03 14:13:48  mbickel
//      Some documentation updates
//      Soundsystem update
//      AI bug fixed
//
//     Revision 1.79  2002/03/02 23:04:01  mbickel
//      Some cleanup of source code
//      Improved Paragui Integration
//      Updated documentation
//      Improved Sound System
//
//     Revision 1.78  2001/12/19 17:16:29  mbickel
//      Some include file cleanups
//
//     Revision 1.77  2001/12/19 11:46:36  mbickel
//      Applied patches from Michael Moerz:
//       - 64bit cleanup of demount.cpp, mount.cpp
//       - removal of #ifdef converter and moved conveter specific functions
//         to independant lib
//
//     Revision 1.76  2001/12/14 10:20:05  mbickel
//      Cleanup and enhancements to configure.in
//      Removed last remains of octagonal version from source files
//
//     Revision 1.75  2001/11/22 15:08:24  mbickel
//      Added gameoption heightChangeMovement
//
//     Revision 1.74  2001/11/05 21:31:04  mbickel
//      Fixed compilation errors
//      new data version required
//
//     Revision 1.73  2001/11/05 21:10:42  mbickel
//      Updated palette code
//
//     Revision 1.72  2001/10/31 18:34:33  mbickel
//      Some adjustments and fixes for gcc 3.0.2
//
//     Revision 1.71  2001/10/16 19:58:19  mbickel
//      Added title screen for mapeditor
//      Updated source documentation
//
//     Revision 1.70  2001/10/11 10:22:50  mbickel
//      Some cleanup and fixes for Visual C++
//
//     Revision 1.69  2001/08/26 20:55:04  mbickel
//      bin2text can now load text files too
//      LoadableItemType interface class added
//
//     Revision 1.68  2001/08/09 19:28:22  mbickel
//      Started adding buildingtype text file functions
//
//     Revision 1.67  2001/08/09 15:58:59  mbickel
//      Some usability improvements in the map editor
//      More flexible BI3 map import
//      Better textfile error messages
//
//     Revision 1.66  2001/08/09 14:50:37  mbickel
//      Added palette.map to data directory
//      Improved usability of terrain selection in mapeditor
//      New terrain translation in bi3 import function
//      Better error messages in text parser
//      Better error message: duplicate ID
//
//     Revision 1.65  2001/08/06 20:54:43  mbickel
//      Fixed lots of crashes related to the new text files
//      Fixed delayed events
//      Fixed crash in terrin change event
//      Fixed visibility of mines
//      Fixed crashes in event loader
//
//     Revision 1.64  2001/08/02 18:50:43  mbickel
//      Corrected Error handling in Text parsers
//      Improved version information
//
//     Revision 1.63  2001/08/02 15:33:02  mbickel
//      Completed text based file formats
//
//     Revision 1.62  2001/07/30 17:43:13  mbickel
//      Added Microsoft Visual Studio .net project files
//      Fixed some warnings
//
//     Revision 1.61  2001/07/28 11:19:12  mbickel
//      Updated weaponguide
//      moved item repository from spfst to itemrepository
//
//     Revision 1.60  2001/07/27 21:13:35  mbickel
//      Added text based file formats
//      Terraintype and Objecttype restructured
//
//     Revision 1.59  2001/07/14 19:13:16  mbickel
//      Rewrote sound system
//      Moveing units make sounds
//      Added sound files to data
//
//     Revision 1.58  2001/05/16 23:21:02  mbickel
//      The data file is mounted using automake
//      Added sgml documentation
//      Added command line parsing functionality;
//        integrated it into autoconf/automake
//      Replaced command line parsing of ASC and ASCmapedit
//
//     Revision 1.57  2001/03/23 16:02:56  mbickel
//      Some restructuring;
//      started rewriting event system
//
//     Revision 1.56  2001/02/18 15:37:19  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.55  2001/02/11 11:39:42  mbickel
//      Some cleanup and documentation
//
//     Revision 1.54  2001/02/04 21:26:59  mbickel
//      The AI status is written to savegames -> new savegame revision
//      Lots of bug fixes
//
//     Revision 1.53  2001/02/01 22:48:48  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.52  2001/01/28 23:00:40  mbickel
//      Made the small editors compilable with Watcom again
//
//     Revision 1.51  2001/01/28 20:42:15  mbickel
//      Introduced a new string class, ASCString, which should replace all
//        char* and std::string in the long term
//      Split loadbi3.cpp into 3 different files (graphicselector, graphicset)
//
//     Revision 1.50  2001/01/28 14:04:19  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.49  2001/01/21 16:37:19  mbickel
//      Moved replay code to own file ( replay.cpp )
//      Fixed compile problems done by cleanup
//
//     Revision 1.48  2001/01/21 12:48:36  mbickel
//      Some cleanup and documentation
//
//     Revision 1.47  2001/01/04 15:14:06  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.46  2000/12/27 22:23:15  mbickel
//      Fixed crash in loading message text
//      Removed many unused variables
//
//     Revision 1.45  2000/11/29 09:40:25  mbickel
//      The mapeditor has now two maps simultaneously active
//      Moved memorychecking functions to its own file: memorycheck.cpp
//      Rewrote password handling in ASC
//
//     Revision 1.44  2000/11/26 22:18:56  mbickel
//      Added command line parameters for setting the verbosity
//      Increased verbose output
//
//     Revision 1.43  2000/11/21 20:27:07  mbickel
//      Fixed crash in tsearchfields (used by object construction for example)
//      AI improvements
//      configure.in: added some debug output
//                    fixed broken check for libbz2
//
//     Revision 1.42  2000/11/14 20:36:41  mbickel
//      The AI can now use supply vehicles
//      Rewrote objecttype IO routines to make the structure independant of
//       the memory layout
//
//     Revision 1.41  2000/11/08 19:31:13  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
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
   delete[] buf;

   return newbuildingpic;
}



pvehicletype   loadvehicletype( const char* name)
{
   displayLogMessage ( 5, " loading vehicle type %s ...", name );
   tnfilestream stream ( name, tnstream::reading );
   pvehicletype vt = loadvehicletype ( stream );
   vt->filename = name;
   displayLogMessage ( 5, " done\n");
   return vt;
}


pvehicletype   loadvehicletype( tnstream& stream )
{
   pvehicletype veh = new Vehicletype;
   veh->read( stream );
   veh->location = stream.getLocation();
   return veh;
}





ptechnology       loadtechnology( const char *       name)
{
   displayLogMessage ( 5, " loading technology %s ...", name );
   tnfilestream stream ( name, tnstream::reading );
   ptechnology t = loadtechnology ( &stream );
   displayLogMessage ( 5, " done\n");
   return t;
}


ptechnology       loadtechnology( pnstream stream )
{ 
   int          w;

   int version = stream->readInt();
   if ( version == technology_version ) {

      ptechnology pt =  new ttechnology;
   
      stream->readdata ( pt, sizeof(*pt) ); 
      if ( pt->name )
         stream->readpchar( &pt->name );

      if ( pt->infotext )
         stream->readpchar( &pt->infotext );

      if ( pt->icon )
         stream->readrlepict ( &pt->icon,false,&w);

      if ( pt->pictfilename ) {
         stream->readpchar( &pt->pictfilename );

         char* pc = pt->pictfilename;
         while ( *pc ) {
            *pc = tolower( *pc );
            pc++;
         }
      }
   
      pt->lvl = -1;

      return pt; 
   } else
      return NULL;
} 

void writetechnology ( ptechnology tech, pnstream stream )
{
   stream->writeInt ( technology_version );

   stream->writedata2 ( *tech );
 
   if (tech->name)
      stream->writepchar( tech->name );
   if (tech->infotext)
      stream->writepchar( tech->infotext );
   if (tech->icon)
      stream->writerlepict( tech->icon );
   if ( tech->pictfilename )
      stream->writepchar( tech->pictfilename );
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
   delete[] buf;

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


pbuildingtype       loadbuildingtype( const char *       name)
{
   displayLogMessage ( 5, " loading building type %s ...", name );

   tnfilestream stream ( name, tnstream::reading );
   pbuildingtype bt = loadbuildingtype ( &stream );
   bt->filename = name;

   displayLogMessage ( 5, " done\n");
   return bt;
}


pbuildingtype       loadbuildingtype( pnstream stream )
{
  pbuildingtype bdt = new BuildingType;
  bdt->read ( *stream );
  bdt->location = stream->getLocation();
  return bdt;
}


void writebuildingtype ( pbuildingtype bld, pnstream stream )
{
  bld->write ( *stream );
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



pquickview generateaveragecol ( TerrainType::Weather* bdn )
{
   pquickview qv;
   qv = new tquickview ;
   for ( int dir = 0; dir < 1; dir++ )
      if ( bdn->pict ) {

         char* c = (char*) &qv->dir[dir].p1 ;
         // int dx,dy;

         for ( int i=1; i<6 ;i+=2 ) {
            // dx = fieldxsize / i;
            // dy = fieldysize / i;
            for ( int k=0;k<i ; k++) {
               for ( int j=0 ; j<i ; j++) {
                   generateaveragecolprt ( k * fieldxsize / i, j * fieldysize / i, (k+1) * fieldxsize / i, (j+1) * fieldysize / i, bdn->pict,  c );
                   c++;
               } /* endfor */
            } /* endfor */
         } /* endfor */
      }

   return qv;
}


pterraintype      loadterraintype( const char *       name)
{
   displayLogMessage ( 5, " loading terrain type %s ...", name );
   tnfilestream stream ( name, tnstream::reading );
   pterraintype tt = loadterraintype ( &stream );
   tt->filename = name;
   displayLogMessage ( 5, " done\n" );
   return tt;
}

pterraintype loadterraintype( pnstream stream )
{
    pterraintype bbt = new TerrainType;
    bbt->read ( *stream );
    bbt->location = stream->getLocation();
    return bbt;
}




pobjecttype   loadobjecttype( const char *       name)
{
   displayLogMessage ( 5, " loading object type %s ...", name );
   tnfilestream stream ( name, tnstream::reading );
   pobjecttype ot = loadobjecttype ( &stream );
   ot->filename = name;
   displayLogMessage ( 5, " done\n" );
   return ot;
/*
   pobjecttype fztn = loadobjecttype ( &stream );
   for ( int i = 0 ; i < cmovemalitypenum; i++) {
       if ( fztn->movemalus_abs[i] > 0  && fztn->movemalus_abs[i] < 10 )
          fprintf(stderr, "The object %s has invalid movemali\n", name );
   }
   return fztn;
*/
}



pobjecttype   loadobjecttype( pnstream stream )
{
   pobjecttype fztn = new ObjectType;
   fztn->read ( *stream );
   fztn->location = stream->getLocation();

   return fztn;
}

void writeobject ( pobjecttype object, pnstream stream, int compressed )
{
   object->write ( *stream );
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



t_carefor_containerstream :: t_carefor_containerstream ( void ) 
{ 
   opencontainer ( "*.con" );
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
            CGameOptions::Instance()->dontMarkFieldsNotAccessible_movement = stream.readInt();
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
            CGameOptions::Instance()->container.emptyeverything = stream.readInt();
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

void checkDataVersion( )
{
      if ( exist ( "data.version" )) {
         tnfilestream s ( "data.version", tnstream::reading );
         dataVersion = s.readInt();
      } else
         dataVersion = 0;

      if ( dataVersion < 8 || dataVersion > 0xffff )
         fatalError("A newer version of the data files is required. \n"
                    "You can get a new data package at http://www.asc-hq.org", 2 );
}

//===================================================================================


int SingleUnitSet :: isMember ( int id )
{
   for ( int i = 0; i < ids.size(); i++ )
     if ( id >= ids[i].from && id <= ids[i].to )
        return 1;
   return 0;
}

/*
         class IdRangeVector {
                public:
                   vector<IdRange> idRange;
                   void parseString ( const char* s );
         };
*/

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

         IdRange ir;
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
            IdRange ir;
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
      fprintf ( stdout, message.c_str() );
      fflush ( stdout );
   }
}
