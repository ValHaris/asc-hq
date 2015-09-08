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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgslider.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

/** @file pgslider.h
	Header file for the PG_Slider class.
*/

#ifndef PG_SLIDER_H
#define PG_SLIDER_H

#include "paragui.h"
#include "pgscrollbar.h"

/** @class PG_Slider
	@author Alexander Pipelka
 
	@image html pgslider_horz.png "horizontal slider screenshot"
	@image html pgslider_vert.png "vertical slider screenshot"
 
	@short Provides a slider.
 
	Usually used to get user input for a value where the range is known and fixed. 
	The position of the slider is tied to an interger value which changes as the user 
	moves the slider.
*/

class DECLSPEC PG_Slider : public PG_ScrollBar {

public:

	/**
	Signal type declaration
	**/
class SignalSlide : public sigc::signal<bool, long> {}
	;

class SignalSlideEnd : public sigc::signal<bool, long> {}
	;

	/**
	 Contructor of the PG_Slider class
	 @param parent	pointer to the parent widget or NULL
	 @param r	position of the PG_Image widget
	 @param direction	of the widget HORIZONTAL or VERTICAL
	 @param id	identification number
	 @param style		widgetstyle to use
	 This constructor creates the slider widget.
	 * */

	PG_Slider(PG_Widget* parent, const PG_Rect& r = PG_Rect::null, ScrollDirection direction = VERTICAL, int id = -1, const std::string& style="Slider");

	/** */
	~PG_Slider();

	void LoadThemeStyle(const std::string& widgettype);

	SignalSlide sigSlide;

	SignalSlideEnd sigSlideEnd;

	void RecalcPositions();

protected:

	/** */
	bool eventMouseButtonUp(const SDL_MouseButtonEvent* button);

};

#endif	// PG_SLIDER_H
