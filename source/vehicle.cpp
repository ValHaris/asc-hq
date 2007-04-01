/***************************************************************************
                          vehicle.cpp  -  description
                             -------------------
    begin                : Fri Sep 29 2000
    copyright            : (C) 2000 by Martin Bickel
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

#include <algorithm>
#include <math.h>
#include "research.h"
#include "vehicletype.h"
#include "vehicle.h"
#include "buildingtype.h"
#include "viewcalculation.h"
#include "errors.h"
#include "graphicset.h"
#include "errors.h"
#include "gameoptions.h"
#include "spfst.h"
#include "itemrepository.h"
#include "graphics/blitter.h"

const float repairEfficiencyVehicle[resourceTypeNum*resourceTypeNum] = { 0,  0,  0,
                                                                         0,  0.5, 0,
                                                                         0.5, 0,  1 };


#ifndef UNITVERSIONLIMIT
# define UNITVERSIONLIMIT 0x7fffffff
#endif



Vehicle :: Vehicle (  )
          : ContainerBase ( NULL, NULL, 0 ), repairEfficiency ( repairEfficiencyVehicle ), typ ( NULL ), reactionfire ( this )
{
}

Vehicle :: Vehicle ( const Vehicle& v )
          : ContainerBase ( NULL, NULL, 0 ), repairEfficiency ( repairEfficiencyVehicle ), typ ( NULL ), reactionfire ( this )
{
}


Vehicle :: Vehicle ( const Vehicletype* t, GameMap* actmap, int player )
          : ContainerBase ( t, actmap, player ), repairEfficiency ( repairEfficiencyVehicle ), typ ( t ), reactionfire ( this )
{
   viewOnMap = false;

   if ( player > 8 )
      fatalError ( "Vehicle :: Vehicle ; invalid player ");

   init();

   gamemap->player[player].vehicleList.push_back ( this );
   gamemap->unitnetworkid++;
   networkid = gamemap->unitnetworkid;
   gamemap->vehicleLookupCache[networkid] = this;
}

Vehicle :: Vehicle ( const Vehicletype* t, GameMap* actmap, int player, int networkID )
          : ContainerBase ( t, actmap, player ), repairEfficiency ( repairEfficiencyVehicle ), typ ( t ), reactionfire ( this )
{
   viewOnMap = false;

   if ( player > 8 )
      fatalError ( "Vehicle :: Vehicle ; invalid player ");

   init();

   gamemap->player[player].vehicleList.push_back ( this );
   if ( networkID == -1 ) {
      gamemap->unitnetworkid++;
      networkid = gamemap->unitnetworkid;
   } else
      if ( networkID >= 0 ) {
         networkid = networkID;
      }

   if ( networkID >= 0 )
      gamemap->vehicleLookupCache[networkid] = this;
}


Vehicle :: ~Vehicle (  )
{
   #ifndef karteneditor
   if ( !typ->wreckageObject.empty() && gamemap && gamemap->state != GameMap::Destruction && !cleanRemove) {
      tfield* fld = getMap()->getField(getPosition());
      if ( fld->vehicle ==  this ) {
         for ( vector<int>::const_iterator i = typ->wreckageObject.begin(); i != typ->wreckageObject.end(); ++i ) {
            ObjectType* obj = getMap()->getobjecttype_byid( *i );
            if ( obj ) {
               getMap()->getField(getPosition()) -> addobject ( obj );
               Object* o = getMap()->getField(getPosition())->checkforobject(obj);
               if ( o )
                  o->setdir( direction );
            }
         }
      }
   }
   #endif

   for ( int i = 0; i < 8; i++ ) {
      delete aiparam[i];
      aiparam[i] = NULL;
   }
      

   
   if ( viewOnMap && gamemap && gamemap->state != GameMap::Destruction ) {
      removeview();
      viewOnMap = false;
   }

   if ( gamemap ) {
      int c = color/8;

      Player::VehicleList::iterator i = find ( gamemap->player[c].vehicleList.begin(), gamemap->player[c].vehicleList.end(), this );
      if ( i != gamemap->player[c].vehicleList.end() )
         gamemap->player[c].vehicleList.erase ( i );

      GameMap::VehicleLookupCache::iterator j = gamemap->vehicleLookupCache.find(networkid);
      if ( j != gamemap->vehicleLookupCache.end() )
         gamemap->vehicleLookupCache.erase(j);
   }

   tfield* fld = gamemap->getField( xpos, ypos);
   if ( fld ) {
      if ( fld->vehicle  == this )
        fld->vehicle = NULL;
      else {
         if ( fld->getContainer() )
            fld->getContainer()->removeUnitFromCargo( this, true );
      }
   }
}




void Vehicle :: init ( void )
{
   xpos = -1;
   ypos = -1;

   for ( int i = 0; i < 16; i++ ) {
      weapstrength[i] = 0;
      ammo[i] = 0;
   }

   damage = 0;

   experience = 0;
   attacked = false;

   if ( typ ) {
      height = 1 << log2( typ->height );

      // These are the preferred levels of height
      if (typ->height & chfahrend )
         height = chfahrend;
      else
         if (typ->height & chschwimmend )
            height = chschwimmend;

      for ( int m = 0; m < typ->weapons.count ; m++)
         weapstrength[m] = typ->weapons.weapon[m].maxstrength;

      setMovement ( typ->movement[log2(height)], 0 );
   } else {
      height = 0;

      setMovement ( 0, 0 );
   }

   direction = 0;
   xpos = -1;
   ypos = -1;
   connection = 0;
   networkid = -1;

   if ( typ && typ->hasFunction(ContainerBaseType::MoveWithReactionFire))
      reactionfire.status = ReactionFire::ready;
   else
      reactionfire.status = ReactionFire::off;

   generatoractive = 0;

   cleanRemove = false;

   for ( int a = 0; a < 8 ; a++ )
      aiparam[a] = NULL;
}


bool Vehicle :: canRepair( const ContainerBase* item ) const
{
   return typ->hasFunction( ContainerBaseType::InternalUnitRepair  ) ||
          typ->hasFunction( ContainerBaseType::ExternalRepair  ) ||
          (item == this && typ->autorepairrate ) ;
}

int Vehicle :: putResource ( int amount, int resourcetype, bool queryonly, int scope, int player )
{
   //  if units start using/storing resources that will not be stored in the unit itself, the replays will fail !

   if ( amount < 0 ) {
      return -getResource( -amount, resourcetype, queryonly, scope, player );
   } else {
      if ( resourcetype == 0 )  // no energy storable
         return 0;
      
      if ( player < 0 )
         player = getMap()->actplayer;
      
      if ( player != getOwner() && !(getMap()->getPlayer(getOwner()).diplomacy.getState(player) > PEACE) )
         return 0;

      int spaceAvail = getStorageCapacity().resource( resourcetype ) - tank.resource(resourcetype);
      if ( spaceAvail < 0 )
         spaceAvail = 0;

      int tostore = min ( spaceAvail, amount);
      if ( !queryonly ) {
         tank.resource(resourcetype) += tostore;
         // if ( tostore > 0 )
         //   resourceChanged();
      }

      return tostore;
   }
}

int Vehicle :: getResource ( int amount, int resourcetype, bool queryonly, int scope, int player )
{
   //  if units start using/storing resources that will not be stored in the unit itself, the replays will fail !

   if ( amount < 0 ) {
      return -putResource( -amount, resourcetype, queryonly, scope, player );
   } else {
      if ( resourcetype == 0 && !getGeneratorStatus() )
         return 0;

      if ( player < 0 )
         player = getMap()->actplayer;
      
      if ( player != getOwner() && !getMap()->getPlayer(getOwner()).diplomacy.isAllied(player) )
         return 0;
      
      
      int toget = min ( tank.resource(resourcetype), amount);
      if ( !queryonly ) {
         tank.resource(resourcetype) -= toget;
         // if ( toget > 0 )
         //   resourceChanged();
      }

      return toget;
   }
}

int Vehicle :: getResource ( int amount, int resourcetype ) const
{
   //  if units start using/storing resources that will not be stored in the unit itself, the replays will fail !

   if ( resourcetype == 0 && !getGeneratorStatus() )
      return 0;

   int toget = min ( tank.resource(resourcetype), amount);
   return toget;
}


Resources Vehicle::getResource ( const Resources& res ) const
{
   Resources got;
   for ( int r = 0; r < 3; ++r )
      got.resource(r) = getResource( res.resource(r), r);
        
   return got;
}


Resources Vehicle::getTank() const
{
   return tank;
}



void Vehicle :: setGeneratorStatus ( bool status )
{
   if ( typ->hasFunction( ContainerBaseType::MatterConverter )) {
      generatoractive = status;
   } else
     generatoractive = 0;
}



int Vehicle::weight( void ) const
{
   return typ->weight + cargoWeight();
}

int Vehicle::size ( void )
{
   return typ->weight;
}

void Vehicle::transform ( const Vehicletype* type )
{
   if ( !type )
      return;
   
   typ = type;

   tank = getStorageCapacity();
   tank.energy = 0;
   
   generatoractive = 0;

   for ( int m = 0; m < typ->weapons.count ; m++) {
      ammo[m] = typ->weapons.weapon[m].count;
      weapstrength[m] = typ->weapons.weapon[m].maxstrength;
   }
}

void Vehicle :: postRepair ( int oldDamage )
{
   for ( int i = 0; i < experienceDecreaseDamageBoundaryNum; i++)
      if ( oldDamage > experienceDecreaseDamageBoundaries[i] && damage < experienceDecreaseDamageBoundaries[i] )
         if ( experience > 0 )
            experience-=1;
}


/*
void Vehicle :: repairunit(Vehicle* vehicle, int maxrepair )
{
   if ( vehicle->damage  &&  tank.fuel  &&  tank.material ) {

      int orgdam = vehicle->damage;

      int dam;
      if ( vehicle->damage > maxrepair )
         dam = maxrepair;
      else
         dam = vehicle->damage;

      int fkost = dam * vehicle->typ->productionCost.energy / (100 * repairefficiency_unit );
      int mkost = dam * vehicle->typ->productionCost.material / (100 * repairefficiency_unit );
      int w;

      if (mkost <= material)
         w = 10000;
      else
         w = 10000 * material / mkost;

      if (fkost > fuel)
         if (10000 * fuel / fkost < w)
            w = 10000 * fuel / fkost;


      vehicle->damage -= dam * w / 10000;

      for ( int i = 0; i < experienceDecreaseDamageBoundaryNum; i++)
         if ( orgdam > experienceDecreaseDamageBoundaries[i] && vehicle->damage < experienceDecreaseDamageBoundaries[i] )
            if ( vehicle->experience > 0 )
               vehicle->experience-=1;


      if ( vehicle != this ) {
         if ( vehicle->getMovement() > movement_cost_for_repaired_unit )
            vehicle->setMovement ( vehicle->getMovement() -  movement_cost_for_repaired_unit );
         else
            vehicle->setMovement ( 0 );

         if ( !attack_after_repair )
            vehicle->attacked = 0;

         int unitloaded = 0;
         for ( int i = 0; i < 32; i++ )
            if ( loading[i] == vehicle )
               unitloaded = 1;

         if ( !unitloaded )
            if ( getMovement() > movement_cost_for_repairing_unit )
               setMovement ( getMovement() - movement_cost_for_repairing_unit );
            else
               setMovement ( 0 );
      }

      material -= w * mkost / 10000;
      fuel -= w * fkost / 10000;

   }
}
*/

