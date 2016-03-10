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
    Update Date:      $Date: 2008-02-01 12:19:52 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgpropertyfield_checkbox.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/


#ifndef PG_PROPERTYFIELD_CHECKBOX_H
#define PG_PROPERTYFIELD_CHECKBOX_H

#include "pgpropertyeditor.h"
#include "pgcheckbutton.h"

/** @class PG_PropertyField_Checkbox
	@author Martin Bickel
 
	Checkbox for a PG_PropertyEditor
*/

template<typename B>
class PG_PropertyField_Checkbox : public PG_PropertyEditor::PG_PropertyEditorField, public sigc::trackable  {
	B* myProperty;
	PG_CheckButton* checkbox;
	bool switchInverted;

	bool click( bool b) {
		sigValueChanged(this,b);
		return true;
	}

public:
	typedef sigc::signal<void, PG_PropertyField_Checkbox*, B> CheckboxPropertySignal;
	CheckboxPropertySignal sigValueChanged;
	CheckboxPropertySignal sigValueApplied;

	/** Creates a PG_PropertyField_Checkbox
	\param propertyEditor The PG_PropertyEditor for which the PropertyField shall be registered for
	\param name  The label shown for the PropertyField
	\param b  The variable that is edited. Make sure the variable exists for the whole lifetime of the PG_PropertyEditor
	\param inverted If set the checkbox will be ticked on false and not ticked on true
	*/
	PG_PropertyField_Checkbox( PG_PropertyEditor* propertyEditor, const std::string& name, B* b, bool inverted = false ) : myProperty( b ), switchInverted( inverted ) {
		PG_Rect r = propertyEditor->RegisterProperty( name, this );
		checkbox = new PG_CheckButton( propertyEditor, r, PG_NULLSTR, -1, propertyEditor->GetStyleName("BoolProperty") );
		checkbox->sigClick.connect( sigc::mem_fun(*this, &PG_PropertyField_Checkbox<B>::click));
		Reload();
	};

	/** Creates a PG_PropertyField_Checkbox
	\param propertyEditor The PG_PropertyEditor for which the PropertyField shall be registered for
	\param name  The label shown for the PropertyField
	\param b  The initial value. 
	\param inverted If set the checkbox will be ticked on false and not ticked on true
	 */
	PG_PropertyField_Checkbox( PG_PropertyEditor* propertyEditor, const std::string& name, const B& b, bool inverted = false ) : myProperty( NULL ), switchInverted( inverted ) {
		PG_Rect r = propertyEditor->RegisterProperty( name, this );
		checkbox = new PG_CheckButton( propertyEditor, r, PG_NULLSTR, -1, propertyEditor->GetStyleName("BoolProperty") );
		checkbox->sigClick.connect( sigc::mem_fun(*this, &PG_PropertyField_Checkbox::click));

		if ( bool(b) ^ switchInverted )
			checkbox->SetPressed();
		else
			checkbox->SetUnpressed();
	};

	bool Valid() {
		return true;
	};
	bool Apply() {
		if ( myProperty )
			*myProperty = checkbox->GetPressed() ^ switchInverted;

		sigValueApplied( this, checkbox->GetPressed() ^ switchInverted );
		return true;
	};
	void Reload() {
		if ( myProperty ) {
			if ( bool(*myProperty) ^ switchInverted )
				checkbox->SetPressed();
			else
				checkbox->SetUnpressed();
      }
	};
};

#endif
