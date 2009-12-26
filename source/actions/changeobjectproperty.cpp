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


#include "changeobjectproperty.h"
#include "removeobject.h"
#include "action-registry.h"

#include "../gamemap.h"
     
ChangeObjectProperty::ChangeObjectProperty( GameMap* map, MapCoordinate& pos, Object* object, Property property, int value, bool valueIsAbsolute )
   : GameAction( map ), originalValue(-1), resultingValue(-1)
{
   this->position = pos;
   this->value = value;
   this->property = property;
   this->objectID = object->typ->id;
   this->valueIsAbsolute = valueIsAbsolute;
}
      
ASCString ChangeObjectProperty::getPropertyName( Property property )
{
   switch ( property ) {
      case Damage: return "Damage";
   };
   return "";
}
      
      
ASCString ChangeObjectProperty::getDescription() const
{
   ASCString res = "Change property " + getPropertyName(property) + " of object with id " + ASCString::toString(objectID) 
         + " on field " + position.toString(false);
   
   if ( !valueIsAbsolute )
      res += " by ";
   else 
      res += " to ";
   res += ASCString::toString(value);
   return  res;
}
      
      
void ChangeObjectProperty::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "ChangeObjectProperty", 1, version );
   
   position.read( stream );
   objectID = stream.readInt();
   property = (Property) stream.readInt();
   value = stream.readInt();
   valueIsAbsolute = stream.readInt();
   originalValue = stream.readInt();
   resultingValue = stream.readInt();
   
};
      
      
void ChangeObjectProperty::writeData ( tnstream& stream )  const
{
   stream.writeInt( 1 );
   position.write( stream );
   stream.writeInt( objectID );
   stream.writeInt( property );
   stream.writeInt( value );
   stream.writeInt( valueIsAbsolute );
   stream.writeInt( originalValue );
   stream.writeInt( resultingValue );
};


GameActionID ChangeObjectProperty::getID() const
{
   return ActionRegistry::ChangeObjectProperty;
}

Object* ChangeObjectProperty::getObject()
{
   MapField* fld = getMap()->getField( position );
   if ( !fld )
      throw ActionResult(21002);
   
   ObjectType* ot = getMap()->getobjecttype_byid( objectID );
   if ( !ot )
      throw ActionResult( 21501 );
   
   Object* o = fld->checkForObject( ot );
   if ( !o )
      throw ActionResult ( 21502 );
   
   return o;
}


int ChangeObjectProperty::getProperty()
{
   switch ( property ) {
      case Damage: return getObject()->damage;
   };
   throw ActionResult(21503 );
}

ActionResult ChangeObjectProperty::setProperty( Property property, int value, const Context& context )
{
   switch ( property ) {
      case Damage: 
         getObject()->damage = value;
         break;
      default:
         throw ActionResult(21503 );
   };
   return ActionResult(0);
}


ActionResult ChangeObjectProperty::runAction( const Context& context )
{
   originalValue = getProperty();
   
   ActionResult res(0);
   if ( !valueIsAbsolute )
      res = setProperty( property, originalValue + value, context );
   else
      res = setProperty( property, value, context );
   
   resultingValue = getProperty();
   return res;
}


ActionResult ChangeObjectProperty::undoAction( const Context& context )
{
   return setProperty ( property, originalValue, context );
}

ActionResult ChangeObjectProperty::preCheck()
{
   if ( getProperty() != originalValue )
      throw ActionResult( 21204, getPropertyName( property ) );
   
   return ActionResult(0);
}

ActionResult ChangeObjectProperty::postCheck()
{
   if ( getProperty() != resultingValue )
      throw ActionResult( 21204, getPropertyName( property ) );
   
   return ActionResult(0);
}


namespace {
   const bool r1 = registerAction<ChangeObjectProperty> ( ActionRegistry::ChangeObjectProperty );
}