void Vehicle :: endRound ( void )
{
   ContainerBase::endRound();
   if ( typ->hasFunction( ContainerBaseType::MatterConverter )) {
      int endiff = getStorageCapacity().energy - tank.energy;
      if ( tank.fuel < endiff * generatortruckefficiency )
         endiff = tank.fuel / generatortruckefficiency;

      tank.energy += endiff;
      tank.fuel -= endiff * generatortruckefficiency ;
   }
}

void Vehicle :: endAnyTurn()
{
   ContainerBase::endAnyTurn();
   reactionfire.endAnyTurn();
}


void Vehicle :: endOwnTurn()
{
   ContainerBase::endOwnTurn();
   
   if ( typ->autorepairrate > 0 )
      if ( damage )
         repairItem ( this, max ( damage - typ->autorepairrate, 0 ) );

   reactionfire.endOwnTurn();

   if ( !gamemap->getField(getPosition())->unitHere(this)) {
      int mx = -1;
      for ( int h = 0; h < 8; h++ )
         if ( typ->height & ( 1 << h))
            if ( typ->movement[h] > mx ) {
               mx = typ->movement[h];
               height = 1 << h;
            }
   }

   for ( int w = 0; w < typ->weapons.count; w++ )
      if ( typ->weapons.weapon[w].gettype() & cwlaserb ) {
         int cnt = 0;
         while ( cnt < typ->weapons.weapon[w].laserRechargeRate  && ammo[w] < typ->weapons.weapon[w].count ) {
            for ( int r = 0; r < 3; r++ )
               if ( typ->weapons.weapon[w].laserRechargeCost.resource(r) < 0 )
                  fatalError (" negative Laser recharge cost !");
            if ( ! (ContainerBase::getResource( typ->weapons.weapon[w].laserRechargeCost, 1 ) < typ->weapons.weapon[w].laserRechargeCost )) {
               ContainerBase::getResource( typ->weapons.weapon[w].laserRechargeCost, 0 );
               ammo[w] += 1;
            }
            ++cnt;
         }
      }

   resetMovement();
   attacked = false;

}

