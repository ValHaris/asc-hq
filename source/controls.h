//     $Id: controls.h,v 1.29 2000-10-18 14:13:57 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.28  2000/10/11 14:26:25  mbickel
//      Modernized the internal structure of ASC:
//       - vehicles and buildings now derived from a common base class
//       - new resource class
//       - reorganized exceptions (errors.h)
//      Split some files:
//        typen -> typen, vehicletype, buildingtype, basecontainer
//        controls -> controls, viewcalculation
//        spfst -> spfst, mapalgorithm
//      bzlib is now statically linked and sources integrated
//
//     Revision 1.27  2000/09/24 19:57:04  mbickel
//      ChangeUnitHeight functions are now more powerful since they use
//        UnitMovement on their own.
//
//     Revision 1.26  2000/09/17 15:20:31  mbickel
//      AI is now automatically invoked (depending on gameoptions)
//      Some cleanup
//
//     Revision 1.25  2000/09/16 13:02:52  mbickel
//      Put the AI in place
//
//     Revision 1.24  2000/09/16 11:47:23  mbickel
//      Some cleanup and documentation again
//
//     Revision 1.23  2000/08/28 19:49:40  mbickel
//      Fixed: replay exits when moving satellite out of orbiter
//      Fixed: airplanes being able to endlessly takeoff and land
//      Fixed: buildings constructable by unit without resources
//
//     Revision 1.22  2000/08/28 14:37:14  mbickel
//      Fixed: satellite not able to leave orbiter
//      Restructured next-turn routines
//
//     Revision 1.21  2000/08/13 09:53:58  mbickel
//      Refuelling is now logged for replays
//
//     Revision 1.20  2000/08/12 12:52:43  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.19  2000/08/12 09:17:21  gulliver
//     *** empty log message ***
//
//     Revision 1.18  2000/08/02 10:28:25  mbickel
//      Fixed: generator vehicle not working
//      Streams can now report their name
//      Field information shows units filename
//
//     Revision 1.17  2000/08/02 08:47:58  mbickel
//      Fixed: Mineral resources where visible for all players
//
//     Revision 1.16  2000/07/23 17:59:52  mbickel
//      various AI improvements
//      new terrain information window
//
//     Revision 1.15  2000/07/06 11:07:27  mbickel
//      More AI work
//      Started modularizing the attack formula
//
//     Revision 1.14  2000/06/09 13:12:25  mbickel
//      Fixed tribute function and renamed it to "transfer resources"
//
//     Revision 1.13  2000/06/08 21:03:41  mbickel
//      New vehicle action: attack
//      wrote documentation for vehicle actions
//
//     Revision 1.12  2000/06/05 18:21:23  mbickel
//      Fixed a security hole which was opened with the new method of loading
//        mail games by command line parameters
//
//     Revision 1.11  2000/05/23 20:40:40  mbickel
//      Removed boolean type
//
//     Revision 1.10  2000/05/07 12:12:14  mbickel
//      New mouse option dialog
//      weapon info can now be displayed by clicking on a unit
//
//     Revision 1.9  2000/05/02 16:20:54  mbickel
//      Fixed bug with several simultaneous vehicle actions running
//      Fixed graphic error at ammo transfer in buildings
//      Fixed ammo loss at ammo transfer
//      Movecost is now displayed for mines and repairs
//      Weapon info now shows unhittable units
//
//     Revision 1.8  2000/04/27 16:25:19  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.7  2000/01/31 16:08:39  mbickel
//      Fixed crash in line
//      Improved error handling in replays
//
//     Revision 1.6  2000/01/01 19:04:16  mbickel
//     /tmp/cvsVhJ4Z3
//
//     Revision 1.5  1999/12/28 21:02:47  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.4  1999/12/07 21:57:56  mbickel
//      Fixed bugs in Weapon information window
//      Added vehicle function "no air refuelling"
//
//     Revision 1.3  1999/11/25 22:00:05  mbickel
//      Added weapon information window
//      Added support for primary offscreen frame buffers to graphics engine
//      Restored file time handling for DOS version
//
//     Revision 1.2  1999/11/16 03:41:18  tmwilson
//     	Added CVS keywords to most of the files.
//     	Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//     	Wrote replacement routines for kbhit/getch for Linux
//     	Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//     	Added autoconf/automake capabilities
//     	Added files used by 'automake --gnu'
//
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


