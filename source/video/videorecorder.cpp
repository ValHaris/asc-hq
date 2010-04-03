/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "videorecorder.h"

#ifdef XVIDEXPORT

#include "../libs/revel/revel.h"
#include "../events.h"
#include "../gameoptions.h"

class VideoRecorderInternals {
   public:
     VideoRecorderInternals() : open(false), frameCounterOut(0), frameCounterIn(0), lastFrame(NULL) {};
     ~VideoRecorderInternals() { if ( lastFrame ) delete[] lastFrame; };
     int encoderHandle; 
     Revel_Params revParams;
     bool open;
     int framerate;
     int ascFramerateLimit;
     long lastTick;
     ASCString filename;
     int frameCounterOut; //<! counts all frames that where send to the videorecorder
     int frameCounterIn;  //<! counts all frames that where passed to the video encoder
     int* lastFrame;
};


void checkErrors( const Revel_Error& err )
{
    if (err != REVEL_ERR_NONE)
    {
            printf("Revel Error : %d\n", err);
    }   
}


VideoRecorder::VideoRecorder( const ASCString& filename, const SDL_Surface* surf, int framerate, int ascFramerateLimit, int quality  )
{
    data = new VideoRecorderInternals();
    data->framerate = framerate;
    data->lastTick = getTicker();
    data->filename = filename;
    data->ascFramerateLimit = ascFramerateLimit;
   
    Revel_Error revError = Revel_CreateEncoder(&data->encoderHandle);
    checkErrors( revError );
   
    Revel_InitializeParams(&data->revParams);
    data->revParams.width = surf->w;
    data->revParams.height = surf->h;
    data->revParams.frameRate = data->framerate;
    float q = quality;
    q /= 100.0f;
    data->revParams.quality = q;
    data->revParams.codec = REVEL_CD_XVID;

    data->revParams.hasAudio = 0;

    // Initiate encoding
    revError = Revel_EncodeStart(data->encoderHandle, filename.c_str(), &data->revParams);
    data->open = true;
}

void VideoRecorder::storeFrame( const SDL_Surface* surf )
{
   if ( !surf )
      surf = SDL_GetVideoSurface();

   if ( !surf )
      return;

   Revel_VideoFrame frame;
   frame.width = surf->w;
   frame.height = surf->h;
   frame.bytesPerPixel = 4;
   
   bool directScreenRender = false;
   /*
   if ( surf->pitch == surf->w*4 && surf->format->BytesPerPixel == 4 ) {
      if ( surf->format->Rshift == 0 && surf->format->Gshift == 8 && surf->format->Bshift == 16 ) {
         frame.pixelFormat = REVEL_PF_RGBA;
         directScreenRender = true;
      }
      
      if ( surf->format->Rshift == 24 && surf->format->Gshift == 16 && surf->format->Bshift == 8 ) {
         frame.pixelFormat = REVEL_PF_ABGR;
         directScreenRender = true;
      }
   }
   */
   
   
   if ( directScreenRender ) {
      int frameSize;
      frame.pixels = surf->pixels;
      Revel_Error revError = Revel_EncodeFrame(data->encoderHandle, &frame, &frameSize);
      checkErrors( revError);
   } else {
      frame.pixelFormat = REVEL_PF_RGBA;
      int* buf;
      frame.pixels = buf = new int[frame.width*frame.height];   
      
      Uint32* pix = (Uint32*)frame.pixels;
      
      bool diff = false;
      int* lastBuf = data->lastFrame;
      if ( !data->lastFrame )
         diff = true;
      
      for ( int y = 0; y < surf->h; ++y ) {
         Uint32* src = ((Uint32*) surf->pixels) + (surf->pitch/4*y);
         for ( int x = 0 ; x < surf->w; ++x ) {
            Uint8 r,g,b,a;
            SDL_GetRGBA( *src, surf->format, &r,&g,&b,&a);
            *pix = r + (g<<8) + (b<<16) + (a<<24);
            if ( lastBuf ) {
               if ( *pix != *lastBuf )
                  diff = true;
               ++lastBuf;
            }
            ++pix;
            ++src;
         }
      }
      if ( diff || 1 ) {
         int frameSize;
         Revel_Error revError = Revel_EncodeFrame(data->encoderHandle, &frame, &frameSize);
         checkErrors( revError);
         ++data->frameCounterIn;
       }
      if ( data->lastFrame )
         delete[] data->lastFrame;
      data->lastFrame = buf;
   }
   
   ++data->frameCounterOut;
   
   // we are limiting the playback to the specified framerate
   while ( getTicker() < data->lastTick + 100/data->ascFramerateLimit )
      releasetimeslice();
   
   data->lastTick = getTicker();
}

void VideoRecorder::close()
{
   if ( data->open ) {
      int totalSize;
      Revel_Error revError = Revel_EncodeEnd(data->encoderHandle, &totalSize);
      checkErrors( revError );
      Revel_DestroyEncoder(data->encoderHandle);      
   }
   printf("recorded %d / %d frames\n", data->frameCounterOut, data->frameCounterIn );
}

const ASCString VideoRecorder::getFilename()
{
   return data->filename;  
}


VideoRecorder::~VideoRecorder()
{
   close();
   delete data;
}

#else

VideoRecorder::VideoRecorder( const ASCString& filename, const SDL_Surface* surf, int framerate, int ascFramerateLimit, int quality  ) {}
const ASCString VideoRecorder::getFilename() { return ""; }
void VideoRecorder::storeFrame( const SDL_Surface* surf ) {}
void VideoRecorder::close() {}
VideoRecorder::~VideoRecorder() {}

#endif
