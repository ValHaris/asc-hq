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
  
  
  #include "typen.h"
  #include "gamemap.h"
  

//! puts a building onto the map. To be used by the map editor! For ingame usage, see ConstructBuildingCommand or SpawnBuilding
extern void  putbuilding( GameMap* gamemap,
                          const MapCoordinate& entryPosition,
                         int          color,
                         const BuildingType* buildingtyp,
                         int          completion,
                         int          ignoreunits = 0 ); 


//! recalculates the connection (like road interconnections) of all objects on the map
extern void  calculateallobjects( GameMap* m );

/** recalculates the connection (like road interconnections) of an object
      \param x The x coordinate of the field
      \param y The y coordinate of the field
      \param mof Should the neighbouring fields be modified if necessary
      \param obj The objecttype that is to be aligned on this field
      \param gamemap the map that the object is on
*/
extern void  calculateobject(int  x,
                             int  y,
                             bool mof,
                             const ObjectType* obj,
                             GameMap* gamemap  );

extern void  calculateobject( const MapCoordinate& pos, 
                             bool mof,
                             const ObjectType* obj,
                             GameMap* gamemap  );



/*! tests if the vehicle can move onto the field
   
   \param uheight the level of height for which the check should be done. Use -2 to use the current height of the unit
   \retval 0 unit cannot move there
   \retval 1 unit can pass over the field
   \retval 2 unit can stop its movement there
*/
extern int fieldAccessible( const MapField*        field,
                            const Vehicle*     vehicle,
                            int  uheight = -2,
                            const bool* attacked = NULL,
                            bool ignoreVisibility = false );


/** removes all units that cannot exist any more, either due to invalid terrin
    (like tanks on melting ice) or too much wind (eg. hoovercrafts in a storm) */
extern void  checkunitsforremoval( GameMap* gamemap );

//! removes all objects that cannot exist where they currently are (terrainaccess)
extern void checkobjectsforremoval( GameMap* gamemap );

//! returns the maximum wind speed that the unit can endure
extern int          getmaxwindspeedforunit ( const Vehicle* eht );

/** Wind may be different at different heights. This function returns the index 
    for the wind array. If uheight != -1 it is assumed the unit was uheight instead
    the actual level of height */
extern int          getwindheightforunit   ( const Vehicle* eht, int uheight = -1 );


/** Checks if the unit can drive on the field
    \param field the field that the unit is checked again
    \param vehicle the unit who accessabilit to field is tested
    \param uheight if != -1, the unit is assumed to be on this height instead of the actual one.
    \returns 0=unit cannot access this field; 
             1=unit can move across this field but cannot keep standing there
             2=unit can move and stand there
*/
extern int          terrainaccessible (  const MapField* field, const Vehicle* vehicle, int uheight = -1 );

/** Checks if the unit can drive on the field
    \param field the field that the unit is checked again
    \param vehicle the unit who accessabilit to field is tested
    \param uheight if != -1, the unit is assumed to be on this height instead of the actual one.
    \returns 0=unit cannot access this field;
             1=unit can move across this field but cannot keep standing there;
             2=unit can move and stand there;
             < 0 unit cannot access this field, because of:
                  -1   very deep water required to submerge deep
                  -2   deep water required to submerge
                  -3   unit cannot drive onto terrain
*/
extern int          terrainaccessible2 ( const MapField*        field, const Vehicle* vehicle, int uheight = -1 );


/** Checks if the field can be accessed
    \returns 0=unit cannot access this field;
             1=unit can move across this field but cannot keep standing there;
             2=unit can move and stand there;
             < 0 unit cannot access this field, because of:
                  -1   very deep water required to submerge deep
                  -2   deep water required to submerge
                  -3   unit cannot drive onto terrain
*/
int          terrainaccessible2 ( const MapField*        field, const TerrainAccess& terrainAccess, int uheight );


/*!
  \brief calculate the height difference between two levels of height.

  Since floating and ground based are assumed to be the same effective height, a simple subtraction isn't sufficient.
  Height is a numeric (and not bitmapped) value
 */
extern int getheightdelta ( const ContainerBase* c1, const ContainerBase* c2 );

  /*!
    evaluates the visibility of a field
    \param pe the field to be evaluated
    \param player the player who is 'looking'
  */
//! {@ 
  extern bool fieldvisiblenow( const MapField* pe, Vehicle* veh, int player   );
  inline bool fieldvisiblenow( const MapField* pe, int player ) { return fieldvisiblenow( pe, NULL, player); };
  inline bool fieldvisiblenow( const MapField* pe, Vehicle* veh  ) { return fieldvisiblenow( pe, veh, pe->getMap()->actplayer); };
  inline bool fieldvisiblenow( const MapField* pe ) { return fieldvisiblenow( pe, pe->getMap()->actplayer);};
//! }@

  /*!
    evaluates the visibility of a field
    \param pe the field to be evaluated
    \param player the player who is 'looking'; without player the active player will be used
   */
//! {@ 
  extern VisibilityStates fieldVisibility  ( const MapField* pe );
  extern VisibilityStates fieldVisibility  ( const MapField* pe, int player );
//! }@


extern SigC::Signal0<void> repaintMap;
extern SigC::Signal0<void> repaintDisplay;
extern SigC::Signal0<void> updateFieldInfo;
extern SigC::Signal0<void> cursorMoved;

//! the view of the player onto the map changed, for example because he scrolled the map
extern SigC::Signal0<void> viewChanged;
extern SigC::Signal1<void,GameMap*> mapChanged;
extern SigC::Signal1<void,ContainerBase*> showContainerInfo;
extern SigC::Signal1<void,VehicleType*> showVehicleTypeInfo;
extern SigC::Signal0<bool> idleEvent;
extern SigC::Signal1<void, const Context&> fieldCrossed;

extern void displaymap();


#endif


