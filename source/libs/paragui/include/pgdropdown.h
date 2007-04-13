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
    Update Date:      $Date: 2007-04-13 16:15:56 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgdropdown.h,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

/** \file pgdropdown.h
	Header file for the PG_DropDown class.
*/

#ifndef PG_DROPDOWN_H
#define PG_DROPDOWN_H

#include "pgwidget.h"
#include "pgsignals.h"
#include "pglabel.h"
#include "pglineedit.h"

class PG_Button;
class PG_LineEdit;
class PG_ListBox;
class PG_ListBoxBaseItem;

/**
	@author Alexander Pipelka
	
	@short A edit / dropdown combo item
 
	Basically a PG_LineEdit widget but with a button to show a dropdown list with
	predefined entries.
*/

class DECLSPEC PG_DropDown : public PG_Widget {
public:

	/**
	Signal type declaration
	**/
	template<class datatype = PG_Pointer>
class SignalSelectItem : public PG_Signal1<PG_ListBoxBaseItem*, datatype> {}
	;

	enum {
	    IDDROPDOWN_BOX = PG_WIDGETID_INTERNAL + 11
	};

	/**
	Construct a PG_DropDown object.
	@param parent pointer to the parent of the widget of NULL
	@param id id of the widget
	@param r position of the widget
	@param style style of the widget (loaded from the theme)
	*/
	PG_DropDown(PG_Widget* parent, const PG_Rect& r = PG_Rect::null, int id = -1, const std::string& style="DropDown");

	/**  */
	~PG_DropDown();

	/** */
	void LoadThemeStyle(const std::string& widgettype);

	/**
	Add a new item to the dropdown box
	@param text text of the new item
	@param userdata pointer to userdefined data linked to the item.
	@param height Height of the listbox item in pixels. If 0 the height will
			be calculated using the text height.
	*/
	void AddItem(const std::string& text, void* userdata = NULL, Uint16 height = 0);

	/**
	Remove all widgets from the drop down(without deletion)
	*/
	void RemoveAll();

	/**
	Delete (destroy) all widgets from the drop down
	*/
	void DeleteAll();

	/**	Set the item indentation
	@param	indent		number of pixels for item indentation (must be set before adding items)
	*/
	void SetIndent(Uint16 indent);

	/** Returns the item indentation */
	Uint16 GetIndent();

	/**
	Set if the widget is editable by the user
	@param	edit		true - widget is editable / false - widget is read only
	*/
	void SetEditable(bool edit);

	/**
	Check if the widget is editable by the user
	@return			true - widget is editable / false - widget is read only
	*/
	bool GetEditable();

	/**
	Get the current text string
	@return			pointer to text string
	*/
	const PG_String& GetText();

	/**
	Set the current text string
	@param	new_text	pointer to text string
	*/
	void SetText(const std::string& new_text);

	/** */
	bool ProcessEvent(const SDL_Event * event, bool bModal);

	/**
	Select the first item
	*/
	void SelectFirstItem();


	/**
	Returns the index of the last selected item.
	*/
	int GetSelectedItemIndex();

	/**
	Select the next item
	*/
	void SelectNextItem();

	/**
	Select the previous item
	*/
	void SelectPrevItem();

	/**
	Select the n-th item
	@param n number of item to select
	*/
	void SelectItem(const int n);

	/**
	Set the item's alignment
	@param style alignment to be set for the item
	*/
	void SetAlignment(PG_Label::TextAlign style);

	/**
	Returns the set alignment rule of this list
	*/
	PG_Label::TextAlign GetAlignment();

	PG_Widget* GetFirstInList();

	void AddChild(PG_Widget* child);

	SignalSelectItem<> sigSelectItem;

	PG_LineEdit::SignalEditBegin<> sigEditBegin;
	PG_LineEdit::SignalEditEnd<> sigEditEnd;
	PG_LineEdit::SignalEditReturn<> sigEditReturn;

protected:

	/** */
	void eventShow();

	/** */
	void eventHide();

	/** */
	virtual bool handleButtonClick(PG_Button* button);

	/** */
	void eventSizeWidget(Uint16 w, Uint16 h);

	/** */
	void eventMoveWidget(int x, int y);

	/** */
	virtual bool eventSelectItem(PG_ListBoxBaseItem* item);

	PG_LineEdit* my_EditBox;

	PG_Button* my_DropButton;

	PG_ListBox* my_DropList;

private:
	bool onDropListDeletion( const PG_MessageObject* dropList );

	DLLLOCAL bool select_handler(PG_ListBoxBaseItem* item);

};

#endif	// PG_DROPDOWN_H
