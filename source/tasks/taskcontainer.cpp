/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.org
     Copyright (C) 1994-2009  Martin Bickel
 
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

#include "taskcontainer.h"

#include "../gamemap.h"
#include "../actions/command.h"
#include "../actions/taskinterface.h"
            
            
static const int yetAnotherTask = 0xbd14cff9;
static const int noOtherTasks = 0x339fbb40;

            
            
TaskContainer::TaskContainer( GameMap* gamemap ) 
{
   this->gamemap = gamemap;
   
   gamemap->sigPlayerTurnHasEnded.connect( SigC::slot( *this, &TaskContainer::endTurn ));
   gamemap->sigPlayerTurnBegins.connect( SigC::slot( *this, &TaskContainer::startTurn ));
}            

TaskContainer::~TaskContainer()
{
   for ( CommandContainer::iterator i = pendingCommands.begin(); i != pendingCommands.end(); ++i )
      delete *i;
}

void TaskContainer::hook( GameMap& gamemap )
{
   gamemap.tasks = new TaskContainer( &gamemap );
}

void TaskContainer::registerHooks()
{
   GameMap::sigMapCreation.connect( SigC::slot( &TaskContainer::hook ));
   ActionContainer::commitCommand.connect( SigC::slot( &TaskContainer::getCommand ));
   
}

void TaskContainer::endTurn( Player& player )
{
   for ( CommandContainer::iterator i = pendingCommands.begin(); i != pendingCommands.end(); ++i )
      delete *i;
   pendingCommands.clear();
}


void TaskContainer::startTurn( Player& player )
{
   
   if ( lastPlayer >= 0 ) {
      /* a player's tasks are submitted at the beginning of the next player's turn, 
         to avoid any timing problems which could cause a task submit happening
         before the actions are transferred */
      
      {
         MemoryStream memstream( newTasks, tnstream::appending );
         memstream.writeInt( noOtherTasks );
      }
      
      delete playerTasks[lastPlayer];
      playerTasks[lastPlayer] = newTasks;
      newTasks = NULL;
      lastPlayer = -1;  
   }
   
   int p = player.getPosition();
   if ( playerTasks[p] ) {
      
      pendingCommands.clear();
      
      MemoryStream stream ( playerTasks[p], tnstream::reading );
      int token = stream.readInt();
      while ( token == yetAnotherTask ) {
         Command* cmd = dynamic_cast<Command*>( GameAction::readFromStream( stream, gamemap ) );
         assertOrThrow( cmd != NULL );
         
         TaskInterface* ti = dynamic_cast<TaskInterface*>( cmd );
         if ( !ti )
            fatalError( "Found a task that does not implement the TaskInterface");
         
         ti->rearm();
         
         pendingCommands.push_back( cmd );
         token = stream.readInt();
      }
   }
   delete playerTasks[p];
   playerTasks[p] = NULL;
}



void TaskContainer::store( const Command& command )
{
   if ( command.getState() == Command::Run ) {
      if ( newTasks == NULL )
         newTasks = new MemoryStreamStorage();
      
      const TaskInterface* ti = dynamic_cast<const TaskInterface*>( &command );
      if ( !ti ) 
         fatalError( "Trying to store a task that does not implement the TaskInterface\n" + command.getDescription());
      
      MemoryStream memstream( newTasks, tnstream::appending );
      memstream.writeInt( yetAnotherTask );
      command.write( memstream, false);
      lastPlayer = gamemap->actplayer;
   }
}
                         
void TaskContainer::remove( Command* cmd )
{
   CommandContainer::iterator i = find ( pendingCommands.begin(), pendingCommands.end(), cmd );
   if ( i != pendingCommands.end() )
      pendingCommands.erase( i );
}


void TaskContainer::getCommand( GameMap* gamemap, Command& command )
{
   if ( gamemap->tasks ) {
      TaskContainer* t = dynamic_cast<TaskContainer*>(gamemap->tasks);
      t->store( command );
   }
}


void TaskContainer::write ( tnstream& stream ) const
{            
   writeStorage( stream );
   stream.writeInt( pendingCommands.size() );
   for ( CommandContainer::const_iterator i = pendingCommands.begin(); i != pendingCommands.end(); ++i )
      (*i)->write( stream );   
   stream.writeInt( taskMagic );
}


void TaskContainer::read ( tnstream& stream )
{
   readStorage( stream );
   
   
   pendingCommands.clear();
   int count = stream.readInt();
   for ( int i = 0; i < count; ++i )
      pendingCommands.push_back( dynamic_cast<Command*>( GameAction::readFromStream( stream, gamemap )));
   
   int magic = stream.readInt();
   if ( magic != taskMagic ) 
      throw new tinvalidversion( stream.getLocation(), taskMagic, magic );
   
}

