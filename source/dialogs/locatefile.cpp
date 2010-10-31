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

#include "../paradialog.h"
#include "../widgets/textrenderer.h"

class FileFindDialog: public ASC_PG_Dialog {
   private:
      TextRenderer* location;
      PG_LineEdit* le;
      bool find( PG_LineEdit* le )
      {
         location->SetText("searching ...");
         ASCString filename = le->GetText();
         tfindfile ff( filename );
         tfindfile::FileInfo fi;
         if ( ff.getnextname( fi )) {
            location->SetText( fi.location );
         } else {
            location->SetText( "-not found-" );
         }
         return true;
      }
   public:
      FileFindDialog () : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 500, 200 ), "Locate File") 
      {
         le = new PG_LineEdit( this, PG_Rect( 20, 40, 460, 25 ));
         le->sigEditEnd.connect( SigC::slot( *this, &FileFindDialog::find )); 
         
         location = new TextRenderer ( this, PG_Rect( 20, 90, 460, 90 ));
      }
      
      int RunModal() {
         le->EditBegin();
         return ASC_PG_Dialog::RunModal();  
      }
};


void locateFile()
{
   FileFindDialog fd;
   fd.Show();
   fd.RunModal();
}
