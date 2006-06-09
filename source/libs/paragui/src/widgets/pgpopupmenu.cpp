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
   Update Date:      $Date: 2006-06-09 19:52:40 $
   Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgpopupmenu.cpp,v $
   CVS/RCS Revision: $Revision: 1.1.2.4 $
   Status:           $State: Exp $
 */

#include "pgdraw.h"
#include "pgapplication.h"
#include "pgpopupmenu.h"
#include "pgtheme.h"
#include "pgeventsupplier.h"

#include <functional>
#include <algorithm>
#include <iostream>

/***********************************
 * MenuItem
 */
PG_PopupMenu::MenuItem::MenuItem(PG_PopupMenu *parent, const std::string& caption, int id, MI_FLAGS flags)
		: myFlags(flags),
		myParent(parent),
		mySubMenu(0),
		myId(id),
		sNormal(0),
		sSelected(0),
		sDisabled(0),
		selected(false),
      needRecalc(true) {
	initItem( caption );
	myFlags &= ~MIF_SUBMENU;
}

PG_PopupMenu::MenuItem::MenuItem(PG_PopupMenu *parent, const std::string& caption, PG_PopupMenu *submenu)
		: myFlags(MIF_SUBMENU),
		myParent(parent),
		mySubMenu(submenu),
		myId(-1),
		sNormal(0),
		sSelected(0),
		sDisabled(0),
		selected(false),
needRecalc(true) {
	initItem( caption );
}

PG_PopupMenu::MenuItem::~MenuItem() {}

void PG_PopupMenu::MenuItem::initItem( const std::string& caption ) {
	my_xpos = my_ypos = my_height = my_width = 0;
	myPoint.x = myPoint.y = 0;


	std::string::size_type tabPos = caption.find('\t');
	if ( tabPos != std::string::npos && tabPos < caption.length()-1 ) {
		myCaption  = caption.substr(0,tabPos);
		myRightCaption = caption.substr(tabPos+1 );
	} else {
		myCaption = caption;
	}

	measureItem(this);
	needRecalc = false;

	if (myCaption.empty())
		myFlags |= MIF_SEPARATOR;
}

bool PG_PopupMenu::MenuItem::measureItem(PG_Rect* rect, bool full) {

	rect->x = x;
	rect->y = y;

	if (isSeparator()) {
		rect->w = myParent->maxItemWidth();
		rect->h = 3;

		return true;
	} else if (myCaption.empty())
		return false;

	if (!needRecalc) {
		if (!full)
			rect->w = my_width;
		else
			rect->w = myParent->maxItemWidth();

		rect->h = my_height;

		return true;
	}

	Uint16 w,h;

	if ( myRightCaption.length()  ) {
		PG_Widget::GetTextSize(
		    w, h,
		    myCaption,
		    myParent->GetFont());

		rect->w = w;
		rect->h = h;

		PG_Widget::GetTextSize(
		    w, h,
		    myRightCaption,
		    myParent->GetFont());

		rect->w = rect->w + w + myParent->minTabWidth;
		if ( h > rect->h )
			rect->h = h;
	} else {
		PG_Widget::GetTextSize(
		    w, h,
		    myCaption,
		    myParent->GetFont());

		rect->w = w;
		rect->h = h;
	}

	//+++
	//    TTF_SizeText(myFont, myCaption.c_str(),
	//                 reinterpret_cast<int*>(&(rect->w)),
	//                 reinterpret_cast<int*>(&(rect->h)));

	//    rect->h += abs(TTF_FontDescent(myFont)) + (TTF_FontAscent(myFont) - rect->h);
	if (full)
		rect->w = myParent->maxItemWidth();

	return true;
}

bool PG_PopupMenu::MenuItem::isPointInside(int x, int y) {
	int posx = x - my_xpos;
	int posy = y - my_ypos;

	if ((posx >= 0) && (posx <= my_width) &&
	        (posy >= 0) && (posy <= my_height))
		return true;

	return false;
}

