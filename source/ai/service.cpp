/***************************************************************************
                          service.cpp  -  description
                             -------------------
    begin                : Fri Mar 30 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ai_common.h"



AI :: ServiceOrder :: ServiceOrder ( AI* _ai, VehicleService::Service _requiredService, int UnitID, int _pos )
{
   active = false;
   failure = 0;
   ai = _ai;
   targetUnitID = UnitID;
   requiredService = _requiredService;
   serviceUnitID = 0;
   position = _pos;
   time = ai->getMap()->time;

   nextServiceBuilding = NULL;
   if ( ai->getMap()->getUnit ( UnitID )->canMove()) {
      MapCoordinate3D sb = ai->findServiceBuilding ( *this, &nextServiceBuildingDistance );
      if ( sb.valid() )
         nextServiceBuilding = ai->getMap()->getField( sb )->building;
   }
}

AI :: ServiceOrder :: ServiceOrder ( AI* _ai, tnstream& stream )
{
  active = false;
  failure = 0;
  ai = _ai;
  read ( stream );
}

void AI :: ServiceOrder :: activate( ServiceOrder& so )
{
   so.active = true;
}


const int currentServiceOrderVersion = 10002;

void AI :: ServiceOrder :: read ( tnstream& stream )
{
  int version = stream.readInt( );
  if ( version > currentServiceOrderVersion )
     throw tinvalidversion ( "AI :: ServiceOrder", currentServiceOrderVersion, version );
  targetUnitID = stream.readInt (  );
  serviceUnitID = stream.readInt (  );
  position = stream.readInt (  );
  time.abstime = stream.readInt (  );
  if ( version >= 10001 ) {
     int i= stream.readInt();
     if ( i )  {
        MapCoordinate3D mc;
        mc.read ( stream );
        nextServiceBuilding = ai->getMap()->getField( mc )->building;
        nextServiceBuildingDistance = stream.readInt();
     } else
        nextServiceBuilding = NULL;

     if ( version >= 10002 )
        requiredService = VehicleService::Service( stream.readInt() );
  }
}

void AI :: ServiceOrder :: write ( tnstream& stream ) const
{
  stream.writeInt( currentServiceOrderVersion );
  stream.writeInt ( targetUnitID );
  stream.writeInt ( serviceUnitID );
  stream.writeInt ( position );
  stream.writeInt ( time.abstime );
  if ( nextServiceBuilding ) {
     stream.writeInt ( 1 );
     nextServiceBuilding->getEntry().write ( stream );
     stream.writeInt ( nextServiceBuildingDistance );
  } else {
     stream.writeInt ( 0 );
  }
  stream.writeInt ( requiredService );
}

bool AI :: ServiceOrder :: completelyFailed ()
{
  return failure > 2 ;
}


int AI::ServiceOrder::possible ( pvehicle supplier )
{
   if ( !ai->getMap()->getField( getTargetUnit()->getPosition())->unitHere ( getTargetUnit() ))
     return 0;
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
      for ( unsigned int j = 0; j < target.service.size(); j++ ) {
         serviceAmount += target.service[j].maxPercentage;
         if ( target.service[j].type == requiredService ) {
            bool enoughResources = false;
            if ( requiredService == VehicleService::srv_repair ) {
                if ( target.service[j].minAmount <  target.service[j].curAmount - 5 )
                  enoughResources = true;
            } else
               if ( target.service[j].targetPos == position ) {  // is this stuff the one we need
                  int requiredAmount = target.service[j].maxAmount - target.service[j].curAmount;
                  if ( requiredAmount > 0 ) {
                     if ( requiredService == VehicleService::srv_resource ) {
                        if ( target.service[j].targetPos == Resources::Fuel ) { // fuel  ; 30 fields requiredForHome
                            if ( requiredAmount < target.service[j].orgSourceAmount - supplier->typ->fuelConsumption * 30 )
                               enoughResources = true;
                        } else {
                            if ( getTargetUnit()->aiparam[ai->getPlayerNum()]->getJob() == AiParameter::job_supply ) {
                               if ( target.service[j].orgSourceAmount > requiredAmount )
                                  enoughResources = true;
                            } else
                               if ( target.service[j].maxPercentage > 80 )
                                  enoughResources = true;
                        }
                     }
                     if ( requiredService == VehicleService::srv_ammo ) {
                        if ( getTargetUnit()->aiparam[ai->getPlayerNum()]->getJob() == AiParameter::job_supply ) {
                           if ( target.service[j].orgSourceAmount > requiredAmount )
                              enoughResources = true;
                        } else
                           if ( target.service[j].maxPercentage > 80 )
                              enoughResources = true;
                     }
                  }
               }

            if ( enoughResources )
               result = true;

         }
      }

      if( result )
         return serviceAmount;
   }
   return 0;
}


bool AI::ServiceOrder::execute1st ( pvehicle supplier )
{
   pvehicle targ = getTargetUnit();
   MapCoordinate3D meet;

   vector<MapCoordinate3D> dest;
   for ( int h = 0; h < 8; h++ ) {
      if ( supplier->typ->height & ( 1<<h)) {
         for ( int i = 0; i < sidenum; i++ ) {
             int x = targ->xpos;
             int y = targ->ypos;
             getnextfield ( x, y, i );
             pfield fld = getfield ( x, y );
             if ( fld && fieldaccessible ( fld, supplier, 1<<h ) == 2 && !fld->building && !fld->vehicle ) {
                bool result = false;
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
                   for ( unsigned int j = 0; j < target.service.size(); j++ )
                      if ( target.service[j].type == requiredService )
                         result = true;
                }

                tus.restore();

                if ( result )
                   dest.push_back ( MapCoordinate3D(x, y, 1<<h ));
             }
         }
      }
   }

   if ( !dest.size() )
      return false;

   AStar3D ast ( ai->getMap(), supplier, true, supplier->typ->maxSpeed()*6 );
   AStar3D::Path path;
   ast.findPath(  path, dest );
   if ( path.size() ) {
      meet = *path.rbegin();

      int supplySpeed = supplier->maxMovement();
      if ( !canWait() )
         supplySpeed += targ->maxMovement();
      if ( supplySpeed == 0 )
         fatalError ( "AI::ServiceOrder::execute1st - supplySpeed is 0 ");

      if ( !targ->maxMovement() || beeline(supplier,targ) / supplySpeed < nextServiceBuildingDistance/targ->maxMovement() || nextServiceBuildingDistance < 0 ) {
         supplier->aiparam[ai->getPlayerNum()]->dest = meet;
         supplier->aiparam[ai->getPlayerNum()]->setTask( AiParameter::tsk_move );
         supplier->aiparam[ai->getPlayerNum()]->dest_nwid = targ->networkid;

         setServiceUnit ( supplier );
         return true;
      }
   } else
      return false;
}

void AI::ServiceOrder::setServiceUnit ( pvehicle veh )
{
   if ( veh )
      serviceUnitID = veh->networkid;
   else
      serviceUnitID = 0;
}

void AI::ServiceOrder::releaseServiceUnit ( ServiceOrder& so )
{
   pvehicle veh = so.getServiceUnit();
   if ( veh ) {
      so.setServiceUnit ( NULL );
      veh->aiparam[so.ai->getPlayerNum()]->resetTask();
   }
}


bool AI::ServiceOrder::timeOut ( )
{
  GameTime t = time;
  t.set ( t.turn() + 2, t.move() );
  return ( t.abstime <= ai->getMap()->time.abstime );
}

bool AI::ServiceOrder::canWait( )
{
   if ( requiredService == VehicleService::srv_repair )
      return false;

   if ( requiredService == VehicleService::srv_ammo )
      if ( getTargetUnit()->ammo[position] )
         return true;


   bool fuelLack = requiredService == VehicleService::srv_resource && position == 2 ;
   bool couldWait = fuelLack || !timeOut();
   if ( couldWait )
      return serviceUnitExists();
   else
      return false;
}

bool AI::ServiceOrder::serviceUnitExists()
{
   for ( Player::VehicleList::iterator vi = ai->getPlayer().vehicleList.begin(); vi != ai->getPlayer().vehicleList.end(); vi++ )
       if ( possible ( *vi ))
          return true;
   return false;
}


bool AI::ServiceOrder::valid (  ) const
{
   switch ( requiredService ) {
      case VehicleService::srv_repair     : return getTargetUnit()->damage > 0;
      case VehicleService::srv_ammo       : return getTargetUnit()->ammo[position] < getTargetUnit()->typ->weapons.weapon[position].count;
      case VehicleService::srv_resource   : return getTargetUnit()->tank.resource(position) < getTargetUnit()->typ->tank.resource(position);
      default: return false;
   }
}

bool AI::ServiceOrder::invalid ( const ServiceOrder& so )
{
   return !so.valid();
}

bool AI::ServiceOrder::operator==( const ServiceOrder& so ) const
{
   return    targetUnitID    == so.targetUnitID
          && requiredService == so.requiredService
          && ai              == so.ai
          && position        == so.position;
}

bool AI::ServiceOrder::operator!=( const ServiceOrder& so ) const
{
  return ! (*this == so);
}


AI :: ServiceOrder :: ~ServiceOrder (  )
{
   if ( active ) {
      pvehicle serv = getServiceUnit();
      if ( serv )
         serv->aiparam[ai->getPlayerNum()]->resetTask();
   }
}

AI::ServiceOrder& AI :: issueService ( VehicleService::Service requiredService, int UnitID, int pos )
{
   ServiceOrder so ( this, requiredService, UnitID, pos );
   ServiceOrderContainer::iterator soi = find ( serviceOrders.begin(), serviceOrders.end(), so );
   if ( soi == serviceOrders.end() ) {
      serviceOrders.push_back ( so );
      ServiceOrder::activate ( *serviceOrders.rbegin() );
      return *serviceOrders.rbegin();
   } else {
      return *soi;
   }
}



void AI :: issueServices ( )
{
   displaymessage2("issuing services ... ");
   serviceOrders.erase ( remove_if ( serviceOrders.begin(), serviceOrders.end(), ServiceOrder::targetDestroyed ), serviceOrders.end());

   for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); vi++ ) {
      pvehicle veh = *vi;
      if ( getMap()->getField( veh->getPosition() )->unitHere( veh )) {
         if ( veh->damage > config.damageLimit )
            issueService ( VehicleService::srv_repair, veh->networkid ) ;

         for ( int i = 0; i< resourceTypeNum; i++ )
            if ( veh->maxMovement() ) // stationary units are ignored
               if ( veh->tank.resource(i) < veh->typ->tank.resource(i) * config.resourceLimit.resource(i) / 100 )
                  issueService ( VehicleService::srv_resource, veh->networkid, i);

         for ( int w = 0; w< veh->typ->weapons.count; w++ )
            if ( veh->typ->weapons.weapon[w].count )
               if ( veh->ammo[w] <= veh->typ->weapons.weapon[w].count * config.ammoLimit / 100 )
                  issueService ( VehicleService::srv_ammo, veh->networkid, w);
      }
   }
}

AI::ServiceOrder& AI :: issueRefuelOrder ( pvehicle veh, bool returnImmediately )
{
   ServiceOrder& so = issueService ( VehicleService::srv_resource, veh->networkid, Resources::Fuel);
   if ( returnImmediately ) {
      if ( RefuelConstraint::necessary ( veh, *this )) {
         RefuelConstraint apl ( *this, veh );
         veh->aiparam[getPlayerNum()]->dest = apl.getNearestRefuellingPosition ( true, true, false );
         veh->aiparam[getPlayerNum()]->setTask( AiParameter::tsk_serviceRetreat );
         runUnitTask ( veh );
      } else {

      }
   }
   return so;
}



MapCoordinate3D AI :: findServiceBuilding ( const ServiceOrder& so, int* distance )
{
   pvehicle veh = so.getTargetUnit();
   if ( getMap()->getField( veh->getPosition())->unitHere ( veh ))
      if ( !veh->canMove() )
         return MapCoordinate3D ( -1, -1, 1 );

   /*
   RefuelConstraint* rc = NULL;
   if ( RefuelConstraint::necessary  ( veh, this ))
      rc = new RefuelConstraint ( veh, this );
   */

   int maxMovement = 0;
   for ( int i = 0; i < 8; i++ )
      maxMovement = max ( maxMovement, veh->typ->movement[i] );

   AStar3D astar ( getMap(), veh, true, maxMovement*10 );
   astar.findAllAccessibleFields ( );

   pbuilding bestBuilding = NULL;
   int bestDistance = maxint;
   MapCoordinate3D bestPos;

   pbuilding bestBuilding_p = NULL;
   int bestDistance_p = maxint;
   MapCoordinate3D bestPos_p;

   for ( Player::BuildingList::iterator bi = getPlayer().buildingList.begin(); bi != getPlayer().buildingList.end(); bi++ ) {
      pbuilding bld = *bi;
      if ( astar.getFieldAccess( bld->getEntry())  ) { // ####TRANS
         MapCoordinate3D buildingPos = bld->getEntry();
         buildingPos.setnum ( buildingPos.x, buildingPos.y, -1 );
         /*
         if ( !(bld->typ->loadcapability & buildingPos.z))
            buildingPos.z = 1 << log2 ( astar.getFieldAccess(bld->getEntry()) & bld->typ->loadcapability );
         */

         bool loadable = true;
         if ( loadable ) {
            int fullfillableServices = 0;
            int partlyFullfillabelServices = 0;
            switch ( so.requiredService ) {
               case VehicleService::srv_repair : if ( bld->canRepair( veh ) ) {
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
                                                     if ( avail < needed )
                                                        avail += bld->netResourcePlus( ) * config.waitForResourcePlus;

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

                                                   for ( int i = 0; i < veh->typ->weapons.count; i++ )
                                                      if ( veh->typ->weapons.weapon[i].requiresAmmo() )
                                                         ammoNeeded[ veh->typ->weapons.weapon[i].getScalarWeaponType() ] += veh->typ->weapons.weapon[i].count - veh->ammo[i];

                                                   Resources needed;
                                                   for ( int  j = 0; j < waffenanzahl; j++ ) {
                                                       int n = ammoNeeded[j] - bld->ammo[j];
                                                       if ( n > 0 )
                                                          if ( bld->typ->special & cgammunitionproductionb ) {
                                                             for ( int r = 0; r < resourceTypeNum; r++ )
                                                                needed.resource(r) += (n+4)/5 * cwaffenproduktionskosten[j][r];
                                                          } else
                                                             missing++;
                                                   }
                                                   Resources avail = bld->getResource ( needed, 1 );
                                                   if ( avail < needed )
                                                      avail += bld->netResourcePlus( ) * config.waitForResourcePlus;

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
               AStar3D::Node* nde = astar.fieldVisited(buildingPos);
               if ( nde ) {
                  int dist = int( nde->gval );
                  if ( dist < bestDistance ) {
                     bestDistance = dist;
                     bestBuilding = bld;
                     bestPos = buildingPos;
                  }
               }
            } else
               if ( partlyFullfillabelServices ) {
                  AStar3D::Node* nde = astar.fieldVisited(buildingPos);
                  if ( nde ) {
                     int dist = int( nde->gval );
                     if ( dist < bestDistance_p ) {
                        bestDistance_p = dist;
                        bestBuilding_p = bld;
                        bestPos_p = buildingPos;
                     }
                  }
               }
         }
      }
   }

   if ( bestBuilding && (bestDistance < bestDistance_p*3)) {
      if ( distance )
         *distance = bestDistance;
      return bestPos;
   } else {
      if ( distance )
         *distance = bestDistance_p;
      return bestPos_p;
   }
}




