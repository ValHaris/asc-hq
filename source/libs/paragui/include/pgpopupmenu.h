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
    Update Date:      $Date: 2009-04-18 13:48:39 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgpopupmenu.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

/** \file pgpopupmenu.h
	Header file for the PG_PopupMenu class.
*/

#ifndef PG_POPUPMENU_H
#define PG_POPUPMENU_H

#include "pgthemewidget.h"
#include "pgsignals.h"
#include "pgstring.h"

#include <string>
#include <list>
#ifdef HASH_MAP_INC
# include HASH_MAP_INC
#else
# include <map>
#endif

/**
 * @author Marek Habersack
 *
 * @short A stand-alone or attached popup menu
 *
 * A popup menu that can be attached to a menu bar or used
 * stand-alone. In the first case, the widget sizes itself so that
 * only the caption is visible, the menu items remain hidden until
 * menu is activated. In the latter case menu sizes itself to
 * encompass all of its items unless the menu size would exceed the
 * space between the menu origin and the edge of the screen. In such
 * case, menu displays a "scroller" button at the bottom.
 *
 * @todo implement the scroller code (menu scrolling when it exceeds the
 *       screen height/width - a kind of specialized widgetlist). Should
 *       display a scroller icon at the bottom/top of the popup menu - a'la
 *       w2k.
 * @todo keyboard handling (accelerators, ESC/ENTER & arrows)
 */

class DECLSPEC PG_PopupMenu : public PG_ThemeWidget {
public:

	/**
	 * @author Marek Habersack
	 *
	 * @short A menu item data structure
	 *
	 * Even though implemented as a class, MenuItem is not intended to be a new
	 * kind of widget - therefore it is derived only from PG_Rect for
	 * convenience. The idea is to have a "smart" data type (or a dumb class,
	 * if you prefer) that knows how to perform simple and specific actions on
	 * itself. The intention is to provide PG_PopupMenu with a fast mechanism
	 * for processing menu items - if MenuItem was derived from any of ParaGUI
	 * classes it would incurr unnecessary processing overhead.
	 *
	 * @todo better separator code
	 * @todo icon drawing
	 */
class DECLSPEC MenuItem : public PG_Rect, public PG_MessageObject {
	public: // types
		enum MI_FLAGS {
		    MIF_NONE = 0,
		    MIF_DISABLED = 0x01,
		    MIF_SEPARATOR = 0x02,
		    MIF_SUBMENU = 0x04
		};

		/**
		Signal type declaration
		**/
		template<class datatype = PG_Pointer>
	class SignalSelectMenuItem : public PG_Signal1<MenuItem*, datatype> {}
		;

		SignalSelectMenuItem<> sigSelectMenuItem;

		typedef SigC::Slot2<bool, MenuItem*, PG_Pointer> MenuItemSlot;

	public: // methods
		MenuItem(PG_PopupMenu *parent,
		         const std::string& caption,
		         int id,
		         MI_FLAGS flags);
		MenuItem(PG_PopupMenu *parent,
		         const std::string& caption,
		         PG_PopupMenu *submenu);
		~MenuItem();

		bool measureItem(PG_Rect* rect,  bool full = false);
		bool isPointInside(int x, int y);
		inline void moveTo(int x, int y);

		inline SDL_Surface* getNormal() const;
		inline SDL_Surface* getDisabled() const;
		inline SDL_Surface* getSelected() const;

		bool paintNormal(SDL_Surface *canvas, PG_Color* tcol, PG_Color* scol = NULL);
		bool paintDisabled(SDL_Surface *canvas, PG_Color* tcol, PG_Color* scol = NULL);
		bool paintSelected(SDL_Surface *canvas, PG_Color* tcol, PG_Color* scol = NULL);

		inline void disable();
		inline void enable();
		inline bool isDisabled() const;
		inline bool isEnabled() const;
		inline void select();
		inline void unselect();
		inline bool isSelected() const;
		inline bool isSeparator() const;
		inline bool isSubMenu() const;
		inline bool isMute() const;

		inline int Width() const;
		inline int Height() const;

		inline int getId() const;
		inline PG_PopupMenu *getSubMenu() const;

		inline const PG_String& getCaption() const;

		inline operator PG_Point const&() const;

