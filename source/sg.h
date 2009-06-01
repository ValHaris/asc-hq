/*! \file sg.h
    \brief Interface for various global functions and variables
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


#ifndef sgH
#define sgH

#include "actions/context.h"
class ASCString;

enum tuseractions { ua_repainthard,     ua_repaint, ua_help, ua_mainmenu, ua_mntnc_morefog,
                    ua_mntnc_lessfog,   ua_mntnc_morewind,   ua_mntnc_lesswind, ua_mntnc_rotatewind, ua_changeresourceview,
                    ua_benchgamewv,     ua_benchgamewov,     ua_viewterraininfo, ua_unitweightinfo,  ua_writemaptopcx,  ua_writescreentopcx,
                    ua_changepassword, ua_gamepreferences, ua_bi3preferences,
                    ua_exitgame,        ua_loadgame,  ua_savegame, ua_setupalliances, ua_settribute, ua_giveunitaway,
                    ua_vehicleinfo,     ua_researchinfo,     ua_newmessage, ua_viewqueuedmessages,
                    ua_viewsentmessages, ua_viewreceivedmessages, ua_viewjournal, ua_editjournal, ua_viewaboutmessage, ua_continuenetworkgame,
                    ua_toggleunitshading, ua_computerturn, ua_setupnetwork, ua_howtostartpbem, ua_howtocontinuepbem, 
                    ua_selectgraphicset, ua_UnitSetInfo, ua_GameParameterInfo, ua_GameStatus, ua_viewunitweaponrange, ua_viewunitmovementrange,
                    ua_aibench, ua_networksupervisor, ua_selectPlayList, ua_soundDialog, ua_reloadDlgTheme, ua_showPlayerSpeed, 
                    ua_statisticdialog, ua_viewPipeNet, ua_cancelResearch, ua_showResearchStatus, ua_exportUnitToFile, ua_viewButtonPanel,
                    ua_viewWindPanel, ua_clearImageCache, ua_viewUnitInfoPanel, ua_cargosummary, ua_viewUnitInfoDialog, ua_showsearchdirs, ua_viewOverviewMapPanel, ua_weathercast,
                    ua_newGame, ua_testMessages, ua_viewMapControlPanel, ua_unitsummary, ua_togglesound, ua_increase_zoom, ua_decrease_zoom, ua_editPlayerData,
                    ua_SDLinfo, ua_visibilityInfo, ua_locatefile, ua_viewfont, ua_resourceAnalysis, ua_unitproductionanalysis, ua_continuerecentnetworkgame, ua_gotoPosition, ua_loadrecentgame,
                    ua_showTechAdapter, ua_showUnitEndurance, ua_getMemoryFootprint, ua_viewlayerhelp, ua_showCargoLayer, ua_showMiningPower, ua_emailOptions, ua_createReminder, ua_undo, ua_redo,
                    ua_writeLuaCommands, ua_runLuaCommands, ua_recompteview, ua_unitGuideDialog, ua_turnUnitLeft, ua_turnUnitRight,
                    ua_viewActionPanel, ua_chooseTechnology, ua_actionManager };

extern void execuseraction ( tuseractions action );

//! takes care of event system switching between old and new functions
extern void execUserAction_ev( tuseractions action );

class GameMap;
extern void hookGuiToMap( GameMap* map );

extern bool loadGame( bool mostrecent );
extern bool loadGameFromFile( const ASCString& filename );
extern void saveGame( bool as );
extern bool continueAndStartMultiplayerGame( bool mostRecent );
extern void changePassword();

extern Context createContext( GameMap* gamemap );


#endif

