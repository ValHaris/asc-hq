/*! \file gamedlg.cpp    \brief Tons of dialog boxes which are used in ASC only (and not in the mapeditor)
*/
//     $Id: gamedlg.cpp,v 1.75 2001-07-25 18:00:16 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.74  2001/07/18 18:15:52  mbickel
//      Fixed: invalid sender of mails
//      Fixed: unmoveable units are moved by AI
//      Some reformatting of source files
//
//     Revision 1.73  2001/07/15 21:00:25  mbickel
//      Some cleanup in the vehicletype class
//
//     Revision 1.72  2001/07/14 19:13:15  mbickel
//      Rewrote sound system
//      Moveing units make sounds
//      Added sound files to data
//
//     Revision 1.71  2001/07/13 12:53:01  mbickel
//      Fixed duplicate icons in replay
//      Fixed crash in tooltip help
//
//     Revision 1.70  2001/07/11 20:44:37  mbickel
//      Removed some vehicles from the data file.
//      Put all legacy units in into the data/legacy directory
//
//     Revision 1.69  2001/07/08 20:09:57  mbickel
//      Added tank2pcx tool to project
//      Fixed bug in refuel dialog
//
//     Revision 1.68  2001/07/04 16:53:28  mbickel
//      Fixed bug in refuel dialog
//      Fixed crash in mount (win32)
//      Improved build process for Borland command line compiler
//      Updated source documentation
//
//     Revision 1.67  2001/02/26 13:49:35  mbickel
//      Fixed bug in message loading
//      readString can now read strings that container \n
//
//     Revision 1.66  2001/02/26 12:35:12  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
////     Revision 1.65  2001/02/18 15:37:10  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.64  2001/02/11 11:39:33  mbickel
//      Some cleanup and documentation
//
//     Revision 1.63  2001/02/01 22:48:40  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.62  2001/01/31 14:52:38  mbickel
//      Fixed crashes in BI3 map importing routines
//      Rewrote memory consistency checking
//      Fileselect dialog now uses ASCStrings
//
//     Revision 1.61  2001/01/28 17:19:09  mbickel
//      The recent cleanup broke some source files; this is fixed now
//
//     Revision 1.60  2001/01/23 21:05:15  mbickel
//      Speed up of AI
//      Lot of bugfixes in AI
//      Moved Research to own files (research.*)
//      Rewrote storing of developed technologies
//      Some cleanup and documentation
//
//     Revision 1.59  2001/01/21 16:37:16  mbickel
//      Moved replay code to own file ( replay.cpp )
//      Fixed compile problems done by cleanup
//
//     Revision 1.58  2001/01/19 13:33:50  mbickel
//      The AI now uses hemming
//      Several bugfixes in Vehicle Actions
//      Moved all view calculation to viewcalculation.cpp
//      Mapeditor: improved keyboard support for item selection
//
//     Revision 1.57  2001/01/04 15:13:49  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.56  2000/12/27 22:23:08  mbickel
//      Fixed crash in loading message text
//      Removed many unused variables
//
//     Revision 1.55  2000/11/29 09:40:21  mbickel
//      The mapeditor has now two maps simultaneously active
//      Moved memorychecking functions to its own file: memorycheck.cpp
//      Rewrote password handling in ASC
//
//     Revision 1.54  2000/11/21 20:27:03  mbickel
//      Fixed crash in tsearchfields (used by object construction for example)
//      AI improvements
//      configure.in: added some debug output
//                    fixed broken check for libbz2
//
//     Revision 1.53  2000/11/12 15:19:06  mbickel
//      Applied patch to configure.in to enable finding SDL if it isn't installed
//       in the default directory
//      GiveUnitAway dialog improved
//
//     Revision 1.52  2000/11/11 11:05:17  mbickel
//      started AI service functions
//
//     Revision 1.51  2000/11/08 19:31:05  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
//     Revision 1.50  2000/10/31 10:42:42  mbickel
//      Added building->vehicle service to vehicle controls
//      Moved tmap methods to gamemap.cpp
//
//     Revision 1.49  2000/10/18 14:14:09  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//
//     Revision 1.48  2000/10/14 14:16:05  mbickel
//      Cleaned up includes
//      Added mapeditor to win32 watcom project
//
//     Revision 1.47  2000/10/14 10:52:49  mbickel
//      Some adjustments for a Win32 port
//
//     Revision 1.46  2000/10/11 14:26:35  mbickel
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
//     Revision 1.45  2000/09/16 11:47:27  mbickel
//      Some cleanup and documentation again
//
//     Revision 1.44  2000/08/26 15:33:42  mbickel
//      Warning message displayed if empty password is entered
//      pcxtank now displays error messages
//
//     Revision 1.43  2000/08/25 13:42:54  mbickel
//      Fixed: zoom dialogbox in mapeditor was invisible
//      Fixed: ammoproduction: no numbers displayed
//      game options: produceammo and fillammo are now modified together
//      Fixed: sub could not be seen when standing on a mine
//      Some AI improvements
//
//     Revision 1.42  2000/08/21 17:50:57  mbickel
//      Fixed: crash when unit reaching max experience
//      Fixed: crash when displaying research image
//      Fixed: crash when events referenced a unit that has been shot down
//      Fixed: screenshot being written to wrong directory
//
//     Revision 1.41  2000/08/13 09:54:01  mbickel
//      Refuelling is now logged for replays
//
//     Revision 1.40  2000/08/12 15:03:22  mbickel
//      Fixed bug in unit movement
//      ASC compiles and runs under Linux again...
//
//     Revision 1.39  2000/08/12 12:52:47  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.38  2000/08/12 09:17:26  gulliver
//     *** empty log message ***
//
//     Revision 1.37  2000/08/08 09:48:13  mbickel
//
//      speed up of dialog boxes in linux
//      fixed graphical errors in attack
//      fixed graphical error in ammo transfer
//      fixed reaction fire not allowing manual attack
//
//     Revision 1.36  2000/08/07 16:29:20  mbickel
//      orbiting units don't consume fuel any more
//      Fixed bug in attack formula; improved attack formula
//      Rewrote reactionfire
//
//     Revision 1.35  2000/08/06 12:18:09  mbickel
//      Gameoptions: new default values
//      Maketank: negative buildable-object-IDs
//
//     Revision 1.34  2000/08/06 11:39:09  mbickel
//      New map paramter: fuel globally available
//      Mapeditor can now filter buildings too
//      Fixed unfreed memory in fullscreen image loading
//      Fixed: wasted cpu cycles in building
//      map parameters can be specified when starting a map
//      map parameters are reported to all players in multiplayer games
//
//     Revision 1.33  2000/08/05 20:17:57  mbickel
//      Restructured Fullscreen Image loading
//

//     Revision 1.32  2000/07/29 14:54:30  mbickel
//      plain text configuration file implemented
//
//     Revision 1.31  2000/07/26 15:58:09  mbickel
//      Fixed: infinite loop when landing with an aircraft which is low on fuel
//      Fixed a bug in loadgame
//
//     Revision 1.30  2000/07/12 08:27:06  mbickel
//      Prepared 1.1.9 release
//
//     Revision 1.29  2000/06/09 13:12:25  mbickel
//      Fixed tribute function and renamed it to "transfer resources"
//
//     Revision 1.28  2000/06/06 20:17:07  mbickel
//      Updated dummy sound headers for DOS
//
//     Revision 1.27  2000/06/06 20:03:17  mbickel
//      Fixed graphical error when transfering ammo in buildings
//      Sound can now be disable by a command line parameter and the game options
//
//     Revision 1.26  2000/05/23 20:40:46  mbickel
//      Removed boolean type
//
//     Revision 1.25  2000/05/22 15:40:34  mbickel
//      Included patches for Win32 version
//
//     Revision 1.24  2000/05/18 14:39:56  mbickel
//      Changed version number to 1.1.4
//
//     Revision 1.23  2000/05/10 19:55:51  mbickel
//      Fixed empty loops when waiting for mouse events
//
//     Revision 1.22  2000/05/07 18:21:21  mbickel
//      Speed of attack animation can now be specified
//
//     Revision 1.21  2000/05/07 12:12:16  mbickel
//      New mouse option dialog
//      weapon info can now be displayed by clicking on a unit
//
//     Revision 1.20  2000/04/27 16:25:23  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.19  2000/04/04 08:31:40  mbickel
//      Fixed a bug that exited ASC when trying to give units to your allies.
//
//     Revision 1.18  2000/03/29 09:58:46  mbickel
//      Improved memory handling for DOS version
//      Many small changes I can't remember ;-)
//
//     Revision 1.17  2000/03/11 18:22:05  mbickel
//      Added support for multiple graphic sets
//
//     Revision 1.16  2000/02/03 20:54:40  mbickel
//      Some cleanup
//      getfiletime now works under Linux too
//
//     Revision 1.15  2000/01/31 16:34:43  mbickel
//      now standard hotkeys in dialog boxes
//
//     Revision 1.14  2000/01/06 11:19:13  mbickel
//      Worked on the Linux-port again...
//
//     Revision 1.13  2000/01/04 19:43:51  mbickel
//      Continued Linux port
//
//     Revision 1.12  2000/01/02 19:47:06  mbickel
//      Continued Linux port
//      Fixed crash at program exit
//
//     Revision 1.11  2000/01/01 19:04:17  mbickel
//     /tmp/cvsVhJ4Z3
//
//     Revision 1.10  1999/12/30 20:30:33  mbickel
//      Improved Linux port again.
//
//     Revision 1.9  1999/12/29 17:38:11  mbickel
//      Continued Linux port
//
//     Revision 1.8  1999/12/28 21:02:53  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.7  1999/12/27 13:00:01  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.6  1999/12/14 20:23:52  mbickel
//      getfiletime now works on containerfiles too
//      improved BI3 map import tables
//      various bugfixes
//
//     Revision 1.5  1999/11/22 18:27:22  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.4  1999/11/18 17:31:09  mbickel
//      Improved BI-map import translation tables
//      Moved macros to substitute Watcom specific routines into global.h
//
//     Revision 1.3  1999/11/16 17:04:02  mbickel
//     Made ASC compilable for DOS again :-)
//     Merged all the bug fixes in that I did last week
//
//     Revision 1.2  1999/11/16 03:41:44  tmwilson
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

#include <stdio.h>                        
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif


#include "vehicletype.h"
#include "buildingtype.h"

#include "basegfx.h"
#include "gamedlg.h"
#include "missions.h"
#include "events.h"
#include "stack.h"
#include "network.h"
#include "controls.h"
#include "sg.h"
#include "gameoptions.h"
#include "loadimage.h"
#include "errors.h"
#include "password_dialog.h"
#include "viewcalculation.h"
#include "replay.h"

#ifdef _SDL_
 #include "sdl/sound.h"
#endif

class   tchoosetechnology : public tdialogbox {
                           dynamic_array<ptechnology> techs;
                           char          markedbackgrnd;
                           int           technum;
                           int           dispnum;
                           int           firstshowntech;
                           int           markedtech;
                           void          disp ( void );
                           void          buttonpressed ( int id  );
                           int           gx ( void );
                           int           gy ( int i );
                           void          check( void );
                      public:
                           void          init ( void );
                           virtual void  run ( void );
                        };

void         tchoosetechnology::init(void)
{ 
   tdialogbox::init();
   title = "research";

   firstshowntech = 0;
   dispnum = 10;
   markedtech = 0;

   check();
   if ( technum > dispnum ) 
      addscrollbar  ( xsize - 40, 50, xsize - 20, ysize - 20, &technum, dispnum, &firstshowntech, 1, 0 );

   buildgraphics(); 
   markedbackgrnd = lightblue;
   disp();

/*   activefontsettings.font = schriften.smallarial; 
   activefontsettings.justify = lefttext; 
   showtext2("choose technology to research :", x1 + 10, y1 + starty + 10); */
} 

void         tchoosetechnology::check(void)
{
  int i;

   technum = 0;
   presearch resrch = &actmap->player[actmap->actplayer].research;

   for (i = 0; i < technologynum; i++) {
       ptechnology tech = gettechnology_forpos( i );
       if ( tech ) { 
          if ( resrch->technologyresearched ( tech->id )) {
             bool fail = false;
             for (int j = 0; j < 6; j++) {
                 if ( tech->requiretechnology[j] > 0 )
                    if ( !resrch->technologyresearched ( tech->id ))
                       fail = true;

                 if ( tech->requiretechnology[j] < 0 )
                    if ( resrch->technologyresearched ( -tech->id ))
                       fail = true;
             }
             if ( tech->requireevent)
                if ( ! actmap->eventpassed(tech->id, cenewtechnologyresearchable) )
                   fail = true;
             if (!fail) {
                techs[technum] = tech;
                technum++;
             }
          } 

       } 
     
   }
   for ( i = 0; i < technum-1; ) {
      if ( techs[i]->lvl > techs[i+1]->lvl ) {
         ptechnology temp = techs[i];    
         techs[i]->lvl = techs[i+1]->lvl;
         techs[i+1] = temp;
         if ( i )
            i--;
      } else
         i++;
   }
}


int          tchoosetechnology::gx ( void )
{
   return x1 + 20;
}

int          tchoosetechnology::gy ( int i )
{
   return y1 + starty + 10 + ( i - firstshowntech ) * 30;
}

void         tchoosetechnology::disp(void)
{
   activefontsettings.font = schriften.smallarial; 
   activefontsettings.justify = lefttext; 
   activefontsettings.length = 300;
   for ( int i = firstshowntech; i < technum && i < firstshowntech + dispnum; i++ ) {
      if ( i == markedtech )
          activefontsettings.background = markedbackgrnd; 
      else 
          activefontsettings.background = dblue; 

      int x = gx();
      int y = gy(i);
      setinvisiblemouserectanglestk ( x,y,x+300,y+20 );

      showtext2(techs[i]->name, x,y ) ;
      getinvisiblemouserectanglestk ();
   }
}

void         tchoosetechnology::buttonpressed ( int id )
{
   if ( id == 1 )
      disp();
}


void         tchoosetechnology::run(void)
{ 
  mousevisible(true);
  if (technum == 0) { 
     showtext2("no further technologies to develop !", x1 + 20, y1 + starty + 30);
     actmap->player[actmap->actplayer].research.activetechnology = NULL;
     wait();
  } 
  else { 
     int fertig = 0;
     do { 
        tdialogbox::run();
        int oldmark = markedtech;
        if ( mouseparams.taste == 1 ) {
            for ( int i = firstshowntech; i < technum && i < firstshowntech + dispnum; i++ ) {
               int x = gx();
               int y = gy(i);
               if ( mouseinrect ( x, y, x+300, y + 20  ) ){
                  if ( markedtech == i )
                     fertig = 1;
                  else
                     markedtech = i;

                  if (markedtech != oldmark) {
                     disp();
                     oldmark = markedtech;
                  }
                  
                  while ( mouseparams.taste )
                     releasetimeslice();
               }
            }
        }

        switch (taste) {
           
           case ct_down:  if ( markedtech+1 < technum )
                               markedtech++;   
           break; 
           
           case ct_up: if ( markedtech )
                          markedtech--;
           break; 
           
           case ct_enter:   
           case ct_space:   { 
                        // resrch->progress = 0; 
                        fertig = true; 
                     } 
           break;
        } 
        if (markedtech != oldmark) {
           if ( markedtech < firstshowntech ) {
              firstshowntech = markedtech;
              enablebutton ( 1 );
           }
           if ( markedtech >= firstshowntech + dispnum ) {
              firstshowntech = markedtech - dispnum + 1;
              enablebutton ( 1 );
           }
           disp();
        }

     }  while ( !fertig ); 

     /*
     if ( actmap->objectcrc )
       if ( !actmap->objectcrc->speedcrccheck->checktech2 ( techs[markedtech] )) 
          displaymessage ("CRC-check inconsistency in choosetechnology :: run ", 2 );
     */

     actmap->player[actmap->actplayer].research.activetechnology = techs[markedtech];

  } 
} 



void         choosetechnology(void)
{ 
         tchoosetechnology ct; 

   ct.init(); 
   ct.run(); 
   ct.done(); 
} 









class tchoosetransfermethod : public tdialogbox {
            protected:

               int linenum;
               int mnum;
               int mfirst;
               int mmark;
               void viewdata ( void );
               int status;
               int linedist;
               int linex1;
               int linelength;
               int liney1;
            public:
               void init  ( int xx1 );
               void redraw ( void );
               void run   ( pbasenetworkconnection  *conn );
               void buttonpressed ( int id );
               int  getcapabilities ( void ) { return 1; };
           };



void  tchoosetransfermethod :: init ( int xx1 )
{
   

   tdialogbox::init();
   status = 0;
   linenum = 7;
   xsize = 200;
   ysize = 300;

   x1 = xx1 - xsize/2;
   y1 = -1;



   title = "choose network transfer method";
   windowstyle &= ~dlg_in3d;

   addbutton ( "~O~k", 10, ysize - 30, xsize/2 - 5, ysize - 10 , 0, 1, 1, true );
   addkey ( 1, ct_enter );
   addbutton ( "~C~ancel", xsize/2 + 5, ysize - 30, xsize-10 - 5, ysize - 10 , 0, 1, 2, true );
   addkey ( 2, ct_esc );

   mfirst = 0;
   mmark = 0;
   mnum = 0;

   pbasenetworkconnection nc = firstnetworkconnection;
   while ( nc ) {
      mnum++;
      nc=nc->next;
   }

   if ( mnum > linenum )
      addscrollbar ( xsize - 30, starty + 10, xsize - 10, ysize - 50, &mnum, linenum, &mfirst, 3, 2 );


   linedist = 25;
   linex1 = x1 + 20;
   linelength = xsize - 50;
   liney1 = y1 + starty + 10;
   buildgraphics();


}

void tchoosetransfermethod :: buttonpressed ( int id )
{
   tdialogbox::buttonpressed ( id );
   if ( id == 3 )
      viewdata();
   if ( id == 1 )
      status = 2;
   if ( id == 2 )
      status = 1;

}

void tchoosetransfermethod :: viewdata ( void )
{
  int i;

   linedist = 25;
   linex1 = x1 + 20;
   linelength = xsize - 50;
   liney1 = y1 + starty + 10;

   setinvisiblemouserectanglestk ( linex1, liney1, linex1 + linelength, liney1 + linedist * linenum );
   activefontsettings.font = schriften.smallarial;
   activefontsettings.length = linelength;
   activefontsettings.justify = lefttext;
   activefontsettings.background = dblue;
   pbasenetworkconnection nc = firstnetworkconnection;
   for (i=0; i < mfirst; i++ )
      nc = nc->next;

   i = 0;
   while ( nc ) {
      if ( i + mfirst == mmark )
        activefontsettings.color = textcolor;
      else
        activefontsettings.color = black;

      if ( i+mfirst ) {
         showtext2( nc-> getname(), linex1 , liney1 + i*linedist );
         nc=nc->next;
      } else
         showtext2( "none", linex1 , liney1 + i*linedist );

      i++;
   }
   getinvisiblemouserectanglestk();
}


