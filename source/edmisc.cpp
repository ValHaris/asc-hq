/** \file edmisc.cpp
    \brief various functions for the mapeditor
*/

//     $Id: edmisc.cpp,v 1.82 2002-04-21 21:27:00 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.81  2002/04/14 17:21:18  mbickel
//      Renamed global variable pf to pf2 due to name clash with SDL_mixer library
//
//     Revision 1.80  2002/04/09 22:19:06  mbickel
//      Fixed AI bugs
//      Fixed: invalid player name displayed in dashboard
//
//     Revision 1.79  2002/03/19 18:42:52  mbickel
//      Fixed bug introduced by fixing the MAA-reaction-fire bug
//
//     Revision 1.78  2002/03/18 21:42:17  mbickel
//      Some cleanup and documentation in the Mine class
//      The number of mines is now displayed in the field information window
//
//     Revision 1.77  2002/03/18 09:31:51  mbickel
//      Fixed: canceling Reactionfire enabled units that can not attack after
//             moving to move in the same turn
//
//     Revision 1.76  2002/03/17 21:25:18  mbickel
//      Fixed: View unit movement revealed the reaction fire status of enemy units
//      Mapeditor: new function "resource comparison"
//
//     Revision 1.75  2002/03/02 23:04:01  mbickel
//      Some cleanup of source code
//      Improved Paragui Integration
//      Updated documentation
//      Improved Sound System
//
//     Revision 1.74  2002/02/21 17:06:50  mbickel
//      Completed Paragui integration
//      Moved mail functions to own file (messages)
//
//     Revision 1.73  2001/12/14 10:20:05  mbickel
//      Cleanup and enhancements to configure.in
//      Removed last remains of octagonal version from source files
//
//     Revision 1.72  2001/11/05 21:31:04  mbickel
//      Fixed compilation errors
//      new data version required
//
//     Revision 1.71  2001/10/31 18:34:31  mbickel
//      Some adjustments and fixes for gcc 3.0.2
//
//     Revision 1.70  2001/10/16 19:58:19  mbickel
//      Added title screen for mapeditor
//      Updated source documentation
//
//     Revision 1.69  2001/10/11 10:41:06  mbickel
//      Restructured platform fileio handling
//      Added map archival information to mapeditor
//
//     Revision 1.68  2001/10/11 10:22:50  mbickel
//      Some cleanup and fixes for Visual C++
//
//     Revision 1.67  2001/10/08 14:44:22  mbickel
//      Some cleanup
//
//     Revision 1.66  2001/10/08 14:12:20  mbickel
//      Fixed crash in AI
//      Speedup of AI
//      Map2PCX improvements
//      Mapeditor usability improvements
//
//     Revision 1.65  2001/10/02 14:06:28  mbickel
//      Some cleanup and documentation
//      Bi3 import tables now stored in .asctxt files
//      Added ability to choose amoung different BI3 import tables
//      Added map transformation tables
//
//     Revision 1.64  2001/09/13 17:43:12  mbickel
//      Many, many bug fixes
//
//     Revision 1.63  2001/08/27 21:03:55  mbickel
//      Terraintype graphics can now be mounted from any number of PNG files
//      Several AI improvements
//
//     Revision 1.62  2001/08/19 12:50:03  mbickel
//      fixed event trigger allenemybuildings
//
//     Revision 1.61  2001/08/09 19:28:22  mbickel
//      Started adding buildingtype text file functions
//
//     Revision 1.60  2001/08/09 10:28:22  mbickel
//      Fixed AI problems
//      Mapeditor can edit a units AI parameter
//
//     Revision 1.59  2001/07/28 21:09:08  mbickel
//      Prepared vehicletype structure for textIO
//
//     Revision 1.58  2001/07/28 11:19:10  mbickel
//      Updated weaponguide
//      moved item repository from spfst to itemrepository
//
//     Revision 1.57  2001/07/27 21:13:34  mbickel
//      Added text based file formats
//      Terraintype and Objecttype restructured
//
//     Revision 1.56  2001/07/15 21:00:25  mbickel
//      Some cleanup in the vehicletype class
//
//     Revision 1.55  2001/07/11 20:44:37  mbickel
//      Removed some vehicles from the data file.
//      Put all legacy units in into the data/legacy directory
//
//     Revision 1.54  2001/05/24 15:37:51  mbickel
//      Fixed: reaction fire could not be disabled when unit out of ammo
//      Fixed several AI problems
//
//     Revision 1.53  2001/03/30 12:43:15  mbickel
//      Added 3D pathfinding
//      some cleanup and documentation
//      splitted the ai into several files, now located in the ai subdirectory
//      AI cares about airplane servicing and range constraints
//
//     Revision 1.52  2001/03/07 21:40:51  mbickel
//      Lots of bugfixes in the mapeditor
//
//     Revision 1.51  2001/02/26 12:35:10  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.50  2001/02/18 15:37:08  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.49  2001/02/11 11:39:32  mbickel
//      Some cleanup and documentation
//
//     Revision 1.48  2001/02/01 22:48:37  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.47  2001/01/31 14:52:36  mbickel
//      Fixed crashes in BI3 map importing routines
//      Rewrote memory consistency checking
//      Fileselect dialog now uses ASCStrings
//
//     Revision 1.46  2001/01/28 20:42:11  mbickel
//      Introduced a new string class, ASCString, which should replace all
//        char* and std::string in the long term
//      Split loadbi3.cpp into 3 different files (graphicselector, graphicset)
//
//     Revision 1.45  2001/01/28 17:19:07  mbickel
//      The recent cleanup broke some source files; this is fixed now
//
//     Revision 1.44  2001/01/25 23:44:58  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.43  2001/01/04 15:13:46  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.42  2000/12/26 21:04:35  mbickel
//      Fixed: putimageprt not working (used for small map displaying)
//      Fixed: mapeditor crashed on generating large maps
//
//     Revision 1.41  2000/11/29 11:05:27  mbickel
//      Improved userinterface of the mapeditor
//      map::preferredfilenames uses now strings (instead of char*)
//
//     Revision 1.40  2000/11/29 09:40:20  mbickel
//      The mapeditor has now two maps simultaneously active
//      Moved memorychecking functions to its own file: memorycheck.cpp
//      Rewrote password handling in ASC
//
//     Revision 1.39  2000/11/21 20:27:01  mbickel
//      Fixed crash in tsearchfields (used by object construction for example)
//      AI improvements
//      configure.in: added some debug output
//                    fixed broken check for libbz2
//
//     Revision 1.38  2000/11/14 20:36:40  mbickel
//      The AI can now use supply vehicles
//      Rewrote objecttype IO routines to make the structure independant of
//       the memory layout
//
//     Revision 1.37  2000/11/08 19:31:03  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
//     Revision 1.36  2000/10/24 15:35:10  schelli
//     MapEd FullScreen support added
//     weapons ammo now editable in MapEd
//
//     Revision 1.35  2000/10/19 11:40:03  mbickel
//      Fixed crash when editing unit values
//
//     Revision 1.34  2000/10/18 14:14:07  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//
//     Revision 1.33  2000/10/14 15:31:53  mbickel
//      Moved key symbols to separate files
//      Moved tnstream to its own file
//
//     Revision 1.32  2000/10/11 15:33:43  mbickel
//      Adjusted small editors to the new ASC structure
//      Watcom compatibility
//
//     Revision 1.31  2000/10/11 14:26:31  mbickel
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
//     Revision 1.30  2000/09/16 11:47:26  mbickel
//      Some cleanup and documentation again
//
//     Revision 1.29  2000/09/07 15:49:41  mbickel
//      some cleanup and documentation
//
//     Revision 1.28  2000/08/12 12:52:46  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.27  2000/08/08 09:48:12  mbickel
//
//      speed up of dialog boxes in linux
//      fixed graphical errors in attack
//      fixed graphical error in ammo transfer
//      fixed reaction fire not allowing manual attack
//
//     Revision 1.26  2000/08/07 21:10:20  mbickel
//      Fixed some syntax errors
//
//     Revision 1.25  2000/08/06 11:39:05  mbickel
//      New map paramter: fuel globally available
//      Mapeditor can now filter buildings too
//      Fixed unfreed memory in fullscreen image loading
//      Fixed: wasted cpu cycles in building
//      map parameters can be specified when starting a map
//      map parameters are reported to all players in multiplayer games
//
//     Revision 1.24  2000/08/04 15:11:02  mbickel
//      Moving transports costs movement for units inside
//      refuelled vehicles now have full movement in the same turn
//      terrain: negative attack / defensebonus allowed
//      new mapparameters that affect damaging and repairing of building
//
//     Revision 1.23  2000/08/03 19:45:14  mbickel
//      Fixed some bugs in DOS code
//      Removed submarine.ascent != 0 hack
//
//     Revision 1.22  2000/08/03 19:21:18  mbickel
//      Fixed: units had invalid height when produced in some buildings
//      Fixed: units could not enter building if unitheightreq==0
//      Started adding SDL_image support
//      Upgraded to SDL1.1.3 (for SDL_image)
//
//     Revision 1.21  2000/08/03 13:12:11  mbickel
//      Fixed: on/off switching of generator vehicle produced endless amounts of energy
//      Repairing units now reduces their experience
//      negative attack- and defenseboni possible
//      changed attackformula
//
//     Revision 1.20  2000/08/02 15:52:56  mbickel
//      New unit set definition files
//      demount accepts now more than one container file
//      Unitset information dialog added
//
//     Revision 1.19  2000/06/28 19:26:15  mbickel
//      fixed bug in object generation by building removal
//      Added artint.cpp to makefiles
//      Some cleanup
//
//     Revision 1.18  2000/05/23 20:40:44  mbickel
//      Removed boolean type
//
//     Revision 1.17  2000/05/10 19:55:49  mbickel
//      Fixed empty loops when waiting for mouse events
//
//     Revision 1.16  2000/05/06 19:57:08  mbickel
//      Mapeditor/linux is now running
//
//     Revision 1.15  2000/05/05 21:15:03  mbickel
//      Added Makefiles for mount/demount and mapeditor
//      mapeditor can now be compiled for linux, but is not running yet
//
//     Revision 1.14  2000/04/27 16:25:21  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.13  2000/04/06 09:07:46  mbickel
//      Fixed a bug in the mapeditor that prevented transports from being loaded
//
//     Revision 1.12  2000/04/01 11:38:38  mbickel
//      Updated the small editors
//      Added version numbering
//
//     Revision 1.11  2000/03/29 09:58:45  mbickel
//      Improved memory handling for DOS version
//      Many small changes I can't remember ;-)
//
//     Revision 1.10  2000/03/16 14:06:55  mbickel
//      Added unitset transformation to the mapeditor
//
//     Revision 1.9  2000/03/11 18:22:04  mbickel
//      Added support for multiple graphic sets
//
//     Revision 1.8  2000/02/03 20:54:39  mbickel
//      Some cleanup
//      getfiletime now works under Linux too
//
//     Revision 1.7  2000/01/24 17:35:43  mbickel
//      Added dummy routines for sound under DOS
//      Cleaned up weapon specification
//
//     Revision 1.6  1999/12/29 12:50:44  mbickel
//      Removed a fatal error message in GUI.CPP
//      Made some modifications to allow platform dependant path delimitters
//
//     Revision 1.5  1999/12/27 12:59:56  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.4  1999/12/07 22:05:09  mbickel
//      Added password verification for loading maps
//
//     Revision 1.3  1999/11/22 18:27:17  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:41:37  tmwilson
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

#include <string>
#include <iostream>
#include <math.h>

#include "vehicletype.h"
#include "buildingtype.h"
#include "edmisc.h"
#include "loadbi3.h"
#include "edevents.h"
#include "edgen.h"
#include "edselfnt.h"
#include "edglobal.h"
#include "password_dialog.h"
#include "mapdisplay.h"
#include "graphicset.h"
#include "itemrepository.h"

#ifdef _DOS_
 #include "dos\memory.h"
#endif

   tkey         ch;
   pfield               pf2;

   pterraintype auswahl;
   pvehicletype auswahlf;
   pobjecttype  actobject;
   pbuildingtype        auswahlb;
   int          auswahls;
   int          auswahlm;       
   int          auswahlw;
   int          auswahld;
   int          farbwahl;
   int                  altefarbwahl;

   tfontsettings        rsavefont;
   int                  lastselectiontype;
   selectrec            sr[10];

   ppolygon             pfpoly;
   char         tfill,polyfieldmode;
   word         fillx1, filly1;

   int                  i;
   pbuilding            gbde;
   char         mapsaved;
   tmycursor            mycursor;

   tpolygon_management  polymanage;
   tpulldown            pd;
   // tcdrom            cdrom;

// õS Checkobject

char checkobject(pfield pf)
{
   return !pf->objects.empty();
}

// õS MouseButtonBox

#define maxmouseboxitems 20

class   tmousebuttonbox {
           public :
                 char holdbutton; //true : menu only opens if button is held
                 int actcode;
                 void init(void);
                 void additem(int code);
                 int getboxundermouse(void); 
                 void checkfields(void);
                 void run(void);
                 void done(void);
           protected :
                 pfont savefont;
                 void *background;
                 int lastpos,actpos;
                 int mousestat;
                 int maxslots;
                 int itemcount,boxxsize,boxysize,slotsize,boxx,boxy;
                 int item[maxmouseboxitems];
                 };

void tmousebuttonbox::init(void)
{
   ch = ct_invvalue;
   mousestat = mouseparams.taste;
   boxx = mouseparams.x;
   boxy = mouseparams.y;
   itemcount = 0;
   holdbutton=false;
   boxxsize = 0;
   boxysize = 0;
   savefont = activefontsettings.font;
   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = black;
   slotsize = activefontsettings.font->height + 4;

   if (maxmouseboxitems * slotsize > agmp->resolutiony ) maxslots = ( agmp->resolutiony - 20 ) / slotsize;
   else maxslots = maxmouseboxitems;
   
}

void tmousebuttonbox::additem(int code)
{  int txtwidth;

   if (itemcount < maxslots ) {

      // two times the same item will be discarded; usually happens with separators only
      if ( itemcount > 0 && item[itemcount-1] == code )
         return;

      item[itemcount] = code;
      txtwidth = gettextwdth((char * ) execactionnames[code], activefontsettings.font);
      if (txtwidth > boxxsize) boxxsize = txtwidth;
      itemcount++;
   } 
}

int tmousebuttonbox::getboxundermouse(void)
{  int mx,my;

   mx=mouseparams.x;
   my=mouseparams.y;
   if ( (mx > boxx ) && (mx < boxx+boxxsize ) ) 
      if ( (my > boxy ) && (my < boxy + boxysize -5 ) ) {
         return ( my - boxy - 4 ) / slotsize;
      } 
   return -1;
}

void tmousebuttonbox::checkfields(void)
{
   if (keypress()) ch = r_key();
   actpos = getboxundermouse();
   if (actpos != lastpos) {
      mousevisible(false);
      if ( ( lastpos != -1 ) && ( item[lastpos] != act_seperator) ) rectangle(boxx + 3, boxy + 3 + lastpos * slotsize, boxx + boxxsize -3, boxy + 3 + ( lastpos +1 ) * slotsize,lightgray);
      if ( ( actpos != -1 ) && ( item[actpos] != act_seperator) ) lines(boxx + 3, boxy + 3 + actpos * slotsize, boxx + boxxsize -3, boxy + 3 + ( actpos +1 ) * slotsize);
      mousevisible(true);
      lastpos =actpos;
      if (actpos != -1 ) {
         if (item[actcode] != act_seperator) actcode = item[actpos];
         else actcode = -1;
      } else actcode = -1;
   }
}

