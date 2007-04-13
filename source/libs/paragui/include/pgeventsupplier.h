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
    Update Date:      $Date: 2007-04-13 16:15:56 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgeventsupplier.h,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

/** \file pgsdleventsupplier.h
 Header file for the PG_EventSupplier class.
 This include file defines the abstract PG_EventSupplier class 
*/

#ifndef PG_EVENTSUPPLIER_H
#define PG_EVENTSUPPLIER_H


#include "SDL.h"

/**
 @author Martin Bickel
 
 @short Interface for classes that supply SDL_Events to Paragui
 
        Paragui works with SDL_Events to obtain input and operating system events. Instead of getting them directly 
        from SDL it uses this interface, allowing the application to preprocess SDL_events or obtain them from an 
        application specific event source.
*/
class DECLSPEC PG_EventSupplier {
public:

	// make gcc 4 happy
	virtual ~PG_EventSupplier() {}
	;

	/**
	Polls for currently pending events, and returns true if there are any pending events, or false if there are none available. 
	If event is not NULL, the next event is removed from the queue and stored in that area.

	@param event pointer to an event structure
	@return  true - events are available
	*/
	virtual bool PollEvent(SDL_Event* event) = 0;

	/**
	Checks if an event is in the queue. If there is, it will be copied into the event structure, 
	WITHOUT being removed from the event queue. 

	@param event pointer to an event structure
	@return  true - events are available
	*/
	virtual bool PeepEvent(SDL_Event* event) = 0;


	/**
	Waits indefinitely for the next available event.

	@param event  pointer to an event structure
	@return  return 0 if there was an error while waiting for events        
	*/
	virtual int WaitEvent(SDL_Event* event) = 0;

	/**
	Get the current mouse position.

	@param x current mouse x position
	@param y current mouse y position
	       @return bitmap of pressed mouse buttons
	*/
	virtual int GetMouseState(int& x, int& y) = 0;
};

#endif // PG_EVENTSUPPLIER_H
