/*! \file sg.cpp
    \brief THE main program: ASC
*/


//     $Id: sg.cpp,v 1.158 2001-08-02 18:50:43 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.157  2001/08/02 15:33:01  mbickel
//      Completed text based file formats
//
//     Revision 1.156  2001/07/30 17:43:13  mbickel
//      Added Microsoft Visual Studio .net project files
//      Fixed some warnings
//
//     Revision 1.155  2001/07/28 11:19:12  mbickel
//      Updated weaponguide
//      moved item repository from spfst to itemrepository
//
//     Revision 1.154  2001/07/27 21:13:35  mbickel
//      Added text based file formats
//      Terraintype and Objecttype restructured
//
//     Revision 1.153  2001/07/25 19:01:32  mbickel
//      Started adding text file formats
//
//     Revision 1.152  2001/07/20 21:27:31  mbickel
//      even more diagnostic messages
//
//     Revision 1.151  2001/07/20 20:37:48  mbickel
//      Enhanced logging capabilities
//
//     Revision 1.150  2001/07/18 16:05:47  mbickel
//      Fixed: infinitive loop in displaying "player exterminated" msg
//      Fixed: construction of units by units: wrong player
//      Fixed: loading bug of maps with mines
//      Fixed: invalid map parameter
//      Fixed bug in game param edit dialog
//      Fixed: cannot attack after declaring of war
//      New: ffading of sounds
//
//     Revision 1.149  2001/07/15 21:00:25  mbickel
//      Some cleanup in the vehicletype class
//
//     Revision 1.148  2001/07/14 21:07:46  mbickel
//      Sound works now under Win32 too
//      Error reporting on Win32 during startup works again.
//
//     Revision 1.147  2001/07/14 19:13:16  mbickel
//      Rewrote sound system
//      Moveing units make sounds
//      Added sound files to data
//
//     Revision 1.146  2001/07/14 13:15:17  mbickel
//      Rewrote sound handling
//
//     Revision 1.145  2001/07/13 14:02:48  mbickel
//      Fixed inconsistency in replay (shareviewchange)
//      Fixed sound initialization problem
//      Speed up of movement
//
//     Revision 1.144  2001/07/11 20:44:37  mbickel
//      Removed some vehicles from the data file.
//      Put all legacy units in into the data/legacy directory
//
//     Revision 1.143  2001/07/03 10:05:54  mbickel
//      Added Makefiles for Borland C++ command line compiler
//
//     Revision 1.142  2001/06/14 14:46:47  mbickel
//      The resolution of ASC can be specified in the configuration file
//      The fileselect dialog box shows the file's location
//      new ascmap2pcx param: outputdir
//
//     Revision 1.141  2001/05/21 12:46:19  mbickel
//      Fixed infinite loop in AI::strategy
//      Fixed bugs in mapeditor - event editing
//      Fixed bugs in even loading / writing
//      Fixed wrong build order AI <-> main program
//
//     Revision 1.140  2001/05/19 13:07:58  mbickel
//      ASC now compiles with Borland C++ Builder again
//      Added getopt for use with BCB
//
//     Revision 1.139  2001/05/17 14:23:19  mbickel
//      Rewrote command line parameters of all programs
//      Made manpages generation optional
//
//     Revision 1.138  2001/05/16 23:21:01  mbickel
//      The data file is mounted using automake
//      Added sgml documentation
//      Added command line parsing functionality;
//        integrated it into autoconf/automake
//      Replaced command line parsing of ASC and ASCmapedit
//
//     Revision 1.137  2001/05/15 09:54:52  mbickel
//      Added complete Data and documentaion to repository
//      The unix programs are now build in the 'unix' subdirectory, not the
//       source directory.
//      The executable names of all ASC helper programs now start with 'asc_'
//
//     Revision 1.136  2001/03/30 12:43:16  mbickel
//      Added 3D pathfinding
//      some cleanup and documentation
//      splitted the ai into several files, now located in the ai subdirectory
//      AI cares about airplane servicing and range constraints
//
//     Revision 1.135  2001/02/26 12:35:28  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.134  2001/02/18 15:37:17  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.133  2001/02/15 21:57:06  mbickel
//      The AI doesn't try to attack with recon units any more
//
//     Revision 1.132  2001/02/11 11:39:41  mbickel
//      Some cleanup and documentation
//
//     Revision 1.131  2001/02/04 21:26:58  mbickel
//      The AI status is written to savegames -> new savegame revision
//      Lots of bug fixes
//
//     Revision 1.130  2001/02/01 22:48:46  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.129  2001/01/31 14:52:41  mbickel
//      Fixed crashes in BI3 map importing routines
//      Rewrote memory consistency checking
//      Fileselect dialog now uses ASCStrings
//
//     Revision 1.128  2001/01/28 20:42:14  mbickel
//      Introduced a new string class, ASCString, which should replace all
//        char* and std::string in the long term
//      Split loadbi3.cpp into 3 different files (graphicselector, graphicset)
//
//     Revision 1.127  2001/01/28 17:19:13  mbickel
//      The recent cleanup broke some source files; this is fixed now
//
//     Revision 1.126  2001/01/28 14:04:16  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.125  2001/01/25 23:45:02  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.124  2001/01/21 16:37:18  mbickel
//      Moved replay code to own file ( replay.cpp )
//      Fixed compile problems done by cleanup
//
//     Revision 1.123  2001/01/19 13:33:52  mbickel
//      The AI now uses hemming
//      Several bugfixes in Vehicle Actions
//      Moved all view calculation to viewcalculation.cpp
//      Mapeditor: improved keyboard support for item selection
//
//     Revision 1.122  2001/01/04 15:14:03  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.121  2000/12/29 16:33:53  mbickel
//      The supervisor may now reset passwords
//
//     Revision 1.120  2000/12/28 16:58:37  mbickel
//      Fixed bugs in AI
//      Some cleanup
//      Fixed crash in building construction
//
//     Revision 1.119  2000/12/28 11:12:46  mbickel
//      Fixed: no redraw when restoring fullscreen focus in WIN32
//      Better error message handing in WIN32
//
//     Revision 1.118  2000/12/27 22:23:13  mbickel
//      Fixed crash in loading message text
//      Remo functioved many unused variables
//
//     Revision 1.117  2000/12/26 21:04:36  mbickel
//      Fixed: putimageprt not working (used for small map displaying)
//      Fixed: mapeditor crashed on generating large maps
//
//     Revision 1.116  2000/12/26 14:46:00  mbickel
//      Made ASC compilable (and runnable) with Borland C++ Builder
//
//     Revision 1.115  2000/12/23 15:58:38  mbickel
//      Trying to get ASC to run with Borland C++ Builder...
//
//     Revision 1.114  2000/11/29 11:05:30  mbickel
//      Improved userinterface of the mapeditor
//      map::preferredfilenames uses now strings (instead of char*)
//
//     Revision 1.113  2000/11/29 09:40:24  mbickel
//      The mapeditor has now two maps simultaneously active
//      Moved memorychecking functions to its own file: memorycheck.cpp
//      Rewrote password handling in ASC
//
//     Revision 1.112  2000/11/26 22:18:54  mbickel
//      Added command line parameters for setting the verbosity
//      Increased verbose output
//
//     Revision 1.111  2000/11/21 20:27:05  mbickel
//      Fixed crash in tsearchfields (used by object construction for example)
//      AI improvements
//      configure.in: added some debug output
//                    fixed broken check for libbz2
//
//     Revision 1.110  2000/11/15 19:28:33  mbickel
//      AI improvements
//
//     Revision 1.109  2000/11/08 19:31:11  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
//     Revision 1.108  2000/10/26 18:14:59  mbickel
//      AI moves damaged units to repair
//      tmap is not memory layout sensitive any more
//
//     Revision 1.107  2000/10/18 18:53:02  mbickel
//      Added JPEG support to windows version
//
//     Revision 1.106  2000/10/18 17:09:39  mbickel
//      Fixed eventhandling for DOS
//
//     Revision 1.105  2000/10/18 15:10:04  mbickel
//      Fixed event handling for windows and dos
//
//     Revision 1.104  2000/10/18 14:14:16  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//
//     Revision 1.103  2000/10/18 12:40:46  mbickel
//      Rewrite event handling for windows
//
//     Revision 1.102  2000/10/14 14:16:06  mbickel
//      Cleaned up includes
//      Added mapeditor to win32 watcom project
//
//     Revision 1.101  2000/10/14 10:52:52  mbickel
//      Some adjustments for a Win32 port
//
//     Revision 1.100  2000/10/12 22:24:00  mbickel
//      Made the DOS part of the new platform system work again
//
//     Revision 1.99  2000/10/12 21:37:53  mbickel
//      Further restructured platform dependant routines
//
//     Revision 1.98  2000/10/11 14:26:44  mbickel
//      Modernized the internal structure of ASC:
//       - vehicles and buildings now derived from a common base class
//       - new resource class
//       - reorganized exceptions (errors.h)
//      Split some files:
//        typen -> typen, vehicletype, buildingtype, basecontainer
//        controls -> controls, viewcalculation
//        spfst -> spfst, mapalgorithm
//      bzlib is now statically linked and sources integrated
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <new.h>
#include <malloc.h>
#include <ctype.h>
#include <signal.h>
#include <algorithm>
#include <memory>


#include "vehicletype.h"
#include "buildingtype.h"
#include "ai/ai.h"
#include "basegfx.h"
#include "misc.h"
#include "loadpcx.h"
#include "newfont.h"
#include "events.h"
#include "typen.h"
#include "spfst.h"
#include "loaders.h"
#include "dlg_box.h"
#include "stack.h"
#include "missions.h"
#include "controls.h"
#include "dlg_box.h"
#include "dialog.h"
#include "gui.h"
#include "pd.h"
#include "strtmesg.h"
#include "weather.h"
#include "gamedlg.h"
#include "network.h"
#include "building.h"
#include "sg.h"
#include "soundList.h"
#include "gameoptions.h"
#include "loadimage.h"
#include "astar2.h"
#include "errors.h"
#include "password.h"
#include "password_dialog.h"
#include "viewcalculation.h"
#include "replay.h"
#include "dashboard.h"
#include "graphicset.h"
#include "loadbi3.h"
#include "itemrepository.h"



