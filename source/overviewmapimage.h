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

//! the image for a terraintype ( #tterraintype ) that is shown on the small map
class OverviewMapImage {
         bool initialized;
      public:
         static const int width = 4;
         static const int height = 4;
         SDLmm::ColorRGBA segment[width][height];
         OverviewMapImage();
         OverviewMapImage( const Surface& image );
         void create( const Surface& image );
         void read( tnstream& stream );
         void write ( tnstream& stream ) const;
         void blit( Surface& s, int x, int y, int layer = 0 ) const;
};


#endif
