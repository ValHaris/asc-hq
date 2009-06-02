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


#ifndef PutMineCommandH
#define PutMineCommandH

#include "unitcommand.h"

#include "../typen.h"
#include "../explosivemines.h"
#include "../mapfield.h"
#include "../objects.h"

class PutMineCommand : public UnitCommand {
   
   public:
      static bool avail ( Vehicle* eht );
   private:
      enum Mode { Build, Remove };
      
      MapCoordinate target;
      MineTypes object;
      Mode mode;
      
      bool putMines;
      bool removeMines;
      
      int weaponNum;
      
      void fieldChecker( const MapCoordinate& pos );
      
      map<MapCoordinate,vector<MineTypes> > objectsCreatable;
      vector<MapCoordinate> objectsRemovable;
      
      PutMineCommand( GameMap* map ) : UnitCommand( map ) {};
      template<class Child> friend GameAction* GameActionCreator( GameMap* map);
      
   protected:
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream ) const;
      
      GameActionID getID() const;
      ASCString getDescription() const;
      
   public:
      PutMineCommand ( Vehicle* unit );
      ActionResult searchFields();
      
      void setCreationTarget( const MapCoordinate& target, MineTypes mineType );
      void setRemovalTarget( const MapCoordinate& target );
      
      ActionResult go ( const Context& context ); 
      ASCString getCommandString() const;
      
      const vector<MineTypes>& getCreatableMines( const MapCoordinate& pos );
      bool getRemovableMines( const MapCoordinate& pos ); 
      
      vector<MapCoordinate> getFields();
};

#endif

