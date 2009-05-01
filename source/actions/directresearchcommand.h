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


#ifndef DirectResearchCommandH
#define DirectResearchCommandH

#include <vector>

#include "command.h"

class Player;
class Technology;

//! cancels the research on the current technology, losing all research points
class DirectResearchCommand : public Command {
   private:
      int researchProgress;
      int targetTechnologyID;
      int player;
      vector<int> immediatelyResearchedTechnologies;
      vector<ASCString> techAdaptersMadeAvailable;
      
      DirectResearchCommand( GameMap* map ) : Command( map ) {};
      template<class Child> friend GameAction* GameActionCreator( GameMap* map);
      
      
   protected:
      
      GameActionID getID() const;
      ASCString getDescription() const;
      
      ActionResult go ( const Context& context ); 
      
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream ) const;
      
      ActionResult undoAction( const Context& context );
      
   public:
      static bool available( const Player& player );
      
      DirectResearchCommand( Player& player );
      
      /** returns the technologies that can be researched by the player
          \param longTerm if false, then only technologies are returned, for which all
                          prerequisite technologies are already researched.
                          if true, then also return technologies for which the prerequisite
                          technologies must still be researched
      */
      vector<const Technology*> getAvailableTechnologies( bool longTerm );
      
      void setTechnology( const Technology* tech );
      
      ASCString getCommandString() const ;
      
};


#endif

