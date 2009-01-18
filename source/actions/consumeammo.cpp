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


#include "consumeammo.h"

#include "../vehicle.h"
#include "../gamemap.h"

#include "consumeresource.h"
     
     
ConsumeAmmo::ConsumeAmmo( ContainerBase* veh, int ammoType, int slot, int count )
   : ContainerAction( veh ), produceAmmo(false), produced(0)
{
   this->ammoType = ammoType;
   this->slot = slot;
   this->count = count;
   
   resultingAmmount = -1;
}

      
ASCString ConsumeAmmo::getDescription() const
{
   ASCString res = "Consume " + ASCString::toString(count) + " pieces of ammo ";
   if ( slot >= 0 ) 
      res += "from slot " + ASCString::toString(slot);
   
   if ( getContainer() ) 
      res += " from unit " + getContainer()->getName();
   return  res;
}
      
static const int consumeAmmoStreamVersion = 2;      
      
void ConsumeAmmo::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version == 1 ) {
      int vehicleID = stream.readInt();
      setID( vehicleID );
   } else {
      if ( version >   consumeAmmoStreamVersion )
         throw tinvalidversion ( "ConsumeAmmo", consumeAmmoStreamVersion, version );
   
      ContainerAction::readData( stream );
   }
      
   ammoType = stream.readInt();
   slot = stream.readInt();
   count = stream.readInt();
   resultingAmmount = stream.readInt();
   
   if ( version >= 2 ) {
      produceAmmo = stream.readInt();  
      produced = stream.readInt();
   }
   
};
      
      
void ConsumeAmmo::writeData ( tnstream& stream ) const
{
   stream.writeInt( consumeAmmoStreamVersion  );
   ContainerAction::writeData( stream );
   stream.writeInt( ammoType );
   stream.writeInt( slot );
   stream.writeInt( count );
   stream.writeInt( resultingAmmount );
   stream.writeInt( produceAmmo ); 
   stream.writeInt( produced );
};

void ConsumeAmmo::setAmmoProduction( bool prod )
{
   produceAmmo = prod;  
}


GameActionID ConsumeAmmo::getID() const
{
   return ActionRegistry::ConsumeAmmo;
}

int ConsumeAmmo::produce( int num, const Context& context, bool queryOnly )
{
   if ( !produceAmmo )
      return 0;
   
   if ( !getContainer()->baseType->hasFunction( ContainerBaseType::AmmoProduction )  ) 
      return 0;
   
   Resources needed ( ammoProductionCost[ammoType][0] * num, ammoProductionCost[ammoType][1] * num, ammoProductionCost[ammoType][2] * num );
   Resources avail = getContainer()->getResource(needed, true, 1, context.actingPlayer->getPosition() );
   
   int producable = num;
   for ( int r = 0; r < 3; ++r )
      if ( ammoProductionCost[ammoType][r] )
         producable = min ( producable, avail.resource(r) / ammoProductionCost[ammoType][r]);
   
   Resources consumed ( ammoProductionCost[ammoType][0] * producable, ammoProductionCost[ammoType][1] * producable, ammoProductionCost[ammoType][2] * producable );
   
   if ( !queryOnly ) {
      ActionResult res = (new ConsumeResource( getContainer(), consumed))->execute( context );
      if ( !res.successful() )
         throw res;
   }
   
   return producable;
}

ActionResult ConsumeAmmo::runAction( const Context& context )
{
   ContainerBase* c = getContainer();
   
   Vehicle* veh = dynamic_cast<Vehicle*>(c);
   if ( veh && slot >= 0 ) {
      if ( count >= 0 ) {
         int toProduce = 0;
         if ( veh->ammo[slot] < count ) {
            toProduce = count - veh->ammo[slot];
            if( produce( toProduce, context, true ) < toProduce )
               return ActionResult( 21100, veh);
         }
         
         veh->ammo[slot] -= count - toProduce;
         if ( toProduce )
            produce( toProduce, context , false );
         
         produced = toProduce;
         
         resultingAmmount = veh->ammo[slot];
         return ActionResult(0);
      } else {
         if ( veh->ammo[slot] - count > veh->typ->weapons.weapon[slot].count )
            return ActionResult(21104);
         
         veh->ammo[slot] -= count;
         resultingAmmount = veh->ammo[slot];
         return ActionResult(0);
      }
   } else {
      if ( count >= 0 ) {
         int toProduce = 0;
         int gettable = c->getAmmo(ammoType,count, true);
         if ( gettable < count ) {
            toProduce = count - gettable;
            if( produce( toProduce, context, true ) < toProduce )
               return ActionResult( 21100, c);
         }
         
         c->getAmmo( ammoType, gettable, false );
         if ( toProduce )
            produce( toProduce, context , false );
         
         produced = toProduce;
         
         resultingAmmount = c->getAmmo(ammoType, maxint, true );
         return ActionResult(0);
      } else {
         c->putAmmo(ammoType, -count, false );
         resultingAmmount = c->getAmmo(ammoType, maxint, true );
         return ActionResult(0);
      }
   }
}


ActionResult ConsumeAmmo::undoAction( const Context& context )
{
   ContainerBase* c = getContainer();
   Vehicle* veh = dynamic_cast<Vehicle*>(c);
   if ( veh && slot >= 0 ) {
      if ( veh->ammo[slot] + count > veh->typ->weapons.weapon[slot].count )
         return ActionResult( 21101, veh);
   
      veh->ammo[slot] += (count - produced);
      return ActionResult(0);
   } else {
      if ( count >= 0 )
         c->putAmmo( ammoType, count - produced, false );
      else
         c->getAmmo( ammoType, -count + produced, false );
      return ActionResult(0);
   }
}

ActionResult ConsumeAmmo::postCheck()
{
   ContainerBase* c = getContainer();
   Vehicle* veh = dynamic_cast<Vehicle*>(c);
   if ( veh && slot >= 0 ) {
      if ( veh->ammo[slot] != resultingAmmount )
         return ActionResult( 21102, veh );  
   }
   
   return ActionResult(0);
}


namespace {
   const bool r1 = registerAction<ConsumeAmmo> ( ActionRegistry::ConsumeAmmo );
}
