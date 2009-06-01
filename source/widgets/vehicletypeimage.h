/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef vehicletypeimageH
#define vehicletypeimageH

#include <pgwidget.h>
#include "../graphics/surface.h"

class Vehicletype;
class Player;

class VehicleTypeImage: public PG_Widget  {
      const Vehicletype* vt;
      const Player& player;
      static Surface clippingSurface;
      Surface& getClippingSurface() { return clippingSurface; };
   public:
      VehicleTypeImage( PG_Widget* parent, const PG_Point& pos, const Vehicletype* vehicletype, const Player& owningPlayer );
      void eventBlit ( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst );
   protected:
      void display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst );
};

#endif
