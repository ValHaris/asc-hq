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



bool AI :: runUnitTask ( pvehicle veh )
{
   if ( veh->aiparam[getPlayerNum()]->task == AiParameter::tsk_move || veh->aiparam[getPlayerNum()]->task == AiParameter::tsk_serviceRetreat ) {
      bool moveIntoBuildings = false;
      if ( veh->aiparam[getPlayerNum()]->job == AiParameter::job_conquer || veh->aiparam[getPlayerNum()]->task == AiParameter::tsk_serviceRetreat )
         moveIntoBuildings = true;

      moveUnit ( veh, veh->aiparam[getPlayerNum()]->dest, moveIntoBuildings );
      if ( veh->getPosition() == veh->aiparam[getPlayerNum()]->dest ) {
         veh->aiparam[getPlayerNum()]->task = AiParameter::tsk_nothing;
         return true;
      } else
         return false;
   }

   return false;
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
            value *=  log( veh->typ->movement[j] );

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
       pvehicle unit;
    public:
       SaveUnitMovement ( pvehicle veh ) {
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



void AI::AirplaneLanding::findPath()
{
   if ( !ast ) {
      ast = new AStar3D ( ai.getMap(), veh );

      int dist;
      if ( maxMove == -1 ) {
         if ( veh->typ->fuelConsumption )
            dist = veh->tank.fuel / veh->typ->fuelConsumption * maxmalq;
         else
            dist = maxint;

         dist = min( dist, veh->maxMovement() * 5);
      } else
         dist = maxMove;

      ast->findAllAccessibleFields ( dist );
      // tanker planes may have a very large range; that's why we top the distance at 5 times the turn-range
   }
}

MapCoordinate3D AI::AirplaneLanding::getNearestLandingPosition ( bool buildingRequired, bool refuel, bool repair )
{
   findPath();

   for ( int x = 0; x < actmap->xsize; x++ )
      for ( int y = 0; y < actmap->ysize; y++ ) {
          pfield fld = getfield( x,y );
          if ( fld->a.temp & chfahrend ) {
             int dist = ast->fieldVisited( MapCoordinate3D( x,y,chfahrend))->gval;
             if ( fld->building && fld->building->color == veh->color )
                reachableBuildings[ dist ] = fld->building;
             // aircraft carriers should be supported too....

             // we don't want to land in hostile territory
             FieldInformation& fi = ai.getFieldInformation ( x, y );
             if ( fi.control == -1 || getdiplomaticstatus2 ( fi.control * 8, ai.getPlayerNum()*8 ) == capeace )
                 landingPositions[dist] = MapCoordinate3D( x,y,chfahrend);

          }
      }

   if ( buildingRequired ) {
      for ( ReachableBuildings::iterator rb = reachableBuildings.begin(); rb != reachableBuildings.end(); rb++  ) {
         if ( !repair || (rb->second->typ->special & cgrepairfacilityb))
            return rb->second->getPosition();
      }
   }
   if ( !reachableBuildings.empty() && !landingPositions.empty()) {
      if ( reachableBuildings.begin()->first < landingPositions.begin()->first )
         return reachableBuildings.begin()->second->getPosition();
      else
         return landingPositions.begin()->second;
   }

   if ( !landingPositions.empty() )
      return landingPositions.begin()->second;
   else
      return MapCoordinate3D();
}

bool AI::AirplaneLanding::returnFromPositionPossible ( const MapCoordinate3D& pos, int theoreticalFuel )
{
   if ( !veh->typ->fuelConsumption )
      return true;

   if ( theoreticalFuel < 0 )
      theoreticalFuel = veh->tank.fuel;

   findPath();
   if ( !positionsCalculated )
      getNearestLandingPosition ( true, true, false );

   int dist  = ast->fieldVisited(pos)->gval;
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
      } else
         dist2 = maxint;
   }

   if ( theoreticalFuel - (dist + dist2) / maxmalq * veh->typ->fuelConsumption > 0.2 * veh->typ->tank.fuel )
      return true;
   else
      return false;

}