void tchoosetransfermethod :: redraw ( void )
{
   tdialogbox::redraw();
   viewdata();
}

void tchoosetransfermethod :: run ( pbasenetworkconnection*  conn )
{
   int moldf = -1;
   int moldm = -1;

   if ( *conn ) {
      pbasenetworkconnection  nc = firstnetworkconnection;
      mmark = 1;
      while ( nc != *conn ) {
         nc = nc->next;
         mmark++;
      }
   } else
     mmark = 0;

   

   mousevisible ( true );
   do {
      tdialogbox::run();
      if ( taste == ct_down )
         if ( mmark < mnum )
            mmark++;

      if ( taste == ct_up )
         if ( mmark > 0 )
            mmark--;

      if ( mouseparams.taste == 1 )
         if ( mouseparams.x >= linex1  &&  mouseparams.x <= linex1+linelength )
            for (int i = 0; i < linenum; i++) {
                if ( mouseparams.y >= liney1 + i * linedist  &&  mouseparams.y < liney1 + ( i+1 ) * linedist )
                   mmark = mfirst + i;
            } /* endfor */

      if ( mmark < mfirst )
         mfirst = mmark;
      if ( mmark > mfirst + linenum )
         mfirst = mmark - linenum;
      if ( mmark > mnum )
         mmark = mnum;
      if ( mmark != moldm  || mfirst != moldf ) {
         viewdata();
         moldf = mfirst;
         moldm = mmark;
      }

   } while ( status == 0 ); /* enddo */
   if ( status == 2) {
      if ( mmark ) {
         *conn = firstnetworkconnection;
         for ( int i=1; i < mmark; i++ )
            *conn = (*conn) -> next;
      } else
         *conn = NULL;
   }

}




class  tsetupnetwork : public tdialogbox {
            protected:
               tnetwork network;
               tnetwork* pnet;

               int edit;        /* edit ist bitmappt:
                                      bit 0 :  receive einstellbar
                                          1 :  send einstellbar
                                          2 :  computer wechselbar
                                */

               int frstcompnum;
               int player;
               void paintcomputernames ( void );
               void paintransfermnames ( void );
               pbasenetworkconnection  getmethod ( tnetworkchannel chan );
            public: 
               int status;
               int  getcapabilities ( void ) { return 1; };
               void init ( tnetwork* nw, int edt, int playr );
               void run  ( void );
               void buttonpressed ( int id );
               void redraw ( void );
           };






void  tsetupnetwork :: init ( tnetwork* nw, int edt, int playr )
{
   player = playr;
   edit = edt;
   network = *nw;
   pnet = nw;
   tdialogbox::init();

   x1 = -1;
   y1 = -1;
   xsize = 600;
   ysize = 350;
   status = 0;
   title = "setup network";
   addbutton ( "~O~k", 10, ysize - 40, xsize / 2 - 5, ysize - 10, 0, 1, 1, true );
   addkey ( 1, ct_enter );
   addbutton ( "e~x~it", xsize / 2 + 5,  ysize - 40, xsize - 10, ysize - 10, 0, 1, 2, player == -1 );
   addkey ( 2, ct_esc );

   if ( edit  &  4) {
      addbutton ( "~<~", 20, starty + 80, 40, starty + 100, 0, 1, 3, true );
      addkey ( 3, ct_left );
      addbutton ( "~>~", xsize - 40, starty + 80, xsize - 20, starty + 100, 0, 1, 4, true );
      addkey ( 4, ct_right );
   }

   addbutton ( "~s~etup parameters", x1 + 20, y1 + ysize - 85, x1 + xsize/2 - 15, y1 + ysize - 60, 0, 1, 6 , (edit & 1) > 0 );
   addbutton ( "setup parameters", x1 + xsize/2 + 15, y1 + ysize - 85, x1 + xsize - 20, y1 + ysize - 60, 0, 1, 7 , (edit & 2) > 0 );

   addbutton ( "choose ~m~ethod", x1 + 20, y1 + ysize - 120, x1 + xsize/2 - 15, y1 + ysize - 95, 0, 1, 8 , (edit & 1) > 0 );
   addbutton ( "choose method", x1 + xsize/2 + 15, y1 + ysize - 120, x1 + xsize - 20, y1 + ysize - 95, 0, 1, 9 , (edit & 2) > 0 );

   windowstyle &= ~dlg_in3d;

   if ( (edit & 4) || !(edit & 8) ) {
      if ( player == -1 ) {
         int fp = actmap->actplayer;
         if ( fp == -1 ) {
            fp = 0;
            while ( !actmap->player[ fp ].exist() )
               fp++;
         }
         frstcompnum = actmap->network->player[ fp ].compposition;
      } else
         frstcompnum = actmap->network->player[ player ].compposition;
   } else
      frstcompnum = 0;


   buildgraphics();
}


void tsetupnetwork :: redraw ( void )
{
   tdialogbox::redraw();
   //rahmen ( true, x1 + 10, y1 + starty + 10, x1 + xsize - 10, y1 + starty + 110 );
   rahmen ( true, x1 + 5, y1 + starty + 5, x1 + xsize - 5, y1 + ysize - 45 );

   if ( edit & 4 ) {
      /*
      trleheader *pw = (trleheader *) icons.computer;
      putspriteimage ( x1         + 20,             y1 + starty + 20, icons.computer );
      putspriteimage ( x1 + (xsize - pw->x) /2    , y1 + starty + 20, icons.computer );
      putspriteimage ( x1 + xsize - 20 - pw->x    , y1 + starty + 20, icons.computer );
      */
      putspriteimage ( x1         + 20,             y1 + starty + 20, icons.computer );
   }

//   rahmen ( true, x1 + 10, y1 + starty + 150, x1 + xsize - 10, y1 + ysize - 50 );
   rahmen3 ( "receive:", x1 + 10, y1 + starty + 150, x1 + xsize/2 - 5, y1 + ysize - 50, 1 );
   rahmen3 ( "send:",    x1 + xsize/2 + 5, y1 + starty + 150, x1 + xsize - 10, y1 + ysize - 50, 1 );

   rahmen ( true, x1 + 20,           y1 + starty + 160, x1 + xsize/2 - 15, y1 + starty + 180 );
   rahmen ( true, x1 + xsize/2 + 15, y1 + starty + 160, x1 + xsize - 20,   y1 + starty + 180 );

   /*
   if ( edit ) {
      while ( !network.computer[frstcompnum].existent )
         frstcompnum++;
   } 
   */

   paintcomputernames();
   paintransfermnames(); 
}


void tsetupnetwork :: paintransfermnames ( void )
{
   activefontsettings.length = xsize/2 - 20 - 15 - 2;
   activefontsettings.font = schriften.smallarial;
   activefontsettings.justify = lefttext;
   activefontsettings.background = dblue;
   activefontsettings.color = textcolor;
   if ( network.computer[frstcompnum].receive.transfermethod )
     showtext2 ( network.computer[frstcompnum].receive.transfermethod->getname (), x1 + 21, y1 + starty + 161 );
   else
     showtext2 ( "NONE", x1 + 21, y1 + starty + 161 );

   if ( network.computer[frstcompnum].send.transfermethod )
     showtext2 ( network.computer[frstcompnum].send.transfermethod->getname (), x1 + xsize/2 + 16, y1 + starty + 161 );
   else
     showtext2 ( "NONE", x1 + xsize/2 + 16, y1 + starty + 161 );

}

pbasenetworkconnection tsetupnetwork ::  getmethod ( tnetworkchannel chan )
{
   tchoosetransfermethod ctm;
   pbasenetworkconnection *conn ;
   int xx1;
   if ( chan == TN_RECEIVE ) {
      xx1 = 220;
      if ( network.computer[frstcompnum].receive.transfermethodid == 0 )
         network.computer[frstcompnum].receive.transfermethod = NULL;
      conn = &network.computer[frstcompnum].receive.transfermethod;
   } else {
      xx1 = 420;
      if ( network.computer[frstcompnum].send.transfermethodid == 0 )
         network.computer[frstcompnum].send.transfermethod = NULL;
      conn = &network.computer[frstcompnum].send.transfermethod;
   }

   ctm.init ( xx1 );
   ctm.run ( conn );
   ctm.done ();

   if ( chan == TN_RECEIVE ) {
      if ( network.computer[frstcompnum].receive.transfermethod )
         network.computer[frstcompnum].receive.transfermethodid = network.computer[frstcompnum].receive.transfermethod->getid();
      else
         network.computer[frstcompnum].receive.transfermethodid = 0;
   } else {
      if ( network.computer[frstcompnum].send.transfermethod )
         network.computer[frstcompnum].send.transfermethodid = network.computer[frstcompnum].send.transfermethod->getid();
      else
         network.computer[frstcompnum].send.transfermethodid = 0;
   }

   return *conn;
}

void tsetupnetwork :: buttonpressed ( int id )
{
   tdialogbox::buttonpressed ( id );
   switch ( id ) {
     case 1: if ( edit == 1 ) {
                if ( network.computer[frstcompnum].receive.transfermethod )
                   if ( network.computer[frstcompnum].receive.transfermethod->validateparams ( &network.computer[frstcompnum].receive.data, TN_RECEIVE ) )
                      status = 2;
             } else
               if ( edit == 2 ) {
                  if ( network.computer[frstcompnum].send.transfermethod )
                     if ( network.computer[frstcompnum].send.transfermethod->validateparams ( &network.computer[frstcompnum].send.data, TN_SEND ) )
                        status = 2;
               } else
                  status = 2;

             break;
     case 2: /*
             {
               int q = 0; 
               if ( edit ) {
                  if (choice_dlg("this will quit the curront map. continue ?","~y~es","~n~o") == 1)  
                     q = 1;
               } else 
                  q = 1;
               if ( q ) {
                  if ( actmap->xsize && actmap->ysize )
                      erasemap();
                  throw NoMapLoaded;
                }
             }
             */
             status = 1;
             break;
     case 4:  do {
                 frstcompnum++;
                 if ( frstcompnum >= network.computernum )
                    frstcompnum = 0;
              } while ( network.computer[frstcompnum].existent == 0 );
              paintcomputernames ();
              paintransfermnames ();
              break;

     case 3:  do {
                 frstcompnum--;
                 if ( frstcompnum < 0 )
                    frstcompnum = network.computernum-1;
              } while ( network.computer[frstcompnum].existent == 0 );
              paintcomputernames ();
              paintransfermnames ();
              break;

     case 6: if ( network.computer[frstcompnum].receive.transfermethod )
                  network.computer[frstcompnum].receive.transfermethod->setupforreceiving ( &network.computer[frstcompnum].receive.data );
             break;

     case 7: if ( network.computer[frstcompnum].send.transfermethod )
                  network.computer[frstcompnum].send.transfermethod->setupforsending ( &network.computer[frstcompnum].send.data );
             break;

     case 8: getmethod ( TN_RECEIVE );
             paintransfermnames();
             break;

     case 9: getmethod ( TN_SEND );
             paintransfermnames();
             break;

   } /* endswitch */
}

void tsetupnetwork :: run ( void )
{
   mousevisible ( true );
   do {
      tdialogbox::run();

   } while ( status == 0 ); /* enddo */

   if ( status == 2 )
      *pnet = network;

}




void tsetupnetwork :: paintcomputernames ( void )
{
   if ( edit & 4 ) {
      npush ( activefontsettings );
      activefontsettings.background = dblue;
      activefontsettings.font = schriften.smallarial;
      activefontsettings.length = 100;
      activefontsettings.justify = centertext;
      showtext2 ( network.computer[ frstcompnum ].name , x1 + ( xsize - activefontsettings.length) / 2 , y1 + starty + 85 );

      activefontsettings.justify = lefttext;
      int i = frstcompnum ;
      do {
         i--;
         if ( i < 0 )
            i = network.computernum-1;
      } while ( network.computer[i].existent == 0 );
      showtext2 ( network.computer[ i ].name , x1 + 50, y1 + starty + 85 );

      activefontsettings.justify = righttext;
      i = frstcompnum ;
      do {
         i++;
         if ( i >= network.computernum )
            i = 0;
      } while ( network.computer[i].existent == 0 );
      showtext2 ( network.computer[ i ].name, x1 + xsize - 50 - activefontsettings.length, y1 + starty + 85 );

      npop ( activefontsettings );
   }
}


/* edt ist bitmappt:
      bit 0 :  receive einstellbar
          1 :  send einstellbar
          2 :  computer wechselbar
*/

int  setupnetwork ( tnetwork* nw, int edt, int player )
{
   if ( nw || actmap->network ) {
      tsetupnetwork sn;
      if ( nw )
         sn.init( nw, edt, player );
      else
         sn.init ( actmap->network, edt, player );

      sn.run();
      int i = sn.status;
      sn.done();
      return i;
   }
   return 1;

}

/*********************************************************************************************************/
/*   Neuen Level starten                                                                               ÿ */
/*********************************************************************************************************/

void         tchoosenewcampaign::evaluatemapinfo( const char* srname, tmap* spfld )
{
   if ( spfld->campaign &&  ( stricmp( spfld->codeword, password ) == 0 )) {
      strcat(message1, srname );
      strcat(message1, " ");
      if (status == 0) {
         strcpy ( mapinfo , dateiinfo );
         if ( spfld->title )
            strcpy ( maptitle, spfld->title);
         else
            maptitle[0] = 0;
         strcpy ( mapname, srname ) ;
         status = 7;
      }
      else {
         status = 5;
      }
   }
}


void         tchoosenewsinglelevel::evaluatemapinfo( const char* srname, tmap* spfld )
{

   if ( stricmp ( spfld->codeword, password ) == 0) {
      strcat(message2, srname);
      strcat(message2, " ");
      if (status == 0) {
         strcpy( mapinfo , dateiinfo );
         if ( spfld->title )
            strcpy ( maptitle, spfld->title);
         else
            maptitle[0] = 0;
         strcpy(mapname,srname);
         status = 7;
      }
      else {
         status = 5;
      }
   }

}

void         tnewcampaignlevel::init(void)
{
   tdialogbox::init();
   mapname[0]  = 0;
   maptitle[0] = 0;
   mapinfo[0]  = 0;
   message1[0] = 0;
   message2[0] = 0;
   dateiinfo[0]= 0;
   status = 0;
}

void         tnewcampaignlevel::searchmapinfo(void)
{

   status = 0;
   message1[0] = 0;
   message2[0] = 0;
   mapinfo[0] = 0;
   maptitle[0] = 0;


   tfindfile ff ( mapextension );
   string filename = ff.getnextname();
   while( !filename.empty() ) {

       try {
          tnfilestream filestream ( filename.c_str(), tnstream::reading );
          tmaploaders spfldloader;
          spfldloader.stream = &filestream;
          // spfldloader.setcachingarrays ( );

          CharBuf description;

          spfldloader.stream->readpchar ( &description.buf );
          // int desclen = strlen ( description.buf ) + 7;
          strncpy ( dateiinfo, description.buf, sizeof ( dateiinfo ) -2  );

          word w;
          spfldloader.stream->readdata2 ( w );

          if ( w != fileterminator )
             throw tinvalidversion ( filename.c_str(), w, fileterminator );

          int version;
          spfldloader.stream->readdata2( version );

         if (version > actmapversion || version < minmapversion )
            throw tinvalidversion ( filename.c_str(), version, actmapversion );

          spfldloader.readmap ();
          spfldloader.readeventstocome();

          evaluatemapinfo( filename.c_str(), spfldloader.spfld );
      }
      catch ( ASCexception ) {
      } /* endcatch */

      filename = ff.getnextname();
   }

   if ((status == 5) || ( status == 6 )) {
      strcpy(message1,  "multiple maps with the same id/password found" );
      // message2[0] = 0;
      mapinfo[0] = 0;
      maptitle[0] = 0;
      mapname[0] = 0;
   }

   if (status == 0) {
      strcpy ( message1 , "no maps found" );
   }
}


void         tnewcampaignlevel::showmapinfo(word         ypos)
{

   activefontsettings.font = schriften.smallarial;
   npush( activefontsettings );
   if (message1[0] == 0) {
       activefontsettings.length = 120;
       activefontsettings.justify = righttext;

      bar(x1 + 26,y1 + ypos,x1 + xsize - 26,y1 + ypos + 120,dblue);
      showtext2("filename:",x1 + 25,y1 + ypos);
      showtext2("mapinfo:",x1 + 25,y1 + ypos + 40);
      showtext2("maptitle:",x1 + 25,y1 + ypos + 80);
      activefontsettings.length = 200;
      activefontsettings.background = dblue;
      activefontsettings.color = black;
      activefontsettings.justify = lefttext;

      showtext2(strupr( mapname ),x1 + 135,y1 + ypos + 20);
      showtext2(mapinfo,x1 + 135,y1 + ypos + 60);
      showtext2(maptitle,x1 + 135,y1 + ypos + 100);
   }
   else {
      paintsurface ( 26,ypos, xsize - 26, ypos + 120);
      activefontsettings.length = 300;
      activefontsettings.background = dblue;
      activefontsettings.color = lightred;
      activefontsettings.justify = lefttext;

      showtext2(message1,x1 + 30,y1 + ypos + 20);
      showtext2(message2,x1 + 30,y1 + ypos + 45);
   }

   npop( activefontsettings );
}


void         tnewcampaignlevel::loadcampaignmap(void)
{

   displaymessage("loading: %s", 0, mapname);

   try {
      if (loader.loadmap(mapname) == 0) {
         actmap->startGame();
         actmap->setupResources();

         do {
           next_turn();
           if ( actmap->time.a.turn == 2 ) {
              displaymessage("no human players found !", 1 );
              delete actmap;
              actmap = NULL;
              throw NoMapLoaded();
           }
         } while ( actmap->player[actmap->actplayer].stat != Player::human ); /* enddo */

      }
   } /* endtry */
   catch ( InvalidID err ) {
      displaymessage( err.getMessage().c_str(), 1 );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      displaymessage( "File %s has invalid version.\nExpected version %d\nFound version %d\n", 1, err.getFileName().c_str(), err.expected, err.found );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading map filename %s ", 1, err.getFileName().c_str() );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( ASCexception ) {
      displaymessage( "error loading map", 1 );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */

   removemessage();

}



void         tnewcampaignlevel::done (void)
{
   tdialogbox::done();
}

void         tcontinuecampaign::setid(word         id)
{
   idsearched = id;
}


void         tcontinuecampaign::showmapinfo(word         ypos)
{
   collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + xsize );
   tnewcampaignlevel::showmapinfo(ypos);
   npush( activefontsettings );

   activefontsettings.font = schriften.smallarial;
   if (message1[0] == 0) {
       activefontsettings.length = 120;
       activefontsettings.justify = righttext;
       activefontsettings.color = textcolor;
       activefontsettings.background = dblue;
       showtext2("password:",x1 + 25,y1 + ypos + 120);

       activefontsettings.color = black;
       activefontsettings.length = 200;
       activefontsettings.justify = lefttext;
       showtext2(strupr(password),x1 + 135,y1 + ypos + 140);

   }

   npop( activefontsettings );
}


