//     $Id: building.h,v 1.6 1999-12-30 20:30:23 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.5  1999/12/28 21:02:42  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.4  1999/12/27 12:59:44  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.3  1999/11/25 22:00:00  mbickel
//      Added weapon information window
//      Added support for primary offscreen frame buffers to graphics engine
//      Restored file time handling for DOS version
//
//     Revision 1.2  1999/11/16 03:41:12  tmwilson
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

/* header  for building.cpp     */                                               

#ifdef karteneditor
#error eigentlich sollte der Karteneditor ohne das building auskommen kînnen !
#endif


#ifndef building_h
#define building_h


#include "gui.h"
#include "controls.h"
#include "gamedlg.h"





#define   delayconst 10
enum tonoff {off,on};      // fÅr scrollbar

enum tcontainermode { mbuilding, mtransport };

enum tunitmode { mnormal, mproduction, mloadintocontainer };


extern void  container ( pvehicle eht, pbuilding bld );

typedef  class ccontainercontrols*  pcontainercontrols;
typedef  class cbuildingcontrols*   pbuildingcontrols;
typedef  class ctransportcontrols*  ptransportcontrols;

extern   int autofill_prodtnk ;
extern   pcontainercontrols         cc;
extern   pbuildingcontrols          cc_b;
extern   ptransportcontrols         cc_t;




class    ccontainercontrols {

            protected:
               class crepairanything {
                   public:
                     int      energycosts, materialcosts, fuelcosts;
                     int      checkto  ( int olddamage, int newdamage, int energycost, int materialcost, int fuelcost, int effizienz );
               };

            public :

               class    crepairunit : public crepairanything {                          // REPAIRUNIT
                  public :
                     virtual int  checkto  (pvehicle eht, char newdamage) = 0;
                     int          repairto (pvehicle eht, char newdamage);
                     int          available ( pvehicle eht );
               };

               class    crefill {
                  public :
                     void     fuel (pvehicle eht, int newfuel);                     // der neue Werte darf durchaus Åber dem Maximum liegen. Eine entsprechende PrÅfung wird durchgefÅhrt.
                     void     material (pvehicle eht, int newmaterial);
                     void     ammunition (pvehicle eht, char weapon, int ammun );
                     void     filleverything ( pvehicle eht );
                     void     emptyeverything ( pvehicle eht );
               } refill;

               class   cmove_unit_in_container {
                   public:
                     int      moveupavail ( pvehicle eht );
                     int      movedownavail ( pvehicle eht, pvehicle into );
                     void     moveup ( pvehicle eht );
                     void     movedown ( pvehicle eht, pvehicle into );
                     pvehicle unittomove;
               } move_unit_in_container;

               ccontainercontrols (void);
               
               virtual char   getactplayer (void) = 0;

               virtual int    movement ( pvehicle eht, int mode = 0 ) = 0;
               virtual int    moveavail ( pvehicle eht ) = 0;


               virtual int    putenergy (int e, int abbuchen = 1 ) = 0;
               virtual int    putmaterial (int m, int abbuchen = 1 ) = 0;
               virtual int    putfuel (int f, int abbuchen = 1) = 0;
               virtual int    getenergy ( int need, int abbuchen ) = 0;
               virtual int    getmaterial ( int need, int abbuchen ) = 0;
               virtual int    getfuel ( int need, int abbuchen ) = 0;

               virtual int    putammunition (int  weapontype, int  ammunition, int abbuchen) = NULL;
               virtual int    getammunition ( int weapontype, int num, int abbuchen, int produceifrequired = 0 ) = 0;
               virtual int    ammotypeavail ( int type ) = 0;

               virtual int    getxpos (void) = 0;
               virtual int    getypos (void) = 0;

               virtual int    getspecfunc ( tcontainermode mode ) = 0;

               virtual pvehicle getloadedunit (int num) = NULL;

               struct { 
                  int height;
                  int movement;
                  int attacked;
                } movementparams;

};
 


class    cbuildingcontrols : public virtual ccontainercontrols {
            private :

               char  getactplayer (void);


