//     $Id: edmisc.h,v 1.37 2004-09-13 16:56:54 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.36  2004/09/08 19:34:31  mbickel
//      Added multiple reaction fire
//
//     Revision 1.35  2004/07/12 18:15:05  mbickel
//      Lots of tweaks and fixed for more portability and compilation with mingw
//
//     Revision 1.34  2004/05/29 15:07:37  mbickel
//      Fixed maps
//      Fixed crash with asc.cache
//      ai speed up
//
//     Revision 1.33  2004/05/20 14:01:10  mbickel
//      Many bugfixes and new features, among them:
//        - Container.FillUnitsAutomatically = 2
//        - generate Tech Tree
//        - show research info
//        - edit research in mapeditor
//        - limit production to units that can leave a building
//
//     Revision 1.32  2004/05/11 20:22:33  mbickel
//      Readded research system to ASC
//
//     Revision 1.31  2004/01/16 15:33:46  mbickel
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
//     Revision 1.30  2003/04/23 18:31:10  mbickel
//      Fixed: AI problems
//      Improved cheating detection in replay
//
//     Revision 1.29  2003/03/26 19:16:46  mbickel
//      Fixed AI bugs
//      Fixed clipboard bugs
//      Fixed: unable to move units out of some transport
//
//     Revision 1.28  2003/03/20 10:08:29  mbickel
//      KI speed up
//      mapeditor: added clipboard
//      Fixed movement issues
//
//     Revision 1.27  2002/11/01 20:44:53  mbickel
//      Added function to specify which units can be build by other units
//
//     Revision 1.26  2002/10/09 16:58:46  mbickel
//      Fixed to GrafikSet loading
//      New item filter for mapeditor
//
//     Revision 1.25  2002/04/21 21:27:00  mbickel
//      Mapeditor: Fixed crash in "Put Resources"
//      Updating the small map after AI
//      Fixed infinite loop "quit game" after sending signal
//
//     Revision 1.24  2002/04/14 17:21:18  mbickel
//      Renamed global variable pf to pf2 due to name clash with SDL_mixer library
//
//     Revision 1.23  2002/03/17 21:25:18  mbickel
//      Fixed: View unit movement revealed the reaction fire status of enemy units
//      Mapeditor: new function "resource comparison"
//
//     Revision 1.22  2002/02/21 17:06:50  mbickel
//      Completed Paragui integration
//      Moved mail functions to own file (messages)
//
//     Revision 1.21  2001/10/11 10:41:06  mbickel
//      Restructured platform fileio handling
//      Added map archival information to mapeditor
//
//     Revision 1.20  2001/10/02 14:06:28  mbickel
//      Some cleanup and documentation
//      Bi3 import tables now stored in .asctxt files
//      Added ability to choose amoung different BI3 import tables
//      Added map transformation tables
//
//     Revision 1.19  2001/05/24 15:37:51  mbickel
//      Fixed: reaction fire could not be disabled when unit out of ammo
//      Fixed several AI problems
//
//     Revision 1.18  2001/03/30 12:43:16  mbickel
//      Added 3D pathfinding
//      some cleanup and documentation
//      splitted the ai into several files, now located in the ai subdirectory
//      AI cares about airplane servicing and range constraints
//
//     Revision 1.17  2000/11/29 11:05:28  mbickel
//      Improved userinterface of the mapeditor
//      map::preferredfilenames uses now strings (instead of char*)
//
//     Revision 1.16  2000/11/29 09:40:21  mbickel
//      The mapeditor has now two maps simultaneously active
//      Moved memorychecking functions to its own file: memorycheck.cpp
//      Rewrote password handling in ASC
//
//     Revision 1.15  2000/11/21 20:27:02  mbickel
//      Fixed crash in tsearchfields (used by object construction for example)
//      AI improvements
//      configure.in: added some debug output
//                    fixed broken check for libbz2
//
//     Revision 1.14  2000/10/18 14:14:09  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//
//     Revision 1.13  2000/10/14 14:16:04  mbickel
//      Cleaned up includes
//      Added mapeditor to win32 watcom project
//
//     Revision 1.12  2000/10/11 14:26:33  mbickel
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
//     Revision 1.11  2000/08/12 12:52:47  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.10  2000/05/30 18:39:24  mbickel
//      Added support for multiple directories
//      Moved DOS specific files to a separate directory
//
//     Revision 1.9  2000/05/23 20:40:45  mbickel
//      Removed boolean type
//
//     Revision 1.8  2000/05/05 21:15:04  mbickel
//      Added Makefiles for mount/demount and mapeditor
//      mapeditor can now be compiled for linux, but is not running yet
//
//     Revision 1.7  2000/04/27 16:25:22  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.6  2000/03/16 14:06:56  mbickel
//      Added unitset transformation to the mapeditor
//
//     Revision 1.5  2000/03/11 18:22:05  mbickel
//      Added support for multiple graphic sets
//
//     Revision 1.4  1999/12/27 12:59:57  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.3  1999/11/22 18:27:20  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:41:39  tmwilson
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

