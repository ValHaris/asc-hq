/*! \file controls.cpp
   Controlling units (which is graudally moved to vehicletype.cpp and unitctrl.cpp );
   Things that are run when starting and ending someones turn
*/


/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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
#include <ctime>

#include "buildingtype.h"
#include "vehicletype.h"
#include "typen.h"

#include "spfst.h"
#include "loaders.h"
#include "controls.h"
#include "attack.h"
#include "gameoptions.h"
#include "errors.h"
#include "viewcalculation.h"
#include "replay.h"
#include "resourcenet.h"
#include "stack.h"


bool checkUnitsForCrash( Player& player, ASCString& text )
{
   bool endangeredUnits = false;
   
   for ( Player::VehicleList::const_iterator i = player.vehicleList.begin(); i != player.vehicleList.end(); ++i ) {
      int endurance = UnitHooveringLogic::getEndurance( *i );
      if ( endurance>= 0 && endurance <= CGameOptions::Instance()->aircraftCrashWarningTime ) {
         endangeredUnits = true;
         text += (*i)->getName() + " " + (*i)->getPosition().toString() + "\n";
      }
        
   }
   return endangeredUnits;
}

pair<int,int> calcMoveMalus( const MapCoordinate3D& start,
                            const MapCoordinate3D& dest,
                            const Vehicle*     vehicle,
                            WindMovement* wm,
                            bool*  inhibitAttack,
                            bool container2container )
{
   int direc = getdirection ( start.x, start.y, dest.x, dest.y );


   int fuelcost = 10;
   int movecost;
   bool checkHemming = true;
   bool checkWind = wm != NULL;
   int dist = 1;

   if ( start.getNumericalHeight() >= 0 && dest.getNumericalHeight() >= 0 ) {

      // changing height
      if ( (start.getNumericalHeight() != dest.getNumericalHeight()) && !container2container ) {
          const VehicleType::HeightChangeMethod* hcm = vehicle->getHeightChange( start.getNumericalHeight() < dest.getNumericalHeight() ? 1 : -1, start.getBitmappedHeight());
          if ( !hcm || hcm->dist != beeline ( start, dest )/maxmalq )
             fatalError("Calcmovemalus called with invalid height change distance");
          dist = hcm->dist;
          movecost = hcm->moveCost;
          fuelcost = max(hcm->dist*10,10);
          if ( inhibitAttack && !hcm->canAttack )
            *inhibitAttack = !hcm->canAttack;
          checkHemming = false;
          if ( start.getNumericalHeight() < 4 || dest.getNumericalHeight() < 4 )
             checkWind = false;
      } else
         // flying
         if (start.getNumericalHeight() >= 4 )
            movecost = maxmalq;
         else
            if ( start.getNumericalHeight() <= 1 ) {
               movecost = submarineMovement;
               checkWind = false;
            } else {
               // not flying
               MapField* fld = vehicle->getMap()->getField( dest.x, dest.y );
               if ( fld->building )
                  movecost = maxmalq;
               else
                  movecost = fld->getmovemalus( vehicle );
               checkWind = false;
            }

   } else
      if ( dest.getNumericalHeight() >= 0 ) {
        // moving out of container
        int mm;
        const ContainerBaseType::TransportationIO* unloadSystem = vehicle->getMap()->getField( start.x, start.y )->getContainer()->vehicleUnloadSystem( vehicle->typ, dest.getBitmappedHeight() );
        if ( unloadSystem )
            mm = unloadSystem->movecost;
        else
           mm = 0;
        
        if ( mm > 0 )
            movecost = mm;
        else {
            if ( dest.getNumericalHeight() >= 4 )
               // flying
               movecost = maxmalq;
            else {
               if ( dest.getNumericalHeight() <= 1 ) {
                  movecost = submarineMovement;
                  checkWind = false;
               } else {
                  movecost = vehicle->getMap()->getField( dest.x, dest.y )->getmovemalus( vehicle );
               }
            }
        }
      } else {
        // moving from one container to another
        movecost = maxmalq;
        checkHemming = false;
        checkWind = false;
      }

   static const  int         movemalus[6]  = { 0, 3, 5, 0, 5, 3 };
   
   if ( checkHemming && dest.getNumericalHeight() >= 0 )
      for (int c = 0; c < sidenum; c++) {
         int x = dest.x + getnextdx ( c, dest.y );
         int y = dest.y + getnextdy ( c );
         MapField* fld = vehicle->getMap()->getField ( x, y );
         if ( fld ) {
           int d = (c - direc);

           if (d >= sidenum)
              d -= sidenum;

           if (d < 0)
              d += sidenum;

           if ( fld->vehicle ) {
              if ( vehicle->getMap()->getPlayer(vehicle).diplomacy.isHostile( fld->vehicle->getOwner() ) )
                 if ( attackpossible28(fld->vehicle,vehicle, NULL, dest.getBitmappedHeight() ))
                    movecost += movemalus[d];
              
           }
         }
      }

    /*******************************/
    /*    Wind calculation         */
    /*******************************/
   if ( wm && checkWind && direc >= 0 && direc <= 5 )
      if (dest.getNumericalHeight() >= 4 && dest.getNumericalHeight() <= 6 &&
          start.getNumericalHeight() >= 4 && start.getNumericalHeight() <= 6 &&
          vehicle->getMap()->weather.windSpeed  ) {
         movecost -=  wm->getDist( direc ) * dist;
         fuelcost -=  wm->getDist ( direc ) * dist;

         if ( movecost < 1 )
           movecost = 1;

         if ( fuelcost <= 0 )
           fuelcost = 0;
      }
   return make_pair(movecost,fuelcost);
}






