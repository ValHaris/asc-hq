//     $Id: unitctrl.cpp,v 1.91 2002-11-01 14:06:53 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.90  2002/10/27 22:19:44  mbickel
//      Added bridge terrain property
//      Some restructuring of the graphic set code
//
//     Revision 1.89  2002/10/01 09:23:42  mbickel
//      Fixed many bugs
//      Added inheritance to text files
//      updated graphics
//
//     Revision 1.88  2002/09/19 20:20:06  mbickel
//      Cleanup and various bug fixes
//
//     Revision 1.87  2002/05/07 19:52:47  mbickel
//      Updated documentation
//      Dialog themes can be reloaded during runtime.
//
//     Revision 1.86  2002/04/09 22:19:06  mbickel
//      Fixed AI bugs
//      Fixed: invalid player name displayed in dashboard
//
//     Revision 1.85  2002/04/05 19:01:45  mbickel
//      Fixed: reaction fire attackes being replayed twice
//      Fixed: crash when reaction fire during landing on carrier
//
//     Revision 1.84  2002/03/27 00:18:21  mbickel
//      Changed the resource weight
//
//     Revision 1.83  2002/03/26 22:23:09  mbickel
//      Fixed: music was started even if turned off in ini file
//      Fixed: crash in reaction fire
//      Fixed: reaction fire not occuring when changing height
//
//     Revision 1.82  2001/12/19 17:16:29  mbickel
//      Some include file cleanups
//
//     Revision 1.81  2001/12/14 10:20:05  mbickel
//      Cleanup and enhancements to configure.in
//      Removed last remains of octagonal version from source files
//
//     Revision 1.80  2001/11/28 14:55:12  mbickel
//      Fixed: airplane flying out of building has wrong movement left
//      Fixed: crash in AI
//
//     Revision 1.79  2001/11/28 13:03:16  mbickel
//      Fixed: attack selectable although 0% hit accuracy
//      Fixed: refuelling not possible if weapon had wrong target height
//      Fixed: repair icon displayed when refuelling
//
//     Revision 1.78  2001/11/22 13:49:32  mbickel
//      Fixed crash in Mapeditor when selection color 9
//      Fixed: turrets being displayed gray
//      Fixed: division by 0 by AI movement
//
//     Revision 1.77  2001/11/18 19:31:05  mbickel
//      Fixed crash when reaction fire kills a unit during height change
//
//     Revision 1.76  2001/11/15 20:46:05  mbickel
//      Fixed: replay not working when moving units out of carriers
//
//     Revision 1.75  2001/11/15 20:16:01  mbickel
//      Added a new BI3 import table
//      Fixed movement reduction changing height of a nearly empty unit and
//         refuelling it afterwards
//      Better error message for "ID not found"
//
//     Revision 1.74  2001/11/12 18:28:34  mbickel
//      Fixed graphical glitches when unit moves near border
//      If max num of mines exceeded, no icon is displayed for placing a new one
//      Fixed: some airplanes could not move after ascend
//      Fixed: couldn't build satellites on fields no accessible if satellite was
//        on ground level.
//
//     Revision 1.73  2001/11/08 17:32:14  mbickel
//      Fixed AI crash
//      Added Replay to AI
//      New replay commands
//
//     Revision 1.72  2001/10/21 20:00:30  mbickel
//      Fixed AI problems
//      Fixed: wrong message when unit could not attack
//      Fxied: invalid order in which objects were displayed
//      Replay: moving units are only displayed if BOTH fields are visible
//
//     Revision 1.71  2001/10/02 14:06:29  mbickel
//      Some cleanup and documentation
//      Bi3 import tables now stored in .asctxt files
//      Added ability to choose amoung different BI3 import tables
//      Added map transformation tables
//
//     Revision 1.70  2001/09/25 15:13:07  mbickel
//      New version number
//      Fixed crash when reaction fire during ascend
//
//     Revision 1.69  2001/09/23 23:06:20  mbickel
//      Fixed:
//       - ascent/descent during reactionfire
//       - movement with nearly empty fuel tank
//       - production icon displayed although unit could not be produced
//       - invisible building becoming visible in fog of war
//
//     Revision 1.68  2001/09/13 17:43:12  mbickel
//      Many, many bug fixes
//
//     Revision 1.67  2001/08/24 15:50:08  mbickel
//      AI performs better services when there are no service units
//
//     Revision 1.66  2001/08/15 14:02:10  mbickel
//      Updated message texts
//
//     Revision 1.65  2001/08/09 15:58:59  mbickel
//      Some usability improvements in the map editor
//      More flexible BI3 map import
//      Better textfile error messages
//
//     Revision 1.64  2001/08/09 10:28:23  mbickel
//      Fixed AI problems
//      Mapeditor can edit a units AI parameter
//
//     Revision 1.63  2001/07/30 18:03:08  mbickel
//      Fixed some more warnings
//
//     Revision 1.62  2001/07/28 11:19:12  mbickel
//      Updated weaponguide
//      moved item repository from spfst to itemrepository
//
//     Revision 1.61  2001/07/27 21:13:35  mbickel
//      Added text based file formats
//      Terraintype and Objecttype restructured
//
//     Revision 1.60  2001/07/25 18:00:16  mbickel
//      Fixed: units could repair without repairing function but
//      Fixed: airplanes tried to ascend through buildings
//      Fixed: crash when kamikaze attack occured during movement
//      Fixed: log(0) error in refuelling dialog
//
//     Revision 1.59  2001/07/15 21:31:03  mbickel
//      The movement sounds can now fade in and out
//
//     Revision 1.58  2001/07/15 21:00:25  mbickel
//      Some cleanup in the vehicletype class
//
//     Revision 1.57  2001/07/14 19:13:16  mbickel
//      Rewrote sound system
//      Moveing units make sounds
//      Added sound files to data
//
//     Revision 1.56  2001/07/13 19:33:30  mbickel
//      Fixed crashes in the dashboards experience display
//      Fixed inconsistent movement cost calculation (which caused
//            AI and replay warnings)
//      Fixed crash when starting network game from main menu
//
//     Revision 1.55  2001/07/13 14:02:48  mbickel
//      Fixed inconsistency in replay (shareviewchange)
//      Fixed sound initialization problem
//      Speed up of movement
//
//     Revision 1.54  2001/07/13 12:53:01  mbickel
//      Fixed duplicate icons in replay
//      Fixed crash in tooltip help
//
//     Revision 1.53  2001/03/30 12:43:16  mbickel
//      Added 3D pathfinding
//      some cleanup and documentation
//      splitted the ai into several files, now located in the ai subdirectory
//      AI cares about airplane servicing and range constraints
//
//     Revision 1.52  2001/03/23 16:02:56  mbickel
//      Some restructuring;
//      started rewriting event system
//
//     Revision 1.51  2001/02/18 17:52:38  mbickel
//      Fixed some compilation problems on Linux
//
//     Revision 1.50  2001/02/18 15:37:23  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.49  2001/02/06 16:27:44  mbickel
//      bugfixes, bugfixes and bugfixes
//
//     Revision 1.48  2001/02/04 21:27:01  mbickel
//      The AI status is written to savegames -> new savegame revision
//      Lots of bug fixes
//
//     Revision 1.47  2001/02/01 22:48:53  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.46  2001/01/28 17:19:16  mbickel
//      The recent cleanup broke some source files; this is fixed now
//
//     Revision 1.45  2001/01/21 16:37:23  mbickel
//      Moved replay code to own file ( replay.cpp )
//      Fixed compile problems done by cleanup
//
//     Revision 1.44  2001/01/19 13:33:57  mbickel
//      The AI now uses hemming
//      Several bugfixes in Vehicle Actions
//      Moved all view calculation to viewcalculation.cpp
//      Mapeditor: improved keyboard support for item selection
//
//     Revision 1.43  2001/01/04 15:14:12  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.42  2000/11/21 20:27:10  mbickel
//      Fixed crash in tsearchfields (used by object construction for example)
//      AI improvements
//      configure.in: added some debug output
//                    fixed broken check for libbz2
//
//     Revision 1.41  2000/11/14 20:36:46  mbickel
//      The AI can now use supply vehicles
//      Rewrote objecttype IO routines to make the structure independant of
//       the memory layout
//
//     Revision 1.40  2000/11/11 11:05:21  mbickel
//      started AI service functions
//
//     Revision 1.39  2000/11/08 19:31:17  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
//     Revision 1.38  2000/10/31 10:42:48  mbickel
//      Added building->vehicle service to vehicle controls
//      Moved tmap methods to gamemap.cpp
//
//     Revision 1.37  2000/10/11 15:33:47  mbickel
//      Adjusted small editors to the new ASC structure
//      Watcom compatibility
//
//     Revision 1.36  2000/10/11 14:26:52  mbickel
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
//     Revision 1.35  2000/09/27 16:08:31  mbickel
//      AI improvements
//
//     Revision 1.34  2000/09/25 20:04:42  mbickel
//      AI improvements
//
//     Revision 1.33  2000/09/25 13:25:54  mbickel
//      The AI can now change the height of units
//      Heightchaning routines improved
//
//     Revision 1.32  2000/09/24 19:57:06  mbickel
//      ChangeUnitHeight functions are now more powerful since they use
//        UnitMovement on their own.
//
//     Revision 1.31  2000/09/17 15:20:38  mbickel
//      AI is now automatically invoked (depending on gameoptions)
//      Some cleanup
//
//     Revision 1.30  2000/09/10 10:19:52  mbickel
//      AI improvements
//
//     Revision 1.29  2000/08/29 10:36:51  mbickel
//      Removed Debug code
//      Fixed bug: movement left when changing height into buildings
//
//     Revision 1.28  2000/08/28 19:49:43  mbickel
//      Fixed: replay exits when moving satellite out of orbiter
//      Fixed: airplanes being able to endlessly takeoff and land
//      Fixed: buildings constructable by unit without resources
//
//     Revision 1.27  2000/08/13 11:55:11  mbickel
//      Attacking now decreases a units movement by 20% if it has the
//        "move after attack" flag.
//
//     Revision 1.26  2000/08/12 15:03:26  mbickel
//      Fixed bug in unit movement
//      ASC compiles and runs under Linux again...
//
//     Revision 1.25  2000/08/09 13:18:11  mbickel
//      Fixed: invalid movement cost for airplanes flying with wind
//      Fixed: building mineral resource info: wrong lines for availability
//
//     Revision 1.24  2000/08/08 09:48:35  mbickel
//
//      speed up of dialog boxes in linux
//      fixed graphical errors in attack
//      fixed graphical error in ammo transfer
//      fixed reaction fire not allowing manual attack
//
//     Revision 1.23  2000/08/07 16:29:23  mbickel
//      orbiting units don't consume fuel any more
//      Fixed bug in attack formula; improved attack formula
//      Rewrote reactionfire
//
//     Revision 1.22  2000/08/05 13:38:48  mbickel
//      Rewrote height checking for moving units in and out of
//        transports / building
//
//     Revision 1.21  2000/08/04 15:11:31  mbickel
//      Moving transports costs movement for units inside
//      refuelled vehicles now have full movement in the same turn
//      terrain: negative attack / defensebonus allowed
//      new mapparameters that affect damaging and repairing of building
//
//     Revision 1.20  2000/08/01 13:50:53  mbickel
//      Chaning the height of airplanes is not affected by wind any more.
//      Fixed: Airplanes could ascend onto buildings
//
//     Revision 1.19  2000/07/31 19:16:50  mbickel
//      Improved handing of multiple directories
//      Fixed: wind direction not displayed when cycling through wind heights
//      Fixed: oil rig not working
//      Fixed: resources becomming visible when checking mining station status
//      Fixed: division by zero when moving unit without fuel consumption
//
//     Revision 1.18  2000/07/29 14:54:55  mbickel
//      plain text configuration file implemented
//
//     Revision 1.17  2000/07/28 10:15:39  mbickel
//      Fixed broken movement
//      Fixed graphical artefacts when moving some airplanes
//
//     Revision 1.16  2000/07/26 15:58:10  mbickel
//      Fixed: infinite loop when landing with an aircraft which is low on fuel
//      Fixed a bug in loadgame
//
//     Revision 1.15  2000/07/24 13:55:18  mbickel
//      Fixed crash when attacking unit is destroyed by attacked unit
//
//     Revision 1.14  2000/07/23 17:59:53  mbickel
//      various AI improvements
//      new terrain information window
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