void tmousebuttonbox::run(void)
{  
   lastpos = -1;
   actcode = -1;
   boxxsize+=15;
   boxysize= slotsize * itemcount + 5;

   if (boxy + boxysize > agmp->resolutiony -5 ) boxy = agmp->resolutiony - 5 - boxysize;
   if (boxx + boxxsize > agmp->resolutionx -5 ) boxx = agmp->resolutionx - 5 - boxxsize;

   background = malloc(imagesize(boxx,boxy,boxx+boxxsize,boxy+boxysize));

   mousevisible(false);
   getimage(boxx,boxy,boxx+boxxsize,boxy+boxysize,background);

   bar(boxx,boxy,boxx+boxxsize,boxy+boxysize,lightgray);
   lines(boxx,boxy,boxx+boxxsize,boxy+boxysize);

   activefontsettings.length = boxxsize - 10;
   activefontsettings.background = lightgray;
   activefontsettings.color = black;
   activefontsettings.justify = lefttext;

   for (int i=0;i < itemcount ;i++ ) {
      if (item[i] != act_seperator) showtext2((char*) execactionnames[item[i]],boxx+5,boxy + 5 + slotsize * i);
      else line( boxx + 5, boxy + 5 + slotsize / 2 + slotsize * i, boxx + boxxsize - 5, boxy + 5 + slotsize / 2 + slotsize * i,black) ;
   } /* endfor */
   mousevisible(true);
   while (mouseparams.taste == mousestat) {
      checkfields();
      releasetimeslice();
   } /* endwhile */
   if ( (actpos == -1) && (holdbutton == false ) ) {
      while ( (mouseparams.taste == 0) && (ch != ct_esc) ) {
         checkfields();
         releasetimeslice();
      } /* endwhile */
   }
}


void tmousebuttonbox::done(void)
{
   mousevisible(false);
   putimage(boxx,boxy,background);
   mousevisible(true);
   asc_free(background);
   activefontsettings.font = savefont;
   while (keypress())
      ch = r_key();

   while (mouseparams.taste != 0)
      releasetimeslice();
   ch = ct_invvalue;
}


// õS Rightmousebox

int rightmousebox(void)
{  tmousebuttonbox tmb;

   tmb.init();
   pf2 = getactfield();
   if (pf2 != NULL) {

      if ( pf2->vehicle ) {
         tmb.additem(act_changeunitvals);
         if ( pf2->vehicle->typ->loadcapacity > 0 )
            tmb.additem(act_changecargo);
         tmb.additem(act_deleteunit);
      }
      tmb.additem(act_seperator);
      if ( pf2->building ) {
         tmb.additem(act_changeunitvals);
         if ( pf2->building->typ->loadcapacity > 0  )
            tmb.additem(act_changecargo);
         if ( pf2->building->typ->special & cgvehicleproductionb )
            tmb.additem(act_changeproduction);
         tmb.additem(act_deletebuilding);
         tmb.additem(act_deleteunit);
      }
      tmb.additem(act_seperator);
      if ( !pf2->mines.empty() ) {
         tmb.additem(act_changeminestrength);
         tmb.additem(act_deletemine);
      }
      tmb.additem(act_seperator);

      tmb.additem(act_changeresources);

      tmb.additem(act_seperator);

      if ( !pf2->objects.empty() ) {
         tmb.additem( act_deletetopmostobject );
         tmb.additem( act_deleteobject );
         tmb.additem( act_deleteallobjects );
      }
   }

   tmb.run();
   tmb.done();
   return tmb.actcode;
}

// õS Leftmousebox

int leftmousebox(void)
{  tmousebuttonbox tmb;

   tmb.init();
   tmb.holdbutton=true;
   tmb.additem(act_placebodentyp);
   tmb.additem(act_placeunit);
   tmb.additem(act_placebuilding);
   tmb.additem(act_placeobject);
   tmb.additem(act_placemine);

   tmb.additem(act_seperator);

   //tmb.additem(act_selbodentyp);
   //tmb.additem(act_selunit);
   //tmb.additem(act_selbuilding);
   //tmb.additem(act_selobject);
   //tmb.additem(act_selmine);
   //tmb.additem(act_selweather);
   tmb.additem(act_setactivefieldvals);

   //tmb.additem(act_seperator);

   tmb.additem(act_viewmap);
   tmb.additem(act_repaintdisplay);

   tmb.additem(act_seperator);

   tmb.additem(act_help);

   tmb.run();
   tmb.done();
   return tmb.actcode;
}

// õS PutResource

void tputresources :: init ( int sx, int sy, int dst, int restype, int resmax, int resmin )
{
   centerPos = MapCoordinate(sx,sy);
   initsearch( centerPos, dst, 0);
   resourcetype = restype;
   maxresource = resmax;
   minresource = resmin;
   maxdst = dst;
   startsearch();
}

void tputresources :: testfield ( const MapCoordinate& mc )
{
   int dist = beeline ( mc, centerPos ) / 10;
   int m = maxresource - dist * ( maxresource - minresource ) / maxdst;

   pfield fld = gamemap->getField ( mc );
   if ( resourcetype == 1 )
      fld->material = m;
   else
      fld->fuel = m;
}

void tputresourcesdlg :: init ( void )
{
   resourcetype = 1;
   restp2 = 0;
   status = 0;
   tdialogbox::init();
   title = "set resources";
   xsize = 400;
   ysize = 300;
   maxresource = 200;
   minresource = 0;
   dist = 10;

   addbutton ( "~O~k", 10, ysize - 30, xsize/2 - 5, ysize - 10 , 0, 1, 1, true );
   addkey ( 1, ct_enter );
   addbutton ( "~C~ancel", xsize/2 + 5, ysize - 30, xsize-10 - 5, ysize - 10 , 0, 1, 2, true );
   addkey ( 2, ct_esc );

   addbutton ( "ma~x~ resources ( at center )", xsize/2 + 5, 80, xsize-20 - 5, 100 , 2, 1, 3, true );
   addeingabe ( 3, &maxresource, 0, 255 );

   addbutton ( "mi~n~ resources ", xsize/2 + 5, 120, xsize-20 - 5, 140 , 2, 1, 4, true );
   addeingabe ( 4, &minresource, 0, 255 );

   addbutton ( "max ~d~istance", 20, 120, xsize/2-5, 140 , 2, 1, 5, true );
   addeingabe ( 5, &dist, 1, 20 );

   addbutton ( "~m~aterial", 20, 160, xsize/2-5, 180, 3, 10, 6, true );
   addeingabe ( 6, &resourcetype, black, dblue );

   addbutton ( "~f~uel", xsize/2 + 5, 160, xsize - 20, 180, 3, 10, 7, true );
   addeingabe ( 7, &restp2, black, dblue );

   buildgraphics(); 

}

void tputresourcesdlg :: buttonpressed ( int id )
{
   tdialogbox :: buttonpressed ( id );

   switch ( id ) {
      case 1: status = 11;
         break;
      case 2: status = 10;
         break;
      case 6: restp2 = 0;
              enablebutton ( 7 );
         break;
      case 7: resourcetype = 0;
              enablebutton ( 6 );
         break;
   } /* endswitch */

}

void tputresourcesdlg :: run ( void )
{
   mousevisible ( true );
   do {
      tdialogbox :: run ( );
   } while ( status < 10 ); /* enddo */
   if ( status == 11 ) {
      tputresources pr ( actmap );
      pr.init ( getxpos(), getypos(), dist, resourcetype ? 1 : 2, maxresource, minresource );
   }
}


//* õS Place-Functions

void placebodentyp(void)
{
   word         fillx2, filly2;
   integer       i, j;

   cursor.hide(); 
   mousevisible(false); 
   mapsaved = false;
   if (polyfieldmode) {
      tfillpolygonbodentyp fillpoly;

      fillpoly.tempvalue = 0;

      if (fillpoly.paint_polygon ( pfpoly ) == 0) 
         displaymessage("Invalid Polygon !",1 );
   
      polyfieldmode = false;
      pdbaroff();
      displaymap();
   } else {
      lastselectiontype = cselbodentyp;
      pf2 = getactfield(); 
      if (tfill) { 
         filly2 = cursor.posy + actmap->ypos; 
         fillx2 = cursor.posx + actmap->xpos;
         if (fillx1 > fillx2) exchg(&fillx1,&fillx2);
         if (filly1 > filly2) exchg(&filly1,&filly2);
         for (i = filly1; i <= filly2; i++) 
            for (j = fillx1; j <= fillx2; j++) { 
               pf2 = getfield(j,i); 
               if ( pf2 ) {
                  if ( auswahl->weather[auswahlw] ) 
                     pf2->typ = auswahl->weather[auswahlw]; 
                  else
                     pf2->typ = auswahl->weather[0]; 
   
                  pf2->direction = auswahld;
                  pf2->setparams();
                  if (pf2->vehicle != NULL) 
                     if ( (pf2->vehicle->typ->terrainaccess.accessible ( 
pf2->bdt ) < 0) || (pf2->vehicle->typ->terrainaccess.accessible ( pf2->bdt 
) == 0 && actmap->getgameparameter( cgp_movefrominvalidfields ) == 0 )) {
                        delete pf2->vehicle;
                        pf2->vehicle = NULL;
                     }
               }
            }
         displaymap();
         tfill = false;
         pdbaroff();
      }
      else {
         if ( auswahl->weather[auswahlw] )
            pf2->typ = auswahl->weather[auswahlw];
         else
            pf2->typ = auswahl->weather[0];
         pf2->direction = auswahld;
         pf2->setparams();
         if (pf2->vehicle != NULL)
            if ( (pf2->vehicle->typ->terrainaccess.accessible ( pf2->bdt ) 
< 0) || (pf2->vehicle->typ->terrainaccess.accessible ( pf2->bdt ) == 0 && 
actmap->getgameparameter( cgp_movefrominvalidfields ) == 0 )) {
               delete pf2->vehicle;
               pf2->vehicle = NULL;
            }
      }
   } /* endif */
   displaymap(); 
   mousevisible(true); 
   cursor.show(); 
}


void placeunit(void)
{
   if (polyfieldmode) {
      cursor.hide(); 
      mousevisible(false); 
      mapsaved = false;

      tfillpolygonunit fillpoly;

      fillpoly.tempvalue = 0;

      if (fillpoly.paint_polygon ( pfpoly ) == 0) 
         displaymessage("Invalid Polygon !",1 );
   
      polyfieldmode = false;
      pdbaroff();

      displaymap(); 
      mousevisible(true); 
      cursor.show(); 
   } else {
      lastselectiontype = cselunit;
      if (farbwahl < 8) {
         cursor.hide(); 
         mousevisible(false); 
         mapsaved = false;
         pf2 = getactfield();
         int accessible = 1;
         if ( auswahlf ) {
            accessible = auswahlf->terrainaccess.accessible ( pf2->bdt );
            if ( auswahlf->height >= chtieffliegend )
               accessible = 1;
         }

         if (pf2 != NULL) 
              // !( pf2->bdt & cbbuildingentry)
            if ( !( pf2->building             ) && ( accessible || 
actmap->getgameparameter( cgp_movefrominvalidfields)) ) {
               int set = 1;
               if ( pf2->vehicle ) {
                  if ( pf2->vehicle->typ != auswahlf ) {
                     delete pf2->vehicle;
                     pf2->vehicle = NULL;
                   } else {
                      set = 0;
                      pf2->vehicle->color = farbwahl * 8;
                   }
               }
               if ((auswahlf != NULL) && set ) {
                  pf2->vehicle = new Vehicle ( auswahlf, actmap, farbwahl 
);
                  pf2->vehicle->setnewposition ( getxpos(), getypos() );
                  pf2->vehicle->fillMagically();
                  pf2->vehicle->height=1;
                  while ( ! ( ( ( ( pf2->vehicle->height & 
pf2->vehicle->typ->height ) > 0) && (terrainaccessible(pf2,pf2->vehicle) 
== 2) ) ) && (pf2->vehicle->height != 0) )
                     pf2->vehicle->height = pf2->vehicle->height * 2;
                  if (pf2->vehicle->height == 0 ) {
                     if ( actmap->getgameparameter( cgp_movefrominvalidfields) ) {
                        pf2->vehicle->height=1;
                        while ( !(pf2->vehicle->height & 
pf2->vehicle->typ->height) && pf2->vehicle->height )
                           pf2->vehicle->height = pf2->vehicle->height * 
2;
                     }
                     if (pf2->vehicle->height == 0 ) {
                        delete pf2->vehicle;
                        pf2->vehicle = NULL;
                     }
                  }
                  if ( pf2->vehicle ) {
                     pf2->vehicle->setMovement ( pf2->vehicle->typ->movement[log2(pf2->vehicle->height)] );
                     pf2->vehicle->direction = auswahld;
                  }
               }
            } 
            else
               if (auswahlf == NULL)
                  if (pf2->vehicle != NULL) {
                     delete pf2->vehicle;
                     pf2->vehicle = NULL;
                  }
         displaymap(); 
         mousevisible(true); 
         cursor.show(); 
      }
   } /* endif */
}


void placeobject(void)
{
   cursor.hide();
   mousevisible(false); 
   mapsaved = false;
   pf2 = getactfield(); 
   if (tfill) { 
      putstreets2(fillx1,filly1,cursor.posx + actmap->xpos,cursor.posy + actmap->ypos, actobject ); 
      // 5. parameter zeiger auf object
      displaymap(); 
      tfill = false; 
      pdbaroff(); 
   } else {
      pf2 = getactfield();
      pvehicle eht = pf2->vehicle;
      pf2->vehicle = NULL;
      pf2->addobject( actobject );
      pf2->vehicle = eht;
   }

   lastselectiontype = cselobject;
   displaymap(); 
   mousevisible(true); 
   cursor.show(); 
}

void placemine(void)
{
   cursor.hide();
   mousevisible(false); 
   mapsaved = false;
   getactfield()->putmine(farbwahl,auswahlm+1,MineBasePunch[auswahlm]);
   lastselectiontype = cselmine;
   displaymap();
   mousevisible(true); 
   cursor.show(); 
}


void putactthing ( void )
{
    cursor.hide(); 
    mousevisible(false); 
    mapsaved = false;
    pf2 = getactfield();
    switch (lastselectiontype) {
       case cselbodentyp :
       case cselweather : placebodentyp();
          break;
       case cselunit:
       case cselcolor:  placeunit();
          break;
       case cselbuilding:   placebuilding(farbwahl,auswahlb,true);
          break;
       case cselobject:   placeobject();
          break;
       case cselmine:   if ( farbwahl < 8 ) getactfield()->putmine(farbwahl,auswahlm+1,MineBasePunch[auswahlm]);
          break;
    } 
    displaymap(); 
    mousevisible(true); 
    cursor.show(); 
}


// õS ShowPalette

void         showpalette(void)
{ 
   bar ( 0, 0, 639, 479, black );
   int x=7;

   for ( char a = 0; a <= 15; a++) 
      for ( char b = 0; b <= 15; b++) { 
         bar(     a * 40, b * 20,a * 40 +  x,b * 20 + 20, xlattables.a.light    [a * 16 + b]);
         bar( x + a * 40, b * 20,a * 40 + 2*x,b * 20 + 20, xlattables.nochange [a * 16 + b]); 
         bar(2*x + a * 40, b * 20,a * 40 + 3*x,b * 20 + 20, xlattables.a.dark1    [a * 16 + b]); 
         bar(3*x + a * 40, b * 20,a * 40 + 4*x,b * 20 + 20, xlattables.a.dark2    [a * 16 + b]); 
         bar(4*x + a * 40, b * 20,a * 40 + 5*x,b * 20 + 20, xlattables.a.dark3    [a * 16 + b]); 
      }      
   wait();
   repaintdisplay();
} 

// õS Lines

void lines(int x1,int y1,int x2,int y2)
{ 
   line(x1,y1,x2,y1,white);
   line(x2,y1,x2,y2,darkgray);
   line(x1,y2,x2,y2,darkgray);
   line(x1,y1,x1,y2,white);
} 



// õS PDSetup

