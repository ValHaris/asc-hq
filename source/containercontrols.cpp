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
#include "unitctrl.h"
#include "gamemap.h"
#include "replay.h"
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
      if ( !getPlayer().research.vehicletypeavailable ( type ) && getMap()->getgameparameter( cgp_produceOnlyResearchedStuffInternally ) ) 
         l |= 1 << 10;
   
      if ( !container->vehicleUnloadable( type ) && !container->baseType->hasFunction( ContainerBaseType::ProduceNonLeavableUnits ))
         l |= 1 << 11;
   } else {
      if ( !getPlayer().research.vehicletypeavailable ( type ) && getMap()->getgameparameter( cgp_produceOnlyResearchedStuffExternally ) ) 
         l |= 1 << 10;
   }
      
   return l;
}


Vehicle* ContainerControls::_produceUnit( const Vehicletype* type, bool fillWithAmmo, bool fillWithResources )
{
   if ( !unitProductionAvailable() )
      return NULL;
   
   if ( unitProductionPrerequisites( type, true ))
      return NULL;

   
   Vehicle* vehicle = new Vehicle ( type, getMap(), getPlayerNum() );
  
   logtoreplayinfo ( rpl_produceunit, type->id , getPlayerNum() * 8, container->getPosition().x, container->getPosition().y, 0, vehicle->networkid );

   vehicle->setnewposition( container->getPosition() );

   if ( getMap()->getgameparameter(cgp_bi3_training) >= 1 ) {
      int cnt = 0;

      for ( Player::BuildingList::iterator bi = actmap->player[actmap->actplayer].buildingList.begin(); bi != actmap->player[actmap->actplayer].buildingList.end(); bi++ )
         if ( (*bi)->typ->hasFunction( ContainerBaseType::TrainingCenter  ) )
            cnt++;

      vehicle->experience += cnt * actmap->getgameparameter(cgp_bi3_training);
      if ( vehicle->experience > maxunitexperience )
         vehicle->experience = maxunitexperience;
   }

   container->getResource( container->getProductionCost( type ), false );

   container->addToCargo( vehicle );

   vehicle->setMovement(0);
   vehicle->setAttacked();
   
   if ( fillWithAmmo ) 
      refillAmmo( vehicle );
   
   if( fillWithResources )
      refillResources( vehicle );


   return vehicle;
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

      for ( Player::BuildingList::iterator bi = actmap->player[actmap->actplayer].buildingList.begin(); bi != actmap->player[actmap->actplayer].buildingList.end(); bi++ )
         if ( (*bi)->typ->hasFunction( ContainerBaseType::TrainingCenter  ) )
            cnt++;

      vehicle->experience += cnt * actmap->getgameparameter(cgp_bi3_training);
      if ( vehicle->experience > maxunitexperience )
         vehicle->experience = maxunitexperience;
   }
   return vehicle;
}



VehicleMovement*   ContainerControls :: movement (  Vehicle* eht, bool simpleMode )
{
   VehicleMovement* vehicleMovement = new VehicleMovement ( &getDefaultMapDisplay(), NULL );
   int mode = 0;
   if ( simpleMode )
      mode |= VehicleMovement::DisableHeightChange;

   int status = vehicleMovement->execute ( eht, -1, -1, 0, -1, mode );

   if ( status > 0 )
      return vehicleMovement;
   else {
      delete vehicleMovement;
      return NULL;
   }
}




int  ContainerControls :: ammoProducable ( int weaptype, int num )
{
   if ( container->baseType->hasFunction( ContainerBaseType::AmmoProduction ) ) {
      Resources needed = Resources( ammoProductionCost[weaptype][0] * num, ammoProductionCost[weaptype][1] * num, ammoProductionCost[weaptype][2] * num );
      Resources avail = container->getResource( needed, true );

      int perc = 100;
      for ( int r = 0; r < 3; ++r )
         if ( needed.resource(r) > 0 )
          perc = min ( 100 * avail.resource(r) / needed.resource(r), perc);

      if ( perc < 0 )
         perc = 0;

      return num * perc / 100;
   } else {
      return 0;
   }
}


