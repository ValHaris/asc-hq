/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2008  Martin Bickel  

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


#ifndef commonH
#define commonH

#include "../typen.h"

#include "../paradialog.h"

class GameMap;
class ObjectType;
class BuildingType;
class Vehicletype;
class TerrainType;

extern GameMap* getActiveMap();

extern const ObjectType* getObjectType( int id );
extern const BuildingType* getBuildingType( int id );
extern const Vehicletype* getUnitType( int id );
extern const TerrainType* getTerrainType( int id );

class StringArray {
   public:
      vector<ASCString> values;
      StringArray(){};
      void add( const ASCString& s ) { values.push_back( s ); };
      ASCString getItem( int n ) //!< 1 based, in best LUA tradition 
      {
         return (n> 0 && n <= values.size()) ? values[n-1] : ASCString();
      }
      int size() { return values.size(); }; //
};

class PropertyDialog : public ASC_PG_Dialog {
   private:
      PG_PropertyEditor* propertyEditor;
      bool result;
   
      bool ok();
      bool cancel();
      
      map<ASCString,bool> boolValues;
      map<ASCString,int>  intValues;
      map<ASCString,ASCString> stringValues;
      
   public:
      PropertyDialog( const ASCString& title );
      void addBool( const ASCString& name, bool defaultValue );
      void addInteger( const ASCString& name, int defaultValue );
      void addIntDropdown ( const ASCString& name, const StringArray& names, int defaultValue );
      void addString( const ASCString& name, const ASCString& defaultValue );
      
      std::string getString( const ASCString& name );
      int getInteger( const ASCString& name );
      bool getBool( const ASCString& name );
      bool run();
};

extern int selectString ( const ASCString& title, const StringArray& entries, int defaultEntry = -1 );

extern GameMap* getLoadingMap();

extern void setLocalizedEventMessage( GameMap* map, int eventID, const ASCString& message );
extern void setLocalizedContainerName( GameMap* map, const MapCoordinate& pos, const std::string& name );

extern MapCoordinate getCursorPosition( const GameMap* gamemap );



#endif