#include <vector> // From STL
#include <algorithm>   // From STL


#include "unitctrl.h"
#include "controls.h"
#include "dialog.h"
#include "attack.h"
#include "stack.h"
#include "missions.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "viewcalculation.h"
#include "replay.h"
#include "dashboard.h"
#include "gameoptions.h"
#include "itemrepository.h"

PendingVehicleActions pendingVehicleActions;








VehicleAction :: VehicleAction ( VehicleActionType _actionType, PPendingVehicleActions _pva  )
{
   if ( _pva ) {
      if ( _pva->action )
         displaymessage ( " VehicleAction :: VehicleAction type %d / another action (type %d) is running !", 2 , _actionType, _pva->actionType );

      _pva->action = this;
      _pva->actionType = _actionType;
   }
   pva = _pva;
   actionType = _actionType;
}

void VehicleAction :: registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva )
{
   if ( _pva ) {
      if ( _pva->action )
         displaymessage ( " VehicleAction :: VehicleAction type %d / another action (type %d) is running !", 2 , _actionType, _pva->actionType );

      _pva->action = this;
      _pva->actionType = _actionType;
   }
   pva = _pva;
   actionType = _actionType;
}


VehicleAction :: ~VehicleAction (  )
{
   if ( pva ) {
      pva->action = NULL;
      pva->actionType = 0;
   }
}



VehicleMovement :: VehicleMovement ( MapDisplayInterface* md, PPendingVehicleActions _pva )
                 : BaseVehicleMovement ( vat_move, _pva )
{
   status = 0;
   mapDisplay = md;
   if ( pva )
      pva->move = this;
}

void VehicleMovement :: registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva )
{
   VehicleAction::registerPVA ( _actionType, _pva );
   if ( pva )
      pva->move = this;
}


VehicleMovement :: ~VehicleMovement ( )
{
   if ( pva )
      pva->move = NULL;
}

int VehicleMovement :: available ( pvehicle veh ) const
{
   if ( status == 0 )
     if ( veh )
       if ( getfield ( veh->xpos, veh->ypos )->unitHere ( veh ) )
          if ( veh->getMovement() >= minmalq && veh->reactionfire.getStatus() == Vehicle::ReactionFire::off )
             if ( terrainaccessible ( getfield ( veh->xpos, veh->ypos ), veh ) || actmap->getgameparameter( cgp_movefrominvalidfields) )
                return 1;

   return 0;
}


int VehicleMovement :: execute ( pvehicle veh, int x, int y, int step, int height, int noInterrupt )
{
   if ( step != status )
      return -1;

   if ( status == 0 ) {
        vehicle = veh ;
      if ( !vehicle ) {
         status = -101;
         return status;
      }
      newheight = height;
      if ( height == -1 )
         height = veh->height;

      if ( veh->maxMovement() )
         if ( veh->typ->movement[log2 ( height ) ] )
            initialMovement = veh->typ->movement[log2 ( height ) ] * veh->getMovement( false ) / veh->maxMovement();
         else
            initialMovement = veh->getMovement( false );
      else
         initialMovement = 0;

      searchstart ( veh->xpos, veh->ypos, newheight );
      if ( search.fieldnum <= 0 ) {
         status =  -107;
         return status;
      }

      status = 2;
      return status;
  } else
   if ( status == 2 ) {
      if ( height == -1)
         height = newheight;
      else
         displaymessage ( "VehicleMovement :: execute / unit heights not matching !", 2 );

      if ( !reachableFields.isMember ( x, y )) {
         status = -105;
         return status;
      }

      fieldReachableRek.run( x, y, vehicle, height, &path, initialMovement );

      status = 3;
      return status;
   } else
    if ( status == 3 ) {
       if ( !path.isMember ( x, y )) {
          status = -105;
          return status;
       }

       int x1 = vehicle->xpos;
       int y1 = vehicle->ypos;
 
      
/*       if ( newheight != -1 && newheight != vehicle->height )
          logtoreplayinfo ( rpl_changeheight2, x1, y1, x, y, vehicle->networkid, (int) vehicle->height, (int) newheight, noInterrupt );
       else */
          logtoreplayinfo ( rpl_move3, x1, y1, x, y, vehicle->networkid, newheight, noInterrupt );
          if ( newheight != -1 )
             vehicle->height = newheight;

       vehicle->setMovement( initialMovement, 0 );

       if ( mapDisplay )
          mapDisplay->startAction();
       int stat = moveunitxy( x, y, path, noInterrupt );
       if ( mapDisplay )
          mapDisplay->stopAction();

       if ( stat < 0 ) 
          status = stat;
       else
          status = 1000;

       return status;
      
    } else
       status = 0;
  return status;
}

int VehicleMovement :: getDistance ( int x, int y )
{
   if ( status < 2 )
      return maxint;

   IntFieldList path;
   fieldReachableRek.run( x, y, vehicle, newheight, &path, initialMovement );
   int dist = 0;
   int lastx = vehicle->xpos;
   int lasty = vehicle->ypos;
   for  ( int i = 1; i < path.getFieldNum(); i++ ) {
      int mm1, mm2;
      int x;
      int y;
      path.getFieldCoordinates ( i, &x, &y );

      calcmovemalus ( lastx, lasty, x, y, vehicle, -1, mm1, mm2  );
      lastx = x;
      lasty = y;
      dist += mm2;
   }
   return dist;
}


int sef_dirnum = 3;
static const  int          sef_directions[3]  = {0, 1, 5 };
static const  int          sef_searchorder[6]  = {0, 1, 2, 3, 4, 5 };


void VehicleMovement :: searchmove(int         x,
                             int         y,
                             int         direc,
                             int         streck,
                             int         fuelneeded)
{ 
   search.tiefe++;

   int ox = x; 
   int oy = y; 

   getnextfield( x, y, direc);

   if ((x == search.startx) && (y == search.starty)) 
      return;

   pfield fld = getfield( x, y ); 
   if ( !fld )
      return;

   int c = fieldaccessible ( fld, vehicle, search. height ); 

   if ( c == 0) 
      return;
   else { 
      int mm1;   // fuelconsumption
      int mm2;   // movementdist

      calcmovemalus(ox,oy,x,y,vehicle,direc, mm1, mm2, search.height);

      streck -= mm2;
      fuelneeded   += mm1;
   } 

   if (streck < 0) 
      return;

   if ( fuelneeded * vehicle->typ->fuelConsumption / maxmalq > vehicle->tank.fuel)
      return;

   if (search.mode == 1) {

      if ( fld->a.temp < streck + 1 ) {
         fld->a.temp = streck + 1;
         if ( c == 2 ) {
            reachableFields.addField ( x, y );
            search.fieldnum++;
         } else
            if ( c == 1 )
               reachableFieldsIndirect.addField ( x, y );
      }
      else
         return;

      for ( int b = 0; b < sef_dirnum; b++) {
         int c = sef_directions[b] + direc;

         if (c >= sidenum ) 
            c -= sidenum; 

         searchmove( x, y, c, streck, fuelneeded ); 
         search.tiefe--; 
      } 
   } 
   else { 
      searchmove( x, y, direc, streck, fuelneeded ); 
      search.tiefe--; 
   } 
} 



void VehicleMovement :: searchstart( int x1, int y1, int hgt )
{ 
   search.startx = x1; 
   search.starty = y1; 
   search.fieldnum = 0;
   search.height = hgt;
   search.tiefe = 0; 

   actmap->cleartemps(7); 
   initwindmovement( vehicle );

   search.mode = 1;
   search.strck = initialMovement;
   // search.strck = vehicle->getMovement( false );

   for ( int maindir = 0; maindir < sidenum; maindir++) {
      searchmove( search.startx, search.starty, sef_searchorder[maindir], search.strck, 0 );
      search.tiefe--;
   }

   actmap->cleartemps(7); 
} 


  static const trichtungen  directions[3][3]  = {{{5, 0, 4, 1, 3, 2 }, {0, 1, 5, 2, 4, 3 }, {1, 0, 2, 5, 3, 4 }},
                                                 {{5, 4, 0, 3, 1, 2 }, {0, 1, 2, 3, 4, 5 }, {1, 2, 0, 3, 5, 4 }}, 
                                                 {{4, 3, 5, 2, 0, 1 }, {3, 4, 2, 5, 1, 0 }, {2, 3, 1, 4, 0, 5 }}}; 


 
