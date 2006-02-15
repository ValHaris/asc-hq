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
    Update Date:      $Date: 2006-02-15 21:30:16 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/core/pgsdleventsupplier.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
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

bool PG_SDLEventSupplier::PeepEvent(SDL_Event* event) {
	return SDL_PeepEvents( event, 1, SDL_PEEKEVENT, 0xffffffff );
}


bool PG_SDLEventSupplier::PollEvent(SDL_Event* event) {
	bool eventAvail = SDL_PollEvent( event );
	if ( eventAvail )
		CombineMouseMotionEvents( event );
	return eventAvail;
}


int PG_SDLEventSupplier::WaitEvent(SDL_Event* event) {
	int res = SDL_WaitEvent( event );
	CombineMouseMotionEvents( event );
	return res;
}

int  PG_SDLEventSupplier::GetMouseState(int& x, int& y) {
	return SDL_GetMouseState(&x, &y);
}