#ifndef karteneditor
#error this file should only be used in the mapeditor
#endif

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
*/
   class tmycursor : public tcursor {
         public :
              int sx,sy,ix,iy;
              virtual void getimg  ( void );
              virtual void putimg  ( void );
              virtual void putbkgr ( void );
   };

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
                               tputresources ( pmap _gamemap ) : SearchFields ( _gamemap ) {};
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


#define objectcount 4
#define minecount 4

   extern tkey                     ch;
   extern pfield                    pf2;
   extern pterraintype      auswahl;
   extern pvehicletype        auswahlf;
   extern pbuildingtype    auswahlb;
   extern int                 auswahls;
   extern pobjecttype      actobject;
   extern int auswahlm;
   extern int auswahlw;
   extern int auswahld;
   extern int farbwahl;
   extern int                     altefarbwahl;
   extern tfontsettings         rsavefont;
   extern int                   lastselectiontype;
   extern selectrec              sr[10];
   // extern ppolygon               pfpoly;
   extern char                tfill,polyfieldmode;
   extern int                    fillx1, filly1;
   extern pbuilding               gbde;
   extern int                    variable;
   extern char                mapsaved;
   extern tmycursor             mycursor;
   extern tpulldown              pd;
//   extern tcdrom                 cdrom;


//* fÅr SelectNr

#define cselbodentyp 1                 //* terrain
#define cselunit 2                      //* vehicletypeen
#define cselcolor 3                       //* Farben
#define cselbuilding 4                   //* Buildings
#define cselunitunit 11                  //* Fahrzeuge fÅr vehicleCargo
#define cselobject 5                   //* Special Objects
#define cselbuildingunit 12              //* Fahrzeuge fÅr BuildingCargo
#define cselproduction 8               //* Fahrzeuge fÅr Production
#define cselmine 6                       //* Minen
#define cselweather 7                //* Wetter

extern void         repaintdisplay(void);
extern void         setstartvariables(void);
extern void         pdbaroff(void);
extern void         pulldown(void);
extern void         exchg(int *a1,int *a2);
extern void         placebuilding(int colorr,pbuildingtype   buildingtyp,char choose);
extern void         k_savemap(char saveas);
extern void         k_loadmap(void);
extern void         showStatusBar(void);
extern void         lines(int      x1,int      y,int      x2,int      y2);

extern void         changeunitvalues(pvehicle ae);
extern void         changeresource(void);
extern void         changebuildingvalues( Building& b);
extern void         changeminestrength(void);
extern void         newmap(void);
extern void         changemapvalues(void);
extern void         unit_cargo( pvehicle vh );
extern void         building_cargo( pbuilding bld );
extern void         building_production( pbuilding bld );
extern void         freevariables(void);
extern int          selectfield(int * cx ,int  * cy);
extern void 	     playerchange(void);

//extern void         setpolytemps (int value, ppolygon poly1); //* setzt in diesem Polygon den Tempvalue auf value
extern void         editpolygon (Poly_gon& poly);
// extern int        getpolygon(ppolygon *poly); //return Fehlerstatus
extern void        showpalette(void);
extern void        pdsetup(void);

extern void cdplayer( void );

extern int rightmousebox(void);
extern int leftmousebox(void);
extern void selectunitsetfilter ( void );
extern void movebuilding();
extern void unitsettransformation( void );

extern void placebodentyp(void);
extern void placeunit(void);
extern void placeobject(void);
extern void placemine(void);
extern void putactthing ( void );

//! The class that manages the switching between the primary map (for editing) and the secondary map (for selecting objects and terrain)
class MapSwitcher {
        int active;
        class Mappntr {
           public:
              pmap map;
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

class ClipBoard {
      tmemorystreambuf buf;
      enum Type { ClipVehicle, ClipBuilding };
      int objectNum; 
   public:
      ClipBoard();
      void clear();
      void addUnit ( pvehicle unit );
      void addBuilding ( pbuilding bld );
      Vehicle* pasteUnit();
      Vehicle* pasteUnit( tnstream& stream);
      void place ( const MapCoordinate& pos );

      void write( tnstream& stream );
      void read( tnstream& stream );
};

extern ClipBoard clipBoard;

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



#endif