void   VehicleMovement :: FieldReachableRek :: move(int          x,
                                                    int          y,
                                                    int          direc,
                                                    int          streck,
                                                    int          fuel )
{ 
    int ox = x; 
    int oy = y; 
    strecke.tiefe++;
    if ( zielerreicht == 2 ) 
       return;

    if (streck > maxwegstrecke )
       return;

    getnextfield ( x, y, direc);

    if ((x < 0) || (y < 0) || (x >= actmap->xsize) || (y >= actmap->ysize)) 
       return;

    if (actmap->weather.wind[ getwindheightforunit ( vehicle ) ].speed && height >= chtieffliegend && height <= chhochfliegend) { // was: vehicle->height instead of height
       int ll = windbeeline(x,y,x2,y2);
       if (ll > maxwegstrecke - streck) 
          return; 
    } else {
       int ll = beeline(x2,y2,x,y);
       if (ll > maxwegstrecke - streck)
          return; 
    }

    pfield fld = getfield(x,y); 
    for ( int b = 1; b < strecke.tiefe ; b++)
       if ((strecke.x[b] == x) && (strecke.y[b] == y))
          return;

    int c = fieldaccessible( fld, vehicle, height); 

    if ( c == 0 ) 
       return;
    else { 
       int mm1;      // fuelusage
       int mm2;      // movementusage

       calcmovemalus(ox, oy, x, y, vehicle, direc, mm1, mm2, height);

       fuel += mm1;
       streck += mm2;
    } 
    if (streck > maxwegstrecke) 
       return;

    if (fuel * vehicle->typ->fuelConsumption / minmalq  > vehicle->tank.fuel)
       return;

    if ((fld->a.temp > 0) && (streck > fld->a.temp))
       return;

    if ((fld->a.temp > streck) || (fld->a.temp == 0)) 
        fld->a.temp = streck;

    if ( strecke.x.size() <= strecke.tiefe ) {
       strecke.x.push_back ( x );
       strecke.y.push_back ( y );
    } else {
       strecke.x[strecke.tiefe] = x;
       strecke.y[strecke.tiefe] = y;
    }

    if ((x == x2) && (y == y2)) { 
       distance = streck; 
       zielerreicht = 1;

       shortestway.tiefe = strecke.tiefe;
       shortestway.x = strecke.x;
       shortestway.y = strecke.y;
       if (actmap->weather.wind[ getwindheightforunit ( vehicle ) ].speed && height >= chtieffliegend && height <= chhochfliegend) { // was vehicle->height
          if ( streck == windbeeline(x1,y1,x2,y2))
             zielerreicht = 2;
       } else {
          if ( streck == beeline(x2,y2,x1,y1))
             zielerreicht = 2;
       }
    } 
    else { 
       int a,b;

       int dx = (2 * x2 + (y2 & 1)) - (2 * x + (y & 1)); 
       int dy = y2 - y; 

       if (dx < 0) 
          a = 0;
       else 
          if (dx == 0) 
             a = 1; 
          else 
             a = 2; 

       if (dy < 0) 
          b = 0; 
       else 
          if (dy == 0) 
             b = 1; 
          else 
             b = 2; 

       const trichtungen*  direc2 = &directions[b][a]; 
       for (int i = 0; i < 6; i++) {
          move( x, y, (*direc2)[i], streck, fuel );
          strecke.tiefe--;
          if ( zielerreicht == 2 ) 
             return;
       } 
    }
 }


void   VehicleMovement :: FieldReachableRek :: run(int          x22,
                                                   int          y22,
                                                   pvehicle     eht,
                                                   int          _height,
                                                   IntFieldList*   path,
                                                   int initialMovement )
               
/*  mode :  1 krzesten weg finden
            2 irgendeinen weg finden  */ 
{ 
   shortestway.tiefe = -1;
   actmap->cleartemps(7); 

   x2 = x22;
   y2 = y22;
   vehicle = eht;
   height = _height;

   x1 = vehicle->xpos; 
   y1 = vehicle->ypos; 
   strecke.x.push_back ( x1 );
   strecke.y.push_back ( y1 );

   maxwegstrecke = initialMovement;
   zielerreicht = 0; 
   distance = 0; 

   int a,b;
   int dx = (2 * x2 + (y2 & 1)) - (2 * x1 + (y1 & 1)); 
   int dy = y2 - y1; 
   if (dx < 0) 
      a = 0;
   else 
      if (dx == 0) 
         a = 1;
      else 
         a = 2;
   if (dy < 0) 
      b = 0;
   else 
      if (dy == 0) 
         b = 1;
      else 
         b = 2;

   const trichtungen*  direc = &directions[b][a];
   for ( int c = 0; c < sidenum; c++ ) { 
      strecke.tiefe = 0;
      move( x1, y1, (*direc)[c], 0, 0 );

      if ( zielerreicht==2 )
         break; 
   } 

   if ( zielerreicht >= 1) {
      path->addField ( eht->xpos, eht->ypos );
      for ( int d = 1; d <= shortestway.tiefe; d++) 
         path->addField ( shortestway.x[d], shortestway.y[d] );
   }
   actmap->cleartemps(7);
}




int  BaseVehicleMovement :: moveunitxy(int xt1, int yt1, IntFieldList& pathToMove, int noInterrupt )
{ 
   pfield fld = getfield( xt1, yt1 ); 

   int fieldnum;
   for ( fieldnum = 0; fieldnum < pathToMove.getFieldNum(); fieldnum++)
      if ( pathToMove.getField( fieldnum ) == fld )
         break; 
   
   
   if ( fieldaccessible(fld, vehicle, newheight) < 2) 
      return -106;

   pfield oldfield = getfield( vehicle->xpos, vehicle->ypos ); 

   if ( oldfield == fld )
      return 0;

   int x ;
   int y ;
   pathToMove.getFieldCoordinates ( 0, &x, &y );

   tsearchreactionfireingunits srfu;
//   treactionfirereplay rfr;

//   treactionfire* rf;
//   if ( runreplay.status > 0  )
//      rf = &rfr;
//   else
   treactionfire* rf = &srfu;

   rf->init( vehicle, &pathToMove );

   if ( oldfield->vehicle == vehicle) {
      vehicle->removeview();
      oldfield->vehicle = NULL; 
   } else { 
      if ( oldfield->vehicle ) { 
         int i = 0;
         while (oldfield->vehicle->loading[i] != vehicle) 
            i++; 
         oldfield->vehicle->loading[i] = NULL; 
      } 
      else 
         if ( oldfield->building ) {
            int i = 0; 
            while (oldfield->building->loading[i] != vehicle) 
               i++; 
            oldfield->building->loading[i] = NULL;
            oldfield->building->regroupUnits();
         }
   }

   SoundLoopManager slm ( SoundList::getInstance().getSound( SoundList::moving, vehicle->typ->movemalustyp ), false );

   int i = 0;
   int cancelmovement = 0;

   int movedist = 0;
   int fueldist = 0;
   int networkID = vehicle->networkid;

   while ( (x != xt1 || y != yt1) && vehicle && cancelmovement!=1 ) {

      if ( cancelmovement > 1 )
         cancelmovement--;

      int x2, y2;
      pathToMove.getFieldCoordinates ( i+1, &x2, &y2 );

      int mm1,mm2;
      calcmovemalus( x, y, x2, y2, vehicle, -1 , mm1, mm2);
      movedist += mm2;
      fueldist += mm1;

      pfield fld1 = getfield ( x,  y  );
      pfield fld2 = getfield ( x2, y2 );



      if ( mapDisplay ) {
         if ( x2 == xt1 && y2 == yt1 ) // the unit will reach its destination
            slm.fadeOut ( CGameOptions::Instance()->movespeed * 10 );
         mapDisplay->displayMovingUnit ( x, y, x2, y2, vehicle, vehicle->height, newheight, i, fieldnum, &slm );
      }

      int dir;
      if (vehicle->functions & ( cffahrspur | cficebreaker ))  {
        dir = getdirection( x, y, x2, y2 );
      
        if ( vehicle->functions & cffahrspur )
           if ( fahrspurobject )
              if ( (fld1->bdt & getTerrainBitType(cbfahrspur)).any() )
                 // if ( ! (actmap->objectcrc   &&   !actmap->objectcrc->speedcrccheck->checkobj2 ( fahrspurobject, 2 )))
                    fld1 -> addobject ( fahrspurobject, 1 << dir );

        if ( vehicle->functions & cficebreaker )
           if ( eisbrecherobject )
                 if (   (fld1->bdt & getTerrainBitType(cbicebreaking) ).any()
                      || fld1->checkforobject ( eisbrecherobject ) ) {
                    fld1 -> addobject ( eisbrecherobject, 1 << dir );
                    fld1->checkforobject ( eisbrecherobject )->time = actmap->time.turn();
                 }

      } 

      i++;
      pathToMove.getFieldCoordinates ( i, &x, &y );

      pfield field3 = fld2; 
      if (vehicle->functions & cffahrspur) {
         if ( fahrspurobject )
            if ( (field3->bdt & getTerrainBitType(cbfahrspur)).any() )
               // if ( ! (actmap->objectcrc   &&   !actmap->objectcrc->speedcrccheck->checkobj2 ( fahrspurobject, 2 )))
                  if (dir >= sidenum/2) 
                    getfield ( x, y ) -> addobject ( fahrspurobject, 1 << (dir - sidenum/2));
                  else 
                    getfield ( x, y ) -> addobject ( fahrspurobject, 1 << (dir + sidenum/2));
      } 
      if ( vehicle->functions & cficebreaker )
         if ( eisbrecherobject )
            if ( (field3->bdt & getTerrainBitType(cbicebreaking) ).any() || field3->checkforobject ( eisbrecherobject ) ) {
               if (dir >= sidenum/2)
                 getfield ( x, y ) -> addobject ( eisbrecherobject, 1 << (dir - sidenum/2));
               else
                 getfield ( x, y ) -> addobject ( eisbrecherobject, 1 << (dir + sidenum/2));

                 field3->checkforobject ( eisbrecherobject )->time = actmap->time.turn();
            }

      if ( vehicle ) {
         vehicle->xpos = x;
         vehicle->ypos = y;
         vehicle->addview();
      }

      int fieldschanged = 0;
      if ( actmap->playerView >= 0 )
         fieldschanged = evaluateviewcalculation ( actmap, 1 << actmap->playerView );
      else
         evaluateviewcalculation ( actmap, 0);

      if ( rf->checkfield ( x, y, vehicle, mapDisplay )) {
         cancelmovement = 1;
         attackedByReactionFire = true;
         vehicle = actmap->getUnit ( networkID );
      }

      if ( mapDisplay ) {
         if ( !vehicle )
            mapDisplay->deleteVehicle( vehicle );

         if ( fieldschanged > 0 )
            mapDisplay->displayMap();
     }

      /*
      if ( vehicle )
         if ( vehicleplattfahrbar(vehicle,field3) )
            bulldoze_trooper.fight( &vehicle, &field3->vehicle);
      */

      if ( vehicle ) {
         vehicle->removeview();
         if ( field3->mineattacks ( vehicle )) {
            tmineattacksunit battle ( field3, -1, vehicle );

            if ( mapDisplay && fieldvisiblenow ( getfield ( x, y ), actmap->playerView) || field3->mineowner() == actmap->playerView )
               battle.calcdisplay ();
            else
               battle.calc();

            battle.setresult ();
            dashboard.x = 0xffff;
            cancelmovement = 1;
        }
      }
      if ( cancelmovement == 1 )
         if ( field3->vehicle || field3->building )
            cancelmovement++;

      if ( vehicle )
         if ( (newheight != -1 && newheight != vehicle->height && cancelmovement) || ( noInterrupt > 0 ))
            cancelmovement = 0;



      if ( vehicle ) {
         npush ( field3->vehicle );
         field3->vehicle = vehicle;
         if ( field3->connection & cconnection_areaentered_anyunit )
            releaseevent ( vehicle, NULL, cconnection_areaentered_anyunit );

         if ((field3->connection & cconnection_areaentered_specificunit ) && ( vehicle->connection & cconnection_areaentered_specificunit ))
            releaseevent ( vehicle, NULL, cconnection_areaentered_specificunit );
         npop ( field3->vehicle );
      }
   }

   fld = getfield ( x, y );

   if ( vehicle ) {

      vehicle->tank.fuel -= fueldist * vehicle->typ->fuelConsumption / maxmalq;
      if ( vehicle->tank.fuel < 0 )
         vehicle->tank.fuel = 0;


      if ( !fld->vehicle && !fld->building )
         vehicle->decreaseMovement ( movedist );
      else {
         vehicle->setMovement ( 0 );
         vehicle->attacked = true;
         if (vehicle->height == chtieffliegend)
            vehicle->height = chfahrend;
      }

      vehicle->setnewposition ( x, y );
      if ( newheight != -1 && vehicle->typ->height & newheight)
         vehicle->setNewHeight ( newheight );

      if ( vehicle && rf->checkfield ( x, y, vehicle, mapDisplay )) {
         attackedByReactionFire = true;
         vehicle = actmap->getUnit ( networkID );
      }

      if ( vehicle ) {

         if ((fld->vehicle == NULL) && (fld->building == NULL)) {
            fld->vehicle = vehicle;
            vehicle->addview();
         } else {
            if ( fld->vehicle  &&  fld->vehicle->typ->loadcapacity ) {
               i = 0;
               while ((fld->vehicle->loading[i] != NULL) && (i < 31))
                 i++;
               fld->vehicle->loading[i] = vehicle;
            }
            else
               if ( fld->building ) {
                  i = 0;
                  while ( fld->building->loading[i]  && (i < 31))
                     i++;
                  fld->building->loading[i] = vehicle;
                  if (fld->building->color != vehicle->color )
                     fld->building->convert( vehicle->color / 8 );

               }
         }

         dashboard.x = 0xffff;
      }
   }


   int fieldschanged;
   if ( actmap->playerView >= 0 )
      fieldschanged = evaluateviewcalculation ( actmap, 1 << actmap->playerView );
   else
      fieldschanged = evaluateviewcalculation ( actmap, 0 );

   if ( mapDisplay ) {
      mapDisplay->resetMovement();
      if ( fieldschanged > 0 )
         mapDisplay->displayMap();
      else
         mapDisplay->displayPosition ( x, y );
   }
   return 0;
}











