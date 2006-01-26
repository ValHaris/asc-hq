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
      ContainerBase* container;
      Resources available;
      Resources storagelimit;
   public:
      ResourceWatch( ContainerBase* container )
      {
         this->container = container;
         available = container->getResource( Resources( maxint, maxint, maxint), true );
         storagelimit = container->putResource( Resources( maxint, maxint, maxint), true ) + available;
      };

      ContainerBase* getContainer()
      {
         return container;
      };

      SigC::Signal1<void, int> sigChanged;

      const Resources avail() { return available; };
      const Resources limit() { return storagelimit; };
      
      bool putResource( int resourcetype, int amount )
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
      
        
      bool getResource( int resourcetype, int amount )
      {
         if ( available.resource(resourcetype) >= amount ) {
            available.resource(resourcetype) -= amount;
            sigChanged( resourcetype );
            return true;
         }
         return false;
      }

      bool getResources( Resources res )
      {
         bool b = true;
         for ( int r = 0; r < resourceTypeNum; ++r)
            if ( res.resource(r) > 0 )
               if ( !getResource( r, res.resource(r) ))
                  b = false;
         return b;
      }
};

class Transferrable: public SigC::Object {
   protected:
      int originalSourceAmount;
      ResourceWatch& source;
      ResourceWatch& dest;

      ResourceWatch& getResourceWatch( const ContainerBase* unit )
      {
         assert( unit == source.getContainer() || unit == dest.getContainer() );
         if ( unit == source.getContainer() )
            return source;
         else
            return dest;
      }
      
      ResourceWatch& getOpposingResourceWatch( const ContainerBase* unit )
      {
         return getResourceWatch ( opposingContainer( unit ));
      }

      ContainerBase* opposingContainer( const ContainerBase* unit )
      {
         assert( unit == source.getContainer() || unit == dest.getContainer() );
         if ( unit == dest.getContainer() )
            return source.getContainer();
         else
            return dest.getContainer();
      }

      void show( const ContainerBase* unit )
      {
         assert( unit == source.getContainer() || unit == dest.getContainer() );
         if ( unit == dest.getContainer() )
            return sigDestAmount( ASCString::toString( getAmount( unit )));
         else
            return sigSourceAmount( ASCString::toString( getAmount( unit )));
      }
      
   public:
      Transferrable( ResourceWatch& s, ResourceWatch& d ) : source( s ) , dest( d ) {};
      virtual ASCString getName() = 0;

      /** get maximum amount for that unit.
         \param avail If true, the amount is limited by the resources which can actually provided by the other unit. If false, return the storage capacity
      */
      virtual int getMax( ContainerBase* c, bool avail ) = 0;
      virtual int getMin( ContainerBase* c, bool avail ) = 0;
      virtual int transfer( ContainerBase* target, int delta ) = 0;
      virtual int getAmount ( const ContainerBase* target ) = 0;

      // virtual bool valid() const = 0;

      void showAll()
      {
         show( source.getContainer() );
         show( dest.getContainer() );
      }
      
      SigC::Signal1<void,const std::string&> sigSourceAmount;
      SigC::Signal1<void,const std::string&> sigDestAmount;
      
      int setAmount( ContainerBase* target, int newamount )
      {
         transfer( target, newamount - getAmount( target ) );
         return getAmount( target );
      }

      void fill( ContainerBase* target )
      {
         setAmount( target, getMax( target, true ));
      }

      
      void empty( ContainerBase* target )
      {
         setAmount( target, 0 );
      }
      
      virtual ~Transferrable() {};
};

class ResourceTransferrable : public Transferrable {
   private:
      int resourceType;
      void warn()
      {
         warning( "Inconsistency in ResourceTransfer");
      };

      void srcChanged( int res )
      {
         if ( res == resourceType ) {
            show( source.getContainer() );
         }
      }
      
      void dstChanged( int res )
      {
         if ( res == resourceType ) {
            show( dest.getContainer() );
         }
      }
            
         
   public:
      ResourceTransferrable( int resource, ResourceWatch& src, ResourceWatch& dst ) : Transferrable( src, dst ), resourceType ( resource )
      {
         source.sigChanged.connect( SigC::slot( *this, &ResourceTransferrable::srcChanged ));
         dest.sigChanged.connect( SigC::slot( *this, &ResourceTransferrable::dstChanged ));
      };
      ASCString getName() { return Resources::name( resourceType ); };
      
