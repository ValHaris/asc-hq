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

#ifndef edmiscH
#define edmiscH

#include <string.h>
#include <stdlib.h>
#include <cstdlib>
#include <ctype.h>

#include "tpascal.inc"
#include "typen.h"
#include "basegfx.h"
#include "mappolygons.h"
#include "misc.h"
#include "newfont.h"
#include "events.h"
#include "loaders.h"
#include "dlg_box.h"
#include "stack.h"
#include "spfst.h"
#include "dialog.h"
#include "strtmesg.h"
#include "pd.h"
#include "palette.h"
#include "mapalgorithms.h"


extern bool mousePressedOnField( const MapCoordinate& pos, const SPoint& mousePos, bool cursorChanged);
extern bool mouseDraggedToField( const MapCoordinate& pos, const SPoint& mousePos, bool cursorChanged);


  typedef struct selectrec {
                  int         srr;
                  int         xpos, ypos;
                  int         maxx,maxy,maxsr;
                  int         maxanz;
                  char     showall;
               } selectrec; 

  typedef struct xy { 
           int      x, y; 
        } xy; 

/*
   typedef struct tpolystructure* ppolystructure;

   typedef struct tpolystructure {
              ppolygon poly;
              pascal_byte place;
              int id;
              ppolystructure next;
        } tpolystruct;

   class   tpolygon_management {
           public :
                 int polygonanz;
                 ppolystructure firstpolygon, lastpolygon;
                 tpolygon_management(void);
                 void addpolygon(ppolygon *poly, int place, int id);
                 void deletepolygon(ppolygon *poly);
            };

   class tmycursor : public tcursor {
         public :
              int sx,sy,ix,iy;
              virtual void getimg  ( void );
              virtual void putimg  ( void );
              virtual void putbkgr ( void );
   };
*/
   class  PolygonEditor {
             Poly_gon& poly;
             void display();
        public:
             PolygonEditor ( Poly_gon& polygon ) : poly( polygon ) {};
             void run(void);
   };
/*
   class  tfillpolygonbodentyp : public tfillpolygonsquarecoord {
        public:
             int tempvalue;
             virtual void initevent ( void );
             virtual void setpointabs ( int x,  int y  );
   };

   class  tfillpolygonunit : public tfillpolygonsquarecoord {
        public:
             int tempvalue;
             virtual void initevent ( void );
             virtual void setpointabs ( int x,  int y  );
   };
*/
   class tputresources : public SearchFields {
                              int resourcetype;
                              int maxresource;
                              int minresource;
                              int maxdst;
                              MapCoordinate centerPos;
                           public:
                               void            init ( int sx , int sy, int dst, int restype, int resmax, int resmin );
                               virtual void    testfield ( const MapCoordinate& mc );
                               tputresources ( GameMap* _gamemap ) : SearchFields ( _gamemap ) {};
                            };

   class tputresourcesdlg : public tdialogbox {
               int resourcetype;
               int restp2;
               int maxresource;
               int minresource;
               int dist;
               int status;
            public:
                void init ( void );
                void run ( void );
                void buttonpressed ( int id );
    };



   extern tkey                     ch;
   extern tfield*                    pf2;
   extern pterraintype      auswahl;
   extern Vehicletype*        auswahlf;
   extern BuildingType*    auswahlb;
   extern int                 auswahls;
   extern ObjectType*      actobject;
   extern int auswahlm;
   extern int auswahlw;
   extern int farbwahl;
   extern int                     altefarbwahl;
   extern tfontsettings         rsavefont;
   extern int                   lastselectiontype;
   // extern ppolygon               pfpoly;
   extern char                tfill,polyfieldmode;
   extern int                    fillx1, filly1;
   extern Building*               gbde;
   extern int                    variable;
   extern char                mapsaved;
//   extern tcdrom                 cdrom;


//* fr SelectNr

#define cselbodentyp 1                 //* terrain
#define cselunit 2                      //* vehicletypeen
#define cselcolor 3                       //* Farben
#define cselbuilding 4                   //* Buildings
#define cselunitunit 11                  //* Fahrzeuge fr vehicleCargo
#define cselobject 5                   //* Special Objects
#define cselbuildingunit 12              //* Fahrzeuge fr BuildingCargo
#define cselproduction 8               //* Fahrzeuge fr Production
#define cselmine 6                       //* Minen
#define cselweather 7                //* Wetter

extern void         setstartvariables(void);
extern void         exchg(int *a1,int *a2);
extern void         k_savemap(char saveas);
extern void         k_loadmap(void);
extern void         lines(int      x1,int      y,int      x2,int      y2);

extern void         changeunitvalues(Vehicle* ae);
extern void         changeresource(void);
extern void         changebuildingvalues( Building& b);
extern void         changeminestrength(void);
extern void         newmap(void);
extern void         changemapvalues(void);
extern void         cargoEditor ( ContainerBase* container );
extern int          selectfield(int * cx ,int  * cy);
extern void 	     playerchange(void);

//extern void         setpolytemps (int value, ppolygon poly1); //* setzt in diesem Polygon den Tempvalue auf value
extern void         editpolygon (Poly_gon& poly);
// extern int        getpolygon(ppolygon *poly); //return Fehlerstatus

extern tfield* getactfield();


extern void cdplayer( void );

extern int rightmousebox(void);
extern int leftmousebox(void);
extern void selectunitsetfilter ( void );
extern void movebuilding();
extern void unitsettransformation( void );

extern void placemine(void);

//! The class that manages the switching between the primary map (for editing) and the secondary map (for selecting objects and terrain)
class MapSwitcher {
        int active;
        class Mappntr {
           public:
              GameMap* map;
              bool changed;
              int cursorx, cursory;
              Mappntr ( ) : map ( NULL ), changed ( false ) { cursorx=cursory=0;};
        } maps[2];
     public:
        enum Action { select, set };

        Action getDefaultAction ( );
        void toggle ();
        string getName ();
        MapSwitcher ( ) : active ( 0 ) {};
};

extern MapSwitcher mapSwitcher;

//! replaces the terrain and the objects on a map according to a translation table
extern void transformMap();

extern void editArchivalInformation();

//! displays a textmessage comparing the resources of all players 
extern void resourceComparison ( );

//! the function to edit the vehicle type that are allowed to be build by units
extern void unitProductionLimitation();


extern void saveClipboard();
extern void readClipboard();
extern void setweatherall ( int weather  );
extern void editResearch();
extern void editResearchPoints();
extern void generateTechTree();
extern void editTechAdapter();
extern Vehicle* selectUnitFromContainer( ContainerBase* container );
extern Vehicle*  selectUnitFromMap();
extern void resetPlayerData();

extern void placeCurrentItem();


#endif
