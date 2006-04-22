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
    Update Date:      $Date: 2006-04-22 14:08:13 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pglistbox.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.2 $
    Status:           $State: Exp $
*/

#include "pglistbox.h"
#include "pglistboxbaseitem.h"
#include "pgscrollarea.h"
#include "pglog.h"

#include "propstrings_priv.h"

PG_ListBox::PG_ListBox(PG_Widget* parent, const PG_Rect& r, const std::string& style) : PG_WidgetList(parent, r, style),
my_selectedItem(NULL), my_alignment(PG_Label::LEFT) {
	my_multiselect = false;
	my_indent = 0;
	my_selectindex = -1;
	PG_ThemeWidget::LoadThemeStyle(style, PG_PropStr::ListBox);
	EnableScrollBar(false, PG_ScrollBar::HORIZONTAL);
}

PG_ListBox::~PG_ListBox() {}

void PG_ListBox::AddChild(PG_Widget* item) {
	if(item == NULL) {
		return;
	}

	item->SizeWidget(Width(), item->Height());

	PG_WidgetList::AddChild(item);

	item->SetID(my_scrollarea->GetChildList()->size() - 1);
}

void PG_ListBox::SetMultiSelect(bool multi) {
	my_multiselect = multi;
}

bool PG_ListBox::GetMultiSelect() {
	return my_multiselect;
}

void PG_ListBox::SelectItem(PG_ListBoxBaseItem* item, bool select) {

	if(item == NULL) {
		if(my_selectedItem != NULL) {
			my_selectedItem->Select(false);
			my_selectedItem->Update();
			my_selectedItem = NULL;
		}
		return;
	}

	if(!my_multiselect) {
		if((my_selectedItem != NULL) && (my_selectedItem != item)) {
			my_selectedItem->Select(false);
			my_selectedItem->Update();
		}

		my_selectedItem = item;
		my_selectindex = item->GetID();
		my_selectedItem->Update();
	}

	sigSelectItem(item);
	eventSelectItem(item);
}

bool PG_ListBox::eventSelectItem(PG_ListBoxBaseItem* item) {
	return false;
}

bool PG_ListBox::eventMouseButtonUp(const SDL_MouseButtonEvent* button) {
	return true;
}

bool PG_ListBox::eventMouseButtonDown(const SDL_MouseButtonEvent* button) {
	return true;
}

bool PG_ListBox::eventMouseMotion(const SDL_MouseMotionEvent* motion) {
	return true;
}

void PG_ListBox::RemoveAll() {
   my_selectindex = -1;
   my_selectedItem = NULL;
	my_scrollarea->RemoveAll();
}

void PG_ListBox::DeleteAll() {
   my_selectindex = -1;
	my_selectedItem = NULL;
	my_scrollarea->DeleteAll();
	my_scrollarea->ScrollTo(0,0);
	Update();
}

PG_ListBoxBaseItem* PG_ListBox::GetSelectedItem() {
	return my_selectedItem;
}

void PG_ListBox::SetIndent(Uint16 indent) {
	my_indent = indent;
	PG_RectList* list = my_scrollarea->GetChildList();
	if(list == NULL) {
		return;
	}

	for(PG_Widget* w = list->first(); w != NULL; w = w->next()) {
		PG_ListBoxBaseItem* item = static_cast<PG_ListBoxBaseItem*>(w);
		item->SetIndent(my_indent);
	}
	Update();
}

void PG_ListBox::SelectFirstItem() {
	my_selectindex = 0;
   PG_ListBoxBaseItem* item = dynamic_cast<PG_ListBoxBaseItem*>(FindWidget(0));

	if(item == NULL) {
		return;
	}

	item->Select();
}

void PG_ListBox::SelectNextItem() {
   PG_ListBoxBaseItem* item = dynamic_cast<PG_ListBoxBaseItem*>(FindWidget(my_selectindex+1));

	if(item == NULL) {
		return;
	}

	my_selectindex++;
	item->Select();
}

void PG_ListBox::SelectPrevItem() {
   PG_ListBoxBaseItem* item = dynamic_cast<PG_ListBoxBaseItem*>(FindWidget(my_selectindex-1));

	if(item == NULL) {
		return;
	}

	my_selectindex--;
	item->Select();
}

int PG_ListBox::GetSelectedIndex() {
	return my_selectindex;
}

void PG_ListBox::GetSelectedItems(std::vector<PG_ListBoxBaseItem*>& items) {
	PG_RectList* list = my_scrollarea->GetChildList();
	if(list == NULL) {
		return;
	}

	for(PG_Widget* i = list->first(); i != NULL; i = i->next()) {
		PG_ListBoxBaseItem* item = static_cast<PG_ListBoxBaseItem*>(i);
		if (item->IsSelected()) {
			items.push_back(item);
		}
	}
}

Uint16 PG_ListBox::GetIndent() {
	return my_indent;
}

void PG_ListBox::SetAlignment(PG_Label::TextAlign style) {
	my_alignment = style;

	PG_RectList* list = my_scrollarea->GetChildList();
	if(list == NULL) {
		return;
	}

	for(PG_Widget* i = list->first(); i != NULL; i = i->next()) {
		static_cast<PG_ListBoxBaseItem*>(i)->SetAlignment(style);
	}
	Update();
}

PG_Label::TextAlign PG_ListBox::GetAlignment() {
	return my_alignment;
}
