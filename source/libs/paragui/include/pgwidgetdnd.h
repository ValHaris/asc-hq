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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgwidgetdnd.h,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

#ifndef PG_WIDGETDND_H
#define PG_WIDGETDND_H

#include "pgwidget.h"

/**
	@author Alexander Pipelka
 
	@short Drag and drop handler.
 
	All classes that would ever require drag and drop functionality
	(esentially all widgets) derive from this (eventually). Handles both the
	sending and receiving of all DnD events.
*/

class DECLSPEC PG_WidgetDnD : public PG_Widget  {
public:

	/**
	*/
	PG_WidgetDnD(PG_Widget* parent, int dndID, const PG_Rect& r);

	/**
	*/
	PG_WidgetDnD(PG_Widget* parent, int dndID, const PG_Rect& r, bool bCreateSurface);

	/**
	*/
	~PG_WidgetDnD();

	/**  */
	bool GetDrop();

	/**  */
	bool GetDrag();

	/**  */
	void RemoveObjectDnD(PG_WidgetDnD* obj);

	/**  */
	void SetDrop(bool drop);

	/**  */
	void SetDrag(bool drag);

	/**  */
	void updateDragArea(PG_Point pt, SDL_Surface* image);

	/**  */
	void drawDragArea(PG_Point pt, SDL_Surface* image);

protected: // Protected methods

	/**  */
	PG_WidgetDnD* FindDropTarget(PG_Point pt);

	/**  */
	bool eventMouseButtonDown(const SDL_MouseButtonEvent* button);

	/**  */
	bool eventMouseMotion(const SDL_MouseMotionEvent* motion);

	/**  */
	bool eventMouseButtonUp(const SDL_MouseButtonEvent* button);

	/**  */
	virtual bool AcceptDrop(PG_WidgetDnD* source, int dndID);

	/**  */
	virtual bool eventDragStart();

	/**  */
	virtual bool eventDragDrop(PG_WidgetDnD* source, int dndID);

	/**  */
	virtual bool eventDragCancel();

	/**  */
	virtual SDL_Surface* eventQueryDragImage();

	/**  */
	virtual SDL_Surface* eventQueryDropImage(SDL_Surface* dragimage = NULL);

	PG_WidgetDnD* dnd_next;
	static PG_WidgetDnD* dnd_objectlist;

private:

	/**  */
	DLLLOCAL void cacheDragArea(PG_Point p);

	/**  */
	DLLLOCAL void restoreDragArea(PG_Point p);

	/**  */
	DLLLOCAL void CheckCursorPos(int& x, int& y);

	/**  */
	DLLLOCAL void slideDragImage(PG_Point start, PG_Point end, int steps, SDL_Surface* image);

	bool CanDrag;

	bool CanDrop;

	bool Draging;

	PG_Point dragPointStart;

	PG_Point dragPointOld;

	PG_Point dragPointCurrent;

	SDL_Surface* dragimage;

	SDL_Surface* dragimagecache;

private: // disable the copy operators

	PG_WidgetDnD(const PG_WidgetDnD&);
	PG_WidgetDnD& operator=(const PG_WidgetDnD&);

};

#endif // PG_WIDGETDND_H
