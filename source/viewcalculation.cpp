/*! \file viewcalculation.cpp
    \brief functions for calculating the view of units and buildings
*/

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

#include "global.h" 
#include "viewcalculation.h"
#include "mapalgorithms.h"

#include "vehicletype.h"
#include "buildingtype.h"
#include "errors.h"

#ifdef sgmain
// #include "gameevents.h"
#endif

SigC::Signal0<void> buildingSeen;


void         tcomputeview::initviewcalculation(  int view, int jamming, int sx, int sy, int _mode, int _height  )  // mode: +1 = add view  ;  -1 = remove view
{
   height = _height;
   if ( view > 255 )
      viewdist = 255;
   else
      viewdist = view;
   jamdist  = jamming;
   mode = _mode;

   int md;
   if ( viewdist > jamdist )
      md = viewdist / minmalq + 1;
   else
      md = jamdist / minmalq + 1;

   initsearch( MapCoordinate(sx, sy), md, 1 );
}



void         tcomputeview::testfield( const MapCoordinate& mc )
{
   int f = beeline(startPos, mc);
   pfield efield = gamemap->getField(mc);

   if ( viewdist && ( f <= 15 ) && (gamemap->getgameparameter( cgp_disableDirectView) == 0  || f < 10 ) )
      efield->view[player].direct += mode;

   int str = viewdist;
   if ( f ) {
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

     if ( startPos.x == -1 || startPos.y == -1 )
        fatalError("error in tcomputeview::testfield" );

     lne.start ( startPos.x, startPos.y, mc.x, mc.y );
     str -= f;
     str -= lne.tempsum;
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

   if ( rangeJamming || !f ) {
      int jamloss = f * gamemap->getgameparameter ( cgp_jammingSlope ) / 10;
      int jamstrength = jamdist * gamemap->getgameparameter ( cgp_jammingAmplifier ) / 100;
      if ( jamstrength >= jamloss )
         efield->view[player].jamming += (jamstrength - jamloss) * mode;
   }

   #ifdef DEBUGVIEW
     if ( efield->view[player].view      < 0 ||
          efield->view[player].sonar     < 0 ||
          efield->view[player].satellite < 0 ||
          efield->view[player].jamming   < 0 ||
          efield->view[player].mine      < 0 )
        displaymessage ( "Warning: inconsistency in view calculation !!\n Please report this bug !", 1 );
   #endif
}



void         tcomputevehicleview::init( const pvehicle eht, int _mode  )   // mode: +1 = add view  ;  -1 = remove view ); )
{
   player = eht->color / 8;

   if ( player >= 8 )
      fatalError ( "ComputeVehicleView::init - invalid player ");

   if ( eht->height == chsatellit )
      satellitenview = 1;
   else
      satellitenview = !!(eht->typ->functions & cfsatellitenview);

   sonar =           !!(eht->typ->functions & cfsonar);
   minenview =      !!(eht->typ->functions & cfmineview);
   if ( eht->typ->functions & cfownFieldJamming )
      rangeJamming = false;

   if ( (eht->typ->functions & cfautodigger) && _mode == 1 )
      eht->searchForMineralResources();

   tcomputeview::initviewcalculation( eht->typ->view+1, eht->typ->jamming, eht->xpos, eht->ypos, _mode, eht->height );
   testfield( eht->getPosition() );

}



void         tcomputebuildingview::init( const pbuilding    bld,  int _mode )
{
   player = bld->color / 8;

   if ( player >= 8 )
      fatalError ( "ComputeBuildingView::init - invalid player ");

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
   if (!actmap || (actmap->xsize <= 0) || (actmap->ysize <= 0))
     return;

   for ( int p = 0; p < 8; p++ )
      for ( tmap::Player::VehicleList::iterator i = actmap->player[p].vehicleList.begin(); i != actmap->player[p].vehicleList.end(); i++ )
         if ( (*i)->isViewing())
            (*i)->removeview();

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
      fld->setVisibility(visible_all, player);
      return 0;
   } else {
      originalVisibility = (fld->visible >> (player * 2)) & 3;
      if ( originalVisibility >= visible_ago || initial == 1)
           fld->setVisibility(visible_ago, player);
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
         buildingSeen();
      #endif

      if (( fld->vehicle  && ( fld->vehicle->color  == player * 8 )) ||
          ( fld->vehicle  && ( fld->vehicle->height  < chschwimmend ) && sonar ) ||
          ( fld->building && ( fld->building->typ->buildingheight < chschwimmend ) && sonar ) ||
          ( !fld->mines.empty() && ( mine  ||  fld->mineowner() == player)) ||
          ( fld->vehicle  && ( fld->vehicle->height  >= chsatellit )  && satellite )) {
             fld->setVisibility( visible_all, player);
             return originalVisibility != visible_all;
      } else {
             fld->setVisibility( visible_now, player);
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

int  evaluateviewcalculation ( pmap actmap, const MapCoordinate& pos, int distance, int player_fieldcount_mask )
{
   distance = (distance+maxmalq-1)/maxmalq;
   int x1 = pos.x - distance;
   if ( x1 < 0 )
      x1 = 0;

   int y1 = pos.y - distance*2;
   if ( y1 < 0 )
      y1 = 0;

   int x2 = pos.x + distance;
   if ( x2 >= actmap->xsize )
      x2 = actmap->xsize-1;

   int y2 = pos.y + distance*2;
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



