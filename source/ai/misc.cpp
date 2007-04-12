/***************************************************************************
                          misc.cpp  -  description
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



bool AI :: runUnitTask ( Vehicle* veh )
{
   if ( veh->aiparam[getPlayerNum()]->getTask() == AiParameter::tsk_move || veh->aiparam[getPlayerNum()]->getTask() == AiParameter::tsk_serviceRetreat ) {
      bool moveIntoBuildings = false;
      if ( veh->aiparam[getPlayerNum()]->getJob() == AiParameter::job_conquer ||
         veh->aiparam[getPlayerNum()]->getTask() == AiParameter::tsk_serviceRetreat ||
         veh->aiparam[getPlayerNum()]->getJob() == AiParameter::job_script )
         moveIntoBuildings = true;

      int nwid = veh->networkid;
      moveUnit ( veh, veh->aiparam[getPlayerNum()]->dest, moveIntoBuildings );
      if ( getMap()->getUnit( nwid ))
         if ( veh->getPosition() == veh->aiparam[getPlayerNum()]->dest ) {
            veh->aiparam[getPlayerNum()]->resetTask ();
            return true;
         }
   }

   return false;
}



int  AI :: getBestHeight (  Vehicle* veh )
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
         float t = sections.getForCoordinate( veh->xpos, veh->ypos ).avgFieldThreat.threat[ veh->getValueType(1<<k) ];

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
         // we save the unit, because we modify a lot of it that should be undone later
         TemporaryContainerStorage tus (veh);
         veh->height = 1<<j;

         // calculating the new threat of the unit
         calculateThreat ( veh );
         Section& sec = sections.getForCoordinate( veh->xpos, veh->ypos );

         float value = veh->aiparam[getPlayerNum()]->getValue();
         if ( veh->typ->movement[j] )
            value *=  log( double(veh->typ->movement[j]) );

         float threat = sec.avgFieldThreat.threat[ veh->aiparam[getPlayerNum()]->valueType ];
         if ( minAvgFieldThreat > 0 ) {
            if ( threat && threat != 1 )
               value *= ( log ( minAvgFieldThreat ) / log ( threat ) );
         } else {
            if ( secminAvgFieldThreat < maxfloat)
               if ( threat && threat != 1 )
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



class SaveUnitMovement {
       int m;
       Vehicle* unit;
    public:
       SaveUnitMovement ( Vehicle* veh ) {
          unit = veh;
          npush ( unit->xpos );
          npush ( unit->ypos );
          npush ( unit->height );
          m = unit->getMovement();
          npush ( unit->attacked );
       };
       ~SaveUnitMovement ( ) {
          npop ( unit->attacked );
          unit->setMovement ( m, 0 );
          npop ( unit->height );
          npop ( unit->ypos );
          npop ( unit->xpos );
       };
};



void AI::RefuelConstraint::findPath()
{
   if ( !ast ) {

      int dist;
      if ( maxMove == -1 ) {
         if ( veh->typ->fuelConsumption )
            dist = veh->getResource(maxint, 2, true)  / veh->typ->fuelConsumption * maxmalq;
         else
            dist = maxint;

         dist = min( dist, veh->maxMovement() * 5);
      } else
         dist = maxMove;

      ast = new AStar3D ( ai.getMap(), veh, true, dist );
      ast->findAllAccessibleFields ( );
      // tanker planes may have a very large range; that's why we top the distance at 5 times the turn-range
   }
}

MapCoordinate3D AI::RefuelConstraint::getNearestRefuellingPosition ( bool buildingRequired, bool refuel, bool repair )
{
   findPath();

   int fuel = veh->getResource(maxint, 2, true);
   int x1,y1,x2,y2;
   x1 = max(veh->xpos - fuel / veh->typ->fuelConsumption, 0 );
   y1 = max(veh->ypos - fuel / veh->typ->fuelConsumption, 0 );
   x2 = min(veh->xpos + fuel / veh->typ->fuelConsumption, actmap->xsize );
   y2 = min(veh->ypos + fuel / veh->typ->fuelConsumption, actmap->ysize );

   for ( AStar3D::Container::iterator i = ast->visited.begin(); i != ast->visited.end(); i++ ) {
      int dist = int(i->gval );
      if ( i->h.getNumericalHeight() == -1 ) {
          tfield* fld = getfield( i->h.x, i->h.y );
          if ( fld->building && fld->building->color == veh->color )
             reachableBuildings[ dist ] = fld->building;
      }
          // aircraft carriers should be supported too....
          // external loading of buildings too....
          // turrets too .....


       // let's check for landing
       if ((veh->height > chfahrend) && (i->h.getNumericalHeight() == chfahrend) ) {
          // we don't want to land in hostile territory
          FieldInformation& fi = ai.getFieldInformation ( i->h.x, i->h.y );
          if ( fi.control == -1 || !actmap->player[fi.control].diplomacy.isHostile( ai.getPlayerNum() ) )
              landingPositions[dist] = i->h;
       }
   }


   /*
   for ( int x = x1; x < x2; x++ )
      for ( int y = y1; y < y2; y++ )
         for ( int h = 0; h < 8; h++ )
            if ( veh->typ->height & ( 1 << h)) {

                tfield* fld = getfield( x,y );
                const AStar3D::Node* node = ast->fieldVisited( MapCoordinate3D( x,y, 1 << h));
                if ( node ) {
                   int dist = int(node->gval);
                   if ( fld->building && fld->building->color == veh->color )
                      reachableBuildings[ dist ] = fld->building;

                   // aircraft carriers should be supported too....
                   // external loading of buildings too....
                   // turrets too .....


                   // let's check for landing
                   if ((veh->height > chfahrend) && (fld->a.temp & chfahrend) && ( 1 << h) == chfahrend ) {
                      // we don't want to land in hostile territory
                      FieldInformation& fi = ai.getFieldInformation ( x, y );
                      if ( fi.control == -1 || getdiplomaticstatus2 ( fi.control * 8, ai.getPlayerNum()*8 ) == capeace )
                          landingPositions[dist] = MapCoordinate3D( x,y,chfahrend);
                   }
                }
            }
            */

   if ( buildingRequired ) {
      for ( ReachableBuildings::iterator rb = reachableBuildings.begin(); rb != reachableBuildings.end(); rb++  ) {
         if ( !repair || (rb->second->typ->hasFunction( ContainerBaseType::InternalUnitRepair )))
            return rb->second->getPosition3D();
      }
   }
   if ( !reachableBuildings.empty() && !landingPositions.empty()) {
      if ( reachableBuildings.begin()->first < landingPositions.begin()->first )
         return reachableBuildings.begin()->second->getPosition3D();
      else
         return landingPositions.begin()->second;
   }

   if ( !landingPositions.empty() )
      return landingPositions.begin()->second;
   else
      return MapCoordinate3D();
}

