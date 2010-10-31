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

#include <pgpropertyeditor.h>
#include <pgpropertyfield_integer.h>
#include <pgpropertyfield_intdropdown.h>
#include <pgpropertyfield_dropdown.h>
#include <pgpropertyfield_checkbox.h>
#include <pgpropertyfield_string.h>


#include "selectionwindow.h"
#include "sigc++/retype.h"
#include "editmapparam.h"
#include "fileselector.h"
#include "../ascstring.h"

#include "../textfileparser.h"
#include "../textfile_evaluation.h"

#include "editgameoptions.h"


bool GetVideoModes::comparator( const ModeRes& a, const ModeRes& b )
{
   if ( a.first == 0 )
      return true;
   
   if ( b.first == 0 )
      return false;
   
   if ( a.first > b.first ) {
      return true;
   } else {
      if ( a.first < b.first )
         return false;
      else {
         if ( a.second > b.second )
            return true;
         else {
            return false;
         }
      }

   }
}

/*
bool GetVideoModes::comparator( const ModeRes& a, const ModeRes& b )
{
   if ( a.first > b.first ) {
      return 1;
   } else {
      if ( a.first < b.first )
         return -1;
      else {
         if ( a.second > b.second )
            return 1;
         else {
            if ( a.second < b.second )
               return -1;
            else
               return 0;
         }
      }

   }
}
*/

GetVideoModes::GetVideoModes() 
{
   int i;


   SDL_PixelFormat format;
   format.palette = NULL;
   format.BitsPerPixel = 32;
   format.BytesPerPixel = 4;
   format.Rloss = format.Gloss = format.Bloss = format.Aloss = 0;
   format.Rshift = 0;
   format.Gshift = 8;
   format.Bshift = 16;
   format.Ashift = 24;
   format.Rmask = 0xff;
   format.Gmask = 0xff00;
   format.Bmask = 0xff0000;
   format.Amask = 0xff000000;
   format.colorkey = 0;
   format.alpha = 0;
   
   
   /* Get available fullscreen/hardware modes */
   modes=SDL_ListModes(&format, SDL_FULLSCREEN);


   listedmodes.push_back( make_pair( 0, 0));
   
   /* Check is there are any modes available */
   if(modes == (SDL_Rect **)0){
      return;
   }
   
   /* Check if our resolution is restricted */
   if(modes == (SDL_Rect **)-1){
      warningMessage("All resolutions available.\n");
      return;
   }
   else{
      for(i=0;modes[i];++i) {

         if ( find ( listedmodes.begin(), listedmodes.end(), make_pair( int(modes[i]->w), int(modes[i]->h ))) != listedmodes.end() )
            continue;
         
         if ( modes[i]->w >= 800 && modes[i]->h >= 600 ) {
            listedmodes.push_back( make_pair( int(modes[i]->w), int(modes[i]->h )));
         }
      }
      sort ( listedmodes.begin(), listedmodes.end(), &GetVideoModes::comparator );

      for ( vector <ModeRes > ::iterator i = listedmodes.begin(); i != listedmodes.end(); ++i ) {
         ASCString s;
         if (  i->first )
            s.format( "%d*%d", i->first, i->second );
         else 
            s = "graphic mode not listed"; 
         
         list.push_back ( s );
      }

   }
   return;
};

int GetVideoModes::getx( int index ) {
   return listedmodes.at(index).first;
};

int GetVideoModes::gety( int index ) {
   return listedmodes.at(index).second;
};

int GetVideoModes::findmodenum( int x, int y ) {
   for ( int j = 0; j < listedmodes.size(); ++j )
      if ( listedmodes[j].first == x && listedmodes[j].second == y )
         return j;
   return 0;
}
      
      
GetVideoModes::ModeRes GetVideoModes::getBest() 
{
   ModeRes res = make_pair(-1,-1);
   
   for ( int j = 0; j < listedmodes.size(); ++j )
      if ( listedmodes[j].first >= res.first && listedmodes[j].second >= res.second )
         res = listedmodes[j];
   
   return res;
}      



const char* mouseButtonNames[] = { "None", "Left", "Center", "Right", "4", "5", NULL };

const char* infoPanelNames[] = { "None", "Left (not recommended)", "Right", "Left+Right", NULL };

