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
#include "gameoptions.h"
#include "CLoadable.h"



CGameOptions::CGameOptions(void)
{
   setDefaults();
}

void CGameOptions::setDefaults ( void )
{
   fastmove	=	0;
   visibility_calc_algo=0;      // 0 sauber, 1 schnell=0;
   movespeed=0;
   endturnquestion=0;
   smallmapactive=0;
   units_gray_after_move=0;
   mapzoom=0;
   mapzoomeditor=0;
   startupcount=0;
   dontMarkFieldsNotAccessible_movement=0;
   attackspeed1=0;
   attackspeed2=0;
   attackspeed3=0;
   disablesound=0;

   mouse.scrollbutton=0;
   mouse.fieldmarkbutton=0;
   mouse.smallguibutton=0;
   mouse.largeguibutton=0;
   mouse.smalliconundermouse=0;  // 0: nie=0;  1: immer=0; 2: nur wenn vehicle, gebÑude, oder temp unter MAUS
   mouse.centerbutton=0;    // Maustaste zum zentrieren des fielder, Åber dem sich die Maus befindet=0;
   mouse.unitweaponinfo=0;
   mouse.dragndropmovement=0;

   container.autoproduceammunition=0;
   container.filleverything=0;

   container.emptyeverything=0;

   onlinehelptime=0;
   smallguiiconopenaftermove=0;
   defaultpassword=0;
   replayspeed=0;


   bi3.dir.setName( NULL );
   bi3.interpolate.terrain=0;
   bi3.interpolate.units=0;
   bi3.interpolate.objects=0;
   bi3.interpolate.buildings=0;

   changed	=	0;
}

void CGameOptions::copy ( const CGameOptions& cgo )
{
   fastmove	             = cgo.fastmove;
   visibility_calc_algo  = cgo.visibility_calc_algo;
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
   disablesound          = cgo.disablesound;

   mouse.scrollbutton    = cgo.mouse.scrollbutton;
   mouse.fieldmarkbutton = cgo.mouse.fieldmarkbutton;
   mouse.smallguibutton  = cgo.mouse.smallguibutton;
   mouse.largeguibutton  = cgo.mouse.largeguibutton;
   mouse.smalliconundermouse = cgo.mouse.smalliconundermouse;  
   mouse.centerbutton    = cgo.mouse.centerbutton;
   mouse.unitweaponinfo  = cgo.mouse.unitweaponinfo;
   mouse.dragndropmovement=cgo.mouse.dragndropmovement;

   container.autoproduceammunition = cgo.container.autoproduceammunition;
   container.filleverything = cgo.container.filleverything;

   container.emptyeverything = cgo.container.emptyeverything;

   onlinehelptime            = cgo.onlinehelptime;
   smallguiiconopenaftermove = cgo.smallguiiconopenaftermove;
   defaultpassword           = cgo.defaultpassword;
   replayspeed               = cgo.replayspeed;


   bi3.dir.setName( cgo.bi3.dir.getName() );
   bi3.interpolate.terrain = cgo.bi3.interpolate.terrain;
   bi3.interpolate.units   = cgo.bi3.interpolate.units;
   bi3.interpolate.objects = cgo.bi3.interpolate.objects;
   bi3.interpolate.buildings = cgo.bi3.interpolate.buildings;

   changed = 1;
}


CGameOptions gameoptions;
