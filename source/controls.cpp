/*! \file controls.cpp
   Controlling units (which is graudally moved to #vehicletype.cpp and #unitctrl.cpp );
   Things that are run when starting and ending someones turn   
*/

//     $Id: controls.cpp,v 1.150 2003-02-19 19:47:25 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.149  2003/01/12 19:37:18  mbickel
//      Rewrote resource production
//
//     Revision 1.148  2003/01/06 16:52:03  mbickel
//      Fixed: units inside transports got wrong movement when moved out
//      Fixed: wind not displayed correctly
//      Fixed: tribute displaying wrong values
//      Fixed: constructing units with untis consumed energy when pipeline near
//      The movement cost for building objects is no longer terrain dependant
//
//     Revision 1.147  2002/12/15 23:54:46  mbickel
//      New: system sends mail if units die
//      Fixed: orbiting and submerged units could crash on water due to wind
//      Updated documentation
//
//     Revision 1.146  2002/12/12 20:36:05  mbickel
//      Updated documentation
//      Fixed: hotkey for gui icons not allways working
//      Fixed: objects in fog of war were always displayed for normal weather
//
//     Revision 1.145  2002/12/12 11:34:17  mbickel
//      Fixed: ai crashing when weapon has no ammo
//      Fixed: ASC crashed when loading game with ID not found
//      Fixed: more ai problems
//      AI now faster
//      Fixed: removing objects overfill a units tank
//
//     Revision 1.144  2002/12/08 21:53:39  mbickel
//      Fixed: mining station produces twice the resources
//      Updated file version numbers
//
//     Revision 1.143  2002/11/27 21:25:51  mbickel
//      AI fixes
//
//     Revision 1.142  2002/11/20 20:00:53  mbickel
//      New features: specify passwords when starting a game
//      Better error messages when loading a game through command line parameters
//      Fixed .ASCTXT problems with alias and inheritance
//
//     Revision 1.141  2002/11/18 10:47:36  mbickel
//      Fixed: units with reaction fire could not attack manually
//      Fixed: crash at end of turn when airplane has maxmovement 0
//
//     Revision 1.140  2002/11/12 13:55:11  mbickel
//      Fixed: invalid icons displayed when building construction failed
//
//     Revision 1.139  2002/11/09 19:10:49  mbickel
//      Fixed: invalid data return after EOF if file very small
//      Resourceproduction of matter converter now working if not all resources could be stored
//      mk1 V3 required
//
//     Revision 1.138  2002/11/07 18:42:57  mbickel
//      Fixed crash in reaction fire
//      Improved autoborder object netification
//      Fixed crash when mail game was loaded by command line parameters
//
//     Revision 1.137  2002/11/05 09:05:16  mbickel
//      Fixed: mining stations produced not enough output
//      Changed default building properties in mapeditor
//      new autoborder object netting function
//
//     Revision 1.136  2002/11/01 20:44:52  mbickel
//      Added function to specify which units can be build by other units
//
//     Revision 1.135  2002/10/12 17:28:03  mbickel
//      Fixed "enemy unit loaded" bug.
//      Changed map format
//      Improved log messages
//
//     Revision 1.134  2002/10/09 16:58:45  mbickel
//      Fixed to GrafikSet loading
//      New item filter for mapeditor
//
//     Revision 1.133  2002/10/01 09:23:41  mbickel
//      Fixed many bugs
//      Added inheritance to text files
//      updated graphics
//
//     Revision 1.132  2002/09/19 20:20:04  mbickel
//      Cleanup and various bug fixes
//
//     Revision 1.131  2002/04/21 21:27:00  mbickel
//      Mapeditor: Fixed crash in "Put Resources"
//      Updating the small map after AI
//      Fixed infinite loop "quit game" after sending signal
//
//     Revision 1.130  2002/04/05 19:01:45  mbickel
//      Fixed: reaction fire attackes being replayed twice
//      Fixed: crash when reaction fire during landing on carrier
//
//     Revision 1.129  2002/04/05 09:25:05  mbickel
//      Project files now for Borland C++ Builder 6
//      Fixed: netcontrol not working
//      Fixed: replay errors when constructing turrets
//      Submarine require no fuel for sufacing
//      Field info dialog extended
//      Fixed several buffer overruns
//
//     Revision 1.128  2002/03/18 21:42:17  mbickel
//      Some cleanup and documentation in the Mine class
//      The number of mines is now displayed in the field information window
//
//     Revision 1.127  2002/03/14 18:14:37  mbickel
//      Improved messages for proposing peace
//      Fixed display error when enterering passwords
//
//     Revision 1.126  2002/03/03 22:19:32  mbickel
//      Updated documentation
//      Improved user interface
//      Fixed AI bug
//
//     Revision 1.125  2002/01/07 11:40:40  mbickel
//      Fixed some syntax errors
//
//     Revision 1.124  2001/12/19 17:16:28  mbickel
//      Some include file cleanups
//
//     Revision 1.123  2001/12/14 10:20:04  mbickel
//      Cleanup and enhancements to configure.in
//      Removed last remains of octagonal version from source files
//
//     Revision 1.122  2001/11/12 18:28:33  mbickel
//      Fixed graphical glitches when unit moves near border
//      If max num of mines exceeded, no icon is displayed for placing a new one
//      Fixed: some airplanes could not move after ascend
//      Fixed: couldn't build satellites on fields no accessible if satellite was
//        on ground level.
//
//     Revision 1.121  2001/11/08 17:32:14  mbickel
//      Fixed AI crash
//      Added Replay to AI
//      New replay commands
//
//     Revision 1.120  2001/11/04 22:52:35  mbickel
//      Fixed bug in wood net calculation
//      Fixed broken refuel dialog
//
//     Revision 1.119  2001/10/28 20:42:17  mbickel
//      Fixed AI crashes
//      Fixed problems when next campaign map could not be found.
//      Fixed forgotten events.
//      Added several new experimental map parameters
//      Some improvements to the campaign maps
//
//     Revision 1.118  2001/10/22 18:22:47  mbickel
//      Reaction fire doesn't prevent units with the wait attribute from attacking
//       manually
//      Fixed behaviour of generator vehicles on BI-mode maps
//
//     Revision 1.117  2001/10/11 10:41:05  mbickel
//      Restructured platform fileio handling
//      Added map archival information to mapeditor
//
//     Revision 1.116  2001/10/08 14:44:22  mbickel
//      Some cleanup
//
//     Revision 1.115  2001/10/02 14:06:27  mbickel
//      Some cleanup and documentation
//      Bi3 import tables now stored in .asctxt files
//      Added ability to choose amoung different BI3 import tables
//      Added map transformation tables
//
//     Revision 1.114  2001/09/26 19:44:09  mbickel
//      Fixed bugs in map resizing
//
//     Revision 1.113  2001/09/25 18:03:34  mbickel
//      Move after attack doesn't reduce a units movement
//
//     Revision 1.112  2001/09/13 17:43:11  mbickel
//      Many, many bug fixes
//
//     Revision 1.111  2001/08/09 19:28:22  mbickel
//      Started adding buildingtype text file functions
//
//     Revision 1.110  2001/08/07 15:58:09  mbickel
//      Fixed crash in mail list
//      Fixed crash in weapon info with mines
//      Fixed cancel Button in object construction
//
//     Revision 1.109  2001/08/06 21:38:00  mbickel
//      Fixed: ghost icons remained after vehicle construction canceled
//
//     Revision 1.108  2001/07/28 21:09:08  mbickel
//      Prepared vehicletype structure for textIO
//
//     Revision 1.107  2001/07/28 11:19:10  mbickel
//      Updated weaponguide
//      moved item repository from spfst to itemrepository
//
//     Revision 1.106  2001/07/27 21:13:34  mbickel
//      Added text based file formats
//      Terraintype and Objecttype restructured
//
//     Revision 1.105  2001/07/18 16:05:47  mbickel
//      Fixed: infinitive loop in displaying "player exterminated" msg
//      Fixed: construction of units by units: wrong player
//      Fixed: loading bug of maps with mines
//      Fixed: invalid map parameter
//      Fixed bug in game param edit dialog
//      Fixed: cannot attack after declaring of war
//      New: ffading of sounds
//
//     Revision 1.104  2001/07/15 21:00:25  mbickel
//      Some cleanup in the vehicletype class
//
//     Revision 1.103  2001/07/02 10:14:41  mbickel
//      Fixed crash when starting new email games
//      New version: 1.3.19
//
//     Revision 1.102  2001/05/21 12:46:18  mbickel
//      Fixed infinite loop in AI::strategy
//      Fixed bugs in mapeditor - event editing
//      Fixed bugs in even loading / writing
//      Fixed wrong build order AI <-> main program
//
//     Revision 1.101  2001/03/30 12:43:15  mbickel
//      Added 3D pathfinding
//      some cleanup and documentation
//      splitted the ai into several files, now located in the ai subdirectory
//      AI cares about airplane servicing and range constraints
//
//     Revision 1.100  2001/03/23 16:02:55  mbickel
//      Some restructuring;
//      started rewriting event system
//
//     Revision 1.99  2001/02/26 12:35:01  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.98  2001/02/18 17:52:35  mbickel
//      Fixed some compilation problems on Linux
//
//     Revision 1.97  2001/02/18 15:37:02  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.96  2001/02/11 11:39:28  mbickel
//      Some cleanup and documentation
//
//     Revision 1.95  2001/02/01 22:48:31  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.94  2001/01/28 17:18:58  mbickel
//      The recent cleanup broke some source files; this is fixed now
//
//     Revision 1.93  2001/01/28 14:04:07  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.92  2001/01/25 23:44:52  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.91  2001/01/23 21:05:12  mbickel
//      Speed up of AI
//      Lot of bugfixes in AI
//      Moved Research to own files (research.*)
//      Rewrote storing of developed technologies
//      Some cleanup and documentation
//
//     Revision 1.90  2001/01/21 16:37:14  mbickel
//      Moved replay code to own file ( replay.cpp )
//      Fixed compile problems done by cleanup
//
//     Revision 1.89  2001/01/19 13:33:47  mbickel
//      The AI now uses hemming
//      Several bugfixes in Vehicle Actions
//      Moved all view calculation to viewcalculation.cpp
//      Mapeditor: improved keyboard support for item selection
//
//     Revision 1.88  2000/12/23 13:19:43  mbickel
//      Made ASC compileable with Borland C++ Builder
//
//     Revision 1.87  2000/12/21 11:00:47  mbickel
//      Added some code documentation
//
//     Revision 1.86  2000/11/29 09:40:13  mbickel
//      The mapeditor has now two maps simultaneously active
//      Moved memorychecking functions to its own file: memorycheck.cpp
//      Rewrote password handling in ASC
//
//     Revision 1.85  2000/11/21 20:26:56  mbickel
//      Fixed crash in tsearchfields (used by object construction for example)
//      AI improvements
//      configure.in: added some debug output
//                    fixed broken check for libbz2
//
//     Revision 1.84  2000/11/11 11:05:15  mbickel
//      started AI service functions
//
//     Revision 1.83  2000/11/08 19:30:56  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs

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
#include <cstring>
#include <math.h>
#include <stdarg.h>

#include "buildingtype.h"
#include "vehicletype.h"
#include "typen.h"

