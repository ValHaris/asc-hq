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


extern void initializeEventHandling ( int (*fn)(const void *) , const void *data, void* mousepointer );



/***************************************************************************
 *                                                                         *
 *   Mouse handling routines                                               *
 *                                                                         *
 ***************************************************************************/


class tmouserect {
     public:
       int x1, y1;
       int x2, y2;
       tmouserect operator+ ( const tmouserect& b ) const;
};

extern void mousevisible( int an );
extern int getmousestatus ();

extern void setmouseposition ( int x, int y );

extern void setinvisiblemouserectangle ( int x1, int y1, int x2, int y2 );
// -1, -1, -1, -1  schaltet die Maus wieder ?berall an

extern void setinvisiblemouserectanglestk ( int x1, int y1, int x2, int y2 );
extern void setinvisiblemouserectanglestk ( tmouserect r1 );
// wie oben, jedoch werden die alten Params auf dem Stack gesichert

extern void getinvisiblemouserectanglestk ( void );
// holt die Params wieder vom Stack

extern void setnewmousepointer ( void* picture, int hotspotx, int hotspoty );

extern int mouseinrect ( int x1, int y1, int x2, int y2 );


extern int mouseinrect ( const tmouserect* rect );

struct tmousesettings {
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
};




class tsubmousehandler {
        protected:
           tmouserect offarea;
        public:
           tsubmousehandler ( void ) { offarea.x1 = 0; offarea.y1 = 0; offarea.x2 = 0; offarea.y2 = 0; };
           virtual void mouseaction ( void ) = 0;
           virtual void invisiblerect ( tmouserect newoffarea ) { offarea = newoffarea; };
     };


#ifdef _NOASM_
 extern void mouseintproc2( void );
 extern volatile tmousesettings mouseparams;

#else
 extern "C" void            mouseintproc2( void );
 extern "C" volatile tmousesettings  mouseparams;

 extern "C" void putmousebackground ( void );
 #pragma aux putmousebackground modify [ eax ebx ecx edx edi esi ]

 extern "C" void putmousepointer ( void );
 #pragma aux putmousepointer modify [ eax ebx ecx edx edi esi ]

#endif


extern void addmouseproc ( tsubmousehandler* proc );
extern void removemouseproc ( tsubmousehandler* proc );

extern void pushallmouseprocs ( void );
extern void popallmouseprocs ( void );

#ifdef _DOS_
extern int initmousehandler ( void* pic );
extern void removemousehandler ();
#endif


/***************************************************************************
 *                                                                         *
 *   Keyboard handling routines                                            *
 *                                                                         *
 ***************************************************************************/


#ifdef _DOS_
 #include "dos/keysymbols.h"
#else
 #ifdef _SDL_
  #include "sdl/keysymbols.h"
 #endif
#endif

#ifdef _DOS_
extern void initkeyb();
extern void closekeyb();
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


#ifdef _DOS_
    volatile extern long        ticker;
#else
    extern volatile int ticker;
#endif
    extern void ndelay(int time);

    extern void starttimer(void); //resets Timer
    extern char time_elapsed(int time); //check if time msecs are elapsed, since starttimer
    extern int  releasetimeslice( void );


#endif
