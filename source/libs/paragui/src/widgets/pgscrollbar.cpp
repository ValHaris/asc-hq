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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgscrollbar.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

#include "pgscrollbar.h"
#include "pgapplication.h"
#include "pgeventsupplier.h"

#include "propstrings_priv.h"

PG_ScrollBar::PG_ScrollBar(PG_Widget* parent, const PG_Rect& r, ScrollDirection direction, int id, const std::string& style) : PG_ThemeWidget(parent, r, style) {
	sb_direction = direction;

	SetID(id);

	scroll_min = 0;
	scroll_max = 4;
	scroll_current = 0;

	my_linesize = 1;
	my_pagesize = 1;

	scrollbutton[0] = new PG_Button(this);
	scrollbutton[0]->SetID((direction == VERTICAL) ? IDSCROLLBAR_UP : IDSCROLLBAR_LEFT);
	scrollbutton[0]->sigClick.connect(slot(*this, &PG_ScrollBar::handleButtonClick));

	scrollbutton[1] = new PG_Button(this);
	scrollbutton[1]->SetID((direction == VERTICAL) ? IDSCROLLBAR_DOWN : IDSCROLLBAR_RIGHT);
	scrollbutton[1]->sigClick.connect(slot(*this, &PG_ScrollBar::handleButtonClick));

	dragbutton = new ScrollButton(this);
	dragbutton->SetID(IDSCROLLBAR_DRAG);
	dragbutton->sigClick.connect(slot(*this, &PG_ScrollBar::handleButtonClick));

	if(style != PG_PropStr::Scrollbar) {
		LoadThemeStyle(PG_PropStr::Scrollbar);
	}
	LoadThemeStyle(style);
	SetPosition(0);
}

PG_ScrollBar::~PG_ScrollBar() {}

void PG_ScrollBar::LoadThemeStyle(const std::string& widgettype) {

	PG_ThemeWidget::LoadThemeStyle(widgettype, PG_PropStr::Scrollbar);

	if(sb_direction == VERTICAL) {
		scrollbutton[0]->LoadThemeStyle(widgettype, PG_PropStr::ScrollbarUp);
		scrollbutton[1]->LoadThemeStyle(widgettype, PG_PropStr::ScrollbarDown);
	} else {
		scrollbutton[0]->LoadThemeStyle(widgettype, PG_PropStr::ScrollbarLeft);
		scrollbutton[1]->LoadThemeStyle(widgettype, PG_PropStr::ScrollbarRight);
	}

	dragbutton->LoadThemeStyle(widgettype, PG_PropStr::ScrollbarDrag);

	if(sb_direction == VERTICAL) {
		dragbutton->LoadThemeStyle(widgettype, PG_PropStr::ScrollbarDragV);
		PG_ThemeWidget::LoadThemeStyle(widgettype, PG_PropStr::ScrollbarV);
	} else {
		dragbutton->LoadThemeStyle(widgettype, PG_PropStr::ScrollbarDragH);
		PG_ThemeWidget::LoadThemeStyle(widgettype, PG_PropStr::ScrollbarH);
	}
	RecalcPositions();
}

