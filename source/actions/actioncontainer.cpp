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
#include "../gamemap.h"

#include "../util/messaginghub.h"
#include "../basestrm.h"
#include "../util/messaginghub.h"

SigC::Signal2<void,GameMap*,const Command&> ActionContainer::postActionExecution;
SigC::Signal2<void,GameMap*,const Command&> ActionContainer::commitCommand;
SigC::Signal1<void,GameMap*> ActionContainer::actionListChanged;


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
   
   commandState_map[action] = true;
   
   postActionExecution( map, *action );
   actionListChanged(map);
}

ActionResult ActionContainer::undo( const Context& context )
{
   ActionResult res(0);
   if ( currentPos == actions.begin() )
      return ActionResult(23400);
   
   try {
      Actions::iterator a = currentPos;
      --a;
      if( isActive_map( *a ) ) {
         res = (*a)->undo ( context );
         commandState_map[*a] = false;
      }
      currentPos = a;
   } catch ( ActionResult result ) {
      errorMessage(result.getMessage());
      return result;
   }
   actionListChanged(map);
   return res;
}


ActionResult ActionContainer::redo( const Context& context )
{
   ActionResult res(0);
   if ( currentPos == actions.end() )
      return res;
   
   try {
      res = (*currentPos)->redo( context );
      commandState_map[*currentPos] = true;
      ++currentPos;
   } catch ( ActionResult result ) {
      errorMessage(result.getMessage());
   }
   actionListChanged(map);
   return res;
}

void ActionContainer::breakUndo()
{
   for ( Actions::iterator i = actions.begin(); i != currentPos; ++i ) {
      if( isActive_map( *i ) ) 
         commitCommand( map, **i );
      delete *i;
   }
   actions.clear();
   currentPos=actions.end();
   
   actionListChanged(map);
}

const int actionContainerversion = 2;

void ActionContainer::read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version > actionContainerversion )
      throw tinvalidversion ( "ActionContainer", actionContainerversion, version );
      
   int actionCount = stream.readInt();
   for ( int i = 0; i < actionCount; ++i ) {
      Command* a = dynamic_cast<Command*>( GameAction::readFromStream( stream, map ));
      actions.push_back ( a );
      
      if ( version >= 2 ) {
         commandState_map[a] = stream.readInt();
         
         int b = stream.readInt();
         if ( b >= 0 )
            commandState_map[a] = b;
      }
   }
   
   int pos = stream.readInt();
   currentPos = actions.begin();
   while ( pos-- )
      currentPos++;
   
   if ( version <= 1 )
      initCommandState( commandState_map );
   
}


void ActionContainer::write ( tnstream& stream )
{
   stream.writeInt( actionContainerversion );
   
   stream.writeInt( actions.size() );
   for ( Actions::iterator i = actions.begin(); i != actions.end(); ++i ) {
      (*i)->write(stream );
      
      if ( commandState_map.find( *i ) != commandState_map.end() )
         stream.writeInt( commandState_map[*i] );
      else
         stream.writeInt( 1 );
      
      if ( commandState_request.find( *i ) != commandState_request.end() )
         stream.writeInt( commandState_request[*i] );
      else
         stream.writeInt( -1 );

   }
   
   int counter = 0;
   Actions::iterator i = actions.begin();
   while ( i != currentPos ) {
      ++counter;
      ++i;
   }
   stream.writeInt( counter );
   
}

void ActionContainer::getActionDescriptions( vector<ASCString>& list )
{
   list.clear();  
   for ( Actions::iterator i = actions.begin(); i != currentPos; ++i )
      list.push_back( (*i)->getDescription() );

}

void ActionContainer::initCommandState( CommandState& commandState )
{
   commandState.clear();
   for ( Actions::iterator i = actions.begin(); i != currentPos; ++i )
      commandState[*i] = true;
   for ( Actions::iterator i = currentPos; i != actions.end(); ++i )
      commandState[*i] = false;
}


bool ActionContainer::isActive_map( const Command* action ) const
{
   if ( commandState_map.find( action ) != commandState_map.end() )
      return commandState_map.find(action)->second;
   else {
      warningMessage("ActionContainer::isActive_map - invalid parameter");
      return false;
   }
}

bool ActionContainer::isActive_req( const Command* action ) 
{
   if ( commandState_request.empty() )
      commandState_request = commandState_map;
   
   if ( commandState_request.find( action ) != commandState_request.end() )
      return commandState_request[action];
   else {
      warningMessage("ActionContainer::isActive - invalid parameter");
      return false;
   }
}


ActionResult ActionContainer::rerun( const Context& context )
{
   
   Actions::iterator currentCommand = currentPos;
   Actions::iterator firstDelta = currentPos;
   Actions::iterator i = currentPos;
   while ( i != actions.begin() ) {
      --i;
      if ( commandState_map.find( *i ) == commandState_map.end() )
         fatalError( "ActionContainer::rerun - inconsistent commandState_map ");
           
      if ( commandState_request.find( *i ) != commandState_request.end() ) {
         if ( commandState_request[*i] != commandState_map[*i] )
            firstDelta = i;
      }
   }
   
   while ( currentPos > firstDelta ) {
      --currentPos;
      if( isActive_map( *currentPos ) ) {
         ActionResult res = (*currentPos)->undo ( context );
         if ( !res.successful()) 
            return res;
      }
   }
   
   for ( CommandState::iterator j = commandState_request.begin(); j != commandState_request.end(); ++j )
      commandState_map[j->first] = j->second;
   
   commandState_request.clear();
   
   while ( currentPos < actions.end() ) {
      if( isActive_map( *currentPos ) ) {
         ActionResult res = (*currentPos)->redo ( context );
         if ( !res.successful()) 
            return res;
      }
      ++currentPos;
   }
   
   
   
   return ActionResult(0);
}


void ActionContainer::setActive( const Command* action, bool active )
{
   if ( commandState_request.empty() )
      commandState_request = commandState_map;
   
   commandState_request[action] = active;
}


void ActionContainer::getCommands( AbstractCommandWriter& writer)
{
   for ( Actions::iterator i = actions.begin(); i != actions.end(); ++i ) {
      if ( isActive_map( *i )) {
         writer.printComment( (*i)->getDescription() );
         writer.printCommand( (*i)->getCommandString() );
      }
   }
}


ActionContainer::~ActionContainer()
{
   for ( Actions::iterator i = actions.begin(); i != actions.end(); ++i ) {
      delete *i;
   }
}

