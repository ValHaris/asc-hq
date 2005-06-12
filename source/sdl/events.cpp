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



#include <queue>
#include <SDL.h>
#include <SDL_thread.h>

#include "ctype.h"

#include "../events.h"
#include "../stack.h"
#include "../basegfx.h"
#include "../global.h"
#include "keysymbols.h"


volatile tmousesettings mouseparams;

SDL_mutex* keyboardmutex = NULL;
SDL_mutex* eventHandlingMutex = NULL;
SDL_mutex* eventQueueMutex = NULL;

queue<tkey>   keybuffer_sym;
queue<Uint32> keybuffer_prnt;
queue<SDL_Event> eventQueue;
bool _queueEvents = false;


int exitprogram = 0;


/***************************************************************************
 *                                                                         *
 *   Mouse handling routines                                               *
 *                                                                         *
 ***************************************************************************/


const int mouseprocnum = 10;
tsubmousehandler* pmouseprocs[ mouseprocnum ];
bool redrawScreen = false;

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




void mousevisible( int an)
{}


int getmousestatus ()
{
   return 2;
}

void callsubhandler ( void )
{
   for ( int i = 0; i < mouseprocnum; i++ )
      if ( pmouseprocs[i] )
         pmouseprocs[i]->mouseaction();
}

int mouseTranslate ( int m)
{

   const int mousetranslate[3] = {
      0, 2,1
   } ;  // in DOS  right button is 1 and center is 2

   if ( m >= 3 )
      return m;
   else
      return mousetranslate[m];
}



void setmouseposition ( int x, int y )
{}



void setnewmousepointer ( void* picture, int hotspotx, int hotspoty )
{}


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



/***************************************************************************
 *                                                                         *
 *   Keyboard handling routines                                            *
 *                                                                         *
 ***************************************************************************/



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


char  skeypress(tkey keynr)
{
   Uint8 *keystate = SDL_GetKeyState ( NULL );
   return keystate[ keynr ];
}




void wait(void)
{}



tkey char2key(int c )
{
   if ( c < 128 )
      return tolower(c);
   else
      return ct_invvalue;
}

/*
char *get_key(tkey keynr)
{
   return "not yet implemented";
}
*/

int  releasetimeslice( void )
{
   SDL_Delay(10);
   if ( redrawScreen ) {
      redrawScreen = false;
      copy2screen();
   }
   return 0;
}








/***************************************************************************
 *                                                                         *
 *   Timer routines                                                        *
 *                                                                         *
 ***************************************************************************/



volatile int  ticker = 0; // was static, but I think this needs to be global somewhere

void ndelay(int time)
{
   long l;

   l = ticker;
   do {
      releasetimeslice();
   }  while (ticker - l > time);
}


int tticker = 0;

void starttimer(void)
{
   tticker = ticker;
}

char time_elapsed(int time)
{
   if (tticker + time <= ticker) return 1;
   else return 0;
}



/***************************************************************************
 *                                                                         *
 *   Event handling routines                                               *
 *                                                                         *
 ***************************************************************************/

//! The handle for the second thread; depending on platform this could be the event handling thread or the game thread
SDL_Thread* secondThreadHandle = NULL;


int closeEventThread = 0;

const int keyTranslationNum = 7;
int keyTranslation[keyTranslationNum][2] = { { 228, 132 }, //   "a
                                             { 246, 148 }, //   "o
                                             { 252, 129 }, //   "u
                                             { 196, 142 }, //   "A
                                             { 214, 153 }, //   "O
                                             { 220, 154 }, //   "U
                                             { 223, 225 } }; // sz

