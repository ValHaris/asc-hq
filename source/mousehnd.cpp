//     $Id: mousehnd.cpp,v 1.3 1999-11-22 18:27:42 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  1999/11/16 03:42:11  tmwilson
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

/*
	MOUSE.C - The following program demonstrates how
	to use the mouse interrupt (0x33) with DOS/4GW.

	Compile and link: wcl386 /l=dos4g mouse
*/
#ifdef _DOS_

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <i86.h>
#include <malloc.h>
#include <string.h>

#include "tpascal.inc"
#include "basegfx.h"
#include "misc.h"
#include "mousehnd.h"
#include "stack.h"


/* Data touched at mouse callback time -- they are in a structure to
	simplify calculating the size of the region to lock.
*/


#ifdef _NOASM_
 void mouseintproc2( void );
 volatile tmousesettings mouseparams;

 int handleractive;

#else

 extern "C" int handleractive;

#endif

#define mouseprocnum 10
tsubmousehandler* pmouseprocs[ mouseprocnum ];

int lock_region (void *address, unsigned length)
{
    union REGS regs;
    unsigned linear;

    /* Thanks to DOS/4GW's zero-based flat memory model, converting
	    a pointer of any type to a linear address is trivial.
    */
    linear = (unsigned) address;

    regs.w.ax = 0x600;			/* DPMI Lock Linear Region */
    regs.w.bx = (unsigned short) (linear >> 16); /* Linear address in BX:CX */
    regs.w.cx = (unsigned short) (linear & 0xFFFF);
    regs.w.si = (unsigned short) (length >> 16); /* Length in SI:DI */
    regs.w.di = (unsigned short) (length & 0xFFFF);
    int386 (0x31, &regs, &regs);
    return (! regs.w.cflag);		/* Return 0 if can't lock */
}





#pragma off (check_stack)

void callsubhandler ( void ) 
{
     for ( int i = 0; i < mouseprocnum; i++ )
        if ( pmouseprocs[i] ) 
           pmouseprocs[i]->mouseaction();
}

void callsubhandler_invrect ( tmouserect offarea ) 
{
     for ( int i = 0; i < mouseprocnum; i++ )
        if ( pmouseprocs[i] ) 
           pmouseprocs[i]->invisiblerect( offarea );
}


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


#ifdef _NOASM_

void putmousebackground ( void )
{
//   putimage ( mouseparams.altx, mouseparams.alty, mouseparams.background );
}


void putmousepointer ( void )
{
   /*
   getimage ( mouseparams.x1, mouseparams.y1, mouseparams.x1 + mouseparams.xsize, mouseparams.y1 + mouseparams.ysize, mouseparams.background );
   putimage ( mouseparams.x1, mouseparams.y1, mouseparams.pictpointer );
   mouseparams.altx = mouseparams.x1;
   mouseparams.alty = mouseparams.y1;
   */
}



void _loadds far click_handler (int max, int mbx, int mcx, int mdx,
				int msi, int mdi)
#pragma aux click_handler parm [EAX] [EBX] [ECX] [EDX] [ESI] [EDI]
{
  if (handleractive==0) {
     handleractive = 1;

     mouseparams.taste = mbx & 0xff;
     mouseparams.x1 = mcx & 0xffff;
     mouseparams.y1 = mdx & 0xffff;

     callsubhandler();

     if ( mouseparams.x1 + mouseparams.xsize > hgmp->resolutionx )
        mouseparams.x1 = hgmp->resolutionx - mouseparams.xsize;
     if ( mouseparams.y1 + mouseparams.ysize > hgmp->resolutiony )
        mouseparams.y1 = hgmp->resolutiony - mouseparams.ysize;

     mouseparams.x = mouseparams.x1 + mouseparams.hotspotx;
     mouseparams.y = mouseparams.y1 + mouseparams.hotspoty;

     if ( mouseparams.status == 2 )
        if ( !mouse_in_off_area() ) {
          if ( mouseparams.x1 != mouseparams.altx  || mouseparams.y1 != mouseparams.alty ) {
              if ( mouseparams.altx != -1 )
                 putmousebackground();
              putmousepointer();
             
          }
        } else
          if ( mouseparams.altx != -1 )
             putmousebackground();

     handleractive = 0;
  }
}

#else
void _loadds far click_handler ( void )
{
  if (handleractive==0) {
     handleractive = 1;
     mouseintproc2();

     callsubhandler();

     if ( mouseparams.x1 + mouseparams.xsize > hgmp->resolutionx )
        mouseparams.x1 = hgmp->resolutionx - mouseparams.xsize;
     if ( mouseparams.y1 + mouseparams.ysize > hgmp->resolutiony )
        mouseparams.y1 = hgmp->resolutiony - mouseparams.ysize;

     mouseparams.x = mouseparams.x1 + mouseparams.hotspotx;
     mouseparams.y = mouseparams.y1 + mouseparams.hotspoty;

     if ( mouseparams.status == 2 )
        if ( !mouse_in_off_area() ) {
          if ( mouseparams.x1 != mouseparams.altx  || mouseparams.y1 != mouseparams.alty ) {
             if ( mouseparams.altx != -1 )
                putmousebackground();
             putmousepointer();
          }
        } else
          if ( mouseparams.altx != -1 )
             putmousebackground();

     handleractive = 0;
  }
}


