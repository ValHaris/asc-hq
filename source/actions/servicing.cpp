/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger
 
     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.
 
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
 
     You should have received a copy of the GNU General Public License
     along with this program; see the file COPYING. If not, write to the 
     Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
     Boston, MA  02111-1307  USA
*/

#include <sigc++/sigc++.h>

#include "servicing.h"

#include "../vehicle.h"
#include "../spfst.h"
#include "../containercontrols.h"
#include "../gameoptions.h"
#include "../replay.h"

#include "consumeresource.h"
#include "consumeammo.h"

#include "context.h"

enum TransferLimitation { NONE, GET, ALL };

TransferLimitation getTransferLimitation( const ContainerBase* target )
{
   if ( target->getOwner() == target->getMap()->actplayer )
      return NONE;

   if ( target->getMap()->player[target->getMap()->actplayer].diplomacy.isAllied( target->getOwner() ))
      return NONE;

   if ( target->getMap()->player[target->getMap()->actplayer].diplomacy.getState( target->getOwner() ) >= PEACE )
      return GET;

   return ALL;
}



ResourceWatch::ResourceWatch( ContainerBase* container )
{
   this->container = container;
   orgAmount = available = container->getResource( Resources( maxint, maxint, maxint), true );
   storagelimit = container->putResource( Resources( maxint, maxint, maxint), true );
   for ( int r = 0; r < 3; ++r )
      if ( maxint - available.resource(r) > storagelimit.resource(r))
         storagelimit.resource(r) += available.resource(r);
      else
         storagelimit.resource(r) = maxint;
};

ContainerBase* ResourceWatch::getContainer()
{
   return container;
};

const Resources ResourceWatch::amount()
{
   return available;
}


const Resources ResourceWatch::avail() {
   TransferLimitation limit = getTransferLimitation( container );
   if ( limit == ALL )
      return Resources();

   if ( limit == GET ) {
      Resources res = available - orgAmount;
      for ( int r = 0; r < resourceTypeNum; ++r )
         if ( res.resource(r) < 0 )
            res.resource(r) = 0;

      return res;
   }

   return available;
};

const Resources ResourceWatch::limit() {
   if(  getTransferLimitation( container ) == ALL )
      return available;
   else
      return storagelimit;
};

bool ResourceWatch::putResource( int resourcetype, int amount )
{
   Resources rr = available;
   rr.resource(resourcetype) += amount;

   for ( int r = 0; r < resourceTypeNum; ++r)
      if ( rr.resource(r) > storagelimit.resource(r))
         return false;
   available = rr;
   sigChanged( resourcetype );
   return true;
}


bool ResourceWatch::getResource( int resourcetype, int amount )
{
   if ( available.resource(resourcetype) >= amount ) {
      available.resource(resourcetype) -= amount;
      sigChanged( resourcetype );
      return true;
   }
   return false;
}

