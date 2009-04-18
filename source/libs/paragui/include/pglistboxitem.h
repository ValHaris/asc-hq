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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pglistboxitem.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

/** \file pglistboxitem.h
	Header file for the PG_ListBoxItem class.
*/

#ifndef PG_LISTBOXITEM_H
#define PG_LISTBOXITEM_H

#include "pglistboxbaseitem.h"
#include "pgdraw.h"

class PG_ListBox;

class DECLSPEC PG_ListBoxItem : public PG_ListBoxBaseItem {
public:

	/** */
	PG_ListBoxItem(PG_Widget* parent, int height, const std::string& text = PG_NULLSTR, SDL_Surface* icon = NULL, void* userdata = NULL, const std::string& style="ListBox");

	/** */
	~PG_ListBoxItem();

	/** */
	void LoadThemeStyle(const std::string& widgettype, const std::string& objectname);

protected:

	/** */
	void eventBlit(SDL_Surface* surface, const PG_Rect& src, const PG_Rect& dst);

	/** */
	void eventSizeWidget(Uint16 w, Uint16 h);

	PG_Gradient* my_gradient[3];

	SDL_Surface* my_background[3];

	SDL_Surface* my_srfHover;

	SDL_Surface* my_srfSelected;

	PG_Draw::BkMode my_bkmode[3];

	Uint8 my_blend[3];
};

#endif	// PG_LISTBOXITEM_H
