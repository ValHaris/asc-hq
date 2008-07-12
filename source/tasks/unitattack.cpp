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


#include "unitattack.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"


UnitAttack :: UnitAttack ( Vehicle* unit )
   : UnitTask ( unit->getMap(), unit->networkid ),
   kamikaze(false)
{
   
}

ActionResult UnitAttack::searchTargets()
{
   if ( !getUnit() ) 
      return ActionResult(201);

   int weaponCount = 0;
   int shootableWeaponCount = 0;
   for ( int w = 0; w < getUnit()->typ->weapons.count; w++ )
      if ( getUnit->typ->weapons.weapon[w].shootable() ) {
            weaponCount++;
            if ( getUnit->typ->weapons.weapon[w].sourceheight & vehicle->height )
               shootableWeaponCount++;
      }

   if ( weaponCount == 0 )
      return ActionResult(214);

   if ( shootableWeaponCount == 0 )
      return ActionResult(213);
   
   
   /*
   moveparams.movesx = angreifer->xpos;  // this is currently still needed for wepselguihost
   moveparams.movesy = angreifer->ypos;
   */
   
   if (fieldvisiblenow( getMap()->getField( getUnit()->getPosition() )) == false)
      return ActionResult(1);

   if (getUnit()->attacked)
      return ActionResult(202);
   

   if (getUnit()->typ->weapons.count == 0)
      return ActionResult(204);

   if ( getUnit()->typ->wait && getUnit()->hasMoved() && getUnit()->reactionfire.getStatus() != Vehicle::ReactionFire::ready )
      return ActionResult(215);


   int d = 0;
   int maxdist = 0;
   int mindist = 20000;
   for ( int a = 0; a < getUnit()->typ->weapons.count; a++)
      if ( getUnit()->ammo[a] > 0) {
         d++;
         maxdist = max( maxdist, getUnit()->typ->weapons.weapon[a].maxdistance / maxmalq );
         mindist = min ( mindist, (getUnit()->typ->weapons.weapon[a].mindistance + maxmalq - 1) / maxmalq);
      }


   if (d == 0)
      return ActionResult(204);

   circularFieldIterator( getMap(), maxdist, mindist, unit->getPosition(), FieldIterationFunctor( this, &UnitAttack::fieldChecker ));
         
   if ( attackableUnits.size() + attackableBuildings.size() + attackableObjects.size() + attackableUnitsKamikaze.size() ) {
      setState(Evaluated);
      return ActionResult(0);
   } else
      return ActionResult(206);
}

void UnitAttack :: fieldChecker( const MapCoordinate& pos )
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
                if (((getUnit()->height >= chtieffliegend) && (eht->height <= angreifer->height) && (eht->height >= chschwimmend)) 
                  || ((getUnit()->height == chfahrend) && (eht->height == chfahrend)) 
                  || ((getUnit()->height == chschwimmend) && (eht->height == chschwimmend))
                  || ((getUnit()->height == chgetaucht) && (eht->height >=  chgetaucht) && (eht->height <= chschwimmend))) {
                  attackableUnitsKamikaze.push_back ( pos );   
                } 
          } 
      }
   } 
}


void UnitAttack :: setTarget( const MapCoordinate& target )
{
   
   if ( getState() == Evaluated ) {
      this->untilDestruction = false;
      
      setState( SetUp );
   }
}

ActionResult UnitAttack::go ( Context& context )
{
   MapCoordinate targetPosition;
   
   if ( getState() != SetUP )
      return ActionResult(21002);
   
   AttackWeap* atw = NULL;
   
   if ( targetUnit > 0 ) {
      Vehicle* veh = getMap()->getUnit( targetUnit );
      if ( !veh )
         return ActionResult(21001);
      
      targetPosition = veh->getPosition();
      if ( attackableVehicles.find(targetPosition) == attackableVehicle.end() )
         return ActionResult(208);
      
      atw = &attackableVehicles[targetPosition];
   } else
      if ( targetBuilding.valid() ) {
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
   
   
   tfight* battle = NULL;
   switch ( atw->target ) {
      case AttackWeap::vehicle: battle = new tunitattacksunit ( vehicle, getfield(x,y)->vehicle, 1, weapnum );
         break;
      case AttackWeap::building: battle = new tunitattacksbuilding ( vehicle, x, y , weapnum );
         break;
      case AttackWeap::object: battle = new tunitattacksobject ( vehicle, x, y, weapnum );
         break;
      default : return ActionResult(217);
   } /* endswitch */

   int ad1 = battle->av.damage;
   int dd1 = battle->dv.damage;

   int xp1 = vehicle->xpos;
   int yp1 = vehicle->ypos;

   if ( context.mapDisplay && fieldvisiblenow ( getMap()->getfield(targetPosition), context.viewingPlayer) ) {
      mapDisplay->displayActionCursor ( getUnit()->getPosition(), targetPosition );
      mapDisplay->showBattle( *battle );
      mapDisplay->removeActionCursor ( );
      shown = 1;
   } else {
      battle->calc();
      shown = 0;
   }

   int ad2 = battle->av.damage;
   int dd2 = battle->dv.damage;

   if ( !vehicle->typ->hasFunction( ContainerBaseType::MoveAfterAttack )) 
      vehicle->setMovement ( 0 );

   battle->setresult ();

   logtoreplayinfo ( rpl_attack, xp1, yp1, x, y, ad1, ad2, dd1, dd2, weapnum );

   evaluateviewcalculation( actmap );

   if ( mapDisplay && shown )
      mapDisplay->displayMap();

}
