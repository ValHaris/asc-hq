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

#include "dropdownselector.h"


DropDownSelector::DropDownSelector( PG_Widget *parent, const PG_Rect &r, int itemnum, const char** items, const std::string &style )
   : PG_DropDown( parent, r, -1, style ), first(true)
{
   SetEditable(false);
   sigSelectItem.connect( SigC::slot( *this, &DropDownSelector::itemSelected ));

   for ( int i = 0; i < itemnum; ++i )
      AddItem( items[i] );
      
}


DropDownSelector::DropDownSelector( PG_Widget *parent, const PG_Rect &r, int id, const std::string &style) 
   : PG_DropDown( parent, r, id, style ), first(true)
{
   SetEditable(false);
   sigSelectItem.connect( SigC::slot( *this, &DropDownSelector::itemSelected ));
}

bool DropDownSelector::itemSelected(  ) // PG_ListBoxBaseItem* i, void* p
{
   selectionSignal( GetSelectedItemIndex ());
   return true;
}


void DropDownSelector::AddItem (const std::string &text, void *userdata, Uint16 height)
{
   PG_DropDown::AddItem( text, userdata, height );
   if ( first ) {
      first = false;
      SelectFirstItem();
   }
}


