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


#ifndef MoveUnitCommandH
#define MoveUnitCommandH

#include <set>
#include <sigc++/sigc++.h>

#include "unitcommand.h"
#include "taskinterface.h"

#include "../typen.h"
#include "../attack.h"
#include "../astar2.h"


class MoveUnitCommand : public UnitCommand, public TaskInterface, public SigC::Object {
   
   public:
      static bool avail ( Vehicle* eht );
      static bool ascendAvail ( Vehicle* eht );
      static bool descendAvail ( Vehicle* eht );
      typedef map<MapCoordinate,AttackWeap > FieldList;
      
   private:
      MapCoordinate3D destination;
      
      void fieldChecker( const MapCoordinate& pos );
      
      set<MapCoordinate3D> reachableFields;
      set<MapCoordinate3D> reachableFieldsIndirect;
      
      AStar3D::Path path;
      
      int flags;
      int verticalDirection;
      bool multiTurnMovement;
      
      MoveUnitCommand( GameMap* map ); 
      template<class Child> friend GameAction* GameActionCreator( GameMap* map);
      
      void changeCoordinates( const MapCoodinateVector& delta );
      
   protected:
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream ) const;
      
      GameActionID getID() const;
      ASCString getDescription() const;
      
      ActionResult go ( const Context& context ); 
      
   public:
      MoveUnitCommand ( Vehicle* unit );
      
      enum  SeachCapabilities { NoInterrupt = 1,   //!< the unit will not interrupt the movement if it runs onto a mine of into reaction fire
                                DisableHeightChange = 2, //!< the unit will not change its height even if that would provide a shortcut
                                LimitVerticalDirection = 4, //! we want only results on a certain level of height
                                ShortestHeightChange  = 8  //! no horizontal movement, only a single height change 
                               };
      
      void setFlags( int flags ) { this->flags = flags; };                               
                               
      ActionResult searchFields(int height = -1, int capabilities = 0);
      
      //! defines whether we want to end up either at the same level of height (0), lower(<0), or heigher(>0)
      void setVerticalDirection( int dir );
      
      int getVerticalDirection() const { return verticalDirection; };
      
      void calcPath();
      const AStar3D::Path& getPath();
      
      void setDestination( const MapCoordinate& destination );
      void setDestination( const MapCoordinate3D& destination );
      ASCString getCommandString() const;
      
      /** checks if the field can be reached by the unit this turn
      Precondition: searchFields(int,int) was called
      \note this function accepts a 2D position, so you can not check whether the
            unit can reach a certain level of height
            This is geared for usage by the GUI and less suited to AI, which should rather use isFieldReachable3D
            
      \param pos the destination field to check
      \param direct if true then only return true of the unit can stop on the destination field 
                    if false, then fields over which the unit can pass, but cannot stop (for example
                              because another unit is standing there) will also return true
      */
      bool isFieldReachable( const MapCoordinate& pos, bool direct );
      
      bool isFieldReachable3D( const MapCoordinate3D& pos, bool direct );
      
      
      /** this will return a 2D representation of the reachable fields.
          That means, if the unit can go to different levels of height on the same field, this
          set will only contain the level of height which can be reached with the least movement.
      
          These functions are geared towards the 2D user interface and are not well suited to AI usage */
      const set<MapCoordinate3D>& getReachableFields() { return reachableFields; };
      const set<MapCoordinate3D>& getReachableFieldsIndirect() { return reachableFieldsIndirect; };
      
      const Vehicle* getUnit() const { return UnitCommand::getUnit(); };
      Vehicle* getUnit() { return UnitCommand::getUnit(); };
      
      bool longDistAvailable( const MapCoordinate& pos );
      
      virtual vector<MapCoordinate> getCoordinates() const;
      
      int getCompletion();
      bool operatable();
      void rearm();
      
};

#endif

