/*! \file controls.h
   Controlling units (which is graudally moved to #vehicletype.cpp and #unitctrl.cpp );
   Resource networks
   Things that are run when starting and ending someones turn   
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

#if defined(karteneditor) && !defined(pbpeditor)
 #error the mapeditor should not need to use controls.h !
#endif


#ifndef controlsH
#define controlsH
#include "typen.h"
// #include "mousecontrol.h"
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

                        int           movesx, movesy, moveerr; 
                        Vehicle*     vehicletomove;
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
                                    const Vehicle*     vehicle,
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


//! An old procedure for building vehicle (like turrets) with a unit.
extern void  constructvehicle( pvehicletype tnk );

//! A helper function for #constructvehicle
extern void build_vehicles_reset( void );

//! An old procedure for putting and removing mines.
extern void  putMine( const MapCoordinate& pos, int typ, int delta );

#if 0
//! An old procedure for constructing a building with a vehicle
extern void  putbuildinglevel1(void);

//! An old procedure for constructing a building with a vehicle
extern void  putbuildinglevel2( const pbuildingtype bld, integer      xp, integer      yp);

//! An old procedure for constructing a building with a vehicle
extern void  putbuildinglevel3(integer      x, integer      y);

#endif

//! An old procedure for removing a building with a vehicle
extern void         destructbuildinglevel2( int xp, int yp);

//! An old procedure for removing a building with a vehicle
extern void         destructbuildinglevel1( int xp, int yp);



//! continues a PBeM game; the current map is deleted
extern void continuenetworkgame ( void );



//! dissects a vehicle; if you haven't researched this vehicle type you will get some research points for it.
extern void dissectvehicle ( Vehicle* eht );




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
                                                Vehicle* &    vehicle,
                                                int               x, 
                                                int               y );

extern int searchexternaltransferfields ( Building* bld );

extern Resources getDestructionCost( Building* bld, Vehicle* veh );


class treactionfire {
          public:
             virtual int  checkfield ( const MapCoordinate3D& pos, Vehicle* &eht, MapDisplayInterface* md ) = 0;
             virtual void init ( Vehicle* eht, const AStar3D::Path&  fieldlist ) = 0;
             virtual ~treactionfire() {};
        };

class treactionfirereplay : public treactionfire {
          protected:
             int num;
             dynamic_array<preactionfire_replayinfo> replay;
             Vehicle* unit;
          public:
             treactionfirereplay ( void );
             ~treactionfirereplay ( );
             virtual int checkfield ( const MapCoordinate3D& pos, Vehicle* &eht, MapDisplayInterface* md );
             virtual void init ( Vehicle* eht, const AStar3D::Path& fieldlist );
   };

class tsearchreactionfireingunits : public treactionfire {
           protected:


                static int maxshootdist[8];     // f?r jede H”henstufe eine
                void addunit ( Vehicle* vehicle );
                void removeunit ( Vehicle* vehicle );
           public:

                tsearchreactionfireingunits( void );
                void init ( Vehicle* eht, const AStar3D::Path& fieldlist );
                int  checkfield ( const MapCoordinate3D& pos, Vehicle* &eht, MapDisplayInterface* md );
                ~tsearchreactionfireingunits();
      };

extern void initNetworkGame( void );

class ReplayMapDisplay : public MapDisplayInterface {
           MapDisplayInterface* mapDisplay;
           int cursorDelay;
           void wait ( int minTime = 0 );
         public:
           ReplayMapDisplay ( MapDisplayInterface* md ) { mapDisplay = md; cursorDelay = 20; };
           int displayMovingUnit ( const MapCoordinate3D& start, const MapCoordinate3D& dest, Vehicle* vehicle, int fieldnum, int totalmove, SoundStartCallback startSound );
           void displayPosition ( int x, int y );
           void displayMap ( Vehicle* additionalVehicle ) { mapDisplay->displayMap( additionalVehicle ); };
           void displayMap ( void ) { mapDisplay->displayMap(); };
           void resetMovement ( void ) { mapDisplay->resetMovement(); };
           void startAction ( void ) { mapDisplay->startAction(); };
           void stopAction ( void ) { mapDisplay->stopAction(); };
           void cursor_goto( const MapCoordinate& pos ) { mapDisplay->cursor_goto(pos);};
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
