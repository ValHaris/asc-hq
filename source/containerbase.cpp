/***************************************************************************
                          containerbase.cpp  -  description
                             -------------------
    begin                : Fri Sep 29 2000
    copyright            : (C) 2003 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file containerbase.cpp
    \brief Implementation of methods that buildings and vehicles have in common
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>
#include "typen.h"
#include "containerbase.h"
#include "vehicletype.h"
#include "vehicle.h"
#include "spfst.h"

ContainerBase ::  ContainerBase ( const ContainerBaseType* bt, pmap map, int player ) : gamemap ( map ), baseType (bt)
{
   for ( int i = 0; i< 32; i++ )
      loading[i] = NULL;
   damage = 0;
   color = player*8;
}


Resources ContainerBase :: putResource ( const Resources& res, int queryonly, int scope  )
{
   Resources result;
   for ( int i = 0; i < resourceNum; i++ )
      result.resource(i) = putResource ( res.resource(i), i , queryonly, scope );
   return result;
}


Resources ContainerBase :: getResource ( const Resources& res, int queryonly, int scope  )
{
   Resources result;
   for ( int i = 0; i < resourceNum; i++ )
      result.resource(i) = getResource ( res.resource(i), i , queryonly, scope );
   return result;
}



int ContainerBase :: repairItem   ( ContainerBase* item, int newDamage  )
{
   if ( !canRepair( item ) )
      return item->damage;

   int orgdam = item->damage;

   Resources cost;
   newDamage = getMaxRepair ( item, newDamage, cost );
   item->damage = newDamage;
   getResource ( cost, 0 );

   item->postRepair( orgdam );

   return newDamage;
}

int ContainerBase :: getMaxRepair ( const ContainerBase* item )
{
   Resources res;
   return getMaxRepair ( item, 0, res );
}

int ContainerBase :: getMaxRepair ( const ContainerBase* item, int newDamage, Resources& cost  )
{
   if ( !canRepair( item ) )
      return item->damage;

   int i;
   if ( newDamage > item->damage )
      newDamage = item->damage;

   int toRepair = item->damage - newDamage;

   Resources maxNeeded = getRepairEfficiency() * item->baseType->productionCost;

   Resources needed;
   for ( i = 0; i < resourceTypeNum; i++ )
      needed.resource(i) = maxNeeded.resource(i) * (item->damage-newDamage) / 100;

   Resources avail = getResource ( needed, 1 );

   for ( i = 0; i < resourceTypeNum; i++ )
      if ( needed.resource(i) ) {
         int repairable = toRepair * avail.resource(i) / needed.resource(i);
         if ( item->damage - repairable > newDamage )
            newDamage = item->damage - repairable;
      }

   for ( i = 0; i < resourceTypeNum; i++ )
      cost.resource(i) = maxNeeded.resource(i) * (item->damage-newDamage) / 100;

   return newDamage;
}

int ContainerBase :: vehiclesLoaded ( void ) const
{
   int a = 0;
   /*
   if (eht->typ->loadcapacity == 0)
      return( 0 );
   */

   for ( int b = 0; b <= 31; b++)
      if ( loading[b] )
         a++;

   return a;
}

void ContainerBase :: addEventHook ( EventHook* eventHook )
{
   eventHooks.push_back ( eventHook );
}

void ContainerBase :: removeEventHook ( const EventHook* eventHook )
{
   list<EventHook*>::iterator i = find ( eventHooks.begin(), eventHooks.end(), eventHook );
   if ( i != eventHooks.end() )
      eventHooks.erase ( i );
}

int ContainerBase::cargo ( void ) const
{
   int w = 0;
   for (int c = 0; c <= 31; c++)
      if ( loading[c] )
         w += loading[c]->weight();
   return w;
}

void ContainerBase::regroupUnits ()
{
   int num = 0;
   for ( int i = 18; i < 32; i++ )
      if ( loading[i] )
         num++;

   if ( num ) {
      for ( int i = 0; i < 18; i++ )
         if ( !loading[i] ) {
            for ( int j = i+1; j < 32; j++ )
               loading[j-1] = loading[j];
            loading[31] = NULL;
         }
   }
}


const ContainerBase* ContainerBase :: findUnit ( const Vehicle* veh ) const
{
   for ( int i = 0; i < 32; i++ ) {
      if ( loading[i] == veh )
         return this;
      else
         if ( loading[i] )
            if ( loading[i]->findUnit ( veh ) )
               return loading[i];
   }
   return NULL;
}


bool ContainerBase :: vehicleFit ( const Vehicle* vehicle ) const
{

   if ( baseType->vehicleFit ( vehicle->typ )) // checks size and type
      if ( vehiclesLoaded() < min ( 32, baseType->maxLoadableUnits ) || (vehicle->color != color ) )
         if ( cargo() + vehicle->weight() <= baseType->maxLoadableWeight || findUnit ( vehicle )) // if the unit is already  loaded, the container already bears its weight
            return true;

   return false;
}


