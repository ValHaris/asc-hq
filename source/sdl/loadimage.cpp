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

#include <SDL/SDL_image.h>
#include "SDLStretch.h"
#include "../basestrm.h"


int loadFullscreenImage ( const char* filename )
{
   tnfilestream s ( filename, 1 );
   SDL_Surface* convimg = IMG_LoadJPG_RW_D( SDL_RWFromStream ( &s ), 8 );
   SDL_Surface* screen = SDL_GetVideoSurface();
                	
   if ( convimg->format->palette )
      SDL_SetColors(screen, convimg->format->palette->colors, 0, convimg->format->palette->ncolors);

   SDL_StretchSurface( convimg,0,0,convimg->w-1,convimg->h-1,
                       screen, 0,0,screen->w-1, screen->h-1);
    	
   SDL_UpdateRect(screen, 0,0,0,0);

   SDL_FreeSurface ( convimg );
}

void closeFullscreenImage ( void )
{
}