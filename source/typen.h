//     $Id: typen.h,v 1.6 1999-12-07 22:02:10 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.5  1999/11/25 22:00:15  mbickel
//      Added weapon information window
//      Added support for primary offscreen frame buffers to graphics engine
//      Restored file time handling for DOS version
//
//     Revision 1.4  1999/11/23 21:07:41  mbickel
//      Many small bugfixes
//
//     Revision 1.3  1999/11/16 17:04:18  mbickel
//     Made ASC compilable for DOS again :-)
//     Merged all the bug fixes in that I did last week
//
//     Revision 1.2  1999/11/16 03:42:45  tmwilson
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

#ifndef typen_h                                                                            
#define typen_h
/* Unit header for: TYPEN.C -- Made by TPTC - Translate Pascal to C */

#include <time.h>
#include "global.h"
#include "tpascal.inc"
#include "misc.h"



   #define maxint 0x7ffffffe
   #define minint -0x7ffffffe

   #define waffenanzahl 8
   #define cwettertypennum 6
   #define cmovemalitypenum 9
   #define ceventactionnum 20
   #define ceventtriggernum 20
   #define cbuildingfunctionnum 17
   #define choehenstufennum 8
   #define cvehiclefunctionsnum 25
   #define maxbuildingpicnum 8
   #define cbodenartennum 33
   #define cnetcontrolnum 12

   #define gameparameternum 8
   #define maxgameparameternum 8

   #define maxobjectonfieldnum 16

   #define maxunitexperience 15


   #define maxloadableunits 27  // Mehr vehicle dÅrfen nicht in einen Transporter rein
  



   class tterrainbits {
            #ifdef converter
             public:
            #endif
               int terrain1;
               int terrain2;
             public:
               tterrainbits ( int i = 0 ) 
               { 
                  set ( i ); 
               };

               tterrainbits ( int i , int j ) 
               { 
                  terrain1 = i; 
                  terrain2 = j; 
               };

               tterrainbits ( tterrainbits &bts ) 
               { 
                  terrain1 = bts.terrain1; 
                  terrain2 = bts.terrain2; 
               };

               void set ( int i = 0, int j = 0 ) { terrain1 = i; terrain2 = j; };
               int toand ( tterrainbits bts );

               int existall ( tterrainbits bdt )
               {
                  return  ((terrain1 & bdt.terrain1) == bdt.terrain1) &&
                          ((terrain2 & bdt.terrain2) == bdt.terrain2);
               };

               tterrainbits& operator|= ( tterrainbits tb ) 
               { 
                  terrain1 |= tb.terrain1; 
                  terrain2 |= tb.terrain2; 
                  return *this;
               };

               tterrainbits& operator&= ( tterrainbits tb ) 
               { 
                  terrain1 &= tb.terrain1; 
                  terrain2 &= tb.terrain2; 
                  return *this;
               };

               tterrainbits& operator^= ( tterrainbits tb ) 
               { 
                  terrain1 ^= tb.terrain1; 
                  terrain2 ^= tb.terrain2; 
                  return *this;
               };

               friend tterrainbits& operator~ ( tterrainbits &tb );

               int getcrc ( void ) {
                  return crc32buf ( &terrain1, 2 * sizeof ( int ));
               }

           };

               extern tterrainbits& operator~ ( tterrainbits &tb );
               extern tterrainbits& operator| ( tterrainbits tb2, tterrainbits tb3 ) ;
               extern int operator& ( tterrainbits tb2, tterrainbits tb3 ) ;
               extern tterrainbits& operator^ ( tterrainbits tb2, tterrainbits tb3 ) ;

   class tterrainaccess {
           public:
               tterrainaccess ( void ) ;
               tterrainbits  terrain;      /*  BM     befahrbare terrain: z.B. Schiene, Wasser, Wald, ...  ; es mu· lediglich eins gesetzt sein */
               tterrainbits  terrainreq;   /*  BM     diese Bits MöSSEN ALLE in gesetzt sein */
               tterrainbits  terrainnot;   /*  BM     sobald eines dieser Bits gesetzt ist, kann die vehicle NICHT auf das field fahren  */
               tterrainbits  terrainkill;  /* falls das aktuelle field nicht befahrbar ist, und bei field->typ->art eine dieser Bits gesetzt ist, verschwindet die vehicle */
               int dummy[10];
               int accessible ( tterrainbits bts );
               int getcrc ( void ) {
                    return terrain.getcrc() + terrainreq.getcrc()*7 + terrainnot.getcrc()*97 + terrainkill.getcrc()*997;  
               };
   };


typedef tterrainaccess *pterrainaccess;

   union tgametime {
              struct { signed short move, turn; }a ;
              int abstime;
         };



   typedef word tthreatvar; 

   struct tweapon {
             word         typ;            /*  BM      <= CWaffentypen  */
             unsigned char         targ;           /*  BM      <= CHoehenstufen  */
             unsigned char         sourceheight;   /*  BM  "  */
             Word         maxdistance;
             Word         mindistance;
             unsigned char         count;
             char         maxstrength;    // Wenn der Waffentyp == Mine ist, dann ist hier die MinenstÑrke als Produkt mit der Bassi 64 abgelegt.
             char         minstrength;
          } ;


//  #if sizeof ( tweapon ) != 11 error ( "invalid compiler options; enable packing !" )
   


   struct tweapons { 
               unsigned char         weaponcount; 
               tweapon      waffe[8]; 
            }; 


   struct tpk { 
          Word         energy; 
          Word         material; 
          };


   struct tcrc {
            int id;
            int crc;
         };
   typedef struct tcrc *pcrc;

   typedef struct tvehicle* pvehicle ;
   typedef struct tvehicletype* pvehicletype ;


   typedef word tmunition[waffenanzahl];
   typedef tmunition* pmunition ;


   struct tclassbound { 
                   word         weapstrength[waffenanzahl]; 
                   word         dummy2;
                   word         armor; 
                   word         techlevel;             //  Techlevel ist eine ALTERNATIVE zu ( techrequired und envetrequired )
                   word         techrequired[4];
                   char         eventrequired;
                   int          vehiclefunctions;
                   char         dummy;
                 };


   typedef class tobjecttype* pobjecttype;

   struct tbuildrange {
               int from;
               int to;
          };
   typedef tbuildrange* pbuildrange;


            struct SingleWeapon {
               int          typ;            /*  BM      <= CWaffentypen  */
               int          targ;           /*  BM      <= CHoehenstufen  */
               int          sourceheight;   /*  BM  "  */
               int          maxdistance;
               int          mindistance;
               int          count;
               int          maxstrength;    // Wenn der Waffentyp == Mine ist, dann ist hier die MinenstÑrke als Produkt mit der Bassi 64 abgelegt.
               int          minstrength;
               int          efficiency[13]; // floating and driving are the same ; 0-5 is lower ; 6 is same height ; 7-12 is higher
               int          reserved[10];
            };

  struct UnitWeapon {
            int count;
            SingleWeapon weapon[16];
            int reserved[10];
         };

  class tvehicletype {   /*  vehicleart: z.B. Schwere Fu·truppe  */
                 public:
                    char*        name;          /* z.B. Exterminator  */
                    char*        description;   /* z.B. Jagdbomber    */
                    char*        infotext;      /* optional, kann sehr ausfÅhrlich sein. Empfehlenswert Åber eine Textdatei einzulesen */
                    tweapons     oldattack;        /*  AngriffsstÑrke der einzelnen Waffen  */
                    tpk          production;    /*  Produktionskosten der vehicle  */
                    Word         armor; 
                    Pointer      picture[8];    /*  0¯  ,  45¯   */
                    unsigned char         height;        /*  BM  Besteht die Mîglichkeit zum Hîhenwechseln  */
                    word         researchid;    // inzwischen ÅberflÅssig, oder ?
                    int          _terrain;    /*  BM     befahrbare terrain: z.B. Schiene, Wasser, Wald, ...  */
                    int          _terrainreq; /*  BM     diese Bits MöSSEN in ( field->typ->art & terrain ) gesetzt sein */
                    int          _terrainkill;  /* falls das aktuelle field nicht befahrbar ist, und bei field->typ->art eine dieser Bits gesetzt ist, verschwindet die vehicle */
                    unsigned char         steigung;      /*  max. befahrbare Hîhendifferenz zwischen 2 fieldern  */
                    unsigned char         jamming;      /*  StÑrke der Stîrstrahlen  */
                    Word         view;         /*  viewweite  */
                    char         wait;        /*  Kann vehicle nach movement sofort schie·en ?  */
                    char         dummy2;
                    Word         loadcapacity;      /*  Transportmîglichkeiten  */
                    word         maxunitweight; /*  maximales Gewicht einer zu ladenden vehicle */
                    char         loadcapability;     /*  BM     CHoehenStufen   die zu ladende vehicle mu· sich auf einer dieser Hîhenstufen befinden */
                    char         loadcapabilityreq;  /*  eine vehicle, die geladen werden soll, mu· auf eine diese Hîhenstufen kommen kînnen */
                    char         loadcapabilitynot;  /*  eine vehicle, die auf eine dieser Hîhenstufen kann, darf NICHT geladen werden. Beispiel: Flugzeuge in Transportflieger */
                    Word         id; 
                    int      tank; 
                    Word         fuelconsumption; 
                    int      energy; 
                    int      material; 
                    int      functions;
                    char         movement[8];      /*  max. movementsstrecke  */
                    char         movemalustyp;     /*  wenn ein Bodentyp mehrere Movemali fÅr unterschiedliche vehiclearten, wird dieser genommen.  <= cmovemalitypes */
                    tthreatvar   generalthreatvalue;   /*  Wird von ArtInt benîtigt, au·erhalb keine Bedeutung  */ 
                    tthreatvar   threatvalue[8];       /*  dito                                                 */
                    char         classnum;         /* Anzahl der Klassen, max 8, min 0 ;  Der EINZIGE Unterschied zwischen 0 und 1 ist der NAME ! */
                    char*        classnames[8];    /* Name der einzelnen Klassen */
                    tclassbound  classbound[8];    /* untergrenze (minimum), die zum erreichen dieser Klasse notwendig ist, classbound[0] gilt fÅr vehicletype allgemein*/
                    char         maxwindspeedonwater;
                    char         digrange;        // Radius, um den nach bodenschÑtzen gesucht wird. 
                    int          initiative;      // 0 ist ausgeglichen // 256 ist verdoppelung
                    int          _terrainnot;    /*  BM     sobald eines dieser Bits gesetzt ist, kann die vehicle NICHT auf das field fahren  */
                    int          _terrainreq1;  // wie terrainreq, es braucht aber nur 1 bit gesetzt zu sein
                    int          objectsbuildablenum;
                    int*         objectsbuildableid;

                    int          weight;           // basic weight, without fuel etc. 
                    pterrainaccess terrainaccess;
                    int          bipicture;
                    int          vehiclesbuildablenum;
                    int*         vehiclesbuildableid;

                    void*        buildicon;
                    int          buildingsbuildablenum;
                    pbuildrange  buildingsbuildable;
                    UnitWeapon*  weapons;
                    int          dummy[2];
                    int maxweight ( void );     // max. weight including fuel and material
                    int maxsize   ( void );     // without fuel and material
                    int vehicleloadable ( pvehicletype fzt );
                 }; 

  typedef class tmap*  pmap;

