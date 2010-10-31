/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger
 
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


#include "spawnmine.h"
#include "action-registry.h"

#include "../vehicle.h"
#include "../gamemap.h"
     
SpawnMine::SpawnMine( GameMap* gamemap, const MapCoordinate& position, MineTypes mineType, int owner, int strength )
   : GameAction( gamemap ), pos(position)
{
   this->type = mineType;
   this->owner = owner;
   this->strength = strength;
   stackPosition = -1;
   mapNetworkIdCounterAfter = -1;
   mapNetworkIdCounterBefore = -1;
}
      
      
ASCString SpawnMine::getDescription() const
{
   return "Spawn mine at position " + pos.toString();
}
      
static const int mineDataVersion = 2;      
      
void SpawnMine::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version < 1 || version > mineDataVersion )
      throw tinvalidversion ( "SpawnMine", mineDataVersion, version );
   
   type = (MineTypes) stream.readInt();
   pos.read( stream );
   owner = stream.readInt();
   strength = stream.readInt();
   stackPosition = stream.readInt();
   if ( version >= 2 ) {
      mapNetworkIdCounterBefore = stream.readInt();
      mapNetworkIdCounterAfter = stream.readInt();
   }
};
      
      
void SpawnMine::writeData ( tnstream& stream ) const
{
   stream.writeInt( mineDataVersion );
   stream.writeInt( type );
   pos.write( stream );
   stream.writeInt( owner );
   stream.writeInt( strength ); 
   stream.writeInt( stackPosition );
   stream.writeInt( mapNetworkIdCounterBefore );
   stream.writeInt( mapNetworkIdCounterAfter );
};


GameActionID SpawnMine::getID() const
{
   return ActionRegistry::SpawnMine;
}

ActionResult SpawnMine::runAction( const Context& context )
{
   MapField* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   mapNetworkIdCounterBefore = getMap()->idManager.unitnetworkid;
   
   bool result = fld->putmine( owner, type, strength );
   
   mapNetworkIdCounterAfter = getMap()->idManager.unitnetworkid;
   
   
   if ( result ) {
      stackPosition = fld->mines.size();
      return ActionResult(0);
   } else
      return ActionResult(21600);
}


ActionResult SpawnMine::undoAction( const Context& context )
{
   MapField* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   if ( fld->mines.size() != stackPosition )
      return ActionResult(21601);
   
   if ( getMap()->idManager.unitnetworkid != mapNetworkIdCounterAfter )
      return ActionResult( 21605 );
   
   getMap()->idManager.unitnetworkid = mapNetworkIdCounterBefore;
   
   fld->mines.pop_back();
   
   
   
   return ActionResult(0);
}

ActionResult SpawnMine::verify()
{
   if ( getMap()->idManager.unitnetworkid != mapNetworkIdCounterAfter )
      return ActionResult( 21605 );

   return ActionResult(0);
}


namespace {
   const bool r1 = registerAction<SpawnMine> ( ActionRegistry::SpawnMine );
}

