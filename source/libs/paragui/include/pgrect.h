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
    Update Date:      $Date: 2007-02-10 19:37:12 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgrect.h,v $
    CVS/RCS Revision: $Revision: 1.1.2.4 $
    Status:           $State: Exp $
*/

/** \file pgrect.h
	Header file for the PG_Rect class.
*/

#ifndef PG_RECT_H
#define PG_RECT_H

#include "pgpoint.h"

class PG_Widget;

/**
	@author Alexander Pipelka
	@short Encapsulation of the SDL_Rect structure
*/

class DECLSPEC PG_Rect : public SDL_Rect {
public:

	/**
	Create a new PG_Rect object with give values
	@param	x	x-startposition
	@param	y	y-startposition
	@param	w	width
	@param	h	height

	Initializes the created PG_Rect object with x/y postions and width/height
	*/
	PG_Rect(Sint16 x = 0, Sint16 y = 0, Uint16 w = 0, Uint16 h = 0);

	/**
	Create a new PG_Rect from a reference
	@param	src	reference rectangle
	 
	Initializes the created PG_Rect object with the position of the reference rectangle
	*/
	PG_Rect(const PG_Rect& src);

	/**
	Create a new PG_Rect from a SDL_Rect structure
	@param	src	source SDL_Rect structure
	 
	Initializes the created PG_Rect object with the position of the SDL_Rect structure
	*/
	PG_Rect(const SDL_Rect& src);

	virtual ~PG_Rect();

	/**
	Set the position of the rectangle
	@param	nx	x-startposition
	@param	ny	y-startposition
	@param	nw	width
	@param	nh	height
	Moves the rectangle to the given values
	*/
	inline void SetRect(Sint16 nx, Sint16 ny, Uint16 nw, Uint16 nh) {
		x = nx;
		y = ny;
		w = nw;
		h = nh;
	}

	/**
	*/
	PG_Rect& operator =(const SDL_Rect& src);

	PG_Rect& operator =(const PG_Rect& src);

	/**
	*/
	PG_Rect operator / ( const PG_Rect& b) const ;

	inline bool operator ==(const PG_Rect& r) const {
		return (x == r.x) && (y == r.y) && (r.w == w) && (r.h == h);
	}

	inline bool operator !=(const PG_Rect& r) const {
		return !((x == r.x) && (y == r.y) && (r.w == w) && (r.h == h));
	}
	/**
	Check if a given point is inside a rectangle (boxtest)
	 
	@param	p		the point to test
	@param	r		the rectangle the point should be inside
	@return			true if the point is inside the rectangle
	*/
	inline static bool IsInside(const PG_Point& p, PG_Rect& r) {
		return r.IsInside(p);
	}

	/**
	Check if a given point is inside a rectangle (boxtest)
	 
	@param	p		the point to test
	@return			true if the point is inside the rectangle
	*/
	inline bool IsInside(const PG_Point& p) const {
		return ( (x <= p.x) && (p.x <= x + w) && (y <= p.y) && (p.y <= y + h) );
	}

	/**
	Intersect two rectangles
	@param	p					reference rectangle
	@param	c					rectangle to intersect with reference
	@return						resulting intersection rectangle
	*/
	static PG_Rect IntersectRect(const PG_Rect& p, const PG_Rect& c);

	/**
	Intersect two rectangles
	@param	p					rectangle to intersect with
	@return						resulting intersection rectangle
	*/
	PG_Rect IntersectRect(const PG_Rect& p) const;

	/**
	Return the width of the rectangle
	@return			width
	*/
	inline Uint16 Width() const {
		return w;
	}

	/**
	Return the height of the rectangle
	@return			height
	*/
	inline Uint16 Height() const {
		return h;
	}

	inline bool IsNull() const {
		return (!Width() && !Height());
	}

	//! Check if two rectangles overlap
	/*!
	\param p, c rectangles to check for overlap
	\return true if the rectangles overlap, false otherwise
	*/
	inline bool OverlapRect(const PG_Rect& p, const PG_Rect& c) const {
		return !( (p.x + p.w < c.x) || (p.x > c.x + c.w) || (p.y + p.h < c.y) || (p.y > c.y + c.h)  || (p.IntersectRect(c).IsNull()) );
	}

	//! Check if this rectangle overlap another one
	/*!
	\param p rectangle to check for overlap
	\return true if this rectangle an p overlap, false otherwise
	*/
	inline bool OverlapRect(const PG_Rect& p) const {
		return OverlapRect(p, *this);
	}

	//! Check if this rectangle overlap another one
	/*!
	\param p pointer to rectangle to check for overlap
	\return true if this rectangle an p overlap, false otherwise
	*/
	inline bool OverlapRect(PG_Rect* p) {
		return OverlapRect(*p, *this);
	}

	/**
	Get the next Rectangle from the list
	Moves to the next rectangle in the list
	*/
	inline PG_Widget* next() {
		return my_next;
	}

	/**
	Get the previous Rectangle from the list
	Moves to the previous rectangle in the list
	*/
	inline PG_Widget* prev() {
		return my_prev;
	}

	Sint16& my_xpos;

	Sint16& my_ypos;

	Uint16& my_width;

	Uint16& my_height;

	Uint32 index;

	static PG_Rect null;

protected:

	PG_Widget* my_next;

	PG_Widget* my_prev;

	friend class PG_RectList;
};

#endif	// PG_RECT_H
