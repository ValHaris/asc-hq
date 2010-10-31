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


#ifndef ConstructBuildingCommandH
#define ConstructBuildingCommandH

#include "unitcommand.h"

#include "../typen.h"
#include "../objects.h"
#include "../mapfield.h"


class ConstructBuildingCommand : public UnitCommand {
   
   public:
      static bool avail ( const Vehicle* eht );
   private:
      MapCoordinate target;
      int buildingTypeID;
      
      void fieldChecker( const MapCoordinate& pos );
      
      ConstructBuildingCommand( GameMap* map ) : UnitCommand( map ) {};
      template<class Child> friend GameAction* GameActionCreator( GameMap* map);
      
      Resources getProductionCost( const BuildingType* bld ) const;
      
   protected:
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream ) const;
      
      GameActionID getID() const;
      ASCString getDescription() const;
      
      bool buildingFits( const MapCoordinate& entry );
      
   public:
      ConstructBuildingCommand ( Vehicle* unit );
      
      ActionResult go ( const Context& context ); 
      ASCString getCommandString() const;
      
      
      class Lack {
          int value;
         public:
            Lack() : value(0) {};
            Lack( int value ) { this->value = value; };
            enum Prerequisites { Energy = 1, Material = 2, Fuel = 4, Research = 8, Level = 0x10 };
            bool ok() const { return value == 0 ; };
            int getValue() const { return value; };
      };
      
      class ProductionEntry {
         public:
            const BuildingType* type;
            Resources cost;
            ConstructBuildingCommand::Lack prerequisites;
            ProductionEntry() : type(NULL) {};
            ProductionEntry(const BuildingType* type, const Resources& cost, ConstructBuildingCommand::Lack prerequisites ) {
               this->type = type;
               this->cost = cost;
               this->prerequisites = prerequisites;
            }
      };
      
      typedef vector<ConstructBuildingCommand::ProductionEntry> Producables;
      Lack buildingProductionPrerequisites( const BuildingType* type ) const;
      Producables getProduceableBuildings();
      
      
      
      void setBuildingType( const BuildingType* type );
      
      
      vector<MapCoordinate> getFields();
      bool isFieldUsable( const MapCoordinate& pos );
      
      Building* getProducedBuilding();
      
      void setTargetPosition( const MapCoordinate& pos );
};

#endif

