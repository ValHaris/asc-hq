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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgwindow.cpp,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

#include "pgwindow.h"
#include "pgapplication.h"
#include "pgtheme.h"
#include "pgbutton.h"

PG_Window::PG_Window(PG_Widget* parent, const PG_Rect& r, const std::string& windowtext, WindowFlags flags, const std::string& style, int heightTitlebar)
		: PG_ThemeWidget(parent, r), my_heightTitlebar(heightTitlebar), my_moveMode(false),
my_showCloseButton((flags & PG_Window::SHOW_CLOSE) != 0), my_showMinimizeButton((flags & PG_Window::SHOW_MINIMIZE) != 0), my_moveable(true) {

	my_titlebar = new PG_ThemeWidget(this, PG_Rect(0, 0, my_width, my_heightTitlebar), style);
	my_titlebar->EnableReceiver(false);

	int w = my_width - my_heightTitlebar*2;
	if(w < 0 ) {
		w = 0;
	}
	my_labelTitle = new PG_Label(my_titlebar, PG_Rect(my_heightTitlebar, 0, w, my_heightTitlebar), windowtext, style);
	my_labelTitle->SetAlignment(PG_Label::CENTER);

	my_buttonClose = new PG_Button(my_titlebar);
	my_buttonClose->SetID(IDWINDOW_CLOSE);
	my_buttonClose->sigClick.connect(slot(*this, &PG_Window::handleButtonClick));

	my_buttonMinimize = new PG_Button(my_titlebar);
	my_buttonMinimize->SetID(IDWINDOW_MINIMIZE);
	my_buttonMinimize->sigClick.connect(slot(*this, &PG_Window::handleButtonClick));

	LoadThemeStyle(style);

	if(!my_showCloseButton) {
		my_buttonClose->Hide();
	}
	if(!my_showMinimizeButton) {
		my_buttonMinimize->Hide();
	}
}

PG_Window::~PG_Window() {}

void PG_Window::SetText(const std::string& text) {
	my_labelTitle->SetText(text);
}

void PG_Window::SetTitle(const std::string& title, PG_Label::TextAlign alignment) {
	my_labelTitle->SetAlignment(alignment);
	SetText(title);
}

const PG_String& PG_Window::GetText() {
	return my_labelTitle->GetText();
}

const PG_String& PG_Window::GetTitle() {
	return GetText();
}

void PG_Window::RecalcPositions() {
	my_titlebar->SizeWidget(my_width, my_heightTitlebar);
	int w = my_width - my_heightTitlebar*2;
	if(w < 0) {
		w = 0;
	}
	my_labelTitle->MoveWidget(PG_Rect(my_heightTitlebar, 0, w, my_heightTitlebar));
	w = my_width - my_heightTitlebar;
	if(w < 0) {
		w = 0;
	}
	my_buttonClose->MoveWidget(PG_Rect(w, 0, my_heightTitlebar, my_heightTitlebar));
	my_buttonMinimize->MoveWidget(PG_Rect(0, 0, my_heightTitlebar, my_heightTitlebar));
}

void PG_Window::LoadThemeStyle(const std::string& widgettype) {
	PG_Theme* t = PG_Application::GetTheme();

	PG_ThemeWidget::LoadThemeStyle(widgettype, "Window");

	my_titlebar->LoadThemeStyle(widgettype, "Titlebar");

	t->GetProperty(widgettype, "Titlebar", "height", my_heightTitlebar);

	PG_Color c = my_labelTitle->GetFontColor();
	t->GetColor(widgettype, "Titlebar", "textcolor", c);
	SetTitlebarColor(c);

	t->GetProperty(widgettype, "Titlebar", "showclosebutton", my_showCloseButton);
	my_buttonClose->LoadThemeStyle(widgettype, "CloseButton");

	t->GetProperty(widgettype, "Titlebar", "showminimizebutton", my_showMinimizeButton);
	my_buttonMinimize->LoadThemeStyle(widgettype, "MinimizeButton");

	if(my_showCloseButton) {
		my_buttonClose->Show();
	}
	if(my_showMinimizeButton) {
		my_buttonMinimize->Show();
	}

	RecalcPositions();
}

void PG_Window::eventSizeWidget(Uint16 w, Uint16 h) {

	PG_ThemeWidget::eventSizeWidget(w, h);

	my_width = w;
	my_height = h;

	RecalcPositions();
}