      int getMax( ContainerBase* c, bool avail )
      {
         if ( avail ) {
            int needed = getResourceWatch( c ).limit().resource(resourceType) - getAmount( c );
            int av = min ( needed, getOpposingResourceWatch( c ).avail().resource(resourceType) );

            return getAmount( c ) + av;
         } else
            return getResourceWatch( c ).limit().resource(resourceType);
      }
      
      int getMin( ContainerBase* c, bool avail )
      {
         if ( avail ) {
            int space = getOpposingResourceWatch( c ).limit().resource(resourceType) - getOpposingResourceWatch( c ).avail().resource(resourceType);
            if ( space > getAmount( c ) )
               return 0;
            else
               return getAmount( c ) - space;
         } else
            return 0;
      }
      
      int getAmount ( const ContainerBase* target )
      {
         return getResourceWatch( target ).avail().resource(resourceType);
      }
      
      int transfer( ContainerBase* target, int delta )
      {
         if ( delta < 0 )
            return transfer( opposingContainer( target ), -delta );
         else {
            delta = min( delta, getOpposingResourceWatch( target ).avail().resource(resourceType) );
            delta = min( delta, getResourceWatch( target ).limit().resource(resourceType) - getAmount( target ));
            getOpposingResourceWatch( target ).getResource( resourceType, delta );
            getResourceWatch( target ).putResource(resourceType, delta);
            return delta;
         }
      }

};

class AmmoTransferrable : public Transferrable {
   private:
      int ammoType;
      int sourceAmmo;
      int destAmmo;
      bool& allowAmmoProduction;


      int& getAmmo( const ContainerBase* unit )
      {
         assert( unit == source.getContainer() || unit == dest.getContainer() );
         if ( unit == source.getContainer() )
            return sourceAmmo;
         else
            return destAmmo;
      }
      
   public:
      AmmoTransferrable( int ammo, ResourceWatch& src, ResourceWatch& dst, bool& allowProduction ) : Transferrable( src, dst ), ammoType ( ammo ), allowAmmoProduction( allowProduction )
      {
         sourceAmmo = src.getContainer()->getAmmo( ammoType, maxint, true );
         destAmmo   = dst.getContainer()->getAmmo( ammoType, maxint, true );
      };
      
      ASCString getName() { return cwaffentypen[ ammoType ]; };
      
      int get( ContainerBase* c, int toGet, bool queryOnly )
      {
         int got = min ( toGet, getAmmo( c ));
         int toProduce = toGet - got;

         if ( allowAmmoProduction && toProduce > 0 && weaponAmmo[ammoType] ) {
            for ( int r = 0; r < resourceTypeNum; ++r ) {
               if ( cwaffenproduktionskosten[ammoType][r] ) {
                  int produceable = getResourceWatch(c).avail().resource(r) / cwaffenproduktionskosten[ammoType][r];
                  if ( produceable < toProduce )
                     toProduce = produceable;
               }
            }
            if ( !queryOnly ) {
               Resources res;
               for ( int r = 0; r < resourceTypeNum; ++r )
                  res.resource(r) = cwaffenproduktionskosten[ammoType][r] * toProduce;

               getResourceWatch( c ).getResources( res );
               
               getAmmo(c) -= got;
            }
            got += toProduce;
         } else {
            if ( !queryOnly )
               getAmmo(c) -= got;
         }
         return got;
      }
      
      int getMax( ContainerBase* c, bool avail )
      {
         if ( avail ) {
            int needed = c->maxAmmo( ammoType ) - getAmount( c );
            int av = get( opposingContainer( c ), needed, true );
            return getAmount( c ) + av;
         } else
            return c->maxAmmo( ammoType );
      }
      
      int getMin( ContainerBase* c, bool avail )
      {
         if ( avail ) {
            int storable = opposingContainer(c)->maxAmmo(ammoType) - getAmmo( opposingContainer(c) );
            int transferable = min( getAmmo(c), storable );
            return getAmmo(c) - transferable;
         } else
            return 0;
      }
      
      int getAmount ( const ContainerBase* target )
      {
         return getAmmo( target );
      }
      
