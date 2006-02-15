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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgmessagebox.h,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

/** \file pgmessagebox.h
	Header file for the PG_MessageBox class.
*/

#ifndef PG_MESSAGEBOX_H
#define PG_MESSAGEBOX_H

#include "paragui.h"
#include "pglabel.h"
#include "pgwindow.h"

class PG_Button;
class PG_MultiLineEdit;

/**
	@author Thomas Bamesberger
*/

class DECLSPEC PG_MessageBox : public PG_Window {
public:
	/**
	Creates a PopUp with 2 Buttons
		
	@param parent Parent widget
	@param r rectangle of PopUp
	@param windowtitle Title of window
	@param windowtext Text to appear in window
	@param btn1 Struct PG_Rect to create Button 1
	@param btn1text Text to appear in Button 1
	@param btn2 Struct PG_Rect to create Button 2
	@param btn2text Text to appear in Button 2
	@param textalign Alignment for windowtext
	@param style widgetstyle to use (default "MessageBox")
	*/
	PG_MessageBox(PG_Widget* parent, const PG_Rect& r, const std::string& windowtitle, const std::string& windowtext, const PG_Rect& btn1, const std::string& btn1text, const PG_Rect& btn2, const std::string& btn2text, PG_Label::TextAlign textalign = PG_Label::CENTER, const std::string& style="MessageBox");

	/**
	Creates a PopUp with 1 Button

	@param parent Parent widget
	@param r rectangle of PopUp
	@param windowtitle Title of window
	@param windowtext Text to appear in window
	@param btn1 Struct PG_Rect to create Button 1
	@param btn1text Text to appear in Button 1
	@param textalign Alignment for windowtext
	@param style widgetstyle to use (default "MessageBox")
	*/
	PG_MessageBox(PG_Widget* parent, const PG_Rect& r, const std::string& windowtitle, const std::string& windowtext, const PG_Rect& btn1, const std::string& btn1text, PG_Label::TextAlign textalign = PG_Label::CENTER, const std::string& style="MessageBox");

	/**
	Destructor
	*/
	~PG_MessageBox();

	void LoadThemeStyle(const std::string& widgettype);

	/**
	OBSOLETE - Waits for a button click and returns Button ID.
	This method is obsolete. Please use the RunModal method.
	*/
	inline int WaitForClick() {
		return RunModal();
	}

protected:

	/**
	Checks if button is pressed

	@param button pointer to PG_BUtton
	*/
	virtual bool handleButton(PG_Button* button);

	PG_Button* my_btnok;

	PG_Button* my_btncancel;

private:

	DLLLOCAL void Init(const std::string& windowtext, int textalign, const std::string& style) ;

	PG_MultiLineEdit* my_textbox;

	int my_msgalign;

};

#endif //PG_MESSAGEBOX_H
