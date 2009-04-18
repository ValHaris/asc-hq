/*! \file mapdisplay.cpp
    \brief everything for displaying the map on the screen
*/


/***************************************************************************
                          mapdisplay.cpp  -  description
                             -------------------
    begin                : Wed Jan 24 2001
    copyright            : (C) 2001 by Martin Bickel
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

// #define debugmapdisplay


#include <cmath>
#include <limits>

#include "pgeventsupplier.h"

#include "global.h"
#include "typen.h"
#include "mapdisplay.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "spfst.h"
#include "dialog.h"
#include "mainscreenwidget.h"
#include "sdl/sound.h"
#include "dialogs/attackpanel.h"
#include "spfst-legacy.h"

extern bool tempsvisible;
extern MapDisplayPG* theGlobalMapDisplay;


#ifdef debugmapdisplay
#include <iostream>
#endif


class PG_MapDisplay : public MapDisplayInterface {
           MapDisplayPG* mapDisplayWidget;
         public:
           PG_MapDisplay( MapDisplayPG* mapDisplayWidget_ ) : mapDisplayWidget( mapDisplayWidget_ ) {};

           int displayMovingUnit ( const MapCoordinate3D& start, const MapCoordinate3D& dest, Vehicle* vehicle, int fieldnum, int totalmove, SoundStartCallback soundStart );
           void deleteVehicle ( Vehicle* vehicle ) {};
           void displayMap ( void );
           void displayMap ( Vehicle* vehicle );
           void displayPosition ( int x, int y );
           void resetMovement ( void ) {};
           void startAction ( void );
           void stopAction ( void );
           void cursor_goto ( const MapCoordinate& pos );
           void displayActionCursor ( int x1, int y1, int x2, int y2 ) {};
           void removeActionCursor ( void ) {
              mapDisplayWidget->cursor.invisible = 1;
           };
           void updateDashboard ();
           void repaintDisplay ();
           void setTempView( bool view ) { tempsvisible = view; };
           void showBattle( tfight& battle );
           void playPositionalSound( const MapCoordinate& pos, Sound* snd );
};



int  PG_MapDisplay :: displayMovingUnit ( const MapCoordinate3D& start, const MapCoordinate3D& dest, Vehicle* vehicle, int fieldnum, int totalmove, SoundStartCallback soundStart )
{
   if ( actmap->getPlayerView() == -2 )
      return 0;

   MapCoordinate3D newstart = start;
   MapCoordinate3D newdest = dest;
   
   if ( newdest.getNumericalHeight() == -1 )
      newdest.setNumericalHeight( newstart.getNumericalHeight() );
   else
      if ( newstart.getNumericalHeight() == -1 )
         newstart.setNumericalHeight( newdest.getNumericalHeight() );

   if ( newstart.getNumericalHeight() < newdest.getNumericalHeight() ) {
      if ( fieldnum+1 < totalmove )
         newdest.setNumericalHeight( newstart.getNumericalHeight() );
   }
   
   if ( newstart.getNumericalHeight() > newdest.getNumericalHeight() ) {
      if ( fieldnum > 0  )
         newstart.setNumericalHeight( newdest.getNumericalHeight() );
   }
   
   tfield* fld1 = actmap->getField ( start );
   int view1 = fieldVisibility ( fld1, actmap->getPlayerView() );

   tfield* fld2 = actmap->getField ( dest );
   int view2 = fieldVisibility ( fld2, actmap->getPlayerView() );

   bool shareView;
   if ( actmap->getPlayerView() >= 0)
      shareView = actmap->player[actmap->getPlayerView()].diplomacy.sharesView( vehicle->getOwner() );
   else
      shareView = false;

   if (  (view1 >= visible_now  ||  view2 >= visible_now ) || ( vehicle->getOwner() == actmap->getPlayerView() ) || shareView || actmap->getPlayerView() == -1 )
      if ( ((vehicle->height >= chschwimmend) && (vehicle->height <= chhochfliegend)) || (( view1 == visible_all) || ( view2 == visible_all )) || ( actmap->actplayer == actmap->getPlayerView() ) || shareView || actmap->getPlayerView() == -1 ) {
         soundStart( 1 );
         mapDisplayWidget->displayUnitMovement( actmap, vehicle, newstart, newdest );
      }


   int result = 0;
   if (view1 >= visible_now )
      result +=1;

   if ( view2 >= visible_now )
      result +=2;

   return result;
}

void PG_MapDisplay :: showBattle( tfight& battle )
{
   showAttackAnimation( battle, actmap );
}


void PG_MapDisplay :: displayMap ( void )
{
   ::repaintMap();
}

void PG_MapDisplay :: cursor_goto ( const MapCoordinate& pos )
{
   mapDisplayWidget->cursor.goTo( pos );
}


void PG_MapDisplay :: displayMap ( Vehicle* vehicle )
{
   mapDisplayWidget->registerAdditionalUnit( vehicle ); 
   ::repaintMap();
   mapDisplayWidget->registerAdditionalUnit( NULL ); 
}


void PG_MapDisplay :: displayPosition ( int x, int y )
{
   if ( !mapDisplayWidget->fieldInView( MapCoordinate(x,y) ))
      mapDisplayWidget->centerOnField( MapCoordinate(x,y) );
   else
      mapDisplayWidget->Update();
}


void PG_MapDisplay :: startAction ( void )
{
}

void PG_MapDisplay :: stopAction ( void )
{
}

void PG_MapDisplay :: updateDashboard ( void )
{
   updateFieldInfo();
}

void PG_MapDisplay :: repaintDisplay ()
{
   ::repaintDisplay();
}

void PG_MapDisplay :: playPositionalSound( const MapCoordinate& pos, Sound* snd )
{
   snd->play();
}
    

MapDisplayInterface& getDefaultMapDisplay()
{
   static PG_MapDisplay* mapDisplay = NULL;
   if ( !mapDisplay ) {
      assert( theGlobalMapDisplay );
      mapDisplay = new PG_MapDisplay( theGlobalMapDisplay );
   }   
   return *mapDisplay;
}

