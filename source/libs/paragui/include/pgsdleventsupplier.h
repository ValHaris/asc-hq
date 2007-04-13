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
    Update Date:      $Date: 2007-04-13 16:15:57 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgsdleventsupplier.h,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

/** \file pgsdleventsupplier.h
 Header file for the PG_EventSupplier class.
 This include file defines the abstract PG_EventSupplier class 
*/

#ifndef PG_SDLEVENTSUPPLIER_H
#define PG_SDLEVENTSUPPLIER_H

#include "pgeventsupplier.h"

/**
 @author Martin Bickel
 
 @short Classes which passes SDL_Events directly from SDL to Paragui
 
        No preprocessing or filtering takes place. 
*/

class DECLSPEC PG_SDLEventSupplier : public PG_EventSupplier {

protected:
	/**
	Mouse motion events can fill the event queue and should also be processed without much delay.
	       This function is passed a newly received Event. It checks if it is a mouse motion event and, 
	       if this is the case, checks the event queue for more mouse motion events, which are then incorporated
	       into the current event and removed from the queue.

	@param event pointer to an event structure with a freshly received event
	*/
	void CombineMouseMotionEvents(SDL_Event* event);

public:


	/**
	Polls for currently pending events, and returns true if there are any pending events, or false if there are none available. 
	If event is not NULL, the next event is removed from the queue and stored in that area.

	@param event pointer to an event structure
	@return  true - events are available
	*/
	bool PollEvent(SDL_Event* event);

	/**
	Checks if an event is in the queue. If there is, it will be copied into the event structure, 
	WITHOUT being removed from the event queue. 

	@param event pointer to an event structure
	@return  true - events are available
	*/
	bool PeepEvent(SDL_Event* event);


	/**
	Waits indefinitely for the next available event.

	@param event  pointer to an event structure
	@return  return 0 if there was an error while waiting for events        
	*/
	int WaitEvent(SDL_Event* event);

	/**
	Get the current mouse position.

	@param x current mouse x position
	@param y current mouse y position
	@return bitmap of pressed mouse buttons
	*/
	int GetMouseState(int& x, int& y);

};

#endif // PG_SDLEVENTSUPPLIER_H