      int transfer( ContainerBase* target, int delta )
      {
         if ( delta < 0 )
            return transfer( opposingContainer( target ), -delta );
         else {
            delta = min( delta, target->maxAmmo(ammoType) - getAmount( target ));
            int got = get( opposingContainer( target ), delta, false );

            getAmmo(target) += got;
            show( target );
            return got;
         }
      }
      
};


class TransferHandler {
   private:
      ResourceWatch sourceRes;
      ResourceWatch destRes;

   public:
      typedef deallocating_vector<Transferrable*> Transfers;
   private:
      Transfers transfers;

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
            if (  externalTransfer ) {
               if ( src->baseType->hasFunction( resourceVehicleFunctions[r] ) ||  dst->baseType->hasFunction( resourceVehicleFunctions[r] ) )
                  transfers.push_back(  new ResourceTransferrable( r, sourceRes, destRes ));
            } else {
               if ( src->getStorageCapacity().resource(r) || dst->getStorageCapacity().resource(r) )
                  transfers.push_back(  new ResourceTransferrable( r, sourceRes, destRes ));
            }

         for ( int a = 0; a < cwaffentypennum; ++a ) 
            if ( weaponAmmo[a] )
               if ( !externalTransfer || src->baseType->hasFunction( ContainerBaseType::ExternalAmmoTransfer ) ||  dst->baseType->hasFunction( ContainerBaseType::ExternalAmmoTransfer ) )
                  if ( src->maxAmmo( a ) && dst->maxAmmo( a )) 
                     transfers.push_back ( new AmmoTransferrable( a, sourceRes, destRes, allowProduction ));
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

      Transfers& getTransfers()
      {
         return transfers;
      }

      void fillDest()
      {
         for ( Transfers::iterator i = transfers.begin(); i != transfers.end(); ++i )
            (*i)->fill( dest );
      }

      void emptyDest()
      {
         for ( Transfers::iterator i = transfers.begin(); i != transfers.end(); ++i )
            (*i)->empty( dest );

      }

      void commit()
      {

      }
      
};


class AmmoTransferWindow : public PG_Window {
   private:
      ContainerBase* first;
      ContainerBase* second;
      TransferHandler handler;
   public:
      AmmoTransferWindow ( ContainerBase* source, ContainerBase* destination, PG_Widget* parent );
};


class TransferWidget : public PG_Widget {

   public:
      TransferWidget ( PG_Widget* parent, const PG_Rect& pos, Transferrable* transferrable ) : PG_Widget( parent,pos )
      {
         new PG_Slider( this, PG_Rect( 0, 30, pos.w, 15 ),  PG_ScrollBar::HORIZONTAL );
         PG_Rect labels = PG_Rect( 0, 0, pos.w, 20 );
         PG_Label* l = new PG_Label ( this, labels, transferrable->getName() );
         l->SetAlignment( PG_Label::CENTER );
         
         l = new PG_Label ( this, labels );
         l->SetAlignment( PG_Label::LEFT );
         transferrable->sigSourceAmount.connect( SigC::slot( *l, &PG_Label::SetText ));
         
         l = new PG_Label ( this, labels );
         l->SetAlignment( PG_Label::RIGHT );
         transferrable->sigDestAmount.connect( SigC::slot( *l, &PG_Label::SetText ));
      };
};


AmmoTransferWindow :: AmmoTransferWindow ( ContainerBase* source, ContainerBase* destination, PG_Widget* parent ) : PG_Window( NULL, PG_Rect( 30, 30, 400, 400 ), "Transfer" ), first (source), second( destination ), handler( source, destination )
{
   int ypos = 40;
   int border = 10;
   for ( TransferHandler::Transfers::iterator i = handler.getTransfers().begin(); i != handler.getTransfers().end(); ++i ) {
      new TransferWidget( this, PG_Rect( border, ypos, w - 2*border, 50 ), *i );
      ypos += 60;
   }
   
   for ( TransferHandler::Transfers::iterator i = handler.getTransfers().begin(); i != handler.getTransfers().end(); ++i ) 
      (*i)->showAll();
}

void ammoTransferWindow ( ContainerBase* source, ContainerBase* destination )
{
   AmmoTransferWindow atw( source, destination, NULL );
   atw.Show();
   atw.RunModal();
}

void ammoTransferWindow ( VehicleService* serviceAction, ContainerBase* destination )
{

}
