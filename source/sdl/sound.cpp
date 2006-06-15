/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cstring>
#include <stdlib.h>


#include <SDL.h>
#include <SDL_mixer.h>

#include "../global.h"

#include "sound.h"

#include "../basestrm.h"
#include "../sgstream.h"
#include "../music.h"
#include "../sgstream.h"

/** How long should this process sleep while waiting for a sound to play
 */
const int WAIT_SLEEP_MSEC = 50;




SoundSystem* SoundSystem::instance = NULL;

SoundSystem  :: SoundSystem ( bool muteEffects, bool muteMusic, bool _off )
   : sdl_initialized(false), mix_initialized( false )
{
   musicState = uninitialized;
   currentPlaylist = NULL;
   musicBuf = NULL;


   this->effectsMuted = muteEffects;
   this->off = _off;

   for ( int i = 0; i < MIX_CHANNELS; i++ )
       channel[i] = NULL;


   if ( instance )
      fatalError ( "Only one instance of SoundSystem possible !");

   instance = this;

   if( off )
      return;

   if ( SDL_Init ( SDL_INIT_AUDIO ) < 0 ) {
      warning("Couldn't initialize SDL audio interface !");
      off = true;
      sdl_initialized = false;
      return;
   } else
      sdl_initialized = true;

   int audio_rate = MIX_DEFAULT_FREQUENCY;
   Uint16 audio_format = MIX_DEFAULT_FORMAT;
   int audio_channels = 2;

   if ( Mix_OpenAudio ( audio_rate, audio_format, audio_channels, 2048 ) < 0 ) {
      mix_initialized = false;
      warning("Couldn't initialize SDL_mixer !");
      off = true;
      return;
   } else {
      mix_initialized = true;
      if ( muteMusic )
         musicState = init_paused;
      else
         musicState = init_ready;

      Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
      displayLogMessage ( 5, "Opened audio at %d Hz %d bit %s\n", audio_rate,
			(audio_format&0xFF),
			(audio_channels > 1) ? "stereo" : "mono");
      Mix_HookMusicFinished ( trackFinished );

      Mix_ChannelFinished( channelFinishedCallback );

   }
}


void SoundSystem::setEffectsMute ( bool mute )
{
   if ( off )
      return;

   if ( mute != this->effectsMuted ) {
      if ( mute ) {
         for ( int i = 0; i < MIX_CHANNELS; i++ )
            if ( Mix_Playing(i)  )
                Mix_HaltChannel( i );

      }
      this->effectsMuted = mute;
   }
}


void SoundSystem :: trackFinished( void )
{
   getInstance()->nextTrack();
}

void SoundSystem :: nextTrack( void )
{
   if ( off || musicState==paused || musicState==init_paused)
      return;

  if ( musicBuf ) {
     Mix_FreeMusic( musicBuf );
     musicBuf = NULL;
  }

  if ( currentPlaylist ) {
     ASCString filename = currentPlaylist->getNextTrack();
     if ( !filename.empty() ) {
        musicState = playing;
        musicBuf = Mix_LoadMUS( filename.c_str() );

        if ( !musicBuf ) {
           displayLogMessage ( 1, "Could not load music file " + filename + " ; SDL reports error " + SDL_GetError() + "\n" );
           SDL_ClearError();
        } else {
           int chan = Mix_PlayMusic ( musicBuf, 1 );
           displayLogMessage ( 4, "Playing music on channel %d \n", chan );
        }
     }
  }
}

void SoundSystem :: playMusic ( MusicPlayList* playlist )
{
  currentPlaylist = playlist;

  nextTrack();
}


void SoundSystem :: pauseMusic()
{
   if ( off )
      return;

   if ( musicState == playing ) {
      Mix_PauseMusic ();
      musicState = paused;
   }
}

void SoundSystem :: resumeMusic()
{
   if ( off )
      return;

   if ( musicState == init_ready || musicState == init_paused ) {
      if (musicState == init_paused)
         musicState = init_ready;
      nextTrack();
      return;
   }

   if ( musicState == paused ) {
      Mix_ResumeMusic ();
      musicState = playing;
   }
}

void SoundSystem :: resumePauseMusic()
{
   if ( musicState == playing )
      pauseMusic();
   else
      resumeMusic();
}

void SoundSystem :: setMusicVolume( int volume )
{
   musicVolume = volume * 128 / 100;

   if ( off )
      return;

   Mix_VolumeMusic ( musicVolume );
}

void SoundSystem :: setEffectVolume( int volume )
{
   effectVolume = volume * 128 / 100;

   if ( off )
      return;

   Mix_Volume ( -1, effectVolume );
}


SoundSystem::~SoundSystem()
{
   if ( !off ) {
      Mix_HaltMusic();

      if ( musicBuf ) {
         Mix_FreeMusic( musicBuf );
         musicBuf = NULL;
      }
   }

   if( mix_initialized )
      Mix_CloseAudio();

   if( sdl_initialized )
      SDL_CloseAudio();


   instance = NULL;
}