// #define MEMCHK

#include "memorycheck.cpp"



class tsgonlinemousehelp : public tonlinemousehelp {
   public:
     tsgonlinemousehelp ( void );
};

tsgonlinemousehelp :: tsgonlinemousehelp ( void )
{
   helplist.num = 12;

   static tonlinehelpitem sghelpitems[12]  = {{{ 498, 26, 576, 36}, 20001 },
                                              {{ 498, 41, 576, 51}, 20002 },
                                              {{ 586, 26, 612, 51}, 20003 },
                                              {{ 499, 57, 575, 69}, 20004 },
                                              {{ 499, 70, 575, 81}, 20005 },
                                              {{ 577, 58, 610, 68}, 20006 },
                                              {{ 577, 70, 610, 80}, 20007 },
                                              {{ 502, 92, 531,193}, 20008 },
                                              {{ 465, 92, 485,194}, 20009 },
                                              {{ 551, 92, 572,193}, 20010 },
                                              {{ 586, 90, 612,195}, 20011 },
#ifdef FREEMAPZOOM
                                              {{ 473,agmp->resolutiony - ( 480 - 449 ), 601,agmp->resolutiony - ( 480 - 460 )}, 20016 }};
#else
                                              {{ 473,agmp->resolutiony - ( 480 - 449 ), 601,agmp->resolutiony - ( 480 - 460 )}, 20012 }};
#endif

   for ( int i = 0; i< helplist.num; i++ ) {
      sghelpitems[i].rect.x1 = agmp->resolutionx - ( 640 - sghelpitems[i].rect.x1 );
      sghelpitems[i].rect.x2 = agmp->resolutionx - ( 640 - sghelpitems[i].rect.x2 );
   }

   helplist.item = sghelpitems;
}

tsgonlinemousehelp* onlinehelp = NULL;




class tsgonlinemousehelpwind : public tonlinemousehelp {
            public:
                tsgonlinemousehelpwind ( void );
          } ;

tsgonlinemousehelpwind :: tsgonlinemousehelpwind ( void )
{
   helplist.num = 3;

   static tonlinehelpitem sghelpitemswind[3]  = { {{ 501,224, 569,290}, 20013 },
                                                  {{ 589,228, 609,289}, 20014 },
                                                  {{ 489,284, 509,294}, 20015 }};

   for ( int i = 0; i< helplist.num; i++ ) {
      sghelpitemswind[i].rect.x1 = agmp->resolutionx - ( 640 - sghelpitemswind[i].rect.x1 );
      sghelpitemswind[i].rect.x2 = agmp->resolutionx - ( 640 - sghelpitemswind[i].rect.x2 );
   }

   helplist.item = sghelpitemswind;
}

tsgonlinemousehelpwind* onlinehelpwind = NULL;



int  abortgame;

int              modenum8;

int videostartpos = 0;

pprogressbar actprogressbar = NULL;
cmousecontrol* mousecontrol = NULL;

#define messagedisplaytime 300



#define maintainence

int maintainencecheck( void )
{
   #ifdef maintainence
    int num = 0;

    for ( int i = 0; i < 8; i++ )
       if ( actmap->player[i].stat == Player::human )
          num++;

    if ( actmap->campaign )
       num++;

    if ( actmap->network )
       num++;

    if ( num <= 1 )
       return 1;
    else
       return 0;

   #else
    return 0;
   #endif
}   




void showmemory ( void )
{
  #ifdef _DOS_
   npush ( activefontsettings );
   activefontsettings.length = 99;
   activefontsettings.background = 0;
   activefontsettings.color = 14;
   activefontsettings.justify = lefttext;
   activefontsettings.font = schriften.smallarial;
   int mss = getmousestatus();
   if (mss == 2)
      setinvisiblemouserectanglestk ( 0, agmp->resolutiony, 640, agmp->resolutiony + 80 ); 

   int a = maxavail();
   //   int b = _memavl();
   //   showtext2( strrr ( _memmax() ), 10,agmp->resolutiony );
   // showtext2( strrr ( b  ), 110,agmp->resolutiony );
   showtext2( strrr ( a ), 210,agmp->resolutiony );
   // showtext2( strrr ( b+a ), 310,agmp->resolutiony );
   showtext2( strrr ( stackfree ()  ), 410,agmp->resolutiony );

   showtext2( strrr ( getxpos()  ), 10,agmp->resolutiony + 30 );
   showtext2( strrr ( getypos()  ), 110,agmp->resolutiony + 30 );

   showtext2( strrr ( actmap->time.a.turn  ), 10,agmp->resolutiony + 60 );
   showtext2( strrr ( actmap->time.a.move  ), 110,agmp->resolutiony + 60 );
   if (mss == 2)
      getinvisiblemouserectanglestk ();

   npop  ( activefontsettings );
  #endif
};




