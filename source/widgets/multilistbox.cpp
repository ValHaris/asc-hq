
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "multilistbox.h"


MultiListBox :: MultiListBox (PG_Widget *parent, const PG_Rect &r ) : PG_Widget( parent, r )
{
   SetTransparency( 255 );

   listbox = new PG_ListBox( parent, PG_Rect( r.x, r.y, r.w, r.h - 30 ) );
   listbox->SetMultiSelect( true );

   (new PG_Button( parent, PG_Rect( r.x, r.y + r.h - 25, r.w/2-5, 25 ), "All"))->sigClick.connect( sigc::mem_fun( *this, &MultiListBox::all ));
   (new PG_Button( parent, PG_Rect( r.x + r.w/2 + 5, r.y + r.h - 25, r.w/2-5, 25 ), "None"))->sigClick.connect( sigc::mem_fun( *this, &MultiListBox::none ));
}

bool MultiListBox::all()
{
   for ( int i = 0; i < listbox->GetWidgetCount(); ++i ) {
      PG_ListBoxBaseItem* bi = dynamic_cast<PG_ListBoxBaseItem*>(listbox->FindWidget(i));
      if ( bi )
         bi->Select( true );
   }
   listbox->Update();
   return true;
}

bool MultiListBox::none()
{
   for ( int i = 0; i < listbox->GetWidgetCount(); ++i ) {
      PG_ListBoxBaseItem* bi = dynamic_cast<PG_ListBoxBaseItem*>(listbox->FindWidget(i));
      if ( bi )
         bi->Select( false );
   }
   listbox->Update();
   return true;
}

