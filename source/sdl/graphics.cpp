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
#include "../util/messaginghub.h"


#include "sdlstretch.cpp"

SDL_Surface *screen = NULL;


void setWindowCaption ( const char* s )
{
   SDL_WM_SetCaption ( s, NULL );
}


bool dummyScreenPaletteSetup = false;

void initASCGraphicSubsystem ( SDL_Surface* _screen )
{
  screen = _screen;
  agmp->resolutionx = screen->w;
  agmp->resolutiony = screen->h;
  agmp->windowstatus = 100;
  agmp->scanlinenumber = screen->h;
  agmp->byteperpix = 1 ;
  agmp->bitperpix = 8;
  agmp->directscreenaccess = 0;
  delete agmp->surface;
  if ( _screen->format->BitsPerPixel == 8 ) {
     agmp->surface = new Surface ( _screen );
     dummyScreenPaletteSetup = true;
  } else {
     agmp->surface = new Surface( Surface::createSurface(screen->w, screen->h, 8 ));
     dummyScreenPaletteSetup = false;
  }
  agmp->linearaddress = (PointerSizedInt) agmp->surface->pixels();
  agmp->scanlinelength = agmp->bytesperscanline = agmp->surface->pitch();
     

  *hgmp = *agmp;
}



void shutdownASCGraphicSubsystem()
{
   if ( agmp && agmp->surface )
      delete agmp->surface;
}

/*
SDL_Surface* getScreen()
{
   return screen;
}
*/

Surface& getActiveSurface()
{
   return *agmp->surface;
}   


//*********** Misc ************


int copy2screen( void )
{
  #ifdef _WIN32_  
   // SDL_ShowCursor(0);
  #endif 
   if ( !dummyScreenPaletteSetup ) {
      hgmp->surface->assignDefaultPalette();
      dummyScreenPaletteSetup = true;
   }   
  
   if ( screen->format->BitsPerPixel > 8 ) 
      SDL_BlitSurface( hgmp->surface->getBaseSurface() , NULL, screen, NULL );
   SDL_UpdateRect ( screen , 0,0,0,0 );
  #ifdef _WIN32_  
   // SDL_ShowCursor(1);
  #endif 
   return 0;
}

int copy2screen( int x1, int y1, int x2, int y2 )
{
   MouseHider mouseHider;

   if ( !dummyScreenPaletteSetup ) {
      hgmp->surface->assignDefaultPalette();
      dummyScreenPaletteSetup = true;
   }   
   
   if ( screen->format->BitsPerPixel > 8 ) {
      SDL_Rect r;
      r.x = min(x1,x2);
      r.y = min(y1,y2);
      r.w = abs(x2-x1)+1;
      r.h = abs(y2-y1)+1;
      
      SDL_BlitSurface( hgmp->surface->getBaseSurface() , &r, screen, &r );
   }
   
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

MouseHider::MouseHider() : locked(true)
{
  #ifdef _WIN32_  
   SDL_GetMouseState(&x, &y);
   SDL_ShowCursor(0);
  #endif 
}

void MouseHider::unlock()
{
  #ifdef _WIN32_  
   SDL_WarpMouse(x, y);
   SDL_ShowCursor(1);
   // SDL_WarpMouse(x, y);
   locked = false;
  #endif 

}

MouseHider::~MouseHider()
{
   if ( locked )
      unlock();
}
