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


#ifndef sg_h
#define sg_h


extern void mainloopgeneralmousecheck ( void );
extern void mainloopgeneralkeycheck ( tkey& ch );
extern void         repaintdisplay(void);

#ifdef sgmain
#include "controls.h"
extern cmousecontrol* mousecontrol;
#endif

extern int modenum24;
extern int modenum8;
extern int cdrom;

extern pprogressbar actprogressbar ;


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

extern void* asc_malloc ( size_t size );
extern void asc_free ( void* p );


extern void viewunreadmessages ( void );

#endif

