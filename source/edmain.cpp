//     $Id: edmain.cpp,v 1.26 2000-10-18 17:09:39 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.25  2000/10/18 15:10:03  mbickel
//      Fixed event handling for windows and dos
//
//     Revision 1.24  2000/10/18 14:14:06  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//
//     Revision 1.23  2000/10/11 14:26:30  mbickel
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
//     Revision 1.22  2000/09/07 16:42:28  mbickel
//      Made some adjustments so that ASC compiles with Watcom again...
//
//     Revision 1.21  2000/09/07 15:49:40  mbickel
//      some cleanup and documentation
//
//     Revision 1.20  2000/08/21 17:50:57  mbickel
//      Fixed: crash when unit reaching max experience
//      Fixed: crash when displaying research image
//      Fixed: crash when events referenced a unit that has been shot down
//      Fixed: screenshot being written to wrong directory
//
//     Revision 1.19  2000/08/02 15:52:56  mbickel
//      New unit set definition files
//      demount accepts now more than one container file
//      Unitset information dialog added
//
//     Revision 1.18  2000/08/01 10:39:09  mbickel
//      Updated documentation
//      Refined configuration file handling
//
//     Revision 1.17  2000/07/31 19:16:42  mbickel
//      Improved handing of multiple directories
//      Fixed: wind direction not displayed when cycling through wind heights
//      Fixed: oil rig not working
//      Fixed: resources becomming visible when checking mining station status
//      Fixed: division by zero when moving unit without fuel consumption
//
//     Revision 1.16  2000/07/31 18:02:53  mbickel
//      New configuration file handling
//      ASC searches its data files in all directories specified in ascrc
//      Renamed all tools so they begin with asc
//
//     Revision 1.15  2000/07/16 14:20:02  mbickel
//      AI has now some primitive tactics implemented
//      Some clean up
//        moved weapon functions to attack.cpp
//      Mount doesn't modify PCX files any more.
//
//     Revision 1.14  2000/05/23 20:40:44  mbickel
//      Removed boolean type
//
//     Revision 1.13  2000/05/22 15:40:34  mbickel
//      Included patches for Win32 version
//
//     Revision 1.12  2000/05/10 19:55:49  mbickel
//      Fixed empty loops when waiting for mouse events
//
//     Revision 1.11  2000/05/06 19:57:08  mbickel
//      Mapeditor/linux is now running
//
//     Revision 1.10  2000/05/05 21:15:02  mbickel
//      Added Makefiles for mount/demount and mapeditor
//      mapeditor can now be compiled for linux, but is not running yet
//
//     Revision 1.9  2000/04/27 16:25:21  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.8  2000/03/16 14:06:54  mbickel
//      Added unitset transformation to the mapeditor
//
//     Revision 1.7  2000/02/24 10:54:08  mbickel
//      Some cleanup and bugfixes
//
//     Revision 1.6  2000/02/03 20:54:39  mbickel
//      Some cleanup
//      getfiletime now works under Linux too
//
//     Revision 1.5  1999/12/27 12:59:54  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.4  1999/12/07 22:05:08  mbickel
//      Added password verification for loading maps
//
//     Revision 1.3  1999/11/22 18:27:16  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:41:36  tmwilson
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

#include "edmisc.h"
#include "loadpcx.h"
#include "loadbi3.h"
#include "edselfnt.h"
#include "edglobal.h"
#include "errors.h"

#include <signal.h>

#ifdef _DOS_
 #include "dos\memory.h"
#endif

// #define backgroundpict1 "BKGR2.PCX"  
#define menutime 35

//* õS Load-Functions

pprogressbar actprogressbar = NULL;


#ifdef HEXAGON
const char* progressbarfilename = "progress.6me";
#else
const char* progressbarfilename = "progress.8me";
#endif


