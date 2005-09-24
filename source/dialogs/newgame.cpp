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

#include "newgame.h"
#include "editmapparam.h"
#include "alliancesetup.h"
#include "playersetup.h"
#include "fileselector.h"
#include "../loaders.h"
#include "../gamemap.h"
#include "../paradialog.h"
#include "../controls.h"
#include "../viewcalculation.h"


class GameParameterEditorWidget;


class StartMultiplayerGame: public ConfigurableWindow {
   private:

      tmap* newMap;
      
      enum Pages { ModeSelection = 1, FilenameSelection, PlayerSetup, AllianceSetup, MapParameterEditor, MultiPlayerOptions, PasswordSearch }; 
      Pages page;
     
      enum Mode { NewCampagin, ContinueCampaign, Skirmish, Hotseat, PBEM, PBP };
      int mode;
      
      static const char* buttonLabels[];
      
      ASCString filename;
      
      GameParameterEditorWidget* mapParameterEditor;
      PG_Widget* mapParameterEditorParent;
      
      AllianceSetupWidget* allianceSetup;
      PG_Widget* allianceSetupParent;
      
      PlayerSetupWidget* playerSetup;
      PG_Widget* playerSetupParent;
   
      bool nextPage(PG_Button* button = NULL);
      void showPage();
      
      void fileNameSelected( const ASCString& filename )
      {
         this->filename = filename;
         nextPage();
      };   

      void fileNameMarked( const ASCString& filename )
      {
         this->filename = filename;
      };   
      
      bool quickstart()
      {
         if ( Apply() )
            start();
         return true;
      }
      
      void showButtons( bool start, bool quickstart, bool next )
      {
         if ( next )
            show("next");
         else
            hide("next");   
            
         if ( quickstart )
            show("quickstart");
         else
            hide("quickstart");   

         if ( start )
            show("start");
         else
            hide("start");   
      };
            
   protected:   
      void userHandler( const ASCString& label, PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams ); 
      bool start();
      bool Apply();
      
   public:
      StartMultiplayerGame(PG_MessageObject* c);
      ~StartMultiplayerGame();
   
};

const char* StartMultiplayerGame::buttonLabels[7] = {
   "NewCampaign",
   "ContinueCampaign",
   "SinglePlayer",
   "Hotseat",
   "PBEM",
   "PBP",
   NULL
};


StartMultiplayerGame::StartMultiplayerGame(PG_MessageObject* c): ConfigurableWindow( NULL, PG_Rect::null, "newmultiplayergame", false ), newMap(NULL), page(ModeSelection), mode ( 0 ), 
   mapParameterEditor(NULL), mapParameterEditorParent(NULL),
   allianceSetup(NULL), allianceSetupParent(NULL),
   playerSetup(NULL), playerSetupParent(NULL)
{
    setup();
    sigClose.connect( SigC::slot( *this, &StartMultiplayerGame::QuitModal ));
    
    PG_RadioButton* b1 = dynamic_cast<PG_RadioButton*>(FindChild(buttonLabels[0], true ));
    if ( b1 ) 
      b1->SetPressed();

    PG_Button* next = dynamic_cast<PG_Button*>(FindChild("next", true ));
    if ( next )
      next->sigClick.connect( SigC::slot( *this, &StartMultiplayerGame::nextPage ));

    PG_Button* start = dynamic_cast<PG_Button*>(FindChild("start", true ));
    if ( start )
      start->sigClick.connect( SigC::slot( *this, &StartMultiplayerGame::start ));
      
    PG_Button* qstart = dynamic_cast<PG_Button*>(FindChild("quickstart", true ));
    if ( qstart )
      qstart->sigClick.connect( SigC::slot( *this, &StartMultiplayerGame::quickstart ));
      
    showPage();
    showButtons(false,false,true);
}


StartMultiplayerGame::~StartMultiplayerGame()
{
   if ( newMap )
      delete newMap;
}      

bool StartMultiplayerGame::Apply()
{
   switch ( page )  {
      case FilenameSelection: {
            if ( !filename.empty() )
               if ( exist( filename )) {
                  newMap = mapLoadingExceptionChecker( filename, MapLoadingFunction( tmaploaders::loadmap ));
                  if ( newMap ) 
                     return true;
                }
               
         }
         break;
      case PlayerSetup: 
            if ( playerSetup->Valid() ) {
               playerSetup->Apply();
               return true;
            }   
            break;       
              
      case AllianceSetup: 
         allianceSetup->Apply();
         return true;
         
      case MapParameterEditor: 
            if ( mapParameterEditor->Valid() ) {
               mapParameterEditor->Apply();
               return true;
            }   
            break;   
              
      default: 
           break;
   }
   
   return false;
}

