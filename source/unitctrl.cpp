//     $Id: unitctrl.cpp,v 1.28 2000-08-28 19:49:43 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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
//     Revision 1.13  2000/07/16 14:20:06  mbickel
//      AI has now some primitive tactics implemented
//      Some clean up
//        moved weapon functions to attack.cpp
//      Mount doesn't modify PCX files any more.
//
//     Revision 1.12  2000/07/02 21:04:14  mbickel
//      Fixed crash in Replay
//      Fixed graphic errors in replay
//
//     Revision 1.11  2000/06/23 11:53:11  mbickel
//      Fixed a bug that crashed ASC when trying to ascend with a unit near the
//       border of the map
//
//     Revision 1.10  2000/06/09 10:51:00  mbickel
//      Repaired keyboard control of pulldown menu
//      Fixed compile errors at fieldlist with gcc
//
//     Revision 1.9  2000/06/08 21:03:44  mbickel
//      New vehicle action: attack
//      wrote documentation for vehicle actions
//
//     Revision 1.8  2000/06/04 21:39:22  mbickel
//      Added OK button to ViewText dialog (used in "About ASC", for example)
//      Invalid command line parameters are now reported
//      new text for attack result prediction
//      Added constructors to attack functions
//
//     Revision 1.7  2000/05/18 14:14:50  mbickel
//      Fixed bug in movemalus calculation for movement
//      Added "view movement range"
//
//     Revision 1.6  2000/05/10 19:15:18  mbickel
//      Fixed invalid height when trying to change a units height
//
//     Revision 1.5  2000/05/07 17:04:07  mbickel
//      Fixed a bug in movement
//
//     Revision 1.4  2000/05/06 20:25:26  mbickel
//      Fixed: -recognition of a second mouse click when selection a pd menu item
//             -movement: fields the unit can only pass, but not stand on them,
//                        are marked darker
//             -intedit/stredit: mouseclick outside is like hitting enter
//
//     Revision 1.3  2000/05/02 16:20:56  mbickel
//      Fixed bug with several simultaneous vehicle actions running
//      Fixed graphic error at ammo transfer in buildings
//      Fixed ammo loss at ammo transfer
//      Movecost is now displayed for mines and repairs
//      Weapon info now shows unhittable units
//
//     Revision 1.2  2000/04/27 17:59:24  mbickel
//      Updated Kdevelop project file
//      Fixed some graphical errors
//
//     Revision 1.1  2000/04/27 16:25:31  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
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
          if ( veh->getMovement() >= minmalq && veh->reactionfire.status == tvehicle::ReactionFire::off )
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

      searchstart ( veh->xpos, veh->ypos, height );
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

      fieldReachableRek.run( x, y, vehicle, 1, height, &path );

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
 
      
       if ( newheight != -1 && newheight != vehicle->height )
          logtoreplayinfo ( rpl_changeheight, x1, y1, x, y, vehicle->networkid, (int) vehicle->height, (int) newheight );
       else 
          logtoreplayinfo ( rpl_move2, x1, y1, x, y, vehicle->networkid, (int) vehicle->height );
 
       if ( mapDisplay )
          mapDisplay->startAction();
       int stat = moveunitxy( x, y, noInterrupt );
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




#ifdef HEXAGON
 int sef_dirnum = 3;
 static const  int          sef_directions[3]  = {0, 1, 5 };
 static const  int          sef_searchorder[6]  = {0, 1, 2, 3, 4, 5 };
#else
 int sef_dirnum = 5;
 static const  int          sef_directions[5]  = {0, 1, 7, 2, 6};
 static const  int          sef_searchorder[8]  = {0, 2, 4, 6, 1, 3, 5, 7};
