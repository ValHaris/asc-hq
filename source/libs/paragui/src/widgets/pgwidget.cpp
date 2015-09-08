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
   Update Date:      $Date: 2009-08-16 16:50:27 $
   Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgwidget.cpp,v $
   CVS/RCS Revision: $Revision: 1.5 $
   Status:           $State: Exp $
 */

#include <cstring>
#include <stdarg.h>

#include "pgwidget.h"
#include "pgapplication.h"
#include "pglog.h"
#include "pgdraw.h"
#include "pglayout.h"
#include "pgtheme.h"
#include "pgeventsupplier.h"

#include "propstrings_priv.h"

#define TXT_HEIGHT_UNDEF 0xFFFF


//bool PG_Widget::bBulkUpdate = false;
PG_RectList PG_Widget::widgetList;
int PG_Widget::my_ObjectCounter = 0;

class PG_WidgetDataInternal {
public:
	PG_WidgetDataInternal() : modalstatus(0), inDestruct(false), inMouseLeave(false), font(NULL), dirtyUpdate(false), id(-1),
			transparency(0), quitModalLoop(false), visible(false), hidden(false), firstredraw(true),
			childList(NULL), haveTooltip(false), fadeSteps(10), mouseInside(false), userdata(NULL),
         userdatasize(0), widthText(TXT_HEIGHT_UNDEF), heightText(TXT_HEIGHT_UNDEF), widgetParent(NULL), hotkeyModifier(-1), hotkey(0) {
		updateOverlappingSiblings = PG_Application::GetUpdateOverlappingSiblings();
	};

	int modalstatus;
	bool inDestruct;
	bool inMouseLeave;
	PG_Font* font;
	bool dirtyUpdate;
	int id;
	Uint8 transparency;
	bool quitModalLoop;
	bool visible;
	bool hidden;
	bool firstredraw;
	PG_RectList* childList;
	bool haveTooltip;
	int fadeSteps;
	bool mouseInside;
	char* userdata;
	int userdatasize;
	Uint16 widthText;
	Uint16 heightText;

	PG_Widget* widgetParent;
	PG_Point ptDragStart;
	PG_Rect rectClip;
	bool havesurface;
	std::string name;
	bool updateOverlappingSiblings;
   int hotkeyModifier;
   PG_Char hotkey;

};

void PG_Widget::createMySurface( int w, int h )
{
   my_srfObject = PG_Draw::CreateRGBSurface(w, h);
}


PG_Widget::PG_Widget(PG_Widget* parent, const PG_Rect& rect, bool bObjectSurface) :
PG_Rect(rect), my_srfObject(NULL), _mid(new PG_WidgetDataInternal) {

	_mid->havesurface = bObjectSurface;

	//Set default font
	if(PG_Application::DefaultFont != NULL) {
		_mid->font = new PG_Font(
		                 PG_Application::DefaultFont->GetName(),
		                 PG_Application::DefaultFont->GetSize());
	} else {
		PG_LogWRN("Unable to get default font! Did you load a theme ?");
	}

	//my_srfScreen = PG_Application::GetScreen();

	if(_mid->havesurface) {
           createMySurface(w, h);
	}

	// ??? - How can i do this better - ???
	char buffer[15];
	sprintf(buffer, "Object%d", ++my_ObjectCounter);
	_mid->name = buffer;

	// default border colors
	my_colorBorder[0][0].r = 255;
	my_colorBorder[0][0].g = 255;
	my_colorBorder[0][0].b = 255;

	my_colorBorder[0][1].r = 239;
	my_colorBorder[0][1].g = 239;
	my_colorBorder[0][1].b = 239;

	my_colorBorder[1][0].r = 89;
	my_colorBorder[1][0].g = 89;
	my_colorBorder[1][0].b = 89;

	my_colorBorder[1][1].r = 134;
	my_colorBorder[1][1].g = 134;
	my_colorBorder[1][1].b = 134;

	if (parent) {
		//my_xpos = _mid->widgetParent->my_xpos + my_xpos;
		//my_ypos = _mid->widgetParent->my_ypos + my_ypos;
		parent->AddChild(this);
	} else {
		AddToWidgetList();
	}
	//_mid->rectClip = *this;
}

void PG_Widget::RemoveAllChilds() {

	// remove all child widgets
	if(_mid->childList != NULL) {

		PG_Widget* i = _mid->childList->first();
		while(i != NULL) {
			PG_Widget* w = i;
			i = i->next();

			RemoveChild(w);
			delete w;
		}
		_mid->childList->clear();
	}

}

PG_Widget::~PG_Widget() {

	_mid->inDestruct = true;

	if(!_mid->havesurface && my_srfObject) {
		PG_LogWRN("DrawObject declared without a surface has unexpectedly born one ?");
	}
	PG_Application::UnloadSurface(my_srfObject);
	my_srfObject = NULL;

	Hide();

	RemoveAllChilds();

	// remove myself from my parent's childlist (if any parent)

	if (GetParent() != NULL) {
		GetParent()->RemoveChild(this);
	} else {
		RemoveFromWidgetList();
	}

	// remove childlist
	delete _mid->childList;
	_mid->childList = NULL;

	if (_mid->userdata != NULL) {
		delete[] _mid->userdata;
	}

	// remove the font
	delete _mid->font;

	// remove my private data
	delete _mid;

	//cout << "Removed widget '" << GetName() << "'" << endl;
}

void PG_Widget::RemoveFromWidgetList() {
	widgetList.Remove(this);
}

void PG_Widget::AddToWidgetList() {
	if(!GetParent()) {
		widgetList.Add(this);
	}
}

/** Check if we can accept the event */

bool PG_Widget::AcceptEvent(const SDL_Event * event) {

	if (!IsVisible() || IsHidden()) {
		return false;
	}

	switch (event->type) {
		case SDL_MOUSEMOTION:
			if ((event->motion.x < _mid->rectClip.my_xpos) ||
			        (event->motion.x > (_mid->rectClip.my_xpos + _mid->rectClip.my_width - 1))) {
				if (_mid->mouseInside) {
					eventMouseLeave();
				}
				return false;
			}
			if ((event->motion.y < _mid->rectClip.my_ypos) ||
			        (event->motion.y > (_mid->rectClip.my_ypos + _mid->rectClip.my_height - 1))) {
				if (_mid->mouseInside) {
					eventMouseLeave();
				}
				return false;
			}
			if (!_mid->mouseInside) {
				_mid->mouseInside = true;
				eventMouseEnter();
				return true;
			}
			break;

		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
			if ((event->button.x < _mid->rectClip.my_xpos) ||
			        (event->button.x > (_mid->rectClip.my_xpos + _mid->rectClip.my_width - 1)))
				return false;

			if ((event->button.y < _mid->rectClip.my_ypos) ||
			        (event->button.y > (_mid->rectClip.my_ypos + _mid->rectClip.my_height - 1)))
				return false;

			break;
	}

	return true;		// accept the event as default
}


/**  */
void PG_Widget::eventMouseEnter() {
	sigMouseEnter();
}


