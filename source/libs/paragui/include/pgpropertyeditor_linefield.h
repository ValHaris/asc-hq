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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgpropertyeditor_linefield.h,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/


#ifndef PG_PROPERTYEDITOR_LINEFIELD_H
#define PG_PROPERTYEDITOR_LINEFIELD_H

#include "paragui.h"
#include "pglineedit.h"
#include "pgpropertyeditor.h"


/** @class PG_PropertyEditor_LineField
	@author Martin Bickel
 
	Abstract base class for various property editor fields .
*/

class DECLSPEC PG_PropertyEditor_LineField : public PG_PropertyEditor::PG_PropertyEditorField, public SigC::Object {
public:
	void Focus() {
		lineEdit->EditBegin();
	};

protected:
	PG_LineEdit* lineEdit;

	PG_PropertyEditor_LineField ( PG_PropertyEditor* propertyEditor, const std::string& name ) {
		PG_Rect r = propertyEditor->RegisterProperty( name, this );
		lineEdit = new PG_LineEdit( propertyEditor, r, propertyEditor->GetStyleName( "LineFieldProperty" ) );
		lineEdit->sigEditEnd.connect( SigC::slot( *this, &PG_PropertyEditor_LineField::EditEnd ));
	}


	virtual bool EditEnd() = 0;
};

#endif