bool AI::RefuelConstraint::returnFromPositionPossible ( const MapCoordinate3D& pos, int theoreticalFuel )
{
   if ( !veh->typ->fuelConsumption )
      return true;

   if ( theoreticalFuel < 0 )
      theoreticalFuel = veh->getResource(maxint, 2, true );

   findPath();
   if ( !positionsCalculated ) {
      getNearestRefuellingPosition ( true, true, false );
      positionsCalculated = true;
   }

   if ( !ast->fieldVisited(pos) )
      return false;

   int dist  = int( ast->fieldVisited(pos)->gval );
   int dist2;
   if ( !reachableBuildings.empty() ) {
      ReachableBuildings::iterator rb = reachableBuildings.begin();
      dist2 = beeline ( pos, rb->second->getPosition() );
      rb++;
      while ( rb != reachableBuildings.end() ) {
         int d = beeline ( pos, rb->second->getPosition() );
         if ( d < dist2 ) {
            dist2 = d;
         }
         rb++;
      }
   } else {
      if ( !landingPositions.empty() ) {
         LandingPositions::iterator lp = landingPositions.begin();
         dist2 = beeline ( pos, lp->second );
         MapCoordinate3D dest = lp->second;
         lp++;
         while ( lp != landingPositions.end() ) {
            int d = beeline ( pos, lp->second );
            if ( d < dist2 ) {
               dist2 = d;
               dest = lp->second;
            }
            lp++;
         }
      } else /*
         if ( veh->height > chfahrend )
            dist2 = maxint;
         else  */
            return true;
   }

   if ( theoreticalFuel - (dist + dist2) / maxmalq * veh->typ->fuelConsumption > 0.2 * veh->getStorageCapacity().fuel )
      return true;
   else
      return false;

}

