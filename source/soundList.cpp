/*! \file soundList.cpp
    \brief The collection of all sounds used by ASC
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2003  Martin Bickel  

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


#include <stdio.h>
#include <stdlib.h>

#ifndef _DOS_
 #include "global.h"
 #include sdlheader
#endif

#include "soundList.h"
#include "basestrm.h"
#include "stringtokenizer.h"

SoundList* SoundList::instance = NULL;




SoundList& SoundList::getInstance()
{
   if ( !instance )
      fatalError("SoundList::getInstance() - Soundlist not initialized");

   return *instance;
}


void SoundList::init( )
{
   if ( instance )
      fatalError("SoundList::init() - Soundlist already initialized");

   instance = new SoundList;
   instance->initialize ( );
}

void SoundList::readLine( PropertyContainer& pc, const ASCString& name, SoundList::Sample sample, int subtype, int fadeIn )
{
   vector<ASCString> labels;
   vector<ASCString> files;
   pc.addStringArray ( name + ".files", files );
   if ( pc.find ( name + ".labels" ))
      pc.addStringArray ( name + ".labels", labels );

   SoundAssignment s;
   if ( files.size() && !files[0].empty() )
      s.defaultSound = getSound( files[0], fadeIn );
   else
      s.defaultSound = NULL;

   s.sample = sample;
   s.subType = subtype;

   for( int i = 0; i < labels.size() && i < files.size() ; i++ )
      s.snd[ copytoLower(labels[i]) ] = getSound( files[i], fadeIn );

   soundAssignments.push_back ( s );
}

void SoundList::initialize(  )
{
   TextPropertyGroup* tpg = NULL;
   {
      tnfilestream s ( "sounds.asctxt", tnstream::reading );

      TextFormatParser tfp ( &s );
      tpg = tfp.run();
   }
   // auto_ptr<TextPropertyGroup> atpg ( tpg );

   PropertyReadingContainer pc ( "sounds", tpg );

   pc.openBracket("shoot");
    readLine( pc, "CRUISEMISSILE", SoundList::shooting, 0 );
    readLine( pc, "MINE", SoundList::shooting, 1 );
    readLine( pc, "BOMB", SoundList::shooting, 2 );
    readLine( pc, "AIRMISSILE", SoundList::shooting, 3 );
    readLine( pc, "GROUNDMISSILE", SoundList::shooting, 4 );
    readLine( pc, "TORPEDO", SoundList::shooting, 5 );
    readLine( pc, "MACHINEGUN", SoundList::shooting, 6 );
    readLine( pc, "CANNON", SoundList::shooting, 7 );
    readLine( pc, "LASER", SoundList::shooting, 10);
   pc.closeBracket();

   pc.openBracket("move");
    readLine( pc, "default", SoundList::moving, 0 );
    readLine( pc, "LIGHT_TRACKED_VEHICLE", SoundList::moving, 1 );
    readLine( pc, "MEDIUM_TRACKED_VEHICLE", SoundList::moving, 2 );
    readLine( pc, "HEAVY_TRACKED_VEHICLE", SoundList::moving, 3 );
    readLine( pc, "LIGHT_WHEELED_VEHICLE", SoundList::moving, 4 );
    readLine( pc, "MEDIUM_WHEELED_VEHICLE", SoundList::moving, 5 );
    readLine( pc, "HEAVY_WHEELED_VEHICLE", SoundList::moving, 6 );
    readLine( pc, "TROOPER", SoundList::moving, 7 );
    readLine( pc, "RAIL_VEHICLE", SoundList::moving, 8 );
    readLine( pc, "MEDIUM_AIRCRAFT", SoundList::moving, 9 );
    readLine( pc, "MEDIUM_SHIP", SoundList::moving, 10 );
    readLine( pc, "TURRET", SoundList::moving, 11 );
    readLine( pc, "LIGHT_AIRCRAFT", SoundList::moving, 12 );
    readLine( pc, "HEAVY_AIRCRAFT", SoundList::moving, 13 );
    readLine( pc, "LIGHT_SHIP", SoundList::moving, 14 );
    readLine( pc, "HEAVY_SHIP", SoundList::moving, 15 );
    readLine( pc, "HELICOPTER", SoundList::moving, 16 );
   pc.closeBracket();

   pc.openBracket("UserInterface");
    readLine( pc, "ACKNOWLEDGE", SoundList::menu_ack );
   pc.closeBracket();
   readLine( pc, "CONQUER_BUILDING", SoundList::conquer_building );
   readLine( pc, "UNIT_EXPLODES", SoundList::unitExplodes );
   readLine( pc, "BUILDING_COLLAPSES", SoundList::buildingCollapses );

   pc.run();

}

Sound* SoundList::getSound( const ASCString& filename, int fadeIn )
{
  if ( soundFiles.find ( filename ) == soundFiles.end() ) {
     Sound* s = new Sound ( filename, fadeIn );
     soundFiles[filename] = s;
     return s;
  } else
     return soundFiles[filename];
}


Sound* SoundList::getSound( Sample snd, int subType, const ASCString& label )
{
   if ( SoundSystem::getInstance()->isOff() )
      return NULL;

   if ( label.find ( "." ) != ASCString::npos ) {
      return getSound ( label, 0 );
   } else {
      
      ASCString newlabel = copytoLower(label);
      for ( vector<SoundAssignment>::iterator i = soundAssignments.begin(); i != soundAssignments.end(); i++ )
         if ( snd == i->sample && subType == i->subType )
            if ( newlabel.empty() || i->snd.find( newlabel ) == i->snd.end() )
               return i->defaultSound;
            else
               return i->snd[newlabel];
   }

   return NULL;
}

Sound* SoundList::playSound( Sample snd, int subType , bool looping, const ASCString& label  )
{
   Sound* sound = getSound ( snd, subType, label );
   if ( !sound )
      return NULL;

   if ( looping )
      sound->playLoop();
   else
      sound->play();

   return sound;
}

SoundList::~SoundList()
{
   for ( SoundFiles::iterator i = soundFiles.begin(); i != soundFiles.end(); i++ )
      delete i->second;
}

