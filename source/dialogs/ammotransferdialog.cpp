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
#include "../containercontrols.h"



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


class ResourceWatch {
      ContainerBase* container;
      Resources available;
      Resources storagelimit;
      Resources orgAmount;
   public:
      ResourceWatch( ContainerBase* container )
      {
         this->container = container;
         orgAmount = available = container->getResource( Resources( maxint, maxint, maxint), true );
         storagelimit = container->putResource( Resources( maxint, maxint, maxint), true ) + available;
      };

      ContainerBase* getContainer()
      {
         return container;
      };

      SigC::Signal1<void, int> sigChanged;

      const Resources avail() {
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
      
      const Resources limit() {
         if(  getTransferLimitation( container ) == ALL )
            return available;
         else
            return storagelimit;
      };
      
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
      virtual void commit() = 0;
      virtual bool isExchangable() const = 0;


      ContainerBase* getSrcContainer()
      {
         return source.getContainer();
      }
      
      ContainerBase* getDstContainer()
      {
         return dest.getContainer();
      }

      bool setDestAmount( long amount )
      {
         setAmount( getDstContainer(), amount );
         return true;
      }
            
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
      bool exchangable;
      
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
            
      void executeTransfer( ContainerBase* from, ContainerBase* to, int amount )
      {
         if ( amount == 0 )
            return;
         
         if ( amount < 0 )
            executeTransfer( to, from, -amount );
         else {
            int got = from->getResource( amount, resourceType, false );
            if ( got != amount )
               warning( ASCString("did not succeed in transfering resource ") + Resources::name( resourceType ) );
            to->putResource( got, resourceType, false );
         }
      }
         
      
   public:
      ResourceTransferrable( int resource, ResourceWatch& src, ResourceWatch& dst, bool isExchangable = true ) : Transferrable( src, dst ), resourceType ( resource ), exchangable( isExchangable )
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

      bool isExchangable() const
      {
         return exchangable;
      }
      
      void commit()
      {
         ContainerBase* target = dest.getContainer();
         executeTransfer( source.getContainer(), target, getAmount( target ) - target->getResource( maxint, resourceType, true ));
      }
      
};

class AmmoTransferrable : public Transferrable {
   private:
      int ammoType;
      int sourceAmmo;
      int destAmmo;
      map<const ContainerBase*,int> produced;
      map<const ContainerBase*,int> orgAmmo;
      bool& allowAmmoProduction;


      int& getAmmo( const ContainerBase* unit )
      {
         assert( unit == source.getContainer() || unit == dest.getContainer() );
         if ( unit == source.getContainer() )
            return sourceAmmo;
         else
            return destAmmo;
      }
      
      int put( ContainerBase* c, int toPut, bool queryOnly )
      {
         TransferLimitation limit = getTransferLimitation( c );

         if ( limit == ALL )
            return 0;
         
         int undoProduction = min( toPut, produced[c]);
         if ( undoProduction > 0 ) {
            if ( !queryOnly ) {
               for ( int r = 0; r < resourceTypeNum; ++r )
                  getResourceWatch( c ).putResource(  r, cwaffenproduktionskosten[ammoType][r] * undoProduction );
               
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
            
      int get( ContainerBase* c, int toGet, bool queryOnly )
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

      void executeTransfer( ContainerBase* from, ContainerBase* to, int amount )
      {
         if ( amount < 0 )
            executeTransfer( to, from, -amount );
         else {
            ContainerControls cc( from );
            int got = cc.getammunition( ammoType, amount, true, allowAmmoProduction );
            if ( got != amount )
               warning( "did not succeed in transfering ammo" );
            to->putAmmo( ammoType, got, false );
         }
      }
      
   public:
      AmmoTransferrable( int ammo, ResourceWatch& src, ResourceWatch& dst, bool& allowProduction ) : Transferrable( src, dst ), ammoType ( ammo ), allowAmmoProduction( allowProduction )
      {
         sourceAmmo = src.getContainer()->getAmmo( ammoType, maxint, true );
         destAmmo   = dst.getContainer()->getAmmo( ammoType, maxint, true );
         
         orgAmmo[src.getContainer()] = sourceAmmo;
         orgAmmo[dst.getContainer()] = destAmmo;
      };
      
      ASCString getName() { return cwaffentypen[ ammoType ]; };
      
      
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
            delta = min( delta, put( target, delta, true ));
            int got = get( opposingContainer( target ), delta, false );
            put( target, got, false );
            return got;
         }
      }
      
      bool isExchangable() const
      {
         return true;
      };
      
      void commit()
      {
         executeTransfer( source.getContainer(), dest.getContainer(), destAmmo - orgAmmo[dest.getContainer()] );
      }
      
};


class TransferHandler : public SigC::Object {
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
         if ( dst->getMap()->player[dst->getMap()->actplayer].diplomacy.getState( dst->getOwner() ) <= TRUCE )
            return;
         
         MapCoordinate spos = src->getPosition();
         MapCoordinate dpos = dst->getPosition();
         
         bool externalTransfer = spos != dpos;
         
         static ContainerBaseType::ContainerFunctions resourceVehicleFunctions[resourceTypeNum] = { ContainerBaseType::ExternalEnergyTransfer,
            ContainerBaseType::ExternalMaterialTransfer,
            ContainerBaseType::ExternalFuelTransfer };