/**  */
void PG_Widget::eventMouseLeave() {
	_mid->mouseInside = false;

	if(GetParent() != NULL && !GetParent()->IsMouseInside()) {
		GetParent()->eventMouseLeave();
	}

	sigMouseLeave();
	/*if(GetParent()) {
		GetParent()->eventMouseLeave();
	}*/
}

/**  */
void PG_Widget::eventShow() {}

/**  */
void PG_Widget::eventHide() {}

/**  */
PG_Point PG_Widget::ClientToScreen(int sx, int sy) {
	return PG_Point(sx + my_xpos, sy + my_ypos);
}

PG_Point PG_Widget::ScreenToClient(int x, int y) {
	return PG_Point(x - my_xpos, y - my_ypos);
}

void PG_Widget::AddChild(PG_Widget * child) {

	if (!child)
		return;

	// remove our new child from previous lists
	if(child->GetParent()) {
		child->GetParent()->RemoveChild(child);
	} else {
		child->RemoveFromWidgetList();
	}

	child->MoveRect(child->my_xpos + my_xpos, child->my_ypos + my_ypos);
	child->_mid->widgetParent = this;

	if (_mid->childList == NULL) {
		_mid->childList = new PG_RectList;
	}

	_mid->childList->Add(child);
}

bool PG_Widget::MoveWidget(int x, int y, bool update) {
	SDL_Surface* screen = PG_Application::GetScreen();

	if (GetParent() != NULL) {
		x += GetParent()->my_xpos;
		y += GetParent()->my_ypos;
	}
	if(x == my_xpos && y == my_ypos) {
		// Optimization: We haven't moved, so do nothing.
		return false;
	}

	if(!IsVisible() || IsHidden() || !update) {
		MoveRect(x, y);
		return true;
	}

	// delta x,y
	int dx = x - my_xpos;
	int dy = y - my_ypos;

	// calculate vertical update rect

	PG_Rect vertical(0, 0, abs(dx), my_height + abs(dy));

	if(dx >= 0) {
		vertical.my_xpos = my_xpos;
	} else {
		vertical.my_xpos = my_xpos + my_width + dx;
	}

	vertical.my_ypos = my_ypos;

	// calculate vertical update rect

	PG_Rect horizontal(0, 0, my_width + abs(dx), abs(dy));

	horizontal.my_xpos = my_xpos;

	if(dy >= 0) {
		horizontal.my_ypos = my_ypos;
	} else {
		horizontal.my_ypos = my_ypos + my_height + dy;
	}

	// move rectangle and store new background
	MoveRect(x, y);

	if(vertical.my_xpos + vertical.my_width > screen->w) {
		vertical.my_width = screen->w - vertical.my_xpos;
	}
	if(vertical.my_ypos + vertical.my_height > screen->h) {
		vertical.my_height = screen->h - vertical.my_ypos;
	}

	if(horizontal.my_xpos + horizontal.my_width > screen->w) {
		horizontal.my_width = screen->w- horizontal.my_xpos;
	}
	if(horizontal.my_ypos + horizontal.my_height > screen->h) {
		horizontal.my_height = screen->h - horizontal.my_ypos;
	}

	if(!PG_Application::GetBulkMode()) {

		// I'm experimenting with cairo - this change was needed to
		// make rendering work as expected -- Alex

		/*UpdateRect(vertical);
		UpdateRect(horizontal);
		UpdateRect(_mid->rectClip);
		PG_Application::LockScreen();
		SDL_Rect rects[3] = {_mid->rectClip, vertical, horizontal};
		PG_Application::UpdateRects(screen, 3, rects);
		PG_Application::UnlockScreen();*/

		int minx, maxx;
		int miny, maxy;
		minx = PG_MIN(vertical.x, horizontal.x);
		minx = PG_MIN(minx, _mid->rectClip.x);
		maxx = PG_MAX(vertical.x+vertical.w, horizontal.x+horizontal.w);
		maxx = PG_MAX(maxx, _mid->rectClip.x+_mid->rectClip.w);

		miny = PG_MIN(vertical.y, horizontal.y);
		miny = PG_MIN(miny, _mid->rectClip.y);
		maxy = PG_MAX(vertical.y+vertical.h, horizontal.y+horizontal.h);
		maxy = PG_MAX(maxy, _mid->rectClip.y+_mid->rectClip.h);

	   PG_Application::ScreenLocker locker(true);
		PG_Rect rect(minx,miny,maxx-minx,maxy-miny);
		UpdateRect(rect);
		PG_Application::UpdateRects(screen, 1, &rect);
	}

	return true;
}

bool PG_Widget::MoveWidget(const PG_Rect& r, bool update) {
	SizeWidget(r.w, r.h, update);
	MoveWidget(r.x, r.y, update);

	return true;
}

bool PG_Widget::SizeWidget(Uint16 w, Uint16 h, bool update) {
	Uint16 old_w = my_width;
	Uint16 old_h = my_height;

	if(my_width == w && my_height == h) {
		return false;
	}

	// create new widget drawsurface
	if(my_srfObject) {
		PG_Application::UnloadSurface(my_srfObject);

		if(w > 0 && h > 0) {
			my_srfObject = PG_Draw::CreateRGBSurface(w, h);
		} else {
			my_srfObject = NULL;
		}
	}

	eventSizeWidget(w, h);

	my_width = w;
	my_height = h;

	if(!IsVisible() || IsHidden() || !update) {
		return true;
	}

	if(my_srfObject) {
		Redraw();
	} else {
		if(old_w > w || old_h > h) {
			PG_Rect u(
			    my_xpos,
			    my_ypos,
			    (old_w > w) ? old_w : w,
			    (old_h > h) ? old_h : h);

			// Fix: If you decrease the resolution of your application,
			// you must decrease the window sizes as well. However, this
			// code will create an out of bounds X error since x + old_w
			// (or y + old_h) are now out of range.
			if (my_xpos + old_w >= PG_Application::GetScreenWidth())
				u.my_width = PG_Application::GetScreenWidth() - my_xpos;

			if (my_ypos + old_h >= PG_Application::GetScreenHeight())
				u.my_height = PG_Application::GetScreenHeight() - my_ypos;

			UpdateRect(u);
			PG_Application::UpdateRects(PG_Application::GetScreen(), 1, &u);
		} else {
			Update();
		}
	}

	return true;
}

/**  */
bool PG_Widget::ProcessEvent(const SDL_Event * event, bool bModal) {

	bool processed = false;
	// do i have a capturehook set ? (modal)
	if(bModal) {
		// i will send that event to my children

		if(_mid->childList != NULL) {
			PG_Widget* list = _mid->childList->first();

			while (!processed && (list != NULL)) {
				processed = list->ProcessEvent(event, true);
				list = list->next();
			}
		}

		if(processed) {
			return processed;
		}
	}

	// let me see if i can process it myself

	if(PG_MessageObject::ProcessEvent(event)) {
		return true;
	}

	if(bModal) {
		return processed;
	}

	// ask my parent to process the event

	if(GetParent()) {
		if(GetParent()->ProcessEvent(event)) {
			return true;
		}
	}

	return false;
}

