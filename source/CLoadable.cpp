/***************************************************************************
                          CLoadable.cpp  -  description
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
#include "CLoadable.h"

#include "Property.h"
#include "gameoptions.h"

CLoadableGameOptions::CLoadableGameOptions(CGameOptions* pOptions)
	:	_pOptions(pOptions)
{
	add(new IntProperty("version"								,&_pOptions->version));
	add(new IntProperty("fastmove"								,&_pOptions->fastmove,3));
	add(new IntProperty("visibility_calc_algo"					,&_pOptions->visibility_calc_algo));
	add(new IntProperty("movespeed"								,&_pOptions->movespeed));
	add(new IntProperty("endturnquestion"						,&_pOptions->endturnquestion));
	add(new IntProperty("smallmapactive"						,&_pOptions->smallmapactive));
	add(new IntProperty("units_gray_after_move"					,&_pOptions->units_gray_after_move));
	add(new IntProperty("mapzoom"								,&_pOptions->mapzoom));
	add(new IntProperty("mapzoomeditor"							,&_pOptions->mapzoomeditor));
	add(new IntProperty("startupcount"							,&_pOptions->startupcount));
	add(new IntProperty("dontMarkFieldsNotAccessible_movement"	,&_pOptions->dontMarkFieldsNotAccessible_movement));
	add(new IntProperty("attackspeed1"							,&_pOptions->attackspeed1));
	add(new IntProperty("attackspeed2"							,&_pOptions->attackspeed2));
	add(new IntProperty("attackspeed3"							,&_pOptions->attackspeed3));
	
	add(new IntProperty("mouse.scrollbutton"					,&_pOptions->mouse.scrollbutton));
	add(new IntProperty("mouse.fieldmarkbutton"					,&_pOptions->mouse.fieldmarkbutton));
	add(new IntProperty("mouse.smallguibutton"					,&_pOptions->mouse.smallguibutton));
	add(new IntProperty("mouse.largeguibutton"					,&_pOptions->mouse.largeguibutton));
	add(new IntProperty("mouse.smalliconundermouse"				,&_pOptions->mouse.smalliconundermouse ));
	add(new IntProperty("mouse.centerbutton"					,&_pOptions->mouse.centerbutton ));
	add(new IntProperty("mouse.unitweaponinfo"					,&_pOptions->mouse.unitweaponinfo));
	add(new IntProperty("mouse.dragndropmovement"				,&_pOptions->mouse.dragndropmovement));
	
	add(new IntProperty("container.autoproduceammunition"	,	&_pOptions->container.autoproduceammunition));
	add(new IntProperty("container.filleverything"			,	&_pOptions->container.filleverything));
	add(new IntProperty("container.emptyeverything"			,	&_pOptions->container.emptyeverything));
	
	add(new IntProperty("onlinehelptime"					,	&_pOptions->onlinehelptime));
	add(new IntProperty("smallguiiconopenaftermove"			,	&_pOptions->smallguiiconopenaftermove));
	add(new IntProperty("defaultpassword"					,	&_pOptions->defaultpassword));
	add(new IntProperty("replayspeed"						,	&_pOptions->replayspeed));
	

	add(new TextProperty("bi3.dir"							,	&_pOptions->bi3.dir	)	);

	add(new IntProperty("bi3.interpolate.terrain"			,	&_pOptions->bi3.interpolate.terrain));
	add(new IntProperty("bi3.interpolate.units"				,	&_pOptions->bi3.interpolate.units));
	add(new IntProperty("bi3.interpolate.objects"			,	&_pOptions->bi3.interpolate.objects));
	add(new IntProperty("bi3.interpolate.buildings"			,	&_pOptions->bi3.interpolate.buildings));
	
	add(new IntProperty("changed"							,	&_pOptions->changed));
}

CLoadableGameOptions::~CLoadableGameOptions()
{}
	  
