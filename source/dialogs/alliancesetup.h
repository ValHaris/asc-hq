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


#ifndef ALLIANCESETUP_H
#define ALLIANCESETUP_H

#include <map>
#include <pgscrollwidget.h>
#include "../gamemap.h"

class AllianceSetupWidget : public PG_ScrollWidget {
      GameMap* actmap;
      bool allEditable;
      
      struct PlayerWidgets {
         PG_LineEdit* name;
         PG_DropDown* type;
         int pos;
      };

   public:   
      enum DiplomaticTransitions { SNEAK_ATTACK, TO_WAR, TO_TRUCE, TO_PEACE, TO_PEACE_SV, TO_ALLIANCE };
      
   protected:   
      
      vector< vector< DiplomaticTransitions > > stateChanges;
      vector< vector< DiplomaticStates > > states;
      
      typedef map<int,PG_Widget*> DiplomaticWidgets;
      DiplomaticWidgets diplomaticWidgets;
         
      vector<PlayerWidgets> playerWidgets;
      
      int linearize( int actingPlayer, int secondPlayer  );
      void setState( DiplomaticStates s, int actingPlayer, int secondPlayer );
      
      DiplomaticStates& getState( int actingPlayer, int secondPlayer );

   public:
      
      class ApplyStrategy {
         public:
            virtual void sneakAttack ( GameMap* map, int actingPlayer, int towardsPlayer ) = 0;
            virtual void setState ( GameMap* map, int actingPlayer, int towardsPlayer, DiplomaticStates newState ) = 0;
            virtual ~ApplyStrategy(){};
      };
      
      AllianceSetupWidget( GameMap* gamemap, ApplyStrategy* applyStrategy, bool allEditable, PG_Widget *parent, const PG_Rect &r, const std::string &style="ScrollWidget" );
      void Apply();
      ~AllianceSetupWidget();
      
   private:
      ApplyStrategy* strategy;

};

class DirectAllianceSetupStrategy : public AllianceSetupWidget::ApplyStrategy {
   virtual void sneakAttack ( GameMap* map, int actingPlayer, int towardsPlayer );
   virtual void setState ( GameMap* map, int actingPlayer, int towardsPlayer, DiplomaticStates newState );
};


/**  runs the Alliance-Setup dialog.
      \returns if the view should be recalculated
*/
bool  setupalliances( GameMap* actmap, AllianceSetupWidget::ApplyStrategy* strategy, bool supervisor = false );


#endif

