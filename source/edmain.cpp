/*! \file edmain.cpp
    \brief The map editor's main program 
*/

//     $Id: edmain.cpp,v 1.65 2003-07-06 15:10:26 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.64  2003/04/23 18:31:09  mbickel
//      Fixed: AI problems
//      Improved cheating detection in replay
//
//     Revision 1.63  2003/03/20 10:08:29  mbickel
//      KI speed up
//      mapeditor: added clipboard
//      Fixed movement issues
//
//     Revision 1.62  2003/01/12 19:37:18  mbickel
//      Rewrote resource production
//
//     Revision 1.61  2002/10/12 17:28:03  mbickel
//      Fixed "enemy unit loaded" bug.
//      Changed map format
//      Improved log messages
//
//     Revision 1.60  2002/10/09 16:58:46  mbickel
//      Fixed to GrafikSet loading
//      New item filter for mapeditor
//
//     Revision 1.59  2002/10/06 15:44:40  mbickel
//      Completed inheritance of .asctxt files
//      Speed up of replays
//
//     Revision 1.58  2001/12/17 19:41:22  mbickel
//      Reactionfire can now be deactivated without consequences if it has just
//        been activated
//      Mapeditor: Objects are selected on the palette map too
//
//     Revision 1.57  2001/12/14 10:20:05  mbickel
//      Cleanup and enhancements to configure.in
//      Removed last remains of octagonal version from source files
//
//     Revision 1.56  2001/10/16 19:58:19  mbickel
//      Added title screen for mapeditor
//      Updated source documentation
//
//     Revision 1.55  2001/10/16 15:33:03  mbickel
//      Added icons to data
//      ASC displays icons
//      Fixed crash when building icons
//      Removed assembled datafiles
//
//     Revision 1.54  2001/10/02 18:08:52  mbickel
//      Changed parser error handling to use exceptions
//      Removed gfx2pcx project
//
//     Revision 1.53  2001/10/02 14:06:28  mbickel
//      Some cleanup and documentation
//      Bi3 import tables now stored in .asctxt files
//      Added ability to choose amoung different BI3 import tables
//      Added map transformation tables
//
//     Revision 1.52  2001/09/23 23:06:20  mbickel
//      Fixed:
//       - ascent/descent during reactionfire
//       - movement with nearly empty fuel tank
//       - production icon displayed although unit could not be produced
//       - invisible building becoming visible in fog of war
//
//     Revision 1.51  2001/09/13 17:43:12  mbickel
//      Many, many bug fixes
//
//     Revision 1.50  2001/08/06 20:54:43  mbickel
//      Fixed lots of crashes related to the new text files
//      Fixed delayed events
//      Fixed crash in terrin change event
//      Fixed visibility of mines
//      Fixed crashes in event loader
//
//     Revision 1.49  2001/08/02 18:50:43  mbickel
//      Corrected Error handling in Text parsers
//      Improved version information
//
//     Revision 1.48  2001/08/02 18:18:02  mbickel
//      BI3 map translation tables now in textfile too
//
//     Revision 1.47  2001/08/02 15:33:01  mbickel
//      Completed text based file formats
//
//     Revision 1.46  2001/07/28 11:19:10  mbickel
//      Updated weaponguide
//      moved item repository from spfst to itemrepository
//
//     Revision 1.45  2001/07/14 21:07:46  mbickel
//      Sound works now under Win32 too
//      Error reporting on Win32 during startup works again.
//
//     Revision 1.44  2001/05/21 12:46:19  mbickel
//      Fixed infinite loop in AI::strategy
//      Fixed bugs in mapeditor - event editing
//      Fixed bugs in even loading / writing
//      Fixed wrong build order AI <-> main program
//
//     Revision 1.43  2001/05/19 13:07:58  mbickel
//      ASC now compiles with Borland C++ Builder again
//      Added getopt for use with BCB
//
//     Revision 1.42  2001/05/17 20:10:22  mbickel
//      Fixed: mapeditor was unable to load maps
//      Removed debugging output from bi3 map loader
//
//     Revision 1.41  2001/05/17 14:23:19  mbickel
//      Rewrote command line parameters of all programs
//      Made manpages generation optional
//
//     Revision 1.40  2001/05/16 23:21:01  mbickel
//      The data file is mounted using automake
//      Added sgml documentation
//      Added command line parsing functionality;
//        integrated it into autoconf/automake
//      Replaced command line parsing of ASC and ASCmapedit
//
//     Revision 1.39  2001/03/07 21:40:51  mbickel
//      Lots of bugfixes in the mapeditor
//
//     Revision 1.38  2001/02/18 15:37:08  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.37  2001/01/31 14:52:36  mbickel
//      Fixed crashes in BI3 map importing routines
//      Rewrote memory consistency checking
//      Fileselect dialog now uses ASCStrings
//
//     Revision 1.36  2001/01/28 14:04:13  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.35  2001/01/25 23:44:57  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.34  2001/01/04 15:13:45  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.33  2000/11/29 11:05:27  mbickel
//      Improved userinterface of the mapeditor
//      map::preferredfilenames uses now strings (instead of char*)
//
//     Revision 1.32  2000/11/29 09:40:20  mbickel
//      The mapeditor has now two maps simultaneously active
//      Moved memorychecking functions to its own file: memorycheck.cpp
//      Rewrote password handling in ASC
//
//     Revision 1.31  2000/11/26 22:18:53  mbickel
//      Added command line parameters for setting the verbosity
//      Increased verbose output
//
//     Revision 1.30  2000/11/08 19:31:03  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
//     Revision 1.29  2000/10/26 19:17:49  mbickel
//      Removed fullscreen settings in mapeditor
//
//     Revision 1.28  2000/10/26 18:55:28  mbickel
//      Fixed crash when editing the properties of a vehicle inside a building
//      Added mapeditorFullscreen switch to asc.ini
//
//     Revision 1.27  2000/10/24 15:35:10  schelli
//     MapEd FullScreen support added
//     weapons ammo now editable in MapEd
//
//     Revision 1.26  2000/10/18 17:09:39  mbickel
//      Fixed eventhandling for DOS
//
//     Revision 1.25  2000/10/18 15:10:03  mbickel
//      Fixed event handling for windows and dos
//
//     Revision 1.24  2000/10/18 14:14:06  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//
//     Revision 1.23  2000/10/11 14:26:30  mbickel
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
//     Revision 1.22  2000/09/07 16:42:28  mbickel
//      Made some adjustments so that ASC compiles with Watcom again...
//
//     Revision 1.21  2000/09/07 15:49:40  mbickel
//      some cleanup and documentation
//
//     Revision 1.20  2000/08/21 17:50:57  mbickel
//      Fixed: crash when unit reaching max experience
//      Fixed: crash when displaying research image
//      Fixed: crash when events referenced a unit that has been shot down
//      Fixed: screenshot being written to wrong directory
//
//     Revision 1.19  2000/08/02 15:52:56  mbickel
//      New unit set definition files
//      demount accepts now more than one container file
//      Unitset information dialog added
//
//     Revision 1.18  2000/08/01 10:39:09  mbickel
//      Updated documentation
//      Refined configuration file handling
//
//     Revision 1.17  2000/07/31 19:16:42  mbickel
//      Improved handing of multiple directories
//      Fixed: wind direction not displayed when cycling through wind heights
//      Fixed: oil rig not working
//      Fixed: resources becomming visible when checking mining station status
//      Fixed: division by zero when moving unit without fuel consumption
//
//     Revision 1.16  2000/07/31 18:02:53  mbickel
//      New configuration file handling
//      ASC searches its data files in all directories specified in ascrc
//      Renamed all tools so they begin with asc
//
//     Revision 1.15  2000/07/16 14:20:02  mbickel
//      AI has now some primitive tactics implemented
//      Some clean up
//        moved weapon functions to attack.cpp
//      Mount doesn't modify PCX files any more.
//
//     Revision 1.14  2000/05/23 20:40:44  mbickel
//      Removed boolean type
//
//     Revision 1.13  2000/05/22 15:40:34  mbickel
//      Included patches for Win32 version
//
//     Revision 1.12  2000/05/10 19:55:49  mbickel
//      Fixed empty loops when waiting for mouse events
//
//     Revision 1.11  2000/05/06 19:57:08  mbickel
//      Mapeditor/linux is now running
//
//     Revision 1.10  2000/05/05 21:15:02  mbickel
//      Added Makefiles for mount/demount and mapeditor
//      mapeditor can now be compiled for linux, but is not running yet
//
//     Revision 1.9  2000/04/27 16:25:21  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.8  2000/03/16 14:06:54  mbickel
//      Added unitset transformation to the mapeditor
//
//     Revision 1.7  2000/02/24 10:54:08  mbickel
//      Some cleanup and bugfixes
//
//     Revision 1.6  2000/02/03 20:54:39  mbickel
//      Some cleanup
//      getfiletime now works under Linux too
//
//     Revision 1.5  1999/12/27 12:59:54  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.4  1999/12/07 22:05:08  mbickel
//      Added password verification for loading maps
//
//     Revision 1.3  1999/11/22 18:27:16  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:41:36  tmwilson
//      Added CVS keywords to most of the files.
//      Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//      Wrote replacement routines for kbhit/getch for Linux
//      Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//      Added autoconf/automake capabilities
//      Added files used by 'automake --gnu'
//
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

