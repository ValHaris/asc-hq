/** This file provides the header for the Sound class */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "../ascstring.h"

class Sound {
public:
  /** Create a Sound from the .wav file specified by filename.
   *  If it's not possible to use the wave file for some reason, the
   *  sound is set to silence
   */
  Sound( const ASCString& filename );

  void play(void);
  void playWait(void);

  void playLoop();
  void stopLoop();

  ~Sound(void);
private:
  /* used to indicate that we converted the sound so that ~Sound uses the
   * correct free routine.
   */
  int converted;

  /** A name for this sound - mostly for debugging purposes */
  const ASCString name;

  /* actual sound data and its length */
  Uint8  *data;
  Uint32  len;
};

/** Initialise sound system.  If you don't call this, the first Sound
 *  created will call it for you.
 */
void initSound(int silent);

/** Clean up after the sound system */
void closeSound();


void disableSound ( void );
void enableSound ( void );
