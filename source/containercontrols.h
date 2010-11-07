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
      int unitProductionPrerequisites( const VehicleType* type, bool internally ) const;

};


#endif
