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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgmaskedit.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

/** \file pgmaskedit.h
	Header file for the PG_MaskEdit class.
*/

#ifndef PG_MASKEDIT_H
#define PG_MASKEDIT_H

#include "pglineedit.h"
#include "pgstring.h"


class DECLSPEC PG_MaskEdit : public PG_LineEdit {
public:

	/** */
	PG_MaskEdit(PG_Widget* parent, const PG_Rect& r = PG_Rect::null, const std::string& style="LineEdit");

	/**
	Set the text of the maskedit control. The new text is merged with the current mask
	@param	new_text		pointer to new text string
	*/
	void SetText(const std::string& new_text);

	/**
	Set the valid input mask
	@param	mask			input mask (e.g. ##.##.####)
	*/
	void SetMask(const std::string& mask);

	/**
	Get current input mask
	@return						input mask
	*/
	const PG_String& GetMask();

	/**
	Set the "spacer" character. This char is displayed instead of "#"
	@param	c					"spacer" character
	*/
	void SetSpacer(const PG_Char& c);

	/**
	Get the current "spacer" character.
	@return						"spacer" character
	*/
	PG_Char GetSpacer();

protected:

	bool eventMouseButtonDown(const SDL_MouseButtonEvent* button);

	void InsertChar(const PG_Char& c);

	void DeleteChar(Uint16 pos);

private:

	PG_String my_mask;

	PG_String my_displaymask;

	PG_Char my_spacer;

};

#endif	// PG_MASKEDIT_H