void PG_Window::eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst) {

	PG_ThemeWidget::eventBlit(srf, src, dst);

	PG_Rect client(0, my_heightTitlebar, my_width, my_height-my_heightTitlebar);
	DrawBorder(client, my_bordersize);

	client.my_xpos++;
	client.my_ypos++;
	client.my_width -= 2;
	client.my_height -= 2;

	DrawBorder(client, my_bordersize, false);
}

bool PG_Window::eventMouseButtonDown(const SDL_MouseButtonEvent* button) {
	if (!my_moveable)
		return false;

	PG_Rect ta(*this);
	ta.my_width = my_titlebar->w;
	ta.my_height = my_titlebar->h;

	int x,y;
	x = button->x;
	y = button->y;

	if((ta.my_xpos <= x) && (x <= ta.my_xpos + ta.my_width)) {
		if((ta.my_ypos <= y) && (y <= ta.my_ypos + ta.my_height)) {
			my_moveMode = true;
			my_moveDelta.x = ta.my_xpos - x;
			my_moveDelta.y = ta.my_ypos - y;
			Show();
			SetCapture();
			return true;
		}
	}

	Show();

	return false; //PG_Widget::eventMouseButtonDown(button);
}

bool PG_Window::eventMouseButtonUp(const SDL_MouseButtonEvent* button) {
	SDL_Surface* screen = PG_Application::GetScreen();
	int x,y;

	x = button->x;
	y = button->y;

	x += my_moveDelta.x;
	y += my_moveDelta.y;

	if(x < 0)
		x=0;
	if(y < 0)
		y=0;
	if(x+my_width > (screen->w - my_width))
		x = (screen->w - my_width);
	if(y+my_height > (screen->h - my_height))
		y = (screen->h - my_height);

	if(my_moveMode) {
		my_moveMode = false;
		ReleaseCapture();
		return true;
	}

	return false; //PG_Widget::eventMouseButtonUp(button);
}

bool PG_Window::eventMouseMotion(const SDL_MouseMotionEvent* motion) {
	if(!my_moveMode) {
		return PG_Widget::eventMouseMotion(motion);
	}

	SDL_Surface* screen = PG_Application::GetScreen();
	int x = motion->x;
	int y = motion->y;

	x += my_moveDelta.x;
	y += my_moveDelta.y;

	if(GetParent() != NULL) {  // Not a top-level-widget
		PG_Point pos = GetParent()->ScreenToClient(x,y);
		x = pos.x;
		y = pos.y;         // Should not be moved out of the parent:
		if(x+my_width > GetParent()->Width())
			x = GetParent()->Width() - my_width;
		if(y+my_height > GetParent()->Height())
			y = GetParent()->Height() - my_height;
	}

	if(x+my_width > screen->w)
		x = (screen->w - my_width);
	if(y+my_height > screen->h)
		y = (screen->h - my_height);

	if(x < 0)
		x=0;
	if(y < 0)
		y=0;

	MoveWidget(x,y);
	return true;
}

bool PG_Window::handleButtonClick(PG_Button* button) {
	switch(button->GetID()) {
			// close window
		case IDWINDOW_CLOSE:
			Hide();
			sigClose(this);
			break;

			// minimize window
		case IDWINDOW_MINIMIZE:
			Hide();
			sigMinimize(this);
			break;
	}

	// just in case we're modal
	QuitModal();

	return true;
}

void PG_Window::SetTitlebarColor(const PG_Color& c) {
	my_labelTitle->SetFontColor(c);
}

PG_Color PG_Window::GetTitlebarColor() {
	return my_labelTitle->GetFontColor();
}

void PG_Window::SetIcon(const std::string& filename) {
	my_labelTitle->SetIcon(filename);
}

void PG_Window::SetIcon(SDL_Surface* icon) {
	my_labelTitle->SetIcon(icon);
}

SDL_Surface* PG_Window::GetIcon() {
	return my_labelTitle->GetIcon();
}

void PG_Window::SetTitlebarHeight(Uint8 height) {
	my_heightTitlebar = height;
	RecalcPositions();
}

Uint8 PG_Window::GetTitlebarHeight() {
	return my_heightTitlebar;
}

void PG_Window::SetMoveable(bool moveable) {
	my_moveable = moveable;
}
