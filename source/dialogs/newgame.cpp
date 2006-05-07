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
#include "emailsetup.h"
#include "../loaders.h"
#include "../gamemap.h"
#include "../paradialog.h"
#include "../turncontrol.h"
#include "../viewcalculation.h"
#include "../networkinterface.h"
#include "../network.h"
#include "../spfst.h"
#include "../windowing.h"
#include "../controls.h"

class GameParameterEditorWidget;


class StartMultiplayerGame: public ConfigurableWindow {
   private:
      bool success;
      GameMap* newMap;
      
      enum Pages { ModeSelection = 1, FilenameSelection, PlayerSetup, EmailSetup, AllianceSetup, MapParameterEditor, MultiPlayerOptions, PasswordSearch }; 
      Pages page;
     
      enum Mode { NewCampagin, ContinueCampaign, Skirmish, Hotseat, PBEM, PBP };
      int mode;
      
      static const char* buttonLabels[];
      
      ASCString filename;
      ASCString PBEMfilename;
      bool replay;
      bool supervisorEnabled;
      Password supervisorPassword;
      
      GameParameterEditorWidget* mapParameterEditor;
      PG_Widget* mapParameterEditorParent;
      
      AllianceSetupWidget* allianceSetup;
      PG_Widget* allianceSetupParent;
      
      PlayerSetupWidget* playerSetup;
      PG_Widget* playerSetupParent;
      
      EmailSetupWidget* emailSetup;
      PG_Widget* emailSetupParent;
   
      bool nextPage(PG_Button* button = NULL);
      void showPage();
      bool checkPlayerStat();
      
      ASCString getDefaultPBEM_Filename();
      void setupNetwork();
      
      
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


      void showSupervisorWidgets()
      {
         if ( page == MultiPlayerOptions )
            if ( supervisorEnabled )
               show( "SuperVisor" );
            else
               hide( "SuperVisor" );

      }
      
      bool togglesupervisor( bool on )
      {
         supervisorEnabled = on;
         showSupervisorWidgets();
         return true;
      }
      
