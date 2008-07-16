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


#include "spawnobject.h"

#include "../vehicle.h"
#include "../gamemap.h"
     
SpawnObject::SpawnObject( GameMap* gamemap, const MapCoordinate& position, int objectID, int direction )
   : GameAction( gamemap ), pos(position)
{
   this->objectID = objectID;
   this->direction = direction;
   objectLaid = false;
}
      
      
ASCString SpawnObject::getDescription() const
{
   const ObjectType* object = getMap()->getobjecttype_byid( objectID );
   ASCString res ;
   if ( object )
      res = "Spawn Object of type " + object->getName();
   else
      res = "Spawn Object " ;
   res += " at " + pos.toString();
   return  res;
}
      
      
void SpawnObject::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "SpawnObject", 1, version );
   
   objectID = stream.readInt();
   pos.read( stream );
   direction = stream.readInt();
   objectLaid = stream.readInt();
};
      
      
void SpawnObject::writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   stream.writeInt( objectID );
   pos.write( stream );
   stream.writeInt( direction );
   stream.writeInt( objectLaid );
};


GameActionID SpawnObject::getID() const
{
   return ActionRegistry::SpawnObject;
}

ActionResult SpawnObject::runAction( const Context& context )
{
   tfield* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   const ObjectType* object = getMap()->getobjecttype_byid( objectID );
   if ( !object )
      return ActionResult( 21201, "Object id is " + ASCString::toString(objectID));
   
   
   objectLaid = fld->addobject( object, direction );
   
   return ActionResult(0);
}


ActionResult SpawnObject::undoAction( const Context& context )
{
   tfield* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   const ObjectType* object = getMap()->getobjecttype_byid( objectID );
   if ( !object )
      return ActionResult( 21201, "Object id is " + ASCString::toString(objectID));
   
   fld->removeobject( object );
   return ActionResult(0);
}

ActionResult SpawnObject::verify()
{
   return ActionResult(0);
}


namespace {
   const bool r1 = registerAction<SpawnObject> ( ActionRegistry::SpawnObject );
}

