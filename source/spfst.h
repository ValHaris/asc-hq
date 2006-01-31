/*! \file spfst.h
    \brief map accessing and usage routines used by ASC and the mapeditor

    spfst comes from german "Spielfeldsteuerung" :-)
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING. If not, write to the 
    Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
    Boston, MA  02111-1307  USA
*/

#ifndef spfstH
  #define spfstH

 #include <sigc++/sigc++.h>
  
  
  #include "newfont.h"
  #include "basegfx.h"
  #include "typen.h"
  #include "events.h"
  #include "gamemap.h"

   struct Schriften {
               pfont        smallarial;
               pfont        smallsystem;
               pfont        large;
               pfont        arial8; 
               pfont        guifont;
               pfont        guicolfont;
               pfont        monogui;
            } ; 
   extern Schriften schriften;


  //! this is the one and only map that is loaded by ASC
  extern tmap* actmap; 


//! returns the field at the given coordinates
extern pfield getfield(int x, int y);


// ! returns the diplomatic status between actmap->actplayer and the player with color b (note that the color is playernum*8 ) 
//extern int getdiplomaticstatus( int b );

// ! returns the diplomatic status between the players with color c and b (note that the color is playernum*8 ) 
// extern int getdiplomaticstatus2( int c, int b);

extern void  putbuilding( const MapCoordinate& entryPosition,
                         int          color,
                         const BuildingType* buildingtyp,
                         int          completion,
                         int          ignoreunits = 0 ); // f?r Kartened

extern void  putbuilding2( const MapCoordinate& entryPosition,
                          int color,
                          BuildingType* buildingtyp);  // f?r Spiel


//! recalculates the connection (like road interconnections) of all objects on the map
extern void  calculateallobjects( pmap m = actmap );

/** recalculates the connection (like road interconnections) of an object
      \param x The x coordinate of the field
      \param y The y coordinate of the field
      \param mof Should the neighbouring fields be modified if necessary
      \param obj The objecttype that is to be aligned on this field
*/
extern void  calculateobject(int  x,
                             int  y,
                             bool mof,
                             const ObjectType* obj,
                             pmap gamemap = actmap );

//! generate a map of size xsize/ysize and consisting just of fields bt. The map is stored in #actmap
extern void  generatemap( TerrainType::Weather* bt,
                          int          xsize,
                          int          ysize);


/*! tests if the vehicle can move onto the field

   \param uheight the level of height for which the check should be done. Use -2 to use the current height of the unit
   \retval 0 unit cannot move there
   \retval 1 unit can pass over the field
   \retval 2 unit can stop its movement there
*/
extern int fieldAccessible( const pfield        field,
                            const Vehicle*     vehicle,
                            int  uheight = -2,
                            const bool* attacked = NULL,
                            bool ignoreVisibility = false );


/** removes all units that cannot exist any more, either due to invalid terrin
    (like tanks on melting ice) or too much wind (eg. hoovercrafts in a storm) */
extern void  checkunitsforremoval ( void );

//! removes all objects that cannot exist where they currently are (terrainaccess)
extern void checkobjectsforremoval ( void );

//! returns the maximum wind speed that the unit can endure
extern int          getmaxwindspeedforunit ( const Vehicle* eht );

/** Wind may be different at different heights. This function returns the index 
    for the wind array. If uheight != -1 it is assumed the unit was uheight instead
    the actual level of height */
extern int          getwindheightforunit   ( const Vehicle* eht, int uheight = -1 );


/** Checks if the unit can drive on the field
    \param uheight if != -1, the unit is assumed to be on this height instead of the actual one.
    \returns 0=unit cannot access this field; 
             1=unit can move across this field but cannot keep standing there
             2=unit can move and stand there
*/
extern int          terrainaccessible (  const pfield field, const Vehicle* vehicle, int uheight = -1 );

/** Checks if the unit can drive on the field
    \param uheight if != -1, the unit is assumed to be on this height instead of the actual one.
    \returns 0=unit cannot access this field;
             1=unit can move across this field but cannot keep standing there;
             2=unit can move and stand there;
             < 0 unit cannot access this field, because of:
                  -1   very deep water required to submerge deep
                  -2   deep water required to submerge
                  -3   unit cannot drive onto terrain
*/
extern int          terrainaccessible2 ( const pfield        field, const Vehicle* vehicle, int uheight = -1 );


/** Checks if the field can be accessed
    \returns 0=unit cannot access this field;
             1=unit can move across this field but cannot keep standing there;
             2=unit can move and stand there;
             < 0 unit cannot access this field, because of:
                  -1   very deep water required to submerge deep
                  -2   deep water required to submerge
                  -3   unit cannot drive onto terrain
*/
int          terrainaccessible2 ( const pfield        field, const TerrainAccess& terrainAccess, int uheight );


/*!
  \brief calculate the height difference between two levels of height. 

  Since floating and ground based are assumed to be the same effective height, a simple subtraction isn't sufficient.
  Height is a numeric (and not bitmapped) value
 */
//! {@ 
extern int getheightdelta ( int height1, int height2 );
extern int getheightdelta ( const ContainerBase* c1, const ContainerBase* c2 );
//! }@

  /*!
    evaluates the visibility of a field
    \param pe the field to be evaluated
    \param player the player who is 'looking'
  */
  extern bool fieldvisiblenow( const pfield pe, int player = actmap->actplayer );

  /*!
    evaluates the visibility of a field
    \param pe the field to be evaluated
    \param player the player who is 'looking'
   */
  extern VisibilityStates fieldVisibility  ( const pfield pe, int player = actmap->actplayer );
  extern VisibilityStates fieldVisibility  ( const pfield pe, int player, tmap* gamemap );




class tdrawline8 : public tdrawline {
         public:
           void start ( int x1, int y1, int x2, int y2 );
           virtual void putpix ( int x, int y );
           virtual void putpix8 ( int x, int y ) = 0;
       };

extern SigC::Signal0<void> repaintMap;
extern SigC::Signal0<void> repaintDisplay;
extern SigC::Signal0<void> updateFieldInfo;
extern SigC::Signal0<void> cursorMoved;
extern SigC::Signal0<void> viewChanged;
extern SigC::Signal1<void,tmap*> mapChanged;
extern SigC::Signal1<void,ContainerBase*> showContainerInfo;
extern SigC::Signal1<void,Vehicletype*> showVehicleTypeInfo;
extern SigC::Signal0<bool> idleEvent;

extern void displaymap();

extern int getUnitSetID( const Vehicletype* veh );
extern int getUnitSetID( const BuildingType* bld );


#endif