void         loadcursor(void)
{ 
  int          w;
  char         i;

   {
      tnfilestream stream ( "height2.raw", tnstream::reading );
      for (i=0;i<3 ;i++ )
         for ( int j=0; j<8; j++)
            stream.readrlepict( &icons.height2[i][j], false, &w );
   }

   {
      tnfilestream stream ("windp.raw", tnstream::reading);
      for (i=0;i<9 ;i++ )
          stream.readrlepict( &icons.wind[i], false, &w );
                                                                                                                                                                                                                                                               
   }

  #ifdef HEXAGON
   {
      tnfilestream stream ("windpfei.raw",tnstream::reading);
      stream.readrlepict( &icons.windarrow, false, &w );
   }
  #endif

   {
      tnfilestream stream ("farbe.raw",tnstream::reading);
      for (i=0;i<8 ;i++ )
          stream.readrlepict( &icons.player[i], false, &w );
   }

   {
      tnfilestream stream ("allianc.raw",tnstream::reading);
      for (i=0;i<8 ;i++ ) {
          stream.readrlepict(   &icons.allianz[i][0], false, &w );
          stream.readrlepict(   &icons.allianz[i][1], false, &w );
          stream.readrlepict(   &icons.allianz[i][2], false, &w );
      } /* endfor */
   }

   {
      tnfilestream stream ("weapicon.raw",tnstream::reading);
      for (i=0; i<13 ;i++ )
            stream.readrlepict(   &icons.unitinfoguiweapons[i], false, &w );
   }

   {
      tnfilestream stream ("expicons.raw",tnstream::reading);
      for (i=0; i<=maxunitexperience ;i++ )
            stream.readrlepict(   &icons.experience[i], false, &w );
   }

   {
      #ifndef HEXAGON
      tnfilestream stream ("va8.raw",tnstream::reading);
      #else
      tnfilestream stream ("hexinvi2.raw",tnstream::reading);
      #endif
      stream.readrlepict(   &icons.view.va8, false, &w);
   }

   {
      #ifndef HEXAGON
      tnfilestream stream ("nv8.raw",tnstream::reading);
      #else
      tnfilestream stream ("hexinvis.raw",tnstream::reading);
      #endif
      stream.readrlepict(   &icons.view.nv8, false, &w);
      void* u = uncompress_rlepict ( icons.view.nv8 );
      if ( u ) {
         delete[] icons.view.nv8;
         icons.view.nv8 = u;
      }
   }

   {
      tnfilestream stream ("fg8.raw",tnstream::reading);
      stream.readrlepict(   &icons.view.fog8, false, &w);
   }

   {
      tnfilestream stream ("windrose.raw",tnstream::reading);
      stream.readrlepict(   &icons.windbackground, false, &w);
   }

   if ( actprogressbar )
      actprogressbar->point();


   #ifndef HEXAGON
   char s[100];
   strcpy(s,"nv4-0.raw");
   for (i=0; i<4 ;i++ ) {

       s[0]='n';
       s[1]='v';
       s[4]='0'+i;
       {
          tnfilestream stream (s,tnstream::reading);
          stream.readrlepict(   &view.nv4[i], false, &w);
       }

       s[0]='v';
       s[1]='a';
       {
          tnfilestream stream (s,tnstream::reading);
          stream.readrlepict(   &view.va4[i], false, &w);
       }

       s[0]='f';
       s[1]='g';
       {
          tnfilestream stream (s,tnstream::reading);
          stream.readrlepict(   &view.fog4[i], false, &w);
       }

   } /* endfor */
   mountview();
   #endif



   {
      #ifdef HEXAGON
      tnfilestream stream ("hexfld_a.raw",tnstream::reading);
      #else
      tnfilestream stream ("markacti.raw",tnstream::reading);
      #endif

      stream.readrlepict(   &icons.container.mark.active, false, &w);
   }

   if ( actprogressbar )
      actprogressbar->point();

   {
      #ifdef HEXAGON
      tnfilestream stream ("hexfld.raw",tnstream::reading);
      #else
      tnfilestream stream ("markinac.raw",tnstream::reading);
      #endif
      stream.readrlepict(   &icons.container.mark.inactive, false, &w);
   }

   {
      #ifdef HEXAGON
      tnfilestream stream ("in_ach.raw",tnstream::reading);
      #else
      tnfilestream stream ("markain.raw",tnstream::reading);
      #endif
      stream.readrlepict(   &icons.container.mark.movein_active, false, &w);
   }

   {
      #ifdef HEXAGON
      tnfilestream stream ("in_h.raw",tnstream::reading);
      #else
      tnfilestream stream ("markain2.raw",tnstream::reading);
      #endif
      stream.readrlepict(   &icons.container.mark.movein_inactive, false, &w);
   }

   {
      #ifdef HEXAGON
      tnfilestream stream ("build_ah.raw",tnstream::reading);
      #else
      tnfilestream stream ("baumoda.raw",tnstream::reading);
      #endif
      stream.readrlepict(   &icons.container.mark.repairactive, false, &w);
   }

   {
      #ifdef HEXAGON
      tnfilestream stream ("build_h.raw",tnstream::reading);
      #else
      tnfilestream stream ("baumod.raw",tnstream::reading);
      #endif
      stream.readrlepict(   &icons.container.mark.repairinactive, false, &w);
   }

   {
      #ifdef HEXAGON
      tnfilestream stream ("hexbuild.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.container_window, false, &w);
      #endif
   }

   if ( actprogressbar )
      actprogressbar->point();

   if ( !asc_paletteloaded )
      loadpalette();

   for (w=0;w<256 ;w++ ) {
      palette16[w][0] = pal[w][0];
      palette16[w][1] = pal[w][1];
      palette16[w][2] = pal[w][2];
      xlattables.nochange[w] = w;
   } /* endfor */

   if ( actprogressbar )
      actprogressbar->point();

   loadicons();

   if ( actprogressbar )
      actprogressbar->point();

   loadmessages();

   if ( actprogressbar )
      actprogressbar->point();

   {
      tnfilestream stream ("waffen.raw",tnstream::reading);
      int num;

      static int xlatselectweaponguiicons[12] = { 2, 7, 6, 3, 4, 9, 0, 5, 10, 11, 11, 11 };

      stream.readdata ( (char*) &num, sizeof( num ));
      for ( i = 0; i < num; i++ )
          stream.readrlepict(   &icons.selectweapongui[xlatselectweaponguiicons[i]], false, &w );
      stream.readrlepict(   &icons.selectweaponguicancel, false, &w );
   }

   {
      tnfilestream stream ("knorein.raw",tnstream::reading);
      stream.readrlepict(   &icons.guiknopf, false, &w );
   }

   {
      tnfilestream stream ("compi2.raw",tnstream::reading);
      stream.readrlepict(   &icons.computer, false, &w );
   }

   {
      tnfilestream stream ("pfeil-A0.raw",tnstream::reading);
      for (i=0; i<8 ;i++ ) {
            stream.readrlepict(   &icons.pfeil2[i], false, &w );
      } /* endfor */
   }

   {
      tnfilestream stream ("gebasym2.raw",tnstream::reading);
      for ( i = 0; i < 12; i++ )
         for ( int j = 0; j < 2; j++ )
            stream.readrlepict(   &icons.container.lasche.sym[i][j], false, &w );
   }

   {
      tnfilestream stream ("netcontr.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.netcontrol.start, false, &w );
      stream.readrlepict(   &icons.container.subwin.netcontrol.inactive, false, &w );
      stream.readrlepict(   &icons.container.subwin.netcontrol.active, false, &w );
   }

   {
      tnfilestream stream ("ammoprod.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.ammoproduction.start, false, &w );
      stream.readrlepict(   &icons.container.subwin.ammoproduction.button, false, &w );
      stream.readrlepict(   &icons.container.subwin.ammoproduction.buttonpressed, false, &w );
      for ( i = 0; i < 4; i++ )
         stream.readrlepict(   &icons.container.subwin.ammoproduction.schieber[i], false, &w );
      stream.readrlepict(   &icons.container.subwin.ammoproduction.schiene, false, &w );
   }

   if ( actprogressbar )
      actprogressbar->point();

   {
      tnfilestream stream ("resorinf.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.resourceinfo.start, false, &w );
   }

   {
      tnfilestream stream ("windpowr.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.windpower.start, false, &w );
   }

   {
      tnfilestream stream ("solarpwr.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.solarpower.start, false, &w );

   }

   {
      tnfilestream stream ("ammotran.raw", tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.ammotransfer.start, false, &w );
      stream.readrlepict(   &icons.container.subwin.ammotransfer.button, false, &w );
      stream.readrlepict(   &icons.container.subwin.ammotransfer.buttonpressed, false, &w );
      for ( i = 0; i < 4; i++ )
         stream.readrlepict(   &icons.container.subwin.ammotransfer.schieber[i], false, &w );
      stream.readrlepict(   &icons.container.subwin.ammotransfer.schieneinactive, false, &w );
      stream.readrlepict(   &icons.container.subwin.ammotransfer.schiene, false, &w );
      if ( dataVersion >= 2 ) {
         stream.readrlepict(   &icons.container.subwin.ammotransfer.singlepage[0], false, &w );
         stream.readrlepict(   &icons.container.subwin.ammotransfer.singlepage[1], false, &w );
         stream.readrlepict(   &icons.container.subwin.ammotransfer.plus[0], false, &w );
         stream.readrlepict(   &icons.container.subwin.ammotransfer.plus[1], false, &w );
         stream.readrlepict(   &icons.container.subwin.ammotransfer.minus[0], false, &w );
         stream.readrlepict(   &icons.container.subwin.ammotransfer.minus[1], false, &w );
      }

   }

   {
      tnfilestream stream ("research.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.research.start, false, &w );
      stream.readrlepict(   &icons.container.subwin.research.button[0], false, &w );
      stream.readrlepict(   &icons.container.subwin.research.button[1], false, &w );
      stream.readrlepict(   &icons.container.subwin.research.schieber, false, &w );
   }

   {
      tnfilestream stream ("pwrplnt2.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.conventionelpowerplant.start, false, &w );
      stream.readrlepict(   &icons.container.subwin.conventionelpowerplant.schieber, false, &w );
      //stream.readrlepict(   &icons.container.subwin.conventionelpowerplant.button[1], false, &w );
   }



   int m;
   {
      tnfilestream stream ( "bldinfo.raw", tnstream::reading );
      stream.readrlepict( &icons.container.subwin.buildinginfo.start, false, &m );
      for ( i = 0; i < 8; i++ )
         stream.readrlepict( &icons.container.subwin.buildinginfo.height1[i], false, &m );
      for ( i = 0; i < 8; i++ )
         stream.readrlepict( &icons.container.subwin.buildinginfo.height2[i], false, &m );
      stream.readrlepict( &icons.container.subwin.buildinginfo.repair, false, &m );
      stream.readrlepict( &icons.container.subwin.buildinginfo.repairpressed, false, &m );
      stream.readrlepict( &icons.container.subwin.buildinginfo.block, false, &m );
   }


   {
      tnfilestream stream ("mining2.raw", tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.miningstation.start, false, &w );
      stream.readrlepict(   &icons.container.subwin.miningstation.zeiger, false, &w );
      /*
      for ( i = 0; i < 2; i++ )
         stream.readrlepict(   &icons.container.subwin.miningstation.button[i], false, &w );
      for ( i = 0; i < 2; i++ )
         stream.readrlepict(   &icons.container.subwin.miningstation.resource[i], false, &w );
      for ( i = 0; i < 3; i++ )
         stream.readrlepict(   &icons.container.subwin.miningstation.axis[i], false, &w );
      for ( i = 0; i < 2; i++ )
         stream.readrlepict(   &icons.container.subwin.miningstation.pageturn[i], false, &w );
      stream.readrlepict(   &icons.container.subwin.miningstation.graph, false, &w );
      */
   }

   {
      tnfilestream stream ("mineral.raw", tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.mineralresources.start, false, &w );
      stream.readrlepict(   &icons.container.subwin.mineralresources.zeiger, false, &w );
   }

   {
      tnfilestream stream ("tabmark.raw", tnstream::reading);
      stream.readrlepict (   &icons.container.tabmark[0], false, &w );
      stream.readrlepict (   &icons.container.tabmark[1], false, &w );
   }


   {
      tnfilestream stream ("traninfo.raw", tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.transportinfo.start, false, &w );
      for ( i = 0; i < 8; i++ )
         stream.readrlepict(   &icons.container.subwin.transportinfo.height1[i], false, &w );
      for ( i = 0; i < 8; i++ )
         stream.readrlepict(   &icons.container.subwin.transportinfo.height2[i], false, &w );
      stream.readrlepict(   &icons.container.subwin.transportinfo.sum, false, &w );
   }

   if ( actprogressbar )
      actprogressbar->point();

   {
      tnfilestream stream ("attack.raw", tnstream::reading);
      stream.readrlepict (   &icons.attack.bkgr, false, &w );
      icons.attack.orgbkgr = NULL;
   }

   {
     #ifdef HEXAGON
      tnfilestream stream ("hexfeld.raw", tnstream::reading);
     #else
      tnfilestream stream ("oktfld2.raw", tnstream::reading);
     #endif
      stream.readrlepict ( &icons.fieldshape, false, &w );
   }

   #ifdef FREEMAPZOOM
   {
      tnfilestream stream ("mapbkgr.raw", tnstream::reading);
      stream.readrlepict ( &icons.mapbackground, false, &w );
   }
   #endif

   {
      tnfilestream stream ("hex2oct.raw", tnstream::reading);
      stream.readrlepict (   &icons.hex2octmask, false, &w );
   }

   {
      tnfilestream stream ("weapinfo.raw", tnstream::reading);
      for ( int i = 0; i < 5; i++ )
         stream.readrlepict (   &icons.weaponinfo[i], false, &w );
   }

   backgroundpict.load();

}

enum tuseractions { ua_repainthard,     ua_repaint, ua_help, ua_showpalette, ua_dispvehicleimprovement, ua_mainmenu, ua_mntnc_morefog,
                    ua_mntnc_lessfog,   ua_mntnc_morewind,   ua_mntnc_lesswind, ua_mntnc_rotatewind, ua_changeresourceview, ua_heapcheck,
                    ua_benchgamewv,     ua_benchgamewov,     ua_viewterraininfo, ua_unitweightinfo,  ua_writemaptopcx,  ua_writescreentopcx,
                    ua_scrolltodebugpage, ua_scrolltomainpage, ua_startnewsinglelevel, ua_changepassword, ua_gamepreferences, ua_bi3preferences,
                    ua_exitgame,        ua_newcampaign,      ua_loadgame,  ua_savegame, ua_setupalliances, ua_settribute, ua_giveunitaway,
                    ua_vehicleinfo,     ua_researchinfo,     ua_unitstatistics, ua_buildingstatistics, ua_newmessage, ua_viewqueuedmessages,
                    ua_viewsentmessages, ua_viewreceivedmessages, ua_viewjournal, ua_editjournal, ua_viewaboutmessage, ua_continuenetworkgame,
                    ua_toggleunitshading, ua_computerturn, ua_setupnetwork, ua_howtostartpbem, ua_howtocontinuepbem, ua_mousepreferences,
                    ua_selectgraphicset, ua_UnitSetInfo, ua_GameParameterInfo  };


class tsgpulldown : public tpulldown {
          public:
             void init ( void );
} pd;

void         tsgpulldown :: init ( void )
{
  addfield ( "Glo~b~al" );
   addbutton ( "toggle ~R~esourceviewõ1", ua_changeresourceview );
   addbutton ( "toggle unit shadingõ2", ua_toggleunitshading );
   addbutton ( "seperator", -1);
   addbutton ( "~O~ptions", ua_gamepreferences );
   addbutton ( "~M~ouse options", ua_mousepreferences );
   addbutton ( "seperator", -1);
   addbutton ( "~M~ain MenuõF2", ua_mainmenu );
   addbutton ( "E~x~itõctrl-x", ua_exitgame );


  addfield ("~G~ame");
   addbutton ( "New ~C~ampaign", ua_newcampaign);
   addbutton ( "~N~ew single Levelõctrl-n", ua_startnewsinglelevel );

   addbutton ( "seperator", -1);
   addbutton ( "~L~oad gameõctrl-l", ua_loadgame );
   addbutton ( "~S~ave game", ua_savegame );
   addbutton ( "seperator", -1 );
   addbutton ( "Continue network gameõF3", ua_continuenetworkgame);
   addbutton ( "setup Net~w~ork", ua_setupnetwork );
   addbutton ( "Change ~P~assword", ua_changepassword );
   addbutton ( "seperator", -1 );
   addbutton ( "~P~layers", ua_setupalliances);
   addbutton ( "transfer ~U~nit control", ua_giveunitaway );
   addbutton ( "~T~ransfer resources", ua_settribute);

  addfield ( "~I~nfo" );
   addbutton ( "~V~ehicle types", ua_vehicleinfo );
   addbutton ( "~U~nit weightõ8", ua_unitweightinfo );
   addbutton ( "~T~errainõ7", ua_viewterraininfo );
   addbutton ( "seperator", -1 );
   addbutton ( "~R~esearch", ua_researchinfo );
   addbutton ( "vehicle ~I~mprovementõF7", ua_dispvehicleimprovement);
   addbutton ( "unit ~S~et informationõ6", ua_UnitSetInfo );
   addbutton ( "show game ~P~arameters", ua_GameParameterInfo );


  addfield ( "~S~tatistics" );
   addbutton ( "~U~nits", ua_unitstatistics );
   addbutton ( "~B~uildings", ua_buildingstatistics );
   // addbutton ( "seperator");
   // addbutton ( "~H~istory");

  addfield ( "~M~essage");
   addbutton ( "~n~ew message", ua_newmessage );
   addbutton ( "view ~q~ueued messages", ua_viewqueuedmessages );
   addbutton ( "view ~s~end messages", ua_viewsentmessages );
   addbutton ( "view ~r~eceived messages", ua_viewreceivedmessages);
   addbutton ( "seperator", -1 );
   addbutton ( "view ~j~ournal", ua_viewjournal );
   addbutton ( "~a~ppend to journal", ua_editjournal );

  addfield ( "~T~ools" );
   addbutton ( "save ~M~ap as PCXõ9", ua_writemaptopcx );
   addbutton ( "save ~S~creen as PCXõ0", ua_writescreentopcx );
   addbutton ( "benchmark without view calc", ua_benchgamewov );
   addbutton ( "benchmark with view calc", ua_benchgamewv);
   addbutton ( "test memory integrity", ua_heapcheck );
   addbutton ( "seperator", -1 );
   addbutton ( "select graphic set", ua_selectgraphicset );

  addfield ( "~H~elp" );
   addbutton ( "HowTo ~S~tart email games", ua_howtostartpbem );
   addbutton ( "HowTo ~C~ontinue email games", ua_howtocontinuepbem );
   addbutton ( "seperator", -1);
   addbutton ( "~K~eys", ua_help );

   addbutton ( "~A~bout", ua_viewaboutmessage );

   tpulldown :: init();
   setshortkeys();
}



void         repaintdisplay(void)
{
   collategraphicoperations cgo;
   int mapexist = actmap && (actmap->xsize > 0) && (actmap->ysize > 0);


   int ms = getmousestatus();
   if ( ms == 2 )
      mousevisible ( false );

   int cv = cursor.an;

   if ( mapexist && cv )
      cursor.hide();
   backgroundpict.paint();
   setvgapalette256(pal);

   if ( mapexist ) {
      displaymap();

      if ( cv )
         cursor.show();
   }

   pd.barstatus = false;

   if ( ms == 2 )
      mousevisible ( true );
   dashboard.x = 0xffff;
   dashboard.repainthard = 1;
   if ( actmap && actmap->ellipse )
      actmap->ellipse->paint();
   if ( actgui && actmap && actmap->xsize>0)

      actgui->painticons();

}

void         repaintdisplayhard(void)
{
   reinitgraphics( modenum8 );
   setdisplaystart ( 0, videostartpos  );
   repaintdisplay();
}


void         ladekarte(void)
{
   ASCString s1;

   mousevisible(false);
   fileselectsvga(mapextension, &s1, 1 );

   if ( !s1.empty() ) {
      mousevisible(false);
      cursor.hide();
      displaymessage("loading map %s",0, s1.c_str() );
      loadmap(s1.c_str());
      actmap->startGame();

      next_turn();

      removemessage();
      if (actmap->campaign != NULL) {
         delete  ( actmap->campaign );
         actmap->campaign = NULL;
      }

      displaymap();
      dashboard.x = 0xffff;
      moveparams.movestatus = 0;
   }
   mousevisible(true);
}


void         ladespiel(void)
{
   mousevisible(false);

   ASCString s1;
   fileselectsvga(savegameextension, &s1, 1 );

   if ( !s1.empty() ) {
      mousevisible(false);
      cursor.hide();
      displaymessage("loading %s ",0, s1.c_str());
      loadgame(s1.c_str() );
      removemessage();
      if ( !actmap || actmap->xsize == 0 || actmap->ysize == 0 )
         throw  NoMapLoaded();

      if ( actmap->network )
         setallnetworkpointers ( actmap->network );

      computeview( actmap );
      displaymap();
      dashboard.x = 0xffff;
      moveparams.movestatus = 0;
   }
   mousevisible(true);
}


void         speicherspiel( int as )
{
   ASCString s1;

   int nameavail = 0;
   if ( !actmap->preferredFileNames.savegame[actmap->actplayer].empty() )
      nameavail = 1;


   if ( as || !nameavail ) {
      mousevisible(false);
      fileselectsvga(savegameextension, &s1, 0);
   } else
      s1 = actmap->preferredFileNames.savegame[actmap->actplayer];

   if ( !s1.empty() ) {
      actmap->preferredFileNames.savegame[actmap->actplayer] = s1;

      mousevisible(false);
      cursor.hide();
      displaymessage("saving %s", 0, s1.c_str());
      savegame(s1.c_str());

      removemessage();
      displaymap();
      cursor.show();
   }
   mousevisible(true);
}


void         showpalette(void)
{
   bar ( 0, 0, 639, 479, black );
   int x=7;

   for ( char a = 0; a <= 15; a++)
      for ( char b = 0; b <= 15; b++) {
         bar(     a * 40, b * 20,a * 40 +  x,b * 20 + 20, xlattables.light    [a * 16 + b]);
         bar( x + a * 40, b * 20,a * 40 + 2*x,b * 20 + 20, xlattables.nochange [a * 16 + b]);
         bar(2*x + a * 40, b * 20,a * 40 + 3*x,b * 20 + 20, xlattables.a.dark1    [a * 16 + b]);
         bar(3*x + a * 40, b * 20,a * 40 + 4*x,b * 20 + 20, xlattables.a.dark2    [a * 16 + b]);
         bar(4*x + a * 40, b * 20,a * 40 + 5*x,b * 20 + 20, xlattables.a.dark3    [a * 16 + b]);
      }
   wait();
   repaintdisplay();
}







void         newcampaign(void)
{
   tchoosenewcampaign tnc;
   tnc.init();
   tnc.run();
   tnc.done();
}



void         newsinglelevel(void)
{
  tchoosenewsinglelevel tnc;

   tnc.init();
   tnc.run();
   tnc.done();
}

void         startnewsinglelevelfromgame(void)
{
    cursor.hide();
    newsinglelevel();
    computeview( actmap );
    displaymap();
    cursor.show();
}



void loadStartupMap ( const char *gameToLoad=NULL )
{
   if ( gameToLoad && gameToLoad[0] ) {
      if ( patimat ( tournamentextension, gameToLoad )) {

         if( validateemlfile( gameToLoad ) == 0 ) {
            fprintf( stderr, "Email gamefile %s is invalid. Aborting.\n", gameToLoad );
            exit(-1);
         }

         try {
            tnfilestream gamefile ( gameToLoad, tnstream::reading );
            tnetworkloaders nwl;
            nwl.loadnwgame( &gamefile );
         }

         catch ( tfileerror ) {
            fprintf ( stderr, "%s is not a legal email game. \n", gameToLoad );
            exit(-1);
         }
      } else if( patimat ( savegameextension, gameToLoad )) {
         if( validatesavfile( gameToLoad ) == 0 ) {
            fprintf( stderr, "The savegame %s is invalid. Aborting.\n", gameToLoad );
            exit( -1 );
         }
         try { loadgame( gameToLoad ); }

         catch ( tfileerror ) {
            fprintf ( stderr, "%s is not a legal savegame. \n", gameToLoad );
            exit(-1);
         }

      } else if( patimat ( mapextension, gameToLoad )) {
         if( validatemapfile( gameToLoad ) == 0 ) {
            fprintf( stderr, "Mapfile %s is invalid. Aborting.\n", gameToLoad );
            exit(-1);
         }

         try { loadmap( gameToLoad ); }
         catch ( tfileerror ) {
            fprintf ( stderr, "%s is not a legal map. \n", gameToLoad );
            exit(-1);
         }
      } else {
         fprintf ( stderr, "Don't know how to handle the file %s \n", gameToLoad );
         exit(-1);
      }

   } else {  // resort to loading defaults

     ASCString s;
     if ( CGameOptions::Instance()->startupcount < 4 ) {
        s = "tutor0" ;
     } else {
        s = "railstat";
     }
     s += &mapextension[1];

     int maploadable;
     {
        tfindfile ff ( s );
        string filename = ff.getnextname();
        maploadable = validatemapfile ( filename.c_str() );
     }

     if ( !maploadable ) {

         tfindfile ff ( mapextension );
         string filename = ff.getnextname();

         if ( filename.empty() )
            displaymessage( "unable to load startup-map",2);

         while ( !validatemapfile ( filename.c_str() ) ) {
            filename = ff.getnextname();
            if ( filename.empty() )
               displaymessage( "unable to load startup-map",2);

         }
         s = filename;
     }

     loadmap(s.c_str());

     displayLogMessage ( 6, "initializing map..." );
     actmap->startGame();
     displayLogMessage ( 6, "done\n Setting up Resources..." );
     actmap->setupResources();
     displayLogMessage ( 6, "done\n" );
   }
}


void benchgame ( int mode )
{
   int t = ticker;
   int n = 0;
   int t2 = t;
   do {
      if ( mode <= 1 ) {
         if ( mode == 1 )
            computeview( actmap );
         displaymap();
      } else {
        #ifndef _DOS_
        copy2screen();
        #endif
      }
      n++;
      t2 = ticker;
   } while ( t + 1000 > t2 ); /* enddo */
   double d = 100 * n;
   d /= (t2-t);
   char buf[100];
   sprintf ( buf, "%3.1f", d );
   displaymessage2 ( " %s fps ", buf );
}


void selectgraphicset ( void )
{
   ASCString filename;
   fileselectsvga("*.gfx",&filename,1);
   if ( !filename.empty() ) {
      int id = getGraphicSetIdFromFilename ( filename.c_str() );
      if ( id != actmap->graphicset ) {
         actmap->graphicset = id;
         displaymap();
      }
   }
}


void execuseraction ( tuseractions action )
{
   switch ( action ) {
            case ua_repainthard  :   repaintdisplayhard();
            break;

            case ua_repaint      :   repaintdisplay();
            break;

            case ua_help         :   help(20);
            break;

            case ua_howtostartpbem :   help(21);
            break;

            case ua_howtocontinuepbem :   help(22);
            break;

            case ua_showpalette  :   showpalette();
            break;

            case ua_dispvehicleimprovement    : displaymessage("Research:\n%s %d \n%s %d \n%s %d \n"
                                                                          "%s %d \n%s %d \n%s %d \n"
                                                                          "%s %d \n%s %d \n%s %d \n"
                                                                          "%s %d",1,
                                                  cwaffentypen[0], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[0]),
                                                  cwaffentypen[1], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[1]),
                                                  cwaffentypen[2], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[2]),
                                                  cwaffentypen[3], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[3]),
                                                  cwaffentypen[4], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[4]),
                                                  cwaffentypen[5], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[5]),
                                                  cwaffentypen[6], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[6]) ,
                                                  cwaffentypen[7], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[7])  ,
                                                  cwaffentypen[10], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[10])     ,
                                                  "armor",         (actmap->player[actmap->actplayer].research.unitimprovement.armor));
                                                  break;

            case ua_mainmenu:  if (choice_dlg("do you really want to close the current game ?","~y~es","~n~o") == 1) {
                                  delete actmap;
                                   actmap = NULL;
                                   throw NoMapLoaded();
                               }
                               break;

            case ua_mntnc_morefog: if (actmap->weather.fog < 255   && maintainencecheck() ) {
                                      actmap->weather.fog++;
                                      computeview( actmap );
                                      displaymessage2("fog intensity set to %d ", actmap->weather.fog);
                                      displaymap();
                                   }
                        break;

            case ua_mntnc_lessfog: if (actmap->weather.fog  && maintainencecheck()) {
                                      actmap->weather.fog--;
                                      computeview( actmap );
                                      displaymessage2("fog intensity set to %d ", actmap->weather.fog);
                                      displaymap();
                                   }
                        break;

            case ua_mntnc_morewind: if ((actmap->weather.wind[0].speed < 254) &&  maintainencecheck()) {
                                       actmap->weather.wind[0].speed+=2;
                                       actmap->weather.wind[2].speed = actmap->weather.wind[1].speed = actmap->weather.wind[0].speed;
                                       displaywindspeed (  );
                                       dashboard.x = 0xffff;
                                    }
                         break;

            case ua_mntnc_lesswind: if ((actmap->weather.wind[0].speed > 1)  && maintainencecheck() ) {
                                       actmap->weather.wind[0].speed-=2;
                                       actmap->weather.wind[2].speed = actmap->weather.wind[1].speed = actmap->weather.wind[0].speed;
                                       displaywindspeed (  );
                                       dashboard.x = 0xffff;
                                    }
                         break;

            case ua_mntnc_rotatewind:  if ( maintainencecheck() ) {
                                          if (actmap->weather.wind[0].direction < sidenum-1 )
                                             actmap->weather.wind[0].direction++;
                                          else
                                             actmap->weather.wind[0].direction = 0;
                                          actmap->weather.wind[2].direction = actmap->weather.wind[1].direction = actmap->weather.wind[0].direction;
                                          displaymessage2("wind dir set to %d ", actmap->weather.wind[0].direction);
                                          dashboard.x = 0xffff;
                                          resetallbuildingpicturepointers();
                                          displaymap();
                                       }
                                       break;

            case ua_changeresourceview:  showresources++;
                                         if ( showresources >= 3 )
                                            showresources = 0;
                                         displaymap();
                             break;

            case ua_heapcheck:
#ifdef _DOS_
         if ( _heapchk() == _HEAPOK )
      displaymessage(" Heap OK", 3 );
         else
      displaymessage(" Heap not OK", 1 );
#endif
         break;
#ifdef _DOS_
            case ua_benchgamewov:  benchgame( 0 );
#else
            case ua_benchgamewov:  benchgame( 2 );
#endif
            break;

            case ua_benchgamewv :  benchgame( 1 );
            break;

            case ua_viewterraininfo:  viewterraininfo();
                          break;

         case ua_unitweightinfo:  if ( fieldvisiblenow  ( getactfield() )) {
                                     pvehicle eht = getactfield()->vehicle;
                                     if ( eht && getdiplomaticstatus ( eht->color ) == capeace )
                                       displaymessage(" weight of unit: \n basic: %d\n+fuel: %d\n+material:%d\n+cargo:%d\n= %d",1 ,eht->typ->weight, eht->tank.fuel * resourceWeight[Resources::Fuel] / 1024 , eht->tank.material * resourceWeight[Resources::Material] / 1024, eht->cargo(), eht->weight() );
                                  }
                         break;

        case ua_writemaptopcx : writemaptopcx ();
                                break;

        case ua_writescreentopcx: {
                                      char* nm = getnextfilenumname ( "screen", "pcx", 0 );
                                      writepcx ( nm, 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, pal );
                                      displaymessage2( "screen saved to %s", nm );
                                   }
                           break;

         case ua_scrolltodebugpage: if (videostartpos < 160) {
                                          videostartpos += 20;
                                          setdisplaystart ( 0, videostartpos  );
                                       }
                          break;

         case ua_scrolltomainpage : if (videostartpos >  0) {
                                       videostartpos -= 20;
                                       setdisplaystart ( 0, videostartpos );
                                    }
                       break;

         case ua_startnewsinglelevel: startnewsinglelevelfromgame();
                       break;

         case ua_changepassword:      {
                                         bool success;
                                         do {
                                            Password oldpwd = actmap->player[actmap->actplayer].passwordcrc;
                                            actmap->player[actmap->actplayer].passwordcrc.reset();
                                            success = enterpassword ( actmap->player[actmap->actplayer].passwordcrc, true, true );
                                            if ( !success )
                                               actmap->player[actmap->actplayer].passwordcrc = oldpwd;
                                         } while ( actmap->player[actmap->actplayer].passwordcrc.empty() && success && viewtextquery ( 910, "warning", "~e~nter password", "~c~ontinue without password" ) == 0 ); /* enddo */
                                      }
                       break;

         case ua_gamepreferences:     gamepreferences();
                       break;

         case ua_mousepreferences:    mousepreferences();
                       break;

         case ua_bi3preferences:      bi3preferences();
                       break;

         case ua_exitgame:            if (choice_dlg("do you really want to quit ?","~y~es","~n~o") == 1)
                                         abortgame = 1;
                       break;

         case ua_newcampaign:         cursor.hide();
                                      newcampaign();
                                      computeview( actmap );
                                      displaymap();
                                      cursor.show();
                       break;

         case ua_loadgame:            ladespiel();
                       break;

        case ua_savegame:             speicherspiel( 1 );
                       break;

        case ua_setupalliances:       setupalliances();
                                      logtoreplayinfo ( rpl_alliancechange );
                                      logtoreplayinfo ( rpl_shareviewchange );

                                      if ( actmap->shareview && actmap->shareview->recalculateview ) {
                                         logtoreplayinfo ( rpl_shareviewchange );
                                         computeview( actmap );
                                         actmap->shareview->recalculateview = 0;
                                         displaymap();
                                      }
                                      dashboard.x = 0xffff;
                       break;

        case ua_settribute :          settributepayments ();
                       break;

        case ua_giveunitaway:         giveunitaway ();
                       break;

        case ua_vehicleinfo:          activefontsettings.font = schriften.smallarial;
                                      vehicle_information();
                       break;

        case ua_researchinfo:         researchinfo ();
                       break;

        case ua_unitstatistics:       statisticarmies();
                       break;

        case ua_buildingstatistics:   statisticbuildings();
                       break;

        case ua_newmessage:           newmessage();
                       break;

        case ua_viewqueuedmessages:   viewmessages( "queued messages", actmap->unsentmessage, 1, 0 );
                       break;

        case ua_viewsentmessages:     viewmessages( "sent messages", actmap->player[ actmap->actplayer ].sentmessage, 0, 0 );
                       break;

        case ua_viewreceivedmessages: viewmessages( "received messages", actmap->player[ actmap->actplayer ].oldmessage, 0, 1 );
                       break;

        case ua_viewjournal:          viewjournal();
                       break;

        case ua_editjournal:          editjournal();
                       break;

        case ua_viewaboutmessage:     {
                                         help(30);
                                         tviewanytext vat;
                                         ASCString s = getstartupmessage();

                                         #ifdef _SDL_
                                         char buf[1000];
                                         SDL_version compiled;
                                         SDL_VERSION(&compiled);
                                         sprintf(buf, "\nCompiled with SDL version: %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
                                         s += buf;
                                         // #endif

                                         sprintf(buf, "Linked with SDL version: %d.%d.%d\n", SDL_Linked_Version()->major, SDL_Linked_Version()->minor, SDL_Linked_Version()->patch);
                                         s += buf;
                                         #endif

                                         vat.init ( "about", s.c_str() );
                                         vat.run();
                                         vat.done();
                                      }
                       break;

        case ua_continuenetworkgame:   continuenetworkgame();
                                       displaymap();
                       break;

        case ua_toggleunitshading:     CGameOptions::Instance()->units_gray_after_move = !CGameOptions::Instance()->units_gray_after_move;
                                       CGameOptions::Instance()->setChanged();
                                       displaymap();
                                       while ( mouseparams.taste )
                                          releasetimeslice();

                                       if ( CGameOptions::Instance()->units_gray_after_move )
                                          displaymessage("units that can not move will now be displayed gray", 3);
                                       else
                                          displaymessage("units that can not move and cannot shoot will now be displayed gray", 3);
                       break;

        case ua_computerturn:          if ( maintainencecheck() || 1 ) {
                                          displaymessage("This function is under development and for programmers only\n"
                                                         "unpredicatable things may happen ...",3 ) ;

                                          if (choice_dlg("do you really want to start the AI?","~y~es","~n~o") == 1) {

                                             if ( !actmap->player[ actmap->actplayer ].ai )
                                                actmap->player[ actmap->actplayer ].ai = new AI ( actmap, actmap->actplayer );

                                             savegame ( "aistart.sav" );
                                             actmap->player[ actmap->actplayer ].ai->run();
                                          }
                                       }
                       break;
        case ua_setupnetwork:       if ( actmap->network )
                                       setupnetwork ( actmap->network );
                                    else
                                       displaymessage("This map is not played across a network",3 );
                       break;
        case ua_selectgraphicset:   selectgraphicset();
                       break;
        case ua_UnitSetInfo:        viewUnitSetinfo();
                       break;
        case ua_GameParameterInfo: showGameParameters();
                       break;
    }


}

