//     $Id: controls.h,v 1.5 1999-12-28 21:02:47 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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
#error eigentlich sollte der Karteneditor ohne die Controls auskommen kînnen !
#endif


#ifndef controls_h
#define controls_h
#include "gui.h"
#include "typen.h"
#include "timer.h"

/* unit header for: controls.c -- made by tptc - translate pascal to c */



   struct tmoveview { 
                        integer      x1, y1, x2, y2, x3, y3, x4, y4; 
                        integer      xpos, ypos; 
                        byte         viewweite; 
                     } ; 
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
                       int          movedisp;   // 0: Movement fÅr Runde    1: movement mit Tank
                       int          windheight; // 4 , 5 , 6 
                       int          windheightshown;
                       void*        windheightbackground;
                       void         putheight(integer      i,   integer      sel);
                       void         paintheight(void);
                       void         painttank(void);
                       void         paintweapon(byte         h, int num, int strength, const SingleWeapon  *weap );
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
                       void         paintlargeweaponefficiency ( int pos, int* e, int alreadypainted );
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
                       void         checkformouse ( void );
                       void paint ( const pfield fld, int playerview );
                       tdashboard ( void );
                       void paintvehicleinfo( const pvehicle     vehicle,
                                              const pbuilding    building,
                                              const pobjectcontainer      object,
                                              const pvehicletype  vehicletype );

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

enum tvisibilitytempbuf { vsight, vjamming, vfeatures };

           struct trectangle {
                     int x1, y1, x2, y2;
                     int xsize;
                  };

  typedef class tvirtualvisibilitybuffer*  pvirtualvisibilitybuffer;
  class tvirtualvisibilitybuffer {
        protected:
           char* buf;

        public:

           trectangle innerrect;
           trectangle outerrect;

           char* value ( int x1, int y1, int player, tvisibilitytempbuf offs );
           void init ( void );
  };



  class tcomputeview : public tsearchfields {
                protected:
                      int actview;
                      int player;
                      int mode;
                      int height;

                      boolean       sonar,satellitenview,minenview;
                      int        viewdist;
                      int        jamdist;
                      virtual void       initviewcalculation( int view, int jamming, int sx, int sy, int _mode, int _height   );   // mode: +1 = add view  ;  -1 = remove view
                      virtual void  testfield ( void );

                public:
                      tcomputeview ( void ) { actview = actmap->playerview; };
                 };

  class tcomputevehicleview : public tcomputeview {
                           public:
                               void          init( const pvehicle eht, int _mode  );   // mode: +1 = add view  ;  -1 = remove view );
                           };

  class tcomputebuildingview : public tcomputeview  {
                              pbuilding         building;
                              integer           orgx, orgy, dx;
                           public:
                              void              init( const pbuilding    bld, int _mode );
                           };




  class tsearchattackablevehicles : public tsearchfields {
                public:
                                  boolean   messages;
                                  word      anzahlgegner;
                                  pvehicle  angreifer;
                                  boolean   attackposs;
                                  boolean   kamikaze;
                                  void            init ( const pvehicle eht );
                                  virtual void    testfield ( void );
                                  virtual void    initattacksearch ( void );
                                  virtual void    startsuche ( void );
                                  void            run ( void );
                               };                                             
  class tsearchattackablevehicleskamikaze : public tsearchattackablevehicles {
                public:
                                  void            init ( const pvehicle eht );
                                  virtual void    testfield ( void );        
                                  virtual void    initattacksearch ( void );
                                       };


   class trefuelvehicle : public tsearchfields {
                public:
                       char             mode;
                       pvehicle         actvehicle;
                       word             numberoffields;

                       virtual void     testfield ( void );
                       void             initrefuelling( word xp1, word yp1, char md );
                       virtual void     startsuche ( void );
                    };

   class tputmine : public tsearchfields {
                public:
                       char             mienentyp;
                       boolean          mienenlegen, mienenraeumen;
                       char             numberoffields;
                       virtual void     testfield ( void );
                       void             initpm( char mt, const pvehicle eht );
                       void             run ( void );
              };

    class tprotfzt   {          // ÅberprÅft, welche vehicletypeen neu verfÅgbar sind
                public:
                    void initbuffer( void );
                    void evalbuffer( void );
    
                    char*  buf;
               };




  extern tmoveparams moveparams; 
  extern tmoveview moveview; 
  extern tdashboard dashboard; 



extern void  movement(pvehicle     vehicle, int unitheight = -1);

extern void  computeview( void );


extern void  attack(boolean      kamikaze, int  weapnum = 0);

extern void  moveheight(pvehicle     vehicle);