class EditGameOptions : public ASC_PG_Dialog {
   private:
      PG_PropertyEditor* propertyEditor;
      
      GetVideoModes vmodes;
      ASCString defaultPassword;
      
      int videoMode;
      bool ascmain;
      
      bool ok()
      {
         if ( propertyEditor->Apply() ) {

            int x = vmodes.getx( videoMode );
            int y = vmodes.gety( videoMode );

            bool warn = false;
            bool fullscreen;
            
            
            if ( ascmain ) {
               if ( (x != CGameOptions::Instance()->xresolution || y != CGameOptions::Instance()->yresolution) && x && y  ) {
                  warn = true;
                  CGameOptions::Instance()->xresolution = x;
                  CGameOptions::Instance()->yresolution = y;
               }

               fullscreen = !CGameOptions::Instance()->forceWindowedMode;

            } else {
               if ( (x != CGameOptions::Instance()->mapeditor_xresolution || y != CGameOptions::Instance()->mapeditor_yresolution) && x && y ) {
                  warn = true;
                  CGameOptions::Instance()->mapeditor_xresolution = x;
                  CGameOptions::Instance()->mapeditor_yresolution = y;
               }
               fullscreen = !CGameOptions::Instance()->mapeditWindowedMode;
            }

            if ( warn )
               infoMessage( "The new graphic settings will be active after you restart ASC");

            if ( getPGApplication().isFullscreen() != fullscreen ) 
               getPGApplication().toggleFullscreen();
            
            
            CGameOptions::Instance()->setChanged();
            if ( !defaultPassword.empty() && defaultPassword.find_first_not_of('*') != ASCString::npos ) {
               Password p;
               p.setUnencoded ( defaultPassword );
               CGameOptions::Instance()->defaultPassword = p.toString();
            }
            
            quitModalLoop(0);

            return true;
         } else
            return false;
      }

