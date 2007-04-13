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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgwidgetlist.h,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

/** \file pgwidgetlist.h
	Header file for the PG_WidgetList class.
*/

#ifndef PG_WIDGETLIST_H
#define PG_WIDGETLIST_H

#include "pgscrollwidget.h"

/**
	@author Alexander Pipelka
 
	@short A group of widgets arranged in a list.
	Generally used to make a large 'pane' of widgets that can be scrolled
	through in a smaller 'portal' with scrollbars.
*/

class DECLSPEC PG_WidgetList : public PG_ScrollWidget  {
public:

	/**
	Constructor of the PG_Widget class
	*/
	PG_WidgetList(PG_Widget* parent, const PG_Rect& r = PG_Rect::null, const std::string& style="WidgetList");

	/**
	Destructor of the PG_Widget class
	*/
	~PG_WidgetList();

	/**
	Find a widget by a given index

	@param	index		index of the widget to find
	@return					pointer to the widget
	*/
	PG_Widget* FindWidget(int index);

	/**
	Find the index of a given widget

	@param	widget		pointer to the widget
	@return				index of the widget
	*/
	int FindIndex(PG_Widget* widget);

	/**
	Scroll to the specified Y-Position
	@param ypos new Y-Position
	Will scroll to the new position and update the scrollbars.
	*/
	void ScrollTo(Uint16 ypos);

	/**
	Scroll one page up
	*/
	void PageUp();

	/**
	Scroll one page down
	*/
	void PageDown();

	void AddChild(PG_Widget* child);

protected:

	/**
	Search for a widget at a given y-position

	@param	y			the position
	@return				pointer to the widget or NULL
	*/
	PG_Widget* GetWidgetFromPos(Sint32 y);

private:

	PG_WidgetList(const PG_WidgetList&);

	PG_WidgetList& operator=(const PG_WidgetList&);

};

#endif // PG_WIDGETLIST_H
