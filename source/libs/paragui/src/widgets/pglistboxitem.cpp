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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pglistboxitem.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

#include "pglistboxitem.h"
#include "pglistbox.h"
#include "pgapplication.h"
#include "pgtheme.h"

#include "propstrings_priv.h"

PG_ListBoxItem::PG_ListBoxItem(PG_Widget* parent, int height, const std::string& text, SDL_Surface* icon, void* userdata, const std::string& style) : PG_ListBoxBaseItem(parent, height, userdata) {
	my_srfHover = NULL;
	my_srfSelected = NULL;

	my_srfIcon = icon;

	for(int i=0; i<3; i++) {
		my_background[i] = NULL;
		my_bkmode[i] = PG_Draw::TILE;
		my_blend[i] = 0;
		my_gradient[i] = NULL;
	}

	SetText(text);
	LoadThemeStyle(style, PG_PropStr::ListBoxItem);

}

PG_ListBoxItem::~PG_ListBoxItem() {
	PG_ThemeWidget::DeleteThemedSurface(my_srfHover);
	PG_ThemeWidget::DeleteThemedSurface(my_srfSelected);
}

void PG_ListBoxItem::eventSizeWidget(Uint16 w, Uint16 h) {
	PG_ThemeWidget::DeleteThemedSurface(my_srfHover);
	PG_ThemeWidget::DeleteThemedSurface(my_srfSelected);

	// reset surface pointers, will be regenerated on next blit
	my_srfHover = NULL;
	my_srfSelected = NULL;
}

void PG_ListBoxItem::eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst) {

	if((dst.my_width == 0) || (dst.my_height == 0)) {
		return;
	}

	if(my_srfHover == NULL) {
		my_srfHover = PG_ThemeWidget::CreateThemedSurface(
		                  PG_Rect(0, 0, my_width, my_height),
		                  my_gradient[2],
		                  my_background[2],
		                  my_bkmode[2],
		                  my_blend[2]);
	}

	if(my_srfSelected == NULL) {
		my_srfSelected = PG_ThemeWidget::CreateThemedSurface(
		                     PG_Rect(0, 0, my_width, my_height),
		                     my_gradient[1],
		                     my_background[1],
		                     my_bkmode[1],
		                     my_blend[1]);
	}

	if(my_selected) {
		PG_Widget::eventBlit(my_srfSelected, src, dst);
	} else if(my_hover) {
		PG_Widget::eventBlit(my_srfHover, src, dst);
	}

	PG_Label::eventBlit(NULL, src, dst);
}

void PG_ListBoxItem::LoadThemeStyle(const std::string& widgettype, const std::string& objectname) {
	PG_Theme* t = PG_Application::GetTheme();
	PG_Gradient* g = NULL;

	my_background[0] = t->FindSurface(widgettype, objectname, PG_PropStr::background0);
	my_background[1] = t->FindSurface(widgettype, objectname, PG_PropStr::background1);
	my_background[2] = t->FindSurface(widgettype, objectname, PG_PropStr::background2);

	t->GetProperty(widgettype, objectname, PG_PropStr::blend0, my_blend[0]);
	t->GetProperty(widgettype, objectname, PG_PropStr::blend1, my_blend[1]);
	t->GetProperty(widgettype, objectname, PG_PropStr::blend2, my_blend[2]);

	t->GetProperty(widgettype, objectname, PG_PropStr::backmode0, my_bkmode[0]);
	t->GetProperty(widgettype, objectname, PG_PropStr::backmode1, my_bkmode[1]);
	t->GetProperty(widgettype, objectname, PG_PropStr::backmode2, my_bkmode[2]);

	g = t->FindGradient(widgettype, objectname, PG_PropStr::gradient0);
	if(g)
		my_gradient[0] = g;
	g = t->FindGradient(widgettype, objectname, PG_PropStr::gradient1);
	if(g)
		my_gradient[1] = g;
	g = t->FindGradient(widgettype, objectname, PG_PropStr::gradient2);
	if(g)
		my_gradient[2] = g;

	PG_Color fontcolor(0xFFFFFF);
	t->GetColor(widgettype, objectname, PG_PropStr::textcolor, fontcolor);
	SetFontColor(fontcolor);
}
