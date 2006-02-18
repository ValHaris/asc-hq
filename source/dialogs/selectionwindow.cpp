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

#include "sigc++/retype_return.h"
#include "selectionwindow.h"
#include "../paradialog.h"

bool SelectionWidget::eventMouseButtonUp (const SDL_MouseButtonEvent *button) 
{
   if ( button->type == SDL_MOUSEBUTTONUP && button->button == SDL_BUTTON_LEFT ) {
      itemSelected( this );
      return true;
   }   
   return false;
};

bool SelectionWidget::eventMouseButtonDown (const SDL_MouseButtonEvent *button) 
{
   if ( button->type == SDL_MOUSEBUTTONDOWN && button->button == SDL_BUTTON_LEFT ) {
      itemMarked( this );
      Update();
      return true;
   }   
   return false;
};

void SelectionWidget::eventBlit ( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst )
{
   if ( selectionCallBack && selectionCallBack->operator()(this) )
      SDL_FillRect( PG_Application::GetScreen(), const_cast<PG_Rect*>(&dst), 0xff888888 );
      
   display( PG_Application::GetScreen(), src, dst );
};



bool ItemSelectorWidget::moveSelection( int amount ) 
{
   WidgetList::iterator i;
   if ( !selectedItem ) {
      i = widgets.begin();
   } else {
      i = find( widgets.begin(), widgets.end(), selectedItem );
      if ( i != widgets.end() ) {
         if ( amount > 0 ) {
            if ( widgets.end() - i > amount )
               i += amount;
            else
               i = widgets.end() - 1;
         } else
         if ( amount < 0 ) {
            if ( i - widgets.begin() >= -amount )
               i += amount;
            else
               i = widgets.begin();
         }
      }
   }

   if ( i == widgets.end() )
      return false;
   
   if ( *i != selectedItem ) {
      selectedItem = *i;
      resetNamesearch();
      scrollWidget->ScrollToWidget( *i );
      Update();
      return true;
   } 
   return false;
}

bool ItemSelectorWidget::eventKeyDown(const SDL_KeyboardEvent* key) 
{
   if ( key->keysym.sym == SDLK_BACKSPACE ) {
      ASCString s = nameSearch->GetText();
      if ( s.length() > 0 ) {
         s.erase( s.length() - 1 );
         nameSearch->SetText( s );
      }
      return true;
   } 
   if ( key->keysym.sym == SDLK_RIGHT )  {
      moveSelection(1);
      return true;
   }
   if ( key->keysym.sym == SDLK_LEFT )  {
      moveSelection(-1);
      return true;
   }
   if ( key->keysym.sym == SDLK_UP )  {
      moveSelection(-columnCount);
      return true;
   }
   if ( key->keysym.sym == SDLK_DOWN )  {
      moveSelection(columnCount);
      return true;
   }

   if ( key->keysym.sym == SDLK_RETURN ) {
      if ( namesConstrained ) {
         if ( selectedItem && (nameMatch( selectedItem, nameSearch->GetText()) || nameSearch->GetText().length() == 0) ) {
            itemSelected( selectedItem, false );
            return true;
         }   
      } else {
         if ( nameSearch->GetText().length() ) {
            nameEntered( nameSearch->GetText() );
            return true;
         } else {
            if ( selectedItem ) {
               itemSelected( selectedItem, false );
               return true;
            }   
         }   
      }
   } 

      
            
   if ( key->keysym.unicode <= 255 && key->keysym.unicode >= 0x20 ) {
      ASCString newtext = nameSearch->GetText() + char ( key->keysym.unicode );
      if ( locateObject( newtext ) || !namesConstrained ) 
         nameSearch->SetText( newtext );
      
      return true;
   }   
   
   return false;
};

void ItemSelectorWidget::itemSelected( const SelectionWidget* w, bool mouse )
{
   Update();
   factory->itemSelected( w, mouse );
   sigItemSelected( w );
}

void ItemSelectorWidget::markItem( const SelectionWidget* w )
{
   selectedItem = w;  
   factory->itemMarked( w );
}


bool ItemSelectorWidget::isItemMarked( const SelectionWidget* w )
{
   return w == selectedItem;
}
      
bool ItemSelectorWidget::locateObject( const ASCString& name ) 
{
   for ( WidgetList::iterator i = widgets.begin(); i != widgets.end(); ++i ) {
      if ( nameMatch( *i, name )  ) {
         markItem( *i );         
         scrollWidget->ScrollToWidget( *i );
         Update();
         return true;
      }   
   }
   return false;
}

