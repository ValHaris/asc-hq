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

class GameParameterEditorWidget;

class StartMultiplayerGame: public ConfigurableWindow {
   private:

      tmap* newMap;
      
      int page;
     
      // enum Mode { NewCampagin, ContinueCampaign, Skirmish, PBP, Hotseat, PBEM } mode;
      int mode;
      
      static const char* buttonLabels[];
      
      ASCString filename;
      
      GameParameterEditorWidget* mapParameterEditor;
   
      bool nextPage(PG_Button* button = NULL);
      void showPage();
      
      void fileNameSelected( const ASCString& filename )
      {
         this->filename = filename;
      };   
      
   protected:   
      void userHandler( const ASCString& label, PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams ); 
      
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

StartMultiplayerGame::StartMultiplayerGame(PG_MessageObject* c): ConfigurableWindow( NULL, PG_Rect::null, "newmultiplayergame", false ), newMap(NULL), page(1), mode ( 0 ), mapParameterEditor(NULL)
{
    setup();
    sigClose.connect( SigC::slot( *this, &StartMultiplayerGame::QuitModal ));
    
    PG_RadioButton* b1 = dynamic_cast<PG_RadioButton*>(FindChild(buttonLabels[0], true ));
    if ( b1 ) 
      b1->SetPressed();

    PG_Button* next = dynamic_cast<PG_Button*>(FindChild("next", true ));
    if ( next )
      next->sigClick.connect( SigC::slot( *this, &StartMultiplayerGame::nextPage ));
      
    showPage();
        
}


StartMultiplayerGame::~StartMultiplayerGame()
{
   if ( newMap )
      delete newMap;
}      


bool StartMultiplayerGame::nextPage(PG_Button* button)
{
   // int oldpage = page;
   switch ( page )  {
      case 1: {
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
               page = 7;
            else
               ++page;
         }
         break;
      case 2: {
            if ( !filename.empty() )
               // if ( exists( filename )) {
                  ++page;
               // }
         }
         break;
      case 3: 
      case 4: ++page;
              break;
              
      case 5: if ( mapParameterEditor->Valid() )
                 ++page;
              break;   
   }
   
   showPage();
   return true;
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
}



void StartMultiplayerGame::userHandler( const ASCString& label, PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams )
{
   if ( label == "FileList" ) {
      FileSelectionItemFactory* factory = new FileSelectionItemFactory( mapextension );
      factory->filenameSelected.connect ( SigC::slot( *this, &StartMultiplayerGame::fileNameSelected ));
      factory->filenameMarked.connect   ( SigC::slot( *this, &StartMultiplayerGame::fileNameSelected ));
      new ItemSelectorWidget( parent, PG_Rect(0, 0, parent->Width(), parent->Height()), factory );
   }
   if ( label == "PlayerSetup" ) {
      new PlayerSetupWidget( actmap, true, parent, PG_Rect( 0, 0, parent->Width(), parent->Height() ));
   }
   if ( label == "AllianceSetup" ) {
      new AllianceSetupWidget( actmap, true, parent, PG_Rect( 0, 0, parent->Width(), parent->Height() ));
   }
   if ( label == "GameParameters" ) {
      mapParameterEditor = new GameParameterEditorWidget( actmap, parent, PG_Rect( 0, 0, parent->Width(), parent->Height() ));
   }
}



void startMultiplayerGame(PG_MessageObject* c)
{
    StartMultiplayerGame smg(c);
    smg.Show();
    smg.RunModal();
}