#endif

void cbc_end (void) /* Dummy function so we can */
{		    /* calculate size of code to lock */
}		    /* (cbc_end - click_handler) */
#pragma on (check_stack)



void mousevisible( int an) {
  if (an) {
     if ( mouseparams.status == 1) {
        handleractive = 1;
        putmousepointer();
        mouseparams.status = 2;
        handleractive = 0;
        callsubhandler();
     }
  } else {
     if ( mouseparams.status == 2 ) {
        callsubhandler();
        handleractive = 1;
        mouseparams.status = 1;
        putmousebackground(); 
        handleractive = 0;
     }
  }
}

byte getmousestatus () {
   return mouseparams.status;
}


int initmousehandler ( void* pic )
{
    for (int j = 0; j < mouseprocnum; j++) 
       pmouseprocs[j] = NULL;

    handleractive = 0;


    struct SREGS sregs;
    union REGS inregs, outregs;
    int installed = 0;
//    void (far *function_ptr)();
    FILE *fp;
    int pictsize;
    word *wp;

    segread(&sregs);


    /* check for mouse driver */

    inregs.w.ax = 0;
    int386 (0x33, &inregs, &outregs);
    installed = (outregs.w.ax == 0xffff) ;

    if (installed)
    {
       if ( !pic ) {
          /* Load MousePointer iamge file */
          char filename[] = "mouse1.RAW";
          fp = fopen(filename, "rb");
          pictsize = filesize ( filename );
          mouseparams.pictpointer = asc_malloc( pictsize );
          fread ( mouseparams.pictpointer, 1, pictsize , fp );
          fclose ( fp );
       } else
           mouseparams.pictpointer = pic;

        wp = (word*) mouseparams.pictpointer;
        mouseparams.xsize = *wp + 1;
        mouseparams.ysize = *(++wp) + 1;

        mouseparams.backgroundsize = imagesize (0,0,mouseparams.xsize, mouseparams.ysize );
        mouseparams.background = asc_malloc( mouseparams.backgroundsize );
        mouseparams.altx = -1;

        mouseparams.off.x1 = -1;
        mouseparams.off.y1 = -1;
        mouseparams.off.x2 = -1;
        mouseparams.off.y2 = -1;
        mouseparams.hotspotx = 0;
        mouseparams.hotspoty = 0;

	/* lock callback code and data (essential under VMM!)
	   note that click_handler, although it does a far return and
	   is installed using a full 48-bit pointer, is really linked
	   into the flat model code segment -- so we can use a regular
	   (near) pointer in the lock_region() call.
	if ( (! lock_region ((void*) &mouseparams, sizeof(mouseparams))) ||
	    (! lock_region ((void near *) click_handler,
	       (char *) cbc_end - (char near *) click_handler)))
	{
	    printf( "locks failed\n" );
	} else {
	*/

       /* get position */
       inregs.w.ax = 0x03;
       int386 (0x33, &inregs, &outregs);
       mouseparams.x1 = outregs.w.cx;
       mouseparams.y1 = outregs.w.dx;
       mouseparams.x = mouseparams.x1;
       mouseparams.y = mouseparams.y1;


        /* hide mouse cursor */
        inregs.w.ax = 0x2;
        int386 (0x33, &inregs, &outregs);
     
               /* set boundaries */
        inregs.w.ax = 0x7;
               inregs.w.cx = 0;
               inregs.w.dx = hgmp-> resolutionx - 1;
        int386 (0x33, &inregs, &outregs);
     
        inregs.w.ax = 0x8;
               inregs.w.cx = 0;
               inregs.w.dx = hgmp-> resolutiony - 1;
        int386 (0x33, &inregs, &outregs);
     
        setmouseposition ( hgmp-> resolutionx / 2, hgmp-> resolutiony / 2 );

     
        /* install click watcher */
               mouseparams.status = 1;
     
     
        inregs.w.ax = 0xC;
        inregs.w.cx = 0x0001 + 0x0002 + 0x0004 + 0x0008 + 0x00010;

#ifndef _NOASM_
        inregs.x.edx = FP_OFF (click_handler);
        sregs.es	 = FP_SEG (click_handler);
        int386x (0x33, &inregs, &outregs, &sregs);
#endif

/*
    void (far *function_ptr)();

	    function_ptr = ( void(__far *)(void) ) click_handler;
	    inregs.x.edx = FP_OFF (function_ptr);
	    sregs.es	    = FP_SEG (function_ptr);

        int386x (0x33, &inregs, &outregs, &sregs);
*/
     } else
        return 1;
       
     return(0);
}