               int   getxpos (void);
               int   getypos (void);

               
            public :
               int    putenergy (int e , int abbuchen = 1);
               int    putmaterial (int m, int abbuchen = 1 );
               int    putfuel (int f, int abbuchen = 1 );
               int    getenergy ( int need, int abbuchen );
               int    getmaterial ( int need, int abbuchen );
               int    getfuel ( int need, int abbuchen );
               int    putammunition ( int weapontype, int  ammunition, int abbuchen);
               int    getspecfunc ( tcontainermode mode );
               int    getammunition ( int weapontype, int num, int abbuchen, int produceifrequired = 0 );
               int    ammotypeavail ( int type );

               pbuilding   building;

               class   crepairunitinbuilding : public virtual crepairunit {
                     public:
                       virtual int      checkto  (pvehicle eht, char newdamage);
               } repairunit;

               class    crecycling {                           // RECYCLING
                    public :
                        int      material, energy;
                        void     resourceuse ( pvehicle eht );
                        void     recycle (pvehicle eht);
               } recycling;
               
               class    ctrainunit {
                   public :         
                      int  available  ( pvehicle eht );
                      void trainunit ( pvehicle eht );
               } training;

               class    cproduceunit {                         // PRODUCEUNIT
                  public :
                     int         available ( pvehicletype fzt );
                     pvehicle    produce ( pvehicletype fzt );
                     pvehicle    produce_hypothetically ( pvehicletype fzt );
               } produceunit ;

               class    cdissectunit {
                   public :         
                      int  available  ( pvehicle eht );
                      void dissectunit ( pvehicle eht );
               } dissectunit;

               class    cnetcontrol {
                      public:
                          void setnetmode (  int category, int stat );  // stat  1: setzen    0: lîschen
                          int  getnetmode ( int mode );
                          void emptyeverything ( void );
                          void reset ( void );
               } netcontrol;

               class    crepairbuilding : public crepairanything  {                          
                  public :
                     int      checkto  ( char newdamage);
                     int      repairto ( char newdamage);
                     int      available ( void );
               } repairbuilding;


               class    cproduceammunition {
                        public:
                          int baseenergyusage; 
                          int basematerialusage; 
                          int basefuelusage; 
                          int materialneeded;
                          int energyneeded;
                          int fuelneeded;
                          int checkavail ( int weaptype, int num );
                          void produce ( int weaptype, int num );
                          cproduceammunition ( void );
               } produceammunition;


               void  removevehicle ( pvehicle *peht );
               int    movement ( pvehicle eht,  int mode = 0 );
               int    moveavail ( pvehicle eht );

               pvehicle getloadedunit (int num);


               cbuildingcontrols (void);
               void  init (pbuilding bldng);
};

class    ctransportcontrols : public virtual ccontainercontrols {
            private :

               char  getactplayer (void);


               int   getxpos (void);
               int   getypos (void);


               
            public :
               int    putenergy (int e, int abbuchen = 1 );
               int    putmaterial (int m, int abbuchen = 1 );
               int    putfuel (int f, int abbuchen = 1 );
               int    getenergy ( int need, int abbuchen );
               int    getmaterial ( int need, int abbuchen );
               int    getfuel ( int need, int abbuchen );
               int    putammunition ( int weapontype, int  ammunition, int abbuchen);
               int    getspecfunc ( tcontainermode mode );
               int    getammunition ( int weapontype, int num, int abbuchen, int produceifrequired = 0 );
               int    ammotypeavail ( int type );
               

               pvehicle vehicle;

               class   crepairunitintransport : public virtual crepairunit {
                     public:
                       virtual int      checkto  (pvehicle eht, char newdamage);
               } repairunit;


               void  removevehicle ( pvehicle *peht );
               int    movement ( pvehicle eht,  int mode = 0 );
               int    moveavail ( pvehicle eht );

               pvehicle getloadedunit (int num);

               ctransportcontrols ( void );
               void  init ( pvehicle eht );
};



typedef class ccontainer*   pcontainer;
typedef class ccontainer_b* pcontainer_b;
typedef class ccontainer_t* pcontainer_t;

typedef class generalicon_c*   pgeneralicon_c;
    


    //0 tncguiicon 
class generalicon_c : public tnguiicon {       // fÅr Container //grundlage f¸r jedes einzelne icon
                        pgeneralicon_c next;
                        pgeneralicon_c *first;
                    protected:
                        pcontainer main;
                        virtual pnguiicon nxt      ( void );
                        virtual void      setnxt   ( pnguiicon ts );
                        virtual pnguiicon frst     ( void );
                        virtual void      setfrst  ( pnguiicon ts );