void         pdsetup(void)
{ 
  pd.addfield ( "~F~ile" );
   pd.addbutton ( "~N~ew mapõctrl+N" , act_newmap    );
   pd.addbutton ( "~L~oad mapõctrl+L", act_loadmap   );
   pd.addbutton ( "~S~ave mapõS",      act_savemap   );
   pd.addbutton ( "Save map ~a~s",     act_savemapas ); 
   pd.addbutton ( "Edit Map ~A~rchival Information", act_editArchivalInformation );
   pd.addbutton ( "seperator",         -1            );
   pd.addbutton ( "~W~rite map to PCX-Fileõctrl+G", act_maptopcx);
    pd.addbutton ( "~I~mport BI mapõctrl-i", act_import_bi_map );
    pd.addbutton ( "Insert ~B~I map", act_insert_bi_map );
   #ifdef FREEMAPZOOM
    pd.addbutton ( "seperator", -1 ); 
    pd.addbutton ( "set zoom level", act_setzoom ); 
   #endif
   pd.addbutton ( "seperator", -1 ); 
   pd.addbutton ( "E~x~itõEsc", act_end);
      
  pd.addfield ("~T~ools"); 
   pd.addbutton ( "~V~iew mapõctrl+V",          act_viewmap );
   pd.addbutton ( "~S~how palette",           act_showpalette );
   pd.addbutton ( "~R~ebuild displayõctrl+R",   act_repaintdisplay );
   pd.addbutton ( "seperator",                  -1 ); 
   pd.addbutton ( "~C~reate ressourcesõctrl+F", act_createresources );
   pd.addbutton ( "~M~ap generatorõG",          act_mapgenerator );
   pd.addbutton ( "Resi~z~e mapõR",             act_resizemap );
   pd.addbutton ( "set global ~w~eatherõctrl-W", act_setactweatherglobal );
   pd.addbutton ( "Sm~o~oth coasts",          act_smoothcoasts );
   pd.addbutton ( "~U~nitset transformation",    act_unitsettransformation );
   pd.addbutton ( "map ~t~ransformation",        act_transformMap );
   pd.addbutton ( "Com~p~are Resources ", act_displayResourceComparison );

   pd.addfield ("~O~ptions");
    pd.addbutton ( "~M~ap valuesõctrl+M",          act_changemapvals );
    pd.addbutton ( "~C~hange playersõO",           act_changeplayers);
    pd.addbutton ( "~E~dit eventsõE",              act_events );
    pd.addbutton ( "~S~etup Player + Alliancesõctrl+A",     act_setupalliances );
    pd.addbutton ( "seperator",                    -1);
    pd.addbutton ( "~T~oggle ResourceViewõctrl+B", act_toggleresourcemode);
    pd.addbutton ( "~B~I ResourceMode",            act_bi_resource );
    pd.addbutton ( "~A~sc ResourceMode",           act_asc_resource );
    pd.addbutton ( "edit map ~P~arameters",        act_setmapparameters );
    pd.addbutton ( "setup unit ~F~iltersõctrl+h",  act_setunitfilter );
    pd.addbutton ( "select ~G~raphic set",         act_selectgraphicset );

   pd.addfield ("~H~elp");
    pd.addbutton ( "~U~nit Informationõctrl+U",    act_unitinfo );
    pd.addbutton ( "unit~S~et Information",        act_unitSetInformation );
    pd.addbutton ( "~T~errain Information",        act_terraininfo );
    pd.addbutton ( "seperator",                    -1 );
    pd.addbutton ( "~H~elp SystemõF1",             act_help );
    pd.addbutton ( "~A~bout",                      act_about ); 

    pd.init();
    pd.setshortkeys();
}

// õS PlayerChange

/* class   tcolorsel : public tstringselect {
           public :
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void gettext(word nr);
                 };

void         tcolorsel::setup(void)
{ 

   action = 0;
   title = "Select Color"; 
   numberoflines = 9;
   xsize = 250;
   ex = xsize - 100;
   ey = ysize - 60; 
   addbutton("~D~one",20,ysize - 40,120,ysize - 20,0,1,2,true); 
   addkey(2,ct_enter); 
   addbutton("~C~ancel",130,ysize - 40,230,ysize - 20,0,1,3,true); 
} 


void         tcolorsel::buttonpressed(int         id)
{ 
   tstringselect::buttonpressed(id);
   switch (id) {
      
      case 2:   
      case 3:   action = id; 
   break; 
   } 
} 


void         tcolorsel::gettext(word nr)
{ 
   if (nr == 8) strcpy(txt,"Neutral");
   else {
      if ((nr>=0) && (nr<=7)) {
         strcpy(txt,"Color ");
         strcat(txt,strrr(nr+1));
      } else {
         strcpy(txt,"");
      } 
   }
} 


void         tcolorsel::run(void)
{ 
   mousevisible(false); 
   for (i=0;i<=8 ;i++ ) bar(x1 + 160,y1 + 52 + i*21 ,x1 + 190 ,y1 + 62 + i * 21,20 + i * 8);
   mousevisible(true); 
   do { 
      tstringselect::run(); 
   }  while ( ! ( (taste == ct_esc) || ( (action == 2) || (action == 3) ) || (msel == 1)) );
   if ( (action == 3) || (taste == ct_esc) ) redline = 255;
} 


int         colorselect(void)
{ 
  tcolorsel  sm; 

   sm.init(); 
   sm.run(); 
   sm.done(); 
   return sm.redline; 
} */


class  tplayerchange : public tdialogbox {
          public :
              int action;
              int sel1,sel2;
              int bkgcolor;
              void init(void);
              virtual void run(void);
              virtual void buttonpressed(int id);
              void anzeige(void);
              };




void         tplayerchange::init(void)
{ 
   char *s1;

   tdialogbox::init(); 
   title = "Player Change";
   x1 = 50;
   xsize = 370; 
   y1 = 50; 
   ysize = 410; 
   action = 0; 
   sel1 = 255;
   sel2 = 255;
   bkgcolor = lightgray;

   windowstyle = windowstyle ^ dlg_in3d; 

   for (i=0;i<=8 ;i++ ) {
      s1 = new(char[12]);
      if (i == 8) {
         strcpy(s1,"~N~eutral");
      } else {
         strcpy(s1,"Player ~");
         strcat(s1,strrr(i+1));
         strcat(s1,"~");
      } 
      addbutton(s1,20,55+i*30,150,75+i*30,0,1,6+i,true); 
      addkey(1,ct_1+i); 
   }  

   //addbutton("~C~hange",20,ysize - 70,80,ysize - 50,0,1,3,true); 
   addbutton("X~c~hange",20,ysize - 70,170,ysize - 50,0,1,4,true); 
   addbutton("~M~erge",200,ysize - 70,350,ysize - 50,0,1,5,true); 

   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,1,true); 
   addkey(1,ct_enter); 
   addbutton("E~x~it",200,ysize - 40,350,ysize - 20,0,1,2,true); 

   buildgraphics(); 

   for (i=0;i<=8 ;i++ ) bar(x1 + 170,y1 + 60 + i*30 ,x1 + 190 ,y1 + 70 + i * 30,20 + ( i << 3 ));

   anzeige();

   mousevisible(true); 
} 

void         tplayerchange::anzeige(void)
{
   int e,b,m[9];
   for (i=0;i<=8 ;i++ ) m[i] =0;
   int i;
   for (i =0;i < actmap->xsize * actmap->ysize ;i++ ) {
      int color = actmap->field[i].mineowner();
      if ( color >= 0 )
         m[color]++;
   }
   activefontsettings.length = 40;
   activefontsettings.background = lightgray;
   activefontsettings.color = red;
   activefontsettings.justify = lefttext;
   activefontsettings.font = schriften.smallarial;
   mousevisible(false); 
   showtext2("Units",x1+210,y1+35);
   showtext2("Build.",x1+260,y1+35);
   showtext2("Mines",x1+310,y1+35);
   for (i=0;i<=8 ;i++ ) {
      if (i == sel1 ) rectangle (x1 + 16,y1+51+i*30,x1+154,y1+79+i*30, 20 );
      else if ( i == sel2 ) rectangle (x1 + 16,y1+51+i*30,x1+154,y1+79+i*30, 28 );
      else rectangle (x1 + 16,y1+51+i*30,x1+154,y1+79+i*30, bkgcolor );
      e = actmap->player[i].vehicleList.size();
      b = actmap->player[i].buildingList.size();
      activefontsettings.justify = righttext;
      showtext2(strrr(e),x1+200,y1+56+i*30);
      showtext2(strrr(b),x1+255,y1+56+i*30);
      showtext2(strrr(m[i]),x1+310,y1+56+i*30);
   } /* endfor */
   mousevisible(true); 
}


void         tplayerchange::run(void)
{ 

   do { 
      tdialogbox::run(); 
      if (taste == ct_f1) help ( 1050 );
   }  while (!((taste == ct_esc) || ((action == 1) || (action ==2)))); 
 // ????  if ((action == 2) || (taste == ct_esc)) ;
} 


void         tplayerchange::buttonpressed(int         id)
{ 
   tdialogbox::buttonpressed(id); 
   switch (id) {
      
      case 1:   
      case 2:   action = id; 
        break;
/*     case 3: {
           int sel = colorselect();
           if ( ( sel != 255) && ( sel != sel1 ) && ( sel1 != 255 ) && 
              ( actmap->player[sel].firstvehicle == NULL ) && ( actmap->player[sel].firstbuilding == NULL )   ) {

           } 
        }
        break; */
     case 4: {
           if ( ( sel1 != 255) && ( sel2 != sel1 ) && ( sel2 != 255 ) ) {

              // exchanging the players sel1 and sel2

              typedef tmap::Player::VehicleList VL;
              typedef tmap::Player::VehicleList::iterator VLI;

              VL vl;
              for ( VLI i = actmap->player[sel1].vehicleList.begin(); i != actmap->player[sel1].vehicleList.end(); ) {
                 (*i)->color = sel2*8;
                 vl.push_back ( *i );
                 i = actmap->player[sel1].vehicleList.erase( i );
              }

              for ( VLI i = actmap->player[sel2].vehicleList.begin(); i != actmap->player[sel2].vehicleList.end(); ) {
                 (*i)->color = sel1*8;
                 actmap->player[sel1].vehicleList.push_back ( *i );
                 i = actmap->player[sel2].vehicleList.erase( i );
              }

              for ( VLI i = vl.begin(); i != vl.end(); ) {
                 actmap->player[sel2].vehicleList.push_back ( *i );
                 i = vl.erase( i );
              }


              typedef tmap::Player::BuildingList BL;
              typedef tmap::Player::BuildingList::iterator BLI;

              BL bl;
              for ( BLI i = actmap->player[sel1].buildingList.begin(); i != actmap->player[sel1].buildingList.end(); ) {
                 (*i)->color = sel2*8;
                 bl.push_back ( *i );
                 i = actmap->player[sel1].buildingList.erase( i );
              }

              for ( BLI i = actmap->player[sel2].buildingList.begin(); i != actmap->player[sel2].buildingList.end(); ) {
                 (*i)->color = sel1*8;
                 actmap->player[sel1].buildingList.push_back ( *i );
                 i = actmap->player[sel2].buildingList.erase( i );
              }

              for ( BLI i = bl.begin(); i != bl.end(); ) {
                 actmap->player[sel2].buildingList.push_back ( *i );
                 i = bl.erase( i );
              }

              for (int i =0;i < actmap->xsize * actmap->ysize ;i++ ) {
                 pfield fld = &actmap->field[i];
                 for ( tfield::MineContainer::iterator i = fld->mines.begin(); i != fld->mines.end(); i++ )
                    if ( i->player == sel1 )
                       i->player = sel2;
                    else
                       if ( i->player == sel2 )
                          i->player = sel1;
                    
                 
              } /* endfor */
              anzeige();
           }
        }
        break;
     case 5: {
           if ( ( sel1 != 255) && ( sel2 != sel1 ) && ( sel2 != 255 ) ) {

              // adding everything from player sel2 to sel1

              for ( tmap::Player::VehicleList::iterator i = actmap->player[sel2].vehicleList.begin(); i != actmap->player[sel2].vehicleList.end(); ) {
                 (*i)->color = sel1*8;
                 actmap->player[sel1].vehicleList.push_back ( *i );
                 i = actmap->player[sel2].vehicleList.erase( i );
              }

              for ( tmap::Player::BuildingList::iterator i = actmap->player[sel2].buildingList.begin(); i != actmap->player[sel2].buildingList.end(); ) {
                 (*i)->color = sel1*8;
                 actmap->player[sel1].buildingList.push_back ( *i );
                 i = actmap->player[sel2].buildingList.erase( i );
              }

              for (int i =0;i < actmap->xsize * actmap->ysize ;i++ ) {
                 pfield fld = &actmap->field[i];
                 for ( tfield::MineContainer::iterator i = fld->mines.begin(); i != fld->mines.end(); i++ )
                    if ( i->player == sel2 )
                       i->player = sel1;

              } /* endfor */
              anzeige();
           }
        }
        break;
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:  
      case 14: if ( id-6 != sel1 ) {
         sel2 = sel1;
         sel1 = id-6;
         anzeige();
      }
   break; 
   } 
} 


void playerchange(void)  
{ tplayerchange       sc; 

   sc.init(); 
   sc.run(); 
   sc.done(); 
} 


//* õS Mycursor

void       tmycursor::getimg ( void )
{
     int xp, yp;

     xp = sx + posx * ix;
     yp = sy + posy * iy;

     if (agmp-> linearaddress != hgmp-> linearaddress ) {
        npush(*agmp);
        *agmp = *hgmp;
        getimage(xp, yp, xp + fieldsizex, yp + fieldsizey, backgrnd );
        npop (*agmp);
     } else {
        getimage(xp, yp, xp + fieldsizex, yp + fieldsizey, backgrnd );
     } 
}

void       tmycursor::putbkgr ( void )
{
     int xp, yp;

     xp = sx + posx * ix;
     yp = sy + posy * iy;

     if (agmp-> linearaddress != hgmp-> linearaddress ) {
        npush( *agmp );
        *agmp = *hgmp;
        putimage(xp, yp, backgrnd );
        npop ( *agmp );
     } else {
        putimage(xp, yp, backgrnd );
     }


}

void       tmycursor::putimg ( void )
{
     int xp, yp;

     xp = sx + posx * ix;
     yp = sy + posy * iy;

     if (agmp-> linearaddress != hgmp-> linearaddress ) {
        npush( *agmp );
        *agmp = *hgmp;
        putrotspriteimage(xp, yp, picture,color );
        npop ( *agmp );
     } else {
        putrotspriteimage(xp, yp, picture,color );
     } 
}


void         exchg(word *       a1,
                   word *       a2)
{ word        ex;

   ex = *a1; 
   *a1 = *a2; 
   *a2 = ex; 
} 


void         pdbaroff(void)
{ 
   mousevisible(false);
   pd.baroff();
   rsavefont = activefontsettings; 

   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = 1;
   activefontsettings.background = black;
   activefontsettings.length = 200;
   activefontsettings.justify = lefttext;
      
   showStatusBar();
   activefontsettings = rsavefont;
   mousevisible(true); 
} 


// õS TCDPlayer

/*

class   tcdplayer : public tstringselect {
           public :
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void gettext(word nr);
                 };

void         tcdplayer ::setup(void)
{ 

   action = 0;
   xsize = 400;
   ysize = 400;
   x1 = 100;
   title = "CD- Player";
   numberoflines = cdrom.cdinfo.lasttrack;
   ey = ysize - 60;
   ex = xsize - 120;
   addbutton("~R~CD",320,50,390,70,0,1,11,true);
   addbutton("~P~lay",320,90,390,110,0,1,12,true);
   addbutton("~S~top",320,130,390,150,0,1,13,true);
   addbutton("E~x~it",10,ysize - 40,290,ysize - 20,0,1,10,true);
} 


void         tcdplayer ::buttonpressed(int         id)
{ 
   tstringselect::buttonpressed(id);
   switch (id) {
      
   case 10:   action = 3;
      break;
   case 11:  {
      cdrom.readcdinfo();
      redline = 0;
      firstvisibleline = 0;
      numberoflines = cdrom.cdinfo.lasttrack;
      viewtext();
      }
      break;
   case 12: {
         cdrom.playtrack(redline);
      }
      break;
   case  13: cdrom.stopaudio ();
      break;
   } 
} 


void         tcdplayer ::gettext(word nr)
{ 
    if (cdrom.cdinfo.track[nr]->type > 3) {
       strcpy(txt,"Data Track ");
       strcat(txt,strrr(nr+1));
    } else {
       strcpy(txt,"");
       strcat(txt,strrr(cdrom.cdinfo.track[nr]->min));
       strcat(txt," : ");
       strcat(txt,strrr(cdrom.cdinfo.track[nr]->sec));
       strcat(txt," : ");
       strcat(txt,strrr(cdrom.cdinfo.track[nr]->frame));
       strcat(txt,"   Track ");
       strcat(txt,strrr(nr+1));
    }
} 


void         tcdplayer ::run(void)
{ 
   do { 
      tstringselect::run(); 
      if ( (msel == 1)  || ( taste == ct_enter ) ) cdrom.playtrack(redline);
   }  while ( ! ( (taste == ct_esc) || (action == 3) ) );
} 


void cdplayer( void )
{
   tcdplayer cd;

   cd.init();
   cd.run();
   cd.done();
}

*/

