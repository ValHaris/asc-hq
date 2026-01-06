/***************************************************************************
                             dropdownselector
                             -------------------
    copyright            : (C)  2006 by Martin Bickel
    email                : <bickel@asc-hq.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef multilistboxH
#define multilistboxH

#include <pgwidget.h>
#include <pglistbox.h>
#include <pglistboxitem.h>

template<typename T>
class PG_ListBoxDataItem : public PG_ListBoxItem {
   public:
      typedef T DataType;
   private:
      T my_data;
   public:
      PG_ListBoxDataItem( PG_Widget *parent, int height, const std::string &text, const T& data, SDL_Surface *icon=NULL ) : PG_ListBoxItem( parent, height, text, icon ), my_data(data) {};
      T getData() { return my_data; };
};



class MultiListBox : public PG_Widget {
      PG_ListBox* listbox;
   public:
      MultiListBox (PG_Widget *parent, const PG_Rect &r, bool multi_select = true );
      PG_ListBox* getListBox() { return listbox; };
      bool all();
      bool none();

      PG_ListBox::SignalSelectItem sigSelectItem;
};


#endif