void         tcontinuecampaign::evaluatemapinfo( const char *srname, tmap* spfld )
{

   if ( spfld->campaign ) {
      char b = 1;
      if (spfld->campaign->id == idsearched) {
         if ( actmap->campaign )
            if ( spfld->campaign->prevmap == actmap->campaign->id) {
               b = 0;
               if ( ( status == 0 ) || ( status == 5) || ( status == 7 )) {
                  status = 8;
                  strcpy ( message2, srname);
                  strcat ( message2, " ");
                  strcpy ( mapinfo, dateiinfo );

                  if ( spfld->title )
                     strcpy ( maptitle, spfld->title);
                  else
                     maptitle[0] = 0;

                  strcpy ( mapname, srname);
                  strcpy(  password, spfld->codeword);
               }
               else
                  if ((status == 6) || (status == 8)) {
                     status = 6;
                     strcat ( message2, srname );
                     strcat ( message2, " ");
                  }
            }

         if ( b ) {
            strcat( message2, srname );
            strcat( message2, " " );

            if (status == 0) {
               strcpy ( mapinfo, dateiinfo );
               if ( spfld->title )
                  strcpy ( maptitle, spfld->title);
               else
                  maptitle[0] = 0;
               strcpy ( mapname, srname);
               strcpy( password, spfld->codeword );
               status = 7;
            }
            else
               if ((status == 5) || (status == 7)) {
                  status = 5;
               }
         }
      }
   }
}



void         tcontinuecampaign::init(void)
{
  #define leftspace 25

   tnewcampaignlevel::init();
   title = "next campaign level";
   x1 = 100;
   xsize = 440;
   y1 = 100;
   ysize = 350;

   addbutton("~s~ave recovery information",leftspace,270,xsize - leftspace,290,0,1,2,true);
   disablebutton ( 2 );

   addbutton("~g~o !",leftspace,300,( xsize - leftspace ) /2 , 330,0,1,1,true);

   addbutton("~q~uit~ !", ( xsize + leftspace ) /2 ,300, xsize - leftspace , 330,0,1,3,true);

   buildgraphics();

   rahmen3("map info",x1 + leftspace,y1 + 50,x1 + xsize - leftspace,y1 + 250,1);
}


void         tcontinuecampaign::buttonpressed(int         id)
{

   switch (id) {

      case 1:   {
            if ((status == 7) || (status == 8 ))
               status = 20;
            else {
               if ((status == 5) || (status == 6 )) {
                  if (viewtextquery( 901,"warning", "~c~ancel", "c~o~ntinue" ) == 1)
                     status = 21;
               }
               if (status <= 1) {
                  if (viewtextquery( 902,"Fatal Error", "~c~ancel", "c~o~ntinue" ) == 1)
                     status = 21;
               }
            }
         }

      break;

      case 2:   {
            ASCString t;

            fileselectsvga("*.rcy", &t, 2);
            if ( !t.empty() )
               savecampaignrecoveryinformation ( t.c_str(), idsearched);
         }
      break;


      case 3: {
         if (choice_dlg("do you really want to quit the program ?","~n~o","~y~es") == 2)
            status = 50;
            }
      break;
   }
}


void         tcontinuecampaign::regroupevents ( pmap map )
{
   int mapid = 0;
   if ( map->campaign )
      mapid = map->campaign->id;


   peventstore oev = oldevent = map->oldevents;

   if ( oev )
      while ( oev->next )
         oev = oev->next;

   pevent ev = map->firsteventpassed;

   while ( ev ) {
      // peventstore foev;

      if ( oev == NULL ) {
         oev = new ( teventstore ) ;
         oldevent = oev;
         // foev = oev;
         oev->next = NULL;
         oev->num = 0;
      }
      else
         if (oev->num == 256) {
            peventstore foev = oev;
            oev = new ( teventstore ) ;
            foev->next = oev;
            oev->num = 0;
            oev->next = NULL;
         }


      oev->eventid[oev->num] = ev->id;
      oev->mapid[oev->num] = mapid;
      oev->num++;
      ev = ev->next;
   }

   tmemorystream  memoryStream ( &memoryStreamBuffer, tnstream::writing );
   for (int i = 0; i<8; i++) {
      map->player[i].research.write_struct ( memoryStream );
      map->player[i].research.write_techs ( memoryStream );
      dissectedunits[i] = map->player[ i ].dissections;
   }
}


void         tcontinuecampaign::run(void)
{

   searchmapinfo();
   showmapinfo(70);
   mousevisible(true);

   do {
      tnewcampaignlevel::run();
   }  while (status <= 10);

   if (status == 20) {
      int i;

      regroupevents( actmap );

      actmap->oldevents = NULL;

      loadcampaignmap();
      actmap->oldevents = oldevent;
      tmemorystream  memoryStream ( &memoryStreamBuffer, tnstream::reading );

      for (i=0;i<8 ; i++) {
         actmap->player[i].research.read_struct ( memoryStream );
         actmap->player[i].research.read_techs ( memoryStream );
         actmap->player[ i ].dissections = dissectedunits[i];
      }

   }
   if (status == 50)
      exit( 0 );
}

void         tchoosenewmap::readmapinfo(void)
{

   message1[0] = 0;
   message2[0] = 0;
   mapinfo[0] = 0;
   maptitle[0] = 0;


    try {
       tnfilestream filestream ( mapname, tnstream::reading );
       tmaploaders spfldloader;
       spfldloader.stream = &filestream;
       // spfldloader.setcachingarrays ( );

       CharBuf description;

       spfldloader.stream->readpchar ( &description.buf );
       // int desclen = strlen ( description.buf ) + 7;
       strncpy ( dateiinfo, description.buf, sizeof ( dateiinfo) - 2  );

       word w;
       spfldloader.stream->readdata2 ( w );

       if ( w != fileterminator )
          throw tinvalidversion ( mapname, w, fileterminator );

       int version;
       spfldloader.stream->readdata2( version );

         if (version > actmapversion || version < minmapversion )
            throw tinvalidversion ( mapname, version, actmapversion );

       spfldloader.readmap ();
       checkforcampaign( spfldloader.spfld );
   }
   catch ( tfileerror ) {
      strcpy( message1, "invalid map version" );
      status = 1;
   } /* endcatch */
}

void         tchoosenewmap::buttonpressed( int id )
{
   ASCString t;

   switch (id) {

      case 2:   {
            fileselectsvga(mapextension, &t, 1);
            if ( !t.empty() ) {
               strcpy(mapname, t.c_str());
               readmapinfo();
            }
            else
               strcpy(message1,"no map selected");
            showmapinfo(260);
         }
      break;

      case 3:   if (status >= 7)
            status = 20;
      break;

      case 4:   status = 15;
      break;

      case 5:   {
            if (password[0] != 0) {
               searchmapinfo();
               showmapinfo(260);
            }
         }
   break;
   }
}




void         tchoosenewmap::init( char* ptitle )
{
   tnewcampaignlevel::init();
   password[0] = 0;
   mapname[0] = 0;
   maptitle[0] = 0;
   mapinfo[0] = 0;
   status = 0;

   title = ptitle;
   x1 = 120;
   xsize = 400;
   y1 = 20;
   ysize = 440;

   addbutton("~p~assword",25,105,140,125,1,0,1,true);
   addeingabe(1, password,10,10);

   addbutton("se~a~rch map",150,100,360,125,0,1,5,true);

   addbutton("s~e~lect map",150,200,360,225,0,1,2,true);


   addbutton("~s~tart",20,390,195,420,0,1,3,true);
   addkey(3,ct_enter);

   addbutton("~c~ancel",205,390,380,420,0,1,4,true);
   addkey(4,ct_esc);

   buildgraphics();
   rahmen3("search map by password",x1 + 10,y1 + starty + 30,x1 + xsize - 20,y1 + starty + 100,1);
   rahmen3("select map by filename",x1 + 10,y1 + starty + 130,x1 + xsize - 20,y1 + starty + 200,1);

}



void         tchoosenewsinglelevel::checkforcampaign( tmap* spfld )
{
   if ( spfld->campaign ) {
      if ( spfld->campaign->directaccess == 0 ) {
         strcpy ( message1, "access only allowed by password" );
         status = 1;
      }
      else {
         strcpy( mapinfo, dateiinfo );
         if ( spfld->title )
            strcpy ( maptitle, spfld->title);
         else
            maptitle[0] = 0;
         status = 7;
      }
   }
   else {
      strcpy( mapinfo, dateiinfo );
      if ( spfld->title )
         strcpy ( maptitle, spfld->title);
      else
         maptitle[0] = 0;
      status = 7;
   }

}

void         tchoosenewcampaign::checkforcampaign( tmap* spfld )
{
   if ( spfld->campaign == NULL ) {
      strcpy( message1, "no campaign map" );
      status = 1;
   }
   else
      if (spfld->campaign->directaccess == false) {
         strcpy ( message1, "access only allowed by password" );
         status = 1;
      } else {
         strcpy( mapinfo, dateiinfo );
         if ( spfld->title )
            strcpy ( maptitle, spfld->title);
         else
            maptitle[0] = 0;
         status = 7;
      }

}

void         tchoosenewcampaign::run(void)
{

   mousevisible(true);
   do {
      tchoosenewmap::run();
   }  while (status <= 10);
   if (status == 20) {
      loadcampaignmap();
   }
}

void         tchoosenewsinglelevel::run(void)
{
   mousevisible(true);
   do {
      tchoosenewmap::run();
   }  while (status <= 10);

   if (status == 20) {
      displaymessage("loading: %s ",0, mapname);

      try {
         loadmap( mapname );
         actmap->startGame();

         removemessage();
         if (actmap->campaign != NULL) {
            delete actmap->campaign;
            actmap->campaign = NULL;
         }

         setupalliances();

         int human = 0;
         for ( int i = 0; i < 8; i++ )
            if ( actmap->player[i].stat == Player::human )
               if ( actmap->player[i].exist() )
                  human++;

         if ( !human ) {
            displaymessage ( "no human players found !", 1 );
            delete actmap;
            actmap = NULL;
            throw NoMapLoaded();
         }

         if ( human > 1 )
            multiplayersettings ();
         else {
            choosetechlevel();
            setmapparameters();
         }

         actmap->setupResources();
         tlockdispspfld lock;
         repaintdisplay();
         computeview( actmap );

         do {
           next_turn();
           if ( actmap->time.a.turn == 2 ) {
              displaymessage("no human players found !", 1 );
              delete actmap;
              actmap = NULL;
              throw NoMapLoaded();
           }
         } while ( actmap->player[actmap->actplayer].stat != Player::human ); /* enddo */

      } /* endtry */

      catch ( InvalidID err ) {
         displaymessage( err.getMessage().c_str(), 1 );
         if ( !actmap || actmap->xsize <= 0)
            throw NoMapLoaded();
      } /* endcatch */
      catch ( tinvalidversion err ) {
         displaymessage( "File %s has invalid version.\nExpected version %d\nFound version %d\n", 1, err.getFileName().c_str(), err.expected, err.found );
         if ( !actmap || actmap->xsize <= 0)
            throw NoMapLoaded();
      } /* endcatch */
      catch ( tfileerror err) {
         displaymessage( "error reading map filename %s ", 1, err.getFileName().c_str() );
         if ( !actmap || actmap->xsize <= 0)
            throw NoMapLoaded();
      } /* endcatch */
      catch ( ASCexception ) {
         displaymessage( "error loading map", 1 );
         if ( !actmap || actmap->xsize <= 0)
            throw NoMapLoaded();
      } /* endcatch */

   } 
} 



void         tchoosenewsinglelevel::init(void)
{ 
   tchoosenewmap::init("new single map");
} 


void         tchoosenewcampaign::init(void)
{ 
   tchoosenewmap::init("new campaign");
} 

     class ttributepayments : public tdialogbox {
                       tmap::ResourceTribute trib;
                       int oldplayer;
                       int player;
                       void paintactplayer ( void );
                       void paintvalues ( void );
                       int status;
                       int players[8];
                       int wind1y, wind2y ;
                       int wind1x, wind2x ;

                    public:
                       void init ( void );
                       void buttonpressed ( int id );
                       void run ( void );
               };


void  ttributepayments :: init ( void )
{
  int i;

   tdialogbox::init();
   oldplayer = -1;
   player = 0;
   status = 0;
   xsize = 550;
   ysize = 400;
   title = "transfer resources";

   trib = actmap->tribute;

   memset ( &players, -1, sizeof( players ));

   windowstyle &= ~dlg_in3d;

   addbutton ( "~O~k",     10,          ysize - 40, xsize/2 - 5,  ysize - 10, 0, 1, 1, true );
   addkey ( 1, ct_enter );

   addbutton ( "~C~ancel", xsize/2 + 5, ysize - 40, xsize-10 - 5, ysize - 10, 0, 1, 2, true );
   addkey ( 2, ct_esc );

   wind1y = starty + 10;



   buildgraphics();

   for (i = 0; i < 3; i++) 
      addbutton ( NULL, 250, wind1y + 15 + i * 40, 350, wind1y + 32 + i * 40, 2, 1, 3 + i, true );

   int pos = 0;
   for ( i = 0; i < 8; i++ )
      if ( actmap->player[i].exist() )
         if ( i != actmap->actplayer )
            players[pos++] = i;


   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = black;
   activefontsettings.length = 145 - 55;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;

   for ( i = 0; i < pos; i++) {
      bar ( x1 + 30, y1 + starty + 30 + i * 30, x1 + 50, y1 + starty + 50 + i * 30, 20 + players[i] * 8 );
      showtext2 ( actmap->player[players[i]].getName().c_str(), x1 + 55, y1 + starty + 30 + i * 30 );
   }

   wind2y = starty + ( ysize - starty - 60 ) /2 + 5;

   wind1x = 255 ;
   wind2x = 375 ;

   rahmen ( true,      x1 + 10,  y1 + starty,      x1 + xsize - 10, y1 + ysize - 50 );
   rahmen3 ( "player", x1 + 20,  y1 + starty + 10, x1 + 150,        y1 + ysize - 60, 1 );

   rahmen3 ( "you send",            x1 + 160, y1 + wind1y,      x1 + wind2x - 10, y1 + wind2y - 10, 1 );
   rahmen3 ( "you still have outstanding",    x1 + 160, y1 + wind2y,      x1 + wind2x - 10, y1 + ysize - 60 , 1 );

   rahmen3 ( "you have sent",    x1 + wind2x - 5, y1 + wind1y,      x1 + xsize - 20, y1 + wind2y - 10, 1 );
   rahmen3 ( "you got",     x1 + wind2x - 5, y1 + wind2y,      x1 + xsize - 20, y1 + ysize - 60 , 1 );


   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = textcolor;
   activefontsettings.length = 0;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;

   for ( i = 0; i < 3; i++) {
      showtext2 ( resourceNames[i], x1 + 170, y1 + wind1y + 15 + i * 40 );
      showtext2 ( resourceNames[i], x1 + 170, y1 + wind2y + 15 + i * 40 );

      rectangle ( x1 + wind2x, y1 + wind1y + 15 + i * 40, x1 + xsize - 30 , y1 + wind1y + 32 + i * 40, black );
      rectangle ( x1 + wind1x, y1 + wind2y + 15 + i * 40, x1 + wind2x - 15, y1 + wind2y + 32 + i * 40, black );
      rectangle ( x1 + wind2x, y1 + wind2y + 15 + i * 40, x1 + xsize - 30 , y1 + wind2y + 32 + i * 40, black );
   } /* endfor */

   paintactplayer();
   paintvalues();
}

void  ttributepayments :: paintactplayer ( void )
{
   int xx1 = x1 + 25;
   int xx2 = x1 + 145;
   int yy1 = y1 + starty + 25 + oldplayer * 30;
   int yy2 = yy1 + 30;
   if ( oldplayer != -1 ) { 
      setinvisiblemouserectanglestk ( xx1, yy1, xx2, yy2 );
      xorrectangle ( xx1, yy1, xx2, yy2, 14 );
      getinvisiblemouserectanglestk ();
   }

   yy1 = y1 + starty + 25 + player * 30;
   yy2 = yy1 + 30;
   if ( player != -1 ) {
      setinvisiblemouserectanglestk ( xx1, yy1, xx2, yy2 );
      xorrectangle ( xx1, yy1, xx2, yy2, 14 );
      getinvisiblemouserectanglestk ();
   }

   oldplayer = player;
}

void  ttributepayments :: paintvalues ( void )
{
   setinvisiblemouserectanglestk ( x1 + 10,  y1 + starty,      x1 + xsize - 10, y1 + ysize - 50 );
 
   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = textcolor;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;
   activefontsettings.background = dblue;

   for ( int i = 0; i < 3; i++) {
      addeingabe ( 3+i, &trib.avail[actmap->actplayer][players[player]].resource(i), 0, maxint );
      enablebutton ( 3+i );

      activefontsettings.length = xsize - 40 - wind2x;
      showtext2 ( strrr ( trib.paid[actmap->actplayer][players[player]].resource(i)), x1 + wind2x + 5, y1 + wind2y + 16 + i * 40 );
      showtext2 ( strrr ( trib.paid[players[player]][actmap->actplayer].resource(i)), x1 + wind2x + 5, y1 + wind1y + 16 + i * 40 );

      activefontsettings.length = wind2x - wind1x - 35;
      showtext2 ( strrr ( trib.avail[players[player]][actmap->actplayer].resource(i)), x1 + wind1x + 5, y1 + wind2y + 16 + i * 40 );
   } /* endfor */

   getinvisiblemouserectanglestk ();
}


void  ttributepayments :: buttonpressed( int id )
{
   tdialogbox::buttonpressed( id );
   if ( id == 1 )
      status = 11;
   if ( id == 2 )
      status = 10;
}
     