#include "basegfx.h"
#include "newfont.h"
#include "spfst.h"
#include "loaders.h"
#include "misc.h"
#include "controls.h"
#include "network.h"
#include "events.h"
#include "loadpcx.h"
#include "stack.h"
#include "dlg_box.h"
#include "dialog.h"
#include "attack.h"
#include "building.h"
#include "gamedlg.h"
#include "missions.h"
#include "sg.h"
#include "weather.h"
#include "gameoptions.h"
#include "ai/ai.h"
#include "errors.h"
#include "password_dialog.h"
#include "viewcalculation.h"
#include "replay.h"
#include "dashboard.h"
#include "resourcenet.h"
#include "itemrepository.h"
#include "strtmesg.h"

tmoveparams moveparams;


class InitControls {
  public:
   InitControls ( void ) 
   {
      memset ( (void*) &moveparams, 0, sizeof ( moveparams ));
   };
} initControls;



  class   tsearchexternaltransferfields : public SearchFields {
                      public:
                                pbuilding            bld;
                                char                    numberoffields;
                                void                    searchtransferfields( pbuilding building );
                                virtual void            testfield ( const MapCoordinate& mc );
                                tsearchexternaltransferfields ( pmap _gamemap ) : SearchFields ( _gamemap ) {};
                             };

void         tsearchexternaltransferfields :: testfield( const MapCoordinate& mc )
{ 
     pfield fld  = gamemap->getField ( mc );
     if ( fld && fld->vehicle )
        if ( fld->vehicle->height & bld->typ->externalloadheight ) {
           numberoffields++;
           fld->a.temp = 123;
        }
}


void tsearchexternaltransferfields :: searchtransferfields( pbuilding building )
{
   actmap->cleartemps( 7 );
   bld = building;
   numberoffields = 0;
   if ( bld->typ->special & cgexternalloadingb ) {
      initsearch( bld->getEntry(), 1, 1 );
      startsearch();
   }
   if ( numberoffields )
      moveparams.movestatus = 130;
}

int searchexternaltransferfields ( pbuilding bld )
{
   tsearchexternaltransferfields setf ( actmap );
   setf.searchtransferfields ( bld );
   return setf.numberoffields;
}

          
// #define netdebug 

  class   tsearchputbuildingfields : public SearchFields {
                      public:
                                pbuildingtype        bld;
                                char                    numberoffields;
                                pfield                   startfield;
                                void                    initputbuilding( word x, word y, pbuildingtype building );
                                virtual void            testfield ( const MapCoordinate& mc );
                                tsearchputbuildingfields ( pmap _gamemap ) : SearchFields ( _gamemap ) {};
                             };

  class   tsearchdestructbuildingfields : public SearchFields {
                      public:
                                char                    numberoffields;
                                pfield                  startfield;
                                void                    initdestructbuilding( int x, int y );
                                virtual void            testfield ( const MapCoordinate& mc );
                                tsearchdestructbuildingfields ( pmap _gamemap ) : SearchFields ( _gamemap ) {};
                             };




void         tsearchputbuildingfields::initputbuilding( word x, word y, pbuildingtype building )
{ 
   pvehicle eht = getfield(x,y)->vehicle; 

   if ( eht->attacked || (eht->typ->wait && eht->hasMoved() )) {
      dispmessage2(302,""); 
      return;
   } 

   actmap->cleartemps(7); 
   initsearch( MapCoordinate(x,y),1,1);
   bld = building;
   numberoffields = 0;
   startfield = getfield(x,y);
   startsearch();
   if (numberoffields > 0) {
      moveparams.movestatus = 111;
      moveparams.vehicletomove = eht;
      moveparams.buildingtobuild = bld;
   }
   else {
      dispmessage2(301,"");
   }
}


void         tsearchputbuildingfields::testfield(const MapCoordinate& mc)
{
      startfield = gamemap->getField(mc);
      bool b = true;
      for ( int y1 = 0; y1 <= 5; y1++)
         for ( int x1 = 0; x1 <= 3; x1++)
            if (bld->getpicture ( BuildingType::LocalCoordinate(x1, y1)) ) {
               pfield fld = actmap->getField ( bld->getFieldCoordinate( mc, BuildingType::LocalCoordinate(x1,y1) ));
               if (fld) {
                  if (fld->vehicle != NULL)
                     b = false;
                  if ( bld->buildingheight <= chfahrend )
                     if ( bld->terrainaccess.accessible ( fld->bdt ) <= 0 )
                        b = false;
                  if (fld->building != NULL) {
                     if (fld->building->typ != bld)
                        b = false;
                     if (fld->building->getCompletion() == fld->building->typ->construction_steps - 1)
                        b = false;
                     if ( (startfield->bdt & getTerrainBitType(cbbuildingentry) ).none() )
                        b = false;
                  }
                  if (startfield->building != fld->building)
                     b = false;
               } else
                  b = false;
            }
      if (b) {
         numberoffields++;
         gamemap->getField(mc)->a.temp = 20;
      }
}







void         putbuildinglevel1(void)
{

   tkey                       taste;
   pvehicle                   eht;

   eht = getactfield()->vehicle;

   int num = 0;
   for ( int i = 0; i < buildingtypenum; i++)
      if ( eht->buildingconstructable ( getbuildingtype_forpos( i ) ))
         num++;

   if ( num == 0 ) {
      dispmessage2( 303, NULL );
      return;
   }

   mousevisible(false);

   actgui->restorebackground();
   npush ( actgui );
   selectbuildinggui.reset ( eht );
   selectbuildinggui.painticons();
   actgui = &selectbuildinggui;

   mousevisible(true);
   do {
      if ( keypress() ) {
         int keyprn;
         tkey input;
         getkeysyms( &input, &keyprn );
         selectbuildinggui.checkforkey(taste, keyprn );
      }

     selectbuildinggui.checkformouse();
     releasetimeslice();

   } while ( (selectbuildinggui.selectedbuilding == NULL) && (selectbuildinggui.cancel == 0));

   selectbuildinggui.restorebackground();
   npop ( actgui );
   mousevisible(false);

   actgui->painticons();
   
   if ( selectbuildinggui.selectedbuilding ) {
      tsearchputbuildingfields   spbf ( actmap );
      spbf.initputbuilding( getxpos(), getypos(), selectbuildinggui.selectedbuilding );
   }

   actgui->painticons();

   mousevisible(true);
}




void         putbuildinglevel2( const pbuildingtype bld,
                               integer      xp,
                               integer      yp)
{
  bool b = true;

   if (getfield(xp,yp)->a.temp == 20)
      if (moveparams.movestatus == 111) {

         actmap->cleartemps(7);
         for ( int y1 = 0; y1 <= 5; y1++)
            for ( int x1 = 0; x1 <= 3; x1++)
               if ( bld->getpicture ( BuildingType::LocalCoordinate(x1, y1) ) ) {
                  MapCoordinate mc = bld->getFieldCoordinate(MapCoordinate(xp,yp),  BuildingType::LocalCoordinate(x1,y1));
                  pfield fld = actmap->getField( mc );
                  if ( fld ) {
                      if ( fld->vehicle || (fld->building && fld->building->getCompletion() == fld->building->typ->construction_steps-1 ))
                         b = false;

                        /* if fld^.typ^.art and bld^.terrain = 0 then
                      b:=false; */
                      if ( b )
                           if ( x1 == bld->entry.x  && y1 == bld->entry.y ) {
                              moveparams.movesx = mc.x;
                              moveparams.movesy = mc.y;
                              fld->a.temp = 23;
                           } else
                              fld->a.temp = 22;
                      else
                         displaymessage("severe error: \n inconsistency between level1 and level2 of putbuilding",2);
                  } else
                    b = false;
               }
         if ( b ) {
            moveparams.movestatus = 112;
         }
      }
}


void         putbuildinglevel3(integer      x,
                               integer      y)
{
  pbuildingtype bld;
  pvehicle     eht;

   if (getfield(x,y)->a.temp == 23)
      if (moveparams.movestatus == 112) {
         actmap->cleartemps(7);
         bld = moveparams.buildingtobuild;
         eht = moveparams.vehicletomove;
         putbuilding2( MapCoordinate(x,y), eht->color,bld);

         logtoreplayinfo ( rpl_putbuilding, (int) x, (int) y, (int) bld->id, (int) eht->color );

         int mf = actmap->getgameparameter ( cgp_building_material_factor );
         int ff = actmap->getgameparameter ( cgp_building_fuel_factor );
         if ( mf <= 0 )
            mf = 100;

         if ( ff <= 0 )
            ff = 100;

         if (eht->tank.material < bld->productionCost.material * mf / 100 ) {
            displaymessage("not enough material!",1);
            eht->tank.material = 0;
         }
         else
            eht->tank.material -= bld->productionCost.material * mf / 100;


         if (eht->tank.fuel < bld->productionCost.fuel * ff / 100) {
            displaymessage("not enough fuel!",1);
            eht->tank.fuel = 0;
         }
         else
            eht->tank.fuel -= bld->productionCost.fuel * ff / 100;

         moveparams.movestatus = 0;
         eht->setMovement ( 0 );
         eht->attacked = true;
         computeview( actmap );
      }
}








void         tsearchdestructbuildingfields::initdestructbuilding( int x, int y )
{
   pvehicle     eht = getfield(x,y)->vehicle;
   if (eht->attacked || (eht->typ->wait && eht->hasMoved() )) {
      dispmessage2(305,NULL);
      return;
   }
   actmap->cleartemps(7);
   initsearch( MapCoordinate(x,y), 1, 1 );
   numberoffields = 0;
   startfield = getfield(x,y);
   startsearch();
   if (numberoffields > 0) {
      moveparams.movestatus = 115;
      moveparams.vehicletomove = eht;
   }
   else {
      dispmessage2(306,"");
   }
}


void         tsearchdestructbuildingfields::testfield(const MapCoordinate& mc)
{
   startfield = gamemap->getField(mc);
   if (startfield->building) {
      numberoffields++;
      startfield->a.temp = 20;
   }
}





void         destructbuildinglevel1(int xp, int yp)
{
   tsearchdestructbuildingfields   sdbf ( actmap );
   sdbf.initdestructbuilding( xp, yp  );
}


void         destructbuildinglevel2( int xp, int yp)
{
   pfield fld = getfield(xp,yp);
   if (fld->a.temp == 20)
      if (moveparams.movestatus == 115) {
         actmap->cleartemps(7);
         pvehicle eht = moveparams.vehicletomove;
         pbuildingtype bld = fld->building->typ;



         pbuilding bb = fld->building;

         eht->tank.material += bld->productionCost.material * (100 - bb->damage) / destruct_building_material_get / 100;
         if ( eht->tank.material > eht->typ->tank.material )
            eht->tank.material = eht->typ->tank.material;

         eht->setMovement ( 0 );
         eht->attacked = 1;
         eht->tank.fuel -= destruct_building_fuel_usage * eht->typ->fuelConsumption;
         if ( bb->getCompletion() ) {
            bb->setCompletion ( bb->getCompletion()-1 );
         } else {
            delete bb;
         }
         logtoreplayinfo ( rpl_removebuilding, xp, yp );
         computeview( actmap );
         displaymap();
         moveparams.movestatus = 0;
      }
}










void         tputmine::testfield(const MapCoordinate& mc)
{
   pfield fld = gamemap->getField(mc);
   if ( !fld->vehicle  &&  !fld->building ) {
      fld->a.temp = 0;
      if ( !fld->mines.empty() ) {
         fld->a.temp += 2;
         numberoffields++;
      }
      if (mienenlegen && (fld->mines.empty() || fld->mineowner() == player) && fld->mines.size() < gamemap->getgameparameter ( cgp_maxminesonfield )) {
         fld->a.temp += 1;
         numberoffields++;
      }
   }
}