bool PG_PopupMenu::MenuItem::renderSurface(SDL_Surface *canvas, SDL_Surface **text, PG_Color* tcol, PG_Color* scol) {
	if (/*!*text ||*/ !canvas)
		return false;

	SDL_Rect      blitRect;
	blitRect.x = x + myParent->x;
	blitRect.y = y  + myParent->y;
	blitRect.w = myParent->maxItemWidth();
	blitRect.h = h;

	if ( myFlags & MIF_SEPARATOR ) {
		PG_Draw::DrawLine(canvas, blitRect.x, blitRect.y + 1, blitRect.x + myParent->maxItemWidth(), blitRect.y + 1, myParent->GetFont()->GetColor(), myParent->separatorLineWidth );
	} else {
		myParent->SetFontColor(*tcol);
		if ( myRightCaption.length()  ) {
			Uint16 tw,th;

			PG_Widget::GetTextSize( tw, th, myRightCaption, myParent->GetFont());

			RenderText(canvas, blitRect, blitRect.x + myParent->maxItemWidth() - tw, blitRect.y+myParent->GetFontAscender(), myRightCaption, myParent->GetFont());
		}
		RenderText(canvas, blitRect, blitRect.x, blitRect.y+myParent->GetFontAscender(), myCaption, myParent->GetFont());

	}
	//SDL_BlitSurface(*text, NULL, canvas, &blitRect);

	return true;
}

inline bool PG_PopupMenu::MenuItem::isValidRect() {
	if ((my_width > 0) && (my_height > 0))
		return true;

	return false;
}

inline bool PG_PopupMenu::MenuItem::paintNormal(SDL_Surface *canvas, PG_Color* tcol, PG_Color* scol) {
	if (!isValidRect())
		return false;

	return renderSurface(canvas, &sNormal, tcol, scol);
}

inline bool PG_PopupMenu::MenuItem::paintSelected(SDL_Surface *canvas, PG_Color* tcol, PG_Color* scol) {
	if (!isValidRect())
		return false;

	return renderSurface(canvas, &sSelected, tcol, scol);
}

inline bool PG_PopupMenu::MenuItem::paintDisabled(SDL_Surface *canvas, PG_Color* tcol, PG_Color* scol) {
	if (!isValidRect())
		return false;

	return renderSurface(canvas, &sDisabled, tcol, scol);
}

/***********************************
 * PG_PopupMenu
 */
PG_PopupMenu::PG_PopupMenu(PG_Widget *parent,
                           int x, int y,
                           const std::string& caption,
                           const std::string& style)
		: PG_ThemeWidget(parent, PG_Rect(0, 0, 1, 1)),
		xPadding(0),
		yPadding(0),
		minTabWidth(5),
		separatorLineWidth(1),
		itemHeight(0),
		selected(0),
		tracking(false),
      wasTracking(false),
		buttonDown(false),
		activeSub(0),
myMaster(0) {
	for (int i = 0; i < 3; i++) {
		miBackgrounds[i] = NULL;
	}
	LoadThemeStyle(style);

	myCaption = caption;

	getCaptionHeight(captionRect, true);

	MoveWidget(PG_Rect(x, y, captionRect.my_width + xPadding, captionRect.my_height + yPadding));

	captionRect.my_xpos = (my_width - captionRect.my_width) >> 1;

	lastH = my_height - (yPadding >> 1) + 1;

	current  = start = items.begin();
	stop = items.end();
}

PG_PopupMenu::~PG_PopupMenu() {
	for (int i = 0; i < 3; i++) {
		if (miBackgrounds[i]) {
			PG_Application::UnloadSurface(miBackgrounds[i]);
		}
	}

	for (MII j = start; j != stop; j++)
		delete (*j);
}

void PG_PopupMenu::appendItem(MenuItem *item) {
	PG_Rect     rect;

	items.push_back(item);
	item->measureItem(&rect);

	item->moveTo(xPadding >> 1, lastH);
	lastH += rect.my_height;

	if (!itemHeight)
		itemHeight = rect.my_height;

	stop = items.end();
	current = start = items.begin();

	recalcRect();

	selected = *start;
	current = start;
	selected->select();
}

PG_PopupMenu& PG_PopupMenu::addMenuItem(const std::string& caption,
                                        int ID,
                                        MenuItem::MenuItemSlot handler,
                                        PG_Pointer data,
                                        MenuItem::MI_FLAGS flags) {

	MenuItem* item = new MenuItem(this, caption, ID, flags);
	appendItem(item);
	item->sigSelectMenuItem.connect(handler, data);

	return *this;
}

