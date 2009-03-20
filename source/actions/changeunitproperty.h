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


#ifndef changeUnitPropertyH
#define changeUnitPropertyH


#include "unitaction.h"
#include "action-registry.h"

/** Changes the properties of a unit. 
    This is done none-recursively, so the cargo of a unit is not affected
*/
class ChangeUnitProperty : public UnitAction {
   public:
      enum Property { Experience, Movement, AttackedFlag, Height, Direction, ReactionFire, PowerGeneration };
   private:
     
      static ASCString getPropertyName( Property property );
      int getUnitProperty();
      void setUnitProperty( Property property, int value, const Context& context);
      
      Property property;
      
      bool valueIsAbsolute;
      int value;
      
      int originalValue;
      int resultingValue;
      
      ChangeUnitProperty( GameMap* map ) : UnitAction( map ) {};
      template<class Child> friend GameAction* GameActionCreator( GameMap* map);
     
   public:
      ChangeUnitProperty( Vehicle* vehicle, Property property, int value, bool valueIsAbsolute = true );
      
      ASCString getDescription() const;
      
   protected:
      virtual GameActionID getID() const;
      
      virtual ActionResult runAction( const Context& context );
      virtual ActionResult undoAction( const Context& context );
      virtual ActionResult preCheck();
      virtual ActionResult postCheck();
      
      virtual void readData ( tnstream& stream );
      virtual void writeData ( tnstream& stream ) const;
      
};

#endif

