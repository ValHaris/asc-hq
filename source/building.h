//     $Id: building.h,v 1.17 2000-12-21 11:00:46 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.16  2000/09/17 15:20:29  mbickel
//      AI is now automatically invoked (depending on gameoptions)
//      Some cleanup
//
//     Revision 1.15  2000/08/30 14:45:07  mbickel
//      ASC compiles and links with GCC again...
//
//     Revision 1.14  2000/08/29 20:21:06  mbickel
//      Tried to make source GCC compliant, but some problems still remain
//
//     Revision 1.13  2000/08/29 17:42:41  mbickel
//      Restructured GUI to make it compilable with VisualC.
//
//     Revision 1.12  2000/08/28 19:49:38  mbickel
//      Fixed: replay exits when moving satellite out of orbiter
//      Fixed: airplanes being able to endlessly takeoff and land
//      Fixed: buildings constructable by unit without resources
//
//     Revision 1.11  2000/08/12 09:17:18  gulliver
//     *** empty log message ***
//
//     Revision 1.10  2000/08/08 09:47:56  mbickel
//
//      speed up of dialog boxes in linux
//      fixed graphical errors in attack
//      fixed graphical error in ammo transfer
//      fixed reaction fire not allowing manual attack
//
//     Revision 1.9  2000/06/19 20:05:04  mbickel
//      Fixed crash when transfering ammo to vehicle with > 8 weapons
//
//     Revision 1.8  2000/04/27 16:25:16  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.7  2000/01/02 19:47:05  mbickel
//      Continued Linux port
//      Fixed crash at program exit
//
//     Revision 1.6  1999/12/30 20:30:23  mbickel
//      Improved Linux port again.
//
//     Revision 1.5  1999/12/28 21:02:42  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.4  1999/12/27 12:59:44  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.3  1999/11/25 22:00:00  mbickel
//      Added weapon information window
//      Added support for primary offscreen frame buffers to graphics engine
//      Restored file time handling for DOS version
//
//     Revision 1.2  1999/11/16 03:41:12  tmwilson
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
 #error the mapeditor should not need to use building* !
#endif


#ifndef building_h
#define building_h

/*! \file building.h
   Just the function for starting the dialog for handling buildings and 
   transports.

   #building.cpp contains the implementation of the user interface as well
   as the implementation of the base logic, which is defined in 
   #building_controls.h
*/


#include "gui.h"
#include "controls.h"
#include "gamedlg.h"





extern int recursiondepth;
extern void  container ( pvehicle eht, pbuilding bld );


#endif