void         repaintdisplay(void)
{ 
   mousevisible(false);
   cursor.hide();
   bar(0,0,agmp->resolutionx-1,agmp->resolutiony-1,0);
   if ( !lockdisplaymap )
      displaymap(); 

   pdbaroff(); 
   showallchoices(); 
   cursor.show();
   mousevisible(true);
} 


void         k_savemap(char saveas)
{ 
   ASCString filename;

   int nameavail = 0;
   if ( !actmap->preferredFileNames.mapname[0].empty() ) {
      nameavail = 1;
      filename = actmap->preferredFileNames.mapname[0];;
   }

   mousevisible(false);
   if ( saveas || !nameavail ) {
      fileselectsvga(mapextension, filename, false);
   } 
   if ( !filename.empty() ) {
      mapsaved = true;
      cursor.hide();
      actmap->preferredFileNames.mapname[0] = filename;
      savemap( filename.c_str() );
      displaymap();
      cursor.show();
   } 
   mousevisible(true); 
}


void         k_loadmap(void)
{
   ASCString s1;

   mousevisible(false); 
   fileselectsvga( mapextension, s1, true );
   if ( !s1.empty() ) {
      cursor.hide(); 
      displaymessage("loading map %s",0, s1.c_str() );
      loadmap(s1.c_str());
      actmap->startGame();

      if ( actmap->campaign && !actmap->campaign->directaccess && actmap->codeword[0]) {
         tlockdispspfld ldsf;
         removemessage(); 
         Password pwd;
         pwd.setUnencoded ( actmap->codeword );
         enterpassword ( pwd );
      } else
         removemessage(); 
   

      displaymap(); 
      cursor.show();
      mapsaved = true;
   } 
   mousevisible(true); 
} 

void         placebuilding(int               colorr,
                          pbuildingtype   buildingtyp,
                          char            choose)

{     
   #define bx   100
   #define by   100  
   #define sts  bx + 200  

   pbuilding    gbde; 

   mousevisible(false);
   cursor.hide();
   mapsaved = false;

   int f = 0;
   if (choose == true) {
      for ( int x = 0; x < 4; x++ )
         for ( int y = 0; y < 6; y++ )
            if ( buildingtyp->getpicture ( BuildingType::LocalCoordinate(x, y) )) {
               MapCoordinate mc = buildingtyp->getFieldCoordinate ( MapCoordinate (getxpos(), getypos()), BuildingType::LocalCoordinate (x, y) );
               if ( !actmap->getField (mc) )
                  return;

               if ( buildingtyp->terrainaccess.accessible ( actmap->getField (mc)->bdt ) <= 0 )
                  f++;
            }
      if ( f )
         if (choice_dlg("Invalid terrain for building !","~i~gnore","~c~ancel") == 2)
            return;

      putbuilding( MapCoordinate( getxpos(),getypos()), colorr * 8,buildingtyp,buildingtyp->construction_steps);
   }

   gbde = getactfield()->building;
   if (gbde == NULL) return;

   gbde->plus = gbde->maxplus;

   activefontsettings.color = 1;
   activefontsettings.background = lightgray;
   activefontsettings.length = 100;
   
   changebuildingvalues(*gbde);
   if (choose == true) 
      building_cargo( gbde );

   lastselectiontype = cselbuilding; 
   displaymap();
   cursor.show();
   mousevisible(true);
} 



void         freevariables(void)
{
   asc_free ( mycursor.backgrnd );
}


void         setstartvariables(void) 
{ 
   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = 1; 
   activefontsettings. background = 0;
   
   mapsaved = true;
   polyfieldmode = false;

   auswahl  = getterraintype_forpos(0);
   auswahlf = getvehicletype_forpos(0); 
   auswahlb = getbuildingtype_forpos(0); 
   actobject = getobjecttype_forpos(0);
   auswahls = 0;
   auswahlm = 1; 
   auswahlw = 0;
   auswahld = 0;
   farbwahl = 0; 
   
   sr[0].maxanz = terraintypenum;
   sr[0].showall = true;
   sr[1].maxanz = vehicletypenum;
   sr[1].showall = true;
   sr[2].maxanz = 9;
   sr[2].showall = true;
   sr[3].maxanz = buildingtypenum;
   sr[3].showall = true;
   sr[4].maxanz = vehicletypenum;
   sr[4].showall = false;
   sr[5].maxanz = objecttypenum;
   sr[5].showall = true;
   sr[6].maxanz = vehicletypenum;
   sr[6].showall = false;
   sr[7].maxanz = vehicletypenum;
   sr[7].showall = false;
   sr[8].maxanz = minecount;
   sr[8].showall = true;
   sr[9].maxanz = cwettertypennum;
   sr[9].showall = true;

   mycursor.picture = cursor.orgpicture;
   mycursor.backgrnd = malloc (10000);

   atexit( freevariables );
} 


int  selectfield(int * cx ,int  * cy)
{
   // int oldposx = getxpos();
   // int oldposy = getypos();
   cursor.gotoxy( *cx, *cy );
   displaymap();
   cursor.show();
   tkey ch = ct_invvalue;
   do {
      if (keypress()) {
         ch = r_key();
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
            case ct_9k + ct_stp:   {
                              mousevisible(false);
                              movecursor(ch);
                              cursor.show();
                              showStatusBar();
                              mousevisible(true);
                           }
                           break;
             } /* endswitch */
      } else
         releasetimeslice();
   } while ( ch!=ct_enter  &&  ch!=ct_space  && ch!=ct_esc); /* enddo */
   cursor.hide();
   *cx=cursor.posx+actmap->xpos;
   *cy=cursor.posy+actmap->ypos;
   // cursor.gotoxy( oldposx, oldposy );
   if ( ch == ct_enter )
      return 1;
   else
      if ( ch == ct_esc )
         return 0;
      else
         return 2;
}

//* õS FillPolygonevent

class  tfillpolygonwevent : public tfillpolygonsquarecoord {
        public:
             int tempvalue;
             virtual void initevent ( void );
             virtual void setpointabs ( int x,  int y  );
           };

void tfillpolygonwevent::setpointabs    ( int x,  int y  )
{
       pfield ffield = getfield ( x , y );
       if (ffield)
           ffield->a.temp = tempvalue;
}



void tfillpolygonwevent::initevent ( void )
{
}

void createpolygon (ppolygon *poly, int place, int id)
{
   polymanage.addpolygon(poly,place,id); 
   changepolygon(*poly);
}

//* õS FillPolygonbdt

void tfillpolygonbodentyp::setpointabs    ( int x,  int y  )
{
       pfield ffield = getfield ( x , y );
       if (ffield) {
           ffield->a.temp = tempvalue;
           if ( auswahl->weather[auswahlw] ) 
              ffield->typ = auswahl->weather[auswahlw]; 
           else
              ffield->typ = auswahl->weather[0]; 
           ffield->direction = auswahld;
           ffield->setparams();
           if (ffield->vehicle != NULL) 
              if ( terrainaccessible(ffield,ffield->vehicle) == false ) {
                 delete ffield->vehicle;
                 ffield->vehicle = NULL;
              }
       }
}

void tfillpolygonbodentyp::initevent ( void )
{
}

//* õS FillPolygonunit

void tfillpolygonunit::setpointabs    ( int x,  int y  )
{
       pfield ffield = getfield ( x , y );
       if (ffield) {
          if ( terrainaccessible(ffield,ffield->vehicle) ) 
               { 
                  if (ffield->vehicle != NULL) {
                     delete ffield->vehicle;
                     ffield->vehicle = NULL;
                  }
                  if (auswahlf != NULL) {
                     ffield->vehicle = new Vehicle ( auswahlf,actmap, farbwahl );
                     ffield->vehicle->fillMagically();
                     ffield->vehicle->height=1;
                     while ( ! ( ( ( ( ffield->vehicle->height & ffield->vehicle->typ->height ) > 0) && (terrainaccessible(ffield,ffield->vehicle) == 2) ) ) && (ffield->vehicle->height != 0) )
                        ffield->vehicle->height = ffield->vehicle->height * 2;
                     for (i = 0; i <= 31; i++) ffield->vehicle->loading[i] = NULL;
                     if (ffield->vehicle->height == 0 ) {
                        delete ffield->vehicle;
                        ffield->vehicle = NULL;
                     }
                     else ffield->vehicle->setMovement ( ffield->vehicle->typ->movement[log2(ffield->vehicle->height)] );
                     ffield->vehicle->direction = auswahld;
                  } 
            } 
            else
               if (auswahlf == NULL)
                  if (ffield->vehicle != NULL) {
                     delete ffield->vehicle;
                     ffield->vehicle=NULL;
                  }
            ffield->a.temp = tempvalue;
       }
}

void tfillpolygonunit::initevent ( void )
{
}


//* õS ChangePoly


void tchangepoly::setpolytemps (int value)
{
   tfillpolygonwevent fillpoly;

   fillpoly.tempvalue = value ;

   if (fillpoly.paint_polygon ( poly ) == 0) {
      if ( value ) 
         displaymessage("Invalid Polygon !",1 );
      setpolypoints(0);
   }
}


void tchangepoly::setpolypoints(int value)
{
   for (int i=0;i < poly->vertexnum ;i++ ) {
      pf2 = getfield(poly->vertex[i].x,poly->vertex[i].y);
      pf2->a.temp = value;
   } /* endfor */
}

int tchangepoly::checkpolypoint(int x, int y)
{
   for (int i=0 ;i < poly->vertexnum ;i++ ) if ( (poly->vertex[i].x == x ) && ( poly->vertex[i].y == y ) ) return 1;
   return 0;
}

void tchangepoly::deletepolypoint(int x, int y)
{
   if ( poly->vertexnum >= 3) setpolytemps(0);
   else setpolypoints(0);
   for (int i=0 ;i < poly->vertexnum ;i++ ) if ( (poly->vertex[i].x == x ) && ( poly->vertex[i].y == y ) ) {
      for (int j = i ;j < poly->vertexnum - 1 ;j++ ) {
         poly->vertex[j].x = poly->vertex[j+1].x;
         poly->vertex[j].y = poly->vertex[j+1].y;
      }
      poly->vertexnum--;
      if (poly->vertexnum > 1) ( poly->vertex ) = (tpunkt*) realloc (poly->vertex, poly->vertexnum * sizeof ( poly->vertex[0] ) );
      if ( poly->vertexnum >= 3) setpolytemps(1);
      setpolypoints(2);
      return;
   }
}


void  tchangepoly::run(void)
{
   int            x,y;

   x=0;
   y=0;
   if ( poly->vertexnum >= 3) setpolytemps(1);
   setpolypoints(2);
   int r;
   displaymessage("use space to select the vertices of the polygon\nfinish the selection by pressing enter",3);
   do {
      mousevisible(false);
      displaymap();
      mousevisible(true);
      r = selectfield(&x,&y);
      if ( r != 1   &&   (x != 50000) ) {
         if ( checkpolypoint(x,y) == 0 ) {
            if ( poly->vertexnum >= 3) setpolytemps(0);
            else setpolypoints(0);
            poly->vertexnum++;
            if (poly->vertexnum > 1) ( poly->vertex ) = (tpunkt*) realloc (poly->vertex, poly->vertexnum * sizeof ( poly->vertex[0] ) );
            poly->vertex[poly->vertexnum-1].x=x;
            poly->vertex[ poly->vertexnum-1].y=y;
            if ( poly->vertexnum >= 3) setpolytemps(1);
            setpolypoints(2);
         }
         else deletepolypoint(x,y);
      } 
   } while ( r != 1 ); /* enddo */
   if ( poly->vertexnum >= 3) setpolytemps(0);
   else setpolypoints(0);
}

void changepolygon(ppolygon poly)
{ 
  tchangepoly cp;

  cp.poly = poly;
  cp.run();

}


//* õS NewMap
   
  class tnewmap : public tdialogbox {
            char maptitle[10000];
        public :
               int action;
               char passwort[11];
               int sxsize,sysize;
               char valueflag,random,campaign;
               tmap::Campaign cmpgn;
               pterraintype         tauswahl;
               word auswahlw;
               void init(void);
               virtual void run(void);
               virtual void buttonpressed(int  id);
               void done(void);
               };


void         tnewmap::init(void)
{ 
  word         w;
  char      b;

   tdialogbox::init();
   action = 0;
   if (valueflag == true )
      title = "New Map";
   else
      title = "Map Values";
   x1 = 70;
   xsize = 500;
   y1 = 70;
   ysize = 350;
   campaign = !!actmap->campaign;
   sxsize = actmap->xsize;
   sysize = actmap->ysize;
   strcpy ( maptitle, actmap->maptitle.c_str() );

   if (valueflag == true ) {
      strcpy(passwort,"");
      memset(&cmpgn,0,sizeof(cmpgn));
   }
   else {
      strcpy(passwort, actmap->codeword);
      if (actmap->campaign != NULL) {
         campaign = true;
         memcpy (&cmpgn , actmap->campaign, sizeof(cmpgn));
      }
      else memset(&cmpgn,0,sizeof(cmpgn));
   }
   random = false; 
   auswahlw = 0; /* !!! */

   w = (xsize - 60) / 2; 
   windowstyle = windowstyle ^ dlg_in3d; 

   addbutton("~T~itle",15,70,xsize - 30,90,1,1,1,true); 
   addeingabe(1,maptitle,0,100);

   if (valueflag == true ) {
      addbutton("~X~ Size",15,130,235,150,2,1,2,true);
      addeingabe(2,&sxsize,idisplaymap.getscreenxsize(1),65534);

      addbutton("~Y~ Size",250,130,470,150,2,1,3,true); 
      addeingabe(3,&sysize,idisplaymap.getscreenysize(1),65534);

      addbutton("~R~andom",250,190,310,210,3,1,11,true);
      addeingabe(11,&random,0,lightgray);

      if ( ! random) b = true;
      else b =false;

      addbutton("~B~dt",350,190,410,210,0,1,12,b);
   } 

   addbutton("~P~assword (10 letters)",15,190,235,210,1,1,9,true);
   addeingabe(9,passwort,10,10);

   addbutton("C~a~mpaign",15,230,235,245,3,1,5,true); 
   addeingabe(5,&campaign,0,lightgray);

   addbutton("~M~ap ID",15,270,235,290,2,1,6,campaign); 
   addeingabe(6,&cmpgn.id,0,65535);

   addbutton("Pr~e~vious Map ID",250,270,470,290,2,1,7,campaign); 
   addeingabe(7,&cmpgn.prevmap,0,65535);

   addbutton("~D~irect access to map",250,230,470,245,3,1,8,campaign); 
   addeingabe(8,&cmpgn.directaccess,0,lightgray);

   if (valueflag == true ) addbutton("~S~et Map",20,ysize - 40,20 + w,ysize - 10,0,1,10,true);
   else addbutton("~S~et Mapvalues",20,ysize - 40,20 + w,ysize - 10,0,1,10,true); 
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,4,true); 

   tauswahl = auswahl;

   buildgraphics(); 
   if (valueflag == true )
      if ( ! random ) {
         mousevisible(false); 
         if ( tauswahl->weather[auswahlw] ) 
            putspriteimage(x1 + 440,y1 + 182,tauswahl->weather[auswahlw]->pict );
         else
            putspriteimage(x1 + 440,y1 + 182,tauswahl->weather[0]->pict );
         mousevisible(true); 
      } 
   rahmen(true,x1 + 10,y1 + starty,x1 + xsize - 10,y1 + ysize - 45); 
   rahmen(true,x1 + 11,y1 + starty + 1,x1 + xsize - 11,y1 + ysize - 46); 
   mousevisible(true); 
} 


void         tnewmap::run(void)
{ 
   do { 
      tdialogbox::run();
      if (action == 3) {
         if ( sysize & 1 ) {
            displaymessage("YSize must be even !",1 ); 
            action = 0; 
         }
         #ifdef UseMemAvail
         if (action != 4) 
            if ( sxsize * sysize * sizeof( tfield ) > memavail() ) {
               displaymessage("Not enough memory for map.\nGenerate smaller map or free more memory",1);
               action = 0; 
            }
         #endif
      } 
   }  while (!((taste == ct_esc) || (action >= 2)));

   if (action == 3) { 
      if (valueflag == true ) {
         if ( tauswahl->weather[auswahlw] )
            generatemap(tauswahl->weather[auswahlw], sxsize , sysize );
         else
            generatemap(tauswahl->weather[0], sxsize , sysize );
         if ( random) 
            mapgenerator();
      }   

      mapsaved = false;

      actmap->maptitle = maptitle;

      strcpy(actmap->codeword,passwort);
      if (campaign == true ) {
         if (actmap->campaign == NULL)
            actmap->campaign = new tmap::Campaign;

         actmap->campaign->id = cmpgn.id; 
         actmap->campaign->prevmap = cmpgn.prevmap; 
         actmap->campaign->directaccess = cmpgn.directaccess; 
      } 
      else
         if ( actmap->campaign ) {
            delete actmap->campaign;
            actmap->campaign = NULL;
         }
   } 
} 