	private: // methods
		DLLLOCAL void initItem( const std::string& caption );
		DLLLOCAL bool renderSurface(SDL_Surface *canvas, SDL_Surface **text, PG_Color* tcol, PG_Color* scol = 0);
		DLLLOCAL bool isValidRect();

	protected: // data
		unsigned      myFlags;
		PG_String     myCaption;
		PG_String     myRightCaption;
		PG_PopupMenu *myParent;

		PG_PopupMenu *mySubMenu;
		int           myId;

		SDL_Surface  *sNormal;
		SDL_Surface  *sSelected;
		SDL_Surface  *sDisabled;

		bool          selected;

	private: // data
		bool          needRecalc;
		PG_Point      myPoint;
	};

#ifndef DOXYGEN_SKIP
class item_with_id : public std::unary_function<MenuItem*, bool> {
		int id;

	public:
		explicit item_with_id(int i)
				: id(i) {}

		bool operator() (const MenuItem* const mi) const {
			return mi->getId() == id;
		}
	};
#endif // DOXYGEN_SKIP

public: // methods

	/**
	Signal type declaration
	**/
	template<class datatype = PG_Pointer>
class SignalSelectMenuItem : public PG_Signal1<MenuItem*, datatype> {}
	;

	PG_PopupMenu(PG_Widget *parent,
	             int x, int y,
	             const std::string& caption = PG_NULLSTR,
	             const std::string& style = "PopupMenu");

	~PG_PopupMenu();

	/** @name Add a new menu item to this menu
	 *
	 * Constructs a new menu item using the provided parameters and
	 * then adds the item to this menu.
	 *@{
	 */

	/**
	 * Adds a menu item whose handler (if any) is set to be a stand-alone
	 * function.
	 *
	 * @param caption   the item caption
	 * @param ID        the item identifier
	 * @param data      application-specific data associated with the menu
	 *                  item action.
	 * @param flags     menu item flags
	 *
	 */
	PG_PopupMenu& addMenuItem(const std::string& caption,
	                          int ID,
	                          MenuItem::MenuItemSlot,
	                          PG_Pointer data = NULL,
	                          MenuItem::MI_FLAGS flags = MenuItem::MIF_NONE);

	PG_PopupMenu& addMenuItem(const std::string& caption,
	                          int ID,
	                          MenuItem::MI_FLAGS flags = MenuItem::MIF_NONE);

	PG_PopupMenu& addMenuItem(const std::string& caption,
	                          PG_PopupMenu *sub,
	                          MenuItem::MI_FLAGS flags = MenuItem::MIF_SUBMENU);

	/**
	 * @return a reference to this menu
	 *@}
	 */

	PG_PopupMenu& addSeparator();

	bool SetMenuItemSlot(int id, MenuItem::MenuItemSlot slot, PG_Pointer clientdata = NULL);

	inline int maxItemWidth() const;

	void disableItem(int dd);
	void enableItem(int id);

	/**
	 * Modal popup menu will be shown - i.e. all mouse/keyboard events will
	 * be captured by the menu and until the user selects any menu item (or
	 * cancels the menu by pressing ESC) no other widget will be
	 * accessible. If @code x @endcoce and @code y @endcode are absent, menu is popped up at
	 * its current position.
	 *
	 * @param x  xpos where the menu should pop up
	 * @param y  ypos where the menu should pop up
	 *
	 */
	void trackMenu(int x = -1, int y = -1);
	void openMenu(int x = -1, int y = -1);


	SignalSelectMenuItem<> sigSelectMenuItem;

protected: // methods

	typedef std::list<MenuItem*>::iterator MII;

	// reimplemented
	void eventBlit(SDL_Surface* surface, const PG_Rect& src, const PG_Rect& dst);

	void eventMouseEnter();
	void eventMouseLeave();

	bool eventMouseMotion(const SDL_MouseMotionEvent *motion);
	bool eventMouseButtonDown(const SDL_MouseButtonEvent *button);
	bool eventMouseButtonUp(const SDL_MouseButtonEvent *button);
	bool eventKeyDown(const SDL_KeyboardEvent *key);

	void eventMoveWidget(int x, int y);
	void eventShow();
	void eventHide();

