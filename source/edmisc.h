//     $Id: edmisc.h,v 1.4 1999-12-27 12:59:57 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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

#include <string.h>
#include <stdlib.h>
#ifdef _DOS_
#include <conio.h>
#include <dos.h>
#endif

#include <malloc.h>
#include <ctype.h>

#include "tpascal.inc"
#include "typen.h"
#include "basegfx.h"
#include "weather.h"
#include "misc.h"
#include "newfont.h"
#include "mousehnd.h"
#include "loaders.h"
#include "keybp.h"
#include "dlg_box.h"
#include "stack.h"
#include "spfst.h"
#include "dialog.h"
#include "strtmesg.h"
#include "pd.h"
#include "cdrom.h"
#include "palette.h"

#ifndef karteneditor
#error this file should only be used in the mapeditor
#endif

  typedef struct selectrec {
                  word         srr;
                  word         xpos, ypos;
                  word         maxx,maxy,maxsr;
                  word         maxanz;
                  boolean     showall;
               } selectrec; 

  typedef struct xy { 
           int      x, y; 
        } xy; 

 
   typedef struct tpolystructure* ppolystructure;

   typedef struct tpolystructure {
              ppolygon poly;
              byte place;
              int id;
              ppolystructure next;
        } tpolystruct; 

   class   tpolygon_management {
           public :
                 int polygonanz;
                 ppolystructure firstpolygon, lastpolygon;
                 tpolygon_management(void);
                 void addpolygon(ppolygon *poly, byte place, int id);
                 void deletepolygon(ppolygon *poly);
                 };

   class tmycursor : public tcursor {
         public :
              word sx,sy,ix,iy;
              virtual void getimg  ( void );
              virtual void putimg  ( void );
              virtual void putbkgr ( void );
   };

   class  tchangepoly {
        public:
             ppolygon poly;
             void setpolypoints(byte value);
             void setpolytemps (byte value);
             byte checkpolypoint(int x, int y);
             void deletepolypoint(int x, int y);
             void run(void);
   };

   class  tfillpolygonbodentyp : public tfillpolygonsquarecoord {
        public:
             byte tempvalue;
             virtual void initevent ( void );
             virtual void setpointabs ( int x,  int y  );
   };

   class  tfillpolygonunit : public tfillpolygonsquarecoord {
        public:
             byte tempvalue;
             virtual void initevent ( void );
             virtual void setpointabs ( int x,  int y  );
   };

   class tputresources : public tsearchfields {
                              int resourcetype;
                              int maxresource;
                              int minresource;
                           public:
                               void            init ( int sx , int sy, int dst, int restype, int resmax, int resmin );
                               virtual void    testfield ( void );
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
                void buttonpressed ( char id );
    };


#define objectcount 4
#define minecount 4

   extern tkey                     ch;
   extern pfield                    pf;
   extern pterraintype      auswahl;
   extern pvehicletype        auswahlf;
   extern pbuildingtype    auswahlb;
   extern int                 auswahls;
   extern pobjecttype      actobject;
   extern byte                    auswahlm;
   extern byte                    auswahlw;
   extern byte                    auswahld;
   extern byte                    farbwahl;
   extern int                     altefarbwahl;
   extern tfontsettings         rsavefont;
   extern int                   lastselectiontype;
   extern selectrec              sr[10];
   extern ppolygon               pfpoly;
   extern boolean                tfill,polyfieldmode;
   extern word                    fillx1, filly1;
   extern pbuilding               gbde;
   extern word                    variable;
   extern boolean                mapsaved;
   extern tmycursor             mycursor;
   extern tpulldown              pd;
   extern tcdrom                 cdrom;


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
extern void         exchg(word *a1,word *a2);
extern void         placebuilding(byte colorr,pbuildingtype   buildingtyp,boolean choose);
extern void         k_savemap(boolean saveas);
extern void         k_loadmap(void);
extern void         showcoordinates(void);
extern void         lines(int      x1,int      y,int      x2,int      y2);

extern void         changeunitvalues(pvehicle ae);
extern void         changeresource(void);
extern void         changebuildingvalues(pbuilding *  b);
extern void         changeminestrength(void);
extern void         newmap(void);
extern void         changemapvalues(void);
extern void         unit_cargo( pvehicle vh );
extern void         building_cargo( pbuilding bld );
extern void         building_production( pbuilding bld );
extern void         freevariables(void);
extern int          selectfield(int * cx ,int  * cy);
extern void 	playerchange(void);

extern void         setpolytemps (byte value, ppolygon poly1); //* setzt in diesem Polygon den Tempvalue auf value
extern void         createpolygon (ppolygon *poly, byte place, int id);
/*
  Place 0 : Event
              id  = event_action
*/
extern void        changepolygon (ppolygon poly);
extern byte        getpolygon(ppolygon *poly); //return Fehlerstatus
extern void        showpalette(void);
extern void        pdsetup(void);

extern void cdplayer( void );

extern void placebodentyp(void);
extern void placeunit(void);
extern void placeobject(void);
extern void placemine(void);
extern void putactthing ( void );

extern int rightmousebox(void);
extern int leftmousebox(void);
extern void selectunitsetfilter ( void );
extern void movebuilding();


