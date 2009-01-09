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


#ifndef actionH
#define actionH


#include "../typen.h"
#include "../ascstring.h"
#include "../basestrm.h"
#include "../util/factorywithnames.h"

#include "context.h"
#include "actionresult.h"

class Vehicle;
class GameMap;


typedef int GameActionID;

/** A #GameAction is an modification of something on a map
        
    \see gameinteraction for an exhaustive description how this works
*/

class GameAction {
   
      typedef deallocating_vector<GameAction*> Children;
      Children children;
   
      GameMap* gamemap;
      
      bool done;
      
      int sequenceNumber;
      
   protected:
      
      GameAction( GameMap* map );
      
      //! adds a child action. Ownership of the child action is passed to THIS
      void addChild( GameAction* action );
      
      virtual ActionResult runAction( const Context& context ) = 0;
      virtual ActionResult undoAction( const Context& context ) = 0;
      virtual ActionResult preCheck()  {return ActionResult(0);};
      virtual ActionResult postCheck() {return ActionResult(0);};
      
      virtual void readData ( tnstream& stream ) = 0;
      virtual void writeData ( tnstream& stream ) const = 0;
      
      virtual GameActionID getID() const = 0;
      
      GameMap* getMap() { return gamemap; };
      const GameMap* getMap() const { return gamemap; } ;
      
   public:
      ActionResult execute( const Context& context );
      void undo( const Context& context );
      
      void read ( tnstream& stream );
      void write ( tnstream& stream ) const;
   
      virtual ASCString getDescription() const = 0;
      
      virtual ~GameAction() {};
      
      static GameAction* readFromStream( tnstream& stream, GameMap* map );
};


typedef Loki::SingletonHolder< Factory1< GameAction , GameActionID, GameMap*  > > gameActionFactory;

template<class Derived>
GameAction* GameActionCreator( GameMap* map )
{
   return new Derived( map );
}

template <typename ActionType > 
bool registerAction( GameActionID id )
{
   return gameActionFactory::Instance().registerClass( id, GameActionCreator<ActionType> );
}


#endif