void         tputmine::initpm(  char mt, const pvehicle eht )
{
  int         i;

   numberoffields = 0;
   mienenlegen = false;
   mienenraeumen = false;
   if (eht->typ->weapons.count > 0)
      for (i = 0; i <= eht->typ->weapons.count - 1; i++)
         if ((eht->typ->weapons.weapon[i].getScalarWeaponType() == cwminen) && eht->typ->weapons.weapon[i].shootable() ) {
            mienenraeumen = true;
            if (eht->ammo[i] > 0)
               mienenlegen = true;
         }
   player = eht->color / 8;
   mienentyp = mt;
   if (eht->getMovement() < mineputmovedecrease) {
      mienenlegen = false;
      mienenraeumen = false;
   }
   if (mienenlegen || mienenraeumen)
      initsearch( MapCoordinate( getxpos(),getypos()), 1, 1 );
}


void         tputmine::run(void)
{
   if ((mienenlegen || mienenraeumen)) {
      startsearch();
      if (numberoffields > 0) {
         moveparams.movestatus = 90;
         moveparams.movesx = getxpos();
         moveparams.movesy = getypos();

      }
   }
}






void  legemine( int typ, int delta )
{
   if (moveparams.movestatus == 0) {
      tputmine ptm ( actmap );
      pvehicle eht = getactfield()->vehicle;
      moveparams.vehicletomove = eht;
      if (eht == NULL)
         return;
      if (eht->color != (actmap->actplayer << 3))
         return;
      ptm.initpm(typ,eht);
      ptm.run();
   }
   else
      if (moveparams.movestatus == 90) {
         pvehicle eht = moveparams.vehicletomove;
         pfield fld = getactfield();
         if ( fld->a.temp ) {

            if ( (fld->a.temp & 1) && ( delta > 0 )) {
               const Vehicletype* fzt = eht->typ;
               int  strength = 64;
               for ( int i = 0; i < fzt->weapons.count ; i++)
                  if ((fzt->weapons.weapon[i].getScalarWeaponType() == cwminen) && fzt->weapons.weapon[i].shootable() )
                     if ( fld-> putmine( actmap->actplayer, typ, MineBasePunch[typ-1] * strength / 64 )) {
                        eht->ammo[i]--;
                        eht->setMovement ( eht->getMovement() - mineputmovedecrease );
                        strength = eht->weapstrength[i];
                        int x = getxpos();
                        int y = getypos();
                        logtoreplayinfo ( rpl_putmine, x, y, (int) actmap->actplayer, (int) typ, (int) MineBasePunch[typ-1] * strength / 64 );
                        break;
                     }


            }

            if ( (fld->a.temp & 2) && ( delta < 0 )) {
               int x = getxpos();
               int y = getypos();
               pfield fld = getactfield();
               fld -> removemine( -1 );
               logtoreplayinfo ( rpl_removemine, x, y );
            }
            actmap->cleartemps(7);
            computeview( actmap );
            moveparams.movestatus = 0;
            displaymap();
         }
      }
}







/*
void         refuelvehicle(int         b)
{
   pvehicle     actvehicle;

   if (moveparams.movestatus == 0) {

      trefuelvehicle rfe;
      rfe.initrefuelling(getxpos(),getypos(),b);
      rfe.startsearch();

   }
   else {
      if (moveparams.movestatus == 65) {
         if (getactfield()->a.temp > 0) {
            actvehicle = getfield(moveparams.movesx,moveparams.movesy)->vehicle;
            verlademunition(getactfield()->vehicle,actvehicle,NULL,3 - b);
            actmap->cleartemps(7);
            moveparams.movestatus = 0;
         }
      }
      else
         if (moveparams.movestatus == 66)
            if (getactfield()->a.temp > 0) {
               actvehicle = getfield(moveparams.movesx,moveparams.movesy)->vehicle;
               // actvehicle->repairunit( getactfield()->vehicle );
               actmap->cleartemps(7);
               moveparams.movestatus = 0;
            }
      dashboard.x = 0xffff;
   }

}
*/

typedef class tobjectcontainers_buildable_on_field *pobjectcontainers_buildable_on_field;
class tobjectcontainers_buildable_on_field {
       public:
          dynamic_array<pobjecttype> objects_buildable;
          dynamic_array<pobjecttype> objects_removable;
          int objects_buildable_num;
          int objects_removable_num;
          int x,y;
          tobjectcontainers_buildable_on_field ( int xp, int yp ) {
              objects_buildable_num = 0;
              objects_removable_num = 0;
              x = xp;
              y = yp;
          };
     };

typedef class tobjectcontainers_buildable *pobjectcontainers_buildable ;
class tobjectcontainers_buildable {
        int inited;
        BasicGuiHost* oldgui;
       public:
        dynamic_array<pobjectcontainers_buildable_on_field> field;
        int fieldnum;

        void init ( void )
        {
           if ( fieldnum && inited )
              reset();
           fieldnum = 0;
           inited = 1;
        };

        void start ( void )
        {
           init();
           oldgui = actgui;
           actgui = &selectobjectcontainergui;
        }


        tobjectcontainers_buildable ( void )
        {
           inited = 0;
           init ();
        };


        void reset ( void )
        {
           for ( int i = 0; i< fieldnum; i++ )
              delete field[i];

           fieldnum = 0;
           // actgui = oldgui;

        };

        ~tobjectcontainers_buildable (  )
        {
           reset ();
        };

};


tobjectcontainers_buildable objects_buildable;


int  object_constructable ( int x, int y, pobjecttype obj )
{
   for ( int i = 0; i < objects_buildable.fieldnum; i++ )
      if ( objects_buildable.field[i]->x == x   &&
           objects_buildable.field[i]->y == y ) {

         pobjectcontainers_buildable_on_field fld = objects_buildable.field[i];
         for ( int j = 0; j < fld->objects_buildable_num; j++ )
            if ( fld->objects_buildable[j] == obj )
               return 1;
      }

  return 0;
}

int  object_removeable ( int x, int y, pobjecttype obj )
{
   for ( int i = 0; i < objects_buildable.fieldnum; i++ )
      if ( objects_buildable.field[i]->x == x   &&
           objects_buildable.field[i]->y == y ) {

         pobjectcontainers_buildable_on_field fld = objects_buildable.field[i];
         for ( int j = 0; j < fld->objects_removable_num; j++ )
            if ( fld->objects_removable[j] == obj )
               return 1;
      }

  return 0;
}

void build_objects_reset( void )
{
   objects_buildable.reset();
   actmap->cleartemps(7);
   moveparams.movestatus = 0;
   displaymap();
   actgui->restorebackground();
   actgui = &gui;
   actgui->painticons();
}

   class tbuildstreet : public SearchFields {
                       enum Mode { Build, Remove };
                       pobjectcontainers_buildable_on_field obj;
                public:
                       pvehicle         actvehicle;
                       word             numberoffields;

                       virtual void     testfield ( const MapCoordinate& mc );
                       void             initbs ( void );
                       void             run ( void );
                       void             checkObject ( pfield fld, pobjecttype objtype, Mode mode );
                       tbuildstreet ( pmap _gamemap ) : SearchFields ( _gamemap ), obj ( NULL ) {};
                  };


void         tbuildstreet::initbs(void)
{
   initsearch ( MapCoordinate ( getxpos(), getypos()), 1, 1 );
   actvehicle = getactfield()->vehicle;

   moveparams.movesx = getxpos();
   moveparams.movesy = getypos();
   numberoffields = 0;
}


void    getobjbuildcosts ( pobjecttype obj, pfield fld, Resources* resource, int* movecost )
{
//   int mvcost;

   if ( !fld->checkforobject ( obj ) ) {
      *resource = obj->buildcost;
      *movecost = obj->build_movecost;
   } else {
      *resource = obj->removecost;
       *movecost = obj->remove_movecost;
   }
 //   *movecost =  ( 8 + ( fld->getmovemalus( 0 ) - 8 ) / ( objectbuildmovecost / 8 ) ) * mvcost  / 8;
}



void tbuildstreet::checkObject( pfield fld, pobjecttype objtype, Mode mode )
{
    if ( !objtype || !fld )
       return;

    if ( mode == Build ) {
       if ( objtype->terrainaccess.accessible( fld->bdt ) > 0 &&  !fld->checkforobject ( objtype ) ) {
          int movecost;
          Resources cost;
          getobjbuildcosts ( objtype, fld, &cost, &movecost );
          if ( actvehicle->tank >= cost && actvehicle->getMovement() >= movecost ) {
             obj->objects_buildable[ obj->objects_buildable_num++ ] = objtype;

             fld->a.temp = 1;
             numberoffields++;
          }
       }
    } else {
       if ( fld->checkforobject ( objtype ) ) {
          int movecost;
          Resources cost;
          getobjbuildcosts ( objtype, fld, &cost, &movecost );
          if ( actvehicle->tank >= cost && actvehicle->getMovement() >= movecost ) {
             obj->objects_removable[ obj->objects_removable_num++ ] = objtype;
             fld->a.temp = 1;
             numberoffields++;
          }
       }
    }
}

void         tbuildstreet::testfield( const MapCoordinate& mc )
{
   pfield fld = gamemap->getField(mc);

   obj = new tobjectcontainers_buildable_on_field ( mc.x, mc.y );
   objects_buildable.field[ objects_buildable.fieldnum++ ] = obj;

   if ( !fld->vehicle && !fld->building ) {
      for ( int i = 0; i < actvehicle->typ->objectsBuildable.size(); i++ )
        for ( int j = actvehicle->typ->objectsBuildable[i].from; j <= actvehicle->typ->objectsBuildable[i].to; j++ )
          checkObject( fld, getobjecttype_forid ( j ), Build );

      for ( int i = 0; i < actvehicle->typ->objectGroupsBuildable.size(); i++ )
        for ( int j = actvehicle->typ->objectGroupsBuildable[i].from; j <= actvehicle->typ->objectGroupsBuildable[i].to; j++ )
          for ( int k = 0; k < objecttypenum; k++ ) {
             pobjecttype objtype = getobjecttype_forpos ( k );
             if ( objtype->groupID == j )
                checkObject( fld, objtype, Build );
          }

      for ( int i = 0; i < actvehicle->typ->objectsRemovable.size(); i++ )
        for ( int j = actvehicle->typ->objectsRemovable[i].from; j <= actvehicle->typ->objectsRemovable[i].to; j++ )
          checkObject( fld, getobjecttype_forid ( j ), Remove );

      for ( int i = 0; i < actvehicle->typ->objectGroupsRemovable.size(); i++ )
        for ( int j = actvehicle->typ->objectGroupsRemovable[i].from; j <= actvehicle->typ->objectGroupsRemovable[i].to; j++ )
          for ( int k = 0; k < objecttypenum; k++ ) {
             pobjecttype objtype = getobjecttype_forpos ( k );
             if ( objtype->groupID == j )
                checkObject( fld, objtype, Remove );
          }
   }
}


void         tbuildstreet::run(void)
{
   startsearch();
   if (numberoffields > 0)
      moveparams.movestatus = 72;
}



class SearchVehicleConstructionFields : public SearchFields {
                       pvehicle         actvehicle;
                public:
                       word             numberoffields;