//  #define MEMCHK


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

  void verifyallblocks( void );

  void* memchkAlloc ( int tp, size_t amt )
  {
     // verifyallblocks();
     int error;
     void* tmp = malloc ( amt + 53 * 4 );
    #ifdef _DOS_
     if ( !tmp )
        new_new_handler();
    #endif

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

     if ( tmpi[1] != (int) tmpi) {
        error++;
        #ifdef logging
         logtofile ( "memory check: verifyblock : error A at address %x", p );
        #endif
     }

     int amt = tmpi[2];

     for ( int i = 0; i < 25; i++ ) {

        if ( tmpi[3 + i] != 0x12345678)
           if ( i == 1  &&  tmpi[3 + i] == -2) {
              error++;  // deallocated twice 
              #ifdef logging
               logtofile ( "memory check: verifyblock : error B at address %x", p );
              #endif
           } else {
              error++;
              #ifdef logging
               logtofile ( "memory check: verifyblock : error C at address %x", p );
              #endif
           }

        if ( tmpi[3 + i + (amt+3)/4 + 25] != 0x87654321 ) {
           error++;
           #ifdef logging
            logtofile ( "memory check: verifyblock : error D at address %x", p );
           #endif
        }
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

        int* tmpi2 = (int*) buf;
        tmpi2 -= 28;
        tmpi2[4] = -2;


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
    #ifdef _DOS_
     if ( tmp == NULL ) 
        new_new_handler();
    #endif
     return tmp;
  }

  void asc_free ( void* p )
  {
     free ( p );
  }


#endif




void         loadcursor(void)
{ 
  int          w,i;

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
      tnfilestream stream ("mapbkgrb.raw", 1);
      stream.readrlepict ( &icons.mapbackground, false, &w );
   }
   #endif

  {
    #ifdef HEXAGON
      tnfilestream stream ("hexfld_a.raw",1); 
    #else
      tnfilestream stream ("markacti.raw",1); 
    #endif
    stream.readrlepict( &icons.stellplatz, false, &w);
  }

  {
    tnfilestream stream ("x.raw",1); 
    stream.readrlepict( &icons.X, false, &w);
  }

  {
     tnfilestream stream ("pfeil-a0.raw",1);
     for (i=0;i<8 ;i++ ) stream.readrlepict( &icons.pfeil2[i], false, &w);
  }

   loadpalette();
   for (w=0;w<256 ;w++ ) {
      palette16[w][0] = pal[w][0];
      palette16[w][1] = pal[w][1];
      palette16[w][2] = pal[w][2];
      xlattables.nochange[w] = w;
   } /* endfor */

   loadicons(); 
   loadmessages(); 

} 





void loaddata( void ) 
{

   actprogressbar = new tprogressbar; 
   {
      tfindfile ff ( progressbarfilename );
      if ( ff.getnextname() ) {
         tnfilestream strm ( progressbarfilename, 1 );
         actprogressbar->start ( 255, 0, agmp->resolutiony-2, agmp->resolutionx-1, agmp->resolutiony-1, &strm );
      } else
         actprogressbar->start ( 255, 0, agmp->resolutiony-2, agmp->resolutionx-1, agmp->resolutiony-1, NULL );
   }

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

  #ifndef FREEMAPZOOM
   idisplaymap.setup_map_mask ( );
  #endif

   loadUnitSets();

   if ( actprogressbar ) {
      actprogressbar->end();
      tnfilestream strm ( progressbarfilename, 2 );
      actprogressbar->writetostream( &strm );
      delete actprogressbar;
      actprogressbar = NULL;
   }
}

void buildemptymap ( void )
{
  #ifdef HEXAGON
   if ( getterraintype_forid(30) )
      generatemap(getterraintype_forid(30)->weather[0], idisplaymap.getscreenxsize(1), idisplaymap.getscreenysize());
   else
      generatemap(getterraintype_forpos(0)->weather[0], idisplaymap.getscreenxsize(1), idisplaymap.getscreenysize());
  #else
   if ( getterraintype_forid(1) )
      generatemap(getterraintype_forid(1)->weather[0], idisplaymap.getscreenxsize(1), idisplaymap.getscreenysize());
   else
      generatemap(getterraintype_forpos(0)->weather[0], idisplaymap.getscreenxsize(1), idisplaymap.getscreenysize());
  #endif
}


