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


#include "changeview.h"


#include "../gamemap.h"
     
ChangeView::ChangeView( GameMap* gamemap, const ViewState& state )
   : GameAction( gamemap )
{
   newState = state;
}
      
      
ASCString ChangeView::getDescription() const
{
   return "Change view";
}
      
      
void ChangeView::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "ChangeView", 1, version );
   
   int count = stream.readInt();
   for ( int i = 0; i< count; ++i ) {
      MapCoordinate pos;
      pos.read( stream );
      newState[pos] = stream.readInt();
   }
   
   count = stream.readInt();
   for ( int i = 0; i< count; ++i ) {
      MapCoordinate pos;
      pos.read( stream );
      oldState[pos] = stream.readInt();
   }

};
      
      
void ChangeView::writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   stream.writeInt( newState.size() );
   for ( ViewState::iterator i = newState.begin(); i != newState.end(); ++i  ) {
      i->first.write( stream );
      stream.writeInt( i->second );
   }
   
   stream.writeInt( oldState.size() );
   for ( ViewState::iterator i = oldState.begin(); i != oldState.end(); ++i  ) {
      i->first.write( stream );
      stream.writeInt( i->second );
   }
   
};


GameActionID ChangeView::getID() const
{
   return ActionRegistry::ChangeView;
}

ActionResult ChangeView::runAction( const Context& context )
{
   for ( ViewState::iterator i = newState.begin(); i != newState.end(); ++i  ) {
      tfield* fld = getMap()->getField( i->first );
      oldState[i->first] = fld->visible;
      fld->visible = newState[i->first];
   }
   return ActionResult(0);
}


ActionResult ChangeView::undoAction( const Context& context )
{
   for ( ViewState::iterator i = newState.begin(); i != newState.end(); ++i  ) {
      tfield* fld = getMap()->getField( i->first );
      if ( fld->visible != newState[i->first] ) {
         ASCString msg;
         msg.format( "; expected: %x ; found %x" , newState[i->first], fld->visible );
         throw ActionResult(21207, "Position is " + i->first.toString(true) + msg );
      }
      fld->visible = oldState[i->first];
   }
   return ActionResult(0);
}

namespace {
   const bool r1 = registerAction<ChangeView> ( ActionRegistry::ChangeView );
}

