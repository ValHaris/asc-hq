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


#ifndef actionContainerH
#define actionContainerH

#include <list>
#include <map>
#include <sigc++/sigc++.h>
#include "loki/Functor.h"
#include "loki/Typelist.h"

#include "command.h"
#include "commandwriter.h"

class ActionContainer {
   
      GameMap* map;
   
   public:
      typedef vector<Command*> Actions;
   private:
      Actions actions;
      
      Actions::iterator currentPos;
   
      typedef std::map<const Command*,bool> CommandState;
      CommandState commandState_request;
      CommandState commandState_map;
      
      void initCommandState( CommandState& commandState );
      
   public:
      ActionContainer( GameMap* gamemap );
      void add( Command* action );
      
      //! returns if the command is active in the current map's state
      bool isActive_map( const Command* action ) const ;
      
      //! returns if the command is active in the queued command list
      bool isActive_req( const Command* action );
      
      void setActive( const Command* action, bool active );
      
      const Actions& getActions() const { return actions; };
      
      //! does a single undo step
      ActionResult undo( const Context& context );
      
      //! does a single redo step
      ActionResult redo( const Context& context );
      
      //! reruns the necessary commands to propage take the changes done by setActive to the map
      ActionResult rerun( const Context& context );
      
      //! to be called when some modification to the map is taking place that is not recorded by undo
      void breakUndo();
      
      void read ( tnstream& stream );
      void write ( tnstream& stream );
      
      void getCommands( AbstractCommandWriter& writer );
      
      void getActionDescriptions( vector<ASCString>& list );
      
      //! signales an executed Command. The command can still be undone after postActionExecution
      static SigC::Signal2<void,GameMap*,const Command&> postActionExecution;
      
      /** The command, which has already been executed, is finally commited and will not be undone any more
          This may be used to store it in the replay structure  */
      static SigC::Signal2<void,GameMap*,Command&> commitCommand;
      
      static SigC::Signal1<void,GameMap*> actionListChanged;
      
      ~ActionContainer();
};



#endif