void         tnewmap::buttonpressed(int id)
{ 
   if (id == 4) 
      action = 2; 
   if (id == 10) 
      action = 3; 
   if (id == 5) 
      if (campaign) { 
         enablebutton(6); 
         enablebutton(7); 
         enablebutton(8); 
      } 
      else { 
         disablebutton(6); 
         disablebutton(7); 
         disablebutton(8); 
      } 
   if (id == 12) { 
      npush ( lastselectiontype );
      npush ( auswahl );

      void *p;
 
      mousevisible(false);
      p=malloc( imagesize(430,0,639,479) );
      getimage(430,0,639,479,p);
      mousevisible(true);
                  
      lastselectiontype = cselbodentyp;
      selterraintype( ct_invvalue );

      mousevisible(false);
      putimage(430,0,p);
      mousevisible(true);

      tauswahl = auswahl;

      npop ( auswahl );
      npop ( lastselectiontype );

      if ( tauswahl->weather[auswahlw] )
         putspriteimage(x1 + 440,y1 + 182,tauswahl->weather[auswahlw]->pict );
      else
         putspriteimage(x1 + 440,y1 + 182,tauswahl->weather[0]->pict );
   } 
   if (id == 11) 
      if ( ! random) {
         enablebutton(12); 
      } 
      else { 
         disablebutton(12); 
      } 
} 


void         tnewmap::done(void)
{ 
   tdialogbox::done();
   if (action == 3) displaymap();
}



void         newmap(void)
{ 
  tnewmap      nm; 

   nm.valueflag = true; 
   nm.init(); 
   nm.run(); 
   nm.done(); 
   pdbaroff(); 
} 


//* õS MapVals


void         changemapvalues(void)
{ 
  tnewmap      nm; 

   nm.valueflag = false; 
   nm.init(); 
   nm.run(); 
   nm.done(); 
//   if (actmap->campaign != NULL) setupalliances();
   pdbaroff(); 
}



     class BuildingValues : public tdialogbox {
               int action;
               Building& gbde;
               int rs,mrs;
               Resources plus,mplus, biplus,storage;
               int col;
               char tvisible;
               char name[260];
               int ammo[waffenanzahl];
               virtual void buttonpressed(int id);
               int lockmaxproduction;
               TemporaryContainerStorage buildingBackup; // if the editing is cancelled

           public :
               BuildingValues ( Building& building ) : gbde ( building ), buildingBackup ( &building ) {};
               void init(void);
               virtual void run(void);
            };


void         BuildingValues::init(void)
{
  char      b;

  for ( int i = 0; i< waffenanzahl; i++ )
     ammo[i] = gbde.ammo[i];

   lockmaxproduction = 1;

   tdialogbox::init();
   action = 0;
   title = "Building Values";
   x1 = 0;
   xsize = 640;
   y1 = 10;
   ysize = 440;
   // int w = (xsize - 60) / 2;
   action = 0;

   windowstyle = windowstyle ^ dlg_in3d;

   storage = gbde.actstorage;
   plus = gbde.plus;
   mplus = gbde.maxplus;
   rs = gbde.researchpoints;
   mrs = gbde.maxresearchpoints;
   tvisible = gbde.visible;
   biplus = gbde.bi_resourceplus;
   col = gbde.color / 8;

   strcpy( name, gbde.name.c_str() );

   addbutton("~N~ame",15,50,215,70,1,1,10,true);
   addeingabe(10,&name[0],0,25);

   addbutton("~E~nergy-Storage",15,90,215,110,2,1,1,true);
   addeingabe(1,&storage.energy,0,gbde.gettank(0));

   addbutton("~M~aterial-Storage",15,130,215,150,2,1,2,true);
   addeingabe(2,&storage.material,0,gbde.gettank(1));

   addbutton("~F~uel-Storage",15,170,215,190,2,1,3,true);
   addeingabe(3,&storage.fuel,0,gbde.gettank(2));

   if ( gbde.typ->special & (cgconventionelpowerplantb | cgsolarkraftwerkb | cgwindkraftwerkb | cgminingstationb )) b = true;
   else b = false;

   addbutton("Energy-Max-Plus",230,50,430,70,2,1,13,b);
   addeingabe(13,&mplus.energy,0,gbde.typ->maxplus.energy);

   if ( gbde.typ->special & (cgconventionelpowerplantb | cgminingstationb )) b = true;
   else b = false;

   addbutton("Energ~y~-Plus",230,90,430,110,2,1,4,b);
   addeingabe(4,&plus.energy,0,mplus.energy);

   if ( (gbde.typ->special & cgconventionelpowerplantb) || ((gbde.typ->special & cgminingstationb ) && gbde.typ->efficiencymaterial ))
      b = true;
   else
      b = false;

   addbutton("Material-Max-Plus",230,130,430,150,2,1,14,b);
   addeingabe(14,&mplus.material,0,gbde.typ->maxplus.material);

   addbutton("M~a~terial-Plus",230,170,430,190,2,1,5,b);
   addeingabe(5,&plus.material,0,mplus.material);

   if ( (gbde.typ->special & cgconventionelpowerplantb) || ((gbde.typ->special & cgminingstationb ) && gbde.typ->efficiencyfuel ))
      b = true;
   else
      b = false;

   addbutton("Fuel-Max-Plus",230,210,430,230,2,1,15,b);
   addeingabe(15,&mplus.fuel,0,gbde.typ->maxplus.fuel);

   addbutton("F~u~el-Plus",230,250,430,270,2,1,6,b);
   addeingabe(6,&plus.fuel, 0, mplus.fuel);

   if ( (  ( gbde.typ->special & cgresearchb ) > 0) && ( gbde.typ->maxresearchpoints > 0)) b = true;
   else b = false;

   addbutton("~R~esearch-Points",15,210,215,230,2,1,9,b);
   addeingabe(9,&rs,0,gbde.typ->maxresearchpoints);

   addbutton("Ma~x~research-Points",15,250,215,270,2,1,11,b);
   addeingabe(11,&mrs,0,gbde.typ->maxresearchpoints);


   addbutton("~V~isible",15,290,215,300,3,1,12,true);
   addeingabe(12,&tvisible,0,lightgray);

   addbutton("~L~ock MaxPlus ratio",230,290,430,300,3,1,120,true);
   addeingabe(120,&lockmaxproduction,0,lightgray);


   addbutton("~C~olor",230,370,430,390,2,1,104,true);
   addeingabe(104,&col,0,8);


   addbutton("BI energy plus",15,330,215,350,2,1,101,1);
   addeingabe(101,&biplus.energy,0,maxint);

   addbutton("BI material plus",230,330,430,350,2,1,102,1);
   addeingabe(102,&biplus.material,0,maxint);

   addbutton("BI fuel plus",15,370,215,390,2,1,103,1);
   addeingabe(103,&biplus.fuel,0,maxint);


   addbutton("~S~et Values",10,ysize - 40, xsize/3-5,ysize - 10,0,1,7,true);
   addkey(7,ct_enter);

   addbutton("Help (~F1~)", xsize/3+5, ysize - 40, xsize/3*2-5, ysize-10, 0, 1, 110, true );
   addkey(110, ct_f1 );

   addbutton("~C~ancel",xsize/3*2+5,ysize - 40,xsize-10,ysize - 10,0,1,8,true);
   addkey(8, ct_esc );


   for ( int i = 0; i < waffenanzahl; i++ ) {
      addbutton ( cwaffentypen[i], 460, 90+i*40, 620, 110+i*40, 2, 1, 200+i, 1 );
      addeingabe(200+i, &ammo[i], 0, maxint );
   }

   buildgraphics();
   line(x1+450, y1+45, x1+450, y1+55+9*40, darkgray );
   activefontsettings.length =0;
   activefontsettings.font = schriften.smallarial;
   showtext2("ammunition:", x1 + 460, y1 + 50 );

   mousevisible(true);
}


void         BuildingValues::run(void)
{
   do {
      tdialogbox::run();
   }  while (!((taste == ct_esc) || (action == 1)));
}


void         BuildingValues::buttonpressed(int         id)
{
   switch(id) {
   case 4:            // energy, material & fuel plus
   case 5:
   case 6: {
              int changed_resource = id - 4;
              for ( int r = 0; r < 3; r++ )
                 if ( r != changed_resource )
                    if ( mplus.resource(changed_resource) ) {
                       int a = mplus.resource(r) * plus.resource(changed_resource) / mplus.resource(changed_resource);
                       if ( a != plus.resource(r) ) {
                          plus.resource(r) = a;
                          showbutton ( 4 + r );
                       }
                    }
            }
      break;
   case 13:        // energy, material and fuel maxplus
   case 14:
   case 15: {
              int changed_resource = id - 13;
              if ( lockmaxproduction )
                 for ( int r = 0; r < 3; r++ )
                    if ( r != changed_resource )
                       if ( gbde.typ->maxplus.resource(changed_resource) ) {
                          int a = gbde.typ->maxplus.resource(r) * mplus.resource(changed_resource) / gbde.typ->maxplus.resource(changed_resource);
                          if ( a != mplus.resource(r) ) {
                             mplus.resource(r) = a;
                             showbutton ( 13 + r );
                          }
                       }

              for ( int r = 0; r < 3; r++ ) {
                 if ( (mplus.resource(r) >= 0 && plus.resource(r) > mplus.resource(r) ) ||
                      (mplus.resource(r) <  0 && plus.resource(r) < mplus.resource(r) )) {
                         plus.resource(r) = mplus.resource(r);
                         showbutton ( 4 + r );
                 }
                 addeingabe(4+r, &plus.resource(r), 0, mplus.resource(r) );
              }
            }
      break;
     case 7: {
           mapsaved = false;
           action = 1;
           gbde.actstorage = storage;

           gbde.plus = plus;
           gbde.maxplus = mplus;
           if ( col != gbde.color/8 )
              gbde.convert ( col );

           gbde.researchpoints = rs;
           gbde.maxresearchpoints = mrs;
           gbde.visible = tvisible;
           gbde.bi_resourceplus = biplus;
           gbde.name = name;
           for ( int i = 0; i< waffenanzahl; i++ )
               gbde.ammo[i] = ammo[i];

        }
        break;
     case 8: action = 1; 
        break;
        
     /*
     
     case 13: {
                     addeingabe(4,&plus.energy,0,mplus.energy);
                     if (mplus.energy < plus.energy ) {
                        plus.energy = mplus.energy;
                        enablebutton(4);
                     } 
                  }
        break;
     case 14: {
                   addeingabe(5,&plus.material,0,mplus.material);
                      if (mplus.material < plus.material) {
                         plus.material =mplus.material;
                         enablebutton(5);
                      } 
                   }
     
        break;
     case 15: {
                   addeingabe(6,&plus.fuel,0,mplus.fuel);
                      if (plus.fuel > mplus.fuel) {
                         plus.fuel = mplus.fuel;
                         enablebutton(6);
                      } 
                   } 
        break;
     */   
  case 110: help ( 41 );
     break;

  }
} 


void         changebuildingvalues( Building& b )
{
   BuildingValues bval ( b );
   bval.init();
   bval.run();
   bval.done();

   displaymap();
} 

// õS Class-Change


class   tclass_change: public tstringselect {
           public :
                 pvehicle unit;
                 word tklasse,tarmor,tfunktion;
                 word tweapstr[8];
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void gettext(word nr);
                 };

void         tclass_change::setup(void)
{  int i,j;
   char s[200], *s2;

   action = 0;
   title = "Change Class";

   x1 = 50;
   y1 = 30;
   xsize = 540;
   ysize = 420;
   ex = 200;
   ey = 150;
   lnshown = 5;
   numberoflines = unit->typ->classnum;

   if (unit->klasse > unit->typ->classnum-1) unit->klasse = 0;
      redline = unit->klasse;
   tklasse = unit->klasse;
   for (j = 0 ; j < unit->typ->weapons.count  ; j++ ) {
       //tweapstr[j] = unit->weapstrength[j];
       tweapstr[j] = unit->typ->weapons.weapon[j].maxstrength * unit->typ->classbound[tklasse].weapstrength[ unit->typ->weapons.weapon[j].getScalarWeaponType() ] / 1024;
   }
   //tarmor = unit->armor;
   tarmor = unit->typ->armor * unit->typ->classbound[tklasse].armor / 1024;
   addbutton("~A~rmor",220,210,420,230,2,1,4,true);
   addeingabe(4,&tarmor,1,65535);
   for (i = 0 ; i < unit->typ->weapons.count  ; i++ ) {
      strcpy ( s, "" );

      strcat( s, cwaffentypen[unit->typ->weapons.weapon[i].getScalarWeaponType() ] );
      strcat( s, " ");

      strcat(s,"(~");
      strcat(s,strrr(i+1));
      strcat(s,"~)");
      s2 = (char * ) malloc( strlen(s) +1 );
      strcpy(s2,s);
      addbutton(s2,220,90+ i * 40 ,420,110 + i * 40,2,1,i+5,true);
      addeingabe(i+5,&tweapstr[i],1,65535);
   }
   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,20,true);
   addkey(20,ct_enter);
   addbutton("~C~ancel",190,ysize - 40,340,ysize - 20,0,1,21,true);
}


void         tclass_change::buttonpressed(int         id)
{
   tstringselect::buttonpressed(id);
   switch (id) {

      case 20:
      case 21:   action = id-18;
   break;
   }
}


void         tclass_change::gettext(word nr)
{
   strcpy(txt,unit->typ->classnames[nr].c_str());
}


void         tclass_change::run(void)
{
   int j;

   do {
      tstringselect::run();
      if ( (msel == 1 ) || ( taste == ct_enter ) ){
         if (tklasse != redline) {
            tklasse = redline;
            for (j = 0 ; j < unit->typ->weapons.count  ; j++ ) {
               tweapstr[j] = unit->typ->weapons.weapon[j].maxstrength * unit->typ->classbound[tklasse].weapstrength[ unit->typ->weapons.weapon[j].getScalarWeaponType() ] / 1024;
               showbutton(j+5);
            }
            tarmor = unit->typ->armor * unit->typ->classbound[tklasse].armor / 1024;
            tfunktion = unit->typ->functions  & unit->typ->classbound[tklasse].vehiclefunctions;

            showbutton(4);
         }
      }
   }  while ( ! ( (taste == ct_esc) || ( (action == 2) || (action == 3) ) ) );
   if (action == 2) {
      unit->klasse = tklasse;
      for (j = 0 ; j < unit->typ->weapons.count  ; j++ ) unit->weapstrength[j] = tweapstr[j];
      unit->armor = tarmor;
      unit->functions = tfunktion;
   }
}


void         class_change(pvehicle p)
{
  tclass_change cc;

   cc.unit = p;
   cc.init();
   cc.run();
   cc.done();
}


// õS Polygon-Management

class tpolygon_managementbox: public tstringselect {
              public:
                 ppolygon poly;
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void gettext(word nr);
                 };


tpolygon_management::tpolygon_management(void)
{
   polygonanz = 0;
   lastpolygon = firstpolygon;
}

void         tpolygon_managementbox::setup(void)
{
   action = 0;
   title = "Choose Polygon";

   x1 = 70;
   y1 = 40;
   xsize = 500;
   ysize = 400;
   lnshown = 10;
   numberoflines = polymanage.polygonanz;
   activefontsettings.length = xsize - 30;
   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,1,true);
   addkey(1,ct_enter);
   addbutton("~C~ancel",190,ysize - 40,340,ysize - 20,0,1,2,true);
}


void         tpolygon_managementbox::buttonpressed(int         id)
{
   tstringselect::buttonpressed(id);
   switch (id) {
      case 1:
      case 2:   action = id;
   break;
   }
}