                    public:
                        void setmain ( pcontainer maintemp );
                        generalicon_c ( void );
                        ~generalicon_c ();
                };








//-------------------------------------------------------------------------subwindows
typedef class csubwindow* psubwindow;

                class csubwindow {
                     protected:
                        int lastclickpos;
                        int subwinid;
                        int active;          // 0: öberhauptnicht vorhanden ;   
                                            // 1: existent, aber nicht aktiv;
                                           // 2: existent und aktiv;


                        psubwindow next;
                        psubwindow nextavailable;
                        pcontainer hostcontainer;
                        char name[30];
                        psubwindow *first;
                        psubwindow *firstavailable;

                        int laschx1, laschx2, laschcol;
                        void* laschpic1;
                        void* laschpic2;
                        void paintlasche ( void );
                        int  objpressedbymouse ( int num );

                        struct { 
                                  int x1, y1, x2, y2;
                                  int t1, t2;
                                  int type;
                              } objcoordinates[20];
                        int objnum;

                        virtual void paintobj ( int num, int stat );
                        
                        virtual void checkformouse ( void ) = 0;



                    public:
                        tonlinehelplist helplist;
                        void sethostcontainerchain ( pcontainer cntn );
                        void setlaschenpos ( int x1, int maxwidth );
                        virtual int subwin_available ( void ) ;
                        virtual void checkforkey( tkey ch ) = 0;
                        virtual void display ( void );
                        void mousecheck ( int checkall );
                        void setactive ( void );
                        void paintalllaschen ( int i = 0 );
                        void sethostcontainer ( pcontainer cntn );
                        void init ( void );
                        virtual void unitchanged ( void );
                        void checklaschkey ( tkey taste );
                                
                        // virtual void setinactive( void );

                        virtual void resetammo ( int mode );
                        virtual void resetresources ( int mode );

                        csubwindow ( void );
                        virtual ~csubwindow ( );
                };














class tcontaineronlinemousehelp : public tonlinemousehelp {
              pcontainer hostcontainer; 
           public:
              tonlinehelplist* helplist2;
              virtual void checkforhelp ( void );
              tcontaineronlinemousehelp ( pcontainer host );
      };











class    ccontainer : public virtual ccontainercontrols {
              void*  containerpicture;
              char*  name1;
              char*  name2;
              int mousestat;
            protected :
//-------------------------------------------------------------------------icons


                        //0 cguihostcontainer 
                class   hosticons_c: public tguihost { // basis fÅr icons ->struct mit allen icons
                    public: 
                        void seticonmains ( pcontainer maintemp );
                };


                class repairicon_c : public generalicon_c , public virtual crepairunit {
                    public:
                        virtual int   available    ( void ) ;
                        virtual void  exec         ( void ) ;
                        virtual char* getinfotext  ( void );
                        repairicon_c ( void );
                };


                       //0 tncguiiconmove                  
                class moveicon_c : public generalicon_c {
                    public:
                        virtual int   available    ( void ) ;
                        virtual void  exec         ( void ) ;
                        moveicon_c ( void );
                };
         

                class fill_dialog_icon_c  : public generalicon_c {
                    public:
                        virtual int   available    ( void ) ;
                        fill_dialog_icon_c ( void );
                };

                class fill_icon_c  : public generalicon_c , public crefill {
                    public:
                        virtual void  exec         ( void ) ;
                        fill_icon_c ( void );
                };

                class exit_icon_c  : public generalicon_c {
                    public:
                        virtual int   available    ( void ) ;
                        virtual void  exec         ( void ) ;
                        exit_icon_c ( void );
                };


                class container_icon_c : public generalicon_c {
                        public:
                          virtual int   available    ( void ) ;
                          virtual void  exec         ( void ) ;
                          container_icon_c ( void );
                };

                class cmoveup_icon_c : public generalicon_c, public cmove_unit_in_container { 
                        public:
                          virtual int   available    ( void ) ;
                          virtual void  exec         ( void ) ;
                          cmoveup_icon_c ( void );
                };

