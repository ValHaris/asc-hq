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


#ifndef viewcalculation_h_included
 #define viewcalculation_h_included

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

                      int sonar,satellitenview,minenview;
                      int        viewdist;
                      int        jamdist;
                      virtual void  initviewcalculation( int view, int jamming, int sx, int sy, int _mode, int _height   );   // mode: +1 = add view  ;  -1 = remove view
                      virtual void  testfield ( const MapCoordinate& mc );

                public:
                      tcomputeview ( pmap _actmap ) : SearchFields ( _actmap ), rangeJamming ( true ) { actView = _actmap->playerView; };
                 };

  class tcomputevehicleview : public tcomputeview {
                           public:
                               tcomputevehicleview ( pmap actmap ) : tcomputeview ( actmap ) {};
                               void          init( const pvehicle eht, int _mode  );   // mode: +1 = add view  ;  -1 = remove view );
                           };

  class tcomputebuildingview : public tcomputeview  {
                              pbuilding         building;
                           public:
                              tcomputebuildingview ( pmap actmap ) : tcomputeview ( actmap ) {};
                              void              init( const pbuilding    bld, int _mode );
                           };

  /** completely computes the view
      \param actmap  the map that the view is generated on
      \param player_fieldcount_mask bitmapped variable containing the players for whom the changed fields are calculated
      \returns the number of fields that have a changed visibility for the given players. If nothing changes, the map must not be displayed again after the view calculation
  */
  extern int computeview( pmap actmap, int player_fieldcount_mask = 0 );

  /** evaluates the view on a given field.
      The view is NOT calculated. This must be done prior to calling this function !
      \param actmap the map that contains field
      \param fld    the field to evaluate
      \param player the player that the view is calculated for
      \param add    a bitmapped variable containing the players that share their view with player
      \param initial the initial visibility of the map when starting the game.
  */
  extern int evaluatevisibilityfield ( pmap actmap, pfield fld, int player, int add, int initial );

  /** evaluates the view on the whole map.
      The view is NOT calculated. This must be done prior to calling this function !
      \param actmap the map that the view is calculated of
      \param player_fieldcount_mask determines, which players should be counted when the view has changed
      \returns the number of fields which have a changed visibility status
  */
  extern int  evaluateviewcalculation ( pmap actmap, int player_fieldcount_mask = 0 );

  /** evaluates the view on a part of the map.
      The view is NOT calculated. This must be done prior to calling this function !
      \param actmap the map that the view is calculated of
      \param pos  the central position around which the view is calculated
      \param distance the radius of the circle around pos in which the view is evaluated. The view is calculated in AT LEAST this circle, in reality it is a rectangle containing this circle.
      \param player_fieldcount_mask determines, which players should be counted when the view has changed
      \returns the number of fields which have a changed visibility status
  */
  extern int  evaluateviewcalculation ( pmap actmap, const MapCoordinate& pos, int distance, int player_fieldcount_mask = 0 );


#endif