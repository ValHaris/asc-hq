//     $Id: sg.cpp,v 1.3 1999-11-16 17:04:09 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  1999/11/16 03:42:25  tmwilson
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

#include "config.h"
#ifdef _DOS_
#include <i86.h>
#include <conio.h>
#include <dos.h>
#endif

#include <stdio.h>                                  
#include <string.h>
#include <stdlib.h>
#include <new.h>
#include <malloc.h>
#include <ctype.h>

#include "tpascal.inc"
#include "vesa.h"
#include "misc.h"
#include "loadpcx.h"
#include "newfont.h"
#include "mousehnd.h"
#include "typen.h"
#include "keybp.h"
#include "spfst.h"
#include "loaders.h"
#include "dlg_box.h"
#include "stack.h"
#include "missions.h"
#include "controls.h"
#include "dlg_box.h"
#include "dialog.h"
#include "gui.h"
#include "timer.h"
#include "pd.h"
#include "strtmesg.h"
#include "weather.h"
#include "gamedlg.h"
#include "network.h"
#include "building.h"
#include "cdrom.h"
#include "loadjpg.h"
#include "sg.h"
#include "artint.h"

#ifndef HAVE_STRICMP
#define strcmpi strcasecmp
#define strncmpi strncasecmp
#define strnicmp strncasecmp
#endif

#ifdef HEXAGON
#include "loadbi3.h"
#endif




// #define MEMCHK

extern int startreplaylate;

class tsgonlinemousehelp : public tonlinemousehelp {
public:
  tsgonlinemousehelp ( void );
};

tsgonlinemousehelp :: tsgonlinemousehelp ( void )
{
   helplist.num = 12;

   static tonlinehelpitem sghelpitems[12]  = {{ 498, 26, 576, 36, 20001 },
                                              { 498, 41, 576, 51, 20002 },
                                              { 586, 26, 612, 51, 20003 },
                                              { 499, 57, 575, 69, 20004 },
                                              { 499, 70, 575, 81, 20005 },
                                              { 577, 58, 610, 68, 20006 },
                                              { 577, 70, 610, 80, 20007 },
                                              { 502, 92, 531,193, 20008 },
                                              { 465, 92, 485,194, 20009 },
                                              { 551, 92, 572,193, 20010 },
                                              { 586, 90, 612,195, 20011 },
#ifdef FREEMAPZOOM
                                              { 473,agmp->resolutiony - ( 480 - 449 ), 601,agmp->resolutiony - ( 480 - 460 ), 20016 }};
#else
                                              { 473,agmp->resolutiony - ( 480 - 449 ), 601,agmp->resolutiony - ( 480 - 460 ), 20012 }};
#endif

   for ( int i = 0; i< helplist.num; i++ ) {
      sghelpitems[i].rect.x1 = agmp->resolutionx - ( 640 - sghelpitems[i].rect.x1 );
      sghelpitems[i].rect.x2 = agmp->resolutionx - ( 640 - sghelpitems[i].rect.x2 );
   }

   helplist.item = sghelpitems;
}

tsgonlinemousehelp* onlinehelp = NULL;




class tsgonlinemousehelpwind : public tonlinemousehelp {
            public:
                tsgonlinemousehelpwind ( void );
          } ;

tsgonlinemousehelpwind :: tsgonlinemousehelpwind ( void )
{
   helplist.num = 3;

   static tonlinehelpitem sghelpitemswind[3]  = { { 501,224, 569,290, 20013 },
                                                  { 589,228, 609,289, 20014 },
                                                  { 489,284, 509,294, 20015 }};

   for ( int i = 0; i< helplist.num; i++ ) {
      sghelpitemswind[i].rect.x1 = agmp->resolutionx - ( 640 - sghelpitemswind[i].rect.x1 );
      sghelpitemswind[i].rect.x2 = agmp->resolutionx - ( 640 - sghelpitemswind[i].rect.x2 );
   }

   helplist.item = sghelpitemswind;
}

tsgonlinemousehelpwind* onlinehelpwind = NULL;



int  memcheckticker;
int  abortgame;

#define keyinputbuffersize 12
#define messagedisplaytime 300

tkey         keyinput[keyinputbuffersize]; 
byte         keyinputptr; 

int              modenum8;
int              modenum24 = -1;
int videostartpos = 0;

int cdrom = 1;

pprogressbar actprogressbar = NULL;

pchar mainmenuitems[6] = { "new map", "new campaign", "load game", "continue network game", "network supervisor", "exit" };


cmousecontrol* mousecontrol = NULL;



class tsgpulldown : public tpulldown {
          protected:
             void* menubackground;
          public:
             int  barvisi;
             tsgpulldown ( void );
             void init ( void );
//             void baron(void);
//             void baroff(void);
             void checkpulldown(void);
} pd;


tsgpulldown :: tsgpulldown ( void )
{
   barvisi = 0;
   menubackground = NULL;
}
/*
void tsgpulldown :: baron ( void )
{
   if ( !barvisi ) {
      if ( !menubackground ) 
           menubackground = new char[ imagesize (0,0,agmp->resolutionx-1,pdb.pdbreite) ];
      setinvisiblemouserectanglestk (0,0,agmp->resolutionx-1,pdb.pdbreite );
      getimage (0,0,agmp->resolutionx-1,pdb.pdbreite, menubackground );
      barvisi = 1;
      getinvisiblemouserectanglestk ();
      tpulldown :: baron();
   }
}

void tsgpulldown :: baroff ( void )
{
   if ( barvisi ) {
      setinvisiblemouserectanglestk (0,0,agmp->resolutionx-1,pdb.pdbreite );
      putimage (0,0, menubackground );
      getinvisiblemouserectanglestk ();
      barvisi = 0;
      barstatus = false;
   }
}
*/
void tsgpulldown :: checkpulldown(void)
{
/*
   checkkeys();
   if ( mouseparams.y <= pdb.pdbreite ) {
      setvars();
      baron();
*/
      tpulldown :: checkpulldown();
/*
   } else {
      baroff();
      fieldnr = 255;
   } 
*/
}



#define CHEATS

int checkforcheats( void )
{
   #ifdef CHEATS

    int num = 0;
    for ( int i = 0; i < 8; i++ )
       if ( actmap->player[i].stat == ps_human )
          num++;

    if ( actmap->campaign )
       num++;

    if ( actmap->network )
       num++;

    if ( num <= 1 )
       return 1;
    else
       return 0;

   #else
   return 0;
   #endif
}   



#define mapborderwidth 4

 tbackgroundpict backgroundpict;

tbackgroundpict :: tbackgroundpict ( void )
{
   background = NULL;
   inited = 0;
   run = 0;
   lastpaintmode = -1;
}

void tbackgroundpict :: paintrectangleborder ( void )
{
   if ( rectangleborder.initialized ) {
         int width = 2;
         for ( int i = 0; i < 3; i++ ) 
            for ( int j = 0; j < width; j++ ) {
               int p = i * width + j;
               int x; 
               int y;
               for ( x = rectangleborder.x1 - p; x < rectangleborder.x2 + p; x++ )
                  putpixel ( x, rectangleborder.y1 - p, xlattables.a.dark2[ getpixel ( x, rectangleborder.y1-p )] );
               for ( y = rectangleborder.y1 - p; y < rectangleborder.y2 + p; y++ )
                  putpixel ( rectangleborder.x2 + p, y, xlattables.light[ getpixel ( rectangleborder.x2 + p, y )] );
               for ( x = rectangleborder.x2 + p; x > rectangleborder.x1 - p; x-- )
                  putpixel ( x, rectangleborder.y2 + p, xlattables.light[ getpixel ( x, rectangleborder.y2+p )] );
               for ( y = rectangleborder.y2 + p; y > rectangleborder.y1 - p; y-- )
                  putpixel ( rectangleborder.x1 - p, y, xlattables.a.dark2[ getpixel ( rectangleborder.x1 - p, y )] );
      
            }
      }
}


void tbackgroundpict :: init ( int reinit )
{
   if ( !inited || reinit ) {
     /*
     int borderx1 = getmapposx ( );
     int bordery1 = getmapposy ( );
     */
     int borderx1 = 0;
     int bordery1 = 0;

     int bordery2 = bordery1 + (idisplaymap.getscreenysize() - 1) * fielddisty + fieldsizey - 1;
     int borderx2 = borderx1 + (idisplaymap.getscreenxsize() - 1 ) * fielddistx + fieldsizex + fielddisthalfx - 1;

    #ifdef HEXAGON
     int height, width;
     getpicsize ( borderpicture[2], width, height );

     borderpos[0].x = borderx1 - mapborderwidth;
     borderpos[0].y = bordery1 - mapborderwidth;

     borderpos[2].x = borderx2 + mapborderwidth - ( width -1 );
     borderpos[2].y = bordery1 - mapborderwidth;


     getpicsize ( borderpicture[6], width, height );

     borderpos[1].x = borderx1 - mapborderwidth + 28;
     borderpos[1].y = bordery1 - mapborderwidth;

     borderpos[6].x = borderx1 - mapborderwidth + 28 + fielddisthalfx;
     borderpos[6].y = bordery2 + mapborderwidth - ( height-1);


     getpicsize ( borderpicture[4], width, height );

     borderpos[3].x = borderx1 - mapborderwidth;
     borderpos[3].y = bordery1 + fielddisty;

     borderpos[4].x = borderx2 + mapborderwidth - ( width - 1 );
     borderpos[4].y = bordery1 + 2 * fielddisty;


     getpicsize ( borderpicture[5], width, height );

     borderpos[5].x = borderx1 - mapborderwidth;
     borderpos[5].y = bordery2 + mapborderwidth - ( height - 1 );


     getpicsize ( borderpicture[7], width, height );

     borderpos[7].x = borderx2 + mapborderwidth - ( width- 1);
     borderpos[7].y = bordery2 + mapborderwidth - ( height - 1 );
    #endif
     inited = 1;

   }
}



void tbackgroundpict :: load ( void )
{
   int w;
   {
      tnfilestream stream ("amatur.raw", 1);
      for ( int i = 0; i< 7; i++ )
         stream.readrlepict ( &dashboard[i], false, &w );
     #ifdef FREEMAPZOOM
      ::dashboard.zoom.pic = dashboard[6];
     #endif
   }
   #ifdef HEXAGON
   {
      tnfilestream stream ("hxborder.raw", 1);
      for ( int i = 0; i < 8; i++ )
         stream.readrlepict ( &borderpicture[i], false, &w );
   }
   #endif
}


void tbackgroundpict :: paintborder ( int dx, int dy )
{
   paintborder ( dx, dy , 0 );
}

void tbackgroundpict :: paintborder ( int dx, int dy, int reinit )
{
   if ( lastpaintmode < 1 ) {
      #ifdef FREEMAPZOOM 
       paintrectangleborder (  );
      #else
   
      if ( !inited || reinit )
           init( reinit );
       putspriteimage ( borderpos[0].x + dx, borderpos[0].y + dy,  borderpicture[0] ); 
       putspriteimage ( borderpos[2].x + dx, borderpos[2].y + dy,  borderpicture[2] ); 
   
       for ( int x = 0; x < idisplaymap.getscreenxsize() -1; x++ ) {
          putspriteimage ( borderpos[1].x + dx + x * fielddistx, borderpos[1].y + dy,  borderpicture[1] ); 
          putspriteimage ( borderpos[6].x + dx + x * fielddistx, borderpos[6].y + dy,  borderpicture[6] ); 
       }
   
       for ( int y = 0; y < idisplaymap.getscreenysize() -2; y+=2 ) {
          putspriteimage ( borderpos[3].x + dx, borderpos[3].y + y * fielddisty + dy,  borderpicture[3] ); 
          putspriteimage ( borderpos[4].x + dx, borderpos[4].y + y * fielddisty + dy,  borderpicture[4] ); 
       }
   
       putspriteimage ( borderpos[5].x + dx, borderpos[5].y + dy,  borderpicture[5] ); 
       putspriteimage ( borderpos[7].x + dx, borderpos[7].y + dy,  borderpicture[7] ); 
      #endif
      lastpaintmode = 1;
   }
}

