//     $Id: spfst.h,v 1.3 1999-11-22 18:28:02 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  1999/11/16 03:42:35  tmwilson
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

#ifndef spfst_h
  #define spfst_h

  #include "keybp.h"
  #include "newfont.h"
  #include "basegfx.h"
  #include "mousehnd.h"


   struct ffonts {          
               pfont        smallarial;
               pfont        smallsystem;
               pfont        large;
               pfont        arial8; 
               pfont        guifont;
               pfont        guicolfont;
               pfont        monogui;
            }; 

   struct tstreet { 
                struct { 
                                               byte         page; 
                                               pointer      position; 
                                            } mineposition[8]; 
             } ; 

   struct tview {                  
               void*     nv8;
               void*     va8;
               void*     fog8;
               #ifndef HEXAGON
               void*     nv4[4];
               void*     va4[4];
               void*     fog4[4];
               void*     viereck[256];
               #endif
            }; 




   struct tstpunkt { 
             word         x, y; 
          }; 
   struct tstrecke {
                 tstpunkt       field[31]; 
                 byte         tiefe; 
                 int          fuelremaining;
                 int          distance;
              }; 
   struct tmoveparams { 
                        unsigned char         movestatus;       /*  Folgende Modi sind definiert : 
                                                                             0:  garnichts, standard
                                                                             1:  movement l1
                                                                             2:  movement l2
                                                                             10: angriff
                                                                             11: movement l1 mit hîhe-wechseln
                                                                             12: movement l2 mit hîhe-wechseln
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
                        integer      movedist; 
                        tstrecke     movepath; 
                        pvehicle     vehicletomove; 
                        byte         newheight; 
                        byte         oldheight; 
                        char         heightdir; 
                        pbuildingtype buildingtobuild;   /*  nur bei movestatus = 111  */ 
                        int          movespeed;
                        int          uheight;
                     }; 



   class tweapdist { 
                  char         data[7][256];        /* mg,bomb,gmissile,amissile,torpedo,cannon,cruise missile  */ 
                public:
                  void loaddata ( void ) ;
                 // byte getweapstrength2 (word typ, word pos, byte mindist, byte maxdist );
                 // byte getweapstrength  (word typ, byte pos, byte mindist, byte maxdist );
                  char getweapstrength ( const SingleWeapon* weap, int dist =-1, int attacker_height =-1, int defender_height = -1, int reldiff = -1 );
               }; 

   typedef class tweapdist* pweapdist ;

   struct tattackweap { 
                    byte         count; 
                    word         strength[8]; 
                    byte         num[8]; 
                    word         typ[8];
                 }; 

   typedef struct tattackweap* pattackweap ;

   class tcursor { 
           public:
              int          posx, posy;
              boolean      an; 
              byte         color; 

              int  gotoxy (int x, int y, int disp = 1 );     // result: 0: map wurde nicht neu angezeigt     1: map wurde neu angezeigt
              void show ( void );
              void hide ( void );
              void setcolor ( byte col );
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
              pointer      backgrnd; 
              virtual void checksize ( void );
   };

  class   tsearchfields {
                public:
                    int         startx, starty;
                    boolean     abbruch;
                    word        maxdistance, mindistance;
                    int         xp, yp;
                    word        dist;
                    tsearchfields ( void );
                    virtual void testfield ( void ) = 0;
                    void initsuche ( int  sx, int  sy, word max, word min );
                    virtual void startsuche ( void );
                    void done ( void );
                 };


  extern tcursor cursor; 
  extern tview view; 

 extern pmap actmap; 

  extern int  terraintypenum, vehicletypenum, buildingtypenum, technologynum, objecttypenum;
  extern int guiiconnum;



  extern boolean godview, tempsvisible; 

  extern ffonts schriften; 
  extern tstreet streets; 
  extern int lasttick;   /*  fÅr paintvehicleinfo  */ 

  extern pweapdist weapdist; 


  /*  bildschirmanzeige mit cursor  */ 

extern void  movecursor(tkey         ch);

extern void  displaymap(void);


  /*  zugriffe auf map und andere strukturen  */ 

extern pfield getactfield(void);

#pragma intrinsic ( getfield );
extern pfield getfield(int          x,
                     int          y);

extern pfield getbuildingfield( const pbuilding    bld,
                              shortint     x,
                              shortint     y);

extern void  getbuildingfieldcoordinates( const pbuilding    bld,
                                         shortint     x,
                                         shortint     y,
                                         int     &    xx,
                                         int     &    yy);

extern word  getxpos(void);

extern word  getypos(void);

extern boolean fieldvisiblenow( const pfield        pe, int player = actmap->actplayer );

extern byte  getdiplomaticstatus(byte         b);

extern byte  getdiplomaticstatus2(byte         c,
                                  byte         b);