void checkpulldown( tkey* ch )
{
   pd.key = *ch;
   pd.checkpulldown();

   if (pd.action2execute >= 0 ) {
      tuseractions ua = (tuseractions) pd.action2execute;
      pd.action2execute = -1;
      execuseraction ( ua );
   }
}

void mainloopgeneralkeycheck ( tkey& ch )
{
    ch = r_key();
    checkpulldown( &ch );

    movecursor(ch);
    actgui->checkforkey ( ch );
}




void mainloopgeneralmousecheck ( void )
{
    if ( exitprogram )
       execuseraction ( ua_exitgame );
    actgui->checkformouse();

    dashboard.checkformouse();

//  if (lasttick + 5 < ticker)
      if ((dashboard.x != getxpos()) || (dashboard.y != getypos())) {
         mousevisible(false);

         dashboard.paint ( getactfield(), actmap->playerView );
         actgui->painticons();

         mousevisible(true);
      }

   if ( lastdisplayedmessageticker + messagedisplaytime < ticker )
      displaymessage2("");

   if ( mousecontrol )
      mousecontrol->chkmouse();

   {
      int oldx = actmap->xpos;
      int oldy = actmap->ypos;
      checkformousescrolling();
      if ( oldx != actmap->xpos || oldy != actmap->ypos )
         dashboard.x = 0xffff;
   }

   if ( onlinehelp )
      onlinehelp->checkforhelp();
   if ( onlinehelpwind && !CGameOptions::Instance()->smallmapactive )
      onlinehelpwind->checkforhelp();

   releasetimeslice();
}