bool AI::RefuelConstraint::necessary (const Vehicle* veh, AI& ai )
{
   if ( !veh->typ->fuelConsumption )
      return false;
   if ( !(veh->height & (chtieffliegend | chfliegend | chhochfliegend))) {
      ServiceOrder so  ( &ai, VehicleService::srv_resource, veh->networkid, 2 );
      if ( so.serviceUnitExists() )
         return false;
      else
         return true;
   }

   return true;
}


AI::VehicleTypeEfficiencyCalculator::VehicleTypeEfficiencyCalculator( AI& _ai, Vehicle* _attacker, Vehicle* _target )
                                : ai ( _ai ), attacker( _attacker ), target( _target)
{
   ownValue = attacker->aiparam[ai.getPlayerNum()]->getValue();
   ownTypeID = attacker->typ->id;
   enemyValue = target->aiparam[ai.getPlayerNum()]->getValue();
   orgOwnDamage = attacker->damage;
   orgEnemyDamage = target->damage;
   enemyID = target->networkid;
   ownID = attacker->networkid;
}


void AI::VehicleTypeEfficiencyCalculator::calc()
{
   int newOwnDamage;
   if ( ai.getMap()->getUnit(ownID) )
      newOwnDamage = attacker->damage;
   else
      newOwnDamage = 100;

   int newEnemyDamage;
   if ( ai.getMap()->getUnit(enemyID) )
      newEnemyDamage = target->damage;
   else
      newEnemyDamage = 100;

   ai.unitTypeSuccess[ownTypeID].first  += enemyValue * (newEnemyDamage - orgEnemyDamage) * 0.01;
   ai.unitTypeSuccess[ownTypeID].second += ownValue   * (newOwnDamage   - orgOwnDamage  ) * 0.01;
}

