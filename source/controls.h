/*! \file controls.h
   Controlling units (which is graudally moved to #vehicletype.cpp and #unitctrl.cpp );
   Resource networks
   Things that are run when starting and ending someones turn   
*/

//     $Id: controls.h,v 1.52 2004-05-11 20:22:33 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.50  2004/01/16 15:33:45  mbickel
//     Completely rewritten game event system
//     TPWM-decoding-Patch
//     Fixed: swallog message: wrong coordinates
//     Autotraining for units with max ammo only
//     Movement required for clearing mines
//     Unit names can be edited
//     weather dependen object properties
//     Unit swallowed by ground -> unified message
//     units cannot enter enemy transports
//     Building entry has constant movemalus
//     Message for resource transfer for providing player
//     increased ammo production cost
//     Fixed: unit could attack after movement (with RF on) although "no attack after move" property was set
//     Buildings: new properties: "ExternalResourceTransfer", "ExternalAmmoTransfer"
//     Container: Movemalus override for unloading
//     Startup map specified in ASC.INI
//
//     Revision 1.49  2003/05/01 18:02:22  mbickel
//      Fixed: no movement decrease for cargo when transport moved
//      Fixed: reactionfire not working when descending into range
//      Fixed: objects not sorted
//      New map event: add object
//
//     Revision 1.48  2003/04/15 20:06:49  mbickel
//      Made readrlepict Endian independant
//      Updated weaponguide
//      Made Replays much more strict
//
//     Revision 1.47  2003/02/27 16:11:00  mbickel
//      Restructuring of new pathfinding code completed
//
//     Revision 1.46  2003/02/19 19:47:25  mbickel
//      Completely rewrote Pathfinding code
//      Wind not different any more on different levels of height
//
//     Revision 1.45  2003/01/28 17:48:42  mbickel
//      Added sounds
//      Rewrote soundsystem
//      Fixed: tank got stuck when moving from one transport ship to another
//
//     Revision 1.44  2003/01/12 19:37:18  mbickel
//      Rewrote resource production
//
//     Revision 1.43  2001/09/13 17:43:12  mbickel
//      Many, many bug fixes
//
//     Revision 1.42  2001/07/18 16:05:47  mbickel
//      Fixed: infinitive loop in displaying "player exterminated" msg
//      Fixed: construction of units by units: wrong player
//      Fixed: loading bug of maps with mines
//      Fixed: invalid map parameter
//      Fixed bug in game param edit dialog
//      Fixed: cannot attack after declaring of war
//      New: ffading of sounds
//
//     Revision 1.41  2001/07/14 19:13:15  mbickel
//      Rewrote sound system
//      Moveing units make sounds
//      Added sound files to data
//
//     Revision 1.40  2001/03/30 12:43:15  mbickel
//      Added 3D pathfinding
//      some cleanup and documentation
//      splitted the ai into several files, now located in the ai subdirectory
//      AI cares about airplane servicing and range constraints
//
//     Revision 1.39  2001/02/26 12:35:04  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.38  2001/01/28 14:04:09  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.37  2001/01/23 21:05:15  mbickel
//      Speed up of AI
//      Lot of bugfixes in AI
//      Moved Research to own files (research.*)
//      Rewrote storing of developed technologies
//      Some cleanup and documentation
//
//     Revision 1.36  2001/01/21 16:37:16  mbickel
//      Moved replay code to own file ( replay.cpp )
//      Fixed compile problems done by cleanup
//
//     Revision 1.35  2001/01/21 12:48:35  mbickel
//      Some cleanup and documentation
//
//     Revision 1.34  2001/01/19 13:33:49  mbickel
//      The AI now uses hemming
//      Several bugfixes in Vehicle Actions
//      Moved all view calculation to viewcalculation.cpp
//      Mapeditor: improved keyboard support for item selection
//
//     Revision 1.33  2000/12/21 11:00:49  mbickel
//      Added some code documentation
//
//     Revision 1.32  2000/11/21 20:26:59  mbickel
//      Fixed crash in tsearchfields (used by object construction for example)
//      AI improvements
//      configure.in: added some debug output
//                    fixed broken check for libbz2
//
//     Revision 1.31  2000/11/08 19:30:59  mbickel
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