bool ResourceWatch::getResources( Resources res )
{
   bool b = true;
   for ( int r = 0; r < resourceTypeNum; ++r)
      if ( res.resource(r) > 0 )
         if ( !getResource( r, res.resource(r) ))
            b = false;
   return b;
}


      
   ResourceWatch& Transferrable::getResourceWatch( const ContainerBase* unit )
   {
      assert( unit == source.getContainer() || unit == dest.getContainer() );
      if ( unit == source.getContainer() )
         return source;
      else
         return dest;
   }

   ResourceWatch& Transferrable::getOpposingResourceWatch( const ContainerBase* unit )
   {
      return getResourceWatch ( opposingContainer( unit ));
   }

   ContainerBase* Transferrable::opposingContainer( const ContainerBase* unit )
   {
      assert( unit == source.getContainer() || unit == dest.getContainer() );
      if ( unit == dest.getContainer() )
         return source.getContainer();
      else
         return dest.getContainer();
   }

   void Transferrable::show( const ContainerBase* unit )
   {
      assert( unit == source.getContainer() || unit == dest.getContainer() );
      if ( unit == dest.getContainer() )
         return sigDestAmount( ASCString::toString( getAmount( unit )));
      else
         return sigSourceAmount( ASCString::toString( getAmount( unit )));
   }


   Transferrable::Transferrable( ResourceWatch& s, ResourceWatch& d ) : source( s ) , dest( d ) {};

   ContainerBase* Transferrable::getSrcContainer()
   {
      return source.getContainer();
   }

   ContainerBase* Transferrable::getDstContainer()
   {
      return dest.getContainer();
   }

   bool Transferrable::setDestAmount( long amount )
   {
      return setAmount( getDstContainer(), amount ) == amount;
   }

   void Transferrable::showAll()
   {
      show( source.getContainer() );
      show( dest.getContainer() );
   }

   int Transferrable::setAmount( ContainerBase* target, int newamount )
   {
      transfer( target, newamount - getAmount( target ) );
      return getAmount( target );
   }

   void Transferrable::fill( ContainerBase* target )
   {
      setAmount( target, getMax( target, true ));
   }


   void Transferrable::empty( ContainerBase* target )
   {
      setAmount( target, 0 );
   }



   class AmmoTransferrable : public Transferrable {
      private:
         int ammoType;
         int sourceAmmo;
         int destAmmo;
         map<const ContainerBase*,int> produced;
         map<const ContainerBase*,int> orgAmmo;
         bool& allowAmmoProduction;


         int& getAmmo( const ContainerBase* unit );
         int put( ContainerBase* c, int toPut, bool queryOnly );
         int get( ContainerBase* c, int toGet, bool queryOnly );
         void executeTransfer( ContainerBase* from, ContainerBase* to, int amount, const Context& context );
      
      public:
         AmmoTransferrable( int ammo, ResourceWatch& src, ResourceWatch& dst, bool& allowProduction );      
         ASCString getName();
      
         int getID();
         int getMax( ContainerBase* c, bool avail );
         int getMin( ContainerBase* c, bool avail );
         int getAmount ( const ContainerBase* target );
         int transfer( ContainerBase* target, int delta );
         bool isExchangable() const;
         void commit( const Context& context );
   };

   class ResourceTransferrable : public Transferrable {
      private:
         int resourceType;
         bool exchangable;
         void warn();
         void srcChanged( int res );
         void dstChanged( int res );
         void executeTransfer( ContainerBase* from, ContainerBase* to, int amount, const Context& context );
      public:
         ResourceTransferrable( int resource, ResourceWatch& src, ResourceWatch& dst, bool isExchangable = true );
         ASCString getName();
         int getID();
         int getMax( ContainerBase* c, bool avail );
         int getMin( ContainerBase* c, bool avail );
         int getAmount ( const ContainerBase* target );
         int getAvail ( const ContainerBase* target );
         int transfer( ContainerBase* target, int delta );
         bool isExchangable() const;
         void commit(const Context& context);
   };
   
   
      


   void ResourceTransferrable::warn()
      {
         warning( "Inconsistency in ResourceTransfer");
      };

      void ResourceTransferrable::srcChanged( int res )
      {
         if ( res == resourceType ) {
            show( source.getContainer() );
         }
      }
      
      void ResourceTransferrable::dstChanged( int res )
      {
         if ( res == resourceType ) {
            show( dest.getContainer() );
         }
      }
            
         
      void ResourceTransferrable::executeTransfer( ContainerBase* from, ContainerBase* to, int amount, const Context& context )
      {
         if ( amount == 0 )
            return;
         
         if ( amount < 0 )
            executeTransfer( to, from, -amount, context );
         else 
            if ( amount > 0 ) {
               Resources r;
               r.resource(resourceType) = amount;
               
               ActionResult res = (new ConsumeResource( from, r ))->execute( context );
               if ( !res.successful() )
                  throw res;
               
               res = (new ConsumeResource( to, -r))->execute( context );
               if ( !res.successful() )
                  throw res;
               
            }
      }
      
      