ChangeVehicleHeight :: ChangeVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva, VehicleActionType vat )
                 : BaseVehicleMovement ( vat, _pva )
{
   status = 0;
   mapDisplay = md;
   vmove = NULL;
}

ChangeVehicleHeight :: ~ChangeVehicleHeight ()
{
/*
   if ( vmove ) {
      delete vmove;
      vmove = NULL;
   }
*/
}

int ChangeVehicleHeight :: execute_withmove ( int allFields )
{
   if ( !vehicle ) 
      return -unspecified_error;

   int steigung = vehicle->typ->steigung;

   if ( vehicle->typ->height & (chtieffliegend | chfliegend | chhochfliegend) ) {
      if ( vehicle->tank.fuel < steigung * maxmalq )
         return -115;

      int w;
      if ( newheight > vehicle->height ) {
         w = (vehicle->height << 1);
         if ( w != chtieffliegend) {
            if (vehicle->getMovement() < steigung * maxmalq + air_heightincmovedecrease)
               return -110;

         }
         else {
            if (vehicle->getMovement() < steigung * maxmalq )
               return -110;
         }

      }
      else {
         w = (vehicle->height >> 1);
         if (vehicle->getMovement() < steigung * maxmalq)
            return -111;

      }

      if ((w & vehicle->typ->height) == 0)
         return -112;

      newheight = w;


      return moveheight( allFields );
   } else {
      displaymessage("changing height with movement of non-airplanes has not been defined yet !",2 );
      return -1;
   }

}


int ChangeVehicleHeight :: moveHeightMoveCost( pvehicle vehicle, const MapCoordinate3D& pos, int newheight, int direc, int& xstop, int& ystop )
{
   int ok = true;

   int& x = xstop;
   int& y = ystop;

   x = pos.x;
   y = pos.y;

   int dist = 0;

   while ( dist < vehicle->typ->steigung * minmalq  && ok) {
      int ox = x;
      int oy = y;
      getnextfield( x, y, direc );
      if ((x < 0) || (y < 0) || (x >= actmap->xsize) || (y >= actmap->ysize))
         ok = false;
      else {
         npush ( vehicle->height );
         vehicle->height = pos.z;

         int fuelcost, movecost;

         calcmovemalus(ox,oy,x,y,vehicle,direc, fuelcost, movecost);
         npop ( vehicle->height );

         /*
         dist += minmalq ;// - windmovement[direc];
         mx -= minmalq ; //- windmovement[direc];

         movecost += windmovement[direc]; // compensating for the wind which.
         */
         dist += movecost;

         pfield fld = getfield(x,y);

         if ( fieldaccessible(fld, vehicle, pos.z ) < 1)
            ok = false;

         if ( fieldaccessible(fld, vehicle, newheight ) < 1)
            ok = false;

         // if ( dist < vehicle->typ->steigung * minmalq )
            if ( fld->building )
               ok = false;
      }
   }

   if ( ok && fieldaccessible( getfield(x,y), vehicle, newheight ) < 2)
      ok = false;
/*
   if ( ok )
      if ( fld->building || fld->vehicle )
         ok = false;
*/

   if ( newheight > pos.z && pos.z != chfahrend )
      dist += air_heightincmovedecrease;

   if ( ok )
      return dist;
   else
      return -1;
}

int ChangeVehicleHeight :: moveheight( int allFields )
{
   initwindmovement( vehicle );

   FieldList<StartPosition> startpos;
   if ( allFields == 1 ) {
      if ( !vmove ) {
         StartPosition sp;
         sp.x = vehicle->xpos;
         sp.y = vehicle->ypos;
         sp.dist = 0;
         startpos.addField ( vehicle->xpos, vehicle->ypos, sp );

         vmove = new VehicleMovement ( NULL, NULL );
         if ( vmove->available ( vehicle ))
            vmove->execute ( vehicle, -1, -1, 0, -1, -1 );
      }

      for ( int i = 0; i < vmove->reachableFields.getFieldNum(); i++ ) {
          StartPosition sp;
          vmove->reachableFields.getFieldCoordinates( i, &sp.x, &sp.y );
          if ( !getfield( sp.x, sp.y)->vehicle && !getfield( sp.x, sp.y)->building ) {
             sp.dist = vmove->getDistance ( sp.x, sp.y);
             startpos.addField ( sp.x, sp.y, sp );
          }
      }
   } else {
      StartPosition sp;
      sp.x = vehicle->xpos;
      sp.y = vehicle->ypos;
      sp.dist = 0;
      startpos.addField ( vehicle->xpos, vehicle->ypos, sp );
   }

   int ok2 = false;

   for ( int i = 0; i < startpos.getFieldNum(); i++ ) {
      StartPosition& sp = startpos.getData ( i );
      int xx;
      int yy;
      startpos.getFieldCoordinates ( i, &xx, &yy );
      for ( int direc = 0; direc < sidenum; direc++) {

         int x, y;
         int dist = moveHeightMoveCost ( vehicle, MapCoordinate3D( xx, yy, vehicle->height), newheight, direc, x, y );

         int mx = vehicle->getMovement() - sp.dist - dist;
         if ( mx < 0 )
            dist = -1;

         if ( dist >= 0 ) {
            if ( reachableFields.isMember ( x, y )) {
               StartPosition& sp = reachableFields.getData ( x, y );
               if ( sp.dist < mx ) {
                  sp.x = xx;
                  sp.y = yy;
                  sp.dist = mx;
               }
            } else {
               StartPosition sp;
               sp.x = xx;
               sp.y = yy;
               sp.dist = mx;
               reachableFields.addField ( x, y, sp );
            }

            ok2 = true;
         }
      }
   }


   if ( ok2 )
      return 0;
   else
      return -116;
}


