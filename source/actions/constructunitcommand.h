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
      enum Mode { undefined, internal, external } state;
   private:
      MapCoordinate target;
      int vehicleTypeID;
      
      void fieldChecker( const MapCoordinate& pos );
      
      map<MapCoordinate,vector<int> > unitsConstructable;
      
      ConstructUnitCommand( GameMap* map ) : ContainerCommand( map ), state( undefined ) {};
      template<class Child> friend GameAction* GameActionCreator( GameMap* map);
      
   protected:
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream ) const;
      
      GameActionID getID() const;
      ASCString getDescription() const;
      
   public:
      ConstructUnitCommand ( ContainerBase* unit );
      ActionResult searchFields();
      
      void setMode( Mode mode ) { state = mode; };
      void setTarget( const MapCoordinate& target, int vehicleTypeID );
      ActionResult go ( const Context& context ); 
      ASCString getCommandString() const;
      
      class Lack {
          int value;
         public:
            Lack() : value(0) {};
            Lack( int value ) { this->value = value; };
            bool energyLacks() { return value & 1 ; };
            bool materialLacks() { return value & 2 ; };
            bool fuelLacks() { return value & 4 ; };
            bool notResearched() { return value & 8; };
      };
      
      /**
         \param internally: true for internal production (inside cargo bay), false for outside production(neighbouring field)
         \return a collection of potentially producable units. Maybe some prerequisites are lacking, which is indicated by the Lack attribute
      */
      map<const Vehicletype*,Lack> getProduceableVehicles(bool internally );
      
      vector<MapCoordinate> getFields();
};

#endif