PG_PopupMenu& PG_PopupMenu::addMenuItem(const std::string& caption,
                                        int ID,
                                        MenuItem::MI_FLAGS flags) {

	MenuItem* item = new MenuItem(this, caption, ID, flags);
	appendItem(item);

	return *this;
}

PG_PopupMenu& PG_PopupMenu::addMenuItem(const std::string& caption,
                                        PG_PopupMenu *sub,
                                        MenuItem::MI_FLAGS flags) {
	MenuItem    *item = new MenuItem(this, caption, sub);

	appendItem(item);

	return *this;
}

/*PG_PopupMenu& PG_PopupMenu::addMenuItem(char *caption,
                                        int ID,
                                        PG_Action *action,
                                        void *data,
                                        MenuItem::MI_FLAGS flags) {
	MenuItem    *item = new MenuItem(this, caption, ID, flags);
 
	appendItem(item);
 
	if (action) {
		if (actions[ID])
			//TODO: an exception here??
			PG_LogWRN("Duplicate action ID %d - Replacing old value", ID);
		actions[ID] = action;
	};
 
	return *this;
}*/

PG_PopupMenu& PG_PopupMenu::addSeparator() {
	// Ugly
	return addMenuItem(PG_NULLSTR, -1, MenuItem::MIF_SEPARATOR);
}

void PG_PopupMenu::disableItem(int id) {
	MII mi;

	mi = find_if(start, stop, item_with_id(id));
	if (*mi && mi != stop)
		((MenuItem*)*mi)->disable();
}

void PG_PopupMenu::enableItem(int id) {
	MII mi;

	mi = find_if(start, stop, item_with_id(id));
	if (*mi && mi != stop)
		((MenuItem*)*mi)->enable();
}

void PG_PopupMenu::trackMenu(int x, int y) {
	tracking = true;
	openMenu( x, y );
	buttonDown = PG_Application::GetEventSupplier()->GetMouseState( x,y ) & SDL_BUTTON_LEFT;
}

void PG_PopupMenu::openMenu(int x, int y) {
	if (x >= 0 && y >= 0) {
		if (x != my_xpos && y != my_ypos)
			MoveWidget(x, y);
	}

	x = my_xpos;
	y = my_ypos;

	if(x + my_width >= PG_Application::GetScreenWidth()) {
		x = PG_Application::GetScreenWidth() - my_width;
	}

	if(y + my_height >= PG_Application::GetScreenHeight()) {
		y = PG_Application::GetScreenHeight() - my_height;
	}

	if (x != my_xpos || y != my_ypos)
		MoveWidget(x, y);

	Show();
}

bool PG_PopupMenu::getCaptionHeight(PG_Rect &rect, bool constructing) {

	if (myCaption.empty())
		return false;

	Uint16 w = 0, h = 0;

	//+++
	GetTextSize(w, h, myCaption);

	// TODO: need to make sure caption isn't wider than screen
	if (!constructing)
		rect.my_xpos = (my_width - w) / 2;
	else
		rect.my_xpos = 0;
	rect.my_ypos = 0;

	rect.my_height = h;
	rect.my_width = w;

	return true;
}

void PG_PopupMenu::recalcRect() {
	PG_Rect newRect;

	getCaptionHeight(newRect);
	newRect.my_xpos = my_xpos;
	newRect.my_ypos = my_ypos;
	newRect.my_height += yPadding;

	if (!items.empty()) {
		PG_Rect itemRect;

		for (MII i = start; i != stop; i++) {
			MenuItem* item = *i;

			item->measureItem(&itemRect);

			if (itemRect.my_width > newRect.my_width)
				newRect.my_width = itemRect.my_width;
			newRect.my_height += itemRect.my_height;
		}
	}

	if ((newRect.my_width != my_width) ||
	        (newRect.my_height != my_height)) {
		int sh = PG_Application::GetScreenHeight();
		int sw = PG_Application::GetScreenWidth();

		if (newRect.my_height > sh)
			newRect.my_height = sh;

		if (newRect.my_width > sw)
			newRect.my_width = sw;

		SizeWidget(newRect.my_width + xPadding, newRect.my_height);

		actionRect.my_xpos = my_xpos + (xPadding >> 1);
		actionRect.my_ypos = my_ypos + captionRect.my_height + (yPadding >> 1);
		actionRect.my_width = my_width - xPadding;
		actionRect.my_height = my_height - xPadding - captionRect.my_height;
	}
}