bool AI::AirplaneLanding::canUnitCrash (const pvehicle veh )
{
   if ( !veh->typ->fuelConsumption )
      return false;
   if ( !(veh->height & (chtieffliegend | chfliegend | chhochfliegend)))
      return false;

   return true;
}





AI::AiResult  AI :: container ( ccontainercontrols& cc )
{
   AiResult result;

   // move idle units out
   std::vector<pvehicle> idleUnits;
   for ( int j= 0; j < 32; j++ ) {
      pvehicle veh = cc.getloadedunit ( j );
      if ( veh )
         if ( veh->aiparam[ getPlayerNum() ]->task == AiParameter::tsk_nothing && cc.moveavail ( veh ))
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
               (*i)->aiparam[ getPlayerNum() ]->task = AiParameter::tsk_wait;
            } else {
               if ( stat== -2 ) {
                  simplyMove = 1;
               } else {
                  result.unitsMoved++;
                  (*i)->aiparam[ getPlayerNum() ]->task = AiParameter::tsk_nothing;
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
                     (*i)->aiparam[ getPlayerNum() ]->task = AiParameter::tsk_tactics;

                  attack = 1;
               }

            }
            if ( !attack ) {
               AiResult res =  moveToSavePlace ( *i, *vm );
               result += res;
               if ( !res.unitsDestroyed )
                  (*i)->aiparam[ getPlayerNum() ]->task = AiParameter::tsk_nothing;
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
   for ( Player::BuildingList::iterator bi = getPlayer().buildingList.begin(); bi != getPlayer().buildingList.end(); bi++ ) {
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
   for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); vi++ ) {
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



bool AI :: moveUnit ( pvehicle veh, const MapCoordinate3D& destination, bool intoBuildings, bool intoTransports )
{
   // are we operating in 3D space or 2D space? Pathfinding in 3D has not
   // been available at the beginning of the AI work; and it is faster anyway
   if ( destination.z == -1 ) {

      VehicleMovement vm ( mapDisplay, NULL );
      vm.execute ( veh, -1, -1, 0, -1, -1 );

      std::vector<MapCoordinate> path;
      AStar* ast = NULL;
      if ( veh->aiparam[getPlayerNum()]->job == AiParameter::job_conquer )
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
            displaymessage ( "AI :: moveUnit \n error in movement step 2 with unit %d", 1, veh->networkid );

         vm.execute ( NULL, xtogo, ytogo, 3, -1,  1 );
         if ( vm.getStatus() != 1000 )
            displaymessage ( "AI :: moveUnit \n error in movement step 3 with unit %d", 1, veh->networkid );

         return true;
      }
      return false;
   } else {
      AStar3D* ast = NULL;
      if ( veh->aiparam[getPlayerNum()]->job == AiParameter::job_conquer )
         ast = new HiddenAStar3D ( this, veh );
      else
         ast = new StratAStar3D ( this, veh );

      auto_ptr<AStar3D> ap ( ast );

      AStar3D::Path path;
      ast->findPath ( path, destination );

      return moveUnit ( veh, path ) == 1 ;
   }
}

int AI::moveUnit ( pvehicle veh, const AStar3D::Path& path )
{
   AStar3D::Path::const_iterator pi = path.begin();
   while ( pi != path.end() ) {
      if ( pi->z == veh->height ) {
         VehicleMovement vm ( mapDisplay, NULL );
         vm.execute ( veh, -1, -1, 0, -1, -1 );

         bool fieldFound = false;
         AStar3D::Path::const_iterator lastmatch = pi;
         AStar3D::Path::const_iterator i = pi;
         while ( i->z == veh->height && i != path.end() ) {
            if ( vm.reachableFields.isMember ( i->x, i->y )) {
               lastmatch = i;
               fieldFound = true;
            }
            i++;
         }

         if ( (lastmatch->x != veh->xpos || lastmatch->y != veh->ypos) && fieldFound ) {
            if ( getfield ( lastmatch->x, lastmatch->y )->building )
               if ( checkReConquer ( getfield ( lastmatch->x, lastmatch->y )->building, veh ))
                  return false;

            vm.execute ( NULL, lastmatch->x, lastmatch->y, 2, -1, -1 );
            if ( vm.getStatus() != 3 )
               displaymessage ( "AI :: moveUnit (path) \n error in movement step 2 with unit %d", 1, veh->networkid );

            vm.execute ( NULL, lastmatch->x, lastmatch->y, 3, -1,  1 );
            if ( vm.getStatus() != 1000 )
               displaymessage ( "AI :: moveUnit (path) \n error in movement step 3 with unit %d", 1, veh->networkid );

            pi = lastmatch+1;
         }
      }

      if ( pi == path.end() )
         return 1;

      if ( pi->z == veh->height ) {
         // movement exhausted
         return 0;
      }

      if ( ChangeVehicleHeight::getMoveCost ( veh, veh->getPosition(), getdirection( veh->xpos, veh->ypos, pi->x, pi->y), pi->z > veh->height ? 1 : -1 ).first > veh->getMovement() )
         return 0;

      ChangeVehicleHeight* cvh;
      if ( pi->z > veh->height )
         cvh = new IncreaseVehicleHeight ( mapDisplay );
      else
         cvh = new DecreaseVehicleHeight ( mapDisplay );

      auto_ptr<ChangeVehicleHeight> ap ( cvh );
      cvh->execute ( veh, -1, -1, 0, pi->z, 0 );
      if ( cvh->getStatus() != 1000 ) {
          if ( cvh->getStatus() < 0 ) {
             displaymessage ( "AI :: moveUnit (path) \n error in changeHeight step 0 with unit %d", 1, veh->networkid );
             return -1;
          }

          cvh->execute ( NULL, pi->x, pi->y, 2, -1, -1 );
          if ( cvh->getStatus() < 0 ) {
             displaymessage ( "AI :: moveUnit (path) \n error in changeHeight step 2 with unit %d", 1, veh->networkid );
             return -1;
          }

          cvh->execute ( NULL, pi->x, pi->y, 3, -1, 1 );
          if ( cvh->getStatus() < 0 ) {
             displaymessage ( "AI :: moveUnit (path) \n error in changeHeight step 3 with unit %d", 1, veh->networkid );
             return -1;
          }
      }

      pi++;
   }
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
      if ( getdiplomaticstatus2 ( player*8, fi.control*8 ) == capeace )
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
         pfield fld = getMap()->getField(x,y);
         if ( fi.control == getPlayerNum() && !fld->building && ( !fld->vehicle || fld->vehicle->aiparam[getPlayerNum()]->job == AiParameter::job_recon )) {
            CheckFieldRecon cfr ( this );
            int qual = cfr.run(x,y);
            if ( qual>= 0 )
               reconPositions[MapCoordinate ( x, y )] = qual;

         }
      }
}

