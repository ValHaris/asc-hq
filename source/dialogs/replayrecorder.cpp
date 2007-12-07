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

#include "replayrecorder.h"
#include "../paradialog.h"

#include "fileselector.h"


bool ReplayRecorderDialog::selectFilename()
{
   ASCString filename = selectFile( "*.avi", false );
   if ( !filename.empty() )
      this->filename->SetText( filename );
   return true;
}
      
bool ReplayRecorderDialog::ok()
{
   if ( !filename->GetText().empty() ) {
      QuitModal();
      return true;
   } else
      return false;
}
      
ReplayRecorderDialog::ReplayRecorderDialog( const ASCString& file ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 400, 250 ), "Replay Recorder" )
{
   filename = new PG_LineEdit( this, PG_Rect( 20, 30, 150, 25 ));
   filename->SetText( file );
   (new PG_Button(this, PG_Rect( 190, 30, 50, 25), "Browse"))->sigClick.connect( SigC::slot( *this, &ReplayRecorderDialog::selectFilename ));
   
   append = new PG_CheckButton( this, PG_Rect( 20, 70, 150, 25), "append to video");
   append->SetPressed();
   
   (new PG_Button( this, PG_Rect( 20, 110, 100, 30 ), "OK"))->sigClick.connect( SigC::slot(*this, &ReplayRecorderDialog::ok ));
   
}

ASCString ReplayRecorderDialog::getFilename()
{
   if ( filename->GetText().find('.') == ASCString::npos ) 
      return filename->GetText() + ".avi";
   else
      return filename->GetText();
}
      
bool ReplayRecorderDialog::getAppend()
{
   return append->GetPressed();  
}
