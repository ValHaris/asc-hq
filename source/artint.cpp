/*! \file artint.cpp
    \brief The artificial intelligence of ASC. 
*/


//     $Id: artint.cpp,v 1.58 2001-02-06 16:27:40 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.57  2001/02/04 21:26:52  mbickel
//      The AI status is written to savegames -> new savegame revision
//      Lots of bug fixes
//
//     Revision 1.56  2001/02/01 22:48:25  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.55  2001/01/28 14:04:00  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.54  2001/01/25 23:44:50  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.53  2001/01/24 14:42:45  mbickel
//      Fixed a crash in the AI
//
//     Revision 1.52  2001/01/24 11:53:09  mbickel
//      Fixed some compilation problems with gcc
//
//     Revision 1.51  2001/01/23 21:05:05  mbickel
//      Speed up of AI
//      Lot of bugfixes in AI
//      Moved Research to own files (research.*)
//      Rewrote storing of developed technologies
//      Some cleanup and documentation
//
//     Revision 1.50  2001/01/21 12:48:33  mbickel
//      Some cleanup and documentation
//
//     Revision 1.49  2001/01/19 13:33:45  mbickel
//      The AI now uses hemming
//      Several bugfixes in Vehicle Actions
//      Moved all view calculation to viewcalculation.cpp
//      Mapeditor: improved keyboard support for item selection
//
//     Revision 1.48  2001/01/11 15:28:01  mbickel
//      AI: conquering units try to move on fields hidden to the enemy
//
//     Revision 1.47  2001/01/04 15:13:23  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.46  2000/12/31 15:25:24  mbickel
//      The AI now conqueres neutral buildings
//      Removed "reset password" buttons when starting a game
//
//     Revision 1.45  2000/12/28 16:58:35  mbickel
//      Fixed bugs in AI
//      Some cleanup
//      Fixed crash in building construction
//
//     Revision 1.44  2000/12/26 14:45:58  mbickel
//      Made ASC compilable (and runnable) with Borland C++ Builder
//
//     Revision 1.43  2000/11/29 11:18:36  mbickel
//      Mapeditor compiles with Watcom again
//
//     Revision 1.42  2000/11/26 14:39:01  mbickel
//      Added Project Files for Borland C++
//      Some modifications to compile source with BCC
//
//     Revision 1.41  2000/11/21 20:26:50  mbickel
//      Fixed crash in tsearchfields (used by object construction for example)
//      AI improvements
//      configure.in: added some debug output
//                    fixed broken check for libbz2
//
//     Revision 1.40  2000/11/15 19:28:32  mbickel
//      AI improvements
//
//     Revision 1.39  2000/11/14 20:36:37  mbickel
//      The AI can now use supply vehicles
//      Rewrote objecttype IO routines to make the structure independant of
//       the memory layout
//
//     Revision 1.38  2000/11/11 11:05:15  mbickel
//      started AI service functions
//
//     Revision 1.37  2000/11/08 19:30:53  mbickel
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <algorithm>
#include <memory>

#include "artint.h"

#include "typen.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "misc.h"
#include "newfont.h"
#include "events.h"
#include "spfst.h"
#include "dlg_box.h"
#include "stack.h"
#include "missions.h"
#include "controls.h"
#include "dialog.h"
#include "gamedlg.h"
#include "attack.h"
#include "gameoptions.h"
#include "astar2.h"
#include "sg.h"

#include "building_controls.h"
#include "viewcalculation.h"


const int value_armorfactor = 100;
const int value_weaponfactor = 3000;

const int ccbt_repairfacility = 200;    //  basic threatvalues for buildings
const int ccbt_hq = 10000;
const int ccbt_recycling = 50;
const int ccbt_training = 150;

const int attack_unitdestroyed_bonus = 90;


void nop ( void )
{
}

int compareinteger ( const void* op1, const void* op2 )
{
   const int* a = (const int*) op1;
   const int* b = (const int*) op2;
   return *a > *b;
}


 class HiddenAStar : public AStar {
       AI* ai;
    protected:
       virtual int getMoveCost ( int x1, int y1, int x2, int y2, const pvehicle vehicle )
       {
          int cost = AStar::getMoveCost ( x1, y1, x2, y2, vehicle );
          int visibility = getfield ( x2, y2 )->visible;
          for ( int i = 0; i< 8; i++ )
             if ( getdiplomaticstatus2 ( i*8, ai->getPlayer()*8 ) != capeace ) {
                int v = (visibility >> ( 2*i)) & 3;
                if ( v >= visible_now )
                   cost += 12;
             }

          return cost;
       };
    public:
       HiddenAStar ( AI* _ai, pvehicle veh ) : AStar ( _ai->getMap(), veh ), ai ( _ai ) {};
 };

 class StratAStar : public AStar {
       AI* ai;
    protected:
       virtual int getMoveCost ( int x1, int y1, int x2, int y2, const pvehicle vehicle )
       {
          int cost = AStar::getMoveCost ( x1, y1, x2, y2, vehicle );
          if ( getfield ( x2, y2 )->vehicle && beeline ( vehicle->xpos, vehicle->ypos, x2, y2) < vehicle->getMovement())
             cost += 2;
          return cost;
       };
    public:
       StratAStar ( AI* _ai, pvehicle veh ) : AStar ( _ai->getMap(), veh ), ai ( _ai ) {};
 };

AI :: AI ( pmap _map, int _player ) : activemap ( _map ) , sections ( this )
{

   player = _player;

   _isRunning = false;
   fieldThreats = NULL;

   reset(); 
   ReplayMapDisplay* r = new ReplayMapDisplay ( &defaultMapDisplay );
   r->setCursorDelay (CGameOptions::Instance()->replayspeed + 30 );
   rmd = r;
   mapDisplay = rmd;
}

void AI :: reset ( void )
{
   maxTrooperMove = 0;  
   maxTransportMove = 0;
   maxUnitMove = 0;

   for ( int i= 0; i < 8; i++ )
      maxWeapDist[i] = -1;  
   baseThreatsCalculated = 0;

   if ( fieldThreats )
      delete[] fieldThreats;

   fieldThreats = NULL;
   fieldNum = 0;

   config.wholeMapVisible = 1;
   config.lookIntoTransports = 1;
   config.lookIntoBuildings = 1;
   config.aggressiveness  = 2;
   config.damageLimit = 70;
   config.resourceLimit = Resources ( 0, 5, 20 );
   config.ammoLimit= 10;
   config.maxCaptureTime = 15;

   sections.reset();
}


AI :: ServiceOrder :: ServiceOrder ( AI* _ai, VehicleService::Service _requiredService, int UnitID, int _pos )
{
   ai = _ai;
   targetUnitID = UnitID;
   requiredService = _requiredService;
   serviceUnitID = 0;
   position = _pos;
   time = ai->getMap()->time;
}

AI :: ServiceOrder :: ServiceOrder ( AI* _ai, tnstream& stream )
{
  ai = _ai;
  read ( stream );
}


void AI :: ServiceOrder :: read ( tnstream& stream )
{
  int version = stream.readInt( );
  targetUnitID = stream.readInt (  );
  serviceUnitID = stream.readInt (  );
  position = stream.readInt (  );
  time.abstime = stream.readInt (  );
}

void AI :: ServiceOrder :: write ( tnstream& stream ) const
{
  stream.writeInt( 10000 );
  stream.writeInt ( targetUnitID );
  stream.writeInt ( serviceUnitID );
  stream.writeInt ( position );
  stream.writeInt ( time.abstime );
}

AI :: ServiceOrder :: ~ServiceOrder (  )
{
   pvehicle serv = getServiceUnit();
   if ( serv )
      serv->aiparam[ai->getPlayer()]->resetTask();
}

void AI :: issueServices ( )
{
   remove_if ( serviceOrders.begin(), serviceOrders.end(), ServiceOrder::targetDestroyed );

   for ( tmap::Player::VehicleList::iterator vi = getMap()->player[ getPlayer() ].vehicleList.begin(); vi != getMap()->player[ getPlayer() ].vehicleList.end(); vi++ ) {
      pvehicle veh = *vi;
      if ( veh->damage > config.damageLimit )
         serviceOrders.push_back ( ServiceOrder ( this, VehicleService::srv_repair, veh->networkid ) );

      for ( int i = 0; i< resourceTypeNum; i++ )
         if ( veh->typ->movement[ log2( veh->height )] ) // stationary units are ignored
            if ( veh->tank.resource(i) < veh->typ->tank.resource(i) * config.resourceLimit.resource(i) / 100 )
               serviceOrders.push_back ( ServiceOrder ( this, VehicleService::srv_resource, veh->networkid, i));

      for ( int w = 0; w< veh->typ->weapons->count; w++ )
         if ( veh->typ->weapons->weapon[w].count )
            if ( veh->ammo[w] <= veh->typ->weapons->weapon[w].count * config.ammoLimit / 100 )
               serviceOrders.push_back ( ServiceOrder ( this, VehicleService::srv_ammo, veh->networkid, w));

   }
}


pbuilding AI :: findServiceBuilding ( const ServiceOrder& so )
{
   pvehicle veh = so.getTargetUnit();

   AStar astar ( getMap(), veh );
   astar.findAllAccessibleFields (  );

   pbuilding bestBuilding = NULL;
   int bestDistance = maxint;

   pbuilding bestBuilding_p = NULL;
   int bestDistance_p = maxint;

   for ( tmap::Player::BuildingList::iterator bi = getMap()->player[ getPlayer() ].buildingList.begin(); bi != getMap()->player[ getPlayer() ].buildingList.end(); bi++ ) {
      pbuilding bld = *bi;
      if ( bld->getEntryField()->a.temp ) {
         // the unit can reach the building


         bool loadable = false;
         if ( bld->vehicleloadable ( veh ))
            loadable = true;
         else
            for ( int i = 0; i < 8; i++ )
                if ( veh->typ->height & ( 1 << i))
                   if ( bld->vehicleloadable ( veh, 1 << i ))
                      loadable = true;

         if ( loadable ) {
            int fullfillableServices = 0;
            int partlyFullfillabelServices = 0;
            switch ( so.requiredService ) {
               case VehicleService::srv_repair : if ( bld->canRepair() ) {
                                                    int mr =  bld->getMaxRepair( veh );
                                                    if ( mr == 0 )
                                                       fullfillableServices++;

                                                    if ( mr < veh->damage )
                                                       partlyFullfillabelServices++;

                                                 }
                                                 break;
               case VehicleService::srv_resource:  {
                                                     Resources needed =  veh->typ->tank - veh->tank;
                                                     Resources avail = bld->getResource ( needed, 1 );
                                                     if ( avail < needed ) {
                                                        Resources plus;
                                                        bld->getresourceplus ( -1, &plus, 1 );
                                                        avail += plus*2;
                                                     }
                                                     int missing = 0;
                                                     int pmissing = 0;
                                                     for ( int r = 0; r < resourceTypeNum; r++ ) {
                                                        if( needed.resource(r) * 75 / 100 > avail.resource(r) )
                                                           missing ++;

                                                        if( needed.resource(r) * 10 / 100 > avail.resource(r) )
                                                           pmissing ++;
                                                     }
                                                     if ( missing == 0 )
                                                        fullfillableServices++;

                                                     if ( pmissing == 0)
                                                       partlyFullfillabelServices++;
                                                 }
                                                 break;
               case VehicleService::srv_ammo :  {
                                                   int missing = 0;
                                                   int pmissing = 0;
                                                   int ammoNeeded[waffenanzahl];
                                                   for ( int t = 0; t < waffenanzahl; t++ )
                                                      ammoNeeded[t] = 0;

                                                   for ( int i = 0; i < veh->typ->weapons->count; i++ )
                                                      if ( veh->typ->weapons->weapon[i].requiresAmmo() )
                                                         ammoNeeded[ veh->typ->weapons->weapon[i].getScalarWeaponType() ] += veh->typ->weapons->weapon[i].count - veh->ammo[i];

                                                   Resources needed;
                                                   for ( int  j = 0; j < waffenanzahl; j++ ) {
                                                       int n = ammoNeeded[j] - bld->munition[j];
                                                       if ( n > 0 )
                                                          if ( bld->typ->special & cgammunitionproductionb ) {
                                                             for ( int r = 0; r < resourceTypeNum; r++ )
                                                                needed.resource(r) += (n+4)/5 * cwaffenproduktionskosten[j][r];
                                                          } else
                                                             missing++;
                                                   }
                                                   Resources avail = bld->getResource ( needed, 1 );
                                                   if ( avail < needed ) {
                                                      Resources plus;
                                                      bld->getresourceplus ( -1, &plus, 1 );
                                                      avail += plus*2;
                                                   }

                                                   for ( int r = 0; r < resourceTypeNum; r++ ) {
                                                      if ( avail.resource(r) < needed.resource (r) )
                                                         missing++;
                                                      if ( avail.resource(r) <= needed.resource (r)/3 )
                                                         pmissing++;
                                                   }

                                                   if ( missing == 0 )
                                                       fullfillableServices++;

                                                   if ( pmissing == 0)
                                                     partlyFullfillabelServices++;

                                                 }
                                                 break;

            };

            if ( fullfillableServices ) {
               MapCoordinate entry = bld->getEntry ( );
               AStar ast ( getMap(), veh );
               vector<MapCoordinate> path;
               ast.findPath (  path, entry.x, entry.y );
               if ( path.size() < bestDistance ) {
                  bestDistance = path.size();
                  bestBuilding = bld;
               }
            } else
               if ( partlyFullfillabelServices ) {
                  MapCoordinate entry = bld->getEntry ( );
                  AStar ast ( getMap(), veh );
                  vector<MapCoordinate> path;
                  ast.findPath (  path, entry.x, entry.y );
                  if ( path.size() < bestDistance_p ) {
                     bestDistance_p = path.size();
                     bestBuilding_p = bld;
                  }
               }
         }
      }
   }

   if ( bestBuilding && (bestDistance < bestDistance_p*3))
      return bestBuilding;
   else
      return bestBuilding_p;
}

