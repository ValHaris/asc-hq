//     $Id: network.cpp,v 1.12 2000-10-14 10:52:52 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.11  2000/08/12 12:52:49  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.10  2000/08/12 09:17:32  gulliver
//     *** empty log message ***
//
//     Revision 1.9  2000/08/01 13:50:52  mbickel
//      Chaning the height of airplanes is not affected by wind any more.
//      Fixed: Airplanes could ascend onto buildings
//
//     Revision 1.8  2000/07/29 14:54:39  mbickel
//      plain text configuration file implemented
//
//     Revision 1.7  2000/05/30 18:39:25  mbickel
//      Added support for multiple directories
//      Moved DOS specific files to a separate directory
//
//     Revision 1.6  2000/05/22 15:40:36  mbickel
//      Included patches for Win32 version
//
//     Revision 1.5  2000/01/31 16:34:46  mbickel
//      now standard hotkeys in dialog boxes
//
//     Revision 1.4  1999/12/07 22:13:24  mbickel
//      Fixed various bugs
//      Extended BI3 map import tables
//
//     Revision 1.3  1999/11/18 17:31:17  mbickel
//      Improved BI-map import translation tables
//      Moved macros to substitute Watcom specific routines into global.h
//
//     Revision 1.2  1999/11/16 03:42:13  tmwilson
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
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "misc.h"
#include "typen.h"
#include "network.h"
#include "dlg_box.h"
#include "dialog.h"
#include "mousehnd.h"
#include "sgstream.h"
#include "loadpcx.h"

int netlevel;

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
   #ifdef _DOS_
    addeingabe ( 1, filename, 1, 8 );
   #else
    addeingabe ( 1, filename, 1, 255 );
   #endif

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
   if ( !filename[0] )
      strcpy ( filename, "turnier%");

   buildgraphics();
}

void  tfiletransfernetworkconnection::tsetup::buttonpressed ( int id )
{
  char         s1[300];

   tdialogbox::buttonpressed ( id );
   switch ( id ) {
      case 2:    mousevisible( false ); 
                 char temp[200];
                 strcpy ( temp, tournamentextension );

                 fileselectsvga( temp, s1, 1);
                 if ( s1[0] ) {
                    strcpy ( filename, s1 );
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
      itoa ( actmap->time.a.turn, temp, 10 );
      while ( strlen ( temp ) + strlen ( newname ) > 8 ) {
         p--;
         newname[p] = 0;
      }
      strcat ( newname, temp );
      strcat ( newname, &oldname[r+1] );

      p = strlen ( newname );
      if ( !strchr ( newname, '.' ) ) {
         while ( strlen ( newname ) > 8 ) {
            p--;
            newname[p] = 0;
         }
      } else {
         while ( strlen ( newname ) > 12 ) {
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
      orgstream = new  tnfilestream ( tempfilename, 2 );
   else
      orgstream = new  tnfilestream ( tempfilename, 1 );

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