int ChangeVehicleHeight :: moveunitxy ( int xt1, int yt1, IntFieldList& pathToMove )
{
   int nwid = vehicle->networkid;
   int oldheight = vehicle->height;
   int res = BaseVehicleMovement :: moveunitxy ( xt1, yt1, pathToMove );
   if ( res < 0 || res == 1000 )
      return res;

   if ( !actmap->getUnit ( nwid ))
      return 1000;

   if ( oldheight < newheight ) {
      if ( oldheight == chfahrend ) {
         vehicle->tank.fuel -= vehicle->typ->fuelConsumption * vehicle->typ->steigung;
         if ( vehicle->tank.fuel < 0 )
            vehicle->tank.fuel = 0;

         vehicle->setMovement ( airplanemoveafterstart );

         vehicle->attacked = 1;
         vehicle->height = chtieffliegend;
      } else {
         vehicle->decreaseMovement ( air_heightincmovedecrease );

         vehicle->tank.fuel -= vehicle->typ->fuelConsumption * air_heightincmovedecrease / maxmalq;
         if ( vehicle->tank.fuel < 0 )
            vehicle->tank.fuel = 0;

         int newmovement = vehicle->typ->movement[log2(newheight)] * vehicle->getMovement( false ) / vehicle->typ->movement[log2(vehicle->height)];
         vehicle->setMovement ( newmovement );

         vehicle->height = newheight;
      }
   } else {
      if ( oldheight == chtieffliegend ) {
         // vehicle->fuel -= vehicle->typ->fuelconsumption * vehicle->typ->steigung;
         // if ( vehicle->fuel < 0 )
         //    vehicle->fuel = 0;

         if ( vehicle->typ->steigung * minmalq <= airplanemoveafterlanding )
            vehicle->setMovement ( 0 );
         else
            vehicle->setMovement ( airplanemoveafterlanding );

         vehicle->attacked = 1;
         vehicle->height = chfahrend;
      } else {
         vehicle->decreaseMovement ( air_heightdecmovedecrease );

         vehicle->tank.fuel -= vehicle->typ->fuelConsumption * air_heightdecmovedecrease / maxmalq;
         if ( vehicle->tank.fuel < 0 )
            vehicle->tank.fuel = 0;

         int newmovement = vehicle->typ->movement[log2(newheight)] * vehicle->getMovement( false ) / vehicle->typ->movement[log2(vehicle->height)];
         vehicle->setMovement ( newmovement );

         vehicle->height = newheight;
      }
   }
   if ( getfield ( vehicle->xpos, vehicle->ypos)->vehicle != vehicle ) {
      vehicle->attacked = 1;
      vehicle->setMovement ( 0 );
   }

   return 0;
}


int ChangeVehicleHeight :: verticalHeightChangeMoveCost ( pvehicle vehicle, const MapCoordinate3D pos, int newheight )
{
   pfield fld = getfield( pos.x, pos.y );

   int oldheight = pos.z;

   if ( pos.z & (chtieffliegend | chfliegend | chhochfliegend)) {
      if ( (newheight < oldheight) && (newheight == chfahrend) ) {
         if ( !terrainaccessible ( fld, vehicle, newheight ))
            return -109;

         return helicopter_landing_move_cost + air_heightdecmovedecrease;

      } else
         if ( newheight < oldheight ) {
            return air_heightdecmovedecrease;
         } else
            return air_heightincmovedecrease;

   } else {   /*  not an aircraft */

      int md;

      if ( (newheight == chfahrend && oldheight == chschwimmend) || (newheight == chschwimmend && oldheight == chfahrend )) {
         md = 0;
      } else {
         if ( newheight > oldheight )
            md = sub_heightincmovedecrease;
         else
            md = sub_heightdecmovedecrease;
      }
      
      int res = terrainaccessible2 ( fld, vehicle, newheight );
      if ( res <= 0 ) {
         if ( newheight > vehicle->height )
            return -108;
         else {
            if ( res == -1 )
               return -113;
            else
               if ( res == -2 )
                  return -114;
               else
                  return -109;
         }
      }

      return md;

   }
//   return -unspecified_error;
}


int ChangeVehicleHeight :: verticalHeightChange ( void )
{
   if ( !vehicle ) 
      return -unspecified_error;

//   pfield fld = getfield(vehicle->xpos,vehicle->ypos);

   int oldheight = vehicle->height;

   int moveCost = verticalHeightChangeMoveCost ( vehicle, MapCoordinate3D( vehicle->xpos, vehicle->ypos, vehicle->height ), newheight );
   if ( moveCost < 0 )
      return moveCost;

   if (vehicle->typ->height & (chtieffliegend | chfliegend | chhochfliegend)) {
      if ( (newheight < oldheight) && (newheight == chfahrend) ) { 

         int newmovement = vehicle->typ->movement[log2(newheight)] * (vehicle->getMovement( false ) - moveCost ) / vehicle->typ->movement[log2(vehicle->height)];
         if ( newmovement < 0)                      
            return -111;

         int fuelcost = vehicle->typ->fuelConsumption * moveCost / maxmalq;
         if ( fuelcost > vehicle->tank.fuel )
            return -115;


         vehicle->setMovement ( newmovement ); 
         if ( !helicopter_attack_after_descent )
            vehicle->attacked = true;
         vehicle->tank.fuel -= fuelcost;

      } else
      if ( newheight < oldheight ) { 
         int newmovement = vehicle->typ->movement[log2(newheight)] * vehicle->getMovement( false ) / vehicle->typ->movement[log2(vehicle->height)];
         if ( newmovement < moveCost )
            return -111;

         int fuelcost = vehicle->typ->fuelConsumption * moveCost / maxmalq;
         if ( fuelcost > vehicle->tank.fuel )
            return -115;


         vehicle->setMovement ( newmovement - moveCost );
         vehicle->tank.fuel -= fuelcost;
      } else
      if (( newheight > oldheight ) && (newheight > chtieffliegend)) {
         int newmovement = vehicle->typ->movement[log2(newheight)] * vehicle->getMovement( false ) / vehicle->typ->movement[log2(vehicle->height)];
         if (newmovement < moveCost)
            return -110;
         int fuelcost = vehicle->typ->fuelConsumption * moveCost / maxmalq;
         if ( fuelcost > vehicle->tank.fuel )
            return -115;

         vehicle->setMovement ( newmovement - moveCost );
         vehicle->tank.fuel -= fuelcost;

         if ((newheight == chtieffliegend) && !helicopter_attack_after_ascent ) 
            vehicle->attacked = 1; 
      } else
         if ( newheight > oldheight  &&  newheight == chtieffliegend ) {
            int newmovement = vehicle->typ->movement[log2(newheight)] * vehicle->getMovement( false ) / vehicle->typ->movement[log2(vehicle->height)];
            if ( newmovement < moveCost )
               return -110;

            if ( !helicopter_attack_after_ascent )
               vehicle->attacked = 1;

            vehicle->setMovement ( newmovement - moveCost);
         }

      logtoreplayinfo ( rpl_changeheight, (int) vehicle->xpos, (int) vehicle->ypos, 
                                          (int) vehicle->xpos, (int) vehicle->ypos, vehicle->networkid, (int) vehicle->height, (int) newheight );
      vehicle->height = newheight; 
   } 
   else {   /*  not an aircraft */ 

      if ( newheight > oldheight ) {
         if ( vehicle->getMovement() < moveCost )
            return -110;
      } 
      else { 
         if ( vehicle->getMovement() < moveCost )
            return -111;
      } 

      int newmovement = vehicle->typ->movement[log2(newheight)] * (vehicle->getMovement( false ) - moveCost ) / vehicle->typ->movement[log2(vehicle->height)];

      int fuelcost = vehicle->typ->fuelConsumption * moveCost / maxmalq;
      if ( fuelcost > vehicle->tank.fuel )
         return -115;

      vehicle->setMovement ( newmovement );
      vehicle->tank.fuel -= fuelcost;

      logtoreplayinfo ( rpl_changeheight, (int) vehicle->xpos, (int) vehicle->ypos,
                                          (int) vehicle->xpos, (int) vehicle->ypos, vehicle->networkid, (int) vehicle->height, (int) newheight );

      vehicle->height = newheight;

   }
   return 0;
}


int ChangeVehicleHeight :: execute ( pvehicle veh, int x, int y, int step, int height, int allFields )
{
   if ( step != status )
      return -1;

   if ( status == 0 ) {
      vehicle = veh ;
      if ( !vehicle ) {
         status = -101;
         return status;
      }
      newheight = height;

      if ( vehicle->typ->steigung ) {
         int res = execute_withmove ( allFields );
         if ( res < 0 ) {
            status = res;
            return status;
         }
         if ( reachableFields.getFieldNum() <= 0 ) {
            status =  -107;
            return status;
         }

         status = 2;
      } else {
         status = verticalHeightChange ();
         if ( status >= 0 ) {
            status = 1000;
            if ( mapDisplay )
               mapDisplay->displayPosition( vehicle->xpos, vehicle->ypos );
         }
      }
      return status;
  } else
   if ( status == 2 ) {
      if ( height == -1)
         height = newheight;
      else
         if ( height != newheight )
            displaymessage ( "ChangeVehicleHeight :: execute / unit heights not matching !", 2 );

      if ( !reachableFields.isMember ( x, y )) {
         status = -105;
         return status;
      }

      int networkID = vehicle->networkid;
      StartPosition& sp = reachableFields.getData ( x, y );
      if ( sp.x != vehicle->xpos || sp.y != vehicle->ypos ) {
         vmove->registerMapDisplay ( mapDisplay );

         int stat = vmove->execute( NULL, sp.x, sp.y, 2, -1, -1 );
         if ( stat != 3 )
            displaymessage ( "ChangeVehicleHeight :: execute / vmove step 2 failed !", 2 );

         stat = vmove->execute ( NULL, sp.x, sp.y, 3, -1, 0 );
         if ( stat != 1000 )
            displaymessage ( "ChangeVehicleHeight :: execute / vmove step 3 failed !", 2 );
      }

      // cancel if the unit was attecked; unless overridden param3 (noInterrupt = allFields)
      if ( actmap->getUnit ( networkID ) && (!vmove || (!vmove->attackedByReactionFire || allFields)) ) {
         modechangePosition = MapCoordinate ( sp.x, sp.y );

         fieldReachableRek.run( x, y, vehicle, height, &path, vehicle->getMovement( false ) );

         status = 3;
      } else
         status = 1000;

      return status;
   } else
    if ( status == 3 ) {
       if ( !path.isMember ( x, y )) {
          status = -105;
          return status;
       }

       if ( vehicle->xpos != modechangePosition.x || vehicle->ypos != modechangePosition.y ) {
          status = -117;
          return status;
       }

       int x1 = vehicle->xpos;
       int y1 = vehicle->ypos;

       logtoreplayinfo ( rpl_changeheight, x1, y1, x, y, vehicle->networkid, (int) vehicle->height, (int) newheight );

       if ( mapDisplay )
          mapDisplay->startAction();

       npush ( newheight );
       newheight = vehicle->height;
       int nwid = vehicle->networkid;
       StartPosition& sp = reachableFields.getData ( x, y );
       int stat = BaseVehicleMovement :: moveunitxy( sp.x, sp.y, path );
       npop ( newheight );

       if ( actmap->getUnit ( nwid ))
          stat = moveunitxy ( x, y, path );

       if ( mapDisplay ) {
          mapDisplay->displayMap();
          mapDisplay->stopAction();
       }

       if ( stat < 0 )
          status = stat;
       else
          status = 1000;

       return status;

    } else
       status = 0;
  return status;
}


