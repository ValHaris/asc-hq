//     $Id: mousehnd.h,v 1.8 2000-08-12 12:52:49 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.7  2000/08/12 09:17:31  gulliver
//     *** empty log message ***
//
//     Revision 1.6  2000/05/10 20:56:20  mbickel
//      mouseparams and ticker now volatile under linux too
//
//     Revision 1.5  1999/12/28 21:03:10  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.4  1999/12/07 22:13:23  mbickel
//      Fixed various bugs
//      Extended BI3 map import tables
//
//     Revision 1.3  1999/11/22 18:27:43  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:42:12  tmwilson
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
#ifndef mousehnd_h
#define mousehnd_h

#include <stdio.h>
#include "tpascal.inc"

class tmouserect {
     public:
       int x1, y1;
       int x2, y2;
       tmouserect operator+ ( const tmouserect& b ) const;
};

extern int initmousehandler ( void* pic = NULL );
extern int removemousehandler ( void );
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


#define mousedblclickxdif 5
#define mousedblclickydif 5


#endif
