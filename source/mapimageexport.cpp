
/***************************************************************************
                             mapimageexport.cpp  
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


#include "loadpcx.h"
#include "mapdisplay.h"
#include "mapimageexport.h"
#include "dialogs/fileselector.h"



WholeMapRenderer :: WholeMapRenderer ( tmap* actmap ) : gamemap ( actmap )
{
   int bufsizex = actmap->xsize * fielddistx + 200 ;
   int bufsizey = actmap->ysize * fielddisty + 200 ;
   surface = Surface::createSurface( bufsizex, bufsizey, 32 );
   
}


void WholeMapRenderer::render()
{
   paintTerrain( surface, gamemap, gamemap->playerView, ViewPort( 0, 0, gamemap->xsize, gamemap->ysize ), MapCoordinate( 0, 0 ) );
}

void WholeMapRenderer::write( const ASCString& filename )
{
   render();
   writepcx( filename, surface, SDLmm::SRect( SPoint( surfaceBorder, surfaceBorder), (gamemap->xsize-1) * fielddistx + fielddisthalfx + fieldsizex, (gamemap->ysize - 1) * fielddisty + fieldysize ) );
}

void writemaptopcx ( tmap* gamemap )
{
   ASCString name = selectFile( "*.pcx", false );
   
   StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "writing map to " + name );
   
   if ( !name.empty() ) {
      WholeMapRenderer wmr( gamemap );
      wmr.write( name );
   }
}