void  tbackgroundpict :: paint ( int resavebackground )
{ 
  init();

  #ifndef HEXAGON
  if (hgmp->resolutionx == 640) { 
     if ( actmap->xsize && !lockdisplaymap ) {
        tnfilestream stream ( "bkgr4.pcx", 1 );
        loadpcxxy( &stream, 0, 0 ); 
        lastpaintmode = 1;
     } else {
        tnfilestream stream ( "bkgr42.pcx", 1 );
        loadpcxxy( &stream, 0, 0 ); 
        lastpaintmode = 0;
     }
  } else 
  #endif
  {
     if ( !background ) {

        char filename[100];
        sprintf( filename, "%d%d.pcx", hgmp->resolutionx, hgmp->resolutiony );
        if ( exist ( filename )) {
           tnfilestream stream ( filename, 1 );
           loadpcxxy( &stream ,0,0);

           background = new char[ imagesize ( 0, 0, agmp->resolutionx, agmp->resolutiony  )];
        } else {
           displaymessage2("generating background picture; please wait ..." );
           if ( !asc_paletteloaded )
              loadpalette();
           int x = hgmp->resolutionx;
           int y = hgmp->resolutiony;
           {
              tvirtualdisplay vdp ( agmp->resolutionx, agmp->resolutiony );
              tnfilestream stream ( "640480.pcx", 1 );
              loadpcxxy( &stream ,0,0);
              char* pic = new char[ imagesize ( 0, 0, 639, 479 )];
              getimage ( 0, 0, 639, 479, pic );
   
              TrueColorImage* img = zoomimage ( pic, x, y, pal, 1, 0  );
              delete pic;
              background = convertimage ( img, pal );
           }
           putimage ( 0, 0, background );
           writepcx ( filename, 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, pal );
   
           /*
           bar ( 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, greenbackgroundcol );
           found = 0;
           */
        }

        putimage ( agmp->resolutionx - ( 640 - 450), 15,  dashboard[0] );
        putimage ( agmp->resolutionx - ( 640 - 450), 211, dashboard[1] );
        putimage ( agmp->resolutionx - ( 640 - 450), agmp->resolutiony - ( 480 - 433),  dashboard[2] );
        int l_width, l_height;
        int m_width, m_height;
        int r_width, r_height;
        getpicsize ( dashboard[3], l_width, l_height );
        getpicsize ( dashboard[4], m_width, m_height );
        getpicsize ( dashboard[5], r_width, r_height );
   
        int lpos = 14;
        putimage ( lpos,  agmp->resolutiony - ( 480 - 442),  dashboard[3] );
        putimage ( lpos + l_width,  agmp->resolutiony - ( 480 - 442),  dashboard[4] );
   
        int rpos = agmp->resolutionx - ( 640 - 433) - r_width + 1;
        lpos += l_width + m_width;
   
        putimage ( rpos,  agmp->resolutiony - ( 480 - 442),  dashboard[5] );
   
        while ( rpos > lpos ) {
           rpos -= m_width;
           putimage ( rpos,  agmp->resolutiony - ( 480 - 442),  dashboard[4] );
        }

        getimage ( 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, background );

     } else 
        putimage ( 0, 0, background );

    lastpaintmode = 0;

    #ifdef HEXAGON
     if ( actmap->xsize && !lockdisplaymap ) 
        paintborder( getmapposx ( ), getmapposy ( ) );

    #endif
  }
  if ( resavebackground  ||  !run ) {
     gui.savebackground ( );
  }
  run++;
  ::dashboard.repainthard = 1;

} 

int tbackgroundpict :: getlastpaintmode ( void )
{
   return lastpaintmode;
}


void* reservememory = NULL;
void* emergencymemory = NULL;
#define reservememorysize 300000
#define emergencymemorysize 4000 

void emergency_new_handler ( void )
{
   displaymessage("run out of memory while in new_new_handler.\n please contact authors.\n",2 );
}

void new_new_handler ( void )
{
   delete  ( reservememory );
   set_new_handler ( emergency_new_handler );
   savegame("rescue.sav","game saved while exiting game due to a lack of memory "); 
   displaymessage("Not enough memory. Saved game to emergncy.sav. ",2 );
}



#ifdef MEMCHK

  int blocknum = 0;
  int blocklist[1000000];


  void addblock ( void* p )
  {
     blocklist[blocknum++] = (int) p;
  }

  int removeblock ( void* p )
  {
     if ( blocknum== 0 )
        return 0;

     int error = 0;
     int found = 0;
     int pos = 0;
     while ( !found && pos < blocknum) {
        if ( blocklist[pos] == (int) p )
           found++;
        else
           pos++;
     } /* endwhile */
     if ( found ) {
        for ( int i = pos+1; i < blocknum; i++ )
           blocklist[i-1] = blocklist[i];
        blocknum--;
     } 

     return found;
  }

  int blockavail( void* p )
  {
     int found = 0;
     int pos = 0;
     while ( !found && pos < blocknum) {
        if ( blocklist[pos] == (int) p )
           found++;
        else
           pos++;
     } /* endwhile */
     return found;
  }


  void* memchkAlloc ( int tp, size_t amt )
  {
     int error;
     void* tmp = malloc ( amt + 53 * 4 );
     int* tmpi = (int*) tmp;
     /*
     if ( (int) tmpi == 0x1bb2138 || (int) tmpi == 0x1bcf178 ) 
        error++;
        */
     tmpi[0] = tp;
     tmpi[1] = (int) tmp;
     tmpi[2] = amt;
     for ( int i = 0; i < 25; i++ ) {
        tmpi[3 + i] = 0x12345678;
        tmpi[3 + i + (amt+3)/4 + 25] = 0x87654321;
     }
     void* p = &tmpi[28];

     addblock ( p );
     return p;
  }


  void* verifyblock ( int tp, void* p )
  {
     int error = 0;
     int* tmpi = (int*) p;
     tmpi -= 28;

     if ( tp != -1 )
        if ( tmpi[0] != tp )
           error++;

     if ( tmpi[1] != (int) tmpi)
        error++;

     int amt = tmpi[2];

     for ( int i = 0; i < 25; i++ ) {

        if ( tmpi[3 + i] != 0x12345678)
           error++;

        if ( tmpi[3 + i + (amt+3)/4 + 25] != 0x87654321 )
           error++;
     }
     return tmpi;
  }

  void verifyallblocks ( void )
  {
     for ( int i = 0; i < blocknum; i++ )
        verifyblock ( -1, (void*) blocklist[i] );
  }

  void memchkFree ( int tp, void* buf )
  {
     if ( removeblock ( buf )) {
        void* tmpi = verifyblock ( tp, buf );
        free ( tmpi );
     } else
       free ( buf );
  }

  void *operator new( size_t amt )
  {
      return( memchkAlloc( 100, amt ) );
  }
  
  void operator delete( void *p )
  {
     if ( p )
      memchkFree( 100, p );
  }
  
  void *operator new []( size_t amt )
  {
      return( memchkAlloc( 102, amt ) );
  }
  
  void operator delete []( void *p )
  {
     if ( p )
      memchkFree( 102, p );
  }

  void* asc_malloc ( size_t size )
  {
     void* tmp = memchkAlloc ( 104, size );
     if ( tmp == NULL ) 
        new_new_handler();
     return tmp;
  }

  void asc_free ( void* p )
  {
     memchkFree ( 104, p );
  }

#else

  void* asc_malloc ( size_t size )
  {
     void* tmp = malloc ( size );
     if ( tmp == NULL ) 
        new_new_handler();
     return tmp;
  }

  void asc_free ( void* p )
  {
     free ( p );
  }


#endif



void showmemory ( void )
{
   npush ( activefontsettings );
   activefontsettings.length = 99;
   activefontsettings.background = 0;
   activefontsettings.color = 14;
   activefontsettings.justify = lefttext;
   activefontsettings.font = schriften.smallarial;
   int mss = getmousestatus();
   if (mss == 2)
      setinvisiblemouserectanglestk ( 0, agmp->resolutiony, 640, agmp->resolutiony + 80 ); 

   int a = maxavail();
   int b;
   //   int b = _memavl();
   //   showtext2( strrr ( _memmax() ), 10,agmp->resolutiony );
   showtext2( strrr ( b  ), 110,agmp->resolutiony );
   showtext2( strrr ( a ), 210,agmp->resolutiony );
   showtext2( strrr ( b+a ), 310,agmp->resolutiony );
   showtext2( strrr ( stackfree ()  ), 410,agmp->resolutiony );

   showtext2( strrr ( getxpos()  ), 10,agmp->resolutiony + 30 );
   showtext2( strrr ( getypos()  ), 110,agmp->resolutiony + 30 );

   showtext2( strrr ( actmap->time.a.turn  ), 10,agmp->resolutiony + 60 );
   showtext2( strrr ( actmap->time.a.move  ), 110,agmp->resolutiony + 60 );
   if (mss == 2)
      getinvisiblemouserectanglestk ();

   npop  ( activefontsettings );
};






#ifndef HEXAGON
static const char vierecktranstable[256] = {   0,    0,    2,    0,    0,    5,    6,    7,    8,    9,   10,
                                               11,   00,   13,   14,   15,   00,   00,   18,   19,   20,
                                               85,   22,   23,   24,   25,   26,   27,   28,   29,   30,
                                               31,   32,   33,   34,   35,   36,   37,   38,   39,   40,
                                               41,   42,   43,   44,   45,   46,   47,   00,   49,   50,
                                               00,   52,   53,   54,   00,   56,   57,   58,   59,   60,
                                               61,   62,  255,   00,   65,   66,   67,   00,   85,   70,
                                               85,   72,   73,   74,   75,   76,   77,   78,   79,   80,
                                               85,   82,   83,   85,   85,   86,   85,   88,   89,   90,
                                               91,   92,   85,   94,   95,   96,   97,   98,   99,  100,
                                               101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  
                                               111,  112,  113,  114,  000,   85,   85,  118,   85 ,  120,
                                               121,  122,  123,  124,  125,  126,  255,  128,  129,  130,
                                               131,  132,  133,  134,  135,  136,  137,  138,  139,  140,  
                                               141,  142,  143,  144,  145,  146,  147,  148,  149,  150,  
                                               151,  152,  153,  154,  155,  156,  157,  158,  159,  160,  
                                               161,  162,  163,  164,  165,  166,  167,  168,  169,  170, 
                                               171,  172,  173,  174,  175,  176,  177,  178,  179,  180,
                                               181,  182,  183,  184,  185,  186,  187,  188,  189,  190,
                                               191,  000,  193,  194,  195,  196,  197,  198,  199,  200,
                                               201,  202,  203,  000,  000,  206,  255,  208,  209,  210,
                                               211,  212,   85,  214,  215,  216,  217,  218,  219,  000,
                                                85,  222,  255,  224,  225,  226,  227,  228,  229,  230,
                                               231,  232,  233,  234,  235,  236,  237,  238,  239,  240,  
                                               241,  242,  255,  244,  245,  246,  255,  248,  249,  250,
                                               251,  255,  255, 254, 255 };



void mountview( void ) 
{
   memset( view.viereck, 0, sizeof ( view.viereck ));
   npush ( *agmp );
   agmp-> linearaddress = (int) new int [ 256 ];

   agmp-> scanlinelength = 20;
   agmp-> windowstatus = 100;

   /*
   initsvga(0x101);

   char t[20];
   */

   int m,n;

   int a,b[5],c,e,w;
   memset(b, 0, sizeof(b));

   w = imagesize ( 0,0,9,9 );

   for (a=0 ;a < 81 ;a++ ) {
         bar(0,0,10,10,255);
         e=0;
         m=0;
         n=0;
         for (c=0;c < 4 ; c++ ) {
            if ( b[c] == 1) {
              putspriteimage(0,0, view.va4[c]);
              e |= (1 << (2*c));
              m++;
            } else {
              if ( b[c] == 2) {
                 putspriteimage(0,0, view.nv4[c]);
                 e |= (3 << (2*c));
                 n++;
              }
            }
         } /* endfor */

         if ( vierecktranstable[e] == e ) {
              view.viereck[e] = new char [  w  ];
              getimage ( 0,0,9,9, view.viereck[e] );
          } /* endif */

         /*
         itoa( e, t, 2);
         activefontsettings.background = white;
         activefontsettings.length = 100;
         activefontsettings.font = schriften.smallarial;
         showtext2( t, 50, 50 );
         getch();
         */

         e=0;
         while (b[e] == 2) {
            b[e] = 0;
            e++;
         } /* endwhile */
         b[e]++;
   } /* endfor */

   for (a=0 ;a < 81 ;a++ ) {
         m=0;
         n=0;
         e=0;
         for (c=0;c < 4 ; c++ ) {
            if ( b[c] == 1) {
              e |= (1 << (2*c));
              m++;
            } else {
              if ( b[c] == 2) {
                 e |= (3 << (2*c));
                 n++;
              }
            }
         } /* endfor */

         if ( view.viereck[e] == NULL ) 
            view.viereck[e] = view.viereck[vierecktranstable[e]];

         e=0;
         while (b[e] == 2) {
            b[e] = 0;
            e++;
         } /* endwhile */
         b[e]++;
   } /* endfor */




   delete  ( (void*) agmp-> linearaddress );
   npop ( *agmp );
}
#endif




