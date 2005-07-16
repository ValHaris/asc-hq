/*! \file network.cpp
    \brief Code for moving a multiplayer game data from one computer to another.
 
    The only method that is currently implemented is writing the data to a file
    and telling the user to send this file by email :-)
    But the interface for real networking is there...
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
#include <stdlib.h>
#include <cstring>

#include "global.h"
#include "misc.h"
#include "typen.h"
#include "network.h"
#include "dlg_box.h"
#include "dialog.h"
#include "events.h"
#include "sgstream.h"
#include "loadpcx.h"
#include "mapdisplay.h"
#include "password_dialog.h"
#include "stack.h"
#include "gamedlg.h"

pbasenetworkconnection firstnetworkconnection = NULL;

tfiletransfernetworkconnection filetransfernetworkconnection;
pbasenetworkconnection defaultnetworkconnection  = &filetransfernetworkconnection;


tbasenetworkconnection::tbasenetworkconnection ( void )
{
   next = firstnetworkconnection;
   firstnetworkconnection = this;
   stream = NULL;
}

tbasenetworkconnection::~tbasenetworkconnection ( )
{
}


tfiletransfernetworkconnection::tfiletransfernetworkconnection ( void )
{
   orgstream = NULL;
   strcpy( suffix, tournamentextension );
}

char* tfiletransfernetworkconnection::getname ( void )                
{
   return "direct file transfer";
}


int   tfiletransfernetworkconnection::getid ( void )                
{
   return 1;  // auch dialogboxen „ndern !
}



tfiletransfernetworkconnection::tsetup::tsetup ( void )
{
   exitpossible = 1;
}

void tfiletransfernetworkconnection::tsetup::init ( void )
{
   status = 0;
   xsize = 400;
   ysize = 300;
   x1 = -1;
   y1 = -1;
   addbutton ( "~f~ilename", 20, starty + 40, xsize - 160, starty + 70, 1 , 0, 1, true );
   addeingabe ( 1, filename, 1, maxfilenamelength );

   addbutton ( "~s~elect",   xsize - 140, starty + 40, xsize - 20, starty + 70, 0 , 1, 2, true );

   if ( exitpossible ) {
      addbutton ( "~O~k", 10, ysize - 40, xsize / 2 - 5, ysize - 10, 0, 1, 3 , true);
      addkey ( 3, ct_enter );
      addbutton ( "e~x~it", xsize / 2 + 5, ysize - 40, xsize - 10, ysize - 10, 0, 1, 4, true );
      addkey ( 4, ct_esc );
   } else {
      addbutton ( "~O~k", 10, ysize - 40, xsize - 10, ysize - 10, 0, 1, 3 , true);
      addkey ( 3, ct_enter );
   }
   if ( !filename[0] && actmap ) {
      ASCString fn = actmap->preferredFileNames.mapname[0];
      if ( fn.find ( "." ) != ASCString::npos )
         fn.erase ( fn.find ( "." ) );

      fn += "-";
      char buf = 'A'+actmap->actplayer;
      fn += buf;
      fn += "-%";
      strcpy ( filename, fn.c_str() );
      //strcpy ( filename, "turnier%");
   }

   buildgraphics();
}

void  tfiletransfernetworkconnection::tsetup::buttonpressed ( int id )
{
   ASCString s1;

   tdialogbox::buttonpressed ( id );
   switch ( id ) {
      case 2:    mousevisible( false ); 
                 fileselectsvga( tournamentextension, s1, true );
                 if ( !s1.empty() ) {
                    strcpy ( filename, s1.c_str() );
                    showbutton ( 1 );
                 }
                 mousevisible( true );
                 break;
                 
      case 3:    status = 2;           
                 break;
   
      case 4:    status = 1;
                 break;
                 
   } /* endswitch */

}


void tfiletransfernetworkconnection::treceivesetup::init ( pnetworkconnectionparameters  d )
{
   tdialogbox::init ();
   dtaptr = d;
   memcpy ( dta , dtaptr, sizeof ( dta ));

   filename = &dta[8];

   int* pi = (int*) dta;
   pi[0] = 1;
   pi[1] = 1;


   title = ttl;
   strcpy ( ttl, "direct file transfer setup for receiving" );
   /*tfiletransfernetworkconnection::*/tsetup::init (  );
}

