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
#include "../spfst.h"
#include "../windowing.h"
#include "../controls.h"
#include "../sg.h"
#include "../gamedlg.h"
#include "../network/simple_file_transfer.h"
#include "../network/pbem-server.h"

class GameParameterEditorWidget;


class StartMultiplayerGame: public ConfigurableWindow {
   private:
      bool startButton;
      bool success;
      GameMap* newMap;
      
      enum Pages { ModeSelection = 1, FilenameSelection, PlayerSetup, EmailSetup, AllianceSetup, MapParameterEditor, MultiPlayerOptions, PasswordSearch, PBEMServerSetup }; 
      Pages page;
     
      enum Mode { NewCampagin, ContinueCampaign, Skirmish, Hotseat, PBEM, PBP, PBEM_Server };
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
      
      PBEMServer* pbemserver;
      
      void loadPBEMServerDefaults();
      
      void fileNameSelected( const ASCString& filename )
      {
         this->filename = filename;
         if ( startButton ) 
            start();
         else
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
         startButton = start;

         assert ( start != next );
         assert ( !(start && quickstart) );
         
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


      GameMap* searchForMap( const ASCString& password );

   protected:   
      void userHandler( const ASCString& label, PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams ); 
      bool start();
      bool Apply();
      
      bool eventKeyDown(const SDL_KeyboardEvent *key){
         if(key->keysym.sym == SDLK_ESCAPE) {
            if ( page <= 2 )
               QuitModal();
            return true;
         }
         return false;
      }
      
   public:
      StartMultiplayerGame(PG_MessageObject* c);
      ~StartMultiplayerGame();

      bool getSuccess()
      {
         return success;
      }
   
};

const char* StartMultiplayerGame::buttonLabels[8] = {
   "NewCampaign",
   "ContinueCampaign",
   "SinglePlayer",
   "Hotseat",
   "PBEM",
   "PBP",
   "PBEMServer",
   NULL
};


StartMultiplayerGame::StartMultiplayerGame(PG_MessageObject* c): ConfigurableWindow( NULL, PG_Rect::null, "newmultiplayergame", false ), startButton(false), success(false), newMap(NULL), page(ModeSelection), mode ( 0 ), replay(true), supervisorEnabled(false),
   mapParameterEditor(NULL), mapParameterEditorParent(NULL),
   allianceSetup(NULL), allianceSetupParent(NULL),
   playerSetup(NULL), playerSetupParent(NULL),
   emailSetup(NULL), emailSetupParent(NULL),
   pbemserver(NULL)
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
    
    loadPBEMServerDefaults();
}


StartMultiplayerGame::~StartMultiplayerGame()
{
   if ( newMap )
      delete newMap;
   
   if ( pbemserver )
      delete pbemserver;
}      

