/***************************************************************************
                          loadimage.cpp  -  description
                             -------------------
    begin                : Sat Aug 5 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// #include "SDL_video.h"
#include "SDLStretch.h"
#include "../basestrm.h"
#include "../loadimage.h"
#include "../dlg_box.h"
#include "../palette.h"
#include "../loadpcx.h"

extern "C" SDL_Surface *IMG_LoadJPG_RW_D(SDL_RWops *src, int depth);


void read_JPEG_file ( pnstream stream )
{
      SDL_RWops *src = SDL_RWFromStream ( stream );
      SDL_Surface* convimg = IMG_LoadJPG_RW_D( src, 8 );
      SDL_RWclose(src);

      SDL_Surface* screen = SDL_GetVideoSurface();
                        
      if ( convimg->format->palette )
         SDL_SetColors(screen, convimg->format->palette->colors, 0, convimg->format->palette->ncolors);

      SDL_StretchSurface( convimg,0,0,convimg->w-1,convimg->h-1,
                          screen, 0,0,screen->w-1, screen->h-1);
        
      SDL_UpdateRect(screen, 0,0,0,0);

      SDL_FreeSurface ( convimg );
}

int loadFullscreenImage ( const char* filename )
{
   try {
      if ( exist ( filename )) {
         tnfilestream stream ( filename, 1 );
         if ( strstr ( filename, ".PCX" ) || strstr ( filename, ".pcx" )) {
            loadpcxxy ( &stream, 0, 0 );
         } else {
            read_JPEG_file ( &stream );
         }
         return 1;
      } else {
         string s = filename;
         while ( s.find ( ".PCX" ) != string::npos )
            s.replace ( s.find ( ".PCX" ), 4, ".JPG");

         while ( s.find ( ".pcx" ) != string::npos )
            s.replace ( s.find ( ".pcx" ), 4, ".jpg");

         if ( exist ( s.c_str() )) {
            tnfilestream stream ( s.c_str(), 1 );
            read_JPEG_file ( &stream );
            return 1;
         } else
            return 0;
      }
   }
   catch ( tfileerror err) {
      displaymessage ( "could not load image %s", 1, err.filename );
      return 0;
   }
}

void closeFullscreenImage ( void )
{
   dacpalette256 blck;

   for ( int i = 0; i< 256; i++ ) {
      blck[i][0] = 0;
      blck[i][1] = 0;
      blck[i][2] = 0;
   }

   setvgapalette256(blck);
   SDL_FillRect ( SDL_GetVideoSurface(), NULL, 0 );
   setvgapalette256(pal);
}


void setFullscreenSetting ( FullscreenImageSettings setting, int value )
{}