extern void  calcmovemalus(int          x1,
                           int          y1,
                           int          x2,
                           int          y2,
                           pvehicle     vehicle,
                           shortint     direc,
                           int&         mm1,               // fÅr Spritfuelconsumption
                           int&         mm2 );            //  fÅr movementdecrease


extern int   changeheight(pvehicle     vehicle,
                          char         ch,
                          int        mode);

extern void  fieldreachablerek(integer      x2,
                               integer      y2,
                                const pvehicle     vehicle,
                               byte         mode);

extern void  setspec( pobjecttype obj );
extern void         constructvehicle( pvehicletype tnk );
extern int  object_constructable ( int x, int y, pobjecttype obj );
extern int  object_removeable ( int x, int y, pobjecttype obj );
extern void build_objects_reset( void );
extern void build_vehicles_reset( void );

extern void  refuelvehicle(byte         b);

extern void  legemine(byte         typ);


extern void  putbuildinglevel1(void);

extern void  putbuildinglevel2( const pbuildingtype bld,
                               integer      xp,
                               integer      yp);

extern void  putbuildinglevel3(integer      x,
                               integer      y);


// extern void  nextturn(void);
extern void next_turn ( void );

extern void searchforminablefields ( pvehicle eht );
extern void initweather ( void );
extern void initwindmovement( const pvehicle vehicle );
extern int windbeeline ( int x1, int y1, int x2, int y2 );
extern tselectbuildingguihost    selectbuildinggui;
extern void continuenetworkgame ( void );
extern void addtechnology ( void );
extern void returnresourcenuseforresearch ( const pbuilding bld, int research, int* energy, int* material );
extern void returnresourcenuseforpowerplant ( const pbuilding bld, int prod, int* material, int* fuel );
extern void dissectvehicle ( pvehicle eht );


extern void getpowerplantefficiency ( const pbuilding bld, int* material, int* fuel );

/*
extern int putenergy(pbuilding    bld,    int      need,    byte         mode);
extern int getenergy(pbuilding    bld,    int      need,    byte         mode);
*/

  /*   modes: 0 = energy   ohne abbuchen
              1 = material ohne abbuchen
              2 = fuel     ohne abbuchen

                +4         mit abbuchen                         /
                +8         nur Tributzahlungen kassieren       /  
               +16         plus zurÅckliefern                 <  diese Bits schlie·en sich gegenseitig aus
               +32         usage zurÅckliefern                 \
               +64         tank zurÅckliefern                   \
               */


struct tmininginfo {
         tresources avail[maxminingrange+2];
         int efficiency[maxminingrange+2];
         tresources max[maxminingrange+2];            // soviel BodenschÑtze kînnten in der Entfernung untergebracht werden.
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



class cmousecontrol {
         protected:

            int mousestat;
         public:
            cmousecontrol( void );
            void chkmouse ( void );
            void reset ( void );
};

enum trpl_actions { rpl_attack, rpl_move, rpl_changeheight, rpl_convert, rpl_remobj, rpl_buildobj, rpl_putbuilding, 
                    rpl_removebuilding, rpl_putmine, rpl_removemine, rpl_produceunit, rpl_removeunit, rpl_trainunit, 
                    rpl_reactionfire, rpl_finished, rpl_shareviewchange, rpl_alliancechange, rpl_move2, rpl_buildtnk }; 

extern void         destructbuildinglevel2( int xp, int yp);
extern void         destructbuildinglevel1( int xp, int yp);

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
            treplayguihost gui;
            void execnextreplaymove ( void );
            tmap orgmap;
            pmemorystream stream;
            int removeunit ( int x, int y, int nwid );
            int removeunit ( pvehicle eht, int nwid );
            void wait ( int t = ticker );
            int actplayer;
            void setcursorpos ( int x, int y );
            struct {
               int x;
               int y;
            } lastvisiblecursorpos;

            trpl_actions nextaction;

            void readnextaction ( void );

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
                                  byte         col,
                                  pvehicle &   vehicle,
                                  int          x, 
                                  int          y );
// Generiert eine LEERE vehicle. MIT abfrage, ob dies Mîglich ist, MIT auswerten der Klassen
//  => Nur fÅrs Spiel

extern void testnet ( void );



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
                virtual void searchfield ( int x, int y, int dir );
             public:
                virtual void start ( int x, int y );
                MapNetwork ( void );
                virtual ~MapNetwork();
           };

class ResourceNet : public MapNetwork {
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
                  GetResource ( void );
   };

class PutResource : public StaticResourceNet {
              protected:
                  virtual void checkbuilding ( pbuilding b );
                  virtual void checkvehicle ( pvehicle v ) {};
                  virtual void start ( int x, int y );
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


#endif