/*! \file controls.h
   Controlling units; unit information on the screen; ...
*/


  //! The unit, weather and map information displayed on the right side of the screen
  class tdashboard {
                public:
                       pfont        font; 
                       int         x, y; 
                       pvehicle     vehicle;
                       pvehicletype vehicletype;
                       pbuilding    building;
                       pobjectcontainer      object;

                       void         allocmem ( void );
                    protected:
                       void*        fuelbkgr;
                       int          fuelbkgrread;
                       void*        imagebkgr;
                       int          imageshown;
                       int          movedisp;   // 0: Movement f?r Runde    1: movement mit Tank
                       int          windheight; // 4 , 5 , 6 
                       int          windheightshown;
                       void*        windheightbackground;
                       void         putheight(integer      i,   integer      sel);
                       void         paintheight(void);
                       void         painttank(void);
                       void         paintweapon( int h, int num, int strength, const SingleWeapon  *weap );
                       void         paintweapons(void);
                       void         paintweaponammount( int h, int num, int max );
                       void         paintdamage(void);
                       void         paintexperience(void);
                       void         paintmovement(void);
                       void         paintarmor(void);
                       void         paintwind( int repaint = 0 );
                       void         paintclasses ( void );
                       void         paintname ( void );
                       void         paintimage( void );
                       void         paintplayer( void );
                       void         paintalliances ( void ); 
                       void         paintsmallmap ( int repaint = 0  );
                       void         paintlargeweaponinfo ( void );
                       void         paintlargeweapon ( int pos, const char* name, int ammoact, int ammomax, int shoot, int refuel, int strengthmax, int strengthmin, int distmax, int distmin, int from, int to );
                       void         paintlargeweaponefficiency ( int pos, int* e, int alreadypainted, int nohit );
                     #ifdef FREEMAPZOOM
                       void         paintzoom( void );
                     public:
                       struct {
                          void*        pic;
                          int x1, y1, x2, y2;
                          int picwidth;
                       } zoom;
                      #endif
                     public:
                       void         checkformouse ( int func = 0 );
                       void paint ( const pfield fld, int playerview );
                       tdashboard ( void );
                       void paintvehicleinfo( const pvehicle     vehicle,
                                              const pbuilding    building,
                                              const pobjectcontainer      object,
                                              const pvehicletype  vehicletype );

                       void         paintlweaponinfo ( void );

                       int backgrndcol ;
                       int vgcol       ;
                       int ymx         ;
                       int ymn         ;
                       int ydl         ;
                       int munitnumberx ;

                       struct {
                           int mindist, maxdist, minstrength, maxstrength;
                           int displayed;
                       } weaps[8];

                       int repainthard;
                       protected:
                         char* str_2 ( int num );
                         int materialdisplayed;


                    }; 
  extern tdashboard dashboard; 



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
  extern tmoveparams moveparams; 





   class tputmine : public tsearchfields {
                       int player;
                public:
                       char             mienentyp;
                       char          mienenlegen, mienenraeumen;
                       char             numberoffields;
                       virtual void     testfield ( void );
                       void             initpm( char mt, const pvehicle eht );
                       void             run ( void );
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


extern int windmovement[8];


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

extern tselectbuildingguihost    selectbuildinggui;

extern void continuenetworkgame ( void );

//! Move the technology that is currently being reseached to the list of discovered technologies
extern void addtechnology ( void );

//! Calculates the resources that are needed to research the given number of research
extern void returnresourcenuseforresearch ( const pbuilding bld, int research, int* energy, int* material );
extern void returnresourcenuseforpowerplant (  const pbuilding bld, int prod, Resources *usage, int percentagee_based_on_maxplus );

extern void dissectvehicle ( pvehicle eht );


extern void getpowerplantefficiency ( const pbuilding bld, int* material, int* fuel );



struct tmininginfo {
         Resources avail[maxminingrange+2];
         int efficiency[maxminingrange+2];
         Resources max[maxminingrange+2];            // soviel Bodenschaetze k”nnten in der Entfernung untergebracht werden.
};                                          


class tgetmininginfo : public tsearchfields {
          protected:
             void testfield ( void );
          public:
             tmininginfo* mininginfo;
             tgetmininginfo ( void );
             void run ( const pbuilding bld );
         };


extern void addanytechnology ( ptechnology tech, int player );





enum trpl_actions { rpl_attack, rpl_move, rpl_changeheight, rpl_convert, rpl_remobj, rpl_buildobj, rpl_putbuilding, 
                    rpl_removebuilding, rpl_putmine, rpl_removemine, rpl_produceunit, rpl_removeunit, rpl_trainunit, 
                    rpl_reactionfire, rpl_finished, rpl_shareviewchange, rpl_alliancechange, rpl_move2, rpl_buildtnk,
                    rpl_refuel }; 


extern void logtoreplayinfo ( trpl_actions action, ... );

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


class trunreplay {
         protected:
            int movenum;
            treplayguihost gui;
            void execnextreplaymove ( void );
            tmap orgmap;
            pmemorystream stream;
            int removeunit ( int x, int y, int nwid );
            int removeunit ( pvehicle eht, int nwid );
            void wait ( int t = ticker );
            int actplayer;

            char nextaction;

            void readnextaction ( void );
            void displayActionCursor ( int x1, int y1, int x2 = -1, int y2 = -1, int secondWait = 0 );
            void removeActionCursor( void );

         public:

            preactionfire_replayinfo getnextreplayinfo ( void );

            trunreplay ( void );
            int status;
            void firstinit ( void );
            int  run ( int player );

   };
extern trunreplay runreplay;
extern void checkforreplay ( void );

extern void   generatevehicle_cl ( pvehicletype fztyp,
                                  int           col,
                                  pvehicle &    vehicle,
                                  int           x, 
                                  int           y );
// Generiert eine LEERE vehicle. MIT abfrage, ob dies M”glich ist, MIT auswerten der Klassen
//  => Nur f?rs Spiel





// nets

class MapNetwork {
                static int instancesrunning;
             protected:
                int pass;

                struct tposition {
                   int x, y ;
                };

                tposition startposition;


                virtual int fieldavail ( int x, int y ) = 0;
                virtual int searchfinished ( void ) = 0;
                virtual void checkvehicle ( pvehicle v ) = 0;
                virtual void checkbuilding ( pbuilding b ) = 0;
                virtual int globalsearch ( void ) = 0;

                virtual void searchbuilding ( int x, int y );
                virtual void searchvehicle  ( int x, int y );
                virtual void searchfield ( int x, int y, int dir );
             public:
                virtual void start ( int x, int y );
                MapNetwork ( int checkInstances = 1 );
                virtual ~MapNetwork();
           };

class ResourceNet : public MapNetwork {
               public:
                  ResourceNet ( int _scope = -1 ) : MapNetwork ( _scope != 0 ) {};
               protected:
                  int resourcetype;
                  int scope;

                  virtual int fieldavail ( int x, int y );
                  virtual int globalsearch ( void ) { return scope; };
             };

class StaticResourceNet : public ResourceNet {
               protected:
                  int need;
                  int got;
                  int queryonly;
                  int player;

                  virtual int searchfinished ( void );

              public:
                  StaticResourceNet ( int scope = -1 ) : ResourceNet ( scope ) {};
                  int getresource ( int x, int y, int resource, int _need, int _queryonly, int _player, int _scope );
                       /* _scope:  0 : only this field
                                   1 : net
                                   2 : global
                       */
       };

class GetResource : public StaticResourceNet {
              protected:
                  int tributegot[3][8];
                  virtual void checkvehicle ( pvehicle v );
                  virtual void checkbuilding ( pbuilding b );
                  virtual void start ( int x, int y );
              public:
                  GetResource ( int scope = -1 );
   };

class PutResource : public StaticResourceNet {
              protected:
                  virtual void checkbuilding ( pbuilding b );
                  virtual void checkvehicle ( pvehicle v ) {};
                  virtual void start ( int x, int y );
               public:
                   PutResource ( int scope = -1 ) : StaticResourceNet ( scope ) {};
   };

class PutTribute : public StaticResourceNet {
              protected:
                  int targplayer;
                  pbuilding startbuilding;
                  virtual void checkbuilding ( pbuilding b );
                  virtual void checkvehicle ( pvehicle v ) {};
                  virtual void start ( int x, int y );
              public:
                  int puttribute ( pbuilding start, int resource, int _queryonly, int _forplayer, int _fromplayer, int _scope );
   };


class GetResourceCapacity : public StaticResourceNet {
              protected:
                  virtual void checkbuilding ( pbuilding b );
                  virtual void checkvehicle ( pvehicle v ) {};
                  virtual void start ( int x, int y );
                  virtual int searchfinished ( void ) { return 0; };
   };

class ResourceChangeNet : public ResourceNet {
               protected:
                  int got;
                  int player;

                  virtual int searchfinished ( void ) { return 0; };

              public:
                  int getresource ( int x, int y, int resource, int _player, int _scope );
                       /* _scope:  0 : only this field
                                   1 : net
                                   2 : global
                       */

       };


class GetResourcePlus : public ResourceChangeNet {
              protected:
                  virtual void checkbuilding ( pbuilding b );
                  virtual void checkvehicle ( pvehicle v );
   };

class GetResourceUsage : public ResourceChangeNet {
              protected:
                  virtual void checkbuilding ( pbuilding b );
                  virtual void checkvehicle ( pvehicle v ) {};
   };

extern int searchexternaltransferfields ( pbuilding bld );
extern void transfer_all_outstanding_tribute( void );


extern int computeview( int player_fieldcount_mask = 0 );
extern int  evaluateviewcalculation ( int player_fieldcount_mask = 0 );     // playermask determines, which players should be counted when the view has changed
                                                                // returns the number which have changed visibilitystatus

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