void         loadcursor(void)
{ 
  int          w;
  char         i;

   {
      tnfilestream stream ( "height2.raw", 1 );
      for (i=0;i<3 ;i++ ) 
         for ( int j=0; j<8; j++) 
            stream.readrlepict( &icons.height2[i][j], false, &w );
   }

   {
      tnfilestream stream ("windp.raw",1);
      for (i=0;i<9 ;i++ ) 
          stream.readrlepict( &icons.wind[i], false, &w );
                                                                                                                                                                                                                                                               
   }

  #ifdef HEXAGON
   {
      tnfilestream stream ("windpfei.raw",1);
      stream.readrlepict( &icons.windarrow, false, &w );
   }
  #endif

   {
      tnfilestream stream ("farbe.raw",1);
      for (i=0;i<8 ;i++ ) 
          stream.readrlepict( &icons.player[i], false, &w );
   }

   {
      tnfilestream stream ("allianc.raw",1);
      for (i=0;i<8 ;i++ ) {
          stream.readrlepict(   &icons.allianz[i][0], false, &w );
          stream.readrlepict(   &icons.allianz[i][1], false, &w );
          stream.readrlepict(   &icons.allianz[i][2], false, &w );
      } /* endfor */
   }

   {
      tnfilestream stream ("weapicon.raw",1);
      for (i=0; i<13 ;i++ ) 
            stream.readrlepict(   &icons.unitinfoguiweapons[i], false, &w );
   }

   {
      tnfilestream stream ("expicons.raw",1);
      for (i=0; i<16 ;i++ ) 
            stream.readrlepict(   &icons.experience[i], false, &w );
   }

   {
      #ifndef HEXAGON
      tnfilestream stream ("va8.raw",1);
      #else
      tnfilestream stream ("hexinvi2.raw",1);
      #endif
      stream.readrlepict(   &view.va8, false, &w);
   }

   {
      #ifndef HEXAGON
      tnfilestream stream ("nv8.raw",1);
      #else
      tnfilestream stream ("hexinvis.raw",1);
      #endif
      stream.readrlepict(   &view.nv8, false, &w);
      void* u = uncompress_rlepict ( view.nv8 );
      if ( u ) {
         delete[] view.nv8;
         view.nv8 = u;
      }
   }

   {
      tnfilestream stream ("fg8.raw",1);
      stream.readrlepict(   &view.fog8, false, &w);
   }

   {
      tnfilestream stream ("windrose.raw",1);
      stream.readrlepict(   &icons.windbackground, false, &w);
   }

   if ( actprogressbar )
      actprogressbar->point();


   #ifndef HEXAGON
   char s[100];
   strcpy(s,"nv4-0.raw");
   for (i=0; i<4 ;i++ ) {

       s[0]='n';
       s[1]='v';
       s[4]='0'+i;
       {
          tnfilestream stream (s,1);
          stream.readrlepict(   &view.nv4[i], false, &w);
       }

       s[0]='v';
       s[1]='a';
       {
          tnfilestream stream (s,1);
          stream.readrlepict(   &view.va4[i], false, &w);
       }

       s[0]='f';
       s[1]='g';
       {
          tnfilestream stream (s,1);
          stream.readrlepict(   &view.fog4[i], false, &w);
       }

   } /* endfor */
   mountview();
   #endif
   
   

   {
      #ifdef HEXAGON
      tnfilestream stream ("hexfld_a.raw",1); 
      #else
      tnfilestream stream ("markacti.raw",1); 
      #endif
      
      stream.readrlepict(   &icons.container.mark.active, false, &w);
   }

   if ( actprogressbar )
      actprogressbar->point();

   {
      #ifdef HEXAGON
      tnfilestream stream ("hexfld.raw",1); 
      #else
      tnfilestream stream ("markinac.raw",1); 
      #endif
      stream.readrlepict(   &icons.container.mark.inactive, false, &w);
   }

   {
      #ifdef HEXAGON
      tnfilestream stream ("in_ach.raw",1); 
      #else
      tnfilestream stream ("markain.raw",1); 
      #endif
      stream.readrlepict(   &icons.container.mark.movein_active, false, &w);
   }

   {
      #ifdef HEXAGON
      tnfilestream stream ("in_h.raw",1); 
      #else
      tnfilestream stream ("markain2.raw",1); 
      #endif
      stream.readrlepict(   &icons.container.mark.movein_inactive, false, &w);
   }

   {
      #ifdef HEXAGON
      tnfilestream stream ("build_ah.raw",1); 
      #else
      tnfilestream stream ("baumoda.raw",1); 
      #endif
      stream.readrlepict(   &icons.container.mark.repairactive, false, &w);
   }

   {
      #ifdef HEXAGON
      tnfilestream stream ("build_h.raw",1); 
      #else
      tnfilestream stream ("baumod.raw",1); 
      #endif
      stream.readrlepict(   &icons.container.mark.repairinactive, false, &w);
   }

   {
      #ifdef HEXAGON
      tnfilestream stream ("hexbuild.raw",1); 
      stream.readrlepict(   &icons.container.container_window, false, &w);
      #endif
   }

   if ( actprogressbar )
      actprogressbar->point();

   if ( !asc_paletteloaded )
      loadpalette();

   for (w=0;w<256 ;w++ ) {
      palette16[w][0] = pal[w][0];
      palette16[w][1] = pal[w][1];
      palette16[w][2] = pal[w][2];
      xlattables.nochange[w] = w;
   } /* endfor */

   if ( actprogressbar )
      actprogressbar->point();

   loadicons(); 

   if ( actprogressbar )
      actprogressbar->point();

   loadmessages(); 

   if ( actprogressbar )
      actprogressbar->point();

   {
      tnfilestream stream ("WAFFEN.RAW",1);       
      int num;
   
      static int xlatselectweaponguiicons[12] = { 2, 7, 6, 3, 4, 9, 0, 5, 10, 11, 11, 11 };
   
      stream.readdata ( (char*) &num, sizeof( num ));
      for ( i = 0; i < num; i++ ) 
          stream.readrlepict(   &icons.selectweapongui[xlatselectweaponguiicons[i]], false, &w );
      stream.readrlepict(   &icons.selectweaponguicancel, false, &w );
   }

   {
      tnfilestream stream ("knorein.RAW",1);
      stream.readrlepict(   &icons.guiknopf, false, &w );
   }

   {
      tnfilestream stream ("compi2.RAW",1);
      stream.readrlepict(   &icons.computer, false, &w );
   }

   {
      tnfilestream stream ("pfeil-A0.raw",1);
      for (i=0; i<8 ;i++ ) {
            stream.readrlepict(   &icons.pfeil2[i], false, &w );
      } /* endfor */
   }

   {
      tnfilestream stream ("mausi.RAW",1);
      stream.readrlepict(   &icons.mousepointer, false, &w );
   }

   {
      tnfilestream stream ("gebasym2.RAW",1);
      for ( i = 0; i < 11; i++ )
         for ( int j = 0; j < 2; j++ )
            stream.readrlepict(   &icons.container.lasche.sym[i][j], false, &w );
   }

   {
      tnfilestream stream ("netcontr.raw",1);
      stream.readrlepict(   &icons.container.subwin.netcontrol.main, false, &w );
      stream.readrlepict(   &icons.container.subwin.netcontrol.inactive, false, &w );
      stream.readrlepict(   &icons.container.subwin.netcontrol.active, false, &w );
   }

   {
      tnfilestream stream ("ammoprod.raw",1);
      stream.readrlepict(   &icons.container.subwin.ammoproduction.main, false, &w );
      stream.readrlepict(   &icons.container.subwin.ammoproduction.button, false, &w );
      stream.readrlepict(   &icons.container.subwin.ammoproduction.buttonpressed, false, &w );
      for ( i = 0; i < 4; i++ )
         stream.readrlepict(   &icons.container.subwin.ammoproduction.schieber[i], false, &w );
      stream.readrlepict(   &icons.container.subwin.ammoproduction.schiene, false, &w );
   }

   if ( actprogressbar )
      actprogressbar->point();

   {
      tnfilestream stream ("resorinf.raw",1);
      stream.readrlepict(   &icons.container.subwin.resourceinfo.main, false, &w );
   }

   {
      tnfilestream stream ("windpowr.raw",1);
      stream.readrlepict(   &icons.container.subwin.windpower.main, false, &w );
   }

   {
      tnfilestream stream ("solarpwr.raw",1);
      stream.readrlepict(   &icons.container.subwin.solarpower.main, false, &w );
   }

   {
      tnfilestream stream ("ammotran.raw",1);
      stream.readrlepict(   &icons.container.subwin.ammotransfer.main, false, &w );
      stream.readrlepict(   &icons.container.subwin.ammotransfer.button, false, &w );
      stream.readrlepict(   &icons.container.subwin.ammotransfer.buttonpressed, false, &w );
      for ( i = 0; i < 4; i++ )
         stream.readrlepict(   &icons.container.subwin.ammotransfer.schieber[i], false, &w );
      stream.readrlepict(   &icons.container.subwin.ammotransfer.schieneinactive, false, &w );
      stream.readrlepict(   &icons.container.subwin.ammotransfer.schiene, false, &w );
   }

   {
      tnfilestream stream ("research.raw",1);
      stream.readrlepict(   &icons.container.subwin.research.main, false, &w );
      stream.readrlepict(   &icons.container.subwin.research.button[0], false, &w );
      stream.readrlepict(   &icons.container.subwin.research.button[1], false, &w );
      stream.readrlepict(   &icons.container.subwin.research.schieber, false, &w );
   }

   {
      tnfilestream stream ("pwrplant.raw",1);
      stream.readrlepict(   &icons.container.subwin.conventionelpowerplant.main, false, &w );
      stream.readrlepict(   &icons.container.subwin.conventionelpowerplant.button[0], false, &w );
      stream.readrlepict(   &icons.container.subwin.conventionelpowerplant.button[1], false, &w );
   }

   
   
   int m;
   {
      tnfilestream stream ( "bldinfo.raw", 1 );
      stream.readrlepict( &icons.container.subwin.buildinginfo.main, false, &m );
      for ( i = 0; i < 8; i++ )
         stream.readrlepict( &icons.container.subwin.buildinginfo.height1[i], false, &m );
      for ( i = 0; i < 8; i++ )
         stream.readrlepict( &icons.container.subwin.buildinginfo.height2[i], false, &m );
      stream.readrlepict( &icons.container.subwin.buildinginfo.repair, false, &m );
      stream.readrlepict( &icons.container.subwin.buildinginfo.repairpressed, false, &m );
      stream.readrlepict( &icons.container.subwin.buildinginfo.block, false, &m );
   }
   

   {
      tnfilestream stream ("mining.raw",1);
      stream.readrlepict(   &icons.container.subwin.miningstation.main, false, &w );
      /*
      for ( i = 0; i < 2; i++ )
         stream.readrlepict(   &icons.container.subwin.miningstation.button[i], false, &w );
      for ( i = 0; i < 2; i++ )
         stream.readrlepict(   &icons.container.subwin.miningstation.resource[i], false, &w );
      for ( i = 0; i < 3; i++ )
         stream.readrlepict(   &icons.container.subwin.miningstation.axis[i], false, &w );
      for ( i = 0; i < 2; i++ )
         stream.readrlepict(   &icons.container.subwin.miningstation.pageturn[i], false, &w );
      stream.readrlepict(   &icons.container.subwin.miningstation.graph, false, &w );
      */
   }

   {
      tnfilestream stream ("tabmark.raw", 1);
      stream.readrlepict (   &icons.container.tabmark[0], false, &w );
      stream.readrlepict (   &icons.container.tabmark[1], false, &w );
   }

   {
      tnfilestream stream ("traninfo.raw",1);
      stream.readrlepict(   &icons.container.subwin.transportinfo.main, false, &w );
      for ( i = 0; i < 8; i++ )
         stream.readrlepict(   &icons.container.subwin.transportinfo.height1[i], false, &w );
      for ( i = 0; i < 8; i++ )
         stream.readrlepict(   &icons.container.subwin.transportinfo.height2[i], false, &w );
      stream.readrlepict(   &icons.container.subwin.transportinfo.sum, false, &w );
   }

   if ( actprogressbar )
      actprogressbar->point();

   {
      tnfilestream stream ("attack.raw", 1);
      stream.readrlepict (   &icons.attack.bkgr, false, &w );
      icons.attack.orgbkgr = NULL;
   }

   {
     #ifdef HEXAGON
      tnfilestream stream ("hexfeld.raw", 1);
     #else
      tnfilestream stream ("oktfld2.raw", 1);
     #endif
      stream.readrlepict ( &icons.fieldshape, false, &w );
   }

   #ifdef FREEMAPZOOM 
   {
      tnfilestream stream ("mapbkgr.raw", 1);
      stream.readrlepict ( &icons.mapbackground, false, &w );
   }
   #endif

   {
      tnfilestream stream ("hex2oct.raw", 1);
      stream.readrlepict (   &icons.hex2octmask, false, &w );
   }

   backgroundpict.load();

} 