                       virtual void     testfield ( const MapCoordinate& mc );
                       void             initbs ( void );
                       void             run ( void );
                       SearchVehicleConstructionFields ( pmap _gamemap ) : SearchFields ( _gamemap ) {};
                  };


void         SearchVehicleConstructionFields::initbs(void)
{
   initsearch ( MapCoordinate( getxpos(), getypos()), 1, 1);
   actvehicle = getactfield()->vehicle;

   numberoffields = 0;
}


void         SearchVehicleConstructionFields::testfield(const MapCoordinate& mc)
{
   pfield fld = gamemap->getField(mc);
   if ( !fld->vehicle && !fld->building ) {
     for ( int i = 0; i < actvehicle->typ->vehiclesBuildable.size(); i++ )
       for ( int j = actvehicle->typ->vehiclesBuildable[i].from; j <= actvehicle->typ->vehiclesBuildable[i].to; j++ ) {
         if ( actmap->getgameparameter(cgp_forbid_unitunit_construction) == 0 || gamemap->unitProduction.check(j) ) {
            pvehicletype v = getvehicletype_forid ( j );
            if ( v && actvehicle->vehicleconstructable ( v, mc.x, mc.y )) {
               fld->a.temp = 1;
               numberoffields++;
            }
         }
      }
   }
}


void         SearchVehicleConstructionFields::run(void)
{
   startsearch();
   if (numberoffields > 0)
      moveparams.movestatus = 120;
}




void         setspec( pobjecttype obj )
{

   if (moveparams.movestatus == 0) {

      tbuildstreet buildstreet ( actmap );

      objects_buildable.init();

      buildstreet.initbs();
      buildstreet.run();

      if ( moveparams.movestatus == 72 ) {
           actgui->restorebackground();
           selectobjectcontainergui.reset ( );
           selectobjectcontainergui.painticons();
           actgui = &selectobjectcontainergui;

      }

   }
   else {
      if (moveparams.movestatus == 72) {
         pvehicle eht = getfield(moveparams.movesx,moveparams.movesy)->vehicle;
         pfield fld = getactfield();

         int movecost;
         Resources cost;
         getobjbuildcosts ( obj, fld, &cost, &movecost );

         int x = getxpos();
         int y = getypos();
         if ( !fld->checkforobject ( obj ) ) {
            getactfield() -> addobject ( obj );
            logtoreplayinfo ( rpl_buildobj, x, y, obj->id );
         } else {
            getactfield() -> removeobject ( obj );
            logtoreplayinfo ( rpl_remobj, x, y, obj->id );
         }

         eht->tank -= cost;
         for ( int i = 0; i < 3; i++ ) {
            if ( eht->tank.resource(i) > eht->typ->tank.resource(i) )
               eht->tank.resource(i) = eht->typ->tank.resource(i);
            if ( eht->tank.resource(i) < 0 )
               eht->tank.resource(i) = 0;
         }

         eht->setMovement ( eht->getMovement() - movecost );

         build_objects_reset();
      }
      dashboard.x = 0xffff;

   }
}


void build_vehicles_reset( void )
{
   objects_buildable.reset();
   actmap->cleartemps(7);
   moveparams.movestatus = 0;
   displaymap();
   actgui->restorebackground();
   actgui = &gui;
   actgui->restorebackground();
}



void         constructvehicle( pvehicletype tnk )
{

   if (moveparams.movestatus == 0) {

      moveparams.vehicletomove = getactfield()->vehicle;

      SearchVehicleConstructionFields svcf ( actmap );

      svcf.initbs();
      svcf.run();

      if ( moveparams.movestatus == 120 ) {
           actgui->restorebackground();
           selectvehiclecontainergui.reset ( moveparams.vehicletomove );
           selectvehiclecontainergui.painticons();
           actgui = &selectvehiclecontainergui;
      }
   }
   else {
      if (moveparams.movestatus == 120 ) {
         pvehicle eht = moveparams.vehicletomove;
         // pfield fld = getactfield();

         /*
         int stat = 0;
         if ( actmap->objectcrc ) {
            if ( actmap->objectcrc->speedcrccheck->checkunit2 ( tnk ))
               stat = 1;
         } else
            stat = 1;

         if ( stat ) {
         */

            int x = getxpos();
            int y = getypos();
            eht->constructvehicle ( tnk, x, y );
            logtoreplayinfo ( rpl_buildtnk, x, y, tnk->id, moveparams.vehicletomove->color/8 );
            computeview( actmap );

         // }

         build_vehicles_reset();
      }
      dashboard.x = 0xffff;

   }
}





int windbeeline ( int x1, int y1, int x2, int y2, WindMovement* wm ) {
   int dist = 0;
   while ( x1 != x2  || y1 != y2 ) {
      dist+= minmalq;
      int direc = getdirection ( x1, y1, x2, y2 );
      dist -= wm->getDist(direc);
      getnextfield ( x1, y1, direc );
   }
   return dist;
}













treactionfirereplay :: treactionfirereplay ( void )
{
   num = 0;
   unit = NULL;
}

void treactionfirereplay :: init ( pvehicle eht, const AStar3D::Path& fieldlist )
{
   if ( runreplay.status > 0 ) {
      preactionfire_replayinfo rpli;
      do {
         rpli = runreplay.getnextreplayinfo ( );
         if ( rpli )
            replay[num++] = rpli;

      } while ( rpli );

      unit = eht;
   }
}

int  treactionfirereplay :: checkfield ( int x, int y, pvehicle &eht, MapDisplayInterface* md )
{
   int attacks = 0;

   if ( eht == unit ) {
      for ( int i = 0; i < num; i++ ) {
         preactionfire_replayinfo rpli = replay[ i ];
         if ( eht && rpli->x2 == x  && rpli->y2 == y ) {

             pfield fld = getfield ( rpli->x1, rpli->y1 );
             pfield targ = getfield ( rpli->x2, rpli->y2 );

             npush ( targ->vehicle );
             targ->vehicle = eht;
             eht->xpos = x;
             eht->ypos = y;


             int attackvisible = fieldvisiblenow ( fld ) || fieldvisiblenow ( targ );


             if ( md && attackvisible ) {
               cursor.setcolor ( 8 );

               cursor.gotoxy ( rpli->x1, rpli->y1 );
               int t = ticker;
               while ( t + 15 > ticker )
                  releasetimeslice();

               cursor.gotoxy ( x, y );
               t = ticker;
               while ( t + 15 > ticker )
                  releasetimeslice();

               cursor.setcolor ( 0 );
               cursor.hide();
             }
             attacks++;

             tunitattacksunit battle ( fld->vehicle, targ->vehicle, 1, rpli->wpnum );
             battle.av.damage = rpli->ad1;
             battle.dv.damage = rpli->dd1;
             if ( md && attackvisible  )
                battle.calcdisplay ( rpli->ad2, rpli->dd2 );
             else {
                battle.calc ();
                battle.av.damage = rpli->ad2;
                battle.dv.damage = rpli->dd2;
             }

             int killed = 0;
             if ( battle.dv.damage >= 100 )
                killed = 1;

             battle.setresult ();
             dashboard.x = 0xffff;

             npop ( targ->vehicle );

             if ( killed )
                eht = NULL;
         }
      }
   }
   return attacks;
}

treactionfirereplay :: ~treactionfirereplay ( )
{
   for ( int i = 0; i < num; i++ )
      delete replay[i];
}



                typedef struct tunitlist* punitlist;
                struct tunitlist {
                        pvehicle eht;
                        punitlist next;
                     };

                punitlist unitlist[8];


int tsearchreactionfireingunits :: maxshootdist[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };

tsearchreactionfireingunits :: tsearchreactionfireingunits ( void )
{
   for ( int i = 0; i < 8; i++ )
      unitlist[i] = NULL;
}

void tsearchreactionfireingunits :: init ( pvehicle vehicle, const AStar3D::Path& fieldlist )
{
   int x1 = maxint;
   int y1 = maxint;
   int x2 = 0;
   int y2 = 0;
   int i, j, h;

   if ( maxshootdist[0] == -1 ) {
      for (i = 0; i < 8; i++ )
         maxshootdist[i] = 0;

      for (i = 0; i < vehicletypenum; i++ ) {
         pvehicletype fzt = getvehicletype_forpos ( i );
         if ( fzt )
            for (j = 0; j < fzt->weapons.count; j++ )
               if ( fzt->weapons.weapon[j].shootable() )
                  for (h = 0; h < 8; h++ )
                     if ( fzt->weapons.weapon[j].targ & ( 1 << h ) )
                        if ( fzt->weapons.weapon[j].maxdistance > maxshootdist[h] )
                           maxshootdist[h] = fzt->weapons.weapon[j].maxdistance;
      }
   }

   for ( AStar3D::Path::const_iterator i = fieldlist.begin(); i != fieldlist.end(); i++) {
      if ( i->x > x2 )
         x2 = i->x ;
      if ( i->y > y2 )
         y2 = i->y ;

      if ( i->x < x1 )
         x1 = i->x ;
      if ( i->y < y1 )
         y1 = i->y ;
   }
   int height = log2 ( vehicle->height );

   x1 -= maxshootdist[height];
   y1 -= maxshootdist[height];
   x2 += maxshootdist[height];
   y2 += maxshootdist[height];

   if ( x1 < 0 )
      x1 = 0;
   if ( y1 < 0 )
      y1 = 0;
   if ( x2 >= actmap->xsize )
      x2 = actmap->xsize -1;
   if ( y2 >= actmap->ysize )
      y2 = actmap->ysize -1;

   for ( int y = y1; y <= y2; y++ )
      for ( int x = x1; x <= x2; x++ ) {
         pvehicle eht = getfield ( x, y )->vehicle;
         if ( eht )
            if ( eht->color != vehicle->color )
               if ( eht->reactionfire.getStatus() >= Vehicle::ReactionFire::ready )
                  if ( eht->reactionfire.enemiesAttackable & ( 1 << ( vehicle->color / 8 )))
                     if ( getdiplomaticstatus ( eht->color ) == cawar )
                        if ( attackpossible2u ( eht, vehicle ) )
                           addunit ( eht );

      }
   if ( getfield(vehicle->xpos, vehicle->ypos)->vehicle == vehicle )
      for ( i = 0; i < 8; i++ )
         if ( fieldvisiblenow ( getfield ( vehicle->xpos, vehicle->ypos ), i )) {
            punitlist ul  = unitlist[i];
            while ( ul ) {
               punitlist next = ul->next;
               pattackweap atw = attackpossible ( ul->eht, vehicle->xpos, vehicle->ypos );
               if ( atw->count )
                  removeunit ( ul->eht );

               delete atw;
               ul = next;
            } /* endwhile */
         }

}

void  tsearchreactionfireingunits :: addunit ( pvehicle eht )
{
   int c = eht->color / 8;
   punitlist ul = new tunitlist;
   ul->eht = eht;
   ul->next= unitlist[c];
   unitlist[c] = ul;
}


void tsearchreactionfireingunits :: removeunit ( pvehicle vehicle )
{
   int c = vehicle->color / 8;
   punitlist ul = unitlist[c];
   punitlist last = NULL;
   while ( ul  &&  ul->eht != vehicle ) {
      last = ul;
      ul = ul->next;
  }
  if ( ul  &&  ul->eht == vehicle ) {
     if ( last )
        last->next = ul->next;
     else
        unitlist[c] = ul->next;

     delete ul;
  }
}


