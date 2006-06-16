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

#include <pgpropertyeditor.h>
#include <pgpropertyfield_integer.h>
#include <pgpropertyfield_intdropdown.h>
#include <pgpropertyfield_checkbox.h>
#include <pgpropertyfield_string.h>


#include "selectionwindow.h"
#include "sigc++/retype.h"
#include "editmapparam.h"
#include "fileselector.h"
#include "../ascstring.h"

#include "../textfileparser.h"
#include "../textfile_evaluation.h"



class GetVideoModes {
   public:
      typedef vector<ASCString> VList;
   private:
      SDL_Rect **modes;
      VList list;

      vector <pair<int,int> > listedmodes;
     
   public:
      GetVideoModes() {
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

         /* Check is there are any modes available */
         if(modes == (SDL_Rect **)0){
            return;
         }

         list.push_back( "graphic mode not listed");
         listedmodes.push_back( make_pair( 0, 0));
         
         /* Check if our resolution is restricted */
         if(modes == (SDL_Rect **)-1){
            warning("All resolutions available.\n");
            return;
         }
         else{
            for(i=0;modes[i];++i) {

               if ( find ( listedmodes.begin(), listedmodes.end(), make_pair( int(modes[i]->w), int(modes[i]->h ))) != listedmodes.end() )
                  continue;
               
               ASCString s;
               s.format( "%d*%d", modes[i]->w, modes[i]->h );
               list.push_back ( s );
               listedmodes.push_back( make_pair( int(modes[i]->w), int(modes[i]->h )));
            }
         }
         return;
      };

      VList& getList() { return list; };

      int getx( int index ) {
         return listedmodes.at(index).first;
      };
      
      int gety( int index ) {
         return listedmodes.at(index).second;
      };

      int findmodenum( int x, int y ) {
         for ( int j = 0; j < listedmodes.size(); ++j )
            if ( listedmodes[j].first == x && listedmodes[j].second == y )
               return j;
         return 0;
      }
      
};




const char* mouseButtonNames[] = { "None", "Left", "Center", "Right", "4", "5", NULL };

class EditGameOptions : public ASC_PG_Dialog {
   private:
      PG_PropertyEditor* propertyEditor;
      
      GetVideoModes vmodes;
      
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
               infoMessage( "The new resolution will be used after you restart ASC");

            if ( getPGApplication().isFullscreen() != fullscreen ) 
               getPGApplication().toggleFullscreen();
            
            
            CGameOptions::Instance()->setChanged();
            quitModalLoop(0);

            return true;
         } else
            return false;
      }

   public:
      EditGameOptions( PG_Widget* parent, bool mainApp ) : ASC_PG_Dialog( parent, PG_Rect( 50, 50, 500, 550 ), "Edit Map Parameters"), videoMode(0), ascmain( mainApp )
      {
         CGameOptions* o = CGameOptions::Instance();

         
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
         } else {
            new PG_PropertyField_Checkbox<bool>( propertyEditor, "MapEd running Fullscreen", &o->mapeditWindowedMode, true );
            new PG_PropertyField_Checkbox<bool>( propertyEditor, "Modal Selection Window", &o->maped_modalSelectionWindow );
         }


            
         new PG_PropertyField_IntDropDown<int, GetVideoModes::VList::iterator>( propertyEditor, "Video Mode", &videoMode, vmodes.getList().begin(), vmodes.getList().end() );
         
         new PG_PropertyField_Checkbox<bool>( propertyEditor, "View own replays", &o->debugReplay );
         new PG_PropertyField_Checkbox<bool>( propertyEditor, "Automatic Training", &o->automaticTraining );

         new PG_PropertyField_IntDropDown<int>( propertyEditor, "Mouse: Field Select", &o->mouse.fieldmarkbutton, mouseButtonNames );
         new PG_PropertyField_IntDropDown<int>( propertyEditor, "Mouse: Center View", &o->mouse.centerbutton, mouseButtonNames );


         if ( !mainApp )
            new PG_PropertyField_String<ASCString>( propertyEditor , "BI3 directory", &o->BI3directory );
            
         if ( mainApp )
            new PG_PropertyField_String<ASCString>( propertyEditor , "Startup Map", &o->startupMap );

         new PG_PropertyField_Checkbox<bool>( propertyEditor, "DEV: Cache GUI Definition (*.ascgui)", &o->cacheASCGUI );
         new PG_PropertyField_Checkbox<bool>( propertyEditor, "DEV: View own replay", &o->debugReplay );

        
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
