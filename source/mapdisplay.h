/***************************************************************************
                          mapdisplay.h  -  description
                             -------------------
    begin                : Wed Jan 24 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef mapdisplay_h_included
 #define mapdisplay_h_included

#include "typen.h"
#include "basegfx.h"
#include "events.h"

 //! displays the map that #actmap points to
extern void  displaymap(void);

extern void  initMapDisplay( );


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
          virtual void cp_buf ( int x1, int y1, int x2, int y2 );

          void  movevehicle( int x1,int y1, int x2, int y2, pvehicle eht, int height1, int height2, int fieldnum, int totalmove );
          void  deletevehicle ( void );

          void resetmovement ( void );

          virtual int getfieldposx ( int x, int y ) ;
          virtual int getfieldposy ( int x, int y ) ;
          tdisplaymap ( void );
     } ;



extern tdisplaymap idisplaymap;
extern int showresources;

extern int   getfieldundermouse ( int* x, int* y );

extern void writemaptopcx ( bool confirmation = true, string filename = "" );

class tlockdispspfld {
      public:
        tlockdispspfld ( void );
        ~tlockdispspfld ();
      };

extern int lockdisplaymap;

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


class MapDisplayInterface {
         public:
           virtual int displayMovingUnit ( int x1,int y1, int x2, int y2, pvehicle vehicle, int height1, int height2, int fieldnum, int totalmove ) = 0;
           virtual void deleteVehicle ( pvehicle vehicle ) = 0;
           virtual void displayMap ( void ) = 0;
           virtual void displayPosition ( int x, int y ) = 0;
           virtual void resetMovement ( void ) = 0;
           virtual void startAction ( void ) = 0;
           virtual void stopAction ( void ) = 0;
           virtual void displayActionCursor ( int x1, int y1, int x2, int y2 ) = 0;
           virtual void removeActionCursor ( void ) = 0;
           virtual ~MapDisplayInterface () {};
       };

class MapDisplay : public MapDisplayInterface {
           dynamic_array<int> cursorstat;
           int cursorstatnum;
         public:
           int displayMovingUnit ( int x1,int y1, int x2, int y2, pvehicle vehicle, int height1, int height2, int fieldnum, int totalmove );
           void deleteVehicle ( pvehicle vehicle );
           void displayMap ( void );
           void displayPosition ( int x, int y );
           void resetMovement ( void );
           void startAction ( void );
           void stopAction ( void );
           void displayActionCursor ( int x1, int y1, int x2, int y2 ) {};
           void removeActionCursor ( void ) {};
    };

extern MapDisplay defaultMapDisplay;

//! return the screencoordinates of the upper left position of the displayed map
extern int getmapposx ( void );

//! return the screencoordinates of the upper left position of the displayed map
extern int getmapposy ( void );


class tbackgroundpict : public tpaintmapborder {
               int lastpaintmode;
               void* dashboard[7];
               void* borderpicture[8];
               void* background;
               int run;
               struct {
                      int x,y;
                 } borderpos[8];
               int inited;
            protected:
               void paintrectangleborder ( void );
            public:
               void init ( int reinit = 0 );
               void load ( void );
               void paint ( int resavebackground = 0 );
               void paintborder ( int x, int y, int reinit );
               void paintborder ( int x, int y );
               tbackgroundpict ( void );
               int getlastpaintmode ( void );
};
extern tbackgroundpict backgroundpict;


#endif