void PG_PopupMenu::handleClick(int x, int y) {
	PG_Point  p;

	p.x = x;
	p.y = y;

	if (actionRect.IsInside(p)) {
		if(!selected)
			return;

		if (!selected->isSubMenu()) {
			if (!selected->isMute()) {
				// call item's callback
				selected->sigSelectMenuItem(selected);
				// call general callback (PG_PopupMenu)
				sigSelectMenuItem(selected);
			}
		}

		selected->unselect();
		selected = 0;
	}

	liberate();
}

void PG_PopupMenu::enslave(PG_PopupMenu *master) {
	myMaster = master;
}

void PG_PopupMenu::liberate() {
	Hide();

	// Pass it down
	if (activeSub) {
		PG_PopupMenu *tmp = activeSub;
		activeSub = 0;
		tmp->liberate();
	}

	// Pass it up
	if (myMaster) {
		myMaster->Hide();
		myMaster->liberate();
		myMaster = 0;
	}
}

void PG_PopupMenu::eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst) {
	SDL_Surface* surface = PG_Application::GetScreen();
	PG_ThemeWidget::eventBlit(srf, src, dst);

	if (!myCaption.empty()) {
		SetFontColor(captionActiveColor);
		DrawText(captionRect, myCaption);
	}

	if (!items.empty()) {
		PG_Rect itemRect;

		for (MII i = start; i != stop; i++) {
			MenuItem* item = *i;

			item->measureItem(&itemRect, true);
			itemRect.x += my_xpos;
			itemRect.y += my_ypos;

			int statesel = 0;
			if (item->isSelected())
				statesel = 1;

			if (item->isDisabled())
				statesel = 2;

			PG_Draw::DrawThemedSurface(surface, itemRect,
			                           miGradients[statesel],
			                           miBackgrounds[statesel],
			                           miBkModes[statesel],
			                           miBlends[statesel]);

			switch(statesel) {
				case 0:
					if (item->isSeparator())
						item->paintNormal(surface, &sepNormalColor, &sepShadowColor);
					else
						item->paintNormal(surface, &miNormalColor);
					break;

				case 1:
					if (item->isSeparator())
						item->paintSelected(surface, &sepNormalColor, &sepShadowColor);
					else
						item->paintSelected(surface, &miSelectedColor);
					break;

				case 2:
					if (item->isSeparator())
						item->paintDisabled(surface, &sepNormalColor, &sepShadowColor);
					else
						item->paintDisabled(surface, &miDisabledColor);
					break;
			}
		}
	}
}

bool PG_PopupMenu::selectItem(MenuItem *item, MII iter) {
	if (selected)
		selected->unselect();
	item->select();
	selected = item;

	if (current != iter)
		current = iter;

	if (activeSub && subParent != selected) {
		activeSub->Hide();
		activeSub = 0;
	} else if (activeSub)
		return false;

	if (!tracking && wasTracking) {
		SetCapture();
		tracking = true;
		wasTracking = false;
	}

	if (selected->isSeparator())
		return false;

	if (selected->isSubMenu()) {
		if (tracking) {
			ReleaseCapture();
			tracking = false;
			wasTracking = true;
		}

		PG_PopupMenu *sub = selected->getSubMenu();
		activeSub = sub;
		subParent = selected;

		PG_Rect   rect;

		sub->getCaptionHeight(rect);
		sub->enslave(this);
		sub->trackMenu(my_xpos + my_width - xPadding,
		               selected->my_ypos + my_ypos - rect.my_height);

		return true;
	}

	return true;
}

