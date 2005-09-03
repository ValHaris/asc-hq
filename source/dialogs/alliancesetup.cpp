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
#include "playersetup.h"
#include "alliancesetup.h"



class DiplomaticModeChooser : public PG_ListBox {

   public:
      DiplomaticModeChooser ( PG_Widget *parent, PG_Point pos ) : PG_ListBox( parent, PG_Rect( pos.x, pos.y, 40,20 ))
      {
         for ( int i = 0; i < 5; ++i ) {
            ASCString filename = "diplo-" + ASCString::toString(i) + ".png";
            AddChild( new PG_Image( NULL, PG_Point(0,0), IconRepository::getIcon( filename ).getBaseSurface(), false ));
            // AddItem( ASCString::toString(i));
         }   
      };
};      
 
// PG_DropDown (PG_Widget *parent, const PG_Rect &r=PG_Rect::null, int id=-1, const std::string &style="DropDown")


AllianceSetupWidget::AllianceSetupWidget( tmap* gamemap, bool allEditable, PG_Widget *parent, const PG_Rect &r, const std::string &style ) : PG_ScrollWidget( parent, r, style ) , actmap ( gamemap )
{
   int playerNum = 0;
   for ( int i = 0; i < actmap->getPlayerCount(); ++i ) 
      if ( actmap->player[i].exist() ) 
         ++playerNum;

         
   const int colWidth = 40;
   const int lineHeight = 30;
   const int barSpace = 5;
   const int spacing = 10;
   const int nameLength = 200;
   const int barOverhang = 20;
   const int sqaureWidth = lineHeight;
   const int lineLength = sqaureWidth + nameLength + playerNum * (colWidth + spacing) + barOverhang;
   const int colHeight  =  barOverhang + playerNum * (lineHeight + spacing) - spacing +  barOverhang;
                  
   int counter = 0; 
   for ( int i = 0; i < actmap->getPlayerCount(); ++i ) 
      if ( actmap->player[i].exist() ) {
      
         PlayerWidgets pw;
         pw.pos  = i;
         
         int y = barOverhang + counter * (lineHeight + spacing);
         int x = sqaureWidth + nameLength + spacing + counter * (colWidth + spacing);
         
         ColoredBar* horizontalBar = new ColoredBar( actmap->player[i].getColor(), this, PG_Rect( 0, y, lineLength, lineHeight ));
         horizontalBar->SetTransparency( 128 );
         
         pw.name = new PG_LineEdit( horizontalBar, PG_Rect( sqaureWidth, barSpace, nameLength, lineHeight-2*barSpace ));
         pw.name->SetText( actmap->player[i].getName());
         pw.name->SetEditable( false );

         PG_ThemeWidget* col = new PG_ThemeWidget( horizontalBar, PG_Rect( barSpace, barSpace, sqaureWidth - 2*barSpace, lineHeight - 2*barSpace ));
         col->SetSimpleBackground(true);
         col->SetBackgroundColor ( actmap->player[i].getColor());
         col->SetBorderSize(0);
         
                        
         ColoredBar* verticalBar = new ColoredBar( actmap->player[i].getColor(), this, PG_Rect( x, 0, colWidth, colHeight ));
         verticalBar->SetTransparency( 128 );

         for ( int j = 0; j < actmap->getPlayerCount(); ++j )
            if ( i != j )
               new DiplomaticModeChooser( verticalBar, PG_Point(0,y));
                        
         /*
         PG_Rect r = PG_Rect( y1 + 20, 5, colbar->Width() - y1 - 40, 20 );
         if ( !allEditable && actmap->actplayer != i ) {
         
            pw.type = new PG_DropDown( colbar, r);
            
            int pos = 0;
            while ( tmap :: Player :: playerStatusNames[pos] ) {
               pw.type->AddItem( tmap :: Player :: playerStatusNames[pos] );
               ++pos;
            }
            
            pw.type->SelectItem( actmap->player[i].stat );
            pw.type->SetEditable(false);
         } else {
            pw.type = NULL;
            PG_LineEdit* le = new PG_LineEdit( colbar, r );
            le->SetText( tmap :: Player :: playerStatusNames[ actmap->player[i].stat ] );
            le->SetEditable( false );
         }
         */
         

         playerWidgets.push_back( pw );
         
                        
         ++counter;
      } 
      
   SetTransparency(255);
};

void AllianceSetupWidget::Apply() {
};
