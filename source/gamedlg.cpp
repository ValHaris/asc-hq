/*! \file gamedlg.cpp    
    \brief Tons of dialog boxes which are used in ASC only (and not in the mapeditor)
*/
/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2005  Martin Bickel  and  Marc Schellenberger

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
#include "gameoptions.h"
#include "loadimage.h"
#include "errors.h"
// #include "dialogs/pwd_dlg.h"
#include "viewcalculation.h"
#include "replay.h"
#include "itemrepository.h"
#include "strtmesg.h"
#include "dialogs/editmapparam.h"
#include "dlg_box.h"
#include "spfst.h"
#include "dialog.h"

#include "widgets/textrenderer.h"

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
         // Vehicletype* vt = actmap->getvehicletype_byid( techs[i]->relatedUnitID );
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

                        // vehicle_information ( vt );
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






     class ttributepayments : public tdialogbox {
                       GameMap::ResourceTribute trib;
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
   rppt = actmap->player[actmap->actplayer].research.getResearchPerTurn();
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
         showtext2 ( strrr ( actmap->player[actmap->actplayer].research.currentTechAvailableIn() ), textxpos, y1 + 290 );

   }
   showtext2 ( strrr ( rppt ),                   textxpos, y1 + 260 );


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
      vector<ASCString> sv;
      for ( vector<int>::iterator i = actmap->player[actmap->actplayer].research.developedTechnologies.begin(); i != actmap->player[actmap->actplayer].research.developedTechnologies.end(); ++i ) {
         Technology* t = technologyRepository.getObject_byID( *i );
         if ( t )
            sv.push_back ( t->name );
      }
      sort ( sv.begin(), sv.end() );

      ASCString s;
      for ( vector<ASCString>::iterator i = sv.begin(); i != sv.end(); ++i )
         s += *i + "\n";


      ViewFormattedText vft ( "Developed Technologies", s, PG_Rect( -1, -1, 500, 500 ));
      vft.Show();
      vft.RunModal();

   }
   if ( skeypress( ct_lshift)) {
      ASCString s = actmap->player[actmap->actplayer].research.listTriggeredTechAdapter();

      ViewFormattedText vft ( "Triggered TechAdapter", s, PG_Rect( -1, -1, 500, 500 ));
      vft.Show();
      vft.RunModal();
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



void writeGameParametersToString ( std::string& s)
{
   s = "The game has been set up with the following game parameters:\n";
   s += "(black line: parameter has default value)\n\n";
   for ( int i = 0; i< gameparameternum; i++ ) {
   
      int d = actmap->getgameparameter(GameParameter(i)) != gameParameterSettings[i].defaultValue;

      if ( d )
         s+= "#color4#";

      s += gameParameterSettings[i].longName;
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
      setmapparameters( actmap );

   if ( id == 1 ) {             // OK
      status = 2;

      if ( techlevel )
         for ( int i = 0; i < 8; i++ )
             actmap->player[i].research.settechlevel ( techlevel );

      if ( replays )
         actmap->replayinfo = new GameMap::ReplayInfo;
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
      CGameOptions::Instance()->defaultSuperVisorPassword = svpwd_enc;
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
             tfield* fld ;
             int num;
             int ply[8];
             int xs;
           public:
             void init ( tfield* fld );
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

void         tgiveunitawaydlg :: init( tfield* fld )
{ 
   this->fld = fld;

   xs = 15;

   num = 0;
   for ( int i = 0; i < 8; i++ )
      if ( actmap->player[i].exist() )
         if ( i != actmap->actplayer &&  actmap->player[actmap->actplayer].diplomacy.getState( i) >= PEACE ) 
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
      logtoreplayinfo ( rpl_convert, fld->getContainer()->getPosition().x, fld->getContainer()->getPosition().y, (int) ply[markedplayer] );
      computeview( actmap );
   }
}

void giveunitaway ( tfield* fld )
{
   if ( ( fld->vehicle && fld->vehicle->color==actmap->actplayer*8) ||  (fld->building && fld->building->color == actmap->actplayer * 8 )) {
      tgiveunitawaydlg gua;
      gua.init ( fld );
      gua.run ();
      gua.done ();
   } else {
      dispmessage2( 450, NULL );
      while ( mouseparams.taste )
         releasetimeslice();

   }
}


#define entervalue ct_space
#if 0
    class  tverlademunition : public tmunitionsbox {
                          int abstand;
                          int firstliney;
                          int startx;
                          int llength;
                          int numlength;
                          VehicleService::Target* target;
                          Vehicle*      source;
                          vector<int> oldpos;
                          vector<int> newpos;
                          vector<int> displayed;
                          int           abbruch;
                          VehicleService* serviceAction;
                          int targetNWID;

                 public:

                          void          init ( Vehicle* src, int _targetNWID, VehicleService* _serviceAction );
                          virtual void  run ( void );
                          virtual void  buttonpressed( int id );
                          void          zeichneschieberegler( int pos);
                          void          checkpossible( int pos );
                          void          setloading ( int pos );
                       };


void         tverlademunition::init( Vehicle* src, int _targetNWID, VehicleService* _serviceAction )
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

   int ix = fieldsizex;

   if ( source )
      source->paint( getActiveSurface(), SPoint(startx+x1 - 60 , y1 + 55));
//   else
//      showtext2( building->typ->name , x1 + 50, y1 + 55);

   target->dest->paint( getActiveSurface(), SPoint(x1 + xsize - 30 - ix , y1 + 55));

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
                            step = int( pow ( double(10), double(log10 (double(step)))));

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

#endif


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
#ifdef WEATHERGENERATOR
   displaymessage2("wind speed is %d which equals %s fields / turn", actmap->weatherSystem->getCurrentWindSpeed(), strrrd8d ( actmap->weatherSystem->getCurrentWindSpeed() * maxwindspeed / 256 ));
#else
   displaymessage2("wind speed is %d which equals %s fields / turn", actmap->weather.windSpeed, strrrd8d ( actmap->weather.windDirection * maxwindspeed / 256 ));
#endif
}

