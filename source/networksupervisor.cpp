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
#include "dialogs/pwd_dlg.h"
#include "stack.h"
#include "loaders.h"
#include "gamemap.h"
#include "turncontrol.h"

#include "dialogs/admingame.h"


void networksupervisor ()
{
   ASCString filename = selectFile( ASCString("*") + tournamentextension + ";*.asc", true );
   if ( filename.empty() )
      return;

   StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "loading " + filename );
   FileTransfer ft;
   auto_ptr<GameMap> newMap ( mapLoadingExceptionChecker( filename, MapLoadingFunction( &ft, &FileTransfer::loadPBEMFile )));
   if ( !newMap.get() )
      return;

   Password pwd = newMap->supervisorpasswordcrc;

   if( pwd.empty() ) {
      for ( int i = 0; i < newMap->getPlayerCount(); ++i )
         if ( newMap->getPlayer(i).stat == Player::supervisor ) {
            pwd = newMap->getPlayer(i).passwordcrc;
            break;
         }

      if ( pwd.empty() ) {
         errorMessage ("no supervisor setup in this game!" );
         return;
      }
   }

   smw.close();
  

   try {
      bool ok = enterpassword ( newMap->supervisorpasswordcrc );
      if ( !ok ) {
         errorMessage ("invalid password!" );
         return;
      }
   }
   catch ( ... ) {
      return;
   }

   TurnSkipper ts ( &skipTurn );
   
   if ( adminGame( newMap.get(), &ts  ) ) {
      if ( !newMap->network ) {
         errorMessage("no network set up for game");
         return;
      }
      newMap->network->send( newMap.get(), -1, -1 );
   }

}


