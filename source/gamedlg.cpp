//     $Id: gamedlg.cpp,v 1.12 2000-01-02 19:47:06 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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
#include <stdio.h>                        
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifdef _DOS_
#include <dos.h>
#include <conio.h>
#endif

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


#include "basegfx.h"
#include "gamedlg.h"
#include "missions.h"
#include "mousehnd.h"
#include "stack.h"
#include "network.h"
#include "controls.h"
#include "timer.h"
#include "sg.h"
#include "loadpcx.h"
#include "loadjpg.h"


class   tchoosetechnology : public tdialogbox {
                           dynamic_array<ptechnology> techs;
                           char          markedbackgrnd;
                           int           technum;
                           int           dispnum;
                           int           firstshowntech;
                           int           markedtech;
                           void          disp ( void );
                           void          buttonpressed ( char id  );
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
          int k = 0; 
          pdevelopedtechnologies devtech = resrch->developedtechnologies;
          while (devtech) {
             if (devtech->tech == tech )
                   k++;
             devtech = devtech->next;
          } /* endwhile */

          if (k > 1) 
             displaymessage( "choosetechnology: technology %s developed multiple times !", 2, tech->name );

          int l = 0; 
          if (k == 0) {   /*  technology not yet developed  */ 
             if ( ! (actmap->objectcrc   &&   !actmap->objectcrc->speedcrccheck->checktech2 ( tech, 0 ))) {
                for (int j = 0; j <= 5; j++) { 
                   int m = 0; 
                   if ( tech->requiretechnology[j] ) { 
                      pdevelopedtechnologies devtech = resrch->developedtechnologies;
                      while (devtech) {
                         if (devtech->tech == tech->requiretechnology[j])
                               m++;
                         devtech = devtech->next;
                      } /* endwhile */
   
   
                      if ( m )
                         l++;
                   } 
                   else 
                      l++;
   
                   if ( tech->requireevent) 
                      if ( ! actmap->eventpassed(tech->id, cenewtechnologyresearchable) )
                         l = 0; 
   
                } 
             }
          } 
          if (l == 6) { 
             techs[technum] = tech; 
             technum++;
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

void         tchoosetechnology::buttonpressed ( char id )
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
                  
                  while ( mouseparams.taste );
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

     if ( actmap->objectcrc )
       if ( !actmap->objectcrc->speedcrccheck->checktech2 ( techs[markedtech] )) 
          displaymessage ("CRC-check inconsistency in choosetechnology :: run ", 2 );

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
               void buttonpressed ( char id );
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

void tchoosetransfermethod :: buttonpressed ( char id )
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
               void buttonpressed ( char id );
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
   addbutton ( "~e~xit", xsize / 2 + 5,  ysize - 40, xsize - 10, ysize - 10, 0, 1, 2, player == -1 );
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
            while ( actmap->player[ fp ].existent == 0 )
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
   if ( chan == receive ) {
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

   if ( chan == receive ) {
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

void tsetupnetwork :: buttonpressed ( char id )
{
   tdialogbox::buttonpressed ( id );
   switch ( id ) {
     case 1: if ( edit == 1 ) {
                if ( network.computer[frstcompnum].receive.transfermethod )
                   if ( network.computer[frstcompnum].receive.transfermethod->validateparams ( &network.computer[frstcompnum].receive.data, receive ) )
                      status = 2;
             } else
               if ( edit == 2 ) {
                  if ( network.computer[frstcompnum].send.transfermethod )
                     if ( network.computer[frstcompnum].send.transfermethod->validateparams ( &network.computer[frstcompnum].send.data, send ) )
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
                  throw tnomaploaded;
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

     case 8: getmethod ( receive );
             paintransfermnames();
             break;
             
     case 9: getmethod ( send );
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
      tsetupnetwork sun;
      if ( nw ) 
         sun.init( nw, edt, player );
      else
         sun.init ( actmap->network, edt, player );

      sun.run();
      int i = sun.status;
      sun.done();
      return i;
   }
   return 1;

}

/*********************************************************************************************************/
/*   Neuen Level starten                                                                               ˇ */
/*********************************************************************************************************/

void         tchoosenewcampaign::evaluatemapinfo( char* srname )             
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


void         tchoosenewsinglelevel::evaluatemapinfo( char* srname )
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
   spfld = NULL; 
} 

void         tnewcampaignlevel::searchmapinfo(void)
{ 

   status = 0; 
   freespfld ();
   message1[0] = 0;
   message2[0] = 0;
   mapinfo[0] = 0;
   maptitle[0] = 0;


   tfindfile ff ( mapextension );
   char* filename = ff.getnextname();
   while( filename ) {

       try {
          tnfilestream filestream ( filename, 1 );
          tmaploaders spfldloader;
          spfldloader.stream = &filestream;
          // spfldloader.setcachingarrays ( );
   
          CharBuf description;
       
          spfldloader.stream->readpchar ( &description.buf );
          int desclen = strlen ( description.buf ) + 7;
          strncpy ( dateiinfo, description.buf, sizeof ( dateiinfo ) -2  );

          word w;
          spfldloader.stream->readdata2 ( w );
       
          if ( w != fileterminator ) 
             throw tinvalidversion ( filename, w, fileterminator );
      
          int version;
          spfldloader.stream->readdata2( version );
       
          if (version != mapversion) { 
             if (  desclen < 31 ) {
                char tempbuf[32];
                spfldloader.stream->readdata ( tempbuf, 31 - desclen );
             }
      
             spfldloader.stream->readdata2( version );
       
             if (version != mapversion) 
                throw tinvalidversion ( filename, w, fileterminator );

          } 
         
          spfldloader.readmap ();
          spfld = spfldloader.spfld;
      
          evaluatemapinfo( filename );
          spfld = NULL;
      } 
      catch ( tfileerror ) {
      } /* endcatch */

      filename = ff.getnextname();
   } 

   if ((status == 5) || ( status == 6 )) {
      strcpy(message1,  "multiple maps with the same id/password found" );
      message2[0] = 0;
      mapinfo[0] = 0;
      maptitle[0] = 0;
      mapname[0] = 0;
   } 

   if (status == 0) { 
      strcpy ( message1 , "no maps found" );
   } 
   spfld = NULL;

   freespfld ();
} 


void         tnewcampaignlevel::showmapinfo(word         ypos)
{ 

   activefontsettings.font = schriften.smallarial;
   push(activefontsettings,sizeof(activefontsettings)); 
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

   pop(activefontsettings,sizeof(activefontsettings)); 
} 


void         tnewcampaignlevel::loadcampaignmap(void)
{ 

   displaymessage("loading: %s", 0, mapname);

   try {
      if (loadmap(mapname) == 0) {
         ::initmap();
         
         do {
           next_turn();
           if ( actmap->time.a.turn == 2 ) {
              displaymessage("no human players found !", 1 );
              erasemap();
              throw tnomaploaded();
           }
         } while ( actmap->player[actmap->actplayer].stat != ps_human ); /* enddo */
         
      } else {
         if (loaderror > 0) 
            displaymessage("error nr. %d : %s", 1, loaderror, loaderrormsg(loaderror));
      }
   } /* endtry */
   catch ( tinvalidid err ) {
      displaymessage( err.msg, 1 );
      if ( !actmap || actmap->xsize <= 0)
         throw tnomaploaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      displaymessage( "File %s has invalid version.\nExpected version %d\nFound version %d\n", 1, err.filename, err.expected, err.found );
      if ( !actmap || actmap->xsize <= 0)
         throw tnomaploaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading map filename %s ", 1, err.filename );
      if ( !actmap || actmap->xsize <= 0)
         throw tnomaploaded();
   } /* endcatch */
   catch ( terror ) {
      displaymessage( "error loading map", 1 );
      if ( !actmap || actmap->xsize <= 0)
         throw tnomaploaded();
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
   collategraphicoperations cgo;
   tnewcampaignlevel::showmapinfo(ypos);
   push(activefontsettings,sizeof(activefontsettings)); 

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

   pop(activefontsettings,sizeof(activefontsettings)); 
} 


void         tcontinuecampaign::evaluatemapinfo(char *       srname)
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

   collategraphicoperations cgo;

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


void         tcontinuecampaign::buttonpressed(byte         id)
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
            char t[30];

            fileselectsvga("*.rcy", t, 2);
            if ( t[0] )
               savecampaignrecoveryinformation ( t, idsearched); 
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
      peventstore foev;

      if ( oev == NULL ) {
         oev = new ( teventstore ) ;
         oldevent = oev;
         foev = oev; 
         oev->next = NULL; 
         oev->num = 0; 
      } 
      else  
         if (oev->num == 256) { 
            foev = oev;
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

   for (int i = 0; i<8; i++) {
      if ( map->player[i].existent ) 
         tech[i] = map->player[i].research.developedtechnologies;
      else
         tech[i] = NULL;

      if ( map->player[i].existent ) 
         dissectedunits[i] = map->player[ i ].dissectedunit;
      else
         dissectedunits[i] = NULL;
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
      for (i=0;i<8 ; i++) {
         actmap->player[i].research.developedtechnologies = NULL;
         actmap->player[ i ].dissectedunit = NULL;
      }

      loadcampaignmap(); 
      if (loaderror == 0) {
         actmap->oldevents = oldevent;
         for (i=0;i<8 ; i++) {
            actmap->player[i].research.developedtechnologies = tech[i];
            actmap->player[ i ].dissectedunit = dissectedunits[i];
         }

      } else
         status = 0;
   } 
   if (status == 50)
      exit( 0 );
} 

void         tchoosenewmap::readmapinfo(void)
{ 

   if (mapname[0] != 0) 
      freespfld ();

   message1[0] = 0;
   message2[0] = 0;
   mapinfo[0] = 0;
   maptitle[0] = 0;


    try {
       tnfilestream filestream ( mapname, 1 );
       tmaploaders spfldloader;
       spfldloader.stream = &filestream;
       // spfldloader.setcachingarrays ( );

       CharBuf description;
    
       spfldloader.stream->readpchar ( &description.buf );
       int desclen = strlen ( description.buf ) + 7;
       strncpy ( dateiinfo, description.buf, sizeof ( dateiinfo) - 2  );

       word w;
       spfldloader.stream->readdata2 ( w );
    
       if ( w != fileterminator ) 
          throw tinvalidversion ( mapname, w, fileterminator );
   
       int version;
       spfldloader.stream->readdata2( version );
    
       if (version != mapversion) { 
          if (  desclen < 31 ) {
             char tempbuf[32];
             spfldloader.stream->readdata ( tempbuf, 31 - desclen );
          }
   
          spfldloader.stream->readdata2( version );
    
          if (version != mapversion) 
             throw tinvalidversion ( mapname, w, fileterminator );

       } 
      
       spfldloader.readmap ();
       spfld = spfldloader.spfld;
   
       checkforcampaign(  );

   } 
   catch ( tfileerror ) {
      strcpy( message1, "invalid map version" );
      status = 1; 
   } /* endcatch */
   spfld = NULL;

} 
 
void         tchoosenewmap::buttonpressed( char id )
{ 
  char         t[100];

   switch (id) {
      
      case 2:   { 
            fileselectsvga(mapextension, t, 1);
            if ( t[0] ) {
               strcpy(mapname, t);
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
   spfld = NULL; 


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


                       
void         tchoosenewsinglelevel::checkforcampaign( void )
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

void         tchoosenewcampaign::checkforcampaign( void )
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
         if ( loadmap( mapname ) == 0) {
            ::initmap();
            
            removemessage();
            if (actmap->campaign != NULL) { 
               delete actmap->campaign;
               actmap->campaign = NULL; 
            }
            
            setupalliances();
            
            int human = 0;
            for ( int i = 0; i < 8; i++ )
               if ( actmap->player[i].stat == ps_human )
                  if ( actmap->player[i].existent )
                     human++;
   
            if ( !human ) {
               displaymessage ( "no human players found !", 1 );
               erasemap();
               throw tnomaploaded();
            }

            if ( human > 1 )
               multiplayersettings ();
            else
               choosetechlevel();
   
            do {
              next_turn();
              if ( actmap->time.a.turn == 2 ) {
                 displaymessage("no human players found !", 1 );
                 erasemap();
                 throw tnomaploaded();
              }
            } while ( actmap->player[actmap->actplayer].stat != ps_human ); /* enddo */
         } else {
            displaymessage("error nr. %d",1, loaderror );
         }
      } /* endtry */

      catch ( tinvalidid err ) {
         displaymessage( err.msg, 1 );
         if ( actmap->xsize <= 0)
            throw tnomaploaded();
      } /* endcatch */
      catch ( tinvalidversion err ) {
         displaymessage( "File %s has invalid version.\nExpected version %d\nFound version %d\n", 1, err.filename, err.expected, err.found );
         if ( actmap->xsize <= 0)
            throw tnomaploaded();
      } /* endcatch */
      catch ( tfileerror err) {
         displaymessage( "error reading map filename %s ", 1, err.filename );
         if ( actmap->xsize <= 0)
            throw tnomaploaded();
      } /* endcatch */
      catch ( terror ) {
         displaymessage( "error loading map", 1 );
         if ( actmap->xsize <= 0)
            throw tnomaploaded();
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
                       tresourcetribute trib;
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
                       void buttonpressed ( char id );
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
   title = "tributes";

   trib = *actmap->tribute;

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
      if ( actmap->player[i].existent )
         if ( i != actmap->actplayer )
            players[pos++] = i;


   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = black;
   activefontsettings.length = 145 - 55;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;

   for ( i = 0; i < pos; i++) {
      bar ( x1 + 30, y1 + starty + 30 + i * 30, x1 + 50, y1 + starty + 50 + i * 30, 20 + players[i] * 8 );
      if ( actmap->player[players[i]].name )
         showtext2 ( actmap->player[players[i]].name, x1 + 55, y1 + starty + 30 + i * 30 );
   }

   wind2y = starty + ( ysize - starty - 60 ) /2 + 5;

   wind1x = 255 ;
   wind2x = 375 ;

   rahmen ( true,      x1 + 10,  y1 + starty,      x1 + xsize - 10, y1 + ysize - 50 );
   rahmen3 ( "player", x1 + 20,  y1 + starty + 10, x1 + 150,        y1 + ysize - 60, 1 );

   rahmen3 ( "you have to pay",    x1 + 160, y1 + wind1y,      x1 + wind2x - 10, y1 + wind2y - 10, 1 );
   rahmen3 ( "you have to get",    x1 + 160, y1 + wind2y,      x1 + wind2x - 10, y1 + ysize - 60 , 1 );

   rahmen3 ( "you paid",    x1 + wind2x - 5, y1 + wind1y,      x1 + xsize - 20, y1 + wind2y - 10, 1 );
   rahmen3 ( "you got",     x1 + wind2x - 5, y1 + wind2y,      x1 + xsize - 20, y1 + ysize - 60 , 1 );


   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = textcolor;
   activefontsettings.length = 0;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;

   for ( i = 0; i < 3; i++) {
      showtext2 ( cdnames[i], x1 + 170, y1 + wind1y + 15 + i * 40 );
      showtext2 ( cdnames[i], x1 + 170, y1 + wind2y + 15 + i * 40 );

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
      addeingabe ( 3+i, &trib.avail.resource[i][actmap->actplayer][players[player]], 0, maxint );
      enablebutton ( 3+i );

      activefontsettings.length = xsize - 40 - wind2x;
      showtext2 ( strrr ( trib.paid.resource[i][actmap->actplayer][players[player]]), x1 + wind2x + 5, y1 + wind2y + 16 + i * 40 );
      showtext2 ( strrr ( trib.paid.resource[i][players[player]][actmap->actplayer]), x1 + wind2x + 5, y1 + wind1y + 16 + i * 40 );

      activefontsettings.length = wind2x - wind1x - 35;
      showtext2 ( strrr ( trib.avail.resource[i][players[player]][actmap->actplayer]), x1 + wind1x + 5, y1 + wind2y + 16 + i * 40 );
   } /* endfor */

   getinvisiblemouserectanglestk ();
}


void  ttributepayments :: buttonpressed( char id )
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
      *actmap->tribute = trib;
}
   

void settributepayments ( void )
{
   ttributepayments tpm;
   tpm.init();
   tpm.run();
   tpm.done();
}

void  tshownewtanks :: init ( boolean*      buf2 ) 
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

void  tshownewtanks :: buttonpressed ( char id )
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
                      void buttonpressed ( char id );
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
   pbuilding bld = actmap->player[actmap->actplayer].firstbuilding;
   while ( bld ) {
      rppt += bld->researchpoints;
      bld=bld->next;
   } /* endwhile */
}

void tresearchinfo::buttonpressed ( char id )
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
      if ( tech->pictfilename  && (modenum24 > 0 ) ) {
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
         while ( mouseparams.taste );
         do {

         } while ( t + 200 > ticker  &&  !keypress()  && !mouseparams.taste); /* enddo */

         int abrt = 0;

         char pcx[300];
         char jpg[300];
         int pic = getbestpictname ( tech->pictfilename, pcx, jpg );

         if ( pic ) {

            reinitgraphics( modenum24 );
            try {
               {
                  if ( pic & 1 ) {
                     tnfilestream stream ( pcx , 1 );
                     loadpcxxy ( &stream, 0, 0 );
                  } else {
                     tnfilestream stream ( jpg, 1 );
                     read_JPEG_file ( &stream );
                  }
               }
            } /* endtry */
            catch ( tfileerror err ) {
               abrt = 1;
            } /* endcatch */
   
   
            t = ticker;
            while ( mouseparams.taste );
            do {
   
            } while ( t + 600 > ticker  &&  !keypress()  && !mouseparams.taste && !abrt ); /* enddo */
   
            reinitgraphics( modenum8 );
         }            


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


void settechlevel ( int techlevel, int playerBM )
{
   if ( techlevel )               
      for (int i = 0; i < 8; i++ ) {
         if ( playerBM & ( 1 << i ))
            if ( actmap->player[i].existent ) {
               actmap->player[i].research.techlevel = techlevel;
               for ( int j = 0; j < technologynum; j++ )
                  if ( gettechnology_forpos ( j ) )
                     if ( gettechnology_forpos(j)->techlevelget <= techlevel ) 
                        addanytechnology ( gettechnology_forpos ( j ), i );
      
            }
      }
}



class tchoosetechlevel : public tdialogbox {
             protected:
                int techlevel; 
                int ok;
             public: 
                void init ( void );
                void buttonpressed ( char id );
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

void tchoosetechlevel :: buttonpressed ( char id )
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
      settechlevel ( techlevel, 0xff );

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
          tparagraph ( pparagraph prv );   // fÅgt einen neuen paragraph hinter prv an

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

          void addtext ( char* txt );
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

#ifndef _NoStaticClassMembers_
StaticClassVariable int tparagraph :: winy1;
StaticClassVariable int tparagraph :: winy2;
StaticClassVariable int tparagraph :: winx1;
StaticClassVariable int tparagraph :: maxlinenum;
#endif

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



void tparagraph :: addtext ( char* txt )
{
    int nsize = strlen ( txt );
    if ( allocated < nsize + size )
       changesize ( nsize + size );
 
    strcat ( text, txt );
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
            return temp;              //  !!!###!!!   gefÑllt mir eigentlich Åberhauptnicht, wird aber wohl laufen. Sonst mÅ·te ich halt erasechar von au·en managen
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
        oldlength = length;
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
   collategraphicoperations cgo;
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
      collategraphicoperations cgo;
      int startline;
      int starty;
      if ( ps.line1num < 0 ) {
         starty = winy1 + (ps.line1num + cursory) * activefontsettings.height;
         startline = -ps.line1num;
      } else {
         starty = winy1 + (ps.line1num + cursory) * activefontsettings.height;
         startline = 0;
      }
                   
      if ( starty  < winy2 ) {
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
   collategraphicoperations cgo;
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
                void inserttext ( char* txt );
                void run ( void );
                void extracttext ( char** c );
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

void tmessagedlg :: inserttext ( char* txt )
{
   if ( !txt )
      firstparagraph = new tparagraph;
   else {
      firstparagraph = NULL;
      actparagraph = NULL;
      char* c = txt;
      char* start = txt;
      while ( *c ) {
         int sz = 0;
         if ( strnicmp(c, "#CRT#" , 5 ) == 0 ) 
            sz = 5;
         if ( strnicmp(c, "\r\n" , 2 ) == 0 )
            sz = 2;
         if ( strnicmp(c, "\n" , 2 ) == 0 )
            sz = 1;
         if ( sz ) {
            // if ( c[sz] ) {
               char d = *c;
               *c = 0;
               actparagraph = new tparagraph ( actparagraph );
               if ( !firstparagraph )
                  firstparagraph = actparagraph;
  
               actparagraph->addtext ( start );
               *c = d;
            // }
            c+=sz;
            start = c;
         } else
            c++;
      } /* endwhile */

      if ( strlen ( start ) ) {
         actparagraph = new tparagraph ( actparagraph );
         if ( !firstparagraph )
             firstparagraph = actparagraph;
         actparagraph->addtext ( start );
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

void tmessagedlg :: extracttext ( char** c  )
{
   tparagraph text;

   actparagraph = firstparagraph;
   while ( actparagraph ) {
      text.addtext ( actparagraph->text );
      text.addtext ( "#crt#" );
      actparagraph = actparagraph->next;
   }
   *c = strdup ( text.text );

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
      if ( actmap->player[i].existent )
         if ( actmap->actplayer != i ) {
            int x = 20 + ( num % 2 ) * 200;
            int y = ty2 + 10 + ( num / 2 ) * 20;
            addbutton ( actmap->player[i].name, x, y, x+ 180, y+15, 3, 0, num+3, true );
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
               void buttonpressed ( char id );
               void run ( void );
       };

tnewmessage :: tnewmessage ( void )
{
   rtitle = "new message";
}

void tnewmessage :: init ( void )
{
   collategraphicoperations cgo;
   tdialogbox :: init ( );
   title = rtitle;
   for ( int i = 0; i < 8; i++ ) 
      if ( actmap->player[i].existent &&  actmap->actplayer != i ) 
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

   inserttext ( NULL );
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


void tnewmessage :: buttonpressed ( char id )
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
      char* c;
      extracttext ( &c );
      pmessage message = new tmessage ( actmap );
      message->text = c;
      for ( int i = 0; i < 8; i++ ) {
         if ( actmap->player[i].existent )
            if ( actmap->actplayer != i ) 
               message->to |= to[i] << i;
      }
      pmessagelist list = new tmessagelist ( &actmap->unsentmessage );
      list->message = message;

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
               pmessage message;
            public:
               void init ( pmessage msg );
               void buttonpressed ( char id );
               void run ( void );
       };


void teditmessage :: init ( pmessage msg  )
{
   collategraphicoperations cgo;
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

   inserttext ( msg->text );
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


void teditmessage :: buttonpressed ( char id )
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
      char* c;
      extracttext ( &c );
      delete[] message->text;
      message->text = c;
      message->to = 0;
      for ( int i = 0; i < 8; i++ ) {
         if ( actmap->player[i].existent )
            if ( actmap->actplayer != i ) 
               message->to |= to[i] << i;
      }
   }
}


void editmessage ( pmessage msg )
{
  teditmessage nm;
  nm.init( msg );
  nm.run();
  nm.done();
}

class tviewmessages : public tdialogbox {
               pmessagelist start;
               pmessagelist *message;
               int act;
               int num;
               int dispnum;
               int firstdisplayed;
               int player[8];
               int mode;
               int ok;

            public:
               tviewmessages ( void );
               void init ( char* ttl, pmessagelist strt, int editable, int md  );    // mode : 0 verschickte ; 1 empfangene
               int  getcapabilities ( void ) { return 1; };
               void redraw ( void );
               void run ( void );
               void buttonpressed ( char id );
               void paintmessages ( void );
               void checkforscroll ( void );
               ~tviewmessages();
          };


tviewmessages :: tviewmessages ( void )
{
   message = NULL;
   ok = 0;
   dispnum = 10;
}

void tviewmessages :: init ( char* ttl, pmessagelist strt, int editable, int md )
{
   mode = md;
   tdialogbox :: init ( );
   num = 0;
   if ( strt ) {
      pmessagelist temp = strt;
      while ( temp->prev )
         temp = temp->prev;
      start = temp;
      act = 0;
      while ( temp ) {
         num++;
         temp = temp->next;
      }
      message = new pmessagelist[num];
   
      temp = strt;
      int i = 0;
      while ( temp ) {
         message[i] = temp;
         i++;
         temp = temp->next;
      }
   }
   firstdisplayed = 0;

   xsize =  400;
   ysize =  300;
   title = ttl;
   windowstyle &= ~dlg_in3d;
   if ( editable ) {
     addbutton ( "~V~iew",    10,              ysize - 30, xsize / 3 - 5, ysize - 10 ,   0, 1, 1, num > 0  );
     addkey    ( 1, ct_enter );

     addbutton ( "~E~dit",    xsize / 3 + 5,   ysize - 30, 2*xsize / 3 - 5, ysize - 10 , 0, 1, 2, num > 0  );

     addbutton ( "~C~ancel",  2*xsize / 3 + 5, ysize - 30, xsize - 10, ysize - 10 ,      0, 1, 3, true );
     addkey    ( 3, ct_esc );
   } else {
     addbutton ( "~V~iew",    10,            ysize - 30, xsize / 2 - 5, ysize - 10 ,   0, 1, 1, num > 0  );
     addkey    ( 1, ct_enter );

     addbutton ( "~C~ancel",  xsize / 2 + 5, ysize - 30, xsize - 10, ysize - 10 ,      0, 1, 3, true );
     addkey    ( 3, ct_esc );
   }

   if ( num > dispnum )
     addscrollbar ( xsize - 25, starty, xsize - 10, ysize - 40 , &num, dispnum, &firstdisplayed, 4, 0 );

   int b = 0;

   for ( int i = 0; i < 8; i++ )
      if ( actmap->player[i].existent  &&  actmap->actplayer != i ) 
            player[i] = b++;
      else
            player[i] = -1;

   buildgraphics();
            
}

void tviewmessages :: buttonpressed ( char id )
{
   if ( id == 1 )
      viewmessage ( message[ act ]->message );

   if ( id == 2 )
      editmessage ( message[ act ]->message );

   if ( id == 3 )
      ok = 1;

   if ( id == 4 )
      paintmessages();
}

void tviewmessages :: paintmessages ( void )
{
   if ( start ) {

       setinvisiblemouserectanglestk ( x1 + 10, y1 + starty, x1 + xsize - 30, y1 + ysize - 40 );

       int a = firstdisplayed;
       activefontsettings.font = schriften.smallarial;
       activefontsettings.background = dblue;
       activefontsettings.justify = lefttext;
       while ( a < num  &&  a-firstdisplayed < dispnum ) {
          if ( a == act )
             activefontsettings.color= white;
          else
             activefontsettings.color= black;
    
          activefontsettings.length = 190;
    
          tm *tmbuf;
	  tmbuf = localtime (&message[a]->message->time);
          int y = y1 + starty + 10 + ( a - firstdisplayed ) * 20 ;

	  showtext2 (asctime (tmbuf), x1 + 20, y);
    
          activefontsettings.length = 100;
          if ( mode ) {
             int fr = log2 ( message[a]->message->from );
             if ( fr < 8 )
                showtext2 ( actmap->player[ fr ].name, x1 + 220, y );
             else
                showtext2 ( "system", x1 + 220, y );
          } else {
             for ( int i = 0; i < 8; i++ )
                if ( player[i] >= 0 ) {
                   int x = x1 + 220 + player[i] * 15;
                   int color;
                   if ( message[a]->message->to & ( 1 << i ) ) 
                      color = 20 + i * 8;
                   else
                      color = dblue ;
    
                   bar ( x, y, x + 10, y + 10, color );
                   
                }
          }
    
          a++;
       }

       getinvisiblemouserectanglestk();
   }
}

void tviewmessages :: checkforscroll ( void )
{
   int oldfirst = firstdisplayed;
   if ( act < firstdisplayed )
      firstdisplayed = act;
   if ( act > firstdisplayed + (dispnum-1) )
      firstdisplayed = act - (dispnum-1) ;

   if ( oldfirst != firstdisplayed )
      if ( num > dispnum )
         enablebutton ( 4 );
}


void tviewmessages :: redraw ( void )
{
   tdialogbox::redraw();
   if ( num > dispnum )
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
      if ( start ) {
         if ( taste == ct_down  &&  act+1 < num ) {
            act++;
            checkforscroll();
            paintmessages();
         }
         if ( taste == ct_up  &&  act ) {
            act--;
            checkforscroll();
            paintmessages();
         }
         if ( taste == ct_pos1  &&  act ) {
            act=0;
            checkforscroll();
            paintmessages();
         }
         if ( taste == ct_ende  &&  act+1 < num ) {
            act = num-1;
            checkforscroll();
            paintmessages();
         }

         if ( mouseinrect ( x1 + 10, y1 + starty, x1 + xsize - 30, y1 + ysize - 40 )) {
            if ( mouseparams.taste == 1 ) {
                int ps = (mouseparams.y - (y1 + starty + 10)) / 20;
                if ( ps < dispnum ) {
                   int n = ps + firstdisplayed;
                   if ( n < num  && n >= 0) {
                      if ( n != act ) {
                         act = n;
                         paintmessages();
                         mstatus = 1;
                      }
                      if ( n == act  &&  mstatus == 2 ) {
                         buttonpressed ( 1 );
                         mstatus = 0;
                      }
                   }
                }
            }

         }
         if ( mouseparams.taste == 0  && mstatus == 1 )
            mstatus = 2;
      }
   } while ( !ok ); /* enddo */

}

tviewmessages :: ~tviewmessages (  )
{
  if ( message ) {
     delete message;
     message = NULL;
  }
}


void viewmessages ( char* title, pmessagelist strt, int editable, int md  )    // mode : 0 verschickte ; 1 empfangene
{
   tviewmessages vm;
   vm.init  ( title, strt, editable, md );
   vm.run ();
   vm.done();
}








  class   tviewmessage : public tdialogbox, public tviewtextwithscrolling {
               public:
                   char                 *txt;
                   boolean              ok;
                    
                   boolean              scrollbarvisible;
                   char                 action;
                   int                  textstart;
                   int                  from;
                   int                  cc;
                   int                  rightspace;

                   void                 init( pmessage msg );
                   virtual void         run ( void );
                   virtual void         buttonpressed( char id);
                   void                 redraw ( void );
                   int                  getcapabilities ( void ) { return 1; };
                   void                 repaintscrollbar( void );
                };



void         tviewmessage::init( pmessage msg )
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


   txt = msg->text;
   from = msg->from;
   cc = msg->to;

                       
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
         showtext2 ( actmap->player[n].name, x1 + 60, y1 + textstart - 45 ); 
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
      if ( actmap->player[i].existent  && i != actmap->actplayer )
         if ( cc & ( 1 << i ))
            n++;
   if ( n ) {
      activefontsettings.length = (xsize - (n+1) * 5 - 60 - rightspace ) / n;
   
      n = 0;
      for ( i = 0; i < 8; i++ )
         if ( actmap->player[i].existent  && i != actmap->actplayer )
            if ( cc & ( 1 << i )) {
               activefontsettings.color = 20 + 8 * i;
               //activefontsettings.background = 17 + 8 * i;
               showtext2 ( actmap->player[i].name, x1 + 60 + n * activefontsettings.length + n * 5 , y1 + textstart - yp );
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


void         tviewmessage::buttonpressed( char id )
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



void viewmessage ( pmessage message )
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
               void buttonpressed ( char id );
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

   inserttext ( actmap->newjournal );
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


void teditjournal :: buttonpressed ( char id )
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
      char* c;
      extracttext ( &c );
      if ( actmap->newjournal )
         delete[] actmap->newjournal;
      actmap->newjournal = c;

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
   if ( active == 2 )
      removehelp();
}

#define maxonlinehelplinenum 5

void tonlinemousehelp :: displayhelp ( int messagenum )
{
  int i;
   if ( active == 1 ) {
      char* str = getmessage ( messagenum );
      if ( str ) {

         char strarr[maxonlinehelplinenum][100];
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
   
         pos.x1 = mouseparams.x - width/2 - 2;
         pos.x2 = mouseparams.x + width/2 + 2;
   
         if ( pos.x1 < 0 ) {
            pos.x1 = 0;
            pos.x2 = width + 4;
         }
         if ( pos.x2 >= agmp->resolutionx ) {
            pos.x2 = agmp->resolutionx - 1;
            pos.x1 = pos.x2 - width - 4;
         }
            
   
         pos.y1 = mouseparams.y + mouseparams.ysize;
         pos.y2 = pos.y1 + (( actlinenum+1) * (schriften.guifont->height+4)) + 2;
   
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
     for ( int i = 0; i < list->num; i++ )
        if ( mouseinrect ( &list->item[i].rect ))
           displayhelp( list->item[i].messagenum );

}


void tonlinemousehelp :: checkforhelp ( void )
{
   if ( gameoptions.onlinehelptime ) 
      if ( ticker > lastmousemove+gameoptions.onlinehelptime )
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
                        tgameoptions actoptions;
                        int status;
                        tmouserect r1, r5, r6, r7, r8;
                        tmouserect dlgoffset;
                        char actgamepath[200];

                     public:
                        char checkvalue( char id, void* p)  ;
                        void init ( void );
                        void buttonpressed ( char id );
                        void paintbutt ( char id );
                        void run ( void );
                    };


void tgamepreferences :: init ( void )
{
   tdialogbox::init();
   title = "options";

   xsize = 600;
   ysize = 460;

   x1 = -1;
   y1 = -1;

   actoptions = gameoptions;

   addbutton ( "~O~K", 10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1, 1, true );
   addkey ( 1, ct_enter );

   addbutton ( "~C~ancel", xsize / 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1, 2, true );
   addkey ( 2, ct_esc );

   addbutton ( "", xsize -35, starty + 20, xsize - 20, starty + 35, 3, 0, 3, true );
   addeingabe ( 3, &actoptions.container.autoproduceammunition, 0, dblue );


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

   r8.x1 = xsize - 150;
   r8.y1 = starty + 165;
   r8.x2 = xsize - 20;
   r8.y2 = starty + 185;
   addbutton ( "", r8, 11, 0, 8, true  );


   addbutton ( "", r8.x1, r8.y1 + 30, r8.x2, r8.y2 + 30, 2, 1, 9, true );
   addeingabe ( 9, &actoptions.onlinehelptime , 0, 10000 );

   addbutton ( "", xsize -35, r8.y1 + 60, r8.x2, r8.y2 + 60, 3, 0, 10, true );
   addeingabe ( 10, &actoptions.smallguiiconopenaftermove , 0, dblue  );

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

   dlgoffset.x1 = x1;
   dlgoffset.y1 = y1;
   dlgoffset.x2 = x1;
   dlgoffset.y2 = y1;

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

   showtext2 ( "onlinehelp-delay in 1/100 sec", x1 + 25, y1 + r8.y1 + 30);

   showtext2 ( "mousegui menu opens after movement", x1 + 25, y1 + r8.y1 + 60);

//   showtext2 ( "game path", x1 + 25, y1 + r8.y1 + 120);
   showtext2 ( "movement speed in 1/100 sec", x1 + 25, y1 + r8.y1 + 120);

   showtext2 ( "prompt for end of turn", x1 + 25, y1 + r8.y1 + 150 );
   status = 0;


}

void tgamepreferences :: paintbutt ( char id )
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
   if ( id >= 5   &&  id <= 8 ) {
      int pos;
      tmouserect r2;
      if ( id == 5 ) {
        r2 = r5 + dlgoffset;
        pos = actoptions.mouse.smallguibutton;
      } 
      if ( id == 6 ) {
        r2 = r6 + dlgoffset;
        pos = actoptions.mouse.fieldmarkbutton;
      }
      if ( id == 7 ) {
        r2 = r7 + dlgoffset;
        pos = actoptions.mouse.scrollbutton;
      }
      if ( id == 8 ) {
        r2 = r8 + dlgoffset;
        pos = actoptions.mouse.centerbutton;
      }

      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial; 
      activefontsettings.justify = centertext; 
      activefontsettings.length = r2.x2 - r2.x1 - 2;
      activefontsettings.background = dblue;
      setinvisiblemouserectanglestk( r2 );
      showtext2 ( mousekeynames[pos], r2.x1 + 1, r2.y1 + 1 );
      getinvisiblemouserectanglestk();
      npop ( activefontsettings );
   }
}


char tgamepreferences :: checkvalue( char id, void* p)  
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


void tgamepreferences :: buttonpressed ( char id )
{
   tdialogbox :: buttonpressed ( id );

   if ( id == 1 ) {
      gameoptions = actoptions;
      gameoptions.changed = 1;
      strcpy( gamepath, actgamepath );
      if ( gamepath[0] )
         if ( gamepath[strlen(gamepath)-1] != '\\' )
            strcat ( gamepath, "\\" );
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

   if ( id == 11 ) {
      gameoptions.defaultpassword = 0;
      enterpassword ( &actoptions.defaultpassword );
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
   tgamepreferences prefs;
   prefs.init();
   prefs.run();
   prefs.done();
}


typedef class tbaseitemlist* pbaseitemlist;
class tbaseitemlist {
             public:
                virtual void additem2 ( void* item ) = 0;
                virtual ~tbaseitemlist() {};
         };


typedef class tmountpicture* pmountpicture;

            class  tmountpicture : public tbaseitemlist {
                       protected:
                         int outofmem;
                         pfont fnt;

                         tgraphmodeparameters gmp;

                         struct tb {
                                      int x, y;
                                      int xs, ys;
                                      int xd;
                                      int num;
                                      char* name;
                                      void* item;
                                      int marked;

                                      tb* next;
                                   };
                         typedef tb* pb ;

                         pb  first;
                         pb  last;


                         int x1;
                         int y1;
                         int yspace;
                         int xsize;
                         void* buf;
                         int bufsize;
                         int itemnum;

                         virtual void putpict ( int n, int y ) ;
                         virtual void putpict ( pb n, int y ) = 0;

                          pb getpbfornum ( int n );


                          void dispimage ( void );

                          virtual pb add_item ( void* item );
                          void markitem ( pb i , int y );
                          void clearbuf ( void );


                       public:
                          int piclen;
                          int ysize;
                          int ystart;
                          int effpiclen;

                          tmountpicture ( int x_1, int y_1, int x_2, int y_2 );
                          tmountpicture ( tmouserect p );
                          void setcoordinates ( int x_1, int y_1, int x_2, int y_2 );
                          void setcoordinates ( tmouserect p );
                          void display ( void );
                          void newbuf ( int ys );
                          int getysize ( void );
                          void removeitem ( void* item );
                          void mountallimages ( void );
                          void addimage ( void );
                          virtual void additem2 ( void* item );
                          int  checkforexistence ( void* item );
                          void transfermarkeditems ( pbaseitemlist lst );
                          void transferallitems ( pbaseitemlist lst );
                          void removemarkeditems ( void );
                          void removeallitems ( void );
                          void unmarkitems( void );
                          virtual ~tmountpicture ( );
                          void processmouseclick ( void );
                   };



class tmultiplayersettings : public tdialogbox {
              protected:
                class tmountbuildingpictures : public tmountpicture {
                       protected:
                         virtual void putpict ( pb n, int y  );
                         virtual pb add_item ( void* item );
                       public:
                         tmountbuildingpictures ( tmouserect p );
                         tmountbuildingpictures ( int x_1, int y_1, int x_2, int y_2 );
                         void additem ( pbuildingtype a );
                         ~tmountbuildingpictures();
                };
                tmountbuildingpictures* buildinglist1;
                tmountbuildingpictures* buildinglist2;


                class tmountunitpictures : public tmountpicture {
                       protected:
                         virtual void putpict ( pb n, int y  );
                         virtual pb add_item ( void* item );
                       public:
                         tmountunitpictures ( tmouserect p );
                         tmountunitpictures ( int x_1, int y_1, int x_2, int y_2 );
                         void additem ( pvehicletype a );
                };
                tmountunitpictures* unitlist1;
                tmountunitpictures* unitlist2;


                class tmounttechnologypictures : public tmountpicture {
                       protected:
                         virtual void putpict ( pb n, int y  );
                         virtual pb add_item ( void* item );
                       public:
                         tmounttechnologypictures ( tmouserect p );
                         tmounttechnologypictures ( int x_1, int y_1, int x_2, int y_2 );
                         void additem ( ptechnology a );
                };
                tmounttechnologypictures* technologylist1;
                tmounttechnologypictures* technologylist2;

                
                class tmountterrainpictures : public tmountpicture {
                       protected:
                         virtual void putpict ( pb n, int y  );
                         virtual pb add_item ( void* item );
                       public:
                         tmountterrainpictures ( tmouserect p );
                         tmountterrainpictures ( int x_1, int y_1, int x_2, int y_2 );
                         void additem ( pterraintype a );
                };
                tmountterrainpictures* terrainlist1;
                tmountterrainpictures* terrainlist2;


                class tmountobjectcontainerpictures : public tmountpicture {
                       protected:
                         virtual void putpict ( pb n, int y  );
                         virtual pb add_item ( void* item );
                       public:
                         tmountobjectcontainerpictures ( tmouserect p );
                         tmountobjectcontainerpictures ( int x_1, int y_1, int x_2, int y_2 );
                         void additem ( pobjecttype a );
                };
                tmountobjectcontainerpictures* objectlist1;
                tmountobjectcontainerpictures* objectlist2;


                class taddcrcs : public tbaseitemlist {
                        protected:
                          int num;
                          dynamic_array<tcrc> crclist;
                        public:
                          taddcrcs ( void ) { num = 0; };
                          virtual void save ( pcrcblock c );
                };


                class taddbuildingcrcs : public taddcrcs {
                           public: 
                             void additem2 ( void* item );
                };
                class taddunitcrcs : public taddcrcs {
                           public: 
                             void additem2 ( void* item );
                };
                class taddterraincrcs : public taddcrcs{
                           public: 
                             void additem2 ( void* item );
                };
                class taddtechnologycrcs : public taddcrcs{
                           public: 
                             void additem2 ( void* item );
                };
                class taddobjectcrcs : public taddcrcs {
                           public: 
                             void additem2 ( void* item );
                };


                int techlevel;
                int replays;
                int status;
                pmountpicture leftlist;
                pmountpicture rightlist;

                pmountpicture allleftlists[5];
                pmountpicture allrightlists[5];


                void checkforscrollbar ( void );
                int leftscrollstatus;
                int rightscrollstatus;
                void toggleuseofchecksums  ( void );

                tmouserect dlgpos;
                tmouserect list1 , list2 ;
                tmouserect lscroll, rscroll;
                void hidescrollbar ( int id );
                void setcoordinates ( void );

                void paintcategories( void );
                void setcategory ( int n );

                int lim1, lim2, lim3;

                int actcategory;
                tmouserect categoryrect;

                int restricted[5];
                int checksumsused;

                void setcrcs ( void );
                int checklimits ( void );  // 0 : alles ok;   1: GebÑude fehlen  2: vehicle fehlen ...

                char* categoryname[5];

              public:
                void init ( void );
                void run ( void );
                void buttonpressed ( char id );

                ~tmultiplayersettings ( );
     };


tmountpicture  :: tmountpicture ( int x_1, int y_1, int x_2, int y_2 )
{
   setcoordinates ( x_1, y_1, x_2, y_2 );
   piclen = 0;
   buf = NULL;
   ystart = 0;
   piclen = 0;
   first = NULL;
   last = NULL;
   yspace = 5;
   fnt = schriften.smallarial;
   outofmem = 0;
   itemnum = 0;
}

void tmountpicture :: setcoordinates ( int x_1, int y_1, int x_2, int y_2 )
{
   xsize = x_2 - x_1;
   ysize = y_2 - y_1;

   x1 = x_1;
   y1 = y_1;

}

tmountpicture  :: tmountpicture ( tmouserect p )
{
   setcoordinates ( p );
   piclen = 0;
   buf = NULL;
   ystart = 0;
   piclen = 0;
   first = NULL;
   last = NULL;
   yspace = 5;
   fnt = schriften.smallarial;
   outofmem = 0;
   itemnum = 0;
}

void tmountpicture :: setcoordinates ( tmouserect p )
{
   setcoordinates ( p.x1, p.y1, p.x2, p.y2 );
}


tmountpicture  :: ~tmountpicture ( )
{
   if ( buf ) {
      delete[] buf;
      buf = NULL;
   }
   while ( first ) {
      pb i = first->next;
      delete first;
      first = i;
   }
}




void tmountpicture  :: dispimage ( void )
{
   if ( ystart + ysize > effpiclen ) {
      ystart = effpiclen - ysize;
      if ( ystart < 0 )
         ystart = 0;
   }

   if ( buf ) {
      setinvisiblemouserectanglestk ( x1, y1, x1+ xsize, y1 + ysize );
      if ( piclen < ysize ) {
         putimage ( x1, y1, buf );
         bar ( x1, y1 + piclen, x1 + xsize, y1 + ysize, dblue );
      } else {
         putimageprt ( x1, y1, x1 + xsize, y1 + ysize-1, buf, 0, ystart );
         /*
         if ( ystart + ysize > effpiclen )
            bar ( x1, effpiclen - ystart, x1 + xsize, y1 + ysize, dblue );*/
      } /* endif */
      getinvisiblemouserectanglestk ();
   }

   if ( outofmem ) {
      npush ( activefontsettings );
   
      activefontsettings.font = fnt;
      activefontsettings.background = dblue;
      activefontsettings.justify = centertext;
      activefontsettings.length = xsize - 10;
      activefontsettings.color = blue;
      showtext2 ( "out of memory", x1 + 5, y1 + 5 );
   
      npop ( activefontsettings );
   }
}


void tmountpicture  :: additem2 ( void* item )
{
   pb i = first;
   while ( i ) {
      if ( i->item == item )
         return;
      i = i->next;
   }

   add_item ( item );
   addimage ();
}


tmountpicture::pb tmountpicture  :: add_item ( void* item )
{
   pb i = first;
   while ( i ) {
      if ( i->item == item )
         return NULL;
      i = i->next;
   }
   


   i = new tb;

   i->next = NULL;

   i->item = item;
   i->marked = 0;
   i->next = NULL;
   i->num = itemnum;
   itemnum++;

   if ( last )
      last->next = i;

   last = i;

   if ( !first )
      first = i;

   return i;
}


void tmountpicture  :: removeitem ( void* item )
{
   pb i = first;
   if ( i->item == item ) {
      if ( last == i ) 
         last = NULL;

      first = i->next;
      delete i;
   } else {

      while ( i->next ) {
         if ( i->next->item == item ) {
            pb j = i->next;

            if ( !j->next )
               last = i;

            i->next = j->next;
            delete j;
         } else
            i = i->next;
      }
   }
   
}

tmountpicture::pb tmountpicture  :: getpbfornum ( int n )
{
   pb i = first;
   while ( n ) {
      if ( i )
         i = i->next;
      n--;
   }
   return i;
}


int tmountpicture :: checkforexistence ( void* item )
{
   pb i = first;
   while ( i ) {
      if ( i->item == item )
         return 1;

      i = i->next;
   }
   return 0;
}


void tmountpicture  :: newbuf ( int ys )
{
   if ( buf ) {
      delete[] buf;
      buf = NULL;
   }

   bufsize = imagesize ( 1, 1, xsize, ys );

   if ( bufsize > maxavail()   ||  outofmem ) {
      outofmem = 1;
      removeallitems();
      return;
   }

   buf = new char [ bufsize ];

   memset ( buf, dblue, bufsize );

   word* wp = (word*) buf;
   wp[0] = xsize - 1;
   wp[1] = ys - 1;

   gmp.resolutionx = xsize;
   gmp.resolutiony = ys;
   gmp.scanlinelength = xsize;
   gmp.scanlinenumber = ys;
   gmp.bytesperscanline = xsize;
   gmp.byteperpix = 1;
   piclen = ys;
   gmp.windowstatus = 100;
   gmp.linearaddress = ((int) buf) + 4;
}

void tmountpicture  :: clearbuf ( void )
{
   if ( buf ) {
      char* c = ((char*) buf) + 4;
      memset ( c, dblue, bufsize - 4);
   }
}


int tmountpicture  :: getysize ( void )
{
   int cnt = 0;
   int s = 0;
   pb i = first;
   while ( i ) {
      s += i->ys + yspace;
      i = i->next;
      cnt++;
   }
   s+= yspace;
   effpiclen = s;
   return s;
}

void tmountpicture  :: putpict ( int n, int y ) 
{
   putpict ( getpbfornum ( n ), y );
}



void tmountpicture  :: mountallimages ( void )
{
   if ( outofmem )
      return;

   int s = getysize ();
   if ( s > piclen )
      newbuf ( s );
   else
      clearbuf ();

   pb i = first;
   int y = yspace;

   npush ( *agmp );
   *agmp = gmp;

   while ( i ) {
      putpict ( i, y );
      if ( i->marked )
         xorrectangle ( 2, y + 1 , xsize - 2, y - 1 + i->ys, blue );

      y += i->ys + yspace;

      i = i->next;
   }

   npop ( *agmp );

}

void tmountpicture  :: display  ( void )
{
   int s = getysize ();
   if ( s > piclen )
      mountallimages();

   dispimage();
}




void tmountpicture  :: addimage ( void )
{
   if ( buf ) {
      int s = getysize ();
      if ( s > piclen ) {
         mountallimages();
      } else {
         int y = yspace;
         pb j = first;
         while ( j ) {
            if ( j->next ) 
               y += j->ys + yspace;
      
            j = j->next;
         }

         npush ( *agmp );
         *agmp = gmp;
         putpict ( last , y );
         npop ( *agmp );

      }
   }
}



void tmountpicture  :: markitem ( pb i, int y )
{

   npush ( *agmp );
   *agmp = gmp;

   xorrectangle ( 2, y + 1 - i->ys , xsize - 2, y - 1, blue );

   npop ( *agmp );

   dispimage();

}



void tmountpicture  :: processmouseclick ( void )
{
   if ( mouseparams.taste == 1 ) {
      tmouserect r;
      r.x1 = x1;
      r.x2 = x1 + xsize;

      int s = 0;
      pb i = first;
      while ( i ) {
         r.y1 = s + 1 + y1 - ystart;
         s += i->ys + yspace;
         r.y2 = s - 1 + y1 - ystart;

         if ( mouseinrect ( &r )) {
            i->marked = !i->marked;
            markitem ( i, s );
            while ( mouseinrect ( &r ) && (mouseparams.taste == 1 ));
         }

         i = i->next;
      }
   }
}



void tmountpicture :: transfermarkeditems ( pbaseitemlist lst )
{
   int t = 0;
   pb i = first;
   while ( i ) {
      if ( i->marked ) {
         lst->additem2 ( i->item );
         t++;
      }
      i = i->next;
   }
}


void tmountpicture :: transferallitems ( pbaseitemlist lst )
{
   int t = 0;
   pb i = first;
   while ( i ) {
      lst->additem2 ( i->item );
      t++;
      i = i->next;
   }
}


void tmountpicture :: unmarkitems( void )
{
   pb i = first;
   while ( i ) {
      i->marked = 0;
      i = i->next;
   }
   mountallimages();
}

void tmountpicture :: removemarkeditems ( void )
{
   pb i = first;
   while ( i ) {
      if ( i->marked ) {
         removeitem ( i->item );
         i = first;
      } else
         i = i->next;
   }
   mountallimages();
   display();
}

void tmountpicture :: removeallitems ( void )
{
   while (  first ) 
      removeitem ( first->item );

   mountallimages();
   display();
}


tmultiplayersettings :: tmountbuildingpictures :: tmountbuildingpictures ( int x_1, int y_1, int x_2, int y_2 )
                                                : tmountpicture ( x_1, y_1, x_2, y_2 )
{
}

tmultiplayersettings :: tmountbuildingpictures :: tmountbuildingpictures ( tmouserect p )
                                                : tmountpicture ( p )
{
}

tmultiplayersettings :: tmountbuildingpictures :: ~tmountbuildingpictures (  )
{
}

tmountpicture::pb tmultiplayersettings :: tmountbuildingpictures :: add_item ( void* item )
{
   pb i = tmountpicture :: add_item ( item );

   pbuildingtype bld = (pbuildingtype) item;
   i->name = bld->name;


   int minx = 10;
   int miny = 10;
   int maxx = 0;
   int maxy = 0;

   for ( int x = 0; x < 4; x++ ) 
      for ( int y = 0; y < 6; y++ )
         if ( bld->getpicture (x, y) ) {
            int x2 = 2 * x + ( y & 1 );
            if ( x2 > maxx )
               maxx = x2;
            if ( y > maxy )
               maxy = y;
            if ( x2 < minx )
               minx = x2;
            if ( y < miny )
               miny = y;
         }

   i->xs = (maxx - minx + 1) *  fielddisthalfx/2 + 5;
   i->ys = (maxy - miny + 2) *  fielddisty/2 + fnt->height;
   return i;
}


void tmultiplayersettings :: tmountbuildingpictures :: putpict ( pb n, int y  )
{
   char* pbuf;
   {
      tvirtualdisplay vfb ( 8 * fieldsizex, 8 * fieldsizey, 255 );
   
      pbuildingtype bld = (pbuildingtype) n->item;
   
      int minx = 1000;
      int miny = 1000;
      int maxx = 0;
      int maxy = 0;
   
      for ( int x = 0; x < 4; x++ ) 
         for ( int ry = 0; ry < 6; ry++ )
            if ( bld->getpicture ( x, ry ) ) {
   
               int xp = 5 + x * fielddistx + ( ry & 1 ) * fielddisthalfx ;
               int yp = 5 + ry * fielddisty;
   
               if ( xp > maxx )
                  maxx = xp;
               if ( yp > maxy )
                  maxy = yp;
               if ( xp < minx )
                  minx = xp;
               if ( yp < miny )
                  miny = yp;
   
               putspriteimage ( xp, yp,  bld->getpicture (x, ry ) );
            }
   
      maxx+= fieldsizex;
      maxy+= fieldsizey;
   
      pbuf = new char [imagesize ( minx, miny, maxx, maxy )];
      getimage ( minx, miny, maxx, maxy, pbuf );
   }

   putspriteimage ( (xsize - n->xs ) / 2, y, halfpict ( pbuf ));

   delete[] pbuf;
   pbuf = NULL;

   npush ( activefontsettings );

   activefontsettings.font = fnt;
   activefontsettings.background = dblue;
   activefontsettings.justify = centertext;
   activefontsettings.length = xsize - 10;
   activefontsettings.color = blue;
   showtext2 ( n->name, 5, y + n->ys - fnt->height );

   npop ( activefontsettings );
   
}


void tmultiplayersettings :: tmountbuildingpictures :: additem ( pbuildingtype a )
{
   additem2 ( a );
}




tmultiplayersettings :: tmountunitpictures :: tmountunitpictures ( int x_1, int y_1, int x_2, int y_2 )
                                                : tmountpicture ( x_1, y_1, x_2, y_2 )
{
}

tmultiplayersettings :: tmountunitpictures :: tmountunitpictures ( tmouserect p )
                                                : tmountpicture ( p )
{
}

tmountpicture::pb tmultiplayersettings :: tmountunitpictures :: add_item ( void* item )
{
   pb i = tmountpicture :: add_item ( item );

   pvehicletype fzt = (pvehicletype) item;
   if ( fzt->name  && fzt->name[0])
      i->name = fzt->name;
   else
      i->name = fzt->description;


   i->xs = fieldsizex + 5;
   i->ys = fieldsizey + fnt->height;
   return i;
}


void tmultiplayersettings :: tmountunitpictures :: putpict ( pb n, int y  )
{

   pvehicletype fzt = (pvehicletype) n->item;

   putspriteimage ( (xsize - n->xs ) / 2, y, fzt->picture[0] );

   npush ( activefontsettings );

   activefontsettings.font = fnt;
   activefontsettings.background = dblue;
   activefontsettings.justify = centertext;
   activefontsettings.length = xsize - 10;
   activefontsettings.color = blue;
   showtext2 ( n->name, 5, y + n->ys - fnt->height );

   npop ( activefontsettings );
   
}

void tmultiplayersettings :: tmountunitpictures :: additem ( pvehicletype a )
{
   additem2 ( a );
}



tmultiplayersettings :: tmounttechnologypictures :: tmounttechnologypictures ( int x_1, int y_1, int x_2, int y_2 )
                                                : tmountpicture ( x_1, y_1, x_2, y_2 )
{
}

tmultiplayersettings :: tmounttechnologypictures :: tmounttechnologypictures ( tmouserect p )
                                                : tmountpicture ( p )
{
}

tmountpicture::pb tmultiplayersettings :: tmounttechnologypictures :: add_item ( void* item )
{
   pb i = tmountpicture :: add_item ( item );

   ptechnology tec = (ptechnology) item;
   i->name = tec->name;

   if ( tec->icon ) {
      int width;
      int height;
      getpicsize ( tec->icon, width, height );

      if ( width + 5 > xsize ) {
         if ( width / 2 + 5  > xsize ) {
            i->xs = xsize - 5;
            i->ys = height / 2 + fnt->height;
         } else {
            i->xs = width / 2 + 5;
            i->ys = height / 2 + fnt->height;
         }
      }else {
         i->xs = width + 5;
         i->ys = height + fnt->height;
      }

   } else {
      i->xs = 0;
      i->ys = fnt->height;
   }

   return i;
}


void tmultiplayersettings :: tmounttechnologypictures :: putpict ( pb n, int y  )
{
   ptechnology tec = (ptechnology) n->item;

   if ( tec->icon ) {
      int width;
      int height;
      getpicsize ( tec->icon, width, height );

      if ( width + 5 > xsize ) {
         if ( width / 2 + 5  > xsize ) {
            putimageprt ( 3, y, xsize - 3, y + height/2, halfpict ( tec->icon ), (width/2 - (xsize - 6)) / 2, 0 );
            // halfpict ( tec->icon );
         } else {
            putimage ( (xsize - n->xs ) / 2, y, halfpict ( tec->icon ));
         }
      } else
         putimage ( (xsize - n->xs ) / 2, y, tec->icon );

   }

   npush ( activefontsettings );

   activefontsettings.font = fnt;
   activefontsettings.background = dblue;
   activefontsettings.justify = centertext;
   activefontsettings.length = xsize - 10;
   activefontsettings.color = blue;
   showtext2 ( n->name, 5, y + n->ys - fnt->height );

   npop ( activefontsettings );
   
}

void tmultiplayersettings :: tmounttechnologypictures :: additem ( ptechnology a )
{
   additem2 ( a );
}




tmultiplayersettings :: tmountterrainpictures :: tmountterrainpictures ( int x_1, int y_1, int x_2, int y_2 )
                                                : tmountpicture ( x_1, y_1, x_2, y_2 )
{
}

tmultiplayersettings :: tmountterrainpictures :: tmountterrainpictures ( tmouserect p )
                                                : tmountpicture ( p )
{
}

tmountpicture::pb tmultiplayersettings :: tmountterrainpictures :: add_item ( void* item )
{
   pb i = tmountpicture :: add_item ( item );

   pterraintype bdt = (pterraintype) item;
   i->name = bdt->name;
   i->xs = fieldxsize + 5;
   i->ys = fieldysize + fnt->height;

   return i;
}


void tmultiplayersettings :: tmountterrainpictures :: putpict ( pb n, int y  )
{

   pterraintype bdt = (pterraintype) n->item;

   int m = 0;
   while ( !bdt->weather[m] ) 
      m++;

   bdt->weather[m]->paint ( (xsize - n->xs ) / 2, y );

   npush ( activefontsettings );

   activefontsettings.font = fnt;
   activefontsettings.background = dblue;
   activefontsettings.justify = centertext;
   activefontsettings.length = xsize - 10;
   activefontsettings.color = blue;
   showtext2 ( n->name, 5, y + n->ys - fnt->height );

   npop ( activefontsettings );
   
}

void tmultiplayersettings :: tmountterrainpictures :: additem ( pterraintype a )
{
   additem2 ( a );
}




tmultiplayersettings :: tmountobjectcontainerpictures :: tmountobjectcontainerpictures ( int x_1, int y_1, int x_2, int y_2 )
                                                : tmountpicture ( x_1, y_1, x_2, y_2 )
{
}

tmultiplayersettings :: tmountobjectcontainerpictures :: tmountobjectcontainerpictures ( tmouserect p )
                                                : tmountpicture ( p )
{
}

tmountpicture::pb tmultiplayersettings :: tmountobjectcontainerpictures :: add_item ( void* item )
{
   pb i = tmountpicture :: add_item ( item );

   pobjecttype obj = (pobjecttype) item;
   i->name = obj->name;

   i->xs = fieldxsize + 5;
   i->ys = fieldxsize + fnt->height;
   return i;
}


void tmultiplayersettings :: tmountobjectcontainerpictures :: putpict ( pb n, int y  )
{

   pobjecttype obj = (pobjecttype) n->item;

   obj->display ( (xsize - n->xs ) / 2, y );

   npush ( activefontsettings );

   activefontsettings.font = fnt;
   activefontsettings.background = dblue;
   activefontsettings.justify = centertext;
   activefontsettings.length = xsize - 10;
   activefontsettings.color = blue;
   showtext2 ( n->name, 5, y + n->ys - fnt->height );

   npop ( activefontsettings );
   
}

void tmultiplayersettings :: tmountobjectcontainerpictures :: additem ( pobjecttype a )
{
   additem2 ( a );
}

void tmultiplayersettings :: taddcrcs :: save ( pcrcblock c )
{
   if ( num ) {
   
      if ( c->crc )
         delete c->crc;
   
      c->crc = new tcrc[num];
      for ( int i = 0; i < num; i++ )
         c->crc[i] = crclist[i];

      c->crcnum = num;
   }
}



void tmultiplayersettings :: taddbuildingcrcs:: additem2 ( void* item )
{
   pbuildingtype bld = (pbuildingtype) item;
   crclist[num].crc = getcrc ( bld );
   crclist[num].id  = bld->id;
   num++;
}


void tmultiplayersettings :: taddunitcrcs:: additem2 ( void* item )
{
   pvehicletype fzt = (pvehicletype) item;
   crclist[num].crc = getcrc ( fzt );
   crclist[num].id  = fzt->id;
   num++;
}

void tmultiplayersettings :: taddterraincrcs:: additem2 ( void* item )
{
   pterraintype bdt = (pterraintype) item;
   crclist[num].crc = getcrc ( bdt );
   crclist[num].id  = bdt->id;
   num++;
}

void tmultiplayersettings :: taddtechnologycrcs:: additem2 ( void* item )
{
   ptechnology fzt = (ptechnology) item;
   crclist[num].crc = getcrc ( fzt );
   crclist[num].id  = fzt->id;
   num++;
}

void tmultiplayersettings :: taddobjectcrcs :: additem2 ( void* item )
{
   pobjecttype obj = (pobjecttype) item;
   crclist[num].crc = getcrc ( obj );
   crclist[num].id  = obj->id;
   num++;
}




void tmultiplayersettings :: setcrcs ( void )
{
   displaymessage("calculating crcs", 0 );

   taddbuildingcrcs bldc;
   taddunitcrcs     fztc;
   taddtechnologycrcs tecc;
   taddterraincrcs  bdtc;
   taddobjectcrcs   objc;

   buildinglist2->transferallitems ( &bldc );
   unitlist2->transferallitems ( &fztc );
   terrainlist2->transferallitems ( &bdtc );
   technologylist2->transferallitems ( &tecc );
   objectlist2->transferallitems ( &objc );


   if ( !actmap->objectcrc )
      actmap->objectcrc = new tobjectcontainercrcs;

   bldc.save ( &actmap->objectcrc->building );
   fztc.save ( &actmap->objectcrc->unit );
   tecc.save ( &actmap->objectcrc->technology );
   bdtc.save ( &actmap->objectcrc->terrain );
   objc.save ( &actmap->objectcrc->object );

   if ( actmap->objectcrc->speedcrccheck )
      delete actmap->objectcrc->speedcrccheck;

   actmap->objectcrc->building.restricted = restricted[0];
   actmap->objectcrc->unit.restricted = restricted[1];
   actmap->objectcrc->technology.restricted = restricted[2];
   actmap->objectcrc->terrain.restricted = restricted[3];
   actmap->objectcrc->object.restricted = restricted[4];

   actmap->objectcrc->speedcrccheck = new tspeedcrccheck ( actmap->objectcrc );


  removemessage();
}

int tmultiplayersettings :: checklimits ( void )         // 0 : alles ok;   1: GebÑude fehlen  2: vehicle fehlen ...
{
   if ( checksumsused ) {

      if ( restricted[0] == 2 ) {
         for ( int s = 0; s < 9; s++ ) {
            pbuilding bld = actmap->player[s].firstbuilding;
            while ( bld ) {
               if ( !buildinglist2->checkforexistence ( bld->typ ))
                  return 1;

               bld = bld->next;
            } /* endwhile */
         }
      }


      if ( restricted[1] == 2 ) {
         int s;
         for ( s = 0; s < 9; s++ ) {
            pbuilding bld = actmap->player[s].firstbuilding;
            while ( bld ) {
               for ( int i = 0; i < 32; i++ ) {
                  if ( bld->loading[i] )
                     if ( !unitlist2->checkforexistence ( bld->loading[i]->typ ) )
                        return 2;

                  if ( bld->production[i] )
                     if ( !unitlist2->checkforexistence ( bld->production[i] ))
                        return 2;
               }
               bld = bld->next;
            } /* endwhile */
         }
         for ( s = 0; s < 8; s++ ) {
            pvehicle eht = actmap->player[s].firstvehicle;
            while ( eht ) {
               for ( int i = 0; i < 32; i++ ) {
                  if ( eht->loading[i] )
                     if ( !unitlist2->checkforexistence ( eht->loading[i]->typ ))
                        return 2;
               }
               if ( !unitlist2->checkforexistence ( eht->typ ))
                  return 2;
               eht = eht->next;
            } /* endwhile */
         }
      }


      if ( restricted[2] == 2 ) {
         int n = actmap->xsize * actmap->ysize;
         for ( int s = 0; s < n; s++ ) 
             if ( !terrainlist2->checkforexistence ( actmap->field[s].typ->terraintype ))
                return 3;
         
      } 

      if ( restricted[4] == 2 ) {
         int n = actmap->xsize * actmap->ysize;
         for ( int s = 0; s < n; s++ ) 
            if ( actmap->field[s].object )
               for ( int t = 0; t < actmap->field[s].object->objnum; t++ )
                 if ( !objectlist2->checkforexistence ( actmap->field[s].object->object[t]->typ ))
                    return 5;
         
      }

   }
   return 0;

}



void tmultiplayersettings :: buttonpressed ( char id )
{
   tdialogbox :: buttonpressed ( id );

   if ( id == 1 ) {             // OK
      int ck = checklimits() ;
      if ( ck ) 
         displaymessage ( "some %s exist on the map, but are not included in the crc list", 1, categoryname[ck-1]  );
      else {

         status = 2;
         if ( checksumsused )
            setcrcs();

         if ( techlevel )
            settechlevel ( techlevel, 0xff );

         if ( replays )
            actmap->replayinfo = new treplayinfo;
         else 
            if ( actmap->replayinfo ) {
               delete actmap->replayinfo;
               actmap->replayinfo = NULL;
            }

        }
   }

   if ( id == 2 )
      help(50);

   if ( id == 4 ) {
       taskforsupervisorpassword afsp;
       afsp.init ( &actmap->supervisorpasswordcrc, 0 );
       afsp.run( NULL );
       afsp.done();
   }

   if ( id == 5 ) 
      toggleuseofchecksums ();

   if ( id == 12 ) {
      lim2 = 0;
      lim3 = 0;
      enablebutton ( 13 );
      enablebutton ( 14 );
   }
   if ( id == 13 ) {
      lim1 = 0;
      lim3 = 0;
      enablebutton ( 12 );
      enablebutton ( 14 );
   }
   if ( id == 14 ) {
      lim1 = 0;
      lim2 = 0;
      enablebutton ( 12 );
      enablebutton ( 13 );
   }

   if ( id >= 12 && id <= 14 ) {
      if ( lim1 )
         restricted[actcategory] = 0;
      else
         if ( lim2 )
            restricted[actcategory] = 1;
         else
            if ( lim3 )
               restricted[actcategory] = 2;
   }

   if ( id == 20  ||  id == 40  ||  id == 60  ||  id == 80  ||  id == 100 ) {
      leftlist->display();
   }
   if ( id == 23  ||  id == 43  ||  id == 63  ||  id == 83  ||  id == 103 ) {
      rightlist->display();
   }

   if ( id == 30 ) {    // add all
      leftlist->transferallitems ( rightlist );
      rightlist->display();
      checkforscrollbar();
   }

   if ( id == 31 ) {    // add used
      if ( actcategory == 0 ) {       // buildings
         for ( int s = 0; s < 9; s++ ) {
            pbuilding bld = actmap->player[s].firstbuilding;
            while ( bld ) {
               buildinglist2->additem ( bld->typ );
               bld = bld->next;
            } /* endwhile */
         }
      } else
      if ( actcategory == 1 ) {     // units
         int s;
         for ( s = 0; s < 9; s++ ) {
            pbuilding bld = actmap->player[s].firstbuilding;
            while ( bld ) {
               for ( int i = 0; i < 32; i++ ) {
                  if ( bld->loading[i] )
                     unitlist2->additem ( bld->loading[i]->typ );
                  if ( bld->production[i] )
                     unitlist2->additem ( bld->production[i] );
               }
               bld = bld->next;
            } /* endwhile */
         }
         for ( s = 0; s < 8; s++ ) {
            pvehicle eht = actmap->player[s].firstvehicle;
            while ( eht ) {
               for ( int i = 0; i < 32; i++ ) {
                  if ( eht->loading[i] )
                     unitlist2->additem ( eht->loading[i]->typ );
               }
               unitlist2->additem ( eht->typ );
               eht = eht->next;
            } /* endwhile */
         }
      } else
      if ( actcategory == 3 ) {   // terrain
         int n = actmap->xsize * actmap->ysize;
         for ( int s = 0; s < n; s++ ) 
             terrainlist2->additem ( actmap->field[s].typ->terraintype );
         
      } else
      if ( actcategory == 4 ) {   // objects
         int n = actmap->xsize * actmap->ysize;
         for ( int s = 0; s < n; s++ ) 
            if ( actmap->field[s].object )
               for ( int t = 0; t < actmap->field[s].object->objnum; t++ )
                 objectlist2->additem ( actmap->field[s].object->object[t]->typ );
         
      }

      rightlist->display();
      checkforscrollbar();

   }

   if ( id == 32 ) {  // add marked
      leftlist->transfermarkeditems ( rightlist );
      leftlist->unmarkitems();
      leftlist->display();
      rightlist->display();
      checkforscrollbar();
   }

   if ( id == 33 ) {    // remove all
      rightlist->removeallitems();
      checkforscrollbar();
   }
   if ( id == 34 ) {    // remove marked
      rightlist->removemarkeditems();
      checkforscrollbar();
   }
}


void tmultiplayersettings :: paintcategories( void )
{
   npush ( activefontsettings );

   activefontsettings.font = schriften.smallarial;
   activefontsettings.justify = lefttext;
   activefontsettings.length = categoryrect.x2 - categoryrect.x1 - 10;

   setinvisiblemouserectanglestk ( categoryrect );

   for ( int i = 0; i < 5; i++ ) {
      if ( checksumsused ) {
         if ( i == actcategory )
            activefontsettings.color = red;
         else
            activefontsettings.color = black;
      } else
         activefontsettings.color = darkgray;

      showtext2 ( categoryname[i], categoryrect.x1 + 5, categoryrect.y1 + 5 + i * ( categoryrect.y2 - categoryrect.y1 - 10 ) / 5 );
   }
   getinvisiblemouserectanglestk ();

   npop ( activefontsettings );
}


void tmultiplayersettings :: setcategory ( int n )
{
   if ( lim1 )
      restricted[actcategory] = 0;
   else
      if ( lim2 )
         restricted[actcategory] = 1;
      else
         if ( lim3 )
            restricted[actcategory] = 2;


   if ( restricted[n] == 0 ) {
      lim1 = 1;
      lim2 = 0;
      lim3 = 0;
   } else
     if ( restricted[n] == 1 ) {
        lim1 = 0;
        lim2 = 1;
        lim3 = 0;
     } else
        if ( restricted[n] == 2 ) {
            lim1 = 0;
            lim2 = 0;
            lim3 = 1;
        }
        
   enablebutton ( 12 );
   enablebutton ( 13 );
   enablebutton ( 14 );



   actcategory = n;

   leftlist = allleftlists[n];
   rightlist = allrightlists[n];

   leftlist->display();
   rightlist->display();

 
   checkforscrollbar();
   checkforscrollbar();

}


void tmultiplayersettings :: toggleuseofchecksums ( void )
{
   int lnum  = leftscrollstatus / 10;
   int rnum  = rightscrollstatus / 10;

   if ( checksumsused ) {
      enablebutton ( 30 );
      enablebutton ( 31 );
      enablebutton ( 32 );
      enablebutton ( 33 );
      enablebutton ( 34 );

      enablebutton ( 12 );
      enablebutton ( 13 );
      enablebutton ( 14 );

      showbutton ( 20 + lnum * 20 );
      showbutton ( 23 + rnum * 20 );
      leftlist->display();
      rightlist->display();

   } else {
      disablebutton ( 30 );
      disablebutton ( 31 );
      disablebutton ( 32 );
      disablebutton ( 33 );
      disablebutton ( 34 );

      disablebutton ( 12 );
      disablebutton ( 13 );
      disablebutton ( 14 );

      hidescrollbar ( 20 + lnum * 20 );
      hidescrollbar ( 23 + rnum * 20 );

      bar ( list1 + dlgpos, dblue );
      bar ( list2 + dlgpos, dblue );
   }
   paintcategories();
}


void tmultiplayersettings :: setcoordinates ( void )
{
   dlgpos.x1 = x1;
   dlgpos.y1 = y1;
   dlgpos.x2 = x1;
   dlgpos.y2 = y1;
   buildinglist1->setcoordinates ( list1 + dlgpos );
   buildinglist2->setcoordinates ( list2 + dlgpos );

   unitlist1->setcoordinates ( list1 + dlgpos );
   unitlist2->setcoordinates ( list2 + dlgpos );

   objectlist1->setcoordinates ( list1 + dlgpos );
   objectlist2->setcoordinates ( list2 + dlgpos );

   terrainlist1->setcoordinates ( list1 + dlgpos );
   terrainlist2->setcoordinates ( list2 + dlgpos );

   technologylist1->setcoordinates ( list1 + dlgpos );
   technologylist2->setcoordinates ( list2 + dlgpos );

}

void tmultiplayersettings :: init ( void )
{
   static char* categorynames[5] = {"buildings", "units", "technologies", "terrain", "objects" };
   for ( int nn = 0; nn < 5; nn++ )
      categoryname[nn] = categorynames[nn];

   checksumsused = 0;

   lim1 = 1;
   lim2 = 0;
   lim3 = 0;
   techlevel = 0;

   actcategory = 0; 

   tdialogbox::init();
   x1 = 20;
   y1 = 10;
   xsize = 600;
   ysize = 460;
   title = "multiplayer settings";

   replays = 1;

   windowstyle &= ~dlg_in3d;

   addbutton ( "~O~K",    10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1 , 1, true );
   addkey ( 1, ct_enter );
   addbutton ( "help (~F1~)", xsize/2 + 5, ysize - 35, xsize -10, ysize - 10, 0, 1 , 2, true );
   addkey ( 2, ct_f1 );

   addbutton ( "use ~c~hecksums", 20, ysize - 80, 160, ysize - 65, 3, 0 , 5, true );
   addeingabe ( 5, &checksumsused, black, dblue );
   disablebutton ( 5 );

   addbutton ( "enable ~r~eplays", 170, ysize - 80, xsize-5, ysize - 65, 3, 0 , 8, true );
   addeingabe ( 8, &replays, black, dblue );


   addbutton ( "start with ~t~echlevel", xsize / 2 + 5 , ysize - 80, xsize - 130 , ysize - 60, 2, 1 , 3, true );
   addeingabe ( 3, &techlevel, 0, 255 );



   addbutton ( "supervisor", xsize - 120, ysize - 80, xsize - 20, ysize - 60, 0, 1 , 4, true );

   dlgpos.x1 = x1;
   dlgpos.y1 = y1;
   dlgpos.x2 = x1;
   dlgpos.y2 = y1;

   tmouserect rahmenoffs;
   rahmenoffs.x1 = -1;
   rahmenoffs.y1 = -1;
   rahmenoffs.x2 = 1;
   rahmenoffs.y2 = 1;

   int lwidth = 130;
   int lheight = 270;

   list1.x1 = 160;
   list1.y1 = starty + 35;
   list1.x2 = list1.x1 + lwidth;
   list1.y2 = list1.y1 + lheight;


   list2.x1 = 540 - lwidth;
   list2.y1 = list1.y1;
   list2.x2 = list2.x1 + lwidth;
   list2.y2 = list2.y1 + lheight;

   lscroll.x1 = list1.x2 + 10;
   lscroll.y1 = list1.y1;
   lscroll.x2 = list1.x2 + 25;
   lscroll.y2 = list1.y2;

   rscroll.x1 = list2.x2 + 10;
   rscroll.y1 = list2.y1;
   rscroll.x2 = list2.x2 + 25;
   rscroll.y2 = list2.y2;

   int buttx1 = list1.x2 + 30;
   int buttx2 = list2.x1 - 10;

   int i;

/////////////  GebÑude


   buildinglist1 = new tmultiplayersettings :: tmountbuildingpictures ( list1 + dlgpos );
   leftlist = buildinglist1;
   for (i = 0; i < buildingtypenum; i++) 
      if ( getbuildingtype_forpos( i ) )
         buildinglist1->additem ( getbuildingtype_forpos( i ) ); 

   addscrollbar ( lscroll, &buildinglist1->effpiclen, buildinglist1->ysize, &buildinglist1->ystart, 20, 0 );
   setscrollspeed ( 20 , 1 );
   if ( buildinglist1->getysize() <= lheight ) { 
      hidebutton ( 20 );
      leftscrollstatus = 0;
   } else
      if ( checksumsused )
         leftscrollstatus = 1;
      else {
         leftscrollstatus = 1;
         hidebutton ( 20 );
      }

   allleftlists[0] = buildinglist1;
           

   buildinglist2 = new tmultiplayersettings :: tmountbuildingpictures ( list2 + dlgpos );
   rightlist = buildinglist2;
   for (i = 0; i < buildingtypenum; i++) 
      if ( getbuildingtype_forpos( i ) )
         buildinglist2->additem ( getbuildingtype_forpos( i ) ); 

   addscrollbar ( rscroll, &buildinglist2->effpiclen, buildinglist2->ysize, &buildinglist2->ystart, 23, 0 );
   setscrollspeed ( 23 , 1 );
   if ( buildinglist2->getysize() <= lheight ) {
      hidebutton ( 23 );
      rightscrollstatus = 0;
   } else
      if ( checksumsused )
         rightscrollstatus  = 1;
      else {
         rightscrollstatus  = 1;
         hidebutton ( 23 );
      }

   allrightlists[0] = buildinglist2;



/////////////  vehicle

   unitlist1 = new tmultiplayersettings :: tmountunitpictures ( list1 + dlgpos );
   for (i = 0; i < vehicletypenum; i++) 
      if ( getvehicletype_forpos( i ) )
         unitlist1->additem ( getvehicletype_forpos( i ) ); 

   addscrollbar ( lscroll, &unitlist1->effpiclen, unitlist1->ysize, &unitlist1->ystart, 40, 0 );
   setscrollspeed ( 40 , 1 );
   hidebutton ( 40 );

   allleftlists[1] = unitlist1;


   unitlist2 = new tmultiplayersettings :: tmountunitpictures ( list2 + dlgpos );
   for (i = 0; i < vehicletypenum; i++) 
      if ( getvehicletype_forpos( i ) )
         unitlist2->additem ( getvehicletype_forpos( i ) ); 

   addscrollbar ( rscroll, &unitlist2->effpiclen, unitlist2->ysize, &unitlist2->ystart, 43, 0 );
   setscrollspeed ( 43 , 1 );
   hidebutton ( 43 );

   allrightlists[1] = unitlist2;


/////////////  Technologien

   technologylist1 = new tmultiplayersettings :: tmounttechnologypictures ( list1 + dlgpos );
   for (i = 0; i < technologynum; i++)
      if ( gettechnology_forpos( i ) )
         technologylist1->additem ( gettechnology_forpos( i ) ); 

   addscrollbar ( lscroll, &technologylist1->effpiclen, technologylist1->ysize, &technologylist1->ystart, 60, 0 );
   setscrollspeed ( 60 , 1 );
   hidebutton ( 60 );

   allleftlists[2] = technologylist1;

   technologylist2 = new tmultiplayersettings :: tmounttechnologypictures ( list2 + dlgpos );
   for (i = 0; i < technologynum; i++)
      if ( gettechnology_forpos( i ) )
         technologylist2->additem ( gettechnology_forpos( i ) ); 

   addscrollbar ( rscroll, &technologylist2->effpiclen, technologylist2->ysize, &technologylist2->ystart, 63, 0 );
   setscrollspeed ( 60 , 1 );
   hidebutton ( 63 );

   allrightlists[2] = technologylist2;



/////////////  terrain

   terrainlist1 = new tmultiplayersettings :: tmountterrainpictures ( list1 + dlgpos );
   for (i = 0; i < terraintypenum; i++) 
      if ( getterraintype_forpos( i ) )
         terrainlist1->additem ( getterraintype_forpos( i ) ); 

   addscrollbar ( lscroll, &terrainlist1->effpiclen, terrainlist1->ysize, &terrainlist1->ystart, 80, 0 );
   setscrollspeed ( 80 , 1 );
   hidebutton ( 80 );

   allleftlists[3] = terrainlist1;


   terrainlist2 = new tmultiplayersettings :: tmountterrainpictures ( list2 + dlgpos );
   for (i = 0; i < terraintypenum; i++) 
      if ( getterraintype_forpos( i ) )
         terrainlist2->additem ( getterraintype_forpos( i ) ); 

   addscrollbar ( rscroll, &terrainlist2->effpiclen, terrainlist2->ysize, &terrainlist2->ystart, 83, 0 );
   setscrollspeed ( 83 , 1 );
   hidebutton ( 83 );

   allrightlists[3] = terrainlist2;


/////////////  Objectlayer

   objectlist1 = new tmultiplayersettings :: tmountobjectcontainerpictures ( list1 + dlgpos );
   for (i = 0; i < objecttypenum; i++)
      if ( getobjecttype_forpos( i ) )
         objectlist1->additem ( getobjecttype_forpos( i ) ); 

   addscrollbar ( lscroll, &objectlist1->effpiclen, objectlist1->ysize, &objectlist1->ystart, 100, 0 );
   setscrollspeed ( 100 , 1 );
   hidebutton ( 100 );

   allleftlists[4] = objectlist1;


   objectlist2 = new tmultiplayersettings :: tmountobjectcontainerpictures ( list2 + dlgpos );
   for (i = 0; i < objecttypenum ; i++)
      if ( getobjecttype_forpos( i ) )
         objectlist2->additem ( getobjecttype_forpos( i ) ); 

   addscrollbar ( rscroll, &objectlist2->effpiclen, objectlist2->ysize, &objectlist2->ystart, 103, 0 );
   setscrollspeed ( 103 , 1 );
   hidebutton ( 103 );

   allrightlists[4] = objectlist2;



   addbutton ( "all >>", buttx1, list1.y1 , buttx2, list1.y1 + 20, 0, 1, 30, checksumsused );
   addbutton ( "used >>", buttx1, list1.y1 + 30, buttx2, list1.y1 + 50, 0, 1, 31, checksumsused );
   addbutton ( "marked >>", buttx1, list1.y1 + 60, buttx2, list1.y1 + 80, 0, 1, 32, checksumsused );


   addbutton ( "<< all", buttx1, list1.y1 + 90, buttx2, list1.y1 + 110, 0, 1, 33, checksumsused );
   addbutton ( "<< marked", buttx1, list1.y1 + 120, buttx2, list1.y1 + 140, 0, 1, 34, checksumsused );

   categoryrect.x1 = 30;
   categoryrect.y1 = list1.y1;
   categoryrect.x2 = 150;
   categoryrect.y2 = categoryrect.y1 + 110;

   addbutton ( "", categoryrect.x1, categoryrect.y2 + 30, categoryrect.x1 + 15, categoryrect.y2 + 45, 3, 10, 12, checksumsused );
   addeingabe ( 12, &lim1, black, dblue );

   addbutton ( "", categoryrect.x1, categoryrect.y2 + 80, categoryrect.x1 + 15, categoryrect.y2 + 95, 3, 10, 13, checksumsused );
   addeingabe ( 13, &lim2, black, dblue );

   addbutton ( "", categoryrect.x1, categoryrect.y2 + 130, categoryrect.x1 + 15, categoryrect.y2 + 145, 3, 10, 14, checksumsused );
   addeingabe ( 14, &lim3, black, dblue );

   buildgraphics();

   setcoordinates();

   categoryrect = categoryrect + dlgpos;

   activefontsettings.font = schriften.smallarial;
   activefontsettings.justify = centertext;
   activefontsettings.background = 255;
   activefontsettings.color = black;
   activefontsettings.length = lwidth;
   showtext2 ( "items available", list1.x1 + dlgpos.x1, list1.y1 + dlgpos.y1 - activefontsettings.font->height - 2 );
   showtext2 ( "CRCs included",   list2.x1 + dlgpos.x1, list2.y1 + dlgpos.y1 - activefontsettings.font->height - 2 );




   activefontsettings.justify = lefttext;
   showtext2 ( "no limit", categoryrect.x1 + 25, categoryrect.y2 + 20 );
   showtext2 ( "no include", categoryrect.x1 + 25, categoryrect.y2 + 40 );

   showtext2 ( "no limit", categoryrect.x1 + 25, categoryrect.y2 + 70 );
   showtext2 ( "include", categoryrect.x1 + 25, categoryrect.y2 + 90 );

   showtext2 ( "limited", categoryrect.x1 + 25, categoryrect.y2 + 130 );

   rahmen ( true, x1 + 10, y1 + starty , x1 + xsize - 10, y1 + ysize - 45 );
   rahmen3 ( "checksums", x1 + 20, y1 + starty + 10, x1 + xsize - 20, y1 + ysize - 100, 1 );

   rahmen ( true, categoryrect );

   rahmen ( true, list1 + dlgpos + rahmenoffs );
   rahmen ( true, list2 + dlgpos + rahmenoffs );

   if ( checksumsused ) {
      buildinglist1->display ();
      buildinglist2->display ();
   }

   paintcategories();

   status = 0;

   for ( int j = 0; j < 5; j++ )
      restricted[j] = 0;

};

void tmultiplayersettings :: hidescrollbar ( int id )
{
   if ( id % 10  ==  0 )
      bar ( lscroll + dlgpos , dblue );
   else
      bar ( rscroll + dlgpos , dblue );
   hidebutton ( id );
}


void tmultiplayersettings :: checkforscrollbar ( void )
{
   int mode = leftscrollstatus % 10;
   int num  = leftscrollstatus / 10;


   if ( num == actcategory ) {
      if ( mode == 1  &&  leftlist->ysize >= leftlist->effpiclen ) {
         hidescrollbar ( 20 + num * 20 );
         leftscrollstatus = num * 10;
      }
         
      if ( mode == 0  &&  leftlist->ysize < leftlist->effpiclen ) {
         showbutton ( 20 + num * 20 );
         leftscrollstatus = num * 10 + 1;
      }

   } else {
      hidescrollbar ( 20 + num * 20 );

      leftlist = allleftlists[actcategory];

      leftscrollstatus = actcategory * 10;
   }



   mode = rightscrollstatus % 10;
   num  = rightscrollstatus / 10;

   if ( num == actcategory ) {
      if ( mode == 1  &&  rightlist->ysize >= rightlist->effpiclen ) {
         hidescrollbar ( 23 + num * 20 );
         rightscrollstatus = num * 10;
      }
         
      if ( mode == 0  &&  rightlist->ysize < rightlist->effpiclen ) {
         showbutton ( 23 + num * 20 );
         rightscrollstatus = num * 10 + 1;
      }

   } else {
      hidescrollbar ( 23 + num * 20 );

      rightlist = allrightlists[actcategory];

      rightscrollstatus = actcategory * 10;
   }

}

void tmultiplayersettings :: run ( void )
{
   mousevisible ( true );
   do {
      tdialogbox::run();

      /*
      if ( _heapchk() != _HEAPOK )
         displaymessage(" Heap not OK", 1 );
      */

      if ( mouseparams.taste == 1 && checksumsused ) {
         tmouserect ll = list1 + dlgpos;
         tmouserect rl = list2 + dlgpos;
         if ( mouseinrect ( &ll ))
            leftlist->processmouseclick();

         if ( mouseinrect ( &rl ))
            rightlist->processmouseclick();

         if ( mouseinrect ( &categoryrect ) )
            for ( int i = 0; i < 5; i++ ) {
               int yp = categoryrect.y1 + 5 + i * ( categoryrect.y2 - categoryrect.y1 - 10 ) / 5 ;
               if ( mouseparams.y >= yp  &&  mouseparams.y <= yp+schriften.smallarial->height  &&  i != actcategory ) {
                  setcategory ( i );
                  paintcategories();
               }
            }


      } 
   } while ( !status ); /* enddo */
}


tmultiplayersettings :: ~tmultiplayersettings ( )
{
   if ( buildinglist1 ) {
      delete buildinglist1;
      buildinglist1 = NULL;
   }
   if ( buildinglist2 ) {
      delete buildinglist2;
      buildinglist2 = NULL;
   }

   if ( unitlist1 ) {
      delete unitlist1;
      unitlist1 = NULL;
   }
   if ( unitlist2 ) {
      delete unitlist2;
      unitlist2 = NULL;
   }

   if ( terrainlist1 ) {
      delete terrainlist1;
      terrainlist1 = NULL;
   }
   if ( terrainlist2 ) {
      delete terrainlist2;
      terrainlist2 = NULL;
   }


   if ( objectlist1 ) {
      delete objectlist1;
      objectlist1 = NULL;
   }
   if ( objectlist2 ) {
      delete objectlist2;
      objectlist2 = NULL;
   }

   if ( technologylist1 ) {
      delete technologylist1;
      technologylist1 = NULL;
   }
   if ( technologylist2 ) {
      delete technologylist2;
      technologylist2 = NULL;
   }
}



void multiplayersettings ( void )
{
   tmultiplayersettings mps;
   mps.init();
   mps.run();
   mps.done();
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
             void buttonpressed ( char id );
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
      showtext2 ( actmap->player[ply[i]].name, x1 + 60, y1 + starty + xs+17 + i * 40 - activefontsettings.font->height / 2 );
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
      if ( actmap->player[i].existent )
         if ( i != actmap->actplayer ) 
            ply[num++] = i;

   if ( fld->building )
      ply[num++] = 8;

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


   markedplayer = -1;
   status = 0;


   buildgraphics(); 


   for ( int j = 0; j < num; j++ )
      paintplayer( j );

} 

void         tgiveunitawaydlg :: buttonpressed ( char id )
{
   if ( id == 1 )
      if ( markedplayer >= 0 )
         status = 12;
   if ( id == 2 )
      status = 10;
}


void tgiveunitawaydlg :: run ( void )
{
   while ( mouseparams.taste );

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
      computeview();
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
      while ( mouseparams.taste );
   }
}