#if defined(karteneditor) && !defined(pbpeditor)
 #error the mapeditor should not need to use controls.h !
#endif


#ifndef controlsH
#define controlsH
#include "gui.h"
#include "typen.h"
#include "mousecontrol.h"
#include "soundList.h"
#include "astar2.h"




   //! some old system for keeping track which unit action is currently running. As units actions are moved to #unitctrl.cpp , this structure is beeing replaced by #pendingVehicleActions
   struct tmoveparams { 
                        unsigned char         movestatus;       /*  Folgende Modi sind definiert : 
                                                                             0:  garnichts, standard
                                                                             72: putstreet
                                                                             90: putmine
                                                                             111: putbuilding l1
                                                                             112: putbuilding l2
                                                                             115: removebuilding
                                                                             120: construct vehicle
                                                                             130: external loading
                                                                   */

                        word         movesx, movesy, moveerr; 
                        pvehicle     vehicletomove;
                        int          newheight; 
                        int          oldheight; 
                        char         heightdir; 
                        pbuildingtype buildingtobuild;   /*  nur bei movestatus = 111  */ 
                        int          movespeed;
                        int          uheight;
                     }; 
  //! see #tmoveparams
  extern tmoveparams moveparams; 



/*! calculates the movement cost for moving vehicle from start to dest.
    \returns : first: movement ; second: fuel consumption
*/
extern pair<int,int> calcMoveMalus( const MapCoordinate3D& start,
                                    const MapCoordinate3D& dest,
                                    pvehicle     vehicle,
                                    WindMovement* wm = NULL,
                                    bool*  inhibitAttack = NULL,
                                    bool container2container  = false );

//! return the distance between x1/y1 and x2/y2 using the power of the wind factors calculated for a specific unit with #initwindmovement
extern int windbeeline ( const MapCoordinate& start, const MapCoordinate& dest, WindMovement* wm );


/*! Ends the turn of the current player and runs AI until a player is human again
    \param playerView -2 = detect automatically; -1 = don't display anything; 0-7 = this player is watching
*/
extern void next_turn ( int playerView = -2 );

//! checks if the current player has terminated another player or even won
extern void  checkforvictory ( );


/////////////////////////////////////////////////////////////////////
///// old vehicle actions
/////////////////////////////////////////////////////////////////////


//! An old procedure for building and removing objects with a unit.
extern void  setspec( pobjecttype obj );

//! A helper function for #setspec
extern int  object_constructable ( int x, int y, pobjecttype obj );

//! A helper function for #setspec
extern int  object_removeable ( int x, int y, pobjecttype obj );

//! A helper function for #setspec
extern void build_objects_reset( void );


//! An old procedure for building vehicle (like turrets) with a unit.
extern void  constructvehicle( pvehicletype tnk );

//! A helper function for #constructvehicle
extern void build_vehicles_reset( void );

//! An old procedure for putting and removing mines.
extern void  legemine( int typ, int delta );

//! An old procedure for constructing a building with a vehicle
extern void  putbuildinglevel1(void);

//! An old procedure for constructing a building with a vehicle
extern void  putbuildinglevel2( const pbuildingtype bld, integer      xp, integer      yp);

//! An old procedure for constructing a building with a vehicle
extern void  putbuildinglevel3(integer      x, integer      y);

//! An old procedure for removing a building with a vehicle
extern void         destructbuildinglevel2( int xp, int yp);

//! An old procedure for removing a building with a vehicle
extern void         destructbuildinglevel1( int xp, int yp);



//! continues a PBeM game; the current map is deleted
extern void continuenetworkgame ( void );