bool StartMultiplayerGame::Apply()
{
   switch ( page )  {
      case FilenameSelection: {
            if ( !filename.empty() )
               if ( exist( filename )) {
                  delete newMap;
                  newMap = mapLoadingExceptionChecker( filename, MapLoadingFunction( tmaploaders::loadmap ));
                  if ( newMap ) {
                     if ( mode != NewCampagin && mode != ContinueCampaign )
                        newMap->campaign.avail = false;
                     else
                        replay = false;



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
      case PBEMServerSetup:
          {
            if ( !pbemserver )
               pbemserver = new PBEMServer();
            
            // @PBEMSERVER
            // transferring the data from the dialog to the class that will be associated with the game
            
            
            PG_LineEdit* le = dynamic_cast<PG_LineEdit*>( FindChild( "Server", true ));
            if ( le &&  !le->GetText().empty() ) 
               pbemserver->setServerAddress ( le->GetText());
            
         }
         return true;
      case PasswordSearch: {
               PG_LineEdit* pw = dynamic_cast<PG_LineEdit*>( FindChild( "MapPassword", true ));
               if ( pw ) {
                  if ( pw->GetText().empty() ) {
                     errorMessage("please enter a password");
                     return false;
                  }
                  
                  GameMap* map = searchForMap( pw->GetText() );
                  if ( !map  ) {
                     errorMessage("No map found");
                     return false;
                  }
                  newMap = map;
                  start();
               }
                           
         }
         break;

      default: 
           break;
   }
   
   return false;
}

GameMap* StartMultiplayerGame::searchForMap( const ASCString& password )
{
   StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "please wait, searching for map...");

   tfindfile ff ( mapextension );
   string filename = ff.getnextname();
   while( !filename.empty() ) {
       try {
          tmaploaders loader;
          GameMap* map = loader.loadmap( filename );
          if ( map->codeWord == password )
             return map;
          else
             delete map;
      }
      catch ( ASCexception ) {
      } /* endcatch */

      filename = ff.getnextname();
   }
   return NULL;
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
            if ( mode == ContinueCampaign )
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
               if ( mode == PBEM || mode == PBP || mode == PBEM_Server )
                  page = EmailSetup;
               else
                  page = AllianceSetup;
                  
            break;       
      case EmailSetup: 
            if ( Apply() ) {
               if ( mode == PBEM_Server )
                  page = PBEMServerSetup;
               else
                  page = AllianceSetup;
            }
            break;
              
      case PBEMServerSetup:
            if ( Apply() ) {
               page = AllianceSetup;
            }
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
         if ( mode == PBP )
            newMap->setgameparameter(cgp_superVisorCanSaveMap,1);
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

void StartMultiplayerGame::loadPBEMServerDefaults()
{
      if ( !pbemserver )
         pbemserver = new PBEMServer();
      
      // @PBEMSERVER
      // transferring the data from the dialog to the class that will be associated with the game
   
      PG_LineEdit* le = dynamic_cast<PG_LineEdit*>( FindChild( "Server", true ));
      if ( le )
         le->SetText( PBEMServer::getDefaultServerAddress() );
   
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
         if ( mode == NewCampagin || mode == ContinueCampaign )
            showButtons(true,false,false);
         else
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
      case PasswordSearch:
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
   ASCString msg;
   if ( mode == NewCampagin || mode == ContinueCampaign ) {
      bool humanFound = false;
      for ( int i = 0; i < newMap->getPlayerCount(); ++i )
         if ( newMap->player[i].exist() )
            if ( newMap->player[i].stat == Player::human )
               humanFound = true;

      if ( !humanFound ) {
         warning("no human players on map!");
         return false;
      }
      return true;
   }

   if ( mode == Skirmish ) {
      bool humanFound = false;
      for ( int i = 0; i < newMap->getPlayerCount(); ++i )
         if ( newMap->player[i].exist() )
            if ( newMap->player[i].stat == Player::human || newMap->player[i].stat == Player::supervisor )
               if ( humanFound ) {
                  newMap->player[i].stat = Player::computer;
                  msg += newMap->player[i].getName() + " has been switch to AI\n";
               } else
                  humanFound = true;   
                  
      if ( !humanFound )            
         for ( int i = 0; i < newMap->getPlayerCount(); ++i )
            if ( newMap->player[i].exist() )
               if ( newMap->player[i].stat == Player::computer ) {
                  newMap->player[i].stat = Player::human;
                  msg += newMap->player[i].getName() + " has been switch to human\n";
                  humanFound = true;   
                  break;
               }   

      bool aiFound = false;
      for ( int i = 0; i < newMap->getPlayerCount(); ++i )
         if ( newMap->player[i].exist() )
            if ( newMap->player[i].stat == Player::computer )
               aiFound = true;   
                  
      if ( !aiFound )            
         for ( int i = 0; i < newMap->getPlayerCount(); ++i )
            if ( newMap->player[i].exist() )
               if ( newMap->player[i].stat == Player::off ) {
                  newMap->player[i].stat = Player::computer;
                  msg += newMap->player[i].getName() + " has been switch to AI\n";
                  aiFound = true;   
                  break;
               }   


       /*
      if ( !aiFound ) {
         MessagingHub::Instance().error("Map has no opponents!");
         return false;
      }   
      */
   }

   if ( mode == PBEM || mode == Hotseat ) {
      int humanNum = 0;
      for ( int i = 0; i < newMap->getPlayerCount(); ++i )
         if ( newMap->player[i].exist() )
            if ( newMap->player[i].stat == Player::human || newMap->player[i].stat == Player::supervisor )
               ++humanNum;   
            
      if ( humanNum <= 1 )
         for ( int i = 0; i < newMap->getPlayerCount(); ++i )
            if ( newMap->player[i].exist() )
               if ( newMap->player[i].stat == Player::computer || newMap->player[i].stat == Player::off ) {
                  newMap->player[i].stat = Player::human;
                  msg += newMap->player[i].getName() + " has been switched to human\n";
                  ++humanNum;
               }
            
            
      if ( humanNum <= 1 ) {
         MessagingHub::Instance().error("Not enough players on map for multiplayer game!");
         return false;
      }   
   }
   
   if ( mode == PBP  ) {
      newMap->player[0].stat = Player::supervisor;
      
      int humanNum = 0;
      for ( int i = 1; i < newMap->getPlayerCount(); ++i )
         if ( newMap->player[i].exist() )
            if ( newMap->player[i].stat == Player::human || newMap->player[i].stat == Player::supervisor )
               ++humanNum;   
            
      if ( humanNum < 1 )
         for ( int i = 1; i < newMap->getPlayerCount(); ++i )
            if ( newMap->player[i].exist() )
               if ( newMap->player[i].stat == Player::computer || newMap->player[i].stat == Player::off ) {
                  newMap->player[i].stat = Player::human;
                  msg += newMap->player[i].getName() + " has been switched to human\n";
                  ++humanNum;
               }
   }
   
  // if ( !msg.empty() )
  //    infoMessage( msg );
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
      if ( newMap && (!newMap->network || newMap->network->getMechanismID() != FileTransfer::mechanismID()) ) {
         
         delete newMap->network;
         
         FileTransfer* ft = new FileTransfer();
         newMap->network = ft;
         
         if ( !PBEMfilename.empty() )
            ft->setup( PBEMfilename );
         else   
            ft->setup( getDefaultPBEM_Filename() );
      }
   } else
   if ( mode == PBEM_Server ) {
      if ( newMap ) {
         delete newMap->network;
         newMap->network = pbemserver;
         
         pbemserver = NULL;
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
   
//   if ( !checkPlayerStat() )
//      return false;

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
   computeview( actmap );
   hookGuiToMap(actmap);
  
   next_turn();
   displaymap();
   updateFieldInfo();
   moveparams.movestatus = 0;
   success = true;

   if ( mode == PBP || mode == PBEM ) 
      sendGameParameterAsMail ( actmap );

   
   return true;
}



bool startMultiplayerGame()
{
   bool res = false;
   try {
    StartMultiplayerGame smg(NULL);
    smg.Show();
    smg.RunModal();
    res = smg.getSuccess();
    smg.Hide();
   }
   catch ( ShutDownMap sdm ) {
      warning("No players found on map");
      throw sdm;
   }
    /*
    if ( res )
       actmap->sigPlayerUserInteractionBegins( actmap->player[actmap->actplayer] );
    */
    
    return res;
}


