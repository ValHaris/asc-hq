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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgpropertyfield_intdropdown.h,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

#ifndef PG_PROPERTYFIELD_INTDROPDOWN_H
#define PG_PROPERTYFIELD_INTDROPDOWN_H

#include "pgpropertyeditor_linefield.h"
#include "pgdropdown.h"

/** @class PG_PropertyField_IntDropDown
   @author Martin Bickel
 
   @short DropDown selector for a PG_PropertyEditor
 
   Editor for numerical values where each value is associated by a name. The value is selected by chosing the name
   from the dropdown widget.
 */
template<typename IntType, typename iterator = char*>
class PG_PropertyField_IntDropDown: public PG_PropertyEditor::PG_PropertyEditorField, public SigC::Object  {
private:
	IntType* myProperty;
	PG_DropDown* dropdown;

	bool click() {
		int value = dropdown->GetSelectedItemIndex();
		sigValueChanged(this,value);
		return true;
	}

	void init( PG_PropertyEditor* propertyEditor, const std::string& name ) {
		PG_Rect r = propertyEditor->RegisterProperty( name, this );
		dropdown = new PG_DropDown( propertyEditor, r, -1, propertyEditor->GetStyleName("DropDownSelectorProperty") );
		dropdown->SetEditable( false );
		dropdown->sigSelectItem.connect( SigC::slot(*this, &PG_PropertyField_IntDropDown::click));
	}

public:
	typedef PG_Signal2<PG_PropertyField_IntDropDown*, IntType> DropDownPropertySignal;
	DropDownPropertySignal sigValueChanged;
	DropDownPropertySignal sigValueApplied;

	/** Creates a PG_PropertyField_IntDropDown
	   \param propertyEditor The PG_PropertyEditor for which the PropertyField shall be registered for
	   \param name  The label shown for the PropertyField
	   \param var  The variable that is edited. Make sure the variable exists for the whole lifetime of the PG_PropertyEditor
	   \param names A NULL terminated array of names for the different values of var
	 */
	PG_PropertyField_IntDropDown( PG_PropertyEditor* propertyEditor, const std::string& name, IntType* var, const char** names ) : myProperty( var ) {
		init( propertyEditor, name );

		int i = 0;
		while ( names[i] ) {
			dropdown->AddItem( names[i] );
			++i;
		}
		dropdown->SelectItem( *var );
	};

	/** Creates a PG_PropertyField_IntDropDown
	   \param propertyEditor The PG_PropertyEditor for which the PropertyField shall be registered for
	   \param name  The label shown for the PropertyField
	   \param var  The variable that is edited. Make sure the variable exists for the whole lifetime of the PG_PropertyEditor
	   \param begin the start iterator of a container carrying the names for the values of var
	   \param end the end iterator
	 */
	PG_PropertyField_IntDropDown( PG_PropertyEditor* propertyEditor, const std::string& name, IntType* var, iterator begin, iterator end ) : myProperty( var ) {
		init( propertyEditor, name );

		while ( begin != end ) {
			dropdown->AddItem( *begin );
			++begin;
		}
		dropdown->SelectItem( *var );
	};

	/** Creates a PG_PropertyField_IntDropDown
	   \param propertyEditor The PG_PropertyEditor for which the PropertyField shall be registered for
	   \param name  The label shown for the PropertyField
	   \param var  The initial value of the field
	   \param names A NULL terminated array of names for the different values of var
	 */
	PG_PropertyField_IntDropDown( PG_PropertyEditor* propertyEditor, const std::string& name, const IntType& var, const char** names ) : myProperty( NULL ) {
		init( propertyEditor, name );

		int i = 0;
		while ( names[i] ) {
			dropdown->AddItem( names[i] );
			++i;
		}
		dropdown->SelectItem( var );
	};

	/** Creates a PG_PropertyField_IntDropDown
	   \param propertyEditor The PG_PropertyEditor for which the PropertyField shall be registered for
	   \param name  The label shown for the PropertyField
	   \param var  The initial value of the field
	   \param begin the start iterator of a container carrying the names for the values of var
	   \param end the end iterator
	 */
	PG_PropertyField_IntDropDown( PG_PropertyEditor* propertyEditor, const std::string& name, const IntType& var, iterator begin, iterator end ) : myProperty( NULL ) {
		init( propertyEditor, name );

		while ( begin != end ) {
			dropdown->AddItem( *begin );
			++begin;
		}
		dropdown->SelectItem( var );
	};


	bool Valid() {
		return true;
	};

	bool Apply() {
		if ( myProperty )
			*myProperty = dropdown->GetSelectedItemIndex();

		sigValueApplied( this, dropdown->GetSelectedItemIndex() );

		return true;
	};

	void Reload() {
		if ( myProperty )
			dropdown->SelectItem( *myProperty );
	};
};


#endif
