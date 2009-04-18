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
    Update Date:      $Date: 2009-04-18 13:48:40 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/core/pgmessageobject.cpp,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

#include "pgmessageobject.h"
#include "pgapplication.h"
#include "pgwidget.h"
#include "pglog.h"

#include <iostream>
#include <algorithm>

// static variables for message processing
//vector<PG_MessageObject*> PG_MessageObject::objectList;
PG_MessageObject* PG_MessageObject::captureObject = NULL;
PG_MessageObject* PG_MessageObject::inputFocusObject = NULL;
PG_Widget* PG_MessageObject::lastwidget = NULL;

/** constructor */

PG_MessageObject::PG_MessageObject() {

	// init vars
	my_canReceiveMessages = true;
	my_oldCapture = NULL;
	my_oldFocus = NULL;

	//objectList.push_back(this);
}


/**  destructor */

PG_MessageObject::~PG_MessageObject() {
	sigDelete(this);
	//RemoveObject(this);

	//PG_UnregisterEventObject(this);

	if (inputFocusObject == this) {
		inputFocusObject = NULL;
	}

	if (lastwidget == this) {
		lastwidget = NULL;
	}

	if (captureObject == this) {
		captureObject = NULL;
	}
}

/** enable / disable if object can receive messages */

void PG_MessageObject::EnableReceiver(bool enable) {
	my_canReceiveMessages = enable;
}


/** message dispatcher */

bool PG_MessageObject::ProcessEvent(const SDL_Event* event) {

	// check if we are able to process messages
	if(!my_canReceiveMessages) {
		return false;
	}

	if(event->type != SDL_USEREVENT) {
		if(captureObject != this)
			if(!AcceptEvent(event)) {
				return false;
			}
	}

	if((captureObject != NULL) && (captureObject != this)) {
		return false;
	}

	bool rc = false;

	// dispatch message
	switch(event->type) {
		case SDL_ACTIVEEVENT:
			rc = eventActive(&event->active) || sigActive(this, &event->active);
			break;

		case SDL_KEYDOWN:
			rc = eventKeyDown(&event->key) || sigKeyDown(this, &event->key);
			break;

		case SDL_KEYUP:
			rc = eventKeyUp(&event->key) || sigKeyUp(this, &event->key);
			;
			break;

		case SDL_MOUSEMOTION:
			rc = eventMouseMotion(&event->motion) || sigMouseMotion(this, &event->motion);
			break;

		case SDL_MOUSEBUTTONDOWN:
			rc = eventMouseButtonDown(&event->button) || sigMouseButtonDown(this, &event->button);
			break;

		case SDL_MOUSEBUTTONUP:
			rc = eventMouseButtonUp(&event->button) || sigMouseButtonUp(this, &event->button);
			break;

		case SDL_QUIT:
			rc = eventQuit(PG_Application::IDAPPLICATION, NULL, (unsigned long)&event->quit) || sigQuit(this);
			break;

		case SDL_SYSWMEVENT:
			rc = eventSysWM(&event->syswm) || sigSysWM(this, &event->syswm);
			break;

		case SDL_VIDEORESIZE:
			rc = eventResize(&event->resize) || sigVideoResize(this, &event->resize);
			break;

		default:
			rc = false;
			break;
	}

	return rc;
}


/** virtual message handlers */

bool PG_MessageObject::eventActive(const SDL_ActiveEvent* active) {
	return false;
}


bool PG_MessageObject::eventKeyDown(const SDL_KeyboardEvent* key) {
	return false;
}


bool PG_MessageObject::eventKeyUp(const SDL_KeyboardEvent* key) {
	return false;
}


bool PG_MessageObject::eventMouseMotion(const SDL_MouseMotionEvent* motion) {
	return false;
}


bool PG_MessageObject::eventMouseButtonDown(const SDL_MouseButtonEvent* button) {
	return false;
}


bool PG_MessageObject::eventMouseButtonUp(const SDL_MouseButtonEvent* button) {
	return false;
}


bool PG_MessageObject::eventQuit(int id, PG_MessageObject* widget, unsigned long data) {
	return false;
}

bool PG_MessageObject::eventQuitModal(int id, PG_MessageObject* widget, unsigned long data) {
	return false;
}


bool PG_MessageObject::eventSysWM(const SDL_SysWMEvent* syswm) {
	return false;
}


bool PG_MessageObject::eventResize(const SDL_ResizeEvent* event) {
	return false;
}

bool PG_MessageObject::AcceptEvent(const SDL_Event* event) {
	return true;				// PG_MessageObject accepts all events
}

/** capture handling (an object can capture all messages) */

PG_MessageObject* PG_MessageObject::SetCapture() {
	if(captureObject == this)
		return my_oldCapture;

	my_oldCapture = captureObject;
	captureObject = this;
	return my_oldCapture;
}


void PG_MessageObject::ReleaseCapture() {
	if(captureObject != this) {
		return;
	}

	captureObject = my_oldCapture;
}

PG_MessageObject* PG_MessageObject::SetInputFocus() {
	if(inputFocusObject == this)
		return my_oldFocus;

	my_oldFocus = inputFocusObject;

	if(my_oldFocus != NULL) {
		my_oldFocus->eventInputFocusLost(inputFocusObject);
	}

	inputFocusObject = this;
	return my_oldFocus;
}


/** */
void PG_MessageObject::eventInputFocusLost(PG_MessageObject* newfocus) {}


/** */
void PG_MessageObject::ReleaseInputFocus() {
	if(inputFocusObject != this) {
		return;
	}

	inputFocusObject = NULL;
}

/*SDL_Event PG_MessageObject::WaitEvent(Uint32 delay) {
	static SDL_Event event;
 
	while(SDL_PollEvent(&event) == 0) {
		//		eventIdle();
		if(delay > 0) {
			SDL_Delay(delay);
		}
	}
 
	return event;
}*/


/** Remove an object from the message queue  */

/*bool PG_MessageObject::RemoveObject(PG_MessageObject* obj) {
	vector<PG_MessageObject*>::iterator list = objectList.begin();
 
	// search the object
	list = find(objectList.begin(), objectList.end(), obj);
 
	// check if object was found
	if(list == objectList.end()) {
		return false;
	}
 
	// mark object for removal
	*list = NULL;
	
	return true;
}*/

PG_MessageObject* PG_MessageObject::GetCapture() {
	return captureObject;
}

bool PG_MessageObject::IsEnabled() {
	return my_canReceiveMessages;
}
