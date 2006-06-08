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
#include <pglistboxbaseitem.h>

#include "../iconrepository.h"
#include "../gamemap.h"
#include "../paradialog.h"
#include "playersetup.h"
#include "alliancesetup.h"


const int diplomaticStateIconSize = 20;
const int diplomaticStateIconSpace = 2;


template<typename SelectionType>
int getItemNum() { return 0; };

template<typename SelectionType>
const char* getStateName(int s) { return NULL; };


template<>
int getItemNum<DiplomaticStates>() { return diplomaticStateNum; };

template<>
const char* getStateName<DiplomaticStates>(int s) { return diplomaticStateNames[s]; };


template<>
int getItemNum<AllianceSetupWidget::DiplomaticTransitions>() { return diplomaticStateNum+1; };

template<>
const char* getStateName<AllianceSetupWidget::DiplomaticTransitions>(int s) 
{ 
   if ( s == 0 ) 
      return "Sneak Attack"; 
   else 
      return diplomaticStateNames[s-1]; 
};


ASCString getDiplomaticStateImage( DiplomaticStates s )
{
   ASCString filename = "diplo-" + ASCString::toString(s) + ".png";
   return filename;
};

ASCString getDiplomaticStateImage( AllianceSetupWidget::DiplomaticTransitions s )
{
   if ( s == AllianceSetupWidget::SNEAK_ATTACK ) {
      ASCString filename = "diplo-sneak.png";
      return filename;
   } else 
      return getDiplomaticStateImage( DiplomaticStates( s-1 ));
};


template <typename SelectionType>
class ListBoxImageItem : public PG_ListBoxBaseItem {
      SelectionType state;
   public:
      ListBoxImageItem( PG_ListBox *parent, PG_Point pos, SelectionType s )  : PG_ListBoxBaseItem( parent, diplomaticStateIconSize + 2*diplomaticStateIconSpace ), state(s)
      {
         new PG_Image( this, PG_Point(diplomaticStateIconSpace,diplomaticStateIconSpace), IconRepository::getIcon( getDiplomaticStateImage(s)).getBaseSurface() , false );
         new PG_Label( this, PG_Rect(diplomaticStateIconSize + 5, diplomaticStateIconSpace, 200,diplomaticStateIconSize), getStateName<SelectionType>(s));
      }
      
      SigC::Signal1<void,SelectionType> sigSet;
      
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





template <typename SelectionType>
class DiplomaticModeChooser : public PG_Widget {
      SelectionType& mode;
      bool writePossible;
      
   protected:

      void selectMode()
      {
         PG_ListBox listBox( NULL, PG_Rect( x, y + Height(), 250, getItemNum<SelectionType>() * ( diplomaticStateIconSpace * 2 + diplomaticStateIconSize ) + 4 ));
         for ( int i = 0; i < getItemNum<SelectionType>(); ++i) {
            ListBoxImageItem<SelectionType>* item = new ListBoxImageItem<SelectionType>( NULL, PG_Point(0,0), SelectionType(i));
            item->sigSet.connect( SigC::slot( *this, &DiplomaticModeChooser::SetState));
            listBox.AddChild( item );
         }
         listBox.Show();
         listBox.RunModal();
      }     
            
   public:
      DiplomaticModeChooser ( PG_Widget *parent, const PG_Rect& pos, SelectionType& dm, bool writeable ) : PG_Widget( parent, pos, true ), mode(dm), writePossible( writeable )
      {
      };

      SigC::Signal1<void,SelectionType> sigStateChange;
            
      void eventDraw (SDL_Surface *surface, const PG_Rect &rect)
      {
         Surface s = Surface::Wrap( surface );
         s.Blit(  IconRepository::getIcon( getDiplomaticStateImage(mode) ) );
      }
      
      void SetState( SelectionType state )
      {
         mode = state;
         Redraw();
         Update(true);
         sigStateChange(state);
      }
      
