/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger
 
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


#ifndef consumeAmmoH
#define consumeAmmoH


#include "containeraction.h"


class ConsumeAmmo : public ContainerAction {
      int ammoType;
      int slot;
      int count;
      
      int resultingAmmount;
      
      bool produceAmmo;
      int produced;
      
      ConsumeAmmo( GameMap* map ) : ContainerAction( map ), produceAmmo(false), produced(0) {};
      template<class Child> friend GameAction* GameActionCreator( GameMap* map);

      /** produces num pieces of ammoType
          If not enough resources available, may return less than requested
         \return number produce
      */
      int produce( int num, const Context& context, bool queryOnly );
      
   public:
      ConsumeAmmo( ContainerBase* veh, int ammoType, int slot, int count );
      
      ASCString getDescription() const;
      
      /** specifies if ammo shall be produced if there is not enough ammo available 
          and the container has ammo production capabilities */
      void setAmmoProduction( bool prod );
      
   protected:
      virtual GameActionID getID() const;
      
      virtual ActionResult runAction( const Context& context );
      virtual ActionResult undoAction( const Context& context );
      virtual ActionResult postCheck();
      
      virtual void readData ( tnstream& stream );
      virtual void writeData ( tnstream& stream ) const;
      
};

#endif