typedef class  tbuildingtype* pbuildingtype;

/*  
   typedef word tweapstrength[8]; 
   typedef tweapstrength* pweapstrength ;
*/
   class tvehicle { /*** Bei énderungen unbedingt Save/LoadGame und Konstruktor korrigieren !!! ***/ 
               public:
                 pvehicletype typ;          /*  vehicleart: z.B. Schwere Fu·truppe  */
                 unsigned char         color; 
                 unsigned char         damage; 
		 tmunition munition;
                 int      fuel; 
                 int*         ammo; 
                 int          evenmoredummy[3];
                 int*         weapstrength;
                 int          moredummy[3];
                 Word         dummy;     /*  Laderaum, der momentan gebraucht wird  */
                 tvehicle*    loading[32]; 
                 unsigned char         experience;    // 0 .. 15 
                 boolean      attacked; 
                 unsigned char         height;       /* BM */   /*  aktuelle Hîhe: z.B. Hochfliegend  */
                 char         movement;     /*  Åbriggebliebene movement fÅr diese Runde  */
                 unsigned char         direction;    /*  Blickrichtung  */
                 Integer      xpos, ypos;   /*  Position auf map  */
                 int      material;     /*  aktuelle loading an Material und  */
                 int      energy;       /*  energy  */
                 pvehicle     next;
                 pvehicle     prev;         /*  fÅr lineare Liste der vehicle */
                 
                 short          cmpchecked;   /*  fÅr Computerintelligenz  */ 

                 tthreatvar   completethreatvaluesurr; 
                 tthreatvar   completethreatvalue;   /*  Wird von ArtInt benîtigt, au·erhalb keine Bedeutung  */ 
                 tthreatvar   threatvalue[8]; 
                 int          threats;   /* BM  => CCA_Threats  */ 
                 word         order; 
                 int      connection; 
                 unsigned char         klasse;
                 word         armor; 
                 int      networkid; 
                 char*        name;
                 int          functions;
                 char         reactionfire;     // BM   ; gibt an, gegen welche Spieler die vehicle noch reactionfiren kann.
                 char         reactionfire_active;
                 int          generatoractive;

                 int enablereactionfire( void );
                 int disablereactionfire ( void );
                 int weight( void );   // weight of unit including cargo, fuel and material
                 int cargo ( void );   // return weight of all loaded units
                 int getmaxfuelforweight ( void );       
                 int getmaxmaterialforweight ( void );
                 int freeweight ( int what = 0 );      // what: 0 = cargo ; 1 = material/fuel
                 int size ( void );
                 void nextturn( void );
                 void repairunit ( pvehicle vehicle, int maxrepair = 100 );
                 void constructvehicle ( pvehicletype tnk, int x, int y );      // current cursor position will be used
                 int  vehicleconstructable ( pvehicletype tnk, int x, int y );
                 void resetmovement( void );
                 void putimage ( int x, int y );
                 int  vehicleloadable ( pvehicle vehicle, int uheight = -1 );
                 void setnewposition ( int x, int y );
                 void setup_classparams_after_generation ( void );
                 void convert ( int col );
                 void setpower( int status );
                 void addview ( void );
                 void removeview ( void );

                 int buildingconstructable ( pbuildingtype bld );

                 // int attackpossible ( int x, int y );
                 int getstrongestweapon( int aheight, int distance );

                 int searchstackforfreeweight( pvehicle eht, int what ); // what: 0=cargo ; 1=material/fuel
                                                                         // should not be called except from freeweight
                 tvehicle ( void );
                 tvehicle ( pvehicle src, pmap actmap ); // if actmap == NULL  ==> unit will not be chained
                 void clone ( pvehicle src, pmap actmap ); // if actmap == NULL  ==> unit will not be chained
                ~tvehicle ( ); 
               private:
                 void init ( void );
              }; 

   typedef pvehicle (tloading[32]);
   typedef tloading* ploading ;
   typedef pvehicletype (tproduction[32]);


   union tresources {
            struct {
              int energy, material, fuel;
            }a;
            int resource[3];
   } ;


   class tbuildingtype_bi_picture {
      public:
         int num [ maxbuildingpicnum ][4][6];

         tbuildingtype_bi_picture ( void ) {
            for ( int i = 0; i < maxbuildingpicnum; i++ )
               for ( int j = 0; j < 4; j++ )
                  for ( int k = 0; j < 6; j++ )
                     num[i][j][k] = -1;
         };
   };

