/*! \file gamedlg.cpp    
    \brief Tons of dialog boxes which are used in ASC only (and not in the mapeditor)
*/
/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2003  Martin Bickel  and  Marc Schellenberger

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
#include <cstring>
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
#include "itemrepository.h"
#include "strtmesg.h"


class   tchoosetechnology : public tdialogbox {
                           typedef vector<const Technology*> Techs;
                           Techs         techs;
                           bool          endDialog;

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
   endDialog = false;
   tdialogbox::init();
   title = "research";

   firstshowntech = 0;
   dispnum = 10;
   markedtech = 0;

   check();
   if ( technum > dispnum )
      addscrollbar  ( xsize - 40, 70, xsize - 20, ysize - 40, &technum, dispnum, &firstshowntech, 1, 0 );

   addbutton ( "~O~k", 10, ysize - 30, xsize - 10, ysize - 10 , 0, 1, 10, true );
   addkey ( 10, ct_enter );
   addkey ( 10, ct_space );


/*   activefontsettings.font = schriften.smallarial;
   activefontsettings.justify = lefttext;
   showtext2("choose technology to research :", x1 + 10, y1 + starty + 10); */
}

bool techComp ( const Technology* t1, const Technology* t2 )
{
   return t1->techlevel < t2->techlevel;
}

void         tchoosetechnology::check(void)
{
   technum = 0;
   Research* resrch = &actmap->player[actmap->actplayer].research;

   for (int i = 0; i < technologyRepository.getNum(); i++) {
      const Technology* tech = technologyRepository.getObject_byPos( i );
      if ( tech ) {
         Research::AvailabilityStatus a = resrch->techAvailable ( tech );
         if ( a == Research::available ) {
            techs.push_back ( tech );
            technum++;
         }
      }
   }
   sort ( techs.begin(), techs.end(), techComp );
}


int          tchoosetechnology::gx ( void )
{
   return x1 + 20;
}

int          tchoosetechnology::gy ( int i )
{
   return y1 + starty + 40 + ( i - firstshowntech ) * 25;
}

void         tchoosetechnology::disp(void)
{
   activefontsettings.font = schriften.smallarial;
   activefontsettings.justify = lefttext;
   activefontsettings.length = 300;
   for ( int i = firstshowntech; i < technum && i < firstshowntech + dispnum; i++ ) {
      if ( i == markedtech )
          activefontsettings.background = 25;
      else
          activefontsettings.background = dblue;

      int x = gx();
      int y = gy(i);

      activefontsettings.length = 250;
      showtext2(techs[i]->name, x,y ) ;

      activefontsettings.length = 60;
      showtext2( strrr( techs[i]->researchpoints), x + 260,y ) ;


      /*
      if ( techs[i]->relatedUnitID > 0 ) {
         Vehicletype* vt = actmap->getvehicletype_byid( techs[i]->relatedUnitID );
         activefontsettings.length = 70;
         if ( vt )
            showtext2("(tech info)", x1 + xsize - 130,y ) ;
      }
      */


      activefontsettings.length = 40;
      if ( techs[i]->relatedUnitID > 0 && actmap->getvehicletype_byid( techs[i]->relatedUnitID )) {
         Vehicletype* vt = actmap->getvehicletype_byid( techs[i]->relatedUnitID );
         showtext2("(O)", x1 + xsize - 90,y ) ;
      } else
         showtext2(" ", x1 + xsize - 90,y ) ;

   }
}

void         tchoosetechnology::buttonpressed ( int id )
{
   if ( id == 1 )
      disp();
   if ( id == 10 )
      endDialog = true;
}