ResourceTransferrable::ResourceTransferrable( int resource, ResourceWatch& src, ResourceWatch& dst, bool isExchangable  ) 
   : Transferrable( src, dst ), resourceType ( resource ), exchangable( isExchangable )
      {
         source.sigChanged.connect( SigC::slot( *this, &ResourceTransferrable::srcChanged ));
         dest.sigChanged.connect( SigC::slot( *this, &ResourceTransferrable::dstChanged ));
      };
      
      ASCString ResourceTransferrable::getName() 
      { 
         return Resources::name( resourceType ); 
      }
      
      int ResourceTransferrable::getID()
      {
         return resourceType + 1000; 
      };
      
      int ResourceTransferrable::getMax( ContainerBase* c, bool avail )
      {
         if ( avail ) {
            int needed = getResourceWatch( c ).limit().resource(resourceType) - getAvail( c );
            int av = min ( needed, getOpposingResourceWatch( c ).avail().resource(resourceType) );

            return getAvail( c ) + av;
         } else
            return getResourceWatch( c ).limit().resource(resourceType);
      }
      
      int ResourceTransferrable::getMin( ContainerBase* c, bool avail )
      {
         if ( avail ) {
            int space = getOpposingResourceWatch( c ).limit().resource(resourceType) - getOpposingResourceWatch( c ).avail().resource(resourceType);
            if ( space > getAvail( c ) )
               return 0;
            else
               return getAvail( c ) - space;
         } else
            return 0;
      }
      
      int ResourceTransferrable::getAmount ( const ContainerBase* target )
      {
         return getResourceWatch( target ).amount().resource(resourceType);
      }
      
      int ResourceTransferrable::getAvail ( const ContainerBase* target )
      {
         return getResourceWatch( target ).avail().resource(resourceType);
      }
      
      int ResourceTransferrable::transfer( ContainerBase* target, int delta )
      {
         if ( delta < 0 )
            return transfer( opposingContainer( target ), -delta );
         else {
            delta = min( delta, getOpposingResourceWatch( target ).avail().resource(resourceType) );
            delta = min( delta, getResourceWatch( target ).limit().resource(resourceType) - getAvail( target ));
            getOpposingResourceWatch( target ).getResource( resourceType, delta );
            getResourceWatch( target ).putResource(resourceType, delta);
            return delta;
         }
      }

      bool ResourceTransferrable::isExchangable() const
      {
         return exchangable;
      }
      
      void ResourceTransferrable::commit(const Context& context)
      {
         ContainerBase* target = dest.getContainer();
         executeTransfer( source.getContainer(), target, getAmount( target ) - target->getResource( maxint, resourceType, true ), context );
      }
      
      
      
      
      
      
      


      int& AmmoTransferrable::getAmmo( const ContainerBase* unit )
      {
         assert( unit == source.getContainer() || unit == dest.getContainer() );
         if ( unit == source.getContainer() )
            return sourceAmmo;
         else
            return destAmmo;
      }
      
      int AmmoTransferrable::put( ContainerBase* c, int toPut, bool queryOnly )
      {
         TransferLimitation limit = getTransferLimitation( c );

         if ( limit == ALL )
            return 0;
         
         int undoProduction = min( toPut, produced[c]);
         if ( undoProduction > 0 ) {
            if ( !queryOnly ) {
               for ( int r = 0; r < resourceTypeNum; ++r )
                  getResourceWatch( c ).putResource(  r, ammoProductionCost[ammoType][r] * undoProduction );
               
               produced[c] -= undoProduction;
            }
         }
         toPut -= undoProduction;

         toPut = min( toPut, c->maxAmmo(ammoType) - getAmmo( c ));

         if( !queryOnly ) {
            getAmmo(c) += toPut;
            show(c);
         }
         
         return toPut + undoProduction;
      }
            
      int AmmoTransferrable::get( ContainerBase* c, int toGet, bool queryOnly )
      {
         TransferLimitation limit = getTransferLimitation( c );

         if ( limit == ALL )
            return 0;
         
         int gettable;
         
         if ( limit == GET ) {
            gettable = getAmmo(c) - orgAmmo[c];
            if ( gettable < 0 )
               gettable = 0;
         } else
            gettable = getAmmo( c );
         
         int got = min ( toGet, gettable);
         int toProduce = toGet - got;

         if ( allowAmmoProduction && toProduce > 0 && weaponAmmo[ammoType] && c->baseType->hasFunction(ContainerBaseType::AmmoProduction) ) {
            for ( int r = 0; r < resourceTypeNum; ++r ) {
               if ( ammoProductionCost[ammoType][r] ) {
                  int produceable = getResourceWatch(c).avail().resource(r) / ammoProductionCost[ammoType][r];
                  if ( produceable < toProduce )
                     toProduce = produceable;
               }
            }
            if ( !queryOnly ) {
               Resources res;
               for ( int r = 0; r < resourceTypeNum; ++r )
                  res.resource(r) = ammoProductionCost[ammoType][r] * toProduce;

               getResourceWatch( c ).getResources( res );
               produced[c] += toProduce;
               
               getAmmo(c) -= got;
            }
            got += toProduce;
         } else {
            if ( !queryOnly ) {
               getAmmo(c) -= got;
               show( c );
            }
               
         }
         return got;
      }

     
      void AmmoTransferrable::executeTransfer( ContainerBase* from, ContainerBase* to, int amount, const Context& context )
      {
         if ( amount < 0 )
            executeTransfer( to, from, -amount, context );
         else 
            if ( amount > 0 ) {
               auto_ptr<ConsumeAmmo> ca1 ( new ConsumeAmmo( from, ammoType, -1, amount ));
               ca1->setAmmoProduction( allowAmmoProduction );
               
               ActionResult res = ca1->execute( context );
               if( res.successful() )
                  ca1.release();
               else
                  throw res;
               
               auto_ptr<ConsumeAmmo> ca2 ( new ConsumeAmmo( to, ammoType, -1, -amount ));
               res = ca2->execute( context );
               if( res.successful() )
                  ca2.release();
               else
                  throw res;
            }
      }
      
      AmmoTransferrable::AmmoTransferrable( int ammo, ResourceWatch& src, ResourceWatch& dst, bool& allowProduction ) : Transferrable( src, dst ), ammoType ( ammo ), allowAmmoProduction( allowProduction )
      {
         sourceAmmo = src.getContainer()->getAmmo( ammoType, maxint, true );
         destAmmo   = dst.getContainer()->getAmmo( ammoType, maxint, true );
         
         orgAmmo[src.getContainer()] = sourceAmmo;
         orgAmmo[dst.getContainer()] = destAmmo;
      };
      
      ASCString AmmoTransferrable::getName() 
      { 
         return cwaffentypen[ ammoType ]; 
      }
      
      int AmmoTransferrable::getID()
      {
         // the AI depends on this value and ID scheme!
         return ammoType + 2000; 
      };
      
      int AmmoTransferrable::getMax( ContainerBase* c, bool avail )
      {
         if ( avail ) {
            int needed = c->maxAmmo( ammoType ) - getAmount( c );
            int av = get( opposingContainer( c ), needed, true );
            return getAmount( c ) + av;
         } else
            return c->maxAmmo( ammoType );
      }
      
      int AmmoTransferrable::getMin( ContainerBase* c, bool avail )
      {
         if ( avail ) {
            int storable = opposingContainer(c)->maxAmmo(ammoType) - getAmmo( opposingContainer(c) );
            int transferable = min( getAmmo(c), storable );
            return getAmmo(c) - transferable;
         } else
            return 0;
      }
      
      int AmmoTransferrable::getAmount ( const ContainerBase* target )
      {
         return getAmmo( target );
      }
      
      int AmmoTransferrable::transfer( ContainerBase* target, int delta )
      {
         if ( delta < 0 )
            return transfer( opposingContainer( target ), -delta );
         else {
            delta = min( delta, put( target, delta, true ));
            int got = get( opposingContainer( target ), delta, false );
            put( target, got, false );
            return got;
         }
      }
      
      bool AmmoTransferrable::isExchangable() const
      {
         return true;
      };
      
      void AmmoTransferrable::commit( const Context& context )
      {
         executeTransfer( source.getContainer(), dest.getContainer(), destAmmo - orgAmmo[dest.getContainer()], context );
      }