#include <algorithm>
#include <memory>
#include <SDL_image.h>


#include "edmisc.h"
#include "loadpcx.h"
#include "loadbi3.h"
#include "edselfnt.h"
#include "edglobal.h"
#include "errors.h"
#include "gameoptions.h"
#include "mapdisplay.h"
#include "itemrepository.h"
#include "loadimage.h"

#include <signal.h>

#ifdef _DOS_
 #include "dos\memory.h"
#endif

// #define backgroundpict1 "BKGR2.PCX"  
#define menutime 35

//* õS Load-Functions

pprogressbar actprogressbar = NULL;


const char* progressbarfilename = "progress.6me";


// #define MEMCHK
#include "memorycheck.cpp"



void         loadcursor(void)
{ 
  int          w,i;

  {
      tnfilestream stream ("hexfeld.raw", tnstream::reading);
      stream.readrlepict ( &icons.fieldshape, false, &w );
  }

  {
      tnfilestream stream ("mapbkgrb.raw", tnstream::reading);
      stream.readrlepict ( &icons.mapbackground, false, &w );
  }

  {
      tnfilestream stream ("hexfld_a.raw", tnstream::reading);
    stream.readrlepict( &icons.stellplatz, false, &w);
  }

  {
    tnfilestream stream ("x.raw", tnstream::reading);
    stream.readrlepict( &icons.X, false, &w);
  }

  {
     tnfilestream stream ("pfeil-a0.raw", tnstream::reading);
     for (i=0;i<8 ;i++ ) stream.readrlepict( &icons.pfeil2[i], false, &w);
  }

   loadpalette();
   for (w=0;w<256 ;w++ ) {
      palette16[w][0] = pal[w][0];
      palette16[w][1] = pal[w][1];
      palette16[w][2] = pal[w][2];
      xlattables.nochange[w] = w;
   } /* endfor */

   loadicons(); 
   loadmessages(); 

} 