AI::AiResult  AI :: container ( ContainerBase* cb )
{
   ContainerControls cc( cb );
   AiResult result;

   // move idle units out
   std::vector<Vehicle*> idleUnits;
   for ( vector<Vehicle*>::const_iterator j = cb->getCargo().begin(); j != cb->getCargo().end(); ++j ) {
      Vehicle* veh = *j;
      if ( veh )
         if ( veh->canMove() )
            if ( veh->aiparam[ getPlayerNum() ]->getTask() == AiParameter::tsk_nothing
               || veh->aiparam[ getPlayerNum() ]->getJob() == AiParameter::job_script  )
            idleUnits.push_back ( veh );
   }
   
   // move the most important unit first, to get the best position
   sort ( idleUnits.begin(), idleUnits.end(), vehicleValueComp );

   for ( std::vector<Vehicle*>::iterator i = idleUnits.begin(); i != idleUnits.end(); i++ ) {

      checkKeys();

      // Vehicle* veh = *i;

      int preferredHeight = getBestHeight ( *i );

      VehicleMovement* vm = ContainerControls::movement ( *i );
      // auto_ptr<VehicleMovement> vm ( cc.movement ( *i ) );
      if ( vm ) {
         vm->registerMapDisplay ( mapDisplay );
         auto_ptr<VehicleMovement> avm ( vm );

         VehicleAttack va ( NULL, NULL );
         int moved = 0;
         if ( va.available ( *i )) {
            TargetVector tv;

            // AStar3D ast ( getMap(), *i, false, (*i)->maxMovement() );  ?? Why was maxMovement used here ??
            AStar3D ast ( getMap(), *i, false, (*i)->getMovement() );
            ast.findAllAccessibleFields ();

            getAttacks ( ast, *i, tv, 0 );

            if ( tv.size() ) {
               AiResult res = executeMoveAttack ( *i, tv );
               result += res;
               if ( !res.unitsDestroyed )
                  (*i)->aiparam[ getPlayerNum() ]->setTask( AiParameter::tsk_tactics );

               moved = 1;
            } else {
               AStar3D ast ( getMap(), *i, false, (*i)->maxMovement()*3 );
               ast.findAllAccessibleFields ();

               getAttacks ( ast, *i, tv, 0 );

               if ( tv.size() ) {
                  MoveVariant* mv = *max_element( tv.begin(), tv.end(), moveVariantComp );
                  if ( moveUnit ( *i, mv->movePos ) )
                     moved = 1;
               }
            }

         }
         if ( !moved ) {
            AiResult res =  moveToSavePlace ( *i, *vm, preferredHeight );
            result += res;
            if ( !res.unitsDestroyed )
               (*i)->aiparam[ getPlayerNum() ]->resetTask();
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
   for ( Player::BuildingList::iterator bi = getPlayer().buildingList.begin(); bi != getPlayer().buildingList.end(); bi++ ) {
      buildingCounter++;
      displaymessage2("processing building %d", buildingCounter );

      ContainerControls bc( *bi );

      int unitCounter = 0;
      for ( vector<Vehicle*>::const_iterator j=  (*bi)->getCargo().begin(); j != (*bi)->getCargo().end(); j++ ) {
         Vehicle* veh = *j;
         if ( veh ) {
            ++unitCounter;
            displaymessage2("processing building %d (unit %d)", buildingCounter, unitCounter );
            if ( veh->aiparam[ getPlayerNum() ]->getJob() != AiParameter::job_supply )
               bc.fillResource( veh, Resources::Fuel, maxint );
            else
               bc.refilleverything( veh );
         }
      }

      result += container ( *bi );

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
   for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); vi++ ) {
      Vehicle* veh = *vi;
      transportCounter++;
      displaymessage2("processing unit %d for transportation ", transportCounter );

      result += container ( veh );

      checkKeys();
   }

   displaymessage2("transport check completed... ");
   return result;
}



bool AI :: moveUnit ( Vehicle* veh, const MapCoordinate3D& destination, bool intoBuildings, bool intoTransports )
{
   // are we operating in 3D space or 2D space? Pathfinding in 3D has not
   // been available at the beginning of the AI work; and it is faster anyway
   #if 0
   if ( destination.getNumericalHeight() == -1 ) {

      VehicleMovement vm ( mapDisplay, NULL );
      vm.execute ( veh, -1, -1, 0, -1, -1 );

      std::vector<MapCoordinate> path;
      AStar* ast = NULL;
      if ( veh->aiparam[getPlayerNum()]->getJob() == AiParameter::job_conquer )
         ast = new HiddenAStar ( this, veh );
      else
         ast = new StratAStar ( this, veh );

      auto_ptr<AStar> ap ( ast );

      ast->findPath ( AStar::HexCoord ( veh->xpos, veh->ypos ), AStar::HexCoord ( destination.x, destination.y ), path );

      int xtogo = veh->xpos;
      int ytogo = veh->ypos;

      for ( unsigned int i = 0; i < path.size(); i++ ) {
         int x = path[i].x;
         int y = path[i].y;

         tfield* fld = getfield ( x, y );
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

         int nwid  = veh->networkid;
         vm.execute ( NULL, xtogo, ytogo, 2, -1, -1 );
         if ( vm.getStatus() != 3 )
            displaymessage ( "AI :: moveUnit \n error in movement step 2 with unit %d", 1, veh->networkid );

         vm.execute ( NULL, xtogo, ytogo, 3, -1,  1 );
         if ( vm.getStatus() != 1000 )
            displaymessage ( "AI :: moveUnit \n error in movement step 3 with unit %d", 1, veh->networkid );

         if ( getMap()->getUnit(nwid) )
            if ( destination.x == xtogo && destination.y == ytogo )
               if ( veh->aiparam[getPlayerNum()]->getJob() == AiParameter::job_conquer )
                  if ( getfield ( xtogo, ytogo)->building )
                     veh->aiparam[getPlayerNum()]->clearJobs();


         return true;
      }
      return false;
   } else {
   #endif
      if ( veh->getPosition3D() == destination )
         return true;
         
      AStar3D* ast = NULL;
      if ( veh->aiparam[getPlayerNum()]->getJob() == AiParameter::job_conquer )
         ast = new HiddenAStar3D ( this, veh );
      else
         if ( beeline ( veh->getPosition(), destination) > veh->maxMovement() )
            ast = new StratAStar3D ( this, veh );
         else
            ast = new AStar3D ( getMap(), veh );

      auto_ptr<AStar3D> ap ( ast );

      AStar3D::Path path;
      ast->findPath ( path, destination );

      return moveUnit ( veh, path, intoBuildings, intoTransports ) == 1 ;
//   }
}

int AI::moveUnit ( Vehicle* veh, const AStar3D::Path& path, bool intoBuildings, bool intoTransports )
{
   AStar3D::Path::const_iterator pi = path.begin();
   if ( pi == path.end() )
      return 0;

   int nwid = veh->networkid;

   AStar3D::Path::const_iterator lastmatch = pi;
   while ( pi != path.end() ) {
      tfield* fld = getfield ( pi->x, pi->y );
      bool ok = true;
      if ( fld->getContainer() ) {
         if ( pi+1 !=path.end() )
             ok = false;
         else {
            if ( fld->building && !intoBuildings )
               ok = false;
            if ( fld->vehicle && !intoTransports )
               ok = false;
         }
      }

      if ( ok )
      // if ( !fld->building || getdiplomaticstatus2 ( fld->building->color, getPlayerNum()*8) == cawar || fld->building->getOwner() == veh->getOwner() )
         if ( pi->dist <= veh->getMovement() )
            lastmatch = pi;

      ++pi;
   }

   if ( getfield ( lastmatch->x, lastmatch->y )->building )
      if ( checkReConquer ( getfield ( lastmatch->x, lastmatch->y )->building, veh ))
         return false;

   if ( lastmatch == path.begin() )
      return 0;

   BaseVehicleMovement vm ( vat_move, NULL, mapDisplay );
   vm.execute ( veh, lastmatch->x, lastmatch->y, 0, lastmatch->getNumericalHeight(), 1 );

   if ( vm.getStatus() != 3 ) {
      displaymessage ( "AI :: moveUnit (path) \n error in movement step 3 with unit %d", 1, veh->networkid );
      return -1;
   }

   vm.execute ( NULL, lastmatch->x, lastmatch->y, 3, -1,  1 );
   if ( vm.getStatus() != 1000 ) {
      displaymessage ( "AI :: moveUnit (path) \n error in movement step 3 with unit %d", 1, veh->networkid );
      return -1;
   }


   //! the unit may have been shot down
   if ( ! getMap()->getUnit ( nwid ))
      return 1;

   if ( pi == path.end() ) 
      if ( veh->aiparam[getPlayerNum()]->getJob() == AiParameter::job_conquer )
         if ( getMap()->getField ( veh->getPosition() )->building )
            veh->aiparam[getPlayerNum()]->clearJobs();

   return 1;
}





AI:: CheckFieldRecon :: CheckFieldRecon ( AI* _ai ) : SearchFields ( _ai->getMap() ), player(_ai->getPlayerNum()), ai ( _ai )
{
   for( int i = 0; i < 3; i++ ) {
      ownFields[i] = 0;
      enemyFields[i] = 0;
   }
}

int AI:: CheckFieldRecon :: run ( int x, int y)
{
   initsearch ( MapCoordinate(x, y), 1 , 2 );
   startsearch();
   if ( ownFields[1] && !enemyFields[1] ) {
      if ( enemyFields[2] > ownFields[2] )
         return 0;
      else
         if ( enemyFields[2] )
            return 1;
   }
   return -1;
}

void AI::CheckFieldRecon :: testfield ( const MapCoordinate& mc )
{
   FieldInformation& fi = ai->getFieldInformation ( mc.x, mc.y );
   if( fi.control != -1 )
      if ( !actmap->player[player].diplomacy.isHostile( fi.control )  )
         ownFields[dist]++;
      else
         enemyFields[dist]++;
}

void AI :: calcReconPositions()
{
   displaymessage2("calculating reconnaissance positions ... ");
   for ( int y = 0; y < getMap()->ysize; y++ )
      for ( int x = 0; x < getMap()->xsize; x++ ) {
         FieldInformation& fi = getFieldInformation ( x, y );
         tfield* fld = getMap()->getField(x,y);
         if ( fi.control == getPlayerNum() && !fld->building && ( !fld->vehicle || fld->vehicle->aiparam[getPlayerNum()]->getJob() == AiParameter::job_recon )) {
            CheckFieldRecon cfr ( this );
            int qual = cfr.run(x,y);
            if ( qual>= 0 )
               reconPositions[MapCoordinate ( x, y )] = qual;

         }
      }
}

void AI ::  runReconUnits ( )
{
   Player::VehicleList::iterator nvi;
   for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); ) {
      nvi = vi;
      ++nvi;

      Vehicle* veh = *vi;

      int maxUnitMovement = veh->typ->maxSpeed();

      // the threat posed should be enemy units should be considered for position choosing too...
      if ( veh->aiparam[getPlayerNum()]->getJob() == AiParameter::job_recon ) {
         if ( reconPositions.find ( veh->getPosition()) == reconPositions.end()) {
            // the unit is not standing on a reconposition
            int mindist = maxint;
            MapCoordinate mc;
            for ( ReconPositions::iterator i = reconPositions.begin(); i != reconPositions.end(); i++ ) {
               tfield* fld = getMap()->getField( i->first );
               if ( !fld->vehicle && !fld->building ) {
                  AStar ast ( getMap(), veh );
                  ast.findAllAccessibleFields( maxUnitMovement );
                  if ( fld->a.temp ) {
                     int vdist = beeline ( veh->getPosition(), i->first )*(1+i->second/2);
                     if( vdist < mindist ) {
                        mindist = vdist;
                        mc = i->first;
                     }
                  }
               }
            }
            if( mindist < maxint ) {
               veh->aiparam[getPlayerNum()]->dest = MapCoordinate3D(mc, veh->height);
               veh->aiparam[getPlayerNum()]->setTask( AiParameter::tsk_move );
               runUnitTask ( veh );
            }
         }
      }
      vi = nvi;
   }
}

AI::UnitDistribution::Group AI::getUnitDistributionGroup ( Vehicletype* vt )
{
   switch ( chooseJob ( vt ).front() ) {
      case AiParameter::job_supply : return UnitDistribution::service;
      case AiParameter::job_recon  : return UnitDistribution::recon;
      case AiParameter::job_conquer: return UnitDistribution::conquer;
      case AiParameter::job_fight:
      case AiParameter::job_guard: {
                                      bool range = false;
                                      for ( int w = 0; w < vt->weapons.count; w++ )
                                         if ( vt->weapons.weapon[w].offensive() )
                                            if ( vt->weapons.weapon[w].maxdistance >= 2 * minmalq )
                                               range = true;
                                      if ( range )
                                         return UnitDistribution::rangeattack;
                                      else
                                         return UnitDistribution::attack;
                                    }
      default:;
    } //switch job
    return UnitDistribution::other;
}


AI::UnitDistribution::Group AI::getUnitDistributionGroup ( Vehicle* veh )
{
   switch ( veh->aiparam[getPlayerNum()]->getJob() ) {
      case AiParameter::job_supply : return UnitDistribution::service;
      case AiParameter::job_recon  : return UnitDistribution::recon;
      case AiParameter::job_conquer: return UnitDistribution::conquer;
      case AiParameter::job_fight:
      case AiParameter::job_guard: {
                                      bool range = false;
                                      for ( int w = 0; w < veh->typ->weapons.count; w++ )
                                         if ( veh->typ->weapons.weapon[w].offensive() )
                                            if ( veh->typ->weapons.weapon[w].maxdistance >= 2 * minmalq )
                                               range = true;
                                      if ( range )
                                         return UnitDistribution::rangeattack;
                                      else
                                         return UnitDistribution::attack;
                                    }
      default:;
    } //switch job
    return UnitDistribution::other;
}

void AI::UnitDistribution::read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version == 15000 ) {
      int gc = stream.readInt();
      calculated = stream.readInt();
      for ( int i = 0; i<  gc; i++ )
         group[i] = stream.readFloat();

      for ( int i = gc; i < groupCount; i++ )
         group[i] = 0;
   }
}