int AI::ServiceOrder::possible ( pvehicle supplier )
{
   bool result = false;

   VehicleService vs ( NULL, NULL);
   vs.fieldSearch.bypassChecks.distance = true;
   vs.fieldSearch.bypassChecks.height   = true;
   vs.fieldSearch.init ( supplier, NULL );
   vs.fieldSearch.checkVehicle2Vehicle ( getTargetUnit(), getTargetUnit()->xpos, getTargetUnit()->ypos );

   VehicleService::TargetContainer::iterator i = vs.dest.find ( targetUnitID );
   if ( i != vs.dest.end() ) {
      VehicleService::Target target = i->second;
      int serviceAmount = 0;
      for ( int j = 0; j < target.service.size(); j++ ) {
         serviceAmount += target.service[j].maxPercentage;
         if ( target.service[j].type == requiredService ) {
            bool enoughResources = false;
            int requiredAmount = target.service[j].maxAmount - target.service[j].curAmount;
            if ( requiredAmount > 0 ) {
               if ( requiredService == VehicleService::srv_repair ) {
                  if ( target.service[j].maxAmount - target.service[j].curAmount > 5 )
                     enoughResources = true;
               } else
                  if ( target.service[j].targetPos == position ) {  // is this stuff the one we need
                     if ( requiredService == VehicleService::srv_resource ) {
                        if ( target.service[j].targetPos == Resources::Fuel ) { // fuel  ; 30 fields requiredForHome
                            if ( requiredAmount < target.service[j].orgSourceAmount - supplier->typ->fuelConsumption * 30 )
                               enoughResources = true;
                        } else {
                            if ( getTargetUnit()->aiparam[ai->getPlayer()]->job == AiParameter::job_supply ) {
                               if ( target.service[j].orgSourceAmount > requiredAmount )
                                  enoughResources = true;
                            } else
                               if ( target.service[j].maxPercentage > 80 )
                                  enoughResources = true;
                        }
                     }
                     if ( requiredService == VehicleService::srv_ammo ) {
                        if ( getTargetUnit()->aiparam[ai->getPlayer()]->job == AiParameter::job_supply ) {
                           if ( target.service[j].orgSourceAmount > requiredAmount )
                              enoughResources = true;
                        } else
                           if ( target.service[j].maxPercentage > 80 )
                              enoughResources = true;
                     }
                  }

               if ( enoughResources )
                  result = true;
            }
         }
      }

      if( result )
         return serviceAmount;
   }
   return 0;
}


bool AI::ServiceOrder::execute1st ( pvehicle supplier )
{
   bool result = false;

   pvehicle targ = getTargetUnit();
   MapCoordinate3D meet;
   int xy_dist = maxint;
   int z_dist = maxint;
   int currentHeight = log2( supplier->height );
   for ( int h = 0; h < 8; h++ )
      if ( supplier->typ->height & ( 1<<h))
         for ( int i = 0; i < sidenum; i++ ) {
             int x = targ->xpos;
             int y = targ->ypos;
             getnextfield ( x, y, i );
             pfield fld = getfield ( x, y );
             if ( fld && fieldaccessible ( fld, supplier, 1<<h ) == 2 && !fld->building && !fld->vehicle ) {
                int d = beeline ( x, y, supplier->xpos, supplier->ypos);
                AStar ast ( ai->getMap(), supplier );
                vector<MapCoordinate> path;
                ast.findPath(  path, x, y );
                if ( path.size() ) {
                   if ( abs ( currentHeight - h) < z_dist || ( abs( currentHeight - h) == z_dist && d < xy_dist )) {
                      TemporaryContainerStorage tus ( supplier );
                      supplier->xpos = x;
                      supplier->ypos = y;
                      supplier->height = 1 << h;

                      VehicleService vs ( NULL, NULL);
                      vs.fieldSearch.init ( supplier, NULL );
                      vs.fieldSearch.checkVehicle2Vehicle ( targ, targ->xpos, targ->ypos );

                      VehicleService::TargetContainer::iterator i = vs.dest.find ( targetUnitID );
                      if ( i != vs.dest.end() ) {
                         VehicleService::Target target = i->second;
                         for ( int j = 0; j < target.service.size(); j++ )
                            if ( target.service[j].type == requiredService )
                               result = true;
                      }

                      tus.restore();

                      if ( result ) {
                         meet.x = x;
                         meet.y = y;
                         meet.z = h;
                         xy_dist = d;
                         z_dist = abs ( currentHeight - h);
                      }

                   }
                }
             }
         }


   if ( xy_dist < maxint ) {
      supplier->aiparam[ai->getPlayer()]->dest = meet;
      supplier->aiparam[ai->getPlayer()]->task = AiParameter::tsk_move;
      supplier->aiparam[ai->getPlayer()]->dest_nwid = targ->networkid;
      setServiceUnit ( supplier );
      return true;
   } else
      return false;
}


bool AI::ServiceOrder::timeOut ( )
{
  tgametime t = time;
  t.a.turn += 2;
  return ( t.abstime <= ai->getMap()->time.abstime );
}


bool AI::ServiceOrder::valid (  ) const
{
   switch ( requiredService ) {
      case VehicleService::srv_repair     : return getTargetUnit()->damage > 0;
      case VehicleService::srv_ammo       : return getTargetUnit()->ammo[position] < getTargetUnit()->typ->weapons[position].count;
      case VehicleService::srv_resource   : return getTargetUnit()->tank.resource(position) < getTargetUnit()->typ->tank.resource(position);
      default: return false;
   }
}

bool AI::ServiceOrder::invalid ( const ServiceOrder& so )
{
   return !so.valid();
}


void AI::removeServiceOrdersForUnit ( const pvehicle veh )
{
   remove_if ( serviceOrders.begin(), serviceOrders.end(), ServiceTargetEquals( veh ) );
}


bool AI :: runUnitTask ( pvehicle veh )
{
   if ( veh->aiparam[getPlayer()]->task == AiParameter::tsk_move ) {
      bool moveIntoBuildings = false;
      if ( veh->aiparam[getPlayer()]->job == AiParameter::job_conquer )
         moveIntoBuildings = true;

      moveUnit ( veh, veh->aiparam[getPlayer()]->dest, moveIntoBuildings );
      if ( veh->xpos == veh->aiparam[getPlayer()]->dest.x && veh->ypos == veh->aiparam[getPlayer()]->dest.y )
         return true;
      else
         return false;
   }

   return false;
}


void AI :: runServiceUnit ( pvehicle supplyUnit )
{
   bool destinationReached = false;
   typedef multimap<float,ServiceOrder*> ServiceMap;
   ServiceMap serviceMap;

   for ( ServiceOrderContainer::iterator i = serviceOrders.begin(); i != serviceOrders.end(); i++ ) {
       if ( !i->getServiceUnit() ) {
          int poss = i->possible( supplyUnit );
          if ( poss ) {
             float f =  i->getTargetUnit()->aiparam[getPlayer()]->getValue() * poss/100 / beeline( i->getTargetUnit() ,supplyUnit );
             serviceMap.insert(make_pair(f,&(*i)));
          }
  /*
          veh->aiparam[getPlayer()]->task = AiParameter::tsk_serviceRetreat;
          pbuilding bld = findServiceBuilding( **i );
          if ( bld )
             veh->aiparam[ getPlayer() ]->dest = bld->getEntry ( );
  */
       }
   }
   for ( ServiceMap::reverse_iterator ri = serviceMap.rbegin(); ri != serviceMap.rend(); ri++ ) {
      if ( (*ri).second->execute1st( supplyUnit ) ) {
         destinationReached = runUnitTask ( supplyUnit );
         break;
      }
   }

   if ( destinationReached ) {
      VehicleService vs ( mapDisplay, NULL );
      if ( !vs.available ( supplyUnit ))
         displaymessage ("AI :: runServiceUnit ; inconsistency in VehicleService.availability",1 );

      vs.execute ( supplyUnit, -1, -1, 0, -1, -1 );
      int target = supplyUnit->aiparam[getPlayer()]->dest_nwid;
      VehicleService::TargetContainer::iterator i = vs.dest.find ( target );
      if ( i != vs.dest.end() ) {
         vs.fillEverything ( target, true );
         removeServiceOrdersForUnit ( getMap()->getUnit(target) );

         // search for next unit to be serviced
         runServiceUnit( supplyUnit );
      } else
         displaymessage ("AI :: runServiceUnit ; inconsistency in VehicleService.execution level 0",1 );

   }
}


AI::AiResult AI :: executeServices ( )
{
  // removing all service orders for units which no longer exist
  removeServiceOrdersForUnit ( NULL );
  remove_if ( serviceOrders.begin(), serviceOrders.end(), ServiceOrder::invalid );

  AiResult res;

  for ( tmap::Player::VehicleList::iterator vi = getMap()->player[ getPlayer() ].vehicleList.begin(); vi != getMap()->player[ getPlayer() ].vehicleList.end(); vi++ ) {
      pvehicle veh = *vi;
      checkKeys();
      if ( veh->aiparam[getPlayer()]->job == AiParameter::job_supply )
         runServiceUnit ( veh );
  }

  for ( ServiceOrderContainer::iterator i = serviceOrders.begin(); i != serviceOrders.end(); i++ ) {
      if ( i->timeOut() ) {
         pvehicle veh = i->getTargetUnit();
         veh->aiparam[getPlayer()]->task = AiParameter::tsk_serviceRetreat;
         pbuilding bld = findServiceBuilding( *i );
         if ( bld )
            veh->aiparam[ getPlayer() ]->dest = bld->getEntry ( );
         else {
            // displaymessage("warning: no service building found found for unit %s - %d!",1, veh->typ->description, veh->typ->id);
         }

      }
  }

  for ( tmap::Player::VehicleList::iterator vi = getMap()->player[ getPlayer() ].vehicleList.begin(); vi != getMap()->player[ getPlayer() ].vehicleList.end(); vi++ ) {
     pvehicle veh = *vi;
     checkKeys();

     if ( veh->aiparam[getPlayer()]->task == AiParameter::tsk_serviceRetreat ) {
        moveUnit ( veh, MapCoordinate ( veh->aiparam[ getPlayer() ]->dest.x, veh->aiparam[ getPlayer() ]->dest.y ), true);
        if ( veh->xpos == veh->aiparam[ getPlayer() ]->dest.x && veh->ypos == veh->aiparam[ getPlayer() ]->dest.y ) {
           VehicleService vc ( mapDisplay, NULL );
           pfield fld = getfield ( veh->xpos, veh->ypos );
           if ( fld->building ) {
              MapCoordinate mc = fld->building->getEntry();
              vc.execute ( NULL, mc.x, mc.y, 0, -1, -1 );
           } else
              if ( fld->vehicle ) {
                 if ( !vc.available( fld->vehicle ))
                    displaymessage("AI :: ExecuteService; inconsistency vehicleService.available ",1 );
                 else
                    vc.execute ( fld->vehicle, -1, -1, 0, -1, -1 );
              }

           if ( vc.getStatus () == 2 ) {
              if ( vc.dest.find ( veh->networkid ) != vc.dest.end() )
                 vc.fillEverything ( veh->networkid, true );
              else
                 displaymessage ( "AI :: executeServices / Vehicle cannot be serviced (1) ", 1);
           }
           else
              displaymessage ( "AI :: executeServices / Vehicle cannot be serviced (2) ", 1);


           for ( ServiceOrderContainer::iterator i = serviceOrders.begin(); i != serviceOrders.end(); i++ ) {
               if ( i->getTargetUnit() == veh )
                  i = serviceOrders.erase ( i );
               else
                  i++;
           }
           veh->aiparam[getPlayer()]->task = AiParameter::tsk_nothing;
        }
     }
  }

  for ( ServiceOrderContainer::iterator i = serviceOrders.begin(); i != serviceOrders.end(); i++ ) {
      if ( !i->timeOut() && i->requiredService == VehicleService::srv_repair ) {
         pvehicle veh = i->getTargetUnit();
         veh->aiparam[getPlayer()]->task = AiParameter::tsk_serviceRetreat;
         pbuilding bld = findServiceBuilding( *i );
         if ( bld )
            veh->aiparam[ getPlayer() ]->dest = bld->getEntry ( );
         else {
            displaymessage("warning: no service building found found for unit %s - %d!",1, veh->typ->description, veh->typ->id);
         }

      }
  }

  return res;

}

float AI :: getCaptureValue ( const pbuilding bld, const pvehicle veh  )
{
   HiddenAStar ast ( this, veh );
   HiddenAStar::Path path;
   ast.findPath ( path, bld->getEntry().x, bld->getEntry().y );
   return getCaptureValue ( bld, ast.getTravelTime() );
}


void AI :: BuildingCapture :: write ( tnstream& stream ) const
{
  stream.writeInt( 20 );
  stream.writeInt ( state );
  stream.writeInt ( unit );
  for ( vector<int>::const_iterator i = guards.begin(); i  != guards.end(); i++ ) {
     stream.writeInt ( 1 );
     stream.writeInt ( *i );
  }
  stream.writeInt ( 0 );
  stream.writeFloat ( captureValue );
  stream.writeInt ( nearestUnit );
}

void AI :: BuildingCapture :: read ( tnstream& stream )
{
  int version = stream.readInt();
  state = BuildingCaptureState( stream.readInt ( ));
  unit = stream.readInt ( );
  int i = stream.readInt ();
  while ( i ) {
     guards.push_back ( stream.readInt() );
     i = stream.readInt();
  }
  captureValue = stream.readFloat ();
  nearestUnit = stream.readInt ();
}



