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
    Update Date:      $Date: 2008-04-20 16:44:33 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgbutton.cpp,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

#include "pgbutton.h"
#include "pgapplication.h"
#include "pgthemewidget.h"
#include "pglog.h"
#include "pgdraw.h"
#include "pgtheme.h"

#include "propstrings_priv.h"

PG_Button::SignalButtonClick<> PG_Button::sigGlobalClick;

class PG_ButtonStateData {
public:
	PG_ButtonStateData() : srf(NULL), srf_icon(NULL), bordersize(1), transparency(0),
	background(NULL), backMode(PG_Draw::TILE), backBlend(0) {}
	;

	SDL_Surface* srf;
	SDL_Surface* srf_icon;
	Uint8 bordersize;
	Uint8 transparency;
	PG_Gradient gradState;
	SDL_Surface* background;
	PG_Draw::BkMode backMode;
	int backBlend;
};

class PG_ButtonDataInternal : public std::map<PG_Button::STATE, PG_ButtonStateData> {
public:

	PG_ButtonDataInternal() : free_icons(false), isPressed(false), togglemode(false), state(PG_Button::UNPRESSED), pressShift(1),
	iconindent(3), behaviour( PG_Button::SIGNALONRELEASE | PG_Button::MSGCAPTURE ) {}
	;

	bool free_icons;
	bool isPressed;
	bool togglemode;
	PG_Button::STATE state;
	int pressShift;
	Uint16 iconindent;
	int behaviour;
};

PG_Button::PG_Button(PG_Widget* parent, const PG_Rect& r, const std::string& text, int id, const std::string& style) : PG_Widget(parent, r) {
	SetDirtyUpdate(false);

	_mid = new PG_ButtonDataInternal;

	SetText(text);
	SetID(id);

	LoadThemeStyle(style);
}

PG_Button::~PG_Button() {
	FreeSurfaces();
	FreeIcons();

	delete _mid;
}

void PG_Button::LoadThemeStyle(const std::string& widgettype) {
	LoadThemeStyle(PG_PropStr::Button, PG_PropStr::Button);
	if(widgettype != PG_PropStr::Button) {
		LoadThemeStyle(widgettype, PG_PropStr::Button);
	}
}

