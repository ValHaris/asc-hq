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


#include "changeunitproperty.h"

#include "../vehicle.h"
#include "../gamemap.h"
     
ChangeUnitProperty::ChangeUnitProperty( GameMap* gamemap, int vehicleID, Property property, int value, bool valueIsAbsolute )
   : UnitAction( gamemap, vehicleID ), originalValue(-1), resultingValue(-1)
{
   this->property = property;
   this->value = value;
   this->valueIsAbsolute = valueIsAbsolute;
}
      
ASCString ChangeUnitProperty::getPropertyName( Property property )
{
   switch ( property ) {
      case Experience: return "Experience";
      case Movement: return "Movement";
      case AttackedFlag: return "AttackedFlag";
   };
   return "";
}
      
      
ASCString ChangeUnitProperty::getDescription() const
{
   ASCString res = "Change property " + getPropertyName(property);
   if ( getUnit(false) ) 
      res += " of unit " + getUnit(false)->getName();
   
   if ( !valueIsAbsolute )
      res += " by ";
   else 
      res += " to ";
   res += ASCString::toString(value);
   return  res;
}
      
      
void ChangeUnitProperty::readData ( tnstream& stream ) 
{
   UnitAction::readData( stream );
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "ChangeUnitMovement", 1, version );
   
   property = (Property) stream.readInt();
   value = stream.readInt();
   valueIsAbsolute = stream.readInt();
   originalValue = stream.readInt();
   resultingValue = stream.readInt();
   
};
      
      
void ChangeUnitProperty::writeData ( tnstream& stream )
{
   UnitAction::readData( stream );
   stream.writeInt( 1 );
   stream.writeInt( property );
   stream.writeInt( valueIsAbsolute );
   stream.writeInt( originalValue );
   stream.writeInt( resultingValue );
};


GameActionID ChangeUnitProperty::getID()
{
   return ActionRegistry::ChangeUnitProperty;
}

int ChangeUnitProperty::getUnitProperty()
{
   switch ( property ) {
      case Experience: return getUnit()->experience;
      case Movement:   return getUnit()->getMovement( false, false );
      case AttackedFlag: return getUnit()->attacked;
   };
   throw ActionResult(21203, getUnit() );
}

void ChangeUnitProperty::setUnitProperty( int value )
{
   switch ( property ) {
      case Experience: 
         getUnit()->experience = value;
         break;
      case Movement:
         getUnit()->setMovement( value );
         break;
      case AttackedFlag: 
         getUnit()->attacked = (value != 0) ;
         break;
      default:
         throw ActionResult(21203, getUnit() );
   };
}


ActionResult ChangeUnitProperty::runAction( const Context& context )
{
   originalValue = getUnitProperty();
   
   if ( !valueIsAbsolute )
      setUnitProperty( originalValue + value );
   else
      setUnitProperty( value );
   
   resultingValue = getUnitProperty();
   return ActionResult(0);
}


ActionResult ChangeUnitProperty::undoAction( const Context& context )
{
   setUnitProperty ( originalValue );
   return ActionResult(0);
}

ActionResult ChangeUnitProperty::preCheck()
{
   if ( getUnitProperty() != originalValue )
      throw ActionResult( 21204, getUnit(), getPropertyName( property ) );
   
   return ActionResult(0);
}

ActionResult ChangeUnitProperty::postCheck()
{
   if ( getUnitProperty() != resultingValue )
      throw ActionResult( 21204, getUnit(), getPropertyName( property ) );
   
   return ActionResult(0);
}