bool AI :: ServiceTargetEquals :: operator() (const ServiceOrder& so ) const
{
   pvehicle t = so.getTargetUnit();
   return target== t;
}



void AI::removeServiceOrdersForUnit ( const pvehicle veh )
{
   serviceOrders.erase ( remove_if ( serviceOrders.begin(), serviceOrders.end(), ServiceTargetEquals( veh ) ), serviceOrders.end());
}

void AI :: runServiceUnit ( pvehicle supplyUnit )
{
   bool destinationReached = false;
   typedef multimap<float,ServiceOrder*> ServiceMap;
   ServiceMap serviceMap;

   // building a list of all fullfillable service tasks
   for ( ServiceOrderContainer::iterator i = serviceOrders.begin(); i != serviceOrders.end(); i++ ) {
       if ( !i->getServiceUnit() && i->getTargetUnit() ) {
          int poss = i->possible( supplyUnit );
          if ( poss ) {
             float f =  i->getTargetUnit()->aiparam[getPlayerNum()]->getValue() * poss/100 / beeline( i->getTargetUnit() ,supplyUnit );
             serviceMap.insert(make_pair(f,&(*i)));
          }
  /*
          veh->aiparam[getPlayerNum()]->getTask() = AiParameter::tsk_serviceRetreat;
          pbuilding bld = findServiceBuilding( **i );
          if ( bld )
             veh->aiparam[ getPlayerNum() ]->dest = bld->getEntry ( );
  */
       }
   }
   int target = 0;
   int counter = 0;
   for ( ServiceMap::reverse_iterator ri = serviceMap.rbegin(); ri != serviceMap.rend(); ri++ ) {
      displaymessage2("test service %d", ++counter);
      if ( ri->second->execute1st( supplyUnit ) ) {
         target = supplyUnit->aiparam[getPlayerNum()]->dest_nwid;
         destinationReached = runUnitTask ( supplyUnit );
         break;
      }
   }

   if ( destinationReached ) {
      VehicleService vs ( mapDisplay, NULL );
      if ( !vs.available ( supplyUnit ))
         displaymessage ("AI :: runServiceUnit ; inconsistency in VehicleService.availability",1 );

      vs.execute ( supplyUnit, -1, -1, 0, -1, -1 );
      VehicleService::TargetContainer::iterator i = vs.dest.find ( target );
      if ( i != vs.dest.end() ) {
         vs.fillEverything ( target, true );
         pvehicle targetUnit = getMap()->getUnit(target);
         if ( targetUnit->aiparam[getPlayerNum()]->getTask() == AiParameter::tsk_serviceRetreat )
             targetUnit->aiparam[getPlayerNum()]->resetTask();


         removeServiceOrdersForUnit ( targetUnit );

         // search for next unit to be serviced
         runServiceUnit( supplyUnit );
      } else
         displaymessage ("AI :: runServiceUnit ; inconsistency in VehicleService.execution level 0 ; unit is %d",1, supplyUnit->networkid  );

   }
}