void  ttributepayments :: run ( void )
{
   mousevisible(true);
   do {
      tdialogbox::run();
      if ( taste == ct_down ) {
         player++;
         if ( players[ player ] == -1 )
            player = 0;
      }
      if ( taste == ct_up ) {
         player--;
         if ( player < 0 )
            player = 7;
         while ( players[ player] == -1 )
            player--;
      }
      if ( mouseparams.taste & 1 )
         for (int i = 0; i < 8; i++) {
            if ( (mouseparams.x >= x1 + 25) && 
                 (mouseparams.y >= y1 + starty + 25 + i * 30) &&
                 (mouseparams.x <= x1 + 145) &&
                 (mouseparams.y <= y1 + starty + 55 + i * 30 ) )
                   if ( players[i] != -1 )
                     player = i;

         } /* endfor */

      if ( player != oldplayer ) {
         paintactplayer();
         paintvalues();
      }

   } while ( status < 10 ); /* enddo */

   if ( status >= 11 )
      actmap->tribute = trib;
}
   

void settributepayments ( void )
{
   ttributepayments tpm;
   tpm.init();
   tpm.run();
   tpm.done();
}

void  tshownewtanks :: init ( char*      buf2 ) 
{
   tdialogbox::init();

   title = "new units available";
   xsize = 400;
   x1 = 120;
   y1 = 100;
   ysize = 280;

   addbutton("~o~k", 10, ysize - 35, xsize - 10, ysize - 10, 0, 1, 1 , true );
   addkey(1, ct_enter);
   addkey(1, ct_space);

   buildgraphics();

   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = black;
   activefontsettings.background = 255;
   activefontsettings.justify = lefttext;
   activefontsettings.length = xsize - 80;

   buf = buf2;
   int i, num = 0;
   for (i=0; i < vehicletypenum ;i++ ) {
      if ( buf[i] ) {
         pvehicletype tnk = getvehicletype_forpos ( i );
         if ( tnk ) {
            bar ( x1 + 25, y1 + 45 + num * 50, x1 + 65, y1 + 85 + num * 50, dblue );
            putrotspriteimage (  x1 + 30, y1 + 50 + num * 50, tnk -> picture[0] , actmap->actplayer * 8);
            showtext2( tnk -> name, x1 + 70, y1 + 45 + num * 50 );
            showtext2( tnk -> description, x1 + 70, y1 + 45 + 40 + num * 50 - activefontsettings.font->height );
            num++;
         }
      }

   } /* endfor */
}

void  tshownewtanks :: run ( void )
{
   status = 0;
   mousevisible(true);
   do {
      tdialogbox :: run ();
   } while ( status == 0 ); /* enddo */
}

void  tshownewtanks :: buttonpressed ( int id )
{
   if (id == 1)
      status = 1;
}

class tresearchinfo: public tdialogbox {
                    protected:
                      int rppt;     // researchpoints per turn
                      int status;



                    public: 
                      tresearchinfo ( void );
                      void init ( void );
                      void count ( void ); 
                      void buttonpressed ( int id );
                      void run ( void );

                  };

tresearchinfo::tresearchinfo ( void )
{
   rppt = 0;
   status = 0;
}

void tresearchinfo::init ( void )
{
   tdialogbox::init();
   x1 = -1;
   y1 = -1;
   xsize = 500;
   ysize = 400;
   title = "research";
   addbutton ( "~O~K", 10, ysize - 30, xsize - 10, ysize - 10, 0, 1, 1, true );
   addkey( 1, ct_enter );
   addkey( 1, ct_esc );
   buildgraphics();
   count();
   rahmen ( true, x1 + 19, y1 + 69, x1 + xsize - 19, y1 + 91 );
   bar ( x1 + 20, y1 + 70, x1 + xsize - 20, y1 + 90, dblue );

   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = textcolor;
   activefontsettings.justify = righttext;
   activefontsettings.length = 200;
   activefontsettings.background = 255;
   
   int textxpos = x1 + xsize / 2 - 5 - activefontsettings.length;

   showtext2 ( "name:", textxpos, y1 + 110 );
   showtext2 ( "research points at all:", textxpos, y1 + 140 );
   showtext2 ( "research points done:", textxpos, y1 + 170 );
   showtext2 ( "research points needed:", textxpos, y1 + 200 );
   showtext2 ( "research done ( % ):", textxpos, y1 + 230 );
   showtext2 ( "research points / turn:", textxpos, y1 + 260 );
   showtext2 ( "time left:", textxpos, y1 + 290 );


   activefontsettings.justify = lefttext;
   activefontsettings.length = 0;
   textxpos = x1 + xsize / 2 + 5 ;
   if ( actmap->player[actmap->actplayer].research.activetechnology ) {
      bar ( x1 + 20, y1 + 70, x1 + 20 + (xsize - 40) * actmap->player[actmap->actplayer].research.progress / actmap->player[actmap->actplayer].research.activetechnology->researchpoints, y1 + 90, textcolor );


      int rp = actmap->player[actmap->actplayer].research.activetechnology->researchpoints;
      int prog = actmap->player[actmap->actplayer].research.progress;
      showtext2 ( actmap->player[actmap->actplayer].research.activetechnology->name,             textxpos, y1 + 110 );                               
      showtext2 ( strrr ( rp ),                       textxpos, y1 + 140 );
      showtext2 ( strrr ( prog ),                     textxpos, y1 + 170 );
      showtext2 ( strrr ( rp - prog ),                textxpos, y1 + 200 );
      if ( rp )
         showtext2 ( strrr ( 100 * prog / rp ),          textxpos, y1 + 230 );
      if ( rppt )
         showtext2 ( strrr ( ( rp - prog ) / rppt + 1 ), textxpos, y1 + 290 );

   }
   showtext2 ( strrr ( rppt ),                   textxpos, y1 + 260 );


}


void tresearchinfo::count ( void )
{
   for ( tmap::Player::BuildingList::iterator i = actmap->player[actmap->actplayer].buildingList.begin(); i != actmap->player[actmap->actplayer].buildingList.end(); i++ )
      rppt += (*i)->researchpoints;
}

void tresearchinfo::buttonpressed ( int id )
{
   if ( id == 1 )
      status = 10;
}

void tresearchinfo::run ( void )
{
   mousevisible(true);
   do {
     tdialogbox::run();
   } while ( status < 10 ); /* enddo */
}


void researchinfo ( void )
{
   tresearchinfo ri;
   ri.init();
   ri.run();
   ri.done();
}



 class   tshowtechnology : public tdialogbox {
               public:
                  ptechnology       tech;
                  void              init( ptechnology acttech );
                  virtual void      run ( void );
                  void              showtec ( void );
         };


void         tshowtechnology::init(  ptechnology acttech  )
{ 
   tdialogbox::init();
   title = "new technology";
   buildgraphics();
   tech = acttech;
} 


void         tshowtechnology::showtec(void)
{ 
   char         *wort1, *wort2;
   char         *pc, *w2;
   word         xp, yp, w;

   activefontsettings.font = schriften.large; 
   activefontsettings.justify = centertext;
   activefontsettings.length = xsize - 40;
   showtext2(tech->name,x1 + 20,y1 + starty + 10); 
   activefontsettings.justify = lefttext; 

   yp = 60; 

   if (tech->icon) {
      int xs,ys;
      getpicsize ( (trleheader*) tech->icon, xs, ys );
      putimage ( x1 + ( xsize - xs) / 2 , y1 + starty + 45 , tech->icon );
      yp += ys + 10;
   }



   wort1 = new char[100];
   wort2 = new char[100];
   strcpy( wort1, "research points: " );
   itoa ( tech->researchpoints, wort2, 10 );
   strcat( wort1, wort2 );

   activefontsettings.font = schriften.smallarial; 
   showtext2(wort1, x1 + 30,y1 + yp);

   if (tech->infotext != NULL) { 
      activefontsettings.color = black; 
      xp = 0; 
      pc = tech->infotext; 
      while (*pc ) {
         w2 = wort1;
         while ( *pc  && *pc != ' ' && *pc != '-' ) {
            *w2 = *pc;
            w2++;
            pc++;
         };
         *w2 = *pc;
         if (*pc) {
            w2++;
            pc++;
         }
         *w2=0;
         
         w = gettextwdth(wort1,NULL);
         if (xp + w > xsize - 40) { 
            yp = yp + 5 + activefontsettings.font->height; 
            xp = 0; 
         } 
         showtext2(wort1,x1 + xp + 20,y1 + starty + yp);
         xp += w;
      } 
   } 
   delete[] wort1 ;
   delete[] wort2 ;
} 



void         tshowtechnology::run(void)
{ 
   showtec(); 
   do { 
      tdialogbox::run();
   }  while ( (taste != ct_esc) && (taste != ct_space) && (taste != ct_enter) );
} 




void         showtechnology(ptechnology  tech )
{ 
   if ( tech ) { 
      if ( tech->pictfilename ) {
         mousevisible(false);
         bar ( 0,0, agmp->resolutionx-1, agmp->resolutiony-1, black );
         activefontsettings.length = agmp->resolutionx - 40;
         activefontsettings.justify = centertext;
         activefontsettings.background = 255;
         activefontsettings.height = 0;
         activefontsettings.font = schriften.large;
         activefontsettings.color = white;
         showtext2 ( "A new technology" , 20, 200 );
         showtext2 ( "has been discovered", 20, 280 );
         int t = ticker;
         while ( mouseparams.taste )
            releasetimeslice();
         do {
            releasetimeslice();
         } while ( t + 200 > ticker  &&  !keypress()  && !mouseparams.taste); /* enddo */

         int abrt = 0;
         while ( keypress() )
           r_key();


         int fs = loadFullscreenImage ( tech->pictfilename );
         if ( fs ) {

            t = ticker;
            while ( mouseparams.taste )
               releasetimeslice();

            do {
               releasetimeslice();
            } while ( t + 600 > ticker  &&  !keypress()  && !mouseparams.taste && !abrt ); /* enddo */
   
            closeFullscreenImage();
         }            
         activefontsettings.length = agmp->resolutionx - 40;
         activefontsettings.justify = centertext;
         activefontsettings.background = 255;
         activefontsettings.height = 0;
         activefontsettings.font = schriften.large;
         activefontsettings.color = white;
         bar ( 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, 0 );
         showtext2 ( tech->name, 20, 20 );

         if ( tech->infotext ) {
            tviewtext vt;
            vt.setparams ( 20, 50, agmp->resolutionx - 20, agmp->resolutiony - 20, tech->infotext, white, black );
            vt.tvt_dispactive = 0;
            vt.displaytext ();

            int textsizeycomplete = vt.tvt_yp;
            int textsizey = agmp->resolutiony - 70 ;

            vt.tvt_dispactive = 1;
            vt.displaytext ();
   
            abrt = 0;
            int scrollspeed = 10;
            do {
               tkey taste = r_key();

               if ( textsizeycomplete > textsizey ) {
                  int oldstarty = vt.tvt_starty;
                  if ( taste == ct_down ) 
                     if ( vt.tvt_starty + textsizey + scrollspeed < textsizeycomplete )
                        vt.tvt_starty += scrollspeed;
                     else
                         vt.tvt_starty = textsizeycomplete - textsizey;

                  if ( taste == ct_up ) 
                     if ( vt.tvt_starty - scrollspeed > 0 )
                        vt.tvt_starty -= scrollspeed;
                     else
                         vt.tvt_starty = 0;

                  if ( oldstarty != vt.tvt_starty )
                      vt.displaytext();

               }

               if ( taste == ct_esc || taste == ct_enter || taste == ct_space )
                  abrt = 1;

            } while ( !abrt ); /* enddo */

            repaintdisplay();
         }
      } else {
         tshowtechnology sh; 
         sh.init( tech ); 
         sh.run(); 
         sh.done(); 
      }
   } 
} 


class tchoosetechlevel : public tdialogbox {
             protected:
                int techlevel; 
                int ok;
             public: 
                void init ( void );
                void buttonpressed ( int id );
                void run ( void );
         };

void tchoosetechlevel :: init ( void )
{
   tdialogbox :: init ( );
   title = "choose techlevel";
   xsize = 200;
   ysize = 150;
   techlevel = 0;
   ok = 0;

   addbutton ( "~O~k", 10, ysize - 30, xsize - 10, ysize - 10 , 0, 1, 1, true );
   addkey ( 1, ct_enter );

   addbutton ( "", 10, 60, xsize - 10, 85                     , 2, 1, 2, true );
   addeingabe ( 2, &techlevel, 0, 255 );
   addkey ( 1, ct_space );

   buildgraphics();
}

void tchoosetechlevel :: buttonpressed ( int id )
{
   tdialogbox:: buttonpressed ( id );
   if ( id == 1 )
      ok = 1;
}

void tchoosetechlevel :: run ( void )
{
   mousevisible ( true );
   do {
      tdialogbox::run();
   } while ( !ok ); 
   if ( techlevel > 0 )
      for ( int i = 0; i < 8; i++ )
         actmap->player[i].research.settechlevel ( techlevel );

};

void choosetechlevel ( void )
{
   tchoosetechlevel ctl;
   ctl.init();
   ctl.run();
   ctl.done();
}



#undef linelength

#define blocksize 256

typedef class tparagraph* pparagraph;
class  tparagraph {
        public:
          tparagraph ( void );
          tparagraph ( pparagraph prv );   // f?gt einen neuen paragraph hinter prv an

          void join ( void );   // returnvalue : paragraph to delete;
          void changesize ( int newsize );

          void addchar ( char c );
          pparagraph erasechar ( int c );
          void checkcursor ( void );
          pparagraph movecursor ( int dx, int dy );
          pparagraph cut ( void );

          int  reflow( int all = 1 );
          void display ( void );

          void checkscrollup ( void );
          void checkscrolldown ( void );
          int  checkcursorpos ( void );

          void addtext ( const ASCString& txt );
          ~tparagraph ();

          void setpos ( int x1, int y1, int y2, int linepos, int linenum );

          void displaycursor ( void );
          int cursor;
          int cursorstat;
          int cursorx;
          int normcursorx;
          int cursory;
          int searchcursorpos;
          static int maxlinenum;

          int size;
          int allocated;
          char* text;

          static int winy1;
          static int winy2;
          static int winx1;
          struct {
             int line1num;
          } ps;

          dynamic_array<char*> linestart;
          dynamic_array<int>   linelength;

          int   linenum;
                         
          pparagraph next;
          pparagraph prev;
      };

StaticClassVariable int tparagraph :: winy1;
StaticClassVariable int tparagraph :: winy2;
StaticClassVariable int tparagraph :: winx1;
StaticClassVariable int tparagraph :: maxlinenum;

tparagraph :: tparagraph ( void )
{
   text = new char [ blocksize ];
   text[0] = 0;
   size = 1;
   allocated = blocksize;

   next = NULL;
   prev = NULL;
   cursor = -1;
   cursorstat = 0;
   ps.line1num = 0;
   linenum = -1;
   searchcursorpos = 0;
}

tparagraph :: tparagraph ( pparagraph prv )
{
   searchcursorpos = 0;
   linenum = -1;
   text = new char [ blocksize ];
   text[0] = 0;
   size = 1;
   allocated = blocksize;
   cursorstat = 0;
   cursor = -1;

   prev = prv;

   if ( prv ) {
      next = prv->next;
      if ( next )
         next->prev = this;
      prv->next = this;
   
      ps.line1num = prv->ps.line1num + 1 + prv->linenum;
   } else {
      next = NULL;
      ps.line1num = 0;
   }
}



tparagraph :: ~tparagraph ()
{
   if ( text ) {
      delete[] text;
      text = NULL;
   }

   if ( prev )
      prev->next = next;

   if ( next )
      next->prev = prev;

}


void tparagraph :: changesize ( int newsize )
{
   newsize = (newsize / blocksize + 1) * blocksize;
   char* temp = new char[newsize];
   if ( text ) {
      strcpy ( temp, text );
      delete[] text;
   } else
      temp[0] = 0;

   allocated = newsize;
   size = strlen ( temp )+1;
   text = temp;
}


void tparagraph :: join ( void )
{
   cursor = size-1;
   addtext ( next->text );
   delete next;
   if ( reflow() ) {
       display();
       if ( checkcursorpos() ) {
          checkscrollup();
          checkscrolldown();
       }
   }


}



void tparagraph :: addtext ( const ASCString& txt )
{
    int nsize = txt.length();
    if ( allocated < nsize + size )
       changesize ( nsize + size );

    strcat ( text, txt.c_str() );
    size = strlen ( text ) + 1;
}

void tparagraph :: checkcursor( void )
{
   if ( cursor != -1 ) {
      if ( cursor >= size )
         cursor = size-1;
      if ( cursor < 0 )
         cursor = 0;
   } else
      displaymessage ( " void tparagraph :: checkcursor ( void ) \ncursor not in paragraph !\n text is : %s", 2, text );
}


void tparagraph :: addchar ( char c )
{
   checkcursor();
   if ( size + 1 > allocated )
      changesize ( size + 1 );

    for ( int i = size; i > cursor; i--)
       text[i] = text[i-1];

    text[cursor] = c;
    cursor++;
    size++;

    if ( reflow() ) {
       display();
       checkcursorpos();
    }
}


pparagraph tparagraph :: erasechar ( int c )
{
   checkcursor();
   if ( c == 1 ) {     // backspace
      if ( cursor ) {
         for ( int i = cursor-1; i < size; i++)
            text[i] = text[i+1];
         cursor--;
         size--;
         if ( reflow() )
            display();
      } else
         if ( prev ) {
            pparagraph temp = prev;
            prev->join();
            return temp;              //  !!!###!!!   gef„llt mir eigentlich ?berhauptnicht, wird aber wohl laufen. Sonst m?áte ich halt erasechar von auáen managen
         }

   }
   if ( c == 2 ) {     // del
      if ( cursor < size-1 ) {
         for ( int i = cursor; i < size; i++)
            text[i] = text[i+1];
         size--;
         if ( reflow() )
            display();
      } else
         if ( next ) 
            join ( );
   }
   return this;
}

int  tparagraph :: checkcursorpos ( void )
{  
   if ( cursor >= 0 ) {
      checkcursor();
      if ( ps.line1num + cursory >= maxlinenum ) {
         ps.line1num = maxlinenum - cursory - 1;
         display();
         checkscrollup();
         checkscrolldown();
         return 0;
      }
   
      if( ps.line1num + cursory < 0 ) {
         ps.line1num = -cursory;
         display();
         checkscrollup();
         checkscrolldown();
         return 0;
      }
   }
   return 1;
}

pparagraph tparagraph :: cut ( void )
{
   checkcursor();
   displaycursor();
   char* tempbuf = strdup ( &text[cursor] );
   text[cursor] = 0;
   size = strlen ( text ) + 1;
   reflow ( 0 );
   new tparagraph ( this );
   if ( tempbuf[0] )
      next->addtext ( tempbuf );

   asc_free ( tempbuf );
   cursor = -1;
   next->cursor = 0;
   reflow( 0 );
   display();
   if ( next->reflow() )
      next->display();

   return next;
}

