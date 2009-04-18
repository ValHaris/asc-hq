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
    Update Date:      $Date: 2009-04-18 13:48:39 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgrectlist.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

/** \file pgrectlist.h
	Header file for the PG_RectList class.
*/

#ifndef PG_RECTLIST_H
#define PG_RECTLIST_H

#include "paragui.h"
#include "pgrect.h"

class PG_Widget;

/**
	@author Alexander Pipelka
	@short A list derived from vector to handle overlapping and child-widgets
*/

class DECLSPEC PG_RectList {

public:

	/**
		constructor of the list
	*/
	PG_RectList();

	/**
		destructor
	*/
	virtual ~PG_RectList();

	/**
		add a widget to the list
		@param rect pointer to the widget (rect)
		@param front insert the rectangle at the head of the list if true
		This functions adds the widget to the back of the list.
	*/
	void Add(PG_Widget* rect, bool front = false);

	/**
		remove a widget from the list
		@param	rect pointer to the widget (rect)
		@return	true - if the widget was remove successfully
	 
		This functions removes the given widget from the list
	*/
	bool Remove(PG_Rect* rect);

	/**
		check if a given point is inside any rectangle in the list
		@param p	point to check
		@return		pointer to the first widget that contains the point / NULL if there is no match
	 
		Returns the first visible widget which contains the given point.
	*/
	PG_Widget* IsInside(const PG_Point& p);

	PG_Widget* Find(int id, bool recursive = false );

	PG_Widget* Find(const std::string& name, bool recursive = false );

	/**
		blit all rectangles in the list to the screen
	 
		This function blits all visible rectangles from the first to the last position to the screen.
	*/
	void Blit();

	/**
		blit all rectangles from the list intersecting a reference rectangle to the screen
		@param	rect	reference rectangle
	 
		Performs an intersection of all visible rectangles in the list with the reference rectangle.
		The resulting list is clipped to the reference and blitted to the screen.
	*/
	void Blit(const PG_Rect& rect);

	void Blit(const PG_Rect& rect, PG_Widget* first, PG_Widget* last = NULL);

	/**
		reorder a widget (rectangle) - front
		@param rect	widget to reorder
		@return	true - the rectangle was found and reordered / false - the rectangle isn't in the list.
	 
		Bring the given widget (rectangle) to the front (will overlap all other widgets in the list).
	*/
	bool BringToFront(PG_Widget* rect);

	/**
		reorder a widget (rectangle) - back
		@param rect	widget to reorder
		@return	true - the rectangle was found and reordered / false - the rectangle isn't in the list.
	 
		Send the given widget (rectangle) to the back (will be overlapped by all other widgets in the list).
	*/
	bool SendToBack(PG_Widget* rect);

	inline PG_Widget* first() {
		return my_first;
	}

	inline PG_Widget* last() {
		return my_last;
	}

	void clear();

	inline Uint32 size() {
		return my_count;
	}

protected:

	PG_Widget* my_first;

	PG_Widget* my_last;

	Uint32 my_count;
};

#endif	// PG_RECTLIST_H
