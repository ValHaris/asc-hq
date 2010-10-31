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


#include "directresearchcommand.h"

#include "action-registry.h"
#include "../gamemap.h"
#include "../itemrepository.h"
#include "../researchexecution.h"

bool DirectResearchCommand::available( const Player& player )
{
   return player.research.activetechnology==NULL  // there is no research currently going on
         && player.research.goal  == NULL 
         && player.research.progress >0;     // and there are some research points to be spend
   
}


DirectResearchCommand::DirectResearchCommand( Player& player )
   : Command( player.getParentMap() ), researchProgress(-1), targetTechnologyID(-1)
{
   this->player = player.getPosition();
}


static const int directResearchCommandStreamVersion = 1;

void DirectResearchCommand::readData ( tnstream& stream )
{
   Command::readData( stream );
   int version = stream.readInt();
   if ( version < 1 || version > directResearchCommandStreamVersion )
      throw tinvalidversion ( "directResearchCommandStreamVersion", directResearchCommandStreamVersion, version );
   
   researchProgress = stream.readInt();
   targetTechnologyID = stream.readInt();
   player = stream.readInt();
   readClassContainer( immediatelyResearchedTechnologies, stream );
   readClassContainer( techAdaptersMadeAvailable, stream );
}


void DirectResearchCommand::writeData ( tnstream& stream ) const
{
   Command::writeData( stream );
   stream.writeInt( directResearchCommandStreamVersion );
   stream.writeInt( researchProgress );
   stream.writeInt( targetTechnologyID );
   stream.writeInt( player );
   writeClassContainer( immediatelyResearchedTechnologies, stream );
   writeClassContainer( techAdaptersMadeAvailable, stream );
}


vector<const Technology*> DirectResearchCommand::getAvailableTechnologies( bool longTerm )
{
   vector<const Technology*> techs;
   
   Player& p = getMap()->getPlayer(player);
   
   for (int i = 0; i < technologyRepository.getNum(); i++) {
      const Technology* tech = technologyRepository.getObject_byPos( i );
      if ( tech ) {
         if ( longTerm ) {
            if ( tech->eventually_available( p.research, NULL ))
               techs.push_back( tech );
         } else {
            ResearchAvailabilityStatus a = p.research.techAvailable ( tech );
            if ( a == Available )
               techs.push_back ( tech );
         }
      }
   }
   return techs;
}


void DirectResearchCommand::setTechnology( const Technology* tech )
{
   targetTechnologyID = tech->id;
   setState( SetUp );
}


ActionResult DirectResearchCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(22000);
   
   if ( !available( getMap()->getPlayer( player ) ))
      return ActionResult( 23201 );
   
   vector<const Technology*> techs = getAvailableTechnologies(true);
   const Technology* newTech = technologyRepository.getObject_byID( targetTechnologyID );
   if ( find ( techs.begin(), techs.end(), newTech ) == techs.end() )
      return ActionResult(23200);
   
   Research& research = getMap()->getPlayer( player ).research;
   
   if ( newTech->techDependency.available( research )) {
      research.activetechnology = newTech;
      research.goal = NULL;
   } else {
      list<const Technology*> techs;
      if ( newTech->eventually_available( research, &techs )) {
         research.activetechnology = *techs.begin();
         research.goal = newTech;
      } else
         return ActionResult( 23202 );
   }
   
   
   vector<const Technology*> researched;
   techAdaptersMadeAvailable.clear();
   runResearch( getMap()->getPlayer( player ), &researched, &techAdaptersMadeAvailable ); 
   for ( vector<const Technology*>::iterator i = researched.begin(); i != researched.end(); i++ )
      immediatelyResearchedTechnologies.push_back( (*i)->id );
   

   setState( Finished );
   
   return ActionResult(0);
}


ActionResult DirectResearchCommand::undoAction( const Context& context )
{
   if ( getState() != Finished )
      return ActionResult(22000);
   
   Research& research = getMap()->getPlayer( player ).research;
   
   for ( vector<int>::const_iterator i = immediatelyResearchedTechnologies.begin(); i != immediatelyResearchedTechnologies.end(); ++i ) {
      
      Technology* t = technologyRepository.getObject_byID( *i );
      if ( !t )
         return ActionResult(21005, ASCString::toString(*i));
      
      vector<int>::iterator j = find( research.developedTechnologies.begin(), research.developedTechnologies.end(), *i );
      if ( j != research.developedTechnologies.end() )
         research.developedTechnologies.erase(j);
      else
         return ActionResult(21204, t->name );
      
      research.progress += t->researchpoints;
   }
      
   for ( vector<ASCString>::iterator i = techAdaptersMadeAvailable.begin(); i != techAdaptersMadeAvailable.end(); ++i ) {
      Research::TriggeredTechAdapter::iterator ta = find( research.triggeredTechAdapter.begin(), research.triggeredTechAdapter.end(), *i );
      if ( ta == research.triggeredTechAdapter.end() )
         return ActionResult(21203, *i );
      else
         research.triggeredTechAdapter.erase( *i );
   }
   
   return Command::undoAction(context);
}


ASCString DirectResearchCommand :: getCommandString() const 
{
   ASCString c;
   c.format("setResearchGoal ( map, %d, %d  )", player, targetTechnologyID );
   return c;
}

GameActionID DirectResearchCommand::getID() const 
{
   return ActionRegistry::DirectResearchCommand;   
}

ASCString DirectResearchCommand::getDescription() const
{
   ASCString s = "Choose technology " + ASCString::toString(targetTechnologyID) + " for player " + ASCString::toString(player);
   return s;
}


namespace {
   const bool r1 = registerAction<DirectResearchCommand> ( ActionRegistry::DirectResearchCommand );
}