int processEvents ( )
{
   SDL_mutexP ( eventHandlingMutex );

   SDL_Event event;
   int result;
   if ( SDL_PollEvent ( &event ) == 1) {
      switch ( event.type ) {
         case SDL_MOUSEBUTTONUP:
         case SDL_MOUSEBUTTONDOWN:
            {
               int taste = mouseTranslate(event.button.button - 1);
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

         case SDL_MOUSEMOTION:
            {
               mouseparams.x = event.motion.x;
               mouseparams.y = event.motion.y;
               mouseparams.x1 = event.motion.x;
               mouseparams.y1 = event.motion.y;
               mouseparams.taste = 0;
               for ( int i = 0; i < 3; i++ )
                  if ( event.motion.state & (1 << i) )
                     mouseparams.taste |= 1 << mouseTranslate(i);
               callsubhandler();
            }
            break;
         case SDL_KEYDOWN:
            {
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

                  int newsym = event.key.keysym.unicode;
                  for ( int i = 0; i < keyTranslationNum; i++ )
                     if ( event.key.keysym.unicode == keyTranslation[i][0] )
                        newsym = keyTranslation[i][1];
                  keybuffer_prnt.push ( newsym );
                  r = SDL_mutexV ( keyboardmutex );
               }
            }
            break;
         case SDL_KEYUP:
         {}
            break;

         case SDL_QUIT:
            exitprogram = 1;
            break;
#ifdef _WIN32_
         case SDL_ACTIVEEVENT:
              // if ( event.active.state == SDL_APPACTIVE )
              //   if ( event.active.gain )
                    redrawScreen = true;
            break;
#endif
      }
      result = 1;
      if ( _queueEvents ) {
         SDL_mutexP( eventQueueMutex );
         eventQueue.push ( event );
         SDL_mutexV( eventQueueMutex );
      }
   } else
      result = 0;

   SDL_mutexV( eventHandlingMutex );
   return result;
}

int eventthread ( void* nothing )
{
   while ( !closeEventThread ) {
      if ( !processEvents() )
         SDL_Delay(10);
      ticker = SDL_GetTicks() / 10;
   }
   return 0;
}

#if defined(_WIN32_) | defined(__APPLE__)
int (*_gamethread)(void *);

int gameThreadWrapper ( void* data )
{
   int res = _gamethread ( data );
   closeEventThread = 1;
   return res;
}
#endif

void initializeEventHandling ( int (*gamethread)(void *) , void *data, void* mousepointer )
{
   mouseparams.xsize = 10;
   mouseparams.ysize = 10;

   keyboardmutex = SDL_CreateMutex ();
   if ( !keyboardmutex ) {
      printf("creating keyboard mutex failed\n" );
      exit(1);
   }

   eventHandlingMutex = SDL_CreateMutex ();
   if ( !eventHandlingMutex ) {
      printf("creating eventHandling mutex failed\n" );
      exit(1);
   }


   eventQueueMutex = SDL_CreateMutex ();
   if ( !eventQueueMutex ) {
      printf("creating eventQueueMutex failed\n" );
      exit(1);
   }


   SDL_EnableUNICODE ( 1 );
   SDL_EnableKeyRepeat ( 250, 30 );

#if defined(_WIN32_) | defined(__APPLE__)
   _gamethread = gamethread;
   secondThreadHandle = SDL_CreateThread ( gameThreadWrapper, data );
   eventthread( NULL );
#else
   secondThreadHandle = SDL_CreateThread ( eventthread, NULL );
   gamethread( data );
   closeEventThread = 1;
#endif

   SDL_WaitThread ( secondThreadHandle, NULL );
}



void queueEvents( bool active )
{
   _queueEvents = active;
   if ( !active ) {
      SDL_mutexP( eventQueueMutex );
      while ( !eventQueue.empty())
         eventQueue.pop();
      SDL_mutexV( eventQueueMutex );
   }
}


bool getQueuedEvent ( SDL_Event& event )
{
   SDL_mutexP( eventQueueMutex );
   if ( !eventQueue.empty() ) {
      event = eventQueue.front();
      eventQueue.pop();
      SDL_mutexV( eventQueueMutex );
      return true;
   }
   SDL_mutexV( eventQueueMutex );
   return false;
}

