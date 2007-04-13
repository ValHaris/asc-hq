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
    Update Date:      $Date: 2007-04-13 16:16:01 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/themes/theme_priv.cpp,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

#include "theme_priv.h"
#include "pgcolor.h"

THEME_THEME::~THEME_THEME() {
	// clean up
	for(MAP_WIDGET::iterator i = widget.begin(); i != widget.end(); i++) {
		delete (*i).second;
	}
	widget.clear();

	delete defaultfont;
}

THEME_WIDGET* THEME_THEME::FindWidget(const std::string& widgettype) {
	MAP_WIDGET::iterator i = widget.find(widgettype);
	if(i == widget.end()) {
		return NULL;
	}

	return (*i).second;
}

THEME_OBJECT* THEME_THEME::FindObject(const std::string& widgettype, const std::string& objectname) {
	THEME_WIDGET* widget = FindWidget(widgettype);

	if(widget == NULL) {
		return NULL;
	}

	THEME_OBJECT* object = widget->FindObject(objectname);

	return object;
}

SDL_Surface* THEME_THEME::FindSurface(const std::string& widgettype, const std::string& objectname, const std::string& name) {
	THEME_OBJECT* object = FindObject(widgettype, objectname);

	if(object == NULL) {
		return NULL;
	}

	return object->FindSurface(name);
}

PG_Gradient* THEME_THEME::FindGradient(const std::string& widgettype, const std::string& objectname, const std::string& name) {
	THEME_OBJECT* object = FindObject(widgettype, objectname);

	if(object == NULL) {
		return NULL;
	}

	return object->FindGradient(name);
}

void THEME_THEME::GetProperty(const std::string& widgettype, const std::string& objectname, const std::string& name, long& prop) {
	THEME_OBJECT* o = FindObject(widgettype, objectname);

	if(o == NULL) {
		return;
	}

	long n = o->FindProperty(name);
	if(n == -1) {
		return;
	}

	prop = n;
}

void THEME_THEME::GetProperty(const std::string& widgettype, const std::string& objectname, const std::string& name, Uint8& prop) {
	THEME_OBJECT* o = FindObject(widgettype, objectname);

	if(o == NULL) {
		return;
	}

	long n = o->FindProperty(name);
	if(n == -1) {
		return;
	}

	prop = (Uint8)n;
}

void THEME_THEME::GetProperty(const std::string& widgettype, const std::string& objectname, const std::string& name, bool& prop) {
	THEME_OBJECT* o = FindObject(widgettype, objectname);

	if(o == NULL) {
		return;
	}

	long n = o->FindProperty(name);
	if(n == -1) {
		return;
	}

	prop = (n == 1);
}

void THEME_THEME::GetProperty(const std::string& widgettype, const std::string& objectname, const std::string& name, int& prop) {
	THEME_OBJECT* o = FindObject(widgettype, objectname);

	if(o == NULL) {
		return;
	}

	long n = o->FindProperty(name);
	if(n == -1) {
		return;
	}

	prop = (int)n;
}

void THEME_THEME::GetProperty(const std::string& widgettype, const std::string& objectname, const std::string& name, PG_Draw::BkMode& prop) {
	THEME_OBJECT* o = FindObject(widgettype, objectname);

	if(o == NULL) {
		return;
	}

	long n = o->FindProperty(name);
	if(n == -1) {
		return;
	}

	prop = (PG_Draw::BkMode)n;
}

const std::string& THEME_THEME::FindString(const std::string& widgettype, const std::string& objectname, const std::string& name) {
	THEME_OBJECT* object = FindObject(widgettype, objectname);

	if(object == NULL) {
		return PG_NULLSTR;
	}

	return object->FindString(name);
}

const std::string& THEME_THEME::FindDefaultFontName() {
	return defaultfont->value;
}

int THEME_THEME::FindDefaultFontSize() {
	if(!defaultfont) {
		return 14;
	}

	return defaultfont->size;
}