void PG_Button::LoadThemeStyle(const std::string& widgettype, const std::string& objectname) {
	PG_Theme* t = PG_Application::GetTheme();

	PG_Color fontcolor = GetFontColor();
	t->GetColor(widgettype, objectname, PG_PropStr::textcolor, fontcolor);
	SetFontColor(fontcolor);

	switch (GetID()) {
		case OK:
			SetIcon(t->FindSurface(widgettype, objectname, "ok_icon"));
			break;

		case YES:
			SetIcon(t->FindSurface(widgettype, objectname, "yes_icon"));
			break;

		case NO:
			SetIcon(t->FindSurface(widgettype, objectname, "no_icon"));
			break;

		case APPLY:
			SetIcon(t->FindSurface(widgettype, objectname, "apply_icon"));
			break;

		case CANCEL:
			SetIcon(t->FindSurface(widgettype, objectname, "cancel_icon"));
			break;

		case CLOSE:
			SetIcon(t->FindSurface(widgettype, objectname, "close_icon"));
			break;

		case HELP:
			SetIcon(t->FindSurface(widgettype, objectname, "help_icon"));
			break;

		default:
			SetIcon(
			    t->FindSurface(widgettype, objectname, PG_PropStr::iconup),
			    t->FindSurface(widgettype, objectname, PG_PropStr::icondown),
			    t->FindSurface(widgettype, objectname, PG_PropStr::iconover)
			);
			break;
	}

	PG_Gradient* g;
	g = t->FindGradient(widgettype, objectname, PG_PropStr::gradient0);
	if(g) {
		(*_mid)[UNPRESSED].gradState = *g;
	}

	g = t->FindGradient(widgettype, objectname, PG_PropStr::gradient1);
	if(g) {
		(*_mid)[HIGHLITED].gradState = *g;
	}

	g = t->FindGradient(widgettype, objectname, PG_PropStr::gradient2);
	if(g) {
		(*_mid)[PRESSED].gradState = *g;
	}

	SDL_Surface* background;
	background = t->FindSurface(widgettype, objectname, PG_PropStr::background0);
	t->GetProperty(widgettype, objectname, PG_PropStr::backmode0, (*_mid)[UNPRESSED].backMode);
	SetBackground(UNPRESSED, background, (*_mid)[UNPRESSED].backMode);

	background = t->FindSurface(widgettype, objectname, PG_PropStr::background1);
	t->GetProperty(widgettype, objectname, PG_PropStr::backmode1, (*_mid)[PRESSED].backMode);
	SetBackground(PRESSED, background, (*_mid)[PRESSED].backMode);

	background = t->FindSurface(widgettype, objectname, PG_PropStr::background2);
	t->GetProperty(widgettype, objectname, PG_PropStr::backmode2, (*_mid)[HIGHLITED].backMode);
	SetBackground(HIGHLITED, background, (*_mid)[HIGHLITED].backMode);

	t->GetProperty(widgettype, objectname, PG_PropStr::blend0, (*_mid)[UNPRESSED].backBlend);
	t->GetProperty(widgettype, objectname, PG_PropStr::blend1, (*_mid)[PRESSED].backBlend);
	t->GetProperty(widgettype, objectname, PG_PropStr::blend2, (*_mid)[HIGHLITED].backBlend);

	t->GetProperty(widgettype, objectname, PG_PropStr::shift, _mid->pressShift);

	t->GetProperty(widgettype, objectname, PG_PropStr::bordersize, (*_mid)[UNPRESSED].bordersize);
	t->GetProperty(widgettype, objectname, PG_PropStr::bordersize, (*_mid)[PRESSED].bordersize);
	t->GetProperty(widgettype, objectname, PG_PropStr::bordersize, (*_mid)[HIGHLITED].bordersize);

	t->GetProperty(widgettype, objectname, PG_PropStr::bordersize0, (*_mid)[UNPRESSED].bordersize);
	t->GetProperty(widgettype, objectname, PG_PropStr::bordersize1, (*_mid)[PRESSED].bordersize);
	t->GetProperty(widgettype, objectname, PG_PropStr::bordersize2, (*_mid)[HIGHLITED].bordersize);

	t->GetProperty(widgettype, objectname, PG_PropStr::transparency0, (*_mid)[UNPRESSED].transparency);
	t->GetProperty(widgettype, objectname, PG_PropStr::transparency1, (*_mid)[PRESSED].transparency);
	t->GetProperty(widgettype, objectname, PG_PropStr::transparency2, (*_mid)[HIGHLITED].transparency);

	t->GetProperty(widgettype, objectname, PG_PropStr::iconindent, _mid->iconindent);

	const std::string& s = t->FindString(widgettype, objectname, PG_PropStr::label);
	if(!s.empty()) {
		SetText(s);
	}

	PG_Widget::LoadThemeStyle(widgettype, objectname);
}

void PG_Button::SetBorderColor(int b, const PG_Color& color) {
	my_colorBorder[b][0] = color;
}

/**  */
void PG_Button::eventSizeWidget(Uint16 w, Uint16 h) {
	FreeSurfaces();
}

/**  */
void PG_Button::FreeSurfaces() {
	PG_ThemeWidget::DeleteThemedSurface((*_mid)[UNPRESSED].srf);
	(*_mid)[UNPRESSED].srf = NULL;

	PG_ThemeWidget::DeleteThemedSurface((*_mid)[HIGHLITED].srf);
	(*_mid)[HIGHLITED].srf = NULL;

	PG_ThemeWidget::DeleteThemedSurface((*_mid)[PRESSED].srf);
	(*_mid)[PRESSED].srf = NULL;
}

void PG_Button::FreeIcons() {

	if(!_mid->free_icons) {
		return;
	}

	if((*_mid)[UNPRESSED].srf_icon) {
		PG_Application::UnloadSurface((*_mid)[UNPRESSED].srf_icon); // false
		(*_mid)[UNPRESSED].srf_icon = NULL;
	}

	if((*_mid)[HIGHLITED].srf_icon) {
		PG_Application::UnloadSurface((*_mid)[HIGHLITED].srf_icon); // false
		(*_mid)[HIGHLITED].srf_icon = NULL;
	}

	if((*_mid)[PRESSED].srf_icon) {
		PG_Application::UnloadSurface((*_mid)[PRESSED].srf_icon); // false
		(*_mid)[PRESSED].srf_icon = NULL;
	}

	_mid->free_icons = false;
}

