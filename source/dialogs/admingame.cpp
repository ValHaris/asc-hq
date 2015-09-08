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


#include <sstream>
#include <pgimage.h>

#include "admingame.h"
#include "../paradialog.h"
#include "../gamemap.h"
#include "../spfst.h"
#include "../loaders.h"
#include "playersetup.h"
#include "fileselector.h"
#include "pwd_dlg.h"

#include "../widgets/multilistbox.h"




class AdminGameWindow : public ASC_PG_Dialog {
   private:
      PG_ListBox* playerlistbox;
      PG_ListBox* actionlistbox;
      vector<PG_ListBoxItem*> players;
      GameMap* gamemap;
      PG_LineEdit* turn;
      PG_LineEdit* currentPlayer;
      PG_LineEdit* filename;

      typedef Loki::Functor<void, LOKI_TYPELIST_1(int) > PlayerActionFunctor;
      typedef PG_ListBoxDataItem<PlayerActionFunctor> ActionItem;

      PG_ScrollWidget* scrollwidget;

      PlayerSetupWidget* playerSetup;
      TurnSkipper* turnSkipper;

      bool successfullyClosed;
      
      void updateTurn()
      {
         if ( turn )
            turn->SetText( ASCString::toString( gamemap->time.turn() ));

         if ( currentPlayer ) {
            if ( gamemap->actplayer >= 0 )
               currentPlayer->SetText( gamemap->player[gamemap->actplayer].getName() );
            else
               currentPlayer->SetText( ASCString::toString( gamemap->actplayer ) );
         }
      }

      bool skipPlayer()
      {
         if ( turnSkipper ) {
            (*turnSkipper)(gamemap);
         }

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
         gamemap->getPlayer(player).resetView();
      }
      void resetResearch( int player )
      {
         gamemap->getPlayer(player).resetResearch();
      }
      
      void resetTribute( int player )
      {
         gamemap->getPlayer(player).resetTribute();
      }
      
      void resetPassword( int player )
      {
         gamemap->getPlayer(player).resetPassword();
      }

      void newPassword( int player )
      {
         enterpassword( gamemap->player[player].passwordcrc, true, true, false);
      }

      void newEmail( int player )
      {
          gamemap->player[player].email = editString2( "Mail Address for " + gamemap->player[player].getName(), gamemap->player[player].email);
      }


      void resetDiplomacy( int player )
      {
         for ( int j = 0; j< gamemap->getPlayerCount(); ++j ) {
            if ( j != player ) {
               gamemap->getPlayer(player).diplomacy.setState( j, WAR );
               gamemap->getPlayer(j).diplomacy.setState( player, WAR );
            }
         }
      }

      void deleteUnits( int player )
      {
         while ( gamemap->player[player].vehicleList.begin() != gamemap->player[player].vehicleList.end() )
            delete *gamemap->player[player].vehicleList.begin();

      }

      void deleteMines( int player )
      {
         for ( int y = 0; y < gamemap->ysize; ++y )
            for ( int x = 0; x < gamemap->xsize; ++x ) {
               MapField* fld = gamemap->getField(x,y);
               for ( MapField::MineContainer::iterator i = fld->mines.begin(); i != fld->mines.end(); )
                  if ( i->player == player )
                     i = fld->mines.erase( i );
                  else
                     ++i;
            }

      }

      void deleteBuildings( int player )
      {
         while ( gamemap->player[player].buildingList.begin() != gamemap->player[player].buildingList.end() )
            delete *gamemap->player[player].buildingList.begin();

      }

