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


#include "actioncontainer.h"

#include "../util/messaginghub.h"
#include "../basestrm.h"

SigC::Signal2<void,GameMap*,const Command&> postActionExecution;


ActionContainer::ActionContainer( GameMap* gamemap )
   : map ( gamemap )
{
   currentPos = actions.begin(); 
   
}

void ActionContainer::add( Command* action )
{
   for ( Actions::iterator i = currentPos; i != actions.end(); ++i )
      delete *i;
   actions.erase( currentPos, actions.end() );
   
   actions.push_back( action );
   currentPos = actions.end();
   
   postActionExecution( map, *action );
}

void ActionContainer::undo( const Context& context )
{
   if ( currentPos == actions.begin() )
      return;
   
   try {
      Actions::iterator a = currentPos;
      --a;
      (*a)->undo ( context );
      currentPos = a;
   } catch ( ActionResult result ) {
      errorMessage(result.getMessage());
   }
}


void ActionContainer::redo( const Context& context )
{
   if ( currentPos == actions.end() )
      return;
   
   try {
      (*currentPos)->redo( context );
      ++currentPos;
   } catch ( ActionResult result ) {
      errorMessage(result.getMessage());
   }
}

void ActionContainer::breakUndo()
{
   for ( Actions::iterator i = actions.begin(); i != actions.end(); ++i )
      delete *i;
   actions.clear();
   currentPos=actions.end();
}

const int actionContainerversion = 1;

void ActionContainer::read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version > actionContainerversion )
      throw tinvalidversion ( "ActionContainer", actionContainerversion, version );
      
   int actionCount = stream.readInt();
   for ( int i = 0; i < actionCount; ++i ) {
      Command* a = dynamic_cast<Command*>( GameAction::readFromStream( stream, map ));
      actions.push_back ( a );
   }
   
   int pos = stream.readInt();
   currentPos = actions.begin();
   while ( pos-- )
      currentPos++;
}


void ActionContainer::write ( tnstream& stream )
{
   stream.writeInt( actionContainerversion );
   
   stream.writeInt( actions.size() );
   for ( Actions::iterator i = actions.begin(); i != actions.end(); ++i )
      (*i)->write(stream );
   
   int counter = 0;
   Actions::iterator i = actions.begin();
   while ( i != currentPos ) {
      ++counter;
      ++i;
   }
   stream.writeInt( counter );
   
}