void         tchoosetechnology::run(void)
{

  if ( !technum && !actmap->player[actmap->actplayer].research.techsAvail )
     return;

  actmap->player[actmap->actplayer].research.techsAvail = true;

  buildgraphics();
  markedbackgrnd = lightblue;
  disp();

  npush ( activefontsettings.color );
   activefontsettings.background = black;
   activefontsettings.color = white;
   activefontsettings.font = schriften.smallarial;
   activefontsettings.justify = lefttext;
   activefontsettings.length = xsize + x1 - gx() - 30;
   showtext2( "Technology", gx(), y1+starty+10 ) ;

   activefontsettings.background = 255;
   showtext2( "Points", gx()+260, y1+starty+10 ) ;

//   showtext2( "TechInfo", gx()+320, y1+starty+10 ) ;
   showtext2( "UnitInfo", x1 + xsize - 110, y1+starty+10 ) ;

  npop ( activefontsettings.color );

  mousevisible(true);
  if (technum == 0) {
     showtext2("no further technologies to develop !", x1 + 20, y1 + starty + 30);
     actmap->player[actmap->actplayer].research.activetechnology = NULL;
     do {
        tdialogbox::run();
     } while (!endDialog);
     actmap->player[actmap->actplayer].research.techsAvail = false;
  }
  else {
     do {
        tdialogbox::run();
        int oldmark = markedtech;
        if ( mouseparams.taste == 1 ) {
            for ( int i = firstshowntech; i < technum && i < firstshowntech + dispnum; i++ ) {
               int x = gx();
               int y = gy(i);
               if ( mouseinrect ( x1 + xsize - 90, y, x1 + xsize - 50, y + 20  ) ) {
                  if ( techs[i]->relatedUnitID > 0 ) {
                     Vehicletype* vt = actmap->getvehicletype_byid( techs[i]->relatedUnitID );
                     if ( vt ) {
                        while ( mouseparams.taste == 1 )
                           releasetimeslice();

                        vehicle_information ( vt );
                     }
                  }
               }

               if ( mouseinrect ( x, y, x1 + xsize - 100, y + 20  ) ){
                  if ( markedtech == i )
                     endDialog = true;
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

     }  while ( !endDialog );

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
         strcpy ( maptitle, spfld->maptitle.c_str());
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
         strcpy ( maptitle, spfld->maptitle.c_str());
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

          int w = spfldloader.stream->readWord();

          if ( w != fileterminator )
             throw tinvalidversion ( filename.c_str(), w, fileterminator );

          int version =  spfldloader.stream->readInt();

         if (version > actmapversion || version < minmapversion )
            throw tinvalidversion ( filename.c_str(), version, actmapversion );

          spfldloader.readmap ();
          // spfldloader.readeventstocome();

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


void         tnewcampaignlevel::showmapinfo( int ypos)
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
         computeview( actmap );

         do {
           next_turn();
           if ( actmap->time.turn() == 2 ) {
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
      if ( err.expected < err.found )
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nPlease install the latest version from www.asc-hq.org", 1, err.getFileName().c_str(), err.expected, err.found );
      else
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nThis is a bug, please report it!", 1, err.getFileName().c_str(), err.expected, err.found );
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

void         tcontinuecampaign::setid( int id)
{
   idsearched = id;
}


void         tcontinuecampaign::showmapinfo( int ypos)
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
                  strcpy ( maptitle, spfld->maptitle.c_str() );

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
               strcpy ( maptitle, spfld->maptitle.c_str());
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

/*   addbutton("~s~ave recovery information",leftspace,270,xsize - leftspace,290,0,1,2,true);
   disablebutton ( 2 ); */

   addbutton("~g~o !",leftspace,300, xsize - leftspace, 330,0,1,1,true);

//   addbutton("~q~uit~ !", ( xsize + leftspace ) /2 ,300, xsize - leftspace , 330,0,1,3,true);

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

            fileselectsvga("*.rcy", t, true );
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

/*
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
*/
   tmemorystream  memoryStream ( &memoryStreamBuffer, tnstream::writing );
   for (int i = 0; i<8; i++) {
      map->player[i].research.write ( memoryStream );
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

      tmap* oldmap = actmap;
      actmap = NULL;

      loadcampaignmap();
      // actmap->oldevents = oldevent;
      tmemorystream  memoryStream ( &memoryStreamBuffer, tnstream::reading );

      for (i=0;i<8 ; i++) {
         actmap->player[i].research.read_struct ( memoryStream );
         actmap->player[i].research.read_techs ( memoryStream );
         actmap->player[ i ].dissections = dissectedunits[i];
      }

   } else
      if ( status == 21 ) {
         delete actmap->campaign;
         actmap->campaign = NULL;
         actmap->continueplaying = 1;
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

       int w = spfldloader.stream->readWord();

       if ( w != fileterminator )
          throw tinvalidversion ( mapname, w, fileterminator );

       int version = spfldloader.stream->readInt();

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
            fileselectsvga( mapextension, t, true );
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
   addeingabe(1, password,50,50);

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
         strcpy ( maptitle, spfld->maptitle.c_str());
         status = 7;
      }
   }
   else {
      strcpy( mapinfo, dateiinfo );
      strcpy ( maptitle, spfld->maptitle.c_str());
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
         strcpy ( maptitle, spfld->maptitle.c_str());
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
           if ( actmap->time.turn() == 2 ) {
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
         if ( err.expected < err.found )
            displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nPlease install the latest version from www.asc-hq.org", 1, err.getFileName().c_str(), err.expected, err.found );
         else
            displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nThis is a bug, please report it!", 1, err.getFileName().c_str(), err.expected, err.found );

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
   if ( oldplayer != -1 )  
      xorrectangle ( xx1, yy1, xx2, yy2, 14 );

   yy1 = y1 + starty + 25 + player * 30;
   yy2 = yy1 + 30;
   if ( player != -1 )
      xorrectangle ( xx1, yy1, xx2, yy2, 14 );

   oldplayer = player;
}

void  ttributepayments :: paintvalues ( void )
{
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
   showtext2 ( "research points:", textxpos, y1 + 140 );
   showtext2 ( "research points done:", textxpos, y1 + 170 );
   showtext2 ( "research points still needed:", textxpos, y1 + 200 );
   showtext2 ( "research done ( % ):", textxpos, y1 + 230 );
   showtext2 ( "research points / turn:", textxpos, y1 + 260 );
   showtext2 ( "time left (turns):", textxpos, y1 + 290 );


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
{  {
      tresearchinfo ri;
      ri.init();
      ri.run();
      ri.done();
   }
   {

   ASCString s;
   for ( vector<int>::iterator i = actmap->player[actmap->actplayer].research.developedTechnologies.begin(); i != actmap->player[actmap->actplayer].research.developedTechnologies.end(); ++i ) {
      Technology* t = technologyRepository.getObject_byID( *i );
      if ( t )
         s += t->name + "\n";
   }


   tviewanytext vat ;
   vat.init ( "Developed Technologies", s.c_str(), 20, -1 , 450, 480 );
   vat.run();
   vat.done();

   }
   if ( skeypress( ct_lshift)) {
      ASCString s = actmap->player[actmap->actplayer].research.listTriggeredTechAdapter();

      tviewanytext vat ;
      vat.init ( "Triggered TechAdapter", s.c_str(), 20, -1 , 450, 480 );
      vat.run();
      vat.done();
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
      const char* str = getmessage ( messagenum );
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
      asc_free( image );
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

   addbutton ( "", xsize -35, starty + 50, xsize - 20, starty + 65, 3, 0, 4, true );
   addeingabe ( 4, &actoptions.automaticTraining, 0, dblue );


//   addbutton ( "", xsize -35, starty + 50, xsize - 20, starty + 65, 3, 0, 4, true );
//   addeingabe ( 4, &actoptions.sound.Emute, 0, dblue );


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
   showtext2 ( "automatic training of units", x1 + 25, y1 + starty + 50 );
/*
#ifndef _DOS_
   showtext2 ( "disable sound",                                 x1 + 25, y1 + starty + 50 );
#endif
*/
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
   tgamepreferences prefs;
   prefs.init();
   prefs.run();
   prefs.done();
}


const int mousebuttonnum = 3;

const char* mousebuttonnames[mousebuttonnum] = { "left", "right", "center" };


class tmousepreferences : public tdialogbox {
                        CGameOptions actoptions;
                        int status;
                        tmouserect r1, r2, r3, ydelta;
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

   r3 = r2 + ydelta;

   addbutton ( "", xsize -35, r3.y1 , r3.x2, r3.y2, 3, 0, 11, true );
   addeingabe ( 11, &actoptions.mouse.singleClickAction , 0, dblue  );


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

   showtext2 ( "single click for field select and menu",  x1 + 25, r3.y1 + dlgoffset.y1  );


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
      showtext2 ( smallguiiconsundermouse[actoptions.mouse.smalliconundermouse], r2.x1 + 1, r2.y1 + 1 );
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
      int d = actmap->getgameparameter(GameParameter(i)) != gameparameterdefault[i];

      if ( d )
         s+= "#color4#";

      s += gameparametername[i];
      s += " = ";
      s += strrr ( actmap->getgameparameter(GameParameter(i)) );

      if ( d )
         s+= "#color0#";
      s += "\n";
   }
}

void sendGameParameterAsMail ( void )
{
   ASCString s;
   writeGameParametersToString ( s );

   if ( actmap->getgameparameter( cgp_superVisorCanSaveMap) ) {
      s = "#color4#Warning ! This game has been started with the option enabled that allows the Supervisor to save the map. This will allow him to spy on everything. \n"
           "If you did not initiate this game yourself, don't play with this option unless you were specifically informed why this option had to be enabled. \n"
           "#color0# \n" + s;
   }

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
         actmap->replayinfo = new tmap::ReplayInfo;
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

   addbutton ( "~s~ave password as default supervisor passw.",  20, 280, xsize / 2 - 5 , 300, 0, 1 , 12, false );

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

         showtext2( text,x1 + 20, y1 + firstliney + pos * abstand);

         rahmen(true, x1 + startx - 11, y1 + firstliney - 2 + pos * abstand, x1 + startx + llength + 11,y1 + firstliney + 18 + pos * abstand);
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
                            step = int( pow ( 10, log10 (double(step))));

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

            newpos[i] = int(p * target->service[displayed[i]].maxAmount);

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
   serviceAction->execute ( source, targetNWID, -1, 2, displayed[pos], newpos[pos] );
   zeichneschieberegler ( -1 );
}


void         verlademunition( VehicleService* serv, int targetNWID )
{
   tverlademunition vlm;
   vlm.init( serv->getVehicle(), targetNWID, serv );
   vlm.run();
   vlm.done(); 
}

void showPlayerTime()
{
   ASCString text;

   int p = actmap->actplayer;
   for ( int i = 0; i < 8; i++ ) {
       if ( actmap->player[p].exist() && actmap->player[p].playTime.size() ) {
          text += actmap->player[p].getName();
          text += " completed turn no. ";
          text += strrr(actmap->player[p].playTime.rbegin()->turn);
          text += " using ASC ";
          text += getVersionString( actmap->player[p].ASCversion );
          text += "\non ";
          text += asctime ( localtime ( &actmap->player[p].playTime.rbegin()->date));
          text += "\n";
       }


       ++p;
       if ( p >= 8 )
          p = 0;
   }

   tviewanytext vat ;
   vat.init ( "Play Time Information", text.c_str(), 20, -1 , 530, 480 );
   vat.run();
   vat.done();
}

void displaywindspeed( void )
{
   displaymessage2("wind speed is %d which equals %s fields / turn", actmap->weather.windSpeed, strrrd8d ( actmap->weather.windSpeed * maxwindspeed / 256 ));
}



#if 0
class SoundPreferences : public tdialogbox {
                        int musicVolume;
                        int effectsVolume;
                        int music
                        int status;

                     public:
                        void init ( void );
                        void buttonpressed ( int id );
                        void run ( void );
                    };


void SoundPreferences :: init ( void )
{
*CGameOptions::Instance()

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

   addbutton ( "", xsize -35, starty + 50, xsize - 20, starty + 65, 3, 0, 4, true );
   addeingabe ( 4, &actoptions.automaticTraining, 0, dblue );


//   addbutton ( "", xsize -35, starty + 50, xsize - 20, starty + 65, 3, 0, 4, true );
//   addeingabe ( 4, &actoptions.sound.Emute, 0, dblue );


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
   showtext2 ( "automatic training of units", x1 + 25, y1 + starty + 50 );
/*
#ifndef _DOS_
   showtext2 ( "disable sound",                                 x1 + 25, y1 + starty + 50 );
#endif
*/
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
   tgamepreferences prefs;
   prefs.init();
   prefs.run();
   prefs.done();
}
#endif