int  tsearchreactionfireingunits :: checkfield ( int x, int y, pvehicle &vehicle, MapDisplayInterface* md )
{

   int attacks = 0;
   int result = 0;

   pfield fld = getfield( x, y );
   npush ( fld->vehicle );
   fld->vehicle = vehicle;
   vehicle->xpos = x;
   vehicle->ypos = y;

   for ( int i = 0; i < 8; i++ ) {
      evaluatevisibilityfield ( actmap, fld, i, -1, actmap->getgameparameter ( cgp_initialMapVisibility ) );
      if ( fieldvisiblenow ( fld, i )) {
         punitlist ul  = unitlist[i];
         while ( ul  &&  !result ) {
            punitlist next = ul->next;
            pattackweap atw = attackpossible ( ul->eht, x, y );
            if ( atw->count && (ul->eht->reactionfire.enemiesAttackable & (1 << (vehicle->color / 8)))) {

               int ad1, ad2, dd1, dd2;
               int ulex = ul->eht->xpos;
               int uley = ul->eht->ypos;

               int strength = 0;
               int num;
               for ( int j = 0; j < atw->count; j++ )
                  if ( atw->strength[j]  > strength ) {
                     strength = atw->strength[j];
                     num = j;
                  }

               if ( md ) {
                  displaymessage2 ( "attacking with weapon %d ", atw->num[num] );
                  cursor.setcolor ( 8 );

                  cursor.gotoxy ( ul->eht->xpos, ul->eht->ypos );
                  int t = ticker;
                  while ( t + 15 > ticker )
                     releasetimeslice();

                  cursor.gotoxy ( x, y );
                  t = ticker;
                  while ( t + 15 > ticker )
                     releasetimeslice();

                  cursor.setcolor ( 0 );
                  cursor.hide();
               }

               pvehicle veh = ul->eht;
               tunitattacksunit battle ( veh, fld->vehicle, 0, atw->num[num] );

               ad1 = battle.av.damage;
               dd1 = battle.dv.damage;

               if ( md )
                  battle.calcdisplay ();
               else
                  battle.calc();

               ad2 = battle.av.damage;
               dd2 = battle.dv.damage;

               attacks++;

               if ( battle.dv.damage >= 100 )
                  result = 1;

               ul->eht->reactionfire.enemiesAttackable &= 0xff ^ ( 1 <<  (vehicle->color / 8) );
               removeunit ( ul->eht );

               battle.setresult();

               if ( ad2 < 100 )
                  veh->attacked = false;

//               logtoreplayinfo ( rpl_reactionfire, ulex, uley, x, y, ad1, ad2, dd1, dd2, atw->num[num] );

               dashboard.x = 0xffff;

            }
            delete atw;
            ul = next;
         } /* endwhile */
      }
   }
   npop ( fld->vehicle );

   if ( result )
      vehicle = NULL;

   return attacks;
}

tsearchreactionfireingunits :: ~tsearchreactionfireingunits()
{
   for ( int i = 0; i < 8; i++ ) {
      punitlist ul = unitlist[i];
      while ( ul ) {
         punitlist ul2 = ul->next;
         delete ul;
         ul = ul2;
      }
   }
}




int square ( int i )
{
   return i*i;
}

float square ( float i )
{
   return i*i;
}


pair<int,int> calcmovemalus( const MapCoordinate3D& start,
                            const MapCoordinate3D& dest,
                            pvehicle     vehicle,
                            WindMovement* wm,
                            bool*  inhibitAttack )
{

   static const  int         movemalus[6]  = { 8, 6, 3, 0, 3, 6 };

   int direc = getdirection ( start.x, start.y, dest.x, dest.y );




   int fuelcost = 1;
   int movecost;
   bool checkHemming = true;
   bool checkWind = wm != NULL;

   if ( start.getNumericalHeight() >= 0 && dest.getNumericalHeight() >= 0 ) {
      // changing height
      if ( start.getNumericalHeight() != dest.getNumericalHeight() ) {
          const Vehicletype::HeightChangeMethod* hcm = vehicle->getHeightChange( start.getNumericalHeight() < dest.getNumericalHeight() ? 1 : -1, start.getBitmappedHeight());
          if ( !hcm || hcm->dist != beeline ( start, dest )/maxmalq )
             fatalError("Calcmovemalus called with invalid height change distance");
          movecost = hcm->moveCost;
          fuelcost = max(hcm->dist,1);
          if ( inhibitAttack && !hcm->canAttack )
            *inhibitAttack = !hcm->canAttack; 
          checkHemming = false;
          checkWind = false;
      } else
         // flying
         if (start.getNumericalHeight() >= 4  )
            movecost = maxmalq;
         else {
            // not flying
            movecost = getfield( dest.x, dest.y )->getmovemalus( vehicle );
            checkWind = false;
         }

   } else
      if ( dest.getNumericalHeight() >= 0 ) {
        // moving out of container
        movecost = getfield( dest.x, dest.y )->getmovemalus( vehicle );
      } else {
        // moving from one container to another
        movecost = maxmalq;
        checkHemming = false;
        checkWind = false;
      }


   if ( checkHemming )
      for (int c = 0; c < sidenum; c++) {
         int x = dest.x;
         int y = dest.y;
         getnextfield( x,  y, c );
         pfield fld = getfield ( x, y );
         if ( fld ) {
           int d = (c - direc);

           if (d >= sidenum)
              d -= sidenum;

           if (d < 0)
              d += sidenum;

           pfield fld = getfield(x,y);
           if ( fld->vehicle && dest.getNumericalHeight() >= 0 ) {
              if ( getdiplomaticstatus(fld->vehicle->color) == cawar ) {

                 npush( vehicle->height );

                 vehicle->height = dest.getNumericalHeight();
                 if ( attackpossible28(fld->vehicle,vehicle ))
                    movecost += movemalus[d];
                 npop( vehicle->height );

              }
           }
         }
      }

    /*******************************/
    /*    Wind calculation        ÿ */
    /*******************************/
   if ( wm && checkWind )
      if (dest.getNumericalHeight() >= 4 && dest.getNumericalHeight() <= 6 &&
          start.getNumericalHeight() >= 4 && start.getNumericalHeight() <= 6 &&
          actmap->weather.windSpeed  ) {
         movecost -=  wm->getDist( direc );
         fuelcost -=  wm->getDist ( direc );
         if ( movecost < minmalq )
           movecost = minmalq;

         if ( fuelcost <= 0 )
           fuelcost = 0;
      }
   return make_pair(movecost,fuelcost);
}


WindMovement::WindMovement ( const pvehicle vehicle )
{
   for ( int i = 0; i < sidenum; i++ )
      wm[i] = 0;

   int movement = maxint;
   for ( int height = 4; height <= 6; height++ )
      if ( vehicle->typ->movement[height] )
         if ( vehicle->typ->movement[height] < movement )
            movement = vehicle->typ->movement[height];


   if ( movement ) {
      for ( int direc = 0; direc < sidenum; direc++) {
         float abswindspeed = ( actmap->weather.windSpeed * maxwindspeed * minmalq / 256 );

         float relwindspeed  =  abswindspeed / movement;

         float pi = 3.14159265;

         float relwindspeedx = 10 * relwindspeed * sin ( 2 * pi * actmap->weather.windDirection / sidenum );
         float relwindspeedy = -10 * relwindspeed * cos ( 2 * pi * actmap->weather.windDirection / sidenum );

         float xtg = 120 * sin ( 2 * pi * direc / sidenum );
         float ytg = -120 * cos ( 2 * pi * direc / sidenum );

         int disttofly = (int)sqrt ( square ( xtg - relwindspeedx) + square ( ytg - relwindspeedy ) );

         wm[direc] =  (120 - disttofly) / 10;
      }
   }
}



int WindMovement::getDist ( int dir )
{
   return 0;
}




void checkalliances_at_endofturn ( void )
{
   for ( int i = 0; i < 8; i++ ) {
      int act = actmap->actplayer;
      if ( i != act ) {

         if ( actmap->alliances[i] [act] == canewsetwar2 ) {
            // message "sneak attack" to all players except attacker

            int to = 0;
            for ( int j = 0; j < 8; j++ )
               if ( j != act )
                  to |= 1 << j;

            char txt[200];
            const char* sp = getmessage( 10001 );
            sprintf ( txt, sp, actmap->player[act].getName().c_str(), actmap->player[i].getName().c_str() );
            new Message ( txt, actmap, to );
         }

         if ( actmap->alliances[i] [act] == capeaceproposal  &&  actmap->alliances_at_beginofturn[i] != capeaceproposal ) {
            int to = 0;
            for ( int j = 0; j < 8; j++ )
               if ( j == i )
                  to |= 1 << j;

            char txt[200];
            const char* sp;
            if ( actmap->alliances[act] [i] == capeaceproposal )
               sp = getmessage( 10005 );  // accepts peace
            else
               sp = getmessage( 10003 );  // proposes peace

            sprintf ( txt, sp, actmap->player[act].getName().c_str() );
            new Message ( txt, actmap, to );
         }

         if ( actmap->alliances[i] [act] == cawarannounce ) {
            int to = 0;
            for ( int j = 0; j < 8; j++ )
               if ( j == i )
                  to |= 1 << j;

            char txt[200];
            const char* sp = getmessage( 10002 );
            sprintf ( txt, sp, actmap->player[act].getName().c_str() );
            new Message ( txt, actmap, to );
         }

      }
   }
}



void checkalliances_at_beginofturn ( void )
{
   int i;

   int act = actmap->actplayer ;
   for (i = 0; i < 8; i++ ) {
      if ( actmap->alliances[i][act] == cawarannounce || actmap->alliances[i][act] == canewsetwar2 ) {
         actmap->alliances[i][act] = cawar;
         actmap->alliances[act][i] = cawar;
         if ( actmap->shareview ) {
            actmap->shareview->mode[act][i] = false;
            actmap->shareview->mode[i][act] = false;
         }
      }
      if ( actmap->alliances[i][act] == capeaceproposal  &&  actmap->alliances[act][i] == capeaceproposal) {
         actmap->alliances[i][act] = capeace;
         actmap->alliances[act][i] = capeace;
      }
   }


   for ( i = 0; i < 8; i++ )
      actmap->alliances_at_beginofturn[i] = actmap->alliances[i][act];

}





void    tprotfzt::initbuffer( void )
{
   buf = new char[ vehicletypenum ];

   int i;
   for ( i=0; i < vehicletypenum ; i++ )
      buf[i] = actmap->player[ actmap->actplayer ].research.vehicletypeavailable ( getvehicletype_forpos ( i ) );
}



void    tprotfzt::evalbuffer( void )
{
   int i, num = 0;
   for ( i=0; i < vehicletypenum ;i++ ) {
      if (buf[i] == 0) {
          buf[i] = actmap->player[ actmap->actplayer ].research.vehicletypeavailable ( getvehicletype_forpos ( i ) );
          if ( buf[i] )
             num++;
      } else
          buf[i] = 0;

   }

   if ( num ) {
      tshownewtanks snt;
      snt.init ( buf );
      snt.run  ();
      snt.done ();
   }

   delete[] buf ;
}








