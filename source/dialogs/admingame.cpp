/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger
 
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


#include <sstream>
#include <pgimage.h>

#include "admingame.h"
#include "../paradialog.h"
#include "../gamemap.h"
#include "../spfst.h"
#include "playersetup.h"





class AdminGameWindow : public ASC_PG_Dialog {
   private:
      PG_ListBox* playerlistbox;
      PG_ListBox* actionlistbox;
      vector<PG_ListBoxItem*> players;
      GameMap* gamemap;
      PG_LineEdit* turn;
      PG_LineEdit* currentPlayer;

      typedef Loki::Functor<void, TYPELIST_1(int) > PlayerActionFunctor;
      typedef PG_ListBoxDataItem<PlayerActionFunctor> ActionItem;

      PG_ScrollWidget* scrollwidget;

      PlayerSetupWidget* playerSetup;
      TurnSkipper* turnSkipper;

      bool successfullyClosed;
      
      void updateTurn()
      {
         if ( turn )
            turn->SetText( ASCString::toString( gamemap->time.turn() ));

         if ( currentPlayer )
            if ( gamemap->actplayer >= 0 )
               currentPlayer->SetText( gamemap->player[gamemap->actplayer].getName() );
            else
               currentPlayer->SetText( ASCString::toString( gamemap->actplayer ) );
      }

      bool skipPlayer()
      {
         if ( turnSkipper )
            (*turnSkipper)(gamemap);

         updateTurn();
         return true;
      }

      bool apply()
      {
         for ( int i = 0; i < playerlistbox->GetWidgetCount(); ++i ) {
            PG_ListBoxBaseItem* bi = dynamic_cast<PG_ListBoxBaseItem*>(playerlistbox->FindWidget(i));
            if ( bi && bi->IsSelected() )
               applyForPlayer( i );
         }
         repaintMap();
         mapChanged( gamemap );
         return true;
      }

      void applyForPlayer( int player )
      {
         for ( int i = 0; i < actionlistbox->GetWidgetCount(); ++i ) {
            ActionItem* bi = dynamic_cast<ActionItem*>(actionlistbox->FindWidget(i));
            if ( bi && bi->IsSelected() )
               bi->getData()(player);
         }
      }

      void resetView( int player )
      {
         for ( int x = 0; x < gamemap->xsize; x++ )
            for ( int y = 0; y < gamemap->ysize; y++ ) {
               tfield* fld = gamemap->getField(x,y);
               fld->setVisibility( visible_not, player );
               if ( fld->resourceview )
                  fld->resourceview->visible &= ~(1<<player);
            }

      }
      void resetResearch( int player )
      {
         gamemap->player[player].research.progress = 0;
         gamemap->player[player].research.activetechnology = NULL;
         gamemap->player[player].research.developedTechnologies.clear();

      }
      
      void resetTribute( int player )
      {
         for ( int j = 0; j< gamemap->getPlayerCount(); ++j ) {
            gamemap->tribute.avail[player][j] = Resources();
            gamemap->tribute.avail[j][player]= Resources();
            gamemap->tribute.paid[player][j] = Resources();
            gamemap->tribute.paid[j][player]= Resources();
         }
      }
      
      void resetPassword( int player )
      {
         gamemap->player[player].passwordcrc.reset();
      }
      
      void resetDiplomacy( int player )
      {
         for ( int j = 0; j< gamemap->getPlayerCount(); ++j ) {
            if ( j != player ) {
               gamemap->getPlayer(player).diplomacy.setState( j, TRUCE, false );
               gamemap->getPlayer(j).diplomacy.setState( player, TRUCE, false );
            }
         }
      }

      void deleteUnits( int player )
      {
         while ( gamemap->player[player].vehicleList.begin() != gamemap->player[player].vehicleList.end() )
            delete *gamemap->player[player].vehicleList.begin();

      }

      void deleteBuildings( int player )
      {
         while ( gamemap->player[player].buildingList.begin() != gamemap->player[player].buildingList.end() )
            delete *gamemap->player[player].buildingList.begin();

      }
      
      void deleteProduction( int player )
      {
         for ( Player::BuildingList::iterator i = gamemap->player[player].buildingList.begin(); i != gamemap->player[player].buildingList.end(); ++i )
            (*i)->unitProduction.clear();

      }
      
