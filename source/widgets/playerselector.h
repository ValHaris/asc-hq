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
      vector<Surface> icons;
      void setup(int extra_spacing);
   public:

      class PlayerAvailability {
      public:
          virtual bool showPlayer(GameMap* gamemap, int p) const =0;
          virtual ~PlayerAvailability() {};
      };

   private:
      const PlayerAvailability* playerAvailability;

   public:

      typedef PG_ListBoxDataItem<int> Item;

      // Will take ownership of playerAvailability and delete it
      PlayerSelector (PG_Widget *parent, const PG_Rect &r, GameMap* map, bool multiselect = true, const PlayerAvailability* playerAvailability = NULL, int extra_spacing = 0 );

      //! this is bitmapped
      int getSelectedPlayers();

      //! this is numerical. Returns -1 is nothing was selected.
      int getFirstSelectedPlayer();

      void setSelection( int s );

      ~PlayerSelector();
};

class PlayerSelector_ExistingExcept: public PlayerSelector::PlayerAvailability {
    int except;
public:
    PlayerSelector_ExistingExcept(int except=0);
    bool showPlayer(GameMap* gamemap, int p) const;
};

class PlayerSelector_ExistingExceptCurrent: public PlayerSelector::PlayerAvailability {
public:
    bool showPlayer(GameMap* gamemap, int p) const;
};

class PlayerSelector_AllExcept: public PlayerSelector::PlayerAvailability {
    int except;
public:
    PlayerSelector_AllExcept(int except=0);
    bool showPlayer(GameMap* gamemap, int p) const;
};

#endif