PG_Font::Style THEME_THEME::FindDefaultFontStyle() {
	if(!defaultfont) {
		return PG_Font::NORMAL;
	}

	return defaultfont->style;
}

const std::string& THEME_THEME::FindFontName(const std::string& widgettype, const std::string& objectname) {
	THEME_OBJECT* o = FindObject(widgettype, objectname);

	if(o == NULL) {
		return PG_NULLSTR;
	}

	if(!o->font) {
		return PG_NULLSTR;
	}

	return o->font->value;
}

int THEME_THEME::FindFontSize(const std::string& widgettype, const std::string& objectname) {
	THEME_OBJECT* o = FindObject(widgettype, objectname);

	if(o == NULL) {
		return 0;
	}

	if(!o->font) {
		return 0;
	}

	return o->font->size;
}

PG_Font::Style THEME_THEME::FindFontStyle(const std::string& widgettype, const std::string& objectname) {
	THEME_OBJECT* o = FindObject(widgettype, objectname);

	if(o == NULL) {
		return PG_Font::NORMAL;
	}

	if(!o->font) {
		return PG_Font::NORMAL;
	}

	return o->font->style;
}

THEME_WIDGET::~THEME_WIDGET() {
	for(MAP_OBJECT::iterator i = object.begin(); i != object.end(); i++) {
		delete (*i).second;
	}
	object.clear();
}

inline THEME_OBJECT* THEME_WIDGET::FindObject(const std::string& objectname) {
	MAP_OBJECT::iterator i = object.find(objectname);
	if(i == object.end()) {
		return NULL;
	}

	return (*i).second;
}

THEME_OBJECT::THEME_OBJECT() {
	font = NULL;
}

THEME_OBJECT::~THEME_OBJECT() {
	for(MAP_FILENAME::iterator f = filename.begin(); f != filename.end(); f++) {
		delete (*f).second;
	}
	filename.clear();

	for(MAP_GRADIENT::iterator g = gradient.begin(); g != gradient.end(); g++) {
		delete (*g).second;
	}
	gradient.clear();

	for(MAP_PROPERTY::iterator p = property.begin(); p != property.end(); p++) {
		delete (*p).second;
	}
	property.clear();

	for(Uint32 i=0; i<strings.size(); i++) {
		delete strings[i];
		strings[i] = NULL;
	}
	strings.clear();

	delete font;
}

SDL_Surface* THEME_OBJECT::FindSurface(const std::string& name) {
	MAP_FILENAME::iterator result = filename.find(name);

	if(result == filename.end()) {
		return NULL;
	}

	return (*result).second->surface;
}

PG_Gradient* THEME_OBJECT::FindGradient(const std::string& name) {
	MAP_GRADIENT::iterator result = gradient.find(name);

	if(result == gradient.end()) {
		return NULL;
	}

	return static_cast<PG_Gradient*>((*result).second);
}

void THEME_THEME::GetAlignment(const std::string& widgettype, const std::string& object, const std::string& name, PG_Label::TextAlign& align) {
	long b = -1;
	GetProperty(widgettype, object, name, b);

	if(b == -1) {
		return;
	}

	switch(b) {
		case 0:
			align = PG_Label::LEFT;
			break;
		case 1:
			align = PG_Label::CENTER;
			break;
		case 2:
			align = PG_Label::RIGHT;
			break;
	}

	return;
}

void THEME_THEME::GetColor(const std::string& widgettype, const std::string& object, const std::string& name, PG_Color& color) {
	long b = -1;
	GetProperty(widgettype, object, name, b);

	if(b == -1) {
		return;
	}

	color = (Uint32)b;
}

long THEME_OBJECT::FindProperty(const std::string& name) {
	MAP_PROPERTY::iterator result = property.find(name);

	if(result == property.end()) {
		return -1;
	}

	return (*result).second->value;
}

const std::string& THEME_OBJECT::FindString(const std::string& name) {
	for(Uint32 i=0; i<strings.size(); i++) {
		if(strings[i]->name == name) {
			return strings[i]->value;
		}
	}

	return PG_NULLSTR;
}