const SingleWeapon* ServiceChecker :: getServiceWeapon()
{
   Vehicle* srcVehicle = dynamic_cast<Vehicle*>(source);
   if ( !srcVehicle )
      return false;

   const SingleWeapon* serviceWeapon = NULL;
   for (int i = 0; i < srcVehicle->typ->weapons.count ; i++)
      if ( srcVehicle->typ->weapons.weapon[i].service() )
         serviceWeapon = &srcVehicle->typ->weapons.weapon[i];

   return serviceWeapon;
}

bool ServiceChecker::serviceWeaponFits( ContainerBase* dest )
{
   const SingleWeapon* serviceWeapon = getServiceWeapon();
   if ( serviceWeapon )
      if ( (ignoreChecks & ignoreHeight) || (serviceWeapon->sourceheight & source->getHeight()) )
         if ( (ignoreChecks & ignoreHeight) || (serviceWeapon->targ & dest->getHeight() )) {
            int dist = beeline( source->getPosition(), dest->getPosition() );
            if ( (ignoreChecks & ignoreDistance) || (serviceWeapon->mindistance <= dist && serviceWeapon->maxdistance >= dist) )
               if ( serviceWeapon->targetingAccuracy[dest->baseType->getMoveMalusType()] > 0  )
                  if ( (ignoreChecks & ignoreHeight) || (serviceWeapon->efficiency[6+getheightdelta(source,dest)] > 0  ))
                     if ( ! ((source->getHeight() & (chtieffliegend | chfliegend | chhochfliegend)) && dest->baseType->hasFunction(ContainerBaseType::NoInairRefuelling)))
                        return true;

         }

   return false;
}