bool StartMultiplayerGame::nextPage(PG_Button* button)
{
   int oldpage = page;
   switch ( page )  {
      case ModeSelection: {
            int i = 0;
            while ( buttonLabels[i] ) {
               PG_RadioButton* b = dynamic_cast<PG_RadioButton*>(FindChild(buttonLabels[i], true ));
               if ( b && b->GetPressed() ) {
                  mode = i;
                  break;
               }
               ++i;
            }   
            if ( mode == 1 )
               page = PasswordSearch;
            else
               page = FilenameSelection;
         }
         break;
         
      case FilenameSelection: 
            if ( Apply() )
               page = PlayerSetup;
            break;
      case PlayerSetup: 
            if ( Apply() )
               page = AllianceSetup;
            break;       
              
      case AllianceSetup: 
            if ( Apply() )
               page = MapParameterEditor;
            break;
              
      case MapParameterEditor: 
            if ( Apply() )
               page = MultiPlayerOptions;
            break;   
            
      default: 
           break;
   }
   
   if ( oldpage != page ) {
      if ( page == PlayerSetup && playerSetupParent ) {
         delete playerSetup;
         if ( mode == Skirmish )
            playerSetup = new PlayerSetupWidget( newMap, PlayerSetupWidget::AllEditableSinglePlayer, playerSetupParent, PG_Rect( 0, 0, playerSetupParent->Width(), playerSetupParent->Height() ));
         else  
            playerSetup = new PlayerSetupWidget( newMap, PlayerSetupWidget::AllEditable, playerSetupParent, PG_Rect( 0, 0, playerSetupParent->Width(), playerSetupParent->Height() ));
      }   
      
      if ( page == AllianceSetup && allianceSetupParent ) {
         delete allianceSetup;
         allianceSetup = new AllianceSetupWidget( newMap, true, allianceSetupParent, PG_Rect( 0, 0, allianceSetupParent->Width(), allianceSetupParent->Height() ));
      }   

      if ( page == MapParameterEditor && mapParameterEditorParent ) {
         delete mapParameterEditor;
         mapParameterEditor = new GameParameterEditorWidget( newMap, mapParameterEditorParent, PG_Rect( 0, 0, mapParameterEditorParent->Width(), mapParameterEditorParent->Height() ));
      }   
         
      showPage();
      return true;
   } else
      return false;
}

void StartMultiplayerGame::showPage()
{
   for ( int i = 0; i < 10; ++i ) {
      ASCString name = "Page" + ASCString::toString(i);
      if ( page == i )
         show( name );
      else   
         hide( name );
   }
   
   switch ( page ) {
      case FilenameSelection: 
         showButtons(false,true,true);
         break;
         
      case MapParameterEditor: 
         if ( mode != Hotseat && mode != PBEM )
            showButtons(true,false,false);
         else   
            showButtons(false,true,true);
         break;
      case MultiPlayerOptions: 
         showButtons(true,false,false);
         break;
      default:
         break;   
    }     
}



void StartMultiplayerGame::userHandler( const ASCString& label, PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams )
{
   if ( label == "FileList" ) {
      FileSelectionItemFactory* factory = new FileSelectionItemFactory( mapextension );
      factory->filenameSelectedKeyb.connect ( SigC::slot( *this, &StartMultiplayerGame::fileNameSelected ));
      factory->filenameSelectedMouse.connect ( SigC::slot( *this, &StartMultiplayerGame::fileNameMarked ));
      factory->filenameMarked.connect   ( SigC::slot( *this, &StartMultiplayerGame::fileNameMarked ));
      new ItemSelectorWidget( parent, PG_Rect(0, 0, parent->Width(), parent->Height()), factory );
   }
   
   if ( label == "PlayerSetup" ) 
      playerSetupParent = parent;
      
   if ( label == "AllianceSetup" ) 
      allianceSetupParent = parent;
      
   if ( label == "GameParameters" )
      mapParameterEditorParent = parent; 
  
}


bool StartMultiplayerGame::start()
{
   if ( !newMap ) 
      newMap = mapLoadingExceptionChecker( filename, MapLoadingFunction( tmaploaders::loadmap ));
     
   if ( !newMap )
      return false;
     
     
   if ( mode == Skirmish ) {
      bool humanFound = false;
      for ( int i = 0; i < newMap->getPlayerCount(); ++i )
         if ( newMap->player[i].exist() )
            if ( newMap->player[i].stat == Player::human )
               if ( humanFound )
                  newMap->player[i].stat = Player::computer;
               else
                  humanFound = true;   
   }

   if ( mode == PBEM || mode == Hotseat ) {
      int humanNum = 0;
      for ( int i = 0; i < newMap->getPlayerCount(); ++i )
         if ( newMap->player[i].exist() )
            if ( newMap->player[i].stat == Player::human )
               ++humanNum;   
            
      if ( humanNum <= 1 )
         for ( int i = 0; i < newMap->getPlayerCount(); ++i )
            if ( newMap->player[i].exist() )
               if ( newMap->player[i].stat == Player::computer ) {
                  newMap->player[i].stat = Player::human;
                  ++humanNum;
               }
            
            
      if ( humanNum <= 1 ) {
         MessagingHub::Instance().error("Not enough players on map for multiplayer game!");
         return false;
      }   
   }
   
  
   delete actmap;
   actmap = newMap;
   newMap = NULL;
   QuitModal();
   Hide();
   next_turn();
   computeview( actmap );
   displaymap();
   updateFieldInfo();
   moveparams.movestatus = 0;
   return true;
}



void startMultiplayerGame(PG_MessageObject* c)
{
    StartMultiplayerGame smg(c);
    smg.Show();
    smg.RunModal();
}


