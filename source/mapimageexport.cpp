
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
#include "graphics/surface2png.h"
#include "spfst.h"
#include "iconrepository.h"

WholeMapRenderer :: WholeMapRenderer ( GameMap* actmap ) : gamemap ( actmap )
{
   int bufsizex = actmap->xsize * fielddistx + 200 ;
   int bufsizey = actmap->ysize * fielddisty + 200 ;
   surface = Surface::createSurface( bufsizex, bufsizey, 32, Surface::transparent << 24 );
   
}


void WholeMapRenderer::render()
{
   paintTerrain( surface, gamemap, gamemap->getPlayerView(), ViewPort( 0, 0, gamemap->xsize, gamemap->ysize ), MapCoordinate( 0, 0 ) );
   renderVisibility();
}

void WholeMapRenderer::renderVisibility()
{
   ColorMerger_AlphaMerge<4> cmam;

   PutPixel<4, ColorMerger_AlphaMerge > pp(surface);

   Surface& mask = IconRepository::getIcon("largehex.pcx");
   for ( int y = 0; y < gamemap->ysize; ++y )
      for ( int x = 0; x < gamemap->xsize; ++x ) {
         int view = -1;
         int maxview = 0;
         for ( int i = 1; i < gamemap->getPlayerCount(); ++i )
            if ( fieldvisiblenow( gamemap->getField(x,y), i, gamemap )) {
               if ( gamemap->getField(x,y)->view[i].view > maxview ) {
                  maxview = gamemap->getField(x,y)->view[i].view;
                  view = i;
               }
            }


         if ( view >= 0 )
            for ( int yp = 0; yp < fieldsizey; ++yp)
               for ( int xp = 0; xp < fieldsizex; ++xp)
                  if ( mask.GetPixel(xp,yp) != 0xff ) 
                     pp.set( getFieldPos(x,y) + SPoint(xp,yp), gamemap->getPlayer( view ).getColor().MapRGBA( surface.getBaseSurface()->format, min(maxview,160)*2/3));
      }
}


void WholeMapRenderer::writePCX( const ASCString& filename )
{
   writepcx( filename, surface, SDLmm::SRect( SPoint( surfaceBorder, surfaceBorder), (gamemap->xsize-1) * fielddistx + fielddisthalfx + fieldsizex, (gamemap->ysize - 1) * fielddisty + fieldysize ) );
}

void WholeMapRenderer::writePNG( const ASCString& filename )
{
   ::writePNG( constructFileName(0,"",filename), surface, SDLmm::SRect( SPoint( surfaceBorder, surfaceBorder), (gamemap->xsize-1) * fielddistx + fielddisthalfx + fieldsizex, (gamemap->ysize - 1) * fielddisty + fieldysize ) );
}

void writemaptopcx ( GameMap* gamemap, bool addview )
{
   ASCString name = selectFile( "*.png", false );
   
   StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "writing map to " + name );
   
   if ( !name.empty() ) {
      WholeMapRenderer wmr( gamemap );
      wmr.render();
      if ( addview ) 
         wmr.renderVisibility();
      
      wmr.writePNG( name );
   }
}