enum tuseractions { ua_repainthard,     ua_repaint, ua_help, ua_showpalette, ua_dispvehicleimprovement, ua_mainmenu, ua_cheat_morefog, 
                    ua_cheat_lessfog,   ua_cheat_morewind,   ua_cheat_lesswind, ua_cheat_rotatewind, ua_changeresourceview, ua_heapcheck,
                    ua_benchgamewv,     ua_benchgamewov,     ua_viewterraininfo, ua_unitweightinfo,  ua_writemaptopcx,  ua_writescreentopcx,
                    ua_scrolltodebugpage, ua_scrolltomainpage, ua_startnewsinglelevel, ua_changepassword, ua_gamepreferences, ua_bi3preferences,
                    ua_exitgame,        ua_newcampaign,      ua_loadgame,  ua_savegame, ua_setupalliances, ua_settribute, ua_giveunitaway, 
                    ua_vehicleinfo,     ua_researchinfo,     ua_unitstatistics, ua_buildingstatistics, ua_newmessage, ua_viewqueuedmessages, 
                    ua_viewsentmessages, ua_viewreceivedmessages, ua_viewjournal, ua_editjournal, ua_viewaboutmessage, ua_continuenetworkgame,
                    ua_toggleunitshading, ua_computerturn, ua_setupnetwork, ua_howtostartpbem, ua_howtocontinuepbem };


void         tsgpulldown :: init ( void )
{ 
  addfield ( "Glo~b~al" ); 
   addbutton ( "toggle ~R~esourceviewõ1", ua_changeresourceview ); 
   addbutton ( "toggle unit shadingõ2", ua_toggleunitshading ); 
   addbutton ( "seperator", -1); 
   addbutton ( "~O~ptions", ua_gamepreferences ); 
   #ifdef HEXAGON
    addbutton ( "~B~I options",ua_bi3preferences); 
   #endif
   addbutton ( "seperator", -1); 
   addbutton ( "~M~ain MenuõF2", ua_mainmenu );
   addbutton ( "E~x~itõctrl-x", ua_exitgame );


  addfield ("~G~ame");
   addbutton ( "New ~C~ampaign", ua_newcampaign); 
   addbutton ( "~N~ew single Levelõctrl-n", ua_startnewsinglelevel ); 
   addbutton ( "seperator", -1); 
   addbutton ( "~L~oad gameõctrl-l", ua_loadgame ); 
   addbutton ( "~S~ave game", ua_savegame ); 
   addbutton ( "seperator", -1 ); 
   addbutton ( "Continue network gameõF3", ua_continuenetworkgame); 
   addbutton ( "setup Net~w~ork", ua_setupnetwork ); 
   addbutton ( "Change ~P~assword", ua_changepassword ); 
   addbutton ( "seperator", -1 ); 
   addbutton ( "~P~layers", ua_setupalliances); 
   addbutton ( "~T~ribute", ua_settribute); 
   addbutton ( "~G~ive unit away", ua_giveunitaway ); 

  addfield ( "~I~nfo" );
   addbutton ( "~V~ehicle types", ua_vehicleinfo ); 
   addbutton ( "~U~nit weightõ8", ua_unitweightinfo ); 
   addbutton ( "~T~errainõ7", ua_viewterraininfo ); 
   addbutton ( "seperator", -1 ); 
   addbutton ( "~R~esearch", ua_researchinfo ); 
   addbutton ( "vehicle ~I~mprovementõF7", ua_dispvehicleimprovement); 

  addfield ( "~S~tatistics" );
   addbutton ( "~U~nits", ua_unitstatistics ); 
   addbutton ( "~B~uildings", ua_buildingstatistics ); 
   // addbutton ( "seperator"); 
   // addbutton ( "~H~istory"); 

  addfield ( "~M~essage");
   addbutton ( "~n~ew message", ua_newmessage ); 
   addbutton ( "view ~q~ueued messages", ua_viewqueuedmessages ); 
   addbutton ( "view ~s~end messages", ua_viewsentmessages ); 
   addbutton ( "view ~r~eceived messages", ua_viewreceivedmessages); 
   addbutton ( "seperator", -1 ); 
   addbutton ( "view ~j~ournal", ua_viewjournal ); 
   addbutton ( "~a~ppend to journal", ua_editjournal ); 

  addfield ( "~T~ools" );
   addbutton ( "save ~M~ap as PCXõ9", ua_writemaptopcx ); 
   addbutton ( "save ~S~creen as PCXõ0", ua_writescreentopcx ); 
   addbutton ( "benchmark without view calcõ5", ua_benchgamewov ); 
   addbutton ( "benchmark with view calcõ6", ua_benchgamewv); 
   addbutton ( "test memory integrityõ3", ua_heapcheck ); 

  addfield ( "~H~elp" );
   addbutton ( "HowTo ~S~tart email games", ua_howtostartpbem ); 
   addbutton ( "HowTo ~C~ontinue email games", ua_howtocontinuepbem ); 
   addbutton ( "seperator", -1); 
   addbutton ( "~K~eys", ua_help ); 

   addbutton ( "~A~bout", ua_viewaboutmessage ); 

/*
   addbutton ( "show ~P~alette"); 
   addbutton ( "~G~enerate sub"); 
   addbutton ( ""); 
   addbutton ( ""); 
   addbutton ( ""); 
   pdb.pdfield[6]->count = 1; 
*/

   tpulldown :: init();
   setshortkeys();
} 



void         repaintdisplay(void)
{ 
   int mapexist = !( actmap->xsize == 0  || actmap->ysize == 0 );


   int ms = getmousestatus();
   if ( ms == 2 )
      mousevisible ( false );

   if ( mapexist )
      cursor.hide(); 
   backgroundpict.paint(); 
   setvgapalette256(pal); 

   if ( mapexist ) {
      displaymap(); 
      cursor.show(); 
   }

//   pd.barvisi = 0;
   if ( ms == 2 )
      mousevisible ( true );
   dashboard.x = 0xffff;
   dashboard.repainthard = 1;
   if ( actmap && actmap->ellipse )
      actmap->ellipse->paint();
   if ( actgui && actmap && actmap->xsize>0)
      actgui->painticons();

} 

void         repaintdisplayhard(void)
{ 
   initsvga( modenum8 );
   setdisplaystart ( 0, videostartpos  );
   repaintdisplay();
} 


void         ladekarte(void)
{ 
  char         s1[30];

   mousevisible(false); 
   char s2 [ 200 ];
   fileselectsvga(mapextension, s1, 1, s2 );

   if ( s1[0] ) {
      mousevisible(false); 
      cursor.hide(); 
      displaymessage("loading map %s",0, s2 );
      loadmap(s1); 
      initmap(); 

      do {
        next_turn();
        if ( actmap->time.a.turn == 2 ) {
           displaymessage("no human players found !", 1 );
           erasemap();
           throw tnomaploaded();
        }
      } while ( actmap->player[actmap->actplayer].stat != ps_human ); /* enddo */

      removemessage(); 
      if (loaderror > 0) { 
         displaymessage("error nr %d",1, loaderror );
      } 
      else { 
         if (actmap->campaign != NULL) { 
            delete  ( actmap->campaign );
            actmap->campaign = NULL; 
         } 
      } 

      // computeview(); 
      // cursor.gotoxy ( actmap->cursorpos.position[ actmap->actplayer ].x, actmap->cursorpos.position[ actmap->actplayer ].y , 0);
      displaymap(); 
      dashboard.x = 0xffff;
      moveparams.movestatus = 0; 
   } 
   mousevisible(true); 
} 


void         ladespiel(void)
{ 
  char         s1[300];

   mousevisible(false); 
   char s2 [200];
   char temp[200];
   strcpy ( temp, gamepath );
   strcat ( temp, savegameextension );

   fileselectsvga(temp, s1, 1, s2 );

   if ( gamepath[0] && s1[0] ) {
      strcpy ( temp, gamepath );
      strcat ( temp, s1 );
      strcpy ( s1, temp );
   }

   if ( s1[0] ) {
      mousevisible(false); 
      cursor.hide(); 
      displaymessage("loading %s ",0, s2);
      loadgame(s1 );
      removemessage(); 
      if (loaderror > 0) { 
         displaymessage( "error nr. %d",1, loaderror);
      } 
      if ( actmap->xsize == 0 || actmap->ysize == 0 )
         throw  tnomaploaded();
         
      if ( actmap->network )
         setallnetworkpointers ( actmap->network );
         
      computeview(); 
      displaymap(); 
      dashboard.x = 0xffff;
      moveparams.movestatus = 0; 
   } 
   mousevisible(true); 
} 


void         speicherspiel( int as )
{ 
   char         s1[300], *s2;

   int nameavail = 0;
   if ( actmap->preferredfilenames && actmap->preferredfilenames->savegame[actmap->actplayer] )
      nameavail = 1;


   if ( as || !nameavail ) {
      mousevisible(false); 
      char temp[200];
      strcpy ( temp, savegameextension );

      fileselectsvga(temp, s1, 0);

   } else
      strcpy ( s1, actmap->preferredfilenames->savegame[actmap->actplayer] );

   if ( s1[0] ) {
      if ( !as && actmap->preferredfilenames && actmap->preferredfilenames->savegamedescription[actmap->actplayer] ) 
         s2 = strdup ( actmap->preferredfilenames->savegamedescription[actmap->actplayer] );
      else {
         tenterfiledescription efd;
         efd.init();
         efd.run();
         
         s2 = efd.description;
         efd.done();
      }

      if ( !actmap->preferredfilenames ) {
         actmap->preferredfilenames = new PreferredFilenames;
         memset ( actmap->preferredfilenames, 0 , sizeof ( PreferredFilenames ));
      }
      if ( actmap->preferredfilenames->savegamedescription[actmap->actplayer] )
         asc_free ( actmap->preferredfilenames->savegamedescription[actmap->actplayer] );
      actmap->preferredfilenames->savegamedescription[actmap->actplayer] = strdup ( s2 );

      if ( actmap->preferredfilenames->savegame[actmap->actplayer] )
         asc_free ( actmap->preferredfilenames->savegame[actmap->actplayer] );
      actmap->preferredfilenames->savegame[actmap->actplayer] = strdup ( s1 );

      mousevisible(false);
      cursor.hide(); 
      displaymessage("saving as %s", 0, s2);
      savegame(s1,s2); 

      removemessage(); 
      displaymap(); 
      cursor.show(); 
      delete  ( s2 );
   } 
   mousevisible(true); 
} 


void         showpalette(void)
{ 
   bar ( 0, 0, 639, 479, black );
   int x=7;

   for ( char a = 0; a <= 15; a++) 
      for ( char b = 0; b <= 15; b++) { 
         bar(     a * 40, b * 20,a * 40 +  x,b * 20 + 20, xlattables.light    [a * 16 + b]); 
         bar( x + a * 40, b * 20,a * 40 + 2*x,b * 20 + 20, xlattables.nochange [a * 16 + b]); 
         bar(2*x + a * 40, b * 20,a * 40 + 3*x,b * 20 + 20, xlattables.a.dark1    [a * 16 + b]); 
         bar(3*x + a * 40, b * 20,a * 40 + 4*x,b * 20 + 20, xlattables.a.dark2    [a * 16 + b]); 
         bar(4*x + a * 40, b * 20,a * 40 + 5*x,b * 20 + 20, xlattables.a.dark3    [a * 16 + b]); 
      }      
   wait();
   repaintdisplay();
} 

