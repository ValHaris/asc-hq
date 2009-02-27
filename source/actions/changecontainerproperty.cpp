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


#include "changecontainerproperty.h"

ChangeContainerProperty::ChangeContainerProperty( ContainerBase* container, Property property, int value, bool valueIsAbsolute )
   : ContainerAction( container ), originalValue(-1), resultingValue(-1)
{
   this->property = property;
   this->value = value;
   this->valueIsAbsolute = valueIsAbsolute;
}
      
ASCString ChangeContainerProperty::getPropertyName( Property property )
{
   switch ( property ) {
      case Owner: return "Owner";
      case Damage: return "Damage";
   };
   return "";
}
      
      
ASCString ChangeContainerProperty::getDescription() const
{
   ASCString res = "Change property " + getPropertyName(property);
   if ( getContainer(false) ) 
      res += " of container " + getContainer(false)->getName();
   
   if ( !valueIsAbsolute )
      res += " by ";
   else 
      res += " to ";
   res += ASCString::toString(value);
   return  res;
}
      
      
void ChangeContainerProperty::readData ( tnstream& stream ) 
{
   ContainerAction::readData( stream );
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "ChangeContainerProperty", 1, version );
   
   property = (Property) stream.readInt();
   value = stream.readInt();
   valueIsAbsolute = stream.readInt();
   originalValue = stream.readInt();
   resultingValue = stream.readInt();
   
};
      
      
void ChangeContainerProperty::writeData ( tnstream& stream ) const
{
   ContainerAction::writeData( stream );
   stream.writeInt( 1 );
   stream.writeInt( property );
   stream.writeInt( value );
   stream.writeInt( valueIsAbsolute );
   stream.writeInt( originalValue );
   stream.writeInt( resultingValue );
};


GameActionID ChangeContainerProperty::getID() const
{
   return ActionRegistry::ChangeContainerProperty;
}

int ChangeContainerProperty::getUnitProperty()
{
   
   switch ( property ) {
      case Damage: return getContainer()->damage;
      default:
         throw ActionResult(21203, getContainer() );
   };
}

void ChangeContainerProperty::setUnitProperty( int value, const Context& context )
{
   switch ( property ) {
      case Damage:
         getContainer()->damage  = value;
         break;
      default:
         throw ActionResult(21203, getContainer() );
   };
}


ActionResult ChangeContainerProperty::runAction( const Context& context )
{
   originalValue = getUnitProperty();
   
   if ( !valueIsAbsolute )
      setUnitProperty( originalValue + value, context );
   else
      setUnitProperty( value, context );
   
   resultingValue = getUnitProperty();
   return ActionResult(0);
}


ActionResult ChangeContainerProperty::undoAction( const Context& context )
{
   setUnitProperty ( originalValue, context );
   return ActionResult(0);
}

ActionResult ChangeContainerProperty::preCheck()
{
   if ( getUnitProperty() != originalValue )
      throw ActionResult( 21204, getContainer(), getPropertyName( property ) );
   
   return ActionResult(0);
}

ActionResult ChangeContainerProperty::postCheck()
{
   if ( getUnitProperty() != resultingValue )
      throw ActionResult( 21204, getContainer(), getPropertyName( property ) );
   
   return ActionResult(0);
}


namespace {
   const bool r1 = registerAction<ChangeContainerProperty> ( ActionRegistry::ChangeContainerProperty );
}