pair<int,MapCoordinate3D> ChangeVehicleHeight :: getMoveCost ( pvehicle veh, const MapCoordinate3D& pos, int direc, int heightdir )
{
   if ( heightdir == 1 )
      return IncreaseVehicleHeight::getMoveCost ( veh, pos, direc );
   else
      return DecreaseVehicleHeight::getMoveCost ( veh, pos, direc );
}


IncreaseVehicleHeight :: IncreaseVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva )
                 : ChangeVehicleHeight ( md, _pva, vat_ascent )
{
   if ( pva )
      pva->ascent = this;
}

void IncreaseVehicleHeight :: registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva )
{
   VehicleAction::registerPVA ( _actionType, _pva );
   if ( pva )
      pva->ascent = this;
}


int IncreaseVehicleHeight :: available ( pvehicle veh ) const
{
   if ( veh ) 
      if ( veh->getMovement() )
         if ( veh->reactionfire.getStatus() == Vehicle::ReactionFire::off )
            if (veh->height < 128)
               if ((veh->height << 1) & veh->typ->height )
                  if ( veh->typ->steigung ) {
                     return 1;
                  } else
                     if ( getfield ( veh->xpos, veh->ypos )->vehicle != veh )
                        return 0;
                     else
                        return 2;
   return 0;
}

pair<int,MapCoordinate3D> IncreaseVehicleHeight :: getMoveCost ( pvehicle veh, const MapCoordinate3D& pos, int direc )
{
   int dist = -1;
   MapCoordinate3D dest = pos;
   if ( pos.z < 128) {
      dest.z <<= 1;
      if ( dest.z & veh->typ->height )
         if ( veh->typ->steigung ) {
            dist = moveHeightMoveCost ( veh, pos, dest.z, direc, dest.x, dest.y  );
         } else
            dist = verticalHeightChangeMoveCost ( veh, pos, dest.z );
   }
   return make_pair( dist, dest );
}


IncreaseVehicleHeight :: ~IncreaseVehicleHeight ( )
{
   if ( pva )
      pva->ascent = NULL;
}




DecreaseVehicleHeight :: DecreaseVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva )
                 : ChangeVehicleHeight ( md, _pva, vat_descent )
{
   if ( pva )
      pva->descent = this;
}

void DecreaseVehicleHeight :: registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva )
{
   VehicleAction::registerPVA ( _actionType, _pva );
   if ( pva )
      pva->descent = this;
}


int DecreaseVehicleHeight :: available ( pvehicle veh ) const
{
   if ( veh )
      if ( veh->getMovement() )
         if ( veh->reactionfire.getStatus() == Vehicle::ReactionFire::off )
            if (veh->height > 1)
               if ((veh->height >> 1) & veh->typ->height )
                  if ( veh->typ->steigung )
                     return 1;
                  else
                     if ( getfield ( veh->xpos, veh->ypos )->vehicle != veh )
                        return 0;
                     else
                        return 2;
   return 0;
}

pair<int,MapCoordinate3D> DecreaseVehicleHeight :: getMoveCost ( pvehicle veh, const MapCoordinate3D& pos, int direc )
{
   int dist = -1;
   MapCoordinate3D dest = pos;
   if ( pos.z ) {
       dest.z >>= 1;
       if ( dest.z & veh->typ->height )
          if ( veh->typ->steigung ) {
             dist = moveHeightMoveCost ( veh, pos, dest.z, direc, dest.x, dest.y  );
          } else
             dist = verticalHeightChangeMoveCost ( veh, pos, dest.z );
   }

   return make_pair( dist, dest );
}

DecreaseVehicleHeight :: ~DecreaseVehicleHeight ( )
{
   if ( pva )
      pva->descent = NULL;
}



PendingVehicleActions :: PendingVehicleActions ( void )
{
   move = NULL;
   action = NULL;
}

PendingVehicleActions :: ~PendingVehicleActions ( )
{
   if ( action ) 
      delete action;
}









VehicleAttack :: VehicleAttack ( MapDisplayInterface* md, PPendingVehicleActions _pva )
               : VehicleAction ( vat_attack, _pva ), search ( actmap )
{
   status = 0;
   mapDisplay = md;
   if ( pva )
      pva->attack = this;
}


int VehicleAttack :: available ( pvehicle eht ) const
{
   if (eht != NULL) 
      if (eht->attacked == false)
         if ( eht->weapexist() ) 
            if ( eht->reactionfire.getStatus() == Vehicle::ReactionFire::off ) {
               if (eht->typ->wait == false  ||  !eht->hasMoved() )
                  return 1;
            } else {
               return 1;
            }

   return 0;
}


int VehicleAttack :: execute ( pvehicle veh, int x, int y, int step, int _kamikaze, int weapnum )
{
   if ( step != status )
      return -1;

   if ( status == 0 ) {
      vehicle = veh ;
      if ( !vehicle ) {
         status = -101;
         return status;
      }

      int weaponCount = 0;
      int shootableWeaponCount = 0;
      for ( int w = 0; w < vehicle->typ->weapons.count; w++ )
         if ( vehicle->typ->weapons.weapon[w].shootable() ) {
              weaponCount++;
              if ( vehicle->typ->weapons.weapon[w].sourceheight & vehicle->height )
                 shootableWeaponCount++;
         }

      if ( weaponCount == 0 )
         return -214;

      if ( shootableWeaponCount == 0 )
         return -213;


      kamikaze = _kamikaze;

      search.init( veh, kamikaze, this ); 
      int res = search.run();
      if ( res < 0 ) {
         status = res;
         return status;
      }

      if ( search.anzahlgegner <= 0 ) {
         status =  -206;
         return status;
      }

      status = 2;
      return status;
  } else
  if ( status == 2 ) {
      pattackweap atw = NULL;
      if ( attackableVehicles.isMember ( x, y ))
         atw = &attackableVehicles.getData ( x, y );
      else
         if ( attackableObjects.isMember ( x, y ))
            atw = &attackableObjects.getData ( x, y );
         else
            if ( attackableBuildings.isMember ( x, y ))
               atw = &attackableBuildings.getData ( x, y );

      tfight* battle = NULL;
      switch ( atw->target ) {
         case AttackWeap::vehicle: battle = new tunitattacksunit ( vehicle, getfield(x,y)->vehicle, 1, weapnum );
            break;
         case AttackWeap::building: battle = new tunitattacksbuilding ( vehicle, x, y , weapnum );
            break;
         case AttackWeap::object: battle = new tunitattacksobject ( vehicle, x, y, weapnum );
            break;
         default : status = -1;
                   return status;
      } /* endswitch */

      int ad1 = battle->av.damage;
      int dd1 = battle->dv.damage;

      int xp1 = vehicle->xpos;
      int yp1 = vehicle->ypos;

      int shown;
      if ( mapDisplay && fieldvisiblenow ( getfield ( x, y ), actmap->playerView) ) {
         mapDisplay->displayActionCursor ( vehicle->xpos, vehicle->ypos, x, y );
         battle->calcdisplay ();
         mapDisplay->removeActionCursor ( );
         shown = 1;
      } else {
         battle->calc();
         shown = 0;
      }

      int ad2 = battle->av.damage;
      int dd2 = battle->dv.damage;
      battle->setresult ();

      logtoreplayinfo ( rpl_attack, xp1, yp1, x, y, ad1, ad2, dd1, dd2, weapnum );

      computeview( actmap );

      if ( mapDisplay && shown )
         mapDisplay->displayMap();


      status = 1000;
  } 
  return status;
}



void     VehicleAttack :: tsearchattackablevehicles :: init( const pvehicle eht, int _kamikaze, VehicleAttack* _va )
{ 
   angreifer = eht; 
   kamikaze = _kamikaze; 
   va = _va;
}



void     VehicleAttack :: tsearchattackablevehicles::testfield( const MapCoordinate& mc )
{ 
   if ( fieldvisiblenow( gamemap->getField(mc)) ) {
      if ( !kamikaze ) {
         pattackweap atw = attackpossible( angreifer, mc.x, mc.y );
         if (atw->count > 0) { 
            switch ( atw->target ) {
               case AttackWeap::vehicle:  va->attackableVehicles.addField  ( mc, *atw );
                  break;                                    
               case AttackWeap::building: va->attackableBuildings.addField ( mc, *atw );
                  break;
               case AttackWeap::object:   va->attackableObjects.addField   ( mc, *atw );
                  break;
            } /* endswitch */
            anzahlgegner++;
         } 
         delete atw;
      } else {
          pfield fld = gamemap->getField(mc);
          if (fieldvisiblenow( fld )) {
             pvehicle eht = fld->vehicle;
             if (eht != NULL) 
                if (((angreifer->height >= chtieffliegend) && (eht->height <= angreifer->height) && (eht->height >= chschwimmend)) 
                  || ((angreifer->height == chfahrend) && (eht->height == chfahrend)) 
                  || ((angreifer->height == chschwimmend) && (eht->height == chschwimmend))
                  || ((angreifer->height == chgetaucht) && (eht->height >=  chgetaucht) && (eht->height <= chschwimmend))) {
                   fld->a.temp = dist;
                   anzahlgegner++; 
                } 
          } 
      }
   } 
} 