void Vehicle :: resetMovement ( void )
{
    int move = typ->movement[log2(height)];
    setMovement ( move, 0 );
    /*
    if (actvehicle->typ->fuelconsumption == 0)
       actvehicle->movement = 0;
    else {
       if ((actvehicle->fuel << 3) / actvehicle->typ->fuelconsumption < move)
          actvehicle->movement = (actvehicle->fuel << 3) / actvehicle->typ->fuelconsumption ;
       else
          actvehicle->movement = move;
    }
    */
}


void Vehicle :: setNewHeight( int bitmappedheight )
{
  if ( maxMovement() ) {
     float oldperc = float(getMovement ( false )) / float(maxMovement());
     height = bitmappedheight;
     setMovement ( int(floor(maxMovement() * oldperc + 0.5)) , 0 );
  } else {
     height = bitmappedheight;
     warning("Internal error: unit has invalid height");
  }
}


void Vehicle :: setMovement ( int newmove, double cargoDivisor )
{

   if ( cargoDivisor < 0 )
      cargoDivisor = typ->cargoMovementDivisor;

   if ( newmove < 0 )
      newmove = 0;

   if ( cargoDivisor > 0 && typ)
      if ( typ->movement[ log2 ( height ) ] ) {
         double diff = _movement - newmove;
         double perc = diff / typ->movement[ log2 ( height ) ] ;
         for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
            if ( *i ) {
               double lperc = perc;
               if ( cargoDivisor && cargoNestingDepth() == 0 )
                  lperc /= cargoDivisor;

               (*i)->decreaseMovement ( max( 1, int( ceil( lperc * double( (*i)->typ->movement[ log2 ( (*i)->height)] )))));
            }
   }
   _movement = newmove;
}

bool Vehicle::hasMoved ( void ) const
{
   return _movement != typ->movement[ log2 ( height )];
}


int Vehicle :: getMovement ( bool checkFuel ) const
{
   if ( !reactionfire.canMove() )
      return 0;

   if ( typ->fuelConsumption && checkFuel ) {
      if ( tank.fuel * minmalq / typ->fuelConsumption < _movement )
         return tank.fuel * minmalq / typ->fuelConsumption;
      else
         return _movement;
   } else
      return _movement;
}

void Vehicle :: decreaseMovement ( int amount )
{
  int newMovement = _movement - amount;
  if ( newMovement < 0 )
    newMovement = 0;
  if ( newMovement > typ->movement[log2(height)] )
    newMovement = typ->movement[log2(height)];
  setMovement ( newMovement );
}

bool Vehicle::movementLeft() const
{
   int mv  = getMovement();
   if ( mv <= 0 )
      return false;
   if ( mv >= 10 )
      return true;

   if ( height & ( chtieffliegend |chfliegend | chhochfliegend )) {
      WindMovement wm ( this );
      for ( int i = 0; i < 6; i++ )
         if ( 10 - wm.getDist( i ) <= mv )
            return true;
   }
   return false;
}

bool Vehicle :: canMove ( void ) const
{
   // if ( attacked && !(typ->functions & cf_moveafterattack))
   //   return false;

   if ( movementLeft() && reactionfire.canMove() ) {
      tfield* fld = gamemap->getField ( getPosition() );
      if ( fld->unitHere ( this ) ) {
         if ( terrainaccessible ( fld, this ) || actmap->getgameparameter( cgp_movefrominvalidfields))
            return true;
      } else {
         ContainerBase* cnt = fld->getContainer();
         if ( cnt )
            for ( Cargo::const_iterator i = cnt->getCargo().begin(); i != cnt->getCargo().end(); ++i )
               if ( *i == this ) 
                  if ( cnt->vehicleUnloadable( typ ) > 0 || cnt->vehicleDocking( this, true ) > 0 )
                     return true;
      }
   }
   return false;
}

bool Vehicle::spawnMoveObjects( const MapCoordinate& start, const MapCoordinate& dest )
{
   if ( start == dest )
      return false;
      
   bool result = false;
   
   if ( typ->objectLayedByMovement.size() && (height == chfahrend || height == chschwimmend))  {
      int dir = getdirection( start, dest );

      tfield* startField = gamemap->getField(start);
      tfield* destField = gamemap->getField(dest);

      for ( int i = 0; i < typ->objectLayedByMovement.size(); i++ ) 
         for ( int id = typ->objectLayedByMovement[i].from; id <= typ->objectLayedByMovement[i].to; ++id ) {
            ObjectType* object = objectTypeRepository.getObject_byID( id );
            if ( object ) 
               if ( startField->addobject ( object, 1 << dir ))
                  result = true;
         }
           
      dir = (dir + sidenum/2) % sidenum;

      for ( int i = 0; i < typ->objectLayedByMovement.size(); i++ ) 
         for ( int id = typ->objectLayedByMovement[i].from; id <= typ->objectLayedByMovement[i].to; ++id ) {
            ObjectType* object = objectTypeRepository.getObject_byID( id );
            if ( object ) 
               if ( destField->addobject ( object, 1 << dir ))
                  result = true;
         }
   }
   
   return result;
}


Vehicle::ReactionFire::ReactionFire ( Vehicle* _unit ) : unit ( _unit )
{
   weaponShots.resize(unit->typ->weapons.count);
   resetShotCount();
}


void Vehicle::ReactionFire::checkData ( )
{
   // the size could have changed because a unit was saved to disk with 1 weapon system, the type modified to 2 and the unit loaded again
   if ( weaponShots.size() < unit->typ->weapons.count ) {
      size_t oldsize = weaponShots.size();
      weaponShots.resize( unit->typ->weapons.count );
      for ( size_t i = oldsize; i < unit->typ->weapons.count; ++i )
         weaponShots[i] = unit->typ->weapons.weapon[i].reactionFireShots;
   }
}