void loaddata( void ) 
{

   actprogressbar = new tprogressbar; 
   {
      tfindfile ff ( progressbarfilename );
      if ( !ff.getnextname().empty() ) {
         tnfilestream strm ( progressbarfilename, tnstream::reading );
         actprogressbar->start ( 255, 0, agmp->resolutiony-2, agmp->resolutionx-1, agmp->resolutiony-1, &strm );
      } else
         actprogressbar->start ( 255, 0, agmp->resolutiony-2, agmp->resolutionx-1, agmp->resolutiony-1, NULL );
   }

   if ( actprogressbar )
      actprogressbar->startgroup();
   loadcursor();

   if ( actprogressbar )
      actprogressbar->startgroup();

   loadguipictures();

   loadalltextfiles();

   readBI3translationTable();

   ItemFiltrationSystem::read();

   if ( actprogressbar )
      actprogressbar->startgroup();

   loadallobjecttypes();

   if ( actprogressbar )
      actprogressbar->startgroup();
   loadallvehicletypes();

   if ( actprogressbar )
      actprogressbar->startgroup();
   loadallbuildingtypes();

   if ( actprogressbar )
      actprogressbar->startgroup();
   loadalltechnologies();

   if ( actprogressbar )
      actprogressbar->startgroup();
   loadstreets();

   if ( actprogressbar )
      actprogressbar->startgroup();
   loadallterraintypes();

   if ( actprogressbar )
      actprogressbar->startgroup();

   loadUnitSets();

   freetextdata();

   if ( actprogressbar ) {
      actprogressbar->end();
      tnfilestream strm ( progressbarfilename, tnstream::writing );
      actprogressbar->writetostream( &strm );
      delete actprogressbar;
      actprogressbar = NULL;
   }
}

