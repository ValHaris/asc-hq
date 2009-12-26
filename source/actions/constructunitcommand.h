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


#ifndef ConstructUnitCommandH
#define ConstructUnitCommandH

#include "containercommand.h"

#include "../typen.h"
#include "../objects.h"
#include "../mapfield.h"


class ConstructUnitCommand : public ContainerCommand {
   
   public:
      static bool externalConstructionAvail( const ContainerBase* eht );
      static bool internalConstructionAvail( const ContainerBase* eht );
      static bool avail ( const ContainerBase* eht );
      enum Mode { undefined, internal, external } ;
   private:
      Mode mode;
      MapCoordinate target;
      int vehicleTypeID;
      int newUnitID;
      
      void fieldChecker( const MapCoordinate& pos );
      
      map<MapCoordinate,vector<int> > unitsConstructable;
      
      ConstructUnitCommand( GameMap* map ) : ContainerCommand( map ), mode( undefined ) {};
      template<class Child> friend GameAction* GameActionCreator( GameMap* map);
      
   protected:
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream ) const;
      
      GameActionID getID() const;
      ASCString getDescription() const;
      
   public:
      ConstructUnitCommand ( ContainerBase* unit );
      
      void setMode( Mode mode );
      ActionResult go ( const Context& context ); 
      ASCString getCommandString() const;
      
      
      class Lack {
          int value;
         public:
            Lack() : value(0) {};
            Lack( int value ) { this->value = value; };
            enum Prerequisites { Energy = 1, Material = 2, Fuel = 4, Research = 8 , Unloadability = 0x10, Movement = 0x20 };
            bool ok() const { return value == 0 ; };
            int getValue() const { return value; };
      };
      
      class ProductionEntry {
         public:
            const VehicleType* type;
            Resources cost;
            ConstructUnitCommand::Lack prerequisites;
            ProductionEntry() : type(NULL) {};
            ProductionEntry(const VehicleType* type, const Resources& cost, ConstructUnitCommand::Lack prerequisites ) {
               this->type = type;
               this->cost = cost;
               this->prerequisites = prerequisites;
            }
      };
      
      typedef vector<ConstructUnitCommand::ProductionEntry> Producables;
      
      Lack unitProductionPrerequisites( const VehicleType* type ) const;
      
      void setVehicleType( const VehicleType* type );
      
      /**
         \param internally: true for internal production (inside cargo bay), false for outside production(neighbouring field)
         \return a collection of potentially producable units. Maybe some prerequisites are lacking, which is indicated by the Lack attribute
      */
      Producables getProduceableVehicles();
      
      vector<MapCoordinate> getFields();
      bool isFieldUsable( const MapCoordinate& pos );
      
      Vehicle* getProducedUnit();
      
      void setTargetPosition( const MapCoordinate& pos );
};

#endif

