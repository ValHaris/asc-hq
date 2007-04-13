/*
    ParaGUI - crossplatform widgetset
    Copyright (C) 2000,2001  Alexander Pipelka
 
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
    Update Date:      $Date: 2007-04-13 16:16:02 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/themes/theme_priv.h,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

#ifndef THEME_PRIV_H
#define THEME_PRIV_H

#include "paragui.h"
#include "pgcolor.h"

#include <string>
#include <vector>

#include "pgfilearchive.h"
#include "pgtheme.h"
#include "pgfont.h"

#ifdef HASH_MAP_INC
#include HASH_MAP_INC
#else
#include <map>
#endif

class THEME_FONT {
public:
	THEME_FONT() {
		size = 14;
		index = 0;
		style = PG_Font::NORMAL;
	}
	std::string name;
	std::string value;
	int size;
	int index;
	PG_Font::Style style;
};

class THEME_STRING {
public:
	virtual ~THEME_STRING() {}

	std::string name;
	std::string value;
};

class THEME_FILENAME {
public:
	THEME_FILENAME() {
		surface = NULL;
	}

	virtual ~THEME_FILENAME() {
		PG_FileArchive::UnloadSurface(surface);
	}

	std::string name;
	std::string value;
	Uint32 colorkey;
	bool hasColorKey;
	SDL_Surface* surface;
};

class THEME_PROPERTY {
public:
	std::string name;
	long value;
};

class THEME_GRADIENT : public PG_Gradient {
public:
	std::string name;
};

#ifdef HASH_MAP_INC
// Why go through the extra job of creating strings for comparision instead
// of using char * directly?
struct pg_hashstr {
	size_t operator()(const std::string& s1) const {
#if PG_VERSIONNUM(__GNUC__, __GNUC_MINOR__, 0) >= PG_VERSIONNUM(3, 1, 0)
		return __gnu_cxx::hash<const char *>()(s1.c_str());
#else

		return hash<const char *>()(s1.c_str());
#endif

	}
};
#endif

class THEME_OBJECT {
public:

	THEME_OBJECT();
	virtual ~THEME_OBJECT();

	SDL_Surface* FindSurface(const std::string& name);
	PG_Gradient* FindGradient(const std::string& name);
	long FindProperty(const std::string& name);
	const std::string& FindString(const std::string& name);
	THEME_FONT* font;

	std::string type;
	std::string name;

	std::vector<THEME_STRING*> strings;

#ifdef HASH_MAP_INC

	typedef STL_MAP<std::string, THEME_FILENAME*, pg_hashstr> MAP_FILENAME;
	typedef STL_MAP<std::string, THEME_GRADIENT*, pg_hashstr> MAP_GRADIENT;
	typedef STL_MAP<std::string, THEME_PROPERTY*, pg_hashstr> MAP_PROPERTY;
#else

	typedef std::map<std::string, THEME_FILENAME*> MAP_FILENAME;
	typedef std::map<std::string, THEME_GRADIENT*> MAP_GRADIENT;
	typedef std::map<std::string, THEME_PROPERTY*> MAP_PROPERTY;
#endif

	MAP_FILENAME filename;
	MAP_GRADIENT gradient;
	MAP_PROPERTY property;
};

class THEME_WIDGET {
public:

	THEME_WIDGET() {}
	;
	virtual ~THEME_WIDGET();

	THEME_OBJECT* FindObject(const std::string& objectname);

	std::string type;

#ifdef HASH_MAP_INC

	typedef STL_MAP<std::string, THEME_OBJECT*, pg_hashstr> MAP_OBJECT;
#else

	typedef std::map<std::string, THEME_OBJECT*> MAP_OBJECT;
#endif

	MAP_OBJECT object;
};

class THEME_THEME : public PG_Theme {
public:

	THEME_THEME() {
		defaultfont = NULL;
	};

	virtual ~THEME_THEME();

	const std::string& FindDefaultFontName();
	int FindDefaultFontSize();
	PG_Font::Style FindDefaultFontStyle();

	const std::string& FindFontName(const std::string&, const std::string&);
	int FindFontSize(const std::string&, const std::string&);
	PG_Font::Style FindFontStyle(const std::string&, const std::string&);

	SDL_Surface* FindSurface(const std::string& widgettype, const std::string& object, const std::string& name);
	PG_Gradient* FindGradient(const std::string& widgettype, const std::string& object, const std::string& name);
	void GetProperty(const std::string& widgettype, const std::string& object, const std::string& name, long& prop);
	void GetProperty(const std::string& widgettype, const std::string& object, const std::string& name, Uint8& prop);
	void GetProperty(const std::string& widgettype, const std::string& object, const std::string& name, bool& prop);
	void GetProperty(const std::string& widgettype, const std::string& object, const std::string& name, int& prop);
	void GetProperty(const std::string& widgettype, const std::string& object, const std::string& name, PG_Draw::BkMode& prop);
	void GetAlignment(const std::string& widgettype, const std::string& object, const std::string& name, PG_Label::TextAlign& align);
	void GetColor(const std::string& widgettype, const std::string& object, const std::string& name, PG_Color& color);
	const std::string& FindString(const std::string& widgettype, const std::string& object, const std::string& name);

	THEME_FONT* defaultfont;

	std::string title;
	std::string description;
	std::string author;
	std::string email;

#ifdef HASH_MAP_INC

	typedef STL_MAP<std::string, THEME_WIDGET*, pg_hashstr> MAP_WIDGET;
#else

	typedef std::map<std::string, THEME_WIDGET*> MAP_WIDGET;
#endif

	MAP_WIDGET widget;

	THEME_WIDGET* FindWidget(const std::string& widgettype);
	THEME_OBJECT* FindObject(const std::string& widgettype, const std::string& objectname);
};

#endif // THEME_PRIV_H
