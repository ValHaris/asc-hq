//     $Id: timer.cpp,v 1.3 2009-04-18 13:48:38 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  2000/10/18 15:10:07  mbickel
//      Fixed event handling for windows and dos
//
//     Revision 1.1  2000/05/30 18:39:29  mbickel
//      Added support for multiple directories
//      Moved DOS specific files to a separate directory
//
//     Revision 1.3  2000/05/07 12:53:59  mbickel
//      some minor adjustments
//
//     Revision 1.2  1999/11/16 03:42:41  tmwilson
//      Added CVS keywords to most of the files.
//      Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//      Wrote replacement routines for kbhit/getch for Linux
//      Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//      Added autoconf/automake capabilities
//      Added files used by 'automake --gnu'
//
//
/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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

#ifdef _DOS_
#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include "../events.h"
#include <stdlib.h>

#define timerintr 0x08
#define pit_freq 0x1234DD  

    void         (__interrupt __far *biostimerhandler)();
    void         (__interrupt __far *tickerpointer)()=NULL;
    volatile long         ticker,clock_ticks, counter,tticker;
    char         init=0;



void starttimer(void)
{
   tticker = ticker;
}

char time_elapsed(int time)
{
   if (tticker + time <= ticker) return 1;
   else return 0;
}

void ndelay(int time)
{ 
  long l;

   l = ticker; 
   do { 
   }  while ((ticker - l > time));
} 



void  __interrupt __far thandler()
{ 
    ticker++;
    /*
    if ( ticker % 100 == 0 )
      if ( _heapchk() != _HEAPOK ) 
         exit(0);
    */

    if (tickerpointer != NULL) _chain_intr(tickerpointer);

    clock_ticks += counter;
    if (clock_ticks >= 0x10000)
      { 
         clock_ticks = clock_ticks - 0x10000; 
         _chain_intr(biostimerhandler);
      } 
   else 
      outp(0x20,0x20);
} 


void settimer(int freq)
{ 
   biostimerhandler = _dos_getvect(timerintr);
   _dos_setvect(timerintr,thandler);
   clock_ticks = 0; 
   counter = 0x1234DD / freq;
   outp(0x43,0x34);
   outp(0x40,counter % 256);
   outp(0x40,counter / 256);
} 


void inittimer(int frequence)
{ 
   if (init != 0) return;
   settimer(frequence);
   init = 1; 
} 


void closetimer(void)
{ 
   if (init == 0) return;
   outp(0x43,0x34);
   outp(0x40,0);
   outp(0x40,0);
   _dos_setvect(timerintr,biostimerhandler);
}

int  releasetimeslice( void )
{
/*    union REGS inregs, outregs;
    inregs.w.ax = 0x1680;
    int386 (0x2f, &inregs, &outregs);
    return outregs.w.ax == 0; */
return 0;
}

#endif
