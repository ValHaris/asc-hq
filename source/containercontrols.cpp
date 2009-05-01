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

#include "containercontrols.h"
#include "gamemap.h"
#include "mapdisplayinterface.h"
#include "itemrepository.h"


const GameMap* ContainerConstControls::getMap() const
{
   return container->getMap();
}


GameMap* ContainerControls::getMap()
{
   return container->getMap();
}

const Player& ContainerConstControls::getPlayer() const
{
   return getMap()->player[ getMap()->actplayer ];
}


Player& ContainerControls::getPlayer()
{
   return getMap()->player[ getMap()->actplayer ];
}

int ContainerControls::getPlayerNum()
{
   return getMap()->actplayer;
}


bool ContainerConstControls::unitProductionAvailable() const
{
   if ( container->getOwner() == container->getMap()->actplayer )
      if ( container->vehiclesLoaded() < container->baseType->maxLoadableUnits )
         if ( container->baseType->hasFunction( ContainerBaseType::InternalVehicleProduction ))
           return true;

   return false;
}

int  ContainerConstControls::unitProductionPrerequisites( const Vehicletype* type, bool internally ) const
{
   int l = 0;
   Resources cost = container->getProductionCost( type );
   for ( int r = 0; r < resourceTypeNum; r++ )
      if ( container->getAvailableResource( cost.resource(r), r ) < cost.resource(r) )
         l |= 1 << r;
   
   if ( internally ) {
      if ( !type->techDependency.available( getPlayer().research ) && getMap()->getgameparameter( cgp_produceOnlyResearchedStuffInternally ) ) 
         l |= 1 << 10;
   
      if ( !container->vehicleUnloadable( type ) && !container->baseType->hasFunction( ContainerBaseType::ProduceNonLeavableUnits ))
         l |= 1 << 11;
   } else {
      if ( !type->techDependency.available( getPlayer().research ) && getMap()->getgameparameter( cgp_produceOnlyResearchedStuffExternally ) ) 
         l |= 1 << 10;
   }
      
   return l;
}



Vehicle* ContainerControls::produceUnitHypothetically( const Vehicletype* type )
{
   if ( !unitProductionAvailable() )
      return NULL;
   
   if ( unitProductionPrerequisites( type, true ))
      return NULL;

   
   Vehicle* vehicle = new Vehicle ( type, getMap(), getPlayerNum() );
   vehicle->setnewposition( container->getPosition() );

   if ( getMap()->getgameparameter(cgp_bi3_training) >= 1 ) {
      int cnt = 0;

      for ( Player::BuildingList::iterator bi = getMap()->player[getMap()->actplayer].buildingList.begin(); bi != getMap()->player[getMap()->actplayer].buildingList.end(); bi++ )
         if ( (*bi)->typ->hasFunction( ContainerBaseType::TrainingCenter  ) )
            cnt++;

      vehicle->experience += cnt * getMap()->getgameparameter(cgp_bi3_training);
      if ( vehicle->experience > maxunitexperience )
         vehicle->experience = maxunitexperience;
   }
   return vehicle;
}