void Vehicle::ReactionFire::resetShotCount()
{
   assertOrThrow( unit->typ->weapons.count <= weaponShots.size() );
   for ( int i = 0; i < unit->typ->weapons.count; ++i ) 
      weaponShots[i] = unit->typ->weapons.weapon[i].reactionFireShots;
}


int Vehicle::ReactionFire::enable ( void )
{
   if ( unit->typ->hasFunction( ContainerBaseType::NoReactionfire  ) )
      return -216;
      
   #ifdef karteneditor
   status = ready;
   #else
   if ( status == off ) {
      int weaponCount = 0;
      int shootableWeaponCount = 0;
      for ( int w = 0; w < unit->typ->weapons.count; w++ )
         if ( unit->typ->weapons.weapon[w].shootable() ) {
              weaponCount++;
              if ( unit->typ->weapons.weapon[w].sourceheight & unit->height )
                 shootableWeaponCount++;
         }

      if ( weaponCount == 0 )
         return -214;

      if ( shootableWeaponCount == 0 )
         return -213;

      if ( unit->typ->wait ) {
         // if ( unit->hasMoved())
         //    status = init1a;
         // else
            status = init2;
      } else {
         status = init2;
      }
   }
   #endif
   return 0;
}

void Vehicle::ReactionFire::disable ( void )
{
   if ( status != off ) {
      if ( status != init1a && status != init2 && !unit->typ->hasFunction(ContainerBaseType::MoveWithReactionFire)  ) {
          unit->setMovement ( 0, 0 );
       }
       status = off;
   }
}

void Vehicle::ReactionFire::endAnyTurn()
{
   resetShotCount();
}


void Vehicle::ReactionFire::endOwnTurn()
{
   if ( status != off ) {
      if ( status == init1a )
         status = init1b;
      else
         if ( status == init2 || status == init1b )
            status = ready;
   }
   nonattackableUnits.clear();
}

bool Vehicle::ReactionFire::canMove() const
{
   if ( unit->typ->hasFunction( ContainerBaseType::MoveWithReactionFire  ))
      return true;
   if ( status == off )
      return true;
   return false;
}

bool Vehicle::ReactionFire:: canPerformAttack( Vehicle* target )
{
   if ( !unit->getCarrier() )
      if ( unit->getMap()->getPlayer(unit).diplomacy.isHostile( actmap->actplayer))
         if ( getStatus() >= ready )
            if ( find ( nonattackableUnits.begin(), nonattackableUnits.end(), target->networkid) == nonattackableUnits.end() ) 
               // if ( enemiesAttackable & ( 1 << target->getOwner() ))
                  return true;
   
   return false;
}



const Vehicletype::HeightChangeMethod* Vehicle::getHeightChange( int dir, int height ) const
{
   if ( !reactionfire.canMove() )
      return NULL;

   if ( height == 0 )
      height = this->height;

   for ( int i = 0; i < typ->heightChangeMethodNum; i++ )
      if ( typ->heightChangeMethod[i].startHeight & height )
         if ( ( dir > 0 && typ->heightChangeMethod[i].heightDelta > 0) || ( dir < 0 && typ->heightChangeMethod[i].heightDelta < 0))
            if ( (1 << (log2(height) + typ->heightChangeMethod[i].heightDelta)) & typ->height )
               return &typ->heightChangeMethod[i];

   return NULL;
}



bool Vehicle :: weapexist( void )
{
   if ( typ->weapons.count > 0)
      for ( int b = 0; b < typ->weapons.count ; b++)
         if ( typ->weapons.weapon[b].shootable() )
            if ( typ->weapons.weapon[b].offensive() )
               if ( ammo[b] )
                  return 1;
    return 0;
}


void Vehicle :: setnewposition ( int x , int y )
{
   xpos = x;
   ypos = y;
   for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i ) 
         (*i)->setnewposition ( x , y );
}

void Vehicle :: setnewposition ( const MapCoordinate& mc )
{
   setnewposition ( mc.x, mc.y );
}

/*
int Vehicle :: getstrongestweapon( int aheight, int distance)
{
   int str = 0;
   int hw = 255;   //  error-wert  ( keine waffe gefunden )
   for ( int i = 0; i < typ->weapons.count; i++) {

      if (( ammo[i]) &&
          ( typ->weapons.weapon[i].mindistance <= distance) &&
          ( typ->weapons.weapon[i].maxdistance >= distance) &&
          ( typ->weapons.weapon[i].targ & aheight ) &&
          ( typ->weapons.weapon[i].sourceheight & height )) {
            int astr = int( weapstrength[i] * weapDist.getWeapStrength( &typ->weapons.weapon[i], distance, height, aheight));
            if ( astr > str ) {
               str = astr;
               hw  = i;
            }
       }
   }
   return hw;
}
*/

void Vehicle::convert ( int col )
{
  if ( col > 8)
     fatalError("convertvehicle: \n color mu�im bereich 0..8 sein ",2);

   int oldcol = getOwner();

   Player::VehicleList::iterator i = find ( gamemap->player[oldcol].vehicleList.begin(), gamemap->player[oldcol].vehicleList.end(), this );
   if ( i != gamemap->player[oldcol].vehicleList.end())
      gamemap->player[oldcol].vehicleList.erase ( i );

   gamemap->player[col].vehicleList.push_back( this );

   color = col << 3;

   for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i ) 
         (*i)->convert( col );

   // emit signal
   conquered();
}

Vehicle* Vehicle :: constructvehicle ( Vehicletype* tnk, int x, int y )
{
   if ( gamemap && vehicleconstructable( tnk, x, y )) {
      Vehicle* v = new Vehicle( tnk, gamemap, color/8 );
      v->xpos = x;
      v->ypos = y;
      v->addview();
   
      for ( int j = 0; j < 8; j++ ) {
         int a = int(height) << j;
         int b = int(height) >> j;
         if ( v->typ->height & a ) {
            v->height = a;
            break;
         }
         if ( v->typ->height & b ) {
            v->height = b;
            break;
         }
      }
      v->setMovement ( 0 );


      gamemap->getField ( x, y )->vehicle = v;
      tank.material -= tnk->productionCost.material;
      tank.fuel -= tnk->productionCost.energy;

      /*
      int refuel = 0;
      for ( int i = 0; i < typ->weapons.count; i++ )
         if ( typ->weapons.weapon[i].service()  )
            for ( int j = 0; j < typ->weapons.count ; j++) {
               if ( typ->weapons.weapon[j].canRefuel() )
                  refuel = 1;
               if ( typ->functions & (cffuelref | cfmaterialref) )
                  refuel = 1;
            }
      */
      v->attacked = 1;
      return v;
   } else
      return NULL;
}

