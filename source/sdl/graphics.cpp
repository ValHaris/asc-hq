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

//     $Id: graphics.cpp,v 1.11 2000-08-12 09:17:42 gulliver Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.10  2000/06/01 15:27:47  mbickel
//      Some changes for the upcoming Win32 version of ASC
//      Fixed error at startup: unable to load smalaril.fnt
//
//     Revision 1.9  2000/02/08 08:26:54  steb
//     Changed SDL.h to SDL/SDL.h to make installation on the average users system
//     easier.
//
//     Revision 1.8  2000/01/31 16:08:40  mbickel
//      Fixed crash in line
//      Improved error handling in replays
//
//     Revision 1.7  2000/01/07 13:20:07  mbickel
//      DGA fullscreen mode now working
//
//     Revision 1.6  2000/01/06 14:11:22  mbickel
//      Fixed a graphic bug in PD and disabled fullscreen mode
//
//     Revision 1.5  2000/01/06 11:19:16  mbickel
//      Worked on the Linux-port again...
//
//     Revision 1.4  2000/01/04 19:43:55  mbickel
//      Continued Linux port
//
//     Revision 1.3  2000/01/02 19:47:09  mbickel
//      Continued Linux port
//      Fixed crash at program exit
//
//     Revision 1.2  2000/01/01 19:04:20  mbickel
//     /tmp/cvsVhJ4Z3
//
//     Revision 1.1  1999/12/28 21:03:31  mbickel
//      Continued Linux port
//      Added KDevelop project files
//


#include <stdlib.h>
#include "graphics.h"
#include "../basegfx.h"
#include "../global.h"
#include sdlheader



SDL_Surface *screen = NULL;
int fullscreen = 1;

int reinitgraphics(int modenum)
{
  return 1;
}

int isfullscreen ( void )
{
   if ( !screen )
      return 0;
   else
      return screen->flags & SDL_FULLSCREEN;
}


int initgraphics ( int x, int y, int depth )
{
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
     fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
     return -1;
  }

  /* Clean up on exit */
  atexit(SDL_Quit);

  SDL_WM_SetCaption ( "Advanced Strategic Command", NULL );
  /* Initialize the display in a 640x480 8-bit palettized mode */
  int flags = SDL_SWSURFACE;
  if ( fullscreen )
     flags |= SDL_FULLSCREEN;

  screen = SDL_SetVideoMode(x, y, depth, flags ); // | SDL_FULLSCREEN
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
	return 0;
}

int copy2screen( int x1, int y1, int x2, int y2 )
{
   if ( x1 == -1 || y1 == -1 || x2 == -1 || y2 == -1 )
      SDL_UpdateRect ( screen , 0,0,0,0 );
   else
      if ( x1 <= x2 && y1 <= y2 )
         SDL_UpdateRect ( screen , x1, y1, x2-x1+1, y2-y1+1 );
      else
         if( x1 > x2 )
            SDL_UpdateRect ( screen , x2, y1, x1-x2+1, y2-y1+1 );
         else
            SDL_UpdateRect ( screen , x1, y2, x2-x1+1, y1-y2+1 );
	return 0;
}


void setdisplaystart( int x, int y)
{
}

void set_vgapalette256 ( dacpalette256 pal )
{
	SDL_Color spal[256];
	int col;
	for ( int i = 0; i < 256; i++ ) {
	   for ( int c = 0; c < 3; c++ ) {
         if ( pal[i][c] == 255 )
            col = activepalette[i][c];
         else {
            col = pal[i][c];
            activepalette[i][c] = col;
         }
         switch ( c ) {
            case 0: spal[i].r = col * 4; break;
            case 1: spal[i].g = col * 4; break;
		      case 2: spal[i].b = col * 4; break;
		   };
      }
	
	
	}	
	int res = SDL_SetColors ( screen, spal, 0, 256 );
	// printf("result of setting the palette is %d \n", res );
}

int dont_use_linear_framebuffer;

