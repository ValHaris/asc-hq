/*! \file spfst.h
    \brief map accessing and usage routines used by ASC and the mapeditor

    spfst comes from german "Spielfeldsteuerung" :-)
*/


//     $Id: spfst.h,v 1.51 2002-11-17 11:43:23 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.50  2002/11/02 14:13:18  mbickel
//      New net handling for objects
//
//     Revision 1.49  2002/10/09 16:58:46  mbickel
//      Fixed to GrafikSet loading
//      New item filter for mapeditor
//
//     Revision 1.48  2002/10/06 15:44:40  mbickel
//      Completed inheritance of .asctxt files
//      Speed up of replays
//
//     Revision 1.47  2002/03/18 21:42:17  mbickel
//      Some cleanup and documentation in the Mine class
//      The number of mines is now displayed in the field information window
//
//     Revision 1.46  2001/12/14 10:20:05  mbickel
//      Cleanup and enhancements to configure.in
//      Removed last remains of octagonal version from source files
//
//     Revision 1.45  2001/10/31 18:34:33  mbickel
//      Some adjustments and fixes for gcc 3.0.2
//
//     Revision 1.44  2001/08/02 18:50:43  mbickel
//      Corrected Error handling in Text parsers
//      Improved version information
//
//     Revision 1.43  2001/07/28 11:19:12  mbickel
//      Updated weaponguide
//      moved item repository from spfst to itemrepository
//
//     Revision 1.42  2001/03/30 12:43:16  mbickel
//      Added 3D pathfinding
//      some cleanup and documentation
//      splitted the ai into several files, now located in the ai subdirectory
//      AI cares about airplane servicing and range constraints
//
//     Revision 1.41  2001/02/26 12:35:33  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.40  2001/02/18 15:37:20  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.39  2001/02/01 22:48:51  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.38  2001/01/28 14:04:20  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.37  2001/01/25 23:45:06  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.36  2001/01/23 21:05:22  mbickel
//      Speed up of AI
//      Lot of bugfixes in AI
//      Moved Research to own files (research.*)
//      Rewrote storing of developed technologies
//      Some cleanup and documentation
//
//     Revision 1.35  2001/01/21 16:37:22  mbickel
//      Moved replay code to own file ( replay.cpp )
//      Fixed compile problems done by cleanup
//
//     Revision 1.34  2001/01/21 12:48:36  mbickel
//      Some cleanup and documentation
//
//     Revision 1.33  2001/01/19 13:33:56  mbickel
//      The AI now uses hemming
//      Several bugfixes in Vehicle Actions
//      Moved all view calculation to viewcalculation.cpp
//      Mapeditor: improved keyboard support for item selection
//
//     Revision 1.32  2000/11/21 20:27:10  mbickel
//      Fixed crash in tsearchfields (used by object construction for example)
//      AI improvements
//      configure.in: added some debug output
//                    fixed broken check for libbz2
//
//     Revision 1.31  2000/11/08 19:31:15  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
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

  #include "newfont.h"
  #include "basegfx.h"
  #include "typen.h"
  #include "events.h"
  #include "gamemap.h"

  #pragma pack(1)

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

   class tcursor { 
           public:
              int       posx, posy;
              char      an; 
              int       color; 

              int  gotoxy (int x, int y, int disp = 1 );     // result: 0: map wurde nicht neu angezeigt     1: map wurde neu angezeigt
              void show ( void );
              void hide ( void );
              void setcolor ( int col );
              void init ( void );
              void display ( void );
              void reset ( void );
              void*        markfield; 
              void*        picture; 
              void*        orgpicture;
              int          actpictwidth;

              int         checkposition ( int x, int y );

              virtual void getimg  ( void );
              virtual void putimg  ( void );
              virtual void putbkgr ( void );
              int          oposx, oposy;
              void*      backgrnd; 
              virtual void checksize ( void );
   };


  extern tcursor cursor;

  //! this is the one and only map that is loaded by ASC
  extern pmap actmap; 


 //! passes a key to the map-cursor
 extern void  movecursor(tkey         ch);
 

