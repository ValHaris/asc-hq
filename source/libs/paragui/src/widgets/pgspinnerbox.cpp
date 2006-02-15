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
    Update Date:      $Date: 2006-02-15 21:30:16 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgspinnerbox.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

#include "pgspinnerbox.h"
#include "pgmaskedit.h"
#include "pglineedit.h"
#include "pgbutton.h"

/**
  *@author Atani - Mike Dunston
  */

PG_SpinnerBox::PG_SpinnerBox(PG_Widget *parent, const PG_Rect& r, const std::string& style) : PG_ThemeWidget( parent, r, style ) {
	PG_Rect box_rect = r;
	PG_Rect up_rect, down_rect;
	box_rect.my_width -= (my_height/2);
	if( box_rect.my_width < my_height ) {
		box_rect.my_width = my_height;
	}
	SizeWidget( box_rect.my_width+(my_height/2), my_height );

	m_pParent = parent;

	box_rect.my_xpos = box_rect.my_ypos = 0;
	up_rect.SetRect( box_rect.my_width, 0, (my_height/2), (my_height/2));
	down_rect.SetRect( box_rect.my_width, my_height - (my_height/2), (my_height/2), (my_height/2));

	m_pEditBox = new PG_MaskEdit(this, box_rect, style);
	m_pEditBox->sigEditEnd.connect(slot(*this, &PG_SpinnerBox::handleEditEnd));

	m_pButtonUp = new PG_Button(this, up_rect);
	m_pButtonUp->SetID(IDSPINNERBOX_UP);
	m_pButtonUp->sigClick.connect(slot(*this, &PG_SpinnerBox::handleButtonClick));
	m_pButtonUp->LoadThemeStyle(style, "ButtonUp");

	m_pButtonDown = new PG_Button( this, down_rect);
	m_pButtonDown->SetID(IDSPINNERBOX_DOWN);
	m_pButtonDown->sigClick.connect(slot(*this, &PG_SpinnerBox::handleButtonClick));
	m_pButtonDown->LoadThemeStyle(style, "ButtonDown");

	m_iMinValue = 0;
	m_iMaxValue = 99;
	m_iValue = 0;
	SetMask( "##" );
	m_pEditBox->SetText( "0" );
	m_pEditBox->SetValidKeys("-0123456789");
}

bool PG_SpinnerBox::handleButtonClick(PG_Button* button) {

	switch(button->GetID()) {

		case IDSPINNERBOX_UP: // Up
			if( m_iValue < m_iMaxValue ) {
				m_iValue++;
				SetTextValue();
				return true;
			}
			return false;

		case IDSPINNERBOX_DOWN: // Down
			if( m_iValue > m_iMinValue ) {
				m_iValue--;
				SetTextValue();
				return true;
			}
			return false;
	}

	return false;
}

void PG_SpinnerBox::SetTextValue() {
	m_pEditBox->SetTextFormat("%u", m_iValue );
	m_pEditBox->Update();
	sigChange(this, m_iValue);
}

void PG_SpinnerBox::AdjustSize() {
	Uint16 x, y;

	PG_FontEngine::GetTextSize(m_sMask, GetFont(), &x, &y);
	x += 3;
	y = m_pEditBox->my_height;

	Hide();

	SizeWidget( x+ (y >> 1), y );
	m_pEditBox->SizeWidget( x, y );
	m_pButtonUp->MoveWidget( x, 0 );
	m_pButtonDown->MoveWidget( x, y - (y >> 1) );

	Show();
}

bool PG_SpinnerBox::handleEditEnd(PG_LineEdit* edit) {
	const std::string& text = m_pEditBox->GetText();
	m_iValue = (!text.empty()) ? atoi(text.c_str()) : 0;

	// AMC Dec.4/2001
	if (m_iValue>m_iMaxValue) {
		m_iValue=m_iMaxValue;
	}

	if (m_iValue<m_iMinValue) {
		m_iValue=m_iMinValue;
	}

	SetTextValue();
	return true;
}

void PG_SpinnerBox::SetMask(const std::string& value) {
	m_sMask = value;
	m_pEditBox->SetMask( m_sMask );
	AdjustSize();
}
