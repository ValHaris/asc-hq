/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <boost/regex.hpp>

#include <cstring>
#include <stdlib.h>


#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_sound.h>


#include "../global.h"

#include "sound.h"

#include "../basestrm.h"
#include "../sgstream.h"
#include "../music.h"
#include "../sgstream.h"

/** How long should this process sleep while waiting for a sound to play
 */
const int WAIT_SLEEP_MSEC = 50;



class SoundSystem_InternalData {
   public:
      Mix_Music *musicBuf;
      MusicPlayList* currentPlaylist;

      Sound* channel[MIX_CHANNELS];

      ASCString lastMusicMessage;

      SoundSystem_InternalData() : musicBuf(NULL), currentPlaylist(NULL) {
         for ( int i = 0; i < MIX_CHANNELS; ++i )
            channel[i] = NULL;

      };
};

SoundSystem* SoundSystem::instance = NULL;

SoundSystem  :: SoundSystem ( bool muteEffects, bool muteMusic, bool _off )
   : sdl_initialized(false), mix_initialized( false )
{
   internalData = new SoundSystem_InternalData;
   
   musicState = uninitialized;

   this->effectsMuted = muteEffects;
   this->off = _off;

   if ( instance )
      fatalError ( "Only one instance of SoundSystem possible !");

   instance = this;

   if( off )
      return;

   displayLogMessage(0,"Initializing sound device. If this hangs, run ASC without sound (asc -q)\n");
   displayLogMessage(0,"Step 1/3 (SDL_Init)...");

   if ( SDL_Init ( SDL_INIT_AUDIO ) < 0 ) {
      warning("Couldn't initialize SDL audio interface !");
      off = true;
      sdl_initialized = false;
      return;
   }

   displayLogMessage(0,"ok\nStep 2/3 (SDL_Sound Sound_Init)...");
   Sound_Init();
   
   sdl_initialized = true;

   int audio_rate = MIX_DEFAULT_FREQUENCY;
   Uint16 audio_format = MIX_DEFAULT_FORMAT;
   int audio_channels = 2;

   displayLogMessage(0,"ok\nStep 3/3 (SDL_Mixer Mix_OpenAudio)...");
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
      displayLogMessage ( 5, "Opened audio at %d Hz %d bit %s\n", audio_rate, (audio_format&0xFF), (audio_channels > 1) ? "stereo" : "mono");
      Mix_HookMusicFinished ( trackFinished );

      Mix_ChannelFinished( channelFinishedCallback );

   }
   displayLogMessage(0,"ok\nSound system successfully initialized!\n");
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

ASCString SoundSystem :: getDiagnosticText()
{

   static ASCString boolean[2] = {"false", "true"};

   ASCString text = "Sound System Status: "; 

   if( off )
      text += "off\n";
   else
      text += "on\n";

   text += "Effects Muted: " + boolean[effectsMuted] + "\n";
   text += "SDL Initialized: " + boolean[sdl_initialized] + "\n";
   text += "Mixer Initialized: " + boolean[mix_initialized] + "\n";
   text += "Music Volume: " + ASCString::toString( musicVolume ) + "\n";
   text += "Effects Volume: " + ASCString::toString( effectVolume ) + "\n";

   text += "\n";
   text += "Last message from mixer was:\n";
   text += internalData->lastMusicMessage + "\n\n";

   if ( internalData->currentPlaylist ) 
      text += internalData->currentPlaylist->getDiagnosticText();
   else
      text += "No play list active!";

   return text;
}


