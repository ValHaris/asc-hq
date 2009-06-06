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

#include "../actions/repairunitcommand.h"
#include "../actions/servicing.h"
#include "../actions/servicecommand.h"


AI :: ServiceOrder :: ServiceOrder ( AI* _ai, AI::ServiceOrder::Service _requiredService, int UnitID, int _pos )
{
   active = false;
   failure = 0;
   ai = _ai;
   targetUnitID = UnitID;
   requiredService = _requiredService;
   serviceUnitID = 0;
   position = _pos;
   time = ai->getMap()->time;

   nextServiceBuildingDistance = -1;
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
        requiredService = ServiceOrder::Service( stream.readInt() );
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


int AI::ServiceOrder::getServiceID() const
{
   if ( requiredService == srv_ammo )
      return 2000+getTargetUnit()->typ->weapons.weapon[position].getScalarWeaponType();
   
   if ( requiredService == srv_resource )
      return 1000+position;
   
   return -1;
}


int AI::ServiceOrder::possible ( Vehicle* supplier )
{
   if ( !ai->getMap()->getField( getTargetUnit()->getPosition())->unitHere ( getTargetUnit() ))
     return 0;
   
   TransferHandler transferHandler( supplier, getTargetUnit(), TransferHandler::ignoreHeight + TransferHandler::ignoreDistance );
   
   int serviceAmount = 0;
   for ( TransferHandler::Transfers::const_iterator i = transferHandler.getTransfers().begin(); i != transferHandler.getTransfers().end(); ++i ) {
      if (  (*i)->getID() == getServiceID() ) {
         int now = (*i)->getAmount(getTargetUnit());
         int poss = (*i)->getMax( getTargetUnit(), true); // the amound that the servicer can provide
         int mx = (*i)->getMax( getTargetUnit(), false); // the maximum storage 
         if ( poss > now && mx > now ) {
            int percentage = 100 * (poss-now) / (mx - now );
            serviceAmount += percentage;
         }
      }
      
   }
   return serviceAmount;
}


bool AI::ServiceOrder::execute1st ( Vehicle* supplier )
{
   Vehicle* targ = getTargetUnit();
   MapCoordinate3D meet;

   vector<MapCoordinate3D> dest;
   for ( int h = 0; h < 8; h++ ) {
      if ( supplier->typ->height & ( 1<<h)) {
         for ( int i = 0; i < sidenum; i++ ) {
             int x = targ->xpos;
             int y = targ->ypos;
             getnextfield ( x, y, i );
             tfield* fld = ai->getMap()->getField ( x, y );
             if ( fld && fieldAccessible ( fld, supplier, 1<<h ) == 2 && !fld->building && !fld->vehicle ) {
                bool result = false;
                TemporaryContainerStorage tus ( supplier );
                supplier->xpos = x;
                supplier->ypos = y;
                supplier->height = 1 << h;

                
                TransferHandler transferHandler( supplier, getTargetUnit() );
   
                for ( TransferHandler::Transfers::const_iterator i = transferHandler.getTransfers().begin(); i != transferHandler.getTransfers().end(); ++i ) 
                   if (  (*i)->getID() == getServiceID() ) 
                      result = true;

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
   }
   return false;
}

void AI::ServiceOrder::setServiceUnit ( Vehicle* veh )
{
   if ( veh )
      serviceUnitID = veh->networkid;
   else
      serviceUnitID = 0;
}

void AI::ServiceOrder::releaseServiceUnit ( ServiceOrder& so )
{
   Vehicle* veh = so.getServiceUnit();
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
   if ( requiredService == srv_repair )
      return false;

   if ( requiredService == srv_ammo ) {
      if ( getTargetUnit()->ammo[position] )
         return true;

      if ( ai->targetsNear( getTargetUnit() ))
         return true;
   }

   bool fuelLack = requiredService == srv_resource && position == 2 ;
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
      case srv_repair     : return getTargetUnit()->damage > 0;
      case srv_ammo       : return getTargetUnit()->ammo[position] < getTargetUnit()->typ->weapons.weapon[position].count;
      case srv_resource   : return getTargetUnit()->getResource(getTargetUnit()->getStorageCapacity().resource(position), position, 1) < getTargetUnit()->getStorageCapacity().resource(position);
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
      Vehicle* serv = getServiceUnit();
      if ( serv )
         serv->aiparam[ai->getPlayerNum()]->resetTask();
   }
}

AI::ServiceOrder& AI :: issueService ( AI::ServiceOrder::Service requiredService, int UnitID, int pos )
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
      Vehicle* veh = *vi;
      if ( getMap()->getField( veh->getPosition() )->unitHere( veh )) {
         if ( veh->damage > config.damageLimit )
            issueService ( ServiceOrder::srv_repair, veh->networkid ) ;

         for ( int i = 0; i< resourceTypeNum; i++ )
            if ( veh->maxMovement() ) // stationary units are ignored
               if ( veh->getResource(maxint, i, 1) < veh->getStorageCapacity().resource(i) * config.resourceLimit.resource(i) / 100 )
                  issueService ( ServiceOrder::srv_resource, veh->networkid, i);

         for ( int w = 0; w< veh->typ->weapons.count; w++ )
            if ( veh->typ->weapons.weapon[w].count )
               if ( veh->ammo[w] <= veh->typ->weapons.weapon[w].count * config.ammoLimit / 100 )
                  issueService ( ServiceOrder::srv_ammo, veh->networkid, w);
      }
   }
}

AI::ServiceOrder& AI :: issueRefuelOrder ( Vehicle* veh, bool returnImmediately )
{
   ServiceOrder& so = issueService ( ServiceOrder::srv_resource, veh->networkid, Resources::Fuel);
   if ( returnImmediately ) {
      if ( RefuelConstraint::necessary ( veh, *this )) {
         RefuelConstraint apl ( *this, veh );
         veh->aiparam[getPlayerNum()]->dest = apl.getNearestRefuellingPosition ( true, true, false );
         if ( veh->aiparam[getPlayerNum()]->dest.valid() ) {
            veh->aiparam[getPlayerNum()]->setTask( AiParameter::tsk_serviceRetreat );
            runUnitTask ( veh );
         }
      } else {

      }
   }
   return so;
}



MapCoordinate3D AI :: findServiceBuilding ( const ServiceOrder& so, int* distance )
{
   Vehicle* veh = so.getTargetUnit();
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

   Building* bestBuilding = NULL;
   int bestDistance = maxint;
   MapCoordinate3D bestPos;

   Building* bestBuilding_p = NULL;
   int bestDistance_p = maxint;
   MapCoordinate3D bestPos_p;

   for ( Player::BuildingList::iterator bi = getPlayer().buildingList.begin(); bi != getPlayer().buildingList.end(); bi++ ) {
      Building* bld = *bi;
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
               case ServiceOrder::srv_repair : if ( bld->canRepair( veh ) ) {
                                                    int mr =  bld->getMaxRepair( veh );
                                                    if ( mr == 0 )
                                                       fullfillableServices++;

                                                    if ( mr < veh->damage )
                                                       partlyFullfillabelServices++;

                                                 }
                                                 break;
                  case ServiceOrder::srv_resource:  {
                                                     Resources needed =  veh->getStorageCapacity() - veh->getResource(Resources(maxint,maxint,maxint), true);
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
                  case ServiceOrder::srv_ammo :  {
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
                                                       if ( n > 0 ) {
                                                          if ( bld->typ->hasFunction( ContainerBaseType::AmmoProduction  )) {
                                                             for ( int r = 0; r < resourceTypeNum; r++ )
                                                                needed.resource(r) += (n+4)/5 * ammoProductionCost[j][r];
                                                          } else
                                                             missing++;
                                                       }
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
               const AStar3D::Node* nde = astar.fieldVisited(buildingPos);
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
                  const AStar3D::Node* nde = astar.fieldVisited(buildingPos);
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
   Vehicle* t = so.getTargetUnit();
   return target== t;
}



void AI::removeServiceOrdersForUnit ( const Vehicle* veh )
{
   serviceOrders.erase ( remove_if ( serviceOrders.begin(), serviceOrders.end(), ServiceTargetEquals( veh ) ), serviceOrders.end());
}

void AI :: runServiceUnit ( Vehicle* supplyUnit )
{
   bool destinationReached = false;
   typedef multimap<float,ServiceOrder*> ServiceMap;
   ServiceMap serviceMap;

   // building a list of all fullfillable service tasks
   for ( ServiceOrderContainer::iterator i = serviceOrders.begin(); i != serviceOrders.end(); i++ ) {
       if ( !i->getServiceUnit() && i->getTargetUnit() ) {
          int poss = i->possible( supplyUnit );
          if ( poss && beeline( i->getTargetUnit() ,supplyUnit ) ) {
             float f =  i->getTargetUnit()->aiparam[getPlayerNum()]->getValue() * poss/100 / beeline( i->getTargetUnit() ,supplyUnit );
             serviceMap.insert(make_pair(f,&(*i)));
          }
  /*
          veh->aiparam[getPlayerNum()]->getTask() = AiParameter::tsk_serviceRetreat;
          Building* bld = findServiceBuilding( **i );
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

   Vehicle* targetUnit = getMap()->getUnit(target);
   if ( destinationReached && targetUnit ) {
      
      auto_ptr<ServiceCommand> sc ( new ServiceCommand( supplyUnit ));
      sc->setDestination( targetUnit );
      sc->getTransferHandler().fillDest();
      sc->saveTransfers();
      ActionResult res = sc->execute( getContext() );
      if ( res.successful() )
         sc.release();
      else
         displayActionError(res);
     
      
      if ( targetUnit->aiparam[getPlayerNum()]->getTask() == AiParameter::tsk_serviceRetreat )
          targetUnit->aiparam[getPlayerNum()]->resetTask();

      removeServiceOrdersForUnit ( targetUnit );

      // search for next unit to be serviced
      runServiceUnit( supplyUnit );
   }
}


AI::AiResult AI :: executeServices ( )
{
  // removing all service orders for units which no longer exist
  removeServiceOrdersForUnit ( NULL );
  serviceOrders.erase ( remove_if ( serviceOrders.begin(), serviceOrders.end(), ServiceOrder::invalid ), serviceOrders.end());

  for_each ( serviceOrders.begin(), serviceOrders.end(), ServiceOrder::releaseServiceUnit );

  AiResult res;

  vector<int> unitIds;
  for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); ++vi )
     unitIds.push_back ( (*vi)->networkid );


  for ( vector<int>::iterator vi = unitIds.begin(); vi != unitIds.end(); ++vi ) {
      Vehicle* veh = getMap()->getUnit( *vi );
      if ( veh ) {
        checkKeys();
        if ( veh->aiparam[getPlayerNum()]->getJob() == AiParameter::job_supply ) 
           runServiceUnit ( veh );
      }
  }

  removeServiceOrdersForUnit ( NULL );
  int counter = 0;
  for ( ServiceOrderContainer::iterator i = serviceOrders.begin(); i != serviceOrders.end();  ) {
      ServiceOrderContainer::iterator nxt = i;
      ++nxt;

      if ( !i->canWait() ) {
         displaymessage2("executing priority service order %d", ++counter);
         Vehicle* veh = i->getTargetUnit();
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

      i = nxt;
  }


  unitIds.clear();
  for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); ++vi ) 
     unitIds.push_back ( (*vi)->networkid );
  
  
  counter = 0;
  for ( vector<int>::iterator vi = unitIds.begin(); vi != unitIds.end(); ++vi ) {
     Vehicle* veh = getMap()->getUnit( *vi );
     checkKeys();

     if ( veh && veh->canMove() && veh->aiparam[getPlayerNum()]->getTask() == AiParameter::tsk_serviceRetreat ) {
        displaymessage2("retreating with unit %d", ++counter);
        int nwid = veh->networkid;
        moveUnit ( veh, veh->aiparam[ getPlayerNum() ]->dest, true );

        // the unit may have been shot down
        Vehicle* veh = getMap()->getUnit ( nwid );
        if ( veh ) {
           if ( veh->getPosition3D() == veh->aiparam[ getPlayerNum() ]->dest ) {
              tfield* fld = getMap()->getField ( veh->xpos, veh->ypos );
              if ( fld->building ) {
                 auto_ptr<ServiceCommand> sc ( new ServiceCommand( fld->building ));
                 sc->setDestination( veh );
                 sc->getTransferHandler().fillDest();
                 sc->saveTransfers();
                 ActionResult res = sc->execute( getContext() );
                 if ( res.successful() )
                    sc.release();
                 else
                    displayActionError(res);
                 
                 
                 if ( veh->damage ) {
                    if( RepairUnitCommand::avail( fld->building )) {
                       auto_ptr<RepairUnitCommand> ruc ( new RepairUnitCommand( fld->building ));
                       if ( ruc->validTarget( veh )) {
                          ruc->setTarget( veh );
                          ActionResult res = ruc->execute( getContext() );
                          if ( res.successful() )
                             ruc.release();
                       }
                    }
                 }
                 
                 removeServiceOrdersForUnit ( veh );

                 if ( veh->aiparam[getPlayerNum()]->getTask() == AiParameter::tsk_serviceRetreat )
                     veh->aiparam[getPlayerNum()]->resetTask();

              }
           }
        }
     }
  }

  /*
  for ( ServiceOrderContainer::iterator i = serviceOrders.begin(); i != serviceOrders.end(); i++ ) {
      if ( !i->timeOut() && i->requiredService == VehicleService::srv_repair ) {
         Vehicle* veh = i->getTargetUnit();
         veh->aiparam[getPlayerNum()]->getTask() = AiParameter::tsk_serviceRetreat;
         Building* bld = findServiceBuilding( *i );
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