void buildemptymap ( void )
{
   if ( getterraintype_forid(30) )
      generatemap(getterraintype_forid(30)->weather[0], idisplaymap.getscreenxsize(1), idisplaymap.getscreenysize());
   else
      generatemap(getterraintype_forpos(0)->weather[0], idisplaymap.getscreenxsize(1), idisplaymap.getscreenysize());
}


void checkLeftMouseButton ( )
{
   static int buttonStat = 0;

   int x,y;
   if ( getfieldundermouse ( &x, &y ) ) {
      x += actmap->xpos;
      y += actmap->ypos;

      if ( mouseparams.taste )
         cursor.gotoxy ( x,y );

      if ( mapSwitcher.getDefaultAction() == MapSwitcher::select ) {
         if ( mouseparams.taste == 1 ) {
            execaction(act_setactivefieldvals);
            while ( mouseparams.taste == 1 )
               releasetimeslice();
         }

      } else {

         if ( mouseparams.taste == 1 ) {
            if ( buttonStat < 2 ) {
               execaction(act_placething);
               if ( lastselectiontype == cselunit || lastselectiontype == cselcolor || lastselectiontype == cselbuilding )
                  buttonStat = 2;
               else {
                 bool moved = false;
                 do {
                    int x1,y1;
                    if ( getfieldundermouse ( &x1, &y1 ) ) {
                       x1 += actmap->xpos;
                       y1 += actmap->ypos;
                       if ( x1 != x || y1 != y )
                          moved = true;
                    } else
                       moved = true;

                    if ( mouseparams.taste != 1 )
                       moved = true;
                 } while ( !moved );
               }
            }
         } else
            if ( buttonStat )
               buttonStat = 0;
      }
   } 

/*
            if ( mouseparams.taste == 1 ) {
               int mx, my;
               starttimer();
               curposchanged = false;
               while ( mouseparams.taste == 1 ) {
                 if ( getfieldundermouse ( &mx, &my ) )
                    if ( ( mx != lastx ) || (my != lasty ) ) {
                       mousevisible(false);
                       cursor.hide();
                       cursor.posx = mx;
                       cursor.posy = my;
                       cursor.show();
                       mousevisible(true);

                       lastx = mx;
                       lasty = my;
                       curposchanged = true;
                       starttimer();
                    }
                    if (time_elapsed(menutime)) {
                       execcode = -1;
                       execcode = leftmousebox();
                       if (execcode != -1 ) execaction(execcode);
                       while ( mouseparams.taste != 0 );
                  }
                  releasetimeslice();
               }
               if (getfieldundermouse ( &mx, &my ) )
                  if ( ! time_elapsed(menutime)) {
                       if ( ! curposchanged ) {
                          execaction(act_placething);
                          while ( mouseparams.taste == 1 )
                             releasetimeslice();
                       }
                  }
            }
*/

}


