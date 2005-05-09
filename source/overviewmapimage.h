/***************************************************************************
                          overviewmapimage.h  -  description
                             -------------------
    begin                : Sat Apr 16 2005
    copyright            : (C) 2005 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef overviewmapimageH
#define overviewmapimageH


#include "graphics/surface.h"
#include "typen.h"

//! the image for a terraintype ( #tterraintype ) that is shown on the small map
class OverviewMapImage {
         bool initialized;
      public:
         static const int width = 4;
         static const int height = 4;
         static SPoint map2surface( const MapCoordinate& pos );
         static MapCoordinate surface2map( const SPoint& pos );
         SDLmm::ColorRGBA segment[width][height];
         OverviewMapImage();
         OverviewMapImage( const Surface& image );
         bool valid() const {  return initialized;};
         void create( const Surface& image );
         void read( tnstream& stream );
         void write ( tnstream& stream ) const;
         void blit( Surface& s, const SPoint& pos, int layer = 0 ) const;
         static void fill( Surface& s, const SPoint& pos, SDLmm::Color color );
         static void fill( Surface& s, const SPoint& pos, SDL_Color color );
         static void fillCenter( Surface& s, const SPoint& pos, SDLmm::Color color );
         static void fillCenter( Surface& s, const SPoint& pos, SDL_Color color );
         static void lighten( Surface& s, const SPoint& pos, float value );
};


#endif
