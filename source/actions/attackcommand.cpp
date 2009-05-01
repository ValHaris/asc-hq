/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2008  Martin Bickel  and  Marc Schellenberger
 
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


#include "attackcommand.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "../mapdisplayinterface.h"
#include "vehicleattack.h"
#include "action-registry.h"


bool AttackCommand :: avail ( Vehicle* eht )
{
   if ( eht )
      if ( eht->attacked == false )
         if ( eht->weapexist() )
            if (eht->typ->wait == false  ||  !eht->hasMoved() )
                  return true;
   return false;
}


AttackCommand :: AttackCommand ( Vehicle* unit )
   : UnitCommand ( unit ), targetUnitID(-1), weapon(-1), kamikaze(false)
{
   
}

ActionResult AttackCommand::searchTargets()
{
   if ( !getUnit() ) 
      return ActionResult(201);

   Vehicle* unit = getUnit();
   
   int weaponCount = 0;
   int shootableWeaponCount = 0;
   for ( int w = 0; w < getUnit()->typ->weapons.count; w++ )
      if ( unit->typ->weapons.weapon[w].shootable() ) {
            weaponCount++;
            if ( unit->typ->weapons.weapon[w].sourceheight & unit->height )
               shootableWeaponCount++;
      }

   if ( weaponCount == 0 )
      return ActionResult(214);

   if ( shootableWeaponCount == 0 )
      return ActionResult(213);
   
   
   
   if (fieldvisiblenow( getMap()->getField( unit->getPosition() ), getMap()->actplayer ) == false)
      return ActionResult(1);

   if (unit->attacked)
      return ActionResult(202);
   

   if ( unit->typ->weapons.count == 0)
      return ActionResult(204);

   if ( unit->typ->wait && getUnit()->hasMoved() && unit->reactionfire.getStatus() != Vehicle::ReactionFire::ready )
      return ActionResult(215);


   attackableUnits.clear();
   attackableUnitsKamikaze.clear();
   attackableBuildings.clear();
   attackableObjects.clear();
   
   
   int d = 0;
   int maxdist = 0;
   int mindist = 20000;
   for ( int a = 0; a < unit->typ->weapons.count; a++)
      if ( unit->ammo[a] > 0) {
         d++;
         maxdist = max( maxdist, unit->typ->weapons.weapon[a].maxdistance / maxmalq );
         mindist = min ( mindist, (unit->typ->weapons.weapon[a].mindistance + maxmalq - 1) / maxmalq);
      }


   if (d == 0)
      return ActionResult(204);

   circularFieldIterator( getMap(), unit->getPosition(), maxdist, mindist, FieldIterationFunctor( this, &AttackCommand::fieldChecker ));
         
   if ( attackableUnits.size() + attackableBuildings.size() + attackableObjects.size() + attackableUnitsKamikaze.size() ) {
      setState(Evaluated);
      return ActionResult(0);
   } else
      return ActionResult(206);
}

void AttackCommand :: fieldChecker( const MapCoordinate& pos )
{
   if ( fieldvisiblenow( getMap()->getField(pos)) ) {
      if ( !kamikaze ) {
         AttackWeap* atw = attackpossible( getUnit(), pos.x, pos.y );
         if (atw->count > 0) { 
            switch ( atw->target ) {
               case AttackWeap::vehicle:  attackableUnits[pos] = *atw ;
                  break;                                    
               case AttackWeap::building: attackableBuildings[pos] = *atw;
                  break;
               case AttackWeap::object:   attackableObjects[pos] = *atw;
                  break;
               default:;
            } /* endswitch */
         } 
         delete atw;
      } else {
          tfield* fld = getMap()->getField(pos);
          if (fieldvisiblenow( fld )) {
             Vehicle* eht = fld->vehicle;
             if (eht != NULL) 
                if (((getUnit()->height >= chtieffliegend) && (eht->height <= getUnit()->height) && (eht->height >= chschwimmend)) 
                  || ((getUnit()->height == chfahrend) && (eht->height == chfahrend)) 
                  || ((getUnit()->height == chschwimmend) && (eht->height == chschwimmend))
                  || ((getUnit()->height == chgetaucht) && (eht->height >=  chgetaucht) && (eht->height <= chschwimmend))) {
                  attackableUnitsKamikaze.push_back ( pos );   
                } 
          } 
      }
   } 
}


void AttackCommand :: setTarget( const MapCoordinate& target, int weapon )
{
   
 //  if ( getState() == Evaluated ) {
      this->weapon = weapon;
      this->target  = target;
      tfield* fld = getMap()->getField( target );
      if ( fld->vehicle )
         targetUnitID = fld->vehicle->networkid;
      else
         targetBuilding = target;
      
      setState( SetUp );
 //  }
}

ActionResult AttackCommand::go ( const Context& context )
{
   MapCoordinate targetPosition;
   
   if ( getState() != SetUp )
      return ActionResult(22000);
   
   searchTargets();
   
   AttackWeap* atw = NULL;
   
   if ( targetUnitID > 0 ) {
      Vehicle* veh = getMap()->getUnit( targetUnitID );
      if ( !veh )
         return ActionResult(21001);
      
      targetPosition = veh->getPosition();
      if ( attackableUnits.find(targetPosition) == attackableUnits.end() )
         return ActionResult(208);
      
      atw = &attackableUnits[targetPosition];
   } else
      if ( targetBuilding.valid() && getMap()->getField(targetBuilding)->building ) {
         if ( attackableBuildings.find(targetBuilding) == attackableBuildings.end() )
            return ActionResult(217);
         atw = &attackableBuildings[targetBuilding];
         targetPosition = targetBuilding;
      } else {
         if ( attackableObjects.find(targetBuilding) == attackableObjects.end() )
            return ActionResult(217);
         atw = &attackableObjects[target];
         targetPosition = target;
      }
   

   if ( !atw ) 
      return ActionResult(217);
   
   ActionResult res = (new VehicleAttackAction(getMap(), getUnitID(), targetPosition, weapon ))->execute( context );
   if ( res.successful() )
      setState( Completed );
   else
      setState( Failed );
   return res;
    
}

static const int attackCommandVersion = 2;

void AttackCommand :: readData ( tnstream& stream )
{
   UnitCommand::readData( stream );
   int version = stream.readInt();
   if ( version > attackCommandVersion )
      throw tinvalidversion ( "AttackCommand", attackCommandVersion, version );
   target.read( stream );
   targetUnitID = stream.readInt();
   targetBuilding.read( stream );
   weapon = stream.readInt();
   if ( version >= 2 )
      kamikaze = stream.readInt();
   else
      kamikaze = false;
}

void AttackCommand :: writeData ( tnstream& stream ) const
{
   UnitCommand::writeData( stream );
   stream.writeInt( attackCommandVersion );
   target.write( stream );
   stream.writeInt( targetUnitID );
   targetBuilding.write( stream );
   stream.writeInt( weapon );
   stream.writeInt( kamikaze );   
}

ASCString AttackCommand :: getCommandString() const {
   ASCString c;
   c.format("attack ( %d, %d, %d, %d )", getUnitID(), target.x, target.y, weapon );
   return c;
   
}

GameActionID AttackCommand::getID() const 
{
   return ActionRegistry::AttackCommand;   
}

ASCString AttackCommand::getDescription() const
{
   ASCString s = "Attacking "; 
   s += " " + target.toString();
   if ( getUnit() ) {
      s += " with " + getUnit()->getName();
   }
   return s;
}

namespace {
   const bool r1 = registerAction<AttackCommand> ( ActionRegistry::AttackCommand );
}