/**  */
void PG_Button::eventMouseEnter() {
	if (!(_mid->togglemode && _mid->isPressed)) {
		_mid->state = HIGHLITED;
	}

	Update();
	PG_Widget::eventMouseEnter();
}

/**  */
void PG_Button::eventMouseLeave() {

	if(_mid->state == HIGHLITED || !(_mid->behaviour & MSGCAPTURE) ) {
		(_mid->togglemode && _mid->isPressed) ? _mid->state = PRESSED : _mid->state = UNPRESSED;
	}

	Update();
	PG_Widget::eventMouseLeave();
}

/**  */
bool PG_Button::eventMouseButtonDown(const SDL_MouseButtonEvent* button) {
	if (!button)
		return false;

	if(button->button == 1) {
		_mid->state = PRESSED;

		if ( _mid->behaviour & MSGCAPTURE ) {
			SetCapture();
		}

		Update();

		if ( _mid->behaviour & SIGNALONCLICK ) {
			sigGlobalClick(this);
			sigClick(this);
		}

		return true;
	}

	return false;
}

/**  */
bool PG_Button::eventMouseButtonUp(const SDL_MouseButtonEvent* button) {
	if (!button)
		return false;

	if(button->button != 1) {
		return false;
	}

	//if the user moved the cursor out of the button, ignore the click
	if (!IsMouseInside()) {
		if (!_mid->togglemode || !_mid->isPressed) {
			_mid->state = UNPRESSED;
		}

		if ( _mid->behaviour & MSGCAPTURE )
			ReleaseCapture();

		Update();
		return false;
	}

	if(_mid->togglemode) {
		if(!_mid->isPressed) {
			_mid->state = PRESSED;
			_mid->isPressed = true;
		} else {
			_mid->state = HIGHLITED;
			_mid->isPressed = false;
		}
	} else {
		_mid->state = HIGHLITED;
		_mid->isPressed = false;
	}

	if ( _mid->behaviour & MSGCAPTURE )
		ReleaseCapture();

	Update();

	if ( _mid->behaviour & SIGNALONRELEASE ) {
		sigGlobalClick(this);
		sigClick(this);
	}

	return true;
}

/**  */
bool PG_Button::SetIcon(const std::string& filenameup, const std::string& filenamedown, const std::string& filenameover, const PG_Color& colorkey) {
	if(!SetIcon(filenameup, filenamedown, filenameover)) {
		return false;
	}

	if((*_mid)[UNPRESSED].srf_icon != NULL) {
		SDL_SetColorKey((*_mid)[UNPRESSED].srf_icon, SDL_SRCCOLORKEY, colorkey);
	}

	if((*_mid)[HIGHLITED].srf_icon != NULL) {
		SDL_SetColorKey((*_mid)[HIGHLITED].srf_icon, SDL_SRCCOLORKEY, colorkey);
	}

	if((*_mid)[PRESSED].srf_icon != NULL) {
		SDL_SetColorKey((*_mid)[PRESSED].srf_icon, SDL_SRCCOLORKEY, colorkey);
	}

	return true;
}

bool PG_Button::SetIcon(const std::string& filenameup, const std::string& filenamedown, const std::string& filenameover) {
	SDL_Surface* icon0 = PG_Application::LoadSurface(filenameup);
	SDL_Surface* icon1 = PG_Application::LoadSurface(filenameover);
	SDL_Surface* icon2 = PG_Application::LoadSurface(filenamedown);

	if(icon0 == NULL) {
		return false;
	}

	FreeIcons();

	(*_mid)[UNPRESSED].srf_icon = icon0;
	(*_mid)[HIGHLITED].srf_icon = icon1;
	(*_mid)[PRESSED].srf_icon = icon2;
	_mid->free_icons = true;

	Update();
	return true;
}


/**  */
bool PG_Button::SetIcon(SDL_Surface* icon_up, SDL_Surface* icon_down,SDL_Surface* icon_over, bool freeSurfaces) {

	if(!icon_up && !icon_down && !icon_over) {
		return false;
	}

	FreeIcons();

	(*_mid)[UNPRESSED].srf_icon = icon_up;
	(*_mid)[HIGHLITED].srf_icon = icon_over;
	(*_mid)[PRESSED].srf_icon = icon_down;

   _mid->free_icons = freeSurfaces;

	return true;
}

