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


#ifndef emailsetupH
#define emailsetupH

#include <map>
#include <pgscrollwidget.h>
#include "../gamemap.h"

class EmailSetupWidget : public PG_ScrollWidget {
      GameMap* actmap;
      int  editable;
      
      struct PlayerWidgets {
         PG_LineEdit* name;
         PG_LineEdit* email;
         int pos;
      };
      static const int spacing = 40;

      vector<PlayerWidgets> playerWidgets;
      
   public:
      EmailSetupWidget( GameMap* gamemap, int editablePlayers, PG_Widget *parent, const PG_Rect &r, const std::string &style="ScrollWidget" );
      void Apply();
};




#endif

