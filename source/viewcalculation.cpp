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
#include "errors.h"

#ifdef sgmain
 #include "missions.h"
#endif


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

   if (bld->getCompletion() == bld->typ->construction_steps - 1) {
      c = bld->typ->view;
      j = bld->typ->jamming;
   } else {
      c = 15;
      j = 0;
   }

   initviewcalculation( c, j, bld->getEntry().x, bld->getEntry().y, _mode, bld->typ->buildingheight );
   sonar = !!(bld->typ->special & cgsonarb);

   minenview = true;
   satellitenview = true;

   building = bld;

   for ( int a = 0; a < 4; a++)
      for (int b = 0; b < 6; b++)
         if ( building->getpicture ( BuildingType::LocalCoordinate( a, b ) )) {
            pfield efield = building->getField ( BuildingType::LocalCoordinate( a, b ) );
            if ( minenview )
               efield->view[player].mine += _mode;
            efield->view[player].direct += _mode;
         }
}





void         clearvisibility( pmap actmap, int  reset )
{
  if ((actmap->xsize == 0) || (actmap->ysize == 0))
     return;

   int l = 0;
   for ( int x = 0; x < actmap->xsize ; x++)
         for ( int y = 0; y < actmap->ysize ; y++) {
            pfield fld = &actmap->field[l];
            memset ( fld->view, 0, sizeof ( fld->view ));
            l++;
         }
}

int  evaluatevisibilityfield ( pmap actmap, pfield fld, int player, int add, int initial )
{
   int originalVisibility;
   if ( initial == 2 ) {
      setvisibility(&fld->visible,visible_all, player);
      return 0;
   } else {
      originalVisibility = (fld->visible >> (player * 2)) & 3;
      if ( originalVisibility >= visible_ago || initial == 1)
           setvisibility(&fld->visible,visible_ago, player);
   }

   if ( add == -1 ) {
      add = 0;
      if ( actmap->shareview )
         for ( int i = 0; i < 8; i++ )
            if ( actmap->shareview->mode[i][player] )
               add |= 1 << i;
   }
   add |= 1 << player;

   int sight = 0;
   int jamming = 0;
   int mine = 0;
   int satellite = 0;
   int direct = 0;
   int sonar = 0;
   for ( int i = 0; i < 8; i++ ){
      if ( add & ( 1 << i )) {
         sight += fld->view[i].view;
         mine  += fld->view[i].mine;
         satellite += fld->view[i].satellite;
         direct += fld->view[i].direct;
         sonar += fld->view[i].sonar;
      } else
         jamming += fld->view[i].jamming;
   }
   if ( sight > jamming   ||  direct  ) {
      #ifdef sgmain
      if ( fld->building && (fld->building->connection & cconnection_seen))
         releaseevent ( NULL, fld->building, cconnection_seen );
      #endif

      if (( fld->vehicle  && ( fld->vehicle->color  == player * 8 )) ||
          ( fld->vehicle  && ( fld->vehicle->height  < chschwimmend ) && sonar ) ||
          ( fld->building && ( fld->building->typ->buildingheight < chschwimmend ) && sonar ) ||
          ( fld->object && fld->object->mine && ( mine  ||  fld->mineowner() == player)) ||
          ( fld->vehicle  && ( fld->vehicle->height  >= chsatellit )  && satellite )) {
             setvisibility(&fld->visible,visible_all, player);
             return originalVisibility != visible_all;
      } else {
             setvisibility(&fld->visible,visible_now, player);
             return originalVisibility != visible_now;
      }
   }
   return 0;
}


int  evaluateviewcalculation ( pmap actmap, int player_fieldcount_mask )
{
   int initial = actmap->getgameparameter ( cgp_initialMapVisibility );
   int fieldsChanged = 0;
   for ( int player = 0; player < 8; player++ )
      if ( actmap->player[player].exist() ) {
         int add = 0;
         if ( actmap->shareview )
            for ( int i = 0; i < 8; i++ )
               if ( actmap->shareview->mode[i][player] )
                  add |= 1 << i;

         int nm = actmap->xsize * actmap->ysize;
         if ( player_fieldcount_mask & (1 << player ))
            for ( int i = 0; i < nm; i++ )
                fieldsChanged += evaluatevisibilityfield ( actmap, &actmap->field[i], player, add, initial );
         else
            for ( int i = 0; i < nm; i++ )
                evaluatevisibilityfield ( actmap, &actmap->field[i], player, add, initial );
      }
   return fieldsChanged;
}

int  evaluateviewcalculation ( pmap actmap, int x, int y, int distance, int player_fieldcount_mask )
{
   distance = (distance+maxmalq-1)/maxmalq;
   int x1 = x - distance;
   if ( x1 < 0 )
      x1 = 0;

   int y1 = y - distance*2;
   if ( y1 < 0 )
      y1 = 0;

   int x2 = x + distance;
   if ( x2 >= actmap->xsize )
      x2 = actmap->xsize-1;

   int y2 = y + distance*2;
   if ( y2 >= actmap->ysize )
      y2 = actmap->ysize-1;

   int initial = actmap->getgameparameter ( cgp_initialMapVisibility );
   int fieldsChanged = 0;
   for ( int player = 0; player < 8; player++ )
      if ( actmap->player[player].exist() ) {
         int add = 0;
         if ( actmap->shareview )
            for ( int i = 0; i < 8; i++ )
               if ( actmap->shareview->mode[i][player] )
                  add |= 1 << i;

         for ( int yy = y1; yy <= y2; yy++ )
            for ( int xx = x1; xx <= x2; xx++ ) {
               pfield fld = actmap->getField ( xx, yy );
               if ( player_fieldcount_mask & (1 << player ))
                  fieldsChanged += evaluatevisibilityfield ( actmap, fld, player, add, initial );
               else
                  evaluatevisibilityfield ( actmap, fld, player, add, initial );
            }
      }
   return fieldsChanged;
}



int computeview( pmap actmap, int player_fieldcount_mask )
{
   if ((actmap->xsize == 0) || (actmap->ysize == 0))
      return 0;

   clearvisibility( actmap, 1 );

   for ( int a = 0; a < 8; a++)
      if (actmap->player[a].exist() ) {

         for ( tmap::Player::VehicleList::iterator i = actmap->player[a].vehicleList.begin(); i != actmap->player[a].vehicleList.end(); i++ ) {
            pvehicle actvehicle = *i;
            if ( actvehicle == actmap->getField(actvehicle->xpos,actvehicle->ypos)->vehicle)
               actvehicle->addview();
         }

         for ( tmap::Player::BuildingList::iterator i = actmap->player[a].buildingList.begin(); i != actmap->player[a].buildingList.end(); i++ )
            (*i)->addview();
      }


   return evaluateviewcalculation ( actmap, player_fieldcount_mask );
}


void setvisibility ( word* visi, int valtoset, int actplayer )
{
   int newval = (valtoset ^ 3) << ( 2 * actplayer );
   int oneval = 3 << ( 2 * actplayer );

   int vis = *visi;
   vis |= oneval;
   vis ^= newval;
   *visi = vis;
}