class  SearchReconquerBuilding : public tsearchfields {
                                protected:
                                   AI& ai;
                                   pbuilding buildingToCapture;
                                   int mode;        // (1): nur fusstruppen; (2): 1 und transporter; (3): 2 und geb„ude
                                   vector<pvehicle> enemyUnits; // that can conquer the building
                                   float getThreatValueOfUnit ( pvehicle veh );
                                public:
                                   void testfield ( );
                                   bool returnresult ( );
                                   void unitfound ( pvehicle eht );
                                   bool canUnitCapture ( pvehicle veh );
                                   SearchReconquerBuilding ( AI& _ai, pbuilding bld ) : tsearchfields ( _ai.getMap() ), ai ( _ai ), buildingToCapture ( bld ), mode(3) {};
                                };

bool SearchReconquerBuilding :: returnresult( )
{
  return !enemyUnits.empty();
}


void         SearchReconquerBuilding :: unitfound(pvehicle     eht)
{
  enemyUnits.push_back ( eht );
  buildingToCapture->aiparam[ai.getPlayer()]->setAdditionalValue ( buildingToCapture->aiparam[ai.getPlayer()]->getValue() );
}

bool SearchReconquerBuilding :: canUnitCapture( pvehicle eht )
{
   return ((eht->functions & cf_conquer ))
           && fieldaccessible ( buildingToCapture->getEntryField(), eht) == 2 ;

}

void         SearchReconquerBuilding :: testfield(void)
{
   if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) {
      pvehicle eht = getfield(xp,yp)->vehicle;
      // pbuilding bld = getfield(xp,yp)->building;
      if ( eht )
         if (getdiplomaticstatus(eht->color) != capeace)
            if ( canUnitCapture ( eht )) {
               VehicleMovement vm ( NULL );
               if ( vm.available ( eht )) {
                  vm.execute ( eht, -1, -1, 0 , -1, -1 );
                  if ( vm.reachableFields.isMember ( startx, starty ))
                     unitfound(eht);
               }
            }
            else
               if (mode >= 2)
                  if (eht->typ->loadcapacity > 0)
                     for ( int w = 0; w <= 31; w++)
                        if ( eht->loading[w] )
                           if ( canUnitCapture ( eht->loading[w] ))
                              if (eht->typ->movement[log2(eht->height)] + eht->loading[w]->typ->movement[log2(eht->loading[w]->height)] >= beeline(xp,yp,startx,starty))
                                 unitfound(eht);


/*
      if ( bld )
         if ( mode >= 3 )
            if (getdiplomaticstatus(bld->color) != capeace)
               for ( int w = 0; w <= 31; w++)
                  if ( bld->loading[w] )
                     if ( canUnitCapture ( bld->loading[w] ))
                        if ( bld->loading[w]->typ->movement[log2(bld->loading[w]->height)] <= beeline(xp,yp,startx,starty))
                           unitfound ( bld->loading[w] );

*/
   }
}


float AI :: getCaptureValue ( const pbuilding bld, int traveltime  )
{
   return float(bld->aiparam[getPlayer()]->getValue()) / float(traveltime+1);
}


bool AI :: checkReConquer ( pbuilding bld, pvehicle veh )
{
   SearchReconquerBuilding srb ( *this, bld );
   srb.initsearch ( bld->getEntry().x, bld->getEntry().y, (maxTransportMove + maxUnitMove/2) / maxmalq + 1, 1 );
   srb.startsearch();
   bool enemyNear = srb.returnresult();

   if ( enemyNear ) {
      float f = 0;
      for ( int i = 0; i < 32; i++ )
         if ( bld->loading[i] )
            if ( bld->loading[i]->getMovement() )
               f += bld->loading[i]->aiparam[ getPlayer()]->getValue();

      //! if the units inside the building are more worth than the own unit, capture the building regardless of whether it can be recaptured
      if ( f > veh->aiparam[getPlayer()]->getValue())
         return false;
      else
         return true;
   } else
      return false;

}

void AI :: conquerBuilding ( pvehicle veh )
{

}

void AI :: checkConquer( )
{
   displaymessage2("check for capturing enemy towns ... ");

   for ( tmap::Player::VehicleList::iterator vi = getMap()->player[ getPlayer() ].vehicleList.begin(); vi != getMap()->player[ getPlayer() ].vehicleList.end(); vi++ ) {
      pvehicle veh = *vi;
      if ( veh->aiparam[getPlayer()]->job == AiParameter::job_conquer && veh->aiparam[getPlayer()]->task == AiParameter::tsk_nothing ) {

         HiddenAStar ast ( this, veh );
         ast.findAllAccessibleFields( veh->typ->movement[log2(veh->height)] * config.maxCaptureTime );

         pbuilding bestBuilding = NULL;
         float bestBuildingCaptureValue = minfloat;

         for ( int c = 0; c <= 8; c++ )
            if ( getMap()->player[c].exist() && getdiplomaticstatus2 ( getPlayer()*8, c*8 ) == cawar ) {
               for ( tmap::Player::BuildingList::iterator bi = getMap()->player[ c ].buildingList.begin(); bi != getMap()->player[ c ].buildingList.end(); bi++ ) {
                  pbuilding bld = *bi;
                  if ( bld->getEntryField()->a.temp  && buildingCapture[bld->getEntry()].state == BuildingCapture::conq_noUnit ) {
                     float v = getCaptureValue ( bld, veh );
                     if ( v > bestBuildingCaptureValue ) {
                        bestBuildingCaptureValue = v;
                        bestBuilding = bld;
                     }
                  }
               }
            }

         if ( bestBuilding ) {
            BuildingCapture& bc = buildingCapture[ bestBuilding->getEntry() ];

            bc.captureValue = bestBuildingCaptureValue;
            bc.state = BuildingCapture::conq_conqUnit;
            bc.unit = veh->networkid;

            veh->aiparam[getPlayer()]->job = AiParameter::job_conquer;
            veh->aiparam[getPlayer()]->task = AiParameter::tsk_move;
            veh->aiparam[getPlayer()]->dest = bestBuilding->getEntry();
         }


      }
   }


   // cycle through all neutral buildings which can be conquered by any unit
   displaymessage2("check for capturing neutral towns ... ");

   typedef vector<pbuilding> BuildingList;
   BuildingList buildingList;

   for ( tmap::Player::BuildingList::iterator bi = getMap()->player[8].buildingList.begin(); bi != getMap()->player[8].buildingList.end(); bi++ ) {
      pbuilding bld = *bi;
      if ( buildingCapture[ bld->getEntry() ].state == BuildingCapture::conq_noUnit ) {
         buildingList.push_back ( bld );

         int travelTime = maxint;
         pvehicle conquerer = NULL;

         for ( tmap::Player::VehicleList::iterator vi = getMap()->player[ getPlayer() ].vehicleList.begin(); vi != getMap()->player[ getPlayer() ].vehicleList.end(); vi++ ) {
            pvehicle veh = *vi;
            if ( veh->aiparam[getPlayer()]->job != AiParameter::job_conquer || veh->aiparam[getPlayer()]->task == AiParameter::tsk_nothing)
               if ( fieldaccessible ( bld->getEntryField(), veh ) == 2 ) {
                  HiddenAStar ast ( this, veh );
                  vector<MapCoordinate> path;
                  ast.findPath ( path, bld->getEntry().x, bld->getEntry().y );
                  int time = ast.getTravelTime();
                  if ( time >= 0 && time < travelTime ) {
                     conquerer = veh;
                     travelTime = time;
                  }
               }
         }
         if ( conquerer ) {
            BuildingCapture& bc = buildingCapture[ bld->getEntry() ];

            bc.nearestUnit  = conquerer->networkid;
            if ( travelTime >= 0 )
               bc.captureValue = getCaptureValue ( bld, travelTime );
            else
               bc.captureValue = 0;
         }

      }
   }

   sort ( buildingList.begin(), buildingList.end(), BuildingValueComp ( this ));

   for ( BuildingList::iterator i = buildingList.begin(); i != buildingList.end(); i++ ) {

      int travelTime = maxint;
      pvehicle conquerer = NULL;

      for ( tmap::Player::VehicleList::iterator vi = getMap()->player[ getPlayer() ].vehicleList.begin(); vi != getMap()->player[ getPlayer() ].vehicleList.end(); vi++ ) {
         pvehicle veh = *vi;
         if ( veh->aiparam[getPlayer()]->job != AiParameter::job_conquer || veh->aiparam[getPlayer()]->task == AiParameter::tsk_nothing )
            if ( fieldaccessible ( (*i)->getEntryField(), veh ) == 2 ) {
               HiddenAStar ast ( this, veh ) ;
               vector<MapCoordinate> path;
               ast.findPath ( path, (*i)->getEntry().x, (*i)->getEntry().y );
               int time = ast.getTravelTime();
               if ( time >= 0 && time < travelTime ) {
                  conquerer = veh;
                  travelTime = time;
               }
            }
      }

      if ( conquerer ) {
        BuildingCapture& bc = buildingCapture[ (*i)->getEntry() ];
        if ( conquerer->functions & cf_conquer )
           bc.state = BuildingCapture::conq_conqUnit;
        else
           bc.state = BuildingCapture::conq_unitNotConq;
        bc.unit = conquerer->networkid;
        conquerer->aiparam[getPlayer()]->job = AiParameter::job_conquer;
        conquerer->aiparam[getPlayer()]->task = AiParameter::tsk_move;
        conquerer->aiparam[getPlayer()]->dest = (*i)->getEntry();
      } else
        buildingCapture[ (*i)->getEntry() ].state = BuildingCapture::conq_unreachable;
   }

   for ( BuildingCaptureContainer::iterator bi = buildingCapture.begin(); bi != buildingCapture.end(); bi++) {
      pvehicle veh = getMap()->getUnit ( bi->second.unit );
      if ( veh ) {
         MapCoordinate dest = veh->aiparam[getPlayer()]->dest;
         moveUnit ( veh, dest, true );
         if ( veh->getPosition() == dest ) {
            veh->aiparam[getPlayer()]->task = AiParameter::tsk_nothing;
            buildingCapture.erase ( bi );
         }
      } else
         buildingCapture.erase ( bi );
   }

}




  class CalculateThreat_VehicleType {
                         protected:
                              AI*               ai;

                              Vehicletype*      fzt;
                              int               weapthreat[8];
                              int               value;

                              virtual int       getdamage ( void )      { return 0;   };
                              virtual int       getexpirience ( void )  { return 0;   };
                              virtual int       getammunition( int i )  { return 1;   };
                              virtual int       getheight ( void )      { return 255; };
                          public:
                              void              calc_threat_vehicletype ( Vehicletype* _fzt );
                              CalculateThreat_VehicleType ( AI* _ai ) { ai = _ai; };
                       };

  class CalculateThreat_Vehicle : public CalculateThreat_VehicleType {
                           protected:
                                pvehicle          eht;
                                virtual int       getdamage ( void );
                                virtual int       getexpirience ( void );
                                virtual int       getammunition( int i );
                                virtual int       getheight ( void );
                           public:
                              void              calc_threat_vehicle ( pvehicle _eht );
                              CalculateThreat_Vehicle ( AI* _ai ) : CalculateThreat_VehicleType ( _ai ) {};
                       };


void         CalculateThreat_VehicleType :: calc_threat_vehicletype ( Vehicletype* _fzt )
{
   fzt = _fzt;

   for ( int j = 0; j < 8; j++ )
      weapthreat[j] = 0;

   for ( int i = 0; i < fzt->weapons->count; i++)
      if ( fzt->weapons->weapon[i].shootable() )
         if ( fzt->weapons->weapon[i].offensive() )
            for ( int j = 0; j < 8; j++)
               if ( fzt->weapons->weapon[i].targ & (1 << j) ) {
                  int d = 0;
                  int m = 0;
                  AttackFormula af;
                  for ( int e = (fzt->weapons->weapon[i].mindistance + maxmalq - 1)/ maxmalq; e <= fzt->weapons->weapon[i].maxdistance / maxmalq; e++ ) {    // the distance between two fields is maxmalq
                     d++;
                     int n = int( weapDist.getWeapStrength( &fzt->weapons->weapon[i], e*maxmalq ) * fzt->weapons->weapon[i].maxstrength * af.strength_damage(getdamage()) * ( 1 + af.strength_experience(getexpirience())) );
                     m += int( n / log10(10*d));
                  }
                  if (getammunition(i) == 0)
                     m /= 2;

                  if ( (fzt->weapons->weapon[i].sourceheight & getheight()) == 0)
                     m /= 2;

                  /*
                  if ( !(getheight() & ( 1 << j )))
                     m /= 2;
                  */

                  if (m > weapthreat[j])
                     weapthreat[j] = m;
               }


   if ( !fzt->aiparam[ai->getPlayer()] )
      fzt->aiparam[ ai->getPlayer() ] = new AiValue ( log2 ( fzt->height ));

   for ( int l = 0; l < 8; l++ )
      fzt->aiparam[ ai->getPlayer() ]->threat.threat[l] = weapthreat[l];

   value = fzt->armor * value_armorfactor * (100 - getdamage()) / 100 ;

   for ( int s = 0; s < 7; )
      if ( weapthreat[s] < weapthreat[s+1] ) {
         int temp = weapthreat[s];
         weapthreat[s] = weapthreat[s+1];
         weapthreat[s+1] = temp;
         if ( s > 0 )
            s--;
      } else
         s++;

   // qsort ( weapthreat, 8, sizeof(int), compareinteger );

   for ( int k = 0; k < 8; k++ )
      value += weapthreat[k] * value_weaponfactor / (k+1);

   fzt->aiparam[ ai->getPlayer() ]->setValue ( value );
   fzt->aiparam[ ai->getPlayer() ]->valueType = 0;
}