void         editor(void)
{  int execcode;

   cursor.show();
   do {
      try {
         do { 
            if ( keypress() ) {
               ch = r_key();

               pd.key = ch;
               switch (ch) {
                  #ifdef NEWKEYB
                  case ct_up:
                  case ct_down:
                  case ct_left:
                  case ct_right:
                  case ct_up + ct_stp:
                  case ct_down + ct_stp:
                  case ct_left + ct_stp:
                  case ct_right + ct_stp:
                  #endif
                  case ct_1k:
                  case ct_2k:
                  case ct_3k:
                  case ct_4k:
                  case ct_5k:
                  case ct_6k:
                  case ct_7k:
                  case ct_8k:
                  case ct_9k:
                  case ct_1k + ct_stp:
                  case ct_2k + ct_stp:
                  case ct_3k + ct_stp:
                  case ct_4k + ct_stp:
                  case ct_5k + ct_stp:
                  case ct_6k + ct_stp:
                  case ct_7k + ct_stp:
                  case ct_8k + ct_stp:
                  case ct_9k + ct_stp:   if ( polyfieldmode == false ) {
                                                    mousevisible(false);
                                                    movecursor(ch);
                                                    cursor.show();
                                                    showStatusBar();
                                                    mousevisible(true);
                                                 }
                     break;
                  case ct_f1:   execaction(act_help);
                     break;
                  case ct_f7 :
                  case ct_f3 : execaction(act_selbodentyp);
                     break;
                  case ct_f3 + ct_shp :
                  case ct_f3 + ct_altp :
                  case ct_f3 + ct_stp : execaction(act_selbodentypAll);
                     break;
                  case ct_f4 : execaction(act_selunit);
                     break;
                  case ct_f5 : execaction(act_selcolor);
                     break;
                  case ct_f6 : execaction(act_selbuilding);
                     break;
                  case ct_f7 + ct_shp :
                  case ct_f7 + ct_altp :
                  case ct_f7 + ct_stp : execaction(act_selobject);
                     break;
                  case ct_f8 : execaction(act_selmine);
                     break;
                  case ct_f9 : execaction(act_selweather);
                     break;
                  case ct_a + ct_stp :  execaction(act_setupalliances);
                     break;
                  case ct_b + ct_stp:  execaction(act_toggleresourcemode);
                     break;
                  case ct_c + ct_stp:  execaction(act_copyToClipboard);
                     break;
                  case ct_d + ct_stp : execaction(act_changeglobaldir);
                     break;
                  case ct_f + ct_stp: execaction(act_createresources);
                     break;
                  case ct_g + ct_stp: execaction(act_maptopcx);
                     break;
                  case ct_h + ct_stp : execaction(act_setunitfilter);
                     break;
                  case ct_i + ct_stp: execaction (act_import_bi_map );
                     break;
                  case ct_l + ct_stp : execaction(act_loadmap);
                     break;
                  case ct_m + ct_stp: execaction(act_changemapvals);
                     break;
                  case ct_n + ct_stp: execaction(act_newmap);
                     break;
                  case ct_o + ct_stp: execaction(act_polymode);
                     break;
                  case ct_p + ct_stp: execaction(act_changeproduction);
                     break;
                  case ct_r + ct_stp: execaction(act_repaintdisplay);
                     break;
                  case ct_u + ct_stp : execaction(act_unitinfo);
                     break;
                  case ct_v + ct_stp: execaction(act_pasteFromClipboard);
                     break;
                  case ct_w + ct_stp : execaction(act_setactweatherglobal);
                     break;
                  case ct_x + ct_stp: execaction(act_end);
                     break;
                  case ct_d + ct_shp: execaction(act_changeunitdir);
                     break;
                  case ct_a:   execaction(act_movebuilding);
                     break;
                  case ct_b:   execaction(act_changeresources);
                     break;
                  case ct_c:   execaction(act_changecargo);
                     break;
                  case ct_d : execaction(act_changeterraindir);
                     break;
                  case ct_e:  execaction(act_events);
                     break;
                  case ct_f:  execaction(act_fillmode);
                     break;
                  case ct_g: execaction(act_mapgenerator);
                     break;
                  case ct_h: execaction(act_setactivefieldvals);
                     break;
                  case ct_entf: execaction(act_deletething);
                      break;
                  case ct_l : execaction(act_showpalette);
                     break;
                  case ct_m : execaction(act_changeminestrength);
                     break;
                  case ct_o: execaction(act_changeplayers);
                     break;
                  case ct_p: execaction(act_changeunitvals);
                     break;
                  case ct_r: execaction(act_resizemap);
                     break;
                  case ct_s : execaction(act_savemap);
                     break;
                  case ct_v:   execaction(act_viewmap);
                     break;
                  case ct_x:   execaction(act_mirrorcursorx);
                     break;
                  case ct_y:   execaction(act_mirrorcursory);
                     break;
                  case ct_z:   execaction(act_setzoom );
                     break;
                  case ct_3 : execaction(act_placebodentyp);
                     break;
                  case ct_4 : execaction(act_placeunit);
                     break;
                  case ct_6 : execaction(act_placebuilding);
                     break;
                  case ct_7 : execaction(act_placeobject);
                     break;
                  case ct_8 : execaction(act_placemine);
                     break;
                  case ct_tab: execaction(act_switchmaps );
                     break;
                  case ct_enter :
                  case ct_space : if ( mapSwitcher.getDefaultAction() == MapSwitcher::select ) 
                                    execaction(act_setactivefieldvals);
                                  else
                                    execaction(act_placething);
                     break;
                  case ct_esc : {
                        if ( polyfieldmode )
                           execaction(act_endpolyfieldmode);
                        else
                           execaction(act_end);
                     }
                     break;
                  }
            } 
            pulldown();
            checkselfontbuttons();
            checkLeftMouseButton();
            if ( mouseparams.taste == 2 ) {
               int mx, my;
               while ( mouseparams.taste == 2 ) {
                 if ( getfieldundermouse ( &mx, &my ) )  {
                    mousevisible(false);
                    cursor.hide();
                    cursor.posx = mx;
                    cursor.posy = my;
                    cursor.show();
                    mousevisible(true);

                    execcode = rightmousebox();
                    if (execcode != -1) execaction(execcode);

                    while ( mouseparams.taste != 0 )
                       releasetimeslice();
                  }
                  releasetimeslice();
               }
            }
      
            checkformousescrolling();
            releasetimeslice();
      
         }  while (! (ch == ct_esc) || (ch == ct_altp+ct_x ) );
      } /* endtry */
      catch ( NoMapLoaded ) {
         buildemptymap();
         repaintdisplay();
      } /* endcatch */
   }  while (! (ch == ct_esc) || (ch == ct_altp+ct_x ) );
}


