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


#ifndef MoveUnitCommandH
#define MoveUnitCommandH

#include <set>

#include "unitcommand.h"

#include "../typen.h"
#include "../attack.h"
#include "../astar2.h"


class MoveUnitCommand : public UnitCommand {
   
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
      
      MoveUnitCommand( GameMap* map ) : UnitCommand( map ) {};
      template<class Child> friend GameAction* GameActionCreator( GameMap* map);
      
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
      
      ActionResult searchFields(int height = -1, int capabilities = 0);
      
      //! defines whether we want to end up either at the same level of height (0), lower(<0), or heigher(>0)
      void setVerticalDirection( int dir );
      
      int getVerticalDirection() const { return verticalDirection; };
      
      void calcPath();
      const AStar3D::Path& getPath();
      
      void setDestination( const MapCoordinate& destination );
      void setDestination( const MapCoordinate3D& destination );
      ASCString getCommandString() const;
      
      bool isFieldReachable( const MapCoordinate& pos, bool direct );
      
      const set<MapCoordinate3D>& getReachableFields() { return reachableFields; };
      const set<MapCoordinate3D>& getReachableFieldsIndirect() { return reachableFieldsIndirect; };
};

#endif

