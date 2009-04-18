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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgmenubar.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

/** \file pgmenubar.h
	Header file for the PG_MenuBar class.
*/

#ifndef PG_MENUBAR_H
#define PG_MENUBAR_H

#include "pgthemewidget.h"

class PG_Button;
class PG_PopupMenu;

/**
	@author Alexander Pipelka
	@short A menubar.
	This class provides a MenuBar where you can snap in different PG_PopupMenu
	objects. Every item creates a button in the bar. By clicking the button the corresponding
	PopupMenu is opened.
*/

class DECLSPEC PG_MenuBar : public PG_ThemeWidget {
public:

	/**
		Creates a new MenuBar object.
		@param parent	pointer to the parent widget
		@param rect		the position of the menubar
		@param style		the default themestyle (MenuBar)

		The constructor creates a new MenuBar object without any items.
		Use the Add member function to insert PG_PopupMenu objects.
	*/
	PG_MenuBar(PG_Widget* parent, const PG_Rect& rect = PG_Rect::null, const std::string& style = "MenuBar");

	/**
		Destroys the MenuBar object.
	*/
	~PG_MenuBar();

	/**
		Add a new item to the MenuBar
		@param text		label of the new item
		@param menu	pointer to the PG_PopupMenu object to add
		@param indent	offset of pixels to the last item
		@param width width of the MenuBar button (if 0 the button will fit the textsize)
	*/
	void Add(const std::string& text, PG_PopupMenu* menu, Uint16 indent = 5, Uint16 width = 0);

protected:

	//! Internal item
	typedef struct {
		PG_Button* button; //!< pointer to the PG_Button object showed in the bar
		PG_PopupMenu* popupmenu; //!< pointer to the linked PG_PopupMenu object
	}
	MenuBarItem;

	std::vector<MenuBarItem*> ItemList;

	Uint16 my_btnOffsetY;

	bool leaveButton ( PG_Pointer last );
	bool enterButton ( PG_Pointer last );
private:


   bool deactivateItem();
   
	/**
		Callback handler for MSG_BUTTONCLICK messages
	*/
	DLLLOCAL bool handle_button(PG_Button* button, PG_Pointer last);

	DLLLOCAL void Cleanup();

	std::string my_style;

	PG_PopupMenu* my_active;

};

#endif // PG_MENUBAR_H