void         tpolygon_managementbox::gettext(word nr)
{
   char s[200];
   ppolystructure pps;
   int i,vn;

   const int showmaxvertex = 5;

   pps = polymanage.firstpolygon;
   for (i=0 ;i<nr; i++ )
       pps =pps->next;
   strcpy(s,"");
   if ( pps->poly->vertexnum> showmaxvertex ) vn=5;
   else vn = pps->poly->vertexnum;
   switch (pps->place) {
   case 0: {
      strcat(s,"Events: ");
      strcat(s,ceventactions[pps->id]);
      }
      break;
   }
   for (i=0;i <= vn-1;i++ ) {
       strcat(s,"(");
       strcat(s,strrr(pps->poly->vertex[i].x));
       strcat(s,"/");
       strcat(s,strrr(pps->poly->vertex[i].y));
       strcat(s,") ");
   } /* endfor */
   if (vn < pps->poly->vertexnum )  strcat(s,"...");
   strcpy(txt,s);
}


void         tpolygon_managementbox::run(void)
{
   do {
      tstringselect::run();
      if (taste ==ct_f1) help (1030);
   }  while ( ! ( (taste == ct_enter ) || (taste == ct_esc) || ( (action == 1) || (action == 2) ) ) );
   if ( ( ( taste == ct_enter ) || (action == 1 ) ) && (redline != 255 ) ) {
      ppolystructure pps;

      pps = polymanage.firstpolygon;
      for (i=0 ;i<redline; i++ ) pps =pps->next;
      poly = pps->poly;
   }
}

void tpolygon_management::addpolygon(ppolygon *poly, int place, int id)
{
   (*poly) = new(tpolygon);

   (*poly)->vertexnum = 0;
   (*poly)->vertex    = (tpunkt*) malloc ( 1 * sizeof ( (*poly)->vertex[0] ) );
   if (polygonanz == 0 ) {
      lastpolygon = new ( tpolystructure );
      firstpolygon = lastpolygon;
   }
   else {
      lastpolygon->next = new ( tpolystructure );
      lastpolygon = lastpolygon->next;
   }
   lastpolygon->poly = (*poly);
   lastpolygon->id = id;
   lastpolygon->place = place;
   polygonanz++;
}

void tpolygon_management::deletepolygon(ppolygon *poly)
{
   asc_free( (*poly)->vertex );
   asc_free(*poly);
}


int        getpolygon(ppolygon *poly) //return Fehlerstatus
{
   tpolygon_managementbox polymanagebox;

   polymanagebox.poly = (*poly);
   polymanagebox.init();
   polymanagebox.run();
   polymanagebox.done();
   (*poly) = polymanagebox.poly;
   if ( (polymanagebox.action == 2) || (polymanagebox.taste == ct_esc ) ) return 1;
   else return 0;
}

// õS Unit-Values


class   StringSelector : public tstringselect {
                 const char** text;
           public :
                 int lastchoice;
                 StringSelector ( char* title_, const char** text_, int itemNum_ ) : text ( text_ )  { lastchoice = 0; numberoflines = itemNum_; title = title_; };
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void gettext(word nr);
                 };

void         StringSelector ::setup(void)
{
   action = 0;
   ey = ysize - 90;
   startpos = lastchoice;
   addbutton("~O~k",20,ysize - 50,xsize-20,ysize - 20,0,1,13,true);
   addkey ( 13, ct_enter );
}


void         StringSelector ::buttonpressed(int         id)
{
   tstringselect::buttonpressed(id);
   switch (id) {

      case 12:  if ( redline >= 0 )
                   action = id-10;
                break;

      case 13:   action = id-10;
                 break;
   }
}


void         StringSelector ::gettext(word nr)
{
   strcpy(txt, text[nr] );
}


void         StringSelector ::run(void)
{
   do {
      tstringselect::run();
      if ( taste == ct_enter )
         if ( redline >= 0 )
            action = 2;
         else
            action = 3;
   }  while ( action == 0 );
}


int selectString( int lc, char* title, const char** text, int itemNum )
{
   StringSelector  ss ( title, text, itemNum );
   ss.lastchoice = lc;
   ss.init();
   ss.run();
   ss.done();
   return ss.redline;
}


class EditAiParam : public tdialogbox {
           pvehicle unit;
           TemporaryContainerStorage tus;
           int action;
           AiParameter& aiv;
        public:
           EditAiParam ( pvehicle veh, int player ) : unit ( veh ), tus ( veh ), aiv ( *veh->aiparam[player] ) {};
           void init ( );
           void run ( );
           void buttonpressed ( int id );
           int  getcapabilities ( void ) { return 1; };
};


void         EditAiParam::init(  )
{
   tdialogbox::init();
   action = 0;
   title = "Unit AI Values";

   windowstyle = windowstyle ^ dlg_in3d;

   x1 = 20;
   xsize = 600;
   y1 = 40;
   ysize = 400;
   int w = (xsize - 60) / 2;
   action = 0;

   addbutton("dest ~X~",50,80,250,100,2,1,1,true);
   addeingabe(1, &aiv.dest.x, minint, maxint);

   addbutton("dest ~Y~",50,120,250,140,2,1,2,true);
   addeingabe(2, &aiv.dest.y, minint, maxint);

   addbutton("dest ~Z~",50,160,250,180,2,1,3,true);
   addeingabe(3, &aiv.dest.z, minint, maxint);

   addbutton("dest ~N~WID",50,200,250,220,2,1,4,true);
   addeingabe(4, &aiv.dest_nwid, minint, maxint);

   addbutton("~V~alue",410,80,570,100,2,1,10,true);
   addeingabe( 10, &aiv.value, minint, maxint );

   addbutton("~A~dded value",410,120,570,140,2,1,11,true);
   addeingabe( 11, &aiv.addedValue, minint, maxint );


   addbutton("~S~elect Dest",50,240,250,260,0,1,22,true);

   addbutton("~T~ask",20,ysize - 70,20 + w,ysize - 50,0,1,20,true);
   addbutton("~J~ob",40 + w,ysize - 70,40 + 2 * w,ysize - 50,0,1,21,true);

   addbutton("~S~et Values",20,ysize - 40,20 + w,ysize - 10,0,1,30,true);
   addkey(30,ct_enter );
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,31,true);
   addkey(31,ct_esc );

   buildgraphics();
   mousevisible(true);
}


void         EditAiParam::run(void)
{
   do {
      tdialogbox::run();
   }  while ( !action );
}


void         EditAiParam::buttonpressed(int         id)
{
   switch (id) {
   case 20 : aiv.setTask( AiParameter::Task( selectString ( aiv.getTask(), "Select Task", AItasks , AiParameter::taskNum) ));
             break;
   case 21 : {
                AiParameter::JobList j;
                j.push_back ( AiParameter::Job( selectString ( aiv.getJob(), "Select Job", AIjobs , AiParameter::jobNum) ) );
                aiv.setJob ( j );
             }
             break;
   case 22 : getxy ( &aiv.dest.x, &aiv.dest.y );
             redraw();
             break;

   case 30 : action = 1;
             break;
   case 31 : action = 1;
              tus.restore();
        break;
   } /* endswitch */
}




     class tunit: public tdialogbox {
                TemporaryContainerStorage tus;
                word        dirx,diry;
                int        action;
                pvehicle    unit;
                int         w2, heightxs;
                char        namebuffer[1000];
                char        reactionfire;
              public:
               // char     checkvalue( char id, char* p );
                tunit ( pvehicle v ) : tus ( v ), unit ( v ) {};
                void        init( void );
                void        run( void );
                void        buttonpressed( int id );
     };


void         tunit::init(  )
{
   word         w;
   char *weaponammo;

   tdialogbox::init();
   action = 0;
   title = "Unit-Values";

   windowstyle = windowstyle ^ dlg_in3d;

   x1 = 20;
   xsize = 600;
   y1 = 40;
   ysize = 400;
   w = (xsize - 60) / 2;
   w2 = (xsize - 40) / 8;
   dirx= x1 + 300;
   diry= y1 + 100;
   action = 0;


   strcpy ( namebuffer, unit->name.c_str() );

   addbutton("~N~ame",50,80,250,100,1,1,29,true);
   addeingabe(29, namebuffer, 0, 100);

   addbutton("E~x~p of Unit",50,120,250,140,2,1,1,true);
   addeingabe(1, &unit->experience, 0, maxunitexperience);

   addbutton("~D~amage of Unit",50,160,250,180,2,1,2,true);
   addeingabe(2, &unit->damage, 0, 100 );

   addbutton("~F~uel of Unit",50,200,250,220,2,1,3,true);
   addeingabe( 3, &unit->tank.fuel, 0, unit->typ->tank.fuel );

   addbutton("~M~aterial",50,240,250,260,2,1,12,true);
   addeingabe(12,&unit->tank.material, 0, unit->typ->tank.material );

   addbutton("AI Parameter", 50, 280, 250, 300, 0, 1, 115, true );

   int unitheights = 0;
   heightxs = 320;
   pfield fld = getfield ( unit->xpos, unit->ypos);
   if ( fld && fld->vehicle == unit ) {
      npush ( unit->height );
      for (i=0;i<=7 ;i++) {
          unit->height = 1 << i;
          if (( ( unit->height & unit->typ->height ) > 0) && ( terrainaccessible( fld, unit ) == 2)) {
             addbutton("",20+( i * w2),heightxs,w2 * (i +1 ),heightxs+24,0,1,i+4,true);
             addkey(i+4,ct_0+i);
          }
          unitheights |= 1 << i;
      } /* endfor  Buttons 4-11*/
      npop ( unit->height );
   }

   if ( unit->typ->classnum > 0 ) addbutton("C~h~ange Class",280,280,450,300,0,1,32,true);

   addbutton("~R~eactionfire",dirx-50,250,dirx+50,260,3,1,22,true);
   reactionfire = unit->reactionfire.getStatus();
   addeingabe(22, &reactionfire, 0, lightgray);

   addbutton("~S~et Values",20,ysize - 40,20 + w,ysize - 10,0,1,30,true);
   addkey(30,ct_enter );
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,31,true);
   addkey(31,ct_esc );

   #define maxeditable 6

   for(i =0;i < unit->typ->weapons.count;i++) {   	
     if (i < maxeditable) {
        weaponammo = new(char[25]);
        strcpy(weaponammo,"Wpn Ammo ");
        strcat(weaponammo,strrr(i+1));
        strcat(weaponammo," (0-255)");
        addbutton(weaponammo,410,80+i*40,570,100+i*40,2,1,33+i,true);
        addeingabe( 33+i, &unit->ammo[i], 0, 255 );
   	} else
   	    showtext2("6 weapons max at the moment",x1+410,y1+80+maxeditable*40);
   	
   }

   buildgraphics();

   mousevisible(false);

   if ( unitheights )
       for (i=0;i<=7 ;i++) {
           if ( unit->height == (1 << i) )
              bar(x1 + 25+( i * w2),y1 + heightxs-5,x1 + w2 * (i +1 ) - 5,y1 + heightxs-3,red);

           if ( unitheights & ( 1 << i ))
              putimage(x1 + 28+( i * w2), y1 + heightxs + 2 ,icons.height[i]);
       }

   // 8 im Kreis bis 7
   float radius = 50;
   float pi = 3.14159265;
   for ( int i = 0; i < sidenum; i++ ) {

      int x = (int) (dirx + radius * sin ( 2 * pi * (float) i / (float) sidenum ));
      int y = (int) (diry - radius * cos ( 2 * pi * (float) i / (float) sidenum ));

      addbutton("", x-10, y - 10, x + 10, y + 10,0,1,14+i,true);
      enablebutton ( 14 + i );
      void* pic = rotatepict ( icons.pfeil2[0], directionangle[i] );
      int h,w;
      getpicsize ( pic, w, h );
      putspriteimage ( x1 + x - w/2, y1 + y - h/2, pic );
      delete pic;
  } /*Buttons 14 - 14 +sidenum*/

   putrotspriteimage(dirx + x1 - fieldsizex/2 ,diry + y1 - fieldsizey/2, unit->typ->picture[ unit->direction ],unit->color);
   mousevisible(true);
}

/*
char      tunit::checkvalue( char id, void* p)
{
   if ( id == 3 ) {
   }
   addbutton("~F~uel of Unit",50,200,250,220,2,1,3,true);
   addeingabe( 3, &unit->fuel, 0, unit->getmaxfuelforweight() );

   addbutton("~M~aterial",50,240,250,260,2,1,12,true);
   addeingabe(12,&unit->material, 0, unit->getmaxmaterialforweight() );
   return  true;
}
*/

void         tunit::run(void)
{
   do {
      tdialogbox::run();
   }  while ( !action );
}


void         tunit::buttonpressed(int         id)
{
   int ht;
   int temp, storage;

   switch (id) {
   case 3:  temp = unit->tank.material;
            unit->tank.material = 0;
            storage = unit->putResource(maxint, Resources::Material, 1 );
            addeingabe(12,&unit->tank.material, 0, storage );
            unit->tank.material = min ( storage, temp );
      break;

   case 4 :
   case 5 :
   case 6 :
   case 7 :
   case 8 :
   case 9 :
   case 10 :
   case 11 :
   {
     int h = 1 << ( id - 4 );
     bar(x1 +20,y1 + heightxs-5,x1 + 480,y1 + heightxs-3,lightgray);
     for (i=0;i<=7 ;i++) {
        ht = 1 << i;
        if ( ht == h ) bar(x1 + 25+( i * w2),y1 + heightxs-5,x1 + w2 * (i +1 ) - 5,y1 + heightxs-3,red);
     } /* endfor */
     unit->height = h;
     unit->setMovement ( unit->typ->movement[ log2 ( unit->height ) ] );
   }
   break;
   case 12: temp = unit->tank.fuel;
            unit->tank.fuel = 0;
            storage = unit->putResource(maxint, Resources::Fuel, 1 );
            addeingabe( 3, &unit->tank.fuel, 0, storage );
            unit->tank.fuel = min ( storage, temp );
      break;

   case 14 :
   case 15 :
   case 16 :
   case 17 :
   case 18 :
   case 19 :
   case 20 :
   case 21 :  {
                  unit->direction = id-14;
                  bar(dirx + x1 -fieldsizex/2, diry + y1 - fieldsizey/2 ,dirx + x1 + fieldsizex/2 ,diry + y1 +fieldsizey/2,lightgray);
                  putrotspriteimage(dirx + x1 - fieldsizex/2 ,diry + y1 - fieldsizey/2, unit->typ->picture[ unit->direction ],unit->color);
               }
         break;
   case 30 : {
         mapsaved = false;
         action = 1;
         if ( reactionfire )  {
            unit->reactionfire.enable();
            unit->reactionfire.enemiesAttackable = 0xff;
         } else
            unit->reactionfire.disable();

         unit->name = namebuffer;
        }
        break;
    case 31 : action = 1;
              tus.restore();
        break;
    case 32: class_change( unit );
        break;
    case 115: {
                 static int player = 0;
                 player = getid("player",player,0,7);
                 if ( !unit->aiparam[player] )
                    unit->aiparam[player] = new AiParameter ( unit );

                 EditAiParam eap ( unit, player );
                 eap.init();
                 eap.run();
              }
   } /* endswitch */
}


void         changeunitvalues(pvehicle ae)
{
   if ( !ae )
      return;

   tunit units ( ae );
   units.init();
   units.run();
   units.done();
}


//* õS Resource

     class tres: public tdialogbox {
            public :
                int action;
                int fuel,material;
                void init(void);
                virtual void run(void);
                virtual void buttonpressed(int id);
                };



void         tres::init(void)
{ word         w;

   tdialogbox::init();
   action = 0;
   title = "Resources";
   x1 = 170;
   xsize = 200;
   y1 = 70;
   ysize = 200;
   w = (xsize - 60) / 2;
   action = 0;
   pf2 = getactfield();
   fuel = pf2->fuel;
   material = pf2->material;

   windowstyle = windowstyle ^ dlg_in3d;

   activefontsettings.length = 200;

   addbutton("~F~uel (0-255)",50,70,150,90,2,1,1,true);
   addeingabe(1,&fuel,0,255);
   addbutton("~M~aterial (0-255)",50,110,150,130,2,1,2,true);
   addeingabe(2,&material,0,255);

   addbutton("~S~et Vals",20,ysize - 40,20 + w,ysize - 10,0,1,7,true);
   addkey(7,ct_enter);
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,8,true);

   buildgraphics();

   mousevisible(true);
}


