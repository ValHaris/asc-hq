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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgpropertyfield_string.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

#ifndef PG_PROPERTYFIELD_STRING_H
#define PG_PROPERTYFIELD_STRING_H

#include "pgpropertyeditor_linefield.h"


/** @class PG_PropertyField_Integer
   @author Martin Bickel
 
   Integer field for a PG_PropertyEditor
 */
template<class StringType=std::string>
class PG_PropertyField_String : public PG_PropertyEditor_LineField {
   StringType* myProperty;

   protected:

      bool EditEnd(PG_LineEdit* widget) {
         sigValueChanged(this,lineEdit->GetText());
         return true;
      }

   public:
      typedef sigc::signal<PG_PropertyField_String*, StringType> StringPropertySignal;
      StringPropertySignal sigValueChanged;
      StringPropertySignal sigValueApplied;

   /** Creates a PG_PropertyField_String
      \param propertyEditor The PG_PropertyEditor for which the PropertyField shall be registered for
      \param name  The label shown for the PropertyField
      \param string  The variable that is edited. Make sure the variable exists for the whole lifetime of the PG_PropertyEditor
    */
      PG_PropertyField_String( PG_PropertyEditor* propertyEditor, const std::string& name, StringType* string ) : PG_PropertyEditor_LineField( propertyEditor, name ), myProperty( string ) {
         Reload();
      };

   /** Creates a PG_PropertyField_String
      \param propertyEditor The PG_PropertyEditor for which the PropertyField shall be registered for
      \param name  The label shown for the PropertyField
      \param string  The initial value for the editor field
    */
      PG_PropertyField_String( PG_PropertyEditor* propertyEditor, const std::string& name, const StringType& string ) : PG_PropertyEditor_LineField( propertyEditor, name ), myProperty( NULL ) {
         lineEdit->SetText( string );
      };

      bool Valid() {
         return true;
      };
      bool Apply() {
         if ( myProperty )
            *myProperty = lineEdit->GetText();

         sigValueApplied( this, lineEdit->GetText() );

         return true;
      };
      void Reload() {
         if ( myProperty )
            lineEdit->SetText( *myProperty );
      };

      void SetPassHidden (const PG_Char &passchar) {
         lineEdit->SetPassHidden( passchar );
      };
};

#endif
