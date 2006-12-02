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

#ifndef playerselectorH
#define playerselectorH

#include "multilistbox.h"
#include "../gamemap.h"

class PlayerSelector : public MultiListBox {
      GameMap* gamemap;
      typedef PG_ListBoxDataItem<int> Item;
      void setup();
      int suppressPlayers;
   public:
      PlayerSelector (PG_Widget *parent, const PG_Rect &r, GameMap* map, bool multiselect = true, int suppress = 0 );
      int getSelectedPlayers();
      void setSelection( int s );
};


#endif
