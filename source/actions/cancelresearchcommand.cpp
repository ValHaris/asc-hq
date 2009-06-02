/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2008  Martin Bickel  and  Marc Schellenberger
 
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


#include "cancelresearchcommand.h"

#include "action-registry.h"
#include "../gamemap.h"
#include "../itemrepository.h"

CancelResearchCommand::CancelResearchCommand( GameMap* map )
   : Command( map ), researchProgress(-1), activeTechnologyID(-1), targetTechnologyID(-1), player(-1)
{
}


void CancelResearchCommand::setPlayer( const Player& player )
{
   this->player = player.getPosition();  
   setState( SetUp );
}


void CancelResearchCommand::readData ( tnstream& stream )
{
   Command::readData( stream );
   stream.readInt();
   researchProgress = stream.readInt();
   activeTechnologyID = stream.readInt();
   targetTechnologyID = stream.readInt();
   player = stream.readInt();
}


void CancelResearchCommand::writeData ( tnstream& stream ) const
{
   Command::writeData( stream );
   stream.writeInt( 1 );
   stream.writeInt( researchProgress );
   stream.writeInt( activeTechnologyID );
   stream.writeInt( targetTechnologyID );
   stream.writeInt( player );
}

ActionResult CancelResearchCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(22000);
   
   Research& research = getMap()->getPlayer( player ).research;
   
   researchProgress = research.progress;
   if ( research.activetechnology ) 
      activeTechnologyID = research.activetechnology->id;
   else
      activeTechnologyID = -1;
   
   if ( research.goal ) 
      targetTechnologyID = research.goal->id;
   else
      targetTechnologyID = -1;
   
   
   research.progress = 0;
   research.activetechnology = NULL;
   research.goal = NULL;

   setState( Completed );
   
   return ActionResult(0);
}

ActionResult CancelResearchCommand::preCheck()
{
   Research& research = getMap()->getPlayer( player ).research;
   
   int _progress = research.progress;
   int _activeTechnologyID;
   if ( research.activetechnology ) 
      _activeTechnologyID = research.activetechnology->id;
   else
      _activeTechnologyID = -1;
   
   int _targetTechnologyID;
   if ( research.goal ) 
      _targetTechnologyID = research.goal->id;
   else
      _targetTechnologyID = -1;
   
   
   if ( _progress != researchProgress
        || _activeTechnologyID != activeTechnologyID
        || _targetTechnologyID != targetTechnologyID )
         return ActionResult( 23000 );
   
   return ActionResult(0);
}

ActionResult CancelResearchCommand::undoAction( const Context& context )
{
   if ( getState() != Completed )
      return ActionResult(22000);
   
   Research& research = getMap()->getPlayer( player ).research;
   
   
   if ( activeTechnologyID != -1 ) {
      Technology* tech = technologyRepository.getObject_byID( activeTechnologyID );
      if ( !tech )
         return ActionResult( 23001 );
      else
         research.activetechnology = tech;
   }
   
   if ( targetTechnologyID != -1 ) {
      Technology* tech = technologyRepository.getObject_byID( targetTechnologyID );
      if ( !tech )
         return ActionResult( 23001 );
      else
         research.goal = tech;
   }

   research.progress = researchProgress;
   
   return Command::undoAction(context);
}


ASCString CancelResearchCommand :: getCommandString() const 
{
   ASCString c;
   c.format("cancelResearch ( map, %d )", player );
   return c;
}

GameActionID CancelResearchCommand::getID() const 
{
   return ActionRegistry::CancelResearchCommand;   
}

ASCString CancelResearchCommand::getDescription() const
{
   ASCString s = "Cancel research for player " + ASCString::toString(player);
   return s;
}


namespace {
   const bool r1 = registerAction<CancelResearchCommand> ( ActionRegistry::CancelResearchCommand );
}