int          CalculateThreat_Vehicle :: getammunition( int i )
{
   return eht->ammo[i];
}

int          CalculateThreat_Vehicle :: getheight(void)
{
   return eht->height;
}

int          CalculateThreat_Vehicle :: getdamage(void)
{
   return eht->damage;
}

int          CalculateThreat_Vehicle :: getexpirience(void)
{
   return eht->experience;
}


void         CalculateThreat_Vehicle :: calc_threat_vehicle ( pvehicle _eht )
{

   eht = _eht;
   calc_threat_vehicletype ( getvehicletype_forid ( eht->typ->id ) );

   if ( !eht->aiparam[ai->getPlayer()] )
      eht->aiparam[ai->getPlayer()] = new AiParameter ( eht );

   AiParameter* aip = eht->aiparam[ai->getPlayer()];
   for ( int l = 0; l < 8; l++ )
      aip->threat.threat[l] = eht->typ->aiparam[ ai->getPlayer() ]->threat.threat[l];

   int value = eht->typ->aiparam[ ai->getPlayer() ]->getValue();
   for ( int i = 0; i < 32; i++ )
      if ( eht->loading[i] ) {
         if ( !eht->loading[i]->aiparam[ai->getPlayer()] ) {
            CalculateThreat_Vehicle ctv ( ai );
            ctv.calc_threat_vehicle( eht->loading[i] );
         }
         value += eht->loading[i]->aiparam[ai->getPlayer()]->getValue();
      }

   aip->setValue ( value );

   if ( aip->job == AiParameter::job_undefined ) {
      if ( eht->weapexist() )
         aip->job = AiParameter::job_fight;

      if ( eht->functions & cf_conquer ) {
         if ( eht->functions & cf_trooper )  {
            if ( eht->typ->height & chfahrend )
               aip->job = AiParameter::job_conquer;
         } else {
            int maxmove = minint;
            for ( int i = 0; i< 8; i++ )
               if ( eht->typ->height & ( 1 << i ))
                  maxmove = max ( eht->typ->movement[i] , maxmove );

            int maxstrength = minint;
            for ( int w = 0; w < eht->typ->weapons->count; w++ )
                maxstrength= max (  eht->typ->weapons->weapon[w].maxstrength, maxstrength );

            if ( maxstrength < maxmove )
               aip->job = AiParameter::job_conquer;

         }
      }
   }

   bool service = false;
   for ( int w = 0; w < eht->typ->weapons->count; w++ )
      if ( eht->typ->weapons->weapon[w].service() )
         service = true;
   if ( ((eht->functions & cfrepair) || service) && eht->typ->movement[log2(eht->height)] >= minmalq )
      aip->job = AiParameter::job_supply;

/*
   generatethreatvalue();
   int l = 0;
   for ( int b = 0; b <= 7; b++) {
      eht->threatvalue[b] = weapthreatvalue[b];
      if (weapthreatvalue[b] > l)
         l = weapthreatvalue[b];
   }
   eht->completethreatvalue = threatvalue2 + l;
   eht->completethreatvaluesurr = threatvalue2 + l;
   eht->threats = 0;
*/
}


void  AI :: calculateThreat ( pvehicletype vt)
{
   CalculateThreat_VehicleType ctvt ( this );
   ctvt.calc_threat_vehicletype( vt );
}


void  AI :: calculateThreat ( pvehicle eht )
{
   CalculateThreat_Vehicle ctv ( this );
   ctv.calc_threat_vehicle( eht );
}


void  AI :: calculateThreat ( pbuilding bld )
{
   calculateThreat ( bld, getPlayer());
//   calculateThreat ( bld, 8 );
}

void  AI :: calculateThreat ( pbuilding bld, int player )
{
   if ( !bld->aiparam[ player ] )
      bld->aiparam[ player ] = new AiValue ( log2 ( bld->typ->buildingheight ) );

   int b;


   // Since we have two different resource modes now, this calculation should be rewritten....
   int value = (bld->plus.energy / 10) + (bld->plus.fuel / 10) + (bld->plus.material / 10) + (bld->actstorage.energy / 20) + (bld->actstorage.fuel / 20) + (bld->actstorage.material / 20) + (bld->maxresearchpoints / 10) ;
   for (b = 0; b <= 31; b++)
      if ( bld->loading[b]  ) {
         if ( !bld->loading[b]->aiparam[ player ] )
            calculateThreat ( bld->loading[b] );
         value += bld->loading[b]->aiparam[ player ]->getValue();
      }

   for (b = 0; b <= 31; b++)
      if ( bld->production[b] )  {
         if ( !bld->production[b]->aiparam[ player ] )
            calculateThreat ( bld->production[b] );
         value += bld->production[b]->aiparam[ player ]->getValue() / 10;
      }

   if (bld->typ->special & cgrepairfacilityb )
      value += ccbt_repairfacility;
   if (bld->typ->special & cghqb )
      value += ccbt_hq;
   if (bld->typ->special & cgtrainingb )
      value += ccbt_training;
   if (bld->typ->special & cgrecyclingplantb )
      value += ccbt_recycling;

   bld->aiparam[ player ]->setValue ( value );
}



void AI :: WeaponThreatRange :: run ( pvehicle _veh, int x, int y, AiThreat* _threat )
{
   threat = _threat;
   veh = _veh;
   for ( height = 0; height < 8; height++ )
      for ( weap = 0; weap < veh->typ->weapons->count; weap++ )
         if ( veh->height & veh->typ->weapons->weapon[weap].sourceheight )
            if ( (1 << height) & veh->typ->weapons->weapon[weap].targ )
                if ( veh->typ->weapons->weapon[weap].shootable()  && veh->typ->weapons->weapon[weap].offensive() ) {
                   initsearch ( x, y, veh->typ->weapons->weapon[weap].maxdistance/maxmalq, veh->typ->weapons->weapon[weap].mindistance/maxmalq );
                   startsearch();
                }
}

void AI :: WeaponThreatRange :: testfield ( void )
{
   if ( getfield ( xp, yp ))
      if ( dist*maxmalq <= veh->typ->weapons->weapon[weap].maxdistance )
         if ( dist*maxmalq >= veh->typ->weapons->weapon[weap].mindistance ) {
            AttackFormula af;
            int strength = int ( weapDist.getWeapStrength( &veh->typ->weapons->weapon[weap], dist*maxmalq, veh->height, 1 << height )
                                 * veh->typ->weapons->weapon[weap].maxstrength
                                 * (1 + af.strength_experience ( veh->experience ) + af.strength_attackbonus ( getfield(startx,starty)->getattackbonus() ))
                                 * af.strength_damage ( veh->damage )
                                );

            if ( strength ) {
               int pos = xp + yp * ai->getMap()->xsize;
               if ( strength > threat[pos].threat[height] )
                  threat[pos].threat[height] = strength;
            }
         }
}

void AI :: calculateFieldThreats ( void )
{
   if ( fieldNum && fieldNum != activemap->xsize * activemap->ysize ) {
      delete[] fieldThreats;
      fieldThreats = NULL;
      fieldNum = 0;
   }
   if ( !fieldThreats ) {
      fieldNum = activemap->xsize * activemap->ysize;
      fieldThreats = new AiThreat[ fieldNum ];
   } else
      for ( int a = 0; a < fieldNum; a++ )
         fieldThreats[ a ].reset();

   AiThreat*  singleUnitThreat = new AiThreat[fieldNum];

   // we now check the whole map
   for ( int y = 0; y < activemap->ysize; y++ ) {
      checkKeys();
      for ( int x = 0; x < activemap->xsize; x++ ) {
         pfield fld = getfield ( x, y );
         if ( config.wholeMapVisible || fieldvisiblenow ( fld, getPlayer() ) )
            if ( fld->vehicle && getdiplomaticstatus2 ( getPlayer()*8, fld->vehicle->color) == cawar ) {
               WeaponThreatRange wr ( this );
               if ( !fld->vehicle->typ->wait ) {

                  // The unit may have already moved this turn.
                  // So we give it the maximum movementrange

                  int move = fld->vehicle->getMovement() ;

                  fld->vehicle->resetMovement ( );

                  VehicleMovement vm ( NULL, NULL );
                  if ( vm.available ( fld->vehicle )) {
                     vm.execute ( fld->vehicle, -1, -1, 0, -1, -1 );

                     // Now we cycle through all fields that are reachable...
                     for ( int f = 0; f < vm.reachableFields.getFieldNum(); f++ ) {
                        int xp, yp;
                        vm.reachableFields.getFieldCoordinates ( f, &xp, &yp );
                        // ... and check for each which fields are threatened if the unit was standing there
                        wr.run ( fld->vehicle, xp, yp, singleUnitThreat );
                     }

                     for ( int g = 0; g < vm.reachableFieldsIndirect.getFieldNum(); g++ ) {
                        int xp, yp;
                        vm.reachableFieldsIndirect.getFieldCoordinates ( g, &xp, &yp );
                        wr.run ( fld->vehicle, xp, yp, singleUnitThreat );
                     }
                  }
                  fld->vehicle->setMovement ( move, 0 );
               } else
                  wr.run ( fld->vehicle, x, y, singleUnitThreat );


               for ( int a = 0; a < fieldNum; a++ ) {
                  for ( int b = 0; b < 8; b++ )
                     fieldThreats[a].threat[b] += singleUnitThreat[a].threat[b];

                  singleUnitThreat[ a ].reset();
               }
            }
      }
   }
}


void     AI :: calculateAllThreats( void )
{
   // Calculates the basethreats for all vehicle types
   if ( !baseThreatsCalculated ) {
      for ( int w = 0; w < vehicletypenum; w++) {
         pvehicletype fzt = getvehicletype_forpos(w);
         if ( fzt )
            calculateThreat( fzt );

      }
      baseThreatsCalculated = 1;
   }

   // Some further calculations that only need to be done once.
   if ( maxTrooperMove == 0) {
      for ( int v = 0; v < vehicletypenum; v++) {
         pvehicletype fzt = getvehicletype_forpos( v );
         if ( fzt )
            if ( fzt->functions & cf_conquer )
               if ( fzt->movement[chfahrend] > maxTrooperMove )   // buildings can only be conquered on ground level, or by moving to adjecent field which is less
                  maxTrooperMove = fzt->movement[chfahrend];
      }
   }
   if ( maxTransportMove == 0 ) {
      for (int v = 0; v < vehicletypenum; v++) {
         pvehicletype fzt = getvehicletype_forpos( v );
         if ( fzt )
            for ( int w = 0; w <= 7; w++) // cycle through all levels of height
               if (fzt->movement[w] > maxTransportMove)
                  maxTransportMove = fzt->movement[w];
      }
      maxUnitMove = maxTransportMove;
   }
   for ( int height = 0; height < 8; height++ )
      if ( maxWeapDist[height] < 0 ) {

         maxWeapDist[height] = 0; // It may be possible that there is no weapon to shoot to a specific height

         for ( int v = 0; v < vehicletypenum; v++) {
            pvehicletype fzt = getvehicletype_forpos( v );
            if ( fzt )
               for ( int w = 0; w < fzt->weapons->count ; w++)
                  if ( fzt->weapons->weapon[w].maxdistance > maxWeapDist[height] )
                     if ( fzt->weapons->weapon[w].targ & ( 1 << height ))   // targ is a bitmap, each bit standing for a level of height
                         maxWeapDist[height] = fzt->weapons->weapon[w].maxdistance;
         }
      }




   // There are only 8 players in ASC, but there may be neutral units (player == 8)
   for ( int v = 0; v < 9; v++)
      if (activemap->player[v].exist() || v == 8) {

         // Now we cycle through all units of this player
         for ( tmap::Player::VehicleList::iterator vi = getMap()->player[v].vehicleList.begin(); vi != getMap()->player[v].vehicleList.end(); vi++ ) {
            pvehicle veh = *vi;
            if ( !veh->aiparam[ getPlayer() ] )
               calculateThreat ( veh );
         }

         // Now we cycle through all buildings
         for ( tmap::Player::BuildingList::iterator bi = getMap()->player[v].buildingList.begin(); bi != getMap()->player[v].buildingList.end(); bi++ )
            calculateThreat ( *bi );
      }

}

AiThreat& AI :: getFieldThreat ( int x, int y )
{
   if ( !fieldThreats )
      calculateFieldThreats ();
   return fieldThreats[y * activemap->xsize + x ];
}


void AI :: Section :: init ( AI* _ai, int _x, int _y, int xsize, int ysize, int _xp, int _yp )
{
   ai = _ai;
   init ( _x, _y, xsize, ysize, _xp, _yp );
}

void AI :: Section :: init ( int _x, int _y, int xsize, int ysize, int _xp, int _yp )
{
   x1 = _x;
   y1 = _y;
   x2 = _x + xsize;
   y2 = _y + ysize;

   xp = _xp;
   yp = _yp;

   if ( x1 < 0 ) x1 = 0;
   if ( y1 < 0 ) y1 = 0;
   if ( x2 >= ai->activemap->xsize ) x2 = ai->activemap->xsize-1;
   if ( y2 >= ai->activemap->ysize ) y2 = ai->activemap->ysize-1;


   centerx = (x1 + x2) / 2;
   centery = (y1 + y2) / 2;
   numberOfFields = (x2-x1+1) * ( y2-y1+1);

   absUnitThreat.reset();
   absFieldThreat.reset();

   for ( int j = 0; j < aiValueTypeNum; j++ )
     value[j] = 0;

   for ( int y = y1; y <= y2; y++ )
      for ( int x = x1; x <= x2; x++ ) {
         absFieldThreat += ai->getFieldThreat ( x, y );
         pfield fld = getfield ( x, y );
         if ( fld->vehicle && getdiplomaticstatus ( fld->vehicle->color )==cawar) {
            if ( !fld->vehicle->aiparam[ ai->getPlayer() ] )
               ai->calculateThreat ( fld->vehicle );
            AiParameter& aip = * fld->vehicle->aiparam[ ai->getPlayer() ];
            absUnitThreat += aip.threat;
            value[ aip.valueType ] += aip.getValue();
         }
      }

   for ( int i = 0; i <  absUnitThreat.threatTypes; i++ ) {
      avgUnitThreat.threat[i] = absUnitThreat.threat[i] / numberOfFields;
      avgFieldThreat.threat[i] = absFieldThreat.threat[i] / numberOfFields;
   }

}

