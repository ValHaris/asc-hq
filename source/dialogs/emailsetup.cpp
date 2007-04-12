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

#include "../iconrepository.h"
#include "../gamemap.h"
#include "../paradialog.h"
#include "emailsetup.h"


 
EmailSetupWidget::EmailSetupWidget( GameMap* gamemap, int editablePlayers, PG_Widget *parent, const PG_Rect &r, const std::string &style ) : PG_ScrollWidget( parent, r, style ) , actmap ( gamemap )
{
   int counter = 0; 
   int y1 = Width() * 4 / 10;
   
   const int colBarXStart = 20;
   new PG_Label( this, PG_Rect( colBarXStart + y1, 20, 200, 20 ), "Email Address" );
   
   for ( int i = 0; i < actmap->getPlayerCount(); ++i ) 
      if ( actmap->player[i].exist() ) {
      
         PlayerWidgets pw;
         pw.pos  = i;
         
         int y = 50 + counter * spacing;
         
         ColoredBar* colbar = new ColoredBar( actmap->player[i].getColor(), this, PG_Rect( colBarXStart, y, Width() - 60, 30 ));
         colbar->SetTransparency( 128 );
         
         
        
         pw.name = new PG_LineEdit( colbar, PG_Rect( 40, 5, y1 - 40, 20 ));
         pw.name->SetText( actmap->player[i].getName());
         pw.name->SetEditable(false );
         
         PG_Rect r = PG_Rect( y1 + 20, 5, colbar->Width() - y1 - 40, 20 );
         pw.email = new PG_LineEdit( colbar, r );
         pw.email->SetText( actmap->player[i].email );

                  
         if ( editablePlayers == -1 || editablePlayers == gamemap->actplayer ) {
            pw.email->SetEditable(true);
         } else {
            pw.email->SetEditable(false);
         }
         
         PG_ThemeWidget* col = new PG_ThemeWidget( colbar, PG_Rect( 5, 5, 20, 20 ));
         col->SetSimpleBackground(true);
         col->SetBackgroundColor ( actmap->player[i].getColor());
         col->SetBorderSize(0);

         playerWidgets.push_back( pw );
                        
         ++counter;
      } 
      
   SetTransparency(255);
};


void EmailSetupWidget::Apply() 
{
   for ( vector<PlayerWidgets>::iterator i = playerWidgets.begin(); i != playerWidgets.end(); ++i ) 
      actmap->player[i->pos].email = i->email->GetText();
};