#endif


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

      calcmovemalus(ox,oy,x,y,vehicle,direc, mm1, mm2);

      streck -= mm2;
      fuelneeded   += mm1;
   } 

   if (streck < 0) 
      return;

   if ( fuelneeded * vehicle->typ->fuelConsumption / 8 > vehicle->fuel)   
      return;

   if (search.mode == 1) { 

      #ifdef fastsearch   
      if ((fld->special & cbmovetempb) != 0) {
         if ((((fld->special & cbmovetempb) >> cbmovetempv) != maindir / 2 + 1) || ((maindir & 1) == 0))
            return;
      } 
      #endif   

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
   } 
   else { 
      // displaymessage (" was soll denn der K„se hier ?? ", 2 );
      /*
      fld->special |= ((direc / 2 + 1) << cbmovetempv);
      fld->a.temp = 1; 
      fieldnum++;
      */
   } 
   if (search.mode == 1) { 
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

   int maindir; 

   #ifdef fastsearch   
   mode = 0; 
   strck = vehicle->getMovement(); 
   for (maindir = 0; maindir <= 3; maindir++) { 
      move(startx,starty,maindir * 2 + 1,strck);
      tiefe--; 
   } 
   #endif   


   search.mode = 1; 
   search.strck = vehicle->getMovement(); 

   for (maindir = 0; maindir < sidenum; maindir++) { 
      searchmove( search.startx, search.starty, sef_searchorder[maindir], search.strck, 0 );
      search.tiefe--;
   }

   actmap->cleartemps(7); 
} 


#ifdef HEXAGON
  static const trichtungen  directions[3][3]  = {{{5, 0, 4, 1, 3, 2 }, {0, 1, 5, 2, 4, 3 }, {1, 0, 2, 5, 3, 4 }},
                                                 {{5, 4, 0, 3, 1, 2 }, {0, 1, 2, 3, 4, 5 }, {1, 2, 0, 3, 5, 4 }}, 
                                                 {{4, 3, 5, 2, 0, 1 }, {3, 4, 2, 5, 1, 0 }, {2, 3, 1, 4, 0, 5 }}}; 
#else
  static const trichtungen  directions[3][3]  = {{{7, 6, 0, 5, 1, 4, 2, 3}, {0, 1, 7, 2, 6, 3, 5, 4}, {1, 0, 2, 7, 3, 6, 4, 5}},
                                                 {{6, 7, 5, 0, 4, 1, 3, 2}, {0, 1, 2, 3, 4, 5, 6, 7}, {2, 1, 3, 0, 4, 7, 5, 6}}, 
                                                 {{5, 6, 4, 7, 3, 0, 2, 1}, {4, 3, 5, 2, 6, 1, 7, 0}, {3, 2, 4, 1, 5, 0, 6, 7}}}; 