void SoundSystem :: nextTrack( void )
{
   if ( off || musicState==paused || musicState==init_paused)
      return;

   if ( internalData->musicBuf ) {
      Mix_FreeMusic( internalData->musicBuf );
      internalData->musicBuf = NULL;
   }

   if ( internalData->currentPlaylist ) {
      ASCString filename = internalData->currentPlaylist->getNextTrack();
      
      if ( !filename.empty() ) {
        musicState = playing;
        internalData->musicBuf = Mix_LoadMUS( filename.c_str() );

        if ( !internalData->musicBuf ) {
           internalData->lastMusicMessage = "Could not load music file " + filename + " ; SDL reports error " + SDL_GetError();
           displayLogMessage ( 1, internalData->lastMusicMessage + "\n" );
           SDL_ClearError();
        } else {
           int chan = Mix_PlayMusic ( internalData->musicBuf, 1 );
           Mix_VolumeMusic ( musicVolume );
           displayLogMessage ( 4, "Playing music on channel %d \n", chan );
        }
     }
  } else
     displayLogMessage ( 1, "No play list available \n" );
}

void SoundSystem :: playMusic ( MusicPlayList* playlist )
{
   internalData->currentPlaylist = playlist;
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

      if ( internalData->musicBuf ) {
         Mix_FreeMusic( internalData->musicBuf );
         internalData-> musicBuf = NULL;
      }
   }

   if( mix_initialized )
      Mix_CloseAudio();

   if( sdl_initialized )
      SDL_CloseAudio();

   delete internalData;

   instance = NULL;
}



void SoundSystem::channelFinishedCallback( int channelnum )
{
   if ( getInstance()->internalData->channel[channelnum] )
      getInstance()->internalData->channel[channelnum]->finishedSignal( channelnum );
}



pair<Sound_Sample*, Mix_Chunk*> loadWave ( const ASCString& name )
{
   Mix_Chunk* chunk  = NULL;
   Sound_Sample* sample  = NULL;
   
   if ( !exist ( name )) {
      errorMessage ( " can't open sound file: " + name );
      return make_pair(sample,chunk);
   }

   tnfilestream stream ( name, tnstream::reading );

   ASCString ext;
   boost::smatch what;
   static boost::regex extension( ".*\\.([^\\.]+)");
   if( boost::regex_match( name, what, extension))
      ext.assign( what[1].first, what[1].second );


   int frequency;
   Uint16 format;
   int channels;
   Mix_QuerySpec(&frequency, &format, &channels);
   Sound_AudioInfo ai;
   ai.format = format;
   ai.channels = channels;
   ai.rate = frequency;
   
   try {
      sample = Sound_NewSample ( SDL_RWFromStream ( &stream ), ext.c_str(), &ai, 1<<16);
      if ( sample )
         displayLogMessage ( 10, " SoundSystem::loadWave - sound " + name + " loaded successfully\n");
      else {
         displayLogMessage ( 10, " SoundSystem::loadWave - sound " + name + " loaded failed\n");
         return make_pair(sample,chunk);
      }
   }
   catch ( tfileerror err ) {
      warning(" Error loading sound file " + name );
      sample = NULL;
      return make_pair(sample,chunk);
   }

   Uint32 size = Sound_DecodeAll ( sample );

   chunk = new Mix_Chunk;
   chunk->allocated = size;
   chunk->abuf = (Uint8*) sample->buffer;
   chunk->alen = size;
   chunk->volume = MIX_MAX_VOLUME;
   
   return make_pair(sample,chunk);
}


class Sound_InternalData {
   public:
     //! the actual wave data
      Mix_Chunk *mainwave;
      Mix_Chunk *startwave;
      Sound_Sample *mainsample;
      Sound_Sample *startsample;
      Sound_InternalData() : mainwave(NULL), startwave(NULL), mainsample(NULL), startsample(NULL) {};
};

Sound::Sound( const ASCString& filename, int _fadeIn ) : name ( filename ), fadeIn ( _fadeIn ), waitingForMainWave(false)
{
   internalData = new Sound_InternalData;
   
   if ( !SoundSystem::instance )
      fatalError ( "Sound::Sound failed, because there is no SoundSystem initialized");

   pair<Sound_Sample*, Mix_Chunk*> res = loadWave( filename );
   internalData->mainsample = res.first;
   internalData->mainwave = res.second;
}

