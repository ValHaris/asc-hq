/***************************************************************************
                          containerbase.cpp  -  description
                             -------------------
    begin                : Fri Sep 29 2000
    copyright            : (C) 2003 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file containerbase.cpp
    \brief Implementation of the common base class that that buildings and 
	       vehicles share
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
#include "graphics/ColorTransform_PlayerColor.h"
#include "containercontrols.h"
#include "resourcenet.h"
#include "accessconstraints.h"


ContainerBase ::  ContainerBase ( const ContainerBaseType* bt, GameMap* map, int player ) : gamemap ( map ), cargoParent(NULL), baseType (bt)
{
   view = bt->view;
   damage = 0;
   color = player*8;
   maxresearchpoints = baseType->defaultMaxResearchpoints;
   researchpoints = min ( maxresearchpoints, baseType->nominalresearchpoints );
   maxplus = baseType->maxplus;
   repairedThisTurn = 0;
  
   plus = baseType->defaultProduction;
   
   for ( int i = 0; i < map->getVehicleTypeNum(); ++i ) {
      const VehicleType* vt = map->getvehicletype_bypos(i);
      if ( vt ) 
         for ( int j = 0; j < bt->vehiclesInternallyProduceable.size(); ++j ) 
            if ( vt->id >= bt->vehiclesInternallyProduceable[j].from  && vt->id <= bt->vehiclesInternallyProduceable[j].to )
               internalUnitProduction.push_back ( vt ); 
   }
}


ASCString ContainerBase::getPrivateName() const
{
   if ( privateName.empty())
      return getName();
   else
      return privateName;  
}


sigc::signal<void,ContainerBase*> ContainerBase :: anyContainerDestroyed;
sigc::signal<void,ContainerBase*> ContainerBase :: anyContainerConquered;


Resources ContainerBase :: putResource ( const Resources& res, bool queryonly, int scope, int player)
{
   Resources result;
   for ( int i = 0; i < resourceNum; i++ )
      result.resource(i) = putResource ( res.resource(i), i , queryonly, scope, player );
   return result;
}


Resources ContainerBase :: getResource ( const Resources& res, bool queryonly, int scope, int player)
{
   Resources result;
   for ( int i = 0; i < resourceNum; i++ )
      result.resource(i) = getResource ( res.resource(i), i , queryonly, scope, player );
   return result;
}

Resources ContainerBase :: getResource ( const Resources& res ) const
{
   Resources result;
   for ( int i = 0; i < resourceNum; i++ )
      result.resource(i) = getAvailableResource ( res.resource(i), i );
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

int ContainerBase :: getMaxRepair ( const ContainerBase* item ) const
{
   Resources res;
   return getMaxRepair ( item, 0, res );
}

int ContainerBase :: getMaxRepair ( const ContainerBase* item, int newDamage, Resources& cost, bool ignoreCost  ) const
{
   if ( !canRepair( item ) )
      return item->damage;

   if ( item == this ) {
      if ( damage - repairableDamage() > newDamage )
         newDamage = damage - repairableDamage();
   } else if ( item->getCarrier() != this ) {
      if ( newDamage < item->baseType->minFieldRepairDamage )
         newDamage = item->baseType->minFieldRepairDamage;
   }

   if ( newDamage > item->damage )
      newDamage = item->damage;
   
   int toRepair = item->damage - newDamage;

   Resources maxNeeded = getRepairEfficiency() * item->baseType->productionCost;

   Resources needed;
   for ( int i = 0; i < resourceTypeNum; i++ )
      needed.resource(i) = maxNeeded.resource(i) * (item->damage-newDamage) / 100;

   if ( !ignoreCost ) {
      Resources avail = getResource ( needed );
   
      for ( int i = 0; i < resourceTypeNum; i++ )
         if ( needed.resource(i) ) {
            int repairable = toRepair * avail.resource(i) / needed.resource(i);
            if ( item->damage - repairable > newDamage )
               newDamage = item->damage - repairable;
         }
   }

   for ( int i = 0; i < resourceTypeNum; i++ )
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


int ContainerBase :: cargoNestingDepth()
{
   ContainerBase* cb = getCarrier();
   if ( cb )
      return cb->cargoNestingDepth() +1;
   else
      return 0;
}

void ContainerBase :: compactCargo()
{
   for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ) {
      if ( *i == NULL )
         i = cargo.erase(i);
      else
         ++i;
   }
}


Vehicle* ContainerBase :: getCargo( int i )
{
   if ( i < 0 || i >= cargo.size() )
      return NULL;
   else  
      return cargo.at(i);
}



ContainerBase* ContainerBase :: getCarrier() const
{
   return cargoParent;
}


Vehicle* ContainerBase :: findUnit ( int nwid, bool recursive ) const
{
   for ( Cargo::const_iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( *i ) {
         if ( (*i)->networkid == nwid )
            return *i;
         else {
            if ( recursive ) {
               Vehicle* cb = (*i)->findUnit( nwid );
               if ( cb )
                  return cb;
            }
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

   return 6 * ( binaryHeight - getFirstBit ( chfahrend ));
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
         if ( img.GetPixelFormat().BytesPerPixel() == 1 ) {
            MegaBlitter<1,gamemapPixelSize,ColorTransform_PlayerCol, ColorMerger_AlphaMixer, SourcePixelSelector_CacheRotation> blitter;
            blitter.setPlayer( getOwner() );
            blitter.setAngle( img, directionangle[dir] );
            blitter.blit( img, dest, pos );

            /*
            megaBlitter< ColorTransform_PlayerCol,
            ColorMerger_AlphaMixer,
            SourcePixelSelector_CacheRotation,
            TargetPixelSelector_All>
            ( img, dest, pos, getOwner(),nullParam, dirpair, nullParam);
            */
         } else {
            MegaBlitter<4,gamemapPixelSize,ColorTransform_PlayerTrueCol, ColorMerger_AlphaMixer, SourcePixelSelector_CacheRotation> blitter;
            blitter.setColor( gamemap->player[getOwner()].getColor() );
            blitter.setAngle( img, directionangle[dir] );
            blitter.blit( img, dest, pos );

         }
      }
   } else {
      if ( height >= chfahrend && shadowDist ) {
         if ( shadowDist == -1 )
            shadowDist = calcShadowDist( getFirstBit( height ));

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
            MegaBlitter<4,gamemapPixelSize,ColorTransform_PlayerTrueCol, ColorMerger_AlphaMerge, SourcePixelSelector_CacheRotation> blitter;
            /// blitter.setPlayer( getOwner() );
            blitter.setColor( gamemap->player[getOwner()].getColor() );
            blitter.setAngle( img, directionangle[dir] );
            blitter.blit( img, dest, pos );
         }
      }
   }
}