/*
void    generatesub ( void )
{
    for (int kl = 0; kl <= vehicletypeenanz; kl++)
        if (vehicletypeen[kl]->id == 34) {
           generatevehicle_cl ( vehicletypeen[kl],0 ,getactfield()->vehicle );
           computeview();
           displaymap();
           dashboard.x = 0xffff;
        }
}
*/





boolean      checkinput(char *       s)
{ 
   char* d   = s;
   int ss;
   if ( keyinputptr > 0 )
     ss = keyinputptr-1 ;
   else
     ss = keyinputbuffersize -1;

   boolean b = true;

   while ( d[1] ) d++;

   do {
      if ( keyinput[ss] != char2key(*d))
         b = false;

      if ( ss )
         ss--;
      else
         ss = keyinputbuffersize-1;

      d--;

   } while ( b && d>=s ); /* enddo */
   return b;
} 




void         newcampaign(void)
{ 
   tchoosenewcampaign tnc; 
   tnc.init(); 
   tnc.run(); 
   tnc.done(); 
} 



void         newsinglelevel(void)
{ 
  tchoosenewsinglelevel tnc; 

   tnc.init(); 
   tnc.run(); 
   tnc.done(); 
} 

void         startnewsinglelevelfromgame(void)
{ 
    cursor.hide(); 
    newsinglelevel(); 
    computeview(); 
    displaymap(); 
    cursor.show(); 
} 


   
void         ladestartkarte(void)
{         
  char s[300];

  if ( gameoptions.startupcount < 4 ) {
     strcpy ( s , "tutor0" );
  } else {
     strcpy ( s , "railstat" );
  }
  strcat ( s, &mapextension[1] );

  int maploadable;
  {

     tfindfile ff ( s );
   
     char* filename = ff.getnextname();

     maploadable = validatemapfile ( filename );
  }

  if ( !maploadable ) {
 
      tfindfile ff ( mapextension );

      char* filename = ff.getnextname();

      if ( !filename )
         displaymessage( "unable to load startup-map",2);
         
      while ( !validatemapfile ( filename ) ) {
         filename = ff.getnextname();
         if ( !filename )
            displaymessage( "unable to load startup-map",2);

      }
      strcpy ( s , filename );
   } 

   loadmap(s);
   initmap(); 
   #ifdef logging
   logtofile("nach initmap");
       {
           for ( int jj = 0; jj < 8; jj++ ) {
           char tmpcbuf[200];
           sprintf(tmpcbuf,"humanplayername; address is %x", actmap->humanplayername[jj]);
           logtofile ( tmpcbuf );
           }
       }
  #endif     
   if (loaderror != 0) 
      displaymessage("loadmap: \n could not open startup map", 2);

} 


void benchgame ( int mode )
{
   int t = ticker;
   int n = 0;
   int t2 = t;
   do {
      if ( mode == 1 )
         computeview();
      displaymap();
      n++;
      t2 = ticker;
   } while ( t + 1000 > t2 ); /* enddo */
   double d = 100 * n;
   d /= (t2-t);
   char buf[100];
   sprintf ( buf, "%3.1f", d );
   displaymessage2 ( " %s fps ", buf );
}

void  checkforvictory ( void )
{

   if ( !actmap->continueplaying ) {
      int plnum = 0;
      for ( int i = 0; i < 8; i++ )
         if ( actmap->player[i].existent )
            if ( !actmap->player[i].firstvehicle && !actmap->player[i].firstbuilding ) {
               actmap->player[i].existent = 0;

               int to = 0;
               for ( int j = 0; j < 8; j++ )
                  if ( j != i )
                     to |= 1 << j;
                     // Message player has been terminated.
   
   
               char txt[1000];
               char* sp = getmessage( 10010 ); 
               sprintf ( txt, sp, actmap->player[i].name );
               sp = strdup ( txt );
               pmessage msg = new tmessage ( sp, to );

               if ( i == actmap->actplayer ) {
                  displaymessage ( getmessage ( 10011 ),1 );
         
                  int humannum=0;
                  for ( int j = 0; j < 8; j++ )
                     if (actmap->player[j].existent && actmap->player[j].stat == ps_human )
                        humannum++;
                  if ( humannum )
                     next_turn();
                  else {
                     erasemap();                                
                     throw tnomaploaded();
                  }
               }
            } else
               plnum++;
   
      if ( plnum <= 1 ) {
         displaymessage("Congratulations!\nYou won!",1);
         if (choice_dlg("Do you want to continue playing ?","~y~es","~n~o") == 2) {
            erasemap();                                
            throw tnomaploaded();
         } else {
            actmap->continueplaying = 1;
            if ( actmap->replayinfo ) {
               delete actmap->replayinfo;
               actmap->replayinfo = 0;
            }
         }
      }
   }
}


void execuseraction ( tuseractions action )
{
   switch ( action ) {
            case ua_repainthard  :   repaintdisplayhard(); 
            break; 
            
            case ua_repaint      :   repaintdisplay(); 
            break; 
            
            case ua_help         :   help(20); 
            break; 
                                      
            case ua_howtostartpbem :   help(21); 
            break; 

            case ua_howtocontinuepbem :   help(22); 
            break; 

            case ua_showpalette  :   showpalette(); 
            break; 
                              
           /*
            case ct_f3:   computerturn(); 
            break; 
            
            case ct_f4:   if (getactfield()->vehicle != NULL)
                           displaymessage("completethreatvalue : %d",1, getactfield()->vehicle->completethreatvalue);
            break; 
            
            case ct_f5:   displaymessage("mine : %d ",1, getactfield()->mine );
            break; 
            
            case ct_f6:   longdistmove(); 
            break; 
            
            ct_f9     : displaymessage(strr(moveparams.movestatus),'',1); 
            break;
            
            case ct_f6:      if ( (++getactfield()->direction) > 7 )
                               getactfield()->direction = 0  ;
                            displaymap();
                            break;
           */                            

            case ua_dispvehicleimprovement    : displaymessage("Research:\n%s %d \n%s %d \n%s %d \n"
                                                                          "%s %d \n%s %d \n%s %d \n"
                                                                          "%s %d \n%s %d \n%s %d \n"
                                                                          "%s %d",1,
                                                  cwaffentypen[0], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[0]),
                                                  cwaffentypen[1], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[1]),
                                                  cwaffentypen[2], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[2]),
                                                  cwaffentypen[3], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[3]),
                                                  cwaffentypen[4], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[4]),
                                                  cwaffentypen[5], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[5]),
                                                  cwaffentypen[6], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[6]) ,
                                                  cwaffentypen[7], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[7])  ,
                                                  cwaffentypen[10], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[10])     ,
                                                  "armor",         (actmap->player[actmap->actplayer].research.unitimprovement.armor));   
                                                  break;
                           
            case ua_mainmenu:  if (choice_dlg("do you really want to close the current game ?","~y~es","~n~o") == 1) { 
                                   erasemap(); 
                                   throw tnomaploaded();
                               }
                               break;
                                     
            case ua_cheat_morefog: if (actmap->weather.fog < 255   && checkforcheats() ) {
                                      actmap->weather.fog++;
                                      computeview();
                                      displaymessage2("fog intensity set to %d ", actmap->weather.fog);
                                      displaymap();
                                   }
                        break;
                        
            case ua_cheat_lessfog: if (actmap->weather.fog  && checkforcheats()) {
                                      actmap->weather.fog--;
                                      computeview();
                                      displaymessage2("fog intensity set to %d ", actmap->weather.fog);
                                      displaymap();
                                   }
                        break;
               
            case ua_cheat_morewind: if ((actmap->weather.wind[0].speed < 254) &&  checkforcheats()) {
                                       actmap->weather.wind[0].speed+=2;
                                       actmap->weather.wind[2].speed = actmap->weather.wind[1].speed = actmap->weather.wind[0].speed;
                                       displaywindspeed (  );
                                       dashboard.x = 0xffff;
                                    }
                         break;
                         
            case ua_cheat_lesswind: if ((actmap->weather.wind[0].speed > 1)  && checkforcheats() ) {
                                       actmap->weather.wind[0].speed-=2;
                                       actmap->weather.wind[2].speed = actmap->weather.wind[1].speed = actmap->weather.wind[0].speed;
                                       displaywindspeed (  );
                                       dashboard.x = 0xffff;
                                    }
                         break;
                         
            case ua_cheat_rotatewind:  if ( checkforcheats() ) {
                                          if (actmap->weather.wind[0].direction < sidenum-1 ) 
                                             actmap->weather.wind[0].direction++;
                                          else
                                             actmap->weather.wind[0].direction = 0;
                                          actmap->weather.wind[2].direction = actmap->weather.wind[1].direction = actmap->weather.wind[0].direction;
                                          displaymessage2("wind dir set to %d ", actmap->weather.wind[0].direction);
                                          dashboard.x = 0xffff;
                                          resetallbuildingpicturepointers();
                                          displaymap();
                                       }
                                       break;
                                       
            case ua_changeresourceview:  showresources++;
                                         if ( showresources >= 3 )
                                            showresources = 0;
                                         displaymap();
                             break;
                             
            case ua_heapcheck: 
#ifdef _DOS_
	      if ( _heapchk() == _HEAPOK )
		displaymessage(" Heap OK", 3 );
	      else
		displaymessage(" Heap not OK", 1 );
#endif
	      break;
                     
            case ua_benchgamewov:  benchgame( 0 );
            break;
            
            case ua_benchgamewv :  benchgame( 1 );
            break;
            
            case ua_viewterraininfo:  viewterraininfo();              
                          break; 
            
         case ua_unitweightinfo:  if ( fieldvisiblenow  ( getactfield() )) {
                                     pvehicle eht = getactfield()->vehicle;
                                     if ( eht && eht->color == actmap->playerview*8 )
                                       displaymessage(" weight of unit: \n basic: %d\n+fuel: %d\n+material:%d\n+cargo:%d\n= %d",1 ,eht->typ->weight, eht->fuel * fuelweight / 1024 , eht->material * materialweight / 1024, eht->cargo(), eht->weight() );
                                  }
                         break;
                         
        case ua_writemaptopcx : writemaptopcx ();  
                                break;

        case ua_writescreentopcx: {
                                      char* nm = getnextfilenumname ( "screen", "pcx", 0 );
                                      writepcx ( nm, 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, pal );
                                      displaymessage2( "screen saved to %s", nm );
                                   }
                           break;

         case ua_scrolltodebugpage: if (videostartpos < 160) {
                                          videostartpos += 20; 
                                          setdisplaystart ( 0, videostartpos  );
                                       }
                          break;
                          
         case ua_scrolltomainpage : if (videostartpos >  0) {
                                       videostartpos -= 20; 
                                       setdisplaystart ( 0, videostartpos );
                                    }
                       break;
                       
         case ua_startnewsinglelevel: startnewsinglelevelfromgame();
                       break;
                       
         case ua_changepassword:      actmap->player[actmap->actplayer].passwordcrc = 0;
                                      enterpassword ( &actmap->player[actmap->actplayer].passwordcrc );
                       break;
                       
         case ua_gamepreferences:     gamepreferences();
                       break;
                       
         case ua_bi3preferences:      bi3preferences();
                       break;
                       
         case ua_exitgame:            if (choice_dlg("do you really want to quit ?","~y~es","~n~o") == 1) 
                                         abortgame = 1;
                       break;
                       
         case ua_newcampaign:         cursor.hide(); 
                                      newcampaign(); 
                                      computeview(); 
                                      displaymap(); 
                                      cursor.show(); 
                       break;
                       
         case ua_loadgame:            ladespiel(); 
                       break;
                       
        case ua_savegame:             speicherspiel( 1 ); 
                       break;
                       
        case ua_setupalliances:       setupalliances(); 
                                      logtoreplayinfo ( rpl_alliancechange );
                                      logtoreplayinfo ( rpl_shareviewchange );
                   
                                      if ( actmap->shareview && actmap->shareview->recalculateview ) {
                                         logtoreplayinfo ( rpl_shareviewchange );
                                         computeview();
                                         actmap->shareview->recalculateview = 0;
                                         displaymap();
                                      }
                                      dashboard.x = 0xffff;
                       break;
                       
        case ua_settribute :          settributepayments (); 
                       break;
                       
        case ua_giveunitaway:         giveunitaway ();
                       break;
                       
        case ua_vehicleinfo:          activefontsettings.font = schriften.smallarial; 
                                      vehicle(); 
                       break;
                       
        case ua_researchinfo:         researchinfo ();
                       break;
                       
        case ua_unitstatistics:       statisticarmies(); 
                       break;
                       
        case ua_buildingstatistics:   statisticbuildings();
                       break;
                       
        case ua_newmessage:           newmessage(); 
                       break;
                       
        case ua_viewqueuedmessages:   viewmessages( "queued messages", actmap->unsentmessage, 1, 0 ); 
                       break;
                       
        case ua_viewsentmessages:     viewmessages( "sent messages", actmap->player[ actmap->actplayer ].sentmessage, 0, 0 ); 
                       break;
                       
        case ua_viewreceivedmessages: viewmessages( "received messages", actmap->player[ actmap->actplayer ].oldmessage, 0, 1 ); 
                       break;
                       
        case ua_viewjournal:          viewjournal();
                       break;
                       
        case ua_editjournal:          editjournal();
                       break;
                       
        case ua_viewaboutmessage:     help(30);
                                      displaymessage2 ( getaboutmessage());
                       break;
                                          
        case ua_continuenetworkgame:   continuenetworkgame();
                                       displaymap();
                       break;
                       
        case ua_toggleunitshading:     gameoptions.units_gray_after_move = !gameoptions.units_gray_after_move;
                                       gameoptions.changed = 1;
                                       displaymap();
                                       while ( mouseparams.taste );
                                       if ( gameoptions.units_gray_after_move )
                                          displaymessage("units that can not move will now be displayed gray", 3);
                                       else
                                          displaymessage("units that can not move and cannot shoot will now be displayed gray", 3);
                       break;