bool PG_Widget::RemoveChild(PG_Widget * child) {
	if(_mid->childList == NULL || child == NULL) {
		return false;
	}

	if(_mid->childList->Remove(child)) {
		child->MoveRect(child->my_xpos - my_xpos, child->my_ypos - my_ypos);
		return true;
	}

	return false;
}

bool PG_Widget::IsMouseInside() {
	int x, y;
	PG_Application::GetEventSupplier()->GetMouseState(x, y);
	PG_Point p(x, y);
	_mid->mouseInside = IsInside(p);

	return _mid->mouseInside;
}

/**  */
bool PG_Widget::Redraw(bool update) {
	PG_Rect r(0, 0, my_width, my_height);

	if(my_srfObject != NULL) {
		eventDraw(my_srfObject, r);
	}

	if(_mid->childList != NULL) {
		for(PG_Widget* i = _mid->childList->first(); i != NULL; i = i->next()) {
			i->Redraw(false);
		}
	}

	if (update) {
		Update();
	}
	return true;
}

void PG_Widget::SetVisible(bool visible) {

	if(IsHidden()) {
		return;
	}

	// Attempt to make object visible
	if(visible) {
		if(_mid->visible) {			// Object already visible
			return;
		} else {					// Display object
			_mid->visible = visible;
			if(_mid->firstredraw) {
				Redraw(false);
				_mid->firstredraw = false;
			}
		}

	}

	// Attempt to make object invisible
	if(!visible) {
		if(!_mid->visible) {			// Object is already invisible
			return;
		} else {					// Hide object
			//RestoreBackground();
			_mid->visible = visible;
		}
	}

	if(_mid->childList != NULL) {
		for(PG_Widget* i = _mid->childList->first(); i != NULL; i = i->next()) {
			i->SetVisible(visible);
			if(!i->IsHidden()) {
				if(visible) {
					i->eventShow();
				} else {
					i->eventHide();
				}
			}
		}
	}
}

/**  */
void PG_Widget::Show(bool fade) {

	if(fade && IsVisible() && !IsHidden()) {
		fade = false;
	}

	PG_Widget* parent = GetParent();
	if(parent == NULL) {
		widgetList.BringToFront(this);
	} else {
		parent->GetChildList()->BringToFront(this);
	}

	SetHidden(false);

	if(parent != NULL && (!parent->IsVisible() || parent->IsHidden())) {
		return;
	}

	SetVisible(true);
	eventShow();

	if (fade) {
		FadeIn();
	}

	if(IsMouseInside()) {
		eventMouseEnter();
	}

	//SDL_SetClipRect(my_srfScreen, NULL);
	Update();

	return;
}

/**  */
void PG_Widget::Hide(bool fade) {
	SDL_Surface* screen = PG_Application::GetScreen();

	if(!IsVisible()) {
		SetHidden(true);
		eventHide();
		return;
	}

	RecalcClipRect();

	if(!_mid->inDestruct && !_mid->inMouseLeave) {
		_mid->inMouseLeave = true;
		eventMouseLeave();
		_mid->inMouseLeave = false;
	}

	if (fade) {
		FadeOut();
	}

	SetVisible(false);
	eventHide();

	ReleaseCapture();
	ReleaseInputFocus();

	SDL_SetClipRect(screen, NULL);

	if(!PG_Application::GetBulkMode()) {
		UpdateRect(_mid->rectClip);

	   PG_Application::ScreenLocker locker(true);
		PG_Application::UpdateRects(screen, 1, &_mid->rectClip);
	}

	SetHidden(true);

	return;
}

/**  */
void PG_Widget::MoveRect(int x, int y) {
	int dx = x - my_xpos;
	int dy = y - my_ypos;

	my_xpos = x;
	my_ypos = y;
	_mid->rectClip.my_xpos += dx;
	_mid->rectClip.my_ypos += dy;

	// recalc cliprect
	RecalcClipRect();

	if(_mid->childList != NULL) {
		for(PG_Widget* i = _mid->childList->first(); i != NULL; i = i->next()) {
			i->MoveRect(i->my_xpos + dx, i->my_ypos + dy);
		}
	}

	eventMoveWidget(x, y);
}

void PG_Widget::Blit(bool recursive, bool restore) {

	if(!_mid->visible || _mid->hidden) {
		return;
	}

	// recalc clipping rectangle
	RecalcClipRect();

	// don't draw a null rect
	if(_mid->rectClip.w == 0 || _mid->rectClip.h == 0) {
		return;
	}

	PG_Rect src;
	PG_Rect dst;
   PG_Application::ScreenLocker locker(true);

	// restore the background
	if(restore) {
		RestoreBackground(&_mid->rectClip);
	}

	// get source & destination rectangles
	src.SetRect(_mid->rectClip.x - my_xpos, _mid->rectClip.y - my_ypos, _mid->rectClip.w, _mid->rectClip.h);
	dst = _mid->rectClip;

	// call the blit handler
	eventBlit(my_srfObject, src, dst);

	// should we draw our children
	if(recursive) {
		// draw the children-list
		if(_mid->childList != NULL) {
			_mid->childList->Blit(_mid->rectClip);
		}
	}
}

/**  */
void PG_Widget::Update(bool doBlit) {
	static PG_Rect src;
	static PG_Rect dst;

	if(PG_Application::GetBulkMode()) {
		return;
	}

	if(!_mid->visible || _mid->hidden) {
		return;
	}

	// recalc cliprect
	RecalcClipRect();

	if(_mid->rectClip.w == 0 || _mid->rectClip.h == 0) {
		return;
	}

   PG_Application::ScreenLocker locker(true);

	// BLIT
	if(doBlit) {

		SDL_SetClipRect(PG_Application::GetScreen(), &_mid->rectClip);
		RestoreBackground(&_mid->rectClip);

		src.SetRect(_mid->rectClip.x - my_xpos, _mid->rectClip.y - my_ypos, _mid->rectClip.w, _mid->rectClip.h);
		dst = _mid->rectClip;

		if(GetParent() != NULL && _mid->updateOverlappingSiblings )
			if( !_mid->dirtyUpdate || (_mid->transparency > 0) ) {
				PG_RectList* children = GetParent()->GetChildList();
				if(children) {
					children->Blit(_mid->rectClip, GetParent()->GetChildList()->first(), this);
				}
			}

		eventBlit(my_srfObject, src, dst);

		if(_mid->childList != NULL) {
			_mid->childList->Blit(_mid->rectClip);
		}

		if(GetParent() != NULL && _mid->updateOverlappingSiblings ) {
			PG_RectList* children = GetParent()->GetChildList();
			if(children) {
				children->Blit(_mid->rectClip, this->next() );
			}
		}

		// find the toplevel widget
		PG_Widget* obj = GetToplevelWidget();
		widgetList.Blit(_mid->rectClip, obj->next());

	}

	// Update screen surface
#ifdef DEBUG
	PG_LogDBG("UPD: x:%d y:%d w:%d h:%d",dst.x,dst.y,dst.w,dst.h);
#endif // DEBUG

	PG_Application::UpdateRects(PG_Application::GetScreen(), 1, &_mid->rectClip);

	SDL_SetClipRect(PG_Application::GetScreen(), NULL);
}