int AI :: Section :: numberOfAccessibleFields ( const pvehicle veh )
{
   int num = 0;
   for ( int y = y1; y <= y2; y++ )
      for ( int x = x1; x <= x2; x++ )
         if ( fieldaccessible ( getfield ( x, y ), veh ) == 2)
            num++;

   return num;
}

AI :: Sections :: Sections ( AI* _ai ) : ai ( _ai ) , section ( NULL )
{
   sizeX = 8;
   sizeY = 16;
   numX = ai->activemap->xsize * 2 / sizeX + 1;
   numY = ai->activemap->ysize * 2 / sizeY + 1;
}

void AI :: Sections :: reset ( void )
{
   if ( section ) {
      delete[] section;
      section = NULL;
   }
}


void AI :: Sections :: calculate ( void )
{
   if ( !section ) {
      section = new Section[ numX*numY ]; //  ( ai );
      for ( int x = 0; x < numX; x++ )
         for ( int y = 0; y < numY; y++ )
            section[ x + numX * y ].init ( ai, x * ai->activemap->xsize / numX, y * ai->activemap->ysize / numY, sizeX, sizeY, x, y );

    }
}

AI::Section& AI :: Sections :: getForCoordinate ( int xc, int yc )
{
   if ( !section )
      calculate();

   int dist = maxint;
   Section* sec = NULL;
   for ( int x = 0; x < numX; x++ )
      for ( int y = 0; y < numY; y++ ) {
         Section& s2 = getForPos ( x, y );
         int d = beeline ( xc, yc, s2.centerx, s2.centery);
         if ( d < dist ) {
            dist = d;
            sec = &getForPos ( x, y );
         }
      }

   return *sec;
}

AI::Section& AI :: Sections :: getForPos ( int xn, int yn )
{
   if ( xn >= numX || yn >= numY || xn < 0 || yn < 0 )
      displaymessage( "AI :: Sections :: getForPos - invalid parameters: %d %d", 2, xn, yn );

   return section[xn+yn*numX];
}

AI::Section* AI :: Sections :: getBest ( int pass, const pvehicle veh, int* xtogo, int* ytogo )
{
   /*
      In the first pass wwe check were all the units would go if there wouldn't be
      a threat anywhere.
      In the second pass the threat of a section is devided by the number of units that
      are going there
   */


   AStar ast ( ai->getMap(), veh );
   ast.findAllAccessibleFields (  );


   AiParameter& aip = *veh->aiparam[ ai->getPlayer() ];

   float d = minfloat;
   float nd = minfloat;
   AI::Section* frst = NULL;

   float maxSectionThread = 0;
   for ( int y = 0; y < numY; y++ )
      for ( int x = 0; x < numX; x++ ) {
          AI::Section& sec = getForPos( x, y );
          int threat = sec.avgUnitThreat.threat[aip.valueType];
          if ( threat > maxSectionThread )
              maxSectionThread = threat;
      }


   for ( int y = 0; y < numY; y++ )
      for ( int x = 0; x < numX; x++ ) {
          int xtogoSec = -1;
          int ytogoSec = -1;

          AI::Section& sec = getForPos( x, y );
          float t = 0;
          for ( int i = 0; i < aiValueTypeNum; i++ )
             t += aip.threat.threat[i] * sec.value[i];

          float f = t;

          if ( sec.avgUnitThreat.threat[aip.valueType] ) {
             int relThreat = int( 4*maxSectionThread / sec.avgUnitThreat.threat[aip.valueType] + 1);
             f /= relThreat;
          }

          /*
          if ( sec.avgUnitThreat.threat[aip.valueType] >= 0 )
             f = t / log( sec.avgUnitThreat.threat[aip.valueType] );
          else
             f = t;
          */

          int dist = beeline ( veh->xpos, veh->ypos, sec.centerx, sec.centery ) + 3 * veh->typ->movement[log2(veh->height)];
          if ( dist )
             f /= log(dist);

          if ( f > d ) {
             int ac  = 0;
             int nac = 0;
             int mindist = maxint;
             int targets = 0;
             for ( int yp = sec.y1; yp <= sec.y2; yp++ )
                for ( int xp = sec.x1; xp <= sec.x2; xp++ ) {
                   pfield fld = getfield ( xp, yp );
                   if ( fld->a.temp == 1 ) {
                      int mandist = abs( sec.centerx - xp ) + 2*abs ( sec.centery - yp );
                      if ( mandist < mindist ) {
                         mindist = mandist;
                         xtogoSec = xp;
                         ytogoSec = yp;
                      }

                      ai->_vision = visible_all;

                      ac++;

                      TemporaryContainerStorage tus ( veh );

                      veh->resetMovement(); // to make sure the wait-for-attack flag doesn't hinder the attack
                      veh->attacked = 0;
                      veh->xpos = xp;
                      veh->ypos = yp;

                      VehicleAttack va ( NULL, NULL );
                      if ( va.available ( veh )) {
                         va.execute ( veh, -1, -1, 0, 0, -1 );
                         targets += va.attackableVehicles.getFieldNum();
                      }
                      tus.restore();
                      ai->_vision = visible_ago;
                   } else
                      nac++;
                }

             int notAccessible = 100 * nac / (nac+ac);
             if ( notAccessible < 85  && targets ) {   // less than 85% of fields not accessible
                float nf = f * ( 100-notAccessible) / 100; // *  ( 100 - notAccessible );
                if ( nf > d ) {
                   d = nf;
                   // nd = nf;
                   frst = &getForPos ( x, y );
                   if ( xtogo && ytogo ) {
                      *xtogo = xtogoSec;
                      *ytogo = ytogoSec;
                   }
                }
             }
          }
      }

   return frst;

}



int  AI :: getBestHeight ( const pvehicle veh )
{
   int heightNum = 0;
   for ( int i = 0; i < 8; i++ )
      if ( veh->typ->height & ( 1 << i ))
         heightNum++;
   if ( heightNum == 1 )
      return veh->typ->height;

   int bestHeight = -1;
   float bestHeightValue = minfloat;

   float maxAvgFieldThreat = minfloat;
   float minAvgFieldThreat = maxfloat;
   float secminAvgFieldThreat = maxfloat;


   for ( int k = 0; k < 8; k++ )
      if ( veh->typ->height & ( 1 << k )) {
         TemporaryContainerStorage tus ( veh );
         veh->height = 1<<k;
         float t = sections.getForCoordinate( veh->xpos, veh->ypos ).avgFieldThreat.threat[ veh->aiparam[getPlayer()]->valueType ];
         tus.restore();

         if ( maxAvgFieldThreat < t )
            maxAvgFieldThreat = t;

         if ( minAvgFieldThreat > t )
            minAvgFieldThreat = t;

         if ( t )
           if ( secminAvgFieldThreat > t )
              secminAvgFieldThreat = t;
      }


   for ( int j = 0; j < 8; j++ )
      if ( veh->typ->height & ( 1 << j )) {
         TemporaryContainerStorage tus (veh);
         veh->height = 1<<j;
         calculateThreat ( veh );
         Section& sec = sections.getForCoordinate( veh->xpos, veh->ypos );

         float value = veh->aiparam[getPlayer()]->getValue();
         if ( veh->typ->movement[j] )
            value *=  log( veh->typ->movement[j] );

         float threat = sec.avgFieldThreat.threat[ veh->aiparam[getPlayer()]->valueType ];
         if ( minAvgFieldThreat ) {
            if ( threat )
               value *= ( log ( minAvgFieldThreat ) / log ( threat ) );
         } else {
            if ( secminAvgFieldThreat < maxfloat)
               if ( threat )
                  value *= ( log ( secminAvgFieldThreat ) / log ( threat ) );
               else
                  value *= 1.5; // no threat
            }

         if ( value > bestHeightValue ) {
            bestHeightValue = value;
            bestHeight = 1 << j;
         }
         tus.restore();
      }

   return bestHeight;
}


void    AI :: setup (void)
{
   displaymessage2("calculating all threats ... ");
   calculateAllThreats ();

   displaymessage2("calculating field threats ... ");
   calculateFieldThreats();

   displaymessage2("calculating sections ... ");
   sections.calculate();

/*
   for ( i = 0; i <= 8; i++) {
      pbuilding building = actmap->player[i].firstbuilding;
      while (building != NULL) {
         generatethreatvaluebuilding(building);
         building = building->next;
      }
      if (i == actmap->actplayer) {
         building = actmap->player[i].firstbuilding;
         while (building != NULL) {
            tcmpcheckreconquerbuilding ccrcb;
            ccrcb.init(3);
            ccrcb.initsuche(building->xpos,building->ypos,(maxfusstruppenmove + maxtransportmove) / 8 + 1,0);
            ccrcb.startsuche();
            int j;
            ccrcb.returnresult( &j );
            ccrcb.done();
            building = building->next;
         }
      }
   }
   */

   /*
    punits units = new tunits;
    tjugdesituationspfd jugdesituationspfd;
    jugdesituationspfd.init(units,1);
    jugdesituationspfd.startsuche();
    jugdesituationspfd.done();
    delete units;
   */

   // showthreats("init: threatvals generated");
   displaymessage2("setup completed ... ");
}

void AI :: searchTargets ( pvehicle veh, int x, int y, TargetVector& tl, int moveDist, VehicleMovement& vm, int hemmingBonus )
{

   npush ( veh->xpos );
   npush ( veh->ypos );

   veh->removeview();
   // int fieldsWithChangedVisibility = evaluateviewcalculation ( getMap(), veh->xpos, veh->ypos, veh->typ->view, 0xff );
   veh->xpos = x;
   veh->ypos = y;
   veh->addview();
   int fieldsWithChangedVisibility = evaluateviewcalculation ( getMap(), veh->xpos, veh->ypos, veh->typ->view, 0xff );


   VehicleAttack va ( NULL, NULL );
   if ( va.available ( veh )) {
      va.execute ( veh, -1, -1, 0 , 0, -1 );
      for ( int g = 0; g < va.attackableVehicles.getFieldNum(); g++ ) {
         int xp, yp;
         va.attackableVehicles.getFieldCoordinates ( g, &xp, &yp );
         pattackweap aw = &va.attackableVehicles.getData ( g );

         int bestweap = -1;
         int targdamage = -1;
         int weapstrength = -1;
         for ( int w = 0; w < aw->count; w++ ) {
            tunitattacksunit uau ( veh, getfield ( xp, yp)->vehicle, 1, aw->num[w] );
            uau.calc();
            if ( uau.dv.damage > targdamage ) {
               bestweap = aw->num[w];
               targdamage = uau.dv.damage;
               weapstrength = aw->strength[w];
            } else
               if ( uau.dv.damage == 100 )
                  if ( weapstrength == -1 || weapstrength > aw->strength[w] ) {
                     bestweap = aw->num[w];
                     targdamage = uau.dv.damage;
                     weapstrength = aw->strength[w];
                  }
         }

         if ( bestweap == -1 )
            displaymessage ( "inconsistency in AI :: searchTarget", 1 );


         MoveVariant* mv = new MoveVariant;

         tunitattacksunit uau ( veh, getfield ( xp, yp)->vehicle, 1, bestweap );
         mv->orgDamage = uau.av.damage;
         mv->damageAfterMove = uau.av.damage;
         mv->enemyOrgDamage = uau.dv.damage;
         uau.calc();


         mv->damageAfterAttack = uau.av.damage;
         mv->enemyDamage = uau.dv.damage;
         mv->enemy = getfield ( xp, yp )->vehicle;
         mv->movex = x;
         mv->movey = y;
         mv->attackx = xp;
         mv->attacky = yp;
         mv->weapNum = bestweap;
         mv->moveDist = moveDist;
         mv->attacker = veh;


         for ( int nf = 0; nf < sidenum; nf++ ) {
            MapCoordinate mc = getNeighbouringFieldCoordinate ( MapCoordinate ( mv->attackx, mv->attacky), nf );
            pfield fld = getMap()->getField(mc);
            mv->neighbouringFieldsReachable[nf] = (vm.reachableFields.isMember( mc ) || ( veh->xpos == mc.x && veh->ypos == mc.y )) && !fld->building && (!fld->vehicle || fld->unitHere(veh));
         }

         int attackerDirection = getdirection ( xp, yp, x,y );
         float hemmingFactor = 1;
         for ( int nf = 0; nf < sidenum-1 && nf < hemmingBonus; nf++ ) {
            // we are starting opposite the attacker, since this is the highest hemming bonus
            int checkDir;
            if ( nf == 0 )
               checkDir = attackerDirection+sidenum/2;
            else {
               if ( nf & 1 )
                  checkDir = attackerDirection+sidenum/2 + (nf+1)/2;
               else
                  checkDir = attackerDirection+sidenum/2 - (nf+1)/2;
            }

            MapCoordinate mc = getNeighbouringFieldCoordinate ( MapCoordinate ( mv->attackx, mv->attacky), (attackerDirection+checkDir)%sidenum );
            pfield fld = getMap()->getField(mc);
            if ( fld && !fld->building && !fld->vehicle )
               hemmingFactor += AttackFormula::getHemmingFactor ( checkDir - attackerDirection );
         }

         mv->result = int ((mv->enemyDamage - mv->enemyOrgDamage) * mv->enemy->aiparam[getPlayer()]->getValue() * hemmingFactor - 1/config.aggressiveness * (mv->damageAfterAttack - mv->orgDamage) * veh->aiparam[getPlayer()]->getValue() );
         if ( mv->enemyDamage >= 100 )
            mv->result += mv->enemy->aiparam[getPlayer()]->getValue() * attack_unitdestroyed_bonus;

         if ( mv->result > 0 )
            tl.push_back ( mv );

      }
   }

   // if ( fieldsWithChangedVisibility )
   //   evaluateviewcalculation ( getMap(), veh->xpos, veh->ypos, veh->typ->view, 0xff );
   veh->removeview();
   npop ( veh->ypos );
   npop ( veh->xpos );

   veh->addview();

   if ( fieldsWithChangedVisibility || 1 )  // viewbug.sav !!!!!
      evaluateviewcalculation ( getMap(), veh->xpos, veh->ypos, veh->typ->view, 0xff );
}