//* õS Diverse

void closesvgamode( void )
{
   closegraphics();
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

   int a = maxavail();
   int b = _memavl();
   showtext2( strrr ( a ), 210,410 );
   showtext2( strrr ( b+a ), 310,410 );
   // showtext2( strrr ( _memmax() ), 10,410 );
   showtext2( strrr ( b  ), 110,480 );
   showtext2( strrr ( stackfree ()  ), 410,410 );
   if (mss == 2)
      setinvisiblemouserectangle ( -1, -1, -1, -1 );

   npop  ( activefontsettings );
  #endif
}

//* õS Main-Program


pfont load_font(char* name)
{
   tnfilestream stream ( name, tnstream::reading );
   return loadfont ( &stream );
}




int mapeditorMainThread ( void* _mapname )
{
   const char* mapname = (const char*) _mapname;
   initMapDisplay( );

   cursor.init();

   try {
      loaddata();

      if ( mapname && mapname[0] ) {
         if( patimat ( savegameextension, mapname )) {
            if( validatesavfile( mapname ) == 0 )
               fatalError ( "The savegame %s is invalid. Aborting.", mapname );

            try {
               loadgame( mapname );
            } catch ( tfileerror ) {
               fatalError ( "%s is not a legal savegame. ", mapname );
            }
         } else
            if( patimat ( mapextension, mapname )) 
               loadmap ( mapname );
            else
               fatalError ( "%s is not an accepted file format", mapname );
      } else
         buildemptymap();

      mapSwitcher.toggle();
      if ( exist ( "palette.map" ))
         loadmap ( "palette.map");
      else
         buildemptymap();

      actmap->preferredFileNames.mapname[0] = "";

      mapSwitcher.toggle();

   } /* end try */
   catch ( ParsingError err ) {
      displaymessage ( "Error parsing text file " + err.getMessage(), 2 );
   }
   catch ( tfileerror err ) {
      displaymessage ( " error loading file %s ",2, err.getFileName().c_str() );
   } /* end catch */

   pulldownfont = schriften.smallarial;

   activefontsettings.font = schriften.arial8;
   activefontsettings.color =lightblue ;
   activefontsettings.background =3;
   activefontsettings.length =100;
   activefontsettings.justify =lefttext;


   setstartvariables();

   addmouseproc ( &mousescrollproc );

   bar( 0, 0, hgmp->resolutionx-1, hgmp->resolutiony-1, 0 );
   setvgapalette256(pal);

   displaymap();
   showallchoices();
   pdsetup();
   pdbaroff();

   mousevisible(true);
   cursor.show();

   gameStartupComplete = true;
   editor();
   return 0;
}

