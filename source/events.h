/***************************************************************************
                          events.h  -  description
                             -------------------
    begin                : Wed Oct 18 2000
    copyright            : (C) 2000 by Martin Bickel
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


/***************************************************************************
 *                                                                         *
 *   Event handling routines                                               *
 *                                                                         *
 ***************************************************************************/

#ifndef events_h_included
#define events_h_included

#include <SDL.h>

extern int ASC_GetTicks();



extern void exit_asc( int returnresult );


/***************************************************************************
 *                                                                         *
 *   Keyboard handling routines                                            *
 *                                                                         *
 ***************************************************************************/


 extern bool isKeyPressed(SDL_KeyCode key);

/***************************************************************************
 *                                                                         *
 *   Timer routines                                                        *
 *                                                                         *
 ***************************************************************************/


    extern void ndelay(int time);

    extern void starttimer(void); //resets Timer
    extern bool time_elapsed(int time); //check if time msecs are elapsed, since starttimer
    extern int  releasetimeslice( void );
    

#endif