//! returns the field that is selected with the cursor
extern pfield getactfield(void);

//! returns the field at the given coordinates
extern pfield getfield(int x, int y);

//! returns the x coordinate of the cursor location
extern int  getxpos(void);

//!returns the y coordinate of the cursor location
extern int  getypos(void);


//! returns the diplomatic status between actmap->actplayer and the player with color b (note that the color is playernum*8 ) 
extern int getdiplomaticstatus( int b );

//! returns the diplomatic status between the players with color c and b (note that the color is playernum*8 ) 
extern int getdiplomaticstatus2( int c, int b);

extern void  putbuilding( const MapCoordinate& entryPosition,
                         int          color,
                         pbuildingtype buildingtyp,
                         int          completion,
                         int          ignoreunits = 0 ); // f?r Kartened

extern void  putbuilding2( const MapCoordinate& entryPosition,
                          int color,
                          pbuildingtype buildingtyp);  // f?r Spiel


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
                             pobjecttype obj,
                             pmap gamemap = actmap );

//! generate a map of size xsize/ysize and consisting just of fields bt. The map is stored in #actmap
extern void  generatemap( TerrainType::Weather* bt,
                          int          xsize,
                          int          ysize);

//! puts a line of objects from x1/y1 to x2/y2, moving round obstacles
extern void  putstreets2( int          x1,
                          int          y1,
                          int          x2,
                          int          y2,
                          pobjecttype obj );


/** removes objects like tracks (by vehicles) or shipping lanes (made by icebreakers) 
    after their lifetime (specified in the mapparameters) is exceeded      */
extern void  clearfahrspuren(void);


/*! tests if the vehicle can move onto the field

   \param uheight the level of height for which the check should be done. Use -1 to use the current height of the unit
   \retval 0 unit cannot move there
   \retval 1 unit can pass over the field
   \retval 2 unit can stop its movement there
*/
extern int fieldaccessible( const pfield        field,
                            const pvehicle     vehicle,
                            int  uheight = -1 );

//! returns the image of an (explosive) mine. The type of the mine is specified by num.
extern void* getmineadress( int num , int uncompressed = 0 );

/** removes all units that cannot exist any more, either due to invalid terrin
    (like tanks on melting ice) or too much wind (eg. hoovercrafts in a storm) */
extern void  checkunitsforremoval ( void );

//! removes all objects that cannot exist where they currently are (terrainaccess)
extern void checkobjectsforremoval ( void );

//! returns the maximum wind speed that the unit can endure
extern int          getmaxwindspeedforunit ( const pvehicle eht );

/** Wind may be different at different heights. This function returns the index 
    for the wind array. If uheight != -1 it is assumed the unit was uheight instead
    the actual level of height */
extern int          getwindheightforunit   ( const pvehicle eht, int uheight = -1 );

/** Each field that has a building on it stores a pointer to the picture of the 
    buildings' part for faster displaying. This function refreshes these pointers
    on all fields */
extern void         resetallbuildingpicturepointers ( void );

/** Checks if the unit can drive on the field
    \param uheight if != -1, the unit is assumed to be on this height instead of the actual one.
    \returns 0=unit cannot access this field; 
             1=unit can move across this field but cannot keep standing there
             2=unit can move and stand there
*/
extern int          terrainaccessible (  const pfield        field, const pvehicle     vehicle, int uheight = -1 );

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
extern int          terrainaccessible2 ( const pfield        field, const pvehicle     vehicle, int uheight = -1 );  


/*!
  \brief calculate the height difference between two levels of height

  Since floating and ground based are assumed to be the same effective height, a simple subtraction isn't sufficient.
 */
extern int getheightdelta ( int height1, int height2 );

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




class tdrawline8 : public tdrawline {
         public:
           void start ( int x1, int y1, int x2, int y2 );
           virtual void putpix ( int x, int y );
           virtual void putpix8 ( int x, int y ) = 0;
       };

extern int  rol ( int valuetorol, int rolwidth );

#pragma pack()

#endif