int      VehicleAttack :: tsearchattackablevehicles::run( void )
{ 
   anzahlgegner = 0; 

   if (angreifer == NULL) 
      return -201;

   moveparams.movesx = angreifer->xpos;  // this is currently still needed for wepselguihost
   moveparams.movesy = angreifer->ypos;
   if (fieldvisiblenow(getfield(angreifer->xpos,angreifer->ypos)) == false)
      return -1;

   if (angreifer->attacked) 
      return -202;
   

   if (angreifer->typ->weapons.count == 0)
      return -204;

   if ( angreifer->typ->wait && angreifer->hasMoved() )
         return -215;


   int d = 0;
   int maxdist = 0;
   int mindist = 20000;
   for ( int a = 0; a < angreifer->typ->weapons.count; a++)
      if (angreifer->ammo[a] > 0) {
         d++;
         maxdist = max( maxdist, angreifer->typ->weapons.weapon[a].maxdistance / maxmalq );
         mindist = min ( mindist, (angreifer->typ->weapons.weapon[a].mindistance + maxmalq - 1) / maxmalq);
      }


   if (d == 0)
      return -204;

   initsearch( angreifer->getPosition(), maxdist, mindist );
   startsearch();

   return 0;
}

void VehicleAttack :: registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva )
{
   VehicleAction::registerPVA ( _actionType, _pva );
   if ( pva )
      pva->attack = this;
}


VehicleAttack :: ~VehicleAttack ( )
{
   if ( pva )
      pva->attack = NULL;
}






VehicleService :: VehicleService ( MapDisplayInterface* md, PPendingVehicleActions _pva )
               : VehicleAction ( vat_service, _pva ), fieldSearch ( *this, actmap )
{
   building = NULL;
   vehicle = NULL;
   status = 0;
   mapDisplay = md;
   guimode = 0;
   if ( pva )
      pva->service = this;
}


int VehicleService :: available ( pvehicle veh ) const
{
   int av = 0;
   if ( veh && !veh->attacked ) {
      if ( veh->canRepair() && (veh->functions & cfrepair))
         for ( int i = 0; i < veh->typ->weapons.count; i++ )
            if ( veh->typ->weapons.weapon[i].service() )
               av++;


      const Vehicletype* fzt = veh->typ;
      for ( int i = 0; i < fzt->weapons.count; i++ ) {
         if ( fzt->weapons.weapon[i].service() ) {

            if ( veh->functions & cfenergyref )
               if ( fzt->tank.energy )
                  av++;

            if ( veh->functions & cfmaterialref )
               if ( fzt->tank.material )
                  av++;

            if ( veh->functions & cffuelref )
               if ( fzt->tank.fuel )
                  av++;

         }
         if ( fzt->weapons.weapon[i].canRefuel() )
            av++;
      }
   }
   if ( veh->reactionfire.getStatus() == Vehicle::ReactionFire::off )
      return av;
   else
      return 0;
}

int VehicleService :: getServices ( pvehicle veh ) const
{
   int res = 0;
   if ( veh ) {
      if ( veh->canRepair() && (veh->functions & cfrepair))
         for ( int i = 0; i < veh->typ->weapons.count; i++ )
            if ( veh->typ->weapons.weapon[i].service() )
               if ( !veh->attacked )
                  res |= 1 << srv_repair;


      const Vehicletype* fzt = veh->typ;
      for ( int i = 0; i < fzt->weapons.count; i++ ) {
         if ( fzt->weapons.weapon[i].service() ) {
            if ( veh->functions & cfenergyref )
               if ( fzt->tank.energy )
                  res |= 1 << srv_resource;
            if ( veh->functions & cfmaterialref )
               if ( fzt->tank.material )
                  res |= 1 << srv_resource;
            if ( veh->functions & cffuelref)
               if ( fzt->tank.fuel )
                  res |= 1 << srv_resource;
         }


         if ( fzt->weapons.weapon[i].canRefuel() )
            res |= 1 << srv_ammo;
      }
   }
   return res;
}




void             VehicleService :: FieldSearch :: checkVehicle2Vehicle ( pvehicle targetUnit, int xp, int yp )
{
   VehicleService::Target targ;
   targ.dest = targetUnit;

   if ( xp == startPos.x && yp == startPos.y )
      return;

   int dist;
   if ( bypassChecks.distance )
      dist = maxmalq;
   else
      dist = beeline ( xp, yp , startPos.x, startPos.y );


   const SingleWeapon* serviceWeapon = NULL;
   for (int i = 0; i < veh->typ->weapons.count ; i++)
      if ( veh->typ->weapons.weapon[i].service() )
         serviceWeapon = &veh->typ->weapons.weapon[i];


   if ( serviceWeapon ) {
      int targheight = 0;

      for ( int h = 0; h < 8; h++ )
         if ( serviceWeapon->targ & ( 1 << h ))
            if ( serviceWeapon->efficiency[ 6 + getheightdelta ( log2(veh->height), h ) ] )
               targheight |= 1 << h;

      if ( (serviceWeapon->sourceheight & veh->height) || ( bypassChecks.height && (serviceWeapon->sourceheight & veh->typ->height)))
         for (int i = 0; i < veh->typ->weapons.count ; i++) {
            const SingleWeapon& sourceWeapon = veh->typ->weapons.weapon[i];
            if ( sourceWeapon.service() || sourceWeapon.canRefuel() ) {
               if ( targetUnit && serviceWeapon )
                  if ( !(targetUnit->functions & cfnoairrefuel) || targetUnit->height <= chfahrend )
                     if (getdiplomaticstatus2(veh->color, targetUnit->color) == capeace)
                        if ( (serviceWeapon->maxdistance >= dist && serviceWeapon->mindistance <= dist) || bypassChecks.distance )
                           if ( targetUnit->height & targheight || ( bypassChecks.height && ( targetUnit->typ->height & targheight) )) {
                              if ( sourceWeapon.canRefuel() ) {
                                 for ( int j = 0; j < targetUnit->typ->weapons.count ; j++) {
                                    const SingleWeapon& targetWeapon = targetUnit->typ->weapons.weapon[j];
                                    if ( targetWeapon.getScalarWeaponType() == sourceWeapon.getScalarWeaponType()
                                         && targetWeapon.requiresAmmo() ) {
                                       VehicleService::Target::Service s;
                                       s.type = VehicleService::srv_ammo;
                                       s.sourcePos = i;
                                       s.targetPos = j;
                                       s.curAmount = targetUnit->ammo[j];
                                       s.orgSourceAmount = veh->ammo[i];
                                       s.maxAmount = min ( targetWeapon.count, s.curAmount+s.orgSourceAmount );
                                       int sourceSpace = sourceWeapon.count - veh->ammo[i];
                                       s.minAmount = max ( s.curAmount - sourceSpace, 0 );
                                       s.maxPercentage = 100 * s.maxAmount/ targetWeapon.count;
                                       targ.service.push_back ( s );
                                    }
                                 }
                              }

                              if ( sourceWeapon.service() ) {
                                 static int resourceVehicleFunctions[resourceTypeNum] = { cfenergyref, cfmaterialref, cffuelref };
                                 for ( int r = 0; r < resourceTypeNum; r++ )
                                    if ( veh->typ->tank.resource(r) && targetUnit->typ->tank.resource(r) && (veh->functions & resourceVehicleFunctions[r])) {
                                       VehicleService::Target::Service s;
                                       s.type = VehicleService::srv_resource;
                                       s.sourcePos = r;
                                       s.targetPos = r;
                                       s.curAmount = targetUnit->tank.resource(r);
                                       s.orgSourceAmount = veh->tank.resource(r);
                                       s.maxAmount = s.curAmount + min ( targetUnit->putResource(maxint, r, 1) , s.orgSourceAmount );
                                       int sourceSpace = veh->putResource(maxint, r, 1);
                                       s.minAmount = max ( s.curAmount - sourceSpace, 0 );
                                       s.maxPercentage = 100 * s.maxAmount/ veh->typ->tank.resource(r);
                                       targ.service.push_back ( s );
                                    }

                                 if ( veh->canRepair() && (veh->functions & cfrepair))
                                    if ( veh->tank.fuel && veh->tank.material )
                                      // if ( targetUnit->getMovement() >= movement_cost_for_repaired_unit )
                                          if ( targetUnit->damage ) {
                                             VehicleService::Target::Service s;
                                             s.type = VehicleService::srv_repair;
                                             s.sourcePos = -1;
                                             s.targetPos = -1;
                                             s.curAmount = targetUnit->damage;
                                             s.orgSourceAmount = 100;
                                             s.maxAmount = targetUnit->damage;
                                             s.minAmount = veh->getMaxRepair ( targetUnit );
                                             s.maxPercentage = 100 - s.minAmount;
                                             targ.service.push_back ( s );
                                          }

                              }
                           }
            }
         }
      }

   if ( vs.dest.find ( targ.dest->networkid ) != vs.dest.end() ) {
      vs.dest[ targ.dest->networkid ] = targ;
   } else
      if ( targ.service.size() > 0 )
         vs.dest[ targ.dest->networkid ] = targ;

}

