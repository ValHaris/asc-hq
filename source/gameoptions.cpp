/*! \file gameoptions.cpp
    \brief The options which configure the behaviour of ASC
*/

/***************************************************************************
                          gameoptions.cpp  -  description
                             -------------------
    begin                : Thu Jun 29 2000
    copyright            : (C) 2000 by frank landgraf
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "global.h"
#include "gameoptions.h"
#include "basestrm.h"


CGameOptions* pStaticGameOptions=NULL;

class Destroyer
{
   public:
      ~Destroyer()
      {
         delete pStaticGameOptions;
      };
}
destroyer;

CGameOptions* CGameOptions::Instance()
{
   if (!pStaticGameOptions)
      pStaticGameOptions = new CGameOptions;
   return pStaticGameOptions;
}

const int CGameOptions::maxSearchPathNum = 30;

/*
CGameOptions::CGameOptions( const CGameOptions& cgo )
{
   copy ( cgo );
}

CGameOptions::CGameOptions(void)
{
   setDefaults();
}

*/

const int mouseButtonNum = 6;
const char* mouseButtons[mouseButtonNum] =
   { "none",
     "left",
     "center",
     "right",
     "4",
     "5" };


     
void CGameOptions::load( const ASCString& filename )
{
   tnfilestream s ( filename, tnstream::reading );

   TextFormatParser tfp ( &s );
   auto_ptr<TextPropertyGroup> tpg ( tfp.run());
   
   PropertyReadingContainer pc ( "config", tpg.get() );
 
   runTextIO( pc );
}   

void CGameOptions::save( const ASCString& filename )
{
   tn_file_buf_stream s ( filename, tnstream::writing );
   PropertyWritingContainer pc ( "config", s );
   runTextIO( pc );
}   
   
   
void CGameOptions::runTextIO ( PropertyContainer& pc )
{
   
   pc.addInteger("XResolution", xresolution, xresolution);
   pc.addInteger("YResolution", yresolution, yresolution);

   pc.addInteger("MapEditor_XResolution", mapeditor_xresolution, mapeditor_xresolution);
   pc.addInteger("MapEditor_YResolution", mapeditor_yresolution, mapeditor_yresolution);

   pc.openBracket("Sound");
   pc.addBool("MuteEffects", sound.muteEffects, sound.muteEffects);
   pc.addBool("MuteMusic", sound.muteMusic, sound.muteMusic);
   pc.addBool("Sound.Off", sound.off, sound.off);
   pc.addInteger("EffectsVolume", sound.soundVolume, sound.soundVolume );
   pc.addInteger("MusicVolume", sound.musicVolume, sound.musicVolume );
   pc.closeBracket();

   pc.addBool("FastMove", fastmove, fastmove);
   pc.addInteger("MovementSpeed", movespeed, movespeed );
   pc.addBool("EndTurnPrompt", endturnquestion, endturnquestion );
   pc.addBool("UnitsGrayAfterMove", units_gray_after_move, units_gray_after_move );
   pc.addInteger( "MapZoom", mapzoom, mapzoom);
   pc.addInteger( "MapZoomEditor", mapzoomeditor, mapzoomeditor );
   // pc.addInteger( "AttackSpeed1", attackspeed1, 0 );
   // pc.addInteger( "AttackSpeed2", attackspeed2, 0);
   // add(new IntProperty("AttackSpeed3"							,&_pOptions->attackspeed3));
   pc.addBool( "ForceWindowedMode", forceWindowedMode, forceWindowedMode );
   pc.addBool( "MapeditWindowedMode", mapeditWindowedMode, mapeditWindowedMode );
   pc.addBool( "AutomaticTraining", automaticTraining, automaticTraining );

   pc.openBracket("Mouse");
   // add(new IntProperty("Mouse.ScrollButton"					,&_pOptions->mouse.scrollbutton));
   pc.addNamedInteger("SelectFieldButton", mouse.fieldmarkbutton, mouseButtonNum, mouseButtons, mouse.fieldmarkbutton );
   // add(new IntProperty("Mouse.SmallGuiIconButton"			,&_pOptions->mouse.smallguibutton));
   // add(new IntProperty("Mouse.LargeGuiIconButton"			,&_pOptions->mouse.largeguibutton));
   // add(new IntProperty("Mouse.SmallGuiIconUnderMouse"		,&_pOptions->mouse.smalliconundermouse ));
   pc.addNamedInteger("MapCenterButton", mouse.fieldmarkbutton, mouseButtonNum, mouseButtons, mouse.fieldmarkbutton );
   // add(new IntProperty("Mouse.UnitWeaponInfoButton"		,&_pOptions->mouse.unitweaponinfo));
   // add(new IntProperty("Mouse.SingleClickAction"	   	,&_pOptions->mouse.singleClickAction));
   // add(new IntProperty("Mouse.dragndropmovement"		,&_pOptions->mouse.dragndropmovement));
   pc.closeBracket();

   pc.addBool("ProduceAmmoAutomatically", container.autoproduceammunition, container.autoproduceammunition );
   pc.addInteger("FillUnitsAutomatically", container.filleverything, container.filleverything );

   pc.addInteger("ToolTipHelpDelay", onlinehelptime, onlinehelptime );
   // add(new IntProperty("SmallGuiIconOpensAfterMove"	,	&_pOptions->smallguiiconopenaftermove));
   pc.addString("DefaultPassword", defaultPassword, defaultPassword );
   pc.addString("DefaultSupervisorPassword", defaultSuperVisorPassword, defaultSuperVisorPassword );
   pc.addInteger("ReplayDelay", replayspeed, replayspeed );
   // add(new IntProperty("ShowUnitOwner"                ,  &_pOptions->showUnitOwner));
   pc.addString("StartupMap", startupMap, startupMap );
   pc.addBool("DebugReplay", debugReplay, debugReplay );

   pc.addString("BI3.path", bi3.dir, bi3.dir );
   /*
   add(new IntProperty("BI3.interpolate.terrain"		,	&_pOptions->bi3.interpolate.terrain));
   add(new IntProperty("BI3.interpolate.units"			,	&_pOptions->bi3.interpolate.units));
   add(new IntProperty("BI3.interpolate.objects"		,	&_pOptions->bi3.interpolate.objects));
   add(new IntProperty("BI3.interpolate.buildings"		,	&_pOptions->bi3.interpolate.buildings));
*/

   int spn = 0;

   if ( pc.isReading() ) 
      pc.addInteger("SearchPathNum", spn, 0 );
   else {
      pc.addInteger("SearchPathNum", searchPathNum, 0 );
      spn = searchPathNum;
   }   
      
         
   for ( int i = 0; i < min(spn, 30); ++i )
      pc.addString(ASCString("SearchPath") + strrr(i), searchPath[i] );
      
   searchPathNum = spn;   
   
     
}

