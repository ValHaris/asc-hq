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
#include "CLoadable.h"
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
      pStaticGameOptions	=	new CGameOptions;
   return pStaticGameOptions;
}

const int CGameOptions::searchPathNum	=	10;

CGameOptions::CGameOptions( const CGameOptions& cgo )
{
   searchPath = new Named[ searchPathNum ];
   copy ( cgo );
}

CGameOptions::CGameOptions(void)
{
   searchPath = new Named[ searchPathNum ];
   setDefaults();
}

void CGameOptions::setDefaults ( void )
{
   forceWindowedMode = 0;
   mapeditForceFullscreenMode = 0;
   runAI = 0;
   fastmove	=	1;
   movespeed=15;
   endturnquestion=0;
   smallmapactive=1;
   units_gray_after_move=0;
   mapzoom=75;
   mapzoomeditor=60;
   startupcount=0;
   dontMarkFieldsNotAccessible_movement=0;
   attackspeed1=0;
   attackspeed2=0;
   attackspeed3=0;
   sound.off=0;
   sound.muteEffects=0;
   sound.muteMusic=0;
   sound.soundVolume=100;
   sound.musicVolume=100;
   specialForestChaining = 1;
   heightChangeMovement = 1;

   mouse.scrollbutton=0;
   mouse.fieldmarkbutton=2;
   mouse.smallguibutton=1;
   mouse.largeguibutton=0;
   mouse.smalliconundermouse=2;  // 0: nie=0;  1: immer=0; 2: nur wenn vehicle, geb„ude, oder temp unter MAUS
   mouse.centerbutton=4;    // Maustaste zum zentrieren des fielder, ?ber dem sich die Maus befindet=0;

   mouse.unitweaponinfo=0;
   mouse.dragndropmovement=0;
   mouse.singleClickAction=0;

   container.autoproduceammunition=1;
   container.filleverything=1;

   container.emptyeverything=1;

   onlinehelptime=100;
   smallguiiconopenaftermove=0;
   replayspeed=0;
   showUnitOwner = 0;


   bi3.dir.setName( NULL );
   bi3.interpolate.terrain=0;
   bi3.interpolate.units=0;
   bi3.interpolate.objects=0;
   bi3.interpolate.buildings=0;

   defaultSuperVisorPassword.setName ( "" );


#if USE_HOME_DIRECTORY == 0
   searchPath[0].setName ( ".\\" );
   for ( int i = 1; i < getSearchPathNum(); i++ )
      searchPath[i].setName ( NULL );
#else
for ( int i = 0; i < getSearchPathNum(); i++ )
   searchPath[i].setName ( NULL );
   searchPath[0].setName ( "~/.asc/" );
   searchPath[1].setName ( "/var/local/games/asc/" );
   searchPath[2].setName ( "/var/games/asc/" );
   searchPath[3].setName ( "/usr/local/share/games/asc/" );
   searchPath[4].setName ( "/usr/share/games/asc/" );
   ASCString s = GAME_DATADIR;
   s += pathdelimitterstring;
   if ( s != searchPath[3].getName() )
      searchPath[5].setName ( s.c_str() );
#endif

   xresolution = 1024;
   yresolution = 768;

   mapeditor_xresolution = 1024;
   mapeditor_yresolution = 768;

   setChanged();
}

void CGameOptions::copy ( const CGameOptions& cgo )
{
   fastmove	             = cgo.fastmove;
   heightChangeMovement  = cgo.heightChangeMovement;
   movespeed             = cgo.movespeed;
   endturnquestion       = cgo.endturnquestion;
   smallmapactive        = cgo.smallmapactive;
   units_gray_after_move = cgo.units_gray_after_move;
   mapzoom               = cgo.mapzoom;
   mapzoomeditor         = cgo.mapzoomeditor;
   startupcount          = cgo.startupcount;
   dontMarkFieldsNotAccessible_movement=cgo.dontMarkFieldsNotAccessible_movement;
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

   container.emptyeverything = cgo.container.emptyeverything;

   onlinehelptime            = cgo.onlinehelptime;
   smallguiiconopenaftermove = cgo.smallguiiconopenaftermove;
   defaultPassword           = cgo.defaultPassword;
   defaultSuperVisorPassword = cgo.defaultSuperVisorPassword;

   replayspeed               = cgo.replayspeed;
   showUnitOwner             = cgo.showUnitOwner;

   xresolution               = cgo.xresolution;
   yresolution               = cgo.yresolution;

   bi3.dir.setName( cgo.bi3.dir.getName() );
   bi3.interpolate.terrain = cgo.bi3.interpolate.terrain;
   bi3.interpolate.units   = cgo.bi3.interpolate.units;
   bi3.interpolate.objects = cgo.bi3.interpolate.objects;
   bi3.interpolate.buildings = cgo.bi3.interpolate.buildings;
   for ( int i = 0; i < getSearchPathNum(); i++ )
      searchPath[i].setName ( cgo.searchPath[i].getName() );

   setChanged();
}

int CGameOptions :: getSearchPathNum ( void )
{
   return searchPathNum;
}

void  CGameOptions ::  setSearchPath ( int i, const char* path )
{
   searchPath[i].setName( path );
}


Password CGameOptions :: getDefaultPassword ( )
{
   Password pwd;
   if ( defaultPassword.getName() )
      pwd.setEncoded ( defaultPassword.getName() );

   return pwd;
}


Password CGameOptions :: getDefaultSupervisorPassword ( )
{
   Password pwd;
   if ( defaultSuperVisorPassword.getName() )
      pwd.setEncoded ( defaultSuperVisorPassword.getName() );

   return pwd;
}
