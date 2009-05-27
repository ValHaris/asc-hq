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
   pc.addString("GraphicsDriver", graphicsDriver, graphicsDriver );
   pc.addBool("HardwareSurface", hardwareSurface, hardwareSurface );

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
   pc.addInteger( "AttackSpeed1", attackspeed1, 30 );
   pc.addInteger( "AttackSpeed2", attackspeed2, 50 );
   pc.addInteger( "AttackSpeed3", attackspeed3, 30 );

   pc.addBool( "ForceWindowedMode", forceWindowedMode, forceWindowedMode );
   pc.addBool( "MapeditWindowedMode", mapeditWindowedMode, mapeditWindowedMode );
   pc.addBool( "MapeditModalSelectionWindow", maped_modalSelectionWindow, maped_modalSelectionWindow );
   pc.addBool( "AutomaticTraining", automaticTraining, automaticTraining );

   pc.openBracket("Mouse");
   // add(new IntProperty("Mouse.ScrollButton"					,&_pOptions->mouse.scrollbutton));
   pc.addNamedInteger("SelectFieldButton", mouse.fieldmarkbutton, mouseButtonNum, mouseButtons, mouse.fieldmarkbutton );
   // add(new IntProperty("Mouse.SmallGuiIconButton"			,&_pOptions->mouse.smallguibutton));
   // add(new IntProperty("Mouse.LargeGuiIconButton"			,&_pOptions->mouse.largeguibutton));
   // add(new IntProperty("Mouse.SmallGuiIconUnderMouse"		,&_pOptions->mouse.smalliconundermouse ));
   pc.addNamedInteger("MapCenterButton", mouse.centerbutton, mouseButtonNum, mouseButtons, mouse.centerbutton );
   pc.addNamedInteger("DragNdrop", mouse.dragndropbutton, mouseButtonNum, mouseButtons, mouse.dragndropbutton );
   pc.addNamedInteger("ZoomOut", mouse.zoomoutbutton, mouseButtonNum, mouseButtons, mouse.zoomoutbutton );
   pc.addNamedInteger("ZoomIn", mouse.zoominbutton, mouseButtonNum, mouseButtons, mouse.zoominbutton );
   // add(new IntProperty("Mouse.UnitWeaponInfoButton"		,&_pOptions->mouse.unitweaponinfo));
   pc.addBool("SingleClickAction", mouse.singleClickAction, false );
   // add(new IntProperty("Mouse.dragndropmovement"		,&_pOptions->mouse.dragndropmovement));
   pc.addBool("HideOnScreenUpdates",hideMouseOnScreenUpdates, hideMouseOnScreenUpdates );
   pc.closeBracket();

   pc.addBool("AmmoProductionOnRefuelling", autoproduceammunition, autoproduceammunition );
   // pc.addInteger("FillUnitsAutomatically", container.filleverything, container.filleverything );

   // pc.addInteger("ToolTipHelpDelay", onlinehelptime, onlinehelptime );
   // add(new IntProperty("SmallGuiIconOpensAfterMove"	,	&_pOptions->smallguiiconopenaftermove));
   pc.addString("DefaultPassword", defaultPassword, defaultPassword );
   pc.addString("DefaultSupervisorPassword", defaultSuperVisorPassword, defaultSuperVisorPassword );
   pc.addInteger("ReplayDelay", replayspeed, replayspeed );
   // add(new IntProperty("ShowUnitOwner"                ,  &_pOptions->showUnitOwner));
   pc.addString("StartupMap", startupMap, startupMap );
   pc.addBool("DebugReplay", debugReplay, debugReplay );

   pc.addString("BI3.path", BI3directory, BI3directory );

   pc.openBracket("UnitProduction");
   pc.addBool("fillResources", unitProduction.fillResources, true );
   pc.addBool("fillAmmo", unitProduction.fillAmmo, true );
   pc.closeBracket();
   
   pc.openBracket("replayVideo");
   pc.addInteger("FrameRate", video.framerate, video.framerate );
   pc.addInteger("Quality", video.quality, video.quality);
   pc.closeBracket();
   
   if ( !pc.isReading() || pc.find("VisibleMapLayer" ))
      pc.addStringArray("VisibleMapLayer", visibleMapLayer );


   pc.openBracket("PBEMServer");
   pc.addString("hostname", pbemServer.hostname, pbemServer.hostname );
   pc.addString("username", pbemServer.username, pbemServer.username );
   pc.addInteger("port", pbemServer.port, pbemServer.port );
   pc.closeBracket();

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


   pc.addBool( "cacheASCGUI", cacheASCGUI, cacheASCGUI );


   vector<ASCString> panels;
   if ( pc.isReading() ) {
      if ( pc.find( "Panels"))
         pc.addStringArray( "Panels", panels );
   } else {
      for ( PanelDataContainer::iterator i = panelData.begin(); i != panelData.end(); ++i )
         panels.push_back( i->first );
      pc.addStringArray( "Panels", panels );
   }

   for ( vector<ASCString>::iterator i = panels.begin(); i != panels.end(); ++i ){
      PanelData& pd = panelData[*i];
      pc.openBracket( *i );
      pc.addInteger( "x", pd.x );
      pc.addInteger( "y", pd.y );
      pc.addBool( "active", pd.visible );
      pc.closeBracket();
   }


   pc.openBracket("DialogMemory");
   vector<ASCString> dialogMemory;
   if ( pc.isReading() ) {
      if ( pc.find( "dialogs"))
         pc.addStringArray( "dialogs", dialogMemory );
   } else {
      for ( DialogAnswers::iterator i = dialogAnswers.begin(); i != dialogAnswers.end(); ++i )
         dialogMemory.push_back( i->first );
      pc.addStringArray( "dialogs", dialogMemory );
   }

   for ( vector<ASCString>::iterator i = dialogMemory.begin(); i != dialogMemory.end(); ++i ) {
      int& value = dialogAnswers[*i];
      pc.addInteger( *i, value );
   }

   pc.closeBracket();

   pc.addInteger( "PanelColumns", panelColumns, 2 );
     
   pc.addString("mailProgram", mailProgram, "");
     
   pc.addInteger( "AircraftCrashWarnTime", aircraftCrashWarningTime, 1 );
}