extern void  removevehicle(pvehicle *   vehicle);

extern void  generatevehicle_ka(pvehicletype fztyp,
                             byte         col,
                             pvehicle &   vehicle);
// Die Einheit wird automatisch vollgefÅllt.
//  => Nur fÅr Karteneditor


extern void         generate_vehicle(pvehicletype fztyp,
                             byte         col,
                             pvehicle &   vehicle);


extern void  putbuilding(int          x,
                         int          y,
                         int          color,
                         pbuildingtype buildingtyp,
                         int          compl,
                         int          ignoreunits = 0 ); // fÅr Kartened

extern void  putbuilding2(integer      x,
                          integer      y,
                          byte         color,
                          pbuildingtype buildingtyp);  // fÅr Spiel


extern void  removebuilding(pbuilding *  bld);


/*  hilfsfunktionen zum handhaben des mapes  */ 

extern void  calculateallobjects(void);

extern void  calculateobject(integer      x,
                              integer      y,
                              boolean      mof,
                              pobjecttype obj);

extern void  generatemap( const pwterraintype bt,
                               int          xsize,
                               int          ysize);

extern void  putstreets2( int          x1,
                          int          y1,
                          int          x2,
                          int          y2,
                          pobjecttype obj );

extern void  getnextfield(int &    x,
                          int &    y,
                          byte         direc);

extern void  getnextfielddir(int &    x,
                             int &    y,
                             byte         direc,
                             byte         sdir);

extern int   getdirection(    int      x1,
                              int      y1,
                              int      x2,
                              int      y2);

extern int resizemap( int top, int bottom, int left, int right );

extern void  cleartemps(byte         b);

extern void  clearfahrspuren(void);

extern void  initmap(void);

  /*  vehicle  */ 

extern pattackweap attackpossible( const pvehicle     angreifer,
                            integer      x,
                            integer      y);

extern boolean attackpossible2u( const pvehicle     angreifer,
                                 const pvehicle     verteidiger);      // Entfernung wird nicht berÅcksichtigt !!

extern boolean attackpossible28( const pvehicle     angreifer,
                                 const pvehicle     verteidiger);       // Als Entfernung wird 8 angenommen

extern boolean attackpossible2n( const pvehicle     angreifer,
                                 const pvehicle     verteidiger);       // Als Entfernung wird die tatsÑchliche angenommen

extern pattackweap   attackpossible3u( const pvehicle     angreifer,
                                       const pvehicle     verteidiger);
extern pattackweap   attackpossible38( const pvehicle     angreifer,
                                       const pvehicle     verteidiger);
extern pattackweap   attackpossible3n( const pvehicle     angreifer,
                                       const pvehicle     verteidiger);

extern boolean vehicleplattfahrbar( const pvehicle     vehicle,
                                    const pfield        field);

extern byte  fieldaccessible( const pfield        field,
                            const pvehicle     vehicle,
                            int  uheight = -1 );

extern boolean weapexist( const pvehicle     eht);

  /*  sonstiges  */ 


extern pointer getmineadress( byte         num );

extern void         initspfst( int x = 10, int y = 20 );

extern void generatespfdspaces();

extern void         movevehiclexy(pvehicle     eht,
                           integer      x1,
                           integer      y1,
                           integer      x2,
                           integer      y2);

extern void         movevehicle(integer      x1,
                         integer      y1,
                         integer      x2,
                         integer      y2,
                         pvehicle     eht);
extern void         checkfieldsformouse ( void );

extern void  checkunitsforremoval ( void );
extern void checkobjectsforremoval ( void );

extern int          getmaxwindspeedforunit ( const pvehicle eht );
extern int          getwindheightforunit   ( const pvehicle eht );
extern void         resetbuildingpicturepointers ( pbuilding bld );
extern void         resetallbuildingpicturepointers ( void );

extern int          terrainaccessible (  const pfield        field, const pvehicle     vehicle, int uheight = -1 );
extern int          terrainaccessible2 ( const pfield        field, const pvehicle     vehicle, int uheight = -1 );  
               /* same as terrainaccessible, but it returns why the unit cannot drive onto the terrain
                  returns:  -1   very deep water required to submerge deep
                            -2   deep water required to submerge 
                            -3   unit cannot drive onto terrain
               */

// extern tdisplaymovingunit displaymovingunit;

extern int getmapposx ( void );  // return the screencoordinates of the upper left position of the displayed map
extern int getmapposy ( void );

class tgeneraldisplaymapbase {
           protected:
             struct {
                   struct {
                      int x1, y1, x2, y2;
                   } disp;
                   int     numberoffieldsx;
                   int     numberoffieldsy;
                   int     orgnumberoffieldsx;
                   int     orgnumberoffieldsy;
                   void*   vfbadress;
                   int     vfbheight;
                   int     vfbwidth;
                 } dispmapdata;

