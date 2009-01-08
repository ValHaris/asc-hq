/***************************************************************************
                          cargodialog.cpp  -  description
                             -------------------
    begin                : Tue Oct 24 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef containercontrolsH
#define containercontrolsH

#include "containerbase.h"

class VehicleMovement;
class Player;
class GameMap;

class ContainerConstControls {
      const ContainerBase* container;

      const Player& getPlayer() const;
      const GameMap* getMap() const;
   public:
      ContainerConstControls( const ContainerBase* cb ) : container( cb ) {};

   protected:
      bool unitProductionAvailable() const;

   public:
      /** returns 0 if the unit is producable and != 0 otherwise. THe bits in the result say why the unit is not producable
          Bit 1,2+3: energy, material and fuel
          Bit 10: not researched yet
          Bit 11: can not be unloaded
      */
      int unitProductionPrerequisites( const Vehicletype* type, bool internally ) const;

};

class ContainerControls : public ContainerConstControls {
      ContainerBase* container;

      GameMap* getMap();
      Player& getPlayer();
      int getPlayerNum();
      
   public:
      ContainerControls( ContainerBase* cb ) : ContainerConstControls( cb ), container( cb ) {};
      static VehicleMovement*   movement (  Vehicle* eht, bool simpleMode = false);

      
      Vehicle* produceUnit( const Vehicletype* type, bool fillWithAmmo, bool fillWithResources );

      Resources calcDestructionOutput( Vehicle* veh );
      void destructUnit( Vehicle* veh );

      bool unitTrainingAvailable( Vehicle* veh );
      void trainUnit( Vehicle* veh );

      vector<const Vehicletype*> productionLinesBuyable();
      
      Resources buildProductionLineResourcesNeeded( const Vehicletype* veh );
      int  buildProductionLine( const Vehicletype* veh );

      Resources removeProductionLineResourcesNeeded( const Vehicletype* veh );
      int  removeProductionLine( const Vehicletype* veh );
      
      //! only to be used by the AI, this doesn't consume any resources, so the unit must be discarded
      Vehicle* produceUnitHypothetically( const Vehicletype* type );

      void  fillResource (Vehicle* eht, int resourcetype, int newamount);
      void  fillAmmo (Vehicle* eht, int weapon, int newa );
      void refilleverything ( Vehicle* eht );
      void refillAmmo ( Vehicle* veh );
      void refillResources ( Vehicle* veh );
      void emptyeverything ( Vehicle* eht );
      int   getammunition ( int weapontype, int num, bool abbuchen, bool produceifrequired );
      bool  ammotypeavail ( int type );
      int  ammoProducable ( int weaptype, int num );
      int  produceAmmo ( int weaptype, int num );

      bool moveUnitUpAvail( const Vehicle* veh );
      bool moveUnitUp( Vehicle* veh );
      
      bool moveUnitDownAvail( const Vehicle* movingUnit );
      bool moveUnitDownAvail( const Vehicle* movingUnit, const Vehicle* newTransport );
      vector<Vehicle*> moveUnitDownTargets( const Vehicle* movingUnit );
      bool moveUnitDown( Vehicle* veh, Vehicle* newTransport );
      
};

#endif
