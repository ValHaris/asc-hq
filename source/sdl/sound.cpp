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

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
//#include <SDL/SDL_audio.h>
//#include <SDL/SDL_error.h>
#include <SDL/SDL.h>
#include "sound.h"

#include "../basestrm.h"

#define DEBUG( msg ) if ( verbosity > 8 ) fprintf( stderr, "DEBUG SOUND: %s\n", msg )
#define DEBUGS( msg ) if ( verbosity > 8 ) fprintf( stderr, "DEBUG SOUND %s: %s\n", name, msg )

/** How long should this process sleep while waiting for a sound to play
 */
#define WAIT_SLEEP_USEC 250000l // 0.25 seconds

/* The actual audio spec of the hardware we have */
static SDL_AudioSpec   actualAudioSpec;

/* The sound data currently being played.  The pointer advabces through the
 * buffer as each part of the sound is played.
 */
static Uint8          *audioData=NULL;

/* The number of bytes remaining to be played pointed to by audioData */
static Uint32          audioDataLen=0;

/* This flag exists to shortcircuit all sound logic when
 * the SDL sound module didn't initialise
 */
static bool            noAudio=false;

/* This flag exists to ensure that the sound module is
 * initialised once and only once before use.
 */
static bool            needInit=true;

/* Current sound is remembered soley to ensure that the
 * sound data isn't freed while it's playing
 */
static Sound          *currentSound=NULL;

/* The audio function callback takes the following parameters:
   stream:  A pointer to the audio buffer to be filled
   len:     The length (in bytes) of the audio buffer
*/
static void fill_audio(void *udata, Uint8 *stream, int len)
{
  /* Only play if we have data left.  Stop callbacks if we're out of data. */
  if ( audioDataLen == 0 ) {
    SDL_PauseAudio(1);
    currentSound=NULL;
    return;
  }

  /* Mix as much data as possible */
  len = ( len > audioDataLen ? audioDataLen : len );
  SDL_MixAudio( stream, audioData, len, SDL_MIX_MAXVOLUME );
  audioData += len;
  audioDataLen -= len;
}

void initSound(void) {
  if(!needInit)
    return;

  DEBUG("initSound Start");
  SDL_AudioSpec wanted;
  
  /* Set the audio format */
  wanted.freq = 22050;
  wanted.format = AUDIO_S16;
  wanted.channels = 2;    /* 1 = mono, 2 = stereo */
  wanted.samples = 1024;  /* Good low-latency value for callback */
  wanted.callback = fill_audio;
  wanted.userdata = NULL;
  
  /* Open the audio device, Adjusting to the hardware.
   * flag no audio available if we can't open audio
   */
  noAudio=SDL_OpenAudio(&wanted, &actualAudioSpec) < 0;
  needInit=false;
  DEBUG("initSound Stop");
}

void closeSound(void) {
  if(!needInit) {
    SDL_CloseAudio();
    needInit=true;
  }
}



static int stream_read(SDL_RWops *context, void *ptr, int size, int maxnum)
{
	pnfilestream stream = (pnfilestream) context->hidden.unknown.data1;
	size_t nread = stream->readdata ( ptr, size * maxnum, 0 );

	if ( nread < 0 ) {
		SDL_SetError("Error reading from datastream");
	}
	return(nread / size);
}

static int stream_close(SDL_RWops *context)
{
	if ( context ) {
		if ( context->hidden.unknown.data1 ) {
			pnfilestream stream = (pnfilestream) context->hidden.unknown.data1;
			delete stream;
		}
		free(context);
	}
	return(0);
}


SDL_RWops *SDL_RWFromStream( pnstream stream )
{
	SDL_RWops *rwops;

	rwops = SDL_AllocRW();
	if ( rwops != NULL ) {
	   rwops->seek = NULL;
	   rwops->read = stream_read;
	   rwops->write = NULL;
	   rwops->close = stream_close;
	   rwops->hidden.unknown.data1 = stream;
	}
	return(rwops);
}


