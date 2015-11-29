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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgspinnerbox.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

/** \file pgspinnerbox.h
	Header file for the PG_SpinnerBox class.
*/

#ifndef PG_SPINNERBOX_H
#define PG_SPINNERBOX_H

#include "pgthemewidget.h"

class PG_Button;
class PG_MaskEdit;
class PG_LineEdit;

/**
 * @author Atani - Mike Dunston
 *
 * @short PG_SpinnerBox creates a textbox with attached increase/decrease buttons to control the numeric value.
 *
 * PG_SpinnerBox creates a textbox with attached increase/decrease buttons to
 * control the numeric value.   Many convenience methods are available...
 * SetValue, SetMinValue, SetMaxValue, SetMask
 * GetValue, GetMinValue, GetMaxValue, GetMask
 *
 * Registerable Events:
 *
 * MSG_SPINNER_CHANGE -
 *
 * Fired when the user clicks either up or down on the spinner buttons.
 * Currently typed modifications are not populated to the value.  This will be
 * added soon.  Note: using SetValue above will fire this event.
 */

class DECLSPEC PG_SpinnerBox : public PG_ThemeWidget {
public:

	/**
	Signal type declaration
	**/
class SignalChange : public sigc::signal<bool, PG_SpinnerBox*, int> {}
	;

	enum {
	    IDSPINNERBOX_UP = PG_WIDGETID_INTERNAL + 12,
	    IDSPINNERBOX_DOWN = PG_WIDGETID_INTERNAL + 13
	};

	/**
	*/
	PG_SpinnerBox(PG_Widget *parent, const PG_Rect& r = PG_Rect::null, const std::string& style = "SpinnerBox");

	void SetValue(int value) {
		m_iValue = value;
		SetTextValue();
	}
	void SetMinValue( int value ) {
		m_iMinValue = value;
	}
	void SetMaxValue( int value ) {
		m_iMaxValue = value;
	}

	void SetMask( const std::string& value );

	int GetValue() {
		return( m_iValue );
	}
	int GetMinValue() {
		return( m_iMinValue );
	}
	int GetMaxValue() {
		return( m_iMaxValue );
	}
	const PG_String& GetMask() {
		return( m_sMask );
	}

	SignalChange sigChange;

protected:

	bool handleButtonClick(PG_Button* button);

	bool handleEditEnd();

private:

	DLLLOCAL void SetTextValue();

	DLLLOCAL void AdjustSize();

	PG_Widget* m_pParent;

	PG_MaskEdit* m_pEditBox;

	PG_Button* m_pButtonUp;

	PG_Button* m_pButtonDown;

	int m_iMinValue;

	int m_iMaxValue;

	int m_iValue;

	PG_String m_sMask;

};

#endif	// PG_SPINNERBOX_H
