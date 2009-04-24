/*! \file asc-mainscreen.h
    \brief Everything that happens on the main screen of ASC
*/

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


#ifndef ascmainscreenH
 #define ascmainscreenH 

#include "paradialog.h"
#include "util/messaginghub.h"
#include "mainscreenwidget.h"

class Menu;
class NewGuiHost;
class MapDisplayPG;
class DashboardPanel;
class UnitWeaponRangeLayer;
class UnitMovementRangeLayer;
class GameMap;
class MapCoordinate;
class WindInfoPanel;
class MapInfoPanel;
class ActionInfoPanel;


/** The widget that controls the main screen of ASC.
    It covers the whole ASC window and will be the parent for all other elements, like
    - map display (	#MapDisplayPG )
	- the button for controlling units ( #NewGuiHost )
    - main menu bar at the top ( #Menu )
    - the various panels displaying information ( children of #DashboardPanel )
	- dialog boxes (children of #ASC_PG_Dialog )
   
    The ASC_MainScreenWidget also controls all the global keyboard shortcuts ( \see ASC_MainScreenWidget::eventKeyDown)   
*/

class ASC_MainScreenWidget : public MainScreenWidget {
      int standardActionsLocked;
   public:
      ASC_MainScreenWidget( PG_Application& application );
      enum Panels { ButtonPanel, WindInfo, UnitInfo, OverviewMap, MapControl, ActionInfo };
      void spawnPanel ( Panels panel );
      void spawnPanel ( const ASCString& panelName );
   
      NewGuiHost* getGuiHost() { return guiHost; };
      DashboardPanel* getUnitInfoPanel() { return unitInfoPanel; };
      
      void showMovementRange( GameMap* gamemap, const MapCoordinate& pos );
      void showWeaponRange( GameMap* gamemap, const MapCoordinate& pos );
      
      ~ASC_MainScreenWidget() { };
   protected:
      NewGuiHost* guiHost;
      Menu* menu;
      DashboardPanel* unitInfoPanel;
      WindInfoPanel* windInfoPanel;
      MapInfoPanel* mapInfoPanel;
      ActionInfoPanel* actionInfoPanel;
      
      UnitWeaponRangeLayer* weaponRangeLayer;
      UnitMovementRangeLayer* movementRangeLayer;
               
      bool eventKeyDown(const SDL_KeyboardEvent* key);
      ASCString getBackgroundImageFilename() { return "gamebackground.png"; };

      void lockStandardActions( int dir, int options);
      
      list<int> lockOptionStack;
   private:
      void mapLayerChanged(bool b, const ASCString& name );
      
};

//! displays a message in the status line of ASC
extern void displaymessage2( const char* formatstring, ... );

extern ASC_MainScreenWidget*  mainScreenWidget ;

#endif

