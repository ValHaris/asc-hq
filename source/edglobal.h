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

//*Actions fr Editor

#ifndef edglobalH
#define edglobalH

#include "ed_mapcomponent.h"

extern int infomessage( char* formatstring, ... );

const int execactionscount = 116;

extern const char*  execactionnames[execactionscount];

enum tuseractions {
     act_end,
     act_help,
     act_seteditmode,
     act_selbodentyp,
     act_selbodentypAll,
     act_selunit,
     act_selcolor,
     act_selbuilding,
     act_selobject,
     act_selobjectAll,
     act_selmine,
     act_selweather,
     act_setupalliances,
     act_setupplayers,
     act_toggleresourcemode,
     act_changeunitdir,
     act_asc_resource,
     act_maptopcx,
     act_loadmap,
     act_changeplayers,
     act_newmap,
     act_polymode,
     act_repaintdisplay,
     act_unitinfo,
     act_viewmap,
     act_about,
     act_createresources,
     act_changecargo,
     act_changeresources,
     act_changeterraindir,
     act_events,
     act_fillmode,
     act_mapgenerator,
     act_setactivefieldvals,
     act_deletething,
     act_showpalette,
     act_changeminestrength,
     act_changemapvals,
     act_changeproduction,
     act_savemap,
     act_changeunitvals,
     act_mirrorcursorx,
     act_mirrorcursory,
     act_placemine,
     act_placething,
     act_deleteunit,
     act_deletebuilding,
     act_deleteobject,
     act_deletetopmostobject,
     act_deleteallobjects,
     act_deletemine,
     act_aboutbox,
     act_savemapas,
     act_endpolyfieldmode,
     act_smoothcoasts,
     act_import_bi_map,
     act_seperator,
     act_bi_resource,
     act_resizemap,
     act_insert_bi_map,
     act_setzoom,
     act_movebuilding,
     act_setactweatherglobal,
     act_setmapparameters,
     act_terraininfo,
     act_setunitfilter,
     act_selectgraphicset,
     act_unitsettransformation,
     act_unitSetInformation,
     act_switchmaps,
     act_transformMap,
     act_editArchivalInformation,
     act_displayResourceComparison,
     act_specifyunitproduction,
     act_pasteFromClipboard,
     act_copyToClipboard,
     act_cutToClipboard,
     act_saveClipboard,
     act_readClipBoard,
     act_setTurnNumber,
     act_showPipeNet,
     act_editResearch,
     act_editResearchPoints,
     act_generateTechTree,
     act_editTechAdapter,
     act_resetPlayerData,
     act_createresources2,
     act_setactnewweather,
     act_primaryAction, 
     act_playerStrengthSummary,
     act_increase_zoom,
     act_decrease_zoom,
     act_editpreferences,
     act_clearresources,
     act_dumpBuilding,
     act_dumpVehicle,
     act_dumpObject,
     act_pbpstatistics,
     act_exchangeGraphics,
     act_openControlPanel,
     act_releaseControlPanel,
     act_dumpAllVehicleDefinitions,
     act_clearSelection,
     act_dumpAllBuildings,
     act_locateItemByID,
     act_mirrorMap,
     act_copyArea,
     act_pasteArea,
     asc_nullPointerCrash,
     asc_testFunction,
     act_unitGuideDialog,
     act_runLuaScript,
     act_runTranslationScript,
     act_dumpAllTerrain,
     act_dumpAllObjects,
     act_showweapnrange

 };

class SelectionHolder : public sigc::trackable {
     const Placeable* currentItem;
     int actplayer;
     int currentWeather;
     
  public:
     SelectionHolder() : currentItem(NULL), actplayer(0), currentWeather(0),brushSize(1) {};
 
     int getPlayer() { return actplayer; };
     void setPlayer( int player );
     sigc::signal<void,int> playerChanged;
     
     void setWeather( int weather );
     int getWeather() { return currentWeather; };
     
     int brushSize;
     const Placeable* getSelection();
     void setSelection( const Placeable& component ) ;
     void pickup ( MapField* fld );
     
     sigc::signal<void,const Placeable*> selectionChanged;
     void clear(); 
};
 
extern SelectionHolder selection;

  
 
extern void         execaction_ev(int code);
extern void         execaction(int code);

#endif