int ContainerControls ::  produceAmmo ( int weaptype, int num )
{
   int n = ammoProducable( weaptype, num );
   container->getResource ( Resources( ammoProductionCost[weaptype][0] * n, ammoProductionCost[weaptype][1] * n, ammoProductionCost[weaptype][2] * n ), false );
   container->putAmmo ( weaptype, n, false );

   logtoreplayinfo ( rpl_produceAmmo, container->getPosition().x, container->getPosition().y, weaptype, n );
   return n;
}



int    ContainerControls :: getammunition ( int weapontype, int num, bool abbuchen, bool produceifrequired )
{
   int got = container->getAmmo( weapontype, num, !abbuchen );
   if ( got < num && container->baseType->hasFunction( ContainerBaseType::AmmoProduction ) && produceifrequired ) {
      if ( abbuchen ) {
         produceAmmo ( weapontype, num - got );
         got += container->getAmmo( weapontype, num - got, !abbuchen );
      } else
         got += ammoProducable( weapontype, num - got );
   }
   return got;
}

bool   ContainerControls :: ammotypeavail ( int type )
{
   return true;
}



void  ContainerControls :: fillResource (Vehicle* eht, int resourcetype, int newamount)
{
   int oldamount = eht->getResource(maxint, resourcetype, true);
   int storable = eht->putResource(newamount - oldamount, resourcetype, true);

   eht->putResource( container->getResource ( storable, resourcetype, false ), resourcetype, false );

   logtoreplayinfo ( rpl_refuel2, eht->xpos, eht->ypos, eht->networkid, int(1000+resourcetype), eht->getTank().resource(resourcetype), oldamount );
   logtoreplayinfo ( rpl_refuel3, container->getIdentification(), int(1000+resourcetype), eht->getTank().resource(resourcetype) - oldamount );
};



void  ContainerControls :: fillAmmo (Vehicle* eht, int weapon, int newa )
{
   if ( eht->typ->weapons.weapon[ weapon ].requiresAmmo() ) {
      if ( newa > eht->typ->weapons.weapon[ weapon ].count )
         newa = eht->typ->weapons.weapon[ weapon ].count;

      if ( newa > eht->ammo[weapon] )
         eht->ammo[weapon]  +=  getammunition ( eht->typ->weapons.weapon[ weapon ].getScalarWeaponType() , newa - eht->ammo[weapon], 1, true ); // CGameOptions::Instance()->container.autoproduceammunition
      else {
         container->putAmmo ( eht->typ->weapons.weapon[ weapon ].getScalarWeaponType() , eht->ammo[weapon]  - newa, 1 );
         eht->ammo[weapon] = newa;
      }
   }
   logtoreplayinfo ( rpl_refuel, eht->xpos, eht->ypos, eht->networkid, weapon, newa );

};



void ContainerControls :: refillAmmo ( Vehicle* eht )
{
   for (int i = 0; i < eht->typ->weapons.count; i++)
      if ( eht->typ->weapons.weapon[ i ].requiresAmmo() )
         fillAmmo ( eht, i, maxint );
}

void ContainerControls :: refillResources ( Vehicle* veh )
{
   fillResource ( veh, 1, maxint );
   fillResource ( veh, 2, maxint );
}

void  ContainerControls :: refilleverything ( Vehicle* eht )
{
   refillResources( eht );
   refillAmmo( eht );
}

void  ContainerControls :: emptyeverything ( Vehicle* eht )
{
   fillResource ( eht, 1, 0 );
   fillResource ( eht, 2, 0 );
   for (int i = 0; i < eht->typ->weapons.count; i++)
      if ( eht->typ->weapons.weapon[ i ].requiresAmmo() )
         fillAmmo ( eht, i, 0 );
/*
   for ( int i = 0; i < 32; i++ )
   if ( eht->loading[i] )
   emptyeverything ( eht->loading[i] );
*/         
}