bool  Vehicle :: vehicleconstructable ( Vehicletype* tnk, int x, int y )
{
   if ( !tnk->techDependency.available ( gamemap->player[getOwner()].research))
      return 0;

//              if ( getheightdelta( log2(tnk->height), log2(height) ) == 0 )
   if( (tnk->height & height ) || (( tnk->height & (chfahrend | chschwimmend)) && (height & (chfahrend | chschwimmend)))) {
      int hgt = height;
      if ( !(tnk->height & height))
         hgt = 1 << log2(tnk->height);
      if ( terrainaccessible2( gamemap->getField(x,y), tnk->terrainaccess, hgt ) > 0 )
   //   tnk->terrainaccess.accessible ( gamemap->getField(x,y)->bdt ) > 0 || height >= chtieffliegend)
         if ( tnk->productionCost.material <= tank.material &&
              tnk->productionCost.energy   <= tank.fuel  )
              if ( beeline (x, y, xpos, ypos) <= maxmalq )
                 return 1;

   }
   return 0;
}


bool Vehicle :: buildingconstructable ( const BuildingType* building, bool checkResources )
{
   if ( !building )
      return 0;


   if ( gamemap->getgameparameter(cgp_forbid_building_construction) )
       return 0;

   if ( !building->techDependency.available ( gamemap->player[getOwner()].research))
      return 0;


   int mf = gamemap->getgameparameter ( cgp_building_material_factor );
   int ff = gamemap->getgameparameter ( cgp_building_fuel_factor );

   if ( !mf )
      mf = 100;
   if ( !ff )
      ff = 100;

   int hd = getheightdelta ( log2 ( height ), log2 ( building->buildingheight ));

   if ( hd != 0 ) // && !(hd ==-1 && (height == chschwimmend || height == chfahrend)) ???  what was that ??
      return 0;


   if ( (building->productionCost.material * mf / 100 <= tank.material   &&   building->productionCost.fuel * ff / 100 <= tank.fuel) || !checkResources ) {
      if ( typ->hasFunction( ContainerBaseType::ConstructBuildings) )
         for ( int i = 0; i < typ->buildingsBuildable.size(); i++ )
            if ( typ->buildingsBuildable[i].from <= building->id &&
                 typ->buildingsBuildable[i].to   >= building->id )
               return true;
   }
   
   return false;
}

int Vehicle :: searchstackforfreeweight ( Vehicle* searchedInnerVehicle )
{
   if ( searchedInnerVehicle == this ) {
      return typ->maxLoadableWeight - cargoWeight();
   } else {
      int currentFreeWeight = typ->maxLoadableWeight - cargoWeight();
      int innerFreeWeight = -1;
      for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
         if ( *i ) {
            int w = (*i)->searchstackforfreeweight ( searchedInnerVehicle );
            if ( w >= 0 )
               innerFreeWeight = w;
         }

      if ( innerFreeWeight != -1 )
         return min ( currentFreeWeight, innerFreeWeight );
      else
         return -1;
   }
}
/*
Vehicle* Vehicle::findCarrierUnit ( const Vehicle* veh )
{
   for ( Cargo::const_iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i ) {
         if ( *i == veh )
            return this;
         else {
            const ContainerBase* cb = (*i)->findParentUnit( veh );
            if ( cb )
               return cb;
         }
      }

   return NULL;
}
*/


int Vehicle :: freeWeight ()
{
   tfield* fld = gamemap->getField ( xpos, ypos );
   if ( fld->vehicle )
      return fld->vehicle->searchstackforfreeweight ( this );
   else
      if ( fld->building ) {
         for ( Cargo::const_iterator i = fld->building->getCargo().begin(); i != fld->building->getCargo().end(); ++i )
            if ( *i ) {
               int w3 = (*i)->searchstackforfreeweight ( this );
               if ( w3 >= 0 )
                  return w3;
            }
      }

   return -2;
}


void Vehicle :: addview ()
{
   if ( viewOnMap )
      fatalError ("void Vehicle :: addview - the vehicle is already viewing the map");

   viewOnMap = true;
   tcomputevehicleview bes ( gamemap );
   bes.init( this, +1 );
   bes.startsearch();
}

void Vehicle :: removeview ()
{
   if ( !viewOnMap )
      fatalError ("void Vehicle :: removeview - the vehicle is not viewing the map");

   tcomputevehicleview bes ( gamemap );
   bes.init( this, -1 );
   bes.startsearch();

   viewOnMap = false;
}


void Vehicle :: postAttack( bool reactionFire )
{
   if ( !reactionFire )
      attacked = true;
   
   if ( typ->hasFunction( ContainerBaseType::MoveAfterAttack  ) )
      decreaseMovement ( maxMovement() * attackmovecost / 100 );
   else
      if ( reactionfire.getStatus() == ReactionFire::off )
         setMovement ( 0 );
}


void Vehicle::setAttacked()
{
   attacked = true;
   for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i ) 
         (*i)->setAttacked();
}


class tsearchforminablefields: public SearchFields {
      int shareview;
    public:
      int numberoffields;
      int run ( const Vehicle*     eht );
      virtual void testfield ( const MapCoordinate& mc );
      tsearchforminablefields ( GameMap* _gamemap ) : SearchFields ( _gamemap ) {};
  };

  /*
bool Vehicle::searchForMineralResourcesAvailable()
{
   return tsearchforminablefields::available( this );
}
  */


void         tsearchforminablefields::testfield( const MapCoordinate& mc )
{
    tfield* fld = gamemap->getField ( mc );
    if ( !fld->building  ||  fld->building->color == gamemap->actplayer*8  ||  fld->building->color == 8*8)
       if ( !fld->vehicle  ||  fld->vehicle->color == gamemap->actplayer*8 ||  fld->vehicle->color == 8*8) {
          if ( !fld->resourceview )
             fld->resourceview = new tfield::Resourceview;

          for ( int c = 0; c < 8; c++ )
             if ( shareview & (1 << c) ) {
                fld->resourceview->visible |= ( 1 << c );
                fld->resourceview->fuelvisible[c] = fld->fuel;
                fld->resourceview->materialvisible[c] = fld->material;
             }
       }
}