/*        case ua_computerturn:          displaymessage("This function is under development and for programmers only\n"
                                                      "unpredicatable things may happen if you continue",3 ) ;
                                        if (choice_dlg("do you really want to start the AI?","~y~es","~n~o") == 1) 
                                           computerturn();
                       break; */
        case ua_setupnetwork:       if ( actmap->network )
                                       setupnetwork ( actmap->network );
                                    else
                                       displaymessage("This map is not played across a network",3 );
                       break;
    }                                   
                       

}

void checkpulldown( tkey* ch )
{
   pd.checkpulldown();

   if (pd.action2execute >= 0 ) {
      tuseractions ua = (tuseractions) pd.action2execute;
      pd.action2execute = -1;
      execuseraction ( ua );
   }
}


void mainloopgeneralkeycheck ( tkey& ch )
{
    screensaverparameters.lasttick = ticker; 
    ch = r_key(); 
    pd.key = ch;
    checkpulldown( &ch );
    keyinput[keyinputptr] = ch;
    keyinputptr++;
    if (keyinputptr >= keyinputbuffersize) 
       keyinputptr = 0;

    if ( checkforcheats() ) {
       if (checkinput("ALLMAP")) { 
          godview = true; 
          displaymap(); 
          ch = ct_invvalue; 
       } 
    }


    movecursor(ch); 

    actgui->checkforkey ( ch ); 


}




void mainloopgeneralmousecheck ( void )
{
    actgui->checkformouse();

    dashboard.checkformouse();

    if ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )   &&   mouseparams.y >= 71    &&   mouseparams.y <= 79   && (mouseparams.taste & 1) ) {
       pfield fld = getactfield();
       if ( fieldvisiblenow ( fld ) ) {
          if ( fld->vehicle )
             displaymessage2("damage is %d", fld->vehicle->damage );
          else
          if ( fld->building )
             displaymessage2("damage is %d", fld->building->damage );
          else
          if ( fld->object ) {
             char temp[1000];
             strcpy ( temp, "damage is " );
             for ( int i = fld->object->objnum-1; i >= 0; i-- ) 
                if ( fld->object->object[i]->typ->armor >= 0 ) {
                   strcat ( temp, strrr ( fld->object->object[i]->damage ));
                   strcat ( temp, " ");
                }

             displaymessage2( temp );
          }

       }
       while ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )  &&   mouseparams.y >= 71    &&   mouseparams.y <= 79   && (mouseparams.taste & 1) ) ;
    }

    if ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )   &&   mouseparams.y >= 59    &&   mouseparams.y <= 67   && (mouseparams.taste & 1) ) {
       pfield fld = getactfield();
       if ( fieldvisiblenow ( fld ) ) {
          if ( fld->vehicle )
             displaymessage2("unit has %d fuel", fld->vehicle->fuel );
       }
       while ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )   &&   mouseparams.y >= 59    &&   mouseparams.y <= 67   && (mouseparams.taste & 1) ) ;
    }

  if (lasttick + 5 < ticker) {
      if ((dashboard.x != getxpos()) || (dashboard.y != getypos())) {
         mousevisible(false);

         dashboard.paint ( getactfield(), actmap->playerview );

         actgui->painticons();

         mousevisible(true);
      } 
   }
   if ( memcheckticker + 100 < ticker ) {
      showmemory();
      memcheckticker = ticker ;
   }
   if ( lastdisplayedmessageticker + messagedisplaytime < ticker ) 
      displaymessage2("");

   if ( mousecontrol )
      mousecontrol->chkmouse();

   // checkfieldsformouse();

   {
      int oldx = actmap->xpos;
      int oldy = actmap->ypos;
      checkformousescrolling();
      if ( oldx != actmap->xpos || oldy != actmap->ypos )
         dashboard.x = 0xffff;
   }

   if ( onlinehelp )
      onlinehelp->checkforhelp();
   if ( onlinehelpwind && !gameoptions.smallmapactive )
      onlinehelpwind->checkforhelp();

   releasetimeslice();
}

void viewunreadmessages ( void )
{
      while ( actmap->player[ actmap->actplayer ].unreadmessage  ) {
         pmessagelist m = actmap->player[ actmap->actplayer ].unreadmessage ;
         viewmessage ( m->message );
         pmessagelist n = new tmessagelist ( &actmap->player[ actmap->actplayer ].oldmessage );
         n->message = m->message;

         actmap->player[ actmap->actplayer ].unreadmessage = m->next;
         delete m;
      }
}

void  mainloop ( void )
{
   tkey ch;
   memcheckticker = ticker;
   abortgame = 0;

   do { 
      viewunreadmessages();
      activefontsettings.background=0;
      activefontsettings.length=50;
      activefontsettings.color=14;
      if (keypress()) {

         mainloopgeneralkeycheck ( ch );

         switch (ch) {
         
            #ifndef NEWKEYB
            case 'R':   execuseraction ( ua_repainthard );
               break; 
            #else
            case ct_shp + ct_r:   execuseraction ( ua_repainthard );
               break; 
            #endif
            
            case ct_stp + ct_l: execuseraction ( ua_loadgame );
               break;
               
            case ct_stp + ct_n: execuseraction ( ua_startnewsinglelevel );
               break;
            
            case ct_r:  execuseraction ( ua_repaint ); 
               break; 
            
            case ct_f1:  execuseraction ( ua_help );
               break; 
                                      
            case ct_f2:  execuseraction ( ua_mainmenu );
               break;

            case ct_f3:  execuseraction ( ua_continuenetworkgame );
               break;

            case ct_f4:  execuseraction ( ua_computerturn );
               break;
               
/*            case ct_oe:  execuseraction ( ua_showpalette );
               break; 
                              
            case ct_f6: ellipse ( 100, 100, 140, 120, yellow, 0.1 );
               break; */
/*
            case ct_f12:  {
                            pvehicle v = getactfield()->vehicle;
                            if ( v ) {
                               if ( v->experience==0 ) {
                                  for ( int i= 0; i < v->typ->attack.weaponcount; i++ )
                                     if ( v->typ->attack.waffe[i].typ & cwweapon )
                                        v->munition[i] = v->typ->attack.waffe[i].count;
                                  char text[1000];
                                  sprintf ( text, "%s refueled his %s", actmap->player[actmap->actplayer].name, v->typ->description );
                                  char* sp = strdup ( text );
                                  pmessage msg = new tmessage ( sp, 255 );
                                  dashboard.x = 0xffff;
                               } else {
                                  char text[1000];
                                  sprintf ( text, "%s tried to refuel his %s, but it failed somehow ;->", actmap->player[actmap->actplayer].name, v->typ->description );
                                  char* sp = strdup ( text );
                                  pmessage msg = new tmessage ( sp, 255^( 1 << actmap->actplayer) );
                               }

                            }
                         }
               break;
*/
            case ct_f7:  execuseraction ( ua_dispvehicleimprovement );
               break;
                           
           /*
            case ct_plus: execuseraction ( ua_cheat_morefog );
               break;
               
            case ct_minus: execuseraction ( ua_cheat_lessfog );
               break;
           */
               
            case ct_f9: execuseraction ( ua_cheat_morewind );
               break;
               
            case ct_f10: execuseraction ( ua_cheat_lesswind );
               break;
               
            case ct_f8:  execuseraction ( ua_cheat_rotatewind );
               break;

            case ct_1:  execuseraction ( ua_changeresourceview );
               break;
            
            case ct_2:  execuseraction ( ua_toggleunitshading );
               break;
               
            case ct_3:  execuseraction ( ua_heapcheck );
               break;
#ifdef FREEMAPZOOM               
            case ct_4: choosezoomlevel();
               break;
#endif               
            case ct_5:  execuseraction ( ua_benchgamewov );
               break;
               
            case ct_6:  execuseraction ( ua_benchgamewv );
               break;
               
            case ct_7:  execuseraction ( ua_viewterraininfo );
               break; 
            
            case ct_8:  execuseraction ( ua_unitweightinfo );
               break;
               
            case ct_9:  execuseraction ( ua_writemaptopcx );
               break;

            case ct_0:  execuseraction ( ua_writescreentopcx );
               break;

            case ct_point: execuseraction ( ua_scrolltodebugpage );
               break;
               
            case ct_komma: execuseraction ( ua_scrolltomainpage );
               break;
               
            case ct_x + ct_stp:   execuseraction ( ua_exitgame );
               break;
         } 

      } 


      mainloopgeneralmousecheck ( );

/************************************************************************************************/
/*        Pulldown Men                                                                       . */
/************************************************************************************************/

      checkpulldown( &ch );


      checkscreensaver(); 


      while ( quedevents[ actmap->actplayer ] )
        checkevents(); 

      checktimedevents();

      checkforvictory();


   }  while ( !abortgame );
}



void dispmessageonexit ( void ) {
   int i;
   printf("\n");
   if (exitmessage[0] != NULL) {
      for (i=0;i<20 ;i++ ) {
          if (exitmessage[i] != NULL) {
             printf(exitmessage[i]);
             printf("\n");
          } /* endif */
      }
      printf("\npress any key to exit\n");
      //      getch();
   } else {
      printf( getstartupmessage() );
      printf("exiting ... \n \n");
   } /* endif */
}


void returntotextmode ( void ) {
   closesvga();
   settextmode ( 3 );
}


pfont load_font ( const char* name )
{
   tnfilestream stream ( name , 1 );
   return loadfont ( &stream );
}

#ifdef HEXAGON
const char* progressbarfilename = "progress.6mn";
#else
const char* progressbarfilename = "progress.8mn";
#endif