      bool eventMouseButtonUp(const SDL_MouseButtonEvent* button) 
      {
         if (button->button != 1) {
            return false;
         }

         if ( writePossible )   
            selectMode();
         return true;
      }
};      
 

AllianceSetupWidget::AllianceSetupWidget( GameMap* gamemap, bool allEditable, PG_Widget *parent, const PG_Rect &r, const std::string &style ) : PG_ScrollWidget( parent, r, style ) , actmap ( gamemap )
{
   this->allEditable = allEditable;
   int playerNum = 0;
   
   for ( int i = 0; i < actmap->getPlayerCount(); ++i ) {
      vector<DiplomaticTransitions> t;
      vector< DiplomaticStates > s;
      
      const DiplomaticStateVector& diplo = actmap->player[i].diplomacy;
      
      for ( int j = 0; j < actmap->getPlayerCount(); ++j )  {
         DiplomaticStateVector::QueuedStateChanges::const_iterator change = diplo.queuedStateChanges.find(j);
         if ( change != diplo.queuedStateChanges.end() )
            t.push_back( DiplomaticTransitions( change->second + 1 ));
         else
            t.push_back( DiplomaticTransitions( diplo.getState(j) + 1 ));
         s.push_back( diplo.getState(j));
      }   
       
      stateChanges.push_back ( t );        
      states.push_back ( s );
      
      if ( actmap->player[i].exist() ) 
         ++playerNum;
   }      

         
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
   for ( int i = 0; i < actmap->getPlayerCount(); ++i ) // rows
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
         for ( int j = 0; j < actmap->getPlayerCount(); ++j ) // columns
            if ( actmap->player[j].exist() )  {
               if ( i != j ) {
                  int x = calcx(cnt2) - barSpace;
                  PG_Rect rect ( x + (colWidth - diplomaticStateIconSize)/2 ,  (lineHeight - diplomaticStateIconSize)/2,diplomaticStateIconSize,diplomaticStateIconSize); 
                  if ( allEditable ) {
                     DiplomaticModeChooser<DiplomaticStates>* dmc = new DiplomaticModeChooser<DiplomaticStates>( horizontalBar, rect, getState(i,j), true );
                     dmc->sigStateChange.connect( SigC::bind( SigC::slot( *this, &AllianceSetupWidget::setState ), j, i));
                     diplomaticWidgets[ linearize( i,j) ] = dmc;
                  } else {
                     DiplomaticTransitions& s = stateChanges[i][j];
                     DiplomaticModeChooser<DiplomaticTransitions>* dmc = new DiplomaticModeChooser<DiplomaticTransitions>( horizontalBar, rect, s, i == gamemap->actplayer );
                     // dmc->sigStateChange.connect( SigC::bind( SigC::slot( *this, &AllianceSetupWidget::setState ), j, i));
                     diplomaticWidgets[ linearize( i,j) ] = dmc;
                  }   
                     
               }
               ++cnt2;
            }   
                        
         playerWidgets.push_back( pw );
                        
         ++counter;
      } 
      
   SetTransparency(255);
};


void AllianceSetupWidget::setState( DiplomaticStates s, int actingPlayer, int secondPlayer )
{
   getState( actingPlayer, secondPlayer) = s;
   
   DiplomaticWidgets::iterator i = diplomaticWidgets.find( linearize( actingPlayer, secondPlayer) );
   if ( i != diplomaticWidgets.end() )
      i->second->Redraw(true);
}

DiplomaticStates& AllianceSetupWidget::getState( int actingPlayer, int secondPlayer )
{
   return states.at(actingPlayer).at(secondPlayer);
}

int AllianceSetupWidget::linearize( int actingPlayer, int secondPlayer  )
{
   return actingPlayer * actmap->getPlayerCount() + secondPlayer;
}


void AllianceSetupWidget::Apply() 
{
   for ( int acting = 0; acting < actmap->getPlayerCount(); ++acting )
      for ( int second = 0; second < actmap->getPlayerCount(); ++second ) {
      
         if ( allEditable ) {
            if ( getState( acting, second ) != actmap->player[acting].diplomacy.getState( second ))
               actmap->player[acting].diplomacy.setState( second, getState( acting, second ));         
         } else {
            if ( stateChanges[acting][second] == SNEAK_ATTACK ) {
               actmap->player[acting].diplomacy.sneakAttack( second );         
            } else {
               DiplomaticStates s = DiplomaticStates( stateChanges[acting][second] - 1);
               DiplomaticStates t;
               DiplomaticStateVector::QueuedStateChanges::iterator q = actmap->player[acting].diplomacy.queuedStateChanges.find( second );
               if ( q == actmap->player[acting].diplomacy.queuedStateChanges.end() )
                  t = getState( acting, second );
               else
                  t = q->second;   
                  
               if ( t != s )
                  actmap->player[acting].diplomacy.propose( second, s );         
            }   
         }
      }
};


AllianceSetupWidget::~AllianceSetupWidget()
{
}   










class AllianceSetupWindow : public ASC_PG_Dialog {
      AllianceSetupWidget* asw;
      bool changed;
   public:
      AllianceSetupWindow( GameMap* actmap, bool allEditable, PG_Widget *parent, const PG_Rect &r ) : ASC_PG_Dialog( parent, r, "Diplomacy" ), changed(false)
      {
         asw = new AllianceSetupWidget( actmap, allEditable, this, PG_Rect( 5, 30, r.Width() - 10, r.Height() - 60 ));
         PG_Button* ok = new PG_Button( this, PG_Rect( Width() - 200, Height() - 30, 90, 20 ), "OK" );
         ok->sigClick.connect( SigC::slot( *this, &AllianceSetupWindow::Apply ));
         PG_Button* cancel = new PG_Button( this, PG_Rect( Width() - 100, Height() - 30, 90, 20 ), "Cancel" );
         cancel->sigClick.connect( SigC::slot( *this, &AllianceSetupWindow::QuitModal ));
      }

      bool Apply()
      {
         asw->Apply();
         QuitModal();
         changed = true;
         return true;
      }

      bool isSomethingChanged() { return changed; };
      
};

bool  setupalliances( GameMap* actmap, bool supervisor  )
{
   AllianceSetupWindow asw ( actmap, supervisor, NULL, PG_Rect( 100, 100, 600, 500 ));
   asw.Show();
   asw.RunModal();
   return asw.isSomethingChanged();
}
