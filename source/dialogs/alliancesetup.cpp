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
#include "playersetup.h"
#include "alliancesetup.h"


const int diplomaticStateIconSize = 18;
const int diplomaticStateIconSpace = 2;

ASCString getDiplomaticStateImage( DiplomaticStates s )
{
   ASCString filename = "diplo-" + ASCString::toString(s) + ".png";
   return filename;
};

class ListBoxImageItem : public PG_ListBoxBaseItem {
      DiplomaticStates state;
   public:
      ListBoxImageItem( PG_ListBox *parent, PG_Point pos, DiplomaticStates s )  : PG_ListBoxBaseItem( parent, diplomaticStateIconSize + 2*diplomaticStateIconSpace ), state(s)
      {
         new PG_Image( this, PG_Point(diplomaticStateIconSpace,diplomaticStateIconSpace), IconRepository::getIcon( getDiplomaticStateImage(s)).getBaseSurface() , false );
         new PG_Label( this, PG_Rect(diplomaticStateIconSize + 5, diplomaticStateIconSpace, 200,diplomaticStateIconSize), diplomaticStateNames[s]);
      }
      
      SigC::Signal1<void,DiplomaticStates> sigSet;
      
      bool eventMouseButtonUp(const SDL_MouseButtonEvent* button) 
      {
            if(button->button != 1) {
                     return false;
            }
            
            PG_ListBox* listbox = dynamic_cast<PG_ListBox*>(GetParent());
      
            if(listbox == NULL || !listbox->IsVisible()) {
                     return true;
            }
      
            listbox->SelectItem(this);
            listbox->QuitModal();
            sigSet(state);
      
            return true;
      }
};


class DiplomaticModeChooser : public PG_Widget {
      DiplomaticStates mode;
      bool writePossible;
      
   protected:

      void selectMode()
      {
         PG_ListBox listBox( NULL, PG_Rect( x, y + Height(), 250, diplomaticStateNum * ( diplomaticStateIconSpace * 2 + diplomaticStateIconSize ) + 4 ));
         for ( int i = 0; i < diplomaticStateNum; ++i) {
            ListBoxImageItem* item = new ListBoxImageItem( NULL, PG_Point(0,0), DiplomaticStates(i));
            item->sigSet.connect( SigC::slot( *this, &DiplomaticModeChooser::SetState));
            listBox.AddChild( item );
         }
         listBox.Show();
         listBox.RunModal();
      }     
            
   public:
      DiplomaticModeChooser ( PG_Widget *parent, const PG_Rect& pos, DiplomaticStates dm, bool writeable ) : PG_Widget( parent, pos, true ), mode(dm), writePossible( writeable )
      {
      };
      
      void eventDraw (SDL_Surface *surface, const PG_Rect &rect)
      {
         Surface s = Surface::Wrap( surface );
         s.Blit(  IconRepository::getIcon( getDiplomaticStateImage(mode) ) );
      }
      
      void SetState( DiplomaticStates state )
      {
         mode = state;
         Redraw();
         Update(true);
      }
      
      bool eventMouseButtonUp(const SDL_MouseButtonEvent* button) 
      {
         if(button->button != 1) {
                  return false;
         }

         if ( writePossible )   
            selectMode();
         // listBox->Show();            
         return true;
      }
};      
 

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

   #define calcx(counter) (sqaureWidth + nameLength + spacing + counter * (colWidth + spacing) )
   #define calcy(counter) (barOverhang + counter * (lineHeight + spacing))

   int counter = 0; 
   for ( int i = 0; i < actmap->getPlayerCount(); ++i ) 
      if ( actmap->player[i].exist() ) {
         int x = calcx(counter);
         
         ColoredBar* verticalBar = new ColoredBar( actmap->player[i].getColor(), this, PG_Rect( x, 0, colWidth, colHeight ));
         verticalBar->SetTransparency( 128 );
         
         ++counter;
      }
   
                        
   counter = 0; 
   for ( int i = 0; i < actmap->getPlayerCount(); ++i ) 
      if ( actmap->player[i].exist() ) {
      
         PlayerWidgets pw;
         pw.pos  = i;
         
         int y = calcy(counter);
         
         ColoredBar* horizontalBar = new ColoredBar( actmap->player[i].getColor(), this, PG_Rect( 0, y, lineLength, lineHeight ));
         horizontalBar->SetTransparency( 128 );
         
         pw.name = new PG_LineEdit( horizontalBar, PG_Rect( sqaureWidth, barSpace, nameLength, lineHeight-2*barSpace ));
         pw.name->SetText( actmap->player[i].getName());
         pw.name->SetEditable( false );

         PG_ThemeWidget* col = new PG_ThemeWidget( horizontalBar, PG_Rect( barSpace, barSpace, sqaureWidth - 2*barSpace, lineHeight - 2*barSpace ));
         col->SetSimpleBackground(true);
         col->SetBackgroundColor ( actmap->player[i].getColor());
         col->SetBorderSize(0);
         
         int cnt2 = 0;
         for ( int j = 0; j < actmap->getPlayerCount(); ++j ) 
            if ( actmap->player[j].exist() )  {
               if ( i != j ) {
                  int x = calcx(cnt2) - barSpace;
                  new DiplomaticModeChooser( horizontalBar, PG_Rect(x + (colWidth - diplomaticStateIconSize)/2 ,  (lineHeight - diplomaticStateIconSize)/2,diplomaticStateIconSize,diplomaticStateIconSize), PEACE, j > i );
               }
               ++cnt2;
            }   
                        
         playerWidgets.push_back( pw );
                        
         ++counter;
      } 
      
   SetTransparency(255);
};



void AllianceSetupWidget::Apply() {
};