void Building :: execnetcontrol ( void )
{
   for ( int i = 0; i < 3; i++ )
      if ( !actmap->isResourceGlobal(i) ) {
         if (  netcontrol & (cnet_moveenergyout << i )) {
            npush (  netcontrol );
            netcontrol |= (cnet_stopenergyinput << i );
            actstorage.resource(i) -= putResource ( actstorage.resource(i), i, 0 );
            npop (  netcontrol );
         } else
            if (  netcontrol & (cnet_storeenergy << i )) {
               npush (  netcontrol );
               netcontrol |= (cnet_stopenergyoutput << i );
               actstorage.resource(i) += getResource ( gettank(i) -  actstorage.resource(i), i, 0 );
               npop (  netcontrol );
            }
      }

}

                    /*   modes: 0 = energy   ohne abbuchen
                                1 = material ohne abbuchen
                                2 = fuel     ohne abbuchen

                                  +4         mit abbuchen                         /
                                  +8         nur Tributzahlungen kassieren       /
                                 +16         plus zur?ckliefern                 <  diese Bits schlieáen sich gegenseitig aus
                                 +32         usage zur?ckliefern                 \
                                 +64         tank zur?ckliefern                   \
                                 */


int  Building :: putResource ( int      need,    int resourcetype, int queryonly, int scope  )
{
   PutResource putresource ( scope );
   return putresource.getresource ( entryPosition.x, entryPosition.y, resourcetype, need, queryonly, color/8, scope );
}


int  Building :: getResource ( int      need,    int resourcetype, int queryonly, int scope )
{
   GetResource gr ( scope );
   return gr.getresource ( entryPosition.x, entryPosition.y, resourcetype, need, queryonly, color/8, scope );
}



void doresearch ( int i )
{
   // typedef struct tresbuild* presbuild;

   struct  tresbuild {
               int        eff;
               pbuilding  bld;
               tresbuild  *next;
           };


   tresbuild* first = NULL;

   for ( tmap::Player::BuildingList::iterator bi = actmap->player[i].buildingList.begin(); bi != actmap->player[i].buildingList.end(); bi++ ) {
      pbuilding bld = *bi;
      if ( bld->typ->special & cgresearchb ) {
         int energy, material;
         returnresourcenuseforresearch ( bld, bld->researchpoints, &energy, &material );

         tresbuild* a = new tresbuild;
         if ( energy )
            a->eff = 16384 * bld->researchpoints / energy ;
         else
            a->eff = maxint;

         a->bld = bld;

         tresbuild* b  = first;
         tresbuild* bp = NULL;
         while ( b && ( b->eff > a->eff ) ) {
            bp = b;
            b = b->next;
         }

         a->next = b;
         if ( bp )
            bp->next = a;
         else
            first = a;

      }
   }

   tresbuild*  a = first;
   while ( a ) {
      pbuilding bld = a->bld;
      int energy, material;
      returnresourcenuseforresearch ( bld, bld->researchpoints, &energy, &material );
      int ena = bld->getResource ( energy,   0, 1 );
      int maa = bld->getResource ( material, 1, 1 );

      int res = bld->researchpoints;
      if ( ena < energy  ||  maa < material ) {
         int diff = bld->researchpoints / 2;
         while ( ena < energy  ||  maa < material  || diff > 1) {
            if ( ena < energy  ||  maa < material )
               res -= diff;
            else
               res += diff;

            if ( diff > 1 )
               diff /=2;
            else
               diff = 1;

            returnresourcenuseforresearch ( bld, res, &energy, &material );
         }

         returnresourcenuseforresearch ( bld, res+1, &energy, &material );

         if ( ena >= energy  &&  maa >= material )
            res++;
         else
            returnresourcenuseforresearch ( bld, res, &energy, &material );
         
      }

      ena = bld->getResource ( energy,   0, 0 );
      maa = bld->getResource ( material, 1, 0 );

      if ( ena < energy || maa < material )
         displaymessage( "controls : doresearch : inconsistency in getting energy or material for building", 2 );

      actmap->player[i].research.progress += res;


      a = a->next;

   }

   tresbuild*  b;
   a = first;
   while ( a ) {
      b = a;
      a = a->next;
      delete b;
   }
}



void endRound ( void )
{
    actmap->actplayer = 0;
    actmap->time.set ( actmap->time.turn()+1, 0 );
    clearfahrspuren();

    for (int i = 0; i <= 7; i++)
       if (actmap->player[i].exist() ) {

          for ( tmap::Player::VehicleList::iterator j = actmap->player[i].vehicleList.begin(); j != actmap->player[i].vehicleList.end(); j++ )
             (*j)->endRound();

          typedef PointerList<Building::Work*> BuildingWork;
          BuildingWork buildingWork;

          for ( tmap::Player::BuildingList::iterator j = actmap->player[i].buildingList.begin(); j != actmap->player[i].buildingList.end(); j++ ) {
             Building::Work* w = (*j)->spawnWorkClasses( false );
             if ( w )
                buildingWork.push_back ( w );
          }

          bool didSomething;
          do {
             didSomething = false;
             for ( BuildingWork::iterator j = buildingWork.begin(); j != buildingWork.end(); j++ ) 
                if ( ! (*j)->finished() )
                   if ( (*j)->run() )
                      didSomething = true;
          } while ( didSomething );
          doresearch( i );
       }
}


void initchoosentechnology( void )
{
   Player& player = actmap->player[actmap->actplayer];
   player.research.progress = 0;

   Player::DissectionContainer::iterator di = player.dissections.begin();
   while ( di != player.dissections.end() ) {
      if ( di->tech == player.research.activetechnology ) {
         player.research.progress += di->points;
         di = player.dissections.erase ( di );
      } else
         di++;
   }
}


void newTurnForHumanPlayer ( int forcepasswordchecking = 0 )
{
   checkalliances_at_beginofturn ();
   for ( int p = 0; p < 8; p++ )
      actmap->player[p].existanceAtBeginOfTurn = actmap->player[p].exist();

   if ( actmap->player[actmap->actplayer].stat == Player::human ) {

      int humannum = 0;
      for ( int i = 0; i < 8; i++ )
         if ( actmap->player[i].exist() )
            if ( actmap->player[i].stat == Player::human )
               humannum++;

      if ( humannum > 1  ||  forcepasswordchecking > 0 ) {
         tlockdispspfld ldsf;
         backgroundpict.paint();

         bool firstRound = actmap->time.turn() == 1;
         bool specifyPassword = firstRound && actmap->player[actmap->actplayer].passwordcrc.empty();
         bool askForPassword = false;

         if ( (!actmap->player[actmap->actplayer].passwordcrc.empty() && actmap->player[actmap->actplayer].passwordcrc != CGameOptions::Instance()->getDefaultPassword() )
            || firstRound || (actmap->network && actmap->network->globalparams.reaskpasswords) ) {
               if ( forcepasswordchecking < 0 ) {
                  delete actmap;
                  actmap = NULL;
                  throw NoMapLoaded();
               } else {
                  bool stat;
                  do {
                     stat = enterpassword ( actmap->player[actmap->actplayer].passwordcrc, specifyPassword );
                  } while ( actmap->player[actmap->actplayer].passwordcrc.empty() && stat && viewtextquery ( 910, "warning", "~e~nter password", "~c~ontinue without password" ) == 0 ); /* enddo */
               }
         } else
            displaymessage("next player is:\n%s",3,actmap->player[actmap->actplayer].getName().c_str() );
      }

      checkforreplay();

      if (actmap->player[actmap->actplayer].research.activetechnology == NULL)
         if ( actmap->player[actmap->actplayer].research.progress ) {
            int mx  = actmap->player[actmap->actplayer].research.progress;
            choosetechnology();
            initchoosentechnology();
            actmap->player[actmap->actplayer].research.progress += mx;
         }
      if (actmap->player[actmap->actplayer].research.activetechnology != NULL) {
         while ((actmap->player[actmap->actplayer].research.activetechnology != NULL) &&
                (actmap->player[actmap->actplayer].research.progress >= actmap->player[actmap->actplayer].research.activetechnology->researchpoints))
                {
                 int mx = actmap->player[actmap->actplayer].research.progress -
                      actmap->player[actmap->actplayer].research.activetechnology->researchpoints;
                 showtechnology(actmap->player[actmap->actplayer].research.activetechnology);

                 tprotfzt   pfzt;
                 pfzt.initbuffer ();

                 actmap->player[actmap->actplayer].research.addtechnology();

                 pfzt.evalbuffer ();


                 choosetechnology();
                 initchoosentechnology();

                 actmap->player[actmap->actplayer].research.progress += mx;
         }
      }

      if ( actmap->lastjournalchange.abstime )
         if ( (actmap->lastjournalchange.turn() == actmap->time.turn() ) ||
              (actmap->lastjournalchange.turn() == actmap->time.turn()-1  &&  actmap->lastjournalchange.move() > actmap->actplayer ) )
                 viewjournal();

      dashboard.x = 0xffff;

      moveparams.movestatus = 0;


   }
   computeview( actmap );

   actmap->playerView = actmap->actplayer;

   initReplayLogging();

   actmap->xpos = actmap->cursorpos.position[ actmap->actplayer ].sx;
   actmap->ypos = actmap->cursorpos.position[ actmap->actplayer ].sy;

   cursor.gotoxy ( actmap->cursorpos.position[ actmap->actplayer ].cx, actmap->cursorpos.position[ actmap->actplayer ].cy , 0);

   dashboard.x = 0xffff;
   transfer_all_outstanding_tribute();
}





void sendnetworkgametonextplayer ( int oldplayer, int newplayer )
{
/*
   int num = 0;
   int pl[8];

   for (int i = 0; i < 8; i++) {
      if ( actmap->player[i].existent )
         if ( actmap->player[i].firstvehicle || actmap->player[i].firstbuilding ) {
            pl[num] = i;
            num++;
          }
   }  endfor */


   tnetworkcomputer* compi = &actmap->network->computer[ actmap->network->player[ oldplayer ].compposition ];
   while ( compi->send.transfermethod == NULL  ||  compi->send.transfermethodid == 0 )
        setupnetwork( actmap->network, 2, oldplayer );

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
      displaymessage ( "error saving file", 1 );
   } /* endcatch */

   delete actmap;
   actmap = NULL;
   displaymessage( " data transfer finished",1);


   throw NoMapLoaded ();

}



