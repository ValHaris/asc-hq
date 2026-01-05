/*! \file gamedlg.cpp    
    \brief Tons of dialog boxes which are used in ASC only (and not in the mapeditor)
*/
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
#include "stack.h"
#include "controls.h"
#include "gameoptions.h"
#include "errors.h"
#include "viewcalculation.h"
#include "itemrepository.h"
#include "strtmesg.h"
#include "dialogs/editmapparam.h"
#include "dlg_box.h"
#include "spfst.h"
#include "dialog.h"
#include "actions/transfercontrolcommand.h"
#include "widgets/textrenderer.h"
#include "sg.h"
#include "spfst-legacy.h"


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



ASCString writeGameParametersToString ( GameMap* actmap )
{
   ASCString s = "The game has been set up with the following game parameters:\n";
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
   return s;
}

void sendGameParameterAsMail ( GameMap* actmap )
{
   ASCString s = writeGameParametersToString ( actmap );

   if ( actmap->getgameparameter( cgp_superVisorCanSaveMap) ) {
      s = "#color4#Warning ! This game has been started with the option enabled that allows the Supervisor to save the map. This will allow him to spy on everything. \n"
           "If you did not initiate this game yourself, don't play with this option unless you were specifically informed why this option had to be enabled. \n"
           "#color0# \n" + s;
   }

   new Message ( s, actmap, 255 );
}


void showGameParameters ( void )
{
   ViewFormattedText vft( "Game Parameters", writeGameParametersToString(actmap), PG_Rect( -1, -1, 500, 550 ) );
   vft.Show();
   vft.RunModal();
}



class tgiveunitawaydlg : public tdialogbox {
             int markedplayer;
             int status;
             void paintplayer ( int i );
             MapField* fld ;
             int xs;
             TransferControlCommand::Receivers receivers;
           public:
             void init ( MapField* fld );
             void run ( const Context& context );
             void buttonpressed ( int id );
       };


void tgiveunitawaydlg :: paintplayer ( int i )
{
   if ( i < 0 ) 
      return;

   if ( i == markedplayer ) {
      bar ( x1 + 20, y1 + starty + xs + i * 40, x1 + xsize - 20, y1 + starty + 60 + i * 40, 20 + receivers[i]->getPosition() * 8 );
   } else {
      bar ( x1 + 20, y1 + starty + xs + i * 40, x1 + xsize - 20, y1 + starty + 60 + i * 40, dblue );
   } /* endif */

   activefontsettings.color = black;
   activefontsettings.font = schriften.smallarial;
   activefontsettings.length = 0;

   if ( receivers[i]->getPosition() < 8 )
      showtext2 ( receivers[i]->getName().c_str(), x1 + 60, y1 + starty + xs+17 + i * 40 - activefontsettings.font->height / 2 );
   else
      showtext2 ( "neutral", x1 + 60, y1 + starty + xs+17 + i * 40 - activefontsettings.font->height / 2 );
}

void         tgiveunitawaydlg :: init( MapField* fld )
{ 
   this->fld = fld;

   xs = 15;

   
   TransferControlCommand tcc( fld->getContainer() );
         
   receivers = tcc.getReceivers();
   
   tdialogbox::init();

   xsize = 250;
   ysize = 110 + receivers.size() * 40;

   if ( fld->vehicle )
      title = "give unit to";
   else if ( fld->building )
      title = "give building to";

   addbutton ( "~O~k", 10, ysize - 30, xsize/2 - 5, ysize - 10 , 0, 1, 1, true );
   addkey ( 1, ct_enter );
   addbutton ( "~C~ancel", xsize/2 + 5, ysize - 30, xsize-10 - 5, ysize - 10 , 0, 1, 2, true );
   addkey ( 2, ct_esc );


   if ( receivers.size() == 1 )
      markedplayer = 0;
   else
      markedplayer = -1;

   status = 0;


   buildgraphics(); 


   for ( int j = 0; j < receivers.size(); j++ )
      paintplayer( j );

} 

void         tgiveunitawaydlg :: buttonpressed ( int id )
{
   if ( id == 1 ) {
      if ( markedplayer >= 0 )
         status = 12;
      else
         displaymessage ( "please select the player you want to give your unit to",1);
   }
   
   if ( id == 2 )
      status = 10;
}


void tgiveunitawaydlg :: run ( const Context& context )
{
   if ( !receivers.size() ) 
      return;

   while ( mouseparams.taste )
      releasetimeslice();

   mousevisible ( true );
   do {
      tdialogbox :: run ( );

      int old = markedplayer;
      if ( taste == ct_up  && markedplayer > 0 ) 
         markedplayer --;

      if ( taste == ct_down  && markedplayer < receivers.size()-1 ) 
         markedplayer ++;
      
      if ( mouseparams.taste == 1 )
         for ( int i = 0; i < receivers.size(); i++ )
            if ( mouseinrect ( x1 + 20, y1 + starty + xs + i * 40, x1 + xsize - 20, y1 + starty + 60 + i * 40 ) ) 
               markedplayer = i;


      if ( old != markedplayer ) {
         paintplayer ( markedplayer );
         paintplayer ( old );
      }
   } while ( status < 10 ); /* enddo */

   if ( status == 12 ) {
      auto_ptr<TransferControlCommand> tcc ( new TransferControlCommand( fld->getContainer() ));
      tcc->setReceiver( receivers[markedplayer] );
      ActionResult res = tcc->execute( context );
      if ( res.successful() )
         tcc.release();
      else {
         displayActionError(res );
      }
   }
}

void giveunitaway ( MapField* fld, const Context& context )
{
   ContainerBase* c = fld->getContainer();
   if ( !c ) {
      dispmessage2( 450, NULL );
      return;
   }
   
   if ( c->getOwner() != c->getMap()->actplayer ) {
      dispmessage2( 451, NULL );
      return;
   }
   
   if ( TransferControlCommand::avail( c ) ) {
      tgiveunitawaydlg gua;
      gua.init ( fld );
      gua.run ( context );
      gua.done ();
   } else {
      dispmessage2( 450, NULL );
      while ( mouseparams.taste )
         releasetimeslice();

   }
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

   ViewFormattedText vft( "Play Time Information", text, PG_Rect( -1, -1 , 500, 400 ));
   vft.Show();
   vft.RunModal();
}

void displaywindspeed( void )
{
#ifdef WEATHERGENERATOR
   displaymessage2("wind speed is %d which equals %s fields / turn", actmap->weatherSystem->getCurrentWindSpeed(), strrrd8d ( actmap->weatherSystem->getCurrentWindSpeed() * maxwindspeed / 256 ));
#else
   displaymessage2("wind speed is %d which equals %s fields / turn", actmap->weather.windSpeed, strrrd8d ( actmap->weather.windDirection * maxwindspeed / 256 ));
#endif
}