void viewunreadmessages ( void )
{
   MessagePntrContainer::iterator mi = actmap->player[ actmap->actplayer ].unreadmessage.begin();
   while ( mi != actmap->player[ actmap->actplayer ].unreadmessage.end()  ) {
      viewmessage ( **mi );
      actmap->player[ actmap->actplayer ].oldmessage.push_back ( *mi );
      mi = actmap->player[ actmap->actplayer ].unreadmessage.erase ( mi );
   }
}



class WeaponRange : public SearchFields {
       public:
         int run ( const pvehicle veh );
         void testfield ( const MapCoordinate& mc ) { gamemap->getField( mc )->tempw = 1; };
         WeaponRange ( pmap _gamemap ) : SearchFields ( _gamemap ) {};
};

int  WeaponRange :: run ( const pvehicle veh )
{
   int found = 0;
   if ( fieldvisiblenow ( getfield ( veh->xpos, veh->ypos )))
      for ( int i = 0; i < veh->typ->weapons.count; i++ ) {
         if ( veh->typ->weapons.weapon[i].shootable() ) {
            initsearch ( veh->getPosition(), veh->typ->weapons.weapon[i].maxdistance/minmalq, (veh->typ->weapons.weapon[i].mindistance+maxmalq-1)/maxmalq );
            startsearch();
            found++;
         }
      }
   return found;
}