bool operator > ( const AI::MoveVariant& mv1, const AI::MoveVariant& mv2 )
{
   return ( mv1.result > mv2.result || (mv1.result == mv2.result && mv1.moveDist > mv2.moveDist ));
}

bool operator < ( const AI::MoveVariant& mv1, const AI::MoveVariant& mv2 )
{
   return ( mv1.result < mv2.result || (mv1.result == mv2.result && mv1.moveDist < mv2.moveDist ));
}


bool moveVariantComp ( const AI::MoveVariant* mv1, const AI::MoveVariant* mv2 )
{
   return ( mv1->result < mv2->result || (mv1->result == mv2->result && mv1->moveDist > mv2->moveDist ));
}

void AI::getAttacks ( VehicleMovement& vm, pvehicle veh, TargetVector& tv, int hemmingBonus )
{
   if( vm.getStatus() != 0 && vm.getStatus() != 2 )
      displaymessage ( "AI::getAttacks / invalid status of VehicleMovement !", 2 );

   int orgxpos = veh->xpos ;
   int orgypos = veh->ypos ;

   if ( getfield ( veh->xpos, veh->ypos )->unitHere ( veh ) )  // unit not inside a building or transport
      searchTargets ( veh, veh->xpos, veh->ypos, tv, 0, vm, hemmingBonus );

   // Now we cycle through all fields that are reachable...
   if ( !veh->typ->wait && vm.available ( veh ) ) {
      if ( vm.getStatus() == 0 )
         vm.execute ( veh, -1, -1, 0, -1, -1 );

      for ( int f = 0; f < vm.reachableFields.getFieldNum(); f++ )
         if ( !vm.reachableFields.getField( f )->vehicle && !vm.reachableFields.getField( f )->building ) {
             int xp, yp;
             vm.reachableFields.getFieldCoordinates ( f, &xp, &yp );
             searchTargets ( veh, xp, yp, tv, beeline ( xp, yp, orgxpos, orgypos ), vm, hemmingBonus );
          }
   }
}

AI::AiResult AI::executeMoveAttack ( pvehicle veh, TargetVector& tv )
{
   int unitNetworkID = veh->networkid;
   AiResult result;

   MoveVariant* mv = *max_element( tv.begin(), tv.end(), moveVariantComp );

   if ( mv->movex != veh->xpos || mv->movey != veh->ypos ) {
      VehicleMovement vm2 ( mapDisplay, NULL );
      vm2.execute ( veh, -1, -1, 0, -1, -1 );
      if ( vm2.getStatus() != 2 )
         displaymessage ( "AI :: executeMoveAttack \n error in movement step 0 with unit %d", 1, veh->networkid );

      vm2.execute ( NULL, mv->movex, mv->movey, 2, -1, -1 );
      if ( vm2.getStatus() != 3 )
         displaymessage ( "AI :: executeMoveAttack \n error in movement step 2 with unit %d", 1, veh->networkid );

      vm2.execute ( NULL, mv->movex, mv->movey, 3, -1,  1 );
      if ( vm2.getStatus() != 1000 )
         displaymessage ( "AI :: executeMoveAttack \n error in movement step 3 with unit %d", 1, veh->networkid );

      result.unitsMoved ++;
   }

   // the unit may have been shot down due to reactionfire during movement
   if ( !getMap()->getUnit(unitNetworkID)) {
      result.unitsDestroyed++;
      return result;
   }

   VehicleAttack va ( mapDisplay, NULL );
   va.execute ( veh, -1, -1, 0 , 0, -1 );
   if ( va.getStatus() != 2 )
      displaymessage ( "AI :: executeMoveAttack \n error in attack step 2 with unit %d", 1, veh->networkid );

   va.execute ( NULL, mv->attackx, mv->attacky, 2 , -1, mv->weapNum );
   if ( va.getStatus() != 1000 )
      displaymessage ( "AI :: executeMoveAttack \n error in attack step 3 with unit %d", 1, veh->networkid );

   result.unitsMoved ++;

   // the unit may have been shot in the attack
   if ( !getMap()->getUnit(unitNetworkID)) {
      result.unitsDestroyed++;
      return result;
   }


   VehicleMovement vm3 ( mapDisplay, NULL );
   if ( vm3.available ( veh ))
      result += moveToSavePlace ( veh, vm3 );

   return result;
}

int AI::getDirForBestTacticsMove ( const pvehicle veh, TargetVector& tv )
{
   if ( tv.size() <= 0 )
      return -1;

   MoveVariant* mv = *max_element( tv.begin(), tv.end(), moveVariantComp );
   return getdirection ( veh->xpos, veh->ypos, mv->movex, mv->movey );
}

MapCoordinate AI::getDestination ( const pvehicle veh )
{
   AiParameter::Task task = veh->aiparam[ getPlayer() ]->task;
   if ( task == AiParameter::tsk_nothing || task == AiParameter::tsk_tactics ) {
      TargetVector tv;
      VehicleMovement vm ( NULL, NULL );
      getAttacks ( vm, veh, tv, 0 );

      if ( tv.size() > 0 ) {

         MoveVariant* mv = *max_element( tv.begin(), tv.end(), moveVariantComp );
         return MapCoordinate ( mv->movex, mv->movey );
      } else
         task = AiParameter::tsk_strategy;
   }

   if ( task == AiParameter::tsk_strategy ) {
      Section* sec = sections.getBest ( 0, veh );
      if ( sec )
         return MapCoordinate ( sec->centerx, sec->centery );
   }

   return MapCoordinate ( veh->xpos, veh->ypos );
}


AI::AiResult AI::moveToSavePlace ( pvehicle veh, VehicleMovement& vm3 )
{
   int unitNetworkID = veh->networkid;

   AiResult result;

   vm3.execute ( veh, -1, -1, 0, -1, -1 );

   // if there are no fields reachable
   if ( vm3.getStatus() == -107 )
      return result;

   if ( vm3.getStatus() != 2 )
      displaymessage ( "AI :: moveToSavePlace \n error in movement3 step 0 with unit %d", 1, veh->networkid );

   int xtogo = veh->xpos;
   int ytogo = veh->ypos;
   int threat = maxint;
   int dist = maxint;

   if ( getfield ( veh->xpos, veh->ypos)->unitHere ( veh ) ) {  // vehicle not in building / transport
      threat = int( getFieldThreat ( veh->xpos, veh->ypos).threat[ veh->aiparam[ getPlayer()]->valueType] * 1.5 + 1);
       // multiplying with 1.5 to make this field a bit unattractive, to allow other units (for example buggies) to attack from this field to, since it is probably quite a good position (else it would not have been chosen)
   }

   for ( int f = 0; f < vm3.reachableFields.getFieldNum(); f++ )
      if ( !vm3.reachableFields.getField( f )->vehicle && !vm3.reachableFields.getField( f )->building ) {
         int x,y;
         vm3.reachableFields.getFieldCoordinates ( f, &x, &y );
         AiThreat& ait = getFieldThreat ( x, y );
         int _dist = beeline ( x, y, veh->xpos, veh->ypos);

            // make fields far away a bit unattractive; we don't want to move the whole distance back again next turn
         int t = int( ait.threat[ veh->aiparam[ getPlayer()]->valueType ] * log ( _dist )/log(10) );

         if ( t < threat || ( t == threat && _dist < dist )) {
            threat = t;
            xtogo = x;
            ytogo = y;
            dist = _dist;
         }
      }

   if ( veh->xpos != xtogo || veh->ypos != ytogo ) {
      vm3.execute ( NULL, xtogo, ytogo, 2, -1, -1 );
      if ( vm3.getStatus() != 3 )
         displaymessage ( "AI :: moveToSavePlace \n error in movement3 step 2 with unit %d", 1, veh->networkid );

      vm3.execute ( NULL, xtogo, ytogo, 3, -1,  1 );
      if ( vm3.getStatus() != 1000 )
         displaymessage ( "AI :: moveToSavePlace \n error in movement3 step 3 with unit %d", 1, veh->networkid );

      result.unitsMoved++;
   }

   if ( !getMap()->getUnit( unitNetworkID ))
      result.unitsDestroyed++;

   return result;
}


int AI::changeVehicleHeight ( pvehicle veh, VehicleMovement* vm, int preferredDirection )
{
   int bh = getBestHeight ( veh );
   if ( bh != veh->height && bh != -1 ) {
      ChangeVehicleHeight* cvh;
      int newheight;
      if ( bh > veh->height ) {
         cvh = new IncreaseVehicleHeight ( mapDisplay, NULL );
         newheight = veh->height << 1;
      } else {
         cvh = new DecreaseVehicleHeight ( mapDisplay, NULL );
         newheight = veh->height >> 1;
      }
      auto_ptr<ChangeVehicleHeight> acvh ( cvh );

      if ( newheight & veh->typ->height ) {
         if ( cvh->available ( veh ) ) {
            if ( vm )
               cvh->registerStartMovement ( *vm );

            int stat = cvh->execute ( veh, -1, -1, 0, newheight, 1 );
            if ( stat == 2 ) {   // if the unit could change its height vertically, or the height change is not available, skip this block

               int bestx = -1;
               int besty = -1;
               int moveremain = minint;

               if ( preferredDirection == -1 ) {
                  // making a backup
                  TemporaryContainerStorage tus ( veh );
                  veh->height = newheight;
                  veh->resetMovement ( );
                  MapCoordinate mc = getDestination ( veh );
                  preferredDirection = getdirection ( veh->xpos, veh->ypos, mc.x, mc.y );
                  tus.restore();
               }

               for ( int i = 0; i < cvh->reachableFields.getFieldNum(); i++ ) {
                  int newMoveRemain = cvh->reachableFields.getData ( i ).dist;

                  // check if we move in the direction that should be moved to
                  if ( preferredDirection != -1 ) {
                     int xp, yp;
                     cvh->reachableFields.getFieldCoordinates ( i, &xp, &yp );

                     int dir = preferredDirection - getdirection ( veh->xpos, veh->ypos, xp, yp );
                     if ( dir > sidenum/2 )
                        dir -= sidenum;
                     if ( dir < -sidenum/2 )
                        dir += sidenum;

                     dir = abs ( dir );
                     // ok, now we have the directional difference in dir, with a range of 0 .. 3 ; 0 meaning moving in the preferred direction


                     newMoveRemain -= (dir*2-3)* veh->typ->steigung*maxmalq / 3;
                     // if dir==0, we subtract -steigung   => newMoveRemain is getting larger because we move in the right direction
                     // if dir==3, we subtract +steigung
                  }

                  if ( newMoveRemain > moveremain ) {
                     cvh->reachableFields.getFieldCoordinates ( i, &bestx, &besty );
                     moveremain = newMoveRemain;
                  }
               }

               if ( bestx != -1 && besty != -1 ) {
                  cvh->execute ( NULL, bestx, besty, 2, -1, -1 );
                  if ( cvh->getStatus() != 3 )
                     displaymessage ( "AI :: changeVehicleHeight \n error in changeHeight step 2 with unit %d", 1, veh->networkid );

                  cvh->execute ( NULL, bestx, besty, 3, -1, -1 );
                  if ( cvh->getStatus() != 1000 )
                     displaymessage ( "AI :: changeVehicleHeight \n error in changeHeight step 3 with unit %d", 1, veh->networkid );

                  return 1;
               } else
                  return -1;
            } else {
               if ( stat == 1000 )
                 return 1;

               if ( veh->typ->steigung == 0 )
                 return -2;

               return -1;
            }
         } else {  // cvh->available
            return -2;
         }
      }

   }
   return 0;
}


