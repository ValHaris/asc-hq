//     $Id: edselfnt.h,v 1.4 2000-08-06 11:39:09 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.3  1999/12/27 13:00:00  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.2  1999/11/16 03:41:41  tmwilson
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

#define selfontyanf 40       						//Auswahlanzeige Yanf
#define selfontxsize 280 						//Breite des Auswahlbalkens am Rand
#define selfontxanf ( agmp->resolutionx - selfontxsize - 10 )  	//Auswahlanzeige Xanf

extern void selterraintype( tkey ench );
extern void selvehicletype(tkey ench );
extern void selcolor( tkey ench );
extern void selobject( tkey ench );
extern void selmine( tkey ench );
extern void selweather( tkey ench );
extern void selbuilding ( tkey ench );
extern void selunitcargo( pvehicle transport );
extern void selbuildingcargo( pbuilding eht );
extern void selbuildingproduction( pbuilding eht );
extern void showallchoices(void);

extern void setnewvehicleselection ( pvehicletype v );
extern void setnewterrainselection ( pterraintype t );
extern void setnewobjectselection  ( pobjecttype o );

extern void resetvehicleselector ( void );
extern void resetbuildingselector ( void );

extern void checkselfontbuttons(void);