#ifdef HEXAGON
   class  tgeneral_old_building { 
#else
   class  tbuildingtype { 
#endif
                public:                         
                         Pointer      picture[ maxbuildingpicnum ][4][6];
                         struct { 
                                  Shortint     x, y; 
                                } entry; 
                         Word         id; 
                         char*        name; 
                         Word         armor; 
                         unsigned char         jamming; 
                         unsigned char         view; 
                         Word         loadcapacity; 
                         unsigned char         loadcapability;   /*  BM => CHoehenstufen; aktuelle Hîhe der reinzufahrenden vehicle
                                                                              mu· hier enthalten sein  */ 
                         unsigned char         unitheightreq;   /*   "       , es dÅrfen nur Fahrzeuge ,
                                        die in eine dieser Hîhenstufen kînnen , geladen werden  */ 

                         struct  { 
                                   word         material; 
                                   word         sprit; 
                                 } produktionskosten; 
                         int      special;   /*  HQ, Trainingslager, ...  */ 


                         unsigned char         technologylevel; 
                         unsigned char         researchid; 
                         struct  { 
                                            shortint     x, y; 
                                          } powerlineconnect, pipelineconnect; 

                         int      terrain;   /*  BM  */   /*  befahrbare terrain: z.B. Schiene, Wasser, Wald, ...  */ 
                         byte         construction_steps;  // 1 .. 8
                         int          maxresearchpoints; 

                         tresources   _tank;
                         tresources   maxplus;

                         int          efficiencyfuel;       // Basis 1024
                         int          efficiencymaterial;   // dito

                         void*        guibuildicon;

                         pterrainaccess terrain_access;
                         int          buildingheight;
                         int          unitheight_forbidden;
                         int          externalloadheight;
                         int          dummy[12];
                         void*        getpicture ( int x, int y );
                         int          vehicleloadable ( pvehicletype fzt );
                         int          gettank ( int resource );
                         void getfieldcoordinates( int bldx, int bldy, int x, int y, int *xx, int *yy);

                      }; 

#ifdef HEXAGON
   class  tbuildingtype { 
                public:                         
                         void*        w_picture [ cwettertypennum ][ maxbuildingpicnum ][4][6];
                         int          bi_picture [ cwettertypennum ][ maxbuildingpicnum ][4][6];
                         struct { 
                                  int     x, y; 
                                } entry, powerlineconnect, pipelineconnect; 
                         int          id; 
                         char*        name; 
                         int          armor; 
                         int          jamming; 
                         int          view; 
                         int          loadcapacity; 
                         unsigned char         loadcapability;   /*  BM => CHoehenstufen; aktuelle Hîhe der reinzufahrenden vehicle
                                                                              mu· hier enthalten sein  */ 
                         unsigned char         unitheightreq;   /*   "       , es dÅrfen nur Fahrzeuge ,
                                        die in eine dieser Hîhenstufen kînnen , geladen werden  */ 

                         struct  { 
                                   int          material; 
                                   int          sprit; 
                                 } produktionskosten; 
                         int          special;   /*  HQ, Trainingslager, ...  */ 


                         unsigned char         technologylevel; 
                         unsigned char         researchid; 

                         tterrainaccess terrainaccess;

                         int          construction_steps;  // 1 .. 8
                         int          maxresearchpoints; 

                         tresources   _tank;
                         tresources   maxplus;

                         int          efficiencyfuel;       // Basis 1024
                         int          efficiencymaterial;   // dito

                         void*        guibuildicon;

                         pterrainaccess terrain_access;
                         tresources   _bi_maxstorage;
                         int          buildingheight;
                         int          unitheight_forbidden;
                         int          externalloadheight;
                         int          dummy[9];
                         void*        getpicture ( int x, int y );

                         tbuildingtype ( void ) 
                         {
                            terrain_access = &terrainaccess;
                         };
                         int          vehicleloadable ( pvehicletype fzt );
                         int          gettank ( int resource );
                         void getfieldcoordinates( int bldx, int bldy, int x, int y, int *xx, int *yy);
                      }; 
#endif


typedef class  tbuilding* pbuilding;

   class  tbuilding {                         
                  int turnstatus;
                  int turnminestatus;
                  int lastenergyavail;
                  int lastmaterialavail;
                  int lastfuelavail;
            public:
                  pbuildingtype typ; 
                  tmunition    munitionsautoproduction; 
                  unsigned char         color; 
                  tproduction  production; 
                  tloading      loading; 
                  unsigned char         damage; 

                  tresources   plus;
                  tresources   maxplus;

                  tresources   actstorage;

                  tmunition    munition; 

                  word         maxresearchpoints; 
                  word         researchpoints; 

                  char*        name;
                  Integer      xpos, ypos; 
                  pbuilding    next;
                  pbuilding    prev; 
                  unsigned char         completion; 
                  tthreatvar   threatvalue; 
                  int          netcontrol; 
                  int      connection; 
                  boolean      visible; 
                  tproduction  productionbuyable;

                  tresources bi_resourceplus;


                  tbuilding( void );
                  int lastmineddist;
                  int  getenergyplus( int mode );  // mode ( bitmapped ) : 1 : maximale energieproduktion ( ansonsten das, was gerade ins netz reingeht )
                                                   //                      2 : windkraftwerk
                                                   //                      4 : solarkraftwerk
                                                   //                      8 : konventionelles Kraftwerk
                  int  getmaterialplus( int mode );  // mode ( bitmapped ) : 1 : maximale energieproduktion ( ansonsten das, was gerade ins netz reingeht )
                  int  getfuelplus( int mode );  // mode ( bitmapped )     : 1 : maximale energieproduktion ( ansonsten das, was gerade ins netz reingeht )

                  void initwork ( void );           
                  int  worktodo ( void );
                  void processwork ( void );

               private:
                  void produceenergy( void );   
                  void producematerial( void );
                  void producefuel( void );
                  int  processmining ( int res, int abbuchen = 1 );

               public:
                  void execnetcontrol ( void );
                  int  getmininginfo ( int res );

                  int  put_energy ( int      need,    int resourcetype, int queryonly  );
                  int  get_energy ( int      need,    int resourcetype, int queryonly );

                  void getresourceusage ( tresources* usage );
                  void changecompletion ( int d );
                  int vehicleloadable ( pvehicle eht, int uheight = -1 );
                  void* getpicture ( int x, int y );
                  void convert ( int col );
                  tbuilding ( pbuilding src, pmap actmap );
                  void addview ( void );
                  void removeview ( void );
                  int  chainbuildingtofield ( int x, int y );
                  int  unchainbuildingfromfield ( void );
               };

   struct tquickview {
              struct {
                 char p1;
                 char p3[3][3];
                 char p5[5][5];
              } dir[8];
           };

   typedef struct tquickview* pquickview;
   typedef struct tterraintype* pterraintype;
   typedef class  twterraintype* pwterraintype ;

    class  twterraintype {
                      public:
                         Pointer        picture[8];
                         pointer        direcpict[8];
                        #ifdef HEXAGON
                         int            dummy1;
                        #else
                         int            art;  
                        #endif
                         Word           defensebonus;
                         word           attackbonus;
                         char           basicjamming;
                         char           movemaluscount;
                         char*          movemalus;
                         pterraintype  terraintype;
                         pquickview     quickview;
                         void           paint ( int x1, int y1 );
                        #ifdef HEXAGON
                         int            bi_picture[6]; 
                         tterrainbits   art; 
                        #endif
                      };

     struct tterraintype {
                      char*              name;
                      int                id;
                      pwterraintype   weather[cwettertypennum];
                      int                neighbouringfield[8];   
                    };



     struct tresourcetribute {
                      struct {
                         union {
                            struct {
                                      int   energy[8][8];
                                      int   material[8][8];
                                      int   fuel[8][8];
                                    }a;
                            int resource[3][8][8];
                         };
                       } avail, paid;
                   };
                          //  resourcetribute.avail.energy[a][b] gibt an, wieviel energie spieler b aus dem netz von spieler a noch abbuchen darf
                          //  resourcetribute.paid.energy[a][b] gibt an, wieviel energie spieler b aus dem netz von spieler a bereits abgebucht hat
                          //  a ist spender, b empfÑnger
     typedef tresourcetribute* presourcetribute ;

  typedef class tfield* pfield ;


#ifdef HEXAGON
struct thexpic {
          void* picture;
          int   bi3pic;
          int   flip;  // Bit 1: Horizontal ; Bit 2: Vertikal
      };


   class tobjecttype {
             public: 
                 int id;
                 int weather;
                 int visibleago;

                 int objectslinkablenum;
                 union {
                    int*            objectslinkableid;
                    pobjecttype*    objectslinkable;
                 };

                 thexpic* oldpicture;

                 int pictnum;
                 int armor;

                 char  movemalus_plus_count;
                 char* movemalus_plus;

                 char  movemalus_abs_count;
                 char* movemalus_abs;

                 int attackbonus_plus;
                 int attackbonus_abs;

                 int defensebonus_plus;
                 int defensebonus_abs;

                 int basicjamming_plus;
                 int basicjamming_abs;

                 int height;   

                 tresources buildcost;
                 tresources removecost;
                 int build_movecost;  // basis 8 
                 int remove_movecost;  // basis 8 

                 char* name;
                 int no_autonet;

                 tterrainaccess terrainaccess;
                 tterrainbits terrain_and;
                 tterrainbits terrain_or;
                 tobjecttype ( void ) : terrain_and ( -1 ) , terrain_or ( 0 ) {};
                                  
                 void* buildicon;
                 void* removeicon;
                 int* dirlist;
                 int dirlistnum;
                 thexpic* picture[cwettertypennum];
                 void display ( int x, int y );
                 void display ( int x, int y, int dir, int weather = 0 );
                 void* getpic ( int i, int weather = 0 );
                 int  buildable ( pfield fld );
                 int connectablewithbuildings ( void );
          };

#else

   class tobjecttype {
             public: 
                 int id;
                 int terrain_and;
                 int terrain_or;
                 int objectslinkablenum;
                 union {
                    int*            objectslinkableid;
                    pobjecttype*    objectslinkable;
                 };

                 void** picture;

                 int pictnum;
                 int armor;

                 char  movemalus_plus_count;
                 char* movemalus_plus;

                 char  movemalus_abs_count;
                 char* movemalus_abs;

                 int attackbonus_plus;
                 int attackbonus_abs;

                 int defensebonus_plus;
                 int defensebonus_abs;

                 int basicjamming_plus;
                 int basicjamming_abs;

                 int height;   

                 tresources buildcost;
                 tresources removecost;
                 int movecost;  // basis 8 

                 char* name;
                 int no_autonet;

                 void* buildicon;
                 void* removeicon;
                 int* dirlist;
                 int dirlistnum;
                 int dummy[6];
                 void display ( int x, int y );
                 void display ( int x, int y, int dir, int weather = 0 );
                 void* getpic ( int i, int weather = 0 );
                 int  buildable ( pfield fld );
                 int connectablewithbuildings ( void );
          };
#endif

   typedef class tobject* pobject;
   class tobject {
               public:
                 pobjecttype typ;
                 int damage;
                 int dir;
                 int time;
                 int dummy[4];
                 //void* picture;
                 tobject ( void );
                 tobject ( pobjecttype t );
                 void display ( int x, int y, int weather = 0 );
                 void setdir ( int dir );
                 int  getdir ( void );
              };


   typedef class  tobjectcontainer* pobjectcontainer;
   class  tobjectcontainer {
           public:
              char         mine;   /*  BM  */ 
              char         minestrength;  

              int objnum;
              pobject object[ maxobjectonfieldnum ];

              tobjectcontainer ( void );
              int checkforemptyness ( void );
              pobject checkforobject ( pobjecttype o );
   };

   typedef struct tresourceview* presourceview;
   struct tresourceview {
              tresourceview ( void );
              char    visible;      // BM
              char    fuelvisible[8];
              char    materialvisible[8];
  };


  class  tfield { 
           public:
              pwterraintype typ;   

              unsigned char         fuel, material; 
              Word         visible;   /*  BM  */ 
              unsigned char         direction; 

              Pointer      picture;   
              unsigned char         movemalus[cmovemalitypenum];
              union  {
                 struct { 
                    char         temp;      
                    char         temp2; 
                 }a;
                 word tempw;
              };
              char         temp3;
              pvehicle     vehicle; 
              pbuilding    building; 

              presourceview  resourceview;
              pobjectcontainer      object;

              tterrainbits  bdt;
              int connection;
             
              int mineexist ( void );
              void addobject ( pobjecttype obj, int dir = -1, int force = 0 );
              void removeobject ( pobjecttype obj );
              void sortobjects ( void );
              void deleteeverything ( void );

              void setparams ( void );
              pobject checkforobject ( pobjecttype o );
              int getdefensebonus ( void );
              int getattackbonus  ( void );
              int getweather ( void );
              int getjamming ( void );

              void  putmine ( int col, int typ, int strength );
              void  removemine ( void );
              struct {
                 int view;
                 int jamming;
                 char mine, satellite, sonar, direct;
              } view[8];
          private:
              int getx( void );
              int gety( void );
           };

   typedef struct tevent* pevent ;

   class  teventtrigger_polygonentered {
         public:
            int size;
            pvehicle vehicle;
            int      vehiclenetworkid;
            int* data;
            int tempnwid;
            int tempxpos;
            int tempypos;
            int color;                // bitmapped
            int reserved[7];
            teventtrigger_polygonentered ( void );
            teventtrigger_polygonentered ( const teventtrigger_polygonentered& poly );
            ~teventtrigger_polygonentered ( );
        };
   typedef struct teventtrigger_polygonentered* peventtrigger_polygonentered;

   class  LargeTriggerData {
                 public:
                       tgametime time;
                       int xpos, ypos;
                       int networkid;
                       pbuilding    building;
                       pvehicle     vehicle;  
                       int          mapid;
                       int          id;  
                       peventtrigger_polygonentered unitpolygon;  
                       int reserved[32];
                       LargeTriggerData ( void );
                       LargeTriggerData ( const LargeTriggerData& data );
                       ~LargeTriggerData();
               };
   typedef LargeTriggerData* PLargeTriggerData;

/*
   struct ttriggerdata { 
                     union {                        
                       tgametime time;
                       struct {  word         xpos, ypos;  };
                       struct {  pbuilding    building;  };
                       struct {  pvehicle     vehicle;  };
                       struct {  pevent       event;  };
                       struct {  int      id;  };
                       struct {  peventtrigger_polygonentered unitpolygon;  };
                       PLargeTriggerData largedata;
                     };
                  };

*/


   class tevent { 
           public:
              union { 
                  struct {  word         saveas; char action, num;  }a;  /*  CEventActions  */
                  int      id;               /* Id-Nr      ==> Technology.Requireevent; Tevent.trigger; etc.  */ 
                 } ;                                          

              byte         player;   // 0..7  fÅr die normalen Spieler
                                      // 8 wenn das Event unabhÑngig vom Spieler sofort auftreten soll
              
              char         description[20]; 

              union {
                void*    rawdata; 
                char*    chardata;
                int*     intdata;
              };
              int          datasize; 
              pevent       next; 
              int          conn;   // wird nur im Spiel gebraucht, BIt 0 gibt an, das andere events abhÑnging sind von diesem
              word         trigger[4];   /*  CEventReason  */ 
              PLargeTriggerData trigger_data[4];

              byte         triggerconnect[4];   /*  CEventTriggerConn */ 
              byte         triggerstatus[4];   /*  Nur im Spiel: 0: noch nicht erfÅllt
                                                                 1: erfÅllt, kann sich aber noch Ñndern
                                                                 2: unwiederruflich erfÅllt
                                                                 3: unerfÅllbar */ 
              tgametime     triggertime;     // Im Karteneditor auf  -1 setzen !! 
                                             // Werte ungleich -1 bedeuten automatisch, da· das event bereits erfÅllt ist und evt. nur noch die Zeit abzuwait ist

              struct {
                     int turn;
                     int move;   // negative Zahlen SIND hier zulÑssig !!! 
              } delayedexecution;

                        /* Funktionsweise der verzîgerten Events: 
                             Sobald die Trigger erfÅllt sind, wird triggertime[0] ausgefÅllt. Dadurch wird das event ausgelîst,
                             sobald das Spiel diese Zeit erreicht ist, unabhÑngig vom Zustand des mapes 
                             ( Trigger werden nicht erneut ausgewertet !)
                         */
              tevent ( void );
              tevent ( const tevent& event );
              ~tevent ( void );
            }; 


   struct teventact { 
                 union { 
                  struct {  word         saveas, action;  }a;  /* Id-Nr */   /*  ==> Technology.Requireevent; Tevent.trigger; etc.  */ 
                  int      ID;    /*  CEventActions  */ 
                 };
              };



     /*  Datenaufbau des triggerData fieldes: [ hi 16 Bit ] [ low 16 Bit ] [ 32 bit Integer ] [ Pointer ]      [ low 24 Bit       ]  [ high 8 Bit ]
     'turn/move',                            move           turn
     'building/unit     ',Kartened/Spiel                                                   PBuilding/Pvehicle
                          disk               ypos           xpos
     'technology researched',                                             Tech. ID
     'event',                                                             Event ID
     'tribut required'                                                                                         Hîhe des Tributes      Spieler, von dem Tribut gefordert wird 
     'all enemy *.*'                                                      Bit 0: alle nicht allierten
                                                                          Bit 1: alle, die Åber die folgenden Bits festgelegt werden, ob alliiert oder nicht
                                                                            Bit 2 : Spieler 0
                                                                            ...
                                                                            Bit 9 : Spieler 7

     'unit enters polygon'  pointer auf teventtrigger_polygonentered

     der Rest benîtigt keine weiteren Angaben
    */ 



     /*  DatenAufbau des Event-Data-Blocks:

      TLosecampaign, TEndCampaign, TWeatherchangeCompleted
                 benîtigen keine weiteren Daten


      TNewTechnologyEvent :
                 data = NULL;
                 SaveAs = TechnologyID;
              Gilt fÅr researched wie auch available

      TMessageEvent
             Data = NULL;
             SaveAs: MessageID des Abschnittes in TextDatei

      TNextMapEvent:
             Data = NULL;
             saveas: ID der nÑchsten Karte;

      TRunScript+NextMapEvent:
             Data = pointer auf Dateinamen des Scriptes ( *.scr );
             saveas: ID der nÑchsten Karte;

      TeraseEvent:
             data[0] = ^int
                       ID des zu lîschenden Events
             data[1] = mapid

      Tweatherchange     	( je ein int , alles unter Data )
              wetter     	( -> cwettertypen , Wind ist eigene eventaction )
              fieldadressierung	     ( 1: gesamtes map     )
                      ≥              ( 0: polygone               )
                      ≥              
                      √ƒƒƒƒƒ 0 ƒƒƒ>  polygonanzahl
                      ≥                   √ƒƒ   eckenanzahl
                      ≥                             √ƒƒ x position
                      ≥                                 y position
                      ≥
                      ≥
                      ¿ƒƒƒƒƒ 1 ƒƒƒ|  

      Twindchange
              intensitÑt[3]         ( fÅr tieffliegend, normalfliegend und hochfliegend ; -1 steht fÅr keine énderung )
              Richtung[3]           ( dito )
 

      Tmapchange               ( je ein int , alles unter Data )        { wetter wird beibehalten ! }
            numberoffields ( nicht die Anzahl fielder insgesamt, 
               √ƒƒ>  bodentypid
                     drehrichtung 
                     fieldadressierung   ( wie bei tweatherchange )


      Treinforcements        ( alles unter DATA )
             int num      // ein int , der die Anzahl der vehicle angibt. Die vehicle, die ein Transporter geladen hat, werden NICHT mitgezÑhlt.
                  √ƒƒƒƒ > die vehicle, mit tspfldloaders::writeunit in einen memory-stream geschrieben. 


      TnewVehicleDeveloped
            saveas  = ID des nun zur VerfÅgung stehenden vehicletypes


      Tpalettechange
           Data =  Pointer auf String, der den Dateinamen der Palettendatei enthÑlt.

      Talliancechange
           Data : Array[8][8] of int                      // status der Allianzen. Sollte vorerst symetrisch bleiben, also nur jeweils 7 Werte abfragen.
                                                             Vorerst einfach Zahlwerte eingeben.
                                                             256 steht fÅr unverÑndert,
                                                             257 fÅr umkehrung

      TGameParameterchange    
           int nummer_des_parameters ( -> gameparametername[] )
           int neuer_wert_des_parameters

      Ellipse
           int x1 , y1, x2, y2, x orientation , y orientation


    Wenn Data != NULL ist, MU· datasize die Grî·e des Speicherbereichs, auf den Data zeigt, beinhalten.

    */


   struct tresearchdatachange { 
                   word         weapons[waffenanzahl];   /*  Basis 1024  */
                   word         armor;         /*  Basis 1024  */
                   unsigned char         dummy[20+(12-waffenanzahl)*2];
                 }; 

   typedef class  ttechnology* ptechnology ;

   class  ttechnology { 
               public:
                   pointer      icon; 
                   char*        infotext; 
                   int          id; 
                   int          researchpoints; 
                   char*        name; 

                   tresearchdatachange unitimprovement; 

                   boolean      requireevent; 

                   union { 
                      ptechnology  requiretechnology[6]; 
                      int      requiretechnologyid[6]; 
                   };

                   int          techlevelget;  // sobald dieser technologylevel erreicht ist, ist die Technologie automatisch verfÅgbar
                   char* pictfilename;
                   int lvl;     // wird nur im Spiel benîtigt: "Level" der benîtigten Techologie. Gibt an, wieviele Basistechnologien insgesamt benîtogt werden.
                   int techlevelset;
                   int dummy[7];
                   int  getlvl( void );
                 };


   typedef class tresearch* presearch ;

   typedef struct tdevelopedtechnologies*  pdevelopedtechnologies;
   struct tdevelopedtechnologies {
             ptechnology               tech;
             pdevelopedtechnologies    next;
          };
             
   class tresearch { 
             public:
                 int                     progress;
                 ptechnology             activetechnology;
                 tresearchdatachange     unitimprovement;
                 int                     techlevel;
                 pdevelopedtechnologies  developedtechnologies;

                 int technologyresearched ( int id );
                 int vehicletypeavailable ( const pvehicletype fztyp, pmap map );       // The map should be saved as a pointer in TRESEARCH, but this will change the size of TMAP and make all existing savegames and maps invalid ....
                 int vehicleclassavailable ( const pvehicletype fztyp , int classnm, pmap map );

               };



   struct tcampaign { 
                 Word         id; 
                 word         prevmap;   /*  ID der vorigen Karte  */ 
                 unsigned char         player;   /*  Farbenummer des Spielers: 0..7  */ 
                 boolean      directaccess;   /*  Kann die Karte einzeln geladen werden oder nicht ?  */ 
                 unsigned char         dummy[21];   /*  fÅr zukÅnftige erweiterungen  */ 
               }; 



   typedef struct tcampaign* pcampaign ;

   struct taiparams {
                      char    dummy[128];
                    };
   typedef struct taiparams* paiparams;

   typedef struct teventstore* peventstore ;


/*
   typedef struct tfieldresourcesshown* pfieldresourcesshown;
   struct tfieldresourcesshown {
                         int fieldnum;
                         word  xp,yp;
                         char  player;   // BM
                         int material[8];
                         int fuel[8];
                         int turn[8];
                    };



   typedef struct tfieldresourcearray* pfieldresourcearray;
                                       
   struct tfieldresourcearray {
               int                     min, max;
               int                     num;
               pfieldresourcearray     next;
               pfieldresourcearray     prev;
               tfieldresourcesshown    data[1024];
           };

*/

   struct teventstore { 
                   int          num; 
                   peventstore  next; 
                   int      eventid[256]; 
                   int      mapid[256];
                 };

   class twind {
          public:
                   char speed;
                   char direction;
                   int operator== ( const twind& b ) const;
      };

   struct tweather {
                       char fog;
                       twind wind[3];
                       char dummy[12];
                    };

   typedef char* pchar;

   typedef class tbasenetworkconnection* pbasenetworkconnection;

   #define tnetworkdatasize 100

   typedef char tnetworkconnectionparameters[ tnetworkdatasize ];
   typedef tnetworkconnectionparameters* pnetworkconnectionparameters;

   enum tnetworkchannel { receive, send };

   class tnetworkcomputer {
      public:
         char*        name;
         struct {
             int          transfermethodid;
             pbasenetworkconnection transfermethod;
             tnetworkconnectionparameters         data;
         } send, receive;
         int          existent;
         tnetworkcomputer ( void );
         ~tnetworkcomputer ( );
   } ;


   class  tnetwork {
                    public:
                        struct {
                            char         compposition;   // Nr. des Computers, an dem der SPieler spielt    => network.computernames
                            int          codewordcrc;
                        } player[8];

                        tnetworkcomputer computer[8];

                        int computernum;
                        int turn;
                        struct tglobalparams {
                                 int enablesaveloadofgames;
                                 int reaskpasswords;
                                 int dummy[48];
                        } globalparams;
                        tnetwork ( void );
                   };
   typedef struct tnetwork* pnetwork;


  struct tcursorpos {
           struct {
                     integer cx;
                     integer sx;
                     integer cy;
                     integer sy;
                  } position[8];
          };


   typedef struct tdissectedunit* pdissectedunit;
   struct tdissectedunit {
                    pvehicletype  fzt;
                    ptechnology   tech;
                    int           orgpoints;
                    int           points;
                    int           num;
                    pdissectedunit next;
                };



  #ifndef pmemorystreambuf_defined
  #define pmemorystreambuf_defined

  typedef class tmemorystreambuf* pmemorystreambuf;
  typedef class tmemorystream* pmemorystream;

  #endif

  class treplayinfo {
          public:
             pmemorystreambuf guidata[8];
             pmemorystreambuf map[8];
             pmemorystream    actmemstream;
             treplayinfo ( void );
             ~treplayinfo ( );
        };

  typedef class  tmessage* pmessage;
  class  tmessage {
         public:
            int from;      // BM ; Bit 9 ist system
            int to;        // BM
            time_t time;
            char* text;
            int id;
            int runde;  //  Zeitpunkt des abschickens
            int move;   //  "
            pmessage next;
            tmessage ( void );
            tmessage ( pmap spfld );
            tmessage ( char* txt, int rec );  // fÅr Meldungen vom System
            ~tmessage();
         };


   typedef class tmessagelist* pmessagelist;
   class tmessagelist {
           public:
              pmessage message;
              pmessagelist next;
              pmessagelist prev;
              tmessagelist( pmessagelist* prv );
              ~tmessagelist(  );
              int getlistsize ( void );  // liefert 1 falls noch weitere Glieder in der Liste existieren, sonst 0;
        };


   typedef class tspeedcrccheck* pspeedcrccheck;
   typedef class tcrcblock* pcrcblock;

   class tcrcblock {
            public:
              int  crcnum;
              pcrc crc;
              int restricted;
                                             // restricted kennt 3 ZustÑnde: 0 = nicht limitiert, neue vehicle werden nicht aufgenommen; 
                                             //                              1 = nicht limitiert, neue vehicle werden aufgenommen
                                             //                              2 = limitiert: es dÅrfen nur vehicle verwendet werden, deren CRCs bekannt sind
              tcrcblock ( void );
         };


   typedef class tobjectcontainercrcs *pobjectcontainercrcs;
   class tobjectcontainercrcs {
             public:
                tcrcblock unit;
                tcrcblock building;
                tcrcblock object;
                tcrcblock terrain;
                tcrcblock technology;

                pspeedcrccheck speedcrccheck;

                tobjectcontainercrcs ( void );

                int dummy[40];
         };


/*
   class tfieldarray {
           struct { tfield fieldl1[1024] };

           fieldl1* fieldl2;

         public:
           tfieldarray ( int size );
           tfieldarray ( void );
           void alloc ( int size );
           void free ( void );
           tfield& operator[] ( int a ) { return fieldl2[ a >> 10 ].field[ a & 1023 ]; }
           ~tfieldarray();


    };
*/


   enum tplayerstat { ps_human, ps_computer, ps_off };

   enum tshareviewmode { sv_none, sv_shareview };

   class  tshareview {
                  public:
                        tshareview ( void ) { recalculateview = 0; };
                        tshareview ( const tshareview* org );
                        tshareviewmode mode[8][8]; 
                        int recalculateview;
                     };
                  // mode[1][6] = visible_all    =>  Spieler 1 gibt Spieler 6 die view frei

   typedef tshareview *pshareview;

   struct PreferredFilenames {
             char* mapname[8];
             char* mapdescription[8];
             char* savegame[8];
             char* savegamedescription[8];
         };

   class  EllipseOnScreen {
          public:
           int x1, y1, x2, y2;
           int color;
           float precision;
           int active;
           EllipseOnScreen ( void ) { active = 0; };
           void paint ( void );
          };


   class tmap { 
           public:
                 word         xsize, ysize;   /*  Grî·e in fielder  */ 
                 word         xpos, ypos;     /*  aktuelle Dargestellte Position  */
                 pfield        field;           /*  die fielder selber */
                 char         codeword[11]; 
                 char*        title;
                 pcampaign    campaign; 

                 signed char  actplayer; 
                 tgametime    time;

                 tweather     weather;

                 int resourcemode;  // 1 = Battle-Isle-Mode

                 
                 char         alliances[8][8];
                 struct {
                                              boolean      existent; 
                                              pvehicle     firstvehicle; 
                                              pbuilding    firstbuilding; 

                                              tresearch    research; 
                                              paiparams    aiparams;

                                              tplayerstat  stat;           // 0: human; 1: computer; 2: off
                                              // char         alliance;       // => actmap->alliances ;  8 bedeuted parteilos
                                              char         dummy;
                                              char         *name;          // kein eigenstÑndiger string; zeigt entweder auf computernames oder playernames 
                                              int          passwordcrc;
                                              pdissectedunit dissectedunit;
                                              pmessagelist  unreadmessage;
                                              pmessagelist  oldmessage; 
                                              pmessagelist  sentmessage; 
                                           } player[9]; 

                 peventstore  oldevents; 
                 pevent       firsteventtocome; 
                 pevent       firsteventpassed; 

                 int eventpassed ( int saveas, int action, int mapid );
                 int eventpassed ( int id, int mapid );

                 int      unitnetworkid; 

                 boolean      levelfinished; 

                 pnetwork     network;

                 // char*        alliancenames[8];
                 int           alliance_names_not_used_any_more[8];

                 tcursorpos   cursorpos;
                 presourcetribute tribute;
                 pmessagelist  unsentmessage;
                 pmessage      message;
                 int           messageid;
                 char*         journal;
                 char*         newjournal;
                 char*         humanplayername[8];
                 char*         computerplayername[8];
                 int           supervisorpasswordcrc;
                 char          alliances_at_beginofturn[8];
                 pobjectcontainercrcs   objectcrc; 
                 pshareview    shareview;
                 int           continueplaying;         // als einzig Åbriggebliebener Spieler
                 treplayinfo*  replayinfo;
                 int           playerview;
                 tgametime     lastjournalchange;
                 tresources    bi_resource[8];
                 PreferredFilenames* preferredfilenames;
                 EllipseOnScreen* ellipse;
                 int           dummy[32];
                 int           gameparameter[ maxgameparameternum ];
/*
                 tmap ( void );
                 tmap ( const tmap &map );
                 ~tmap ( );
*/
                 void chainunit ( pvehicle unit );
                 void chainbuilding ( pbuilding bld );
                 pvehicle getunit ( int x, int y, int nwid );
               private:
                 pvehicle getunit ( pvehicle eht, int nwid );

              }; 




  typedef struct tguiicon* pguiicon ;

  struct tguiicon { 
            Pointer      picture[2]; 
            char         txt[31]; 
            unsigned char         id; 
            char         key[4]; 
            word         realkey[4]; 
            unsigned char         order; 
          };

   struct ticons { 
               struct { 
                             void      *pfeil1, *pfeil2; 
                          } weapinfo; 
               Pointer      statarmy[3]; 
               Pointer      height[8];      // fÅr vehicleinfo - DLG-Box
               void*        height2[3][8];  // fÅr vehicleinfo am map
               void*        player[8];      // aktueller Spieler in der dashboard: FARBE.RAW 
               void*        allianz[8][3];  // Allianzen in der dashboard: ALLIANC.RAW 
               void*        diplomaticstatus[8]; 
               void*        selectweapongui[12];
               void*        selectweaponguicancel;
               void*        unitinfoguiweapons[13];
               void*        experience[16];
               void*        wind[9];
               void*        windarrow;
               void*        stellplatz;
               void*        guiknopf;   // reingedrÅckter knopf
               void*        computer;
               void*        windbackground;
               void*        smallmapbackground;
               void*        weaponinfo[5];
               void*        X;
               struct {
                  struct       {
                                  void* active;
                                  void* inactive;
                                  void* repairactive;
                                  void* repairinactive;
                                  void* movein_active;
                                  void* movein_inactive;
                               } mark;
                  struct       {
                                  struct {
                                     void* main;
                                     void* active;
                                     void* inactive;
                                  } netcontrol;
                                  struct {
                                     void* main;
                                     void* button;
                                     void* buttonpressed;
                                     void* schieber[4];
                                     void* schiene;
                                  } ammoproduction;
                                  struct {
                                     void* main;
                                  } resourceinfo;
                                  struct {
                                     void* main;
                                  } windpower;
                                  struct {
                                     void* main;
                                  } solarpower;
                                  struct {
                                     void* main;
                                     void* button;
                                     void* buttonpressed;
                                     void* schieber[4];
                                     void* schiene;
                                     void* schieneinactive;
                                  } ammotransfer;
                                  struct {
                                     void* main;
                                     void* button[2];
                                     void* schieber;
                                  } research;
                                  struct {
                                     void* main;
                                     void* button[2];
                                     void* schieber;
                                  } conventionelpowerplant;
                                  struct {
                                     void* main;
                                     void* height1[8];
                                     void* height2[8];
                                     void* repair;
                                     void* repairpressed;
                                     void* block;
                                  } buildinginfo;
                                  struct {
                                     void* main;
                                     void* button[2];
                                     void* resource[2];
                                     void* graph;
                                     void* axis[3];
                                     void* pageturn[2];

                                     void* schieber;
                                  } miningstation;
                                  struct {
                                     void* main;
                                     void* height1[8];
                                     void* height2[8];
                                     void* sum;
                                  } transportinfo;

                               } subwin;
                  union        {
                                  void* sym[11][2];
                                  struct {
                                     void*  ammotransfer[2];
                                     void*  research[2];
                                     void*  resourceinfo[2];
                                     void*  netcontrol[2];
                                     void*  solar[2];
                                     void*  ammoproduction[2];
                                     void*  wind[2];
                                     void*  powerplant[2];
                                     void*  buildinginfo[2];
                                     void*  miningstation[2];
                                     void*  transportinfo[2];
                                  } a;
                               } lasche;
    
                  void* tabmark[2];
                  void* container_window;
               } container;
               struct {
                  void* bkgr;
                  void* orgbkgr;
               } attack;
               void*        pfeil2[8];     // beispielsweise fÅr das Mouse-Scrolling 
               void*        mousepointer;
               void*        fieldshape;
               void*        hex2octmask;
               void*        mapbackground;
            }; 




   class tnomaploaded {
   };







     /*  conquered = You conquered sth.
          lost = an enemy conquered sth.   */ 


     /* Zeigt an, bei welchen aktionen die dazugehîrige Mission/Event abgecheckt werden soll.
     bitmapped */ 




   extern  const char* cconnections[6];
   #define cconnection_destroy 1  
   #define cconnection_conquer 2  
   #define cconnection_lose 4  
   #define cconnection_seen 8
   #define cconnection_areaentered_anyunit 16
   #define cconnection_areaentered_specificunit 32

   extern const char* ceventtriggerconn[]; 
   #define ceventtrigger_and 1  
   #define ceventtrigger_or 2  
   #define ceventtrigger_not 4  
   #define ceventtrigger_klammerauf 8  
   #define ceventtrigger_2klammerauf 16  
   #define ceventtrigger_2klammerzu 32  
   #define ceventtrigger_klammerzu 64  
     /*  reihenfolgenprioritÑt: in der Reihenfolge von oben nach unten wird der TriggerCon ausgewertet
               AND   OR
               NOT
               (
               eigentliches event
               )
    */ 

  extern const char* ceventactions[ceventactionnum]  ; 
       /*  NICHT gebitmapped  */ 
   #define cemessage 0  
   #define ceweatherchange 1  
   #define cenewtechnology 2  
   #define celosecampaign 3  
   #define cerunscript  4  
   #define cenewtechnologyresearchable 5  
   #define cemapchange 6  
   #define ceeraseevent 7  
   #define cecampaignend 8  
   #define cenextmap 9   
   #define cereinforcement 10
   #define ceweatherchangecomplete 11
   #define cenewvehicledeveloped 12
   #define cepalettechange 13
   #define cealliancechange 14
   #define cewindchange 15
   #define cenothing 16
   #define cegameparamchange 17
   #define ceellipse 18
   #define ceremoveellipse 19

     /*  Event-Auslîser:  */ 
   extern const char*  ceventtrigger[]; 

   #define ceventt_turn 1  
   #define ceventt_buildingconquered 2  
   #define ceventt_buildinglost 3  
   #define ceventt_buildingdestroyed 4  
   #define ceventt_unitlost 5  
   #define ceventt_technologyresearched 6  
   #define ceventt_event 7  
   #define ceventt_unitconquered 8  
   #define ceventt_unitdestroyed 9  
   #define ceventt_allenemyunitsdestroyed 10  
   #define ceventt_allunitslost 11  
   #define ceventt_allenemybuildingsdestroyed 12  
   #define ceventt_allbuildingslost 13  
   #define ceventt_energytribute 14
   #define ceventt_materialtribute 15
   #define ceventt_fueltribute 16
   #define ceventt_any_unit_enters_polygon 17
   #define ceventt_specific_unit_enters_polygon 18
   #define ceventt_building_seen 19


   #define guiiconsizex 49  
   #define guiiconsizey 35  


   #define constfieldsize 16  
   #define maxguiiconnumber 100

   #define cminenum 4
   extern const char*  cminentypen[cminenum] ;
      #define cmantipersonnelmine 1  
      #define cmantitankmine 2  
      #define cmmooredmine 3  
      #define cmfloatmine 4  

   extern const unsigned char cminestrength[cminenum]  ;


   extern const char*  cbuildingfunctions[cbuildingfunctionnum]; 
      #define cghqn 0  
      #define cghqb ( 1 << cghqn  )
      #define cgtrainingn 1  
      #define cgtrainingb ( 1 << cgtrainingn  )
      #define cgrefineryn 2  
      #define cgrefineryb ( 1 << cgrefineryn  )
      #define cgvehicleproductionn 3  
      #define cgvehicleproductionb ( 1 << cgvehicleproductionn  )
      #define cgammunitionproductionn 4  
      #define cgammunitionproductionb ( 1 << cgammunitionproductionn  )
      #define cgenergyproductionn 5  
      #define cgenergyproductionb ( 1 << cgenergyproductionn  )
      #define cgmaterialproductionn 6  
      #define cgmaterialproductionb ( 1 << cgmaterialproductionn  )
      #define cgfuelproductionn 7  
      #define cgfuelproductionb ( 1 << cgfuelproductionn  )
      #define cgrepairfacilityn 8  
      #define cgrepairfacilityb ( 1 << cgrepairfacilityn  )
      #define cgrecyclingplantn 9  
      #define cgrecyclingplantb ( 1 << cgrecyclingplantn  )
      #define cgresearchn 10  
      #define cgresearchb ( 1 << cgresearchn  )
      #define cgsonarn 11  
      #define cgsonarb ( 1 << cgsonarn )
      #define cgwindkraftwerkn 12
      #define cgwindkraftwerkb ( 1 << cgwindkraftwerkn )
      #define cgsolarkraftwerkn 13
      #define cgsolarkraftwerkb ( 1 << cgsolarkraftwerkn )
      #define cgconventionelpowerplantn 14
      #define cgconventionelpowerplantb ( 1 << cgconventionelpowerplantn )
      #define cgminingstationn 15
      #define cgminingstationb ( 1 << cgminingstationn )
      #define cgexternalloadingn 16
      #define cgexternalloadingb ( 1 << cgexternalloadingn )



      extern const char*  cvehiclefunctions[]; 
   #define cfsonar 1  
   #define cfparatrooper 2  
   #define cfminenleger 4  
   #define cf_trooper 8  
   #define cfrepair 16  
   #define cf_conquer 32
   #define cf_moveafterattack 64
   #define cfsatellitenview 128  
   #define cfputbuilding 256  
   #define cfmineview 512  
   #define cfvehicleconstruction 1024  
   #define cfspecificbuildingconstruction 2048  
   #define cffuelref 4096  
   #define cficebreaker 8192  
   #define cfnoairrefuel 16384
   #define cfmaterialref 32768  
   // #define cfenergyref 65536  
   #define cffahrspur ( 1 << 17 )   /*  !!  */
   #define cfmanualdigger ( 1 << 18 )
   #define cfwindantrieb ( 1 << 19 )
   #define cfautorepair ( 1 << 20 )
   #define cfgenerator ( 1 << 21 )
   #define cfautodigger ( 1 << 22 )
   #define cfkamikaze ( 1 << 23 )

   #define cfvehiclefunctionsanzeige 0xFFFFFFFF  


   extern const char*  cbodenarten[]  ; 


extern tterrainbits cbwater0 ;
extern tterrainbits cbwater1 ;
extern tterrainbits cbwater2 ;
extern tterrainbits cbwater3 ;
extern tterrainbits cbwater  ;

extern tterrainbits cbstreet ;
extern tterrainbits cbrailroad ;
extern tterrainbits cbbuildingentry ;
extern tterrainbits cbharbour ;
extern tterrainbits cbrunway ;
extern tterrainbits cbrunway ;
extern tterrainbits cbpipeline ;
extern tterrainbits cbpowerline;
extern tterrainbits cbfahrspur ;
extern tterrainbits cbfestland ;
extern tterrainbits cbsnow1 ;
extern tterrainbits cbsnow2 ;
extern tterrainbits cbhillside ;
extern tterrainbits cbsmallrocks ;
extern tterrainbits cblargerocks ;

 extern  const char*  choehenstufen[8] ;
        #define chtiefgetaucht 1  
        #define chgetaucht 2  
        #define chschwimmend 4  
        #define chfahrend 8  
        #define chtieffliegend 16  
        #define chfliegend 32  
        #define chhochfliegend 64  
        #define chsatellit 128  

   #define cwaffentypennum 12
 extern const char*  cwaffentypen[cwaffentypennum] ; 

   #define cwcruisemissile 0
   #define cwcruisemissileb ( 1 << cwcruisemissile )
   #define cwminen 1  
   #define cwmineb ( 1 << cwminen   )
   #define cwbombn 2  
   #define cwbombb ( 1 << cwbombn  )
   #define cwairmissilen 3  
   #define cwairmissileb ( 1 << cwairmissilen  )
   #define cwgroundmissilen 4  
   #define cwgroundmissileb ( 1 << cwgroundmissilen  )
   #define cwtorpedon 5  
   #define cwtorpedob ( 1 << cwtorpedon  )
   #define cwmachinegunn 6  
   #define cwmachinegunb ( 1 << cwmachinegunn )
   #define cwcannonn 7  
   #define cwcannonb ( 1 << cwcannonn )
   #define cwweapon ( cwcruisemissileb | cwbombb | cwairmissileb | cwgroundmissileb | cwtorpedob | cwmachinegunb | cwcannonb )
   #define cwshootablen 11  
   #define cwshootableb ( 1 << cwshootablen  )
   #define cwammunitionn 9  
   #define cwammunitionb ( 1 << cwammunitionn )
   #define cwservicen 8  
   #define cwserviceb ( 1 << cwservicen )

   extern const char*  cdnames[3];
     /* CDmaterialN = 1;
   CDmaterialB = 1 shl CDMaterialN;
   CDfuelN = 2;
   CDfuelB = 1 shl CDfuelN;
   CDenergyN = 0;
   CDenergyB = 1 shl CDenergyN;  */ 

     /* CWmaterialN=8;
   CWmaterialB=1 shl CWmaterialN;
   CWenergyN=12;
   CWenergyB=1 shl CWenergyN; */ 

     /*  Angabe: Waffentyp; energy - Material - Sprit ; jeweils fÅr 5er Pack */         
  extern const Word  cwaffenproduktionskosten[cwaffentypennum][3];
/*
   extern const byte cstreetproductioncosts[3];
   extern const byte crailroadproductioncosts[3];
   extern const byte cpipelineproductioncosts[3];
   extern const byte cpowerlineproductioncosts[3];
   #define streetbuildmovedecrease 16
   */


   #define movement_cost_for_repaired_unit 24
   #define movement_cost_for_repairing_unit 12
   #define attack_after_repair 1       // Can the unit that is beeing repaired attack afterwards? 

   #define mineputmovedecrease 8  
   extern const unsigned char     cstreetdestructioncosts[3];
   #define streetmovemalus 8  
   #define railroadmovemalus 8  
   #define searchforresorcesmovedecrease 8

   #define capeace 0  
   #define cawar 1  
   #define cawarannounce 2
   #define capeaceproposal 3
   #define canewsetwar1 4
   #define canewsetwar2 5
   #define canewpeaceproposal 6
   #define capeace_with_shareview 7

   #define diplomaticstatnum 1  


#ifdef HEXAGON
   #define maxmalq 10  
   #define minmalq 10
#else
   #define maxmalq 12  
   #define minmalq 8
#endif

   #define visible_not 0  
   #define visible_ago 1  
   #define visible_now 2  
   #define visible_all 3  

/*   #define stoerwidth 24  
   #define stoerstaerkenfaktor 6  */

   #ifdef HEXAGON
     #define fieldxsize 48    /*  Breite eines terrainbildes  */ 
     #define fieldysize 48  
     #define fielddistx 64
     #define fielddisty 24
     #define fielddisthalfx 32
     #define sidenum 6
   #else
     #define fieldxsize 40    /*  Breite eines terrainbildes  */ 
     #define fieldysize 39  
     #define fielddirecpictsize 800
     #define fielddistx 40
     #define fielddisty 20
     #define fielddisthalfx 20
     #define sidenum 8
   #endif

   #define fieldsizex fieldxsize
   #define fieldsizey fieldysize

   extern const int directionangle [ sidenum ];


   #define fieldsize (fieldxsize * fieldysize + 4 )
//   #define oldfieldsize (31 * 40 + 4  )
   #define unitsizex 30
   #define unitsizey 30
   #define tanksize (( unitsizex+1 ) * ( unitsizey+1 ) + 4 )
   #define unitsize tanksize
   #define fusstruppenplattfahrgewichtsfaktor 2  
   #define mingebaeudeeroberungsbeschaedigung 80  
   #define flugzeugtraegerrunwayverkuerzung 2  
     /* flugzeugstartmovefaktor = 2; */ 
   #define repairefficiency_unit 2  
   #define repairefficiency_building 3

   #define autorepairdamagedecrease 10    // => in 10 Runden kann sich eine vehicle komplett selbst reparieren

   #define refineryefficiency 8  
   #define air_heightincmovedecrease 18  
   #define air_heightdecmovedecrease 0
   #define sub_heightincmovedecrease 12
   #define sub_heightdecmovedecrease 12
   #define helicopter_attack_after_ascent 1  // nach abheben angriff mîglich
   #define helicopter_attack_after_descent 0  // nach landen angriff mîglich
   #define helicopter_landing_move_cost 16   // zusÑtzlich zu den Kosten fÅr das Wechseln der Hîhenstufe 
   #define weaponpackagesize 5

   #define trainingexperienceincrease 2

   #define brigde1buildcostincrease 12       // jeweils Basis 8; flaches Wasser
   #define brigde2buildcostincrease 16       // jeweils Basis 8; mitteltiefes Wasser
   #define brigde3buildcostincrease 36       // jeweils Basis 8; tiefes Wasser

     /* submovedecfactor =  6; */ 
     /* aircraftlandedmovedecrease = 3; */ 


   #define lookintoenemytransports false  
   #define lookintoenemybuildings false  

   #define recyclingoutput 2    /*  Material div RecyclingOutput  */ 
   #define destructoutput 5
   #define nowindplanefuelusage 1      // herrscht kein Wind, braucht ein Flugzeug pro Runde soviel Sprit wie das fliegend dieser Anzahl fielder
//   #define maxwindplainfuelusage 32   // beim nextturn: tank -= fuelconsumption * (maxwindplainfuelusage*nowindplainfuelusage + windspeed) / maxwindplainfuelusage     
   #define maxwindspeed 128          // Wind der StÑrke 256 legt pro Runde diese Strecke zurÅck: 128 entspricht 16 fieldern diagonal !


   #define chainsawpossibility 16000    // Basis ist 32768

   #define generatortruckefficiency 2  // FÅr jede vehicle Power wird soviel Sprit gebraucht !

   #define researchenergycost 512      // fÅr 1000 researchpoints wird soviel energie benîtigt.
   #define researchmaterialcost 200    //                                     material
   #define researchcostdouble 10000    // bei soviel researchpoints verdoppeln sich die Kosten
   #define minresearchcost 0.5
   #define maxresearchcost 4
   #define airplanemoveafterstart 12
   #define airplanemoveafterlanding (2*minmalq - 1 )

   #define  viewadditiv 1

   #define tfieldtemp2max 255
   #define tfieldtemp2min 0


   #define cnet_storeenergy        0x001           // es wird garantiert,  da· material immer das 2 und fuel das 4 fache von energy ist
   #define cnet_storematerial      0x002
   #define cnet_storefuel          0x004

   #define cnet_moveenergyout      0x008
   #define cnet_movematerialout    0x010
   #define cnet_movefuelout        0x020

   #define cnet_stopenergyinput    0x040
   #define cnet_stopmaterialinput  0x080
   #define cnet_stopfuelinput      0x100

   #define cnet_stopenergyoutput   0x200
   #define cnet_stopmaterialoutput 0x400
   #define cnet_stopfueloutput     0x800


   #define resource_fuel_factor 100         // die im boden liegenden BodenschÑtzen ergeben effektiv soviel mal mehr ( bei Bergwerkseffizienz 1024 )
   #define resource_material_factor 100     // "

   #define destruct_building_material_get 3 // beim Abrei·en erhÑlt man 1/3 des eingesetzten Materials zurÅck
   #define destruct_building_fuel_usage 10  // beim Abrei·en wird 10 * fuelconsumption Fuel fuelconsumptiont


   #define dissectunitresearchpointsplus  2    // Beim dissectn einer vehicle wird der sovielte Teil der Researchpoints jeder unbekannten Technologie gutgeschrieben

   #define dissectunitresearchpointsplus2 3    // Beim dissectn einer vehicle wird der sovielte Teil der Researchpoints jeder unbekannten Technologie gutgeschrieben.
                                               // FÅr die Technologie existieren aber bereits von einem anderen sezierten vehicletype gutschriften.

   #define maxminingrange 10     // soviele fielder such ein Bergwerk ab.

   #define fuelweight  4         // 1024 fuel wiegen soviel
   #define materialweight 12     // 1024 material wiegen soviel

   #define objectbuildmovecost 16  // vehicle->movement -= (8 + ( fld->movemalus[0] - 8 ) / ( objectbuildmovecost / 8 ) ) * kosten des obj


   class tgameoptions {
           public:
            tgameoptions ( void );
            void setdefaults ( void );
            int version;
            int fastmove;
            int visibility_calc_algo;      // 0 sauber, 1 schnell;  
            int movespeed;
            int endturnquestion;
            int smallmapactive;
            int units_gray_after_move;
            int mapzoom;
            int mapzoomeditor;
            int startupcount;
            int dummy[14];

            struct {
                       int scrollbutton;
                       int fieldmarkbutton;
                       int smallguibutton;
                       int largeguibutton;
                       int smalliconundermouse;  // 0: nie;  1: immer; 2: nur wenn vehicle, gebÑude, oder temp unter MAUS
                       int centerbutton;    // Maustaste zum zentrieren des fielder, Åber dem sich die Maus befindet;
                       int dummy[9];
            } mouse;
            struct {
                       int autoproduceammunition;
                       int filleverything;
                       int emptyeverything;
                       int dummy[10];
            } container;
            int onlinehelptime;
            int smallguiiconopenaftermove;
            int defaultpassword;
            int replayspeed;
            struct {
               char* dir;
               struct {
                  int terrain;
                  int units;
                  int objects;
                  int buildings;
               } interpolate;
            } bi3;
            int dummy2[41];
            int changed;
            char filename[20];
         };

extern tgameoptions gameoptions;


extern const char*  cwettertypen[];

extern const char* gameparametername[ gameparameternum ];
extern const int gameparameterdefault [ gameparameternum ];
#define cgp_fahrspur  0
#define cgp_eis  1
#define cgp_movefrominvalidfields 2
#define cgp_building_material_factor 3
#define cgp_building_fuel_factor 4
#define cgp_forbid_building_construction 5
#define cgp_forbid_unitunit_construction 6
#define cgp_bi3_training 7


extern const char*  cmovemalitypes[cmovemalitypenum];
extern const int csolarkraftwerkleistung[];
extern const char* cnetcontrol[cnetcontrolnum];
extern const char* cgeneralnetcontrol[];

#define unspecified_error 9999

#define greenbackgroundcol 156


#ifdef MEMCHECK
#include "memchk.h2"
#endif

#endif
