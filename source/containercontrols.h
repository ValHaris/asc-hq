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
class tmap;

class ContainerControls {
      ContainerBase* container;

      tmap* getMap();
      Player& getPlayer();
      int getPlayerNum();
      
   public:
      ContainerControls( ContainerBase* cb ) : container( cb ) {};
      static VehicleMovement*   movement (  Vehicle* eht, bool simpleMode );


      bool unitProductionAvailable();
      
      //! returns 0 if the unit is producable and != 0 otherwise. THe bits in the result say why the unit is not producable
      int unitProductionPrerequisites( const Vehicletype* type );
      Vehicle* produceUnit( const Vehicletype* type, bool fillWithAmmo, bool fillWithResources );

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
      
};

#endif
