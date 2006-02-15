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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/core/pgrectlist.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

#include "pgrectlist.h"
#include "pgwidget.h"
#include "pgapplication.h"
#include "pglog.h"

#include <algorithm>

PG_RectList::PG_RectList() :
		my_first(NULL),
		my_last(NULL),
my_count(0) {}

PG_RectList::~PG_RectList() {}

/*PG_RectList PG_RectList::Intersect(PG_Rect* rect, PG_Rect* first, PG_Rect* last) {
	PG_RectList result;
 
	if(first == NULL) {
		return result;
	}
 
	//PG_Widget* testwidget;
	PG_Rect* testrect;
 
	// loop through all rects
	for(PG_Widget* i = static_cast<PG_Widget*>(first); i != last; i = static_cast<PG_Widget*>(i->next)) {
 
		// get the next rectangle to test
		//testwidget = (*i).second;
 
		if(!i->IsVisible() || i->IsHidden()) {
			continue;
		}
 
		testrect = i->GetClipRect();
		if(rect->OverlapRect(*testrect)) {
			// append the matching rectangle
			result.Add(new PG_Rect(*i));
		}
	}
 
	return result;
}*/

PG_Widget* PG_RectList::IsInside(const PG_Point& p) {
	// loop down all rects till we find a match
	for(PG_Widget* i = last(); i != NULL; i = i->prev()) {

		// check if the tested rect is visible
		if(!i->IsVisible() || i->IsHidden()) {
			continue;
		}

		// check for a match
		if(i->GetClipRect()->IsInside(p)) {
			return i;
		}
	}

	return NULL;
}

void PG_RectList::Add(PG_Widget* rect, bool front) {
	if(rect->next() != NULL || rect->prev() != NULL) {
		PG_LogWRN("PG_RectList::Add(...) Trying to add a linked PG_Rect object");
		return;
	}

	Uint32 index = 2^31;
	my_count++;

	// get highest index
	if(my_last != NULL) {
		index = my_last->index;
	}
	index++;

	if(front) {
		if(my_first != NULL) {
			my_first->my_prev = rect;
			rect->index = my_first->index-1;
		} else {
			rect->index = index;
		}
		rect->my_next = my_first;
		rect->my_prev = NULL;
		my_first = rect;
		return;
	}

	if(my_first == NULL) {
		my_first = rect;
		rect->my_prev = NULL;
		rect->my_next = NULL;
	} else {
		my_last->my_next = rect;
		rect->my_next = NULL;
		rect->my_prev = my_last;
	}
	my_last = rect;
	rect->index = index;
}

bool PG_RectList::Remove(PG_Rect* rect) {
	if(rect == NULL) {
		return false;
	}

	if((rect->next() == NULL) && (rect->prev() == NULL) && (my_first != rect)) {
		//PG_LogWRN("PG_RectList::Remove(...) Trying to remove an unlinked PG_Rect object");
		return false;
	}

	if(my_count > 0) {
		my_count--;
	}

	// first in list
	if(rect->my_prev == NULL) {
		my_first = rect->next();
		if(my_first != NULL) {
			my_first->my_prev = NULL;
		}
		// first and last
		else {
			my_last = NULL;
		}
	}
	// last
	else if(rect->my_next == NULL) {
		my_last = rect->my_prev;
		if(my_last != NULL) {
			my_last->my_next = NULL;
		}
	}
	// in between
	else {
		rect->my_prev->my_next = rect->my_next;
		rect->my_next->my_prev = rect->my_prev;
	}

	rect->my_next = NULL;
	rect->my_prev = NULL;

	return true;
}

void PG_RectList::Blit(const PG_Rect& rect) {
	Blit(rect, first());
}

void PG_RectList::Blit(const PG_Rect& rect, PG_Widget* start, PG_Widget* end) {
	if(start == NULL) {
		return;
	}

	PG_RectList* childs;
	SDL_Surface* screen = PG_Application::GetScreen();

	// store old clipping rectangle
	PG_Rect o;
	SDL_GetClipRect(screen, &o);

	// blit all objects in the list
	for(PG_Widget* i = start; i != end; i = i->next()) {

		if(!i->IsVisible() || i->IsHidden()) {
			continue;
		}

		// calculate the clipping rectangle
		// cliprect = blittingregion / widgetcliprect
		PG_Rect* cr = i->GetClipRect();
		if(!rect.OverlapRect(*cr)) {
			continue;
		}

		PG_Rect c = cr->IntersectRect(rect);
		SDL_SetClipRect(screen, &c);

		// blit it
		i->Blit(false, false);

		// blit all children of the widget
		childs = i->GetChildList();
		if(childs) {
			childs->Blit(rect);
		}
	}

	// reset clipping rectangle
	SDL_SetClipRect(PG_Application::GetScreen(), &o);
}

void PG_RectList::Blit() {
	// blit all objects in the list
	for(PG_Widget* i = first(); i != NULL; i = i->next()) {
		if(!i->IsVisible() || i->IsHidden()) {
			continue;
		}

		i->Blit(true, false);
	}
}

bool PG_RectList::BringToFront(PG_Widget* rect) {
	if(!Remove(rect)) {
		return false;
	}
	Add(rect);

	return true;
}

bool PG_RectList::SendToBack(PG_Widget* rect) {
	if(!Remove(rect)) {
		return false;
	}
	Add(rect, true);

	return true;
}

PG_Widget* PG_RectList::Find(int id, bool recursive ) {
	for(PG_Widget* i = first(); i != NULL; i = i->next()) {
		if(i->GetID() == id) {
			return i;
		}
	}
	if ( recursive )
		for(PG_Widget* i = first(); i != NULL; i = i->next()) {
			PG_Widget* w = i->FindChild(id, recursive );
			if( w ) {
				return w;
			}
		}
	return NULL;
}

PG_Widget* PG_RectList::Find(const std::string& name, bool recursive ) {
	for(PG_Widget* i = first(); i != NULL; i = i->next()) {
		if(i->GetName() == name) {
			return i;
		}
	}
	if ( recursive )
		for(PG_Widget* i = first(); i != NULL; i = i->next()) {
			PG_Widget* w = i->FindChild(name, recursive );
			if( w ) {
				return w;
			}
		}
	return NULL;
}

void PG_RectList::clear() {
	my_first = NULL;
	my_last = NULL;
	my_count = 0;
}