Resources ContainerControls :: calcDestructionOutput( Vehicle* veh )
{
    int   output;
    if ( container->baseType->hasFunction( ContainerBaseType::RecycleUnits ) )
       output = recyclingoutput;
    else
       output = destructoutput;
   
    Resources res;

    for ( ContainerBase::Cargo::const_iterator i = veh->getCargo().begin(); i != veh->getCargo().end(); i++)
       if ( *i )
          res += calcDestructionOutput ( *i );
   
    res.material += veh->typ->productionCost.material * (100 - veh->damage/2 ) * output / (100*100);
    return res;
}


void ContainerControls :: destructUnit( Vehicle* veh )
{
   Resources res = calcDestructionOutput ( veh );
   emptyeverything ( veh );

   logtoreplayinfo ( rpl_recycleUnit, container->getIdentification(), veh->networkid );

   container->putResource ( res.material, Resources::Material, false );
   container->removeUnitFromCargo( veh );
   delete veh;
}



bool ContainerControls::unitTrainingAvailable( Vehicle* veh )
{
   GameMap* actmap = container->getMap();
   if ( actmap->getgameparameter( cgp_bi3_training ) )
      return false;

   if ( veh->experience < actmap->getgameparameter ( cgp_maxtrainingexperience ) )
      if ( !veh->attacked ) 
         if (  container->baseType->hasFunction( ContainerBaseType::TrainingCenter )) {
            int num = 0;
            int numsh = 0;
            for (int i = 0; i < veh->typ->weapons.count; i++ )
               if ( veh->typ->weapons.weapon[i].shootable() ) {
                  if ( veh->ammo[i] )
                     numsh++;
                  else
                     num++;
               }
            if ( num == 0  &&  numsh > 0 )
               return true;
         }
         
   return false;
}


void ContainerControls::trainUnit( Vehicle* veh )
{
   GameMap* actmap = container->getMap();
   if ( unitTrainingAvailable ( veh ) ) {
      veh->experience+= actmap->getgameparameter( cgp_trainingIncrement );
      for (int i = 0; i < veh->typ->weapons.count; i++ )
         if ( veh->typ->weapons.weapon[i].shootable() )
            veh->ammo[i]--;

      if ( veh->experience > actmap->getgameparameter ( cgp_maxtrainingexperience ) )
         veh->experience = actmap->getgameparameter ( cgp_maxtrainingexperience );

      veh->attacked = 1;
      veh->setMovement ( 0 );
      logtoreplayinfo ( rpl_trainunit, container->getPosition().x, container->getPosition().y, veh->experience, veh->networkid );
   }
};


Resources ContainerControls :: buildProductionLineResourcesNeeded( const Vehicletype* veh )
{
   return veh->productionCost * productionLineConstructionCostFactor;
}


int ContainerControls :: buildProductionLine ( const Vehicletype* veh  )
{
   if ( container->baseType->hasFunction(ContainerBaseType::NoProductionCustomization))
      return -505;
   
   if ( find( container->getProduction().begin(), container->getProduction().end(), veh ) != container->getProduction().end() )
      return -503;
   
   if ( container->getResource( buildProductionLineResourcesNeeded(veh), 1 ) < buildProductionLineResourcesNeeded(veh))
      return -500;

   container->getResource( buildProductionLineResourcesNeeded(veh), 0 );
   container->addProductionLine( veh );

   logtoreplayinfo ( rpl_buildProdLine, container->getIdentification(), veh->id );

   return 0;
}


Resources ContainerControls :: removeProductionLineResourcesNeeded( const Vehicletype* veh )
{
   return veh->productionCost * productionLineRemovalCostFactor;
}


