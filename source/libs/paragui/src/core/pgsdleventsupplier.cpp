/*
    ParaGUI - crossplatform widgetset
    Copyright (C) 2000,2001,2002  Alexander Pipelka
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
 
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
    Alexander Pipelka
    pipelka@teleweb.at
 
    Last Update:      $Author: mbickel $
    Update Date:      $Date: 2008-06-24 18:20:57 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/core/pgsdleventsupplier.cpp,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

#include "SDL.h"

#include "pgsdleventsupplier.h"

void PG_SDLEventSupplier::CombineMouseMotionEvents(SDL_Event* event) {
	if ( event->type == SDL_MOUSEMOTION ) {
		SDL_Event nextEvent;
		if ( PeepEvent ( &nextEvent ) )
			if ( nextEvent.type == SDL_MOUSEMOTION ) {
				int motionxrel = event->motion.xrel;
				int motionyrel = event->motion.yrel;

				// get the event from the queue
				WaitEvent( event );

				// add the motion distances of the last event
				event->motion.xrel += motionxrel;
				event->motion.yrel += motionyrel;
			}
	}
}

void PG_SDLEventSupplier::evaluateMouseEvents(SDL_Event* event) {
   if ( event->type == SDL_MOUSEMOTION ) {
      mousex = event->motion.x;
      mousey = event->motion.y;
   } else if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP ) {
      mousex = event->button.x;
      mousey = event->button.y;
   }
}


bool PG_SDLEventSupplier::PeepEvent(SDL_Event* event) {
	return SDL_PeepEvents( event, 1, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) > 0;
}


bool PG_SDLEventSupplier::PollEvent(SDL_Event* event) {
	bool eventAvail = SDL_PollEvent( event ) > 0;
	if ( eventAvail ) {
		CombineMouseMotionEvents( event );
		evaluateMouseEvents( event );
	}
	return eventAvail;
}


int PG_SDLEventSupplier::WaitEvent(SDL_Event* event) {
	int res = SDL_WaitEvent( event );
	CombineMouseMotionEvents( event );
	evaluateMouseEvents( event );
	return res;
}

int  PG_SDLEventSupplier::GetMouseState(int& x, int& y) {
    // the mouse coordinates return by SDL_GetMouseState are not adjusted to the display scaling, unlike the events.
    x = mousex;
    y = mousey;
	return SDL_GetMouseState(NULL, NULL);
}