void viewunitweaponrange ( const pvehicle veh, tkey taste )
{
   if ( veh && !moveparams.movestatus  ) {
      actmap->cleartemps ( 7 );
      WeaponRange wr ( actmap );
      int res = wr.run ( veh );
      if ( res ) {
         displaymap();

         #ifndef NEWKEYB
         taste = ct_invvalue;
         #endif

         if ( taste != ct_invvalue ) {
            while ( skeypress ( taste )) {

               while ( keypress() )
                  r_key();
            }
         } else {
            int mb = mouseparams.taste;
            while ( mouseparams.taste == mb )
               releasetimeslice();
         }
         actmap->cleartemps ( 7 );
         displaymap();
      }
   }
}

void viewunitmovementrange ( pvehicle veh, tkey taste )
{
   if ( veh && !moveparams.movestatus && fieldvisiblenow ( getfield ( veh->xpos, veh->ypos ))) {
      actmap->cleartemps ( 7 );
      int move = veh->getMovement();
      veh->setMovement ( veh->typ->movement[log2(veh->height)]);
      VehicleMovement vm ( NULL, NULL );
      if ( vm.available ( veh )) {
         vm.execute ( veh, -1, -1, 0, -1, -1 );
         if ( vm.reachableFields.getFieldNum()) {
            for  ( int i = 0; i < vm.reachableFields.getFieldNum(); i++ )
               vm.reachableFields.getField ( i )->a.temp = 1;
            for  ( int j = 0; j < vm.reachableFieldsIndirect.getFieldNum(); j++ )
               vm.reachableFieldsIndirect.getField ( j )->a.temp = 1;

            displaymap();

            #ifndef NEWKEYB
            taste = ct_invvalue;
            #endif

            if ( taste != ct_invvalue ) {
               while ( skeypress ( taste )) {

                  while ( keypress() )
                     r_key();
               }
            } else {
               int mb = mouseparams.taste;
               while ( mouseparams.taste == mb )
                  releasetimeslice();
            }
            actmap->cleartemps ( 7 );
            displaymap();
         }
      }
      veh->setMovement ( move );
   }
}

extern void testland();
extern void testtext( TerrainType* pt, ObjectType* po );

void  mainloop ( void )
{
   tkey ch;
   abortgame = 0;

   do {
      viewunreadmessages();
      activefontsettings.background=0;
      activefontsettings.length=50;
      activefontsettings.color=14;
      if (keypress()) {

         mainloopgeneralkeycheck ( ch );

         switch (ch) {

            #ifndef NEWKEYB
            case 'R':   execuseraction ( ua_repainthard );
               break;
            #else
            case ct_shp + ct_r:   execuseraction ( ua_repainthard );
               break;
            #endif

            case ct_stp + ct_l: execuseraction ( ua_loadgame );
               break;

            case ct_stp + ct_n: execuseraction ( ua_startnewsinglelevel );
               break;

            case ct_r:  execuseraction ( ua_repaint );
               break;

            case ct_f1:  execuseraction ( ua_help );
               break;

            case ct_f2:  execuseraction ( ua_mainmenu );
               break;

            case ct_f3:  execuseraction ( ua_continuenetworkgame );
               break;

            case ct_f4:  execuseraction ( ua_computerturn );
               break;

            case ct_f7:  execuseraction ( ua_dispvehicleimprovement );
               break;

            case ct_f8:  {
                            int color;
                            if ( getactfield()->vehicle )
                               color = getactfield()->vehicle->color / 8 ;
                            else
                               color = actmap->actplayer;

                            if ( actmap->player[color].ai ) {
                                AI* ai = (AI*) actmap->player[color].ai;
                                ai->showFieldInformation ( getxpos(), getypos() );
                            } else {
                                int x = getxpos();
                                int y = getypos();
                                displaymessage("cursorposition: %d / %d", 3, x, y );

                                if ( getactfield()->vehicle ) {
                                   displaymessage("unitposition: %d / %d\nnetworkid: %d", 3, getactfield()->vehicle->xpos, getactfield()->vehicle->ypos, getactfield()->vehicle->networkid );
                                   displaymessage("number of units: %d", 3, actmap->player[getactfield()->vehicle->color/8].vehicleList.size() );
                                }
                            }

                         }
               break;

            case ct_1:  execuseraction ( ua_changeresourceview );
               break;

            case ct_2:  execuseraction ( ua_toggleunitshading );
               break;

            case ct_3:  viewunitweaponrange ( getactfield()->vehicle, ct_3 );
               break;

            case ct_4:  viewunitmovementrange ( getactfield()->vehicle, ct_4 );
               break;

            case ct_6:  execuseraction ( ua_UnitSetInfo );
               break;

            case ct_7:  execuseraction ( ua_viewterraininfo );
               break;

            case ct_8:  execuseraction ( ua_unitweightinfo );
               break;

            case ct_9: {
                          // testtext ( getterraintype_forid ( 1011 ), getobjecttype_forid ( 1 ) );
                                     /*
                          static pvehicle veh = 0;
                          if ( !veh ) {
                             veh = getactfield()->vehicle;
                          } else {
                             actmap->cleartemps ( 15 );
                             std::vector<MapCoordinate3D> path;
                             AStar3D ast ( actmap, veh );
                             ast.findAllAccessibleFields ( veh->tank.fuel / veh->typ->fuelConsumption * maxmalq );

                             for ( int x = 0; x < actmap->xsize; x++ )
                                for ( int y = 0; y < actmap->ysize; y++ )
                                    if ( !(getfield( x,y )->a.temp & chfahrend) )
                                        getfield( x,y )->a.temp = 0;
                                       */
/*
                             int x = veh->xpos;
                             int y = veh->ypos;
                             actmap->cleartemps( 15 );
                             for ( int i = 0; i < path.size(); i++ )
                                actmap->getField ( path[i] )->a.temp = 1;

                             AI ai ( actmap, actmap->actplayer );
                             int res = ai.moveUnit ( veh, path );
                             if ( res < 0 )
                                displaymessage("error", 1 );
                                */
                             /*
                                if ( !getfield ( x, y ))
                                   break;

                                getfield ( x, y )->a.temp = 1;
                             } */

                             /*
                             for ( int xp = 0; xp < actmap->xsize; xp++ )
                                for ( int yp = 0; yp < actmap->ysize; yp++ )
                                   if ( ast.fieldVisited ( xp, yp ))
                                      getfield ( xp, yp )->a.temp = 1;
                             */
                             /*
                             displaymap();
                             veh = NULL;

                          }  */
                          /*
                          static AStar* ast = 0;
                          if ( ast ) {
                             delete ast;
                             ast =  NULL;
                          } else {
                             ast = new AStar ( actmap, getactfield()->vehicle );
                             ast->findAllAccessibleFields ( );
                             displaymap();
                          } */
                          // testland();
                       }
               break;

            case ct_0:  execuseraction ( ua_writescreentopcx );
               break;

            case ct_point: execuseraction ( ua_scrolltodebugpage );
               break;

            case ct_komma: execuseraction ( ua_scrolltomainpage );
               break;

            case ct_x + ct_stp:   execuseraction ( ua_exitgame );
               break;
         }

      } else
         ch = ct_invvalue;


      mainloopgeneralmousecheck ( );

/************************************************************************************************/
/*        Pulldown Men?                                                                       . */
/************************************************************************************************/

      checkpulldown( &ch );

      while ( actmap->player[ actmap->actplayer ].queuedEvents )
        checkevents( &defaultMapDisplay );

      checktimedevents( &defaultMapDisplay );

      checkforvictory();


   }  while ( !abortgame );
}