// including the command line parser, which is generated by genparse
#include "clparser/mapedit.cpp"

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
      ASCString msg = kgetstartupmessage();
      printf( msg.c_str() );
      exit(0);
   }

   verbosity = cl->r();

   int              modenum8;

   #ifdef logging
    logtofile ( kgetstartupmessage() );
    logtofile ( "\n new log started \n ");
   #endif

   #ifdef _DOS_
    if ( showmodes ) {
       showavailablemodes();
       return 0;
    }
   #endif

   signal ( SIGINT, SIG_IGN );

   initFileIO( cl->c().c_str() );

   fullscreen = CGameOptions::Instance()->mapeditForceFullscreenMode;
   if ( cl->f() )
      fullscreen = 1;
   if (  cl->w() )
      fullscreen = 0;

   checkDataVersion();
   check_bi3_dir ();

   SDLmm::Surface* icon = NULL;
   try {
      tnfilestream iconl ( "icon_mapeditor.gif", tnstream::reading );
      SDL_Surface *icn = IMG_LoadGIF_RW( SDL_RWFromStream ( &iconl ));
      SDL_SetColorKey(icn, SDL_SRCCOLORKEY, *((Uint8 *)icn->pixels));
      icon = new SDLmm::Surface ( icn );
   }
   catch ( ... ) {
   }



   int xr = 800;
   int yr = 600;
   // determining the graphics resolution
   if ( CGameOptions::Instance()->mapeditor_xresolution != 800 )
      xr = CGameOptions::Instance()->mapeditor_xresolution;
   if ( cl->x() != 800 )
      xr = cl->x();

   if ( CGameOptions::Instance()->mapeditor_yresolution != 600 )
      yr = CGameOptions::Instance()->mapeditor_yresolution;
   if ( cl->y() != 600 )
      yr = cl->y();

   modenum8 = initgraphics ( xr, yr, 8, icon );

   if ( modenum8 < 0 )
      return 1;
   atexit ( closesvgamode );

   #ifdef pbpeditor
   setWindowCaption ( "Advanced Strategic Command : PBP Editor ");
   #else
   setWindowCaption ( "Advanced Strategic Command : Map Editor ");
   #endif

   schriften.smallarial = load_font("smalaril.fnt");
   schriften.large = load_font("usablack.fnt");
   schriften.arial8 = load_font("arial8.fnt");
   schriften.smallsystem = load_font("msystem.fnt");
   schriften.monogui = load_font("monogui.fnt");
   

   virtualscreenbuf.init();

   int fs = loadFullscreenImage ( "title_mapeditor.jpg" );
   if ( !fs ) {
      tnfilestream stream ( "logo640.pcx", tnstream::reading );
      loadpcxxy( &stream, (hgmp->resolutionx - 640)/2, (hgmp->resolutiony-35)/2, 1 );
      int whitecol = 251;
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background = 255;
      activefontsettings.justify = centertext;
      activefontsettings.color = whitecol;
      activefontsettings.length = hgmp->resolutionx-20;
      showtext2 ("Map Editor", 10, hgmp->resolutiony - activefontsettings.font->height - 2 );
   }

   {
      int w;
      tnfilestream stream ("mausi.raw", tnstream::reading);
      stream.readrlepict(   &icons.mousepointer, false, &w );
   }


   char* buf = new char[cl->l().length()+10];
   strcpy ( buf, cl->l().c_str() );
   initializeEventHandling ( mapeditorMainThread, buf, icons.mousepointer );
   delete[] buf;

   cursor.hide();
   writegameoptions ();

  #ifdef MEMCHK
   verifyallblocks();
  #endif
   return 0;
}