/**  */
void PG_Widget::SetChildTransparency(Uint8 t) {
	if(_mid->childList == NULL) {
		return;
	}

	for(PG_Widget* i = _mid->childList->first(); i != NULL; i = i->next()) {
		i->SetTransparency(t, true);
	}
	Update();
}

void PG_Widget::StartWidgetDrag() {
	int x, y;

	PG_Application::GetEventSupplier()->GetMouseState(x, y);
	_mid->ptDragStart.x = static_cast<Sint16>(x) - my_xpos;
	_mid->ptDragStart.y = static_cast<Sint16>(y) - my_ypos;
}

void PG_Widget::WidgetDrag(int x, int y) {

	x -= _mid->ptDragStart.x;
	y -= _mid->ptDragStart.y;

	if(x < 0)
		x=0;
	if(y < 0)
		y=0;
	if(x > (PG_Application::GetScreenWidth() - my_width -1))
		x = (PG_Application::GetScreenWidth() - my_width -1);
	if(y > (PG_Application::GetScreenHeight() - my_height -1))
		y = (PG_Application::GetScreenHeight() - my_height -1);

	MoveWidget(x,y);
}

void PG_Widget::EndWidgetDrag(int x, int y) {
	WidgetDrag(x,y);
	_mid->ptDragStart.x = 0;
	_mid->ptDragStart.y = 0;
}

void PG_Widget::HideAll() {
	for(PG_Widget* i = widgetList.first(); i != NULL; i = i->next()) {
		i->Hide();
	}
}

/*void PG_Widget::BulkUpdate() {
	bBulkUpdate = true;
 
	for(PG_Widget* i = widgetList.first(); i != NULL; i = i->next()) {
		if(i->IsVisible()) {
			i->Update();
		}
	}
 
	bBulkUpdate = false;
}*/

void PG_Widget::BulkBlit() {
	//bBulkUpdate = true;
	widgetList.Blit();
	PG_Application::DrawCursor();
	//bBulkUpdate = false;
}

void PG_Widget::LoadThemeStyle(const std::string& widgettype, const std::string& objectname) {
	PG_Theme* t = PG_Application::GetTheme();
	PG_Color c;

	const std::string& font = t->FindFontName(widgettype, objectname);
	int fontsize = t->FindFontSize(widgettype, objectname);
	PG_Font::Style fontstyle = t->FindFontStyle(widgettype, objectname);

	if(!font.empty()) {
		SetFontName(font, true);
	}

	if (fontsize > 0)
		SetFontSize(fontsize, true);

	if (fontstyle >= 0)
		SetFontStyle(fontstyle, true);

	c = GetFontColor();
	t->GetColor(widgettype, objectname, PG_PropStr::textcolor, c);
	SetFontColor(c);

   c = GetFontHighlightColor();
   t->GetColor(widgettype, objectname, PG_PropStr::texthighlightcolor, c);
   SetFontHighlightColor(c);

   
	t->GetColor(widgettype, objectname, PG_PropStr::bordercolor0, my_colorBorder[0][0]);
	t->GetColor(widgettype, objectname, PG_PropStr::bordercolor1, my_colorBorder[1][0]);
	t->GetColor(widgettype, objectname, PG_PropStr::bordercolor0i, my_colorBorder[0][1]);
	t->GetColor(widgettype, objectname, PG_PropStr::bordercolor1i, my_colorBorder[1][1]);
}

void PG_Widget::LoadThemeStyle(const std::string& widgettype) {}

void PG_Widget::FadeOut() {
	PG_Rect r(0, 0, my_width, my_height);

	// blit the widget to screen (invisible)
	Blit();

	// create a temp surface
	SDL_Surface* srfFade = PG_Draw::CreateRGBSurface(my_width, my_height);
	SDL_Surface* screen = PG_Application::GetScreen();

	int d = (255-_mid->transparency)/ _mid->fadeSteps;
	if(!d) {
		d = 1;
	} // minimum step == 1

   PG_Application::ScreenLocker locker(true);

	// blit the widget to temp surface
	PG_Draw::BlitSurface(screen, *this, srfFade, r);

	for(int i=_mid->transparency; i<255; i += d) {
		RestoreBackground(NULL, true);
		SDL_SetAlpha(srfFade, SDL_SRCALPHA, 255-i);
		SDL_BlitSurface(srfFade, NULL, screen, this);
		PG_Application::UpdateRects(screen, 1, &_mid->rectClip);
	}

	RestoreBackground(NULL, true);
	SDL_SetAlpha(srfFade, SDL_SRCALPHA, 0);
	SDL_BlitSurface(srfFade, NULL, screen, this);
	SetVisible(false);
	locker.unlock();

	Update(false);

	PG_Application::UnloadSurface(srfFade);
}

void PG_Widget::FadeIn() {
	SDL_Surface* screen = PG_Application::GetScreen();

	// blit the widget to screen (invisible)
	SDL_SetClipRect(screen, NULL);
	Blit();

	PG_Rect src(
	    0,
	    0,
	    (my_xpos < 0) ? my_width + my_xpos : my_width,
	    (my_ypos < 0) ? my_height + my_ypos : my_height);

	// create a temp surface
	SDL_Surface* srfFade = PG_Draw::CreateRGBSurface(w, h);

   PG_Application::ScreenLocker locker(true);

	// blit the widget to temp surface
	PG_Draw::BlitSurface(screen, _mid->rectClip, srfFade, src);

	int d = (255-_mid->transparency)/ _mid->fadeSteps;

	if(!d) {
		d = 1;
	} // minimum step == 1
	for(int i=255; i>_mid->transparency; i -= d) {
		RestoreBackground(NULL, true);
		SDL_SetAlpha(srfFade, SDL_SRCALPHA, 255-i);
		PG_Draw::BlitSurface(srfFade, src, screen, _mid->rectClip);
		PG_Application::UpdateRects(screen, 1, &_mid->rectClip);
	}

	locker.unlock();

	Update();

	PG_Application::UnloadSurface(srfFade);
}

void PG_Widget::SetFadeSteps(int steps) {
	_mid->fadeSteps = steps;
}

bool PG_Widget::Action(KeyAction action) {
	int x = my_xpos + my_width / 2;
	int y = my_ypos + my_height / 2;

	switch(action) {
		case ACT_ACTIVATE:
			SDL_WarpMouse(x,y);
			eventMouseEnter();
			break;

		case ACT_DEACTIVATE:
			eventMouseLeave();
			break;

		case ACT_OK:
			SDL_MouseButtonEvent button;
			button.button = 1;
			button.x = x;
			button.y = y;
			eventMouseButtonDown(&button);
			SDL_Delay(200);
			eventMouseButtonUp(&button);
			Action(ACT_ACTIVATE);
			break;

		default:
			break;
	}

	return false;
}

