/***************************************************************************
                          mousesdl.cpp  -  description
                             -------------------
    begin                : Sun Dec 19 1999
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



#include <SDL.h>
#include <SDL_thread.h>

bool isKeyPressed(SDL_KeyCode key)
{
   int numkeys;
   const Uint8 *keystate = SDL_GetKeyboardState ( &numkeys );
   if ( key >= numkeys)
	   return false;
   return keystate[ key ];
}

int  releasetimeslice( void )
{
   SDL_Delay(10);
   return 0;
}


void exit_asc( int returnresult )
{

}