int  tsearchforminablefields::run( const Vehicle* eht )
{
   if ( !eht->typ->hasFunction( ContainerBaseType::DetectsMineralResources  ) )
      return -311;


   shareview = 1 << ( eht->color / 8);
   for ( int i = 0; i < 8; i++ )
      if ( i*8 != eht->color )
         if ( gamemap->player[i].exist() )
            if ( gamemap->getPlayer(eht).diplomacy.sharesView(i) )
               shareview += 1 << i;

   numberoffields = 0;
   initsearch( eht->getPosition(), eht->typ->digrange, 0 );
   if ( eht->typ->digrange )
      startsearch();

//   if ( (eht->typ->functions & cfmanualdigger) && !(eht->typ->functions & cfautodigger) )
//      eht->setMovement ( eht->getMovement() - searchforresorcesmovedecrease );

//   if ( !gamemap->mineralResourcesDisplayed && (eht->typ->functions & cfmanualdigger) && !(eht->typ->functions & cfautodigger))
//      gamemap->mineralResourcesDisplayed = 1;

   return 1;
}


int Vehicle::maxMovement ( void ) const
{
   return typ->movement[log2(height)];
}

int Vehicle::searchForMineralResources ( void ) const
{
    tsearchforminablefields sfmf ( gamemap );
    return sfmf.run( this );
}


void Vehicle :: fillMagically( bool ammunition, bool resources )
{
   if ( resources )
      tank = getStorageCapacity();

   if ( ammunition ) 
      for ( int m = 0; m < typ->weapons.count ; m++) {
         ammo[m] = typ->weapons.weapon[m].count;
         weapstrength[m] = typ->weapons.weapon[m].maxstrength;
      }
}




#define cem_experience    0x1
#define cem_damage        0x2
#define cem_fuel          0x4
#define cem_ammunition    0x8
#define cem_weapstrength  0x10
#define cem_loading       0x20
#define cem_attacked      0x40
#define cem_height        0x80
#define cem_movement      0x100
#define cem_direction     0x200
#define cem_material      0x400
#define cem_energy        0x800
#define cem_class         0x1000
#define cem_networkid     0x2000
#define cem_name          0x4000
#define cem_armor         0x8000
#define cem_reactionfire  0x10000
#define cem_reactionfire2 0x20000
#define cem_poweron       0x40000
#define cem_weapstrength2 0x80000
#define cem_ammunition2   0x100000
#define cem_energyUsed    0x200000
#define cem_position      0x400000
#define cem_aiparam       0x800000
#define cem_version       0x1000000




const int vehicleVersion = 6;

void   Vehicle::write ( tnstream& stream, bool includeLoadedUnits )
{
    stream.writeWord ( 0 );
    stream.writeInt( min( vehicleVersion, UNITVERSIONLIMIT ) );
    stream.writeInt( typ->id );

    stream.writeChar ( color );

    int bm = cem_version;

    if ( experience )
       bm |= cem_experience;
    if ( damage    )
       bm |= cem_damage;

    bm |= cem_energy;
    bm |= cem_fuel;
    bm |= cem_material;

    if ( typ->weapons.count )
       for (int m = 0; m < typ->weapons.count ; m++) {
          if ( ammo[m] < typ->weapons.weapon[m].count )
             bm |= cem_ammunition2;
          if ( weapstrength[m] != typ->weapons.weapon[m].maxstrength )
             bm |= cem_weapstrength2;
       }

    if ( includeLoadedUnits )
       for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
          if ( *i ) 
             bm |= cem_loading;

    if ( attacked  )
       bm |= cem_attacked;
    if ( height != chfahrend )
       bm |= cem_height;

//    if ( _movement < typ->movement[log2(height)] )
    bm |= cem_movement;

    if ( direction )
       bm |= cem_direction;


    if ( networkid )
       bm |= cem_networkid;

    if ( !name.empty() )
       bm |= cem_name;

    if ( reactionfire.status )
       bm |= cem_reactionfire;

    if ( generatoractive )
       bm |= cem_poweron;

    if ( xpos != 0 || ypos != 0 )
       bm |= cem_position;

    for ( int i = 0; i < 8; i++ )
       if ( aiparam[i] )
          bm |= cem_aiparam;



    stream.writeInt( bm );

    stream.writeInt( min( vehicleVersion, UNITVERSIONLIMIT )  );

    if ( bm & cem_experience )
         stream.writeChar ( experience );

    if ( bm & cem_damage )
         stream.writeChar ( damage );

    if ( bm & cem_fuel )
         stream.writeInt ( tank.fuel );

    if ( bm & cem_ammunition2 )
       for ( int j= 0; j < 16; j++ )
         stream.writeInt ( ammo[j] );

    if ( bm & cem_weapstrength2 )
       for ( int j = 0; j < 16; j++ )
         stream.writeInt ( weapstrength[j] );

    if ( bm & cem_loading ) {
       int c=0;
       for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
          if ( *i ) 
             ++c;

       if ( UNITVERSIONLIMIT > 3 ) 
         stream.writeInt ( c );
       else
         stream.writeChar ( c );

       for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
          if ( *i ) 
             (*i)->write ( stream );
    }

    if ( bm & cem_height )
         stream.writeChar ( height );

    if ( bm & cem_movement )
         stream.writeChar ( _movement );

    if ( bm & cem_direction )
         stream.writeChar ( direction );

    if ( bm & cem_material )
         stream.writeInt ( tank.material );

    if ( bm & cem_energy )
         stream.writeInt ( tank.energy );

    if ( bm & cem_networkid )
         stream.writeInt ( networkid );

    if ( bm & cem_attacked )
         stream.writeChar ( attacked );

    if ( bm & cem_name     )
         stream.writeString ( name );

    if ( bm & cem_reactionfire )
       stream.writeChar ( reactionfire.status );

    if ( bm & cem_poweron )
       stream.writeInt ( generatoractive );

    if ( bm & cem_position ) {
       stream.writeInt ( xpos );
       stream.writeInt ( ypos );
    }

    if ( bm & cem_aiparam ) {
       stream.writeInt( 0x23451234 );
       for ( int i = 0; i < 8; i++ )
          stream.writeInt ( aiparam[i] != NULL );

       for ( int i = 0; i < 8; i++ )
          if ( aiparam[i] )
             aiparam[i]->write ( stream );

       stream.writeInt( 0x23451234 );
    }

    writeClassContainer( reactionfire.weaponShots, stream );
    writeClassContainer( reactionfire.nonattackableUnits, stream );

    if ( UNITVERSIONLIMIT >= 5 ) {
      stream.writeInt( internalUnitProduction.size() );
      for ( int i = 0; i < internalUnitProduction.size(); ++i )
         stream.writeInt( internalUnitProduction[i]->id );
    }

    if ( UNITVERSIONLIMIT >= 6 ) {
      stream.writeInt( maxresearchpoints );
      stream.writeInt( researchpoints );
      plus.write( stream );
      maxplus.write( stream );
      // actstorage.write( stream );
      bi_resourceplus.write( stream );
    }
}