void         editor(void)
{  int execcode;
   int lastx;
   int lasty;
   char curposchanged;

   cursor.show();
   do {
      try {
         do { 
            if ( ( keypress() ) || ( ( ch >=ct_f3) && (ch<=ct_f9) ) ) {
               if (! ( ( ch >=ct_f3) && (ch<=ct_f9) ) ) ch = r_key();
               pd.key = ch;
                  switch (ch) {
                  #ifdef NEWKEYB
                  case ct_up:
                  case ct_down:
                  case ct_left:
                  case ct_right:
                  case ct_up + ct_stp:
                  case ct_down + ct_stp:
                  case ct_left + ct_stp:
                  case ct_right + ct_stp:
                  #endif
                  case ct_1k:   
                  case ct_2k:   
                  case ct_3k:   
                  case ct_4k:   
                  case ct_5k:   
                  case ct_6k:   
                  case ct_7k:   
                  case ct_8k:   
                  case ct_9k:   
                  case ct_1k + ct_stp:   
                  case ct_2k + ct_stp:   
                  case ct_3k + ct_stp:   
                  case ct_4k + ct_stp:   
                  case ct_5k + ct_stp:   
                  case ct_6k + ct_stp:   
                  case ct_7k + ct_stp:   
                  case ct_8k + ct_stp:   
                  case ct_9k + ct_stp:   if ( polyfieldmode == false ) { 
                                                    mousevisible(false); 
                                                    movecursor(ch); 
                                                    cursor.show(); 
                                                    showcoordinates();
                                                    mousevisible(true); 
                                                 }
                     break;
                  case ct_f1:   execaction(act_help);
                     break;
                  case ct_f3 : execaction(act_selbodentyp);
                     break;
                  case ct_f4 : execaction(act_selunit);
                     break;
                  case ct_f5 : execaction(act_selcolor);
                     break;
                  case ct_f6 : execaction(act_selbuilding);
                     break;
                  case ct_f7 : execaction(act_selobject);
                     break;
                  case ct_f8 : execaction(act_selmine);
                     break;
                  case ct_f9 : execaction(act_selweather);
                     break;
                  case ct_a + ct_stp :  execaction(act_setupalliances);
                     break;
                  case ct_b + ct_stp:  execaction(act_toggleresourcemode);
                     break;
                  case ct_d + ct_stp : execaction(act_changeglobaldir);
                     break;
                  case ct_f + ct_stp: execaction(act_createresources);
                     break;
                  case ct_g + ct_stp: execaction(act_maptopcx);  
                     break;
                  case ct_h + ct_stp : execaction(act_setunitfilter);
                     break;
                  case ct_i + ct_stp: execaction (act_import_bi_map );
                     break;
                  case ct_l + ct_stp : execaction(act_loadmap); 
                     break;
                  case ct_m + ct_stp: execaction(act_changemapvals);
                     break;
                  case ct_n + ct_stp: execaction(act_newmap);
                     break;
                  case ct_o + ct_stp: execaction(act_polymode);  
                     break;            
                  case ct_r + ct_stp: execaction(act_repaintdisplay);
                     break;
                  case ct_u + ct_stp : execaction(act_unitinfo);
                     break;
                  case ct_v + ct_stp: execaction(act_viewmap);
                     break;
                  case ct_w + ct_stp : execaction(act_setactweatherglobal);
                     break;
                  case ct_x + ct_stp: execaction(act_end);
                     break;
                  case ct_d + ct_shp: execaction(act_changeunitdir);
                     break;
                  case ct_a:   execaction(act_movebuilding);
                     break;
                  case ct_b:   execaction(act_changeresources);
                     break;
                  case ct_c:   execaction(act_changecargo); 
                     break;
                  case ct_d : execaction(act_changeterraindir);
                     break;
                  case ct_e:  execaction(act_events);
                     break;
                  case ct_f:  execaction(act_fillmode); 
                     break;
                  case ct_g: execaction(act_mapgenerator);
                     break;
                  case ct_h: execaction(act_setactivefieldvals);
                     break;
                  case ct_entf: execaction(act_deletething); 
                      break;
                  case ct_l : execaction(act_showpalette);
                     break;
                  case ct_m : execaction(act_changeminestrength);
                     break;
                  case ct_o: execaction(act_changeplayers);
                     break;
                  case ct_p: execaction(act_changeproduction);
                     break;
                  case ct_r: execaction(act_resizemap);
                     break;
                  case ct_s : execaction(act_savemap);
                     break;
                  case ct_v:   execaction(act_changeunitvals); 
                     break;
                  case ct_x:   execaction(act_mirrorcursorx);
                     break;
                  case ct_y:   execaction(act_mirrorcursory);
                     break;
                  case ct_z:   execaction(act_setzoom );
                     break;
                  case ct_3 : execaction(act_placebodentyp);
                     break;
                  case ct_4 : execaction(act_placeunit);
                     break;
                  case ct_6 : execaction(act_placebuilding);
                     break;
                  case ct_7 : execaction(act_placeobject);
                     break;
                  case ct_8 : execaction(act_placemine);
                     break;
                  case ct_space : execaction(act_placething);
                     break;
                  case ct_esc : {
                        if (polyfieldmode) execaction(act_endpolyfieldmode);
                        else execaction(act_end);
                     }
                     break;
                  }
            } 
            pulldown();
            checkselfontbuttons();
            if ( mouseparams.taste == 1 ) {
               int mx, my;
               starttimer();
               curposchanged = false;
               while ( mouseparams.taste == 1 ) {
                 if ( getfieldundermouse ( &mx, &my ) ) 
                    if ( ( mx != lastx ) || (my != lasty ) ) {
                       mousevisible(false);
                       cursor.hide();
                       cursor.posx = mx;
                       cursor.posy = my;
                       cursor.show();
                       mousevisible(true);
      
                       lastx = mx;
                       lasty = my;
                       curposchanged = true;
                       starttimer();
                    }
                    if (time_elapsed(menutime)) {
                       execcode = -1;
                       execcode = leftmousebox();
                       if (execcode != -1 ) execaction(execcode);
                       while ( mouseparams.taste != 0 );
                  }
                  releasetimeslice();
               }
               if (getfieldundermouse ( &mx, &my ) ) 
                  if ( ! time_elapsed(menutime)) {
                       if ( ! curposchanged ) { 
                          execaction(act_placething);
                          while ( mouseparams.taste == 1 )
                             releasetimeslice();
                       }
                  }
            }
            if ( mouseparams.taste == 2 ) {
               int mx, my;
               while ( mouseparams.taste == 2 ) {
                 if ( getfieldundermouse ( &mx, &my ) )  {
                    mousevisible(false);
                    cursor.hide();
                    cursor.posx = mx;
                    cursor.posy = my;
                    cursor.show();
                    mousevisible(true);
      
                    lastx = mx;
                    lasty = my;
      
                    execcode = -1;
                    execcode = rightmousebox();
                    if (execcode != -1) execaction(execcode);
      
                    while ( mouseparams.taste != 0 )
                       releasetimeslice();
                  }
                  releasetimeslice();
               }
            }
      
            checkformousescrolling();
            releasetimeslice();
      
         }  while (! (ch == ct_esc) || (ch == ct_altp+ct_x ) );
      } /* endtry */
      catch ( NoMapLoaded ) {
         buildemptymap();
         repaintdisplay();
      } /* endcatch */
   }  while (! (ch == ct_esc) || (ch == ct_altp+ct_x ) );
} 


