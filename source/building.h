/*! \file building.h
    \brief Just the interface for the function starting the UI dialog that handles buildings and transports.

   #building.cpp contains the implementation of the user interface as well
   as the implementation of the base logic, which is defined in 
   #building_controls.h
*/

//     $Id: building.h,v 1.19 2001-01-28 14:04:06 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.18  2001/01/24 11:53:12  mbickel
//      Fixed some compilation problems with gcc
//
//     Revision 1.17  2000/12/21 11:00:46  mbickel
//      Added some code documentation

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



extern int recursiondepth;
extern void  container ( pvehicle eht, pbuilding bld );


#endif