bool ItemSelectorWidget::nameMatch( const SelectionWidget* selection, const ASCString& name )
{
   ASCString a = name;
   a.toLower();
   ASCString b = selection->getName().toLower();
   if ( a.length() > 0 && b.length() > 0 )
      if ( b.find ( a ) == 0 ) 
         return true;
   return false;
};   


ItemSelectorWidget::ItemSelectorWidget( PG_Widget *parent, const PG_Rect &r , SelectionItemFactory* itemFactory ) 
   : PG_Widget( parent,r ), namesConstrained(true), rowCount(0), scrollWidget( NULL), nameSearch(NULL), selectedItem(NULL), factory( itemFactory ), columnCount(-1), selectionCallBack( this, &ItemSelectorWidget::isItemMarked ) {
   SetTransparency(255);
   reLoad();
   int bottom = 0; // itemFactory->getBottomLineHeight();
   Emboss* e = new Emboss( this, PG_Rect( 1, Height() - 26 - bottom, Width()-20, 22), true );
   nameSearch = new PG_Label ( e, PG_Rect( 4,1, e->Width()-4 , e->Height()-2 ));

   // factory->spawnBottonWidgets( this, PG_Rect( 1, Height() - bottom, Width()-20, bottom-1));
   // nameSearch = new PG_Label ( this, PG_Rect( 5, Height() - 25, Width() - 10, 20 ));
};


void ItemSelectorWidget::constrainNames( bool constrain )
{
   namesConstrained = constrain;
}   

void ItemSelectorWidget::reLoad( bool show ) 
{
   int orgx = -1;
   int orgy = -1;
   if ( scrollWidget ) {
      orgx = scrollWidget->GetScrollPosX();
      orgy = scrollWidget->GetScrollPosY();
   }
   delete scrollWidget;
   scrollWidget = new PG_ScrollWidget( this , PG_Rect( 0, 0, Width(), Height() - 30 ));
   scrollWidget->SetTransparency(255);
   widgets.clear();

   int x = 0;
   int y = 0;
   
   factory->restart();
   SelectionWidget* w; 
   while ( (w = factory->spawnNextItem( scrollWidget, PG_Point(x,y))) ) {
   
      if ( columnCount < 0 )
         columnCount = scrollWidget->Width() / (w->Width() + gapWidth);

      w->itemSelected.connect( SigC::bind( SigC::slot( *this, &ItemSelectorWidget::itemSelected ), true ));
      w->itemMarked.connect( SigC::slot( *this, &ItemSelectorWidget::markItem ));
      w->setSelectionCallback( &selectionCallBack );
      widgets.push_back ( w );
      
      x += w->Width() + gapWidth;
      if ( x + w->Width() + gapWidth >= scrollWidget->Width() ) {
         x = 0; 
         y += w->Height() + gapWidth;
      }
   }

   if ( orgx >= 0 && orgy >= 0 )
      scrollWidget->ScrollTo ( orgx, orgy );
   
   if ( show )
      scrollWidget->Show();
}


void ItemSelectorWidget::resetNamesearch()
{
   nameSearch->SetText( "" );
}

ItemSelectorWidget::~ItemSelectorWidget()
{
   delete factory;
}



ItemSelectorWindow::ItemSelectorWindow( PG_Widget *parent, const PG_Rect &r , const ASCString& title, SelectionItemFactory* itemFactory )
   : PG_Window( parent,r, title) 
{
   SetTransparency( 0 );
   
   itemSelector = new ItemSelectorWidget( this, PG_Rect( 5, GetTitlebarHeight () + 2, Width()-5, Height()- GetTitlebarHeight ()- 5 ), itemFactory );
   
   itemSelector->sigItemSelected.connect(  SigC::slot( *this, &ItemSelectorWindow::itemSelected ));
   itemSelector->sigQuitModal.connect( SigC::slot( *this, &ItemSelectorWindow::QuitModal));
};

void ItemSelectorWindow::itemSelected( const SelectionWidget* )
{
   Hide();
   QuitModal();
}

bool ItemSelectorWindow::eventKeyDown(const SDL_KeyboardEvent* key)
{
   if ( key->keysym.sym == SDLK_ESCAPE )  {
      QuitModal();
      return true;
   }
   return false;
}

int ItemSelectorWindow::RunModal()
{
   itemSelector ->resetNamesearch();
   return PG_Window::RunModal();
}

void ItemSelectorWindow::reLoad()
{
   itemSelector->reLoad();
}