void tparagraph :: checkscrollup ( void )
{
   if ( prev ) {
      if ( prev->ps.line1num + prev->linenum + 1 != ps.line1num ) {
           prev->ps.line1num = ps.line1num - 1 - prev->linenum ;
           prev->display();
           prev->checkscrollup();
      }
   }
}

void tparagraph :: checkscrolldown ( void )
{
   if ( next ) {
      if ( ps.line1num + linenum + 1 != next->ps.line1num ) {
         next->ps.line1num = ps.line1num + linenum + 1;
         next->display();
         next->checkscrolldown ();
      }
   } else {
      if ( ps.line1num + linenum < maxlinenum )
         bar ( winx1, winy1 + (ps.line1num + linenum + 1) * activefontsettings.height, winx1 + activefontsettings.length, winy1 + maxlinenum * activefontsettings.height, dblue );

   }
}


int  tparagraph :: reflow( int all  )
{                                  
   int oldlinenum = linenum;

   pfont font = activefontsettings.font;
   linenum = 0;
   int pos = 0;
   linestart [ linenum ] = text;
   int length = 0;
   int oldlength = 0;

   do {        

     if ( font->character[text[pos]].size ) {
        length += font->character[text[pos]].width + 2 ;
        if ( pos )
           length += font->kerning[text[pos]][text[pos-1]];
     }

     if ( length > activefontsettings.length ) {
        int pos2 = pos;
        while ( ( text[pos2] != ' ' )  &&  ( &text[pos2] - linestart[linenum] )  )
           pos2--;
                                   
        if ( &text[pos2] == linestart[linenum] )
           pos2 = pos;
        else 
           pos2++;

        linelength[linenum] = &text[pos2] - linestart[linenum];
        if ( linelength[linenum] > 200 ) {
           printf("\a");
        }
        linenum++;
        linestart[linenum] = &text[pos2];
        length = 0;
        pos = pos2;
     } else
        pos++;
   } while ( pos <= size ); /* enddo */

   linelength[linenum] = &text[pos] - linestart[linenum];
        if ( linelength[linenum] > 200 ) {
           printf("\a");
        }


   oldlength = 0;
   for (int i = 0; i <= linenum; i++) {
      length = 0;
      for ( int j = 0; j < linelength[i]; j++ ) {
         pos = linestart[i] - text + j;

         if ( pos == cursor  && !searchcursorpos ) {
             cursorx = length;
             normcursorx = cursorx;
             cursory = i;
         }
    
         if ( searchcursorpos ) {
            if ( i == cursory )
               if ( length >= normcursorx ) {
                  cursorx = length;
                  cursor = pos;
                  searchcursorpos = 0;
               }
            if ( i > cursory ) {
               cursorx = oldlength;
               cursor = pos-1;
               searchcursorpos = 0;
            }
        }

        if ( font->character[text[pos]].size ) {
             length += font->character[text[pos]].width + 2 ;
             if ( pos )
                length += font->kerning[text[pos]][text[pos-1]];
        }

     }
     oldlength = length;
   } /* endfor */


   if ( searchcursorpos ) {
      cursorx = length;
      cursor = size-1;
      searchcursorpos = 0;
   }


   if ( all ) {
      if ( linenum != oldlinenum ) {
         display ( );
         if ( checkcursorpos() ) { 
            checkscrollup();
            checkscrolldown();
         }
         return 0;
      }
   }
   return 1;

}


pparagraph tparagraph :: movecursor ( int dx, int dy )
{
   pparagraph newcursorpos = this;

   if ( cursorstat )
      displaycursor();

   if ( dx == -1 )
      if ( cursor > 0 ) 
         cursor--;
      else 
         if ( prev ) {
            cursor = -1;
            newcursorpos = prev;
            prev->cursor = prev->size - 1;
         }
   if ( dx == 1 )
      if ( cursor < size-1 ) 
         cursor++;
      else
         if ( next ) {
            cursor = -1;
            next->cursor = 0;
            newcursorpos = next;
         }

   if ( dy == 1 ) 
      if ( cursory < linenum ) {
         cursory++;
         searchcursorpos++;
      } else
        if ( next ) {
           cursor = -1;
           next->cursor = 0;
           next->normcursorx = normcursorx;
           next->cursorx = normcursorx;
           next->cursory = 0;
           next->searchcursorpos = 1;
           newcursorpos = next;
        }

   if ( dy == -1 ) 
      if ( cursory > 0 ) {
         cursory--;
         searchcursorpos++;
      } else
        if ( prev ) {
           cursor = -1;
           prev->cursor = 0;
           prev->normcursorx = normcursorx;
           prev->cursorx = normcursorx;
           prev->cursory = prev->linenum;
           prev->searchcursorpos = 1;
           newcursorpos = prev;
        }

   newcursorpos->reflow( 0 );
   newcursorpos->checkcursorpos();
   newcursorpos->displaycursor();
   return newcursorpos;
}


void tparagraph :: displaycursor ( void )
{
   if ( cursor >= 0 ) {
      int starty;
      if ( ps.line1num < 0 ) {
         starty = winy1 + (ps.line1num + cursory) * activefontsettings.height;
         // startline = -ps.line1num;
      } else {
         starty = winy1 + (ps.line1num + cursory) * activefontsettings.height;
         // startline = 0;
      }

      if ( starty  < winy2 ) {
         collategraphicoperations cgo ( winx1 + cursorx-2,   starty, winx1 + cursorx+2,   starty + activefontsettings.height );
         setinvisiblemouserectanglestk ( winx1 + cursorx-2,   starty, winx1 + cursorx+2,   starty + activefontsettings.height  );
         xorline ( winx1 + cursorx,   starty+1, winx1 + cursorx,   starty + activefontsettings.height - 1, blue  );
         if ( cursorx )
           xorline ( winx1 + cursorx-1, starty+1, winx1 + cursorx-1, starty + activefontsettings.height - 1, blue  );
         cursorstat = !cursorstat;
         getinvisiblemouserectanglestk();
      }
   }
}

void tparagraph :: setpos ( int x1, int y1, int y2, int linepos, int linenum  )
{
   winx1 = x1;
   winy1 = y1;
   winy2 = y2;
   ps.line1num = linepos;
   maxlinenum = linenum;
}

void tparagraph :: display ( void )
{
   if ( cursorstat )
      displaycursor();
   cursorstat = 0;

   int startline;
   int starty;
   if ( ps.line1num < 0 ) {
      starty = winy1;
      startline = -ps.line1num;
   } else {
      starty = winy1 + ps.line1num * activefontsettings.height;
      startline = 0;
   }

   while ( startline <= linenum  && starty < winy2 ) {
      char *c = &linestart[ startline ] [ linelength [ startline ]] ;
      char d = *c;
      *c = 0;

      setinvisiblemouserectanglestk ( winx1, starty, winx1+activefontsettings.length, starty + activefontsettings.height );
      showtext2 ( linestart[ startline ], winx1, starty );
      getinvisiblemouserectanglestk ();

      *c = d;
      starty += activefontsettings.height;
      startline++;
   } ; /* enddo */

   if ( cursor >= 0 )
      displaycursor();
}



class tmessagedlg : public tdialogbox {
           protected:
               int to[8];

                pparagraph firstparagraph;
                pparagraph actparagraph;

                int tx1, ty1, tx2, ty2,ok;
                int lastcursortick;
                int blinkspeed;

            public:
                tmessagedlg ( void );
                virtual void setup ( void );
                void inserttext ( const ASCString& txt );
                void run ( void );
                ASCString extracttext ();
                ~tmessagedlg();


         };





tmessagedlg :: tmessagedlg ( void )
{
    firstparagraph = NULL;
    lastcursortick = 0;
    blinkspeed = 80;
    #ifdef _DOS_
     #ifdef NEWKEYB
     closekeyb();
     #endif
    #endif
}

void tmessagedlg :: inserttext ( const ASCString& txt )
{
   if ( txt.empty() )
      firstparagraph = new tparagraph;
   else {
      firstparagraph = NULL;
      actparagraph = NULL;
      int pos = 0;
      int start = 0;
      const char* c = txt.c_str();
      while ( c[pos] ) {
         int sz = 0;
         if ( strnicmp(&c[pos], "#CRT#" , 5 ) == 0 )
            sz = 5;
         if ( strnicmp(&c[pos], "\r\n" , 2 ) == 0 )
            sz = 2;
         if ( strnicmp(&c[pos], "\n" , 2 ) == 0 )
            sz = 1;
         if ( sz ) {
            ASCString s = txt;
            if ( start )
               s.erase ( 0, start );
            s.erase ( pos );
            actparagraph = new tparagraph ( actparagraph );
            if ( !firstparagraph )
               firstparagraph = actparagraph;

            actparagraph->addtext ( s );
            pos+=sz;
            start = pos;
         } else
            c++;
      } /* endwhile */

      if ( start < txt.length() ) {
         actparagraph = new tparagraph ( actparagraph );
         if ( !firstparagraph )
             firstparagraph = actparagraph;
         actparagraph->addtext ( &(txt.c_str()[start]) );
      }

   }
}

void tmessagedlg :: run ( void )
{
   tdialogbox::run ( );
   if ( prntkey != cto_invvalue ) {
      if ( prntkey == cto_bspace )
         actparagraph = actparagraph->erasechar ( 1 );
      else
      if ( taste == cto_entf )
         actparagraph->erasechar ( 2 );
      else
      if ( taste == cto_left )
         actparagraph = actparagraph->movecursor ( -1, 0 );
      else
      if ( taste == cto_right )
         actparagraph = actparagraph->movecursor ( 1, 0 );
      else
      if ( taste == cto_up )
         actparagraph = actparagraph->movecursor ( 0, -1 );
      else
      if ( taste == cto_down )
         actparagraph = actparagraph->movecursor ( 0, 1 );
      else
      if ( taste == cto_enter ) 
         actparagraph = actparagraph->cut ( );
      else
      if ( taste == cto_esc ) 
         printf("\a");
      else
      if ( prntkey > 31 && prntkey < 256 )
         actparagraph->addchar ( prntkey );
   }
   if ( lastcursortick + blinkspeed < ticker ) {
      actparagraph->displaycursor();
      lastcursortick = ticker;
   }
}

ASCString tmessagedlg :: extracttext ()
{
   tparagraph text;

   actparagraph = firstparagraph;
   while ( actparagraph ) {
      text.addtext ( actparagraph->text );
      text.addtext ( "#crt#" );
      actparagraph = actparagraph->next;
   }
   return text.text;

}

tmessagedlg :: ~tmessagedlg ( )
{
   actparagraph = firstparagraph;
   while ( actparagraph ) {
      pparagraph temp = actparagraph->next;
      delete actparagraph;
      actparagraph = temp;
   }
   #ifdef _DOS_
    #ifdef NEWKEYB
    initkeyb();
    #endif
   #endif
}

void tmessagedlg :: setup ( void )
{
   xsize = 500;
   ysize = 400;
   ok = 0;
   tx1 = 20;
   ty1 = starty + 10;
   tx2 = xsize - 40;
   ty2 = ysize - ty1 - 40;

   addbutton ( "~S~end", 10, ysize - 30, xsize / 2 - 5, ysize - 10 , 0, 1, 1, true );
   clearkey ( 1 );
   addkey ( 1, cto_stp + cto_s );

   addbutton ( "~C~ancel", xsize / 2 + 5, ysize - 30, xsize - 10, ysize - 10 , 0, 1, 2, true );
   clearkey ( 2 );
   addkey ( 2, cto_stp + cto_c );

   int num = 0;
   for ( int i = 0; i < 8; i++ ) {
      if ( actmap->player[i].exist() )
         if ( actmap->actplayer != i ) {
            int x = 20 + ( num % 2 ) * 200;
            int y = ty2 + 10 + ( num / 2 ) * 20;
            addbutton ( actmap->player[i].getName().c_str(), x, y, x+ 180, y+15, 3, 0, num+3, true );
            addeingabe ( num+3, &to[i], 0, dblue );
            num++;
         }
   }


}



class tnewmessage : public tmessagedlg  {
            protected:
               char* rtitle;
            public:
               tnewmessage( void );
               void init ( void );
               void buttonpressed ( int id );
               void run ( void );
       };

tnewmessage :: tnewmessage ( void )
{
   rtitle = "new message";
}

void tnewmessage :: init ( void )
{
   tdialogbox :: init ( );
   title = rtitle;
   for ( int i = 0; i < 8; i++ ) 
      if ( actmap->player[i].exist() &&  actmap->actplayer != i )
         to[i] = 1;
      else
         to[i] = 0;

   setup();
   buildgraphics();


   rectangle ( x1 + tx1 - 2, y1 + ty1 - 2, x1 + tx2 + 2, y1 + ty2 + 2 ,black );


   activefontsettings.font = schriften.smallarial;
   activefontsettings.length = tx2 - tx1 -1 ;
   activefontsettings.height = activefontsettings.font->height+5;
   activefontsettings.background = dblue;
   activefontsettings.color = black;
   activefontsettings.justify = lefttext;

   inserttext ( "" );
   actparagraph = firstparagraph;
   actparagraph->cursor = 0;
   actparagraph->setpos( x1 + tx1, y1 + ty1, y1 + ty2, 0, 13 );

   while ( actparagraph ) {
      actparagraph->reflow( 0 );
      actparagraph = actparagraph->next;
   }
   actparagraph = firstparagraph;
   actparagraph->checkscrolldown();
   actparagraph->display();
  

}


void tnewmessage :: buttonpressed ( int id )
{
   tmessagedlg :: buttonpressed ( id );
   if ( id == 1 )
      ok = 1;
   if ( id == 2 )
      ok = 2;
}


void tnewmessage :: run ( void )
{
   mousevisible ( true );

   do {

      tmessagedlg::run ( );

   } while ( !ok ); /* enddo */
   if ( ok == 1 ) {
      Message* message = new Message ( extracttext(), actmap, 0, 1 << actmap->actplayer );
      for ( int i = 0; i < 8; i++ ) {
         if ( actmap->player[i].exist() )
            if ( actmap->actplayer != i )
               message->to |= to[i] << i;
      }

      actmap->unsentmessage.push_back ( message );
   }
}

void newmessage ( void )
{
  tnewmessage nm;
  nm.init();
  nm.run();
  nm.done();
}



class teditmessage : public tmessagedlg  {
            protected:
               Message* message;
            public:
               void init ( Message* msg );
               void buttonpressed ( int id );
               void run ( void );
       };


void teditmessage :: init ( Message* msg  )
{
   message = msg;
   tdialogbox :: init ( );
   title = "edit message";

   for ( int i = 0; i < 8; i++ )
      to[i] = (message->to & ( 1 << i )) > 0;

   setup();
   buildgraphics();


   rectangle ( x1 + tx1 - 2, y1 + ty1 - 2, x1 + tx2 + 2, y1 + ty2 + 2 ,black );


   activefontsettings.font = schriften.smallarial;
   activefontsettings.length = tx2 - tx1 -1 ;
   activefontsettings.height = activefontsettings.font->height+5;
   activefontsettings.background = dblue;
   activefontsettings.color = black;
   activefontsettings.justify = lefttext;

   inserttext ( msg->text.c_str() );
   actparagraph = firstparagraph;
   actparagraph->cursor = 0;
   actparagraph->setpos( x1 + tx1, y1 + ty1, y1 + ty2, 0, 13 );

   while ( actparagraph ) {
      actparagraph->reflow( 0 );
      actparagraph = actparagraph->next;
   }
   actparagraph = firstparagraph;
   actparagraph->checkscrolldown();
   actparagraph->display();
  

}


void teditmessage :: buttonpressed ( int id )
{
   tmessagedlg :: buttonpressed ( id );
   if ( id == 1 )
      ok = 1;
   if ( id == 2 )
      ok = 2;
}


void teditmessage :: run ( void )
{
   mousevisible ( true );

   do {
      tmessagedlg::run ( );
   } while ( !ok ); /* enddo */

   if ( ok == 1 ) {
      message->text = extracttext();
      message->to = 0;
      for ( int i = 0; i < 8; i++ ) {
         if ( actmap->player[i].exist() )
            if ( actmap->actplayer != i )
               message->to |= to[i] << i;
      }
   }
}


void editmessage ( Message& msg )
{
  teditmessage nm;
  nm.init( &msg );
  nm.run();
  nm.done();
}

class tviewmessages : public tdialogbox {
               typedef vector<Message*> MsgVec;
               MsgVec msg;
               MsgVec::iterator firstdisplayed;
               MsgVec::iterator marked;
               int player[8];
               int mode;
               int ok;
               int dispnum;
               int scrollpos;
               int __num;

            public:
               tviewmessages ( const MessagePntrContainer& _msglist );
               void init ( char* ttl, bool editable, int md  );    // mode : 0 verschickte ; 1 empfangene
               int  getcapabilities ( void ) { return 1; };
               void redraw ( void );
               void run ( void );
               void buttonpressed ( int id );
               void paintmessages ( void );
               void checkforscroll ( void );
          };


tviewmessages :: tviewmessages ( const MessagePntrContainer& msglist )
{
   msg.insert ( msg.end(), msglist.begin(), msglist.end());
   ok = 0;
   dispnum = 10;
   marked = msg.end();
   firstdisplayed = msg.begin();
   __num = msg.size();
}

void tviewmessages :: init ( char* ttl, bool editable, int md )
{
   mode = md;
   tdialogbox :: init ( );

   xsize =  400;
   ysize =  300;
   title = ttl;
   windowstyle &= ~dlg_in3d;
   if ( editable ) {
     addbutton ( "~V~iew",    10,              ysize - 30, xsize / 3 - 5, ysize - 10 ,   0, 1, 1, msg.size() > 0 );
     addkey    ( 1, ct_enter );

     addbutton ( "~E~dit",    xsize / 3 + 5,   ysize - 30, 2*xsize / 3 - 5, ysize - 10 , 0, 1, 2, msg.size() > 0 );

     addbutton ( "e~x~it",  2*xsize / 3 + 5, ysize - 30, xsize - 10, ysize - 10 ,      0, 1, 3, true );
     addkey    ( 3, ct_esc );
   } else {
     addbutton ( "~V~iew",    10,            ysize - 30, xsize / 2 - 5, ysize - 10 ,   0, 1, 1, msg.size() > 0  );
     addkey    ( 1, ct_enter );

     addbutton ( "e~x~it",  xsize / 2 + 5, ysize - 30, xsize - 10, ysize - 10 ,      0, 1, 3, true );
     addkey    ( 3, ct_esc );
   }

   if ( msg.size() > dispnum )
     addscrollbar ( xsize - 25, starty, xsize - 10, ysize - 40 , &__num, dispnum, &scrollpos, 4, 0 );

   int b = 0;

   for ( int i = 0; i < 8; i++ )
      if ( actmap->player[i].exist()  &&  actmap->actplayer != i )
         player[i] = b++;
      else
         player[i] = -1;

   buildgraphics();

}

