
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "playerselector.h"


PlayerSelector :: PlayerSelector (PG_Widget *parent, const PG_Rect &r, GameMap* map, bool multiselect, const PlayerAvailability* playerAvailability, int extra_spacing ) : MultiListBox( parent, r, multiselect), gamemap ( map ), playerAvailability(playerAvailability)
{
   for ( int i = 0; i < gamemap->getPlayerCount(); ++i) {
       Surface s = Surface::createSurface(15,15,8,20+i*8);
       s.assignDefaultPalette();
       icons.push_back( s );
   }
   setup(extra_spacing);
};

void PlayerSelector :: setup(int extra_spacing)
{
   for ( int i = 0; i < gamemap->getPlayerCount(); ++i )
      if ( playerAvailability->showPlayer(gamemap, i) )
         new Item( getListBox(), 15 + extra_spacing, gamemap->getPlayer(i).getName(), i, icons.at(i).GetSurface() );
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

int PlayerSelector :: getFirstSelectedPlayer()
{
   for ( int i = 0; i < getListBox()->GetWidgetCount(); ++i ) {
      Item* bi = dynamic_cast< Item* >( getListBox()->FindWidget(i) );
      if ( bi && bi->IsSelected() )
         return bi->getData();
    }
    return -1;
}

void PlayerSelector :: setSelection( int s )
{
   for ( int i = 0; i < getListBox()->GetWidgetCount(); ++i ) {
      Item* bi = dynamic_cast< Item* >( getListBox()->FindWidget(i) );
      if ( bi )
         bi->Select ( s & (1 << bi->getData()));
         // getListBox()->SelectItem( bi, s & (1 << bi->getData()));
    }
}

PlayerSelector::~PlayerSelector()
{
    delete playerAvailability;
}


PlayerSelector_ExistingExcept::PlayerSelector_ExistingExcept(int except) : except(except)
{
}

bool PlayerSelector_ExistingExcept::showPlayer(GameMap* gamemap, int p) const
{
    return gamemap->getPlayer(p).exist() && !(except & (1<<p));
}


PlayerSelector_AllExcept::PlayerSelector_AllExcept(int except) : except(except)
{
}

bool PlayerSelector_AllExcept::showPlayer(GameMap* gamemap, int p) const
{
    return !(except & (1<<p));
}

bool PlayerSelector_ExistingExceptCurrent::showPlayer(GameMap* gamemap, int p) const {
    return gamemap->getPlayer(p).exist() && p != gamemap->actplayer;
}
