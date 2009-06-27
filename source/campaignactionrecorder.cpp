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

#include "campaignactionrecorder.h"

#include "dialogs/fileselector.h"



void CampaignActionLogger::endTurn( Player& player )
{
   gamemap->actions.breakUndo();
   commands.push_back( "-- Ending turn " + ASCString::toString( player.getParentMap()->time.turn() ) + "\n" );
   commands.push_back( "asc.endTurn()\n" );
}

void CampaignActionLogger::mapWon( Player& player )
{
   ASCString filename = selectFile( "*.lua", false );
   if ( !filename.empty() ) {
      tn_file_buf_stream stream ( filename, tnstream::writing );
      for ( CommandList::const_iterator i = commands.begin(); i != commands.end(); ++i )
         stream.writeString( *i, false );
   }
}


void CampaignActionLogger::commitCommand( GameMap* map, const Command& command )
{
   if ( map == gamemap && map->getCurrentPlayer().isHuman() ) {
      writer.printComment( command.getDescription() );
      writer.printCommand( command.getCommandString() );
   }
}

CampaignActionLogger::CampaignActionLogger ( GameMap* map ) : gamemap ( map ), commands(), writer( commands )
{
   gamemap->sigPlayerUserInteractionEnds.connect( SigC::slot( *this, &CampaignActionLogger::endTurn ));
   gamemap->sigMapWon.connect( SigC::slot( *this, &CampaignActionLogger::mapWon ));
   gamemap->actions.commitCommand.connect( SigC::slot( *this, &CampaignActionLogger::commitCommand ));
}


void CampaignActionLogger::readData ( tnstream& stream )
{
   stream.readInt();
   int count = stream.readInt();
   for ( int i = 0; i <count; ++i ) {
      ASCString s;
      stream.readTextString( s, true );
      commands.push_back( s );
   }
   int check = stream.readInt();
   if ( check != 0xbac0 )
      throw ASCmsgException("marker not matched when loading CampaignActionLogger");
      
}

void CampaignActionLogger::writeData ( tnstream& stream )
{
   stream.writeInt(1);
   stream.writeInt( commands.size() );
   for ( CommandList::const_iterator i = commands.begin(); i != commands.end(); ++i ) {
      stream.writeString( *i );
   }
   stream.writeInt( 0xbac0 );
};

