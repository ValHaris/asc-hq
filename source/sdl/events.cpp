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

//     $Id: events.cpp,v 1.19 2000-10-12 21:37:57 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.18  2000/10/11 14:26:57  mbickel
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
//     Revision 1.17  2000/08/12 15:03:26  mbickel
//      Fixed bug in unit movement
//      ASC compiles and runs under Linux again...
//
//     Revision 1.16  2000/08/12 09:17:41  gulliver
//     *** empty log message ***
//
//     Revision 1.15  2000/06/09 10:51:01  mbickel
//      Repaired keyboard control of pulldown menu
//      Fixed compile errors at fieldlist with gcc
//
//     Revision 1.14  2000/06/01 15:27:47  mbickel
//      Some changes for the upcoming Win32 version of ASC
//      Fixed error at startup: unable to load smalaril.fnt
//
//     Revision 1.13  2000/05/10 20:56:20  mbickel
//      mouseparams and ticker now volatile under linux too
//
//     Revision 1.12  2000/05/10 19:55:57  mbickel
//      Fixed empty loops when waiting for mouse events
//
//     Revision 1.11  2000/04/27 16:25:34  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.10  2000/02/05 12:13:46  steb
//     Sundry tidying up to get a clean compile and run.  Presently tending to SEGV on
//     startup due to actmap being null when trying to report errors.
//
//     Revision 1.9  2000/01/25 19:28:20  mbickel
//      Fixed bugs:
//        invalid mouse buttons reported when moving the mouse
//        missing service weapon in weapon information
//        invalid text displayed in ammo production
//        undamaged units selected in repair vehicle function
//
//      speed up when playing weapon sound
//
//     Revision 1.8  2000/01/07 13:20:07  mbickel
//      DGA fullscreen mode now working
//
//     Revision 1.7  2000/01/06 11:19:16  mbickel
//      Worked on the Linux-port again...
//
//     Revision 1.6  2000/01/04 19:43:54  mbickel
//      Continued Linux port
//
//     Revision 1.5  2000/01/02 19:47:08  mbickel
//      Continued Linux port
//      Fixed crash at program exit
//
//     Revision 1.4  2000/01/01 19:04:20  mbickel
//     /tmp/cvsVhJ4Z3
//
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
#include "ctype.h"

#include "../misc.h"
#include "../mousehnd.h"
#include "../keybp.h"
#include "../timer.h"
#include "../stack.h"

#include "../global.h"
#include sdlheader
#ifdef _WIN32_
 #include "SDL_thread.h"
#else
 #include "SDL/SDL_thread.h"
#endif


/* Data touched at mouse callback time -- they are in a structure to
	simplify calculating the size of the region to lock.
*/


volatile tmousesettings mouseparams = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0, 0, 0, 0 };

SDL_mutex* keyboardmutex = NULL;

std::queue<tkey>   keybuffer_sym;
std::queue<Uint32> keybuffer_prnt;


int eventthreadinitialized = 0;
int closethread = 0;
int exitprogram = 0;

#define mouseprocnum 10
tsubmousehandler* pmouseprocs[ mouseprocnum ];


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

int getmousestatus ()
{
   if ( eventthreadinitialized )
      return 2;
   else
      return 0;
}

void callsubhandler ( void )
{
     for ( int i = 0; i < mouseprocnum; i++ )
        if ( pmouseprocs[i] )
           pmouseprocs[i]->mouseaction();
}

const int mousetranslate[3] = { 0, 2,1 };  // in DOS  right button is 1 and center is 2

int eventhandler ( void* nothing )
{
	SDL_Event event;
	while ( !closethread ) {
      if ( SDL_PollEvent ( &event ) == 1) {
         switch ( event.type ) {
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN: {
               int taste = mousetranslate[event.button.button - 1];
               int state = event.button.type == SDL_MOUSEBUTTONDOWN;
               if ( state )
                  mouseparams.taste |= (1 << taste);
               else
                  mouseparams.taste &= ~(1 << taste);
               mouseparams.x = event.button.x;
               mouseparams.y = event.button.y;
               callsubhandler();
            }
            break;

            case SDL_MOUSEMOTION: {
               mouseparams.x = event.motion.x;
               mouseparams.y = event.motion.y;
               mouseparams.x1 = event.motion.x;
               mouseparams.y1 = event.motion.y;
               mouseparams.taste = 0;
               for ( int i = 0; i < 3; i++ )
                  if ( event.motion.state & (1 << i) )
                     mouseparams.taste |= 1 << mousetranslate[i];
               callsubhandler();
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

            case SDL_QUIT: exitprogram = 1;
            break;
         }
      } else
         SDL_Delay(10);
      ticker = SDL_GetTicks() / 10;
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
   }
   eventthreadinitialized++;
	return 0;
}

int closeeventthread ( void )
{
   if ( eventthreadinitialized ) {
      eventthreadinitialized--;
      if ( !eventthreadinitialized ) {
         closethread = 1;
      }
   }
	return 0; 
}


int initmousehandler ( void* pic )
{
   initeventthread();
   mouseparams.xsize = 10;
   mouseparams.ysize = 10;
   return(0);
}


int removemousehandler ( void )
{
    closeeventthread();
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
   int i;
   for (i = 0; i < mouseprocnum ; i++) {
      if ( !pmouseprocs[i] ) {
         pmouseprocs[i] = proc;
         break;
      }
   } /* endfor */

   if ( i >= mouseprocnum )
      exit(1);
}

void removemouseproc ( tsubmousehandler* proc )
{
   for (int i = 0; i < mouseprocnum ; i++)
      if ( pmouseprocs[i] == proc )
         pmouseprocs[i] = NULL;
}

void pushallmouseprocs ( void )
{
   for (int i = 0; i < mouseprocnum ; i++) {
      npush ( pmouseprocs[i] );
      pmouseprocs[i] = NULL;
   }
}


void popallmouseprocs ( void )
{
   for (int i = 0; i < mouseprocnum ; i++)
      npop ( pmouseprocs[i] );
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

#ifndef _NOASM_

class tinitmousehandler {
        public:
           tinitmousehandler ( void );
       } init_mousehandler;

tinitmousehandler :: tinitmousehandler ( void ) {
   memset ( &mouseparams, 0 , sizeof ( mouseparams ));
};

#endif


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
      	while ( t + 5 > ticker )
      	   releasetimeslice();
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
      	while ( t + 5 > ticker )
      	   releasetimeslice();
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
      	while ( t + 5 > ticker )
      	   releasetimeslice();
      }	
   } while ( !found ); 	
}


void initkeyb(void)
{
   initeventthread();
   SDL_EnableKeyRepeat ( 250, 30 );
}


void  closekeyb(void)
{
   closeeventthread();
}


char  skeypress(tkey keynr)
{
   Uint8 *keystate = SDL_GetKeyState ( NULL );
   return keystate[ keynr ];
}




void wait(void)
{
}


tkey char2key(int c )
{
  if ( c < 128 )
    return tolower(c);
  else
    return ct_invvalue;
}

char *get_key(tkey keynr)
{
   return "not yet implemented";
}