void         tres::run(void)
{
   do {
      tdialogbox::run();
   }  while (!((taste == ct_esc) || (action == 1)));
}


void         tres::buttonpressed(int         id)
{
   if (id == 7) {
      mapsaved = false;
      action = 1;
      pf2->fuel = fuel;
      pf2->material = material;
   }
   if (id == 8) action = 1;
}


void         changeresource(void)
{
   tres         resource;

   resource.init();
   resource.run();
   resource.done();
}

//* õS MineStrength

     class tminestrength: public tdialogbox {
            public :
                int action;
                int strength;
                void init(void);
                virtual void run(void);
                virtual void buttonpressed(int id);
                };



void         tminestrength::init(void)
{ word         w;

   tdialogbox::init();
   action = 0;
   title = "Minestrength";
   x1 = 170;
   xsize = 200;
   y1 = 70;
   ysize = 160;
   w = (xsize - 60) / 2;
   action = 0;
   pf2 = getactfield();
   strength = pf2->mines.begin()->strength;

   windowstyle = windowstyle ^ dlg_in3d;

   activefontsettings.length = 200;

   addbutton("~S~trength (0-255)",30,70,170,90,2,1,1,true);
   addeingabe(1,&strength,0,255);

   addbutton("~S~et Vals",20,ysize - 40,20 + w,ysize - 10,0,1,7,true);
   addkey(7,ct_enter);
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,8,true);

   buildgraphics();

   mousevisible(true);
}


void         tminestrength::run(void)
{
   do {
      tdialogbox::run();
   }  while (!((taste == ct_esc) || (action == 1)));
}


void         tminestrength::buttonpressed(int         id)
{
   if (id == 7) {
      mapsaved = false;
      action = 1;
      pf2->mines.begin()->strength = strength;
   }
   if (id == 8) action = 1;
}


void         changeminestrength(void)
{
   pf2 =  getactfield();
   if ( pf2->mines.empty() )
      return;

   tminestrength  ms;
   ms.init();
   ms.run();
   ms.done();
}


//* õS Pulldown

void         pulldown( void )

{
   pd.checkpulldown();
   if ( pd.action2execute >= 0 ) {
      execaction ( pd.action2execute );

      if (mouseparams.y <= pd.pdb.pdbreite )
         pd.baron();
      pd.action2execute = -1;
   }
}

//* õS Laderaum Unit-Cargo

class tladeraum : public tdialogbox {
               protected:
                    int maxusable;
                    int itemsperline;
                    int action;
                    virtual const char* getinfotext ( int pos );
                    void displayinfotext ( void );

                    virtual void additem ( void ) = 0;
                    virtual void removeitem ( int pos ) = 0;
                    virtual void checkforadditionalkeys ( tkey ch );
                    void displayallitems ( void );
                    int mouseoverfield ( int pos );
                    virtual void displaysingleitem ( int pos, int x, int y ) = 0;
                    void displaysingleitem ( int pos );
                    int cursorpos;
                    virtual void finish ( int cancel ) = 0;
                    int getcapabilities ( void ) { return 1; };
                    virtual void redraw ( void );

               public :
                    void init( char* ttl );
                    virtual void run(void);
                    virtual void buttonpressed(int id);
                    void done(void);
                 };

void tladeraum::redraw ( void )
{
   tdialogbox::redraw();
   displayallitems();
}


void         tladeraum::init( char* ttl )
{
  maxusable = 18;

   itemsperline = 8;

   npush ( farbwahl );
   cursorpos = 0;

   tdialogbox::init();
   title = ttl;
   x1 = 20;
   xsize = 600;
   y1 = 50;
   ysize = 400;
   int w = (xsize - 60) / 2;
   action = 0;

   windowstyle = windowstyle ^ dlg_in3d;

   addbutton("~N~ew",20,40,100,70,0,1,1,true);
   addbutton("~E~rase",120,40,220,70,0,1,2,true);

   addbutton("~O~K",20,ysize - 40,20 + w,ysize - 10,0,1,7,true);
   addkey(7,ct_enter);
   addbutton("Cancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,8,true);
   addkey(8,ct_esc );

   buildgraphics();

   displayallitems();
   displayinfotext();

}

void tladeraum :: displayallitems ( void )
{
   for ( int i = 0; i < 32; i++ )
      displaysingleitem ( i );
}


void tladeraum :: displaysingleitem ( int pos )
{
   int x = x1 + 28 + (pos % itemsperline) * fielddistx;
   int y = y1 + 88 + (pos / itemsperline) * fielddisty * 3;
   putspriteimage( x, y, icons.stellplatz );
   if ( pos >= maxusable )
      putspriteimage( x, y, icons.X );

   displaysingleitem ( pos, x, y );
   if ( pos == cursorpos )
      putspriteimage ( x, y, mycursor.picture );
}

int tladeraum :: mouseoverfield ( int pos )
{
   int x = x1 + 28 + (pos % itemsperline) * fielddistx;
   int y = y1 + 88 + (pos / itemsperline) * fielddisty * 3;
   return mouseinrect ( x, y, x+fieldsizex, y + fieldsizey );
}

const char* tladeraum :: getinfotext( int pos )
{
   return NULL;
}

void tladeraum :: checkforadditionalkeys ( tkey ch )
{
}

void         tladeraum::displayinfotext ( void )
{
   npush ( activefontsettings );
   activefontsettings.background = lightgray;
   activefontsettings.color = red;
   activefontsettings.length = 185;
   activefontsettings.font = schriften.smallarial;
   const char* it = getinfotext( cursorpos );
   if ( it )
      showtext2(it, x1 + 240, y1 + 50);
   npop ( activefontsettings );
}

void         tladeraum::run(void)
{
   mousevisible( true );
   do {
      tdialogbox::run();

      checkforadditionalkeys ( taste );
      int oldpos = cursorpos;
      switch (taste) {
       #ifdef NEWKEYB
         case ct_up:
       #endif
         case ct_8k:   cursorpos -= itemsperline;
            break;
       #ifdef NEWKEYB
         case ct_left:
       #endif
         case ct_4k:   cursorpos--;
            break;
       #ifdef NEWKEYB
         case ct_right:
       #endif
         case ct_6k:   cursorpos++;
            break;
       #ifdef NEWKEYB
         case ct_down:
       #endif
         case ct_2k:   cursorpos += itemsperline;
            break;
      }
      if ( cursorpos < 0 )
         cursorpos = 0;
      if ( cursorpos >= 32 )
         cursorpos = 31;

      if ( mouseparams.taste == 1)
         for ( int i = 0; i < 32; i++ )
            if ( mouseoverfield ( i ))
               cursorpos = i;

      if ( cursorpos != oldpos ) {
         displaysingleitem ( oldpos );
         displaysingleitem ( cursorpos );
         displayinfotext();
      }

   }  while ( !action );
   finish ( action == 1 );

}


void         tladeraum::buttonpressed( int id )
{
   if (id == 1) {
      additem ();
      redraw();
   }
   if (id == 2) {
      removeitem ( cursorpos );
      displayallitems();
   }
   if (id == 7) {
      mapsaved = false;
      action = 2;
   }
   if (id == 8)
      action = 1;

}


void         tladeraum::done(void)
{
   tdialogbox::done();
   npop ( farbwahl );
   ch = 0;
}




class tvehiclecargo : public tladeraum {
                    TemporaryContainerStorage tus;
               protected:
                    pvehicle transport;
                    virtual const char* getinfotext ( int pos );
                    virtual void additem ( void );
                    virtual void removeitem ( int pos );
                    virtual void checkforadditionalkeys ( tkey ch );
                    void displaysingleitem ( int pos, int x, int y );
                    virtual void finish ( int cancel );

                public:
                    tvehiclecargo ( pvehicle unit ) : tus ( unit, true ), transport ( unit ) {};
                    void init (  );


};


const char* tvehiclecargo :: getinfotext ( int pos )
{
   if ( transport->loading[ pos ] ) 
      if ( !transport->loading[ pos ]->name.empty() )
         return transport->loading[ pos ]->name.c_str();
      else
         if ( !transport->loading[ pos ]->typ->name.empty() )
            return transport->loading[ pos ]->typ->name.c_str();
         else
            return transport->loading[ pos ]->typ->description.c_str();
   return NULL;
}


void tvehiclecargo :: init (  )
{
   tladeraum::init ( "cargo" );
}

void tvehiclecargo :: displaysingleitem ( int pos, int x, int y )
{
   if ( transport->loading[ pos ] )
      putrotspriteimage ( x, y, transport->loading[ pos ]->typ->picture[0], farbwahl * 8 );
}

void tvehiclecargo :: additem  ( void )
{
   selunitcargo ( transport );
}

void tvehiclecargo :: removeitem ( int pos )
{
   if ( transport->loading[ pos ] ) {
      delete transport->loading[ pos ] ;
      transport->loading[ pos ] = NULL;
   }
}

void tvehiclecargo :: checkforadditionalkeys ( tkey ch )
{
   if ( transport->loading[ cursorpos ] ) {
       if ( ch == ct_v )  
          changeunitvalues( transport->loading[ cursorpos ] );
       if ( ch == ct_c )  
          unit_cargo( transport->loading[ cursorpos ] );
   }
} 


void tvehiclecargo :: finish ( int cancel )
{
   if ( cancel )
      tus.restore();
}


void         unit_cargo( pvehicle vh )
{ 
   if ( vh && vh->typ->loadcapacity ) {
      tvehiclecargo laderaum ( vh );
      laderaum.init();
      laderaum.run(); 
      laderaum.done(); 
   }
} 


//* õS Laderaum2 Building-Cargo

class tbuildingcargoprod : public tladeraum {
                    TemporaryContainerStorage tus;
               protected:
                    pbuilding building;
                    void finish ( int cancel );
                public:
                    tbuildingcargoprod ( pbuilding bld ) : tus ( bld, true ), building ( bld ) {};
       };


void tbuildingcargoprod :: finish ( int cancel )
{
   if ( cancel )
      tus.restore();
}

class tbuildingcargo : public tbuildingcargoprod {
               protected:
                    virtual const char* getinfotext ( int pos );
                    virtual void additem ( void );
                    virtual void removeitem ( int pos );
                    virtual void checkforadditionalkeys ( tkey ch );
                    void displaysingleitem ( int pos, int x, int y );
               public:
                    tbuildingcargo ( pbuilding bld ) : tbuildingcargoprod ( bld ) {};
   };



void tbuildingcargo :: displaysingleitem ( int pos, int x, int y )
{
   if ( building->loading[ pos ] )
      putrotspriteimage ( x, y, building->loading[ pos ]->typ->picture[0], farbwahl * 8 );
}

void tbuildingcargo :: additem  ( void )
{
   selbuildingcargo ( building );
}

void tbuildingcargo :: removeitem ( int pos )
{
   if ( building->loading[ pos ] ) {
      delete building->loading[ pos ] ;
      building->loading[ pos ] = NULL;
   }
}

void tbuildingcargo :: checkforadditionalkeys ( tkey ch )
{
   if ( building->loading[ cursorpos ] ) {
       if ( ch == ct_v )  
          changeunitvalues( building->loading[ cursorpos ] );
       if ( ch == ct_c )  
          unit_cargo( building->loading[ cursorpos ] );
   }
} 

const char* tbuildingcargo :: getinfotext ( int pos )
{
   if ( building->loading[ pos ] ) 
      if ( !building->loading[ pos ]->name.empty() )
         return building->loading[ pos ]->name.c_str();
      else
         if ( !building->loading[ pos ]->typ->name.empty() )
            return building->loading[ pos ]->typ->name.c_str();
         else
            return building->loading[ pos ]->typ->description.c_str();
   return NULL;
}


void         building_cargo( pbuilding bld )
{
   if ( bld  ) {
      tbuildingcargo laderaum ( bld );
      laderaum.init( "cargo" );
      laderaum.run(); 
      laderaum.done(); 
   }
} 

//* õS Production Building-Production

class tbuildingproduction : public tbuildingcargoprod {
               protected:
                    virtual const char* getinfotext ( int pos );
                    virtual void additem ( void );
                    virtual void removeitem ( int pos );
                    void displaysingleitem ( int pos, int x, int y );
               public:
                    tbuildingproduction ( pbuilding bld ) : tbuildingcargoprod ( bld ) {};
   };

void tbuildingproduction :: displaysingleitem ( int pos, int x, int y )
{
   if ( building->production[ pos ] )
      putrotspriteimage ( x, y, building->production[ pos ]->picture[0], farbwahl * 8 );
}

void tbuildingproduction :: additem  ( void )
{
   selbuildingproduction ( building );
}

void tbuildingproduction :: removeitem ( int pos )
{
   building->production[ pos ] = NULL;
}

const char* tbuildingproduction :: getinfotext ( int pos )
{
   if ( building->production[ pos ] ) 
      if ( !building->production[ pos ]->name.empty() )
         return building->production[ pos ]->name.c_str();
      else
         return building->production[ pos ]->description.c_str();
   return NULL;
}


void         building_production( pbuilding bld )
{
   if ( bld  && (bld->typ->special & cgvehicleproductionb ) ) {
      tbuildingproduction laderaum ( bld );
      laderaum.init( "production" );
      laderaum.run(); 
      laderaum.done(); 
   }
}


void movebuilding ( void )
{
   pfield fld = getactfield();
   if ( fld->vehicle ) {
      pvehicle v = fld->vehicle;
      fld->vehicle = NULL;
   
      int x = v->xpos;
      int y = v->ypos;
      int res2 = -1;
      do {
         res2 = selectfield ( &x, &y );
         if ( res2 > 0 ) {
            if ( getfield(x,y)->vehicle || getfield(x,y)->vehicle  || !terrainaccessible(getfield(x,y),v)) {
               displaymessage ( "you cannot place the vehicle here", 1 );
               res2 = -1;
            } else {
               getfield(x,y)->vehicle = v;
               v->setnewposition( x, y );
            }
         }
      } while ( res2 < 0  );
      displaymap();

   }
   if ( fld->building ) {
      pbuilding bld = fld->building;
 
      bld->unchainbuildingfromfield ();

      MapCoordinate mc = bld->getEntry();
      MapCoordinate oldPosition = mc;
      int res2 = -1;
      do {
         res2 = selectfield ( &mc.x, &mc.y );
         if ( res2 > 0 ) {
            int res = bld->chainbuildingtofield ( mc );
            if ( res ) {
               displaymessage ( "you cannot place the building here", 1 );
               res2 = -1;
            }
         }
      } while ( res2 < 0  );
      if ( res2 == 0 ) {   // operation canceled
         if ( bld->chainbuildingtofield ( oldPosition ))
            displaymessage ( "severe inconsistency in movebuilding !", 1 );
      } 
      displaymap();
   }
}



class SelectUnitSet : public tdialogbox {
               int* active;
               int action;
           public :
               void init(void);
               virtual void run(void);
               virtual void buttonpressed(int id);
           };


void         SelectUnitSet::init(void)
{ 

   tdialogbox::init();
   action = 0; 
   title = "Select Unit Sets";
   x1 = 90;
   xsize = 445;
   y1 = 10;
   ysize = 440;
   int w = (xsize - 60) / 2; 
   action = 0; 

   windowstyle = windowstyle ^ dlg_in3d; 



   active = new int [unitSets.size()];

   for ( int i = 0; i < unitSets.size(); i++ ) {
      active[i] = unitSets[i]->active;
      addbutton ( unitSets[i]->name.c_str(), 30, 60 + i * 25, xsize - 50, 80 + i * 25, 3, 0, 10 + i, 1 );
      addeingabe ( 10 + i, &active[i], black, dblue );
   }

   addbutton("~O~k",20,ysize - 40,20 + w,ysize - 10,0,1,7,true); 
   addkey(7,ct_enter ); 

   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,8,true);
   addkey(8, ct_esc ); 

   buildgraphics(); 

   mousevisible(true); 
} 


void         SelectUnitSet::run(void)
{ 
   do { 
      tdialogbox::run();
   }  while ( action == 0 ); 
} 


void         SelectUnitSet::buttonpressed(int         id)
{
   switch(id) {
       case 7: { 
                  action = 1; 
                  for ( int i = 0; i < unitSets.size(); i++ )
                      unitSets[i]->active = active[i];
                  resetvehicleselector();
                  resetbuildingselector();
          }
          break;
       case 8: action = 2; 
          break;
  } 
} 

