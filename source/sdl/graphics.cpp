/***************************************************************************
                         graphics.cpp  -  description
                             -------------------
    begin                : Sat Dec 18 1999
    copyright            : (C) 1999 by Martin Bickel
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

//     $Id: graphics.cpp,v 1.1 1999-12-28 21:03:31 mbickel Exp $
//
//     $Log: not supported by cvs2svn $


#include <stdlib.h>
#include "graphics.h"
#include "../basegfx.h"
#include "SDL.h"



SDL_Surface *screen = NULL;


int reinitgraphics(int modenum)
{
  return 1;
}

int initgraphics ( int x, int y, int depth )
{
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
     fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
     return -1;
  }

  /* Clean up on exit */
  atexit(SDL_Quit);

  /* Initialize the display in a 640x480 8-bit palettized mode */
  screen = SDL_SetVideoMode(x, y, depth, SDL_SWSURFACE);
  if ( screen == NULL ) {
     fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n",x,y,depth, SDL_GetError());
     return -1;
  }

  agmp->resolutionx = x;
  agmp->resolutiony = y;
  agmp->windowstatus = 100;
  agmp->scanlinelength = screen->w;
  agmp->scanlinenumber = screen->h;
  agmp->bytesperscanline = x * depth/8;
  agmp->byteperpix = screen->format->BytesPerPixel ;
  agmp->linearaddress = (int) screen->pixels;
/*
            char          redmasksize       ;
            char          redfieldposition  ;
            char          greenmasksize     ;
            char          greenfieldposition;
            char          bluemasksize      ;
            char          bluefieldposition ;
*/
  agmp->bitperpix = screen->format->BitsPerPixel;
//            char          memorymodel;
  agmp->directscreenaccess = 0;

  *hgmp = *agmp;

	graphicinitialized = 1;
   return 1;
}
             // returns > 0  modenum to reestablish this mode
             //         < 0 : error

void  closegraphics ( void )
{
	SDL_FreeSurface ( screen );
	screen = NULL;
}



//*********** Misc ************


int copy2screen( void )
{
   SDL_UpdateRect ( screen , 0,0,0,0 );
}

void setdisplaystart( int x, int y)
{
}

void set_vgapalette256 ( dacpalette256 pal )
{
	SDL_Color spal[256];
	for ( int i = 0; i < 256; i++ ) {
		spal[i].r = pal[i][0] * 4;
		spal[i].g = pal[i][1] * 4;
		spal[i].b = pal[i][2] * 4;
	}	
	int res = SDL_SetColors ( screen, spal, 0, 256 );
	printf("result of setting the palette is %d \n", res );
}

int dont_use_linear_framebuffer;

