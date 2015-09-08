
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

#include "taskmanager.h"

#include "../paradialog.h"
#include "../actions/actioncontainer.h"
#include "../actions/action.h"
#include "../actions/command.h"
#include "../gamemap.h"
#include "../sg.h"
#include "../dialog.h"
#include "../widgets/vehicletypeimage.h"
#include "../actions/unitcommand.h"
#include "../itemrepository.h"
#include "../mapdisplay.h"
#include "selectionwindow.h"
#include "../mainscreenwidget.h"
#include "../spfst.h"
#include "../tasks/taskcontainer.h"

#include "actionwidget.h"
#include "../actions/taskinterface.h"
#include "../contextutils.h"


class TaskWidget: public ActionWidget
{
      GameMap* gamemap;
      TaskContainer* taskContainer;
      Command* command;
   
      bool run() {
         TaskInterface* ti = dynamic_cast<TaskInterface*>( command );
         
         if ( ti->operatable() ) {
            ActionResult res = command->execute( createFollowerContext( gamemap ));
            if ( !res.successful() )
               displayActionError(res);
         }
         
         taskContainer->remove( command );
         Hide();
         return true;
      }
      
      bool cancel() {
         taskContainer->remove( command );
         delete command;
         Hide();
         return true;
      }
   
   public:
      TaskWidget( PG_Widget* parent, const PG_Point& pos, int width, Command* command, GameMap* map, TaskContainer* taskContainer ) 
   : ActionWidget( parent,pos, width, *command, map ) , gamemap( map )
      {
         this->taskContainer = taskContainer;
         this->command = command;
         
         (new PG_Button(this, PG_Rect( width-70, 5, 60, 15 ), "Run"))->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &TaskWidget::run )));
         (new PG_Button(this, PG_Rect( width-70, 22, 60, 15 ), "Cancel"))->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &TaskWidget::cancel )));
      }
   protected:
      bool click( )
      {
         return true;
      }

};


class TaskFactory : public SelectionItemFactory {
   TaskContainer* tasks;
   TaskContainer::CommandContainer::const_iterator a;
   GameMap* map;
   public:
      TaskFactory( GameMap* map ) 
      {
         this->map = map;
         tasks = dynamic_cast<TaskContainer*>( map->tasks );
         restart();
      };
      
      void restart()
      {
         a = tasks->pendingCommands.begin();
      };
      
      SelectionWidget* spawnNextItem( PG_Widget* parent, const PG_Point& pos ) 
      {
         if ( a != tasks->pendingCommands.end() ) {
            return new TaskWidget( parent, pos, parent->Width() - 20, *(a++),map, tasks );
         } else
            return NULL;
      }
      
      void itemSelected( const SelectionWidget* widget, bool mouse ) 
      {
         if ( mouse ) {
            const TaskWidget* asw = dynamic_cast<const TaskWidget*>( widget );
            if ( asw ) {
               vector<MapCoordinate>  pos = asw->getCoordinates();
               if ( pos.size() > 0 ) {
                  MapDisplayPG* md = getMainScreenWidget()->getMapDisplay();
                  md->cursor.goTo( pos[0] );
               }
            }
         }
      }

};

class TaskManager : public ASC_PG_Dialog {
   GameMap* gamemap;
      
   bool ok() 
   {
      QuitModal();  
      return true;
   }
      
   bool run() 
   {
      Hide();
      try {
         TaskContainer* tc = dynamic_cast<TaskContainer*>( gamemap->tasks );
         if ( tc ) {
            while ( tc->pendingCommands.begin() != tc->pendingCommands.end() ) {
               Command* c = tc->pendingCommands.front();
               TaskInterface* ti = dynamic_cast<TaskInterface*>( c );
         
               if ( ti->operatable() ) {
                  ActionResult res = c->execute( createFollowerContext( gamemap ));
                  if ( !res.successful() )
                     dispmessage2(res);
               }
         
               tc->pendingCommands.erase( tc->pendingCommands.begin() );
            }
         }
         
         ActionResult res = gamemap->actions.rerun( createContext( gamemap ));
         if ( !res.successful() )
            displayActionError( res );
         repaintMap();
      } catch ( ActionResult res ) {
         displayActionError( res );
      }
      Show();
      selection->reLoad(true);
      return true;  
   }
      
   ItemSelectorWidget* selection;
      
   public:   
      TaskManager( GameMap* map ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 600, 550 ), "Manage Tasks" ), gamemap( map ) 
      {
         StandardButtonDirection( Horizontal );
         AddStandardButton("Close")->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &TaskManager::ok )));
         AddStandardButton("Run All")->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &TaskManager::run )));
         selection = new ItemSelectorWidget( this, PG_Rect( 5, 20, Width()-10, Height() - 70 ), new TaskFactory( map ));
      }
   
};


void taskManager( GameMap* map )
{
   if ( ! dynamic_cast<TaskContainer*>( map->tasks ) ) {
      warningMessage("No Tasks to manage");
      return;
   }
   
   TaskManager am ( map );
   am.Show();
   am.RunModal();  
}

