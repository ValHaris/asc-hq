/***************************************************************************
                          mousesdl.cpp  -  description
                             -------------------
    begin                : Sun Dec 19 1999
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

//     $Id: events.cpp,v 1.4 2000-01-01 19:04:20 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.3  1999/12/30 20:30:44  mbickel
//      Improved Linux port again.
//
//     Revision 1.2  1999/12/29 17:38:22  mbickel
//      Continued Linux port
//
//     Revision 1.1  1999/12/28 21:03:31  mbickel
//      Continued Linux port
//      Added KDevelop project files
//


#include <queue>

#include "../misc.h"
#include "../mousehnd.h"
#include "../keybp.h"
#include "../timer.h"

#include "SDL.h"
#include "SDL_thread.h"

/* Data touched at mouse callback time -- they are in a structure to
	simplify calculating the size of the region to lock.
*/


static tmousesettings mouseparams;

SDL_mutex* keyboardmutex = NULL;

queue<tkey>   keybuffer_sym;
queue<Uint32> keybuffer_prnt;


int eventthreadinitialized = 0;

int mouse_in_off_area ( void )
{
   if ( mouseparams.off.x1 == -1     ||   mouseparams.off.y1 == -1 )
      return 0;
   else
      return ( mouseparams.x1+mouseparams.xsize >= mouseparams.off.x1  &&
               mouseparams.y1+mouseparams.ysize >= mouseparams.off.y1   &&
               mouseparams.x1 <= mouseparams.off.x2  &&
               mouseparams.y1 <= mouseparams.off.y2 ) ;
}




void mousevisible( int an) {
}

byte getmousestatus ()
{
   if ( eventthreadinitialized )
      return 2;
   else
      return 0;
}


int eventhandler ( void* nothing )
{
	SDL_Event event;
	while ( 1 ) {
      SDL_WaitEvent ( &event );
      switch ( event.type ) {
         case SDL_MOUSEBUTTONUP:
         case SDL_MOUSEBUTTONDOWN: {
            int taste = event.button.button - 1;
            int state = event.button.type == SDL_MOUSEBUTTONDOWN;

            if ( state )
               mouseparams.taste |= (1 << taste);
            else
               mouseparams.taste &= ~(1 << taste);
            mouseparams.x = event.button.x;
            mouseparams.y = event.button.y;
         }
         break;

         case SDL_MOUSEMOTION: {
            mouseparams.x = event.motion.x;
            mouseparams.y = event.motion.y;
            mouseparams.taste = event.motion.state;
         }
         break;
         case SDL_KEYDOWN: {
         	int r = SDL_mutexP ( keyboardmutex );
         	if ( !r ) {
         	   tkey key = event.key.keysym.sym;
         	   if ( event.key.keysym.mod & KMOD_ALT )
         	      key |= ct_altp;
         	   if ( event.key.keysym.mod & KMOD_CTRL )
         	      key |= ct_stp;
         	   if ( event.key.keysym.mod & KMOD_SHIFT )
         	      key |= ct_shp;
         	   keybuffer_sym.push ( key );
         	   keybuffer_prnt.push ( event.key.keysym.unicode );
         	   r = SDL_mutexV ( keyboardmutex );
         	}
         }
         break;
         case SDL_KEYUP: {
         }
         break;

         case SDL_QUIT: {
                 printf("Quit requested, quitting.\n");
                 exit(0);
         }
         break;
      }
   }
   return 0;
}

SDL_Thread* eventthread = NULL;

int initeventthread ( void )
{
   if ( !eventthreadinitialized ) {
      keyboardmutex = SDL_CreateMutex ();
      if ( !keyboardmutex ) {
         printf("creating keyboard mutex failed\n" );
      	exit(1);
      }
      SDL_EnableUNICODE ( 1 );
      eventthread = SDL_CreateThread ( eventhandler, NULL );
      eventthreadinitialized = 1;
   }

}

int initmousehandler ( void* pic )
{
   initeventthread();
   return(0);
}


int removemousehandler ( void )
{
    return(0);
}

void setmouseposition ( int x, int y )
{
}


void setinvisiblemouserectanglestk ( int x1, int y1, int x2, int y2 )
{
}

void setinvisiblemouserectanglestk ( tmouserect r1 )
{
}


