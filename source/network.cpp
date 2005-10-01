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


void FileTransfer::readChildData ( tnstream& stream )
{
   stream.readInt();
   filename = stream.readString();
}

void FileTransfer::writeChildData ( tnstream& stream ) const
{
   stream.writeInt( 1 );
   stream.writeString( filename );
}

void FileTransfer::setup()
{
   enterfilename();
}


void FileTransfer::setup( const ASCString& filename )
{
   this->filename = filename;
}

bool FileTransfer::enterfilename()
{
   filename = editString( "Enter Filename", filename );
   return true;
}


void FileTransfer::send( const tmap* map, int lastPlayer, int lastturn )
{
   while ( filename.empty() ) {
      if( !enterfilename() )
         return;
   }

   try {      
      ASCString fname = constructFileName( map, lastPlayer, lastturn );
      {
         tnfilestream gamefile ( fname, tnstream::writing );
         tnetworkloaders nwl;
         nwl.savenwgame( &gamefile );
      }
      ASCString msg = "Data written to file\nPlease send " + fname + " to \n" + map->player[map->actplayer].getName();
      if ( !map->player[map->actplayer].email.empty() )
         msg += " (" + map->player[map->actplayer].email + ")";
      infoMessage( msg );
   } catch ( tfileerror ) {
      fatalError ( "error writing file %s ", filename.c_str() );
   }
}


tmap* FileTransfer::receive()
{
   return loadPBEMFile( filename );
}   

tmap* FileTransfer::loadPBEMFile( const ASCString& filename )
{
   tmap* map = NULL;
   try {      
      tnfilestream gamefile ( filename, tnstream::reading );
      tnetworkloaders nwl;
      map = nwl.loadnwgame( &gamefile );
   } catch ( tfileerror ) {
      fatalError ( "%s is not a legal email game.", filename.c_str() );
   }
   return map;
}

ASCString FileTransfer::constructFileName( const tmap* actmap, int lastPlayer, int lastturn ) const
{
   ASCString s = filename;
   while ( s.find( "$p") != ASCString::npos )
      s.replace( s.find( "$p"), 2, 1, 'A' + lastPlayer );

   while ( s.find( "$t") != ASCString::npos )
      s.replace( s.find( "$t"), 2, ASCString::toString( lastturn ) );
      
   s += tournamentextension;
   return s;
}


#if 0
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
   #endif



namespace {
   const bool r1 = networkTransferMechanismFactory::Instance().registerClass( FileTransfer::mechanismID(), ObjectCreator<GameTransferMechanism, FileTransfer> );
}