SDL_Surface* PG_Button::GetIcon(STATE num) {
	return (*_mid)[num].srf_icon;
}

/**  */
void PG_Button::SetBorderSize(int norm, int pressed, int high) {

	if(norm >= 0) {
		(*_mid)[UNPRESSED].bordersize = norm;
	}

	if(pressed >= 0) {
		(*_mid)[PRESSED].bordersize = pressed;
	}

	if(high >= 0) {
		(*_mid)[HIGHLITED].bordersize = high;
	}
}

/**  */
void PG_Button::SetToggle(bool bToggle) {
	_mid->togglemode = bToggle;
}

/**  */
void PG_Button::SetPressed(bool pressed) {
	if(!_mid->togglemode)
		return;

	_mid->isPressed = pressed;
	_mid->state = (_mid->isPressed ? PRESSED : UNPRESSED);

	Update();
}
void PG_Button::SetTransparency(Uint8 t, bool bRecursive) {
	(*_mid)[UNPRESSED].transparency = t;
	(*_mid)[PRESSED].transparency = t;
	(*_mid)[HIGHLITED].transparency = t;

	if(!bRecursive || (GetChildList() == NULL)) {
		return;
	}

	for(PG_Widget* i = GetChildList()->first(); i != NULL; i = i->next()) {
		i->SetTransparency(t, true);
	}
}

/**  */
void PG_Button::SetTransparency(Uint8 norm, Uint8 pressed, Uint8 high) {
	(*_mid)[UNPRESSED].transparency = norm;
	(*_mid)[PRESSED].transparency = pressed;
	(*_mid)[HIGHLITED].transparency = high;
}

/**
 * Set the moving distance of the image when we press on it
 */
void PG_Button::SetShift(int pixelshift) {
	_mid->pressShift = pixelshift;
}


void PG_Button::SetBehaviour( int behaviour ) {
	_mid->behaviour = behaviour;
}


/**  */
void PG_Button::eventButtonSurface(SDL_Surface** surface, STATE newstate, Uint16 w, Uint16 h) {
	if (!surface)
		return;

	PG_Rect r(0, 0, w, h);

	// remove the old button surface (if there are no more references)
	PG_ThemeWidget::DeleteThemedSurface(*surface);

	// create a new one
	*surface = PG_ThemeWidget::CreateThemedSurface(
	               r,
	               &((*_mid)[newstate].gradState),
	               (*_mid)[newstate].background,
	               (*_mid)[newstate].backMode,
	               (*_mid)[newstate].backBlend);
}

/**  */
void PG_Button::SetGradient(STATE state, const PG_Gradient& gradient) {
	(*_mid)[state].gradState = gradient;
}

void PG_Button::SetBackground(STATE state, SDL_Surface* background, PG_Draw::BkMode mode) {

	if(!background) {
		return;
	}

	(*_mid)[state].background = background;
	(*_mid)[state].backMode = mode;
}

/**  */
bool PG_Button::GetPressed() {
	if(_mid->togglemode) {
		return _mid->isPressed;
	} else {
		return (_mid->state == PRESSED);
	}
}