bool PG_Widget::RestoreBackground(PG_Rect* clip, bool force) {

	if(_mid->dirtyUpdate && (_mid->transparency == 0) && !force) {
		return false;
	}

	if(PG_Application::GetBulkMode()) {
		return false;
	}

	if(clip == NULL) {
		clip = &_mid->rectClip;
	}

	if(GetParent() == NULL) {
		PG_Application::RedrawBackground(*clip);

		if(widgetList.first() != this) {
			SDL_SetClipRect(PG_Application::GetScreen(), clip);
			widgetList.Blit(*clip, widgetList.first(), this);
			SDL_SetClipRect(PG_Application::GetScreen(), NULL);
		}
		return true;
	}

	GetParent()->RestoreBackground(clip);
	SDL_SetClipRect(PG_Application::GetScreen(), clip);
	GetParent()->Blit(false, false);
	SDL_SetClipRect(PG_Application::GetScreen(), NULL);

	return true;
}

PG_Widget* PG_Widget::FindWidgetFromPos(int x, int y) {
	PG_Point p;
	p.x = x;
	p.y = y;
	bool finished = false;

	PG_Widget* toplevel = widgetList.IsInside(p);
	PG_Widget* child = NULL;

	if(!toplevel) {
		return NULL;
	}

	while(!finished) {

		if(toplevel->GetChildList()) {
			child = toplevel->GetChildList()->IsInside(p);

			if(child) {
				toplevel = child;
				child = NULL;
			} else {
				finished = true;
			}

		} else {
			finished = true;
		}
	}

	return toplevel;
}

void PG_Widget::UpdateRect(const PG_Rect& r) {
	if(PG_Application::GetBulkMode()) {
		return;
	}

	SDL_Surface* screen = PG_Application::GetScreen();

   PG_Application::ScreenLocker locker(true);
	PG_Application::RedrawBackground(r);
	SDL_SetClipRect(screen, (PG_Rect*)&r);
	widgetList.Blit(r);
	SDL_SetClipRect(screen, NULL);
}

void PG_Widget::UpdateScreen() {
	UpdateRect( PG_Rect(0, 0, PG_Application::GetScreenWidth(), PG_Application::GetScreenHeight()) );
   PG_Application::UpdateRect( PG_Application::GetScreen(), 0, 0, 0, 0 );
}

bool PG_Widget::IsInFrontOf(PG_Widget* widget) {
	PG_Widget* w1 = NULL;
	PG_Widget* w2 = NULL;

	// do both widgets have the same parent ?
	if((GetParent() != NULL) && (GetParent() == widget->GetParent())) {
		w1 = this;
		w2 = widget;
	} else {
		w1 = this->GetToplevelWidget();
		w2 = widget->GetToplevelWidget();
	}

	return (w1->index > w2->index);
}

PG_Widget* PG_Widget::GetToplevelWidget() {
	if(GetParent() == NULL) {
		return this;
	}

	return GetParent()->GetToplevelWidget();
}

void PG_Widget::SendToBack() {
	if(GetParent() == NULL) {
		widgetList.SendToBack(this);
	} else {
		GetParent()->GetChildList()->SendToBack(this);
	}
	Update();
}

void PG_Widget::BringToFront() {
	if(GetParent() == NULL) {
		widgetList.BringToFront(this);
	} else {
		GetParent()->GetChildList()->BringToFront(this);
	}
	Update();
}

void PG_Widget::RecalcClipRect() {
	PG_Rect pr;

	if (_mid->widgetParent != NULL) {
		pr = *(_mid->widgetParent->GetClipRect());
	} else {
		pr.SetRect(
		    0,
		    0,
		    PG_Application::GetScreenWidth(),
		    PG_Application::GetScreenHeight());
	}

	PG_Rect ir = IntersectRect(pr);
	SetClipRect(ir);
}

bool PG_Widget::LoadLayout(const std::string& name) {
	bool rc = PG_Layout::Load(this, name, NULL, NULL);
	Update();
	return rc;
}

bool PG_Widget::LoadLayout(const std::string& name, void (* WorkCallback)(int now, int max)) {
	bool rc = PG_Layout::Load(this, name, WorkCallback, NULL);
	Update();
	return rc;

}

bool PG_Widget::LoadLayout(const std::string& name, void (* WorkCallback)(int now, int max),void *UserSpace) {
	bool rc = PG_Layout::Load(this, name, WorkCallback, UserSpace);
	Update();
	return rc;
}

void PG_Widget::SetUserData(void *userdata, int size) {
	_mid->userdata = new char[size];
	memcpy(_mid->userdata, userdata, size);
	_mid->userdatasize = size;
}

int PG_Widget::GetUserDataSize() {
	return _mid->userdatasize;
}

void PG_Widget::GetUserData(void *userdata) {
	if (_mid->userdata == NULL)
		return;

	memcpy(userdata, _mid->userdata, _mid->userdatasize);
}

void PG_Widget::ReleaseUserData() {
	if (_mid->userdata != NULL)
		delete[] _mid->userdata;
	_mid->userdatasize = 0;
}

void PG_Widget::AddText(const std::string& text, bool update) {
	my_text += text;
	_mid->widthText = TXT_HEIGHT_UNDEF;
	_mid->heightText = TXT_HEIGHT_UNDEF;

	//TO-DO : Optimalize this !!! - because of widget functions overloading SetText()
	if (update) {
		SetText(GetText());
	}
}

void PG_Widget::SetText(const std::string& text) {

	_mid->widthText = TXT_HEIGHT_UNDEF;
	_mid->heightText = TXT_HEIGHT_UNDEF;

	my_text = text;
	Update();
}

void PG_Widget::SetTextFormat(const char* text, ...) {
	va_list ap;
	va_start(ap, text);
	char temp[256];

	if(text == NULL) {
		my_text = "";
		return;
	}

	if(text[0] == 0) {
		my_text = "";
		return;
	}

	vsprintf(temp, text, ap);
	SetText(temp);
	va_end(ap);
}

void PG_Widget::SetFontColor(const PG_Color& Color, bool bRecursive) {
	_mid->font->SetColor(Color);

	if(!bRecursive || (GetChildList() == NULL)) {
		return;
	}

	for(PG_Widget* i = GetChildList()->first(); i != NULL; i = i->next()) {
		i->SetFontColor(Color, true);
	}
}

void PG_Widget::SetFontHighlightColor(const PG_Color& Color, bool bRecursive) {
   _mid->font->SetHighlightColor(Color);

   if(!bRecursive || (GetChildList() == NULL)) {
      return;
   }

   for(PG_Widget* i = GetChildList()->first(); i != NULL; i = i->next()) {
      i->SetFontHighlightColor(Color, true);
   }
}


void PG_Widget::SetFontAlpha(int Alpha, bool bRecursive) {
	_mid->font->SetAlpha(Alpha);

	if(!bRecursive || (GetChildList() == NULL)) {
		return;
	}

	for(PG_Widget* i = GetChildList()->first(); i != NULL; i = i->next()) {
		i->SetFontAlpha(Alpha, true);
	}
}

