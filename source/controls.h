/*! \file controls.h
   Controlling units (which is graudally moved to #vehicletype.cpp and #unitctrl.cpp );
   Resource networks
   Things that are run when starting and ending someones turn   
*/

//     $Id: controls.h,v 1.38 2001-01-28 14:04:09 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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

#ifdef karteneditor
 #error the mapeditor should not need to use controls.h !
#endif


#ifndef controls_h
#define controls_h
#include "gui.h"
#include "typen.h"
#include "mousecontrol.h"





   //! some old system for keeping track which unit action is currently running. As units actions are moved to #unitctrl.cpp , this structure is beeing replaced by #pendingVehicleActions
   struct tmoveparams { 
                        unsigned char         movestatus;       /*  Folgende Modi sind definiert : 
                                                                             0:  garnichts, standard
                                                                             1:  movement l1
                                                                             2:  movement l2
                                                                             10: angriff
                                                                             11: movement l1 mit h”he-wechseln
                                                                             12: movement l2 mit h”he-wechseln
                                                                             65: refuel
                                                                             66: reparieren
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




   //! looks up the fields a unit can put or remove a mine from
   class tputmine : public tsearchfields {
                       int player;
                public:
                       char             mienentyp;
                       char          mienenlegen, mienenraeumen;
                       char             numberoffields;
                       virtual void     testfield ( void );
                       void             initpm( char mt, const pvehicle eht );
                       void             run ( void );
                       tputmine ( pmap _gamemap ) : tsearchfields ( _gamemap ) {};
              };

    //! checks, which vehicle types are newly available 
    class tprotfzt   {         
                    char*  buf;
                public:
                    //! initializes an internal buffer and checks which vehicles are currently available
                    void initbuffer( void );

                    //! checks, which vehicle are now available that where not available when initbuffer was called. The new ones are displayed by calling #tshownewtanks
                    void evalbuffer( void );
    
               };


/*! calculates the movement cost for moving vehicle from x1/y1 to x2/y2

   \param direc If x2 and y2 are -1, they are assumend to be the field beeing next to x1/y1 in the direction of direc
   \param fuelcost The fuelconsumption is written here
   \param movecost The required movement points are written to this variable
*/
    
extern void  calcmovemalus(int          x1,
                           int          y1,
                           int          x2,
                           int          y2,
                           pvehicle     vehicle,
                           int          direc,
                           int&         fuelcost,               
                           int&         movecost );            



/*! Ends the turn of the current player and runs AI until a player is human again
    \param playerView -2 = detect automatically; -1 = don't display anything; 0-7 = this player is watching
*/
extern void next_turn ( int playerView = -2 );


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

//! Initializes the wind calculations for moving vehicle
extern void initwindmovement( const pvehicle vehicle );

//! return the distance between x1/y1 and x2/y2 using the power of the wind factors calculated for a specific unit with #initwindmovement
extern int windbeeline ( int x1, int y1, int x2, int y2 );

//! continues a PBeM game; the current map is deleted
extern void continuenetworkgame ( void );



//! Calculates the resources that are needed to research the given number of research
extern void returnresourcenuseforresearch ( const pbuilding bld, int research, int* energy, int* material );

//! Calculates the resources that are needed run a power plant
extern void returnresourcenuseforpowerplant (  const pbuilding bld, int prod, Resources *usage, int percentagee_based_on_maxplus );

//! dissects a vehicle; if you haven't researched this vehicle type you will get some research points for it.
extern void dissectvehicle ( pvehicle eht );



//! calculates some mining statistics for a mining station
class tgetmininginfo : public tsearchfields {
          protected:
             void testfield ( void );
          public:
             struct tmininginfo {
                      Resources avail[maxminingrange+2];
                      int efficiency[maxminingrange+2];
                      Resources max[maxminingrange+2];            // the theoretical maximum of the mineral resources in the given distance
             };                                          
             tmininginfo* mininginfo;
             tgetmininginfo ( pmap _gamemap );
             void run ( const pbuilding bld );
         };





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
             virtual int  checkfield ( int x, int y, pvehicle &eht, MapDisplayInterface* md ) = 0;
             virtual void init ( pvehicle eht, IntFieldList* fieldlist ) = 0;
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
             virtual int checkfield ( int x, int y, pvehicle &eht, MapDisplayInterface* md );
             virtual void init ( pvehicle eht, IntFieldList* fieldlist );
   };

class tsearchreactionfireingunits : public treactionfire {
           protected:


                static int maxshootdist[8];     // f?r jede H”henstufe eine
                void addunit ( pvehicle vehicle );
                void removeunit ( pvehicle vehicle );
           public:

                tsearchreactionfireingunits( void );
                void init ( pvehicle eht, IntFieldList* fieldlist );
                int  checkfield ( int x, int y, pvehicle &eht, MapDisplayInterface* md );  
                ~tsearchreactionfireingunits();
      };

extern void initNetworkGame( void );

class ReplayMapDisplay : public MapDisplayInterface {
           MapDisplay* mapDisplay;
           int cursorDelay;
           void wait ( void );
         public:
           ReplayMapDisplay ( MapDisplay* md ) { mapDisplay = md; cursorDelay = 20; };
           int displayMovingUnit ( int x1,int y1, int x2, int y2, pvehicle vehicle, int height1, int height2, int fieldnum, int totalmove );
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
    };


#endif
