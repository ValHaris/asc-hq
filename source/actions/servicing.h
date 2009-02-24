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


#ifndef servicingH
#define servicingH

#include "../containerbase.h"
#include "../vehicletype.h"


class ResourceWatch {
      ContainerBase* container;
      Resources available;
      Resources storagelimit;
      Resources orgAmount;
   public:
      ResourceWatch( ContainerBase* container );
      ContainerBase* getContainer();
      SigC::Signal1<void, int> sigChanged;

      const Resources amount();
      const Resources avail();
      const Resources limit();
      bool putResource( int resourcetype, int amount );
      bool getResource( int resourcetype, int amount );
      bool getResources( Resources res );
};

class Transferrable: public SigC::Object {
   protected:
      ResourceWatch& source;
      ResourceWatch& dest;

      ResourceWatch& getResourceWatch( const ContainerBase* unit );
      ResourceWatch& getOpposingResourceWatch( const ContainerBase* unit );
      ContainerBase* opposingContainer( const ContainerBase* unit );

      void show( const ContainerBase* unit );
      
   public:
      Transferrable( ResourceWatch& s, ResourceWatch& d );
      virtual ASCString getName() = 0;

      /** get maximum amount for that unit.
      \param c the container for which the maximum amount is calculated
      \param avail If true, the amount is limited by the resources which can actually provided by the other unit. If false, return the storage capacity
       */
      virtual int getMax( ContainerBase* c, bool avail ) = 0;
      virtual int getMin( ContainerBase* c, bool avail ) = 0;
      virtual int transfer( ContainerBase* target, int delta ) = 0;
      virtual int getAmount ( const ContainerBase* target ) = 0;
      
      //! \deprecated 
      virtual void commit() = 0;
      virtual void commit( const Context& context ) = 0;
      virtual bool isExchangable() const = 0;

      //! the id is used to identify the transferrable when serializng to disk. In each service operation, the id must be unique through all transferrables
      virtual int getID() = 0;
      
      ContainerBase* getSrcContainer();
      ContainerBase* getDstContainer();
      bool setDestAmount( long amount );
      void showAll();
      
      SigC::Signal1<void,const std::string&> sigSourceAmount;
      SigC::Signal1<void,const std::string&> sigDestAmount;
      
      int setAmount( ContainerBase* target, int newamount );
      void fill( ContainerBase* target );
      void empty( ContainerBase* target );
      virtual ~Transferrable() {};
};



class ServiceChecker {
   protected:
      ContainerBase* source;
      int ignoreChecks;

      const SingleWeapon* getServiceWeapon();
      virtual void ammo( ContainerBase* dest, int type ) = 0;
      virtual void resource( ContainerBase* dest, int type, bool active )  = 0;
      virtual void repair( ContainerBase* dest ) = 0;

   private:
      bool serviceWeaponFits( ContainerBase* dest );

   public:
      static const int ignoreHeight = 1;
      static const int ignoreDistance = 2;
      ServiceChecker( ContainerBase* src, int skipChecks = 0 );


      void check( ContainerBase* dest );
      virtual ~ServiceChecker() {};
};


class ServiceTargetSearcher : protected ServiceChecker {

   private:
      GameMap* gamemap;
      int checks;

      void fieldChecker( const MapCoordinate& pos );
      void addTarget( ContainerBase* target );
   public:
      typedef vector<ContainerBase*> Targets;
   protected:
      Targets targets;

      void ammo( ContainerBase* dest, int type );
      void resource( ContainerBase* dest, int type, bool active );
      void repair( ContainerBase* dest );

   public:
      bool available();
      static const int checkAmmo = 1;
      static const int checkResources = 2;
      static const int checkRepair = 4;
      
      ServiceTargetSearcher( ContainerBase* src, int checkFlags );
      void startSearch();
      const Targets& getTargets() const { return targets; };
};




class TransferHandler : public SigC::Object, protected ServiceChecker {
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
      
   protected:
      void ammo( ContainerBase* dest, int type );
      void resource( ContainerBase* dest, int type, bool active );
      void repair( ContainerBase* dest ) {};
      
   public:
      TransferHandler( ContainerBase* src, ContainerBase* dst );
      bool allowAmmoProduction( bool allow );
      bool ammoProductionPossible();
      Transfers& getTransfers();
      void fillDest();
      void fillDestAmmo();
      void fillDestResource();
      void emptyDest();
      bool commit();
      bool commit( const Context& context );

      SigC::Signal0<bool> updateRanges;
      ~TransferHandler();
};



#endif

