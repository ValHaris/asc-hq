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


#ifndef CommandH
#define CommandH

#include "action.h"

/*! \page gameinteraction Interacting with the game
  
  This chapter covers ASC game itself, the mapeditor is working differently in that it doesn't have any Commands or GameActions.

  \section readony Read-only access
  All major classes provide lots of methods to extract information out of ASC. 
  But you should be very careful about doing any modifications.
  Lots of classes must not be changed, like all the Type classes (#Vehicletype, #BuildingType, #ObjectType, #TerrainType, #Technology).

  \section modifications Changing the map
  Other items (like #Vehicle, #Building) will definitly be changed. But this should not be done through write access to 
  these instances, but rather through the classes derived from #Command . This is the classic Command design pattern.

  The commands ensure that all modifications are recorded in the gamemap, which servers several purposes:
  \li the player can undo and redo these modifications
  \li the replay is based on these commands to show your moves to the other players (well, only the moves that happen in 
      their radar coverage, of course)
  \li the replay is also the ensurance against illegal modifcations for cheating. Running the same Commands on the other 
      player's computers must result in exactly the same modifications to the map

  \section history History  
  The introduction of the Command pattern is a relatively new addition the ASC engine. Not all modifications are have been converted
  to Command classes. This is an ongoing effort.
  If you are writing new code that modifies the map, you should either use existing Commands (see the descends of the #Command class) 
  or write new child-Command classes.

  \section Commands Commands and GameActions
  While #Command derives from #GameAction, there is a very severe distinction between #Command and non-command #GameAction

  A Command is a top level command from the user to some building, vehicle of something. A user never issues normal #GameAction
  The Command may need to do some lower-level modifications of the map, which are governed by the game machanis.

  Here is a (simplified) example, showing what happens if the user wants to move a tank from one field to a neighbouring field.
  A #MoveUnitCommand object is created, which will then create lots of child #GameAction objects, breaking all the map modifications
  down into atomic actions.

  \verbatim
      MoveUnitCommand
       + Unregister unit from starting field (Action: UnitFieldRegistration)
       + put a track object on departing field (Action: SpawnObject)
       [display animation of moving tank] 
       + put a track object on destination field (Action: SpawnObject) 
       + Register unit on destination field (Action: UnitFieldRegistration)
       + consume fuel for movement (Action: ConsumeResource)
       [an enemy unit start reactionfire]
       + attack Tank with EnemyUnit
       ++ Enemy unit consumes one piece of ammo (Action: ConsumeAmmo)
       ++ Enemy unit gets experience (Action: ChangeUnitProperty)
       ++ Tank is damaged (Action: ChangeUnitProperty)
       [ damage is 100%, so our Tank is destroyed ]
       +++ Tank is destroyed and deleted from map (Action: DestructContainer)
       ++++ Wrackage object is created (Action: SpawnObject)
       + View is recalculated (Action: ViewRegistration)
  \endverbatim

*/


/** A Command is an action that the player initiates. 
    It contains all the logic about when the command is available and which options it may be given.
    For example, the PutObjectCommand will provide all information which objects a unit may produce, and 
    on which fields this is possible.

    Unlike the atomic #GameAction, a prepared but not yet executed #Command can still be run 
    in a changed environment compared to the time it was initially issued. It may, of course, report a failure
    if the intervening map modification has negative impacts on the command.

    Once a #Command is successfully completed, it adds itself automatically to the undo list of the current #GameMap.
    By doing so, ownership of the #Command object passes to the #GameMap.

    If a #Command does NOT complete successfully, it must be manually disposed 
    (auto_ptr is very useful to do that reliably).

    Typcial usage pattern of a command is:
    \code
      auto_ptr<DoSomethingCommand> command ( new DoSomethingCommand( subject ));
      command->prepareCommand( whatEverInput );
      ActionResult result = command->execute( context );
      if ( result.successful() )
         command.release();
      else 
         reportError ( res ); 
    \endcode

    \see GameAction
*/

class Command : public GameAction {
   public:
      enum State { Planned, Evaluated, SetUp, Completed, Failed };
   private:
      State state;
   public:
      State getState() const { return state; };
      
      virtual ASCString getCommandString() const = 0;
      
      ActionResult redo( const Context& context );
      
      //! this is for informational purposes, so the player can see where a Command has taken place when reviewing it
      virtual vector<MapCoordinate> getCoordinates() const;
      
   protected:
      Command( GameMap* gamemap );
      
      void setState( State state );
      
      virtual ActionResult go ( const Context& context ) = 0; 
      
      virtual ActionResult runAction( const Context& context ) ;
      virtual ActionResult undoAction( const Context& context );
      
      virtual void readData ( tnstream& stream );
      virtual void writeData ( tnstream& stream ) const;
            
      virtual ActionResult checkExecutionPrecondition() const;
};

#endif

