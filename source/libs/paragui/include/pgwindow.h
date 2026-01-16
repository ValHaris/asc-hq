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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgwindow.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

/** \file pgwindow.h
	Header file for the PG_Window class.
*/

#ifndef PG_WINDOW_H
#define PG_WINDOW_H

#include "pgthemewidget.h"
#include "pglabel.h"
#include <string>
#include <sigc++/sigc++.h>

class PG_Button;

/**
	@author Alexander Pipelka
 
	@short A window inside the program (not a windowing environment window)
 
	Creates a window entirely inside ParaGUI. This window can be modal or non-modal.
	Non-modal windows can be brought in front of others by clicking the titlebar.
*/

class DECLSPEC PG_Window : public PG_ThemeWidget  {
public:


	typedef enum {
	    MODAL = 0x01,
	    SHOW_CLOSE = 0x02,
	    SHOW_MINIMIZE = 0x04,
	    DEFAULT = SHOW_CLOSE
	} WindowFlags;

	enum {
	    IDWINDOW_CLOSE	 = PG_WIDGETID_INTERNAL + 14,
	    IDWINDOW_MINIMIZE = PG_WIDGETID_INTERNAL + 15,
	    IDWINDOW_RESTORE = PG_WIDGETID_INTERNAL + 15
	};

	/**
	Signal type declaration
	**/
	typedef sigc::signal<bool> SignalWindowClose;
	typedef sigc::signal<bool> SignalWindowMinimize;
	typedef sigc::signal<bool> SignalWindowRestore;

	/**
	Constructor for the PG_Window class
	@param parent pointer to the parent widget or NULL
	@param r screen position of the button
	@param windowtext window titlebar label
	@param flags (DEFAULT = SHOW_CLOSE | SHOW_CLOSE | SHOW_MINIMIZE | MODAL )
	@param style themestyle of the window
	@param heightTitlebar height of the window titlebar
	*/
	PG_Window(PG_Widget* parent, const PG_Rect& r = PG_Rect::null_rect, const std::string& windowtext = PG_NULLSTR, WindowFlags flags = DEFAULT, const std::string& style="Window", int heightTitlebar = 25);

	/** */
	~PG_Window();

	void LoadThemeStyle(const std::string& widgettype);

	/**
	set window titlebar color
	@param	c	pointer to a PG_Color
	*/
	void SetTitlebarColor(const PG_Color& c);

	/**
	get window title
	@return	PG_Color	Giving the height of the titlebar
	*/
	PG_Color GetTitlebarColor();

	/**
	set window titlebar height
	@return	height	height of the titlebar
	*/
	void SetTitlebarHeight(Uint8 height);

	/**
	get window titlebar height
	@return	Uint8	height of the titlebar
	*/
	Uint8 GetTitlebarHeight();

	/**
	set window title and alignment
	@param	title					new window title
	@param alignment	alignment of the text (PG_Label::LEFT | PG_Label::CENTER | PG_Label::RIGHT)
	*/
	void SetTitle(const std::string& title, PG_Label::TextAlign alignment = PG_Label::CENTER);

	void SetText(const std::string& text);

	/**
	get window title
	@return	pointer to the window title string
	*/
	const PG_String& GetTitle();

	const PG_String& GetText();

	/**
	get window icon
	@return pointer to the window's icon
	*/
	SDL_Surface* GetIcon();

	/**
	set window icon
	@param filename	image-file to load
	*/
	void SetIcon(const std::string& filename);

	/**
	set window icon
	@param icon		pointer to imagedata (SDL_Surface)
	*/
	void SetIcon(SDL_Surface* icon);

	/**
	Make window handle / ignore dragging
	*/
	void SetMoveable(bool moveable = true);

	SignalWindowClose sigClose;
	SignalWindowMinimize sigMinimize;
	SignalWindowRestore sigRestore;

protected:

	void RecalcPositions();

	/** */
	void eventBlit(SDL_Surface* surface, const PG_Rect& src, const PG_Rect& dst);

	/** */
	void eventSizeWidget(Uint16 w, Uint16 h);

	/** */
	virtual bool handleButtonClick(PG_Button* button);

	/** */
	bool eventMouseButtonDown(const SDL_MouseButtonEvent* button);

	/** */
	bool eventMouseButtonUp(const SDL_MouseButtonEvent* button);

	/** */
	bool eventMouseMotion(const SDL_MouseMotionEvent* motion);

private:

	Uint8 my_heightTitlebar;

	PG_Button* my_buttonClose;

	PG_Button* my_buttonMinimize;

	PG_Button* my_buttonIcon;

	PG_ThemeWidget* my_titlebar;

	PG_Label* my_labelTitle;

	bool my_moveMode;

	Uint8 my_moveTransparency;

	PG_Point my_moveDelta;

	bool my_showCloseButton;

	bool my_showMinimizeButton;

	bool my_moveable;

private: // disable the copy operators

	PG_Window(const PG_Window&);
	PG_Window& operator=(const PG_Window&);

};

#endif // PG_WINDOW_H