bool PG_PopupMenu::handleMotion(PG_Point const &p) {
	MenuItem  *item = 0;
	PG_Rect    itemRect;

	if (current != stop) {
		item = *current;
		item->measureItem(&itemRect, true);
	} else
		return false;

	if (item && itemRect.IsInside(p)) {
		if (!selectItem(item, current))
			return false;
	} else
		for (MII i = start; i != stop; i++) {
			item = *i;
			item->measureItem(&itemRect, true);
			itemRect.my_xpos += my_xpos;
			itemRect.my_ypos += my_ypos;

			if (itemRect.IsInside(p)) {
				if (!selectItem(item, i))
					return false;
				break;
			}
		}

	return true;
}

bool PG_PopupMenu::eventMouseMotion(const SDL_MouseMotionEvent *motion) {
	if (items.empty())
		return false;

	PG_Point p;

	p.x = motion->x;
	p.y = motion->y;

	if (!actionRect.IsInside(p)) {
		if (selected) {
			selected->unselect();
			selected = 0;
			Redraw();
		}

		if (myMaster && tracking && myMaster->IsInside(p)) {
			ReleaseCapture();
			tracking = false;
			wasTracking = true;
			myMaster->ProcessEvent(reinterpret_cast<const SDL_Event*>(motion));
		}

		return false;
	}

	PG_Rect    itemRect;
	MenuItem  *oldSel = selected;

	if (selected) {
		selected->measureItem(&itemRect, true);
		itemRect.my_xpos += my_xpos;
		itemRect.my_ypos += my_ypos;
		if (itemRect.IsInside(p))
			return false;

		if (motion->yrel < 0  && current != start)
			current--;
		else if (motion->yrel > 0 && current != stop)
			current++;
	} else {
		current = start;
	}

	if (!handleMotion(p))
		return false;

	if (oldSel != selected)
		Redraw();

	return true;
}

bool PG_PopupMenu::eventMouseButtonDown(const SDL_MouseButtonEvent *button) {
	PG_ThemeWidget::eventMouseButtonDown(button);

	if (button->button == SDL_BUTTON_LEFT) {
		buttonDown = true;
		return true;
	}

	return false;
}

bool PG_PopupMenu::eventMouseButtonUp(const SDL_MouseButtonEvent *button) {
	PG_ThemeWidget::eventMouseButtonUp(button);

	if (button->button == SDL_BUTTON_LEFT && buttonDown) {
		buttonDown = false;

		if (activeSub) {
			activeSub->liberate();
			return true;
		}

		Hide();

		if (selected && selected->isSeparator())
			return true;

		handleClick(button->x, button->y);

		return false;
	}

	return false;
}

bool PG_PopupMenu::eventKeyDown(const SDL_KeyboardEvent *key) {
	SDL_KeyboardEvent key_copy = *key; // copy key structure
	PG_Application::TranslateNumpadKeys(&key_copy);
	// from now, we use key_copy which was copied or translated from key
   

   if ( key->keysym.unicode )
   for ( MII i = items.begin(); i != items.end(); ++i ) {
      if ( *i )
         if ( extractHotkey( (*i)->getCaption()) == key->keysym.unicode ) {
            Hide();
            if ( !(*i)->isDisabled()) {
                  // call item's callback
               (*i)->sigSelectMenuItem(selected);
                  // call general callback (PG_PopupMenu)
               sigSelectMenuItem((*i));
               return true;
            }
         }
   }
   
   
	switch (key_copy.keysym.sym) {
		case SDLK_ESCAPE:
			Hide();
			break;

		case SDLK_RETURN:
			if (selected) {
				Hide();

				if (/*actions[selected->getId()] &&*/ !selected->isDisabled()) {
					// call item's callback
					selected->sigSelectMenuItem(selected);
					// call general callback (PG_PopupMenu)
					sigSelectMenuItem(selected);
				}

				selected->unselect();
				selected = 0;
			}
			break;

		case SDLK_UP:
			if (items.empty())
				break;

			if (current == start)
				current = stop;

			do {
				current--;
			} while ((current != start) && (*current)->isMute());

			if (handleMotion(**current))
				Redraw();
			break;

		case SDLK_DOWN:
			if (items.empty())
				break;

			if (current == stop) {
				// special case
				current = start;
				while (current != stop && (*current)->isMute())
					current++;
			} else
				do {
					current++;
				} while ((current != stop) && (*current)->isMute());

			if (current == stop)
				return false;

			if (handleMotion(**current))
				Redraw();
			break;

		default:
			return false;
	}

	return true;
}