//* õS Diverse

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
   } else {
      printf("exiting ... \n \n");
   } /* endif */
}


void closesvgamode( void )
{
   closegraphics();
}    


void showmemory ( void )
{
  #ifdef _DOS_
   npush ( activefontsettings );
   activefontsettings.length = 99;
   activefontsettings.background = 0;
   activefontsettings.color = 14;
   activefontsettings.justify = lefttext;
   activefontsettings.font = schriften.smallarial;
   int mss = getmousestatus();

   int a = maxavail();
   int b = _memavl();
   showtext2( strrr ( a ), 210,410 );
   showtext2( strrr ( b+a ), 310,410 );
   // showtext2( strrr ( _memmax() ), 10,410 );
   showtext2( strrr ( b  ), 110,480 );
   showtext2( strrr ( stackfree ()  ), 410,410 );
   if (mss == 2)
      setinvisiblemouserectangle ( -1, -1, -1, -1 );

   npop  ( activefontsettings );
  #endif
}

//* õS Main-Program


pfont load_font(char* name)
{
   tnfilestream stream ( name, 1 );
   return loadfont ( &stream );
}




int mapeditorMainThread ( void* _mapname )
{
   char* mapname = (char*) _mapname;
   #ifdef HEXAGON
    initspfst( -1, -1 );
   #else
    if ( resolx == 640  && resoly == 480 )
       initspfst();
    else
       initspfst( -1, -1 );
   #endif

   try {
      loaddata();
      if ( mapname )
         loadmap ( mapname );
      else
         buildemptymap();
   } /* end try */
   catch ( tfileerror err ) {
      displaymessage ( " error loading file %s ",2,err.filename );
   } /* end catch */

   pulldownfont = schriften.smallarial;

   activefontsettings.font = schriften.arial8;
   activefontsettings.color =lightblue ;
   activefontsettings.background =3;
   activefontsettings.length =100;
   activefontsettings.justify =lefttext;

   cursor.init();

   setstartvariables();

   addmouseproc ( &mousescrollproc );

   godview = true;

   bar( 0, 0, hgmp->resolutionx-1, hgmp->resolutiony-1, 0 );
   setvgapalette256(pal);

   displaymap();
   showallchoices();
   pdsetup();
   pdbaroff();

   mousevisible(true);
   cursor.show();

   editor();
   return 0;
}