CGameOptions::CGameOptions()
{
   setDefaults();
}   


void CGameOptions::setDefaults ( void )
{
   hideMouseOnScreenUpdates = true;
   forceWindowedMode = false;
   mapeditWindowedMode = true;
   maped_modalSelectionWindow = true;
   fastmove = true;
   movespeed=15;
   endturnquestion=false;
   // smallmapactive=1;
   units_gray_after_move=false;
   mapzoom=75;
   mapzoomeditor=60;
   // startupcount=0;
   // dontMarkFieldsNotAccessible_movement=0;
   
   attackspeed1=30;
   attackspeed2=50;
   attackspeed3=30;
   
   sound.off=false;
   sound.muteEffects=false;
   sound.muteMusic=false;
   sound.soundVolume=100;
   sound.musicVolume=100;

   // mouse.scrollbutton=0;
   mouse.fieldmarkbutton=1;
   // mouse.smallguibutton=1;
   // mouse.largeguibutton=0;
   //mouse.smalliconundermouse=2;  // 0: nie=0;  1: immer=0; 2: nur wenn vehicle, gebude, oder temp unter MAUS
   mouse.centerbutton=2;    // Maustaste zum zentrieren des fielder, ?ber dem sich die Maus befindet=0;
   mouse.dragndropbutton = 3;
   mouse.zoomoutbutton = 4;
   mouse.zoominbutton = 5;
   mouse.singleClickAction = false;

   replayspeed=0;
   debugReplay = 0;

   autoproduceammunition = false;
/*
   bi3.dir.setName( NULL );
   bi3.interpolate.terrain=0;
   bi3.interpolate.units=0;
   bi3.interpolate.objects=0;
   bi3.interpolate.buildings=0;
   */

   // defaultSuperVisorPassword.setName ( "" );
   startupMap =  "asc001.map";

   cacheASCGUI = true;

   setDefaultDirectories();

   xresolution = 1024;
   yresolution = 768;

   mapeditor_xresolution = 1024;
   mapeditor_yresolution = 740;

   graphicsDriver = "default";
   hardwareSurface = false;

   automaticTraining = true;

   replayMovieMode = false;

   unitProduction.fillAmmo = true;
   unitProduction.fillResources = true;

   video.quality = 100;
   video.framerate = 15;
   
   panelColumns = 2;

   pbemServer.username="";
   pbemServer.port = 8080;
   pbemServer.hostname="terdon.asc-hq.org";

   aircraftCrashWarningTime = 1;
   
   setChanged();
}


void CGameOptions::setDefaultDirectories()
{
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

}

ASCString CGameOptions::Mouse::getButtonName(int button)
{
   static const char* my_mouseButtonNames[] = { "None", "Left", "Center", "Right", "4", "5", NULL };
   if ( button <= 3 ) 
      return my_mouseButtonNames[button];
   else
      return ASCString::toString( button );
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

void CGameOptions :: updatePanelData( const ASCString& name, PanelData data )
{
   panelData[name] = data;
   setChanged();
}

bool CGameOptions :: getPanelData( const ASCString& name, PanelData& data )
{
   if ( panelData.find( name ) != panelData.end() ) {
      data = panelData[name];
      return true;
   } else
      return false;
}