      void neutralBuildings( int player )
      {
         while ( gamemap->player[player].buildingList.begin() != gamemap->player[player].buildingList.end() )
            gamemap->player[player].buildingList.front()->convert(8);

      }
      
      
      void deleteProduction( int player )
      {
         for ( Player::BuildingList::iterator i = gamemap->player[player].buildingList.begin(); i != gamemap->player[player].buildingList.end(); ++i )
            (*i)->deleteAllProductionLines();

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

      static const int techID = 0xE1C8;

      void exportTechs( int player )
      {
         ASCString filename = selectFile( "*.asctechs", false );
         if ( filename.empty() )
            return;

         tnfilestream stream( filename, tnstream::writing );
         stream.writeInt( techID );
         stream.writeInt( 1 );
         writeClassContainer( gamemap->player[player].research.developedTechnologies, stream );
      }

      void importTechs( int player )
      {
         ASCString filename = selectFile( "*.asctechs", true );
         if ( filename.empty() )
            return;

         tnfilestream stream( filename, tnstream::reading );
         if ( stream.readInt() != techID ) {
            errorMessage("invalid file contents");
            return;
         }

         if ( stream.readInt() != 1 ) {
            errorMessage("invalid file version");
            return;
         }
         vector<int> dt;
         readClassContainer( dt, stream );
         vector<int>& techs = gamemap->player[player].research.developedTechnologies;
         copy( dt.begin(), dt.end(), back_inserter( techs ));
         sort( techs.begin(), techs.end() );
         techs.erase( unique( techs.begin(), techs.end()), techs.end() );
      }


      bool saveAsMap()
      {
         apply();
         if ( playerSetup->Apply() ) {
            GameTransferMechanism* gtm = gamemap->network;
            gamemap->network = NULL;

            if ( gamemap->_resourcemode == 1)
               warningMessage("This map uses BI resource mode. Saving as map will delete all global resources of all players");
               
            ASCString name = selectFile( mapextension, false);
            if ( !name.empty() ) {
               StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "saving " + name );
               savemap( name, gamemap );
            }

            gamemap->network = gtm;
            QuitModal();
            return true;
         } else
            return false;
         return true;
      }

      bool turnEdited()
      {
         gamemap->time.set( atoi( turn->GetText() ), 0 );
         updateTurn();
         return true;
      }
      