int main(int argc, char *argv[] )
{ 
   signal ( SIGINT, SIG_IGN );
   fullscreen = 0;

   int resolx = 800;
   int resoly = 600;
   int              modenum8;
   int              i;

   printf( kgetstartupmessage() );

   #ifdef logging
   logtofile ( kgetstartupmessage() );
   logtofile ( "\n new log started \n ");
   #endif

   #ifdef _DOS_
    initmemory();
   #endif

   char *mapname = NULL, *configfile = NULL;
   int showmodes = 0;

   for (i = 1; i<argc; i++ ) {
      if ( argv[i][0] == '/'  ||  argv[i][0] == '-' ) {
#ifdef _DOS_
      if ( strcmpi ( &argv[i][1], "V1" ) == 0 ) {
         vesaerrorrecovery = 1; continue;
      }

      if ( strcmpi ( &argv[i][1], "SHOWMODES" ) == 0 ) {
         showmodes = 1; continue;
      }
#else
   /*
      // Added support for the -w and --window options
      // (equivalent to -window), since -w and --window are more
      // intuitive for *ux users (gnu option convention)
      if ( strcmpi ( &argv[i][1], "WINDOW" ) == 0 ||
          strcmpi ( &argv[i][1], "W" ) == 0 ||
          strcmpi ( &argv[i][1], "-WINDOW" ) == 0 ) {
        fullscreen = 0; continue;
      }
   */

#endif
      if ( strnicmp ( &argv[i][1], "x=", 2 ) == 0 ) {
           resolx = atoi ( &argv[i][3] ); continue;
      }

      if ( strnicmp ( &argv[i][1], "x:" ,2 ) == 0 ) {
           resolx = atoi ( &argv[i][3] ); continue;
      }

      if ( strnicmp ( &argv[i][1], "y=" ,2 ) == 0 ) {
           resoly = atoi ( &argv[i][3] ); continue;
      }

      if ( strnicmp ( &argv[i][1], "y:" ,2 ) == 0 ) {
           resoly = atoi ( &argv[i][3] ); continue;
      }

      if ( strcmpi ( &argv[i][1], "-loadmap" ) == 0 ||
            strcmpi( &argv[i][1], "lm" ) == 0 ) {
         mapname = argv[++i]; continue;
      }

      if ( strcmpi ( &argv[i][1], "-configfile" ) == 0 ||
           strcmpi ( &argv[i][1], "cf" ) == 0 ) {
         configfile = argv[++i]; continue;
      }

     if ( ( strcmpi ( &argv[i][1], "?" ) == 0 ) ||
          ( strcmpi ( &argv[i][1], "h" ) == 0 ) ||
          ( strcmpi ( &argv[i][1], "-help" ) == 0 ) ){
        printf( " Parameters: \n"
                "\t-h                 this page\n"
                "\t-lm file\n"
                "\t--loadmap file     start with a given map\n"
                "\t-cf file\n"
                "\t--configfile file  use given configuration file\n"
                "\t-x:X               Set horizontal resolution to X; default is 800 \n"
                "\t-y:Y               Set verticalal resolution to Y; default is 600 \n"
#ifdef _DOS_
                "\t-v1                Set vesa error recovery level to 1 \n"
                //"\t/nocd\t\tDisable music \n"
                //"\t-8bitonly          Disable truecolor graphic mode \n"
                "\t-showmodes         Display list of available graphic modes \n" );
#else
                // "\t-window\t\tDisable fullscreen mode \n"
                );
#endif
        exit (0);
     }

   }

   printf ( "\nInvalid command line parameter: %s \n", argv[i]);
   printf ( "Use /h to for help\n"  );
   exit(1);

  } /* endfor */


   if ( resolx < 640 || resoly < 480 ) {
      printf ( "Cannot run in resolution smaller than 640*480 !\n");
      exit(1);
   }

