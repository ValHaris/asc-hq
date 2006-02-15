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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pglabel.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

#include "paragui.h"
#include "pglabel.h"
#include "pgapplication.h"
#include "pgtheme.h"
#include "pglog.h"

#include "propstrings_priv.h"

PG_Label::PG_Label(PG_Widget* parent, const PG_Rect& r, const std::string& text, const std::string& style) :
		PG_Widget(parent, r),
my_indent(0) {

	my_alignment = LEFT;
	my_srfIcon = NULL;
	my_freeicon = false;

	SetText(text);
	LoadThemeStyle(style);
}

PG_Label::~PG_Label() {
	if(my_freeicon) {
		PG_Application::UnloadSurface(my_srfIcon); // false
	}
}

void PG_Label::LoadThemeStyle(const std::string& style) {
	if(style != PG_PropStr::Label) {
		PG_Label::LoadThemeStyle(PG_PropStr::Label, PG_PropStr::Label);
	}
	PG_Label::LoadThemeStyle(style, PG_PropStr::Label);
}

void PG_Label::LoadThemeStyle(const std::string& widgettype, const std::string& object) {
	PG_Theme* t = PG_Application::GetTheme();

	const std::string& s = t->FindString(widgettype, object, PG_PropStr::label);

	if(!s.empty()) {
		SetText(s);
	}

	t->GetAlignment(widgettype, object, PG_PropStr::alignment, my_alignment);

	PG_Widget::LoadThemeStyle(widgettype, object);
}

void PG_Label::eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst) {
	PG_Rect my_rectLabel;
	int xshift = my_indent;

	// should we blit an icon ?
	if(my_srfIcon != NULL) {
		xshift = my_srfIcon->w + my_indent;

		PG_Rect icon_rect(
		    my_xpos + my_indent,
		    my_ypos + (my_height - my_srfIcon->h)/2,
		    my_srfIcon->w,
		    my_srfIcon->h);

		PG_Rect icon_src;
		PG_Rect icon_dst;

		GetClipRects(icon_src, icon_dst, icon_rect);

		PG_Widget::eventBlit(my_srfIcon, icon_src, icon_dst);
	}

	Uint16 wl;
	Uint16 hl;

	GetTextSize(wl, hl);

	switch (my_alignment) {
		case LEFT:
			my_rectLabel.my_xpos = xshift;
			my_rectLabel.my_ypos = (my_height - hl) >> 1;
			break;

		case RIGHT:
			my_rectLabel.my_xpos = (my_width - wl);
			my_rectLabel.my_ypos = (my_height - hl) >> 1;
			break;

		case CENTER:
			my_rectLabel.my_xpos = (my_width - wl) >> 1;
			my_rectLabel.my_ypos = (my_height - hl) >> 1;
			break;
	}

	DrawText(my_rectLabel, my_text);
}

/**  */
void PG_Label::eventDraw(SDL_Surface* surface, const PG_Rect& rect) {}

void PG_Label::SetAlignment(TextAlign a) {
	my_alignment = a;
	Update();
}

SDL_Surface* PG_Label::SetIcon(SDL_Surface* icon) {

	if((icon != my_srfIcon)  && my_freeicon) {
		PG_Application::UnloadSurface(my_srfIcon);
	}

	my_srfIcon = icon;
	my_freeicon = false;
	Update();

	return my_srfIcon;
}

SDL_Surface* PG_Label::SetIcon(const std::string& filename) {
	if(my_freeicon) {
		PG_Application::UnloadSurface(my_srfIcon);
	}

	my_srfIcon = PG_Application::LoadSurface(filename);
	my_freeicon = true;
	Update();

	return my_srfIcon;
}

SDL_Surface* PG_Label::GetIcon() {
	return my_srfIcon;
}

void PG_Label::SetIndent(Uint16 indent) {
	my_indent = indent;
}

Uint16 PG_Label::GetIndent() {
	return my_indent;
}

void PG_Label::SetSizeByText(int Width, int Height, const std::string& Text) {
	if (GetIcon() == NULL) {
		PG_Widget::SetSizeByText(Width, Height, Text);
		return;
	}

	Uint16 w,h;
	int baselineY;

	if(Text.empty()) {
		if (!PG_FontEngine::GetTextSize(my_text, GetFont(), &w, &h, &baselineY)) {
			return;
		}
	} else {
		PG_String ytext = Text;
		if (!PG_FontEngine::GetTextSize(ytext, GetFont(), &w, &h, &baselineY)) {
			return;
		}
	}


	if (GetIcon()->w > w) {
		my_width = GetIcon()->w + my_indent + Width;
	} else {
		my_width = w + GetIcon()->w + my_indent + Width;
	}
	my_height = PG_MAX(GetIcon()->h, h + baselineY) + Height + baselineY;
}
