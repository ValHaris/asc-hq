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


#ifndef PutObjectCommandH
#define PutObjectCommandH

#include "unitcommand.h"

#include "../typen.h"
#include "../objects.h"
#include "../mapfield.h"


class PutObjectCommand : public UnitCommand {
   
   public:
      static bool avail ( Vehicle* eht );
   private:
      enum Mode { Build, Remove };
      
      MapCoordinate target;
      int object;
      Mode mode;
      
      void fieldChecker( const MapCoordinate& pos );
      
      map<MapCoordinate,vector<int> > objectsCreatable;
      map<MapCoordinate,vector<int> > objectsRemovable;
      
      bool checkObject( MapField* fld, ObjectType* objtype, Mode mode );
      
      
      PutObjectCommand( GameMap* map ) : UnitCommand( map ) {};
      template<class Child> friend GameAction* GameActionCreator( GameMap* map);
      
   protected:
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream ) const;
      
      GameActionID getID() const;
      ASCString getDescription() const;
      
   public:
      PutObjectCommand ( Vehicle* unit );
      ActionResult searchFields();
      
      Vehicle* getAttacker() { return getUnit(); };
      
      void setTarget( const MapCoordinate& target, int objectID );
      ActionResult go ( const Context& context ); 
      ASCString getCommandString() const;
      
      const vector<int>& getCreatableObjects( const MapCoordinate& pos );
      const vector<int>& getRemovableObjects( const MapCoordinate& pos ); 
      
      vector<MapCoordinate> getFields();
};

#endif

