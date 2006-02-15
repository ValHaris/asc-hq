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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgpropertyeditor.h,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

/** @file pgpropertyeditor.h
	Header file for the PG_PropertyEditor class.
*/

#ifndef PG_PROPERTYEDITOR_H
#define PG_PROPERTYEDITOR_H

#include "paragui.h"
#include "pgscrollwidget.h"

/** @class PG_PropertyEditor
	@author Martin Bickel
 
	@short Widget for editing properties.
 
	This is a property editor as popular with VisualBasic and other RAD tools
	It is a container for other a number of PropertyFields 
*/

class DECLSPEC PG_PropertyEditor : public PG_ScrollWidget {
public:

	/**
	   Creates a PG_PropertyEditor widget

	   @param parent     the parentobject for the new widget or NULL if it is a toplevel widget 
	   @param rect    initial position for the widget
	   @param style      Widgetstyle to load. This style should be defined at your theme file (default = "PropertyEditor")
	   @param labelWidthPercentage   The PropertyEditor consists of two columns: label and editable field. This sets the width of the label column
	 */
	PG_PropertyEditor ( PG_Widget *parent, const PG_Rect &rect, const std::string &style="PropertyEditor", int labelWidthPercentage = 70 );

	//! Interface for all the different PropertyFields
	class PG_PropertyEditorField {
	public:
		//! checks if the entered value is valid
		virtual bool Valid() = 0;

		/** Write the entered value back into the referenced variable (if it exists), provided the value is valid.
		\returns false if the value is not valid
		*/
		virtual bool Apply() = 0;

		//! Reloads the entry field with the current value of the referenced variable (if it exists)
		virtual void Reload() = 0;

		//! Sets the Focus onto the field
		virtual void Focus() {}
		;
		virtual ~PG_PropertyEditorField() {}
		;
	};


	//! Returns the style name that the PropertyFields shall use
	virtual std::string GetStyleName( const std::string& widgetName );

	//! Reloads all PropertyFields from their original values and updates the widget
	void Reload();

	//! Checks if all PropertyFields contain valid values
	bool Valid();

	//! Writes the PropertyFields' values to their original variables, provided they are valid
	bool Apply();

	/** Adds a PropertyField to the widget.

	    The PropertyEditor will take ownership of the PropertyEditorFields and delete them on its
	    own destruction.
	*/
	PG_Rect RegisterProperty( const std::string& name, PG_PropertyEditorField* propertyEditorField );

	~PG_PropertyEditor() ;

private:
	typedef std::vector<PG_PropertyEditorField*> PropertyFieldsType;
	PropertyFieldsType propertyFields;

	std::string styleName;

	int ypos;
	int lineHeight;
	int lineSpacing;
	int labelWidth;
};



#endif