CGameOptions::CGameOptions()
{
   setDefaults();
}   


void CGameOptions::setDefaults ( void )
{
   forceWindowedMode = false;
   mapeditWindowedMode = true;
   fastmove = true;
   movespeed=15;
   endturnquestion=false;
   // smallmapactive=1;
   units_gray_after_move=false;
   mapzoom=75;
   mapzoomeditor=60;
   // startupcount=0;
   // dontMarkFieldsNotAccessible_movement=0;
   /*
   attackspeed1=0;
   attackspeed2=0;
   attackspeed3=0;
   */
   sound.off=false;
   sound.muteEffects=false;
   sound.muteMusic=false;
   sound.soundVolume=100;
   sound.musicVolume=100;

   // mouse.scrollbutton=0;
   mouse.fieldmarkbutton=1;
   // mouse.smallguibutton=1;
   // mouse.largeguibutton=0;
   //mouse.smalliconundermouse=2;  // 0: nie=0;  1: immer=0; 2: nur wenn vehicle, geb„ude, oder temp unter MAUS
   mouse.centerbutton=2;    // Maustaste zum zentrieren des fielder, ?ber dem sich die Maus befindet=0;

   /*
   mouse.unitweaponinfo=0;
   mouse.dragndropmovement=0;
   mouse.singleClickAction=0;
   */

   container.autoproduceammunition=true;
   container.filleverything=2;

   onlinehelptime=150;
   // smallguiiconopenaftermove=0;
   replayspeed=0;
//   showUnitOwner = 0;
   debugReplay = 0;

/*
   bi3.dir.setName( NULL );
   bi3.interpolate.terrain=0;
   bi3.interpolate.units=0;
   bi3.interpolate.objects=0;
   bi3.interpolate.buildings=0;
   */

   // defaultSuperVisorPassword.setName ( "" );
   startupMap =  "asc001.map";


#if USE_HOME_DIRECTORY == 0

   searchPathNum = 1;
   searchPath[0] =  ".\\" ;
#else

   searchPathNum = 6;
   
   searchPath[0] =  "~/.asc/" ;
   searchPath[1] = GAME_DATADIR ;
   searchPath[2] = "/var/local/games/asc/" ;
   searchPath[3] = "/var/games/asc/" ;
   searchPath[4] = "/usr/local/share/games/asc/" ;
   searchPath[5] =  "/usr/share/games/asc/" ;
#endif

   xresolution = 1024;
   yresolution = 768;

   mapeditor_xresolution = 1024;
   mapeditor_yresolution = 740;
   automaticTraining = true;

   setChanged();
}


