
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "playerselector.h"


PlayerSelector :: PlayerSelector (PG_Widget *parent, const PG_Rect &r, GameMap* map, bool multiselect ) : MultiListBox( parent, r), gamemap ( map )
{
   setup();
};

void PlayerSelector :: setup()
{
   for ( int i = 0; i < gamemap->getPlayerCount(); ++i )
      if ( gamemap->getPlayer(i).exist() )
         new Item( getListBox(), 15, gamemap->getPlayer(i).getName(), i );
}


int PlayerSelector :: getSelectedPlayers()
{
   int result = 0;
   for ( int i = 0; i < getListBox()->GetWidgetCount(); ++i ) {
      Item* bi = dynamic_cast< Item* >( getListBox()->FindWidget(i) );
      if ( bi && bi->IsSelected() )
         result |= 1 << bi->getData();
    }
    return result;
}