void ContainerBase :: clearCargo()
{
   while ( !cargo.empty() )
      if ( cargo.front() )
         delete cargo.front();
      else
         cargo.erase( cargo.begin() );

   cargoChanged();
}


void ContainerBase :: addToCargo( Vehicle* veh, int position )
{
   if ( veh == this )
      fatalError ("Trying to add unit to its own cargo");
   
   bool slotFound = false;
   for ( Cargo::iterator i = cargo.begin(); i != cargo.end(); ++i )
      if ( ! (*i) && (position == -1 || position == i - cargo.begin() )) {
         *i = veh;
         slotFound = true;
         break;
      }

   if ( !slotFound )
      cargo.push_back( veh );
   
   veh->cargoParent = this;
   veh->setnewposition(getPosition());
   cargoChanged();
}

bool ContainerBase :: removeUnitFromCargo( Vehicle* veh, bool recursive )
{
   if ( !veh )
      return false;
   else {
      if ( removeUnitFromCargo( veh->networkid, recursive )) {
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
            (*i)->cargoParent = NULL;
            *i = NULL;

            if ( cargo.size() > baseType->maxLoadableUnits ) {
               // we only compact the cargo when we have more slots than allowed (may happen when conquering fully occupied buildings).
               // because else we would confuse the user if the position of units in the cargo dialog changes
               compactCargo();
            }

            cargoChanged();
            return true;
         }
         if ( recursive )
            if ( (*i)->removeUnitFromCargo( nwid, recursive ))
               return true;
      }

   return false;
}

bool ContainerBase :: canCarryWeight( int additionalWeight, const Vehicle*  vehicle) const
{
   // if the unit already carries this unit, there is no additional weight to check
   if ( vehicle && findUnit( vehicle->networkid ))
      additionalWeight = 0;


   if ( cargoWeight() + additionalWeight > baseType->maxLoadableWeight )
      return false;
   else
      if ( getCarrier() )
         return getCarrier()->canCarryWeight( additionalWeight, vehicle );
      else
         return true;
}


bool ContainerBase :: vehicleFit ( const Vehicle* vehicle ) const
{
   bool isConquering = isBuilding() && getMap()->getPlayer(this).diplomacy.isHostile( vehicle) && vehicle->color != color;
   if ( baseType->vehicleFit ( vehicle->typ )) // checks size and type
      if ( vehiclesLoaded() < baseType->maxLoadableUnits || isConquering )
         if ( canCarryWeight( vehicle->weight(), vehicle ) || findUnit ( vehicle->networkid ) || isConquering) // if the unit is already  loaded, the container already bears its weight
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
               if ( i->height_rel == -100 || i->height_rel == getheightdelta ( getPosition().getNumericalHeight(), getFirstBit(uheight)  ) )
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
                                    if (damage >= minimumBuildingDamageForConquering  || vehicle->typ->hasFunction( ContainerBaseType::ConquerBuildings ) )
                                       return true;
                              }
                           }
                        }
   return false;
}

int  ContainerBase :: vehicleUnloadable ( const VehicleType* vehicleType, int carrierHeight ) const
{
   if (carrierHeight == -1 )
      return baseType->vehicleUnloadable( vehicleType, getPosition().getNumericalHeight());
   else
      return baseType->vehicleUnloadable( vehicleType, carrierHeight );

}

