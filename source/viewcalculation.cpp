/***************************************************************************
                          viewcalculation.cpp  -  description
                             -------------------
    begin                : Thu Oct 5 2000
    copyright            : (C) 2000 by Martin Bickel
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

#include "viewcalculation.h"
#include "mapalgorithms.h"

#include "vehicletype.h"
#include "buildingtype.h"

extern void fatalError ( const char* formatstring, ... );


void         tcomputeview::initviewcalculation(  int view, int jamming, int sx, int sy, int _mode, int _height  )  // mode: +1 = add view  ;  -1 = remove view
{
   height = _height;
   viewdist = view;
   jamdist  = jamming;
   mode = _mode;

   int md;
   if ( viewdist > jamdist )
      md = viewdist / minmalq + 1;
   else
      md = jamdist / minmalq + 1;

   initsearch( sx, sy, md, 1 );
}



void         tcomputeview::testfield(void)
{

   if ((xp >= 0) && (yp >= 0) && (xp < gamemap->xsize) && (yp < gamemap->ysize)) {

      int f = beeline(startx,starty,xp,yp);
      pfield efield = gamemap->getField(xp,yp);

      if ( viewdist && ( f <= 15 ))
         efield->view[player].direct += mode;

      int str = viewdist;
      if ( f ) {
         /*
         if ( CGameOptions::Instance()->visibility_calc_algo == 1 ) {
            int x = startx ;
            int y = starty ;
            while ( x != xp || y != yp ) {
               int d = getdirection ( x, y, xp, yp );
               getnextfield ( x, y, d );
               str -= getfield(x,y)->typ->basicjamming + actmap->weather.fog ;
               if ( d & 1 )
                  str-=8;
               else
                  str-=12;
            };
         } else {
         */
            int freefields = 0;
            if ( height > chhochfliegend )
               freefields = 5;
            else
            if ( height == chhochfliegend )
               freefields = 3;
            else
            if ( height == chfliegend )
               freefields = 2;
            else
            if ( height == chtieffliegend )
               freefields = 1;
           tdrawgettempline lne ( freefields, gamemap );

           if ( startx == -1 || starty == -1 )
              fatalError("error in tcomputeview::testfield" );

           lne.start ( startx, starty, xp, yp );
           str -= f;
           str -= lne.tempsum;
        // }

      }

      if ( str > 0 ) {
         efield->view[player].view += str * mode;

         if ( sonar )
            efield->view[player].sonar += mode;

         if ( satellitenview )
            efield->view[player].satellite += mode;

         if ( minenview )
            efield->view[player].mine += mode;
      }

      if ( jamdist >= f )
         efield->view[player].jamming += (jamdist - f) * mode;

      #ifdef DEBUGVIEW
        if ( efield->view[player].view      < 0 ||
             efield->view[player].sonar     < 0 ||
             efield->view[player].satellite < 0 ||
             efield->view[player].jamming   < 0 ||
             efield->view[player].mine      < 0 )
           displaymessage ( "Warning: inconsistency in view calculation !!\n Please report this bug !", 1 );
      #endif
   }
}



void         tcomputevehicleview::init( const pvehicle eht, int _mode  )   // mode: +1 = add view  ;  -1 = remove view ); )
{
   player = eht->color / 8;

   if ( eht->height == chsatellit )
      satellitenview = 1;
   else
      satellitenview = !!(eht->functions & cfsatellitenview);

   sonar =           !!(eht->functions & cfsonar);
   minenview =      !!(eht->functions & cfmineview);

   if ( (eht->functions & cfautodigger) && mode == 1 )
      eht->searchForMineralResources();

   xp = eht->xpos;
   yp = eht->ypos;
   tcomputeview::initviewcalculation( eht->typ->view+1, eht->typ->jamming, eht->xpos, eht->ypos, _mode, eht->height );
   testfield();

}



void         tcomputebuildingview::init( const pbuilding    bld,  int _mode )
{
   player = bld->color / 8;

   int  c, j ;

   if (bld->completion == bld->typ->construction_steps - 1) {
      c = bld->typ->view;
      j = bld->typ->jamming;
   } else {
      c = 15;
      j = 0;
   }

   initviewcalculation( c, j, bld->xpos, bld->ypos, _mode, bld->typ->buildingheight );
   sonar = !!(bld->typ->special & cgsonarb);

   minenview = true;
   satellitenview = true;

   building = bld;

   orgx = bld->xpos - building->typ->entry.x;
   orgy = bld->ypos - building->typ->entry.y;

   dx = orgy & 1;

   orgx = orgx + (dx & (~bld->typ->entry.y));

   for ( int a = orgx; a <= orgx + 3; a++)
      for (int b = orgy; b <= orgy + 5; b++)
         if ( building->getpicture ( a - orgx, b - orgy )) {
            int xp = a - (dx & b);
            int yp = b;
            pfield efield = gamemap->getField ( xp, yp );
            if ( minenview )
               efield->view[player].mine += _mode;
            efield->view[player].direct += _mode;
         }
}