//! dissects a vehicle; if you haven't researched this vehicle type you will get some research points for it.
extern void dissectvehicle ( pvehicle eht );




struct treactionfire_replayinfo {
         int x1 ;
         int y1 ;
         int x2 ;
         int y2 ;
         int ad1 ;
         int ad2 ;
         int dd1 ;
         int dd2 ;
         int wpnum;
};
typedef treactionfire_replayinfo* preactionfire_replayinfo;




/** creates a new vehicle, setting up its class depending on the research. If this vehicletype is
     not available (because it hasn't been researched, for example), vehicle will be set to NULL,
     else it will contain a pointer to the newly created vehicle. The vehicle will be empty after
     creation (no fuel, etc). The resources for the creation must be seperately 'consumed'. */
extern void   generatevehicle_cl ( pvehicletype fztyp,
                                                int               col,
                                                pvehicle &    vehicle,
                                                int               x, 
                                                int               y );

extern int searchexternaltransferfields ( pbuilding bld );


class treactionfire {
          public:
             virtual int  checkfield ( const MapCoordinate3D& pos, pvehicle &eht, MapDisplayInterface* md ) = 0;
             virtual void init ( pvehicle eht, const AStar3D::Path&  fieldlist ) = 0;
             virtual ~treactionfire() {};
        };

class treactionfirereplay : public treactionfire {
          protected:
             int num;
             dynamic_array<preactionfire_replayinfo> replay;
             pvehicle unit;
          public:
             treactionfirereplay ( void );
             ~treactionfirereplay ( );
             virtual int checkfield ( const MapCoordinate3D& pos, pvehicle &eht, MapDisplayInterface* md );
             virtual void init ( pvehicle eht, const AStar3D::Path& fieldlist );
   };

class tsearchreactionfireingunits : public treactionfire {
           protected:


                static int maxshootdist[8];     // f?r jede H”henstufe eine
                void addunit ( pvehicle vehicle );
                void removeunit ( pvehicle vehicle );
           public:

                tsearchreactionfireingunits( void );
                void init ( pvehicle eht, const AStar3D::Path& fieldlist );
                int  checkfield ( const MapCoordinate3D& pos, pvehicle &eht, MapDisplayInterface* md );
                ~tsearchreactionfireingunits();
      };

extern void initNetworkGame( void );

class ReplayMapDisplay : public MapDisplayInterface {
           MapDisplay* mapDisplay;
           int cursorDelay;
           void wait ( int minTime = 0 );
         public:
           ReplayMapDisplay ( MapDisplay* md ) { mapDisplay = md; cursorDelay = 20; };
           int displayMovingUnit ( const MapCoordinate3D& start, const MapCoordinate3D& dest, pvehicle vehicle, int fieldnum, int totalmove, SoundLoopManager* slm );
           void displayPosition ( int x, int y );
           void deleteVehicle ( pvehicle vehicle ) { mapDisplay->deleteVehicle ( vehicle ); };
           void displayMap ( void ) { mapDisplay->displayMap(); };
           void resetMovement ( void ) { mapDisplay->resetMovement(); };
           void startAction ( void ) { mapDisplay->startAction(); };
           void stopAction ( void ) { mapDisplay->stopAction(); };
           void displayActionCursor ( int x1, int y1, int x2 , int y2 , int secondWait );
           void displayActionCursor ( int x1, int y1 ) { displayActionCursor ( x1, y1, -1, -1, 0 ); };
           void displayActionCursor ( int x1, int y1, int x2 , int y2 ) { displayActionCursor ( x1, y1, x2, y2, 0 ); };
           void removeActionCursor ( void );
           int checkMapPosition ( int x, int y );
           void setCursorDelay  ( int time ) { cursorDelay = time; };
           void updateDashboard() { mapDisplay->updateDashboard(); };
           void repaintDisplay () { mapDisplay->repaintDisplay(); };

    };


#endif