void selectunitsetfilter ( void )
{
   if ( unitSets.size() > 0 ) {
      SelectUnitSet sus;
      sus.init();
      sus.run();
      sus.done();
   } else
      displaymessage ( " no unitsets defined !", 1 );
}


class UnitTypeTransformation {

              class   UnitSetSelection : public tstringselect {
                       public :
                             virtual void setup(void);
                             virtual void buttonpressed(int id);
                             virtual void run(void);
                             virtual void gettext(word nr);
                         };
              class   TranslationTableSelection : public tstringselect {
                              int unitsetnum;
                         public :
                               virtual void setup( void );
                               void setup2 ( int _unitset ) { unitsetnum = _unitset; };
                               virtual void buttonpressed(int id);
                               virtual void run(void);
                               virtual void gettext(word nr);
                           };

                int unitstransformed;
                int unitsnottransformed;

                pvehicletype transformvehicletype ( const Vehicletype* type, int unitsetnum, int translationnum );
                void transformvehicle ( pvehicle veh, int unitsetnum, int translationnum );
                dynamic_array<int> vehicleTypesNotTransformed;
                int vehicleTypesNotTransformedNum ;
             public:
                 void run ( void );
      } ;


void         UnitTypeTransformation :: UnitSetSelection::setup(void)
{ 
   action = 0;
   title = "Select UnitSet";
   numberoflines = unitSets.size();
   ey = ysize - 60; 
   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,2,true); 
   addkey(2,ct_enter); 
   addbutton("~C~ancel",190,ysize - 40,340,ysize - 20,0,1,3,true); 
   addkey(3,ct_esc);
} 

void         UnitTypeTransformation :: UnitSetSelection::buttonpressed(int         id)
{ 
   tstringselect::buttonpressed(id);
   switch (id) {
      case 2:   
      case 3:   action = id; 
                break; 
   } 
} 

void         UnitTypeTransformation :: UnitSetSelection::gettext(word nr)
{ 
   strcpy(txt,unitSets[nr]->name.c_str() );
} 

void         UnitTypeTransformation :: UnitSetSelection::run(void)
{ 
   do { 
      tstringselect::run(); 
   }  while ( ! ( action > 0 || (msel == 1)) );

   if ( action == 3) 
      redline = -1;
} 



void         UnitTypeTransformation :: TranslationTableSelection::setup( void )
{ 
   action = 0;
   title = "Select Transformation Table";
   numberoflines = unitSets[unitsetnum]->transtab.size();
   ey = ysize - 60; 
   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,2,true); 
   addkey(2,ct_enter); 
   addbutton("~C~ancel",190,ysize - 40,340,ysize - 20,0,1,3,true); 
   addkey(3,ct_esc);
} 

void         UnitTypeTransformation :: TranslationTableSelection::buttonpressed(int         id)
{ 
   tstringselect::buttonpressed(id);
   switch (id) {
      case 2:   
      case 3:   action = id; 
                break; 
   } 
} 

void         UnitTypeTransformation :: TranslationTableSelection::gettext(word nr)
{ 
   strcpy(txt, unitSets[unitsetnum]->transtab[nr]->name.c_str() );
} 

void         UnitTypeTransformation :: TranslationTableSelection::run(void)
{ 
   do { 
      tstringselect::run(); 
   }  while ( ! ( action > 0 || (msel == 1)) );

   if ( action == 3) 
      redline = -1;
} 

pvehicletype UnitTypeTransformation :: transformvehicletype ( const Vehicletype* type, int unitsetnum, int translationnum )
{
   for ( int i = 0; i < unitSets[unitsetnum]->transtab[translationnum]->translation.size(); i++ )
      if ( unitSets[unitsetnum]->transtab[translationnum]->translation[i].from == type->id ) {
         pvehicletype tp = getvehicletype_forid ( unitSets[unitsetnum]->transtab[translationnum]->translation[i].to );
         if ( tp ) 
            return tp;
      }

   int fnd = 0;
   for ( int j = 0; j < vehicleTypesNotTransformedNum; j++ )
       if ( vehicleTypesNotTransformed[j] == type->id )
          fnd ++;

   if ( !fnd ) 
      vehicleTypesNotTransformed[vehicleTypesNotTransformedNum++] = type->id;

   return NULL;
}

void  UnitTypeTransformation ::transformvehicle ( pvehicle veh, int unitsetnum, int translationnum )
{
   for ( int i = 0; i < 32; i++ )
      if ( veh->loading[i] )
         transformvehicle ( veh->loading[i], unitsetnum, translationnum );

   pvehicletype nvt = transformvehicletype ( veh->typ, unitsetnum, translationnum );
   if ( !nvt ) {
      unitsnottransformed++;
      return;
   }
   veh->transform ( nvt );

   unitstransformed++;
}


void UnitTypeTransformation :: run ( void )
{
   vehicleTypesNotTransformedNum = 0;

   UnitSetSelection uss;
   uss.init();
   uss.run();
   uss.done();

   int unitsetnum = uss.redline;
   if ( unitsetnum == -1 )
      return;

   TranslationTableSelection tss;
   tss.setup2( unitsetnum );
   tss.init();
   tss.run();
   tss.done();

   int translationsetnum = tss.redline;
   if ( translationsetnum == -1 || translationsetnum == 255 )
      return;

   unitstransformed = 0;
   unitsnottransformed = 0;


   for ( int y = 0; y < actmap->ysize; y++ )
      for ( int x = 0; x < actmap->xsize; x++ ) {
         pfield fld = getfield ( x, y );
         if ( fld->vehicle )
            transformvehicle ( fld->vehicle, unitsetnum, translationsetnum );
         if ( fld->building && (fld->bdt & getTerrainBitType(cbbuildingentry) ).any() )
            for ( int i = 0; i < 32; i++ ) {
               if ( fld->building->loading[i] ) 
                  transformvehicle ( fld->building->loading[i], unitsetnum, translationsetnum );
               if ( fld->building->production[i] ) {
                  pvehicletype vt = transformvehicletype ( fld->building->production[i], unitsetnum, translationsetnum );
                  if ( vt ) {
                     fld->building->production[i] = vt;
                     unitstransformed++;
                  } else
                     unitsnottransformed++;
               }
            }
      }

    if ( vehicleTypesNotTransformedNum ) {
       string s = "The following vehicles could not be transformed: ";
       for ( int i = 0; i < vehicleTypesNotTransformedNum; i++ ) {
          s += "\n ID ";
          s += strrr ( vehicleTypesNotTransformed[i] );
          s += " : ";
          pvehicletype vt = getvehicletype_forid ( vehicleTypesNotTransformed[i] );
          if ( !vt-> name.empty() )
             s += vt->name;
          else
             s += vt->description;
       }
       
       s += "\n";
       s += strrr ( unitstransformed );
       s += " units were transformed\n";
       s += strrr ( unitsnottransformed );
       s += " units were NOT transformed\n (production included)";

       tviewanytext vat;
       vat.init ( "warning", s.c_str() );
       vat.run();
       vat.done();
    } else 
       displaymessage ( "All units were transformed !\ntotal number: %d", 3, unitstransformed );


}

void unitsettransformation( void )
{
   UnitTypeTransformation utt;
   utt.run();
}


void MapSwitcher :: toggle ( )
{
   maps[active].map = actmap;
   maps[active].changed = !mapsaved;
   maps[active].cursorx = getxpos();
   maps[active].cursory = getypos();
   active = !active;

   actmap = maps[active].map;
   mapsaved = !maps[active].changed;
   if  ( actmap )
      cursor.gotoxy( maps[active].cursorx, maps[active].cursory );
}

string MapSwitcher :: getName ()
{
  const char* name[2] = { "primary", "secondary" };
  string s;
  s = name[active];

  if ( actmap )
     if ( !actmap->preferredFileNames.mapname[0].empty() ) {
        s += " ( ";
        s += actmap->preferredFileNames.mapname[0];
        s += " ) ";
     }

  return s;
}

MapSwitcher::Action MapSwitcher :: getDefaultAction ( )
{
   if ( active )
      return select;
   else
      return set;
}


MapSwitcher mapSwitcher;


void showStatusBar(void)
{
   npush ( activefontsettings );

   char       s[200];
   s[0] = 0;
   if ( actmap )
      sprintf(s, "X:%d/%d Y:%d/%d", getxpos(), int(actmap->xsize), getypos(), int(actmap->ysize));

   int y = agmp->resolutiony - 45;

   activefontsettings.color = black;
   activefontsettings.length = 150;
   activefontsettings.background = lightgray;
   activefontsettings.height = 20;
   activefontsettings.font = schriften.smallarial;
   activefontsettings.justify = centertext;

   int x = 10;
   showtext2(s,x,y+20);
   x+=activefontsettings.length+2;

   activefontsettings.length = 200;
   showtext2(mapSwitcher.getName().c_str(),x,y+20);
   x+=activefontsettings.length+2;

/*
   if ( actmap ) {
      strcpy(s,"Title : ");
      showtext2(strcat(s,actmap->title),10,5);
      strcpy(s,"X-Size : ");
      showtext2(strcat(s,strrr()),10,y);
      strcpy(s,"Y-Size : ");
      showtext2(strcat(s,strrr(actmap->ysize)),100,y);
   }
*/

   string ss = "PolyFill: ";
   if (polyfieldmode)
      ss += "on";
   else
      ss += "off";

   activefontsettings.length = 100;
   showtext2(ss.c_str(),x,y+20);
   x+=activefontsettings.length+2;


   activefontsettings.length = 120;
   ss = "RectFill: ";
   if (tfill) {
      ss += "on (";
      ss += strrr ( fillx1 );
      ss += "/";
      ss += strrr ( filly1 );
      ss += ")";
   } else
      ss += "off";

   showtext2(ss.c_str(),x,y+20);
   x+=activefontsettings.length+2;

   npop ( activefontsettings );
}


void transformMap ( )
{

   ASCString filename;
   fileselectsvga ( "*.ascmapxlat", filename, true );

   if ( filename.empty() )
      return;


   vector<int> terraintranslation;
   vector<int> objecttranslation;
   vector<int> terrainobjtranslation;
   try {
      tnfilestream s ( filename, tnstream::reading );

      TextFormatParser tfp ( &s );
      TextPropertyGroup* tpg = tfp.run();

      PropertyReadingContainer pc ( "maptranslation", tpg );

      pc.addIntegerArray ( "TerrainTranslation", terraintranslation );
      pc.addIntegerArray ( "TerrainObjTranslation", terrainobjtranslation );
      pc.addIntegerArray ( "ObjectTranslation", objecttranslation );

      pc.run();

      delete tpg;
   }
   catch ( ParsingError err ) {
      displaymessage ( "Error parsing file : " + err.getMessage() , 1 );
      return;
   }
   catch ( ASCexception ) {
      displaymessage ( "Error reading file " + filename , 1 );
      return;
   }

   if ( terraintranslation.size() % 2 ) {
      displaymessage ( "Map Translation : terraintranslation - Invalid number of entries ", 1);
      return;
   }

   if ( terrainobjtranslation.size() % 3 ) {
      displaymessage ( "Map Translation : terrainobjtranslation - Invalid number of entries ", 1);
      return;
   }

   if ( objecttranslation.size() % 2 ) {
      displaymessage ( "Map Translation : objecttranslation - Invalid number of entries ", 1);
      return;
   }


   for ( int y = 0; y < actmap->ysize; y++ )
      for ( int x = 0; x < actmap->xsize; x++ ) {
          pfield fld = actmap->getField ( x, y );
          for ( int i = 0; i < terraintranslation.size()/2; i++ )
             if ( fld->typ->terraintype->id == terraintranslation[i*2] ) {
                TerrainType* tt = getterraintype_forid ( terraintranslation[i*2+1] );
                if ( tt ) {
                   fld->typ = tt->weather[fld->getweather()];
                   fld->setparams();
                }
             }

          for ( int i = 0; i < terrainobjtranslation.size()/3; i++ )
             if ( fld->typ->terraintype->id == terrainobjtranslation[i*3] ) {
                TerrainType* tt = getterraintype_forid ( terrainobjtranslation[i*3+1] );
                if ( tt ) {
                   fld->addobject ( getobjecttype_forid ( terrainobjtranslation[i*3+2] ));
                   fld->typ = tt->weather[fld->getweather()];
                   fld->setparams();
                }
             }


          for ( tfield::ObjectContainer::iterator o = fld->objects.begin(); o != fld->objects.end(); o++ )
             for ( int i = 0; i < objecttranslation.size(); i++ )
                if ( o->typ->id == objecttranslation[i*2] ) {
                   ObjectType* ot = getobjecttype_forid ( objecttranslation[i*2+1] );
                   if ( ot ) {
                      o->typ = ot;
                      fld->sortobjects();
                      fld->setparams();
                   }
                }
   }
   calculateallobjects();
   displaymap();
}


class EditArchivalInformation : public tdialogbox {
         tmap* gamemap;
         char maptitle[10000];
         char author[10000];
         ASCString description;
         char tags[10000];
         char requirements[10000];
         int action;
       public:
         EditArchivalInformation ( tmap* map );
         void init();
         void run();
         void buttonpressed ( int id );
};


EditArchivalInformation :: EditArchivalInformation ( tmap* map ) : gamemap ( map )
{
  strcpy ( maptitle, map->maptitle.c_str() );
  strcpy ( author, map->archivalInformation.author.c_str() );
  description = map->archivalInformation.description;
  strcpy ( tags, map->archivalInformation.tags.c_str() );
  strcpy ( requirements, map->archivalInformation.requirements.c_str() );
}

void EditArchivalInformation::init()
{
   tdialogbox::init();
   title = "Edit Archival Information";
   xsize = 600;
   ysize = 410;
   action = 0;


   addbutton("~T~itle",20,70,xsize - 20,90,1,1,10,true);
   addeingabe(10,maptitle,0,10000);

   addbutton("~A~uthor",20,120,xsize - 20,140,1,1,11,true);
   addeingabe(11,author,0,10000);

   addbutton("Ta~g~s",20,170,xsize - 20,190,1,1,12,true);
   addeingabe(12,tags,0,10000);

   addbutton("~R~equirements ( see http://asc-hq.sf.net/req.php for possible values)",20,220,xsize - 20,240,1,1,13,true);
   addeingabe(13,requirements,0,10000);


   addbutton("Edit ~D~escription",20,ysize - 70,170,ysize - 50,0,1,14,true);

   addbutton("~O~k",20,ysize - 40,xsize/2-10,ysize - 20,0,1,1,true);
   addkey(1,ct_enter);
   addbutton("~C~ancel",xsize/2+10,ysize - 40,xsize-20,ysize - 20,0,1,2,true);
   addkey(2,ct_esc);

   buildgraphics();
}

void EditArchivalInformation :: buttonpressed ( int id )
{
   switch ( id ) {
      case 14: {
                  MultilineEdit mle ( description, "Map Description" );
                  mle.init();
                  mle.run();
                  mle.done();
                  break;
                }
      case 1: gamemap->archivalInformation.author = author;
              gamemap->archivalInformation.tags   = tags;
              gamemap->archivalInformation.requirements   = requirements;
              gamemap->archivalInformation.description   = description;
              gamemap->maptitle = maptitle;
              mapsaved = false;
      case 2: action = id;
   }
}

void EditArchivalInformation :: run ()
{
   do {
      tdialogbox::run();
   } while ( action == 0);
}


void editArchivalInformation()
{
  EditArchivalInformation eai ( actmap );
  eai.init();
  eai.run();
  eai.done();
}

void resourceComparison ( )
{
   ASCString s;
   for ( int i = 0; i < 9; i++ ) {
      s += ASCString("player ") + strrr ( i ) + " : ";
      Resources plus;
      Resources have;
      for ( Player::BuildingList::iterator b = actmap->player[i].buildingList.begin(); b != actmap->player[i].buildingList.end(); ++b ) {
         Building& bld = **b;
         if ( actmap->_resourcemode == 0 )
            plus += (*b)->plus;
         else
            plus += (*b)->bi_resourceplus;

         have += (*b)->actstorage;
      }
      for ( int r = 0; r < resourceTypeNum; r++ ) {
         s += ASCString(resourceNames[r]) + " " + strrr ( have.resource(r)) + " +";
         s += strrr(plus.resource(r));
         s += "; ";
      }

      s += "\n\n";
   }
   displaymessage ( s, 3 );
}
