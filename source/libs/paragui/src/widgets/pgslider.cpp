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
    Update Date:      $Date: 2009-04-18 13:48:40 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgslider.cpp,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

#include "pgslider.h"
#include "pgapplication.h"
#include "pgtheme.h"

PG_Slider::PG_Slider(PG_Widget* parent, const PG_Rect& r, ScrollDirection direction, int id, const std::string& style) : PG_ScrollBar(parent, r, direction, id) {
	delete scrollbutton[0];
	scrollbutton[0] = NULL;

	delete scrollbutton[1];
	scrollbutton[1] = NULL;

	// connect signals
	sigScrollPos.connect(sigSlideEnd.make_slot());
	sigScrollTrack.connect(sigSlide.make_slot());

	LoadThemeStyle(style);
	SetPosition(scroll_min);
}

PG_Slider::~PG_Slider() {}

void PG_Slider::LoadThemeStyle(const std::string& widgettype) {
	PG_Theme* t = PG_Application::GetTheme();

	dragbutton->LoadThemeStyle(widgettype, "SliderDrag");

	if(sb_direction == VERTICAL) {
		Uint16 h = dragbutton->h;
		t->GetProperty(widgettype, "SliderDragV", "height", h);
		dragbutton->LoadThemeStyle(widgettype, "SliderDragV");
		dragbutton->SizeWidget(dragbutton->w, h);
		PG_ThemeWidget::LoadThemeStyle(widgettype, "SliderV");
	} else {
		Uint16 w = dragbutton->w;
		t->GetProperty(widgettype, "SliderDragH", "width", w);
		dragbutton->LoadThemeStyle(widgettype, "SliderDragH");
		dragbutton->SizeWidget(w, dragbutton->h);
		PG_ThemeWidget::LoadThemeStyle(widgettype, "SliderH");
	}

	RecalcPositions();
}

void PG_Slider::RecalcPositions() {
	position[0] = PG_Rect::null;
	position[1] = PG_Rect::null;

	position[2].x = 0;
	position[2].y = 0;
	position[2].w = w;
	position[2].h = h;

	if(sb_direction == VERTICAL) {
		position[3].x = 0;
		position[3].w = w;
		position[3].h = dragbutton->h;

		if((scroll_max - scroll_min) == 0) {
			position[3].y = position[2].y;
		} else {
			position[3].y = ((position[2].h - position[3].h) / (scroll_max - scroll_min)) * scroll_current;
		}
	} else {
		position[3].y = 0;
		position[3].w = dragbutton->w;
		position[3].h = h;

		if((scroll_max - scroll_min) == 0) {
			position[3].x = position[2].x;
		} else {
			position[3].x = ((position[2].w - position[3].w) / (scroll_max - scroll_min)) * scroll_current;
		}
	}

	int pos = 	scroll_current - scroll_min;

	if(sb_direction == VERTICAL) {
		position[3].x = 0;
		position[3].h = (Uint16)((double)position[2].h / ((double)position[2].h / (double)position[3].h));
		position[3].y = (Uint16)(position[0].h + (((double)position[2].h - (double)position[3].h) / (double)(scroll_max - scroll_min)) * (double)pos);
	} else {
		position[3].y = 0;
		position[3].w = (Uint16)((double)position[2].w / ((double)position[2].w / (double)position[3].w) );
		position[3].x = (Uint16)(position[0].w + (((double)position[2].w - (double)position[3].w) / (double)(scroll_max - scroll_min)) * (double)pos);
	}

	// bordersize
	for(int i=2; i<4; i++) {
		if(i == 3 || i == 2) {
			if(sb_direction == VERTICAL) {
				position[i].x += my_bordersize;
				if(position[i].w > 2*my_bordersize) {
					position[i].w -= 2*my_bordersize;
				}
			} else {
				position[i].y += my_bordersize;
				if(position[i].h > 2*my_bordersize) {
					position[i].h -= 2*my_bordersize;
				}
			}
			continue;
		}
		position[i].x += my_bordersize;
		position[i].y += my_bordersize;
		if(position[i].w > 2*my_bordersize) {
			position[i].w -= 2*my_bordersize;
		}
		if(position[i].h > 2*my_bordersize) {
			position[i].h -= 2*my_bordersize;
		}
	}
	if(scrollbutton[0] != NULL) {
		scrollbutton[0]->MoveWidget(position[0]);
	}
	if(scrollbutton[1] != NULL) {
		scrollbutton[1]->MoveWidget(position[1]);
	}
	dragbutton->MoveWidget(position[3]);
}

bool PG_Slider::eventMouseButtonUp(const SDL_MouseButtonEvent* button) {

	switch(button->button) {
		case 4:
			if(scroll_current <= scroll_min + my_linesize) {
				SetPosition(scroll_min);
			} else {
				SetPosition(scroll_current - my_linesize);
			}
			break;

		case 5:
			SetPosition(scroll_current + my_linesize);
			break;
	}

	sigSlideEnd(this, scroll_current);

	return true;
}
