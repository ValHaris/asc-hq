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
#include "containercontrols.h"
#include "resourcenet.h"


ContainerBase ::  ContainerBase ( const ContainerBaseType* bt, GameMap* map, int player ) : gamemap ( map ), baseType (bt)
{
   damage = 0;
   color = player*8;
   maxresearchpoints = baseType->defaultMaxResearchpoints;
   researchpoints = min ( maxresearchpoints, baseType->nominalresearchpoints );
   maxplus = baseType->maxplus;
}

SigC::Signal1<void,ContainerBase*> ContainerBase :: anyContainerDestroyed;


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

Resources ContainerBase :: getResource ( const Resources& res ) const
{
   Resources result;
   for ( int i = 0; i < resourceNum; i++ )
      result.resource(i) = getResource ( res.resource(i), i );
   return result;
}


int ContainerBase :: repairItem   ( ContainerBase* item, int newDamage  )
{
   if ( !canRepair( item ) )
      return item->damage;

   if ( item == this ) 
      if ( damage - repairableDamage() > newDamage )
         newDamage = damage - repairableDamage();
   
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

int ContainerBase :: getMaxRepair ( const ContainerBase* item, int newDamage, Resources& cost, bool ignoreCost  )
{
   if ( !canRepair( item ) )
      return item->damage;

   int i;
   if ( newDamage > item->damage )
      newDamage = item->damage;

   if ( item == this )
      if ( damage - repairableDamage() > newDamage )
         newDamage = damage - repairableDamage();

   int toRepair = item->damage - newDamage;

   Resources maxNeeded = getRepairEfficiency() * item->baseType->productionCost;

   Resources needed;
   for ( i = 0; i < resourceTypeNum; i++ )
      needed.resource(i) = maxNeeded.resource(i) * (item->damage-newDamage) / 100;

   if ( !ignoreCost ) {
      Resources avail = getResource ( needed, 1 );
   
      for ( i = 0; i < resourceTypeNum; i++ )
         if ( needed.resource(i) ) {
            int repairable = toRepair * avail.resource(i) / needed.resource(i);
            if ( item->damage - repairable > newDamage )
               newDamage = item->damage - repairable;
         }
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


int ContainerBase::cargoWeight() const
{
   int w = 0;
   for ( Cargo::const_iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i )
         w += (*i)->weight();

   return w;
}



const ContainerBase* ContainerBase :: findParent ( const ContainerBase* veh ) const
{
   for ( Cargo::const_iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i ) {
         if ( *i == veh )
            return this;
         else {
            const ContainerBase* cb = (*i)->findParent( veh );
            if ( cb )
               return cb;
         }
      }

   return NULL;
}

ContainerBase* ContainerBase :: findParent ( const ContainerBase* veh )
{
   for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i ) {
         if ( *i == veh )
            return this;
         else {
            ContainerBase* cb = (*i)->findParent( veh );
            if ( cb )
               return cb;
         }
      }

      return NULL;
}

ContainerBase* ContainerBase :: getCarrier() const
{
   tfield* fld = getMap()->getField( getPosition() );
   if ( fld->vehicle == this )
      return NULL;
   
   if ( fld->building )
      return fld->building->findParent( this );
   
   if ( fld->vehicle )
      return fld->vehicle->findParent( this );

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
class ColorTransform_UnitGray
{ }
;

template<>
class ColorTransform_UnitGray<1> : public ColorTransform_XLAT<1>
{
   public:
      ColorTransform_UnitGray( NullParamType npt  = nullParam )
      {
         setTranslationTable( *xlatpictgraytable );
      };
};

template<>
class ColorTransform_UnitGray<4> : public ColorTransform_Gray<4>
{
   public:
      ColorTransform_UnitGray( NullParamType npt  = nullParam )
      {}
}
;


int ContainerBase::calcShadowDist( int binaryHeight )
{
   if ( binaryHeight <= 1 )
      return 0;

   if ( binaryHeight <= 3 )
      return 1;

   return 6 * ( binaryHeight - log2 ( chfahrend ));
}


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
      if ( height >= chfahrend && shadowDist ) {
         if ( shadowDist == -1 )
            shadowDist = calcShadowDist( log2( height ));

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


void ContainerBase :: clearCargo()
{
   cargo.clear();
   cargoChanged();
}


void ContainerBase :: addToCargo( Vehicle* veh )
{
   for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( ! (*i) ) {
         *i = veh;
         return;
      }

   cargo.push_back( veh );
   cargoChanged();
}

bool ContainerBase :: removeUnitFromCargo( Vehicle* veh, bool recursive )
{
   if ( !veh )
      return false;
   else {
                     if ( removeUnitFromCargo( veh->networkid )) {
         cargoChanged();
         return true;
      } else
         return false;
   }
}

bool ContainerBase :: removeUnitFromCargo( int nwid, bool recursive )
{
   for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i ) {

         if ( (*i)->networkid == nwid ) {
            *i = NULL;
            cargoChanged();
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
      if ( vehiclesLoaded() < baseType->maxLoadableUnits || (vehicle->color != color ) )
         if ( cargoWeight() + vehicle->weight() <= baseType->maxLoadableWeight || findParent ( vehicle ) || (vehicle->color != color )) // if the unit is already  loaded, the container already bears its weight
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
                     if ( vehicle->typ->hasAnyFunction(i->requiresUnitFeature) || i->requiresUnitFeature.none() )
                        if ( i->vehicleCategoriesLoadable & (1<<vehicle->typ->movemalustyp)) {
                           if ( getMap()->getPlayer(this).diplomacy.isAllied( vehicle->getOwner()) ) 
                              return true;

                           if ( isBuilding() ) {
                              if ( !hasAttacked ) {
                                 if ( getOwner() == 8 )
                                    return true;
                                 if ( gamemap->getPlayer(this).diplomacy.isHostile( vehicle->getOwner())  )
                                    if (damage >= mingebaeudeeroberungsbeschaedigung  || vehicle->typ->hasFunction( ContainerBaseType::ConquerBuildings ) )
                                       return true;
                              }
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
                  if ( vehicleType->hasAnyFunction(i->requiresUnitFeature) || i->requiresUnitFeature.none() )
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
                  if ( vehicleType->hasAnyFunction(i->requiresUnitFeature) || i->requiresUnitFeature.none() )
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
      anyContainerDestroyed( this );
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
   if ( _storeCargo ) {
      cb->clearCargo();
   }

   tmemorystream stream ( &buf, tnstream::reading );
   cb->read ( stream );
}






void ContainerBase::endOwnTurn( void )
{
#ifndef karteneditor
   if ( baseType->hasFunction( ContainerBaseType::TrainingCenter  ) ) {
      for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
         if ( *i ) {
            /*
            bool ammoFull = true;
            for ( int w = 0; w < (*i)->typ->weapons.count; ++w )
               if ( (*i)->ammo[w] < (*i)->typ->weapons.weapon[w].count )
                  ammoFull = false;

            if ( ammoFull ) {
            */
               ContainerControls cc ( this );
               if ( cc.unitTrainingAvailable( *i )) {
                  cc.trainUnit( *i );
                  cc.refillAmmo ( *i );
               }
            // }
         }
   }
#endif
}

void ContainerBase::endAnyTurn( void )
{
}

void ContainerBase::endRound ( void )
{
}



Resources ContainerBase ::netResourcePlus( ) const
{
   Resources res;
   for ( int resourcetype = 0; resourcetype < resourceTypeNum; resourcetype++ ) {
      GetResourcePlus grp ( getMap() );
      res.resource(resourcetype) += grp.getresource ( getPosition().x, getPosition().y, resourcetype, color/8, 1 );
   }
   return res;
}






Resources ContainerBase :: getResourcePlus( )
{
   Work* w = spawnWorkClasses ( true );
   Resources r;
   if ( w )
      r = w->getPlus();
   delete w;

   //  printf ("building %s %d / %d : plus %d %d %d \n", typ->name.c_str(), getPosition().x, getPosition().y, r.energy, r.material, r.fuel );

   return r;
}

Resources ContainerBase :: getResourceUsage( )
{
   Work* w = spawnWorkClasses ( true );
   Resources r;
   if ( w )
      r = w->getUsage();
   delete w;

   if ( baseType->hasFunction( ContainerBaseType::Research ) )
      r += returnResourcenUseForResearch( this, researchpoints );


   return r;
}

Resources ContainerBase::getStorageCapacity() const
{
   if ( gamemap && gamemap->_resourcemode == 1 && isBuilding() )
      return baseType->getStorageCapacity( 1 );
   else
      return baseType->getStorageCapacity( 0 );
}




bool ContainerBase::registerWorkClassFactory( WorkClassFactory* wcf, bool ASCmode )
{
   if ( ASCmode )  {
      if ( !workClassFactoriesASC )
         workClassFactoriesASC = new WorkerClassList;
      
      workClassFactoriesASC->push_back( wcf );
   } else {
      if ( !workClassFactoriesBI )
         workClassFactoriesBI = new WorkerClassList;
      
      workClassFactoriesBI->push_back( wcf );
   }
   return true;
}

ContainerBase::WorkerClassList* ContainerBase::workClassFactoriesASC = NULL;
ContainerBase::WorkerClassList* ContainerBase::workClassFactoriesBI = NULL;


ContainerBase ::Work* ContainerBase ::spawnWorkClasses( bool justQuery )
{
   if ( getMap()->_resourcemode != 1 ) {
      for ( WorkerClassList::iterator i = workClassFactoriesASC->begin(); i != workClassFactoriesASC->end(); ++i )
         if ( (*i)->available( this ) )
            return (*i)->produce(this, justQuery);
   } else {
      for ( WorkerClassList::iterator i = workClassFactoriesBI->begin(); i != workClassFactoriesBI->end(); ++i )
         if ( (*i)->available( this ) )
            return (*i)->produce(this, justQuery);
   }
   return NULL;
}