void Building :: execnetcontrol ( void )
{
   for ( int i = 0; i < 3; i++ )
      if ( !getMap()->isResourceGlobal(i) ) {
         if (  netcontrol & (cnet_moveenergyout << i )) {
            npush (  netcontrol );
            netcontrol |= (cnet_stopenergyinput << i );
            actstorage.resource(i) -= putResource ( actstorage.resource(i), i, 0 );
            npop (  netcontrol );
         } else
            if (  netcontrol & (cnet_storeenergy << i )) {
               npush (  netcontrol );
               netcontrol |= (cnet_stopenergyoutput << i );
               actstorage.resource(i) += getResource ( getStorageCapacity().resource(i) -  actstorage.resource(i), i, false );
               npop (  netcontrol );
            }
      }

}

                    /*   modes: 0 = energy   ohne abbuchen
                                1 = material ohne abbuchen
                                2 = fuel     ohne abbuchen

                                  +4         mit abbuchen                         /
                                  +8         nur Tributzahlungen kassieren       /
                                 +16         plus zurueckliefern                 <  diese Bits schliessen sich gegenseitig aus
                                 +32         usage zurueckliefern                 \
                                 +64         tank zurueckliefern                   \
                                 */


int  Building :: putResource ( int      need,    int resourcetype, bool queryonly, int scope, int player  )
{
   if ( need < 0 )
      return -getResource( -need, resourcetype, queryonly, scope, player );
   else {
      int placed;
      {
         PutResource putresource ( getMap(), scope );
         placed = putresource.getresource ( entryPosition.x, entryPosition.y, resourcetype, need, queryonly, player >= 0 ? player : getMap()->actplayer, scope );
      }
      // if ( !queryonly && placed > 0 )
      //   resourceChanged();
      return placed;
   }
}


int  Building :: getResource ( int      need,    int resourcetype, bool queryonly, int scope, int player )
{
   if ( need < 0 )
      return -putResource( -need, resourcetype, queryonly, scope, player );
   else {
      int got;
      {
         GetResource gr ( getMap(), scope );
         got = gr.getresource ( entryPosition.x, entryPosition.y, resourcetype, need, queryonly, player >= 0 ? player : getMap()->actplayer, scope );
      }
      // if ( !queryonly && got > 0 )
      //   resourceChanged();
      return got;
      
   }
}

int  Building :: getAvailableResource ( int      need,    int resourcetype, int scope ) const
{
   int got;
   {
      GetResource gr ( getMap(), scope );
      got = gr.getresource ( entryPosition.x, entryPosition.y, resourcetype, need, true, getMap()->actplayer, scope );
   }
   return got;
}

