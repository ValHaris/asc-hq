/***************************************************************************
                          containerbase.cpp  -  description
                             -------------------
    begin                : Fri Sep 29 2000
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

#include "typen.h"
#include "containerbase.h"

Resources ContainerBase :: putResource ( const Resources& res, int queryonly, int scope = 1 )
{
   Resources result;
   for ( int i = 0; i < resourceNum; i++ )
      result.resource(i) = putResource ( res.resource(i), i , queryonly, scope );
   return result;
}


Resources ContainerBase :: getResource ( const Resources& res, int queryonly, int scope = 1 )
{
   Resources result;
   for ( int i = 0; i < resourceNum; i++ )
      result.resource(i) = getResource ( res.resource(i), i , queryonly, scope );
   return result;
}



int ContainerBase :: repairItem   ( ContainerBase* item, int newDamage = 0 )
{
   if ( !canRepair() )
      return item->damage;

   Resources cost;
   newDamage = getMaxRepair ( item, newDamage, cost );
   item->damage = newDamage;
   getResource ( cost, 0 );
   return newDamage;
}

int ContainerBase :: getMaxRepair ( const ContainerBase* item )
{
   Resources res;
   return getMaxRepair ( item, 0, res );
}

int ContainerBase :: getMaxRepair ( const ContainerBase* item, int newDamage, Resources& cost  )
{
   if ( !canRepair() )
      return item->damage;

   int i;
   if ( newDamage > item->damage )
      newDamage = item->damage;

   int toRepair = item->damage - newDamage;

   Resources needed = getRepairEfficiency() * item->baseType->productionCost;

   for ( i = 0; i < resourceTypeNum; i++ )
      needed.resource(i) = needed.resource(i) * (item->damage-newDamage) / 100;

   cost = getResource ( needed, 1 );

   for ( i = 0; i < resourceTypeNum; i++ )
      if ( cost.resource(i) ) {
         int nd = toRepair * cost.resource(i) / cost.resource(i);
         if ( item->damage - nd > newDamage )
            newDamage = item->damage - nd;
      }

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
