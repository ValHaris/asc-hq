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
    Update Date:      $Date: 2007-04-13 16:16:04 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgwidgetlist.cpp,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

#include "pgwidgetlist.h"
#include "pgapplication.h"
#include "pgscrollarea.h"
#include "pglog.h"


PG_WidgetList::PG_WidgetList(PG_Widget* parent, const PG_Rect& r, const std::string& style) : PG_ScrollWidget(parent, r, style) {
	my_scrollarea->SetShiftOnRemove(false, true);

	if(style != "WidgetList") {
		LoadThemeStyle("WidgetList");
	}
	LoadThemeStyle(style);
}

PG_WidgetList::~PG_WidgetList() {}

void PG_WidgetList::AddChild(PG_Widget* w) {
	if(w == NULL) {
		return;
	}

	if (my_objVerticalScrollbar == NULL || my_objHorizontalScrollbar == NULL || my_scrollarea == NULL) {
		PG_Widget::AddChild(w);
		return;
	}

	w->MoveRect(0, w->my_ypos + my_scrollarea->GetAreaHeight());
	my_scrollarea->AddChild(w);
}

PG_Widget* PG_WidgetList::GetWidgetFromPos(Sint32 y) {
	Uint32 dy = 0;
	Uint32 min_dy = 100000000;

	PG_Widget* result = NULL;

	PG_Widget* list = GetChildList()->first();
	for( ; list != NULL; list = list->next()) {
		dy = abs(0- (list->my_ypos - my_ypos));

		if(dy < min_dy) {
			min_dy = dy;
			result = list;
		}
	}

	return result;
}

PG_Widget* PG_WidgetList::FindWidget(int index) {

	if((index < 0) || (index >= GetWidgetCount())) {
		return NULL;
	}

	int i = 0;
	PG_Widget* list = my_scrollarea->GetChildList()->first();
	for( ; list != NULL; list = list->next()) {
		if(i == index) {
			return list;
		}
		i++;
	}

	return NULL;
}

int PG_WidgetList::FindIndex(PG_Widget* w) {
	int index = 0;

	PG_Widget* list = GetChildList()->first();
	for( ; list != NULL; list = list->next()) {
		if(list == w) {
			return index;
		}
		index++;
	}

	return -1;
}

void PG_WidgetList::ScrollTo(Uint16 ypos) {
	my_scrollarea->ScrollTo(my_scrollarea->GetScrollPosX(), ypos);
	CheckScrollBars();
}

void PG_WidgetList::PageUp() {
	my_scrollarea->ScrollTo(my_scrollarea->GetScrollPosX(), my_scrollarea->GetScrollPosY() - my_height );
	CheckScrollBars();
}

void PG_WidgetList::PageDown() {
	my_scrollarea->ScrollTo(my_scrollarea->GetScrollPosX(), my_scrollarea->GetScrollPosY() + my_height );
	CheckScrollBars();
}