pfont load_font ( const char* name )
{
   tnfilestream stream ( name , tnstream::reading );
   return loadfont ( &stream );
}

#ifdef HEXAGON
const char* progressbarfilename = "progress.6mn";
#else
const char* progressbarfilename = "progress.8mn";
#endif







void loaddata( int resolx, int resoly, const char *gameToLoad=NULL )
{
   actprogressbar = new tprogressbar;
   if ( actprogressbar ) {
      tfindfile ff ( progressbarfilename );
      if ( !ff.getnextname().empty() ) {
         tnfilestream strm ( progressbarfilename, tnstream::reading );
         actprogressbar->start ( 255, 0, agmp->resolutiony-3, agmp->resolutionx-1, agmp->resolutiony-1, &strm );
      } else {
         actprogressbar->start ( 255, 0, agmp->resolutiony-3, agmp->resolutionx-1, agmp->resolutiony-1, NULL );
      }
   }

   schriften.smallarial = load_font("smalaril.fnt");
   schriften.large = load_font("usablack.fnt");
   schriften.arial8 = load_font("arial8.fnt");
   schriften.smallsystem = load_font("msystem.fnt");
   schriften.guifont = load_font("gui.fnt");
   schriften.guicolfont = load_font("guicol.fnt");
   schriften.monogui = load_font("monogui.fnt");
   activefontsettings.markfont = schriften.guicolfont;
   shrinkfont ( schriften.guifont, -1 );
   shrinkfont ( schriften.guicolfont, -1 );
   shrinkfont ( schriften.monogui, -1 );
   pulldownfont = schriften.smallarial ;

   if ( actprogressbar ) actprogressbar->startgroup();

   SoundList::init();

   if ( actprogressbar ) actprogressbar->startgroup();

   loadcursor();

   if ( actprogressbar ) actprogressbar->startgroup();

   loadalltextfiles();

   if ( actprogressbar ) actprogressbar->startgroup();
   loadguipictures();
   loadallobjecttypes();

   if ( actprogressbar ) actprogressbar->startgroup();
   loadallvehicletypes();
   loadUnitSets();

   if ( actprogressbar ) actprogressbar->startgroup();
   loadallbuildingtypes();


   if ( actprogressbar ) actprogressbar->startgroup();
   loadalltechnologies();


   if ( actprogressbar ) actprogressbar->startgroup();
   loadstreets();

   if ( actprogressbar ) actprogressbar->startgroup();
   loadallterraintypes();

   if ( actprogressbar ) actprogressbar->startgroup();

   freetextdata();

   cursor.init();
   selectbuildinggui.init( resolx, resoly );
   selectobjectcontainergui.init( resolx, resoly );
   selectvehiclecontainergui.init( resolx, resoly );

#ifndef FREEMAPZOOM
   idisplaymap.setup_map_mask ( );
#endif

   if ( actprogressbar ) actprogressbar->startgroup();

   loadStartupMap( gameToLoad );

   if ( actprogressbar ) actprogressbar->startgroup();

   displayLogMessage ( 6, "loading gui icons..." );
   gui.starticonload();
   displayLogMessage ( 6, "done\n" );

   if ( actprogressbar ) actprogressbar->startgroup();

   dashboard.allocmem ();

   mousecontrol = new cmousecontrol;

   if ( actprogressbar ) {
      actprogressbar->end();
      try {
         tnfilestream strm ( progressbarfilename, tnstream::writing );
         actprogressbar->writetostream( &strm );
      } /* endtry */
      catch ( tfileerror ) { } /* endcatch */

      delete actprogressbar;
      actprogressbar = NULL;
   }
}




void networksupervisor ( void )
{
   class tcarefordeletionofmap {
          public:
            ~tcarefordeletionofmap (  )
            {
                if ( actmap && (actmap->xsize > 0  ||  actmap->ysize > 0) ) {
                     delete actmap;
                     actmap = NULL;
                }
            };
         } carefordeletionofmap;


   tlockdispspfld ldsf;

   tnetwork network;

   int stat;
   do {
      stat = setupnetwork( &network, 1+8 );
      if ( stat == 1 )
         return;

   } while ( (network.computer[0].receive.transfermethod == 0) || (network.computer[0].receive.transfermethodid != network.computer[0].receive.transfermethod->getid()) ); /* enddo */


   try {
       displaymessage ( " starting data transfer ",0);

       network.computer[0].receive.transfermethod->initconnection ( TN_RECEIVE );
       network.computer[0].receive.transfermethod->inittransfer ( &network.computer[0].receive.data );

       tnetworkloaders nwl;
       nwl.loadnwgame ( network.computer[0].receive.transfermethod->stream );

       network.computer[0].receive.transfermethod->closetransfer();
       network.computer[0].receive.transfermethod->closeconnection();

       removemessage();
       if ( actmap->network )
          setallnetworkpointers ( actmap->network );
   } /* endtry */

   catch ( tfileerror ) {
      displaymessage ("a file error occured while loading game",1 );
      delete actmap;
      actmap = NULL;
      return;
   } /* endcatch */
   catch ( ASCexception ) {
      displaymessage ("error loading game",1 );
      delete actmap;
      actmap = NULL;
      return;
   } /* endcatch */


   int ok = 0;
   if ( !actmap->supervisorpasswordcrc.empty() ) {
       ok = enterpassword ( actmap->supervisorpasswordcrc );
   } else {
      displaymessage ("no supervisor defined",1 );
      delete actmap;
      actmap = NULL;
      return;
   }

   if ( ok ) {
      npush ( actmap->actplayer );
      actmap->actplayer = -1;
      setupalliances( 1 );
      npop ( actmap->actplayer );

      do {
         stat = setupnetwork( &network, 2+8 );
         if ( stat == 1 ) {
            displaymessage ("no changes were saved",1 );
            delete actmap;
            actmap = NULL;
            return;
         }

      } while ( (network.computer[0].send.transfermethod == 0) || (network.computer[0].send.transfermethodid != network.computer[0].send.transfermethod->getid()) ); /* enddo */

      tnetworkcomputer* compi = &network.computer[ 0 ];

      displaymessage ( " starting data transfer ",0);

      try {
         compi->send.transfermethod->initconnection ( TN_SEND );
         compi->send.transfermethod->inittransfer ( &compi->send.data );

         tnetworkloaders nwl;
         nwl.savenwgame ( compi->send.transfermethod->stream );

         compi->send.transfermethod->closetransfer();
         compi->send.transfermethod->closeconnection();
      } /* endtry */
      catch ( tfileerror ) {
         displaymessage ( "a file error occured while saving file", 1 );
      } /* endcatch */
      catch ( ASCexception ) {
         displaymessage ( "error saving file", 1 );
      } /* endcatch */

      delete actmap;
      actmap = NULL;
      displaymessage ( "data transfer finished",1);

   } else {
      displaymessage ("no supervisor defined or invalid password",1 );
      delete actmap;
      actmap = NULL;
   }

}





