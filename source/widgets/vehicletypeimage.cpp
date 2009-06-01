/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <pgdraw.h>

#include "vehicletypeimage.h"

#include "../typen.h"
#include "../player.h"
#include "../vehicletype.h"

#include <pgapplication.h>

VehicleTypeImage::VehicleTypeImage( PG_Widget* parent, const PG_Point& pos, const Vehicletype* vehicletype, const Player& owningPlayer )
   : PG_Widget( parent, PG_Rect( pos.x, pos.y, fieldsizex, fieldsizey )),
                vt( vehicletype ), player( owningPlayer )
{
   
}


Surface VehicleTypeImage::clippingSurface;

void VehicleTypeImage::display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst )
{
   if ( !getClippingSurface().valid() )
      getClippingSurface() = Surface::createSurface( fieldsizex + 10, fieldsizey + 10, 32, 0 );

   getClippingSurface().Fill(0);

   vt->paint( getClippingSurface(), SPoint(5,5), player.getPlayerColor(), 0 );
   PG_Draw::BlitSurface( getClippingSurface().getBaseSurface(), PG_Rect( src.x + 5, src.y + 5, src.w, src.h), surface, dst);
}

void VehicleTypeImage::eventBlit ( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst )
{
   display( PG_Application::GetScreen(), src, dst );
};