Mix_Chunk* SoundSystem::loadWave ( const ASCString& name )
{
   if ( off )
      return NULL;

   if ( !exist ( name.c_str() )) {
      errorMessage ( " can't open sound file: " + name );
      return NULL;
   }

   tnfilestream stream ( name, tnstream::reading );

   Mix_Chunk* chunk = Mix_LoadWAV_RW( SDL_RWFromStream ( &stream ), 1);
   if ( chunk )
      displayLogMessage ( 10, " SoundSystem::loadWave - sound " + name + " loaded successfully\n");
   else
      displayLogMessage ( 10, " SoundSystem::loadWave - sound " + name + " loaded failed\n");

   return chunk;
}

void SoundSystem::channelFinishedCallback( int channelnum )
{
   if ( getInstance()->channel[channelnum] ) 
      getInstance()->channel[channelnum]->finishedSignal( channelnum );
}




Sound::Sound( const ASCString& filename, int _fadeIn ) : name ( filename ), mainwave(NULL), startwave(NULL), fadeIn ( _fadeIn ), waitingForMainWave(false)
{
   if ( !SoundSystem::instance )
      fatalError ( "Sound::Sound failed, because there is no SoundSystem initialized");

   mainwave = SoundSystem::instance->loadWave( filename );
}

Sound::Sound( const ASCString& startSoundFilename, const ASCString& continuousSoundFilename, int _fadeIn ) : name ( startSoundFilename ), mainwave(NULL), startwave(NULL), fadeIn ( _fadeIn ), waitingForMainWave(false) 
{
   if ( !SoundSystem::instance )
      fatalError ( "Sound::Sound failed, because there is no SoundSystem initialized");

   startwave = SoundSystem::instance->loadWave( startSoundFilename );
   mainwave = SoundSystem::instance->loadWave( continuousSoundFilename );
}


int Sound::startPlaying( bool loop )
{
   int channel;

   int loopcontrol;
   if ( loop ) 
      loopcontrol = -1;
   else
      loopcontrol = 0;

   Mix_Chunk* wave  = startwave;
   if ( !wave ) 
      wave = mainwave;
   else
      loopcontrol = 0;

   if ( fadeIn )
      channel = Mix_FadeInChannel ( -1, wave, loopcontrol, fadeIn );
   else {
      channel = Mix_PlayChannel ( -1, wave, loopcontrol );
      Mix_Volume ( channel, SoundSystem::instance->getEffectVolume() );
   }
   if ( startwave ) {
      waitingForMainWave = true;
   }

   return channel;
}

void Sound::finishedSignal( int channelnum )
{
   if ( waitingForMainWave ) {
      Mix_PlayChannel ( channelnum, mainwave, -1 );
      Mix_Volume ( channelnum, SoundSystem::instance->getEffectVolume() );
      waitingForMainWave = false;
   }
}

void Sound::play(void)
{
   if( SoundSystem::instance->areEffectsMuted() || !mainwave)
      return;

   int channel = startPlaying( false );
   SoundSystem::instance->channel[ channel ] = this;
}

void Sound::playLoop()
{
   if( SoundSystem::instance->areEffectsMuted() || !mainwave)
      return;

   int channel = startPlaying( true );
   SoundSystem::instance->channel[ channel ] = this;
}

void Sound::stop()
{
   for ( int i = 0; i < MIX_CHANNELS; i++ )
      if ( SoundSystem::instance->channel[ i ] == this  && Mix_Playing(i)  )
          Mix_HaltChannel( i );
}


void Sound::playWait(void)
{
   if( SoundSystem::instance->areEffectsMuted() || !mainwave)
      return;

   if ( startwave ) {
      int channel = Mix_PlayChannel ( -1, startwave, 0 );
      SoundSystem::instance->channel[ channel ] = this;
      do {
         SDL_Delay(WAIT_SLEEP_MSEC);
      } while( SoundSystem::instance->channel[ channel ] == this  && Mix_Playing(channel)  );
   }

   int channel = Mix_PlayChannel ( -1, mainwave, 0 );
   SoundSystem::instance->channel[ channel ] = this;

   do {
      SDL_Delay(WAIT_SLEEP_MSEC);
   } while( SoundSystem::instance->channel[ channel ] == this  && Mix_Playing(channel)  );
}

void Sound :: fadeOut ( int ms )
{
   for ( int i = 0; i < MIX_CHANNELS; i++ )
      if ( SoundSystem::instance->channel[ i ] == this  && Mix_Playing(i)  )
          Mix_FadeOutChannel( i, ms );
}



Sound::~Sound(void)
{
   stop();

   if ( mainwave )
      Mix_FreeChunk ( mainwave );

   if ( startwave )
      Mix_FreeChunk ( startwave );
}


