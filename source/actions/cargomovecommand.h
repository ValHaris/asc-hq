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


#ifndef CargoMoveCommandH
#define CargoMoveCommandH

#include "unitcommand.h"

/** Cargo Move is moving a unit between different carriers that are nested */ 
class CargoMoveCommand : public UnitCommand {
   
   public:
      static bool moveOutAvail( const Vehicle* movingUnit  );
      static bool moveInAvail( const Vehicle* movingUnit , Vehicle* newCarrier );
      enum Mode { undefined, moveInwards, moveOutwards };
   private:
      Mode mode;
      int targetCarrier;
      
      CargoMoveCommand( GameMap* map ) : UnitCommand( map ), mode( undefined ) {};
      template<class Child> friend GameAction* GameActionCreator( GameMap* map);
      
   protected:
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream ) const;
      
      GameActionID getID() const;
      ASCString getDescription() const;
      
   public:
      CargoMoveCommand ( Vehicle* unit );
      
      void setMode( Mode mode );
      ActionResult go ( const Context& context ); 
      ASCString getCommandString() const;
      
      void setTargetCarrier( Vehicle* targetCarrier );
      
      vector<Vehicle*> getTargetCarriers();
};

#endif