void checkformouseinchangingrectangles( int frst, int scnd )
{
}

void getinvisiblemouserectanglestk ( void )
{
}


void setinvisiblemouserectangle ( int x1, int y1, int x2, int y2 )
{
}



void setnewmousepointer ( void* picture, int hotspotx, int hotspoty )
{
}

int mouseinrect ( int x1, int y1, int x2, int y2 )
{
   if ( mouseparams.x >= x1  && mouseparams.y >= y1  && mouseparams.x <= x2 && mouseparams.y <= y2 )
      return 1;
   else
      return 0;
}

int mouseinrect ( const tmouserect* rect )
{
   if ( mouseparams.x >= rect->x1  && mouseparams.y >= rect->y1  && mouseparams.x <= rect->x2 && mouseparams.y <= rect->y2 )
      return 1;
   else
      return 0;
}



void addmouseproc ( tsubmousehandler* proc )
{
}

void removemouseproc ( tsubmousehandler* proc )
{
}

void pushallmouseprocs ( void )
{
}


void popallmouseprocs ( void )
{
}


tmouserect tmouserect :: operator+ ( const tmouserect& b ) const
{
   tmouserect c;
   c.x1 = x1 + b.x1;
   c.y1 = y1 + b.y1;
   c.x2 = x2 + b.x2;
   c.y2 = y2 + b.y2;
   return c;
}

class tinitmousehandler {
        public:
           tinitmousehandler ( void );
       } init_mousehandler;

tinitmousehandler :: tinitmousehandler ( void ) {
   memset ( &mouseparams, 0 , sizeof ( mouseparams ));
};



int keypress( void )
{
   int result = 0;
  	int r = SDL_mutexP ( keyboardmutex );
  	if ( !r ) {
  		result = !keybuffer_sym.empty ( );
  	   r = SDL_mutexV ( keyboardmutex );
  	}
   return result;
}


tkey r_key(void)
{
	int found = 0;
   tkey key;
  	do {
      int r = SDL_mutexP ( keyboardmutex );
    	if ( !r ) {
    	   if ( !keybuffer_sym.empty() ) {
            key = keybuffer_sym.front();
            keybuffer_sym.pop();
            keybuffer_prnt.pop();
            found++;
         }
         r = SDL_mutexV ( keyboardmutex );
    	}
    	if (!found ) {
      	int t = ticker;
      	while ( t + 5 > ticker );
      }	
   } while ( !found ); 	
   return key;
}

int rp_key(void)
{
	int found = 0;
   tkey key;
  	do {
      int r = SDL_mutexP ( keyboardmutex );
    	if ( !r ) {
    	   if ( !keybuffer_prnt.empty() ) {
            key = keybuffer_prnt.front();
            keybuffer_sym.pop();
            keybuffer_prnt.pop();
            found++;
         }
         r = SDL_mutexV ( keyboardmutex );
    	}
    	if (!found ) {
      	int t = ticker;
      	while ( t + 5 > ticker );
      }	
   } while ( !found ); 	
   return key;
}

void getkeysyms ( tkey* keysym, int* keyprnt )
{
	int found = 0;
  	do {
      int r = SDL_mutexP ( keyboardmutex );
    	if ( !r ) {
    	   if ( !keybuffer_prnt.empty() ) {
            *keysym = keybuffer_sym.front();
            *keyprnt = keybuffer_prnt.front();
            keybuffer_sym.pop();
            keybuffer_prnt.pop();
            found++;
         }
         r = SDL_mutexV ( keyboardmutex );
    	}
    	if (!found ) {
      	int t = ticker;
      	while ( t + 5 > ticker );
      }	
   } while ( !found ); 	
}


void initkeyb(void)
{
   initeventthread();
}


void  closekeyb(void)
{
}


char  skeypress(tkey keynr)
{
   Uint8 *keystate = SDL_GetKeyState ( NULL );
   return keystate[ keynr ];
}





int getch(void)
{
  return 0;
}

int kbhit(void)
{
  return 0;
}

void wait(void)
{
}

void set_keypress(void)
{
}

void reset_keypress(void)
{
}

tkey char2key(int c )
{
  if ( c < 128 )
    return c;
  else
    return -1;
}

char *get_key(tkey keynr)
{
   return "not yet implemented";
}
