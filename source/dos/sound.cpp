/** This file provides a simple way to play sounds.  If the last sound is still
 *  playing when the next sound is called for, the last sound is stopped.
 *  Parts of this code were shamelessly copied from the SDL Audio examples.
 *
 *  Implements the Sound class.  To load a wave file:
 *      Sound *s=new Sound( filename ).
 *  To play the sound : 
 *      s.play();
 *  To release the sound :
 *      delete s;
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sound.h"


void initSound(void) {
}

void closeSound(void) {
}

Sound::Sound( const char *filename ) {
}

void Sound::play(void) {
}

/** Play the sound, but don't return control to this thread until
 *  the sound has finished playing.
 */
void Sound::playWait(void) {
}

Sound::~Sound(void) {
}


