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
    Update Date:      $Date: 2007-04-13 16:15:57 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgpropertyfield_button.h,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/


#ifndef PG_PROPERTYFIELD_BUTTON_H
#define PG_PROPERTYFIELD_BUTTON_H

#include "pgpropertyeditor.h"
#include "pgbutton.h"

/** @class PG_PropertyField_Button
	@author Martin Bickel
 
	Button for a PG_PropertyEditor
*/

class PG_PropertyField_Button : public PG_PropertyEditor::PG_PropertyEditorField, public SigC::Object  {
	PG_Button* button;
	bool switchInverted;

	bool click( ) {
		sigClick(this);
		return true;
	}

public:
	typedef PG_Signal1<PG_PropertyField_Button*> ButtonPropertySignal;
   ButtonPropertySignal sigClick;

	/** Creates a PG_PropertyField_Checkbox
	\param propertyEditor The PG_PropertyEditor for which the PropertyField shall be registered for
	\param name  The label shown for the PropertyField
	\param buttonname  The text that shown on the button
	\param height The height of the line, -1 for default line height
	*/
   PG_PropertyField_Button( PG_PropertyEditor* propertyEditor, const std::string& name, const std::string& buttonname, int height = -1 ) {
		PG_Rect r = propertyEditor->RegisterProperty( name, this, height );
      button = new PG_Button( propertyEditor, PG_Rect( r.x + 2, r.y + 2, r.w - 4, r.h -4 ), PG_NULLSTR, -1, propertyEditor->GetStyleName("ButtonProperty") );
      button->SetText( buttonname );
		button->sigClick.connect( SigC::slot(*this, &PG_PropertyField_Button::click));
	};

	bool Valid() {
		return true;
	};
	bool Apply() {
		return true;
	};
	void Reload() {
	};

   PG_Button* GetButton()
   {
      return button;
   };
};

#endif
