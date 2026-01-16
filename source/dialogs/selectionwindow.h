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

#ifndef selectionwindowH
#define selectionwindowH

#include <algorithm>
#include <vector>

#include <paragui.h>
#include <pgwidget.h>
#include <pgwidgetlist.h>
#include <pgwindow.h>
#include <pgapplication.h>
#include <pglineedit.h>

#include "loki/Functor.h"

#include "../paradialog.h"
#include "../global.h"
#include "../ascstring.h"


class SelectionWidget : public PG_Widget {
   protected:
      SelectionWidget ( PG_Widget *parent, const PG_Rect &rect ) : PG_Widget( parent, rect ), selectionCallBack(NULL ) {};
   
   public:
      typedef Loki::Functor<bool, LOKI_TYPELIST_1(const SelectionWidget*) > SelectionCallBack;

   private:      
      SelectionCallBack* selectionCallBack;
   public:
   
      void setSelectionCallback ( SelectionCallBack* callBack ) {
          selectionCallBack = callBack;
      }
      
      virtual ~SelectionWidget() {};

      virtual ASCString getName() const = 0;
      

      sigc::signal<void,const SelectionWidget*> itemSelected;
      sigc::signal<void,const SelectionWidget*> itemMarked;
                 
      virtual int gap() { return 5; };
   protected:
      
      bool eventMouseButtonUp (const SDL_MouseButtonEvent *button);
      bool eventMouseButtonDown (const SDL_MouseButtonEvent *button);
      void eventBlit ( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst );
     
      virtual void display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst ) = 0;
};

class ItemSelectorWidget;

class SelectionItemFactory{
   public:
      virtual void restart() = 0;
      virtual SelectionWidget* spawnNextItem( PG_Widget* parent, const PG_Point& pos ) = 0;
      
      virtual void itemSelected( const SelectionWidget* widget, bool mouse ) = 0;
      virtual void itemMarked  ( const SelectionWidget* widget ) {};
      virtual SelectionWidget* getDefaultItem() { return NULL; };
      virtual ~SelectionItemFactory() {};
};



class ItemSelectorWidget : public PG_Widget {
      
      bool namesConstrained;

      typedef vector<SelectionWidget*> WidgetList;
      WidgetList widgets;
      
      int rowCount;
      PG_ScrollWidget* scrollWidget;
      
      PG_LineEdit* nameSearch;
      const SelectionWidget* selectedItem;
      
      SelectionItemFactory* factory;
      int columnCount;
      int visibleRowCount;
      
      SelectionWidget::SelectionCallBack selectionCallBack;
      
      bool searchWithinNames;

   protected:   
      bool moveSelection( int amount ) ;
      bool eventKeyDown(const SDL_KeyboardEvent* key);
  	  bool eventTextInput(const SDL_TextInputEvent* text);
      void itemSelected( const SelectionWidget* w, bool mouse );
      void markItem( const SelectionWidget* w );
      bool isItemMarked( const SelectionWidget* w );
      bool locateObject( const ASCString& name );
      bool nameMatch( const SelectionWidget* selection, const ASCString& name );
      
   public:
      // the ItemSelectorWindow will take ownership over the itemFactory
      ItemSelectorWidget( PG_Widget *parent, const PG_Rect &r , SelectionItemFactory* itemFactory ) ;

      sigc::signal<void,const SelectionWidget*> sigItemSelected;
      sigc::signal<void,ASCString> nameEntered;
      sigc::signal<bool> sigQuitModal;
      
      void constrainNames( bool constrain );
      int getItemNum() const { return widgets.size(); };

      void selectItem(Loki::Functor<bool, LOKI_TYPELIST_1(SelectionWidget*)> predicate, bool fromTop = true );
            
      void reLoad( bool show = false );
      void resetNamesearch();
      void setSearchWithinNames(bool within);
      ~ItemSelectorWidget();
};


class ItemSelectorWindow: public ASC_PG_Dialog {
      ItemSelectorWidget* itemSelector;
      virtual void itemSelected( const SelectionWidget* );
   protected:
      bool eventKeyDown(const SDL_KeyboardEvent* key);
      
   public:
      // the ItemSelectorWindow will take ownership over the itemFactory
      ItemSelectorWindow( PG_Widget *parent, const PG_Rect &r , const ASCString& title, SelectionItemFactory* itemFactory ) ;
      void setSearchWithinNames(bool within) { itemSelector->setSearchWithinNames(within);};
     
      int RunModal();
      void reLoad();

};


#endif