void tfiletransfernetworkconnection::tsendsetup::init ( pnetworkconnectionparameters  d, int exitposs  )
{
   tdialogbox::init ();
   title = ttl;
   strcpy ( ttl, "direct file transfer setup for sending" );

   dtaptr = d;
   memcpy ( dta , dtaptr, sizeof ( dta ));
   filename = &dta[8];

   int* pi = (int*) dta;
   pi[0] = 1;
   pi[1] = 2;
   exitpossible = exitposs;

   /*tfiletransfernetworkconnection::*/tsetup::init (  );

   activefontsettings.font = schriften.smallarial;
   activefontsettings.justify = lefttext;
   activefontsettings.length = 0;
   showtext2("Please enter the filename into which ASC will", x1+25, y1+130 );
   showtext2("write your game. Send this file to the next player.", x1+25, y1+150 );
   showtext2("You will not be asked for this filename again during", x1+25, y1+170 );
   showtext2("this game, so place the % character somehwere.", x1+25, y1+190 );
   showtext2("The % character will automatically be replaced by", x1+25, y1+210 );
   showtext2("the turn number to allow archiving of the files.", x1+25, y1+230 );
}


void tfiletransfernetworkconnection::tsetup::run ( void )
{
   mousevisible ( true );
   do {
       tdialogbox::run();
   } while ( status == 0 ); /* enddo */
   if ( status == 2 ) 
     memcpy ( dtaptr , dta, sizeof ( dta ));

}


int   tfiletransfernetworkconnection::setupforsending   ( pnetworkconnectionparameters  data, int exitpossible  )
{

   tsendsetup sendsetup;
   sendsetup.init( data, exitpossible );
   sendsetup.run();
   sendsetup.done();
   return sendsetup.status-1;
}

int   tfiletransfernetworkconnection::setupforreceiving ( pnetworkconnectionparameters  data )
{
   treceivesetup receivesetup;
   receivesetup.init( data );
   receivesetup.run();
   receivesetup.done();
   return receivesetup.status-1;
}


void  tfiletransfernetworkconnection::initconnection  ( tnetworkchannel channel )          
{
   chann = channel;
}

int   tfiletransfernetworkconnection::connectionopen  ( void )          
{
   return 0;
}


void  tfiletransfernetworkconnection::closeconnection ( void )          
{

}

void  tfiletransfernetworkconnection::mountfilename ( char* newname, char* oldname )
{
   strcpy ( newname, oldname );

   int p = 0; 
   while ( newname[p] != 0  && newname[p] != '%' )
      p++;

   if ( newname[p] == '%' ) {
      int r = p;
      newname[p] = 0;
      char temp[10];
      itoa ( actmap->time.turn(), temp, 10 );
      while ( strlen ( temp ) + strlen ( newname ) > maxfilenamelength ) {
         p--;
         newname[p] = 0;
      }
      strcat ( newname, temp );
      strcat ( newname, &oldname[r+1] );

      p = strlen ( newname );
      if ( !strchr ( newname, '.' ) ) {
         while ( strlen ( newname ) > maxfilenamelength ) {
            p--;
            newname[p] = 0;
         }
      } else {
         while ( strlen ( newname ) > maxfilenamelength ) {
            p--;
            newname[p] = 0;
         }
      }
   }

   if ( strchr ( newname, '.' ) == NULL )
      strcat ( newname, &suffix[1] );

}


int   tfiletransfernetworkconnection::validateparams ( pnetworkconnectionparameters data, tnetworkchannel chann  )
{
   int* pi = (int*) data;
   if ( pi[0] != getid() )
      return 0;
      
   filename = &(*data)[8];
   if ( !filename[0] )
      return 0;

   if ( chann == TN_SEND ) {
      char tempfilename[200];
      mountfilename ( tempfilename, filename );

      if ( exist( tempfilename ) ) {
         char stempp[100];
         sprintf(stempp, "file %s already exists ! Overwrite ?", tempfilename );
         if (choice_dlg(stempp,"~y~es","~n~o") == 2) 
            return 0;
      }

   } else {
      char temp[20];
      strcpy ( temp, filename );
      if ( strchr ( temp, '.' ) == NULL )
         strcat ( temp, &suffix[1] );
      if ( !exist( temp ) )
         return 0;
   }

   return 1;
}


int   tfiletransfernetworkconnection::transferopen  ( void )          
{
   if ( stream || orgstream )
      return 1;
   else 
      return 0;
}


void  tfiletransfernetworkconnection::inittransfer  ( pnetworkconnectionparameters data )          
{
   if ( stream )
      displaymessage ( "tfiletransfernetworkconnection::inittransfer ( pnetworkconnectionparameters ) \n stream bereits initialisiert !",2);

   if ( orgstream )
      displaymessage ( "tfiletransfernetworkconnection::inittransfer ( pnetworkconnectionparameters ) \n orgstream bereits initialisiert !",2);

   while ( validateparams ( data, chann ) == 0 ) {
      if ( chann == TN_SEND )
        setupforsending ( data, 0 );
      else
        setupforreceiving ( data );
   }

   char tempfilename[200];
   mountfilename ( tempfilename, filename );

   if ( chann == TN_SEND )
      orgstream = new  tnfilestream ( tempfilename, tnstream::writing );
   else
      orgstream = new  tnfilestream ( tempfilename, tnstream::reading );

   stream = orgstream;
}

