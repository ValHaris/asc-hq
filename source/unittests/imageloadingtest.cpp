/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "imageloadingtest.h"
#include "../fieldimageloader.h"
#include "../graphics/surface2png.h"
#include "../itemrepository.h"
#include "../loaders.h"
#include "unittestutil.h"
#include "../graphics/surface2png.h"


void validateFieldShape( Surface& s ) {
   int transparent_on_body = 0;
   int non_transparent_outside = 0;
   for ( int x = 0; x < fieldsizex; x++)
      for ( int y = 0; y < fieldsizey; y++ ) {
         Color c = s.GetPixel(SPoint(x,y));
         if ( getFieldMask().GetPixel(SPoint(x,y))) {
            if ( !s.isTransparent(c) )
               non_transparent_outside++;
         } else {
            if ( s.isTransparent(c) )
               transparent_on_body++;

         }
      }

   assertOrThrow(transparent_on_body == 0);
   assertOrThrow(non_transparent_outside == 0);

}

void testTerrain() {

   Surface* reference = new Surface ( Surface::CreateSurface(0, 10, 10, 32, 0xff0000, 0xff00, 0xff, 0xff000000) );
   Surface::SetScreen(reference->getBaseSurface());
   TerrainImagePreparator imagePrepper;

   Surface s = loadASCFieldImage("texture-11.png", &imagePrepper);
   validateFieldShape(s);

   Surface s2 = loadASCFieldImage("texture-11.png pfuetze-1a.png", &imagePrepper);
   validateFieldShape(s2);
   writePNG("pfuetze.png", s2);

}

 void testImageLoading()
 {
    testTerrain();
    auto_ptr<GameMap> game ( startMap("unittest-movement.map"));

    VehicleType* wedgeType = vehicleTypeRepository.getObject_byID(14);
    assertOrThrow(wedgeType != NULL);

    Vehicle* wedge0 = new Vehicle(wedgeType, game.get(), 0);
    Vehicle* wedge2 = new Vehicle(wedgeType, game.get(), 2);

    Surface player0 = Surface::createSurface(fieldsizex,fieldsizey, 32 );
    Surface player2 = Surface::createSurface(fieldsizex,fieldsizey, 32 );

    wedge0->paint(player0, SPoint(0,0), false, 0);
    wedge2->paint(player2, SPoint(0,0), false, 0);

    writePNG("redwedge.png", player0);
    writePNG("yellowwedge.png", player2);

    int difference = 0;
    for ( int x = 0; x < fieldsizex; x++)
       for ( int y = 0; y < fieldsizey; y++ )
          if ( player0.GetPixel(SPoint(x,y)) != player2.GetPixel(SPoint(x,y)))
             difference++;

    assertOrThrow(difference > 100);

    int transparent = 0;
    for ( int x = 0; x < fieldsizex; x++)
       for ( int y = 0; y < fieldsizey; y++ )
          if ( getFieldMask().GetPixel(SPoint(x,y))) {
             Color c = wedge0->typ->getImage().GetPixel(SPoint(x,y));
             if ( wedge0->typ->getImage().isTransparent(c) )
                transparent++;
          }

    assertOrThrow(transparent > 100);

    Uint32 marker = 0x123456;
    Surface rotated = Surface::createSurface(fieldsizex,fieldsizey, 32, marker );
    wedge0->direction = 1;
    wedge0->paint(player0, SPoint(0,0), false, 0);

    int found = 0;
    for ( int x = 0; x < 10; x++)
       for ( int y = 0; y < 3; y++ ) {
          Color col = rotated.GetPixel(SPoint(x,y));
          if ( col == marker )
             found++;
       }

    assertOrThrow(found == 30);
 }