                class cunitinformation_icon : public generalicon_c { 
                        public:
                          virtual int   available    ( void ) ;
                          virtual void  exec         ( void ) ;
                          cunitinformation_icon ( void );
                };

                class cmovedown_icon_c : public generalicon_c, public cmove_unit_in_container { 
                        public:
                          virtual int   available    ( void ) ;
                          virtual void  exec         ( void ) ;
                          cmovedown_icon_c ( void );
                };
 
               // auch fÅr "move in"
                class productioncancelicon_cb : public generalicon_c  { 
                    public:
                        virtual int   available    ( void ) ;
                        virtual void  exec         ( void ) ;
                        productioncancelicon_cb    ( void );
                };


//-------------------------------------------------------------------------subwindows

               class  cammunitiontransfer_subwindow : public csubwindow,  public crefill {
                           protected:
                             struct {
                                   int maxnum;    
                                   int orgnum;
                                   int actnum;
                                   int type;
                                   int pos;
                                   int buildnum;
                                   const char* name;
                             } weaps[12];
                             int schieblength;
                             int actdisp[8];
                             void checkformouse ( void );
                             int num;
                             pvehicle eht;
                             void reset ( pvehicle veh = NULL );
                             int  gpres ( int i );
                             void check ( int i );
                             void transfer ( void );
                             void paintobj ( int num, int stat );
                             char* txtptr;
                             int actschieber;
                             int externalloadingactive;
                             virtual int externalloadavailable ( void );
                             virtual void execexternalload ( void );
                           public: 
                             cammunitiontransfer_subwindow ( void );
                             int  subwin_available ( void );
                             void display ( void ) ;
                             void displayvariables ( void ) ;
                             void checkforkey ( tkey taste );
                             void unitchanged ( void );
                             void resetammo ( int mode );
                             void resetresources ( int mode );
               };


                psubwindow allsubwindows[10];
                                
                struct  {
                    int x;
                    int y;
                } mark;

            protected:

                void*    picture[32];
                char     pictgray[32];

                void*    activefield;
                void*    inactivefield;

                void     checkformouse();
                int      getfieldundermouse ( int* x, int* y );

                void     showresources  ( void );
                void     showammo ( void );
                virtual void paintvehicleinfo ( void );

                tcontaineronlinemousehelp* containeronlinemousehelp;

            public :
                virtual void unitchanged( void );
                int      keymode;   // mit tab wird umgeschaltet. 0 : Laderaum ; 1 : Subwin ; 2 : Laschen 
                int      repaintammo;  
                int      repaintresources;

                void     setactunittogray ( void );
                void     buildgraphics();


                psubwindow actsubwindow;
                int  allsubwindownum;
                int end;
                pguihost          oldguihost;
                virtual void     setpictures ( void );

                tunitmode unitmode;  // wird erst im Building-Container benîtigt, aber damit die Icons darauf zugreifen kînnen ist das teil schon hier ...

                void     init ( void *pict, int col, char *name, char *descr);
                void     registersubwindow ( psubwindow subwin );
                void     run (void);
                void     done (void);
                void     movemark (int direction);
                virtual pvehicle getmarkedunit (void) = NULL;
                virtual pvehicletype getmarkedunittype ( void );
                void     displayloading ( int x, int y, int dx = 0, int dy = 0 );    
                void     displayloading (void);    
                ccontainer (void);
                virtual ~ccontainer (void);
};







typedef class cbuildingsubwindow* pbuildingsubwindow;

               class cbuildingsubwindow : public virtual csubwindow 
               {
                        void sethostcontainerchain ( pcontainer_b cntn );
                   protected: 
                        pbuildingsubwindow         nextb;
                        pcontainer_b               hostcontainerb;
                        pbuildingsubwindow         *firstb;
                    public:
                        void sethostcontainer ( pcontainer_b cntn );
                        cbuildingsubwindow ( void );
                        ~cbuildingsubwindow ( );
               };


class    ccontainer_b : public cbuildingcontrols , public ccontainer {

            protected :

                class repairicon_cb : public repairicon_c , public crepairunitinbuilding {
                       virtual int      checkto  (pvehicle eht, char newdamage);
                };

                class fill_dialog_icon_cb  : public fill_dialog_icon_c {
                    public:
                        virtual void  exec         ( void ) ;
                };

