//     $Id: guifunctions.h,v 1.1.2.4 2005-12-11 14:42:38 mbickel Exp $
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

#if defined(karteneditor) && !defined(pbpeditor)
#error the mapeditor is not supposed the gui icons  !
#endif


#ifndef guifunctionsH
#define guifunctionsH

#include "guiiconhandler.h"

extern void registerGuiFunctions( GuiIconHandler& handler );
extern void registerReplayGuiFunctions( GuiIconHandler& handler );

//! generic functions that are available in the cargo dialog
extern void registerCargoGuiFunctions( GuiIconHandler& handler );


namespace GuiFunctions {
   extern GuiIconHandler primaryGuiIcons;
}

#endif

