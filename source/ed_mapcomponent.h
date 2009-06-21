/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

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

#ifndef edmapcomponentH
#define edmapcomponentH

#include <algorithm>

#include <paragui.h>
#include <pgwidget.h>

#include "mapitemtype.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "objecttype.h"
#include "objects.h"


extern SigC::Signal0<void> filtersChangedSignal;


class Placeable {
   public:
      virtual ~Placeable() {};
      virtual bool supportMultiFieldPlacement() const = 0;
      virtual int place( const MapCoordinate& mc ) const = 0;
       //! just a wrapper so we have a function return void
      void vPlace( const MapCoordinate& mc ) const { place( mc ); };
      virtual Placeable* clone() const = 0;
      virtual bool remove ( const MapCoordinate& mc ) const { return false; };
      virtual ASCString getName() const = 0;
};

/** A MapComponent represents any kind of item that can be placed on the map by the user.
    It is the abstract class on which the Brush edit function operates
*/
class MapComponent : public Placeable {
       static int currentPlayer;
       static void setPlayer( int player );
       static bool initialized;
    protected:
       const MapItemType* mapItem;
       virtual Surface& getClippingSurface() const = 0;
       int getPlayer() const { return currentPlayer; };
    public:
       MapComponent( const MapItemType* item );
       static const int fontHeight = 20;
       const MapItemType* getItemType() const { return mapItem; };
       virtual int displayWidth() const = 0;
       virtual int displayHeight() const = 0;
       virtual void display( Surface& s, const SPoint& pos ) const = 0;
       virtual bool supportMultiFieldPlacement() const { return true; };
       void displayClip( PG_Widget* parent, SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst ) const;
       virtual ASCString getName() const  { return mapItem->getName(); };
};


template<class Item> class BasicItem : public MapComponent {
    protected:
       const Item* item;
       static Surface clippingSurface;
       Surface& getClippingSurface() const { return clippingSurface; };
    public:
       BasicItem( const Item* i ) : MapComponent(i), item( i ) {};
       ASCString getName() const { return item->getName(); };
       virtual int displayWidth() const { return Width(); };
       static  int Width() { return fieldsizex; };
       virtual int displayHeight() const { return Height(); };
       static int Height() { return fieldsizey; };
       virtual int getID() const { return item->id; };
};

template<class C> class ItemTypeSelector {};

class VehicleItem : public BasicItem<Vehicletype> {
    public:
       VehicleItem( const Vehicletype* vehicle ) : BasicItem<Vehicletype>( vehicle ) {};
       virtual int place( const MapCoordinate& mc ) const ;
       static int place( GameMap* gamemap, const MapCoordinate& mc, const Vehicletype* v, int owner );
       virtual void display( Surface& s, const SPoint& pos ) const;
       virtual MapComponent* clone() const { return new VehicleItem( item ); };
};
template<> class ItemTypeSelector<Vehicletype> {
   public:
      typedef VehicleItem type;
};



class BuildingItem : public MapComponent {
       const BuildingType* bld;
       static Surface clippingSurface;
       static Surface fullSizeImage;
    protected:
       Surface& getClippingSurface() const { return clippingSurface; };
    public:
       BuildingItem( const BuildingType* building ) : MapComponent( building ), bld( building ) {};
       ASCString getName() const { return bld->getName(); };
       virtual int displayWidth() const { return Width(); };
       static  int Width() { return (fieldsizex+(4-1)*fielddistx+fielddisthalfx)/2; };
       virtual int displayHeight() const { return Height(); };
       static int Height() { return (fieldsizey+(6-1)*fielddisty)/2; };
       virtual bool supportMultiFieldPlacement() const { return false; };
       virtual int place( const MapCoordinate& mc ) const ;
       virtual void display( Surface& s, const SPoint& pos ) const;
       virtual MapComponent* clone() const { return new BuildingItem( bld ); };
       virtual int getID() const { return bld->id; };
};
template<> class ItemTypeSelector<BuildingType> {
   public:
      typedef BuildingItem type;
};


class ObjectItem : public BasicItem<ObjectType> {
    public:
       ObjectItem( const ObjectType* object ) : BasicItem<ObjectType>( object ) {};
       virtual int place( const MapCoordinate& mc ) const;
       virtual void display( Surface& s, const SPoint& pos ) const { item->display (s, pos); };
       virtual MapComponent* clone() const { return new ObjectItem( item ); };
       virtual bool remove ( const MapCoordinate& mc ) const;
};
template<> class ItemTypeSelector<ObjectType> {
   public:
      typedef ObjectItem type;
};


class TerrainItem : public BasicItem<TerrainType> {
    public:
       TerrainItem( const TerrainType* object ) : BasicItem<TerrainType>( object ) {};
       virtual int place( const MapCoordinate& mc ) const;
       virtual void display( Surface& s, const SPoint& pos ) const { item->weather[0]->paint (s, pos); };
       virtual MapComponent* clone() const { return new TerrainItem( item ); };
};

template<> class ItemTypeSelector<TerrainType> {
   public:
      typedef TerrainItem type;
};




class MineItem : public BasicItem<MineType> {
    public:
       MineItem( const MineType* object ) : BasicItem<MineType>( object ) {};
       virtual int place( const MapCoordinate& mc ) const;
       virtual void display( Surface& s, const SPoint& pos ) const { item->paint(s, pos); };
       virtual MapComponent* clone() const { return new MineItem( item ); };
};

template<> class ItemTypeSelector<MineType> {
   public:
      typedef MineItem type;
};



class LuaBrush : public Placeable {
   protected:
      ASCString script;
   public:
      LuaBrush( const ASCString& filename );
      bool supportMultiFieldPlacement() const { return false; };
      int place( const MapCoordinate& mc ) const;
      virtual LuaBrush* clone() const;
      virtual ASCString getName() const { return script; };
};


#endif