void  tfiletransfernetworkconnection::closetransfer ( void )          
{
   if ( orgstream ) { 
      delete orgstream;
      orgstream = NULL;
      stream = NULL;
   }
}






pbasenetworkconnection getconnectforid( int id )
{
   pbasenetworkconnection conn = firstnetworkconnection;
   while ( conn && conn->getid() != id  )
      conn = conn->next;

   return conn;
}

void setallnetworkpointers ( pnetwork net )
{
   for (int i = 0; i < 8; i++) {
      if ( net->computer[i].send.transfermethodid )
         net->computer[i].send.transfermethod = getconnectforid ( net->computer[i].send.transfermethodid );
      if ( net->computer[i].receive.transfermethodid )
         net->computer[i].receive.transfermethod = getconnectforid ( net->computer[i].receive.transfermethodid );
   } /* endfor */
}



void networksupervisor ( void )
{
   class tcarefordeletionofmap {
         pmap tmp;
      public:
         tcarefordeletionofmap ()
         {
            tmp= actmap;
            actmap = NULL;
         }
         ~tcarefordeletionofmap (  )
         {
            if ( actmap && (actmap->xsize > 0  ||  actmap->ysize > 0) )
               delete actmap;
            actmap = tmp;
         };
   }
   carefordeletionofmap;


   tlockdispspfld ldsf;

   tnetwork network;
   /*
      int stat;
      do {
         stat = setupnetwork( &network, 1+8 );
         if ( stat == 1 )
            return;

      } while ( (network.computer[0].receive.transfermethod == 0) || (network.computer[0].receive.transfermethodid != network.computer[0].receive.transfermethod->getid()) );
   */
   int stat;
   int go = 0;
   do {
      stat = network.computer[0].receive.transfermethod->setupforreceiving ( &network.computer[0].receive.data );
      if ( stat == 0 )
         return;

      if ( network.computer[0].receive.transfermethod  &&
            network.computer[0].receive.transfermethodid == network.computer[0].receive.transfermethod->getid()  &&
            network.computer[0].receive.transfermethod->validateparams( &network.computer[0].receive.data, TN_RECEIVE ))
         go = 1;
   } while ( !go );

   try {
      displaymessage ( " starting data transfer ",0);

      network.computer[0].receive.transfermethod->initconnection ( TN_RECEIVE );
      network.computer[0].receive.transfermethod->inittransfer ( &network.computer[0].receive.data );

      tnetworkloaders nwl;
      nwl.loadnwgame ( network.computer[0].receive.transfermethod->stream );

      network.computer[0].receive.transfermethod->closetransfer();
      network.computer[0].receive.transfermethod->closeconnection();

      removemessage();
      if ( actmap->network )
         setallnetworkpointers ( actmap->network );
   } /* endtry */

   catch ( tfileerror ) {
      displaymessage ("a file error occured while loading game",1 );
      delete actmap;
      actmap = NULL;
      return;
   } /* endcatch */
   catch ( ASCexception ) {
      displaymessage ("error loading game",1 );
      delete actmap;
      actmap = NULL;
      return;
   } /* endcatch */


   int ok = 0;
   if ( !actmap->supervisorpasswordcrc.empty() ) {
      ok = enterpassword ( actmap->supervisorpasswordcrc );
   } else {
      displaymessage ("no supervisor defined",1 );
      delete actmap;
      actmap = NULL;
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
            delete actmap;
            actmap = NULL;
            return;
         }

      } while ( (network.computer[0].send.transfermethod == 0) || (network.computer[0].send.transfermethodid != network.computer[0].send.transfermethod->getid()) ); /* enddo */

      tnetworkcomputer* compi = &network.computer[ 0 ];

      displaymessage ( " starting data transfer ",0);

      try {
         compi->send.transfermethod->initconnection ( TN_SEND );
         compi->send.transfermethod->inittransfer ( &compi->send.data );

         tnetworkloaders nwl;
         nwl.savenwgame ( compi->send.transfermethod->stream );

         compi->send.transfermethod->closetransfer();
         compi->send.transfermethod->closeconnection();
      } /* endtry */
      catch ( tfileerror ) {
         displaymessage ( "a file error occured while saving file", 1 );
      } /* endcatch */
      catch ( ASCexception ) {
         displaymessage ( "error saving file", 1 );
      } /* endcatch */

      delete actmap;
      actmap = NULL;
      displaymessage ( "data transfer finished",1);

   } else {
      displaymessage ("no supervisor defined or invalid password",1 );
      delete actmap;
      actmap = NULL;
   }
}