Sound::Sound( const ASCString& startSoundFilename, const ASCString& continuousSoundFilename, int _fadeIn ) : name ( startSoundFilename ), fadeIn ( _fadeIn ), waitingForMainWave(false)
{
   internalData = new Sound_InternalData;
   
   if ( !SoundSystem::instance )
      fatalError ( "Sound::Sound failed, because there is no SoundSystem initialized");

   pair<Sound_Sample*, Mix_Chunk*> res = loadWave( startSoundFilename );
   internalData->startsample = res.first;
   internalData->startwave = res.second;
   
   res = loadWave( continuousSoundFilename );
   internalData->mainsample = res.first;
   internalData->mainwave = res.second;
}




int Sound::startPlaying( bool loop )
{
   int channel;

   int loopcontrol;
   if ( loop ) 
      loopcontrol = -1;
   else
      loopcontrol = 0;

   Mix_Chunk* wave  = internalData->startwave;
   if ( !wave ) 
      wave = internalData->mainwave;
   else
      loopcontrol = 0;

   if ( fadeIn )
      channel = Mix_FadeInChannel ( -1, wave, loopcontrol, fadeIn );
   else {
      channel = Mix_PlayChannel ( -1, wave, loopcontrol );
      Mix_Volume ( channel, SoundSystem::instance->getEffectVolume() );
   }
   if ( internalData->startwave ) {
      waitingForMainWave = true;
   }

   return channel;
}

void Sound::finishedSignal( int channelnum )
{
   if ( waitingForMainWave ) {
      Mix_PlayChannel ( channelnum, internalData->mainwave, -1 );
      Mix_Volume ( channelnum, SoundSystem::instance->getEffectVolume() );
      waitingForMainWave = false;
   }
}

void Sound::play(void)
{
   if( SoundSystem::instance->areEffectsMuted() || !internalData->mainwave)
      return;

   int channel = startPlaying( false );
   SoundSystem::instance->internalData->channel[ channel ] = this;
}

void Sound::playLoop()
{
   if( SoundSystem::instance->areEffectsMuted() || !internalData->mainwave)
      return;

   int channel = startPlaying( true );
   SoundSystem::instance->internalData->channel[ channel ] = this;
}

void Sound::stop()
{
   for ( int i = 0; i < MIX_CHANNELS; i++ )
      if ( SoundSystem::instance->internalData->channel[ i ] == this  && Mix_Playing(i)  )
          Mix_HaltChannel( i );
}


void Sound::playWait(void)
{
   if( SoundSystem::instance->areEffectsMuted() || !internalData->mainwave)
      return;

   if ( internalData->startwave ) {
      int channel = Mix_PlayChannel ( -1, internalData->startwave, 0 );
      SoundSystem::instance->internalData->channel[ channel ] = this;
      do {
         SDL_Delay(WAIT_SLEEP_MSEC);
      } while( SoundSystem::instance->internalData->channel[ channel ] == this  && Mix_Playing(channel)  );
   }

   int channel = Mix_PlayChannel ( -1, internalData->mainwave, 0 );
   SoundSystem::instance->internalData->channel[ channel ] = this;

   do {
      SDL_Delay(WAIT_SLEEP_MSEC);
   } while( SoundSystem::instance->internalData->channel[ channel ] == this  && Mix_Playing(channel)  );
}

void Sound :: fadeOut ( int ms )
{
   for ( int i = 0; i < MIX_CHANNELS; i++ )
      if ( SoundSystem::instance->internalData->channel[ i ] == this  && Mix_Playing(i)  )
          Mix_FadeOutChannel( i, ms );
}



Sound::~Sound(void)
{
   stop();

   if ( internalData->mainwave )
      delete internalData->mainwave ;

   if ( internalData->startwave )
      delete internalData->startwave;

   if ( internalData->mainsample )
      Sound_FreeSample ( internalData->mainsample );

   if ( internalData->startsample )
      Sound_FreeSample ( internalData->startsample );

   delete internalData;
}


