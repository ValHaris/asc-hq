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

#include "selectionwindow.h"
#include "sigc++/adaptors/retype.h"
#include "editmapparam.h"
#include "fileselector.h"

#include <pgpropertyeditor.h>
#include <pgpropertyfield_integer.h>
#include <pgpropertyfield_checkbox.h>


#include "../textfileparser.h"
#include "../textfile_evaluation.h"



     
void GameParameterEditorWidget :: runTextIO ( PropertyContainer& pc ) 
{
   for ( int i = 0; i< gameparameternum; ++i ) {
      if ( !gameParameterSettings[i].legacy ) {
         pc.addInteger( gameParameterSettings[i].name, values[i], values[i] );
         if ( pc.isReading() ) {
            if ( ! (values[i] >=  gameParameterSettings[i].minValue  && values[i] <= gameParameterSettings[i].maxValue  ))
               values[i] = gameParameterSettings[i].defaultValue;
         }
      }
   }

}



bool GameParameterEditorWidget :: LoadParameter()
{
   ASCString filename = selectFile( fileNamePattern, true );
   if ( !filename.empty() ) {
      tnfilestream s ( filename, tnstream::reading );
      
      TextFormatParser tfp ( &s );
      auto_ptr<TextPropertyGroup> tpg ( tfp.run());
      
      PropertyReadingContainer pc ( blockName, tpg.get() );
   
      runTextIO( pc );
      propertyEditor->Reload();
      return true;  
   } 
   return false;
}

bool GameParameterEditorWidget :: SaveParameter()
{
   ASCString filename = selectFile( fileNamePattern, false );
   if ( !filename.empty() ) {
      tn_file_buf_stream s ( filename, tnstream::writing );
      PropertyWritingContainer pc ( blockName, s );
      runTextIO( pc );
      return true;
   }
   return false;  
}

bool GameParameterEditorWidget :: ResetParameter()
{
   for ( int i = 0; i< gameparameternum; ++i ) 
      values[i] = gameParameterSettings[i].defaultValue;
   propertyEditor->Reload(); 
   return true;  
}
            
GameParameterEditorWidget :: GameParameterEditorWidget ( GameMap* gamemap, PG_Widget* parent, const PG_Rect& rect ) : PG_Widget( parent, rect ), actmap ( gamemap )
{
   SetTransparency(255);
   
   propertyEditor = new PG_PropertyEditor( this, PG_Rect( 0,0, rect.Width() - 110, rect.Height() ), "PropertyEditor", 70 );
   
   PG_Button* load = new PG_Button( this, PG_Rect( rect.Width() - 100, 0,  100, 30 ), "Load" );
   load->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &GameParameterEditorWidget::LoadParameter )));
   
   PG_Button* save = new PG_Button( this, PG_Rect( rect.Width() - 100, 40, 100, 30 ), "Save" );
   save->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &GameParameterEditorWidget::SaveParameter )));
   
   PG_Button* def = new PG_Button( this, PG_Rect( rect.Width() - 100, 80, 100, 30 ), "Default" );
   def->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &GameParameterEditorWidget::ResetParameter )));
   
   
   for ( int i = 0; i< gameparameternum; ++i ) {
      values[i] = actmap->getgameparameter ( GameParameter(i) );
      if ( values[i] < gameParameterSettings[i].minValue || values[i] > gameParameterSettings[i].maxValue )
         values[i] = gameParameterSettings[i].defaultValue;
      
      if ( !gameParameterSettings[i].legacy ) {
         if ( gameParameterSettings[i].minValue == 0 && gameParameterSettings[i].maxValue == 1 ) {
            new PG_PropertyField_Checkbox<int>( propertyEditor , gameParameterSettings[i].longName, &values[i] );
         } else {
            PG_PropertyField_Integer<int>* ip = new PG_PropertyField_Integer<int>( propertyEditor , gameParameterSettings[i].longName, &values[i] );
            ip->SetRange( gameParameterSettings[i].minValue, gameParameterSettings[i].maxValue );
         }
      }   
   }
};

bool GameParameterEditorWidget :: Valid()
{
   return propertyEditor->Valid( true );
}

bool GameParameterEditorWidget :: Apply()
{
   bool res = propertyEditor->Apply();
   for ( int i = 0; i< gameparameternum; ++i )
      actmap->setgameparameter ( GameParameter(i), values[i] );
   return res;
}
      

const char* GameParameterEditorWidget :: fileNamePattern = "*.asc.gameparam";
const char* GameParameterEditorWidget :: blockName = "GameParam";


class EditMapParameters : public ASC_PG_Dialog {
      GameParameterEditorWidget* gpew;
      
      bool ok()
      {
         if ( gpew->Valid() ) {
            gpew->Apply();
            quitModalLoop(0);

            return true;
         } else
            return false;
      }

   public:
      EditMapParameters( GameMap* actmap, PG_Widget* parent ) : ASC_PG_Dialog( parent, PG_Rect( -1, -1, 750, 500 ), "Edit Map Parameters")
      {
         gpew = new GameParameterEditorWidget ( actmap, this, PG_Rect( 10, GetTitlebarHeight(), Width() - 20, Height() - GetTitlebarHeight() - 40 ));
         PG_Button* ok = new PG_Button( this, PG_Rect( Width() - 100, Height() - 40, 90, 30), "OK" );
         ok->sigClick.connect( sigc::hide(sigc::mem_fun( *this, &EditMapParameters::ok )));
      };

};


void setmapparameters ( GameMap* gamemap )
{
   EditMapParameters emp( gamemap, NULL );
   emp.Show();
   emp.RunModal();
}
