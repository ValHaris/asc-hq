/***************************************************************************
                          sdlgraph.h  -  description
                             -------------------
    begin                : Sat Dec 18 1999
    copyright            : (C) 1999 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef graphicsH
#define graphicsH

#include "../global.h"
#include "../tpascal.inc"
#include "../palette.h"
#include "../basegfx.h"
#include "../graphics/surface.h"



//*********** Initialization ************

extern int reinitgraphics(int modenum);
extern int initgraphics ( int x, int y, int depth, SDLmm::Surface* icon = NULL );
             // returns > 0  modenum to reestablish this mode
             //         < 0 : error

extern void initASCGraphicSubsystem ( SDL_Surface* screen, SDLmm::Surface* icon );

extern void  closegraphics ( void );



//*********** Misc ************


extern int copy2screen( void );
extern int copy2screen( int x1, int y1, int x2, int y2 );

extern void setdisplaystart( int x, int y);
extern void set_vgapalette256 ( dacpalette256 pal );

extern int graphicinitialized;

extern int isfullscreen ( void );

extern SDL_Surface* getScreen();

extern Surface& getActiveSurface();

#endif
