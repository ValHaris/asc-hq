//     $Id: network.cpp,v 1.3 1999-11-18 17:31:17 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  1999/11/16 03:42:13  tmwilson
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
#include <stdlib.h>
#include <string.h>

#include "tpascal.inc"
#include "misc.h"
#include "typen.h"
#include "network.h"
#include "dlg_box.h"
#include "dialog.h"
#include "mousehnd.h"
#include "sgstream.h"
#include "spfldutl.h"
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
   addeingabe ( 1, filename, 1, 12 );

   addbutton ( "~s~elect",   xsize - 140, starty + 40, xsize - 20, starty + 70, 0 , 1, 2, true );

   if ( exitpossible ) {
      addbutton ( "~O~k", 10, ysize - 40, xsize / 2 - 5, ysize - 10, 0, 1, 3 , true);
      addkey ( 3, ct_enter );
      addbutton ( "~E~xit", xsize / 2 + 5, ysize - 40, xsize - 10, ysize - 10, 0, 1, 4, true );
      addkey ( 4, ct_esc );
   } else {
      addbutton ( "~O~k", 10, ysize - 40, xsize - 10, ysize - 10, 0, 1, 3 , true);
      addkey ( 3, ct_enter );
   }
   if ( !filename[0] )
      strcpy ( filename, "turnier%");

   buildgraphics();
}

