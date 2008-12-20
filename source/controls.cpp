/*! \file controls.cpp
   Controlling units (which is graudally moved to vehicletype.cpp and unitctrl.cpp );
   Things that are run when starting and ending someones turn
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

#include <stdio.h>           
#include <cstring>
#include <math.h>
#include <stdarg.h>
#include <ctime>

#include "buildingtype.h"
#include "vehicletype.h"
#include "typen.h"

#include "newfont.h"
#include "spfst.h"
#include "loaders.h"
#include "misc.h"
#include "controls.h"
#include "stack.h"
#include "dlg_box.h"
#include "dialog.h"
#include "attack.h"
#include "gamedlg.h"
#include "gameoptions.h"
#include "ai/ai.h"
#include "errors.h"
#include "viewcalculation.h"
#include "replay.h"
#include "resourcenet.h"
#include "itemrepository.h"
#include "strtmesg.h"
#include "messagedlg.h"
#include "gameevent_dialogs.h"
#include "cannedmessages.h"
#include "mapdisplay.h"

#include "dialogs/choosetech.h"

tmoveparams moveparams;

void tmoveparams::reset(){
   movestatus = 0; 
   movesx = 0;
   movesy = 0; 
   moveerr = 0; 
   vehicletomove = NULL; 
   newheight = 0; 
   oldheight = 0; 
   heightdir = 0; 
   buildingtobuild = NULL;    
   movespeed = 0;
   uheight = 0;
}



  class   tsearchdestructbuildingfields : public SearchFields {
                      public:
                                Vehicle*                vehicle;
                                char                    numberoffields;
                                tfield*                  startfield;
                                void                    initdestructbuilding( int x, int y );
                                virtual void            testfield ( const MapCoordinate& mc );
                                tsearchdestructbuildingfields ( GameMap* _gamemap ) : SearchFields ( _gamemap ) {};
                             };


void         tsearchdestructbuildingfields::initdestructbuilding( int x, int y )
{
   Vehicle*     eht = getfield(x,y)->vehicle;
   vehicle = eht;
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
   if (startfield->building && getheightdelta(log2(vehicle->height), log2(startfield->building->typ->buildingheight)) == 0 && !startfield->building->typ->buildingNotRemovable ) {
      numberoffields++;
      startfield->a.temp = 20;
   }
}





void         destructbuildinglevel1(int xp, int yp)
{
   tsearchdestructbuildingfields   sdbf ( actmap );
   sdbf.initdestructbuilding( xp, yp  );
}

Resources getDestructionCost( Building* bld, Vehicle* veh )
{
   Resources r;
   r.material = - bld->typ->productionCost.material * (100 - bld->damage) / destruct_building_material_get / 100;
   r.fuel = destruct_building_fuel_usage * veh->typ->fuelConsumption;
   return r;
}

void         destructbuildinglevel2( int xp, int yp)
{
   tfield* fld = getfield(xp,yp);
   if (fld->a.temp == 20)
      if (moveparams.movestatus == 115) {
         actmap->cleartemps(7);
         Vehicle* eht = moveparams.vehicletomove;

         Building* bb = fld->building;
         Resources res = eht->getResource( getDestructionCost( bb, eht ), false);

         eht->setMovement ( 0 );
         eht->attacked = 1;

         if ( bb->getCompletion() ) {
            bb->setCompletion ( bb->getCompletion()-1 );
         } else {
            bb->netcontrol = cnet_stopenergyinput + (cnet_stopenergyinput << 1) + (cnet_stopenergyinput << 2);
            Resources put = bb->putResource( bb->actstorage, false );
            delete bb;
         }
         logtoreplayinfo ( rpl_removebuilding3, xp, yp, eht->networkid, res.energy, res.material, res.fuel );
         computeview( actmap );
         displaymap();
         moveparams.movestatus = 0;
      }
}



int windbeeline ( const MapCoordinate& start, const MapCoordinate& dest, WindMovement* wm ) {
   int x1 = start.x;
   int y1 = start.y;
   int dist = 0;
   while ( x1 != dest.x  || y1 != dest.y ) {
      dist+= minmalq;
      int direc = getdirection ( x1, y1, dest.x, dest.y );
      dist -= wm->getDist(direc);
      getnextfield ( x1, y1, direc );
   }
   return dist;
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
          const Vehicletype::HeightChangeMethod* hcm = vehicle->getHeightChange( start.getNumericalHeight() < dest.getNumericalHeight() ? 1 : -1, start.getBitmappedHeight());
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
               tfield* fld = vehicle->getMap()->getField( dest.x, dest.y );
               if ( fld->building )
                  movecost = maxmalq;
               else
                  movecost = fld->getmovemalus( vehicle );
               checkWind = false;
            }

   } else
      if ( dest.getNumericalHeight() >= 0 ) {
        // moving out of container
        int mm = vehicle->getMap()->getField( start.x, start.y )->getContainer()->vehicleUnloadSystem( vehicle->typ, dest.getBitmappedHeight() )->movecost;
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
   
   if ( checkHemming )
      for (int c = 0; c < sidenum; c++) {
         int x = dest.x;
         int y = dest.y;
         getnextfield( x,  y, c );
         tfield* fld = vehicle->getMap()->getField ( x, y );
         if ( fld ) {
           int d = (c - direc);

           if (d >= sidenum)
              d -= sidenum;

           if (d < 0)
              d += sidenum;

           tfield* fld = vehicle->getMap()->getField(x,y);
           if ( fld->vehicle && dest.getNumericalHeight() >= 0 ) {
              if ( vehicle->getMap()->getPlayer(vehicle).diplomacy.isHostile( fld->vehicle->getOwner() ) )
                 if ( attackpossible28(fld->vehicle,vehicle, NULL, dest.getBitmappedHeight() ))
                    movecost += movemalus[d];
              
           }
         }
      }

    /*******************************/
    /*    Wind calculation         */
    /*******************************/
   if ( wm && checkWind && direc >= 0 && direc < 5 )
      if (dest.getNumericalHeight() >= 4 && dest.getNumericalHeight() <= 6 &&
          start.getNumericalHeight() >= 4 && start.getNumericalHeight() <= 6 &&
          actmap->weather.windSpeed  ) {
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

void setupNextTech( Player& player )
{
   if ( player.research.goal ) {
      if ( player.research.techResearched( player.research.goal->id )) {
         chooseTechnology( player );
      } else {
         list<const Technology*> techs;
         if ( player.research.goal->eventually_available( player.research, &techs ))
            player.research.activetechnology = *techs.begin();
         else
            chooseTechnology( player );
      }
   } else
      chooseTechnology( player );

   
}

bool anyTechAvailable( const Player& player )
{
   for (int i = 0; i < technologyRepository.getNum(); i++) {
      const Technology* tech = technologyRepository.getObject_byPos( i );
      if ( tech ) {
         ResearchAvailabilityStatus a = player.research.techAvailable ( tech );
         if ( a == Available )
            return true;
      }
   }
   return false;
}



void researchCheck( Player& player )
{
   // we have no research at all set up in the map
   if ( !anyTechAvailable( player ) ) // && player.research.developedTechnologies.empty() )
      return;

   Research& research = player.research;
   if (research.activetechnology == NULL && research.progress ) 
      setupNextTech( player );
      
   if ( research.activetechnology )
      if( find ( research.developedTechnologies.begin(), research.developedTechnologies.end(), research.activetechnology->id ) != research.developedTechnologies.end()) {
         research.progress = 0;
         setupNextTech( player );
      }
      
   while ( research.activetechnology  &&  (research.progress >= research.activetechnology->researchpoints)) {
      int mx = research.progress - research.activetechnology->researchpoints;

      showtechnology( research.activetechnology );
      
      if ( research.activetechnology )
         logtoreplayinfo ( rpl_techResearched, research.activetechnology->id, player.getPosition() );

      NewVehicleTypeDetection pfzt ( player.getParentMap() );

      research.addtechnology();

      pfzt.evalbuffer ();

      research.progress = mx;

      setupNextTech( player );

   }
}