      bool netSetup()
      {
         if ( gamemap && gamemap->network )
            gamemap->network->setup();
         return true;
      }
      
      
   public:
      AdminGameWindow( GameMap* actmap, PG_Widget *parent, TurnSkipper* turnSkipper ) : ASC_PG_Dialog( parent, PG_Rect( -1, -1, 600, min(1000, PG_Application::GetScreenHeight()) ), "Admin Game" ), gamemap( actmap ), successfullyClosed(false)
      {
         this->turnSkipper = turnSkipper;
         
         scrollwidget = new PG_ScrollWidget( this, PG_Rect( 1, GetTitlebarHeight(), Width() - 2, Height() - GetTitlebarHeight() - 2 ));
         scrollwidget->SetTransparency(255);
         
         const int selectorHeight = 250;
         const int selectorWidth = 200;
         const int gap = 20;
         int ypos  = gap;
         playerlistbox = (new MultiListBox( scrollwidget, PG_Rect( gap, ypos, selectorWidth, selectorHeight )))->getListBox();

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
         new ActionItem( actionlistbox, 20, "reset diplomacy (->war)", PlayerActionFunctor( this, &AdminGameWindow::resetDiplomacy));
         new ActionItem( actionlistbox, 20, "reset password", PlayerActionFunctor( this, &AdminGameWindow::resetPassword));
         new ActionItem( actionlistbox, 20, "delete units", PlayerActionFunctor( this, &AdminGameWindow::deleteUnits));
         new ActionItem( actionlistbox, 20, "delete buildings", PlayerActionFunctor( this, &AdminGameWindow::deleteBuildings));
         new ActionItem( actionlistbox, 20, "delete production", PlayerActionFunctor( this, &AdminGameWindow::deleteProduction));
         new ActionItem( actionlistbox, 20, "delete resources + ammo", PlayerActionFunctor( this, &AdminGameWindow::deleteResources));
         new ActionItem( actionlistbox, 20, "delete mines", PlayerActionFunctor( this, &AdminGameWindow::deleteMines));
         new ActionItem( actionlistbox, 20, "make buildings neutral", PlayerActionFunctor( this, &AdminGameWindow::neutralBuildings));
         new ActionItem( actionlistbox, 20, "enter new password", PlayerActionFunctor( this, &AdminGameWindow::newPassword));
         new ActionItem( actionlistbox, 20, "edit email", PlayerActionFunctor( this, &AdminGameWindow::newEmail));
         new ActionItem( actionlistbox, 20, "export technology", PlayerActionFunctor( this, &AdminGameWindow::exportTechs));
         new ActionItem( actionlistbox, 20, "import technology", PlayerActionFunctor( this, &AdminGameWindow::importTechs));

         (new PG_Button( scrollwidget, PG_Rect( 3*gap+2*selectorWidth, ypos, 50, selectorHeight ), "Apply" ))->sigClick.connect( sigc::mem_fun( *this, &AdminGameWindow::apply ));
         
         ypos += 270;
         
         new PG_Label( scrollwidget, PG_Rect( 20, ypos, 100, 20 ), "Turn:" );
         turn = new PG_LineEdit( scrollwidget, PG_Rect( 130, ypos, 50, 20));
         turn->SetEditable( true );
         turn->sigEditEnd.connect( sigc::mem_fun( *this, &AdminGameWindow::turnEdited ));
         turn->sigEditReturn.connect( sigc::mem_fun( *this, &AdminGameWindow::turnEdited ));

         if ( turnSkipper ) {
             PG_Button* b = new PG_Button ( scrollwidget, PG_Rect( 200, ypos, 100, 50 ), "skip player" );
             b->sigClick.connect( sigc::mem_fun( *this, &AdminGameWindow::skipPlayer ));
         }

         ypos += 30;
         new PG_Label( scrollwidget, PG_Rect( 20, ypos, 100, 20 ), "Player:" );
         currentPlayer = new PG_LineEdit( scrollwidget, PG_Rect( 130, ypos, 50, 20));
         currentPlayer->SetEditable( false );


         
         if ( actmap->network ) {
            ypos += 30;         
            (new PG_Button( scrollwidget, PG_Rect( 20, ypos, 200, 20 ), "Setup data transfer" ))->sigClick.connect( sigc::mem_fun( *this, &AdminGameWindow::netSetup )) ;
         }
         
         ypos += 30;
         playerSetup = new PlayerSetupWidget( gamemap, PlayerSetupWidget::AllEditable, scrollwidget, PG_Rect(gap, ypos, Width() - 3*gap, PlayerSetupWidget::guessHeight(gamemap) ) );

         ypos += PlayerSetupWidget::guessHeight(gamemap) + gap;


         (new PG_Button( scrollwidget, PG_Rect( gap, ypos, Width() - 3* gap, 30), "OK" ))->sigClick.connect( sigc::mem_fun( *this, &AdminGameWindow::ok));
         ypos += 40;

         if ( actmap->getgameparameter(cgp_superVisorCanSaveMap)) {
            (new PG_Button( scrollwidget, PG_Rect( gap, ypos, Width() - 3* gap, 30), "Save as Map" ))->sigClick.connect( sigc::mem_fun( *this, &AdminGameWindow::saveAsMap));
            ypos += 40;
         }

         updateTurn();
      }


      bool ok()
      {
         apply();
         if ( playerSetup->Apply() ) {
            successfullyClosed = true;
            QuitModal();
            return true;
         } else
            return false;
      }


      bool GetStatus()
      {
         return successfullyClosed;
      }
};





bool adminGame( GameMap* actmap, TurnSkipper* turnSkipper )
{
   AdminGameWindow agw( actmap, NULL, turnSkipper);
   agw.Show();
   agw.RunModal();
   return agw.GetStatus();
}