void PG_Widget::SetFontStyle(PG_Font::Style Style, bool bRecursive) {
	_mid->font->SetStyle(Style);

	if(!bRecursive || (GetChildList() == NULL)) {
		return;
	}

	for(PG_Widget* i = GetChildList()->first(); i != NULL; i = i->next()) {
		i->SetFontStyle(Style, true);
	}
}

int PG_Widget::GetFontSize() {
	return _mid->font->GetSize();
}

void PG_Widget::SetFontSize(int Size, bool bRecursive) {
	_mid->font->SetSize(Size);

	if(!bRecursive || (GetChildList() == NULL)) {
		return;
	}

	for(PG_Widget* i = GetChildList()->first(); i != NULL; i = i->next()) {
		i->SetFontSize(Size, true);
	}

}

void PG_Widget::SetFontIndex(int Index, bool bRecursive) {
	//	_mid->font->SetIndex(Index);
}

void PG_Widget::SetFontName(const std::string& Name, bool bRecursive) {
	_mid->font->SetName(Name);

	if(!bRecursive || (GetChildList() == NULL)) {
		return;
	}

	for(PG_Widget* i = GetChildList()->first(); i != NULL; i = i->next()) {
		i->SetFontName(Name, true);
	}

}

void PG_Widget::SetSizeByText(int Width, int Height, const std::string& Text) {
	Uint16 w,h;
	int baselineY;

	if (Text.empty()) {
		if (!PG_FontEngine::GetTextSize(my_text, _mid->font, &w, &h, &baselineY)) {
			return;
		}
	} else {
		PG_String ytext = Text;
		if (!PG_FontEngine::GetTextSize(ytext, _mid->font, &w, &h, &baselineY)) {
			return;
		}
	}

	if (my_width == 0 && my_height > 0 && Width == 0) {
		my_width = w;
		my_ypos += (my_height - h - baselineY) >> 1;
		my_height = h + baselineY;
	} else if (my_height == 0 && my_width > 0 && Height == 0) {
		my_xpos += (my_width - w) >> 1;
		my_width = w;
		my_height = h + baselineY;
	} else {
		my_width = w + Width;
		my_height = h + Height + baselineY;
	}

}

void PG_Widget::SetFont(PG_Font* font) {
	if(_mid->font != NULL) {
		delete _mid->font;
	}

	_mid->font = new PG_Font(font->GetName(), font->GetSize());
}

void PG_Widget::GetTextSize(Uint16& w, Uint16& h, const std::string& text) {
	if(text.empty()) {
		if(_mid->widthText != TXT_HEIGHT_UNDEF) {
			w = _mid->widthText;
			h = _mid->heightText;
			return;
		}
		GetTextSize(w, h, my_text, _mid->font);
		_mid->widthText = w;
		_mid->heightText = h;
		return;
	}

	GetTextSize(w, h, text, _mid->font);
}

void PG_Widget::GetTextSize(Uint16& w, Uint16& h, const PG_String& text, PG_Font* font) {
   PG_Char c = PG_Application::GetHighlightingTag();
   if ( !c || text.find( c) == PG_String::npos )
      PG_FontEngine::GetTextSize(text, font, &w);
   else {
      PG_String t = text;
      PG_String::size_type pos = t.find( c);
      while ( pos != PG_String::npos )  {
         t.erase( pos, 1 );
         pos = t.find( c);
      }
      PG_FontEngine::GetTextSize(t, font, &w);
   }
      
	h = font->GetFontHeight();
}

int PG_Widget::GetTextWidth() {

	if(_mid->widthText != TXT_HEIGHT_UNDEF) {
		return _mid->widthText;
	}

	GetTextSize(_mid->widthText, _mid->heightText);

	return _mid->widthText;
}

int PG_Widget::GetTextHeight() {
	return _mid->font->GetFontAscender();
}

bool PG_Widget::RenderText(SDL_Surface *Surface, const PG_Rect& ClipRect, int BaseLineX, int BaseLineY, const PG_String& Text, PG_Font* ParamIn)
{
   PG_Char c = PG_Application::GetHighlightingTag();
   if ( !c || Text.find( c) == PG_String::npos )
      return PG_FontEngine::RenderText( Surface, ClipRect, BaseLineX, BaseLineY, Text, ParamIn );
   else {

      PG_Font param = *ParamIn;

      bool highLight = false;
      
      int pos = 0;
      PG_String::size_type rendered = 0;
      PG_String::size_type length = Text.length();
      if ( !length )
         return true;
      
      do {
         PG_String::size_type i = Text.find( c, rendered );
         if ( i > rendered || i == PG_String::npos ) {
            PG_String subtext;
            if ( i == PG_String::npos ) {
               subtext = Text.substr( rendered );
               rendered = length;
            } else {
               subtext = Text.substr( rendered, i - rendered );
               rendered = i + 1;
            }

            if ( highLight )
               param.SetColor( ParamIn->GetHighlightColor() );
            else
               param.SetColor( ParamIn->GetColor() );

            PG_FontEngine::RenderText( Surface, ClipRect, BaseLineX + pos, BaseLineY, subtext, &param );
            
            Uint16 width;
            PG_FontEngine::GetTextSize( subtext, ParamIn, &width );
            pos += width;
               
         } else
            if ( i == rendered )
               ++rendered; // we skip the tag character
         highLight = !highLight;
      } while ( rendered < length );
   }
   return true;
      
}


void PG_Widget::DrawText(const PG_Rect& rect, const PG_String& text) {
	if(my_srfObject == NULL) {
		RenderText(PG_Application::GetScreen(), _mid->rectClip, my_xpos+ rect.x, my_ypos + rect.y + GetFontAscender(), text, _mid->font);
	} else {
		RenderText(my_srfObject, PG_Rect(0,0,Width(),Height()), rect.x, rect.y + GetFontAscender(), text, _mid->font);
	}
}

void PG_Widget::DrawText(int x, int y, const PG_String& text) {
	DrawText(PG_Rect(x,y,w-x,h-y), text);
}

void PG_Widget::DrawText(int x, int y, const PG_String& text, const PG_Rect& cliprect) {
	if(my_srfObject == NULL) {
		PG_Rect rect = cliprect;
		rect.x += my_xpos;
		rect.y += my_ypos;
		//		PG_Rect r = this->IntersectRect(rect);
		PG_FontEngine::RenderText(PG_Application::GetScreen(), rect, my_xpos + x, my_ypos + y + GetFontAscender(), text, _mid->font);
	} else {
		//		PG_Rect rect = this->IntersectRect(cliprect);
		PG_FontEngine::RenderText(my_srfObject, cliprect, x, y + GetFontAscender(), text, _mid->font);
	}
}

void PG_Widget::DrawText(const PG_Rect& rect, const PG_String& text, const PG_Color& c) {
	SetFontColor(c);
	DrawText(rect, text);
}