#endif


 
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

    if (mode == 1)
       if (streck > vehicle->getMovement() )
          return;                      

    getnextfield ( x, y, direc);

    if ((x < 0) || (y < 0) || (x >= actmap->xsize) || (y >= actmap->ysize)) 
       return;

    if (actmap->weather.wind[ getwindheightforunit ( vehicle ) ].speed && vehicle->height >= chtieffliegend && vehicle->height <= chhochfliegend) {
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
       if ((strecke.field[b].x == x) && (strecke.field[b].y == y)) 
          return;

    int c = fieldaccessible( fld, vehicle, height); 

    if ( c == 0 ) 
       return;
    else { 
       int mm1;      // fuelusage
       int mm2;      // movementusage

       calcmovemalus(ox, oy, x, y, vehicle, direc, mm1, mm2);

       fuel += mm1;
       streck += mm2;
    } 
    if (streck > maxwegstrecke) 
       return;

    if (fuel * vehicle->typ->fuelConsumption / minmalq  > vehicle->fuel) 
       return;

    if (mode == 1) 
       if ((fld->a.temp > 0) && (streck > fld->a.temp)) return;

    if ((fld->a.temp > streck) || (fld->a.temp == 0)) 
        fld->a.temp = streck;

    strecke.field[strecke.tiefe].x = x; 
    strecke.field[strecke.tiefe].y = y; 
    if ((x == x2) && (y == y2)) { 
       distance = streck; 
       zielerreicht = 1;

       shortestway = strecke; 
       if (actmap->weather.wind[ getwindheightforunit ( vehicle ) ].speed && vehicle->height >= chtieffliegend && vehicle->height <= chhochfliegend) {
          if ((mode == 2) || ((mode == 1) && (streck == windbeeline(x1,y1,x2,y2))))
             zielerreicht = 2;
       } else {
          if ((mode == 2) || ((mode == 1) && (streck == beeline(x2,y2,x1,y1))))
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
                                                   int          md,
                                                   int          _height,
                                                   IntFieldList*   path )
               
/*  mode :  1 krzesten weg finden
            2 irgendeinen weg finden  */ 
{ 
   shortestway.tiefe = -1;
   actmap->cleartemps(7); 

   x2 = x22;
   y2 = y22;
   mode = md;
   vehicle = eht;
   height = _height;

   x1 = vehicle->xpos; 
   y1 = vehicle->ypos; 

   maxwegstrecke = vehicle->getMovement(); 
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
      /*
      if ((mode == 2) && zielerreicht) 
         if (distance <= maxwegstrecke) {
            moveparams.movepath = shortestway; 
            moveparams.movedist = distance; 
            return;
         }  */

      if ((mode == 1) && zielerreicht==2) 
         break; 
   } 

   if ( zielerreicht >= 1) {
      path->addField ( eht->xpos, eht->ypos );
      for ( int d = 1; d <= shortestway.tiefe; d++) 
         path->addField ( shortestway.field[d].x, shortestway.field[d].y ); 
   }
   actmap->cleartemps(7); 
} 




int  BaseVehicleMovement :: moveunitxy(int xt1, int yt1, int noInterrupt )
{ 
   pfield fld = getfield( xt1, yt1 ); 

   int fieldnum;
   for ( fieldnum = 0; fieldnum < path.getFieldNum(); fieldnum++) 
      if ( path.getField( fieldnum ) == fld ) 
         break; 
   
   
   if ( fieldaccessible(fld, vehicle, newheight) < 2) 
      return -106;

   pfield oldfield = getfield( vehicle->xpos, vehicle->ypos ); 


   int x ;
   int y ;
   path.getFieldCoordinates ( 0, &x, &y );

   tsearchreactionfireingunits srfu;
   treactionfirereplay rfr;    

   treactionfire* rf;
   if ( runreplay.status > 0  )
      rf = &rfr;
   else
      rf = &srfu;

   rf->init( vehicle, &path );

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
         } 
   } 

   int i = 0;
   int cancelmovement = 0;

   int movedist = 0;
   int fueldist = 0;

   while ( (x != xt1 || y != yt1) && vehicle && cancelmovement!=1 ) { 

      if ( cancelmovement > 1 )
         cancelmovement--;

      int x2, y2;
      path.getFieldCoordinates ( i+1, &x2, &y2 );

      int mm1,mm2;
      calcmovemalus( x, y, x2, y2, vehicle, -1 , mm1, mm2);
      movedist += mm2;
      fueldist += mm1;

      pfield fld1 = getfield ( x,  y  );
      pfield fld2 = getfield ( x2, y2 );



      if ( mapDisplay ) 
         mapDisplay->displayMovingUnit ( x, y, x2, y2, vehicle, vehicle->height, newheight, i, fieldnum );

      int dir;
      if (vehicle->functions & ( cffahrspur | cficebreaker ))  {
        dir = getdirection( x, y, x2, y2 );
      
        if ( vehicle->functions & cffahrspur )
           if ( fahrspurobject )
              if ( fld1->bdt & cbfahrspur )
                 if ( ! (actmap->objectcrc   &&   !actmap->objectcrc->speedcrccheck->checkobj2 ( fahrspurobject, 2 ))) 
                    fld1 -> addobject ( fahrspurobject, 1 << dir );

        if ( vehicle->functions & cficebreaker )
           if ( eisbrecherobject )
              if ( ! (actmap->objectcrc   &&   !actmap->objectcrc->speedcrccheck->checkobj2 ( eisbrecherobject, 2 ))) 
                 if ( (fld1->bdt & cbsnow1 )  || ( fld1->bdt & cbsnow2 ) || fld1->checkforobject ( eisbrecherobject ) ) {
                    fld1 -> addobject ( eisbrecherobject, 1 << dir );
                    fld1->checkforobject ( eisbrecherobject )->time = actmap->time.a.turn;
                 }

      } 

      i++;
      path.getFieldCoordinates ( i, &x, &y );

      pfield field3 = fld2; 
      if (vehicle->functions & cffahrspur) {
         if ( fahrspurobject )
            if ( field3->bdt & cbfahrspur )
               if ( ! (actmap->objectcrc   &&   !actmap->objectcrc->speedcrccheck->checkobj2 ( fahrspurobject, 2 ))) 
                  if (dir >= sidenum/2) 
                    getfield ( x, y ) -> addobject ( fahrspurobject, 1 << (dir - sidenum/2));
                  else 
                    getfield ( x, y ) -> addobject ( fahrspurobject, 1 << (dir + sidenum/2));
      } 
      if ( vehicle->functions & cficebreaker )
         if ( eisbrecherobject )
            if ( ! (actmap->objectcrc   &&   !actmap->objectcrc->speedcrccheck->checkobj2 ( eisbrecherobject, 2 ))) 
                 if ( (field3->bdt & cbsnow1 )  || ( field3->bdt & cbsnow2 ) || field3->checkforobject ( eisbrecherobject ) ) {
                  if (dir >= sidenum/2) 
                    getfield ( x, y ) -> addobject ( eisbrecherobject, 1 << (dir - sidenum/2));
                  else 
                    getfield ( x, y ) -> addobject ( eisbrecherobject, 1 << (dir + sidenum/2));
      
                    field3->checkforobject ( eisbrecherobject )->time = actmap->time.a.turn;
      
               }

      if ( vehicle ) {
         vehicle->xpos = x;
         vehicle->ypos = y;
         vehicle->addview();
      }

      int fieldschanged = evaluateviewcalculation ( 1 << actmap->playerview );

      if ( rf->checkfield ( x, y, vehicle, mapDisplay ))
         cancelmovement = 1;

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

            if ( mapDisplay && fieldvisiblenow ( getfield ( x, y ), actmap->playerview) || field3->mineowner() == actmap->playerview )
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


      vehicle->fuel -= fueldist * vehicle->typ->fuelConsumption / 8;
      if ( vehicle->fuel < 0 )
         vehicle->fuel = 0;


      if (fld->vehicle == vehicle) { 
         i = vehicle->getMovement() - movedist;
         if (i > 0) 
           vehicle->setMovement ( i );
         else 
           vehicle->setMovement ( 0 );

        /*
         if ( vehicle->typ->fuelconsumption )
           if ( (vehicle->getMovement() >> 3) > (vehicle->fuel / vehicle->typ->fuelconsumption) )
              vehicle->movement = (vehicle->fuel << 3) / vehicle->typ->fuelconsumption;
        */
      } 
      else { 
         vehicle->setMovement ( 0 );
         vehicle->attacked = true; 
         if (vehicle->height == chtieffliegend) 
            vehicle->height = chfahrend; 
      } 

      vehicle->setnewposition ( x, y );
      if ( newheight != -1 && vehicle->typ->height & newheight)
         vehicle->height = newheight;

      
      dashboard.x = 0xffff; 
   } 

   int fieldschanged = computeview( 1 << actmap->playerview ); 

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
}