ServiceChecker :: ServiceChecker( ContainerBase* src, int skipChecks ) : source(src), ignoreChecks( skipChecks )
{
}


void ServiceChecker :: check( ContainerBase* dest )
{
   MapCoordinate spos = source->getPosition();
   MapCoordinate dpos = dest->getPosition();
   bool externalTransfer = spos != dpos;

   if ( source->isBuilding() && dest->isBuilding() )
      return; 

   if ( getTransferLimitation( dest ) == ALL )
      return;


   if ( source->getMap()->getPlayer(source).diplomacy.getState(dest->getOwner()) < PEACE )
      return;

   static ContainerBaseType::ContainerFunctions resourceVehicleFunctions[resourceTypeNum] = { ContainerBaseType::ExternalEnergyTransfer,
      ContainerBaseType::ExternalMaterialTransfer,
      ContainerBaseType::ExternalFuelTransfer };


      for ( int r = 0; r < resourceTypeNum; r++ ) {
         if (  externalTransfer ) {
            Vehicle* srcVehicle = dynamic_cast<Vehicle*>(source);
            if ( !srcVehicle ) {// it's a building
               Building* bld = dynamic_cast<Building*>(source);
               assert(bld);
               bool active = source->baseType->hasFunction( resourceVehicleFunctions[r] ) ||  dest->baseType->hasFunction( resourceVehicleFunctions[r] );
               if ( (ignoreChecks & ignoreHeight) || (bld->typ->externalloadheight & dest->getHeight()) )
                  if ( (ignoreChecks & ignoreDistance) || beeline(source->getPosition(), dest->getPosition()) < 20 )
                     resource( dest, r, active );
            } else {
               if ( serviceWeaponFits( dest )) {
                  bool active = source->baseType->hasFunction( resourceVehicleFunctions[r] ) ||  dest->baseType->hasFunction( resourceVehicleFunctions[r] );
                  resource( dest, r, active );
               }
            }

         } else {
            bool active =  (source->getStorageCapacity().resource(r) || source->isBuilding() )
                        && (dest->getStorageCapacity().resource(r) || dest->isBuilding());
            resource( dest, r, active );
         }
      }

   /* it is important that the ammo transfers are in front of the resource transfers, because ammo production affects resource amounts
      and their prelimarny commitment would cause inconsistencies */

      for ( int a = 0; a < weaponTypeNum; ++a ) {
         if ( source->maxAmmo( a ) && dest->maxAmmo( a )) {
            if ( weaponAmmo[a] ) {
               if ( externalTransfer ) {
                  if ( source->baseType->hasFunction( ContainerBaseType::ExternalAmmoTransfer ) ||  dest->baseType->hasFunction( ContainerBaseType::ExternalAmmoTransfer ) ) {
                     Vehicle* srcVehicle = dynamic_cast<Vehicle*>(source);
                     if ( !srcVehicle ) {// it's a building
                        Building* bld = dynamic_cast<Building*>(source);
                        assert(bld);
                        if ( (ignoreChecks & ignoreHeight) || (bld->typ->externalloadheight & dest->getHeight()) )
                           if ( (ignoreChecks & ignoreDistance) || beeline(source->getPosition(), dest->getPosition()) < 20 )
                              ammo(dest, a);
                     } else {
                        if ( serviceWeaponFits( dest ) )
                           ammo(dest, a);
                     }
                  }
               } else {
                  ammo(dest, a);
               }
            }
         }
      }

      
      
      if (  externalTransfer ) {
         Vehicle* srcVehicle = dynamic_cast<Vehicle*>(source);
         if ( srcVehicle ) {// it's a unit
            if ( srcVehicle->baseType->hasFunction( ContainerBaseType::ExternalRepair ))
               if ( serviceWeaponFits( dest ) && dest->damage > 0 ) {
                  repair( dest );
               }
         }

      } else {
         if ( source->baseType->hasFunction( ContainerBaseType::InternalUnitRepair ))
            repair( dest );
      }
      
      
      
}



