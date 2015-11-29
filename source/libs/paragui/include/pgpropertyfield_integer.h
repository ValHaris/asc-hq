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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgpropertyfield_integer.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

#ifndef PG_PROPERTYFIELD_INTEGER_H
#define PG_PROPERTYFIELD_INTEGER_H

#include <sstream>
#include <limits>

#include "pgpropertyeditor_linefield.h"


/** @class PG_PropertyField_Integer
   @author Martin Bickel
 
   Integer field for a PG_PropertyEditor
 */
template <class IntegerType=int>
class PG_PropertyField_Integer : public PG_PropertyEditor_LineField {
	IntegerType* myProperty;
	IntegerType minValue;
	IntegerType maxValue;
protected:
	bool convert( IntegerType& i ) {
		std::istringstream s( lineEdit->GetText() );
		if ( !( s >> i ))
			return false;

		return s.eof();
	};

	void set
		( IntegerType i ) {
		std::ostringstream s;
		s << i;
		lineEdit->SetText( s.str() );
	}

	bool EditEnd() {
		IntegerType i ;
		if ( convert(i)) {
			sigValueChanged(this,i);
			return true;
		} else
			return false;
	}

public:

	typedef sigc::signal<void, PG_PropertyField_Integer*, IntegerType> IntegerPropertySignal;
	IntegerPropertySignal sigValueChanged;
	IntegerPropertySignal sigValueApplied;

	/** Creates a PG_PropertyField_Integer
	\param propertyEditor The PG_PropertyEditor for which the PropertyField shall be registered for
	\param name  The label shown for the PropertyField
	\param myInteger  The variable that is edited. Make sure the variable exists for the whole lifetime of the PG_PropertyEditor
	   */
	PG_PropertyField_Integer( PG_PropertyEditor* propertyEditor, const std::string& name, IntegerType* myInteger ) : PG_PropertyEditor_LineField( propertyEditor, name ), myProperty( myInteger ) {
		minValue = std::numeric_limits<IntegerType>::min();
		maxValue = std::numeric_limits<IntegerType>::max();
		Reload();
	};

	/** Creates a PG_PropertyField_Integer
	\param propertyEditor The PG_PropertyEditor for which the PropertyField shall be registered for
	\param name  The label shown for the PropertyField
	\param myInteger  The initial value for the editor field
	 */
	PG_PropertyField_Integer( PG_PropertyEditor* propertyEditor, const std::string& name, IntegerType myInteger ) : PG_PropertyEditor_LineField( propertyEditor, name ), myProperty( NULL ) {
		minValue = std::numeric_limits<IntegerType>::min();
		maxValue = std::numeric_limits<IntegerType>::max();
		set
			( myInteger );
	};

	void SetRange( IntegerType min, IntegerType max ) {
		if ( min <= max ) {
			minValue = min;
			maxValue = max;
		}
	}

	virtual bool CheckValue( IntegerType value ) {
		return value >= minValue && value <= maxValue;
	}

	bool Valid() {
		IntegerType i;
		if ( !convert(i) )
			return false;

		if ( !CheckValue( i ))
			return false;

		return true;
	};

	bool Apply() {
		IntegerType i;
		if ( !convert(i) )
			return false;

		if ( !CheckValue( i ))
			return false;

		sigValueApplied( this, i );

		if ( myProperty )
			*myProperty = i;

		return true;
	};

	void Reload() {
		if ( myProperty )
			set
				( *myProperty );
	};

};

#endif
