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



#include <stdlib.h>
#include <SDL.h>
#include "graphics.h"
#include "../basegfx.h"
#include "../global.h"
#include "../ascstring.h"
#include "../errors.h"



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

void setWindowCaption ( const char* s )
{
   SDL_WM_SetCaption ( s, NULL );
}

int initgraphics ( int x, int y, int depth, SDLmm::Surface* icon )
{
  if ( SDL_Init(SDL_INIT_VIDEO  ) < 0 )  // | SDL_INIT_NOPARACHUTE
     fatalError ( ASCString("Couldn't initialize SDL: ") + SDL_GetError());


  setWindowCaption ( "Advanced Strategic Command" );

  if ( icon )
     SDL_WM_SetIcon( icon->GetSurface(), NULL );


  /* Initialize the display in a 640x480 8-bit palettized mode */
  int flags = SDL_SWSURFACE;
  if ( fullscreen )
     flags |= SDL_FULLSCREEN;

  SDL_Surface* screen = SDL_SetVideoMode(x, y, depth, flags ); // | SDL_FULLSCREEN
  if ( !screen )
     fatalError ( "Couldn't set %dx%dx%d video mode: %s\n", x,y,depth, SDL_GetError());

  initASCGraphicSubsystem ( screen, icon );

  return 1;
}

void initASCGraphicSubsystem ( SDL_Surface* _screen, SDLmm::Surface* icon )
{
  screen = _screen;
  agmp->resolutionx = screen->w;
  agmp->resolutiony = screen->h;
  agmp->windowstatus = 100;
  agmp->scanlinelength = screen->w;
  agmp->scanlinenumber = screen->h;
  agmp->bytesperscanline = screen->w * screen->format->BytesPerPixel;
  agmp->byteperpix = screen->format->BytesPerPixel ;
  agmp->linearaddress = (PointerSizedInt) screen->pixels;
  agmp->bitperpix = screen->format->BitsPerPixel;
  agmp->directscreenaccess = 0;

  *hgmp = *agmp;

  graphicinitialized = 1;
}


void  closegraphics ( void )
{
        SDL_FreeSurface ( screen );
        screen = NULL;
}

SDL_Surface* getScreen()
{
   return screen;
}



//*********** Misc ************


int copy2screen( void )
{
  #ifdef _WIN32_  
   SDL_ShowCursor(0);
  #endif 
   SDL_UpdateRect ( screen , 0,0,0,0 );
  #ifdef _WIN32_  
   SDL_ShowCursor(1);
  #endif 
   return 0;
}

int copy2screen( int x1, int y1, int x2, int y2 )
{
  #ifdef _WIN32_  
   SDL_ShowCursor(0);
  #endif 
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


  #ifdef _WIN32_  
   SDL_ShowCursor(1);
  #endif 
          
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
        SDL_SetColors ( screen, spal, 0, 256 );
}