/*
void CGameOptions::copy ( const CGameOptions& cgo )
{
   fastmove              = cgo.fastmove;
   movespeed             = cgo.movespeed;
   endturnquestion       = cgo.endturnquestion;
   smallmapactive        = cgo.smallmapactive;
   units_gray_after_move = cgo.units_gray_after_move;
   mapzoom               = cgo.mapzoom;
   mapzoomeditor         = cgo.mapzoomeditor;
   startupcount          = cgo.startupcount;
   debugReplay           = cgo.debugReplay;
   // dontMarkFieldsNotAccessible_movement=cgo.dontMarkFieldsNotAccessible_movement;
   attackspeed1          = cgo.attackspeed1;
   attackspeed2          = cgo.attackspeed2;
   attackspeed3          = cgo.attackspeed3;
   sound.off             = cgo.sound.off;
   sound.muteEffects     = cgo.sound.muteEffects;
   sound.muteMusic       = cgo.sound.muteMusic;
   sound.soundVolume     = cgo.sound.soundVolume;
   sound.musicVolume     = cgo.sound.musicVolume;

   mouse.scrollbutton    = cgo.mouse.scrollbutton;
   mouse.fieldmarkbutton = cgo.mouse.fieldmarkbutton;
   mouse.smallguibutton  = cgo.mouse.smallguibutton;
   mouse.largeguibutton  = cgo.mouse.largeguibutton;
   mouse.smalliconundermouse = cgo.mouse.smalliconundermouse;
   mouse.centerbutton    = cgo.mouse.centerbutton;
   mouse.unitweaponinfo  = cgo.mouse.unitweaponinfo;
   mouse.dragndropmovement=cgo.mouse.dragndropmovement;
   mouse.singleClickAction=cgo.mouse.singleClickAction;

   container.autoproduceammunition = cgo.container.autoproduceammunition;
   container.filleverything = cgo.container.filleverything;

   onlinehelptime            = cgo.onlinehelptime;
   smallguiiconopenaftermove = cgo.smallguiiconopenaftermove;
   defaultPassword           = cgo.defaultPassword;
   defaultSuperVisorPassword = cgo.defaultSuperVisorPassword;

   replayspeed               = cgo.replayspeed;
   showUnitOwner             = cgo.showUnitOwner;

   xresolution               = cgo.xresolution;
   yresolution               = cgo.yresolution;
   automaticTraining         = cgo.automaticTraining;

   bi3.dir.setName( cgo.bi3.dir.getName() );
   bi3.interpolate.terrain = cgo.bi3.interpolate.terrain;
   bi3.interpolate.units   = cgo.bi3.interpolate.units;
   bi3.interpolate.objects = cgo.bi3.interpolate.objects;
   bi3.interpolate.buildings = cgo.bi3.interpolate.buildings;
   for ( int i = 0; i < getSearchPathNum(); i++ )
      searchPath[i].setName ( cgo.searchPath[i].getName() );

   startupMap = cgo.startupMap;
   setChanged();
}
*/

int CGameOptions :: getSearchPathNum ( void )
{
   return searchPathNum;
}

void  CGameOptions ::  setSearchPath ( int i, const ASCString& path )
{
   searchPath[i] = path;
}

void  CGameOptions ::  addSearchPath ( const ASCString& path )
{
   setSearchPath( searchPathNum++, path );
}

ASCString CGameOptions :: getSearchPath( int i)
{
   return searchPath[i];
}   



Password CGameOptions :: getDefaultPassword ( )
{
   Password pwd;
   if ( !defaultPassword.empty() )
      pwd.setEncoded ( defaultPassword );

   return pwd;
}


Password CGameOptions :: getDefaultSupervisorPassword ( )
{
   Password pwd;
   if ( !defaultSuperVisorPassword.empty() )
      pwd.setEncoded ( defaultSuperVisorPassword );

   return pwd;
}
