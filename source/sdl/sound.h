/** This file provides the header for the Sound class */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef sound_h_included
#define sound_h_included

#include sdlmixerheader
#include "../ascstring.h"


class Sound {
public:
  /** Create a Sound from the .wav file specified by filename.
   *  If it's not possible to use the wave file for some reason, the
   *  sound is set to silence.
   *  \param fadeIn is a time in milliseconds
   */
  Sound( const ASCString& filename, int fadeIn = 0 );

  void play(void);
  void playWait(void);

  void playLoop();
  void stop();

  void fadeOut ( int ms );

  ~Sound(void);
private:
  /** A name for this sound - mostly for debugging purposes */
  const ASCString name;

  //! the actual wave data
  Mix_Chunk *wave;

  int fadeIn;

};


class SoundSystem {
      bool mute;
      int off;
      bool sdl_initialized;
      bool mix_initialized;

      static SoundSystem* instance;

      Sound* channel[MIX_CHANNELS];

   protected:

      //! loads a sound from the wave file called name to an Mix_buffer.
      Mix_Chunk* SoundSystem::loadWave ( const ASCString& name );
      friend class Sound;

   public:
      /** Sets up ASC's sound system.
         \param mute The sound is going to be initialized, but no sounds played. Sounds can be enabled at runtime
         \param off  The sound system is not even going to be initiliazed. Can only be restartet by restarting ASC
      */
      SoundSystem ( bool mute, bool off );

      //! Turns the sound on and off
      void setMute ( bool mute );

      //! can sounds be played right now ?
      bool isMuted ( ) { return mute || off; };

      //! is the soundsystem completely disabled ?
      bool isOff ( ) { return off; };

      static SoundSystem* getInstance() { return instance; };

      ~SoundSystem();
};

#endif