                class fill_icon_cb  : public fill_icon_c {
                    public:
                        virtual int   available    ( void ) ;
                };

/*
                class repairicon_c : public generalicon_c , public virtual crepairunit {
                    public:
                        virtual int   available    ( void ) ;
                        virtual void  exec         ( void ) ;
                        virtual char* getinfotext  ( void );
                        repairicon_c ( void );
                };
*/

                     //0 tnbcguiiconrecycling 
                class recyclingicon_cb : public generalicon_c , public crecycling {
                    public:
                        virtual int   available    ( void ) ;
                        virtual void  exec         ( void ) ;
                        virtual char* getinfotext  ( void );
                        recyclingicon_cb ( void );
                };


                class trainuniticon_cb : public generalicon_c , public ctrainunit {
                    public:
                        virtual int   available    ( void ) ;
                        virtual void  exec         ( void ) ;
                        trainuniticon_cb ( void );
                };

                class dissectuniticon_cb : public generalicon_c , public cdissectunit {
                    public:
                        virtual int   available    ( void ) ;
                        virtual void  exec         ( void ) ;
                        dissectuniticon_cb ( void );
                };

                class produceuniticon_cb : public generalicon_c, public cproduceunit {
                    public:
                        virtual int   available    ( void ) ;
                        virtual void  exec         ( void ) ;
                        virtual char* getinfotext  ( void );
                        produceuniticon_cb ( void );
                };

                class takeofficon_cb : public generalicon_c  {
                    public:
                        virtual int   available    ( void ) ;
                        virtual void  exec         ( void ) ;
                        takeofficon_cb             ( void ) ;
                };


                        //0 cguihostcontainerb 
                class   chosticons_cb : public hosticons_c {
                    public: 
                        void init ( int resolutionx, int resolutiony );
                        struct I1 {
                            moveicon_c          movement;
                            recyclingicon_cb    recycling;
                            repairicon_cb       repair;
                            trainuniticon_cb    training; 
                            dissectuniticon_cb  dissect;
                            //fill_dialog_icon_cb filldialog;
                            fill_icon_cb        fill; 
                            produceuniticon_cb  produceunit;
                            productioncancelicon_cb cancel;
                            // takeofficon_cb      takeofficon;
                            exit_icon_c         exit;
                            container_icon_c    contain;
                            cmovedown_icon_c    movedown;
                            cmoveup_icon_c      moveup;
                            cunitinformation_icon unitinformation;
                        } icons;
                } hosticons_cb;







//-------------------------------------------------------------------------subwindows

               class  crepairbuilding_subwindow : public cbuildingsubwindow , public crepairbuilding  {
                           public: 
                             crepairbuilding_subwindow ( void );
                             int  subwin_available ( void );
                             void display ( void ) ;
                             void checkforkey ( tkey taste );
                           protected:
                             void paintobj ( int num, int stat );
                             void checkformouse ( void );
                             int  ndamag;
                             void paintvariables ( void );
               };


               class  cnetcontrol_subwindow : public cbuildingsubwindow , public cnetcontrol {
                           protected:
                             void checkformouse ( void );
                             void objpressed ( int num );
                             void paintobj ( int num, int stat );
                           public: 
                             cnetcontrol_subwindow ( void );
                             int  subwin_available ( void );
                             void display ( void ) ;
                             void checkforkey ( tkey taste );
                             int getstatus ( int num );
               };

               class  cconventionelpowerplant_subwindow : public cbuildingsubwindow  {
                           protected:
                             static int allbuildings;
                             void checkformouse ( void );
                             void setnewpower ( int pwr );
                             void displayvariables ( void );
                             void paintobj ( int num, int stat );

                             int materialcolor, fuelcolor;
                             int power;
                             int gx1, gy1, gx2, gy2;
                           public: 
                             cconventionelpowerplant_subwindow ( void );
                             int  subwin_available ( void );
                             void display ( void ) ;
                             void checkforkey ( tkey taste );
               };

               class  cwindpowerplant_subwindow : public cbuildingsubwindow  {
                           protected:
                             void checkformouse ( void );
                           public: 
                             cwindpowerplant_subwindow ( void );
                             int  subwin_available ( void );
                             void display ( void ) ;
                             void checkforkey ( tkey taste );
                             void resetresources ( int mode );
               };

