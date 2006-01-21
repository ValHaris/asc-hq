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

#include "ammotransferdialog.h"

#include "../unitctrl.h"



class ResourceWatch {

      Resources available;
      Resources storagelimit;
   public:
      ResourceWatch( ContainerBase* container )
      {
         available = container->getResource( Resources( maxint, maxint, maxint), true );
         storagelimit = container->putResource( Resources( maxint, maxint, maxint), true ) + available;
      };

      SigC::Signal0<void> sigChanged;

      Resources avail() { return available; };
      Resources limit() { return storagelimit; };
      
      bool putResources( Resources res )
      {
         Resources rr = available + res;
         for ( int r = 0; r < resourceTypeNum; ++r)
            if ( rr.resource(r) > storagelimit.resource(r))
               return false;
         available = rr;
         sigChanged();
         return true;
      }
         
      bool getResources( Resources res )
      {
         if ( available >= res ) {
            available -= res;
            sigChanged();
            return true;
         }
         return false;
      }
};

class Transferrable {
   protected:
      int originalSourceAmount;
   public:
      virtual ASCString getName() = 0;
      virtual void fillDest() = 0;
      virtual void emptyDest() = 0;
      int a;
      virtual ~Transferrable() {};
};

class ResourceTransferrable : public Transferrable {
   private:
      int resourceType;
      ResourceWatch& source;
      ResourceWatch& dest;
      void warn()
      {
         warning( "Inconsistency in ResourceTransfer");
      };
         
   public:
      ResourceTransferrable( int resource, ResourceWatch& src, ResourceWatch& dst ) : resourceType ( resource ), source(src), dest(dst) {};
      ASCString getName() { return Resources::name( resourceType ); };
      
      void fillDest()
      {
         int toTransfer = min( source.avail().resource(resourceType), dest.limit().resource(resourceType) - dest.avail().resource(resourceType));
         Resources r;
         r.resource( resourceType ) = toTransfer;
         if ( !source.getResources( r ))
            warn();
          
         if( !dest.putResources( r ))
            warn();
      }
      
      void emptyDest()
      {
         int toTransfer = min( dest.avail().resource(resourceType), source.limit().resource(resourceType) - source.avail().resource(resourceType));
         Resources r;
         r.resource( resourceType ) = toTransfer;
         if ( !dest.getResources( r ))
            warn();
          
         if( !source.putResources( r ))
            warn();
      }
};

class AmmoTransferrable : public Transferrable {
   private:
      int ammoType;
      int sourceAmmo;
      int destAmmo;
      ContainerBase* srcUnit;
      ContainerBase* dstUnit;
      ResourceWatch& source;
      ResourceWatch& dest;
      bool& allowAmmoProduction;
   public:
      AmmoTransferrable( int ammo, ContainerBase* s, ContainerBase* d, ResourceWatch& src, ResourceWatch& dst, bool& allowProduction ) : ammoType ( ammo ), srcUnit(s), dstUnit(d), source(src), dest(dst), allowAmmoProduction( allowProduction )
      {
         sourceAmmo = s->getAmmo( ammoType, maxint, true );
         destAmmo   = d->getAmmo( ammoType, maxint, true );
      };
      
      ASCString getName() { return cwaffentypen[ ammoType ]; };
      
      void fillDest()
      {
         int toTransfer = min( sourceAmmo, dstUnit->maxAmmo( ammoType) - destAmmo);
         destAmmo += toTransfer;
         sourceAmmo -= toTransfer;

         if ( allowAmmoProduction && destAmmo < dstUnit->maxAmmo( ammoType) && weaponAmmo[ammoType] ) {
            int delta = dstUnit->maxAmmo( ammoType) - destAmmo;

            
            for ( int r = 0; r < resourceTypeNum; ++r ) {
               if ( cwaffenproduktionskosten[ammoType][r] ) {
                  int produceable = source.avail().resource(r) / cwaffenproduktionskosten[ammoType][r];
                  if ( produceable < delta )
                     delta = produceable;
               }
            }

            Resources res;
            for ( int r = 0; r < resourceTypeNum; ++r ) 
               res.resource(r) = cwaffenproduktionskosten[ammoType][r] * delta;

            source.getResources( res );
            destAmmo += delta;
         }
      }
      
      void emptyDest()
      {
         int toTransfer = min( destAmmo, srcUnit->maxAmmo( ammoType) - sourceAmmo);
         destAmmo -= toTransfer;
         sourceAmmo += toTransfer;
      }
};


class TransferHandler {

   private:
      ResourceWatch sourceRes;
      ResourceWatch destRes;
      
      deallocating_vector<Transferrable*> transfers;

      bool allowProduction;

      ContainerBase* source;
      ContainerBase* dest;
      
   public:
      TransferHandler( ContainerBase* src, ContainerBase* dst ) : sourceRes( src ), destRes( dst ), allowProduction(false), source(src), dest(dst)
      {
         MapCoordinate spos = src->getPosition();
         MapCoordinate dpos = dst->getPosition();
         
         bool externalTransfer = spos != dpos;
         
         static ContainerBaseType::ContainerFunctions resourceVehicleFunctions[resourceTypeNum] = { ContainerBaseType::ExternalEnergyTransfer,
            ContainerBaseType::ExternalMaterialTransfer,
            ContainerBaseType::ExternalFuelTransfer };

         for ( int r = 0; r < resourceTypeNum; r++ ) 
            if (  !externalTransfer ||  src->baseType->hasFunction( resourceVehicleFunctions[r] ) ||  dst->baseType->hasFunction( resourceVehicleFunctions[r] ) )
               transfers.push_back(  new ResourceTransferrable( r, sourceRes, destRes ));

         for ( int a = 0; a < cwaffentypennum; ++a ) 
            if ( weaponAmmo[a] )
               if ( !externalTransfer || src->baseType->hasFunction( ContainerBaseType::ExternalAmmoTransfer ) ||  dst->baseType->hasFunction( ContainerBaseType::ExternalAmmoTransfer ) )
                  if ( src->maxAmmo( a ) && dst->maxAmmo( a )) 
                     transfers.push_back ( new AmmoTransferrable( a, src, dst, sourceRes, destRes, allowProduction ));
      };

      bool allowAmmoProduction( bool allow )
      {
         if ( ammoProductionPossible() ) {
            allowProduction = allow;
            return true;
         } else
            return false;
      }

      bool ammoProductionPossible()
      {
         return source->baseType->hasFunction( ContainerBaseType::AmmoProduction ) ||  dest->baseType->hasFunction( ContainerBaseType::AmmoProduction );
      }

      vector<Transferrable*> getTransfers()
      {
         return transfers;
      }

      void fillDest()
      {

      }

      void emptyDest()
      {

      }

      void commit()
      {

      }
      
};



AmmoTransferWindow :: AmmoTransferWindow ( ContainerBase* source, ContainerBase* destination, PG_Widget* parent ) : PG_Window( NULL, PG_Rect( 30, 30, 400, 400 ), "Transfer" ), first (source), second( destination )
{
   
}

void ammoTransferWindow ( ContainerBase* source, ContainerBase* destination )
{
   VehicleService service( );
}

void ammoTransferWindow ( VehicleService* serviceAction, ContainerBase* destination )
{

}