const ContainerBaseType::TransportationIO* ContainerBase::vehicleUnloadSystem ( const VehicleType* vehicleType, int height )
{
   if ( baseType->vehicleFit ( vehicleType ))
      for ( ContainerBaseType::EntranceSystems::const_iterator i = baseType->entranceSystems.begin(); i != baseType->entranceSystems.end(); i++ )
         if ( i->mode & ContainerBaseType::TransportationIO::Out )
            if ( (i->container_height & getPosition().getBitmappedHeight()) || (i->container_height == 0))
               if ( i->vehicleCategoriesLoadable & (1<<vehicleType->movemalustyp))
                  if ( vehicleType->hasAnyFunction(i->requiresUnitFeature) || i->requiresUnitFeature.none() ) {
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
                  }
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
               if ( i->vehicleCategoriesLoadable & (1<<vehicle->typ->movemalustyp)) {
                  if ( i->dockingHeight_abs != 0 && i->dockingHeight_rel != -100 )
                     height |= i->dockingHeight_abs & (1 << (getPosition().getNumericalHeight() + i->dockingHeight_rel ));
                  else
                     if ( i->dockingHeight_rel != -100 )
                        height |= 1 << (getPosition().getNumericalHeight() + i->dockingHeight_rel) ;
                     else
                        height |= i->dockingHeight_abs ;
               }
   return height;
}

const ContainerBase::Production& ContainerBase::getProduction() const
{
   if ( productionCache.empty() && !internalUnitProduction.empty() ) {
      for ( int height = 0; height < 8; ++height )
         if ( (1 << height) & baseType->height )
            for ( Production::const_iterator i = internalUnitProduction.begin(); i != internalUnitProduction.end(); ++i )
               if ( baseType->hasFunction( ContainerBaseType::ProduceNonLeavableUnits ) || vehicleUnloadable( *i, height ) )
                  if( find ( productionCache.begin(), productionCache.end(), *i ) == productionCache.end() )
                     productionCache.push_back ( *i );
   }

   return productionCache;
}

Resources ContainerBase::getProductionCost( const VehicleType* unit ) const
{
   return baseType->productionEfficiency * unit->productionCost;
}


void ContainerBase ::deleteProductionLine( const VehicleType* type )
{
   internalUnitProduction.erase( remove( internalUnitProduction.begin(), internalUnitProduction.end(), type ), internalUnitProduction.end());
   productionCache.clear();
}

void ContainerBase ::deleteAllProductionLines()
{
   internalUnitProduction.clear();
   productionCache.clear();
}

void ContainerBase :: addProductionLine( const VehicleType* type )
{
   if ( find ( internalUnitProduction.begin(), internalUnitProduction.end(), type ) == internalUnitProduction.end() )
      internalUnitProduction.push_back( type );
   productionCache.clear();
}

bool ContainerBase :: hasProductionLine( const VehicleType* type )
{
   return find ( internalUnitProduction.begin(), internalUnitProduction.end(), type ) != internalUnitProduction.end();
}


void ContainerBase :: setProductionLines( const Production& production  )
{
   internalUnitProduction = production;
   productionCache.clear();
}


void ContainerBase :: setName ( const ASCString& name )
{
   if ( !checkModificationConstraints( this ) )
      return;
   
   this->name = name;
}


ContainerBase :: ~ContainerBase ( )
{
   /* removing a unit from cargo (which is done when deleting) may cause a call to compactCargo, 
      which in turn invalidates the iterators to cargo 
      That's why we iterate through a copy of cargo */ 
   Cargo toDelete = cargo;
   
   for ( Cargo::iterator i = toDelete.begin(); i != toDelete.end(); ++i )
      if ( *i )
         delete *i;

   if ( gamemap->state != GameMap::Destruction ) {
      destroyed();
      anyContainerDestroyed( this );
   }
}


TemporaryContainerStorage :: TemporaryContainerStorage ( ContainerBase* _cb, bool storeCargo )
{
   cb = _cb;
   MemoryStream stream ( &buf, tnstream::writing );
   cb->write ( stream, storeCargo );
   _storeCargo = storeCargo;
}

void TemporaryContainerStorage :: restore (  )
{
   if ( _storeCargo ) {
      cb->clearCargo();
   }

   MemoryStream stream ( &buf, tnstream::reading );
   cb->read ( stream );
}






void ContainerBase::endOwnTurn( void )
{
}

void ContainerBase::endAnyTurn( void )
{
}

void ContainerBase::endRound ( void )
{
   view = baseType->view;
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



Player& ContainerBase :: getOwningPlayer() const 
{ 
   return getMap()->getPlayer(this); 
}

void ContainerBase :: setInternalResourcePlus( const Resources& res )
{
   for ( int r = 0; r < Resources::count; ++r ) 
      plus.resource(r) = min ( res.resource(r), min( maxplus.resource(r), baseType->maxplus.resource(r)));
}

void ContainerBase :: setInternalResourceMaxPlus( const Resources& res )
{
   for ( int r = 0; r < Resources::count; ++r ) 
      maxplus.resource(r) = min ( res.resource(r), baseType->maxplus.resource(r));
}

Resources ContainerBase :: getInternalResourcePlus() const
{
   return plus;
}

Resources ContainerBase :: getInternalResourceMaxPlus() const
{
   return maxplus;
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



