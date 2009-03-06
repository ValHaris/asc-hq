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
    Update Date:      $Date: 2009-03-06 15:16:32 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgdropdown.cpp,v $
    CVS/RCS Revision: $Revision: 1.4 $
    Status:           $State: Exp $
*/

#include <algorithm>

#include "pgdropdown.h"
#include "pglog.h"
#include "pglistbox.h"
#include "pglistboxitem.h"
#include "pgapplication.h"

PG_DropDown::PG_DropDown(PG_Widget* parent, const PG_Rect& r, int id, const std::string& style) : PG_Widget(parent, r),
		// needed for AddChild() evaluation - H. C.
my_EditBox(NULL), my_DropButton(NULL), my_DropList(NULL) {
	PG_Rect rect(0, 0, r.my_width - r.my_height, r.my_height);

	SetID(id);

	my_EditBox = new PG_LineEdit(this, rect, style);
	my_EditBox->sigEditBegin.connect(sigEditBegin.slot());
	my_EditBox->sigEditEnd.connect(sigEditEnd.slot());
	my_EditBox->sigEditReturn.connect(sigEditReturn.slot());

	PG_Rect rbutton(abs(r.my_width - r.my_height), 0, r.my_height, r.my_height);
	my_DropButton = new PG_Button(this, rbutton, PG_NULLSTR, -1, style);
	my_DropButton->SetID(IDDROPDOWN_BOX);
	my_DropButton->sigClick.connect(slot(*this, &PG_DropDown::handleButtonClick));

	PG_Rect rlist(r.my_xpos, r.my_ypos + r.my_height +1, r.my_width, r.my_height /* * 5 */);
	my_DropList = new PG_ListBox(NULL, rlist, style);
	my_DropList->SetAutoResize(true, true);
	my_DropList->SetShiftOnRemove(false, true);

	//my_DropList->EnableScrollBar(false);
	my_DropList->sigSelectItem.connect(slot(*this, &PG_DropDown::select_handler));
	my_DropList->sigDelete.connect(slot(*this, &PG_DropDown::onDropListDeletion));

	LoadThemeStyle(style);
}


bool PG_DropDown::onDropListDeletion( const PG_MessageObject* dropList ) {
	if ( dropList == my_DropList )
		my_DropList = NULL;
	return true;
}


PG_DropDown::~PG_DropDown() {
	delete my_DropList;
}

void PG_DropDown::LoadThemeStyle(const std::string& style) {
	my_EditBox->LoadThemeStyle(style);
	my_DropButton->LoadThemeStyle(style);
	my_DropList->LoadThemeStyle(style);
}

void PG_DropDown::AddChild(PG_Widget* child) {
	if (my_EditBox == NULL || my_DropButton == NULL || my_DropList == NULL) {
		PG_Widget::AddChild(child);
	} else {
		my_DropList->AddChild(child);
		//my_DropList->SizeWidget(my_width, my_DropList->GetListHeight() + my_DropList->GetBorderSize()*2);
	}
}

// obsolete -> roadmap
void PG_DropDown::AddItem(const std::string& text, void* userdata, Uint16 height) {
	Uint16 h = height;
	static PG_String ytext = "A";

	if(height == 0) {
		PG_FontEngine::GetTextSize(ytext, GetFont(), NULL, NULL, NULL, NULL, &h);
		h += 2;
	}

	new PG_ListBoxItem(this, h, text, NULL, userdata);
	//my_DropList->SizeWidget(my_width, my_DropList->GetListHeight() + my_DropList->GetBorderSize()*2);
}


void PG_DropDown::RemoveAll() {
	if ( my_DropList )
		my_DropList->RemoveAll();
}

void PG_DropDown::DeleteAll() {
	if ( my_DropList )
		my_DropList->DeleteAll();
}

void PG_DropDown::eventShow() {
	if ( my_DropList )
		my_DropList->SetVisible(false);
}

void PG_DropDown::eventHide() {
	if ( my_DropList )
		my_DropList->Hide();
}

bool PG_DropDown::handleButtonClick(PG_Button* button) {
	if(button->GetID() != IDDROPDOWN_BOX) {
		return false;
	}

	if(my_DropList->IsVisible()) {
		my_DropList->Hide();
	} else {
		PG_Rect pos = *my_DropList;
		pos.x = my_xpos;
		pos.y = my_ypos+my_height;

		pos.w = PG_MIN( my_DropList->my_width, PG_Application::GetScreenWidth() - pos.x );
		pos.h = PG_MIN( my_DropList->my_height, PG_Application::GetScreenHeight() - pos.y);

		my_DropList->MoveWidget( pos );

		my_DropList->Show();
	}

	return true;
}

void PG_DropDown::SetIndent(Uint16 indent) {
	my_DropList->SetIndent(indent);
}

void PG_DropDown::SetEditable(bool edit) {
	my_EditBox->SetEditable(edit);
}

bool PG_DropDown::GetEditable() {
	return my_EditBox->GetEditable();
}

const PG_String& PG_DropDown::GetText() {
	return my_EditBox->GetText();
}

void PG_DropDown::SetText(const std::string& new_text) {
	my_EditBox->SetText(new_text);
}

bool PG_DropDown::eventSelectItem(PG_ListBoxBaseItem* item) {
	return false;
}

void PG_DropDown::eventSizeWidget(Uint16 w, Uint16 h) {}

void PG_DropDown::eventMoveWidget(int x, int y) {
	if(my_DropList && my_DropList->IsVisible()) {
		my_DropList->Hide();
	}
}

bool PG_DropDown::select_handler(PG_ListBoxBaseItem* item) {
	my_EditBox->SetText(item->GetText());
	item->Select(false);
	my_DropList->SelectItem(NULL);
	my_DropList->Hide();

	eventSelectItem(item);
	sigSelectItem(item);

	return true;
}

bool PG_DropDown::ProcessEvent(const SDL_Event * event, bool bModal) {

	if(bModal && my_DropList->IsVisible()) {
		if(my_DropList->ProcessEvent(event, true)) {
			return true;
		}
	}

	return PG_Widget::ProcessEvent(event, bModal);
}

void PG_DropDown::SelectFirstItem() {
	my_DropList->SelectFirstItem();
}

void PG_DropDown::SelectNextItem() {
	my_DropList->SelectNextItem();
}

void PG_DropDown::SelectPrevItem() {
	my_DropList->SelectPrevItem();
}

void PG_DropDown::SelectItem(const int n) {
	int i;

	my_DropList->SelectFirstItem( n == 0 );

	for (i=0; i < n; i++)
		my_DropList->SelectNextItem( i == (n-1));
}

int PG_DropDown::GetSelectedItemIndex() {
	return my_DropList->GetSelectedIndex();
}


Uint16 PG_DropDown::GetIndent() {
	return my_DropList->GetIndent();
}

void PG_DropDown::SetAlignment(PG_Label::TextAlign style) {
	my_DropList->SetAlignment(style);
}

PG_Label::TextAlign PG_DropDown::GetAlignment() {
	return my_DropList->GetAlignment();
}

PG_Widget* PG_DropDown::GetFirstInList() {
	return my_DropList->GetFirstInList();
}