void tviewmessages :: buttonpressed ( int id )
{
   if ( id == 1 )
      if ( marked != msg.end() )
         viewmessage ( **marked );

   if ( id == 2 )
      if ( marked != msg.end() )
         editmessage ( **marked );

   if ( id == 3 )
      ok = 1;

   if ( id == 4 )
      paintmessages();
}

void tviewmessages :: paintmessages ( void )
{
       setinvisiblemouserectanglestk ( x1 + 10, y1 + starty, x1 + xsize - 30, y1 + ysize - 40 );

       MsgVec::iterator a = firstdisplayed;
       activefontsettings.font = schriften.smallarial;
       activefontsettings.background = dblue;
       activefontsettings.justify = lefttext;
       int displayed = 0;
       while ( a != msg.end()  &&  displayed < dispnum ) {
          if ( a == marked )
             activefontsettings.color= white;
          else
             activefontsettings.color= black;

          activefontsettings.length = 190;

          tm *tmbuf;
          tmbuf = localtime ( &( (*a)->time ) );
          int y = y1 + starty + 10 + ( a - firstdisplayed ) * 20 ;

          showtext2 (asctime (tmbuf), x1 + 20, y);

          activefontsettings.length = 100;
          if ( mode ) {
             int fr = log2 ( (*a)->from );
             if ( fr < 8 )
                showtext2 ( actmap->player[ fr ].getName().c_str(), x1 + 220, y );
             else
                showtext2 ( "system", x1 + 220, y );
          } else {
             for ( int i = 0; i < 8; i++ )
                if ( player[i] >= 0 ) {
                   int x = x1 + 220 + player[i] * 15;
                   int color;
                   if ( (*a)->to & ( 1 << i ) )
                      color = 20 + i * 8;
                   else
                      color = dblue ;

                   bar ( x, y, x + 10, y + 10, color );

                }
          }

          a++;
          displayed++;
       }

       getinvisiblemouserectanglestk();
}

void tviewmessages :: checkforscroll ( void )
{
   firstdisplayed = msg.begin() + scrollpos;
   if ( firstdisplayed >= msg.end() )
      displaymessage (" tviewmessages :: checkforscroll  -- invalid scrolling operation ", 2 );

   MsgVec::iterator oldfirst = firstdisplayed;
   if ( marked < firstdisplayed )
      firstdisplayed = marked;
   if ( marked > firstdisplayed + (dispnum-1) )
      firstdisplayed = marked - (dispnum-1) ;

   if ( oldfirst != firstdisplayed )
      if ( msg.size() > dispnum )
         enablebutton ( 4 );
}


void tviewmessages :: redraw ( void )
{
   tdialogbox::redraw();
   if ( msg.size() > dispnum )
     rahmen ( true, x1 + 10, y1 + starty, x1 + xsize - 30, y1 + ysize - 40 );
   else
     rahmen ( true, x1 + 10, y1 + starty, x1 + xsize - 10, y1 + ysize - 40 );

   paintmessages();
}

void tviewmessages :: run ( void )
{
   int mstatus = 0;

   mousevisible( true );
   do {
      tdialogbox :: run ( );
         if ( taste == ct_down  &&  marked+1 < msg.end() ) {
            marked++;
            checkforscroll();
            paintmessages();
         }
         if ( taste == ct_up  &&  marked > msg.begin() ) {
            marked--;
            checkforscroll();
            paintmessages();
         }
         if ( taste == ct_pos1  && marked > msg.begin() ) {
            marked=msg.begin();
            checkforscroll();
            paintmessages();
         }
         if ( taste == ct_ende  &&  marked+1 < msg.end() ) {
            marked = msg.end()-1;
            checkforscroll();
            paintmessages();
         }

         if ( mouseinrect ( x1 + 10, y1 + starty, x1 + xsize - 30, y1 + ysize - 40 )) {
            if ( mouseparams.taste == 1 ) {
                int ps = (mouseparams.y - (y1 + starty + 10)) / 20;
                if ( ps < dispnum && ps >= 0 ) {
                   MsgVec::iterator n = firstdisplayed + ps;
                   if ( n < msg.end() ) {
                      if ( n != marked ) {
                         marked = n;
                         paintmessages();
                         mstatus = 1;
                      }
                      if ( n == marked  &&  mstatus == 2 ) {
                         buttonpressed ( 1 );
                         mstatus = 0;
                      }
                   }
                }
            }

         }
         if ( mouseparams.taste == 0  && mstatus == 1 )
            mstatus = 2;

   } while ( !ok ); /* enddo */

}


void viewmessages ( char* title, const MessagePntrContainer& msg, bool editable, int md  )    // mode : 0 verschickte ; 1 empfangene
{
   tviewmessages vm ( msg );
   vm.init  ( title, editable, md );
   vm.run ();
   vm.done();
}








  class   tviewmessage : public tdialogbox, public tviewtextwithscrolling {
               public:
                   const char*          txt;
                   char                 ok;

                   char                 scrollbarvisible;
                   char                 action;
                   int                  textstart;
                   int                  from;
                   int                  cc;
                   int                  rightspace;

                   void                 init( const Message& msg );
                   virtual void         run ( void );
                   virtual void         buttonpressed( int id);
                   void                 redraw ( void );
                   int                  getcapabilities ( void ) { return 1; };
                   void                 repaintscrollbar( void );
                };



void         tviewmessage::init( const Message& msg )
{

   // dialogbox initialisieren

   tdialogbox::init();
   x1 = 20;
   y1 = 50;
   xsize = 500;
   ysize = 360;
   textstart = 42 + 45;
   textsizey = (ysize - textstart - 35);
   starty = starty + 10;
   title = "message";
   windowstyle ^= dlg_in3d;
   action=0;


   txt = msg.text.c_str();
   from = msg.from;
   cc = msg.to;


   setparams ( x1 + 13, y1 + textstart, x1 + xsize - 41, y1 + ysize - 35, txt, black, dblue);
   tvt_dispactive = 0;
   displaytext(  );
   textsizeycomplete = tvt_yp;
   tvt_dispactive = 1;


   if (textsizeycomplete >= textsizey) {
      scrollbarvisible = true;

      #ifdef NEWKEYB
      addscrollbar(xsize - 30,starty,xsize - 15,ysize - 35,&textsizeycomplete, textsizey, &tvt_starty,1,0);
      #else
      addscrollbar(xsize - 30,starty,xsize - 15,ysize - 35,&textsizeycomplete, textsizey, &tvt_starty,1,1);
      #endif
      setscrollspeed ( 1 , 1 );

      rightspace = 40;
   }
   else {
      rightspace = 10;
      scrollbarvisible = false;
   }

   addbutton ( "e~x~it", 10, ysize - 25, xsize - 10, ysize - 5, 0, 1, 4, true );

   tvt_starty = 0; 
   buildgraphics();
} 


void         tviewmessage::repaintscrollbar( void )
{
   enablebutton ( 1 );
}


void         tviewmessage::redraw(void)
{ 
  int i;

   tdialogbox::redraw();
   rahmen(true,x1 + 10,y1 + textstart - 2,x1 + xsize - rightspace,y1 + textstart + textsizey + 2); 


   activefontsettings.font = schriften.smallarial; 
   activefontsettings.color = black; 
   activefontsettings.justify = lefttext; 
   activefontsettings.background = dblue;
   activefontsettings.length =0;

   int yp = 36;
   if ( from != 1 << actmap->actplayer ) {
      showtext2 ( "from: ", x1 + 13, y1 + textstart - 45 ); 
      int n = log2 ( from );
      if ( n < 8 ) {
         activefontsettings.color = 20 + 8 * n;
         showtext2 ( actmap->player[n].getName().c_str(), x1 + 60, y1 + textstart - 45 );
      } else
         showtext2 ( "system", x1 + 60, y1 + textstart - 45 ); 

      yp = 26;
   }

   activefontsettings.color = black; 
   if ( from != 1 << actmap->actplayer ) 
      showtext2 ( "cc: ", x1 + 13, y1 + textstart - yp );
   else
      showtext2 ( "to: ", x1 + 13, y1 + textstart - yp ); 

   int n = 0;
   for (i = 0; i < 8; i++ )
      if ( actmap->player[i].exist()  && i != actmap->actplayer )
         if ( cc & ( 1 << i ))
            n++;
   if ( n ) {
      activefontsettings.length = (xsize - (n+1) * 5 - 60 - rightspace ) / n;
   
      n = 0;
      for ( i = 0; i < 8; i++ )
         if ( actmap->player[i].exist()  && i != actmap->actplayer )
            if ( cc & ( 1 << i )) {
               activefontsettings.color = 20 + 8 * i;
               //activefontsettings.background = 17 + 8 * i;
               showtext2 ( actmap->player[i].getName().c_str(), x1 + 60 + n * activefontsettings.length + n * 5 , y1 + textstart - yp );
               n++;
            }
   }
   activefontsettings.color = black; 
   activefontsettings.height = textsizey / linesshown; 

   rahmen(true,x1 + 10,y1 + textstart - 47,x1 + xsize - rightspace,y1 + textstart - 7 ); 

   int xd = 15 - (rightspace - 10) / 2;

   setpos ( x1 + 13 + xd, y1 + textstart, x1 + xsize - 41 + xd, y1 + ysize - 35 );
   displaytext(); 

} 


void         tviewmessage::buttonpressed( int id )
{
   tdialogbox::buttonpressed(id);
   if (id == 1) 
      displaytext();

   if (id == 4)
      action = 12;
} 


void         tviewmessage::run(void)
{ 
   mousevisible(true);
   do { 
      tdialogbox::run();
      checkscrolling ( );
      if (taste == ct_esc || taste == ct_enter || taste == ct_space ) 
         action = 11; 
   }  while (action < 10);
}



void viewmessage ( const Message& message )
{
   tviewmessage vm;
   vm.init ( message );
   vm.run();
   vm.done();
}

void viewjournal ( void )
{
   if ( actmap->journal ) {
      tviewanytext vat;
      vat.init ( "journal", actmap->journal );
      vat.run();
      vat.done();
   }
}

class teditjournal : public tmessagedlg  {
            public:
               void init ( void );
               void setup ( void );
               void buttonpressed ( int id );
               void run ( void );
       };


void teditjournal :: init ( void )
{
   tdialogbox :: init ( );
   title = "edit journal";

   setup();
   buildgraphics();


   rectangle ( x1 + tx1 - 2, y1 + ty1 - 2, x1 + tx2 + 2, y1 + ty2 + 2 ,black );


   activefontsettings.font = schriften.smallarial;
   activefontsettings.length = tx2 - tx1 -1 ;
   activefontsettings.height = activefontsettings.font->height+5;
   activefontsettings.background = dblue;
   activefontsettings.color = black;
   activefontsettings.justify = lefttext;

   inserttext ( actmap->newjournal ? actmap->newjournal : "" );
   actparagraph = firstparagraph;
   actparagraph->cursor = 0;
   actparagraph->setpos( x1 + tx1, y1 + ty1, y1 + ty2, 0, 13 );

   while ( actparagraph ) {
      actparagraph->reflow( 0 );
      actparagraph = actparagraph->next;
   }
   actparagraph = firstparagraph;
   actparagraph->checkscrolldown();
   actparagraph->display();
  

}


void teditjournal :: setup ( void )
{
   xsize = 500;
   ysize = 400;
   ok = 0;
   tx1 = 20;
   ty1 = starty + 10;
   tx2 = xsize - 40;
   ty2 = ysize - ty1 - 40;

   addbutton ( "~S~end", 10, ysize - 30, xsize / 2 - 5, ysize - 10 , 0, 1, 1, true );
   clearkey ( 1 );
   addkey ( 1, cto_stp + cto_s );

   addbutton ( "~C~ancel", xsize / 2 + 5, ysize - 30, xsize - 10, ysize - 10 , 0, 1, 2, true );
   clearkey ( 2 );
   addkey ( 2, cto_stp + cto_c );

}


void teditjournal :: buttonpressed ( int id )
{
   tmessagedlg :: buttonpressed ( id );
   if ( id == 1 )
      ok = 1;
   if ( id == 2 )
      ok = 2;
}


void teditjournal :: run ( void )
{

   mousevisible ( true );

   do {

      tmessagedlg::run ( );

   } while ( !ok ); /* enddo */
   if ( ok == 1 ) {
      ASCString c = extracttext();
      if ( actmap->newjournal )
         delete[] actmap->newjournal;
      actmap->newjournal = strdup ( c.c_str() );

      actmap->lastjournalchange.a.turn = actmap->time.a.turn;
      actmap->lastjournalchange.a.move = actmap->actplayer;
   }
}


void editjournal ( void )
{
      teditjournal ej;
      ej.init ();
      ej.run ();
      ej.done ();
}

tonlinemousehelp :: tonlinemousehelp ( void )
{
   active = 1;
   image = NULL;
   lastmousemove = ticker;
   addmouseproc ( this );
}


tonlinemousehelp :: ~tonlinemousehelp (  )
{
   if ( active == 2 )
      removehelp();

   removemouseproc ( this );
}


void tonlinemousehelp :: mouseaction ( void )
{
   lastmousemove = ticker;
  #ifdef _DOS_
   if ( active == 2 )
      removehelp();
  #endif
}

#define maxonlinehelplinenum 5

void tonlinemousehelp :: displayhelp ( int messagenum )
{
   int i;
   if ( active == 1 ) {
      char* str = getmessage ( messagenum );
      if ( str ) {

         char strarr[maxonlinehelplinenum][10000];
         for (i = 0; i < maxonlinehelplinenum; i++ )
            strarr[i][0] = 0;

         i = 0;
         int actlinenum = 0;
         int actlinepos = 0;
         while ( str[i] ) {
            if ( str[i] != '$' )
               strarr[actlinenum][actlinepos++] = str[i];
            else {
               strarr[actlinenum][actlinepos] = 0;
               actlinenum++;
               actlinepos = 0;
            }
            i++;
         } /* endwhile */
         strarr[actlinenum][actlinepos] = 0;


         int width = 0;
         for ( i = 0; i <= actlinenum; i++ ) {
            int j = gettextwdth ( strarr[i], schriften.guifont );
            if ( j > width )
               width = j;
         }
        #ifdef _DOS_
         pos.x1 = mouseparams.x - width/2 - 2;
         pos.x2 = mouseparams.x + width/2 + 2;
        #else
         pos.x1 = mouseparams.x - width - 2;
         pos.x2 = mouseparams.x + 2;
        #endif

         if ( pos.x1 < 0 ) {
            pos.x1 = 0;
            pos.x2 = width + 4;
         }
         if ( pos.x2 >= agmp->resolutionx ) {
            pos.x2 = agmp->resolutionx - 1;
            pos.x1 = pos.x2 - width - 4;
         }

         int height = ( actlinenum+1) * (schriften.guifont->height+4) + 2;
         pos.y1 = mouseparams.y + mouseparams.ysize;
         pos.y2 = pos.y1 + height;

         if ( pos.y1 < 0 ) {
            pos.y1 = 0;
            pos.y2 = height;
         }
         if ( pos.y2 >= agmp->resolutiony ) {
            pos.y2 = agmp->resolutiony - 1;
            pos.y1 = pos.y2 - height;
         }



         if ( !rectinoffarea() ) {
            active = 2;
            image = new char[imagesize ( pos.x1, pos.y1, pos.x2, pos.y2 )];
            getimage ( pos.x1, pos.y1, pos.x2, pos.y2, image );
            bar ( pos.x1, pos.y1, pos.x2, pos.y2, black );
            npush ( activefontsettings );

            activefontsettings.font = schriften.guifont;
            activefontsettings.background = 255;
            activefontsettings.justify = centertext;
            activefontsettings.length = pos.x2 - pos.x1;

            for ( i = 0; i <= actlinenum; i++ )
              showtext2c ( strarr[i], pos.x1, pos.y1 + 3 + i * (schriften.guifont->height+4));

            rectangle ( pos.x1, pos.y1, pos.x2, pos.y2, white );

            npop ( activefontsettings );
         }
      }
   }
}

void tonlinemousehelp :: removehelp ( void )
{
   if ( active == 2 ) {
      putimage ( pos.x1, pos.y1, image );
      delete[] image;
      active = 1;
   }
   lastmousemove = ticker;
}

void tonlinemousehelp :: checklist ( tonlinehelplist* list )
{
     int mbut = mouseparams.taste;
     for ( int i = 0; i < list->num; i++ )
        if ( mouseinrect ( &list->item[i].rect )) {
          #ifdef _DOS_
           displayhelp( list->item[i].messagenum );
          #else
           displayhelp( list->item[i].messagenum );
           while ( mouseinrect ( &list->item[i].rect ) && mouseparams.taste == mbut && !keypress())
              releasetimeslice();
           removehelp();
          #endif
        }

}


void tonlinemousehelp :: checkforhelp ( void )
{
        if ( CGameOptions::Instance()->onlinehelptime ) 
      if ( ticker > lastmousemove+CGameOptions::Instance()->onlinehelptime )
         if ( active == 1 )
            if ( mouseparams.taste == 0 )
               checklist ( &helplist );
}

int  tonlinemousehelp :: rectinoffarea ( void )
{
   if ( offarea.x1 == -1     ||   offarea.y1 == -1 )
      return 0;
   else
      return ( pos.x2 >= offarea.x1  &&  
               pos.y2 >= offarea.y1  &&   
               pos.x1 <= offarea.x2  &&   
               pos.y1 <= offarea.y2 ) ;
}


void tonlinemousehelp :: invisiblerect ( tmouserect newoffarea )
{
   tsubmousehandler :: invisiblerect ( newoffarea ); 

   if ( rectinoffarea() && active == 2 )
      removehelp();
}





const char* smallguiiconsundermouse[3] = { "never", "always", "units, buildings, marked fields" };
const char* mousekeynames[9] = { "none", "left", "right", "left + right", "center", "center + left", "center + right", "center + left + right", "disabled"};

class tgamepreferences : public tdialogbox {
                        CGameOptions actoptions;
                        int status;
                        tmouserect r1, r5, r6, r7, r8;
                        tmouserect dlgoffset;
                        char actgamepath[200];

                     public:
                        char checkvalue( int id, void* p)  ;
                        void init ( void );
                        void buttonpressed ( int id );
                        void run ( void );
                        tgamepreferences ( )
                                                        : actoptions ( *CGameOptions::Instance() ) {};
                    };


