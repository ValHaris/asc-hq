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


#ifndef fieldmarkerH
#define fieldmarkerH

#include "../paradialog.h"

class MapDisplayPG;
class OverviewMapPanel;
class GameMap;

class SelectFromMap : public ASC_PG_Dialog{
   private:
      MapDisplayPG* md;
      OverviewMapPanel* omp;

      PG_ListBox* listbox;
      bool justOne;
      bool readOnly;

      bool markField2( const MapCoordinate& pos, const SPoint& mouse, bool cursorChanged, int button, int prio );
      int oldprio;
      
   public:
      typedef vector<MapCoordinate> CoordinateList;
   protected:
      GameMap* actmap;
      CoordinateList& coordinateList;
      
      bool ProcessEvent ( const SDL_Event *   event,bool   bModal = false  );
      virtual bool accept( const MapCoordinate& pos );
      virtual void showFieldMarking( const CoordinateList& coordinateList );
      virtual bool mark();
      virtual bool markField( const MapCoordinate& pos );
      bool eventKeyDown (const SDL_KeyboardEvent *key);
      void updateList();
      bool listItemClicked( PG_ListBoxBaseItem* item );
      virtual bool isOk() { return true; };
      bool closeDialog() { if ( isOk()) QuitModal(); return true; };
   public:   
      SelectFromMap( CoordinateList& list, GameMap* map, bool justOne = false, bool readOnly = false );
      void Show( bool fade = false );
      ~SelectFromMap();
         
};


class SelectBuildingFromMap : public SelectFromMap {
   protected:
      virtual bool accept( const MapCoordinate& pos );
      virtual bool isOk() { return !coordinateList.empty(); };
   public:
      SelectBuildingFromMap ( CoordinateList& list, GameMap* map ) : SelectFromMap ( list, map, true ) {};
};

class SelectUnitFromMap : public SelectFromMap {
   protected:
      virtual bool accept( const MapCoordinate& pos );
      virtual bool isOk() { return !coordinateList.empty(); };
   public:
      SelectUnitFromMap ( CoordinateList& list, GameMap* map ) : SelectFromMap ( list, map, true ) {};
};




#endif