int ChangeVehicleHeight :: execute_withmove ( void )
{
   if ( !vehicle ) 
      return -unspecified_error;

   pfield fld = getfield( vehicle->xpos, vehicle->ypos ); 

   int steigung = vehicle->typ->steigung; 

   if ( vehicle->typ->height & (chtieffliegend | chfliegend | chhochfliegend) ) {
      int w;
      if ( vehicle->fuel < steigung * maxmalq )
         return -115;

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

      return moveheight( ); 
   } else {
      displaymessage("changing height with movement of non-airplanes has not been defined yet !",2 );
      return -1;
   }

}


int ChangeVehicleHeight :: moveheight( void )
{ 

   initwindmovement( vehicle );

   int ok2 = false; 

   int xx = vehicle->xpos;
   int yy = vehicle->ypos;
   for ( int direc = 0; direc < sidenum; direc++) { 
      int ok = true; 
      int x = xx; 
      int y = yy; 

      int dist = 0;
      int mx = vehicle->getMovement();
      while ( dist < vehicle->typ->steigung * minmalq  && mx > 0 && ok) {
         int ox = x;
         int oy = y;
         getnextfield( x, y, direc );
         if ((x < 0) || (y < 0) || (x >= actmap->xsize) || (y >= actmap->ysize))
            ok = false;
         else {

            int fuelcost, movecost;
            calcmovemalus(ox,oy,x,y,vehicle,direc, fuelcost, movecost);
            /*
            dist += minmalq ;// - windmovement[direc];
            mx -= minmalq ; //- windmovement[direc];

            movecost += windmovement[direc]; // compensating for the wind which.
            */
            dist += movecost;
            mx -= movecost;

            pfield fld = getfield(x,y);
            if ( fieldaccessible(fld, vehicle, vehicle->height ) < 1) 
               ok = false; 
            if ( fieldaccessible(fld, vehicle, newheight ) < 1) 
               ok = false; 
         } 
      } 
      if ( mx < 0 )
         ok = false;


      if ( ok && fieldaccessible( getfield(x,y), vehicle, newheight ) < 2)
         ok = false;
/*
      if ( ok )
         if ( fld->building || fld->vehicle )
            ok = false;
*/

      if (ok) { 
         reachableFields.addField ( x, y );
         ok2 = true; 
      } 

   } 
   if ( ok2 ) 
      return 0;
   else
      return -116;
} 