   public:
      EditGameOptions( PG_Widget* parent, bool mainApp ) : ASC_PG_Dialog( parent, PG_Rect( 50, 50, 500, 550 ), "Edit Map Parameters"), videoMode(0), ascmain( mainApp )
      {
         CGameOptions* o = CGameOptions::Instance();

         if ( !o->defaultPassword.empty() )
            defaultPassword = "******";

         
         if ( mainApp ) 
            videoMode = vmodes.findmodenum( CGameOptions::Instance()->xresolution, CGameOptions::Instance()->yresolution );
         else
            videoMode = vmodes.findmodenum( CGameOptions::Instance()->mapeditor_xresolution, CGameOptions::Instance()->mapeditor_yresolution );
            
         propertyEditor = new ASC_PropertyEditor( this, PG_Rect( 10, GetTitlebarHeight(), Width() - 20, Height() - GetTitlebarHeight() - 50 ), "PropertyEditor", 70 );

         new PG_PropertyField_Checkbox<bool>( propertyEditor, "Direct Movement", &o->fastmove );
         new PG_PropertyField_Integer<int>( propertyEditor , "Movement Speed (1/100 sec)", &o->movespeed );
         new PG_PropertyField_Checkbox<bool>( propertyEditor, "Confirm EndOfTurn", &o->endturnquestion );
         new PG_PropertyField_Checkbox<bool>( propertyEditor, "Units shaded after movement", &o->units_gray_after_move );
         new PG_PropertyField_Integer<int>( propertyEditor , "Attack Dialog PreWait (1/100 sec)", &o->attackspeed1 );
         new PG_PropertyField_Integer<int>( propertyEditor , "Attack Dialog Animate (1/100 sec)", &o->attackspeed2 );
         new PG_PropertyField_Integer<int>( propertyEditor , "Attack Dialog PostWait 3 (1/100 sec)", &o->attackspeed3 );
         if ( mainApp ) {
            new PG_PropertyField_Checkbox<bool>( propertyEditor, "Game running Fullscreen", &o->forceWindowedMode, true );
            new PG_PropertyField_Checkbox<bool>( propertyEditor, "Produce Ammo when refuelling", &o->autoproduceammunition );
            new PG_PropertyField_IntDropDown<int>( propertyEditor, "InfoPanels", &o->panelColumns, infoPanelNames );
            new PG_PropertyField_Integer<int>( propertyEditor, "Aircraft Crash Warning Time", &o->aircraftCrashWarningTime );
         } else {
            new PG_PropertyField_Checkbox<bool>( propertyEditor, "MapEd running Fullscreen", &o->mapeditWindowedMode, true );
            new PG_PropertyField_Checkbox<bool>( propertyEditor, "Modal Selection Window", &o->maped_modalSelectionWindow );
         }

#ifdef WIN32
         static const char* graphicDrivers[] = { "default", "windib", "directx", NULL };
         new PG_PropertyField_DropDown<ASCString>( propertyEditor , "Graphics backend", &o->graphicsDriver, graphicDrivers );
#endif


            
         new PG_PropertyField_IntDropDown<int, GetVideoModes::VList::iterator>( propertyEditor, "Video Mode", &videoMode, vmodes.getList().begin(), vmodes.getList().end() );
         
         new PG_PropertyField_Checkbox<bool>( propertyEditor, "Automatic Training", &o->automaticTraining );

         new PG_PropertyField_IntDropDown<int>( propertyEditor, "Mouse: Field Select", &o->mouse.fieldmarkbutton, mouseButtonNames );
         new PG_PropertyField_IntDropDown<int>( propertyEditor, "Mouse: Center View", &o->mouse.centerbutton, mouseButtonNames );
         new PG_PropertyField_IntDropDown<int>( propertyEditor, "Mouse: Drag'N'Drop", &o->mouse.dragndropbutton, mouseButtonNames );
         new PG_PropertyField_IntDropDown<int>( propertyEditor, "Mouse: Zoom Out", &o->mouse.zoomoutbutton, mouseButtonNames );
         new PG_PropertyField_IntDropDown<int>( propertyEditor, "Mouse: Zoom In", &o->mouse.zoominbutton, mouseButtonNames );
         if ( mainApp ) 
            new PG_PropertyField_Checkbox<bool>( propertyEditor, "Single click action", &o->mouse.singleClickAction );


         if ( !mainApp )
            new PG_PropertyField_String<ASCString>( propertyEditor , "BI3 directory", &o->BI3directory );
            
         if ( mainApp ) 
            new PG_PropertyField_String<ASCString>( propertyEditor , "Startup Map", &o->startupMap );

         //if ( mainApp ) 
         (new PG_PropertyField_String<ASCString>( propertyEditor , "Default Password", &defaultPassword ))->SetPassHidden('*');

         new PG_PropertyField_Checkbox<bool>( propertyEditor, "DEV: Cache GUI Definition (*.ascgui)", &o->cacheASCGUI );
         new PG_PropertyField_Checkbox<bool>( propertyEditor, "DEV: View own replay", &o->debugReplay );

         if ( mainApp )  {
            new PG_PropertyField_Checkbox<bool>( propertyEditor, "Replays as Movies (not saved)", &o->replayMovieMode );

            new PG_PropertyField_Checkbox<bool>( propertyEditor, "Record Campaign Map solutions", &o->recordCampaignMaps );
            
            new PG_PropertyField_String<ASCString>( propertyEditor, "PBEM server hostname", &o->pbemServer.hostname );
            new PG_PropertyField_String<ASCString>( propertyEditor, "PBEM server username", &o->pbemServer.username );
            new PG_PropertyField_Integer<int>( propertyEditor, "PBEM server port", &o->pbemServer.port);
            
            new PG_PropertyField_Checkbox<bool>( propertyEditor, "Log kills to console", &o->logKillsToConsole );
         }
         
         (new PG_PropertyField_String<ASCString>( propertyEditor , "Language Override", &o->languageOverride ));
         
         if ( !mainApp )
            new PG_PropertyField_Checkbox<bool>( propertyEditor, "DEV: Save event message seperately", &o->saveEventMessagesExternal );

        
         PG_Button* ok = new PG_Button( this, PG_Rect( Width() - 100, Height() - 40, 90, 30), "OK" );
         ok->sigClick.connect( SigC::slot( *this, &EditGameOptions::ok ));
      };

};


void editGameOptions ( bool mainApp  )
{
   try {
      EditGameOptions ego ( NULL, mainApp );
      ego.Show();
      ego.RunModal();
   }
   catch ( ... ) {
      errorMessage( "An exception was caught" );
   }
}