AI::AiResult AI :: executeServices ( )
{
  // removing all service orders for units which no longer exist
  removeServiceOrdersForUnit ( NULL );
  serviceOrders.erase ( remove_if ( serviceOrders.begin(), serviceOrders.end(), ServiceOrder::invalid ), serviceOrders.end());

  for_each ( serviceOrders.begin(), serviceOrders.end(), ServiceOrder::releaseServiceUnit );

  AiResult res;

  Player::VehicleList::iterator nvi;
  for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); ) {
      nvi = vi;
      ++nvi;

      pvehicle veh = *vi;
      checkKeys();
      int counter = 0;
      if ( veh->aiparam[getPlayerNum()]->getJob() == AiParameter::job_supply ) {
         runServiceUnit ( veh );
      }

      vi = nvi;
  }

  removeServiceOrdersForUnit ( NULL );
  int counter = 0;
  for ( ServiceOrderContainer::iterator i = serviceOrders.begin(); i != serviceOrders.end(); i++ ) {
      if ( !i->canWait() ) {
         displaymessage2("executing priority service order %d", ++counter);
         pvehicle veh = i->getTargetUnit();
         if ( i->getServiceUnit() ) {
            if ( veh->canMove() ) {
               veh->aiparam[ getPlayerNum() ]->dest = i->getServiceUnit()->getPosition();
               veh->aiparam[ getPlayerNum() ]->dest_nwid = i->getServiceUnit()->networkid;
               veh->aiparam[ getPlayerNum() ]->setTask( AiParameter::tsk_serviceRetreat );
            }
         } else {

            //! not all service tasks are refuelling; don't land immediately
            if ( (veh->height & ( chtieffliegend | chfliegend | chhochfliegend )) && veh->typ->fuelConsumption && false ) {
               RefuelConstraint apl ( *this, veh );
               MapCoordinate3D dst = apl.getNearestRefuellingPosition( true, true, false );
               if ( dst.x != -1 ) {
                  veh->aiparam[ getPlayerNum() ]->dest = dst;
                  veh->aiparam[ getPlayerNum() ]->setTask( AiParameter::tsk_serviceRetreat );
               }
            } else {
               MapCoordinate3D dest = findServiceBuilding( *i );
               if ( dest.valid() && veh->canMove() ) {
                  veh->aiparam[ getPlayerNum() ]->dest = dest;
                  veh->aiparam[ getPlayerNum() ]->setTask( AiParameter::tsk_serviceRetreat );
               } else {
                  // displaymessage("warning: no service building found found for unit %s - %d!",1, veh->typ->description, veh->typ->id);
               }
            }
         }

      }
  }


  counter = 0;
  for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); ) {
     nvi = vi;
     ++nvi;

     pvehicle veh = *vi;
     checkKeys();

     if ( veh->canMove() && veh->aiparam[getPlayerNum()]->getTask() == AiParameter::tsk_serviceRetreat ) {
        displaymessage2("retreating with unit %d", ++counter);
        int nwid = veh->networkid;
        moveUnit ( veh, veh->aiparam[ getPlayerNum() ]->dest, true );

        // the unit may have been shot down
        if ( getMap()->getUnit ( nwid )) {
           if ( veh->getPosition3D() == veh->aiparam[ getPlayerNum() ]->dest ) {
              VehicleService vc ( mapDisplay, NULL );
              pfield fld = getfield ( veh->xpos, veh->ypos );
              if ( fld->building ) {
                 MapCoordinate mc = fld->building->getEntry();
                 vc.execute ( NULL, mc.x, mc.y, 0, -1, -1 );

                 if ( vc.getStatus () == 2 ) {
                    if ( vc.dest.find ( veh->networkid ) != vc.dest.end() )
                       vc.fillEverything ( veh->networkid, true );
                    else
                       displaymessage ( "AI :: executeServices / Vehicle cannot be serviced (1) ", 1);
                 } else
                    displaymessage ( "AI :: executeServices / Vehicle cannot be serviced (2) ", 1);

                 removeServiceOrdersForUnit ( veh );

                 if ( veh->aiparam[getPlayerNum()]->getTask() == AiParameter::tsk_serviceRetreat )
                     veh->aiparam[getPlayerNum()]->resetTask();

              }
           }
        }
     }
     vi = nvi;
  }

  /*
  for ( ServiceOrderContainer::iterator i = serviceOrders.begin(); i != serviceOrders.end(); i++ ) {
      if ( !i->timeOut() && i->requiredService == VehicleService::srv_repair ) {
         pvehicle veh = i->getTargetUnit();
         veh->aiparam[getPlayerNum()]->getTask() = AiParameter::tsk_serviceRetreat;
         pbuilding bld = findServiceBuilding( *i );
         if ( bld )
            veh->aiparam[ getPlayerNum() ]->dest = bld->getEntry ( );
         else {
            // displaymessage("warning: no service building found found for unit %s - %d!",1, veh->typ->description, veh->typ->id);
         }

      }
  }
*/
  return res;

}
