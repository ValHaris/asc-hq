/***************************************************************************
                             TargetCoordinateLocator
                             -------------------
    copyright            : (C)  2006 by Martin Bickel
    email                : <bickel@asc-hq.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef targetcoordinatelocatorH
#define targetcoordinatelocatorH

#include <SDL.h>

#include "pgwidget.h"
#include "pgimage.h"
#include "../dialogs/fieldmarker.h"

class TargetCoordinateLocator : public PG_Image  {
      static SDL_Surface* getImage();
      SelectFromMap::CoordinateList positions;
   protected:
      bool 	eventMouseButtonDown (const SDL_MouseButtonEvent *button);

   public:
      TargetCoordinateLocator(PG_Widget* parent, const PG_Point& pos, const SelectFromMap::CoordinateList& coordinates );

};

#endif
