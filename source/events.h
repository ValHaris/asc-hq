/***************************************************************************
                          events.h  -  description
                             -------------------
    begin                : Wed Oct 18 2000
    copyright            : (C) 2000 by Martin Bickel
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


/***************************************************************************
 *                                                                         *
 *   Event handling routines                                               *
 *                                                                         *
 ***************************************************************************/

#ifndef events_h_included
#define events_h_included


/**
  Defines what happens with the SDL events
  \param queue   Events are queued to be extracted with getQueuedEvent
  \param legacy  Events are evaluated and the global legacy structures updated
  \returns the previous state of the event queing 
*/    
extern bool setEventRouting( bool queue, bool legacy );

extern bool legacyEventSystemActive();

extern void initializeEventHandling ( int (*fn)(void *) , void *data, void* mousepointer );

extern SDL_mutex* eventHandlingMutex;

extern void exit_asc( int returnresult );


/***************************************************************************
 *                                                                         *
 *   Mouse handling routines                                               *
 *                                                                         *
 ***************************************************************************/


struct tmouserect {
       int x1, y1;
       int x2, y2;
       tmouserect operator+ ( const tmouserect& b ) const;
//       tmouserect ( int _x1, int _y1, int _x2, int _y2 ) : x1(_x1), y1(_y1), x2(_x2), y2(_y2 ) {};
//       tmouserect(){ x1=0;y1=0;x2=0;y2=0;};
};

extern void mousevisible( int an );
extern int getmousestatus ();

extern void setmouseposition ( int x, int y );

extern void setnewmousepointer ( void* picture, int hotspotx, int hotspoty );

extern int mouseinrect ( int x1, int y1, int x2, int y2 );


extern int mouseinrect ( const tmouserect* rect );

class tmousesettings {
  public:
   int x;
   int y;
   int x1;
   int y1;
   int altx;
   int alty;
   void *background;
   void *pictpointer;
   int xsize;
   int ysize;
   char taste;
   char status;
   tmouserect off;
   int hotspotx;
   int hotspoty;
   int backgroundsize;
   tmousesettings ( ) { x=0;y=0;x1=0;y1=0;altx=0;alty=0;
                        background=NULL;pictpointer=NULL;
                        xsize=0;ysize=0;taste=0;status=0;
                        hotspotx=0;hotspoty=0;backgroundsize=0;
                        off.x1=0;off.y1=0;off.x2=0;off.y2=0;
                      };
};





extern void mouseintproc2( void );
extern volatile tmousesettings mouseparams;


/***************************************************************************
 *                                                                         *
 *   Keyboard handling routines                                            *
 *                                                                         *
 ***************************************************************************/


 #ifdef _SDL_
  #include "sdl/keysymbols.h"
 #endif

 typedef int tkey;

 extern char  skeypress( tkey keynr);
 extern char *get_key(tkey keynr);
 extern tkey r_key(void);
 extern int  rp_key(void);
 extern int keypress(void);
 extern void wait(void);
 extern tkey char2key (int ch);
 extern void getkeysyms ( tkey* keysym, int* keyprnt );

 extern int exitprogram;


/***************************************************************************
 *                                                                         *
 *   Timer routines                                                        *
 *                                                                         *
 ***************************************************************************/


    extern volatile int ticker;
    extern void ndelay(int time);

    extern void starttimer(void); //resets Timer
    extern char time_elapsed(int time); //check if time msecs are elapsed, since starttimer
    extern int  releasetimeslice( void );


    //! if the events are being queue, get one. \returns false if no event available
    extern bool getQueuedEvent ( SDL_Event& event );

    //! gets the next event without removing it from the queue. \returns false if no event available
    extern bool peekEvent ( SDL_Event& event );

    

#endif