      void deleteResources( int player )
      {
         for ( Player::BuildingList::iterator i = gamemap->player[player].buildingList.begin(); i != gamemap->player[player].buildingList.end(); ++i ) {
            for ( int j = 0; j < waffenanzahl; ++j )
               (*i)->ammo[j] = 0;

            (*i)->actstorage = Resources();
         }
         gamemap->bi_resource[player] = Resources();
         
         resetTribute( player );
      }
      
      
   public:
      AdminGameWindow( GameMap* actmap, PG_Widget *parent, TurnSkipper* turnSkipper ) : ASC_PG_Dialog( parent, PG_Rect( -1, -1, 600, 600 ), "Admin Game" ), gamemap( actmap ), successfullyClosed(false)
      {
         this->turnSkipper = turnSkipper;
         
         scrollwidget = new PG_ScrollWidget( this, PG_Rect( 1, GetTitlebarHeight(), Width() - 2, Height() - GetTitlebarHeight() - 2 ));
         scrollwidget->SetTransparency(255);
         
         const int selectorHeight = 250;
         const int selectorWidth = 200;
         const int gap = 20;
         playerlistbox = (new MultiListBox( scrollwidget, PG_Rect( gap, gap, selectorWidth, selectorHeight )))->getListBox();

         int lastPlayer = 0;
         for ( int i = 0; i < actmap->getPlayerCount(); ++i )
            if ( actmap->player[i].exist() )
               lastPlayer = i;
         
         for ( int i = 0; i <= lastPlayer; ++i ) {
            if ( actmap->player[i].exist() ) 
               players.push_back( new PG_ListBoxItem( playerlistbox, 20, actmap->player[i].getName() ));
            else
               players.push_back ( new PG_ListBoxItem( playerlistbox, 20, "-" ) );
         }

         actionlistbox = (new MultiListBox( scrollwidget, PG_Rect( 2*gap + selectorWidth, gap, selectorWidth, selectorHeight )))->getListBox();
         new ActionItem( actionlistbox, 20, "reset view", PlayerActionFunctor( this, &AdminGameWindow::resetView) );
         new ActionItem( actionlistbox, 20, "reset research", PlayerActionFunctor( this, &AdminGameWindow::resetResearch));
         new ActionItem( actionlistbox, 20, "reset tribute", PlayerActionFunctor( this, &AdminGameWindow::resetTribute));
         new ActionItem( actionlistbox, 20, "reset diplomacy (->truce)", PlayerActionFunctor( this, &AdminGameWindow::resetDiplomacy));
         new ActionItem( actionlistbox, 20, "reset password", PlayerActionFunctor( this, &AdminGameWindow::resetPassword));
         new ActionItem( actionlistbox, 20, "delete units", PlayerActionFunctor( this, &AdminGameWindow::deleteUnits));
         new ActionItem( actionlistbox, 20, "delete buildings", PlayerActionFunctor( this, &AdminGameWindow::deleteBuildings));
         new ActionItem( actionlistbox, 20, "delete production", PlayerActionFunctor( this, &AdminGameWindow::deleteProduction));
         new ActionItem( actionlistbox, 20, "delete resources + ammo", PlayerActionFunctor( this, &AdminGameWindow::deleteResources));

         (new PG_Button( scrollwidget, PG_Rect( 3*gap+2*selectorWidth, gap, 50, selectorHeight ), "Apply" ))->sigClick.connect( SigC::slot( *this, &AdminGameWindow::apply ));
         
         
         new PG_Label( scrollwidget, PG_Rect( 20, 300, 100, 20 ), "Turn:" );
         turn = new PG_LineEdit( scrollwidget, PG_Rect( 130, 300, 50, 20));
         turn->SetEditable( true );
                        
         new PG_Label( scrollwidget, PG_Rect( 20, 330, 100, 20 ), "Player:" );
         currentPlayer = new PG_LineEdit( scrollwidget, PG_Rect( 130, 330, 50, 20));
         currentPlayer->SetEditable( false );


         
         if ( turnSkipper ) {
             PG_Button* b = new PG_Button ( scrollwidget, PG_Rect( 200, 300, 100, 50 ), "skip player" );
             b->sigClick.connect( SigC::slot( *this, &AdminGameWindow::skipPlayer ));
         }

         playerSetup = new PlayerSetupWidget( gamemap, PlayerSetupWidget::AllEditable, scrollwidget, PG_Rect(gap, 360, Width() - 3*gap, PlayerSetupWidget::guessHeight(gamemap) ) );
         
         updateTurn();
      }

      bool GetStatus()
      {
         return successfullyClosed;
      }
};





bool adminGame( GameMap* actmap, TurnSkipper* turnSkipper )
{
   AdminGameWindow agw( actmap, NULL, turnSkipper );
   agw.Show();
   agw.RunModal();
   return agw.GetStatus();
}