AI::AiResult AI::tactics( void )
{
   const int tsk_num = 3;
   AiParameter::Task tasks[tsk_num] = { AiParameter::tsk_nothing,
                                        AiParameter::tsk_tactics,
                                        AiParameter::tsk_tactwait
                                      };
   AiResult result;

   displaymessage2("starting tactics ... ");

   typedef list<pvehicle> TactVehicles;
   TactVehicles tactVehicles;

   for ( tmap::Player::VehicleList::iterator vi = getMap()->player[ getPlayer() ].vehicleList.begin(); vi != getMap()->player[ getPlayer() ].vehicleList.end(); vi++ ) {
      pvehicle veh = *vi;

      bool unitUsable = false;
      if ( veh->aiparam[ getPlayer() ]->job == AiParameter::job_fight || veh->aiparam[ getPlayer() ]->job == AiParameter::job_undefined )
         for ( int j = 0; j < tsk_num; j++ )
            if ( veh->aiparam[ getPlayer() ]->task == tasks[j] )
               unitUsable = true;

      int maxWeapDist = minint;
      for ( int w = 0; w < veh->typ->weapons->count; w++ )
         if ( veh->typ->weapons->weapon[w].shootable() )
            maxWeapDist = max ( veh->typ->weapons->weapon[w].maxdistance , maxWeapDist );

      int maxMove = minint;
      for ( int h = 0; h < 8; h++ )
         if ( veh->typ->height & ( 1 << h ))
            maxMove = max ( veh->typ->movement[h], maxMove );

      bool enemiesNear = false;
      int ydist = (maxMove + maxWeapDist) / maxmalq;
      int xdist = ydist / 2;
      for ( int x = veh->xpos - xdist; x <= veh->xpos + xdist; x++ )
         for ( int y = veh->ypos - ydist; y <= veh->ypos + ydist; y++ ) {
            pfield fld = getMap()->getField(x,y );
            if ( fld ) {
               if ( fld->vehicle && getdiplomaticstatus2 ( veh->color, fld->vehicle->color ) == cawar )
                  enemiesNear = true;
               if ( fld->building && getdiplomaticstatus2 ( veh->color, fld->building->color ) == cawar )
                  enemiesNear = true;
            }
         }

      if ( unitUsable && enemiesNear)
         tactVehicles.push_back ( veh );
   }

   int hemmingBonus = 5;

   while ( !tactVehicles.empty() ) {

      typedef map<int, MoveVariantContainer> Targets;
      Targets targets;

      int directAttackNum;
      do {
         directAttackNum = 0;
         for ( TactVehicles::iterator i = tactVehicles.begin(); i != tactVehicles.end(); ) {
            pvehicle veh = *i;

            unitCounter++;
            displaymessage2("tact: processing operation %d", unitCounter );
            checkKeys();

            int stat = changeVehicleHeight ( veh, NULL );
            if ( stat == -1 ) { // couldn't change height due to blocked way or something similar
               veh->aiparam[ getPlayer() ]->task = AiParameter::tsk_wait;
               result.unitsWaiting++;
               i++;
            } else {

               VehicleMovement vm ( NULL, NULL );
               TargetVector tv;
               getAttacks ( vm, veh, tv, hemmingBonus );

               if ( tv.size() ) {
                  MoveVariant* mv = *max_element( tv.begin(), tv.end(), moveVariantComp );
                  bool directAttack = false;
                  if ( beeline ( mv->movex, mv->movey, mv->attackx, mv->attacky ) > maxmalq )
                     directAttack = true;

                  int freeNeighbouringFields = 0;
                  for ( int j = 0; j < sidenum; j++ ) {
                     pfield fld = getMap()->getField ( getNeighbouringFieldCoordinate ( MapCoordinate(mv->attackx, mv->attacky), j));
                     if ( !fld->building && !fld->vehicle )
                        freeNeighbouringFields++;
                  }

                  if ( freeNeighbouringFields <= 1 )
                     directAttack = true;

                  if ( mv->enemyDamage >= 100 )
                     directAttack = true;

                  if ( directAttack ) {
                     AiResult res = executeMoveAttack ( veh, tv );
                     i = tactVehicles.erase ( i );

                     if ( !res.unitsDestroyed )
                        veh->aiparam[ getPlayer() ]->task = AiParameter::tsk_tactics;

                     result += res;
                     directAttackNum++;

                  } else {
                     targets[mv->enemy->networkid].push_back( *mv );
                     i++;
                  }
               } else {
                  // there is nothing the unit can do in tactics mode
                  veh->aiparam[ getPlayer() ]->task = AiParameter::tsk_nothing;
                  i = tactVehicles.erase ( i );
               }
            }
         }

         // if there were direct attacks, some fields may now be blocked by units that did a directAttack,
         // so all other units plans must be recalculated

         if ( directAttackNum )
            targets.clear();

      } while ( directAttackNum );

      if ( !targets.empty() ) {
         // we are beginning with the targets that the most own units want to attack
         Targets::iterator currentTarget = targets.begin();
         for ( Targets::iterator i = targets.begin(); i != targets.end(); i++ )
            if ( i->second.size() > currentTarget->second.size())
               currentTarget = i;

         if ( currentTarget->second.size()-1 < hemmingBonus )
            hemmingBonus = currentTarget->second.size()-1;

         typedef list<MapCoordinate> AffectedFields;
         AffectedFields affectedFields;


         /* we don't need to discard all the calculations made above after a single attack.
            Only the attacks that are near enough to be affected by the last movement and attack
            will be ignored and recalculated in the next pass
         */
         do {   // while currentTarget != targets.end()

            pvehicle enemy = getMap()->getUnit( currentTarget->first);
            // the enemy may have been shot down by a direct attack earlier
            if ( enemy ) {
               affectedFields.push_back ( MapCoordinate(enemy->xpos, enemy->ypos) );

               MoveVariantContainer attacker = currentTarget->second;
               sort( attacker.begin(), attacker.end() );

               MoveVariantContainer::iterator mvci = attacker.begin();
               pvehicle positions[sidenum];
               pvehicle finalPositions[sidenum];
               for ( int i = 0; i< sidenum; i++ ) {
                  positions[i] = NULL;
                  finalPositions[i] = NULL;
               }
               float finalValue = 0;

               tactics_findBestAttackUnits ( attacker, mvci, positions, 0, finalPositions, finalValue, 0, 0 );

               for ( int i = 0; i < sidenum; i++ )
                  if ( finalPositions[i] ) {
                     int nwid = finalPositions[i]->networkid;
                     moveUnit ( finalPositions[i], getNeighbouringFieldCoordinate( MapCoordinate( enemy->xpos, enemy->ypos), i));
                     affectedFields.push_back ( MapCoordinate(finalPositions[i]->xpos, finalPositions[i]->ypos) );
                     // the unit may have been shot down due to reaction fire

                     if ( !getMap()->getUnit ( nwid ) ) {
                        TactVehicles::iterator att = find ( tactVehicles.begin(), tactVehicles.end(), finalPositions[i] ) ;
                        tactVehicles.erase ( att );
                        finalPositions[i] = NULL;
                     }
                  }

               int attackOrder[sidenum];
               int finalOrder[sidenum];
               for ( int i = 0; i< sidenum; i++ )
                  attackOrder[i] = finalOrder[i] = -1;


               int finalDamage = -1;
               int finalAttackNum = maxint;
               tactics_findBestAttackOrder ( finalPositions, attackOrder, enemy, 0, enemy->damage, finalDamage, finalOrder, finalAttackNum );


               pfield enemyField = getMap()->getField(enemy->xpos, enemy->ypos);
               for ( int i = 0; i < finalAttackNum && enemyField->vehicle == enemy; i++ ) {
                  checkKeys();
                  // if ( i+1 < finalAttackNum ) {
                  if ( i < finalAttackNum ) {
                     VehicleAttack va ( mapDisplay, NULL );
                     va.execute ( finalPositions[finalOrder[i]], -1, -1, 0, 0, -1 );
                     if ( va.getStatus() != 2 )
                        displaymessage("inconsistency #1 in AI::tactics attack", 1 );

                     va.execute ( NULL, enemy->xpos, enemy->ypos, 2, 0, -1 );
                     if ( va.getStatus() != 1000 )
                        displaymessage("inconsistency #1 in AI::tactics attack", 1 );


                     pvehicle a = finalPositions[finalOrder[i]];
                     TactVehicles::iterator att = find ( tactVehicles.begin(), tactVehicles.end(), a ) ;
                     tactVehicles.erase ( att );
                  }
               }

            } // if enemy

            bool processable = true;
            do {
               currentTarget++;
               if ( currentTarget != targets.end() )
                  if ( hemmingBonus == currentTarget->second.size()-1 ) {
                     for ( AffectedFields::iterator i = affectedFields.begin(); i != affectedFields.end(); i++ )
                        for ( MoveVariantContainer::iterator j = currentTarget->second.begin(); j != currentTarget->second.end(); j++ ) {
                            pvehicle veh = j->attacker;

                            // here are only vehicles that attack neighbouring fields; ranged attacks are handled as "directAttacks" earlier
                            if ( beeline ( *i, veh->getPosition()) < veh->typ->movement[log2(veh->height)]+20 )
                               processable = false;
                        }
                  } else
                     processable = false;
            } while ( !processable && currentTarget != targets.end() );

         } while ( currentTarget != targets.end() );

      } else {
         // no attacks are possible
         tactVehicles.clear();
      }
   }

   displaymessage2("tactics completed ... ");

   return result;
}

void AI :: tactics_findBestAttackOrder ( pvehicle* units, int* attackOrder, pvehicle enemy, int depth, int damage, int& finalDamage, int* finalOrder, int& finalAttackNum )
{
   if ( damage > enemy->damage )
      if ( (depth < finalAttackNum && damage==finalDamage) || damage > finalDamage ) {
         finalDamage = damage;
         finalAttackNum = depth;
         for ( int i = 0; i < sidenum; i++ )
            finalOrder[i] = attackOrder[i];
      }

   if ( damage >= 100 || depth >= 6  )
      return;

   for ( int i = 0; i< sidenum; i++ ) {
       bool found = false;
       for ( int j = 0; j < depth; j++ )
          if ( attackOrder[j] == i)
              found = true;
       if ( units[i] && !found ) {
         // attackOrder[3] = 1   means that the unit on position 1 should attack as third
         attackOrder[depth] = i;

         npush ( units[i]->xpos );
         npush ( units[i]->ypos );
         MapCoordinate mc = getNeighbouringFieldCoordinate ( MapCoordinate ( enemy->xpos, enemy->ypos), i );
         units[i]->xpos = mc.x;
         units[i]->ypos = mc.y;

         tunitattacksunit battle ( units[i], enemy );
         int newdamage = battle.dv.damage;
         battle.calc();
         newdamage = battle.dv.damage - newdamage;
         npop ( units[i]->ypos );
         npop ( units[i]->xpos );

         tactics_findBestAttackOrder ( units, attackOrder, enemy, depth+1, damage+newdamage, finalDamage, finalOrder, finalAttackNum );

         attackOrder[depth] = -1;
      }
   }

}


void AI :: tactics_findBestAttackUnits ( const MoveVariantContainer& mvc, MoveVariantContainer::iterator& m, pvehicle* positions, float value, pvehicle* finalPosition, float& finalValue, int unitsPositioned, int recursionDepth )
{
   if ( m == mvc.end() || unitsPositioned >= 6 || recursionDepth >= 8 ) {
      if ( value > finalValue ) {
         for ( int i = 0; i < sidenum; i++ )
            finalPosition[i] = positions[i] ;
         finalValue = value;
      }
   } else {
      for ( int i = 0; i< sidenum; i++ ) {
         if ( m->neighbouringFieldsReachable[i] && !positions[i] ) {
            positions[i] = m->attacker;
            if ( m->result > 0 )
               value += m->result;
            else
               value += 1;
            m++;
            tactics_findBestAttackUnits ( mvc, m, positions, value, finalPosition, finalValue, unitsPositioned+1, recursionDepth+1 );
            m--;
            value -= m->result;
            positions[i] = NULL;
         } else {
            m++;
            tactics_findBestAttackUnits ( mvc, m, positions, value, finalPosition, finalValue, unitsPositioned, recursionDepth+1 );
            m--;
         }
      }
   }
}



bool AI :: vehicleValueComp ( const pvehicle v1, const pvehicle v2 )
{
   return v1->aiparam[ v1->color/8 ]->getValue() < v2->aiparam[ v1->color/8 ]->getValue();
}

bool AI :: buildingValueComp ( const pbuilding v1, const pbuilding v2 )
{
   return v1->aiparam[ v1->color/8 ]->getValue() < v2->aiparam[ v1->color/8 ]->getValue();
}


AI::AiResult  AI :: container ( ccontainercontrols& cc )
{
   AiResult result;

   // move idle units out
   std::vector<pvehicle> idleUnits;
   for ( int j= 0; j < 32; j++ ) {
      pvehicle veh = cc.getloadedunit ( j );
      if ( veh )
         if ( veh->aiparam[ getPlayer() ]->task == AiParameter::tsk_nothing && cc.moveavail ( veh ))
            idleUnits.push_back ( veh );
   }
   // move the most important unit first, to get the best position
   sort ( idleUnits.begin(), idleUnits.end(), vehicleValueComp );

   for ( std::vector<pvehicle>::iterator i = idleUnits.begin(); i != idleUnits.end(); i++ ) {

      checkKeys();

      int simplyMove = 0;
      if ( getBestHeight ( *i ) != (*i)->height ) {
         VehicleMovement* vm = cc.movement ( *i );
         if ( vm ) {

            auto_ptr<VehicleMovement> avm ( vm );
            int stat = changeVehicleHeight ( *i, vm );
            if ( stat == -1 ) {
               result.unitsWaiting++;
               (*i)->aiparam[ getPlayer() ]->task = AiParameter::tsk_wait;
            } else {
               if ( stat== -2 ) {
                  simplyMove = 1;
               } else {
                  result.unitsMoved++;
                  (*i)->aiparam[ getPlayer() ]->task = AiParameter::tsk_nothing;
                  if ( (*i)->getMovement() >= minmalq && !(*i)->attacked && (*i)->weapexist() )
                     simplyMove = 1;
                  else {
                     VehicleMovement vm2 ( mapDisplay, NULL );
                     if ( vm2.available ( *i ))
                        moveToSavePlace ( *i, vm2 );
                  }
               }
            }

         }
      } else
         simplyMove = 2;

      if ( simplyMove ) {
         VehicleMovement* vm = cc.movement ( *i );
         // auto_ptr<VehicleMovement> vm ( cc.movement ( *i ) );
         if ( vm ) {
            vm->registerMapDisplay ( mapDisplay );
            auto_ptr<VehicleMovement> avm ( vm );

            VehicleAttack va ( NULL, NULL );
            int attack = 0;
            if ( va.available ( *i )) {
               TargetVector tv;
               getAttacks ( *vm, *i, tv, 0 );

               if ( tv.size() ) {
                  AiResult res = executeMoveAttack ( *i, tv );
                  result += res;
                  if ( !res.unitsDestroyed )
                     (*i)->aiparam[ getPlayer() ]->task = AiParameter::tsk_tactics;

                  attack = 1;
               }

            }
            if ( !attack ) {
               AiResult res =  moveToSavePlace ( *i, *vm );
               result += res;
               if ( !res.unitsDestroyed )
                  (*i)->aiparam[ getPlayer() ]->task = AiParameter::tsk_nothing;
            }
         }

      }
   }

   return result;
}


