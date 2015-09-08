/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <boost/regex.hpp>

#include "gotoposition.h"
#include "../gamemap.h"
#include "../mainscreenwidget.h"
#include "../mapdisplay.h"

bool GotoPosition::ok()
{
   static boost::regex numercial("\\d+");

   if( boost::regex_match( xfield->GetText(), numercial)  &&
         boost::regex_match( yfield->GetText(), numercial)) {
      int xx = atoi( xfield->GetText() );
      int yy = atoi( yfield->GetText() );
      if ( xx >= 0 && yy >= 0 && xx < gamemap->xsize && yy < gamemap->ysize ) {
         Hide();
         MapDisplayPG* md = getMainScreenWidget()->getMapDisplay();
         md->cursor.goTo( MapCoordinate( xx, yy) );
         QuitModal();
         return true;
      }
   }
   return false;
}
      
bool GotoPosition::cancel()
{
   QuitModal();
   return true;
}

bool GotoPosition::line1completed()
{
   if ( yfield ) {
      yfield->EditBegin();
      return true;
   } else
      return false;
}
      
GotoPosition::GotoPosition ( GameMap* gamemap ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 300, 120), "Enter Coordinates")
{
   this->gamemap = gamemap;
   int fieldwidth = (Width()-3*border)/2;
   xfield = new PG_LineEdit( this, PG_Rect( border, 40, fieldwidth, 20));
   // xfield->SetText( ASCString::toString( gamemap->getCursor().x ));
   xfield->sigEditReturn.connect( sigc::mem_fun( *this, &GotoPosition::line1completed ));

   yfield = new PG_LineEdit( this, PG_Rect( (Width()+border)/2, 40, fieldwidth, 20));
   // yfield->SetText( ASCString::toString( gamemap->getCursor().y ));
   yfield->sigEditReturn.connect( sigc::mem_fun( *this, &GotoPosition::ok ));

   AddStandardButton( "~O~k" )->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &GotoPosition::ok )));
};

int GotoPosition::RunModal()
{
   xfield->EditBegin();
   return ASC_PG_Dialog::RunModal();
}

