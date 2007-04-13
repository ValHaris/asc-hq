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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgtheme.h,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

/** \file pgtheme.h
	Header file for the PG_Theme interface class.
*/

#ifndef PG_THEME_H
#define PG_THEME_H

#include "paragui.h"
#include "pglabel.h"
#include "pgfont.h"
#include "pgdraw.h"

/**
	@short Theme/style definition class
*/

class DECLSPEC PG_Theme {
public:

	virtual ~PG_Theme() {}
	;

	virtual const std::string& FindDefaultFontName() = 0;
	virtual int FindDefaultFontSize() = 0;
	virtual PG_Font::Style FindDefaultFontStyle() = 0;
	virtual const std::string& FindFontName(const std::string& widgettype, const std::string& objectname) = 0;
	virtual int FindFontSize(const std::string& widgettype, const std::string& objectname) = 0;
	virtual PG_Font::Style FindFontStyle(const std::string& widgettype, const std::string& objectname) = 0;
	virtual SDL_Surface* FindSurface(const std::string& widgettype, const std::string& object, const std::string& name) = 0;
	virtual PG_Gradient* FindGradient(const std::string& widgettype, const std::string& object, const std::string& name) = 0;
	virtual void GetProperty(const std::string& widgettype, const std::string& object, const std::string& name, long& prop) = 0;
	virtual void GetProperty(const std::string& widgettype, const std::string& object, const std::string& name, Uint8& prop) = 0;
	virtual void GetProperty(const std::string& widgettype, const std::string& object, const std::string& name, bool& prop) = 0;
	virtual void GetProperty(const std::string& widgettype, const std::string& object, const std::string& name, int& prop) = 0;
	inline void GetProperty(const std::string& widgettype, const std::string& object, const std::string& name, Uint16& prop) {
		long b=-1;
		GetProperty(widgettype, object, name, b);
		if(b == -1) {
			return;
		}
		prop = (Uint16)b;
	}
	virtual void GetProperty(const std::string& widgettype, const std::string& object, const std::string& name, PG_Draw::BkMode& prop) = 0;
	virtual void GetAlignment(const std::string& widgettype, const std::string& object, const std::string& name, PG_Label::TextAlign& align) = 0;
	virtual void GetColor(const std::string& widgettype, const std::string& object, const std::string& name, PG_Color& color) = 0;
	virtual const std::string& FindString(const std::string& widgettype, const std::string& object, const std::string& name) = 0;

	/**
	Load and process a XML theme definition file.

	@param xmltheme The file name for the theme. The extension ".theme" is assumed.
	@return pointer to a newly created PG_Theme object or NULL if the operation failed

	Given the name of a file that contains the theme definition, it loads
	the Theme into ParaGUI's system
	*/
	static PG_Theme* Load(const std::string& xmltheme);

	/**
	*/
	static void Unload(PG_Theme* theme);
};

#endif // PG_THEME_H