void PG_Widget::DrawText(int x, int y, const PG_String& text, const PG_Color& c) {
	DrawText(PG_Rect(x,y,w,h), text, c);
}

bool PG_Widget::QuitModal() {
	return eventQuitModal(GetID(), this, 0);
}

bool PG_Widget::WillQuitModal() {
	return _mid->quitModalLoop;
}

void PG_Widget::StopQuitModal() {
	_mid->quitModalLoop = false;
}

int PG_Widget::RunModal() {
	SDL_Event event;
	_mid->quitModalLoop = false;

	//PG_Application::FlushEventQueue();

	// run while in modal mode
	while(!_mid->quitModalLoop) {
		if( PG_Application::GetApp()->GetAppIdleCallsEnabled () ) {
			if ( PG_Application::GetEventSupplier()->PollEvent(&event) == 0) {
				PG_Application::GetApp()->sigAppIdle( );
				SDL_Delay(1);
				continue;
			}
		} else {
			if( PG_Application::GetEventSupplier()->WaitEvent(&event) != 1) {
				SDL_Delay(10);
				continue;
			}
		}
		PG_Application::ClearOldMousePosition();
		ProcessEvent(&event, true);
		PG_Application::DrawCursor();
	}

	return _mid->modalstatus;
}

bool PG_Widget::eventQuitModal(int id, PG_MessageObject* widget, unsigned long data) {
	_mid->quitModalLoop = true;
	return true;
}

const PG_String& PG_Widget::GetText() {
	return my_text;
}

void PG_Widget::eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst) {

	// Don't blit an object without a surface

	if(srf == NULL) {
		return;
	}

	// Set alpha
	Uint8 a = 255-_mid->transparency;
	if(a != 0) {
		SDL_SetAlpha(srf, SDL_SRCALPHA, a);

		// Blit widget surface to screen
#ifdef DEBUG

		PG_LogDBG("SRC BLIT: x:%d y:%d w:%d h:%d",src.x,src.y,src.w,src.h);
		PG_LogDBG("DST BLIT: x:%d y:%d w:%d h:%d",dst.x,dst.y,dst.w,dst.h);
#endif // DEBUG

	   PG_Application::ScreenLocker locker(true);
		PG_Draw::BlitSurface(srf, src, PG_Application::GetScreen(), dst);
	}
}

void PG_Widget::DrawBorder(const PG_Rect& r, int size, bool up) {
	int i0, i1;

	if(!IsVisible()) {
		return;
	}

	i0 = (up) ? 0 : 1;
	i1 = (up) ? 1 : 0;

	// outer frame
	if (size >= 1) {
		DrawHLine(r.x + 0, r.y + 0, r.w, my_colorBorder[i0][0]);
		DrawVLine(r.x + 0, r.y + 0, r.h - 1, my_colorBorder[i0][0]);

		DrawHLine(r.x + 0, r.y + r.h - 1, r.w - 1, my_colorBorder[i1][0]);
		DrawVLine(r.x + r.w - 1, r.y + 1, r.h - 1, my_colorBorder[i1][0]);
	}
	// inner frame
	if (size >= 2) {
		DrawHLine(r.x + 1, r.y + 1, r.w - 1, my_colorBorder[i0][1]);
		DrawVLine(r.x + 1, r.y + 1, r.h - 2, my_colorBorder[i0][1]);

		DrawHLine(r.x + 1, r.y + r.h - 2, r.w - 2, my_colorBorder[i1][1]);
		DrawVLine(r.x + r.w - 2, r.y + 2, r.h - 2, my_colorBorder[i1][1]);
	}
}

void PG_Widget::SetTransparency(Uint8 t, bool bRecursive) {
	_mid->transparency = t;

	if(!bRecursive || (GetChildList() == NULL)) {
		return;
	}

	for(PG_Widget* i = GetChildList()->first(); i != NULL; i = i->next()) {
		i->SetTransparency(t, true);
	}
}

void PG_Widget::SetClipRect( const PG_Rect& r) {
	_mid->rectClip = r;
}

void PG_Widget::GetClipRects(PG_Rect& src, PG_Rect& dst, const PG_Rect& rect) {

	dst = IntersectRect(_mid->rectClip, rect);

	int dx = dst.my_xpos - rect.my_xpos;
	int dy = dst.my_ypos - rect.my_ypos;

	if(dx < 0) {
		dx = 0;
	}

	if(dy < 0) {
		dy = 0;
	}

	src.my_xpos = dx;
	src.my_ypos = dy;
	src.my_width = dst.my_width;
	src.my_height = dst.my_height;
}

void PG_Widget::SetPixel(int x, int y, const PG_Color& c) {
	static PG_Point p;

	if(my_srfObject == NULL) {
		p.x = my_xpos + x;
		p.y = my_ypos + y;
		if(_mid->rectClip.IsInside(p)) {
			PG_Draw::SetPixel(p.x, p.y, c, PG_Application::GetScreen());
		}
	} else {
		PG_Draw::SetPixel(x, y, c, my_srfObject);
	}
}

void PG_Widget::DrawHLine(int x, int y, int w, const PG_Color& color) {
	static PG_Rect rect;
	SDL_Surface* surface = my_srfObject;

   PG_Application::ScreenLocker locker;
	
	if(my_srfObject == NULL) {
		surface = PG_Application::GetScreen();
		locker.lock();
	}

	x += my_xpos;
	y += my_ypos;

	if((y < _mid->rectClip.y) || (y >= (_mid->rectClip.y+_mid->rectClip.h))) {
		return;
	}

	// clip to widget cliprect
	int x0 = PG_MAX(x, _mid->rectClip.x);
	int x1 = PG_MIN(x+w, _mid->rectClip.x+_mid->rectClip.w);
	Uint32 c = color.MapRGB(surface->format);

	int wl = (x1-x0);

	if(wl <= 0) {
		return;
	}

	if(my_srfObject != NULL) {
		x0 -= my_xpos;
		y -= my_ypos;
	}

	rect.SetRect(x0, y, wl, 1);
	SDL_FillRect(surface, &rect, c);

}

void PG_Widget::DrawVLine(int x, int y, int h, const PG_Color& color) {
	static PG_Rect rect;
	SDL_Surface* surface = my_srfObject;

   PG_Application::ScreenLocker locker;
	if(my_srfObject == NULL) {
		surface = PG_Application::GetScreen();
		locker.lock();
	}

	x += my_xpos;
	y += my_ypos;

	if((x < _mid->rectClip.x) || (x >= (_mid->rectClip.x+_mid->rectClip.w))) {
		return;
	}

	// clip to widget cliprect
	int y0 = PG_MAX(y, _mid->rectClip.y);
	int y1 = PG_MIN(y+h, _mid->rectClip.y+_mid->rectClip.h);
	Uint32 c = color.MapRGB(surface->format);

	int hl = (y1-y0);

	if(hl <= 0) {
		return;
	}

	if(my_srfObject != NULL) {
		y0 -= my_ypos;
		x -= my_xpos;
	}

	rect.SetRect(x, y0, 1, hl);
	SDL_FillRect(surface, &rect, c);
}