int ChangeVehicleHeight :: moveunitxy ( int xt1, int yt1 )
{
   int oldheight = vehicle->height;
   int res = BaseVehicleMovement :: moveunitxy ( xt1, yt1 );
   if ( res < 0 )
      return res;

   if ( oldheight < newheight ) {
      if ( oldheight == chfahrend ) {
         vehicle->fuel -= vehicle->typ->fuelConsumption * vehicle->typ->steigung;
         if ( vehicle->fuel < 0 )
            vehicle->fuel = 0;

         if ( vehicle->typ->steigung * minmalq <= airplanemoveafterstart )
            vehicle->setMovement ( 0 );
         else
            vehicle->setMovement ( airplanemoveafterstart );

         vehicle->attacked = 1;
         vehicle->height = chtieffliegend;
      } else {
         vehicle->setMovement ( vehicle->getMovement() - air_heightincmovedecrease );

         vehicle->fuel -= vehicle->typ->fuelConsumption * air_heightincmovedecrease / 8;
         if ( vehicle->fuel < 0 )
            vehicle->fuel = 0;

         int newmovement = vehicle->typ->movement[log2(newheight)] * vehicle->getMovement() / vehicle->typ->movement[log2(vehicle->height)];
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
         vehicle->setMovement ( vehicle->getMovement() - air_heightdecmovedecrease );

         vehicle->fuel -= vehicle->typ->fuelConsumption * air_heightdecmovedecrease / 8;
         if ( vehicle->fuel < 0 )
            vehicle->fuel = 0;

         int newmovement = vehicle->typ->movement[log2(newheight)] * vehicle->getMovement() / vehicle->typ->movement[log2(vehicle->height)];
         vehicle->setMovement ( newmovement );

         vehicle->height = newheight;
      }
   }
   return 0;
}


