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

#include "../ascstring.h"
#include "../music.h"


class Sound_InternalData;

class Sound {
   public:
     /** Create a Sound from the .wav file specified by filename.
      *  If it's not possible to use the wave file for some reason, the
      *  sound is set to silence.
      *  \param filename the file that is loaded
      *  \param fadeIn is a time in milliseconds
      */
     Sound( const ASCString& filename, int fadeIn = 0 );
     Sound( const ASCString& startSoundFilename, const ASCString& continuousSoundFilename, int fadeIn = 0 );


     void play(void);
     void playWait(void);

     void playLoop();
     void stop();

     void fadeOut ( int ms );

     bool load();

     friend class SoundSystem;

     ~Sound(void);
   private:
     /** A name for this sound - mostly for debugging purposes */
     const ASCString name;

     //! returns the channel
     int startPlaying( bool loop ); 

     void finishedSignal( int channelnum );

     Sound_InternalData* internalData;
     
     int fadeIn;
     bool waitingForMainWave;
};


class SoundSystem_InternalData;

class SoundSystem {
      bool effectsMuted;
      bool off;
      bool sdl_initialized;
      bool mix_initialized;
      int musicVolume;
      int effectVolume;

      static SoundSystem* instance;

      SoundSystem_InternalData* internalData;
      

      //! callback for SDL_mixer
      static void trackFinished( void );

      void nextTrack ( void );

      static void channelFinishedCallback( int channelnum );

      enum MusicState { uninitialized, init_ready, init_paused, playing, paused } musicState;
   protected:

      //! loads a sound from the wave file called name to an Mix_buffer.
      friend class Sound;

   public:
      /** Sets up ASC's sound system.
         \param muteEffects The sound is going to be initialized, but no sounds played. Sounds can be enabled at runtime
         \param muteMusic The sound is going to be initialized, but no music played. Music can be enabled at runtime
         \param off  The sound system is not even going to be initiliazed. Can only be restartet by restarting ASC
      */
      SoundSystem ( bool muteEffects, bool muteMusic, bool off );

      //! Turns the sound on and off
      void setEffectsMute ( bool mute );

      //! can sounds be played right now ?
      bool areEffectsMuted ( ) { return effectsMuted || off; };

      //! is the soundsystem completely disabled ?
      bool isOff ( ) { return off; };

      //! plays the pieces of music which are referenced in the playlist
      void playMusic ( MusicPlayList* playlist );

      //! Pauses the music that is currently being played
      void pauseMusic();

      //! resumes the music
      void resumeMusic();

      //! resumes or resumes the music, depending whether is music is paused or playing
      void resumePauseMusic();

      //! Sets the music volume. Range is 0 .. 100
      void setMusicVolume( int Volume );

      //! Sets the sound effect volume. Range is 0 .. 100
      void setEffectVolume( int Volume );

      //! Returns the sound effect volume. This is already normalized to the 0 .. 128 range of SDL_Mixer !
      int getEffectVolume( ) { return effectVolume; };

      static SoundSystem* getInstance() { return instance; };

      ~SoundSystem();
};

#endif