void endTurn ( void )
{
   closeReplayLogging();

   /* *********************  vehicle ********************  */

   mousevisible(false);
   if ( actmap->actplayer >= 0 ) {
      actmap->cursorpos.position[actmap->actplayer].cx = getxpos();
      actmap->cursorpos.position[actmap->actplayer].cy = getypos();
      actmap->cursorpos.position[actmap->actplayer].sx = actmap->xpos;
      actmap->cursorpos.position[actmap->actplayer].sy = actmap->ypos;
      actmap->player[actmap->actplayer].ASCversion = getNumericVersion();

      tmap::Player::VehicleList toRemove;
      for ( tmap::Player::VehicleList::iterator v = actmap->player[actmap->actplayer].vehicleList.begin(); v != actmap->player[actmap->actplayer].vehicleList.end(); v++ ) {
         pvehicle actvehicle = *v;

         // Bei Žnderungen hier auch die Windanzeige dashboard.PAINTWIND aktualisieren !!!

         if (( actvehicle->height >= chtieffliegend )   &&  ( actvehicle->height <= chhochfliegend ) && ( getfield(actvehicle->xpos,actvehicle->ypos)->vehicle == actvehicle)) {
            if ( getmaxwindspeedforunit ( actvehicle ) < actmap->weather.windSpeed*maxwindspeed ){
               ASCString ident = "The unit " + (*v)->getName() + " at position ("+strrr((*v)->getPosition().x)+"/"+strrr((*v)->getPosition().y)+") crashed because of the strong wind";
               new Message ( ident, actmap, 1<<(*v)->getOwner());
               toRemove.push_back ( *v );
            } else {

               int j = actvehicle->tank.fuel - actvehicle->typ->fuelConsumption * nowindplanefuelusage;

               if ( actvehicle->height <= chhochfliegend ) {
                  int mo = actvehicle->typ->movement[log2(actvehicle->height)];
                  if ( mo )
                     j -= ( actvehicle->getMovement() * 64 / mo)
                          * (actmap->weather.windSpeed * maxwindspeed / 256 ) * actvehicle->typ->fuelConsumption / ( minmalq * 64 );
                  else
                     j -= (actmap->weather.windSpeed * maxwindspeed / 256 ) * actvehicle->typ->fuelConsumption / ( minmalq * 64 );
               }
              //          movement * 64        windspeed * maxwindspeed         fuelConsumption
              // j -=   ----------------- *  ----------------------------- *   -----------
              //          typ->movement                 256                       64 * 8
              //
              //

              //gek?rzt:
              //
              //             movement            windspeed * maxwindspeed
              // j -= --------------------- *  ----------------------------   * fuelConsumption
              //           typ->movement             256   *      8
              //
              //
              //
              // Falls eine vehicle sich nicht bewegt hat, bekommt sie soviel Sprit abgezogen, wie sie zum zur?cklegen der Strecke,
              // die der Wind pro Runde zur?ckgelegt hat, fuelConsumptionen w?rde.
              // Wenn die vehicle sich schon bewegt hat, dann wurde dieser Abzug schon beim movement vorgenommen, so daá er hier nur

              // noch fuer das ?briggebliebene movement stattfinden muá.
              //


               if (j < 0) {
                   ASCString ident = "The unit " + (*v)->getName() + " at position ("+strrr((*v)->getPosition().x)+"/"+strrr((*v)->getPosition().y)+") crashed due to lack of fuel";
                   new Message ( ident, actmap, 1<<(*v)->getOwner());

                  toRemove.push_back ( *v );
               } else
                  actvehicle->tank.fuel = j;
            }
         }

         if ( actvehicle )
            actvehicle->endTurn();

      }

      for ( tmap::Player::VehicleList::iterator v = toRemove.begin(); v != toRemove.end(); v++ )
         delete *v;

      checkunitsforremoval ();
   }

     /* *********************  allianzen ********************  */

  checkalliances_at_endofturn ();

     /* *********************  messages ********************  */

  MessagePntrContainer::iterator mi = actmap->unsentmessage.begin();
  while ( mi != actmap->unsentmessage.end() ) {
     actmap->player[ actmap->actplayer ].sentmessage.push_back ( *mi );
     for ( int i = 0; i < 8; i++ )
        if ( (*mi)->to & ( 1 << i ))
           actmap->player[ i ].unreadmessage.push_back ( *mi );

     mi = actmap->unsentmessage.erase ( mi );
  }

  if ( !actmap->newJournal.empty() ) {
     ASCString add = actmap->gameJournal;

     char tempstring[100];
     char tempstring2[100];
     sprintf( tempstring, "#color0# %s ; turn %d #color0##crt##crt#", actmap->player[actmap->actplayer].getName().c_str(), actmap->time.turn() );
     sprintf( tempstring2, "#color%d#", getplayercolor ( actmap->actplayer ));

     int fnd;
     do {
        fnd = 0;
        if ( !add.empty() )
           if ( add.find ( '\n', add.length()-1 ) != add.npos ) {
              add.erase ( add.length()-1 );
              fnd++;
           } else
             if ( add.length() > 4 )
                if ( add.find ( "#crt#", add.length()-5 ) != add.npos ) {
                  add.erase ( add.length()-5 );
                  fnd++;
                }

     } while ( fnd ); /* enddo */

     add += tempstring2;
     add += actmap->newJournal;
     add += tempstring;

     actmap->gameJournal = add;
     actmap->newJournal.erase();

     actmap->lastjournalchange.set ( actmap->time.turn(), actmap->actplayer );
  }
}


void nextPlayer( void )
{
   int oldplayer = actmap->actplayer;

   int runde = 0;
   do {
      actmap->actplayer++;
      actmap->time.set ( actmap->time.turn(), 0 );
      if (actmap->actplayer > 7) {
         endRound();
         runde++;
      }
   }  while (!(actmap->player[actmap->actplayer].exist()  || (runde > 2) ));

   if (runde > 2) {
      displaymessage("There are no players left any more !",1);
      delete actmap;
      actmap = NULL;
      throw NoMapLoaded ();
   }

   int newplayer = actmap->actplayer;
   actmap->playerView = actmap->actplayer;

   if ( oldplayer >= 0 && actmap->network &&  oldplayer != actmap->actplayer && actmap->network->player[ newplayer ].compposition != actmap->network->player[ oldplayer ].compposition )
      sendnetworkgametonextplayer ( oldplayer, newplayer );
   else {
/*
      tlockdispspfld ldsf;

      int forcepwd;  // Wenn der aktuelle player gerade verloren hat, muá fuer den n„chsten player die Passwortabfrage kommen, auch wenn er nur noch der einzige player ist !
      if ( oldplayer >= 0  &&  !actmap->player[oldplayer].existent )
         forcepwd = 1;
      else
         forcepwd = 0;

      newturnforplayer( forcepwd );
*/
   }
}


void runai( int playerView )
{
   if ( CGameOptions::Instance()->runAI >= 0 ) {
      actmap->playerView = playerView;

      if ( !actmap->player[ actmap->actplayer ].ai )
         actmap->player[ actmap->actplayer ].ai = new AI ( actmap, actmap->actplayer );

      actmap->player[ actmap->actplayer ].ai->run();
      dashboard.x = 0xffff;

   } else {
      tlockdispspfld displock;
      checkalliances_at_beginofturn ();
      computeview( actmap );
      displaymessage("no AI available yet", 1 );
   }
}

void next_turn ( int playerView )
{
   int startTurn = actmap->time.turn();

   int pv;
   if ( playerView == -2 ) {
      if ( actmap->time.turn() <= 0 || actmap->actplayer < 0 )
         pv = -1;
      else
         if ( actmap->player[actmap->actplayer].stat != Player::human )
            pv = -1;
         else
            pv = actmap->actplayer;
   } else
      pv = playerView;


   int bb = cursor.an;
   if (bb)
      cursor.hide();

   do {
     endTurn();
     nextPlayer();
     if ( actmap->player[actmap->actplayer].stat == Player::computer )
        runai( pv );

     if ( actmap->time.turn() >= startTurn+2 ) {
        displaymessage("no human players found !", 1 );
        delete actmap;
        actmap = NULL;
        throw NoMapLoaded();
     }

   } while ( actmap->player[actmap->actplayer].stat != Player::human ); /* enddo */

   newTurnForHumanPlayer();

   if (bb)
     cursor.display();
}

void initNetworkGame ( void )
{
   while ( actmap->player[actmap->actplayer].stat != Player::human ) {

     if ( actmap->player[actmap->actplayer].stat == Player::computer ) {
        computeview( actmap );
        runai(-1);
     }
     endTurn();
     nextPlayer();
   }

   newTurnForHumanPlayer( 0 );

   for ( int i = 0; i < 8; i++ )
      if  ( actmap->player[i].exist() )
         if ( actmap->player[i].ASCversion > 0 )
            if ( (actmap->player[i].ASCversion & 0xffffff00) > getNumericVersion() ) {
               new Message ( ASCString("Player ") + actmap->player[i].getName() + " is using a newer version of ASC. \nPlease check www.asc-hq.org for updates.", actmap, 1<<actmap->actplayer );
               return;
            }

}


void continuenetworkgame ( void )
{
   tlockdispspfld ldsf;

   tnetwork network;
/*
   int stat;
   int go = 0;
   do {
      stat = setupnetwork( &network, 1+8 );
      if ( stat == 1 ) {
         return;
      }
      if ( network.computer[0].receive.transfermethod == 0 )
         displaymessage("please choose a transfer method !",1 );
      else
         if ( network.computer[0].receive.transfermethodid != network.computer[0].receive.transfermethod->getid() )
            displaymessage("please setup transfer method !", 1 );
         else
            if ( !network.computer[0].receive.transfermethod->validateparams( &network.computer[0].receive.data, TN_RECEIVE ))
               displaymessage("please setup transfer method !", 1 );
            else
               go = 1;
   } while ( !go );

*/
   int stat;
   int go = 0;
   do {
      stat = network.computer[0].receive.transfermethod->setupforreceiving ( &network.computer[0].receive.data );
      if ( stat == 0 )
         return;

      if ( network.computer[0].receive.transfermethod  &&
           network.computer[0].receive.transfermethodid == network.computer[0].receive.transfermethod->getid()  &&
           network.computer[0].receive.transfermethod->validateparams( &network.computer[0].receive.data, TN_RECEIVE ))
           go = 1;
   } while ( !go );


   try {
       displaymessage ( " starting data transfer ",0);

       network.computer[0].receive.transfermethod->initconnection ( TN_RECEIVE );
       network.computer[0].receive.transfermethod->inittransfer ( &network.computer[0].receive.data );

       tnetworkloaders nwl;
       nwl.loadnwgame ( network.computer[0].receive.transfermethod->stream );

       network.computer[0].receive.transfermethod->closetransfer();

       network.computer[0].receive.transfermethod->closeconnection();

       if ( actmap->network )
          setallnetworkpointers ( actmap->network );

       removemessage();
   } /* endtry */

   catch ( InvalidID err ) {
      displaymessage( err.getMessage().c_str(), 1 );
      throw NoMapLoaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      if ( err.expected < err.found )
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nPlease install the latest version from www.asc-hq.org", 1, err.getFileName().c_str(), err.expected, err.found );
      else
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nThis is a bug, please report it!", 1, err.getFileName().c_str(), err.expected, err.found );

      throw NoMapLoaded();
   } /* endcatch */
   catch ( tcompressionerror err ) {
      displaymessage( "The file cannot be decompressed. \nIt has probably been damaged during transmission from the previous player to you.\nTry sending it zip compressed or otherwise encapsulated.\n", 1 );
      throw NoMapLoaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading game %s ", 1, err.getFileName().c_str() );
      throw NoMapLoaded();
   } /* endcatch */
   catch ( ASCexception ) {
      displaymessage( "error loading game", 1 );
      throw NoMapLoaded();
   } /* endcatch */
   if ( !actmap || actmap->xsize <= 0 || actmap->ysize <= 0 )
      throw NoMapLoaded();


   initNetworkGame( );
}





void returnresourcenuseforresearch ( const pbuilding bld, int research, int* energy, int* material )
{
   /*
   double esteigung = 55;
   double msteigung = 40;
   */

   double res = research;
   double deg = res / bld->typ->maxresearchpoints;

   double m = 1 / log ( minresearchcost + maxresearchcost );

   *energy   = (int)(researchenergycost   * research * ( exp ( deg / m ) - ( 1 - minresearchcost ) ) / 1000 * (researchcostdouble+res)/researchcostdouble);
   *material = (int)(researchmaterialcost * research * ( exp ( deg / m ) - ( 1 - minresearchcost ) ) / 1000 * (researchcostdouble+res)/researchcostdouble);
/*
   if ( bld->typ->maxresearchpoints > 0 ) {
      *material = researchmaterialcost * research * 
      ( exp ( res / msteigung ) - 1 ) / ( exp ( (double)bld->typ->maxresearchpoints / (msteigung*2) ) - 1 ) * (10000+res)/10000 / 1000;
      *energy   = researchenergycost   * ( exp ( res / esteigung ) - 1 ) / ( exp ( (double)bld->typ->maxresearchpoints / (esteigung*2) ) - 1 ) * (10000+res)/10000 / 1000;
   } else {
      *material = 0;
      *energy = 0;
   }
  */
}