void   Vehicle::read ( tnstream& stream )
{
    int _id = stream.readWord ();
    int version = 0;
    if ( _id == 0 ) {
       version = stream.readInt();
       _id = stream.readInt();
    }

    stream.readChar (); // color
    if ( _id != typ->id )
       fatalError ( "Vehicle::read - trying to read a unit of different type" );

    readData ( stream );
}

void   Vehicle::readData ( tnstream& stream )
{

    int bm = stream.readInt();

    int version = 0;
    if ( bm & cem_version )
       version = stream.readInt();

    if ( bm & cem_experience )
       experience = stream.readChar();
    else
       experience = 0;

    if ( bm & cem_damage )
       damage = stream.readChar();
    else
       damage = 0;

    if ( bm & cem_fuel ) {
       tank.fuel = stream.readInt();
       if ( tank.fuel > getStorageCapacity().fuel )
          tank.fuel = getStorageCapacity().fuel;
    } else
       tank.fuel = getStorageCapacity().fuel;

    if ( bm & cem_ammunition ) {
       for ( int i = 0; i < 8; i++ )
         ammo[i] = stream.readWord();
    } else
     if ( bm & cem_ammunition2 ) {
        for ( int i = 0; i < 16; i++ ) {
          ammo[i] = stream.readInt();
          if ( ammo[i] > typ->weapons.weapon[i].count )
             ammo[i] = typ->weapons.weapon[i].count;
          if ( ammo[i] < 0 )
             ammo[i] = 0;
        }

     } else
       for (int i=0; i < typ->weapons.count ;i++ )
          ammo[i] = typ->weapons.weapon[i].count;


    if ( bm & cem_weapstrength ) {
       for ( int i = 0; i < 8; i++ )
         weapstrength[i] = stream.readWord();

    } else
     if ( bm & cem_weapstrength2 ) {
        for ( int i = 0; i < 16; i++ )
           weapstrength[i] = stream.readInt();
     } else
       for (int i=0; i < typ->weapons.count ;i++ )
          weapstrength[i] = typ->weapons.weapon[i].maxstrength;

    if ( bm & cem_loading ) {
       int c;
       if ( version <= 3 )
          c = stream.readChar();
       else
          c = stream.readInt();   

       if ( c ) {
          for (int k = 0; k < c; k++) {
             Vehicle* v = Vehicle::newFromStream ( gamemap, stream );
             addToCargo(v);
          }
       }
    }

    if ( bm & cem_height )
       height = stream.readChar();
    else
       height = chfahrend;

    if ( ! (height & typ->height) )
       height = 1 << log2 ( typ->height );

    if ( bm & cem_movement ) {
       int m = stream.readChar ( );
       setMovement ( min( m,typ->movement [ log2 ( height ) ]), 0 );
    } else
       setMovement ( typ->movement [ log2 ( height ) ], 0 );

    if ( bm & cem_direction )
       direction = stream.readChar();
    else
       direction = 0;

    if ( bm & cem_material ){
       tank.material = min( stream.readInt(), getStorageCapacity().material);
    } else
       tank.material = getStorageCapacity().material;

    if ( bm & cem_energy ) {
       tank.energy = min ( stream.readInt(), getStorageCapacity().energy);
       if ( tank.energy < 0 )
          tank.energy = 0;
    } else
       tank.energy = getStorageCapacity().energy;

    if ( bm & cem_class )
       stream.readChar(); // was: class

    if ( bm & cem_armor )
       stream.readWord(); // was: armor

    if ( bm & cem_networkid )
       networkid = stream.readInt();
    else
       networkid = 0;

    if ( bm & cem_attacked )
       attacked = stream.readChar();
    else
       attacked = false;

    if ( bm & cem_name )
       name = stream.readString ( );

    int reactionfirestatus = 0;
    if ( bm & cem_reactionfire )
       reactionfirestatus = stream.readChar();

    int reactionfireenemiesAttackable = 0;
    if ( bm & cem_reactionfire2 )
       reactionfireenemiesAttackable = stream.readChar();  

    if ( reactionfirestatus >= 8 && reactionfireenemiesAttackable <= 4 ) { // for transition from the old reactionfire system ( < ASC1.2.0 ) to the new one ( >= ASC1.2.0 )
       reactionfire.status = ReactionFire::Status ( reactionfireenemiesAttackable );
       setMovement ( typ->movement [ log2 ( height ) ], 0 );
    } else
       reactionfire.status = ReactionFire::Status ( reactionfirestatus );

    if ( bm & cem_poweron )
       generatoractive = stream.readInt();
    else
       generatoractive = 0;

    if ( bm & cem_energyUsed )
       stream.readInt ();

    if ( bm & cem_position ) {
       int x = stream.readInt ( );
       int y = stream.readInt ( );
       setnewposition ( x, y );
    } else
       setnewposition ( 0, 0 );

    if ( bm & cem_aiparam ) {
       int magic = stream.readInt();
       if ( magic != 0x23451234 )
          throw ASCmsgException ( "Vehicle::read() - inconsistent data stream" );
       for ( int i = 0; i < 8; i++ ) {
          bool b = stream.readInt ( );
          if ( b )
             aiparam[i] = new AiParameter ( this );
          else
             if ( aiparam[i] ) {
               delete aiparam[i];
               aiparam[i] = NULL;
             }
       }

       for ( int i = 0; i < 8; i++ )
          if ( aiparam[i] )
             aiparam[i]->read ( stream );

       magic = stream.readInt();
       if ( magic != 0x23451234 )
          throw ASCmsgException ( "Vehicle::read() - inconsistent data stream" );
    }

    for ( int m = 0; m < typ->weapons.count ; m++)
       if ( typ->weapons.weapon[m].getScalarWeaponType() >= 0 )
          weapstrength[m] = typ->weapons.weapon[m].maxstrength;
       else
          weapstrength[m] = 0;

    if ( version >= 1 ) {
      readClassContainer( reactionfire.weaponShots, stream );
      reactionfire.checkData();
    }
    if ( version >= 2 )
      readClassContainer( reactionfire.nonattackableUnits, stream );

    internalUnitProduction.clear();
    if ( version >= 5 ) {
       int pcount = stream.readInt();
       for ( int i = 0; i< pcount; ++i ) {
          int id = stream.readInt();
          internalUnitProduction.push_back ( gamemap->getvehicletype_byid ( id ) );
          if ( !internalUnitProduction[i] )
             throw InvalidID ( "unit", id );
       }
    }

    if ( version >= 6 ) {
       maxresearchpoints = stream.readInt();
       researchpoints = stream.readInt();
       plus.read( stream );
       maxplus.read( stream );
       bi_resourceplus.read( stream );
    } 
}