void             VehicleService :: FieldSearch :: checkBuilding2Vehicle ( pvehicle targetUnit )
{
   if ( getdiplomaticstatus2(bld->color, targetUnit->color) != capeace)
      return;
      
   VehicleService::Target targ;
   MapCoordinate pos = bld->getEntry();
   int dist = beeline ( pos.x, pos.y, targetUnit->xpos, targetUnit->ypos );
   if ( dist > 0 )
      if ( ! (targetUnit->height & bld->typ->externalloadheight))
         return;

   targ.dest = targetUnit;

   for (int i = 0; i < targetUnit->typ->weapons.count ; i++)
      if ( targetUnit->typ->weapons.weapon[i].requiresAmmo() ) {
         int type = targetUnit->typ->weapons.weapon[i].getScalarWeaponType();
         if ( type >= 0 )
            if ( bld->ammo[type] || targetUnit->ammo[i] || (bld->typ->special & cgammunitionproductionb)) {
               const SingleWeapon& destWeapon = targetUnit->typ->weapons.weapon[i];

               VehicleService::Target::Service s;
               s.type = VehicleService::srv_ammo;
               s.sourcePos = type;
               s.targetPos = i;
               s.curAmount = targetUnit->ammo[i];
               s.orgSourceAmount = bld->ammo[type];
               int stillNeeded = destWeapon.count - targetUnit->ammo[i] - s.orgSourceAmount;
               int produceable;
               if ( (stillNeeded > 0) && (bld->typ->special & cgammunitionproductionb)) {
                  stillNeeded = ((stillNeeded +4) / 5)*5;
                  Resources res;
                  for( int j = 0; j< resourceTypeNum; j++ )
                     res.resource(j) = cwaffenproduktionskosten[type][j] * stillNeeded / 5;

                  ContainerBase* cb = bld;
                  Resources res2 = cb->getResource ( res, 1 );
                  int perc = 100;
                  for ( int i = 0; i< resourceTypeNum; i++ )
                      if ( res.resource(i) )
                         perc = min ( perc, 100 * res2.resource(i) / res.resource(i) );

                  produceable = stillNeeded * perc / 100 ;
                  produceable = (produceable / 5) * 5;
               } else
                  produceable = 0;

               s.maxAmount = min ( destWeapon.count,  targetUnit->ammo[i] + s.orgSourceAmount + produceable );
               s.minAmount = 0;
               targ.service.push_back ( s );
            }
      }

   for ( int r = 0; r < resourceTypeNum; r++ )
      if ( targetUnit->typ->tank.resource(r) ) {
         VehicleService::Target::Service s;
         s.type = VehicleService::srv_resource;
         s.sourcePos = r;
         s.targetPos = r;
         s.curAmount = targetUnit->tank.resource(r);
         s.orgSourceAmount = bld->getResource (maxint, r, 1 );
         s.maxAmount = s.curAmount + min ( targetUnit->putResource(maxint, r, 1) , s.orgSourceAmount );
         int sourceSpace = bld->putResource(maxint, r, 1);
         s.minAmount = max ( s.curAmount - sourceSpace, 0 );
         targ.service.push_back ( s );
      }


   if ( bld->canRepair() )
      if ( targetUnit->damage ) {
         VehicleService::Target::Service s;
         s.type = VehicleService::srv_repair;
         s.sourcePos = -1;
         s.targetPos = -1;
         s.curAmount = targetUnit->damage;
         s.orgSourceAmount = 100;
         s.maxAmount = targetUnit->damage;
         s.minAmount = bld->getMaxRepair ( targetUnit );
         targ.service.push_back ( s );
      }

   if ( vs.dest.find ( targ.dest->networkid ) != vs.dest.end() ) {
      vs.dest[ targ.dest->networkid ] = targ;
   } else
      if ( targ.service.size() > 0 )
         vs.dest[ targ.dest->networkid ] = targ;
}


void  VehicleService :: FieldSearch :: testfield( const MapCoordinate& mc )
{
   pfield fld = gamemap->getField ( mc );
   if ( fld && veh && fld->vehicle ) {
      if ( fld->vehicle == veh ) {
         for ( int i = 0; i < 32; i++ )
            if ( veh->loading[i] )
              checkVehicle2Vehicle ( veh->loading[i], mc.x, mc.y );
      }
      if ( fld->vehicle )
         checkVehicle2Vehicle ( fld->vehicle, mc.x, mc.y );
   }

   if ( fld && bld ) {
      if ( fld->building == bld ) {
         for ( int i = 0; i < 32; i++ )
            if ( bld->loading[i] )
               checkBuilding2Vehicle ( bld->loading[i] );
      }
      if ( fld->vehicle )
         checkBuilding2Vehicle ( fld->vehicle );
   }
}


bool  VehicleService :: FieldSearch ::initrefuelling( int xp1, int yp1 )
{
   int mindist = maxint;
   int maxdist = minint;
   if ( veh ) {
      for ( int i = 0; i < veh->typ->weapons.count; i++ ) {
         const SingleWeapon& w = veh->typ->weapons.weapon[i];
         if ( w.service() || w.canRefuel() ) {
            maxdist = max( maxdist, w.maxdistance / maxmalq );
            mindist = min ( mindist, (w.mindistance + maxmalq - 1) / maxmalq);
         }
      }
   }

   if ( bld ) {
      if ( bld->typ->special & cgexternalloadingb )
         maxdist = 1;
      else
         maxdist = 0;
      mindist = 0;
   }

   initsearch( MapCoordinate( xp1,yp1),mindist,maxdist);
   return true;
}



void VehicleService :: FieldSearch :: init ( pvehicle _veh, pbuilding _bld )
{
   if ( !_veh && !_bld)
      return;

   veh = _veh;
   bld = _bld;

   if ( veh )
      initrefuelling ( veh->xpos, veh->ypos );
   else {
      MapCoordinate mc = bld->getEntry();
      initrefuelling ( mc.x, mc.y );
   }

}

void VehicleService :: FieldSearch :: run (  )
{
   startsearch();
}


int VehicleService :: execute ( pvehicle veh, int targetNWID, int dummy, int step, int pos, int amount )
{
   if ( step != status )
      return -1;

   if ( status == 0 ) {
      vehicle = veh ;
      if ( !vehicle ) {
         int xp = targetNWID;
         int yp = dummy;
         if ( xp >= 0 && yp >= 0 )
            building = getfield ( xp, yp )->building;

         if ( !building ) {
            status = -101;
            return status;
         }
      }

      fieldSearch.init ( vehicle, building );
      fieldSearch.run (  );
      if ( dest.size() > 0 )
         status = 2;
      else
         status = -210;

      return status;
  } else
  if ( status == 2 ) {
     TargetContainer::iterator i = dest.find(targetNWID);
     if ( i == dest.end() )
        return -211;

     Target& t = i->second;
     if ( pos < 0 || pos >= t.service.size())
        return -211;

     Target::Service& serv = t.service[pos];
     if ( amount < serv.minAmount || amount > serv.maxAmount )
        return -212;

     if ( vehicle ) {
        int delta;
        switch ( serv.type ) {
           case srv_resource: {
                                 delta = amount - serv.curAmount;
                                 int put = t.dest->putResource ( delta, serv.targetPos, 0 );
                                 vehicle->getResource ( put, serv.sourcePos, 0 );
                                 logtoreplayinfo ( rpl_refuel, t.dest->xpos, t.dest->ypos, t.dest->networkid, int(1000+serv.targetPos), amount );
                                 logtoreplayinfo ( rpl_refuel, vehicle->xpos, vehicle->ypos, vehicle->networkid, int(1000+serv.sourcePos), vehicle->tank.resource(serv.sourcePos) );
                              }
                              break;
           case srv_repair: vehicle->repairItem ( t.dest, amount );
                            // logtoreplayinfo ( rpl_refuel, eht->xpos, eht->ypos, eht->networkid, int(1002), newfuel );
                            break;
           case srv_ammo: delta = amount - serv.curAmount;
                          t.dest->ammo[ serv.targetPos ] += delta;
                          vehicle->ammo[ serv.sourcePos ] -= delta;
                          logtoreplayinfo ( rpl_refuel, t.dest->xpos, t.dest->ypos, t.dest->networkid, serv.targetPos, t.dest->ammo[ serv.targetPos ] );
                          logtoreplayinfo ( rpl_refuel, vehicle->xpos, vehicle->ypos, vehicle->networkid, serv.targetPos, vehicle->ammo[ serv.sourcePos ] );
                          break;
        }
     } else if ( building ) {
        int delta;
        switch ( serv.type ) {
           case srv_resource: {
                                 delta = amount - serv.curAmount;
                                 int put = t.dest->putResource ( delta, serv.targetPos, 0 );
                                 building->getResource ( put, serv.sourcePos, 0 );
                                 logtoreplayinfo ( rpl_refuel, t.dest->xpos, t.dest->ypos, t.dest->networkid, int(1000+serv.targetPos), amount );
                                 MapCoordinate mc = building->getEntry();
                                 logtoreplayinfo ( rpl_bldrefuel, mc.x, mc.y, int(1000+serv.sourcePos), put );
                              }
                              break;
           case srv_repair: building->repairItem ( t.dest, amount );
                            // logtoreplayinfo ( rpl_refuel, eht->xpos, eht->ypos, eht->networkid, int(1002), newfuel );
                            break;
           case srv_ammo: delta = amount - serv.curAmount;
                          t.dest->ammo[ serv.targetPos ] += delta;
                          building->ammo[ serv.sourcePos ] -= delta;
                          if ( building->ammo[ serv.sourcePos ] < 0 ) {
                             building->produceAmmo ( serv.sourcePos, -building->ammo[ serv.sourcePos ] );
                          }
                          logtoreplayinfo ( rpl_refuel, t.dest->xpos, t.dest->ypos, t.dest->networkid, serv.targetPos, t.dest->ammo[ serv.targetPos ] );
                          MapCoordinate mc = building->getEntry();
                          logtoreplayinfo ( rpl_bldrefuel, mc.x, mc.y, serv.targetPos, building->ammo[ serv.sourcePos ] );
                          break;
        }
     }
     fieldSearch.init ( vehicle, building );
     fieldSearch.run (  );
  }
  return status;
}


int VehicleService :: fillEverything ( int targetNWID, bool repairsToo )
{
   if ( status != 2 )
      return -1;

   TargetContainer::iterator i = dest.find(targetNWID);
   if ( i == dest.end() )
      return -211;

   Target& t = i->second;

   for ( int j = 0; j< t.service.size(); j++ )
      if ( t.service[j].type != srv_repair )
         execute ( NULL, targetNWID, -1, 2, j, t.service[j].maxAmount );
      else
         if ( repairsToo )
            execute ( NULL, targetNWID, -1, 2, j, t.service[j].minAmount );
   return 0;
}


void VehicleService :: registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva )
{
   VehicleAction::registerPVA ( _actionType, _pva );
   if ( pva )
      pva->service = this;
}


VehicleService :: ~VehicleService ( )
{
   if ( pva )
      pva->service = NULL;
}