              struct {
                       void*   address;
                       tgraphmodeparameters parameters;
              } vfb;

           public:
              tmouserect invmousewindow;
              virtual int getfieldsizex ( void ) = 0;
              virtual int getfieldsizey ( void ) = 0;
              virtual int getfielddistx ( void ) = 0; 
              virtual int getfielddisty ( void ) = 0;
              virtual int getfieldposx ( int x, int y ) = 0; 
              virtual int getfieldposy ( int x, int y ) = 0;

              virtual void pnt_terrain ( void ) = 0;
              virtual void pnt_main ( void ) = 0;
              virtual void cp_buf ( void ) = 0;
              void setmouseinvisible ( void );
              void restoremouse ( void );
              tgeneraldisplaymapbase ( void );
};

#ifndef FREEMAPZOOM

class tgeneraldisplaymap : public tgeneraldisplaymapbase {
      protected:
         #ifndef HEXAGON
          char**     viereck; 
         #endif

          void putdirecpict ( int xp, int yp,  const void* ptr );
          void pnt_terrain_rect ( void );

          void clearvf ( void );
          virtual void displayadditionalunits ( int height );

      public:
          void calcviereck(void);
          int playerview;
          virtual void init ( int xs, int ys );

          virtual void pnt_terrain ( void );
          virtual void pnt_main ( void );

          virtual int  getscreenxsize( int target = 0 );   // since the screen sizes for the mapeditor and the game may be different target = 1 return the maximum of both 
          virtual int  getscreenysize( int target = 0 );
          virtual int getfieldsizex ( void ) { return fieldsizex; };
          virtual int getfieldsizey ( void ) { return fieldsizey; };
          virtual int getfielddistx ( void ) { return fielddistx; };
          virtual int getfielddisty ( void ) { return fielddisty; };
          virtual int getfieldposx ( int x, int y ) ; 
          virtual int getfieldposy ( int x, int y ) ;
      };



class tdisplaymap : public tgeneraldisplaymap {
        protected:
          struct {
                    pvehicle eht;
                    int xpos, ypos;
                    int dx,   dy;
                    int hgt;
                 } displaymovingunit ;

          struct {
             #ifndef HEXAGON
              int*    spacingbuf;
              int*    direcpictbuf;
             #endif
              int*    screenmaskbuf;
          } displaybuffer;
          int resolutionx;
          int resolutiony;

          virtual void displayadditionalunits ( int height );

          void generate_map_mask ( int* sze );

          tgraphmodeparameters rgmp;
          int game_x;
          int maped_x;

          virtual void gnt_terrain ( void );

       public:
          void* getbufptr( void ) { return dispmapdata.vfbadress; };
          virtual void init ( int xs, int ys );
          void setxsizes ( int _game_x, int _maped_x );
         #ifndef HEXAGON
          void generatespfdspaces ( void );
          void generatespfdptrs ( int xp, int yp );
         #endif

          void setup_map_mask ( void );


          virtual void pnt_terrain ( void );
          virtual void cp_buf ( void );

          void  movevehicle(integer x1,  integer y1,  integer x2,  integer y2,  pvehicle eht);
          void  deletevehicle ( void ); 

          void resetmovement ( void );
          virtual int  getscreenxsize( int target = 0 );
     } ;

#else

class tgeneraldisplaymap : public tgeneraldisplaymapbase {
      protected:
          int zoom;

          void putdirecpict ( int xp, int yp,  const void* ptr );
          void pnt_terrain_rect ( void );

          struct {
             int xsize, ysize;
          } window;

          virtual void displayadditionalunits ( int height );

          virtual void _init ( int xs, int ys );

      public:
          virtual void init ( int xs, int ys );
          int playerview;

          virtual void pnt_terrain ( void );
          virtual void pnt_main ( void );
          virtual void setnewsize ( int _zoom );

          virtual int  getscreenxsize( int target = 0 );   // since the screen sizes for the mapeditor and the game may be different target = 1 return the maximum of both 
          virtual int  getscreenysize( int target = 0 );
          virtual int getfieldsizex ( void );
          virtual int getfieldsizey ( void );
          virtual int getfielddistx ( void );
          virtual int getfielddisty ( void );
      };



class tdisplaymap : public tgeneraldisplaymap {
         tgraphmodeparameters oldparameters;
        protected:
          struct {
                    pvehicle eht;
                    int xpos, ypos;
                    int dx,   dy;
                    int hgt;
                 } displaymovingunit ;

          int windowx1, windowy1;

          virtual void displayadditionalunits ( int height );

          void generate_map_mask ( int* sze );

          tgraphmodeparameters rgmp;

          int* copybufsteps;
          int* copybufstepwidth;
          int vfbwidthused;
          void calcdisplaycache( void );