void loaddata( int resolx, int resoly ) {
                actprogressbar = new tprogressbar; 
                if ( actprogressbar ) {
                   tfindfile ff ( progressbarfilename );
                   if ( ff.getnextname() ) {
                      tnfilestream strm ( progressbarfilename, 1 );
                      actprogressbar->start ( 255, 0, agmp->resolutiony-3, agmp->resolutionx-1, agmp->resolutiony-1, &strm );
                   } else
                      actprogressbar->start ( 255, 0, agmp->resolutiony-3, agmp->resolutionx-1, agmp->resolutiony-1, NULL );
                }

                weapdist = new tweapdist;

                schriften.smallarial = load_font("smalaril.FNT");
                schriften.large = load_font("USABLACK.FNT");
                schriften.arial8 = load_font("ARIAL8.FNT");
                schriften.smallsystem = load_font("msystem.fnt");
                schriften.guifont = load_font("gui.fnt");
                schriften.guicolfont = load_font("guicol.fnt");
                schriften.monogui = load_font("monogui.fnt");
                activefontsettings.markfont = schriften.guicolfont;
                shrinkfont ( schriften.guifont, -1 );
                shrinkfont ( schriften.guicolfont, -1 );
                pulldownfont = schriften.smallarial ;

                if ( actprogressbar )
                   actprogressbar->startgroup();

                loadcursor();

                if ( actprogressbar )
                   actprogressbar->startgroup();
                loadguipictures();
                loadallobjecttypes();

                if ( actprogressbar )
                   actprogressbar->startgroup();
                loadallvehicletypes();

                if ( actprogressbar )
                   actprogressbar->startgroup();
                loadallbuildingtypes();

                if ( actprogressbar )
                   actprogressbar->startgroup();
                loadalltechnologies();


                if ( actprogressbar )
                   actprogressbar->startgroup();
                loadstreets();

                if ( actprogressbar )
                   actprogressbar->startgroup();
                loadallterraintypes();

                if ( actprogressbar )
                   actprogressbar->startgroup();


                cursor.init();
                selectbuildinggui.init( resolx, resoly );
                selectobjectcontainergui.init( resolx, resoly );
                selectvehiclecontainergui.init( resolx, resoly );

               #ifndef FREEMAPZOOM
                idisplaymap.setup_map_mask ( );
               #endif

                if ( actprogressbar )
                   actprogressbar->startgroup();

                ladestartkarte();
                // computeview();

                if ( actprogressbar )
                   actprogressbar->startgroup();

                gui.starticonload();

   #ifdef logging
   logtofile("nach gui.starticonload");
       {
           for ( int jj = 0; jj < 8; jj++ ) {
           char tmpcbuf[200];
           sprintf(tmpcbuf,"humanplayername; address is %x", actmap->humanplayername[jj]);
           logtofile ( tmpcbuf );
           }
       }
  #endif     

                if ( actprogressbar )
                   actprogressbar->startgroup();

                dashboard.allocmem ();

   #ifdef logging
   logtofile("vor mousecontrol");
       {
           for ( int jj = 0; jj < 8; jj++ ) {
           char tmpcbuf[200];
           sprintf(tmpcbuf,"humanplayername; address is %x", actmap->humanplayername[jj]);
           logtofile ( tmpcbuf );
           }
       }
  #endif     
                mousecontrol = new cmousecontrol;

                if ( actprogressbar ) {
                   actprogressbar->end();
                   try {
                      tnfilestream strm ( progressbarfilename, 2 );
                      actprogressbar->writetostream( &strm );
                   } /* endtry */
                   catch ( tfileerror ) {
                   } /* endcatch */
                   delete actprogressbar;
                   actprogressbar = NULL;
                }
}

void closemouse ( void )
{
   removemousehandler();
}








void networksupervisor ( void )
{
   class tcarefordeletionofmap {
          public:
            ~tcarefordeletionofmap (  ) 
            {  
                if ( actmap->xsize > 0  ||  actmap->ysize > 0 )
                   erasemap();
            };
         } carefordeletionofmap;


   tlockdispspfld ldsf;

   tnetwork network;

   int stat;
   do {
      stat = setupnetwork( &network, 1+8 );
      if ( stat == 1 ) 
         return;

   } while ( (network.computer[0].receive.transfermethod == 0) || (network.computer[0].receive.transfermethodid != network.computer[0].receive.transfermethod->getid()) ); /* enddo */


   try {
       displaymessage ( " starting network transfer ",0);
    
       network.computer[0].receive.transfermethod->initconnection ( receive );
       network.computer[0].receive.transfermethod->inittransfer ( &network.computer[0].receive.data );
    
       tnetworkloaders nwl;
       nwl.loadnwgame ( network.computer[0].receive.transfermethod->stream );
    
       network.computer[0].receive.transfermethod->closetransfer();
       network.computer[0].receive.transfermethod->closeconnection();
    
    
       // displaymessage ( " network transfer finished",1);
       removemessage();
       if ( actmap->network )
          setallnetworkpointers ( actmap->network );
   } /* endtry */

   catch ( tfileerror ) {
      displaymessage ("a file error occured while loading game",1 );
      erasemap();
      return;
   } /* endcatch */
   catch ( terror ) {
      displaymessage ("error loading game",1 );
      erasemap();
      return;
   } /* endcatch */


   int ok = 0;
   if ( actmap->supervisorpasswordcrc ) {
       taskforsupervisorpassword afsp;
       int a;
       afsp.init ( &actmap->supervisorpasswordcrc, 1 );
       afsp.run( &a );
       afsp.done();

       if ( a == 1 ) 
         ok = 1;
   } else {
      displaymessage ("no supervisor defined",1 );
      erasemap();
      return;
   }




   if ( ok ) {
      npush ( actmap->actplayer );
      actmap->actplayer = -1;
      setupalliances( 1 );
      npop ( actmap->actplayer );

      do {
         stat = setupnetwork( &network, 2+8 );
         if ( stat == 1 ) {
            displaymessage ("no changes were saved",1 );
            erasemap();
            return;
         }

      } while ( (network.computer[0].send.transfermethod == 0) || (network.computer[0].send.transfermethodid != network.computer[0].send.transfermethod->getid()) ); /* enddo */

      tnetworkcomputer* compi = &network.computer[ 0 ];

      displaymessage ( " starting network transfer ",0);

      try {
         compi->send.transfermethod->initconnection ( send );
         compi->send.transfermethod->inittransfer ( &compi->send.data );
   
         char* desciption = NULL;
         {
            tenterfiledescription efd;
            efd.init();
            efd.run();
            desciption = efd.description;
            efd.done();
         }
   
   
         tnetworkloaders nwl;
         nwl.savenwgame ( compi->send.transfermethod->stream, desciption );
   
         if ( desciption ) 
            delete desciption;
   
         compi->send.transfermethod->closetransfer();
         compi->send.transfermethod->closeconnection();
      } /* endtry */
      catch ( tfileerror ) {
         displaymessage ( "a file error occured while saving file", 1 );
      } /* endcatch */
      catch ( terror ) {
         displaymessage ( "error saving file", 1 );
      } /* endcatch */

      erasemap();
      displaymessage ( " network transfer finished",1);

   } else {
      displaymessage ("no supervisor defined or invalid password",1 );
      erasemap();
   }

}





void runmainmenu ( void )
{
    int mainmenuitemnum = 6;

    int unitsearched = 30;
    pvehicletype unit = getvehicletype_forid ( unitsearched );
    int xz = 228;
    int xs = 100;
    int ys = 25;
    int yd = 35;
    int y1 = 220;
    int xud = 40;
    int yud = -10;
    mousevisible ( false );
    backgroundpict.paint(); 
    activefontsettings.font = schriften.arial8;
    activefontsettings.background = 255;
    activefontsettings.justify = centertext;
    activefontsettings.height = 0;
    activefontsettings.length = 2*xs;
    for ( int i = 0; i < mainmenuitemnum; i++) {
       bar ( xz - xs, y1 + i * yd, xz + xs, y1 + i * yd + ys, dblue );
       showtext2 ( mainmenuitems[i], xz - xs, y1 + i * yd + ys/2 - activefontsettings.font->height / 2 );
    } /* endfor */
    int stat = -1;
    int pos = -1;
    int oldpos = -1;
    void* bkgr[2] ;
    int tnksize = imagesize ( 0, 0, fieldsizex, fieldsizey );
    bkgr[0] = asc_malloc ( tnksize );
    bkgr[1] = asc_malloc ( tnksize );
    mousevisible( true );
    do {
       if ( mouseparams.taste & 1 )
          for (int i = 0; i < mainmenuitemnum; i++) 
             if ( mouseparams.x >  xz - xs && mouseparams.y > y1 + i * yd  && mouseparams.x < xz + xs && mouseparams.x < y1 + i * yd + ys ) {
                stat = i;
                while ( mouseparams.taste & 1 );
             } /* endfor */

       if (keypress()) {
         tkey ch = r_key(); 
         switch ( ch ) {
            case ct_down: if ( pos < mainmenuitemnum-1 )
                             pos++;
                          else
                             pos=0;
               break;
            case ct_up: if ( pos > 0 )
                            pos--;
                        else
                            pos = mainmenuitemnum-1;
               break;
            case ct_enter:
            case ct_space: stat = pos;
               break;
         } /* endswitch */
       }
       if ( pos != oldpos ) {
          if ( oldpos != -1 ) {
             setinvisiblemouserectanglestk ( xz - xs - xud - fieldsizex, y1 + oldpos * yd + yud,  
                                             xz - xs - xud            , y1 + oldpos * yd + yud + fieldsizey );
             putimage ( xz - xs - xud - fieldsizex, y1 + oldpos * yd + yud,  bkgr[0] );
             getinvisiblemouserectanglestk ();

             setinvisiblemouserectanglestk ( xz + xs + xud,             y1 + oldpos * yd + yud,  
                                             xz + xs + xud + fieldsizex, y1 + oldpos * yd + yud + fieldsizey );
             putimage ( xz + xs + xud, y1 + oldpos * yd + yud,  bkgr[1] );
             getinvisiblemouserectanglestk ();
          }
          if ( pos != -1 ) {
             setinvisiblemouserectanglestk ( xz - xs - xud - fieldsizex, y1 + pos * yd + yud,  
                                             xz - xs - xud            , y1 + pos * yd + yud + fieldsizey );
             getimage ( xz - xs - xud - fieldsizex, y1 + pos * yd + yud,  
                        xz - xs - xud            , y1 + pos * yd + yud + fieldsizey,  bkgr[0] );
             putrotspriteimage90  ( xz - xs - xud - fieldsizex, y1 + pos * yd + yud,  unit->picture[0], 0 );
             getinvisiblemouserectanglestk ();

             setinvisiblemouserectanglestk ( xz + xs + xud,             y1 + pos * yd + yud,  
                                             xz + xs + xud + fieldsizex, y1 + pos * yd + yud + fieldsizey );
             getimage ( xz + xs + xud,             y1 + pos * yd + yud,  
                        xz + xs + xud + fieldsizex, y1 + pos * yd + yud + fieldsizey,  bkgr[1] );
             putrotspriteimage270 ( xz + xs + xud,             y1 + pos * yd + yud,  unit->picture[0], 0 );
             getinvisiblemouserectanglestk ();
          }
          oldpos = pos;
       }
    } while ( stat == -1 ); /* enddo */

    delete  ( bkgr[0] );
    delete  ( bkgr[1] );

    switch ( stat ) {
    case 0: newsinglelevel();
       break;
    case 1: newcampaign();
       break;
    case 2: ladespiel();
       break;
    case 3: continuenetworkgame (  );
       break;
    case 4: networksupervisor ( );
       break;
    case 5: abortgame = 1;
       break;
    } /* endswitch */
}


void closecdrom( void )
{
//   end_real_int();
//   freecdinfo();
}



void startcdaudio ( char c )
{
   /*
    if ( toupper( c ) < 'A' || toupper( c ) > 'Z' )
       cdrom = 0;
 
    if ( cdrom ) {
       init_real_int(cdromintmemsize);
       atexit ( closecdrom );
       if ( testcdrom()) {
          getcdrominfo();
          activecdrom = cdrominfo.driveletter[0];
          if ( 'A' + activecdrom == toupper( c ))
             cdrom = 0;
          else { 
             readcdinfo();
             if ( cdinfo.lasttrack >= 2 ) 
                playaudiotrack(cdinfo.track[1]->start, cdinfo.size - cdinfo.track[0]->size);
             else
                cdrom = 0;
          }
       } else
          cdrom = 0;
    }
    */
}


class tnewkeyb {
               public:
               tnewkeyb ( void ) 
               {
                  #ifdef NEWKEYB
                  initkeyb();
                  #endif
               };
                                  
               ~tnewkeyb (  ) 
               {
                  #ifdef NEWKEYB
                  closekeyb();
                  #endif
               };
          };

void showavailablemodes ( void ) 
{
   printf("\navailable graphic modes:\n");
   pavailablemodes avm = searchformode ( 0, 0, 8 );
   for ( int i = 0; i < avm->num; i++ )
      if ( avm->mode[i].x >= 640  &&  avm->mode[i].y >= 480 )
         printf("  %d * %d \n", avm->mode[i].x, avm->mode[i].y );
}


