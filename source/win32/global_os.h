//     $Id: global_os.h,v 1.2 2000-10-14 10:52:56 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.1  2000/10/12 21:37:57  mbickel
//      Further restructured platform dependant routines
//
//     Revision 1.1  2000/10/12 20:21:43  mbickel
//      Restructuring operating system dependant files
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

// this file defines operating system specific macros and is only included by
// global.h

#ifdef __BORLANDC__
 #include "borland/global_os.h"
#else
 #ifdef __WATCOM_CPLUSPLUS__
  #include "watcom/global_os.h"
 #else
  #ifdef _MSC_VER
   #include "msvc/global_os.h"
  #endif
 #endif
#endif
