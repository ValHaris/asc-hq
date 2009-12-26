/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2008  Martin Bickel  and  Marc Schellenberger
 
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


#include "discoverresources.h"
#include "action-registry.h"

#include "../gamemap.h"
     
DiscoverResources::DiscoverResources(  Vehicle* unit )
   : UnitAction( unit)
{
   shareview = 1 << ( unit->color / 8);
   for ( int i = 0; i < 8; i++ )
      if ( i*8 != unit->color )
         if ( getMap()->player[i].exist() )
            if ( getMap()->getPlayer(unit).diplomacy.sharesView(i) )
               shareview += 1 << i;

}
      
      
ASCString DiscoverResources::getDescription() const
{
   return "Discover Resources";
}


void DiscoverResources::ResourceViewState::read ( tnstream& stream ) 
{
   pos.read( stream );
   fuel = stream.readInt();
   material = stream.readInt();
   visible = stream.readInt();
   player = stream.readInt();
}

void DiscoverResources::ResourceViewState::write ( tnstream& stream ) const
{
   pos.write( stream );
   stream.writeInt( fuel );
   stream.writeInt( material );
   stream.writeInt( visible );
   stream.writeInt( player );
}

void DiscoverResources::ResourceViewState::initFromField( const MapField* fld, int player )
{
   this->player = player;
   if ( fld->resourceview ) {
      fuel = fld->resourceview->fuelvisible[player];
      material = fld->resourceview->materialvisible[player];
      visible = fld->resourceview->visible & (1 << player);
   } else {
      fuel = 0;
      material = 0;
      visible = 0;
   }
}

void DiscoverResources::ResourceViewState::writeToField( MapField* fld )
{
   if ( !fld->resourceview ) 
      fld->resourceview= new MapField::Resourceview;
   fld->resourceview->fuelvisible[player] = fuel;
   fld->resourceview->materialvisible[player] = material; 
   if ( visible )
      fld->resourceview->visible |= (1 << player);
   else
      fld->resourceview->visible &= ~(1 << player);
}

      
void DiscoverResources::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "DiscoverResources", 1, version );
   
   readClassContainer( newState, stream );
   readClassContainer( oldState, stream );
};
      
      
void DiscoverResources::writeData ( tnstream& stream ) const
{
   stream.writeInt( 1 );
   writeClassContainer( newState, stream );
   writeClassContainer( oldState, stream );
};


GameActionID DiscoverResources::getID() const
{
   return ActionRegistry::DiscoverResources;
}



void  DiscoverResources::testField( const MapCoordinate& mc )
{
    MapField* fld = getMap()->getField ( mc );
    if ( !fld->building  ||  fld->building->color == getMap()->actplayer*8  ||  fld->building->color == 8*8)
       if ( !fld->vehicle  ||  fld->vehicle->color == getMap()->actplayer*8 ||  fld->vehicle->color == 8*8) {
          if ( !fld->resourceview )
             fld->resourceview = new MapField::Resourceview;

          for ( int c = 0; c < 8; c++ )
             if ( shareview & (1 << c) ) {

                if ( !(fld->resourceview->visible & (1 << c)) || fld->resourceview->materialvisible[c] != fld->material || fld->resourceview->fuelvisible[c] != fld->fuel ) {
                   ResourceViewState rvs( fld, c);
                   rvs.pos = mc;
                   oldState.push_back( rvs );


                   ResourceViewState newView;
                   newView.fuel = fld->fuel;
                   newView.material = fld->material;
                   newView.visible = true;
                   newView.player = c;
                   newView.pos = mc;

                   newState.push_back( newView );
                   newView.writeToField ( fld );
                }
             }
       }
}


ActionResult DiscoverResources::runAction( const Context& context )
{
   if ( !getUnit()->typ->hasFunction( ContainerBaseType::DetectsMineralResources  ) )
      return ActionResult(311);

   circularFieldIterator( getMap(), getUnit()->getPosition(), 0, getUnit()->typ->digrange, FieldIterationFunctor(this, &DiscoverResources::testField ));

   return ActionResult(0);
}


ActionResult DiscoverResources::undoAction( const Context& context )
{
   for ( ViewState::iterator i = oldState.begin(); i != oldState.end(); ++i  ) {
      MapField* fld = getMap()->getField( i->pos );
      /*
      if ( fld->visible != newState[i->first] ) {
         ASCString msg;
         msg.format( "; expected: %x ; found %x" , newState[i->first], fld->visible );
         throw ActionResult(21207, "Position is " + i->first.toString(true) + msg );
      }
      */
      i->writeToField( fld );
   }
   return ActionResult(0);
}

namespace {
   const bool r1 = registerAction<DiscoverResources> ( ActionRegistry::DiscoverResources );
}