/**  */
void PG_Widget::DrawRectWH(int x, int y, int w, int h, const PG_Color& c) {

	DrawHLine(x, y, w, c);
	DrawHLine(x, y + h - 1, w, c);
	DrawVLine(x, y, h, c);
	DrawVLine(x + w - 1, y, h, c);

}

void PG_Widget::DrawLine(Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, const PG_Color& color, Uint8 width) {
	SDL_Surface* surface = my_srfObject;

	if(surface == NULL) {
		surface = PG_Application::GetScreen();
		x0 += my_xpos;
		y0 += my_ypos;
		x1 += my_xpos;
		y1 += my_ypos;
	}

	PG_Draw::DrawLine(surface, x0, y0, x1, y1, color, width);
}

void PG_Widget::eventDraw(SDL_Surface* surface, const PG_Rect& rect) {}

void PG_Widget::eventMoveWidget(int x, int y) {}

/*void PG_Widget::eventMoveWindow(int x, int y) {
}*/

/*void PG_Widget::eventSizeWindow(Uint16 w, Uint16 h) {
}*/

void PG_Widget::eventSizeWidget(Uint16 w, Uint16 h) {}

SDL_Surface* PG_Widget::GetWidgetSurface() {
	return my_srfObject;
}

/*SDL_Surface* PG_Widget::GetScreenSurface() {
	return my_srfScreen;
}*/

bool PG_Widget::IsVisible() {
	return _mid->visible;
}

PG_Widget* PG_Widget::GetParent() {
	return _mid->widgetParent;
}

int PG_Widget::GetID() {
	return _mid->id;
}

PG_Widget* PG_Widget::FindChild(int id, bool recursive) {
	if(_mid->childList == NULL) {
		return NULL;
	}
	return _mid->childList->Find(id, recursive );
}

PG_Widget* PG_Widget::FindChild(const std::string& name, bool recursive) {
	if(_mid->childList == NULL) {
		return NULL;
	}
	return _mid->childList->Find(name, recursive );
}

PG_RectList* PG_Widget::GetChildList() {
	return _mid->childList;
}

int PG_Widget::GetChildCount() {
	return _mid->childList ? _mid->childList->size() : 0;
}

PG_RectList* PG_Widget::GetWidgetList() {
	return &widgetList;
}

void PG_Widget::SetName(const std::string& name) {
	_mid->name = name;
}

const std::string& PG_Widget::GetName() {
	return _mid->name;
}

int PG_Widget::GetFontAscender() {
	return _mid->font->GetFontAscender();
}

int PG_Widget::GetFontHeight() {
	return _mid->font->GetFontHeight();
}

PG_Color PG_Widget::GetFontColor() {
	return _mid->font->GetColor();
}

PG_Color PG_Widget::GetFontHighlightColor() {
   return _mid->font->GetHighlightColor();
}


PG_Font* PG_Widget::GetFont() {
	return _mid->font;
}

Uint8 PG_Widget::GetTransparency() {
	return _mid->transparency;
}

PG_Rect* PG_Widget::GetClipRect() {
	RecalcClipRect();
	return &_mid->rectClip;
}

bool PG_Widget::IsClippingEnabled() {
	return ((_mid->rectClip.my_width != my_width) || (_mid->rectClip.my_height != my_height));
}

void PG_Widget::GetClipRects(PG_Rect& src, PG_Rect& dst) {
	GetClipRects(src, dst, *this);
}

void PG_Widget::SetID(int id) {
	_mid->id = id;
}

void PG_Widget::SetDirtyUpdate(bool bDirtyUpdate) {
	if(PG_Application::GetDirtyUpdatesDisabled()) {
		_mid->dirtyUpdate = false;
		return;
	}

	_mid->dirtyUpdate = bDirtyUpdate;
}

void PG_Widget::UpdateOverlappingSiblings( bool enable, bool recursive ) {
	_mid->updateOverlappingSiblings = enable;

	if ( recursive && GetChildList() )
		for(PG_Widget* i = GetChildList()->first(); i != NULL; i = i->next())
			i->UpdateOverlappingSiblings(enable, true);
}


bool PG_Widget::GetDirtyUpdate() {
	return _mid->dirtyUpdate;
}

void PG_Widget::SetHidden(bool hidden) {
	_mid->hidden = hidden;
}


bool PG_Widget::IsHidden() {
	return _mid->hidden;
}

void PG_Widget::SetParent(PG_Widget* parent) {
	_mid->widgetParent = parent;
}

void PG_Widget::SetModalStatus(int status) {
	_mid->modalstatus = status;
}

void PG_Widget::EnableReceiver(bool enable, bool bRecursive) {
	PG_MessageObject::EnableReceiver(enable);

	if(!bRecursive || (GetChildList() == NULL)) {
		return;
	}

	for(PG_Widget* i = GetChildList()->first(); i != NULL; i = i->next()) {
		i->EnableReceiver(enable, true);
	}
}

void PG_Widget::SetBorderSize(int b) {
	my_bordersize = b;
}

int PG_Widget::GetBorderSize() {
	return my_bordersize;
}

void PG_Widget::activateHotkey( int keymodifier )
{
   _mid->hotkeyModifier = keymodifier;

   // I don't like the way keypresses are NOT propagated to child in non-modal operation mode,
   // but changing that would be a major change to Paraguis event handling
   // So as a workaround we just register at the global signal 
   PG_Application::GetApp()->sigKeyDown.connect(  sigc::mem_fun( *this, &PG_Widget::propagateKeyDown ));
}

bool PG_Widget::propagateKeyDown( PG_MessageObject* object, const SDL_KeyboardEvent* event ) {
	return eventKeyDown( event );
}

int PG_Widget::getHotkeyModifier()
{
   return _mid->hotkeyModifier ;
}


bool PG_Widget::checkForHotkey( const SDL_KeyboardEvent* key )
{
   if ( !_mid->hotkey )
      return false;

   if ( _mid->hotkeyModifier < 0 )
      return false;

   if ( key->type != SDL_KEYDOWN )
      return false;

   int mod = key->keysym.mod & ~( KMOD_NUM | KMOD_CAPS );

   if( (mod & _mid->hotkeyModifier) || (mod == _mid->hotkeyModifier) )
      if( key->keysym.unicode == _mid->hotkey )
         return true;

   return false;
}

PG_Char PG_Widget::extractHotkey( const std::string& s )
{
   if ( !PG_Application::GetHighlightingTag() )
      return 0;
   
   bool found = false;
   for ( unsigned int i = 0; i < s.length(); ++i ) {
      if ( found ) 
         return tolower(s[i]);
      
      if ( s[i] == PG_Application::GetHighlightingTag() )
         found = true;
   }
   return 0;
}


bool PG_Widget::extractAndStoreHotkey( const std::string& s )
{
   PG_Char c = extractHotkey( s );
   if ( !c )
      return false;
   else {
      _mid->hotkey = c;
      return true;
   }
}
