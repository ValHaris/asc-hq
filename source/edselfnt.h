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


extern void checkselfontbuttons(void);
