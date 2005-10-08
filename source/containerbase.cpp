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
#include "graphics/blitter.h"


ContainerBase ::  ContainerBase ( const ContainerBaseType* bt, pmap map, int player ) : gamemap ( map ), baseType (bt)
{
   damage = 0;
   color = player*8;
}

SigC::Signal0<void> ContainerBase :: anyContainerDestroyed;


Resources ContainerBase :: putResource ( const Resources& res, bool queryonly, int scope  )
{
   Resources result;
   for ( int i = 0; i < resourceNum; i++ )
      result.resource(i) = putResource ( res.resource(i), i , queryonly, scope );
   return result;
}


Resources ContainerBase :: getResource ( const Resources& res, bool queryonly, int scope  )
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

   for ( Cargo::const_iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i )
         ++a; 

   return a;
}

bool ContainerBase :: searchAndRemove( Vehicle* veh )
{
   if ( removeUnitFromCargo( veh ))
      return true;
   else
      for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
         if ( *i )
            if ( (*i)->searchAndRemove( veh ))
               return true;
               
   return false;               
}



int ContainerBase::cargoWeight() const
{
   int w = 0;
   for ( Cargo::const_iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i )
         w += (*i)->weight();
         
   return w;
}



const ContainerBase* ContainerBase :: findParentUnit ( const Vehicle* veh ) const
{
   for ( Cargo::const_iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i ) {
         if ( *i == veh )
            return this;
         else {
            const ContainerBase* cb = (*i)->findParentUnit( veh );
            if ( cb )
               return cb;
         }
      }
      
   return NULL;
}

bool ContainerBase::unitLoaded( int nwid )
{
   for ( Cargo::const_iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i ) {
         if ( (*i)->networkid == nwid )
            return true;
         else {
            if ( (*i)->unitLoaded( nwid ) )
               return true;
         }
      }
      
   return false;

}

Vehicle* ContainerBase :: findUnit ( int nwid ) 
{
   for ( Cargo::const_iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i ) {
         if ( (*i)->networkid == nwid )
            return *i;
         else {
            Vehicle* cb = (*i)->findUnit( nwid );
            if ( cb )
               return cb;
         }
      }
      
   return NULL;
}


 template<int pixelSize>
 class ColorTransform_UnitGray { };

 template<>
 class ColorTransform_UnitGray<1> : public ColorTransform_XLAT<1> {
    public:
       ColorTransform_UnitGray( NullParamType npt  = nullParam ) {
          setTranslationTable( *xlatpictgraytable );
       };
 };

 template<>
 class ColorTransform_UnitGray<4> : public ColorTransform_Gray<4> {
     public:
       ColorTransform_UnitGray( NullParamType npt  = nullParam ) {}
 };