int ContainerControls :: removeProductionLine ( const Vehicletype* veh  )
{
   if ( container->baseType->hasFunction(ContainerBaseType::NoProductionCustomization))
      return -505;
   
   if ( find( container->getProduction().begin(), container->getProduction().end(), veh ) == container->getProduction().end() )
      return -502;
   
   if ( container->getResource( removeProductionLineResourcesNeeded(veh), 1 ) < removeProductionLineResourcesNeeded(veh))
      return -500;

   container->getResource( removeProductionLineResourcesNeeded(veh), 0 );
   container->deleteProductionLine( veh );

   logtoreplayinfo( rpl_removeProdLine, container->getIdentification(), veh->id );
   return 0;
}


vector<const Vehicletype*> ContainerControls :: productionLinesBuyable()
{

   vector<const Vehicletype*>  list;
   
   if ( container->baseType->hasFunction(ContainerBaseType::NoProductionCustomization))
      return list;

   Resources r = container->getResource( Resources(maxint, maxint, maxint), 1 );
   
   for ( int i = 0; i < vehicleTypeRepository.getNum(); ++i ) {
      Vehicletype* veh = actmap->getvehicletype_bypos ( i );
      if ( veh ) {
         bool found = find( container->getProduction().begin(), container->getProduction().end(), veh ) != container->getProduction().end();
         if ( container->baseType->vehicleFit ( veh ) && !found )
            if ( container->vehicleUnloadable(veh) || container->baseType->hasFunction( ContainerBaseType::ProduceNonLeavableUnits ))
               if ( veh->techDependency.available ( container->getMap()->getCurrentPlayer().research ))
                  if ( container->baseType->vehicleCategoriesProduceable & (1 << veh->movemalustyp))
                     list.push_back( veh );
      }
   }
   return list;
}



bool ContainerControls :: moveUnitUpAvail( const Vehicle* veh )
{
   ContainerBase* carr = veh->getCarrier();
   if ( carr ) {
      ContainerBase* carr2 = carr->getCarrier();
      if ( carr2 )
         return carr2->vehicleFit( veh );
   }
   return false;
}

bool ContainerControls :: moveUnitUp( Vehicle* veh )
{
   if ( !veh )
      return false;
   
   if ( !moveUnitUpAvail( veh ) )
      return false;
   
   ContainerBase* source = veh->getCarrier();
   if ( source ) {
      ContainerBase* target = source->getCarrier();
      if ( target  ) {
         source->removeUnitFromCargo( veh, false );
         target->addToCargo( veh );
         
         logtoreplayinfo ( rpl_moveUnitUp, target->getPosition().x, target->getPosition().y, veh->networkid );
         return true;
      }
   }
   return false;
}


bool ContainerControls::moveUnitDownAvail( const Vehicle* movingUnit )
{
   return moveUnitDownTargets( movingUnit ).size() > 0;
}

bool ContainerControls::moveUnitDownAvail( const Vehicle* movingUnit, const Vehicle* newTransport )
{
   return newTransport->vehicleFit( movingUnit );  
}


vector<Vehicle*> ContainerControls::moveUnitDownTargets( const Vehicle* movingUnit )
{
   vector<Vehicle*> targets;
   
   if ( !container )
      return targets;
   
   for ( ContainerBase::Cargo::const_iterator i = container->getCargo().begin(); i != container->getCargo().end(); ++i )
      if ( *i != movingUnit && *i )
         if ( moveUnitDownAvail ( movingUnit, *i ))
            targets.push_back( *i );

   return targets;
}


bool ContainerControls::moveUnitDown( Vehicle* veh, Vehicle* newTransport )
{
   if ( !container || !veh || !newTransport )
      return false;

   if ( !newTransport->vehicleFit( veh ))
      return false;

   container->removeUnitFromCargo( veh );
   newTransport->addToCargo( veh );
   
   logtoreplayinfo ( rpl_moveUnitUpDown, container->getPosition().x, container->getPosition().y, 0, newTransport->networkid, veh->networkid );
   return true;
}
