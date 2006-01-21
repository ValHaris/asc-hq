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


#ifndef PLAYERSETUP_H
#define PLAYERSETUP_H

#include <pgscrollwidget.h>
#include "../gamemap.h"
#include "../paradialog.h"

class PlayerSetupWidget : public PG_ScrollWidget {
      tmap* actmap;
      static const int spacing = 40;
      
      struct PlayerWidgets {
         PG_LineEdit* name;
         PG_DropDown* type;
         int pos;
      };
         
      
      vector<PlayerWidgets> playerWidgets;
   public:
      enum Mode{ AllEditable, AllEditableSinglePlayer, SelfEditable };
      PlayerSetupWidget( tmap* gamemap, Mode mode, PG_Widget *parent, const PG_Rect &r, const std::string &style="ScrollWidget" );
      bool Valid();
      bool Apply();
   private:
      Mode mode;   
};

void  setupPlayers( tmap* actmap, bool supervisor = false );


#endif