int ChangeVehicleHeight :: verticalHeightChange ( void )
{
   if ( !vehicle ) 
      return -unspecified_error;

   pfield fld = getfield(vehicle->xpos,vehicle->ypos); 

   int steigung = vehicle->typ->steigung; 
   int oldheight = vehicle->height;

   if (vehicle->typ->height & (chtieffliegend | chfliegend | chhochfliegend)) {
      if ( (newheight < oldheight) && (newheight == chfahrend) ) { 
         if ( !terrainaccessible ( fld, vehicle, newheight ))
            return -109;

         int newmovement = vehicle->typ->movement[log2(newheight)] * (vehicle->getMovement() - (helicopter_landing_move_cost + air_heightdecmovedecrease)) / vehicle->typ->movement[log2(vehicle->height)];
         if ( newmovement < 0)                      
            return -111;

         int fuelcost = vehicle->typ->fuelConsumption * (helicopter_landing_move_cost + air_heightdecmovedecrease) / 8;
         if ( fuelcost > vehicle->fuel )
            return -115;


         vehicle->setMovement ( newmovement ); 
         if ( !helicopter_attack_after_descent )
            vehicle->attacked = 0; 
         vehicle->fuel -= fuelcost;

      } else
      if ( newheight < oldheight ) { 
         int newmovement = vehicle->typ->movement[log2(newheight)] * vehicle->getMovement() / vehicle->typ->movement[log2(vehicle->height)];
         if ( newmovement < air_heightdecmovedecrease )
            return -111;

         int fuelcost = vehicle->typ->fuelConsumption * air_heightdecmovedecrease / 8;
         if ( fuelcost > vehicle->fuel )
            return -115;


         vehicle->setMovement ( newmovement - air_heightdecmovedecrease ); 
         vehicle->fuel -= fuelcost;
      } else
      if (( newheight > oldheight ) && (newheight > chtieffliegend)) { 
         int newmovement = vehicle->typ->movement[log2(newheight)] * vehicle->getMovement() / vehicle->typ->movement[log2(vehicle->height)];
         if (newmovement < air_heightincmovedecrease) 
            return -110;
         int fuelcost = vehicle->typ->fuelConsumption * air_heightincmovedecrease / 8;
         if ( fuelcost > vehicle->fuel )
            return -115;

         vehicle->setMovement ( newmovement - air_heightincmovedecrease ); 
         vehicle->fuel -= fuelcost;

         if ((newheight == chtieffliegend) && helicopter_attack_after_ascent ) 
            vehicle->attacked = 1; 
      } else
         if ( newheight > oldheight  &&  newheight == chtieffliegend ) {
            int newmovement = vehicle->typ->movement[log2(newheight)] * vehicle->getMovement() / vehicle->typ->movement[log2(vehicle->height)];
            if ( newmovement < air_heightincmovedecrease )
               return -110;

            if ( helicopter_attack_after_ascent ) 
               vehicle->attacked = 1; 

            vehicle->setMovement ( newmovement - air_heightincmovedecrease);
         }
         
      logtoreplayinfo ( rpl_changeheight, (int) vehicle->xpos, (int) vehicle->ypos, 
                                          (int) vehicle->xpos, (int) vehicle->ypos, vehicle->networkid, (int) vehicle->height, (int) newheight );
      vehicle->height = newheight; 
   } 
   else {   /*  not an aircraft */ 

      int md;

      if ( newheight > oldheight ) { 
         md = sub_heightincmovedecrease; 
         if ( vehicle->getMovement() < md )
            return -110;
      } 
      else { 
         md = sub_heightdecmovedecrease; 
         if ( vehicle->getMovement() < md )
            return -111;
      } 

      int newmovement = vehicle->typ->movement[log2(newheight)] * (vehicle->getMovement() - md ) / vehicle->typ->movement[log2(vehicle->height)];

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

      int fuelcost = vehicle->typ->fuelConsumption * md / 8;
      if ( fuelcost > vehicle->fuel )
         return -115;

      vehicle->setMovement ( newmovement );
      vehicle->fuel -= fuelcost; 

      logtoreplayinfo ( rpl_changeheight, (int) vehicle->xpos, (int) vehicle->ypos, 
                                          (int) vehicle->xpos, (int) vehicle->ypos, vehicle->networkid, (int) vehicle->height, (int) newheight );

      vehicle->height = newheight; 
     
   } 
   return 0;
}