void PG_ScrollBar::RecalcPositions() {

	if(sb_direction == VERTICAL) {
		position[0].x = 0;
		position[0].y = 0;
		position[0].w = w;
		position[0].h = w;

		position[1].x = 0;
		position[1].y = abs(h-w);
		position[1].w = w;
		position[1].h = w;

		position[2].x = 0;
		position[2].y = w;
		position[2].w = w;
		position[2].h = abs(h-(w*2));

		position[3].x = 0;
		position[3].w = w;
		position[3].h = position[2].h / 2;

		if ( my_pagesize <= 0 || scroll_max - scroll_min <= 0 )
			position[3].h = position[2].h;
		else {
			position[3].h = (position[2].h * my_pagesize ) / (scroll_max - scroll_min + my_pagesize );
			if ( position[3].h < position[3].w * 3 / 2)
				position[3].h = position[3].w * 3 / 2 ;
		}
	} else {
		position[0].x = 0;
		position[0].y = 0;
		position[0].w = h;
		position[0].h = h;

		position[1].x = abs(w - h);
		position[1].y = 0;
		position[1].w = h;
		position[1].h = h;

		position[2].x = h;
		position[2].y = 0;
		position[2].w = abs(w-(h*2));
		position[2].h = h;

		position[3].y = 0;
		position[3].w = (Uint16)((double)position[2].w / 2.0);
		position[3].h = h;

		if ( my_pagesize <= 0 || scroll_max - scroll_min <= 0 )
			position[3].w = position[2].w;
		else {
			position[3].w = (position[2].w * my_pagesize ) / (scroll_max - scroll_min + my_pagesize  );
			if ( position[3].w < position[3].h * 3 / 2)
				position[3].w = position[3].h * 3 / 2;
		}
	}

	int pos = 	scroll_current - scroll_min;

	if(sb_direction == VERTICAL) {
		position[3].x = 0;
		if ( (scroll_max - scroll_min) * pos <= 0 )
			position[3].y = position[0].h;
		else
			position[3].y = (Uint16)(position[0].h + (((double)position[2].h - (double)position[3].h) / (double)(scroll_max - scroll_min)) * (double)pos);
	} else {
		position[3].y = 0;
		if ( (scroll_max - scroll_min) * pos <= 0 )
			position[3].x = position[0].w;
		else
			position[3].x = (Uint16)(position[0].w + (((double)position[2].w - (double)position[3].w) / (double)(scroll_max - scroll_min)) * (double)pos);
	}

	// bordersize
	for(int i=0; i<4; i++) {
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

void PG_ScrollBar::eventSizeWidget(Uint16 w, Uint16 h) {

	PG_ThemeWidget::eventSizeWidget(w, h);

	my_height = h;
	my_width = w;

	RecalcPositions();
	SetPosition(scroll_current);
	return;
}

/**  */
bool PG_ScrollBar::eventMouseMotion(const SDL_MouseMotionEvent* motion) {
	return PG_ThemeWidget::eventMouseMotion(motion);
}

/**  */
bool PG_ScrollBar::eventMouseButtonUp(const SDL_MouseButtonEvent* button) {
	int x;
	int y;
	int mx = dragbutton->my_xpos + dragbutton->my_width / 2;
	int my = dragbutton->my_ypos + dragbutton->my_height / 2;

	PG_Application::GetEventSupplier()->GetMouseState(x,y);

	switch (button->button) {
		case 1:
			if(sb_direction == VERTICAL) {
				if(y < my) {
					SetPosition(scroll_current - my_pagesize);
				} else {
					SetPosition(scroll_current + my_pagesize);
				}
			} else {
				if(x < mx) {
					SetPosition(scroll_current - my_pagesize);
				} else {
					SetPosition(scroll_current + my_pagesize);
				}
			}

			sigScrollPos(this, scroll_current);
			return true;

		case 4:
			if(scroll_current <= scroll_min + my_linesize) {
				SetPosition(scroll_min);
			} else {
				SetPosition(scroll_current - my_linesize);
			}
			sigScrollPos(this, scroll_current);
			return true;

		case 5:
			SetPosition(scroll_current + my_linesize);
			sigScrollPos(this, scroll_current);
			return true;
	}

	return PG_ThemeWidget::eventMouseButtonUp(button);
}


PG_ScrollBar::ScrollButton::ScrollButton(PG_ScrollBar* parent, const PG_Rect& r) : PG_Button(parent, r) {
	SetID(IDSCROLLBAR_DRAG);
	my_tickMode = false;
}

PG_ScrollBar::ScrollButton::~ScrollButton() {}

/**  */
bool PG_ScrollBar::ScrollButton::eventMouseMotion(const SDL_MouseMotionEvent* motion) {
	PG_Point p;

	if(GetPressed()) {

		p = GetParent()->ScreenToClient(motion->x, motion->y);

		if(GetParent()->sb_direction == VERTICAL) {
			p.y -= offset.y;

			if(p.y < GetParent()->position[2].y) {
				p.y = GetParent()->position[2].y;
			}

			int maxy;
			if(!my_tickMode) {
				maxy = GetParent()->position[2].y + (GetParent()->position[2].h) - my_height;
			} else {
				maxy = GetParent()->my_height - my_height;
			}

			if(p.y > maxy) {
				p.y = maxy;
			}

			MoveWidget(GetParent()->position[2].x, p.y);
		} else {
			p.x -= offset.x;

			if(p.x < GetParent()->position[2].x) {
				p.x = GetParent()->position[2].x;
			}

			int maxx;
			if(!my_tickMode) {
				maxx = GetParent()->position[2].x + (GetParent()->position[2].w) - (my_width);
			} else {
				maxx = GetParent()->my_width - my_width;
			}

			if(p.x > maxx) {
				p.x = maxx;
			}

			MoveWidget(p.x, GetParent()->position[2].y);
		}

		int pos = GetPosFromPoint(p);
		if(GetParent()->scroll_current != pos || my_tickMode) {
			GetParent()->scroll_current = pos;
			GetParent()->sigScrollTrack(GetParent(), pos);
		}

	}

	return true;
}

/**  */
bool PG_ScrollBar::ScrollButton::eventMouseButtonDown(const SDL_MouseButtonEvent* button) {
	int x,y;

	if(button->button == 1) {
		PG_Application::GetEventSupplier()->GetMouseState(x,y);
		offset = ScreenToClient(x, y);
	}

	return PG_Button::eventMouseButtonDown(button);
}

/**  */
PG_ScrollBar* PG_ScrollBar::ScrollButton::GetParent() {
	return (PG_ScrollBar*)PG_Button::GetParent();
}

/**  */
void PG_ScrollBar::SetPosition(int pos) {

	if(pos < scroll_min) {
		pos = scroll_min;
	}

	if(pos > scroll_max) {
		pos = scroll_max;
	}

	scroll_current = pos;
	pos -= scroll_min;

	// check if we are currently in a drag operation
	if(dragbutton->GetPressed()) {
		return;
	}

	RecalcPositions();
}

int PG_ScrollBar::GetPosition() {
	return scroll_current;
}

/**  */
bool PG_ScrollBar::handleButtonClick(PG_Button* button) {

	if(button == scrollbutton[0]) {		// UP | LEFT
		if(scroll_current == scroll_min) {
			return false;
		}
		SetPosition(scroll_current - my_linesize);
		sigScrollPos(this, scroll_current);
		return true;
	}

	if(button == scrollbutton[1]) {		// DOWN | RIGHT
		if(scroll_current == scroll_max) {
			return false;
		}
		SetPosition(scroll_current + my_linesize);
		sigScrollPos(this, scroll_current);
		return true;

	}

	return false;
}

/**  */
bool PG_ScrollBar::eventMouseButtonDown(const SDL_MouseButtonEvent* button) {
	return false;
}

/**  */
bool PG_ScrollBar::ScrollButton::eventMouseButtonUp(const SDL_MouseButtonEvent* button) {
	int pos = GetParent()->scroll_current; //my_tempPos;

	if(button->button != 1) {
		return false;
	}

	PG_Button::eventMouseButtonUp(button);

	//GetParent()->SetPosition(pos);
	GetParent()->sigScrollPos(GetParent(), pos);

	return true;
}

void PG_ScrollBar::ScrollButton::SetTickMode(bool on) {
	my_tickMode = on;
}

/**  */
void PG_ScrollBar::SetRange(Uint32 min, Uint32 max) {
	scroll_min = min;
	scroll_max = max;

	// checks if the position is valid and recalculates the size
	SetPosition( scroll_current );
}

int PG_ScrollBar::GetMinRange() {
	return scroll_min;
}

/**  */
int PG_ScrollBar::GetMaxRange() {
	return scroll_max;
}

/**  */
int PG_ScrollBar::ScrollButton::GetPosFromPoint(PG_Point p) {
	Uint32 range = (GetParent()->scroll_max - GetParent()->scroll_min);
	int pos = 0;

	if(p.x < 0)
		p.x = 0;
	if(p.y < 0)
		p.y = 0;

	//if(!my_tickMode) {
	if(GetParent()->sb_direction == VERTICAL) {
		pos = (int)((double)(p.y - GetParent()->position[2].y) / (((double)GetParent()->position[2].h - (double)GetParent()->position[3].h) / (double)range));
	} else {
		pos = (int)((double)(p.x - GetParent()->position[2].x) / (((double)GetParent()->position[2].w - (double)GetParent()->position[3].w) / (double)range));
	}
	/*} else {
		if(GetParent()->sb_direction == VERTICAL) {
			pos = (int)( (((double)(p.y)) * (double)(range)) / ( (double)GetParent()->position[2].h - (double)GetParent()->position[3].h) + .5 );
		} else {
			pos = (int)( (((double)(p.x)) * (double)(range)) / ( (double)GetParent()->position[2].w - (double)GetParent()->position[3].w) + .5 );
		}
	}*/

	if(pos < 0)
		pos = 0;

	pos = GetParent()->scroll_min+pos;

	if(pos > GetParent()->scroll_max) {
		pos = GetParent()->scroll_max;
	}

	if(pos < GetParent()->scroll_min) {
		pos = GetParent()->scroll_min;
	}

	return pos;
}

void PG_ScrollBar::SetLineSize(int ls) {
	my_linesize = ls;
}

void PG_ScrollBar::SetPageSize(int ps) {
	my_pagesize = ps;
	RecalcPositions();
}
