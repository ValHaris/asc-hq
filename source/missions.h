//     $Id: missions.h,v 1.5 2001-01-19 13:33:51 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.4  2000/10/11 14:26:44  mbickel
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
//     Revision 1.3  2000/07/05 10:49:36  mbickel
//      Fixed AI bugs
//      setbuildingdamage event now updates the screen
//
//     Revision 1.2  1999/11/16 03:42:10  tmwilson
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


#ifdef karteneditor
#error The mapeditor should not use missions.h
#endif


#ifndef missions_h
#define missions_h

#include "controls.h"


extern void  checkevents( MapDisplayInterface* md );

extern void  checksingleevent( pevent ev, MapDisplayInterface* md );


extern void  releaseevent(pvehicle     eht,
                          pbuilding    bld,
                          int      action);

extern void  execevent( pevent ev, MapDisplayInterface* md );

extern void  checktimedevents( MapDisplayInterface* md );

extern void  getnexteventtime(void);

extern void  initmissions( void );

extern void mark_polygon_fields_with_connection ( int* data, int mark );
extern int unit_in_polygon ( peventtrigger_polygonentered trigger );


#endif
