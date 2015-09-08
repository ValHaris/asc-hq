/*! \file network.cpp
    \brief Code for moving a multiplayer game data from one computer to another.
 
    The only method that is currently implemented is writing the data to a file
    and telling the user to send this file by email :-)
    But the interface for real networking is there...
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
#include <stdlib.h>
#include <cstring>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>


#include "../global.h"
#include "../misc.h"
#include "../sgstream.h"
#include "../loaders.h"
#include "../gamemap.h"
#include "../dialog.h"
#include "pbem-server.h"

#include "pbem-server-interaction.h"

#include "../paradialog.h"

#include <pgpropertyeditor.h>
#include <pgpropertyfield_integer.h>
#include <pgpropertyfield_intdropdown.h>
#include <pgpropertyfield_dropdown.h>
#include <pgpropertyfield_checkbox.h>
#include <pgpropertyfield_string.h>


// @PBEMSERVER 
// the interface between the ASC engine and the server communication interface


class AuthenticationDialog : public ASC_PG_Dialog {
   private:
      ASCString name;
      ASCString password;
      
      ASC_PropertyEditor* propertyEditor;
   
      bool ok()
      {
         if ( propertyEditor->Apply() )
            QuitModal();
         return true;  
      }
      
   public:
      AuthenticationDialog() : ASC_PG_Dialog(NULL, PG_Rect( -1, -1, 300, 150 ), "Logging in to server...")
      {
         propertyEditor = new ASC_PropertyEditor( this, PG_Rect( 10, GetTitlebarHeight() + 20, Width() - 20, Height() - GetTitlebarHeight() - 50 ), "PropertyEditor", 50 );
      
         new PG_PropertyField_String<ASCString>( propertyEditor , "Username", &name );
         (new PG_PropertyField_String<ASCString>( propertyEditor , "Default Password", &password ))->SetPassHidden('*');
         
         PG_Button* ok = new PG_Button( this, PG_Rect( Width() - 100, Height() - 40, 90, 30), "OK" );
         ok->sigClick.connect( sigc::mem_fun( *this, &AuthenticationDialog::ok ));
      }
      
      const ASCString& getName() { return name; };
      const ASCString& getPassword() { return password;  };
};




PBEMServer :: PBEMServer() 
    : filename("myFileName"), gameID(-1)
{
   
}


void PBEMServer::readChildData ( tnstream& stream )
{
   stream.readInt();
   serverAddress = stream.readString();
   gameID = stream.readInt();
}

void PBEMServer::writeChildData ( tnstream& stream ) const
{
   stream.writeInt( 1 );
   stream.writeString(serverAddress );
   stream.writeInt ( gameID );
}

void PBEMServer::setup()
{
   // enterfilename();
}



ASC_PBEM* PBEMServer::getSession()
{
   static ASC_PBEM* session = NULL;
   if  ( !session ) {
      session = new ASC_PBEM( serverAddress );
      
      bool success;
      do {
         AuthenticationDialog ad;
         ad.Show();
         ad.RunModal();
         success = session->login( ad.getName(), ad.getPassword() );
         if( !success )
            warningMessage("login failed!");
         //TODO dialog: retry? Cancel?
      } while ( !success );
   }
   return session;
}


void PBEMServer::send( const GameMap* map, int lastPlayer, int lastturn )
{

   
   if ( choice_dlg("Do you want to upload the game to the ASC server?", "~y~es","~n~o") == 1) {
        
      
      if ( gameID == -1 )
         gameID = editInt( "turn", gameID );

      
      MemoryStreamStorage buffer;
      
      {
         MemoryStream stream ( &buffer, tnstream::writing );
         
         tnetworkloaders nwl;
         nwl.savenwgame( &stream, map );
      }
      
      // now we have the data to be transmitted in the buffer
      // buffer.
      
      ASC_PBEM* session = getSession();
      bool result = session->uploadFile( filename, buffer.getBuffer(), buffer.getSize(), gameID );
      if ( !result )
         warningMessage( "upload failed");
      
   } else {
      // writing to file  
      
   }
}


GameMap* PBEMServer::receive()
{
   /*
   GameMap* map = NULL;
   try {      
      tnfilestream gamefile ( filename, tnstream::reading );
      tnetworkloaders nwl;
      map = nwl.loadnwgame( &gamefile );
   } 
   catch ( tinvalidversion iv ) {
      throw iv;
   }
   catch ( tfileerror ) {
      errorMessage ( filename + " is not a legal email game" );
      return NULL;
   }
   return map;
   */
   return NULL;
}

ASCString PBEMServer::getDefaultServerAddress()
{
   
   // TODO getting from GameOptions...
   // return "http://84.59.97.144:8080/ascServer";
     return "http://localhost:8080/ascServer";
}


void PBEMServer::setServerAddress( const ASCString& address )
{
   serverAddress = address;
}


namespace {
   const bool r1 = networkTransferMechanismFactory::Instance().registerClass( PBEMServer::mechanismID(), ObjectCreator<GameTransferMechanism, PBEMServer> );
}