      void showButtons( bool start, bool quickstart, bool next )
      {
         assert ( start != next );
         assert ( start != quickstart );
         
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
      
      bool eventKeyDown(const SDL_KeyboardEvent *key){
         if(key->keysym.sym == SDLK_ESCAPE) {
            if ( page <= 2 )
               QuitModal();
         }
         return true;
      }
      
   public:
      StartMultiplayerGame(PG_MessageObject* c);
      ~StartMultiplayerGame();

      bool getSuccess()
      {
         return success;
      }
   
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


StartMultiplayerGame::StartMultiplayerGame(PG_MessageObject* c): ConfigurableWindow( NULL, PG_Rect::null, "newmultiplayergame", false ), success(false), newMap(NULL), page(ModeSelection), mode ( 0 ), replay(true), supervisorEnabled(false),
   mapParameterEditor(NULL), mapParameterEditorParent(NULL),
   allianceSetup(NULL), allianceSetupParent(NULL),
   playerSetup(NULL), playerSetupParent(NULL),
   emailSetup(NULL), emailSetupParent(NULL)
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
      
    PG_CheckButton* supervisor = dynamic_cast<PG_CheckButton*>(FindChild("SupervisorOn", true ));
    if ( supervisor ) {
       if ( supervisorEnabled )
         supervisor->SetPressed();
       else
          supervisor->SetUnpressed();
       supervisor->sigClick.connect( SigC::slot( *this, &StartMultiplayerGame::togglesupervisor ));
    }

    PG_LineEdit* s1 = dynamic_cast<PG_LineEdit*>( FindChild( "SupervisorPlain", true ));
    if ( s1 )
       s1->SetPassHidden ('*');
    
    
    PG_CheckButton* cb = dynamic_cast<PG_CheckButton*>( FindChild( "Replay", true ));
    if ( cb )
       if ( replay )
         cb->SetPressed( );
       else
         cb->SetUnpressed();

    
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
                  if ( newMap ) {
                     if ( checkPlayerStat() )
                        return true;
                     else {
                        delete newMap;
                        newMap = NULL;
                        return false;
                     }      
                  }
                }
               
         }
         break;
      case PlayerSetup: 
            if ( playerSetup->Valid() ) {
               playerSetup->Apply();
               return true;
            }   
            break;       
      case EmailSetup:
            emailSetup->Apply();      
            return true;
              
      case AllianceSetup: 
         allianceSetup->Apply();
         return true;
         
      case MapParameterEditor: 
            if ( mapParameterEditor->Valid() ) {
               mapParameterEditor->Apply();
               return true;
            }   
            break;   
      case MultiPlayerOptions:
         {
            PG_CheckButton* cb = dynamic_cast<PG_CheckButton*>( FindChild( "Replay", true ));
            if ( cb )
               replay = cb->GetPressed();
            
            PG_LineEdit* le = dynamic_cast<PG_LineEdit*>( FindChild( "Filename", true ));
            if ( le &&  !le->GetText().empty() ) 
               PBEMfilename = le->GetText();
               
            if ( supervisorEnabled ) {
               PG_LineEdit* s1 = dynamic_cast<PG_LineEdit*>( FindChild( "SupervisorPlain", true ));
               PG_LineEdit* s2 = dynamic_cast<PG_LineEdit*>( FindChild( "SupervisorEnc", true ));

               if ( s1 && s2 && !s1->GetText().empty() && !s2->GetText().empty()) {
                  Password pwd;
                  pwd.setUnencoded( s1->GetText() );
                  
                  Password pwd2;
                  pwd2.setEncoded( s2->GetText() );
                  if ( pwd != pwd2 ) {
                     warning ( "Passwords don't match!");
                     return false;
                  }
               }

               if ( s1 && !s1->GetText().empty() )
                  supervisorPassword.setUnencoded( s1->GetText() );

               if ( s2 && !s2->GetText().empty() )
                  supervisorPassword.setEncoded( s2->GetText() );
            }
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
               if ( mode == PBEM || mode == PBP )
                  page = EmailSetup;
               else
                  page = AllianceSetup;
                  
            break;       
      case EmailSetup: 
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

      if ( page == EmailSetup && emailSetupParent ) {
         delete emailSetup;
         emailSetup = new EmailSetupWidget( newMap, -1, emailSetupParent, PG_Rect( 0, 0, emailSetupParent->Width(), emailSetupParent->Height() ));
      }   
      
      if ( page == MultiPlayerOptions ) {
         PG_LineEdit* le = dynamic_cast<PG_LineEdit*>( FindChild( "Filename", true ));
         PBEMfilename = getDefaultPBEM_Filename();
         if ( le )
            le->SetText( PBEMfilename );         
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
   showSupervisorWidgets();
   
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
      
   if ( label == "EmailSetup" )
      emailSetupParent = parent;
  
}

bool StartMultiplayerGame::checkPlayerStat()
{
   if ( mode == Skirmish ) {
      bool humanFound = false;
      for ( int i = 0; i < newMap->getPlayerCount(); ++i )
         if ( newMap->player[i].exist() )
            if ( newMap->player[i].stat == Player::human )
               if ( humanFound )
                  newMap->player[i].stat = Player::computer;
               else
                  humanFound = true;   
                  
      if ( !humanFound )            
         for ( int i = 0; i < newMap->getPlayerCount(); ++i )
            if ( newMap->player[i].exist() )
               if ( newMap->player[i].stat == Player::computer ) {
                  newMap->player[i].stat = Player::human;
                  humanFound = true;   
               }   
               
      if ( !humanFound ) {
         MessagingHub::Instance().error("Map has no players!");
         return false;
      }   
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
   return true;
}

ASCString StartMultiplayerGame::getDefaultPBEM_Filename()
{
   ASCString filename = newMap->preferredFileNames.mapname[0];
   if( filename.empty() )
      filename = "game";   
   
   if ( filename.find( '.') != ASCString::npos )
      filename.erase( filename.find( '.'));
      
   filename += "-$p-$t";
   
   return filename;
}


void StartMultiplayerGame::setupNetwork()
{
   if ( mode == PBP || mode == PBEM ) {
      if ( newMap && !newMap->network ) {
         FileTransfer* ft = new FileTransfer();
         newMap->network = ft;
         
         if ( !PBEMfilename.empty() )
            ft->setup( PBEMfilename );
         else   
            ft->setup( getDefaultPBEM_Filename() );
      }
   }
}


bool StartMultiplayerGame::start()
{
   if ( !Apply() )
      return false;
   
   if ( !newMap ) 
      newMap = mapLoadingExceptionChecker( filename, MapLoadingFunction( tmaploaders::loadmap ));
     
   if ( !newMap )
      return false;
   
   if ( !checkPlayerStat() )
      return false;

   if ( replay )
      newMap->replayinfo = new GameMap::ReplayInfo;
   else
      if ( newMap->replayinfo ) {
         delete newMap->replayinfo;
         newMap->replayinfo = NULL;
      }

   if ( supervisorEnabled )
      newMap->supervisorpasswordcrc = supervisorPassword;
      
   
   setupNetwork();
  
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
   success = true;
   return true;
}



bool startMultiplayerGame()
{
    StartMultiplayerGame smg(NULL);
    smg.Show();
    smg.RunModal();
    return smg.getSuccess();
}


