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


#include "removeobject.h"
#include "action-registry.h"

#include "../vehicle.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
     
RemoveObject::RemoveObject( GameMap* gamemap, const MapCoordinate& position, int objectID )
   : GameAction( gamemap ), pos(position), objectBuffer(NULL)
{
   this->objectID = objectID;
   additionalObjectCount = 0;
}
      
      
ASCString RemoveObject::getDescription() const
{
   ASCString res = "Remove object with id " + ASCString::toString(objectID) + " at " + pos.toString(false);
   return  res;
}
      
      
static const int removeObjectStreamVersion = 2;      
      
void RemoveObject::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version < 1 || version > removeObjectStreamVersion )
      throw tinvalidversion ( "RemoveObject", removeObjectStreamVersion, version );
   
   objectID = stream.readInt();
   pos.read( stream );
   
   if ( stream.readInt() ) {
      objectBuffer = new MemoryStreamStorage();
      objectBuffer->readfromstream( &stream );  
   } else
      objectBuffer = NULL;
      
   if ( version >= 2 ) 
      additionalObjectCount = stream.readInt();
   else
      additionalObjectCount = 0;
};
      
      
void RemoveObject::writeData ( tnstream& stream ) const
{
   stream.writeInt( removeObjectStreamVersion );
   stream.writeInt( objectID );
   pos.write( stream );
   
   if ( objectBuffer ) {
      stream.writeInt( 1 );
      objectBuffer->writetostream( &stream );
   } else
      stream.writeInt( 0 );
   
   stream.writeInt( additionalObjectCount );
};


GameActionID RemoveObject::getID() const
{
   return ActionRegistry::RemoveObject;
}


class ObjectRemovalStrategy2 : public MapField::ObjectRemovalStrategy {
      tnstream& buffer;
      int counter;
   public:
      ObjectRemovalStrategy2( tnstream& objectBuffer ) 
         : buffer( objectBuffer ), counter( 0 )
      {}
      
      virtual void removeObject( MapField* fld, const ObjectType* objectType ) {
         for ( MapField::ObjectContainer::iterator o = fld->objects.begin(); o != fld->objects.end();  ) {
            if ( o->typ == objectType ) {
               buffer.writeInt( o->typ->id );
               o->write( buffer );
               ++counter;
               o = fld->objects.erase( o );
            } else
               ++o ;
         }
      };
      
      int getCounter() 
      {
         return counter;
      }
   
};


ActionResult RemoveObject::runAction( const Context& context )
{
   MapField* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   ObjectType* ot = getMap()->getobjecttype_byid( objectID );
   if ( !ot )
      return ActionResult( 21501 );
   
   Object* o = fld->checkForObject( ot );
   if ( !o )
      return ActionResult( 21502 );
   
   objectBuffer = new MemoryStreamStorage();
   MemoryStream memstream( objectBuffer, tnstream::writing );
   o->write( memstream );
   
   ObjectRemovalStrategy2 removalStrat( memstream );
   
   bool result = fld->removeObject( ot, true, &removalStrat );
   additionalObjectCount = removalStrat.getCounter();
   
   if ( result )
      return ActionResult(0);
   else
      return ActionResult(21508);
}


ActionResult RemoveObject::undoAction( const Context& context )
{
   MapField* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   
   ObjectType* ot = getMap()->getobjecttype_byid( objectID );
   if ( !ot )
      return ActionResult( 21501 );
   
   fld->addobject( ot, -1, true );
   Object* o = fld->checkForObject( ot );
   if ( !o )
      return ActionResult( 21502 );
   
   computeview( getMap() );
   
   MemoryStream memstream( objectBuffer, tnstream::reading );
   o->read( memstream );
   
   for ( int i = 0; i < additionalObjectCount; ++i ) {
      int id  = memstream.readInt();
      
      ObjectType* aot = getMap()->getobjecttype_byid( id );
      if ( !aot )
         return ActionResult( 21501 );
      
      fld->addobject( aot, -1, true );
      Object* o = fld->checkForObject( aot );
      if ( !o )
         return ActionResult( 21502 );
      
      o->read( memstream );
   }
   
   return ActionResult(0);
}

ActionResult RemoveObject::verify()
{
   return ActionResult(0);
}

RemoveObject::~RemoveObject()
{
   delete objectBuffer;  
}


namespace {
   const bool r1 = registerAction<RemoveObject> ( ActionRegistry::RemoveObject );
}