void ServiceTargetSearcher::fieldChecker( const MapCoordinate& pos )
{
   tfield* fld = gamemap->getField( pos );
   if ( fld && fld->getContainer() )
      check( fld->getContainer() );
}

void ServiceTargetSearcher::addTarget( ContainerBase* target )
{
   if ( find( targets.begin(), targets.end(), target ) == targets.end() )
      targets.push_back( target );
}

void ServiceTargetSearcher::ammo( ContainerBase* dest, int type )
{
   if ( checks & checkAmmo )
      addTarget ( dest );
}

void ServiceTargetSearcher::resource( ContainerBase* dest, int type, bool active )
{
   if ( checks & checkResources )
      if ( active )
         addTarget ( dest );
}
      
void ServiceTargetSearcher::repair( ContainerBase* dest)
{
   if ( checks & checkRepair )
      addTarget ( dest );
}
      
      
ServiceTargetSearcher::ServiceTargetSearcher( ContainerBase* src, int checkFlags ) : ServiceChecker( src ), checks ( checkFlags )
{
   gamemap = src->getMap();
}


bool ServiceTargetSearcher::externallyAvailable()
{
   Vehicle* srcVehicle = dynamic_cast<Vehicle*>(source);
   if ( srcVehicle ) {
      if ( srcVehicle->attacked ) 
         return false;


      if ( srcVehicle->reactionfire.getStatus() == Vehicle::ReactionFire::off || srcVehicle->baseType->hasFunction(ContainerBaseType::MoveWithReactionFire))
         if ( source->getMap()->getField( source->getPosition() )->unitHere( srcVehicle )) {
            const SingleWeapon* weap = getServiceWeapon();
            if( weap ) 
               if ( source->baseType->hasFunction( ContainerBaseType::ExternalEnergyTransfer  ) || 
                    source->baseType->hasFunction( ContainerBaseType::ExternalMaterialTransfer  ) || 
                    source->baseType->hasFunction( ContainerBaseType::ExternalFuelTransfer  ) || 
                    source->baseType->hasFunction( ContainerBaseType::ExternalAmmoTransfer  )) 
                    return true;
            
         }         
   } else {
      if ( source->baseType->hasFunction( ContainerBaseType::ExternalEnergyTransfer  ) )
         // if ( source->getStorageCapacity().energy )
            return true;

      if ( source->baseType->hasFunction( ContainerBaseType::ExternalMaterialTransfer  ) )
         // if ( source->getStorageCapacity().material )
            return true;

      if ( source->baseType->hasFunction( ContainerBaseType::ExternalFuelTransfer  ) )
         // if ( source->getStorageCapacity().fuel )
            return true;

   }
   return false;
}


