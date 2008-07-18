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


#include "destructunit.h"
#include "spawnobject.h"

#include "../vehicle.h"
#include "../gamemap.h"
     
DestructUnit::DestructUnit( GameMap* gamemap, int unitID )
   : GameAction( gamemap ), unitBuffer(NULL)
{
   this->unitID = unitID;
}
      
      
ASCString DestructUnit::getDescription() const
{
   return "Destruct unit with ID " + ASCString::toString(unitID);
}
      
      
void DestructUnit::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "DestructUnit", 1, version );
   
   unitID = stream.readInt();
   if ( stream.readInt() ) {
      unitBuffer = new tmemorystreambuf();
      unitBuffer->readfromstream( &stream );  
   } else
      unitBuffer = NULL;
};
      
      
void DestructUnit::writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   stream.writeInt( unitID );
   if ( unitBuffer ) {
      stream.writeInt( 1 );
      unitBuffer->writetostream( &stream );
   } else
      stream.writeInt( 0 );
};


GameActionID DestructUnit::getID() const
{
   return ActionRegistry::DestructUnit;
}

ActionResult DestructUnit::runAction( const Context& context )
{
   Vehicle* veh = getMap()->getUnit(unitID);
   if ( !veh )
      return ActionResult( 21001, "UnitID is " + ASCString::toString(unitID) );

   unitBuffer = new tmemorystreambuf();
   tmemorystream memstream( unitBuffer, tnstream::writing );
   veh->write( memstream );
   
   
   if ( !veh->typ->wreckageObject.empty() && getMap()->state != GameMap::Destruction ) {
      tfield* fld = getMap()->getField(veh->getPosition());
      if ( fld->vehicle == veh ) {
         for ( vector<int>::const_iterator i = veh->typ->wreckageObject.begin(); i != veh->typ->wreckageObject.end(); ++i ) {
            ObjectType* obj = getMap()->getobjecttype_byid( *i );
            if ( obj ) {
               GameAction* o = new SpawnObject( getMap(), veh->getPosition(), *i, veh->direction );
               o->execute( context ); 
            }
         }
      }
   }
   delete veh;
   return ActionResult(0);
}


ActionResult DestructUnit::undoAction( const Context& context )
{
   tmemorystream memstream( unitBuffer, tnstream::reading );
   Vehicle* veh = Vehicle::newFromStream( getMap(), memstream );
   getMap()->getField( veh->getPosition() )->vehicle = veh;
   veh->addview();
   
   return ActionResult(0);
}

DestructUnit::~DestructUnit()
{
   delete unitBuffer;  
}


namespace {
   const bool r1 = registerAction<DestructUnit> ( ActionRegistry::DestructUnit );
}
