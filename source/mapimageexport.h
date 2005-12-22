
/***************************************************************************
                             mapimageexport.h
                             -------------------
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

#ifndef mapimageexportH
 #define mapimageexportH

#include "gamemap.h"
#include "graphics/surface.h"

#include "mapdisplay.h"

class WholeMapRenderer : public MapRenderer {
   tmap* gamemap;
   Surface surface;
   void render();
   public:
      WholeMapRenderer( tmap* actmap );
      void write( const ASCString& filename );

};

extern void writemaptopcx ( tmap* gamemap );

#endif