	void LoadThemeStyle(const std::string& widgettype);
	void LoadThemeStyle(const std::string& widgettype, const std::string& objectname);

	// own
	virtual bool getCaptionHeight(PG_Rect &rect, bool constructing = false);
	virtual void recalcRect();
	virtual void handleClick(int x, int y);
	virtual void enslave(PG_PopupMenu *master);
	virtual void liberate();

private: // methods

	DLLLOCAL bool selectItem(MenuItem *item, MII iter);
	DLLLOCAL bool handleMotion(PG_Point const&);
	DLLLOCAL void appendItem(MenuItem *item);

protected: // data

	std::list<MenuItem*>  items; /** the menu items collection */
	std::string           myCaption; /** menu caption */

	PG_Color             captionActiveColor;
	PG_Color             captionInactiveColor;

	PG_Color             miNormalColor;
	PG_Color             miSelectedColor;
	PG_Color             miDisabledColor;

	PG_Color             sepNormalColor;
	PG_Color             sepShadowColor;

	int                   xPadding;
	int                   yPadding;

	/** if a menu entry has a right justified component
	           ( e.g. "save\tctrl-s" ), this is the minimum space for the tab  */
	int                  minTabWidth;

	int                  separatorLineWidth;

private: // data
	PG_Rect               captionRect;
	PG_Rect               actionRect;

	PG_Gradient          *miGradients[3];
	SDL_Surface          *miBackgrounds[3];
	PG_Draw::BkMode		  miBkModes[3];
	Uint8                 miBlends[3];
	int                   itemHeight;
	int                   lastH;
	MenuItem             *selected;

	bool                  tracking;
	bool                  wasTracking;
	bool                  buttonDown;

	MII                   stop;
	MII                   start;
	MII                   current;

	PG_PopupMenu         *activeSub;
	PG_PopupMenu         *myMaster;
	MenuItem             *subParent;
};

inline int PG_PopupMenu::maxItemWidth() const {
	return w - xPadding;
}

inline void PG_PopupMenu::MenuItem::moveTo(int _x, int _y) {
	myPoint.x = x = _x;
	myPoint.y = y = _y;
};

inline SDL_Surface* PG_PopupMenu::MenuItem::getNormal() const {
	return sNormal;
}

inline SDL_Surface* PG_PopupMenu::MenuItem::getDisabled() const {
	return sDisabled;
}

inline SDL_Surface* PG_PopupMenu::MenuItem::getSelected() const {
	return sSelected;
}

inline void PG_PopupMenu::MenuItem::disable() {
	myFlags |= MIF_DISABLED;
}

inline void PG_PopupMenu::MenuItem::enable() {
	myFlags &= ~MIF_DISABLED;
}

inline bool PG_PopupMenu::MenuItem::isDisabled() const {
	return (myFlags & MIF_DISABLED);
}

inline bool PG_PopupMenu::MenuItem::isEnabled() const {
	return !(myFlags & MIF_DISABLED);
}

inline void PG_PopupMenu::MenuItem::select() {
	selected = true;
}

inline void PG_PopupMenu::MenuItem::unselect() {
	selected = false;
}

inline bool PG_PopupMenu::MenuItem::isSelected() const {
	return selected;
}

inline bool PG_PopupMenu::MenuItem::isSeparator() const {
	return ((myFlags & MIF_SEPARATOR) != 0);
}

inline bool PG_PopupMenu::MenuItem::isSubMenu() const {
	return ((myFlags & MIF_SUBMENU) != 0);
}

inline bool PG_PopupMenu::MenuItem::isMute() const {
	return ((myFlags & MIF_DISABLED) ||
	        (myFlags & MIF_SEPARATOR));
}

inline int PG_PopupMenu::MenuItem::Width() const {
	return w;
}

inline int PG_PopupMenu::MenuItem::Height() const {
	return h;
}

inline int PG_PopupMenu::MenuItem::getId() const {
	return myId;
}

inline PG_PopupMenu *PG_PopupMenu::MenuItem::getSubMenu() const {
	return mySubMenu;
}

inline const PG_String& PG_PopupMenu::MenuItem::getCaption() const {
	return myCaption;
}

inline PG_PopupMenu::MenuItem::operator PG_Point const&() const {
	return myPoint;
}

#endif // PG_POPUPMENU_H
