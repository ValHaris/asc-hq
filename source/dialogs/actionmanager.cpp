
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

#include "actionmanager.h"

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
#include "actionwidget.h"
#include "loki/Functor.h"


class ActionSelectionWidget: public ActionWidget
{
      
      ActionContainer& actions;
      PG_CheckButton* check;
      
   public:
      ActionSelectionWidget( PG_Widget* parent, const PG_Point& pos, int width, const Command& action, ActionContainer& actionContainer, GameMap* map ) 
         : ActionWidget( parent,pos, width, action, map ) ,  actions( actionContainer )
      {
         check = new PG_CheckButton( this, PG_Rect( 5, (Height()-15)/2, 15, 15 ));
         if ( actions.isActive_req( &action ) )
            check->SetPressed();
         
         check->sigClick.connect( sigc::mem_fun( *this, &ActionSelectionWidget::click ));
      }
   protected:
      bool click( bool b )
      {
         actions.setActive( &act, check->GetPressed() );
         return true;
      }

};


class ActionFactory : public SelectionItemFactory {
      ActionContainer& actions;
      ActionContainer::Actions::const_iterator a;
      GameMap* map;
   public:
      ActionFactory( GameMap* map ) : actions( map->actions )
      {
         this->map = map;
         restart();
      };
      
      void restart()
      {
         a = actions.getActions().begin();
      };
      
      SelectionWidget* spawnNextItem( PG_Widget* parent, const PG_Point& pos ) 
      {
         if ( a != actions.getActions().end() ) {
            return new ActionSelectionWidget( parent, pos, parent->Width() - 20, **(a++), actions, map );
         } else
            return NULL;
      }
      
      void itemSelected( const SelectionWidget* widget, bool mouse ) 
      {
         if ( mouse ) {
            const ActionSelectionWidget* asw = dynamic_cast<const ActionSelectionWidget*>( widget );
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

class ActionManager : public ASC_PG_Dialog {
      GameMap* gamemap;
      
      bool ok() {
         QuitModal();  
         return true;
      }
      
      bool run() {
         Hide();
         try {
            ActionResult res = gamemap->actions.rerun( createContext( gamemap ));
            if ( !res.successful() )
               displayActionError( res );
            repaintMap();
         } catch ( ActionResult res ) {
            displayActionError( res );
         }
         Show();
         return true;  
      }
      
      ItemSelectorWidget* selection;
      

      bool isAtCursor(SelectionWidget* widget) {
          const ActionSelectionWidget* asw = dynamic_cast<const ActionSelectionWidget*>( widget );
          if ( asw )  {
			 vector<MapCoordinate>  pos = asw->getCoordinates();
			 if ( pos.size() > 0 ) {
				return pos[0] == gamemap->getCursor();
			 }
          }
          return false;
      }


   public:   
      ActionManager( GameMap* map ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 400, 550 ), "Manage Actions" ), gamemap( map ) 
      {
         StandardButtonDirection( Horizontal );
         AddStandardButton("Close")->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &ActionManager::ok )));
         AddStandardButton("Run")->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &ActionManager::run )));
         selection = new ItemSelectorWidget( this, PG_Rect( 5, 20, Width()-10, Height() - 70 ), new ActionFactory( map ));

         findUnitAtCursor();
      }
   


      void findUnitAtCursor() {
    	  if ( gamemap->getCursor().valid()) {
    		  selection->selectItem(Loki::Functor<bool, LOKI_TYPELIST_1(SelectionWidget*)>(this, &ActionManager::isAtCursor), false);
    	  }

      }

};


void actionManager( GameMap* map )
{
   ActionManager am ( map );
   am.Show();
   am.RunModal();  
}