void AI ::  runReconUnits ( )
{
   for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); vi++ ) {
      pvehicle veh = *vi;

      // the threat posed should be enemy units should be considered for position choosing too...
      if ( veh->aiparam[getPlayerNum()]->job == AiParameter::job_recon ) {
         if ( reconPositions.find ( veh->getPosition()) == reconPositions.end()) {
            // the unit is not standing on a reconposition
            int mindist = maxint;
            MapCoordinate mc;
            for ( ReconPositions::iterator i = reconPositions.begin(); i != reconPositions.end(); i++ ) {
               pfield fld = getMap()->getField( i->first );
               if ( !fld->vehicle && !fld->building ) {
                  AStar ast ( getMap(), veh );
                  ast.findAllAccessibleFields();
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
               veh->aiparam[getPlayerNum()]->dest = mc;
               veh->aiparam[getPlayerNum()]->task = AiParameter::tsk_move;
               runUnitTask ( veh );
            }
         }
      }
   }
}

AI::UnitDistribution::Group AI::getUnitDistributionGroup ( pvehicletype vt )
{
         switch ( chooseJob ( vt, vt->functions ) ) {
            case AiParameter::job_supply : return UnitDistribution::service;
            case AiParameter::job_recon  : return UnitDistribution::recon;
            case AiParameter::job_conquer: return UnitDistribution::conquer;
            case AiParameter::job_fight:
            case AiParameter::job_guard: {
                                            bool range = false;
                                            for ( int w = 0; w < vt->weapons->count; w++ )
                                               if ( vt->weapons->weapon[w].offensive() )
                                                  if ( vt->weapons->weapon[w].maxdistance >= 2 * minmalq )
                                                     range = true;
                                            if ( range )
                                               return UnitDistribution::rangeattack;
                                            else
                                               return UnitDistribution::attack;
                                          }
          } //switch job
          return UnitDistribution::other;
}