       public:
          virtual void init ( int x1, int y1, int x2, int y2 );
          virtual void setnewsize ( int _zoom );


          virtual void cp_buf ( void );

          void  movevehicle(integer x1,  integer y1,  integer x2,  integer y2,  pvehicle eht);
          void  deletevehicle ( void ); 

          void resetmovement ( void );

          virtual int getfieldposx ( int x, int y ) ; 
          virtual int getfieldposy ( int x, int y ) ;
          tdisplaymap ( void );
     } ;
#endif







extern tdisplaymap idisplaymap;
extern int showresources;
extern void checkplayernames ( void );

extern int   getfieldundermouse ( int* x, int* y );

extern int getcrc ( const pvehicletype fzt );
extern int getcrc ( const ptechnology tech );
extern int getcrc ( const pobjecttype obj );
extern int getcrc ( const pterraintype bdn );
extern int getcrc ( const pbuildingtype bld );

extern void writemaptopcx ( void );

class tlockdispspfld {
      public:
        tlockdispspfld ( void );
        ~tlockdispspfld ();
      };

extern int lockdisplaymap;

extern int beeline ( int x1, int y1, int x2, int y2 );
extern void smooth ( int what );
extern void  stu_height ( pvehicle vehicle );


extern pterraintype getterraintype_forid ( int id, int crccheck = 1 );
extern pobjecttype getobjecttype_forid ( int id, int crccheck = 1 );
extern pvehicletype getvehicletype_forid ( int id, int crccheck = 1 );
extern pbuildingtype getbuildingtype_forid ( int id, int crccheck = 1 );
extern ptechnology gettechnology_forid ( int id, int crccheck = 1 );

extern pterraintype getterraintype_forpos ( int pos, int crccheck = 1 );
extern pobjecttype getobjecttype_forpos ( int pos, int crccheck = 1 );
extern pvehicletype getvehicletype_forpos ( int pos, int crccheck = 1 );
extern pbuildingtype getbuildingtype_forpos ( int pos, int crccheck = 1 );
extern ptechnology gettechnology_forpos ( int pos, int crccheck = 1 );

extern void addterraintype ( pterraintype bdt );
extern void addobjecttype ( pobjecttype obj );
extern void addvehicletype ( pvehicletype vhcl );
extern void addbuildingtype ( pbuildingtype bld );
extern void addtechnology ( ptechnology tech );


typedef dynamic_array<pvehicletype> VehicleTypeVector;
extern VehicleTypeVector& getvehicletypevector ( void );

typedef dynamic_array<pterraintype> TerrainTypeVector;
extern TerrainTypeVector& getterraintypevector ( void );

typedef dynamic_array<pbuildingtype> BuildingTypeVector;
extern BuildingTypeVector& getbuildingtypevector ( void );

typedef dynamic_array<pobjecttype> ObjectTypeVector;
extern ObjectTypeVector& getobjecttypevector ( void );


class tpaintmapborder {
          protected:
            struct {
               int x1, y1, x2, y2;
               int initialized;
            } rectangleborder;
          public:
            virtual void paintborder ( int x, int y ) = 0;
            virtual void paint ( int resavebackground = 0 ) = 0;
            virtual int getlastpaintmode ( void ) = 0;
            virtual void setrectangleborderpos ( int x1, int y1, int x2, int y2 ) {
               rectangleborder.x1 = x1;
               rectangleborder.x2 = x2;
               rectangleborder.y1 = y1;
               rectangleborder.y2 = y2;
               rectangleborder.initialized = 1;
            };
            tpaintmapborder ( void ) {
               rectangleborder.initialized = 0;
            };
      };

extern tpaintmapborder* mapborderpainter;
extern void swapbuildings ( pbuilding orgbuilding, pbuilding building );
// extern void swapvehicles ( pvehicle orgvehicle, pvehicle transport );

extern void checkformousescrolling ( void );

class tmousescrollproc : public tsubmousehandler {
         public:
           void mouseaction ( void );
};
extern tmousescrollproc mousescrollproc ;
extern const int mousehotspots[9][2];

#ifdef FREEMAPZOOM
class ZoomLevel {
         int zoom;
         int queried;
       public:
         int getzoomlevel ( void );
         void setzoomlevel ( int newzoom );
         int getmaxzoom( void );
         int getminzoom( void );
         ZoomLevel ( void );
      };
extern ZoomLevel zoomlevel;
#endif


class tdrawline8 : public tdrawline {
         public: 
           void start ( int x1, int y1, int x2, int y2 );
           virtual void putpix ( int x, int y );
           virtual void putpix8 ( int x, int y ) = 0;
       };


#ifdef _NOASM_
 int  rol ( int valuetorol, int rolwidth );
 void setvisibility ( word* visi, int valtoset, int actplayer );
#endif

#endif


