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

#ifndef DROPDOWNSELECTOR_H
#define DROPDOWNSELECTOR_H

#include <vector>
#include <pgdropdown.h>
#include "../ascstring.h"

class DropDownSelector: public PG_DropDown {
        bool first;
   protected:
      bool itemSelected( ); // PG_ListBoxBaseItem* i, void* p );
   public:
      DropDownSelector( PG_Widget *parent, const PG_Rect &r=PG_Rect::null_rect, int id=-1, const std::string &style="DropDown");
      DropDownSelector( PG_Widget *parent, const PG_Rect &r, int itemnum, const char** items, const std::string &style="DropDown" );
      DropDownSelector( PG_Widget *parent, const PG_Rect &r, const std::vector<ASCString>& names,  const std::string &style="DropDown" );
      void AddItem (const std::string &text, void *userdata=NULL, Uint16 height=0);
      sigc::signal<void, int> selectionSignal;
};

#endif
