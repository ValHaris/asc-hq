//     $Id: network.h,v 1.3 1999-12-07 22:13:25 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  1999/11/16 03:42:14  tmwilson
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

#ifdef karteneditor
#error eigentlich sollte der Karteneditor ohne das Network auskommen k”nnen !
#endif


#ifndef network_h
#define network_h

#include "typen.h"
#include "sgstream.h"
#include "dlg_box.h"

extern int netlevel;



extern pbasenetworkconnection firstnetworkconnection ;
extern pbasenetworkconnection defaultnetworkconnection ;

class tbasenetworkconnection {
        public:
           pbasenetworkconnection next;

           virtual char* getname ( void )                  = 0;
           virtual int   getid ( void )                    = 0;
           virtual int   setupforsending   ( pnetworkconnectionparameters   data, int exitpossible = 1 )  = 0;
           virtual int   setupforreceiving ( pnetworkconnectionparameters   data )  = 0;
           virtual int validateparams ( pnetworkconnectionparameters data, tnetworkchannel chann ) = 0;
           virtual void  initconnection  ( tnetworkchannel channel )          = 0;
           virtual void  inittransfer    ( pnetworkconnectionparameters data )          = 0;
           virtual int   transferopen ( void ) = 0;
           virtual int   connectionopen ( void ) = 0;
           virtual void  closeconnection ( void )          = 0;
           virtual void  closetransfer   ( void )          = 0;
           tbasenetworkconnection ( void );
           ~tbasenetworkconnection (  );
           pnstream stream;
       };


class tfiletransfernetworkconnection : public tbasenetworkconnection {
        protected:
            tnetworkchannel chann;
            char suffix[8];
            char* filename;
            tnfilestream* orgstream;

            class tsetup : public tdialogbox {
                               protected:
                                   char ttl[100];
                                   char* filename;

                                   tnetworkconnectionparameters  dta;
                                   pnetworkconnectionparameters  dtaptr;
                                   int exitpossible;
                               public: 
                                   int  status;
                                   tsetup ( void );
                                   int  getcapabilities ( void ) { return 1; };
                                   void init ( void );
                                   virtual void buttonpressed ( char id );
                                   void run ( void );
                               } ;
            class tsendsetup : public tsetup {
                               public: 
                                   void init ( pnetworkconnectionparameters  d, int exittposs = 1 );
                               };

            class treceivesetup : public tsetup {
                               public: 
                                   void init ( pnetworkconnectionparameters  d );
                              };


           void mountfilename ( char* newname, char* oldname );
        public:                 
           tfiletransfernetworkconnection( void );  
           virtual char* getname ( void )                  ;
           virtual int   getid ( void )                    ;
           virtual int validateparams ( pnetworkconnectionparameters data, tnetworkchannel chann  );
           virtual int   setupforsending   ( pnetworkconnectionparameters data, int exitpossible = 1 )  ;
           virtual int   setupforreceiving ( pnetworkconnectionparameters data )  ;
           virtual void  initconnection  ( tnetworkchannel channel )          ;
           virtual void  inittransfer    ( pnetworkconnectionparameters data )          ;
           virtual int   transferopen ( void );
           virtual int   connectionopen ( void );
           virtual void  closeconnection ( void )          ;
           virtual void  closetransfer   ( void )          ;
       };




extern pbasenetworkconnection getconnectforid( int id );
extern void setallnetworkpointers ( pnetwork net );
#endif
