/*! \file objects.cpp
    \brief Mine and Objects which can be placed on a map field
*/

/***************************************************************************
                          gamemap.cpp  -  description
                             -------------------
    begin                : Tue May 21 2005
    copyright            : (C) 2005 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>
#include <ctime>
#include <cmath>

#include "global.h"
#include "misc.h"
#include "typen.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "itemrepository.h"
#include "graphics/blitter.h"
#include "iconrepository.h"
#include "objects.h"
#include "graphics/blitter.h"


const char* MineNames[cminenum]  = {"antipersonnel mine", "antitank mine", "antisub mine", "antiship mine"};
const int MineBasePunch[cminenum]  = { 60, 120, 180, 180 };

void MineType :: paint ( Surface& surface, const SPoint& pos ) const 
{
   paint( type, 0, surface, pos );
}

void MineType::paint( MineTypes type, int player, Surface& surf, const SPoint& pos )
{
   static Surface* images[5] = { NULL, NULL, NULL, NULL, NULL };
   if ( !images[type] ) {
      switch ( type ) {
         case cmantipersonnelmine: images[type] = & IconRepository::getIcon( "antipersonellmine.png" );
         break;
         case cmantitankmine: images[type] = & IconRepository::getIcon( "antitankmine.png" );
         break;
         case cmfloatmine:
         case cmmooredmine: images[type] = & IconRepository::getIcon( "antishipmine.png" );
         break;
     };
   }
   if ( images[type] ) {
      if ( type != cmmooredmine )
         megaBlitter< ColorTransform_None, ColorMerger_AlphaOverwrite, SourcePixelSelector_Plain, TargetPixelSelector_All > 
                ( *images[type], surf, pos, nullParam,nullParam, nullParam,nullParam);
      else
         megaBlitter< ColorTransform_None, ColorMerger_AlphaMixer, SourcePixelSelector_Plain, TargetPixelSelector_All > 
                ( *images[type], surf, pos, nullParam,nullParam, nullParam,nullParam);
   }
}


bool AgeableItem::age( AgeableItem& obj )
{
   if ( obj.lifetimer > 0 ) {
      --obj.lifetimer;
      return obj.lifetimer==0;
   } else
      return false;
}


Object :: Object ( )
{
   typ = NULL;
   dir = 0;
   damage = 0;
   remainingGrowthTime = -1;
}

Object :: Object ( const ObjectType* o )
{
   lifetimer = o->lifetime;
   typ = o;
   dir = 0;
   damage = 0;
   remainingGrowthTime = o->growthDuration;
}


void Object :: setDir ( int direc )
{
   dir = direc;
}

int  Object :: getDir() const
{
   return dir;
}

void Object :: display ( Surface& surface, const SPoint& pos, int weather ) const
{
   typ->display ( surface, pos, dir, weather );
}

const OverviewMapImage* Object :: getOverviewMapImage( int weather )
{
   return typ->getOverviewMapImage( dir, weather );
}

const int objectVersion = 1;

void Object::write ( tnstream& stream )
{
   stream.writeInt( objectVersion );
   stream.writeInt( lifetimer );
   stream.writeInt( damage );
   stream.writeInt( dir );
   stream.writeInt( remainingGrowthTime );
}

void Object::read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version < 1 || version > objectVersion )
      throw tinvalidversion ( "Object", 1, version );
   
   lifetimer = stream.readInt();
   damage  = stream.readInt();
   dir = stream.readInt();
   remainingGrowthTime = stream.readInt();
}

