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


#ifndef DiscoverResourcesH
#define DiscoverResourcesH


#include "unitaction.h"

#include "../typen.h"
#include "../mapfield.h"

class DiscoverResources : public UnitAction {
   public:
      class ResourceViewState {
         public:
            ResourceViewState() {};
            ResourceViewState( const MapField* fld, int player ) { initFromField( fld, player ); };
            MapCoordinate pos;
            int fuel;
            int material;
            bool visible;
            int player;
            void read ( tnstream& stream ) ;
            void write ( tnstream& stream ) const;
            void initFromField( const MapField* fld, int player );
            void writeToField( MapField* fld );
      };
      typedef vector<ResourceViewState> ViewState;
      DiscoverResources( Vehicle* unit );
      
      
      ASCString getDescription() const;
   private:
      ViewState newState;
      ViewState oldState;
      int shareview;
      
      void testField( const MapCoordinate& pos );
      
      DiscoverResources( GameMap* map ) : UnitAction( map ) {};
      template<class Child> friend GameAction* GameActionCreator( GameMap* map);
      
   protected:
      virtual GameActionID getID() const;
      
      virtual ActionResult runAction( const Context& context );
      virtual ActionResult undoAction( const Context& context );
      
      virtual void readData ( tnstream& stream );
      virtual void writeData ( tnstream& stream ) const;
      
};

#endif

