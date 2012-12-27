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


#include "action.h"

#include "../vehicle.h"
#include "action-registry.h"
#include "actioncontainer.h"

GameAction::GameAction( GameMap* map )
 : gamemap(map)
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
      displayLogMessage(4, "executing " + getDescription() + "\n");
      ActionResult result = runAction( c );
      Command* command = dynamic_cast<Command*>(this);
      if ( context.actionContainer && result.successful() && !context.parentAction && command)
         context.actionContainer->add( command );
      return result;
   } catch ( const ActionResult& res ) {
      return res;
   }
}

ActionResult GameAction::undoChildren( const Context& context )
{
   ActionResult res(0);
   for ( Children::reverse_iterator i = children.rbegin(); i != children.rend(); ++i ) {
      ActionResult r = (*i)->undo( context );
      if ( !r.successful() && res.successful() )
         res =  r;
   }
   return res;
}

static void mergeActionFailures( ActionResult& failure, const ActionResult& res )
{
   if ( failure.successful() && !res.successful())
      failure = res;  
}

ActionResult GameAction::undo( const Context& context ) 
{
   // if there is one failuire, we'll preserve it
   ActionResult failure(0);
   if ( undoOrderChildFirst() )  {
      ActionResult r = undoChildren(context);
      mergeActionFailures( failure, r );
   }
   
   displayLogMessage(4, "undoing #" + ASCString::toString(sequenceNumber) + " " + getDescription() + "\n");
   ActionResult res = undoAction( context );
   if ( !res.successful() )
      warningMessage("error undoing " + getDescription() + "\n" + res.getMessage() );
   mergeActionFailures( failure, res );
   
   if ( !undoOrderChildFirst() ) {
      ActionResult r = undoChildren(context);
      mergeActionFailures( failure, r );
   }
   
   return failure;
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
   displayLogMessage(9, "reading " + ASCString::toString(getID()) + "\n");
   
   int version = stream.readInt();
   if ( version > currentGameActionVersion )
      throw tinvalidversion ( "GameAction", currentGameActionVersion, version );

   readData( stream );
   
   int token = stream.readInt();
   if ( token != gameActionToken )
      throw tinvalidversion ("GameActionToken for id " + ASCString::toString( getID() ), gameActionToken, token );
   
   int childNum = stream.readInt();
   for ( int i = 0; i < childNum; i++ ) {
      GameAction* child = readFromStream( stream, getMap() );
      addChild( child );
   }
}

void GameAction::write ( tnstream& stream, bool persistChildren ) const
{
   stream.writeInt( getID() );
   stream.writeInt( currentGameActionVersion );
   writeData( stream );
   stream.writeInt( gameActionToken );
   if ( persistChildren ) {
      stream.writeInt( children.size() );
      for ( int i = 0; i < children.size(); ++i ) 
         children[i]->write( stream );
   } else
      stream.writeInt( 0 );
}

void GameAction::write ( tnstream& stream ) const
{
   write( stream, true );
}

GameAction* GameAction::readFromStream( tnstream& stream, GameMap* map )
{
   GameActionID id = GameActionID( stream.readInt() );
   GameAction* action = gameActionFactory::Instance().createObject( id, map );
   action->read( stream );
   return action;
}


void GameAction::deleteChildren()
{
   for ( Children::iterator i = children.begin(); i != children.end(); ++i )
      delete *i;
   children.clear();
}
