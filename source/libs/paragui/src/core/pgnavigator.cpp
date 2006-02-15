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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/core/pgnavigator.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

#include "pgnavigator.h"

#include <algorithm>
#include <cmath>

PG_Widget* PG_Navigator::my_currentWidget = NULL;

void PG_Navigator::Add(PG_Widget* widget) {
	iterator i = find(begin(), end(), widget);

	if(i != end()) {
		return;
	}

	push_back(widget);
}

void PG_Navigator::Remove(PG_Widget* widget) {
	iterator i = find(begin(), end(), widget);

	if(i == end()) {
		return;
	}

	erase(i);
}

bool PG_Navigator::Action(PG_Widget::KeyAction action) {
	if(my_currentWidget == NULL) {
		return false;
	}

	return my_currentWidget->Action(action);
}

PG_Widget* PG_Navigator::Goto(PG_Widget* widget) {
	iterator i = find(begin(), end(), widget);

	if(i == end()) {
		return NULL;
	}

	Action(PG_Widget::ACT_DEACTIVATE);
	my_currentWidget = widget;
	Action(PG_Widget::ACT_ACTIVATE);

	return my_currentWidget;
}

PG_Widget* PG_Navigator::GotoFirst() {
	iterator i = begin();

	if(i == end()) {
		return NULL;
	}

	return Goto(*i);
}

PG_Widget* PG_Navigator::GotoLast() {
	if(size() == 0) {
		return NULL;
	}

	if(size() == 1) {
		return GotoFirst();
	}

	iterator i = end();
	i--;

	return Goto(*i);
}

PG_Widget* PG_Navigator::GotoNext() {
	iterator i = find(begin(), end(), my_currentWidget);

	if(i == end()) {
		return NULL;
	}

	i++;
	return Goto(*i);
}

PG_Widget* PG_Navigator::GotoPrev() {
	iterator i = find(begin(), end(), my_currentWidget);

	if(i == begin()) {
		return NULL;
	}

	i--;
	return Goto(*i);
}

PG_Widget* PG_Navigator::FindWidget(PG_Widget* from, PG_Point ref, bool absx, bool absy, int xmode, int ymode) {
	if (!from)
		return NULL;

	int i_x = 0;
	int i_y = 0;
	int my_x = ref.x;
	int my_y = ref.y;
	PG_Widget* result = NULL;
	PG_Widget* parent = from->GetParent();
	int dy = 0;
	int dx = 0;
	double min_l = 100000;

	for(iterator i = begin(); i != end(); i++) {

		// check if we have the same parent-widget
		if((*i)->GetParent() != parent) {
			continue;
		}

		// discard myself and invisible objects
		if( ((*i) == from) || ((*i)->IsVisible() == false) ) {
			continue;
		}

		// get middle of the widget
		i_x = (*i)->x + (*i)->w / 2;
		i_y = (*i)->y + (*i)->h / 2;

		// delta x & y
		dy = absy ? abs(i_y - my_y) : (i_y - my_y);
		dx = absx ? abs(i_x - my_x) : (i_x - my_x);

		if((xmode != 0) && (dx*xmode < 0)) {
			continue;
		}

		if((ymode != 0) && (dy*ymode < 0)) {
			continue;
		}

		double l = sqrt((double)(dx*dx + dy*dy));
		if( ((xmode != 0) && (dx != 0)) || ((ymode != 0) && (dy != 0)) ) {
			if(l < min_l) {
				min_l = l;
				result = (*i);
			}
		}
	}

	return result;
}

PG_Widget* PG_Navigator::FindLeft(PG_Widget* widget) {
	if(widget == NULL) {
		widget = my_currentWidget;
	}

	PG_Point p;
	p.x = widget->x + widget->w / 2;
	p.y = widget->y + widget->h / 2;

	return FindWidget(widget, p, false, true, -1, 0);
}

PG_Widget* PG_Navigator::FindRight(PG_Widget* widget) {
	if(widget == NULL) {
		widget = my_currentWidget;
	}

	PG_Point p;
	p.x = widget->x + widget->w / 2;
	p.y = widget->y + widget->h / 2;

	return FindWidget(widget, p, false, true, +1, 0);
}

PG_Widget* PG_Navigator::FindUp(PG_Widget* widget) {
	if(widget == NULL) {
		widget = my_currentWidget;
	}

	PG_Point p;
	p.x = widget->x + widget->w / 2;
	p.y = widget->y + widget->h / 2;

	return FindWidget(widget, p, true, false, 0, -1);
}

PG_Widget* PG_Navigator::FindDown(PG_Widget* widget) {
	if(widget == NULL) {
		widget = my_currentWidget;
	}

	PG_Point p;
	p.x = widget->x + widget->w / 2;
	p.y = widget->y + widget->h / 2;

	return FindWidget(widget, p, true, false, 0, +1);
}
