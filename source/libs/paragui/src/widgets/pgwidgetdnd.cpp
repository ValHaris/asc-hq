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
    Update Date:      $Date: 2006-05-14 19:02:32 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgwidgetdnd.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.2 $
    Status:           $State: Exp $
*/

#include "pgwidgetdnd.h"
#include "pgapplication.h"
#include "pgdraw.h"
#include "pgeventsupplier.h"

PG_WidgetDnD* PG_WidgetDnD::dnd_objectlist = NULL;

PG_WidgetDnD::PG_WidgetDnD(PG_Widget* parent, int dndID, const PG_Rect& r) :
		PG_Widget(parent, r, false),
		CanDrag(true),
		CanDrop(true),
		Draging(false),
		dragimage(NULL),
dragimagecache(NULL) {
	SetID(dndID);

	// put myself into the dnd chain
	dnd_next = dnd_objectlist;
	dnd_objectlist = this;
}

PG_WidgetDnD::PG_WidgetDnD(PG_Widget* parent, int dndID, const PG_Rect& r, bool bCreateSurface) :
		PG_Widget(parent, r, bCreateSurface),
		CanDrag(true),
		CanDrop(true),
		Draging(false),
		dragimage(NULL),
dragimagecache(NULL) {
	SetID(dndID);

	// put myself into the dnd chain
	dnd_next = dnd_objectlist;
	dnd_objectlist = this;
}

PG_WidgetDnD::~PG_WidgetDnD() {
	RemoveObjectDnD(this);
}

/**  */
bool PG_WidgetDnD::GetDrag() {
	return CanDrag;
}

/**  */
bool PG_WidgetDnD::GetDrop() {
	return CanDrop;
}

/**  */
void PG_WidgetDnD::RemoveObjectDnD(PG_WidgetDnD* obj) {
	PG_WidgetDnD* obj_before = NULL;
	PG_WidgetDnD* list = dnd_objectlist;

	// search the object
	while((list != NULL) && (list != obj)) {
		obj_before = list;
		list = list->dnd_next;
	}

	// check if object was found
	if(list == NULL)
		return;

	// remove the object from the chain

	if(obj_before == NULL)
		dnd_objectlist = this->dnd_next;
	else {
		obj_before->dnd_next = obj->dnd_next;
	}
	obj->dnd_next = NULL;
}

/**  */
PG_WidgetDnD* PG_WidgetDnD::FindDropTarget(PG_Point pt) {
	PG_WidgetDnD* list = dnd_objectlist;

	// Process all DnD widgets
	while(list != NULL) {
		if(list->IsInside(pt) && list->IsVisible()) {					// drop point inside widget -> possible detection (kind regards seti)
			break;
		}
		list = list->dnd_next;
	}

	if(list == NULL) {							// no drop target found
		return NULL;
	}

	if(!list->GetDrop()) {					// check if target is able to catch drops
		return NULL;
	}

	if(list->AcceptDrop(this, GetID())) {		// ask if we may drop
		return list;
	}

	return NULL;		// no one wants us to drop on them
}

/**  */
bool PG_WidgetDnD::AcceptDrop(PG_WidgetDnD* source, int dndID) {
	return CanDrop;
}

/**  */
void PG_WidgetDnD::SetDrag(bool drag) {
	CanDrag = drag;
}

/**  */
void PG_WidgetDnD::SetDrop(bool drop) {
	CanDrop = drop;
}

/**  */
bool PG_WidgetDnD::eventDragStart() {
	return true;
}

/**  */
bool PG_WidgetDnD::eventMouseButtonDown(const SDL_MouseButtonEvent* button) {
	int x,y;

	PG_Application::GetEventSupplier()->GetMouseState(x, y);
	CheckCursorPos(x, y);

	if((button->button == 1) && CanDrag) {
		SetCapture();

		dragPointOld.x = x;
		dragPointOld.y = y;

		dragPointStart.x = x;
		dragPointStart.y = y;

		Draging = true;
		eventDragStart();
		dragimage = eventQueryDragImage();

		if(dragimage != NULL) {
			dragimagecache = PG_Draw::CreateRGBSurface(dragimage->w, dragimage->h);
		}

		cacheDragArea(dragPointOld);

		return true;
	}

	return false;
}

/**  */
bool PG_WidgetDnD::eventMouseMotion(const SDL_MouseMotionEvent* motion) {
	PG_WidgetDnD* target = NULL;
	SDL_Surface* dropimage = NULL;

	int x,y;
	PG_Point pt;

	PG_Application::GetEventSupplier()->GetMouseState(x, y);
	CheckCursorPos(x, y);
	pt.x = x;
	pt.y = y;

	// Process message if we are in drag-mode
	if(Draging) {

		if(dragimage == NULL) {
			return true;
		}

		target = FindDropTarget(pt);

		if(target != NULL) {
			dropimage = target->eventQueryDropImage(dragimage);

			if(dropimage != NULL) {
				dragimage = dropimage;
			}
		} else {
			dragimage = eventQueryDragImage();
		}

		dragPointCurrent.x = x;
		dragPointCurrent.y = y;

		// restore area on old drag position
		restoreDragArea(dragPointOld);

		// copy new area into cache
		cacheDragArea(dragPointCurrent);

		// paint and update new drag area
		drawDragArea(dragPointCurrent, dragimage);
		updateDragArea(dragPointCurrent, dragimage);

		//  update old dragposition (screen)
		updateDragArea(dragPointOld, dragimagecache);

		// old = current
		dragPointOld.x = x;
		dragPointOld.y = y;

		return true;
	}

	return  PG_Widget::eventMouseMotion(motion);
}

