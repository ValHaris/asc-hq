/*! \file soundList.cpp
    \brief The collection of all sounds used by ASC
*/

#include <stdio.h>
#include <stdlib.h>

#ifndef _DOS_
 #include "global.h"
 #include sdlheader
#endif

#include "soundList.h"
tSoundList sound;

#ifdef _DOS_
 #define DEBUG( msg ) 
#else
 #define DEBUG( msg ) printf( "%s\n", msg )
#endif

void initSoundList(int silent) {
  initSound(silent);
  sound.init(WEAPON_SOUND_COUNT);
}

void tSoundList::init( unsigned weaponCount ) {
  char weaponSoundFileName[128];
  soundFail = new Sound("Fail.wav");
  boom = new Sound("Boom.wav");
  menuPopup = new Sound("Click.wav");
  menuSelect = new Sound("Doink.wav");
  menuAbort = new Sound("Bweep.wav");
  weaponSoundCount=weaponCount;
  weaponSoundArray=(Sound **)malloc(sizeof(*weaponSoundArray)*weaponCount);
  for( unsigned u=0; u<weaponCount; ++u ) {
    sprintf( weaponSoundFileName, WEAPON_SOUND_FILENAME_TEMPLATE, u );
    weaponSoundArray[u]=new Sound(weaponSoundFileName);
  }
}

/** This function exists primarily to safeguard against failure to supply
 *  enough weapon sounds.  It's guaranteed to return a valid sound (possibly
 *  silence.
 */
Sound *tSoundList::weaponSound(unsigned weapon) {
  // printf( "Fetching sound for weapon %u\n", weapon );
  if( weaponSoundCount < weapon )
    return soundFail;

  return weaponSoundArray[weapon];
}