SDL_AudioSpec* loadWave ( const char* name, SDL_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len)
{
   if ( !exist ( name ))
      return NULL;

   tnfilestream* stream = new tnfilestream ( name, 1 );
   
   return SDL_LoadWAV_RW( SDL_RWFromStream ( stream ), 1, spec, audio_buf, audio_len);
}


Sound::Sound( const char *filename ) {
  SDL_AudioSpec  sampleAudioSpec;
  Uint8         *tmpData;
  Uint32         tmpLen;

  name=strdup(filename);
  DEBUGS("initialising");

  /* Load wave file or set to silence on failure.  Also set for silence if
   * There is noAudio.
   */
  if( noAudio || !loadWave( filename, &sampleAudioSpec, &tmpData, &tmpLen ) ) {
    data=NULL;
    converted=0;
    len=0;
    DEBUGS( "set silent" );
    return;
  }

  /* Determine if sample must be converted for the sound hardware.
   * The converted flag is set so we use the correct free routine later.
   */
  converted=(sampleAudioSpec.format != actualAudioSpec.format)
    ||      (sampleAudioSpec.freq != actualAudioSpec.freq)
    ||      (sampleAudioSpec.channels != actualAudioSpec.channels);
  
  /* Attempt conversion if required */
  if( converted ) {
    DEBUGS( "Converting!" );
    SDL_AudioCVT converter;

    /* Setup the conversion and either convert or set silent if we can't
     * create an adequate converter
     */
    if( SDL_BuildAudioCVT( &converter,
			   sampleAudioSpec.format, sampleAudioSpec.channels,
			   sampleAudioSpec.freq,
			   actualAudioSpec.format, actualAudioSpec.channels,
			   actualAudioSpec.freq ) >= 0 ) {

      /* Perform conversion */
      converter.len=tmpLen;
      converter.buf=(Uint8 *)malloc( converter.len * converter.len_mult);

      /* If we successfully allocated RAM, do conversion, otherwise
       * set sound to silence
       */
      if( converter.buf ) {
	memcpy( converter.buf, tmpData, tmpLen );
	SDL_ConvertAudio( &converter );
      } else {
	converter.len=0;
      }

      DEBUGS( "Converted ok" );
      data=converter.buf;
      len=converter.len*converter.len_mult;

    } else {

      /* Set sound to silence if we can't convert */
      data=NULL;
      len=0;
    }

    /* Free original sample */
    DEBUGS( "Freeing original sound memory" );
    SDL_FreeWAV( tmpData );

  } else {

    /* Just use it if no conversion required */
    data=tmpData;
    len=tmpLen;
  }
  DEBUGS("initialised");
}

void Sound::play(void) {
  DEBUGS( "play" );

  if( noAudio )
    return;

  /* Set the sound buffer to play the current sound */
  SDL_LockAudio();
  audioData=data;
  audioDataLen=len;
  currentSound=this;
  SDL_UnlockAudio();

  /* Ensure the playback loop is running */
  SDL_PauseAudio(0);
}

/** Play the sound, but don't return control to this thread until
 *  the sound has finished playing.
 */
void Sound::playWait(void) {
  play();

  // This is not a very efficent way to wait for the sound to end,
  // but it's a lot simpler than setting up a semaphore.
  do {
    SDL_Delay(WAIT_SLEEP_USEC);
  } while( currentSound==this );
}

Sound::~Sound(void) {

  /* If this sound is playing, stop it before we free the buffer
   */
  SDL_LockAudio();
  if( currentSound==this ) {
    currentSound=NULL;
    audioData=NULL;
    audioDataLen=0;
  }
  SDL_UnlockAudio();

  /* Free the audio data */
  if( data ) {
    if( converted )
      free(data);
    else
      SDL_FreeWAV( data );
  }
}


