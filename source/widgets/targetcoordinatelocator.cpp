
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "targetcoordinatelocator.h"
#include "../dialogs/fieldmarker.h"
#include "../spfst.h"
#include "../spfst-legacy.h"
#include "../mainscreenwidget.h"
#include "../mapdisplay.h"
#include "../iconrepository.h"



SDL_Surface* TargetCoordinateLocator :: getImage()
{
   return IconRepository::getIcon("target.png").getBaseSurface();
};
bool 	TargetCoordinateLocator :: eventMouseButtonDown (const SDL_MouseButtonEvent *button)
{
   if ( button->button == 3 ) {
      SelectFromMap sfm( positions, actmap, false, true);
      sfm.Show();
      sfm.RunModal();
   }
   if ( button->button == 1 ) {
      MapDisplayPG* md = getMainScreenWidget()->getMapDisplay();
      md->cursor.goTo( *positions.begin() );
      actmap->getCursor() = *positions.begin();
      cursorMoved();
   }

   return true;
}

TargetCoordinateLocator::TargetCoordinateLocator(PG_Widget* parent, const PG_Point& pos, const SelectFromMap::CoordinateList& coordinates ) : PG_Image( parent, pos, getImage(), false )
{
   positions = coordinates;
}