bool  ContainerBase :: vehicleLoadable ( const Vehicle* vehicle, int uheight, const bool* attacked ) const
{
   if ( vehicle->attacked )
      return false;

   bool hasAttacked = vehicle->attacked;
   if ( attacked )
      hasAttacked = *attacked;

   if ( uheight == -1 )
      uheight = vehicle->height;

   if ( vehicleFit ( vehicle ))
      for ( ContainerBaseType::EntranceSystems::const_iterator i = baseType->entranceSystems.begin(); i != baseType->entranceSystems.end(); i++ )
         if ( (i->height_abs & uheight) || (i->height_abs == 0 ))
            if ( i->mode & ContainerBaseType::TransportationIO::In )
               if ( i->height_rel == -100 || i->height_rel == getheightdelta ( log2(uheight), getPosition().getNumericalHeight() ) )
                  if ( (i->container_height & getPosition().getBitmappedHeight()) || (i->container_height == 0))
                     if ( (i->requireUnitFunction & vehicle->typ->functions) || i->requireUnitFunction == 0 )
                        if ( i->vehicleCategoriesLoadable & (1<<vehicle->typ->movemalustyp)) {
                           if ( isBuilding() ) {
                              if ( getOwner() == vehicle->getOwner())
                                 return true;
                              if ( !hasAttacked ) {
                                 if ( getOwner() == 8 )
                                    return true;
                                 if ( getdiplomaticstatus2(color, vehicle->color ) == cawar  )
                                    if (damage >= mingebaeudeeroberungsbeschaedigung  || (vehicle->typ->functions & cf_conquer) )
                                       return true;
                              }
                           } else {
                              if ( getOwner() == vehicle->getOwner() )
                                 return true;
                           }
                        }
   return false;
}

int  ContainerBase :: vehicleUnloadable ( const Vehicletype* vehicleType ) const
{
   int height = 0;

   if ( baseType->vehicleFit ( vehicleType ))
      for ( ContainerBaseType::EntranceSystems::const_iterator i = baseType->entranceSystems.begin(); i != baseType->entranceSystems.end(); i++ )
         if ( i->mode & ContainerBaseType::TransportationIO::Out )
            if ( (i->container_height & getPosition().getBitmappedHeight()) || (i->container_height == 0))
               if ( i->vehicleCategoriesLoadable & (1<<vehicleType->movemalustyp))
                  if ( (i->requireUnitFunction & vehicleType->functions) || i->requireUnitFunction == 0 )
                     if ( i->height_abs != 0 && i->height_rel != -100 )
                        height |= i->height_abs & (1 << (getPosition().getNumericalHeight() + i->height_rel ));
                     else
                       if ( i->height_rel != -100 )
                          height |= 1 << (getPosition().getNumericalHeight() + i->height_rel) ;
                       else
                          height |= i->height_abs ;
   return height & vehicleType->height;
}

const ContainerBaseType::TransportationIO* ContainerBase::vehicleUnloadSystem ( const Vehicletype* vehicleType, int height )
{
  if ( baseType->vehicleFit ( vehicleType ))
      for ( ContainerBaseType::EntranceSystems::const_iterator i = baseType->entranceSystems.begin(); i != baseType->entranceSystems.end(); i++ )
         if ( i->mode & ContainerBaseType::TransportationIO::Out )
            if ( (i->container_height & getPosition().getBitmappedHeight()) || (i->container_height == 0))
               if ( i->vehicleCategoriesLoadable & (1<<vehicleType->movemalustyp))
                  if ( (i->requireUnitFunction & vehicleType->functions) || i->requireUnitFunction == 0 )
                     if ( i->height_abs != 0 && i->height_rel != -100 ) {
                        if ( height & ( i->height_abs & (1 << (getPosition().getNumericalHeight() + i->height_rel ))))
                           return &(*i);
                     } else
                       if ( i->height_rel != -100 ) {
                          if ( height & ( 1 << (getPosition().getNumericalHeight() + i->height_rel)))
                             return &(*i);
                       } else
                          if ( height & i->height_abs )
                             return &(*i);
   return NULL;

}

int  ContainerBase :: vehicleDocking ( const Vehicle* vehicle, bool out ) const
{
   if ( vehicle == this )
      return 0;

   int height = 0;

   if ( baseType->vehicleFit ( vehicle->typ ) && ( vehicleFit( vehicle ) || out ) )
      for ( ContainerBaseType::EntranceSystems::const_iterator i = baseType->entranceSystems.begin(); i != baseType->entranceSystems.end(); i++ )
         if ( i->mode & ContainerBaseType::TransportationIO::Docking )
            if ( (i->container_height & getPosition().getBitmappedHeight()) || (i->container_height == 0))
               if ( i->vehicleCategoriesLoadable & (1<<vehicle->typ->movemalustyp))
                  if ( i->dockingHeight_abs != 0 && i->dockingHeight_rel != -100 )
                     height |= i->dockingHeight_abs & (1 << (getPosition().getNumericalHeight() + i->dockingHeight_rel ));
                  else
                    if ( i->dockingHeight_rel != -100 )
                       height |= 1 << (getPosition().getNumericalHeight() + i->dockingHeight_rel) ;
                    else
                       height |= i->dockingHeight_abs ;
   return height;
}



ContainerBase :: ~ContainerBase ( )
{
   for ( list<EventHook*>::iterator i = eventHooks.begin(); i != eventHooks.end(); i++ )
      (*i)->receiveEvent( EventHook::removal, 0 );
}


TemporaryContainerStorage :: TemporaryContainerStorage ( ContainerBase* _cb, bool storeCargo )
{
   cb = _cb;
   tmemorystream stream ( &buf, tnstream::writing );
   cb->write ( stream, storeCargo );
   _storeCargo = storeCargo;
}

void TemporaryContainerStorage :: restore (  )
{
   if ( _storeCargo )
      for ( int i = 0; i < 32; i++ )
         if ( cb->loading[i] ) {
            delete cb->loading[i];
            cb->loading[i] = NULL;
         }

   tmemorystream stream ( &buf, tnstream::reading );
   cb->read ( stream );
}

void TemporaryContainerStorage :: receiveEvent ( Events ev, int data )
{
   if ( ev == removal )
      fatalError ( " TemporaryContainerStorage::restore - unit deleted");
}



