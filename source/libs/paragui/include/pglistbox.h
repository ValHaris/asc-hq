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
    Update Date:      $Date: 2008-05-24 18:21:04 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pglistbox.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

#ifndef PG_LISTBOX_H
#define PG_LISTBOX_H

#include <sigc++/sigc++.h>
#include "pgwidgetlist.h"
#include "pglabel.h"

class PG_ListBoxBaseItem;

/** \file pglistbox.h
	Header file for the PG_ListBox class.
*/

/**
	@author Alexander Pipelka
	
	@short A scrollable box that can hold any number of text items
 
	@image html listbox.png "listbox screenshot"
*/

class DECLSPEC PG_ListBox : public PG_WidgetList {
public:

	/**
	Signal type declaration
	**/
class SignalSelectItem : public sigc::signal<bool, PG_ListBoxBaseItem*> {}
	;

	/** */
	PG_ListBox(PG_Widget* parent, const PG_Rect& r = PG_Rect::null_rect, const std::string& style="ListBox");

	/** */
	~PG_ListBox();

	/** */
	void SetMultiSelect(bool multi = true);

	/** */
	bool GetMultiSelect();

	/** */
	PG_ListBoxBaseItem* GetSelectedItem();

	/** */
	void SelectItem(PG_ListBoxBaseItem* item, bool select = true, bool fireEvent = true);

	/* */
	void SelectFirstItem( bool fireEvent = true );

	/* */
	void SelectNextItem( bool fireEvent = true );

	/* */
	void SelectPrevItem( bool fireEvent = true );

	/**
	Remove all widgets from the list (without deletion)
	*/
	void RemoveAll();

	/**
	Delete (destroy) all widgets in the list
	*/
	void DeleteAll();

	/**
	Set the item indentation
	@param	indent		number of pixels for item indentation (must be set before adding items)
	*/
	void SetIndent(Uint16 indent);

	/**
	Returns the item indentation
	*/
	Uint16 GetIndent();

	/**
	Set the alignment for all items
	@param style alignment to be used for all items
	*/
	void SetAlignment(PG_Label::TextAlign style);

	/**
	Returns the set alignment rule of this list
	*/
	PG_Label::TextAlign GetAlignment();

	/**
	Returns the index of the last selected item
	*/
	int GetSelectedIndex();

	void GetSelectedItems(std::vector<PG_ListBoxBaseItem*>& items);

	void AddChild(PG_Widget* child);

	SignalSelectItem sigSelectItem;

protected:

	/** */
	virtual bool eventSelectItem(PG_ListBoxBaseItem* item);

	/** */
	bool eventMouseButtonUp(const SDL_MouseButtonEvent* button);

	/** */
	bool eventMouseButtonDown(const SDL_MouseButtonEvent* button);

	/** */
	bool eventMouseMotion(const SDL_MouseMotionEvent* motion);

	friend class PG_ListBoxBaseItem;

private:

	bool my_multiselect;

	Uint16 my_indent;

	int my_selectindex;

	PG_ListBoxBaseItem* my_selectedItem;

	PG_Label::TextAlign my_alignment;

};

#endif	// PG_LISTBOX_H