void tgamepreferences :: init ( void )
{
   tdialogbox::init();
   title = "options";

   xsize = 600;
   ysize = 460;

   x1 = -1;
   y1 = -1;

   addbutton ( "~O~K", 10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1, 1, true );
   addkey ( 1, ct_enter );

   addbutton ( "~C~ancel", xsize / 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1, 2, true );
   addkey ( 2, ct_esc );

   addbutton ( "", xsize -35, starty + 20, xsize - 20, starty + 35, 3, 0, 3, true );
   addeingabe ( 3, &actoptions.container.autoproduceammunition, 0, dblue );

#ifndef _DOS_
   addbutton ( "", xsize -35, starty + 50, xsize - 20, starty + 65, 3, 0, 4, true );
   addeingabe ( 4, &actoptions.sound_mute, 0, dblue );
#endif


/*
   r1.x1 = xsize - 150;
   r1.y1 = starty + 45;
   r1.x2 = xsize - 20;
   r1.y2 = starty + 65;
   addbutton ( "", r1, 11, 0, 4, true  );


   r5.x1 = xsize - 150;
   r5.y1 = starty + 75;
   r5.x2 = xsize - 20;
   r5.y2 = starty + 95;
   addbutton ( "", r5, 11, 0, 5, true  );
   

   r6.x1 = xsize - 150;
   r6.y1 = starty + 105;
   r6.x2 = xsize - 20;
   r6.y2 = starty + 125;
   addbutton ( "", r6, 11, 0, 6, true  );


   r7.x1 = xsize - 150;
   r7.y1 = starty + 135;
   r7.x2 = xsize - 20;
   r7.y2 = starty + 155;
   addbutton ( "", r7, 11, 0, 7, true  );
*/

   r8.x1 = xsize - 200;
   r8.y1 = starty + 165;
   r8.x2 = xsize - 20;
   r8.y2 = starty + 185;
//   addbutton ( "", r8, 11, 0, 8, true  );

   addbutton ( "", r8.x1, r8.y1 + 30, r8.x2, r8.y2 + 30, 2, 1, 9, true );
   addeingabe ( 9, &actoptions.onlinehelptime , 0, 10000 );

   addbutton ( "delay1", r8.x1, r8.y1 , r8.x1 + (r8.x2 - r8.x1)/3 - 2, r8.y2, 2, 1, 91, true );
   addeingabe ( 91, &actoptions.attackspeed1 , 0, 10000 );
   addbutton ( "anim",   r8.x1 + (r8.x2 - r8.x1)/3 + 2, r8.y1 , r8.x1 + 2*(r8.x2 - r8.x1)/3 - 2, r8.y2, 2, 1, 92, true );
   addeingabe ( 92, &actoptions.attackspeed2 , 0, 10000 );
   addbutton ( "delay2", r8.x1 + 2*(r8.x2 - r8.x1)/3 + 2, r8.y1 , r8.x2, r8.y2, 2, 1, 93, true );
   addeingabe ( 93, &actoptions.attackspeed3 , 0, 10000 );


/*
   addbutton ( "", xsize -35, r8.y1 + 60, r8.x2, r8.y2 + 60, 3, 0, 10, true );
   addeingabe ( 10, &actoptions.smallguiiconopenaftermove , 0, dblue  );
*/

   addbutton ( "~E~nter default password", 25 , r8.y1 + 90, xsize - 20, r8.y2 + 90, 0, 1, 11, true );

/*
   addbutton ( "", xsize - 150 , r8.y1 + 120, xsize - 20, r8.y2 + 120, 1, 0, 12, true );
   addeingabe ( 12, actgamepath, 0, 100 );
*/

   addbutton ( "", r8.x1, r8.y1 + 120, r8.x2, r8.y2 + 120, 2, 1, 20, true );
   addeingabe ( 20, &actoptions.movespeed , 0, 10000 );


   addbutton ( "", xsize - 35 , r8.y1 + 150, r8.x2, r8.y2 + 150, 3, 0, 13, true );
   addeingabe ( 13, &actoptions.endturnquestion, black, dblue );

   buildgraphics(); 

   activefontsettings.font = schriften.smallarial; 
   activefontsettings.justify = lefttext; 
   activefontsettings.length = 0;
   activefontsettings.background = 255;

   showtext2 ( "automatic ammunition production in buildings", x1 + 25, y1 + starty + 20 );
#ifndef _DOS_
   showtext2 ( "disable sound",                                 x1 + 25, y1 + starty + 50 );
#endif

   dlgoffset.x1 = x1;
   dlgoffset.y1 = y1;
   dlgoffset.x2 = x1;
   dlgoffset.y2 = y1;

/*
   showtext2 ( "small gui icons below mouse pointer", x1 + 25, y1 + r1.y1 );
   rahmen ( true, r1 + dlgoffset );
   paintbutt( 4 );
   
   showtext2 ( "mouse key for small gui icons", x1 + 25, y1 + r5.y1 );
   rahmen ( true, r5 + dlgoffset );
   paintbutt( 5 );

   showtext2 ( "mouse key to mark fields", x1 + 25, y1 + r6.y1 );
   rahmen ( true, r6 + dlgoffset );
   paintbutt( 6 );

   showtext2 ( "mouse key to scroll", x1 + 25, y1 + r7.y1 );
   rahmen ( true, r7 + dlgoffset );
   paintbutt( 7 );

   showtext2 ( "mouse key to center screen", x1 + 25, y1 + r8.y1 );
   rahmen ( true, r8 + dlgoffset );
   paintbutt( 8 );
*/

   showtext2 ( "mousetip help delay (in 1/100 sec)", x1 + 25, y1 + r8.y1 + 30);

   showtext2 ( "speed of attack display (in 1/100 sec)",          x1 + 25, y1 + r8.y1 );

//   showtext2 ( "mousegui menu opens after movement", x1 + 25, y1 + r8.y1 + 60);

//   showtext2 ( "game path", x1 + 25, y1 + r8.y1 + 120);
   showtext2 ( "movement speed in 1/100 sec", x1 + 25, y1 + r8.y1 + 120);

   showtext2 ( "prompt for end of turn", x1 + 25, y1 + r8.y1 + 150 );
   status = 0;


}


char tgamepreferences :: checkvalue( int id, void* p)  
{
   if ( id != 12 )
      return true;
   else {
      char* cp = (char*) p;
      if ( !cp[0] )
         return 1;
      else {
         char temp[200];
         strcpy ( temp, cp );
         if ( temp[0] )
            if ( temp[strlen(temp)-1] != '\\' )
               strcat ( temp, "\\" );
         return checkforvaliddirectory ( temp );
      }
  }
}


void tgamepreferences :: buttonpressed ( int id )
{
   tdialogbox :: buttonpressed ( id );

   if ( id == 1 ) {
      actoptions.container.filleverything = actoptions.container.autoproduceammunition;
      CGameOptions::Instance()->copy ( actoptions );
      status = 10;
   }

   if ( id == 2 ) 
      status = 10;
/*
   if ( id == 4 ) {
      actoptions.mouse.smalliconundermouse++;
      if ( actoptions.mouse.smalliconundermouse > 2 )
         actoptions.mouse.smalliconundermouse = 0;
      paintbutt ( id );
   }

   if ( id >= 5  &&  id <= 7) {
      int* pi;
      if ( id == 5 ) 
        pi = &actoptions.mouse.smallguibutton;
      
      if ( id == 6 ) 
        pi = &actoptions.mouse.fieldmarkbutton;
      
      if ( id == 7 ) 
        pi = &actoptions.mouse.scrollbutton;

      if ( *pi )
         *pi *= 2;
      else
         *pi = 1;

      if ( *pi > 8 ) {
         if ( id == 7 )
            *pi = 0;
         else
            *pi = 1;
      }

      paintbutt ( id );
   }

   if ( id == 8 ) {
      actoptions.mouse.centerbutton++;
      if ( actoptions.mouse.centerbutton > 8 )
         actoptions.mouse.centerbutton = 0;
      paintbutt( 8 );
   }
*/

   if ( id == 11 ) {
      Password pwd = actoptions.getDefaultPassword();
      bool success = enterpassword ( pwd, true, true, false );
      if ( success )
         actoptions.defaultPassword.setName ( pwd.toString().c_str() );
   }
}


void tgamepreferences :: run ( void )
{
   mousevisible ( true );
   do {
      tdialogbox::run();
   } while ( status < 10 ); /* enddo */
}

void gamepreferences  ( void )
{
   int oldSoundStat = CGameOptions::Instance()->sound_mute;
   tgamepreferences prefs;
   prefs.init();
   prefs.run();
   prefs.done();
   if ( oldSoundStat != CGameOptions::Instance()->sound_mute )
      SoundSystem::getInstance()->setMute( CGameOptions::Instance()->sound_mute );
}


const int mousebuttonnum = 3;

const char* mousebuttonnames[mousebuttonnum] = { "left", "right", "center" };


class tmousepreferences : public tdialogbox {
                        CGameOptions actoptions;
                        int status;
                        tmouserect r1, r2, ydelta;
                        tmouserect dlgoffset;
                        char actgamepath[200];
                        int mouseActionNum;

                        struct MouseAction {
                           char* name;
                           int unallowMouseButtons;
                           int singleButton;
                           int* optionAddress;
                           int checkBoxStatus[3];
                        };
                        dynamic_array<MouseAction> mouseAction;

                     public:
                        void init ( void );
                        void buttonpressed ( int id );
                        void paintbutt ( char id );
                        void run ( void );
                        tmousepreferences ( ) : actoptions ( *CGameOptions::Instance()) {};
                    };


void tmousepreferences :: init ( void )
{
   mouseActionNum = 5;

   tdialogbox::init();

   xsize = 600;
   ysize = 460;

   title = "mouse options";

   mouseAction[0].name = "mouse button for small gui icons";
   mouseAction[0].unallowMouseButtons = 0;
   mouseAction[0].singleButton = 0;
   mouseAction[0].optionAddress = &actoptions.mouse.smallguibutton;


   mouseAction[1].name = "mouse button to set the cursor on a field";
   mouseAction[1].unallowMouseButtons = 0;
   mouseAction[1].singleButton = 0;
   mouseAction[1].optionAddress = &actoptions.mouse.fieldmarkbutton;


   mouseAction[2].name = "mouse button to scroll the map";
   mouseAction[2].unallowMouseButtons = 0;
   mouseAction[2].singleButton = 0;
   mouseAction[2].optionAddress = &actoptions.mouse.scrollbutton;


   mouseAction[3].name = "mouse button to center screen on field";
   mouseAction[3].unallowMouseButtons = 0;
   mouseAction[3].singleButton = 0;
   mouseAction[3].optionAddress = &actoptions.mouse.centerbutton;


   mouseAction[4].name = "mouse button to show unit weap information";
   mouseAction[4].unallowMouseButtons = 0;
   mouseAction[4].singleButton = 0;
   mouseAction[4].optionAddress = &actoptions.mouse.unitweaponinfo;

/*
   mouseAction[5].name = "mouse button for drag'n'drop unit movement";
   mouseAction[5].unallowMouseButtons = 0;
   mouseAction[5].singleButton = 0;
   mouseAction[5].optionAddress = &actoptions.mouse.dragndropmovement;
*/

   int lastypos;
   for ( int i = 0; i < mouseActionNum; i++ ) {
       for ( int j = 0; j < mousebuttonnum; j++ ) {
          mouseAction[i].checkBoxStatus[j] = (*mouseAction[i].optionAddress >> j ) & 1;
          addbutton ( "", xsize - (mousebuttonnum - j ) * 40,      starty + 45 + i * 30, 
                          xsize - (mousebuttonnum - j ) * 40 + 15, starty + 60 + i * 30, 3, 0, 100 + i * 10 + j, true );
          addeingabe ( 100 + i * 10 + j, &mouseAction[i].checkBoxStatus[j], 0, dblue  );
       }
       lastypos = starty + 45 + (i+1) * 30;
   }


   x1 = -1;
   y1 = -1;


   addbutton ( "~O~K", 10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1, 1, true );
   addkey ( 1, ct_enter );

   addbutton ( "~C~ancel", xsize / 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1, 2, true );
   addkey ( 2, ct_esc );


   ydelta.x1 = 0;
   ydelta.y1 = 30;
   ydelta.x2 = 0;
   ydelta.y2 = 30;

   r1.x1 = xsize - 150;
   r1.y1 = lastypos;
   r1.x2 = xsize - 20;
   r1.y2 = r1.y1 + 15;


   addbutton ( "", r1, 11, 0, 4, true  );

   r2 = r1 + ydelta;

   addbutton ( "", xsize -35, r2.y1 , r2.x2, r2.y2, 3, 0, 10, true );
   addeingabe ( 10, &actoptions.smallguiiconopenaftermove , 0, dblue  );


   buildgraphics(); 

   dlgoffset.x1 = x1;
   dlgoffset.y1 = y1;
   dlgoffset.x2 = x1;
   dlgoffset.y2 = y1;

   activefontsettings.font = schriften.smallarial; 
   activefontsettings.justify = centertext; 
   activefontsettings.length = 40;
   activefontsettings.background = 255;

   for ( int m = 0; m < mousebuttonnum; m++ )
       showtext2 ( mousebuttonnames[m], dlgoffset.x1 + xsize - (mousebuttonnum - m ) * 40 - 13,  dlgoffset.y1 + starty + 25);

   activefontsettings.justify = lefttext; 
   activefontsettings.length = 0;

   for ( int j = 0; j < mouseActionNum; j++ ) 
       showtext2 ( mouseAction[j].name, x1 + 25, dlgoffset.y1 + starty + 45 + j * 30 );


   showtext2 ( "small gui icons below mouse pointer", x1 + 25, r1.y1 + dlgoffset.y1 );
   rahmen ( true, r1 + dlgoffset );
   paintbutt( 4 );
   
   showtext2 ( "mousegui menu opens after movement",  x1 + 25, r2.y1 + dlgoffset.y1 );

   status = 0;
}



void tmousepreferences :: paintbutt ( char id )
{
   if ( id == 4 ) {
      tmouserect r2 = r1 + dlgoffset;

      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial; 
      activefontsettings.justify = centertext; 
      activefontsettings.length = r2.x2 - r2.x1 - 2;
      activefontsettings.background = dblue;
      setinvisiblemouserectanglestk( r2 );
      showtext2 ( smallguiiconsundermouse[actoptions.mouse.smalliconundermouse], r2.x1 + 1, r2.y1 + 1 );
      getinvisiblemouserectanglestk();
      npop ( activefontsettings );
   }
}


void tmousepreferences :: buttonpressed ( int id )
{
   tdialogbox :: buttonpressed ( id );

   if ( id == 1 ) {
      CGameOptions::Instance()->copy ( actoptions );
      status = 10;
   }

   if ( id == 2 ) 
      status = 10;

   if ( id == 4 ) {
      actoptions.mouse.smalliconundermouse++;
      if ( actoptions.mouse.smalliconundermouse > 2 )
         actoptions.mouse.smalliconundermouse = 0;
      paintbutt ( id );
   }

   for ( int i = 0; i < mouseActionNum; i++ ) 
       for ( int j = 0; j < mousebuttonnum; j++ ) 
          if ( id == 100 + i * 10 + j ) 
             if ( mouseAction[i].checkBoxStatus[j] )
                *mouseAction[i].optionAddress |= 1 << j;
             else
                *mouseAction[i].optionAddress &= ~(1 << j);
}


void tmousepreferences :: run ( void )
{
   mousevisible ( true );
   do {
      tdialogbox::run();
   } while ( status < 10 ); /* enddo */
}

void mousepreferences  ( void )
{
   tmousepreferences prefs;
   prefs.init();
   prefs.run();
   prefs.done();
}


void writeGameParametersToString ( std::string& s)
{
   s = "The game has been set up with the following game parameters:\n";
   s += "(black line: parameter has default value)\n\n";
   for ( int i = 0; i< gameparameternum; i++ ) {
      int d = actmap->getgameparameter(i) != gameparameterdefault[i];

      if ( d )
         s+= "#color4#";

      s += gameparametername[i];
      s += " = ";
      s += strrr ( actmap->getgameparameter(i) );

      if ( d )
         s+= "#color0#";
      s += "\n";
   }
}

void sendGameParameterAsMail ( void )
{
   ASCString s;
   writeGameParametersToString ( s );
   new Message ( s, actmap, 255 );
}


void showGameParameters ( void )
{
/*
   string s;
   writeGameParametersToString ( s );

   tviewanytext vat;
   vat.init ( "Game Parameter", s.c_str(), 50, 50, 550, 400 );
   vat.run();
   vat.done();

   */
}



class tmultiplayersettings : public tdialogbox {
              protected:
                //! the encoded supervisor password
                char svpwd_enc[1000];

                //! the plaintext supervisor password
                char svpwd_plt[1000];

                int techlevel;
                int replays;
                int status;
                int defaultSettings;
                int supervisor;

                void setcoordinates ( void );

              public:
                void init ( void );
                void run ( void );
                void buttonpressed ( int id );

                ~tmultiplayersettings ( );
     };



void tmultiplayersettings :: buttonpressed ( int id )
{
   tdialogbox :: buttonpressed ( id );
   if ( id == 125 )
      setmapparameters();

   if ( id == 1 ) {             // OK
      status = 2;

      if ( techlevel )
         for ( int i = 0; i < 8; i++ )
             actmap->player[i].research.settechlevel ( techlevel );

      if ( replays )
         actmap->replayinfo = new treplayinfo;
      else
         if ( actmap->replayinfo ) {
            delete actmap->replayinfo;
            actmap->replayinfo = NULL;
         }

      if ( supervisor )
         actmap->supervisorpasswordcrc.setEncoded( svpwd_enc );
      else
         actmap->supervisorpasswordcrc.reset();
   }

   if ( id == 2 )
      help(50);

      /*
   if ( id == 4 ) {
       taskforsupervisorpassword afsp;
       afsp.init ( &actmap->supervisorpasswordcrc, 0 );
       afsp.run( NULL );
       afsp.done();
   }
   */

   if ( id == 10 ) {
      Password pwd;
      pwd.setUnencoded ( svpwd_plt );
      strcpy ( svpwd_enc, pwd.toString().c_str() );
      enablebutton ( 11 );
   }

   if ( id == 100 ) {
      if ( !defaultSettings ) {
         enablebutton ( 101 );
         enablebutton ( 3 );
         enablebutton ( 8 );
         enablebutton ( 10 );
         enablebutton ( 11 );
         enablebutton ( 12 );
         // enablebutton ( 13 );
      } else {
         disablebutton ( 101 );
         disablebutton ( 3 );
         disablebutton ( 8 );
         disablebutton ( 10 );
         disablebutton ( 11 );
         disablebutton ( 12 );
         // disablebutton ( 13 );
      }
   }
   if ( id == 12 ) {
      CGameOptions::Instance()->defaultSuperVisorPassword.setName ( svpwd_enc );
      CGameOptions::Instance()->setChanged();
   }
}