               class  csolarpowerplant_subwindow : public cbuildingsubwindow /*, public crepairbuilding */ {
                           protected:
                             void checkformouse ( void );
                           public: 
                             csolarpowerplant_subwindow ( void );
                             int  subwin_available ( void );
                             void display ( void ) ;
                             void checkforkey ( tkey taste );
                             void resetresources ( int mode );
               };

               class  cammunitionproduction_subwindow : public cbuildingsubwindow , public cproduceammunition {
                             int toproduce[waffenanzahl];
                             double maxproduceablenum;
                             double grad;
                           protected:
                             int schieblength;
                             void checkformouse ( void );
                             int  pos2num ( int pos );
                             int  num2pos ( int num );
                             void displayusage( void );
                             void checknewval ( int weaptype, int num );
                             void setnewamount (  int weaptype, int num  );
                             void paintobj ( int num, int stat );
                             void produce( void );
                             char* txtptr;
                             int actschieber;

                           public: 
                             cammunitionproduction_subwindow ( void );
                             int  subwin_available ( void );
                             void display ( void ) ;
                             void checkforkey ( tkey taste );
               };


               class  cresourceinfo_subwindow : public cbuildingsubwindow /*, public crepairbuilding */ {
                           protected:
                             void checkformouse ( void );
                             /*
                             struct {
                                struct tres {
                                      union {
                                        int energy, material, fuel;
                                        int resource[3];
                                      };
                                } ;
                                union {
                                   tres avail, tank, plus, use;
                                   tres mode[4];
                                };
                             } resource;
                             */
                             struct {
                                   int r[4][3];
                             } resource;
                             int recalc;

                           public: 
                             cresourceinfo_subwindow ( void );
                             int  subwin_available ( void );
                             int  getvalue ( int x, int y, int mode  );
                             void displayvariables( void );
                             void display ( void ) ;
                             void checkforkey ( tkey taste );
                             void resetammo ( int mode );
                             void resetresources ( int mode );
               };

               class  cresearch_subwindow : public cbuildingsubwindow /*, public crepairbuilding */ {
                           protected:
                             static int allbuildings;
                             int research;    // 1024 ist maximale Forschung
                             void checkformouse ( void );
                             int gx1, gy1, gx2, gy2;
                             int materialcolor, energycolor;
                           public: 
                             cresearch_subwindow ( void );
                             int  subwin_available ( void );
                             void display ( void ) ;
                             void displayvariables ( void );
                             void checkforkey ( tkey taste );
                             void setnewresearch ( int res );
                             void paintobj ( int num, int stat );
               };

               class  cminingstation_subwindow : public cbuildingsubwindow {
                           protected:
                             static int allbuildings;
                             int mode;          // 1: material; 2 : fuel
                             int extraction;    // 1024 ist maximale Production
                             void checkformouse ( void );
                             int gx1, gy1, gx2, gy2;
                             int materialcolor, energycolor, fuelcolor;

                           public: 
                             tmininginfo* mininginfo;
                             cminingstation_subwindow ( void );
                             int  subwin_available ( void );
                             void display ( void ) ;
                             void displayvariables ( void );
                             void checkforkey ( tkey taste );
                             void setnewextraction ( int res );
                             void paintobj ( int num, int stat );
                             ~cminingstation_subwindow();
               };
               class cammunitiontransferb_subwindow : public cammunitiontransfer_subwindow {
                          protected:
                             virtual int externalloadavailable ( void );
                             virtual void execexternalload ( void );
               };

               int    putenergy (int e, int abbuchen = 1 );
               int    putmaterial (int m, int abbuchen = 1 );
               int    putfuel (int f, int abbuchen = 1 );
               int    getenergy ( int need, int abbuchen );
               int    getmaterial ( int need, int abbuchen );
               int    getfuel ( int need, int abbuchen );
               int    putammunition (int  weapontype, int  ammunition, int abbuchen);
               int    getammunition ( int weapontype, int num, int abbuchen, int produceifrequired = 0 );
               pvehicle getloadedunit (int num);
               virtual void paintvehicleinfo ( void );
               virtual void     setpictures ( void );

               pvehicletype getmarkedunittype ( void );
               pvehicletype produceableunits[32];


