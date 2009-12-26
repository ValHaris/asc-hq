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
#include "action-registry.h"

#include "removeobject.h"
#include "../vehicle.h"
#include "../gamemap.h"
     
SpawnObject::SpawnObject( GameMap* gamemap, const MapCoordinate& position, int objectID, int direction )
   : GameAction( gamemap ), pos(position)
{
   this->objectID = objectID;
   this->direction = direction;
   objectLaid = false;
   
   objectAvailableBeforehand = false;
   oldObjectDirection = 0;
   objectImmediatelyDisappearsAgain = false;
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
      
static const int spawnObjectStreamVersion = 3;      
      
void SpawnObject::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version < 1 || version > spawnObjectStreamVersion )
      throw tinvalidversion ( "SpawnObject", spawnObjectStreamVersion, version );
   
   objectID = stream.readInt();
   pos.read( stream );
   direction = stream.readInt();
   objectLaid = stream.readInt();
   if ( version >= 2 ) {
      objectAvailableBeforehand = stream.readInt();
      oldObjectDirection = stream.readInt();
   } else {
      objectAvailableBeforehand = false;
      oldObjectDirection = 0;
   }
   
   if ( version >= 3 ) 
      objectImmediatelyDisappearsAgain = stream.readInt();
   else
      objectImmediatelyDisappearsAgain = false;
};
      
      
void SpawnObject::writeData ( tnstream& stream ) const
{
   stream.writeInt( spawnObjectStreamVersion );
   stream.writeInt( objectID );
   pos.write( stream );
   stream.writeInt( direction );
   stream.writeInt( objectLaid );
   stream.writeInt( objectAvailableBeforehand );
   stream.writeInt( oldObjectDirection );
   stream.writeInt( objectImmediatelyDisappearsAgain );
};


GameActionID SpawnObject::getID() const
{
   return ActionRegistry::SpawnObject;
}

class ActionObjectRemovalStrategy : public MapField::ObjectRemovalStrategy {
      const Context& context;
      const ObjectType* originalObject;
      bool immediateRemoval;
   public:
      ActionObjectRemovalStrategy( const Context& actionContext, const ObjectType* object ) 
   : context( actionContext ), originalObject( object ), immediateRemoval(false)
      {}
      
      virtual void removeObject( MapField* fld, const ObjectType* objectType ) {
         if ( objectType != originalObject )
            (new RemoveObject( fld->getMap(), fld->getPosition(), objectType->id ))->execute( context );
         else {
            for ( MapField::ObjectContainer::iterator o = fld->objects.begin(); o != fld->objects.end();  ) {
               if ( o->typ == objectType ) {
                  o = fld->objects.erase( o );
               } else
                  ++o ;
            }
            
            immediateRemoval = true;
         }
      };
   
      bool getImmediateRemoval() 
      {
         return immediateRemoval;
      }
};

ActionResult SpawnObject::runAction( const Context& context )
{
   MapField* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   const ObjectType* object = getMap()->getobjecttype_byid( objectID );
   if ( !object )
      return ActionResult( 21201, "Object id is " + ASCString::toString(objectID));
   
   Object* old = fld->checkForObject( object );
   if ( old ) {
      objectAvailableBeforehand = true;
      oldObjectDirection = old->dir;
   } else {
      objectAvailableBeforehand = false;
      oldObjectDirection = 0;
   }
   
   ActionObjectRemovalStrategy aors( context, object );
   objectLaid = fld->addobject( object, direction, false, &aors );
   
   objectImmediatelyDisappearsAgain = aors.getImmediateRemoval();
   return ActionResult(0);
}


ActionResult SpawnObject::undoAction( const Context& context )
{
   MapField* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   const ObjectType* object = getMap()->getobjecttype_byid( objectID );
   if ( !object )
      return ActionResult( 21201, "Object id is " + ASCString::toString(objectID));
   
   if ( objectAvailableBeforehand ) {
      Object* o = fld->checkForObject( object );
      if ( !o )
         return ActionResult( 21505 );
      o->dir = oldObjectDirection;
   } else
      if ( !objectImmediatelyDisappearsAgain )
         fld->removeObject( object, true );
      
   return ActionResult(0);
}

ActionResult SpawnObject::verify()
{
   return ActionResult(0);
}


namespace {
   const bool r1 = registerAction<SpawnObject> ( ActionRegistry::SpawnObject );
}