/**  */
bool PG_WidgetDnD::eventMouseButtonUp(const SDL_MouseButtonEvent* button) {
	PG_WidgetDnD* target = NULL;
	int x,y;
	PG_Point pt;

	PG_Application::GetEventSupplier()->GetMouseState(x, y);
	CheckCursorPos(x, y);

	pt.x = x;
	pt.y = y;

	if((button->button == 1) && Draging) {
		target = FindDropTarget(pt);

		if(dragimagecache != NULL) {

			// restore area on old drag position
			restoreDragArea(dragPointOld);

			//  update old dragposition (screen)

			drawDragArea(dragPointOld, dragimagecache);
			updateDragArea(dragPointOld, dragimagecache);
		}

		if(target != NULL) {
			target->eventDragDrop(this, GetID());
		} else {
			slideDragImage(pt, dragPointStart, 20, dragimage);
			eventDragCancel();
		}

		if(dragimagecache != NULL) {
			// free surface
			PG_Application::UnloadSurface(dragimagecache);

			dragimage = NULL;
			dragimagecache = NULL;
		}

		ReleaseCapture();
		Draging = false;

		return true;
	}

	return false;
}

/**  */
bool PG_WidgetDnD::eventDragDrop(PG_WidgetDnD* source, int dndID) {
	return true;
}

/**  */
bool PG_WidgetDnD::eventDragCancel() {
	return true;
}

/**  */
SDL_Surface* PG_WidgetDnD::eventQueryDragImage() {
	return NULL;
}

/**  */
void PG_WidgetDnD::cacheDragArea(PG_Point p) {
	SDL_Rect srcrect;
	SDL_Rect dstrect;

	if(!dragimagecache || !dragimage)
		return;

	PG_Application::LockScreen();

	srcrect.x = p.x;
	srcrect.y = p.y;
	srcrect.w = dragimagecache->w;
	srcrect.h = dragimagecache->h;

	dstrect.x = 0;
	dstrect.y = 0;
	dstrect.w = dragimagecache->w;
	dstrect.h = dragimagecache->h;

	PG_Draw::BlitSurface(PG_Application::GetScreen(), srcrect, dragimagecache, dstrect);

	PG_Application::UnlockScreen();
}

/**  */
void PG_WidgetDnD::restoreDragArea(PG_Point p) {
	SDL_Rect srcrect;
	SDL_Rect dstrect;

	if((dragimagecache == NULL) || (dragimage == NULL))
		return;

	PG_Application::LockScreen();

	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = dragimagecache->w;
	srcrect.h = dragimagecache->h;

	dstrect.x = p.x;
	dstrect.y = p.y;
	dstrect.w = dragimagecache->w;
	dstrect.h = dragimagecache->h;

	PG_Draw::BlitSurface(dragimagecache, srcrect, PG_Application::GetScreen(), dstrect);

	PG_Application::UnlockScreen();
}
/**  */
void PG_WidgetDnD::CheckCursorPos(int& x, int& y) {

	if(dragimage == NULL)
		return;

	x -= dragimage->w / 2;
	y -= dragimage->h / 2;

	if(x<0)
		x=0;
	if(y<0)
		y=0;

	if(x + dragimage->w > PG_Application::GetScreen()->w)
		x = PG_Application::GetScreen()->w - dragimage->w;

	if(y + dragimage->h > PG_Application::GetScreen()->h)
		y = PG_Application::GetScreen()->h - dragimage->h;
}

/** */
SDL_Surface* PG_WidgetDnD::eventQueryDropImage(SDL_Surface* dragimage) {
	return NULL;
}
/**  */
void PG_WidgetDnD::drawDragArea(PG_Point pt, SDL_Surface* image) {
	SDL_Rect srcrect;
	SDL_Rect dstrect;

	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = image->w;
	srcrect.h = image->h;

	dstrect.x = pt.x;
	dstrect.y = pt.y;
	dstrect.w = image->w;
	dstrect.h = image->h;

	PG_Application::LockScreen();
	PG_Draw::BlitSurface(image, srcrect, PG_Application::GetScreen(), dstrect);
	PG_Application::UnlockScreen();

}
/**  */
void PG_WidgetDnD::updateDragArea(PG_Point pt, SDL_Surface* image) {
	SDL_Rect dstrect;

	dstrect.x = pt.x;
	dstrect.y = pt.y;
	dstrect.w = image->w;
	dstrect.h = image->h;

	PG_Application::UpdateRects(PG_Application::GetScreen(), 1, &dstrect);
}

/**  */
void PG_WidgetDnD::slideDragImage(PG_Point start, PG_Point end, int steps, SDL_Surface* image) {
	double dx, dy;
	PG_Point current,old;

	dx = end.x - start.x;
	dy = end.y - start.y;

	dx /= steps;
	dy /= steps;

	for(int i=0; i<steps; i++) {
		current.x = (int)((double)start.x + (double)i*dx);
		current.y = (int)((double)start.y + (double)i*dy);

		if(i>0) {
			drawDragArea(old, dragimagecache);
		}

		cacheDragArea(current);
		drawDragArea(current, dragimage);
		updateDragArea(current, dragimage);

		if(i>0) {
			updateDragArea(old, dragimagecache);
		}

		old = current;

		SDL_Delay(10);
	}

	drawDragArea(old, dragimagecache);
	updateDragArea(old, dragimagecache);
}