void PG_PopupMenu::eventMoveWidget(int x, int y) {
	PG_ThemeWidget::eventMoveWidget(x, y);

	actionRect.my_xpos = my_xpos + (xPadding >> 1);
	actionRect.my_ypos = my_ypos + captionRect.my_height + (yPadding >> 1);
}

void PG_PopupMenu::eventShow() {
	PG_ThemeWidget::eventShow();

	if (tracking) {
		SetCapture();
	}

	current = start;
}

void PG_PopupMenu::eventHide() {
	PG_ThemeWidget::eventHide();

	if (tracking) {
		ReleaseCapture();
		tracking = false;
	}
}

void PG_PopupMenu::LoadThemeStyle(const std::string& widgettype) {
	PG_ThemeWidget::LoadThemeStyle(widgettype);

	PG_Theme *theme = PG_Application::GetTheme();

	// Global
	theme->GetProperty(widgettype, "PopupMenu", "xPadding", xPadding);
	theme->GetProperty(widgettype, "PopupMenu", "yPadding", yPadding);
	theme->GetProperty(widgettype, "PopupMenu", "minTabWidth", minTabWidth);

	// caption
	PG_ThemeWidget::LoadThemeStyle(widgettype, "Caption");
	theme->GetColor(widgettype, "Caption", "Active", captionActiveColor);
	theme->GetColor(widgettype, "Caption", "Inactive", captionInactiveColor);
	theme->GetColor(widgettype, "MenuItem", "Normal", miNormalColor);
	theme->GetColor(widgettype, "MenuItem", "Selected", miSelectedColor);
	theme->GetColor(widgettype, "MenuItem", "Disabled", miDisabledColor);
	theme->GetColor(widgettype, "MenuItem", "SepNormal", sepNormalColor);
	theme->GetColor(widgettype, "MenuItem", "SepShadow", sepShadowColor);
	theme->GetProperty(widgettype, "MenuItem", "separatorLineWidth", separatorLineWidth);

	miGradients[0] = theme->FindGradient(widgettype, "MenuItem", "gradientNormal");
	miGradients[1] = theme->FindGradient(widgettype, "MenuItem", "gradientSelected");
	miGradients[2] = theme->FindGradient(widgettype, "MenuItem", "gradientDisabled");

	miBackgrounds[0] = theme->FindSurface(widgettype, "MenuItem", "backNormal");
	miBackgrounds[1] = theme->FindSurface(widgettype, "MenuItem", "backSelected");
	miBackgrounds[2] = theme->FindSurface(widgettype, "MenuItem", "backDisabled");

	theme->GetProperty(widgettype, "MenuItem", "backmodeNormal", miBkModes[0]);
	theme->GetProperty(widgettype, "MenuItem", "backmodeSelected", miBkModes[1]);
	theme->GetProperty(widgettype, "MenuItem", "backmodeDisabled", miBkModes[2]);

	theme->GetProperty(widgettype, "MenuItem", "blendNormal", miBlends[0]);
	theme->GetProperty(widgettype, "MenuItem", "blendSelected", miBlends[1]);
	theme->GetProperty(widgettype, "MenuItem", "blendDisabled", miBlends[2]);
}

void PG_PopupMenu::LoadThemeStyle(const std::string& widgettype, const std::string& objectname) {
	PG_ThemeWidget::LoadThemeStyle(widgettype, objectname);
}

void PG_PopupMenu::eventMouseEnter() {
	PG_ThemeWidget::eventMouseEnter();
}

void PG_PopupMenu::eventMouseLeave() {
	PG_ThemeWidget::eventMouseLeave();
}

bool PG_PopupMenu::SetMenuItemSlot(int id, MenuItem::MenuItemSlot slot, PG_Pointer clientdata) {
	MII it;
	PG_PopupMenu::MenuItem* item;
	int itid;

	it=items.begin();
	while(it!=items.end()) {

		itid=(*it)->getId();

		if(id==itid) {
			item=*it;
			item->sigSelectMenuItem.connect(slot, clientdata);
			return true;
		}
		it++;
	}
	return false;
}
