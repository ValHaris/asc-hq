/***************************************************************************
                          viewcalculation.h  -  description
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


#ifndef viewcalculationH
 #define viewcalculationH

 #include "actions/context.h"
 #include "typen.h"
 #include "mapalgorithms.h"
 #include "gamemap.h"

  class tcomputeview : public SearchFields {
                protected:
                      int actView;
                      int player;
                      int mode;
                      int height;
                      bool rangeJamming;
                      int baseJammingMultiplier; // in 1/100

                      int sonar,satellitenview,minenview;
                      int        viewdist;
                      int        jamdist;
                      virtual void  initviewcalculation( int view, int jamming, int sx, int sy, int _mode, int _height   );   // mode: +1 = add view  ;  -1 = remove view
                      virtual void  testfield ( const MapCoordinate& mc );

                public:
                      tcomputeview ( GameMap* _actmap ) : SearchFields ( _actmap ), rangeJamming ( true ), baseJammingMultiplier(100) { actView = _actmap->getPlayerView(); };
                 };

  class tcomputevehicleview : public tcomputeview {
                           public:
                               tcomputevehicleview ( GameMap* gamemap ) : tcomputeview ( gamemap ) {};
                               void          init( const Vehicle* eht, int _mode  );   // mode: +1 = add view  ;  -1 = remove view );
                           };

  class tcomputebuildingview : public tcomputeview  {
                              const Building*         building;
                           public:
                              tcomputebuildingview ( GameMap* gamemap ) : tcomputeview ( gamemap ) {};
                              void              init( const Building*    bld, int _mode );
                           };

  /** completely computes the view
      \param gamemap  the map that the view is generated on
      \param player_fieldcount_mask bitmapped variable containing the players for whom the changed fields are calculated
      \param disableShareView sharing the view between different players is disabled. 
      \returns the number of fields that have a changed visibility for the given players. If nothing changes, the map must not be displayed again after the view calculation
  */
  extern int computeview( GameMap* gamemap, int player_fieldcount_mask = 0, bool disableShareView = false, const Context* context = NULL );

  /** evaluates the view on a given field and saves it for that field. Calls #calcvisibilityfield for the calculation
      Radar and jamming values must have already been applied to the field! 
      \param gamemap the map that contains field
      \param fld    the field to evaluate
      \param player the player that the view is calculated for
      \param add    a bitmapped variable containing the players that share their view with player
      \param initial the initial visibility of the map when starting the game.
  */
  extern int evaluatevisibilityfield ( GameMap* gamemap, tfield* fld, int player, int add, int initial );

  /** evaluates the view on the whole map.
      Radar and jamming values must have already been applied to the field! 
      \param gamemap the map that the view is calculated of
      \param player_fieldcount_mask determines, which players should be counted when the view has changed
      \param disableShareView sharing the view between different players is disabled. 
      \returns the number of fields which have a changed visibility status
  */
  extern int  evaluateviewcalculation ( GameMap* gamemap, int player_fieldcount_mask = 0, bool disableShareView = false, const Context* context = NULL );

  /** evaluates the view on a part of the map.
      Radar and jamming values must have already been applied to the field! 
      \param gamemap the map that the view is calculated of
      \param pos  the central position around which the view is calculated
      \param distance the radius of the circle around pos in which the view is evaluated. The view is calculated in AT LEAST this circle, in reality it is a rectangle containing this circle.
      \param player_fieldcount_mask determines, which players should be counted when the view has changed
      \param disableShareView sharing the view between different players is disabled. 
      \returns the number of fields which have a changed visibility status
  */
  extern int  evaluateviewcalculation ( GameMap* gamemap, const MapCoordinate& pos, int distance, int player_fieldcount_mask = 0, bool disableShareView = false, const Context* context = NULL );

  /** calculates the view on a given field.
      Radar and jamming values must have already been applied to the field! 
      \param gamemap the map that contains field
      \param fld    the field to evaluate
      \param player the player that the view is calculated for
      \param add    a bitmapped variable containing the players that share their view with player
      \param initial the initial visibility of the map when starting the game.
      \param additionalEnemyJamming if > 0 run an WhatIf analysis and don't save the result
   */
  extern VisibilityStates calcvisibilityfield ( GameMap* gamemap, tfield* fld, int player, int add, int initial, int additionalEnemyJamming );

  
  // extern VisibilityStates fieldVisibility  ( tfield* pe, int player, GameMap* gamemap, int additionalEnemyJamming );
  extern int getPlayersWithSharedViewMask( int player, GameMap* gamemap );
  
  extern SigC::Signal0<void> buildingSeen;

  class RecalculateAreaView {
     MapCoordinate position;
     int range;
     GameMap* gamemap; 
     bool active;

     void removeFieldView( const MapCoordinate& pos );
     void addFieldView( const MapCoordinate& pos );
   public:
      RecalculateAreaView( GameMap* gamemap, const MapCoordinate& pos, int range );
      void removeView();
      void addView();
      ~RecalculateAreaView();
  };

#endif
