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


#include "inflictunitdamage.h"

#include "../vehicle.h"
#include "../gamemap.h"

#include "destructunit.h"
     
InflictUnitDamage::InflictUnitDamage( GameMap* gamemap, int vehicleID, int damage )
   : GameAction( gamemap )
{
   this->vehicleID = vehicleID;
   this->damage = damage;
   this->originalDamage = -1;
   this->resultingDamage = -1;
}
      
      
ASCString InflictUnitDamage::getDescription() const
{
   ASCString res = "Inflict " + ASCString::toString(damage);
   const Vehicle* veh = getMap()->getUnit( vehicleID );
   if ( veh ) 
      res += " to unit " + veh->getName();
   return  res;
}
      
      
void InflictUnitDamage::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "InflictUnitDamage", 1, version );
   
   vehicleID = stream.readInt();
   damage = stream.readInt();
   originalDamage = stream.readInt();
   resultingDamage = stream.readInt();
};
      
      
void InflictUnitDamage::writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   stream.writeInt( vehicleID );
   stream.writeInt( damage );
   stream.writeInt( originalDamage );
   stream.writeInt( resultingDamage );
};


GameActionID InflictUnitDamage::getID() const
{
   return ActionRegistry::InflictUnitDamage;
}

ActionResult InflictUnitDamage::runAction( const Context& context )
{
   Vehicle* veh = getMap()->getUnit( vehicleID );
   if ( veh == NULL )
      return ActionResult( 21001, veh);
   
   originalDamage = veh->damage;
   veh->damage += damage;
   if ( veh->damage >= 100 ) 
      veh->damage = 100;
   
   resultingDamage = veh->damage;
   if ( veh->damage >= 100 ) {
      GameAction* a = new DestructUnit( getMap(), veh->networkid );
      ActionResult r = a->execute( context );
      if ( !r.successful() )
         return r;
   }
   return ActionResult(0);
}


ActionResult InflictUnitDamage::undoAction( const Context& context )
{
   Vehicle* veh = getMap()->getUnit( vehicleID );
   if ( veh == NULL )
      return ActionResult( 21001, veh);
   
   if ( veh->damage < damage )
      return ActionResult( 21201, veh);
   
   veh->damage -= damage;
   return ActionResult(0);
}

ActionResult InflictUnitDamage::postCheck()
{
   Vehicle* veh = getMap()->getUnit( vehicleID );
   if ( veh == NULL )
      return ActionResult( 21001, veh);
   
   if ( veh->damage != resultingDamage )
      return ActionResult( 21201, veh );  
   
   return ActionResult(0);
}



namespace {
   const bool r1 = registerAction<InflictUnitDamage> ( ActionRegistry::InflictUnitDamage );
}
