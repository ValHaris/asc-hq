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

#include <stdio.h>

#ifndef mousehnd_h
#define mousehnd_h

class tmouserect {
     public:
       int x1, y1;
       int x2, y2;
       tmouserect operator+ ( const tmouserect& b ) const;
};

extern int initmousehandler ( void* pic = NULL );
extern int removemousehandler ( void );
extern void mousevisible( int an );
extern byte getmousestatus ();

extern void setmouseposition ( int x, int y );
extern int  getdoubleclick ( int key );

extern void setinvisiblemouserectangle ( int x1, int y1, int x2, int y2 );
// -1, -1, -1, -1  schaltet die Maus wieder �berall an

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


extern "C" void            mouseintproc2( void );
extern "C" volatile tmousesettings  mouseparams;

extern void addmouseproc ( tsubmousehandler* proc );
extern void removemouseproc ( tsubmousehandler* proc );

extern void pushallmouseprocs ( void );
extern void popallmouseprocs ( void );

extern "C" putmousebackground ( void );
#pragma aux putmousebackground modify [ eax ebx ecx edx edi esi ]

extern "C" putmousepointer ( void );
#pragma aux putmousepointer modify [ eax ebx ecx edx edi esi ]

#define mousedblclickxdif 5
#define mousedblclickydif 5


#endif