void ContainerBase::paintField ( const Surface& img, Surface& dest, SPoint pos, int dir, bool shaded, int shadowDist ) const
{

    pair<const Surface*, int> dirpair = make_pair(&img, directionangle[dir]);

    int height = getHeight();
    if ( height <= chgetaucht ) {
        if ( shaded ) {
           megaBlitter< ColorTransform_UnitGray,
                        ColorMerger_AlphaMixer,
                        SourcePixelSelector_CacheRotation,
                        TargetPixelSelector_All>
                      ( img, dest, pos, nullParam,nullParam, dirpair, nullParam);
        } else {
           megaBlitter< ColorTransform_PlayerCol,
                        ColorMerger_AlphaMixer,
                        SourcePixelSelector_CacheRotation,
                        TargetPixelSelector_All>
                      ( img, dest, pos, getOwner(),nullParam, dirpair, nullParam);
        }
    } else {
        if ( height >= chfahrend ) {
           if ( shadowDist == -1 )
              if ( height >= chtieffliegend ) {
                 shadowDist = 6 * ( log2 ( height) - log2 ( chfahrend ));
              } else
                 shadowDist = 1;

           megaBlitter< ColorTransform_None,
                        ColorMerger_AlphaShadow,
                        SourcePixelSelector_CacheRotation,
                        TargetPixelSelector_All>
                      ( img, dest, SPoint(pos.x+shadowDist, pos.y+shadowDist), nullParam,nullParam, dirpair, nullParam);
        }

        if ( shaded ) {
           megaBlitter< ColorTransform_UnitGray,
                        ColorMerger_AlphaOverwrite,
                        SourcePixelSelector_CacheRotation,
                        TargetPixelSelector_All>
                      ( img, dest, pos, nullParam,nullParam, dirpair, nullParam);
        } else {
           if ( img.GetPixelFormat().BytesPerPixel() == 1 ) {
               MegaBlitter<1,gamemapPixelSize,ColorTransform_PlayerCol, ColorMerger_AlphaOverwrite, SourcePixelSelector_CacheRotation> blitter;
               blitter.setPlayer( getOwner() );
               blitter.setAngle( img, directionangle[dir] );
               blitter.blit( img, dest, pos );
           } else {
               MegaBlitter<4,gamemapPixelSize,ColorTransform_PlayerTrueCol, ColorMerger_AlphaOverwrite, SourcePixelSelector_CacheRotation> blitter;
               blitter.setColor( gamemap->player[getOwner()].getColor() );
               blitter.setAngle( img, directionangle[dir] );
               blitter.blit( img, dest, pos );
           }
        }
    }
}


void ContainerBase :: addToCargo( Vehicle* veh )
{
   for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( ! (*i) ) {
         *i = veh;
         return;
      }
         
   cargo.push_back( veh );
}

bool ContainerBase :: removeUnitFromCargo( Vehicle* veh, bool recursive )
{
   if ( !veh )
      return false;
   else   
      return removeUnitFromCargo( veh->networkid );
}

bool ContainerBase :: removeUnitFromCargo( int nwid, bool recursive )
{
   for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i ) 
      if ( *i ) {
   
         if ( (*i)->networkid == nwid ) {
            *i = NULL;
            return true;
         }
         if ( recursive )
            if ( (*i)->removeUnitFromCargo( nwid, recursive ))
               return true;
      }
      
   return false;   
}


bool ContainerBase :: vehicleFit ( const Vehicle* vehicle ) const
{

   if ( baseType->vehicleFit ( vehicle->typ )) // checks size and type
      if ( vehiclesLoaded() < min ( 32, baseType->maxLoadableUnits ) || (vehicle->color != color ) )
         if ( cargoWeight() + vehicle->weight() <= baseType->maxLoadableWeight || findParentUnit ( vehicle ) || (vehicle->color != color )) // if the unit is already  loaded, the container already bears its weight
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
               if ( i->height_rel == -100 || i->height_rel == getheightdelta ( getPosition().getNumericalHeight(), log2(uheight)  ) )
                  if ( (i->container_height & getPosition().getBitmappedHeight()) || (i->container_height == 0))
                     if ( (i->requireUnitFunction & vehicle->typ->functions) || i->requireUnitFunction == 0 )
                        if ( i->vehicleCategoriesLoadable & (1<<vehicle->typ->movemalustyp)) {
                           if ( isBuilding() ) {
                              if ( getOwner() == vehicle->getOwner())
                                 return true;
                              if ( !hasAttacked ) {
                                 if ( getOwner() == 8 )
                                    return true;
                                 if ( gamemap->getPlayer(this).diplomacy.isHostile( vehicle->getOwner())  )
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
                     if ( i->height_abs != 0 && i->height_rel != -100 ) {
                        int h = 0;
                        for ( int hh = 0; hh < 8; ++hh)
                           if ( getheightdelta(getPosition().getNumericalHeight(), hh) == i->height_rel )
                              h += 1 << hh;
                        height |= i->height_abs & h;
                     } else
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
   for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i )
         delete *i;

   if ( !gamemap->__mapDestruction ) {
      destroyed();
      anyContainerDestroyed();
   }
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
      cb->cargo.clear();

   tmemorystream stream ( &buf, tnstream::reading );
   cb->read ( stream );
}