         /* it is important that the ammo transfers are in front of the resource transfers, because ammo production affects resource amounts
            and their prelimarny commitment would cause inconsistencies */ 
            
         for ( int a = 0; a < cwaffentypennum; ++a )
            if ( weaponAmmo[a] )
               if ( !externalTransfer || src->baseType->hasFunction( ContainerBaseType::ExternalAmmoTransfer ) ||  dst->baseType->hasFunction( ContainerBaseType::ExternalAmmoTransfer ) )
                  if ( src->maxAmmo( a ) && dst->maxAmmo( a )) 
                     transfers.push_back ( new AmmoTransferrable( a, sourceRes, destRes, allowProduction ));

         for ( int r = 0; r < resourceTypeNum; r++ ) {
            bool active;
            if (  externalTransfer ) {
               active = src->baseType->hasFunction( resourceVehicleFunctions[r] ) ||  dst->baseType->hasFunction( resourceVehicleFunctions[r] );
            } else {
               active = src->getStorageCapacity().resource(r) || dst->getStorageCapacity().resource(r) ;
            }
            transfers.push_back(  new ResourceTransferrable( r, sourceRes, destRes, active ));
         }
      };

      bool allowAmmoProduction( bool allow )
      {
         if ( ammoProductionPossible() ) {
            allowProduction = allow;
            updateRanges();
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

      bool commit()
      {
         for ( Transfers::iterator i = transfers.begin(); i != transfers.end(); ++i )
            (*i)->commit();
         
         return true;
      }

      SigC::Signal0<bool> updateRanges;
};


class TransferWidget : public PG_Widget {
   private:
      Transferrable* trans;
      PG_Slider* slider;

      bool updatePos( long a = 0 )
      {
         if ( slider )
            slider->SetPosition( trans->getAmount( trans->getDstContainer() ));
         return true;
      }

      bool updateRange()
      {
         int min = trans->getMin( trans->getDstContainer(), false );
         
         int max;
         if ( trans->getMax( trans->getDstContainer(), true ) * 3 < trans->getMax( trans->getDstContainer(), false ))
            max = trans->getMax( trans->getDstContainer(), true );
         else
            max = trans->getMax( trans->getDstContainer(), false );

         if ( slider )
            slider->SetRange( min, max );
         
         updatePos();
         return true;
      }
      
      
   public:
      TransferWidget ( PG_Widget* parent, const PG_Rect& pos, Transferrable* transferrable, TransferHandler& handler ) : PG_Widget( parent,pos ), trans( transferrable ), slider(NULL)
      {
         if ( transferrable->isExchangable() ) {
            slider = new PG_Slider( this, PG_Rect( 0, 30, pos.w, 15 ),  PG_ScrollBar::HORIZONTAL );
   
            updateRange();
   
            slider->sigSlide.connect( SigC::slot( *transferrable, &Transferrable::setDestAmount ));
            slider->sigSlideEnd.connect( SigC::slot( *this, &TransferWidget::updatePos));

            handler.updateRanges.connect( SigC::slot( *this, &TransferWidget::updateRange));
         }

         
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


class AmmoTransferWindow : public PG_Window {
   private:
      ContainerBase* first;
      ContainerBase* second;
      TransferHandler handler;

      bool ok()
      {
         handler.commit();
         QuitModal();
         return true;
      }
      
   public:
      AmmoTransferWindow ( ContainerBase* source, ContainerBase* destination, PG_Widget* parent );

      bool somethingToTransfer() { return handler.getTransfers().size(); };
      
      bool eventKeyDown(const SDL_KeyboardEvent* key)
      {
         if ( key->keysym.sym == SDLK_ESCAPE )  {
            QuitModal();
            return true;
         }
         return false;
      }
      
};


AmmoTransferWindow :: AmmoTransferWindow ( ContainerBase* source, ContainerBase* destination, PG_Widget* parent ) : PG_Window( NULL, PG_Rect( 30, 30, 400, 400 ), "Transfer" ), first (source), second( destination ), handler( source, destination )
{
   int ypos = 30;
   int border = 10;

   if ( handler.ammoProductionPossible() ) {
      PG_CheckButton* production = new PG_CheckButton( this, PG_Rect( border, ypos, w - 2*border, 20 ), "allow ammo production" );
      production->sigClick.connect( SigC::slot( handler, &TransferHandler::allowAmmoProduction ));
      ypos += 30;
   }
   
   for ( TransferHandler::Transfers::iterator i = handler.getTransfers().begin(); i != handler.getTransfers().end(); ++i ) {
      new TransferWidget( this, PG_Rect( border, ypos, w - 2*border, 50 ), *i, handler );
      ypos += 60;
   }

   int buttonWidth = 150;
   PG_Button* b = new PG_Button( this, PG_Rect( w - buttonWidth - border, ypos, buttonWidth, 30), "OK" );
   b->sigClick.connect( SigC::slot( *this, &AmmoTransferWindow::ok ));
   
   for ( TransferHandler::Transfers::iterator i = handler.getTransfers().begin(); i != handler.getTransfers().end(); ++i ) 
      (*i)->showAll();
}

void ammoTransferWindow ( ContainerBase* source, ContainerBase* destination )
{
   AmmoTransferWindow atw( source, destination, NULL );
   if ( atw.somethingToTransfer() ) {
      atw.Show();
      atw.RunModal();
   }
}

void ammoTransferWindow ( VehicleService* serviceAction, ContainerBase* destination )
{

}