AI::UnitDistribution::Group AI::getUnitDistributionGroup ( pvehicle veh )
{
         switch ( veh->aiparam[getPlayerNum()]->job ) {
            case AiParameter::job_supply : return UnitDistribution::service;
            case AiParameter::job_recon  : return UnitDistribution::recon;
            case AiParameter::job_conquer: return UnitDistribution::conquer;
            case AiParameter::job_fight:
            case AiParameter::job_guard: {
                                            bool range = false;
                                            for ( int w = 0; w < veh->typ->weapons->count; w++ )
                                               if ( veh->typ->weapons->weapon[w].offensive() )
                                                  if ( veh->typ->weapons->weapon[w].maxdistance >= 2 * minmalq )
                                                     range = true;
                                            if ( range )
                                               return UnitDistribution::rangeattack;
                                            else
                                               return UnitDistribution::attack;
                                          }
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


// avoiding a gcc 2.95.3 bug....
typedef AI::UnitDistribution AIUD;
AIUD AI::calcUnitDistribution ()
//AI::UnitDistribution AI::calcUnitDistribution ()
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
      if ( getdiplomaticstatus2( p*8, getPlayerNum()*8 ) != capeace )
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
      pbuilding bld = *bli;
      for ( int i = 0; i < 32; i++ )
         if ( bld->production[i] ) {
            Vehicletype* typ = bld->production[i];
            float rating = 0;
            for ( int j = 0; j < aiValueTypeNum; j++ )
               rating += enemyValue[j] * typ->aiparam[getPlayerNum()]->threat.threat[j];

            int danger = 1;
            if ( typ->aiparam[getPlayerNum()]->getValue() ) {
               if ( typ->aiparam[getPlayerNum()]->getValue() < enemyThreat.threat[ typ->aiparam[getPlayerNum()]->valueType] )
                  danger = enemyThreat.threat[ typ->aiparam[getPlayerNum()]->valueType] / typ->aiparam[getPlayerNum()]->getValue();
            }

            rating /= 1 + log ( danger );

            int cost = 0;
            for ( int j = 0; j < resourceTypeNum; j++ )
               cost += typ->productionCost.resource(j);

            if ( cost )
               rating /= cost;

            ProductionRating pr = { typ, bld, rating };
            produceable.insert ( make_pair(rating, pr));
         }
   }

   if ( !produceable.empty() ) {

      GetConnectedBuildings::BuildingContainer lockedBuildings;

      bool produced;
      do {
          produced = false;
          for ( int i = 0; i < UnitDistribution::groupCount; i++ ) {
             if ( currentUnitDistribution.group[i] < originalUnitDistribution.group[i] ) {
                for ( Produceable::reverse_iterator p = produceable.rbegin(); p != produceable.rend(); p++ ) {
                   if ( getUnitDistributionGroup ( p->second.vt) == i ) {
                      ProductionRating& pr = p->second;

                      if ( find ( lockedBuildings.begin(), lockedBuildings.end(), pr.bld ) == lockedBuildings.end()) {
                         cbuildingcontrols bc;
                         bc.init ( pr.bld );
                         int lack;
                         if  ( bc.produceunit.available( pr.vt, &lack ) ) {
                             pvehicle veh = bc.produceunit.produce( pr.vt );
                             calculateThreat ( veh );
                             container ( bc );
                             currentUnitDistribution.group[i] += inc;
                             produced = true;
                             break;  // exit produceable llop
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
                   } // else
                     //  printf(" %s \n", p->second.vt->description );
                }
             }
          }
      } while ( produced );
   }

}


