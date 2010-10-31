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


#ifndef AttackCommandH
#define AttackCommandH

#include "unitcommand.h"

#include "../typen.h"
#include "../attack.h"

class AttackCommand : public UnitCommand {
   
   public:
      static bool avail ( Vehicle* eht );
      typedef map<MapCoordinate,AttackWeap > FieldList;
      
   private:
      MapCoordinate target;
      int targetUnitID;
      MapCoordinate targetBuilding;
      int weapon;
      
      bool kamikaze;
      
      void fieldChecker( const MapCoordinate& pos );
      
      FieldList attackableUnits;
      vector<MapCoordinate> attackableUnitsKamikaze;
      FieldList attackableBuildings;
      FieldList attackableObjects;
      
      
      AttackCommand( GameMap* map ) : UnitCommand( map ) {};
      template<class Child> friend GameAction* GameActionCreator( GameMap* map);
      
   protected:
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream ) const;
      
      GameActionID getID() const;
      ASCString getDescription() const;
      
   public:
      AttackCommand ( Vehicle* unit );
      ActionResult searchTargets();
      
      Vehicle* getAttacker() { return getUnit(); };
      
      void setTarget( const MapCoordinate& target, int weapon = -1 );
      ActionResult go ( const Context& context ); 
      ASCString getCommandString() const;
      
      const FieldList& getAttackableUnits() { return attackableUnits; };
      const FieldList& getAttackableBuildings() { return attackableBuildings; };
      const FieldList& getAttackableObjects() { return attackableObjects; };
      
};

#endif