AI::AiResult AI::buildings( int process )
{
   AiResult result;

   displaymessage2("checking buildings ... ");

   int buildingCounter = 0;
   for ( tmap::Player::BuildingList::iterator bi = getMap()->player[ getPlayer() ].buildingList.begin(); bi != getMap()->player[ getPlayer() ].buildingList.end(); bi++ ) {
      buildingCounter++;
      displaymessage2("processing building %d", buildingCounter );

      cbuildingcontrols bc;
      bc.init ( *bi );

      result += container ( bc );

      checkKeys();
   }

   displaymessage2("building check completed... ");
   return result;
}


AI::AiResult AI::transports( int process )
{
   AiResult result;

   displaymessage2("checking transports ... ");

   int transportCounter = 0;
   for ( tmap::Player::VehicleList::iterator vi = getMap()->player[ getPlayer() ].vehicleList.begin(); vi != getMap()->player[ getPlayer() ].vehicleList.end(); vi++ ) {
      pvehicle veh = *vi;
      transportCounter++;
      displaymessage2("processing unit %d for transportation ", transportCounter );

      ctransportcontrols tc;
      tc.init ( veh );

      result += container ( tc );

      checkKeys();
   }

   displaymessage2("transport check completed... ");
   return result;
}



void AI::findStratPath ( vector<MapCoordinate>& path, pvehicle veh, int x, int y )
{
  StratAStar stratAStar ( this, veh );
  stratAStar.findPath ( AStar::HexCoord ( veh->xpos, veh->ypos ), AStar::HexCoord ( x, y ), path );
}


bool AI :: moveUnit ( pvehicle veh, const MapCoordinate& destination, bool intoBuildings, bool intoTransports )
{
   VehicleMovement vm ( mapDisplay, NULL );
   vm.execute ( veh, -1, -1, 0, -1, -1 );

   std::vector<MapCoordinate> path;
   AStar* ast = NULL;
   if ( veh->aiparam[getPlayer()]->job == AiParameter::job_conquer )
      ast = new HiddenAStar ( this, veh );
   else
      ast = new StratAStar ( this, veh );

   auto_ptr<AStar> ap ( ast );

   ast->findPath ( AStar::HexCoord ( veh->xpos, veh->ypos ), AStar::HexCoord ( destination.x, destination.y ), path );

   int xtogo = veh->xpos;
   int ytogo = veh->ypos;

   for ( int i = 0; i < path.size(); i++ ) {
      int x = path[i].x;
      int y = path[i].y;

      pfield fld = getfield ( x, y );
      if ( !fld)
         break;


      if ( vm.reachableFields.isMember ( x, y )
           && fieldaccessible ( fld, veh ) == 2
           && (!fld->building  || intoBuildings)
           && (!fld->vehicle || intoTransports )) {
         xtogo = x;
         ytogo = y;
      }
   }

   if ( xtogo != veh->xpos || ytogo != veh->ypos ) {
      if (  getfield ( xtogo, ytogo )->building )
         if ( checkReConquer ( getfield ( xtogo, ytogo )->building, veh ))
            return false;

      vm.execute ( NULL, xtogo, ytogo, 2, -1, -1 );
      if ( vm.getStatus() != 3 )
         displaymessage ( "AI :: strategy \n error in movement step 2 with unit %d", 1, veh->networkid );

      vm.execute ( NULL, xtogo, ytogo, 3, -1,  1 );
      if ( vm.getStatus() != 1000 )
         displaymessage ( "AI :: strategy \n error in movement step 3 with unit %d", 1, veh->networkid );

      return true;
   }
   return false;
}


AI::AiResult AI::strategy( void )
{
   AiResult result;

   int stratloop = 0;
   AiResult localResult;
   do {
      localResult.unitsMoved = 0;
      localResult.unitsWaiting = 0;
      stratloop++;

      for ( tmap::Player::VehicleList::iterator vi = getMap()->player[ getPlayer() ].vehicleList.begin(); vi != getMap()->player[ getPlayer() ].vehicleList.end(); vi++ ) {
         pvehicle veh = *vi;
         if ( veh->aiparam[ getPlayer() ]->job == AiParameter::job_fight ) {
            if ( veh->weapexist() && veh->aiparam[ getPlayer() ]->task != AiParameter::tsk_tactics ) {
               /*
               int orgmovement = veh->getMovement();
               int orgxpos = veh->xpos ;
               int orgypos = veh->ypos ;
               */

               VehicleMovement vm ( mapDisplay, NULL );
               if ( vm.available ( veh )) {
                  int x2, y2;

                  AI::Section* sec = sections.getBest ( 0, veh, &x2, &y2 );
                  if ( sec ) {
                     if ( moveUnit ( veh, MapCoordinate ( x2, y2 ), false))
                        localResult.unitsMoved++;

                     AiParameter& aip = *veh->aiparam[getPlayer()];

                     aip.dest.x = x2;
                     aip.dest.y = y2;
                     aip.dest.z = -1;
                  }
               }
            }
         } else {
            runUnitTask ( veh );
         }

         displaymessage2("strategy loop %d ; moved unit %d ... ", stratloop, localResult.unitsMoved );

         checkKeys();
      }
      result += localResult;
   } while ( localResult.unitsMoved );

   displaymessage2("strategy completed ... ");

   return result;
}


void AI::checkKeys ( void )
{
   if ( keypress() ) {
      tkey k = r_key();
      if ( k == ct_esc ) {
         mapDisplay = NULL;
         tlockdispspfld a;
         repaintdisplay();
      }
   }
}


void AI:: run ( void )
{
   mapDisplay = rmd;
   int startTime = ticker;
   AiResult res;

   cursor.hide();

   unitCounter = 0;
   _isRunning = true;
   _vision = visible_ago;

   int setupTime = ticker;
   tempsvisible = false;
   setup();
   tempsvisible = true;
   setupTime = ticker-setupTime;

   int serviceTime = ticker;
   issueServices( );
   executeServices();
   serviceTime = ticker-serviceTime;

   int conquerTime = ticker;
   checkConquer();
   conquerTime = ticker - conquerTime;

   int containerTime = ticker;
   buildings( 3 );
   transports ( 3 );
   containerTime = ticker-containerTime;

   int tacticsTime = ticker;
   do {
      res = tactics();
   } while ( res.unitsMoved );
   tacticsTime = ticker - tacticsTime;

   int strategyTime = ticker;
   strategy();
   strategyTime = ticker - strategyTime;

   buildings( 1 );
   transports ( 3 );
   _isRunning = false;
   if ( !mapDisplay )
      displaymap();
   int duration = ticker-startTime;

   if ( CGameOptions::Instance()->runAI == 2 )
      if ( duration > 100*60 )
         displaymessage ("The AI took %d seconds to run\n"
                         " setup: %d \n"
                         " service: %d \n"
                         " conquer: %d \n"
                         " container: %d \n"
                         " tactics: %d \n"
                         " strategy: %d \n",
           3, duration / 100,
              setupTime / 100,
              serviceTime/100,
              conquerTime /100,
              containerTime/100,
              tacticsTime/100,
              strategyTime/100 );
}


void AI :: read ( tnstream& stream )
{
   int version = stream.readInt ( );
   _isRunning = stream.readInt ();
   _vision = stream.readInt ( );
   unitCounter = stream.readInt ( );

   int i = stream.readInt();
   while ( i ) {
      ServiceOrder so ( this, stream );
      serviceOrders.push_back ( so );
      i = stream.readInt();
   }


   i = stream.readInt();
   while ( i ) {
      MapCoordinate mc;
      mc.read ( stream );

      AI::BuildingCapture bc;
      bc.read ( stream );

      buildingCapture[mc] = bc;

      i = stream.readInt();
   }

   config.lookIntoTransports = stream.readInt();
   config.lookIntoBuildings = stream.readInt( );
   config.wholeMapVisible = stream.readInt( );
   config.aggressiveness = stream.readFloat( );
   config.damageLimit = stream.readInt();
   config.resourceLimit.read( stream );
   config.ammoLimit = stream.readInt();
   config.maxCaptureTime = stream.readInt();
   int version2 = stream.readInt();
}

void AI :: write ( tnstream& stream ) const
{
   const int version = 100;
   stream.writeInt ( version );
   stream.writeInt ( _isRunning );
   stream.writeInt ( _vision );
   stream.writeInt ( unitCounter );

   for ( ServiceOrderContainer::const_iterator i = serviceOrders.begin(); i != serviceOrders.end(); i++) {
      stream.writeInt ( 1 );
      i->write ( stream );
   }

   stream.writeInt ( 0 );

   for ( map<MapCoordinate,BuildingCapture>::const_iterator i = buildingCapture.begin(); i != buildingCapture.end(); i++ ) {
      stream.writeInt ( 1 );
      i->first.write ( stream );
      i->second.write ( stream );
   }
   stream.writeInt ( 0 );

   stream.writeInt( config.lookIntoTransports );   /*  gegnerische transporter einsehen  */
   stream.writeInt( config.lookIntoBuildings );
   stream.writeInt( config.wholeMapVisible );
   stream.writeFloat( config.aggressiveness );   // 1: units are equally worth ; 2
   stream.writeInt( config.damageLimit );
   config.resourceLimit.write( stream );
   stream.writeInt( config.ammoLimit );
   stream.writeInt( config.maxCaptureTime );

   stream.writeInt ( version );
}


bool AI :: isRunning ( void )
{
   return _isRunning;
}


int AI :: getVision ( void )
{
   return _vision;
}

void AI :: showFieldInformation ( int x, int y )
{
   if ( !fieldThreats )
      calculateFieldThreats();

   const char* fieldinfo = "#font02#Field Information (%d,%d)#font01##aeinzug20##eeinzug10##crtp10#"
                           "threat orbit: %d\n"
                           "threat high-level flight: %d\n"
                           "threat flight: %d\n"
                           "threat low-level flight: %d\n"
                           "threat ground level: %d\n"
                           "threat floating: %d\n"
                           "threat submerged: %d\n"
                           "threat deep submerged: %d\n";

   char text[10000];
   int pos = x + y * activemap->xsize;
   sprintf(text, fieldinfo, x,y,fieldThreats[pos].threat[7], fieldThreats[pos].threat[6], fieldThreats[pos].threat[5],
                                fieldThreats[pos].threat[4], fieldThreats[pos].threat[3], fieldThreats[pos].threat[2],
                                fieldThreats[pos].threat[1], fieldThreats[pos].threat[0] );

   pfield fld = getfield (x, y );
   if ( fld->vehicle && fieldvisiblenow ( fld )) {
      char text2[1000];
      sprintf(text2, "\nunit nwid: %d ; typeid: %d", fld->vehicle->networkid, fld->vehicle->typ->id );
      strcat ( text, text2 );
      AiParameter& aip = *fld->vehicle->aiparam[getPlayer()];

      if ( fld->vehicle->aiparam ) {
         static char* tasks[] = { "nothing",
                                  "tactics",
                                  "tactwait",
                                  "stratwait",
                                  "wait",
                                  "strategy",
                                  "serviceRetreat",
                                  "move" };
        static char* jobs[] = { "undefined",
                                "fight",
                                "supply",
                                "conquer",
                                "build" };

         sprintf(text2, "\nunit value: %d; xtogo: %d, ytogo: %d; ztogo: %d;\njob %s ; task %s \n", aip.getValue(), aip.dest.x, aip.dest.y, aip.dest.z, jobs[aip.job], tasks[aip.task] );
         strcat ( text, text2 );
      }

      if ( aip.dest.x >= 0 && aip.dest.y >= 0 ) {
         getMap()->cleartemps ( 1 );
         getfield ( aip.dest.x, aip.dest.y )->a.temp = 1;
         displaymap();
      }

   }
   strcat ( text, "\n#font02#Section Information#font01##aeinzug20##eeinzug10##crtp10#");
   string s;

   Section& sec = sections.getForCoordinate ( x, y );

   s += "xp = ";
   s += strrr ( sec.xp );
   s += " ; yp = ";
   s += strrr ( sec.yp );
   s += "\n";
   const char* threattypes[4] = { "absUnitThreat", "avgUnitThreat", "absFieldThreat", "avgFieldThreat" };

   for ( int i = 0; i < 4; i++ ) {
      for ( int h = 0; h < 8; h++ ) {
         s += threattypes[i];
         s += " ";
         s += choehenstufen [h];
         s += " ";
         switch ( i ) {
            case 0: s += strrr ( sec.absUnitThreat.threat[h] );
                    break;
            case 1: s += strrr ( sec.avgUnitThreat.threat[h] );
                    break;
            case 2: s += strrr ( sec.absFieldThreat.threat[h] );
                    break;
            case 3: s += strrr ( sec.avgFieldThreat.threat[h] );
                    break;
         }
         s += "\n";
      }
      s += "\n";
   }

   for ( int j = 0; j < aiValueTypeNum; j++ ) {
      s+= "\nvalue ";
      s+= strrr ( j );
      s+= " = ";
      s+= strrr ( sec.value[j] );
   }

   strcat ( text, s.c_str() );
   tviewanytext vat;
   vat.init ( "AI information", text );
   vat.run();
   vat.done();
}


AI :: ~AI ( )
{
   if ( fieldThreats ) {
      delete[] fieldThreats;
      fieldThreats = NULL;
      fieldNum = 0;
   }
}