void AI::UnitDistribution::write ( tnstream& stream ) const
{
   stream.writeInt ( 15000 );
   stream.writeInt ( groupCount );
   stream.writeInt ( calculated );
   for ( int i = 0; i < groupCount; i++ )
      stream.writeFloat ( group[i] );
}


AI::UnitDistribution AI::calcUnitDistribution ()
{
   UnitDistribution unitDistribution;
   int unitCount = getPlayer().vehicleList.size();
   if ( unitCount ) {
      float inc = float(1) / float(unitCount);
      for ( Player::VehicleList::iterator i = getPlayer().vehicleList.begin(); i != getPlayer().vehicleList.end(); i++ )
         unitDistribution.group[ getUnitDistributionGroup ( *i )] += inc;

   }
   unitDistribution.calculated = true;
   return unitDistribution;
}


void AI::production()
{
   float inc;
   int unitCount = getPlayer().vehicleList.size();
   if ( unitCount ) 
      inc = float(1) / float(unitCount);
   else
      inc = 1;

   UnitDistribution currentUnitDistribution = calcUnitDistribution();

   // we can't have enough attacking units
   currentUnitDistribution.group[ UnitDistribution::attack ] = 0;
   
   displaymessage2("producing units ... ");

   AiThreat enemyThreat;
   float    enemyValue[aiValueTypeNum];
   for ( int i = 0; i < aiValueTypeNum; i++ )
      enemyValue[i] = 0;

   for ( int p = 0; p < 8; p++ )
      if ( actmap->player[p].diplomacy.isHostile( getPlayerNum() ) )
          for ( Player::VehicleList::iterator vli = getMap()->player[p].vehicleList.begin(); vli != getMap()->player[p].vehicleList.end(); vli++ ) {
              enemyThreat += (*vli)->aiparam[getPlayerNum()]->threat;
              enemyValue[(*vli)->aiparam[getPlayerNum()]->valueType] += (*vli)->aiparam[getPlayerNum()]->getValue();
          }


   int emptyCount = 0;
   for ( int i = 0; i < aiValueTypeNum; i++ )
      if ( enemyValue[i] == 0)
         emptyCount++;

   // there are no enemies; the ai doesn't know what to produce
   if ( emptyCount == aiValueTypeNum )
      return ;



   typedef multimap<float,ProductionRating> Produceable;
   Produceable produceable;

   for ( Player::BuildingList::iterator bli = getPlayer().buildingList.begin(); bli != getPlayer().buildingList.end(); bli ++ ) {
      Building* bld = *bli;
      for ( int i = 0; i < bld->getProduction().size(); i++ )
         if ( bld->getProduction()[i] && bld->vehicleUnloadable ( bld->getProduction()[i] )) {
            const Vehicletype* typ = bld->getProduction()[i];
            float rating = 0;
            for ( int j = 0; j < aiValueTypeNum; j++ )
               rating += enemyValue[j] * typ->aiparam[getPlayerNum()]->threat.threat[j];

            int danger = 1;
            if ( typ->aiparam[getPlayerNum()]->getValue() ) {
               if ( typ->aiparam[getPlayerNum()]->getValue() < enemyThreat.threat[ typ->aiparam[getPlayerNum()]->valueType] )
                  danger = enemyThreat.threat[ typ->aiparam[getPlayerNum()]->valueType] / typ->aiparam[getPlayerNum()]->getValue();
            }

            rating /= 1 + log ( double(danger) );

            UnitTypeSuccess::iterator uts = unitTypeSuccess.find ( bld->getProduction()[i]->id  );
            if ( uts != unitTypeSuccess.end() ) {
               if ( uts->second.second >= 1 )
                  rating *= uts->second.first / uts->second.second;
            }


            int cost = 0;
            for ( int j = 0; j < resourceTypeNum; j++ )
               cost += typ->productionCost.resource(j);

            if ( cost )
               rating /= cost;

            ProductionRating pr = { typ, bld, rating };
            produceable.insert ( make_pair(rating, pr));
         }
   }

   for ( Produceable::iterator i = produceable.begin(); i != produceable.end(); ++i ) {
      float f = i->first;
      const char* d = i->second.vt->description.c_str();
      printf(" typ: %s  ; rating: %e \n", d, f );
   }

   if ( !produceable.empty() ) {

      GetConnectedBuildings::BuildingContainer lockedBuildings;

      bool produced;
      do {
          produced = false;
//           for ( int i = 0; i < UnitDistribution::groupCount; i++ ) {
//              if ( currentUnitDistribution.group[i] < originalUnitDistribution.group[i] ) {
                for ( Produceable::reverse_iterator p = produceable.rbegin(); p != produceable.rend(); p++ ) {
//                    if ( getUnitDistributionGroup ( p->second.vt) == i ) {
                      ProductionRating& pr = p->second;

                      if ( find ( lockedBuildings.begin(), lockedBuildings.end(), pr.bld ) == lockedBuildings.end()) {
                         ContainerControls bc( pr.bld );
                         int lack = bc.unitProductionPrerequisites( pr.vt );
                         if  ( !lack && pr.bld->vehicleUnloadSystem ( pr.vt, 255 ) ) {
                             Vehicle* veh = bc.produceUnit( pr.vt, true, true );
                             if ( veh ) {
                                 calculateThreat ( veh );
                                 container ( pr.bld );
                                 // currentUnitDistribution.group[i] += inc;
                                 produced = true;
                                 break;  // exit produceable loop
                             }
                         } else {

                            // the ai will save for move expensive units
                            if ( !(lack & ( 1<<10 ))) {
                               if ( pr.bld->getResource ( pr.vt->productionCost, 1 ) + pr.bld->netResourcePlus( ) * config.waitForResourcePlus >= pr.vt->productionCost )
                                  for ( int r = 0; r < resourceTypeNum; r++ )
                                     if ( lack & (1 << r )) {
                                        GetConnectedBuildings gcb ( lockedBuildings, getMap(), r );
                                        gcb.start ( pr.bld->getEntry().x, pr.bld->getEntry().y );
                                     }
                               sort  ( lockedBuildings.begin(), lockedBuildings.end());
                               unique( lockedBuildings.begin(), lockedBuildings.end());
                            }
                         }
                      }
                   // } // else
                     //  printf(" %s \n", p->second.vt->description );
                }
             // }
          // }
      } while ( produced );
   }

}


