/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.org
     Copyright (C) 1994-2010  Martin Bickel 
 
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

#ifndef taskcontainerH
#define taskcontainerH

#include <list>
#include "abstracttaskcontainer.h"
#include "../gamemap.h"

class tnstream;
class Command;
class MemoryStreamStorage;

class TaskContainer : public AbstractTaskContainer, public sigc::trackable {
      GameMap* gamemap;
      static void hook( GameMap& gamemap );
      static void getCommand( GameMap* gamemap, Command& command );
      
      void store( const Command& command );
      
      void startTurn( Player& player );
      void endTurn( Player& player );
      
   public:
      typedef list<Command*> CommandContainer;
      CommandContainer pendingCommands;
      
      void remove( Command* cmd );
      
      TaskContainer( GameMap* gamemap );
      ~TaskContainer();
      
      void read ( tnstream& stream );
      void write ( tnstream& stream ) const;
     
      static void registerHooks();
      
      virtual void foo() {};
};

#endif