int removemousehandler ( void )
{
    union REGS inregs, outregs;

    if (mouseparams.status ) {
    /* check installation again (to clear watcher) */

    inregs.w.ax = 0;
    int386 (0x33, &inregs, &outregs);
/*/    if (outregs.w.ax == 0xffff)
//	printf( "DONE : Mouse still installed...\n" );
//    else
//	printf( "DONE : Mouse NOT installed...\n" ); */
    } /* endif */
    return(0);
}

void setmouseposition ( int x, int y )
{
    union REGS inregs, outregs;
    inregs.w.ax = 4;
    inregs.w.cx = x;
    inregs.w.dx = y;
    int386 (0x33, &inregs, &outregs);

    /* hide mouse cursor */
    inregs.w.ax = 0x2;
    int386 (0x33, &inregs, &outregs);

    mouseparams.x1 = x;
    mouseparams.y1 = y;
    mouseparams.x = mouseparams.x1 + mouseparams.hotspotx;
    mouseparams.y = mouseparams.y1 + mouseparams.hotspoty;
}


int  getdoubleclick ( int key ) {
    union REGS inregs, outregs;
    inregs.w.ax = 5;
    inregs.w.bx = key;
    int386 (0x33, &inregs, &outregs);


    if (outregs.w.cx > mousedblclickxdif)
       return 0;

    if (outregs.w.dx > mousedblclickydif)
       return 0;

    if (outregs.w.bx > 0)
       return 0;

    if ((outregs.w.ax & ( 1 << key)) == 0)
       return 0;

    return 1;
}


void setinvisiblemouserectanglestk ( int x1, int y1, int x2, int y2 )
{
   npush ( mouseparams.off );
   setinvisiblemouserectangle ( x1, y1, x2, y2 );
}

void setinvisiblemouserectanglestk ( tmouserect r1 )
{
   npush ( mouseparams.off );
   setinvisiblemouserectangle ( r1.x1, r1.y1, r1.x2, r1.y2 );
}   


void checkformouseinchangingrectangles( int frst, int scnd )
{
   if ( mouseparams.status == 2 ) {
      if ( frst ) {
         if ( !scnd )
            putmousepointer();
          else
             if ( mouseparams.altx != -1 )
                putmousebackground();
      } else {
         if ( scnd )
            if ( mouseparams.altx != -1 )
               putmousebackground();

      }
   }

}

void getinvisiblemouserectanglestk ( void )
{
   handleractive = 1;


   int frst = mouse_in_off_area (  );
   npop ( mouseparams.off );
   int scnd = mouse_in_off_area (  );
   checkformouseinchangingrectangles ( frst, scnd );


   // what a mess to get rid of the volatile attribute ....
   // Does anybody know a more elegant way that does work with Watcom C 11 ? Watcom C 10.6 did not care ... 
   tmouserect* mrp = (tmouserect*) &mouseparams.off; 
   tmouserect mr = *mrp; 

   callsubhandler_invrect( mr );

   handleractive = 0;
}


void setinvisiblemouserectangle ( int x1, int y1, int x2, int y2 )
{
   handleractive = 1;

   int frst = mouse_in_off_area();

   mouseparams.off.x1 = x1;
   mouseparams.off.y1 = y1;
   mouseparams.off.x2 = x2;
   mouseparams.off.y2 = y2;

   tmouserect* mrp = (tmouserect*) &mouseparams.off; 
   tmouserect mr = *mrp; 

   callsubhandler_invrect( mr );

   int scnd = mouse_in_off_area();
   checkformouseinchangingrectangles ( frst, scnd );

   handleractive = 0;
}



void setnewmousepointer ( void* picture, int hotspotx, int hotspoty )
{

   if (mouseparams.altx != -1) 
      putmousebackground ();

   mouseparams.pictpointer = picture;
   mouseparams.hotspotx = hotspotx;
   mouseparams.hotspoty = hotspoty;

   word* wp = (word*) mouseparams.pictpointer;
   mouseparams.xsize = *wp + 1;
   mouseparams.ysize = *(++wp) + 1;

   int newsize = imagesize (0,0,mouseparams.xsize, mouseparams.ysize );

   if ( newsize > mouseparams.backgroundsize ) {
      asc_free ( mouseparams.background );
      mouseparams.background = asc_malloc( newsize );
      mouseparams.backgroundsize = newsize ;
   }

   if ( mouseparams.x1 + mouseparams.xsize > hgmp->resolutionx )
      mouseparams.x1 = hgmp->resolutionx - mouseparams.xsize;
   if ( mouseparams.y1 + mouseparams.ysize > hgmp->resolutiony )
      mouseparams.y1 = hgmp->resolutiony - mouseparams.ysize;

   if ( mouseparams.status == 2 )
     if ( ! mouse_in_off_area() ) 
        putmousepointer();

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
   for (int i = 0; i < mouseprocnum ; i++) {
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

class tinitmousehandler {
        public:
           tinitmousehandler ( void );
       } init_mousehandler;

tinitmousehandler :: tinitmousehandler ( void ) {
   tmouserect* mrp = (tmouserect*) &mouseparams; 
   memset ( mrp, 0 , sizeof ( mouseparams ));
};

#endif
