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


#include "action.h"

#include "../vehicle.h"
#include "action-registry.h"
#include "actioncontainer.h"

GameAction::GameAction( GameMap* map )
 : gamemap(map), done(false)
{
   static int counter = 0;
   sequenceNumber = ++counter;
}
 

void GameAction::addChild( GameAction* action )
{
   children.push_back ( action );
}
      
ActionResult GameAction::execute( const Context& context ) 
{
   if ( context.parentAction ) 
      context.parentAction->addChild( this );
   
   Context c ( context, this );
   
   try {
      displayLogMessage(0, "executing " + getDescription() + "\n");
      ActionResult result = runAction( c );
      if ( context.actionContainer && result.successful() && !context.parentAction)
         context.actionContainer->add( this );
      return result;
   } catch ( ActionResult res ) {
      return res;
   }
}

void GameAction::redo( const Context& context )
{
   
}

void GameAction::undo( const Context& context ) 
{
   for ( Children::reverse_iterator i = children.rbegin(); i != children.rend(); ++i ) {
      // displayLogMessage(0, "undoing #" + ASCString::toString((*i)->sequenceNumber) + " (child) " + (*i)->getDescription() + "\n");
      (*i)->undo( context );
   }
   displayLogMessage(0, "undoing #" + ASCString::toString(sequenceNumber) + " " + getDescription() + "\n");
   undoAction( context );
}

/*
GameActionID GameAction::getID() const
{
   return ActionRegistry::Root;
}
*/

const int currentGameActionVersion = 1;
const int gameActionToken = 0x12003496;
      
void GameAction::read ( tnstream& stream )
{
   displayLogMessage(0, "reading " + ASCString::toString(getID()) + "\n");
   
   int version = stream.readInt();
   if ( version > currentGameActionVersion )
      throw tinvalidversion ( "GameAction", currentGameActionVersion, version );

   readData( stream );
   
   int token = stream.readInt();
   if ( token != gameActionToken )
      throw tinvalidversion ("GameActionToken", gameActionToken, token );
   
   int childNum = stream.readInt();
   for ( int i = 0; i < childNum; i++ ) {
      GameAction* child = readFromStream( stream, getMap() );
      addChild( child );
   }
}

void GameAction::write ( tnstream& stream )
{
   stream.writeInt( getID() );
   stream.writeInt( currentGameActionVersion );
   writeData( stream );
   stream.writeInt( gameActionToken );
   stream.writeInt( children.size() );
   for ( int i = 0; i < children.size(); ++i ) 
      children[i]->write( stream );
   
}

GameAction* GameAction::readFromStream( tnstream& stream, GameMap* map )
{
   GameActionID id = GameActionID( stream.readInt() );
   GameAction* action = gameActionFactory::Instance().createObject( id, map );
   action->read( stream );
   return action;
}