void  tfiletransfernetworkconnection::tsetup::buttonpressed ( char id )
{
  char         s1[300];

   tdialogbox::buttonpressed ( id );
   switch ( id ) {
      case 2:    mousevisible( false ); 
                 char temp[200];
                 strcpy ( temp, gamepath );
                 strcat ( temp, tournamentextension );

                 fileselectsvga( temp, s1, 1);
                 if ( s1[0] ) {
                    strcpy ( filename, gamepath );
                    strcat ( filename, s1 );
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
   tfiletransfernetworkconnection::tsetup::init (  );
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

   tfiletransfernetworkconnection::tsetup::init (  );
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

   if ( gamepath[0] &&  !strchr(newname, '\\' )) {
      char tmp[100];
      strcpy ( tmp, newname );
      strcpy ( newname, gamepath );
      strcat ( newname, tmp );
   }

}


int   tfiletransfernetworkconnection::validateparams ( pnetworkconnectionparameters data, tnetworkchannel chann  )
{
   int* pi = (int*) data;
   if ( pi[0] != getid() )
      return 0;
      
   filename = &(*data)[8];
   if ( !filename[0] )
      return 0;

   if ( chann == send ) {
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
      if ( chann == send )
        setupforsending ( data, 0 );
      else
        setupforreceiving ( data );
   }

   char tempfilename[200];
   mountfilename ( tempfilename, filename );

   if ( chann == send )                            
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



int encodepassword ( char* pw )
{
   if ( !pw )
      return 0;

   int len = strlen ( pw );

   if ( len )
      return crc32buf( pw, len+1 );
   else
      return 0;

   /*
   char* p = pw;
   int cr = 0;
   while ( pw[0] ) {
      cr+= ( pw[0] ^ pw[1] );
      cr = rol ( cr, pw[1]+1 );
      cr^= pw[0];
      pw++;
   } 

   if ( *p && !cr )
      cr = 1;

   return cr;

   */
}



int   tenterpassword :: gettextwdth_stredit ( char* txt, pfont font )
{
  char* ss2 = strdup ( txt );

  int i = 0;
  while ( ss2[i] )
     ss2[i++] = '*';
  
  ss2 [ i ] = 0;

  i = gettextwdth(ss2, font);
  asc_free ( ss2 );

  return i;
}


int   tenterpassword :: getcapabilities ( void )
{
   return 0;
}


void         tenterpassword ::lne(int          x1,
                 int          y1,
                 char *       s,
                 int          position,
                 boolean      einfuegen)
{ 
  char* ss2 = strdup ( s );

  int i = 0;
  while ( ss2[i] )
     ss2[i++] = '*';
  
  ss2 [ position ] = 0;
  i = x1 + gettextwdth(ss2,activefontsettings.font);
  int j = y1; 
  int k = y1 + activefontsettings.font->height; 
  xorline(i,j,i,k,3); 
  if (einfuegen == false) { 
     xorline(i + 1,j,i + 1,k,3); 
     xorline(i - 1,j,i - 1,k,3); 
  }  
  asc_free ( ss2 );
} 

void tenterpassword :: dispeditstring ( char* st, int x1, int y1 )
{
   char* ss2 = strdup ( st );

   int i = 0;
   while ( ss2[i] )
     ss2[i++] = '*';

   showtext2(ss2,x1,y1); 

  asc_free ( ss2 );
}

int tenterpassword :: checkforreask ( int crc )
{
   if ( crc == 0 || actmap->network && actmap->network->globalparams.reaskpasswords )
      return 1;
   else 
      return 0;
}

void tenterpassword :: init ( int* crc, int mode, char* ttl  )
{
   tdialogbox::init();
   xsize = 200;
   if ( *crc )
     ysize = 150;
   else
     ysize = 180;
   y1 = -1;
   x1 = -1;
   cr = crc;


   reask = checkforreask( *crc );

   windowstyle ^= dlg_in3d;
   if ( mode == 0 ) {
      if ( *crc == 0   &&  gameoptions.defaultpassword ) {
         addbutton ( "~O~k", 10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1, 1, reask );
         addbutton ( "~D~efault", xsize / 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1,7, true );
      } else {
         addbutton ( "~O~k", 10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1, 1, reask );
         addbutton ( "~E~xit", xsize / 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1,8, true );
      }

   } else {
      if ( *crc == 0   &&  gameoptions.defaultpassword ) {
         addbutton ( "~O~k", 10, ysize - 35, xsize / 3 - 5, ysize - 10, 0, 1, 1, reask );
         addbutton ( "~D~efault", xsize / 3 + 5,   ysize - 35, 2 * xsize / 3 - 5, ysize - 10, 0, 1,7, true );
         addbutton ( "~C~ancel", 2 * xsize / 3 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1, 6, true );
      } else {
         addbutton ( "~O~k", 10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1, 1, reask );
         addbutton ( "~C~ancel", xsize / 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1, 6, true );
      }
   }

   addkey ( 1, ct_enter );
   addkey ( 1, ct_enterk );


   if ( !ttl )
      title = "enter password";
   else 
      title = ttl;

   

   addbutton ( "password:", 10, 50, xsize - 10, 75, 1, 0, 2, true );
   addeingabe ( 2, strng1, 0, 39 );

   if ( *crc == 0 ) {
      addbutton ( "confirmation:", 10, 95, xsize - 10, 120, 1, 0, 3, true );
      addeingabe ( 3, strng2, 0, 39 );
      confirm = 1;
   } else
      confirm = 0;

   strng1[0] = 0;
   strng2[0] = 0;
   status = 0;

   buildgraphics();

};


void tenterpassword :: buttonpressed ( char id )
{
   tdialogbox::buttonpressed ( id );
   if ( id == 1 )
      status = 1;
   else
      if ( id == 6 )
         status = 10;
      else
         if ( id == 7 ) {
            *cr = gameoptions.defaultpassword;
            status = 2;
         } else
            if ( id == 8 ) {
               erasemap();
               throw tnomaploaded();
            } else
               if ( reask == 0 )
                  if ( encodepassword ( strng1 ) == *cr )
                     enablebutton ( 1 );
                  else
                     disablebutton ( 1 );
               else
                  if ( strcmp ( strng1, strng2 ) == 0)
                     enablebutton ( 1 );
                  else
                     disablebutton ( 1 );
}

void tenterpassword :: run ( int* result )
{
   pbutton pb = firstbutton;
   while ( pb &&  (pb->id != 2)) 
      pb = pb->next;

   if ( pb )
      if ( pb->id == 2 )
         execbutton( pb , false );

   if ( strng1[0] ) {
      pb = firstbutton;
      while ( pb &&  (pb->id != 3)) 
         pb = pb->next;
   
      if ( pb )
         if ( pb->id == 3 )
            execbutton( pb , false );
   }

   mousevisible ( true );
   do {
      tdialogbox::run ();
   } while ( status == 0 ); /* enddo */

   if ( status == 1 )
      *cr = encodepassword ( strng1 );

   if ( result )
      *result = status;
}

void enterpassword ( int* cr )
{
   tenterpassword epw;
   epw.init ( cr, 0 );
   epw.run ( NULL );
   epw.done ();
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