int ChangeVehicleHeight :: execute ( pvehicle veh, int x, int y, int step, int height, int param2 )
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
         int res = execute_withmove ( );
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

      fieldReachableRek.run( x, y, vehicle, 1, height, &path );

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
 
       logtoreplayinfo ( rpl_changeheight, x1, y1, x, y, vehicle->networkid, (int) vehicle->height, (int) newheight );
 
       if ( mapDisplay )
          mapDisplay->startAction();
       int stat = moveunitxy( x, y );
       if ( mapDisplay ) {
          // mapDisplay->displayMap();
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
         if (veh->height < 128) 
            if ((veh->height << 1) & veh->typ->height ) 
               if ( veh->typ->steigung )
                  return 1;
               else
                  return 2;
   return 0;
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
         if (veh->height > 1) 
            if ((veh->height >> 1) & veh->typ->height ) 
               if ( veh->typ->steigung )
                  return 1;
               else
                  return 2;
   return 0;
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
               : VehicleAction ( vat_attack, _pva )
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
            if ( eht->reactionfire.status == tvehicle::ReactionFire::off ) {
               if (eht->typ->wait == false  ||  !eht->hasMoved() )
                  return 1;
            } else {
               // if ( reactionfire_active >= 3 )
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

      kamikaze = _kamikaze;

      search.init( veh, kamikaze, this ); 
      search.run(); 

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
         atw = attackableVehicles.getData ( x, y );
      else
         if ( attackableObjects.isMember ( x, y ))
            atw = attackableObjects.getData ( x, y );
         else
            if ( attackableBuildings.isMember ( x, y ))
               atw = attackableBuildings.getData ( x, y );

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

      if ( mapDisplay ) {
         mapDisplay->displayActionCursor ( vehicle->xpos, vehicle->ypos, x, y );
         battle->calcdisplay ();
      } else
         battle->calc();

      int ad2 = battle->av.damage;
      int dd2 = battle->dv.damage;
      battle->setresult ();

      if ( ad2 < 100 ) {
         if ( vehicle->functions & cf_moveafterattack )
            vehicle->setMovement ( vehicle->getMovement() - vehicle->typ->movement[log2(vehicle->height)]*attackmovecost / 100 );
         else   
            vehicle->setMovement ( 0 );
      }

      logtoreplayinfo ( rpl_attack, xp1, yp1, x, y, ad1, ad2, dd1, dd2, weapnum );

      computeview();

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



void     VehicleAttack :: tsearchattackablevehicles::testfield( void )
{ 
   if ( fieldvisiblenow(getfield(xp,yp)) ) { 
      if ( !kamikaze ) {
         pattackweap atw = attackpossible( angreifer, xp, yp ); 
         if (atw->count > 0) { 
            switch ( atw->target ) {
               case AttackWeap::vehicle:  va->attackableVehicles.addField  ( xp, yp, atw );
                  break;                                    
               case AttackWeap::building: va->attackableBuildings.addField ( xp, yp, atw );
                  break;
               case AttackWeap::object:   va->attackableObjects.addField   ( xp, yp, atw );
                  break;
            } /* endswitch */
            anzahlgegner++;
         } 
         delete atw;
      } else {
          if (fieldvisiblenow(getfield(xp,yp))) { 
             pvehicle eht = getfield(xp,yp)->vehicle; 
             if (eht != NULL) 
                if (((angreifer->height >= chtieffliegend) && (eht->height <= angreifer->height) && (eht->height >= chschwimmend)) 
                  || ((angreifer->height == chfahrend) && (eht->height == chfahrend)) 
                  || ((angreifer->height == chschwimmend) && (eht->height == chschwimmend))
                  || ((angreifer->height == chgetaucht) && (eht->height >=  chgetaucht) && (eht->height <= chschwimmend))) {
                   getfield(xp,yp)->a.temp = dist; 
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
   

   moveparams.movesx = angreifer->xpos; 
   moveparams.movesy = angreifer->ypos; 
   if (fieldvisiblenow(getfield(moveparams.movesx,moveparams.movesy)) == false) 
      return -1;

   if (angreifer->attacked) 
      return -202;
   

   if (angreifer->typ->weapons->count == 0) 
      return -204;
   
   if ( angreifer->typ->wait && angreifer->hasMoved() ) 
         return -205;
      

   int d = 0; 
   int maxdist = 0; 
   int mindist = 20000; 
   for ( int a = 0; a < angreifer->typ->weapons->count; a++) 
      if (angreifer->ammo[a] > 0) {
         d++; 

         if ((angreifer->typ->weapons->weapon[a].maxdistance >> 3) > maxdist) 
            maxdist = (angreifer->typ->weapons->weapon[a].maxdistance >> 3);

         if ((angreifer->typ->weapons->weapon[a].mindistance >> 3) < mindist) 
            mindist = (angreifer->typ->weapons->weapon[a].mindistance >> 3);
      }
   

   if (d == 0) 
      return -204;
   
   initsuche( angreifer->xpos,angreifer->ypos, maxdist + 1, mindist ); 
   startsuche();

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