void dissectvehicle ( pvehicle eht )
{
  int i,j,k;

   ptechnology techs[32];
   int technum = 0;
   memset ( techs, 0, sizeof ( techs ));
   for (i = 0; i <= eht->klasse; i++) 
      for (j = 0; j < 4; j++ )
         if ( eht->typ->classbound[i].techrequired[j] ) 
           if ( !actmap->player[actmap->actplayer].research.technologyresearched ( eht->typ->classbound[i].techrequired[j] )) {
               int found =  0;
               for (k = 0; k < technum; k++ )
                  if ( techs[k]->id == eht->typ->classbound[i].techrequired[j] )
                     found = 1;
               if ( !found ) {
                  ptechnology tec = gettechnology_forid ( eht->typ->classbound[i].techrequired[j] );
                  if ( tec )
                     techs[technum++] = tec;
               }
           }


   for ( i = 0; i < technum; i++ ) {

      Player& player = actmap->player[actmap->actplayer];
      if ( player.research.activetechnology != techs[i] ) {

         int found = 0;     // Bit 1: Technologie gefunden
                            //     2: vehicletype gefunden      
                            //     3: Technologie+vehicletype gefunden

         Player::DissectionContainer::iterator di = player.dissections.begin();
         while ( di != player.dissections.end() ) {
            if ( di->fzt == eht->typ )
               if ( di->tech == techs[i] )
                  found |= 4;
               else
                  found |= 2;
            
            if ( di->tech == techs[i] )
               found |= 1;
      
            di++;
         }
   
         if ( found & 4 ) {
            di = player.dissections.begin();
            while ( di != player.dissections.end() ) {
               if ( di->fzt == eht->typ )
                  if ( di->tech == techs[i] ) {
                     di->points += di->orgpoints / ( 1 << di->num);
                     di->num++;
                  }
               di++;
            }
         } else {
            Player::Dissection du;

            du.tech = techs[i];
            du.fzt = getvehicletype_forid ( eht->typ->id );
   
            if ( found & 1 )
               du.orgpoints = du.tech->researchpoints / dissectunitresearchpointsplus2;
            else
               du.orgpoints = du.tech->researchpoints / dissectunitresearchpointsplus;
   
            du.points = du.orgpoints;
            du.num = 1;

            player.dissections.push_back ( du );
         }
      } else 
         player.research.progress+= techs[i]->researchpoints / dissectunitresearchpointsplus;

   }
}



void         generatevehicle_cl ( pvehicletype fztyp,
                                  int         col,
                                  pvehicle &   vehicle,
                                  int          x,
                                  int          y )
{ 
   if ( actmap->player[ actmap->actplayer ].research.vehicletypeavailable ( fztyp ) ) {

      vehicle = new Vehicle ( fztyp, actmap, col );
      if ( fztyp->classnum )
        for (int i = 0; i < fztyp->classnum ; i++ ) 
           if ( actmap->player[ actmap->actplayer ].research.vehicleclassavailable( fztyp, i ) )
              vehicle->klasse = i;
           else
              break;
              
      logtoreplayinfo ( rpl_produceunit, (int) fztyp->id , (int) col * 8, x, y, (int) vehicle->klasse, (int) vehicle->networkid );

      vehicle->xpos = x;
      vehicle->ypos = y;
      vehicle->setup_classparams_after_generation ();

      if ( actmap->getgameparameter(cgp_bi3_training) >= 1 ) {
         int cnt = 0;

         for ( tmap::Player::BuildingList::iterator bi = actmap->player[actmap->actplayer].buildingList.begin(); bi != actmap->player[actmap->actplayer].buildingList.end(); bi++ )
            if ( (*bi)->typ->special & cgtrainingb )
               cnt++;

         vehicle->experience += cnt * actmap->getgameparameter(cgp_bi3_training);
         if ( vehicle->experience > maxunitexperience )
            vehicle->experience = maxunitexperience;
      }
   } else
     vehicle = NULL;
} 







void cmousecontrol :: reset ( void )
{
   mousestat = 0;
}


void cmousecontrol :: chkmouse ( void )
{
   if ( CGameOptions::Instance()->mouse.fieldmarkbutton )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.fieldmarkbutton ) {
         int x; 
         int y;
         int r = getfieldundermouse ( &x, &y );
   
         if ( r ) 
            if ( (cursor.posx != x || cursor.posy != y) && ( moveparams.movestatus == 0) ) {
               // collategraphicoperations cgo;
               mousestat = 1;
               mousevisible(false);
               cursor.hide();
               cursor.posx = x;
               cursor.posy = y;
               cursor.show();

               dashboard.paint( getactfield(), actmap-> playerView );

               mousevisible(true);
            } 
      }

   if ( CGameOptions::Instance()->mouse.centerbutton )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.centerbutton ) {
         int x; 
         int y;
         int r = getfieldundermouse ( &x, &y );
         x += actmap->xpos;
         y += actmap->ypos;
   
         if ( r ) {
            int newx = x - idisplaymap.getscreenxsize() / 2;
            int newy = y - idisplaymap.getscreenysize() / 2;

            if ( newx < 0 )
               newx = 0;
            if ( newy < 0 )
               newy = 0;
            if ( newx > actmap->xsize - idisplaymap.getscreenxsize() )
               newx = actmap->xsize - idisplaymap.getscreenxsize();
            if ( newy > actmap->ysize - idisplaymap.getscreenysize() )
               newy = actmap->ysize - idisplaymap.getscreenysize();
   
            if ( newy & 1 )
               newy--;

            if ( newx != actmap->xpos  || newy != actmap->ypos ) {
               // collategraphicoperations cgo;
               cursor.hide();
               actmap->xpos = newx;
               actmap->ypos = newy;
               displaymap();
               cursor.posx = x - actmap->xpos;
               cursor.posy = y - actmap->ypos;

               // cursor.gotoxy ( x, y );
               cursor.show();
            }
            while ( mouseparams.taste == CGameOptions::Instance()->mouse.centerbutton )
               releasetimeslice();
         }
      }

   if ( CGameOptions::Instance()->mouse.smallguibutton )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.smallguibutton ) {
         int x; 
         int y;
         int r = getfieldundermouse ( &x, &y );
   
         if ( r ) 
            if ( (cursor.posx != x || cursor.posy != y) && ( moveparams.movestatus == 0   ||  getfield(actmap->xpos + x , actmap->ypos + y)->a.temp == 0) ) {
               // collategraphicoperations cgo;
               mousestat = 1;
               mousevisible(false);
               cursor.hide();
               cursor.posx = x;
               cursor.posy = y;
               cursor.show();
               mousevisible(true);
            } else 
              if (    mousestat == 2
                  ||  mousestat == 0
                  ||  ((moveparams.movestatus || pendingVehicleActions.action) && getfield( actmap->xpos + x, actmap->ypos + y)->a.temp )
                  ||  CGameOptions::Instance()->mouse.singleClickAction ) {
                 {
                    // collategraphicoperations cgo;
                    if ( cursor.posx != x || cursor.posy != y ) {
                       mousevisible(false);
                       cursor.hide();
                       cursor.posx = x;
                       cursor.posy = y;
                       cursor.show();

                       dashboard.paint( getactfield(), actmap-> playerView );

                       mousevisible(true);
                    }

                    actgui->painticons();
                 }
                 pfield fld = getactfield();
                 bool positionedUnderCursor = false;
                 if (( fld->vehicle || fld->building) && fieldvisiblenow(fld))
                    positionedUnderCursor = true;
                 if ( fld->a.temp )
                    positionedUnderCursor = true;
                 actgui->paintsmallicons( CGameOptions::Instance()->mouse.smallguibutton, !positionedUnderCursor );
                 mousestat = 0;
              }
      } else 
        if ( mousestat == 1 )
           mousestat = 2;

   if ( CGameOptions::Instance()->mouse.largeguibutton )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.largeguibutton ) {
         int x; 
         int y;
         int r = getfieldundermouse ( &x, &y );
         if ( r && ( cursor.posx != x || cursor.posy != y) ) {
            mousevisible(false);
            cursor.hide();
            cursor.posx = x;
            cursor.posy = y;
            cursor.show();
            mousevisible(true);
         }
         actgui->painticons();

         actgui->runpressedmouse ( mouseparams.taste );
      }

   if ( CGameOptions::Instance()->mouse.unitweaponinfo )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.unitweaponinfo ) {
         int x; 
         int y;
         int r = getfieldundermouse ( &x, &y );
         if ( r && ( cursor.posx != x || cursor.posy != y) ) {
            mousevisible(false);
            cursor.hide();
            cursor.posx = x;
            cursor.posy = y;
            cursor.show();
            mousevisible(true);
         }
         actgui->painticons();
         if ( getactfield()->vehicle ) {
            dashboard.paintvehicleinfo( getactfield()->vehicle, NULL, NULL, NULL );
            dashboard.paintlweaponinfo();
         }
      }

/*
   if ( mouseparams.taste == minmenuekey ) {
      int x; 
      int y;
      if (  getfieldundermouse ( &x, &y ) ) {
         actgui->paintsmallicons( minmenuekey );
      }
   }
*/

}


void  checkforvictory ( )
{
   if ( !actmap->continueplaying ) {
      int plnum = 0;
      for ( int i = 0; i < 8; i++ )
         if ( !actmap->player[i].exist() && actmap->player[i].existanceAtBeginOfTurn ) {
            int to = 0;
            for ( int j = 0; j < 8; j++ )
               if ( j != i )
                  to |= 1 << j;


            char txt[1000];
            const char* sp = getmessage( 10010 ); // Message "player has been terminated"

            sprintf ( txt, sp, actmap->player[i].getName().c_str() );
            new Message ( txt, actmap, to  );

            actmap->player[i].existanceAtBeginOfTurn = false;

            if ( i == actmap->actplayer ) {
               displaymessage ( getmessage ( 10011 ),1 );

               int humannum=0;
               for ( int j = 0; j < 8; j++ )
                  if (actmap->player[j].exist() && actmap->player[j].stat == Player::human )
                     humannum++;
               if ( humannum )
                  next_turn();
               else {
                  delete actmap;
                  actmap = NULL;
                  throw NoMapLoaded();
               }
            }
         } else
            plnum++;

      if ( plnum <= 1 ) {
         if ( actmap->player[actmap->actplayer].ai &&  actmap->player[actmap->actplayer].ai->isRunning() ) {
            displaymessage("You lost!",1);
         } else {
            displaymessage("Congratulations!\nYou won!",1);
            if (choice_dlg("Do you want to continue playing ?","~y~es","~n~o") == 2) {
               delete actmap;
               actmap = NULL;
               throw NoMapLoaded();
            } else {
               actmap->continueplaying = 1;
               if ( actmap->replayinfo ) {
                  delete actmap->replayinfo;
                  actmap->replayinfo = 0;
               }
            }
         }
      }
   }
}