void ServiceTargetSearcher::startSearch()
{
   Vehicle* srcVehicle = dynamic_cast<Vehicle*>(source);
   if ( srcVehicle ) {
      if ( source->getMap()->getField( source->getPosition() )->unitHere( srcVehicle )) {
         const SingleWeapon* weap = getServiceWeapon();
         if( weap )
            circularFieldIterator(source->getMap(), source->getPosition(), weap->maxdistance / maxmalq, (weap->mindistance + maxmalq - 1) / maxmalq, FieldIterationFunctor( this, &ServiceTargetSearcher::fieldChecker ) );
      }
   } else
      circularFieldIterator(source->getMap(), source->getPosition(), 1, 1, FieldIterationFunctor( this, &ServiceTargetSearcher::fieldChecker ) );

      for ( ContainerBase::Cargo::const_iterator i = source->getCargo().begin(); i != source->getCargo().end(); ++i )
         if ( *i )
            check( *i );
};



void TransferHandler::ammo( ContainerBase* dest, int type )
{
   transfers.push_back ( new AmmoTransferrable( type, sourceRes, destRes, allowProduction ));
}

void TransferHandler::resource( ContainerBase* dest, int type, bool active )
{
   transfers.push_back(  new ResourceTransferrable( type, sourceRes, destRes, active ));
}

TransferHandler::TransferHandler( ContainerBase* src, ContainerBase* dst, int flags ) : ServiceChecker( src, flags ), sourceRes( src ), destRes( dst ), source(src), dest(dst)
{
   allowProduction = CGameOptions::Instance()->autoproduceammunition ;

   if ( dest->getMap()->player[dest->getMap()->actplayer].diplomacy.getState( dest->getOwner() ) <= TRUCE )
      return;

   check( dst );
};

bool TransferHandler::allowAmmoProduction( bool allow )
{
   if ( ammoProductionPossible() ) {
      allowProduction = allow;
      updateRanges();
      return true;
   } else
      return false;
}


TransferHandler::~TransferHandler()
{
   if ( allowProduction != CGameOptions::Instance()->autoproduceammunition  ) {
      CGameOptions::Instance()->autoproduceammunition = allowProduction ;
      CGameOptions::Instance()->setChanged();
   }
}

bool TransferHandler::ammoProductionPossible()
{
   return source->baseType->hasFunction( ContainerBaseType::AmmoProduction ) ||  dest->baseType->hasFunction( ContainerBaseType::AmmoProduction );
}

TransferHandler::Transfers& TransferHandler::getTransfers()
{
   return transfers;
}

void TransferHandler::fillDest()
{
   for ( Transfers::iterator i = transfers.begin(); i != transfers.end(); ++i )
      if ( (*i)->isExchangable())
         (*i)->fill( dest );
}

void TransferHandler::fillDestAmmo()
{
   for ( Transfers::iterator i = transfers.begin(); i != transfers.end(); ++i )
      if ( (*i)->isExchangable())
         if ( dynamic_cast<AmmoTransferrable*>(*i))
            (*i)->fill( dest );
}

void TransferHandler::fillDestResource()
{
   for ( Transfers::iterator i = transfers.begin(); i != transfers.end(); ++i )
      if ( (*i)->isExchangable())
         if ( dynamic_cast<ResourceTransferrable*>(*i))
            (*i)->fill( dest );
}


void TransferHandler::emptyDest()
{
   for ( Transfers::iterator i = transfers.begin(); i != transfers.end(); ++i )
      (*i)->empty( dest );

}


bool TransferHandler::commit( const Context& context )
{
   for ( Transfers::iterator i = transfers.begin(); i != transfers.end(); ++i )
      (*i)->commit( context );

   return true;
}