void tmultiplayersettings :: setcoordinates ( void )
{
}

void tmultiplayersettings :: init ( void )
{
   techlevel = 0;

   strcpy ( svpwd_enc, CGameOptions::Instance()->getDefaultSupervisorPassword().toString().c_str() );

   tdialogbox::init();
   x1 = 20;
   y1 = 10;
   xsize = 600;
   ysize = 460;
   title = "multiplayer settings";

   replays = 1;
   supervisor = 0;
   defaultSettings = 1;

   windowstyle &= ~dlg_in3d;

   addbutton ( "~O~K",    10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1 , 1, true );
   addkey ( 1, ct_enter );
   addbutton ( "help (~F1~)", xsize/2 + 5, ysize - 35, xsize -10, ysize - 10, 0, 1 , 2, true );
   addkey ( 2, ct_f1 );


   addbutton ( "use ~d~efault settings", 10, 60, xsize-10, 75, 3, 0, 100, true );
   addeingabe ( 100, &defaultSettings, black, dblue );

   addbutton ( "enable ~r~eplays", 15, 120, xsize-15, 135, 3, 0 , 8, false );
   addeingabe ( 8, &replays, black, dblue );

   addbutton ( "start with ~t~echlevel", 15 , 150, 100 , 170, 2, 3 , 3, false );
   addeingabe ( 3, &techlevel, 0, 255 );


   addbutton ( "enable ~s~upervisor", 20, 195, xsize-25, 210, 3, 3 , 101, false );
   addeingabe ( 101, &supervisor, black, dblue );
/*
   addbutton ( "start with ~t~echlevel", 15 , 120, 100 , 140, 2, 1 , 3, false );
   addeingabe ( 3, &techlevel, 0, 255 );
*/

/*
   addbutton ( "use ~c~hecksums", 20, ysize - 80, 160, ysize - 65, 3, 0 , 5, true );
   addeingabe ( 5, &checksumsused, black, dblue );
   disablebutton ( 5 );
*/
   addbutton ( "modify game parameters", 20, ysize - 80, 160, ysize - 65, 0, 1 , 125, true );


   addbutton ( "supervisor password (plain text)", 20 , 220, 150 , 240, 1, 3 , 10, false );
   addeingabe ( 10, &svpwd_plt, 0, 900 );

   addbutton ( "supervisor password (encoded)", 20 , 250, 150 , 270, 1, 3 , 11, false );
   addeingabe ( 11, &svpwd_enc, 0, 900 );

   addbutton ( "~s~ave password as default",  20, 280, xsize / 2 - 5 , 300, 0, 1 , 12, false );

   buildgraphics();

   rahmen3 ( "advanced options", x1 + 10, y1 + 100, x1 + xsize - 10, y1 + ysize - 45, 1  );

   rahmen3 ( "supervisor", x1 + 15, y1 + 185, x1 + xsize - 15, y1 + ysize - 85, 1  );

   status = 0;
}


void tmultiplayersettings :: run ( void )
{
   mousevisible ( true );
   do {
      tdialogbox::run();
   } while ( !status ); /* enddo */
}


tmultiplayersettings :: ~tmultiplayersettings ( )
{
}



void multiplayersettings ( void )
{
   tmultiplayersettings mps;
   mps.init();
   mps.run();
   mps.done();
   sendGameParameterAsMail();
}


class tgiveunitawaydlg : public tdialogbox {
             int markedplayer;
             int status;
             void paintplayer ( int i );
             pfield fld ;
             int num;
             int ply[8];
             int xs;
           public:
             void init ( void );
             void run ( void );
             void buttonpressed ( int id );
       };


void tgiveunitawaydlg :: paintplayer ( int i )
{
   if ( i < 0 ) 
      return;

   setinvisiblemouserectanglestk ( x1 + 20, y1 + starty + xs + i * 40, x1 + xsize - 20, y1 + starty + 60 + i * 40 );

   if ( i == markedplayer ) {
      bar ( x1 + 20, y1 + starty + xs + i * 40, x1 + xsize - 20, y1 + starty + 60 + i * 40, 20 + ply[i] * 8 );
   } else {
      bar ( x1 + 20, y1 + starty + xs + i * 40, x1 + xsize - 20, y1 + starty + 60 + i * 40, dblue );
   } /* endif */

   activefontsettings.color = black;
   activefontsettings.font = schriften.smallarial;
   activefontsettings.length = 0;

   if ( ply[i] < 8 )
      showtext2 ( actmap->player[ply[i]].getName().c_str(), x1 + 60, y1 + starty + xs+17 + i * 40 - activefontsettings.font->height / 2 );
   else
      showtext2 ( "neutral", x1 + 60, y1 + starty + xs+17 + i * 40 - activefontsettings.font->height / 2 );

   getinvisiblemouserectanglestk ();
}

void         tgiveunitawaydlg :: init(void)
{ 
   fld = getactfield();

   xs = 15;

   num = 0;
   for ( int i = 0; i < 8; i++ )
      if ( actmap->player[i].exist() )
         if ( i != actmap->actplayer && getdiplomaticstatus ( i*8 ) == capeace ) 
            ply[num++] = i;

   if ( fld->building )
      ply[num++] = 8;


   if ( !num ) {
      displaymessage("You don't have any allies", 1 );
      return;
   }


   tdialogbox::init();

   xsize = 250;
   ysize = 110 + num * 40;

   if ( fld->vehicle )
      title = "give unit to";
   else if ( fld->building )
      title = "give building to";

   addbutton ( "~O~k", 10, ysize - 30, xsize/2 - 5, ysize - 10 , 0, 1, 1, true );
   addkey ( 1, ct_enter );
   addbutton ( "~C~ancel", xsize/2 + 5, ysize - 30, xsize-10 - 5, ysize - 10 , 0, 1, 2, true );
   addkey ( 2, ct_esc );


   if ( num == 1 )
      markedplayer = 0;
   else
      markedplayer = -1;

   status = 0;


   buildgraphics(); 


   for ( int j = 0; j < num; j++ )
      paintplayer( j );

} 

void         tgiveunitawaydlg :: buttonpressed ( int id )
{
   if ( id == 1 )
      if ( markedplayer >= 0 )
         status = 12;
      else
         displaymessage ( "please select the player you want to give your unit to",1);
   if ( id == 2 )
      status = 10;
}


void tgiveunitawaydlg :: run ( void )
{
   if ( !num ) 
      return;

   while ( mouseparams.taste )
      releasetimeslice();

   mousevisible ( true );
   do {
      tdialogbox :: run ( );

      int old = markedplayer;
      if ( taste == ct_up  && markedplayer > 0 ) 
         markedplayer --;

      if ( taste == ct_down  && markedplayer < num-1 ) 
         markedplayer ++;
      
      if ( mouseparams.taste == 1 )
         for ( int i = 0; i < num; i++ )
            if ( mouseinrect ( x1 + 20, y1 + starty + xs + i * 40, x1 + xsize - 20, y1 + starty + 60 + i * 40 ) ) 
               markedplayer = i;


      if ( old != markedplayer ) {
         paintplayer ( markedplayer );
         paintplayer ( old );
      }
   } while ( status < 10 ); /* enddo */

   if ( status == 12 ) {
      if ( fld->vehicle )
         fld->vehicle->convert ( ply[markedplayer] );
      else
         if ( fld->building )
            fld->building->convert ( ply[markedplayer] );
      logtoreplayinfo ( rpl_convert, (int) getxpos(), (int) getypos(), (int) ply[markedplayer] );
      computeview( actmap );
   }
}

void giveunitaway ( void )
{
   pfield fld = getactfield();
   if ( ( fld->vehicle && fld->vehicle->color==actmap->actplayer*8) ||  (fld->building && fld->building->color == actmap->actplayer * 8 )) {
      tgiveunitawaydlg gua;
      gua.init ();
      gua.run ();
      gua.done ();
   } else {
      dispmessage2( 450, NULL );
      while ( mouseparams.taste )
         releasetimeslice();

   }
}

    struct tvweapon { 
                  int         typ;   /*  position in cwaffentypen  */ 
                  int         sourcepos;   /*  position im munitionsarray des containers; cenergy .. g?ltig */ 
                  int         destpos;   /*  position im munitionsarray des gel. fahrzeugs  */ 
                  int          sourceamount;
                  int          maxsourceamount;
                  int          destamount;
                  int          maxdestamount;
                  int          newdestamount;
               }; 


    struct tvweapons { 
                   int         count; 
                   int         mode; 
                   tvweapon     weap[10]; 
                } ;                 
                
#define entervalue ct_space  

    class  tmunitionsbox : public tdialogbox {
                  public:
                       char          backgrnd;
                       int           maxproduction;
                       char*         strng2;
                       char*         strr ( int a );
                       void          init ( void );
                       void          done ( void );
                    };


    class  tverlademunition : public tmunitionsbox {
                          int abstand;
                          int firstliney;
                          int startx;
                          int llength;
                          int numlength;
                          VehicleService::Target* target;
                          pvehicle      source;
                          vector<int> oldpos;
                          vector<int> newpos;
                          vector<int> displayed;
                          int           abbruch;
                          VehicleService* serviceAction;
                          int targetNWID;

                 public:

                          void          init ( pvehicle src, int _targetNWID, VehicleService* _serviceAction );
                          virtual void  run ( void );
                          virtual void  buttonpressed( int id ); 
                          void          zeichneschieberegler( int pos);
                          void          checkpossible( int pos );
                          void          setloading ( int pos );
                       };



void tmunitionsbox::init ( void ) {
   tdialogbox::init();
   strng2 = new char[100];
}

void tmunitionsbox::done ( void ) {
   delete[] strng2 ;
   tdialogbox::done();
}


char* tmunitionsbox::strr ( int a ) {
   itoa( a, strng2, 10 );
   return strng2;
}


void         tverlademunition::init( pvehicle src, int _targetNWID, VehicleService* _serviceAction )
{
   targetNWID = _targetNWID;
   serviceAction = _serviceAction;
   source = src;

   VehicleService::TargetContainer::iterator i = serviceAction->dest.find ( targetNWID );
   if ( i == serviceAction->dest.end() )
      throw ASCmsgException ( "tverlademunition::init / target not found" );

   target = &(i->second);

   tmunitionsbox::init();
   x1 = 40;
   xsize = 560;
   y1 = 55;

   title = "transfer ammunition and resources";
   backgrnd = dblue;
   windowstyle = windowstyle ^ dlg_in3d;

   firstliney = 120;
   abstand = 25;
   startx = 230;
   llength = xsize - startx - 90;
   numlength = 40;
} 


void         tverlademunition::buttonpressed( int id)
{ 
   if (id == 1) { 
      // setloading();
      abbruch = true; 
   } 
} 


#define yoffset 105
void         tverlademunition::run(void)
{ 
   if ( !target->service.size() ) {
      displaymessage ( "nothing to transfer", 1 );
      return;
   }

   y1 = 150 - 13*target->service.size();
   ysize = 480 - 2 * y1; 

   int i;

   addbutton("~l~oad",10,ysize - 40,xsize - 10,ysize - 10,0,1,1,true);
   addkey(1,ct_enter);
   buildgraphics();

   rahmen(true,x1 + 10,y1 + 50,x1 + xsize - 10,y1 + ysize - 50);

   activefontsettings.justify = lefttext;
   activefontsettings.font = schriften.smallarial;
   activefontsettings.length = 100;

   int ix, iy;
   getpicsize ( target->dest->typ->picture[0], ix, iy );

//   if ( vehicle2 )
   if ( source )
      putrotspriteimage(startx+x1 - 60 , y1 + 55, source->typ->picture[0], actmap->actplayer * 8);
//   else
//      showtext2( building->typ->name , x1 + 50, y1 + 55);

   putrotspriteimage(x1 + xsize - 30 - ix , y1 + 55, target->dest->typ->picture[0], actmap->actplayer * 8);

   int pos = 0;
   for (i = 0; i < target->service.size() ; i++)
      if ( target->service[i].type != VehicleService::srv_repair ) {
         activefontsettings.length = (x1 + startx - numlength - 10 ) - (x1 + 20 );
         const char* text;
         switch ( target->service[i].type ) {
            case VehicleService::srv_ammo : text = cwaffentypen[ target->dest->typ->weapons.weapon[ target->service[i].targetPos ].getScalarWeaponType() ];
                                            break;
            case VehicleService::srv_resource : text = resourceNames[target->service[i].targetPos];
                                            break;
            case VehicleService::srv_repair : text = "damage";
                                            break;
         }
         oldpos.push_back ( target->service[i].curAmount );
         newpos.push_back ( target->service[i].curAmount );
         displayed.push_back ( i );

         showtext2( text,x1 + 20, y1 + firstliney + i * abstand);

         rahmen(true, x1 + startx - 11, y1 + firstliney - 2 + i * abstand, x1 + startx + llength + 11,y1 + firstliney + 18 + i * abstand);
         zeichneschieberegler(pos);
         pos++;
      }

   if ( !pos ) {
      displaymessage ( "nothing to transfer", 1 );
      return;
   }

   mousevisible(true);
   abbruch = 0;

   int mp = 0;
   do {
      int op = mp;
      tmunitionsbox::run();
      switch (taste) {

         case ct_up:   {
                   op = mp;
                   if (mp > 0)
                      mp--;
                   else
                      mp = displayed.size() - 1;
                }
         break;

         case ct_down:   {
                     op = mp;
                     if (mp >= displayed.size() - 1)
                        mp = 0;
                     else
                        mp++;
                  }
         break;

         case ct_left:
         case ct_right:   {
                            int step =  ( target->service[displayed[mp]].maxAmount - target->service[displayed[mp]].minAmount ) / 100;
                            if ( step == 0 )
                               step = 1;
                            step = int( pow ( 10, int ( log10 ( step ))));

                            oldpos[mp] = newpos[mp];

                            if( taste == ct_left )
                               newpos[mp] -= step;
                            else
                               newpos[mp] += step;

                            checkpossible(mp);
                            if (oldpos[mp] != newpos[mp] )
                               setloading ( mp );
                           }
         break;

      }

      if (taste == ct_esc) {
          abbruch = true;
      }

      if (taste == entervalue) {
         mousevisible(false);
         oldpos[mp] = newpos[mp];
         bar(x1 + xsize - 60, y1 + firstliney + mp * abstand,x1 + xsize - 17,y1 + firstliney + 15 + mp * abstand, backgrnd);
         intedit( &newpos[mp], x1 + xsize - 60, y1 + firstliney + mp * abstand, 43, 0, 65535);
         checkpossible(mp);
         if ( oldpos[mp] != newpos[mp] )
            setloading ( mp );
         mousevisible(true);
      }
      if ( mp != op ) {
         mousevisible(false);
         if ( op >= 0 )
            xorrectangle(x1 + 15,y1 + firstliney - 5 + op * abstand,x1 + xsize - 15,y1 + firstliney - 5 + (op + 1) * abstand,14);
         op = mp;
         xorrectangle(x1 + 15,y1 + firstliney - 5 + op * abstand,x1 + xsize - 15,y1 + firstliney - 5 + (op + 1) * abstand,14);
         mousevisible(true);
      }

      int mx = mouseparams.x;
      for (i = 0; i < displayed.size() ; i++) {
         if ((mouseparams.taste == 1) && mouseinrect ( x1 + startx - 10, y1 + firstliney + i * abstand, x1 + startx + llength + 10, y1 + firstliney + i * abstand + 20 )) {
            oldpos[i] = newpos[i];
            float p =  float(mx - (startx + x1)) / llength;
            if (p < 0)
               p = 0;

            if ( p > 1 )
               p = 1;

            newpos[i] = p * target->service[displayed[i]].maxAmount;

            if (newpos[i] > target->service[displayed[i]].maxAmount)
               newpos[i] = target->service[displayed[i]].maxAmount;

            if (newpos[i] < target->service[displayed[i]].minAmount)
               newpos[i] = target->service[displayed[i]].minAmount;

            checkpossible(i);

            if (oldpos[i] != newpos[i]) {
               mousevisible(false);
               setloading ( i );
               mousevisible(true);
            }
         }
      }

   }  while ( abbruch==0 );

   mousevisible(false);

}





void         tverlademunition::zeichneschieberegler( int b )
{ 
   int r = getmousestatus();
   if (r == 2)
      mousevisible(false);


   int sta, sto;
   if ( b == -1 ) {
      sta = 0;
      sto = displayed.size()-1;
   } else {
      sta = b;
      sto = b;
   }

   for ( b = sta; b <= sto; b++ ) {

      int      l;

      bar(x1 + xsize - 60, y1 + firstliney + b * abstand, x1 + xsize - 17,y1 + firstliney + 15 + b * abstand,backgrnd);

      activefontsettings.justify = lefttext;
      activefontsettings.background = dblue;
      activefontsettings.length = numlength;

      showtext2(strrr( target->service[displayed[b]].curAmount ),x1 + startx + llength + 15, y1 + firstliney + b * abstand);

      if ( target->service[displayed[b]].maxAmount )
         l = x1 + startx + llength * oldpos[b] / target->service[displayed[b]].maxAmount;
      else
         l = x1 + startx ;

      rectangle(l - 10, y1 + firstliney - 1 + b * abstand,l + 10,y1 + firstliney - 1 + b * abstand + 18,backgrnd);

      if ( target->service[displayed[b]].maxAmount )
         l = x1 + startx + llength * target->service[displayed[b]].curAmount / target->service[displayed[b]].maxAmount;
      else
         l = x1 + startx ;

      rahmen(false, l - 10,y1 + firstliney - 1 + b * abstand,l + 10,y1 + firstliney - 1 + b * abstand + 18);

   //   bar(x1 + 116,y1 + 100 - 35 + b * abstand,startx - 12,y1 + 115 - 35 + b * abstand,backgrnd);

      showtext2(strrr( target->service[displayed[b]].orgSourceAmount - (target->service[displayed[b]].curAmount - newpos[b])), x1 + startx - numlength -15 ,y1 + firstliney + b * abstand);

   }

   if (r == 2)
      mousevisible(true);
}


void         tverlademunition::checkpossible(int         b)
{
   if ( newpos[b] > target->service[displayed[b]].maxAmount )
      newpos[b] = target->service[displayed[b]].maxAmount;

   if ( newpos[b] < target->service[displayed[b]].minAmount )
      newpos[b] = target->service[displayed[b]].minAmount;
}



void         tverlademunition::setloading( int pos )
{
   serviceAction->execute ( source, targetNWID, -1, 2, pos, newpos[pos] );
   zeichneschieberegler ( -1 );
}


void         verlademunition( VehicleService* serv, int targetNWID )
{
   tverlademunition vlm;
   vlm.init( serv->getVehicle(), targetNWID, serv );
   vlm.run();
   vlm.done(); 
}