void runmainmenu ( void )
{
    const char* mainmenuitems[6] = { "new map", "new campaign", "load game", "continue network game", "network supervisor", "exit" };

    int mainmenuitemnum = 6;

    int unitsearched = 30;
    pvehicletype unit = getvehicletype_forid ( unitsearched );
    int xz = 228;
    int xs = 100;
    int ys = 25;
    int yd = 35;
    int y1 = 220;
    int xud = 40;
    int yud = -10;
    mousevisible ( false );
    backgroundpict.paint();
    activefontsettings.font = schriften.arial8;
    activefontsettings.background = 255;
    activefontsettings.justify = centertext;
    activefontsettings.height = 0;
    activefontsettings.length = 2*xs;
    for ( int i = 0; i < mainmenuitemnum; i++) {
       bar ( xz - xs, y1 + i * yd, xz + xs, y1 + i * yd + ys, dblue );
       showtext2 ( mainmenuitems[i], xz - xs, y1 + i * yd + ys/2 - activefontsettings.font->height / 2 );
    } /* endfor */
    int stat = -1;
    int pos = -1;
    int oldpos = -1;
    void* bkgr[2] ;
    int tnksize = imagesize ( 0, 0, fieldsizex, fieldsizey );
    bkgr[0] = asc_malloc ( tnksize );
    bkgr[1] = asc_malloc ( tnksize );
    mousevisible( true );
    do {
       if ( mouseparams.taste & 1 )
          for (int i = 0; i < mainmenuitemnum; i++)
             if ( mouseparams.x >  xz - xs && mouseparams.y > y1 + i * yd  && mouseparams.x < xz + xs && mouseparams.x < y1 + i * yd + ys ) {
                stat = i;
                while ( mouseparams.taste & 1 )
                   releasetimeslice();
             } /* endfor */

       if (keypress()) {
         tkey ch = r_key();

         switch ( ch ) {
            case ct_down: if ( pos < mainmenuitemnum-1 )
                             pos++;
                          else
                             pos=0;
               break;
            case ct_up: if ( pos > 0 )
                            pos--;
                        else
                            pos = mainmenuitemnum-1;
               break;
            case ct_enter:
            case ct_space: stat = pos;
               break;
         } /* endswitch */
       }
       if ( pos != oldpos ) {
          if ( oldpos != -1 ) {
             setinvisiblemouserectanglestk ( xz - xs - xud - fieldsizex, y1 + oldpos * yd + yud,
                                             xz - xs - xud            , y1 + oldpos * yd + yud + fieldsizey );
             putimage ( xz - xs - xud - fieldsizex, y1 + oldpos * yd + yud,  bkgr[0] );
             getinvisiblemouserectanglestk ();

             setinvisiblemouserectanglestk ( xz + xs + xud,             y1 + oldpos * yd + yud,
                                             xz + xs + xud + fieldsizex, y1 + oldpos * yd + yud + fieldsizey );
             putimage ( xz + xs + xud, y1 + oldpos * yd + yud,  bkgr[1] );
             getinvisiblemouserectanglestk ();
          }
          if ( pos != -1 ) {
             setinvisiblemouserectanglestk ( xz - xs - xud - fieldsizex, y1 + pos * yd + yud,
                                             xz - xs - xud            , y1 + pos * yd + yud + fieldsizey );
             getimage ( xz - xs - xud - fieldsizex, y1 + pos * yd + yud,
                        xz - xs - xud            , y1 + pos * yd + yud + fieldsizey,  bkgr[0] );
             putrotspriteimage90  ( xz - xs - xud - fieldsizex, y1 + pos * yd + yud,  unit->picture[0], 0 );
             getinvisiblemouserectanglestk ();

             setinvisiblemouserectanglestk ( xz + xs + xud,             y1 + pos * yd + yud,
                                             xz + xs + xud + fieldsizex, y1 + pos * yd + yud + fieldsizey );
             getimage ( xz + xs + xud,             y1 + pos * yd + yud,
                        xz + xs + xud + fieldsizex, y1 + pos * yd + yud + fieldsizey,  bkgr[1] );
             putrotspriteimage270 ( xz + xs + xud,             y1 + pos * yd + yud,  unit->picture[0], 0 );
             getinvisiblemouserectanglestk ();
          }
          oldpos = pos;
       }
    } while ( stat == -1 ); /* enddo */

    delete  ( bkgr[0] );
    delete  ( bkgr[1] );

    switch ( stat ) {
    case 0: newsinglelevel();
       break;
    case 1: newcampaign();
       break;
    case 2: ladespiel();
       break;
    case 3: continuenetworkgame (  );
       break;
    case 4: networksupervisor ( );
       break;
    case 5: abortgame = 1;
       break;
    } /* endswitch */
}



struct GameThreadParams {
   ASCString filename;
};

int gamethread ( void* data )
{
      GameThreadParams* gtp = (GameThreadParams*) data;

      initMapDisplay( );

      int resolx = agmp->resolutionx;
      int resoly = agmp->resolutiony;
      gui.init ( resolx, resoly );
      virtualscreenbuf.init();

      try {
         int fs = loadFullscreenImage ( "helisun.jpg" );
         if ( !fs ) {
            tnfilestream stream ( "logo640.pcx", tnstream::reading );
            loadpcxxy( &stream, (hgmp->resolutionx - 640)/2, (hgmp->resolutiony-35)/2, 1 );
         }
         loaddata( resolx, resoly, gtp->filename.c_str() );
         if ( fs )
            closeFullscreenImage ();

      }
      catch ( tfileerror err ) {
         displaymessage ( "unable to access file %s \n", 2, err.getFileName().c_str() );
      }
      catch ( toutofmem ) {
         displaymessage ( "loading of game failed due to insufficient memory", 2 );
      }
      catch ( ASCexception ) {
         displaymessage ( "loading of game failed", 2 );
      }

      displayLogMessage ( 5, "loaddata completed successfully.\n" );
      setvgapalette256(pal);

      addmouseproc ( &mousescrollproc );

      loadtexture();

      onlinehelp = new tsgonlinemousehelp;
      onlinehelpwind = new tsgonlinemousehelpwind;

      pd.init();

      abortgame = 0;
      gameStartupComplete = true;


      displayLogMessage ( 5, "entering outer main loop.\n" );
      do {
         try {
            if ( !actmap || actmap->xsize <= 0 || actmap->ysize <= 0 ) {
               displayLogMessage ( 8, "gamethread :: starting main menu.\n" );
               runmainmenu();
            } else {
               if ( actmap->actplayer == -1 ) {
                  displayLogMessage ( 8, "gamethread :: performing next_turn..." );
                  next_turn();
                  displayLogMessage ( 8, "done.\n" );
               }

               displayLogMessage ( 8, "gamethread :: Painting background pict..." );
               backgroundpict.paint();

               if ( !gtp->filename.empty() && patimat ( tournamentextension, gtp->filename.c_str() ) ) {
                  displayLogMessage ( 5, "Initializing network game..." );
                  initNetworkGame ( );
                  displayLogMessage ( 5, "done\n" );
               }

               displayLogMessage ( 8, "gamethread :: displaying map..." );
               displaymap();
               displayLogMessage ( 8, "done.\n" );
               cursor.show();

               moveparams.movestatus = 0;

               displayLogMessage ( 8, "gamethread :: painting gui icons..." );
               actgui->painticons();
               displayLogMessage ( 8, "done.\n" );
               mousevisible(true);

               dashboard.x = 0xffff;
               dashboard.y = 0xffff;

               /*
               static int displayed = 0;
               if ( !displayed )
                  displaymessage2( "time for startup: %d * 1/100 sec", ticker-cntr );

               displayed = 1;
               */

               displayLogMessage ( 5, "entering inner main loop.\n" );
               mainloop();
               mousevisible ( false );
            }
         } /* endtry */
         catch ( NoMapLoaded ) { } /* endcatch */
      } while ( abortgame == 0);
   return 0;
}


// including the command line parser, which is generated by genparse
#include "clparser/asc.cpp"

int main(int argc, char *argv[] )
{
   Cmdline* cl = NULL;
   auto_ptr<Cmdline> apcl ( cl );
   try {
      cl = new Cmdline ( argc, argv );
   }
   catch ( string s ) {
      cerr << s.c_str();
      exit(1);
   }

   if ( cl->next_param() < argc ) {
      cerr << "invalid command line parameter\n";
      exit(1);
   }

   if ( cl->v() ) {
      printf( getstartupmessage() );
      exit(0);
   }

   if ( cl->w() )
      fullscreen = 0;

   if ( cl->f() )
      fullscreen = 1;

   verbosity = cl->r();

#ifdef _DOS_
   if ( showmodes ) {
      showavailablemodes();
      return 0;
   }
#endif

   mapborderpainter = &backgroundpict;

   initmissions();

   initFileIO( cl->c().c_str() );  // passing the filename from the command line options

   try {
      checkDataVersion();
      check_bi3_dir ();
   }
   catch ( tfileerror err ) {
      displaymessage ( "unable to access file %s \n", 2, err.getFileName().c_str() );
   }
   catch ( ... ) {
      displaymessage ( "loading of game failed during pre graphic initializing", 2 );
   }

   if ( CGameOptions::Instance()->forceWindowedMode && !cl->f() )  // cl->f == force fullscreen command line param
      fullscreen = 0;



   int xr = 800;
   int yr = 600;
   // determining the graphics resolution
   if ( CGameOptions::Instance()->xresolution != 800 )
      xr = CGameOptions::Instance()->xresolution;
   if ( cl->x() != 800 )
      xr = cl->x();

   if ( CGameOptions::Instance()->yresolution != 600 )
      yr = CGameOptions::Instance()->xresolution;
   if ( cl->y() != 600 )
      yr = cl->y();

   modenum8 = initgraphics ( xr, yr, 8 );
   
   /* Clean up on exit */
   atexit(SDL_Quit);

   SoundSystem soundSystem ( CGameOptions::Instance()->sound_mute, cl->q() || CGameOptions::Instance()->sound_off );

   if ( modenum8 > 0 ) {
      #ifdef _DOS_
       setFullscreenSetting ( FIS_oldModeNum, modenum8 );
      #endif

      GameThreadParams gtp;
      gtp.filename = cl->l();

      {
         int w;
         tnfilestream stream ("mausi.raw", tnstream::reading);
         stream.readrlepict(   &icons.mousepointer, false, &w );
      }

      initializeEventHandling ( gamethread, &gtp, icons.mousepointer );

      closegraphics();

      writegameoptions ( );

      delete onlinehelp;
      onlinehelp = NULL;
      
      delete onlinehelpwind;
      onlinehelpwind = NULL;
   } // if 8 bit initialization successfull

#ifdef MEMCHK
   verifyallblocks();
#endif
   return(0);
}