#ifdef _DOS_
   if ( showmodes ) {
      showavailablemodes();
      return 0;
   }
#endif

   memset(exitmessage, 0, sizeof ( exitmessage ));
   atexit ( dispmessageonexit );

   initFileIO( configfile );

   #ifdef HEXAGON
    check_bi3_dir ();
   #endif


   modenum8 = initgraphics( resolx, resoly, 8 );
   if ( modenum8 < 0 )
      return 1;
   atexit ( closesvgamode ); 

   setWindowCaption ( "Advanced Strategic Command : map editor ");

   schriften.smallarial = load_font("smalaril.FNT");
   schriften.large = load_font("USABLACK.FNT");
   schriften.arial8 = load_font("ARIAL8.FNT");
   schriften.smallsystem = load_font("msystem.fnt");
   schriften.monogui = load_font("monogui.fnt");
   

   virtualscreenbuf.init();

   {
       tnfilestream stream ( "logo640.pcx", 1 );
       loadpcxxy( &stream, (hgmp->resolutionx - 640)/2, (hgmp->resolutiony-35)/2, 1 ); 
       int whitecol = 251;
       activefontsettings.font = schriften.smallarial;
       activefontsettings.background = 255;
       activefontsettings.justify = centertext;
       activefontsettings.color = whitecol;
       activefontsettings.length = hgmp->resolutionx-20;
       showtext2 ("Map Editor", 10, hgmp->resolutiony - activefontsettings.font->height - 2 );

   }
   {
      int w;
      tnfilestream stream ("mausi.raw",1);
      stream.readrlepict(   &icons.mousepointer, false, &w );
   }


   initializeEventHandling ( mapeditorMainThread, mapname, icons.mousepointer );

   cursor.hide();
   writegameoptions ();

  #ifdef MEMCHK
   verifyallblocks();
  #endif
   return 0;
}

