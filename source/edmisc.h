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

#include "pgpopupmenu.h"

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


extern bool mousePressedOnField( const MapCoordinate& pos, const SPoint& mousePos, bool cursorChanged, int button, int prio );
extern bool mouseDraggedToField( const MapCoordinate& pos, const SPoint& mousePos, bool cursorChanged, int prio );

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



   extern int auswahlm;
   extern int auswahlw;
   extern char                mapsaved;

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
// extern int          selectfield(int * cx ,int  * cy);
extern void 	     playerchange(void);

//extern void         setpolytemps (int value, ppolygon poly1); //* setzt in diesem Polygon den Tempvalue auf value
extern void         editpolygon (Poly_gon& poly);
// extern int        getpolygon(ppolygon *poly); //return Fehlerstatus

extern tfield* getactfield();


extern void cdplayer( void );

extern void selectunitsetfilter ( void );
extern void movebuilding();
extern void unitsettransformation( void );

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
// extern Vehicle* selectUnitFromContainer( ContainerBase* container );
// extern Vehicle*  selectUnitFromMap();
extern void resetPlayerData();

extern void placeCurrentItem();


#endif
