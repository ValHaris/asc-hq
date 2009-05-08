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


#include "removeobject.h"
#include "action-registry.h"

#include "../vehicle.h"
#include "../gamemap.h"
     
RemoveObject::RemoveObject( GameMap* gamemap, const MapCoordinate& position, int objectID )
   : GameAction( gamemap ), pos(position), objectBuffer(NULL)
{
   this->objectID = objectID;
}
      
      
ASCString RemoveObject::getDescription() const
{
   ASCString res = "Remove object with id " + ASCString::toString(objectID) + " at " + pos.toString(false);
   return  res;
}
      
      
void RemoveObject::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "RemoveObject", 1, version );
   
   objectID = stream.readInt();
   pos.read( stream );
   
   if ( stream.readInt() ) {
      objectBuffer = new tmemorystreambuf();
      objectBuffer->readfromstream( &stream );  
   } else
      objectBuffer = NULL;
   
};
      
      
void RemoveObject::writeData ( tnstream& stream ) const
{
   stream.writeInt( 1 );
   stream.writeInt( objectID );
   pos.write( stream );
   
   if ( objectBuffer ) {
      stream.writeInt( 1 );
      objectBuffer->writetostream( &stream );
   } else
      stream.writeInt( 0 );
   
};


GameActionID RemoveObject::getID() const
{
   return ActionRegistry::RemoveObject;
}

ActionResult RemoveObject::runAction( const Context& context )
{
   tfield* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   ObjectType* ot = getMap()->getobjecttype_byid( objectID );
   if ( !ot )
      return ActionResult( 21501 );
   
   Object* o = fld->checkforobject( ot );
   if ( !o )
      return ActionResult( 21502 );
   
   objectBuffer = new tmemorystreambuf();
   tmemorystream memstream( objectBuffer, tnstream::writing );
   o->write( memstream );
   
   if ( fld->removeobject( ot, true ) )
      return ActionResult(0);
   else
      return ActionResult(21508);
}


ActionResult RemoveObject::undoAction( const Context& context )
{
   tfield* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   
   ObjectType* ot = getMap()->getobjecttype_byid( objectID );
   if ( !ot )
      return ActionResult( 21501 );
   
   fld->addobject( ot, -1, true );
   Object* o = fld->checkforobject( ot );
   if ( !o )
      return ActionResult( 21502 );
   
   tmemorystream memstream( objectBuffer, tnstream::reading );
   o->read( memstream );
   
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