int main(int argc, char *argv[] )
{  
   // dont_use_linear_framebuffer = 1;


  
  if( sizeof(tvehicletype) != 601 || 
      sizeof(byte) != 1 ||
      sizeof(word) != 2 || 
      sizeof(integer) != 2 ) {
      printf("\n ASC was compiled with invalid structure sizes! Pack all structures ! \n\n" );
      return 1;
  }


  int i;

  #ifdef HEXAGON
   int resolx = 800;
   int resoly = 600;
  #else
   int resolx = 640;
   int resoly = 480;
  #endif

   int showmodes = 0;

   #ifdef logging
   logtofile ( getstartupmessage() );
   logtofile ( "\n\n new log started ");
   #ifdef NEWKEYB
   logtofile ( "new keyboard handler ist enabled" );
   #else
   logtofile ( "new keyboard handler ist disabled" );
   #endif
   #endif

       int cntr = gettimestampcounter();

        printf( getstartupmessage() );


        for (i = 1; i<argc; i++ ) {
           if ( argv[i][0] == '/'  ||  argv[i][0] == '-' ) {
              if ( strcmpi ( &argv[i][1], "V1" ) == 0 ) 
                 vesaerrorrecovery = 1;
              else 
                 if ( strcmpi ( &argv[i][1], "NOCD" ) == 0 )
                    cdrom = 0;
                 else
                    /*
                    if ( (strnicmp ( &argv[i][1], "GAME:" , 5) == 0) || (strnicmp ( &argv[i][1], "GAME=" , 5) == 0)) {
                       strcpy ( gamepath, &argv[i][6]);
                       if ( gamepath[strlen(gamepath)-1] != '\\' )
                          strcat ( gamepath, "\\" );

                       if ( !checkforvaliddirectory ( gamepath ) ) {
                           printf ( "\nInvalid directory: %s \n", gamepath );
                           exit(1);
                       }
                    } else */
                       if ( strcmpi ( &argv[i][1], "SHOWMODES" ) == 0 ) {
                          showmodes = 1;
                       } else
                          if ( strcmpi ( &argv[i][1], "8BITONLY" ) == 0 )
                             modenum24 = -2;
                          else
                             if ( strnicmp ( &argv[i][1], "x=", 2 ) == 0 ) {
                                resolx = atoi ( &argv[i][3] );
                             } else
                             if ( strnicmp ( &argv[i][1], "x:" ,2 ) == 0 ) {
                                resolx = atoi ( &argv[i][3] );
                             } else
                             if ( strnicmp ( &argv[i][1], "y=" ,2 ) == 0 ) {
                                resoly = atoi ( &argv[i][3] );
                             } else
                             if ( strnicmp ( &argv[i][1], "y:" ,2 ) == 0 ) {
                                resoly = atoi ( &argv[i][3] );
                             }
                             else 
                                if ( ( strcmpi ( &argv[i][1], "?" ) == 0 ) || ( strcmpi ( &argv[i][1], "h" ) == 0 ) ){
                                   printf( " Parameters: \n"
                                           "     /h          This page\n"
                                           "     /v1         Set vesa error recovery level to 1 \n"    
                                           "     /nocd       Disable music \n"
                                           "     /8bitonly   Disable truecolor graphic mode \n"
                                           "     /x:X        Set horizontal resolution to X; default is 800 \n"
                                           "     /y:Y        Set verticalal resolution to Y; default is 600 \n"
                                           "     /showmodes  Display list of available graphic modes \n");
                                           //"     /game:X     Set gamepath to X \n\n");
                                   exit (0);
                
                                } else {
                                    printf ( "\nInvalid command line parameter: %s \nUse /h to for help\n", argv[i] );
                                    exit(1);
                                }
           } else {
               printf ( "\nInvalid command line parameter: %s \n", argv[i] );
               exit(1);
           }
        } /* endfor */


       if ( resolx < 640 || resoly < 480 ) {
          printf ( "Cannot run in resolution smaller than 640*480 !\n");
          exit(1);
       }
       if ( showmodes ) {
           showavailablemodes();
           return 0;
       }

       // The following code is DOS only.  Unix machines will swap to make
       // room for the memory needed.  Only need to make sure we check all
       // dynamic allocation calls for failure.

#ifdef _DOS_
       int memneeded = 20000000;
       int ma = maxavail() + _memavl();

       printf(" \n memory avaiable: %d \n ", ma );
       if ( ma < memneeded ) {
	 printf(" Not enough momory available, at least %d MB recommended.\n"
		" You may try to run the game, but unpredictable results can happen.\n"
		" Press R to start the game anyway or any other key to quit\n", memneeded/1000000 );
	 
	 int ch = getch();
	 if ( toupper ( ch ) != 'R' )
	   exit(2);
       }
#endif


        #ifdef logging
        logtofile ( "sg.cpp / main / allocating reserved memory ");
        #endif

        mapborderpainter = &backgroundpict;

        boolean          truecoloravail;


        reservememory = malloc ( reservememorysize );
        emergencymemory = malloc ( emergencymemorysize );
        set_new_handler ( new_new_handler );

        #ifdef logging
        logtofile ( "sg.cpp / main / initmissions ");
        #endif
        initmissions();

        memset(exitmessage, 0, sizeof ( exitmessage ));
        atexit ( dispmessageonexit );

        #ifdef logging
        logtofile ( "sg.cpp / main / initmisc ");
        #endif
        initmisc ();



        #ifdef logging
        logtofile ( "sg.cpp / main / initializing containerstream ");
        #endif
        t_carefor_containerstream cfc;

        readgameoptions();
       #ifdef HEXAGON
        check_bi3_dir ();
       #endif

/*
        for ( int z = 0; i < 20; i++ ) {
           int a = maxavail();
           int b = _memavl();
           char buf[15000];
           tnfilestream stream ( "smalaril.fnt", 1 );
           stream.readdata ( buf, 15000 );
        }
        int a = maxavail();
        int b = _memavl();
*/

        #ifdef logging
        logtofile ( "sg.cpp / main / searching for 8 bit graphic mode ");
        #endif
         pavailablemodes avm = searchformode ( resolx, resoly, 8 );

        if (avm->num > 0) {

           for (i=0;i<avm->num ;i++ ) {
              printf("8 Bit mode available: %x        X: %d   Y: %d \n", avm->mode[i].num, avm->mode[i].x, avm->mode[i].y);
              modenum8 = avm->mode[i].num;
           } /* endfor */

           printf("Initializing ... \n");
           
           #ifdef logging
           logtofile ( "sg.cpp / main / initializing 8-Bit graphic mode ");
           #endif
           
           initsvga( modenum8 );
           atexit ( returntotextmode );

           
           #ifdef logging
           logtofile ( "sg.cpp / main / initspfst ");
           #endif

           #ifdef HEXAGON
           initspfst( -1, -1 ); // 6, 16 
           #else
           initspfst();
           #endif

           gui.init ( resolx, resoly );

           virtualscreenbuf.init();

           delete  ( avm );


           #ifdef logging
           logtofile ( "sg.cpp / main / searching for 32-Bit graphic mode ");
           #endif
           
           if ( modenum24 == -1 ) {      // modenum24 is -2 when the command line parameter /8bitonly is used
              avm = searchformode ( 640, 480,32 );
              if (avm->num > 0) {
      
                 for (i=0;i<avm->num ;i++ ) {
                    // printf("32 Bit mode available: %x        X: %d   Y: %d \n", avm->mode[i].num, avm->mode[i].x, avm->mode[i].y);
                    modenum24 = avm->mode[i].num;
                 } /* endfor */
      
              } else {
                 delete  ( avm );
                 avm = searchformode ( 640 ,480, 24 );
                 if (avm->num > 0) {
                    for (i=0;i<avm->num ;i++ ) {
                      // printf("24 Bit mode available: %x        X: %d   Y: %d \n", avm->mode[i].num, avm->mode[i].x, avm->mode[i].y);
                      modenum24 = avm->mode[i].num;
                    } /* endfor */
                 }
              } /* endif */
              delete  ( avm );
           }
   

           if ( modenum24 > 0 ) {
              closesvga();
              
              #ifdef logging
              logtofile ( "sg.cpp / main / initializing truecolor graphic mode ");
              #endif
              initsvga( modenum24 );

              char pcx[300];
              char jpg[300];
              int md = getbestpictname ( "helisun", pcx, jpg );

              try {

                  if ( md & 1 ) {
                     tnfilestream stream ( pcx , 1 );
                     loadpcxxy ( &stream, 0, 0 );
                  } else {
                     tnfilestream stream ( jpg, 1 );
                     read_JPEG_file ( &stream );
                  }

                 #ifdef logging
                 logtofile ( "sg.cpp / main / loading data ");
                 #endif
                 
                 loaddata( resolx, resoly );
                 // startcdaudio ( argv[0][0] );
              } /* endtry */

              catch ( tfileerror err ) {
                 displaymessage ( "unable to access file %s \n", 2, err.filename );
              } /* endcatch */
              catch ( terror err ) {
                 displaymessage ( "loading of game failed", 2 );
              } /* endcatch */

              closesvga();
              initsvga( modenum8 );
           } else {
              truecoloravail = false;

              try {
                 {
                    tnfilestream stream ( "logo640.pcx", 1 );
                    loadpcxxy( &stream, (hgmp->resolutionx - 640)/2, (hgmp->resolutiony-35)/2, 1 ); 
                 } 
                 #ifdef logging
                 logtofile ( "sg.cpp / main / loading data ");
                 #endif
                 loaddata( resolx, resoly );
                 
                 // startcdaudio ( argv[0][0] );
              } /* endtry */

              catch ( tfileerror err ) {
                 displaymessage ( "unable to access file %s \n", 2, err.filename );
              } /* endcatch */
              catch ( terror err ) {
                 displaymessage ( "loading of game failed", 2 );
              } /* endcatch */
   
           } /* endif */

           #ifdef logging
           logtofile ( "sg.cpp / main / initializing keyboard handler ");
           {
               for ( int jj = 0; jj < 8; jj++ ) {
                  char tmpcbuf[200];
                  sprintf(tmpcbuf,"humanplayername; address is %x", actmap->humanplayername[jj]);
                  logtofile ( tmpcbuf );
               }
           }    
           #endif
                 
           if ( initmousehandler( icons.mousepointer )) 
              displaymessage("mouse required", 2 );

           atexit ( closemouse );

           tnewkeyb nkb;

           setvgapalette256(pal);
           xlatpictgraytable = (ppixelxlattable) asc_malloc( sizeof(*xlatpictgraytable) );
           generategrayxlattable(xlatpictgraytable,160,16); 
           (*xlatpictgraytable)[255] = 255; 

           #ifdef logging
           logtofile ( "sg.cpp / main / initializing mouse handler ");
           #endif
           addmouseproc ( &mousescrollproc );

           loadtexture();

           #ifdef logging
           logtofile ( "sg.cpp / main / initializing timer handler ");
           #endif
           inittimer(100);
           atexit ( closetimer );
           
           onlinehelp = new tsgonlinemousehelp;
           onlinehelpwind = new tsgonlinemousehelpwind;

           /* erstaufbau();  */
           pd.init();

           memset( keyinput, 0, sizeof(keyinput));
           keyinputptr = 0;
           abortgame = 0;
           do {
              try {
                 if ( actmap->xsize == 0 || actmap->ysize == 0 ) {
                    runmainmenu();
                 } else {
                    if ( actmap->actplayer == -1 )
                       next_turn();

                    backgroundpict.paint(); 

                    #ifdef logging
                    logtofile ( "sg.cpp / main / vor displaymap ");
                    #endif

                    displaymap();

                    #ifdef logging
                    logtofile ( "sg.cpp / main / nach displaymap ; vor cursor.show ");
                    #endif

                    // cursor.gotoxy ( actmap->cursorpos.position[ actmap->actplayer ].x, actmap->cursorpos.position[ actmap->actplayer ].y , 0);
                    cursor.show(); 
         
                    #ifdef logging
                    logtofile ( "sg.cpp / main / vor painticons ");
                    #endif

                    moveparams.movestatus = 0; 
         
                    actgui->painticons();
   
                    mousevisible(true); 
                    dashboard.x = 0xffff; 
                    dashboard.y = 0xffff; 
                    #ifdef logging
                    /*
                    // char* a = (char*) 0x600000;
                    // char* a = (char*) 0xb80000;
                    char* a = (char*) computerturn;
                    *a = 123;
                    */
                    logtofile ( "sg.cpp / main / starting mainloop ");
                    #endif
                    static int displayed = 0;
                    if ( !displayed ) 
                       displaymessage2( "million clock ticks for startup: %d", gettimestampcounter()-cntr );
                    displayed = 1;
                    mainloop();
                    mousevisible ( false );
                 }
              } /* endtry */
              catch ( tnomaploaded ) {
              } /* endcatch */
           } while ( abortgame == 0);

           closesvga();

           writegameoptions ();

           delete onlinehelp;
           onlinehelp = NULL;
           delete onlinehelpwind;
           onlinehelpwind = NULL;

     
        } else {
           printf("No graphic-mode available !\n");
           showavailablemodes();
           return 1;
        } /* endif */
        

        #ifdef logging
        logtofile ( "sg.cpp / main / returning ");
        #endif
        #ifdef MEMCHK
        verifyallblocks();
        #endif
        return(0);
}