void PG_Button::eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst) {
	PG_Rect rect = *this;
	PG_Rect r;
	PG_Rect my_src, my_dst;
	SDL_Surface** surface;

	Uint8 t = 0;

	// check for surfaces
	surface = &((*_mid)[UNPRESSED].srf);
	if(*surface == NULL) {
		FreeSurfaces();

		eventButtonSurface(surface, UNPRESSED, w, h);
		if(*surface) {
			SDL_SetAlpha(*surface, SDL_SRCALPHA, 255-(*_mid)[UNPRESSED].transparency);
		}

		surface = &((*_mid)[PRESSED].srf);
		eventButtonSurface(surface, PRESSED, w, h);
		if(*surface) {
			SDL_SetAlpha(*surface, SDL_SRCALPHA, 255-(*_mid)[PRESSED].transparency);
		}

		surface = &((*_mid)[HIGHLITED].srf);
		eventButtonSurface(surface, HIGHLITED, w, h);
		if(*surface) {
			SDL_SetAlpha(*surface, SDL_SRCALPHA, 255-(*_mid)[HIGHLITED].transparency);
		}
	}

	// get the right surface for the current state
	t = (*_mid)[_mid->state].transparency;
	srf = (*_mid)[_mid->state].srf;

	// blit it
	PG_Application::ScreenLocker locker(true);

	if(t != 255) {
		SDL_SetAlpha(srf, SDL_SRCALPHA, 255-t);
		PG_Draw::BlitSurface(srf, src, PG_Application::GetScreen(), dst);
	}

	// check for icon srf
	SDL_Surface* iconsrf;
	if(_mid->state == PRESSED) {
		if((*_mid)[PRESSED].srf_icon == 0) {
			iconsrf = (*_mid)[UNPRESSED].srf_icon;
		} else {
			iconsrf = (*_mid)[PRESSED].srf_icon;
		}
	} else if(_mid->state == HIGHLITED) {
		if((*_mid)[HIGHLITED].srf_icon == 0) {
			iconsrf = (*_mid)[UNPRESSED].srf_icon;
		} else {
			iconsrf = (*_mid)[HIGHLITED].srf_icon;
		}
	} else {
		iconsrf = (*_mid)[UNPRESSED].srf_icon;
	}

	int tw = my_width;
	int shift = (((_mid->state == PRESSED) || (_mid->togglemode && _mid->isPressed)) ? 1 : 0) * _mid->pressShift;

	if(iconsrf) {

		int dx = my_text.empty() ? (rect.my_width - iconsrf->w) / 2 : _mid->iconindent;
		int dy = (rect.my_height - iconsrf->h) >> 1;

		r.my_xpos = rect.my_xpos + dx + shift;
		r.my_ypos = rect.my_ypos + dy + shift;
		r.my_width = iconsrf->w;
		r.my_height = iconsrf->h;

		// calc new cliprect for icon
		GetClipRects(my_src, my_dst, r);

		// blit the icon
		PG_Draw::BlitSurface(iconsrf, my_src, PG_Application::GetScreen(), my_dst);

		tw -= (iconsrf->w + _mid->iconindent);
	}

	// draw the text
	if(!my_text.empty()) {
		Uint16 w, h;
		GetTextSize(w, h);

		int tx = (tw - w)/2 + shift;
		int ty = ((my_height - h)/2) + shift;

		if (iconsrf) {
			tx += (iconsrf->w + _mid->iconindent);
		}

		DrawText(tx, ty, my_text);
	}

//        bool i0;
        bool i1;

	if(!_mid->togglemode) {
//		i0 = (_mid->state == PRESSED) ? true : false;
		i1 = (_mid->state == PRESSED) ? false : true;
	} else {
//		i0 = (_mid->isPressed) ? true : false;
		i1 = (_mid->isPressed) ? false : true;
	}

	DrawBorder(PG_Rect(0, 0, Width(), Height()), (*_mid)[_mid->state].bordersize, i1);
}

void PG_Button::SetBlendLevel(STATE mode, Uint8 blend) {
	(*_mid)[mode].backBlend = blend;
}

Uint8 PG_Button::GetBlendLevel(STATE mode) {
	return (*_mid)[mode].backBlend;
}

void PG_Button::SetSizeByText(int Width, int Height, const std::string& Text) {
	Width += 2 * (*_mid)[UNPRESSED].bordersize + _mid->pressShift;

	SDL_Surface* srf = (*_mid)[UNPRESSED].srf_icon;

	if (srf == NULL) {
		PG_Widget::SetSizeByText(Width, Height, Text);
		eventSizeWidget(my_width, my_height);
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

	Uint16 dx = srf->w + Width;

	my_width = (srf->w > w) ? dx : w + dx;
	my_height = PG_MAX(srf->h, h + baselineY) + Height;

	eventSizeWidget(my_width, my_height);
}

void PG_Button::SetIconIndent(Uint16 indent) {
	_mid->iconindent = indent;
}

void PG_Button::SetText(const std::string& text)
{
   extractAndStoreHotkey( text );
   PG_Widget::SetText( text );
}


bool   PG_Button::eventKeyDown (const SDL_KeyboardEvent *key)
{
   if ( checkForHotkey( key )) {
      sigClick( this );
      return true;
   }
   return false;
}

