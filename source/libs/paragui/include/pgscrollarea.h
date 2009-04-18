/*
   ParaGUI - crossplatform widgetset
   Copyright (C) 2000-2004  Alexander Pipelka
 
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
   Update Date:      $Date: 2009-04-18 13:48:39 $
   Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgscrollarea.h,v $
   CVS/RCS Revision: $Revision: 1.3 $
   Status:           $State: Exp $
*/

/** \file pgscrollarea.h
	Header file for the PG_ScrollArea class.
*/

#ifndef PG_SCROLLAREA_H
#define PG_SCROLLAREA_H

#include "pgwidget.h"
#include "pgpoint.h"

/**
	@author Alexander Pipelka
	@short A widget containing other widgets which can be paned.
	With this widget you can create larger scrollable areas. This could be one larger
	client-widget or any number of widgets "spanning" the scrollable area. This
	widget doesn't provide scrollbars you can only move to a position by using the
	PG_ScrollArea::ScrollTo method.
*/

class DECLSPEC PG_ScrollArea : public PG_Widget {
public:

class SignalAreaChangedHeight : public PG_Signal2<PG_ScrollArea*, Uint16> {}
	;
class SignalAreaChangedWidth : public PG_Signal2<PG_ScrollArea*, Uint16> {}
	;

	/**
	Create a scrollarea widget.
	@param parent parent widget if the scrollarea should be within the client
	context of an other widget, or NULL if this widget has no parent.
	@param r position and dimensions of the widget
	*/
	PG_ScrollArea(PG_Widget* parent, const PG_Rect& r = PG_Rect::null);

	~PG_ScrollArea();

	/**
	Shift widgets on removal.
	@param shiftx	shift all widgets to the right of the removed widgets.
	@param shifty	shift all widgets beneath the removed one.
	This method controls the behaviour if a widget will be removed from the client
	context.
	*/
	void SetShiftOnRemove(bool shiftx, bool shifty);

	/**
	scroll to a give X/Y-Coordinate within the client area.
	@param x X-Position
	@param y Y-Position
	*/
	void ScrollTo(int x, int y);

	/**
	Scroll to a widget
	@param widget the target widget
	@param bVertical scroll direction
	*/
	void ScrollToWidget(PG_Widget* widget, bool bVertical = true);

	/**
	Set the width of the scrollable area manually.
	@param w new width of the scrollable area.
	This method overrides the automatically computed width of the scrollable area.
	*/
	void SetAreaWidth(Uint16 w);

	/**
	Set the height of the scrollable area manually.
	@param h new height of the scrollable area.
	This method overrides the automatically computed height of the scrollable area.
	*/
	void SetAreaHeight(Uint16 h);

	/**
	Get the width of the scrollable area.
	@return width (in pixels) of the scrollable area
	*/
	inline Uint16 GetAreaWidth() {
		return my_area.w;
	};

	/**
	Get the height of the scrollable area.
	@return width (in pixels) of the scrollable area
	*/
	inline Uint16 GetAreaHeight() {
		return my_area.h;
	};

	/**
	Remove all widgets from the list (without deletion)
	*/
	void RemoveAll();

	/**
	Delete (destroy) all widgets in the list
	*/
	void DeleteAll();

	/**
	Get the number of widgets in the list
	*/
	Uint16 GetWidgetCount();

	Uint16 GetScrollPosX();

	Uint16 GetScrollPosY();

	SignalAreaChangedHeight sigAreaChangedHeight;

	SignalAreaChangedWidth sigAreaChangedWidth;

	void AddChild(PG_Widget* child);

	bool RemoveChild(PG_Widget* child);

	PG_Widget* GetFirstInList();

	/**
	Automatically adjusts the parent's size to the actual scroll area size;
	@param bRemove adjusts size when removing a child
	@param bAdd adjusts size when adding a child
	*/
	void SetResizeParent(bool bRemove, bool bAdd);

protected:

	void eventSizeWidget(Uint16 w, Uint16 h);

	PG_Rect my_area;

	bool my_shiftx;

	bool my_shifty;

	bool my_AddResizeParent;

	bool my_RemoveResizeParent;
};

#endif // PG_SCROLLAREA_H
