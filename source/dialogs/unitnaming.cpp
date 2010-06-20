/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "unitnaming.h"
#include "../gamemap.h"
#include "../actions/renamecontainercommand.h"
#include "../contextutils.h"

bool UnitNaming::ok()
{
   if ( publicName->GetText() != unit->name || privateName->GetText() != unit->privateName ) {
      auto_ptr<RenameContainerCommand> rcc ( new RenameContainerCommand( unit ));
      rcc->setName( publicName->GetText(), privateName->GetText() );
      ActionResult res = rcc->execute( createContext( unit->getMap() ));
      if ( res.successful() ) 
         rcc.release();
   }
   QuitModal();
   return true;
}
      
bool UnitNaming::cancel()
{
   QuitModal();
   return true;
}

bool UnitNaming::line1completed()
{
   if ( privateName ) {
      privateName->EditBegin();
      return true;
   } else
      return false;
}
      
UnitNaming::UnitNaming ( ContainerBase* myUnit) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 300, 150), "Enter Names")
{
   unit = myUnit;
   
   int width = 90;
   new PG_Label( this, PG_Rect( border, 40, width, 20), "Public: ");
   
   int fieldwidth = Width() - 3 * border - width;
   publicName = new PG_LineEdit( this, PG_Rect( border*2 + width, 40, fieldwidth, 20));
   publicName->SetText( myUnit->name );
   publicName->sigEditReturn.connect( SigC::slot( *this, &UnitNaming::line1completed ));

   new PG_Label( this, PG_Rect( border, 70, width, 20), "Private: ");
   privateName = new PG_LineEdit( this, PG_Rect( border*2 + width, 70, fieldwidth, 20));
   privateName->SetText( myUnit->privateName);
   privateName->sigEditReturn.connect( SigC::slot( *this, &UnitNaming::ok ));

   AddStandardButton( "~O~k" )->sigClick.connect( SigC::slot( *this, &UnitNaming::ok ));
};

int UnitNaming::RunModal()
{
   publicName->EditBegin();
   return ASC_PG_Dialog::RunModal();
}