MapCoordinate3D Vehicle :: getPosition ( ) const
{
   return MapCoordinate3D ( xpos, ypos, height );
}

MapCoordinate3D Vehicle :: getPosition3D ( ) const
{
   if ( gamemap->getField(xpos,ypos)->unitHere(this) )
      return MapCoordinate3D ( xpos, ypos, height );
   else {
      MapCoordinate3D pos;
      pos.setnum ( xpos, ypos, -1 );
      return pos;
   }
}


ASCString  Vehicle::getName() const
{
   if ( name.empty() )
      return typ->getName();
   else
      return name;
}

int Vehicle::getAmmo( int type, int num, bool queryOnly ) 
{
   if ( num < 0 )
      return -putAmmo( type, -num, queryOnly );

   int got = 0;
   int weap = 0;
   while ( weap < typ->weapons.count && got < num ) {
      if ( typ->weapons.weapon[weap].getScalarWeaponType() == type ) {
         int toget = min( num - got, ammo[weap]);
         if ( !queryOnly )
            ammo[weap] -= toget;
         got += toget;
      }
      ++weap;
   }
   // if ( got && !queryOnly )
   //   ammoChanged();
   return got;
}

int Vehicle::putAmmo( int type, int num, bool queryOnly )
{
   if ( num < 0 )
      return -getAmmo( type, -num, queryOnly );

   int put = 0;
   int weap = 0;
   while ( weap < typ->weapons.count && put < num ) {
      if ( typ->weapons.weapon[weap].getScalarWeaponType() == type ) {
         int toput = min( num - put, typ->weapons.weapon[weap].count - ammo[weap]);
         if ( !queryOnly )
            ammo[weap] += toput;
         put += toput;
      }
      ++weap;
   }
   // if ( put && !queryOnly )
   //   ammoChanged();
   return put;
}


int Vehicle::maxAmmo( int type ) const
{
   int ammo = 0;
   for ( int i = 0; i < typ->weapons.count; ++i ) 
      if ( typ->weapons.weapon[i].getScalarWeaponType() == type )
         ammo += typ->weapons.weapon[i].count;
   return ammo;

}


int Vehicle::getArmor() const
{
   return typ->armor;
}


void Vehicle::paint ( Surface& s, SPoint pos, int shadowDist ) const
{
  #ifdef sgmain
   bool shaded = (!canMove()) && maxMovement() && ( color == gamemap->actplayer*8) && (attacked || !typ->weapons.count || CGameOptions::Instance()->units_gray_after_move );
  #else
   bool shaded = 0;
  #endif

   paintField( typ->getImage(), s, pos, direction, shaded, shadowDist );
}

void Vehicle::paint ( Surface& s, SPoint pos, bool shaded, int shadowDist ) const
{
   paintField( typ->getImage(), s, pos, direction, shaded, shadowDist );
}

vector<MapCoordinate> Vehicle::getCoveredFields()
{
   vector<MapCoordinate> fields;
   fields.push_back( getPosition() );
   return fields;
}



Vehicle* Vehicle::newFromStream ( GameMap* gamemap, tnstream& stream, int forceNetworkID )
{
   int id = stream.readWord ();
   int version = 0;
   if ( id == 0 ) {
      version = stream.readInt();
      id = stream.readInt();
   }

   Vehicletype* fzt = gamemap->getvehicletype_byid ( id );
   if ( !fzt )
      throw InvalidID ( "vehicle", id );

   int color = stream.readChar ();

   Vehicle* v = new Vehicle ( fzt, gamemap, color/8, -2 );

   v->readData ( stream );

   if ( forceNetworkID > 0 )
      v->networkid = forceNetworkID;

   GameMap::VehicleLookupCache::iterator j = gamemap->vehicleLookupCache.find( v->networkid);
   if ( j != gamemap->vehicleLookupCache.end() ) {
      // warning("duplicate network id detected !");
      gamemap->unitnetworkid++;
      v->networkid = gamemap->unitnetworkid;
   }

   gamemap->vehicleLookupCache[v->networkid] = v;
   return v;
}

/** Returns the SingleWeapon corresponding to the weaponNum for this
 *  vehicle.
 */
const SingleWeapon* Vehicle::getWeapon( unsigned weaponNum )
{
  if ( weaponNum <= typ->weapons.count )
     return &typ->weapons.weapon[weaponNum];
  else
     return NULL;
}



ASCString getUnitReference ( Vehicle* veh )
{
   ASCString s = "The unit " + veh->getName();
   s += " (position: "+ veh->getPosition().toString() + ") ";
   return s;
}



int UnitHooveringLogic::calcFuelUsage( const Vehicle* veh )
{
   if ( veh->height < chtieffliegend || veh->height > chhochfliegend )
      return 0;

   int mo = veh->maxMovement();
   if ( mo )
      return ( mo - veh->getMovement(false) )  * FuelConsumption / (100 * minmalq ) * veh->typ->fuelConsumption;
   else
      return (veh->getMap()->weather.windSpeed * maxwindspeed / 256 ) * veh->typ->fuelConsumption / ( minmalq * 64 );
}

int UnitHooveringLogic::getEndurance ( const Vehicle* veh )
{
   if ( veh->height < chtieffliegend || veh->height > chhochfliegend || veh->typ->fuelConsumption <= 0)
      return -1;


   int fuelUsage = calcFuelUsage( veh );
   if ( fuelUsage > veh->getTank().fuel )
      return 0;


   int fields = (veh->getTank().fuel - fuelUsage) * FuelConsumption / (veh->typ->fuelConsumption * 100 ); 
   if ( veh->maxMovement() )
      return 1 + (fields / veh->maxMovement() );
   else
      return 1;
}
