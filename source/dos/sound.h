/** This file provides the header for the Sound class */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
class Sound {
public:
  /** Create a Sound from the .wav file specified by filename.
   *  If it's not possible to use the wave file for some reason, the
   *  sound is set to silence
   */
  Sound( const char *filename );

  void play(void);
  void playWait(void);

  ~Sound(void);
};

/** Initialise sound system.  If you don't call this, the first Sound
 *  created will call it for you.
 */
void initSound(int silent);

/** Clean up after the sound system */
void closeSound();

void enableSound();
void disableSound();