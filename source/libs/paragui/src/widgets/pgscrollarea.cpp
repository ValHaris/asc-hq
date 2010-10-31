/*
    ParaGUI - crossplatform widgetset
    Copyright (C) 2000-2010  Alexander Pipelka
 
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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgscrollarea.cpp,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

#include <algorithm>

#include "pgscrollarea.h"
#include "pglog.h"
#include "pgapplication.h"

PG_ScrollArea::PG_ScrollArea(PG_Widget* parent, const PG_Rect& r) : PG_Widget(parent, r),
my_shiftx(false), my_shifty(false), my_AddResizeParent(false), my_RemoveResizeParent(false) {}

PG_ScrollArea::~PG_ScrollArea() {}

void PG_ScrollArea::ScrollTo( int x, int y) {
   if ( x < 0 )
      x = 0;

   if ( y < 0 )
      y = 0;


	if (my_area.x == x && my_area.y == y)
		return;

	if(y > my_area.h - my_height ) {  
      if ( my_area.h > my_height ) {
		   y = my_area.h - my_height;
         if ( y < 0 )
            y = 0;
      } else
         y = 0;
	}

	if(x > my_area.w - my_width ) {  
      if ( my_area.w > my_width ) {
		   x = my_area.w - my_width;
         if ( x < 0 )
            x = 0;
      } else
         x = 0;
	}

	Sint32 dx = my_area.x - x;
	Sint32 dy = my_area.y - y;

	my_area.x = x;
	my_area.y = y;

	if(GetChildList() == NULL) {
		Update();
		return;
	}

	for(PG_Widget* i = GetChildList()->first(); i != NULL; i = i->next()) {
		i->MoveRect(i->x + dx, i->y + dy);
	}

	Update();
}

void PG_ScrollArea::AddChild(PG_Widget* child) {
	PG_Widget::AddChild(child);
	child->MoveRect(child->x - my_area.x, child->y - my_area.y);

	if(child->x+child->w+my_area.x-my_xpos > my_area.w) {
		my_area.w = child->x+child->w+my_area.x-my_xpos;

		if(my_AddResizeParent) {
			GetParent()->SizeWidget( my_area.w + GetParent()->GetBorderSize()*2, GetParent()->my_height);
		}
		sigAreaChangedWidth(this, my_area.w);
	}
	if(child->y+child->h+my_area.y-my_ypos > my_area.h) {
		my_area.h = child->y+child->h+my_area.y-my_ypos;

		if(my_AddResizeParent) {
			GetParent()->SizeWidget(GetParent()->my_width, my_area.h + GetParent()->GetBorderSize()*2 );
		}
		sigAreaChangedHeight(this, my_area.h);
	}

	if(IsVisible()) {
		child->Show();
	}
}

void PG_ScrollArea::ScrollToWidget(PG_Widget* widget, bool bVertical) {
	if(GetWidgetCount() == 0) {
		return;
	}

	Uint16 ypos = 0;
	Uint16 xpos = 0;

	if(bVertical) {
		// widget within visible area, no need to scroll
		if(widget->y >= my_ypos && widget->y+widget->h <= my_ypos+my_height) {
			return;
		}
		ypos = widget->y - my_ypos + my_area.y;
		xpos = my_area.x;
		if(my_area.h > my_height && ypos > my_area.h - my_height) {
			ypos = my_area.h - my_height;
		}
	} else {
		// widget within visible area, no need to scroll
		if(widget->x >= my_xpos && widget->x+widget->w <= my_xpos+my_width) {
			return;
		}
		xpos = widget->x - my_xpos + my_area.x;
		ypos = my_area.y;
		if(my_area.w > my_width && xpos > my_area.w - my_width) {
			xpos = my_area.w - my_width;
		}
	}

	ScrollTo(xpos, ypos);
}

bool PG_ScrollArea::RemoveChild(PG_Widget* child) {

	// recalc scrollarea
	Uint16 w = 0;
	Uint16 h = 0;

	if(GetChildList() == NULL) {
		return false;
	}
	if(GetChildList()->size() == 0) {
		return false;
	}

	// remove widget
	PG_Rect r = *child;
	if(!PG_Widget::RemoveChild(child)) {
		return false;
	}

	for(PG_Widget* i = GetChildList()->first(); i != NULL; i = i->next()) {

		// check if i should move the widget (x)
		if(my_shiftx && (i->x >= r.x+r.w)) {
			i->MoveRect(i->x - r.w, i->y);
		}

		// check if i should move the widget (y)
		if(my_shifty && (i->y >= r.y+r.h)) {
			i->MoveRect(i->x, i->y - r.h);
		}

		// recalc new scrollarea
		if(i->x+i->w+my_area.x-my_xpos > w) {
			w = i->x+i->w+my_area.x-my_xpos;
		}
		if(i->y+i->h+my_area.y-my_ypos > h) {
			h = i->y+i->h+my_area.y-my_ypos;
		}
	}

	// signal changes
	if(w != my_area.w) {
		my_area.w = w;
		sigAreaChangedWidth(this, my_area.w);

		if(my_RemoveResizeParent) {
			GetParent()->SizeWidget(my_area.w, GetParent()->my_height);
		}
	}

	if(h != my_area.h) {
		my_area.h = h;
		sigAreaChangedHeight(this, my_area.h);

		if(my_RemoveResizeParent) {
			GetParent()->SizeWidget(GetParent()->my_width, my_area.h);
		}
	}

	Update();
	return true;
}

void PG_ScrollArea::RemoveAll() {
	if(GetChildList() == NULL) {
		return;
	}
	GetChildList()->clear();

	if(my_shiftx) {
		my_area.w = 0;
		sigAreaChangedWidth(this, 0);
	}

	if(my_shifty) {
		my_area.h = 0;
		sigAreaChangedHeight(this, 0);
	}

	Update();
}

void PG_ScrollArea::DeleteAll() {
	if(GetChildList() == NULL) {
		return;
	}

	PG_Widget* list = GetChildList()->first();

	GetChildList()->clear();
	Update();

	for(; list != NULL; ) {
		PG_Widget* w = list;
		list = list->next();
		w->SetVisible(false);
		delete w;
	}
	my_area.w = 0;
	my_area.h = 0;
	sigAreaChangedWidth(this, my_area.w);
	sigAreaChangedHeight(this, my_area.h);
}

Uint16 PG_ScrollArea::GetWidgetCount() {
	if(GetChildList() == NULL) {
		return 0;
	}

	return GetChildList()->size();
}

Uint16 PG_ScrollArea::GetScrollPosX() {
	return my_area.x;
}

Uint16 PG_ScrollArea::GetScrollPosY() {
	return my_area.y;
}

void PG_ScrollArea::SetShiftOnRemove(bool shiftx, bool shifty) {
	my_shiftx = shiftx;
	my_shifty = shifty;
}

void PG_ScrollArea::SetAreaWidth(Uint16 w) {
	if(my_area.w == w) {
		return;
	}
	my_area.w = w;
	sigAreaChangedWidth(this, my_area.w);
}

void PG_ScrollArea::SetAreaHeight(Uint16 h) {
	if(my_area.h == h) {
		return;
	}
	my_area.h = h;
	sigAreaChangedHeight(this, my_area.h);
}

PG_Widget* PG_ScrollArea::GetFirstInList() {
	PG_RectList* list = GetChildList();
	if(list == NULL) {
		return NULL;
	}
	return list->first();
}

void PG_ScrollArea::SetResizeParent(bool bRemove, bool bAdd) {
	if(GetParent() == NULL) {
		return;
	}

	my_RemoveResizeParent = bRemove;
	my_AddResizeParent = bAdd;
}

void PG_ScrollArea::eventSizeWidget(Uint16 w, Uint16 h) {
	if(w != my_width) {
		this->w = w;
		// my_area doesn't change, so why should we issue a AreaChange event ? (Martin)
		// sigAreaChangedWidth(this, my_area.w);
	}
	if(h != my_height) {
		this->h = h;
		// sigAreaChangedHeight(this, my_area.h);
	}
}