               struct I2 {
                    crepairbuilding_subwindow             repairbuilding_subwindow;
                    cnetcontrol_subwindow                 netcontrol_subwindow;
                    cconventionelpowerplant_subwindow     conventionelpowerplant_subwindow;
                    cwindpowerplant_subwindow             windpowerplant_subwindow ;
                    csolarpowerplant_subwindow            solarpowerplant_subwindow;
                    cammunitionproduction_subwindow       ammunitionproduction_subwindow;
                    cammunitiontransferb_subwindow        ammunitiontransfer_subwindow;
                    cresourceinfo_subwindow               resourceinfo_subwindow;
                    cresearch_subwindow                   research_subwindow;
                    cminingstation_subwindow              miningstation_subwindow;
               } subwindows;


            public :
                virtual void unitchanged( void );
               void     init (pbuilding bld);

               pvehicle getmarkedunit (void);
               ccontainer_b ( void );
               ~ccontainer_b ( void );

};









/*
typedef class cbuildingsubwindow* pbuildingsubwindow;

               class cbuildingsubwindow : public csubwindow 
               {
                        void sethostcontainerchain ( pcontainer_b cntn );
                   protected: 
                        pbuildingsubwindow         nextb;
                        pcontainer_b               hostcontainerb;
                        static pbuildingsubwindow  firstb;
                    public:
                        void sethostcontainer ( pcontainer_b cntn );
                        cbuildingsubwindow ( void );
                        ~cbuildingsubwindow ( );
               };
*/



class    ccontainer_t : public ctransportcontrols , public ccontainer {

            protected :

                class repairicon_ct : public repairicon_c , public virtual crepairunitintransport {
                       virtual int      checkto  (pvehicle eht, char newdamage);
                };

                class fill_dialog_icon_ct  : public fill_dialog_icon_c {
                    public:
                        virtual void  exec         ( void ) ;
                };

                class fill_icon_ct  : public fill_icon_c {
                    public:
                        virtual int   available    ( void ) ;
                };

                class   chosticons_ct : public hosticons_c {
                    public: 
                        void init ( int resolutionx, int resolutiony );
                        struct I3 {
                            moveicon_c          movement;
                            repairicon_ct       repair;
                            //fill_dialog_icon_ct filldialog;
                            fill_icon_ct        fill; 
                            exit_icon_c         exit;
                            container_icon_c    contain;
                            cmovedown_icon_c    movedown;
                            cmoveup_icon_c      moveup;
                            productioncancelicon_cb cancel;
                            cunitinformation_icon unitinformation;
                         } icons;
                } hosticons_ct;

               pvehicle getloadedunit (int num);

               int    putmaterial (int m, int abbuchen = 1 );
               int    putfuel (int f, int abbuchen = 1 );
               int    getenergy ( int need, int abbuchen );
               int    getmaterial ( int need, int abbuchen );
               int    getfuel ( int need, int abbuchen );
               int    putammunition (int  weapontype, int  ammunition, int abbuchen);
               int    getammunition ( int weapontype, int num, int abbuchen, int produceifrequired = 0 );

               class    ctransportinfo_subwindow : public csubwindow {
                                      public: 
                                        ctransportinfo_subwindow ( void );
                                        int  subwin_available ( void );
                                        void display ( void ) ;
                                        void unitchanged ( void );
                                        void checkforkey ( tkey taste );
                                      protected:
                                        void checkformouse ( void );
                                        void paintvariables ( void );
                              };

               struct I4 {
                  ctransportinfo_subwindow            transportinfo;
                  cammunitiontransfer_subwindow       ammunitiontransfer;
               } subwindows;

            public :
               void     init ( pvehicle eht );

               pvehicle getmarkedunit (void);
               ccontainer_t ( void );
               ~ccontainer_t ( void );

};

extern int recursiondepth;
struct tbuildingparamstack {
        pgeneralicon_c generalicon_c__first;
        pgeneralicon_c generalicon_c__firstguiicon;
        psubwindow csubwindow_first;
        psubwindow csubwindow_firstavailable;
        pbuildingsubwindow cbuildingsubwindow_firstb;

        char* name;
        pbuilding bld;
        pvehicle eht;
   } ;

extern tbuildingparamstack buildingparamstack[];


#endif
