/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef weaponrangelayerH
#define weaponrangelayerH

#include "typen.h"
#include "vehicle.h"


#include "mapdisplayinterface.h"

#include "mapdisplay.h"




class UnitWeaponRangeLayer : public MapLayer {
      Surface& icon1;
      Surface& icon2;
      GameMap* gamemap;
   
      map<MapCoordinate,int> fields;
   
      void markField( const MapCoordinate& pos );
   
      bool addUnit( Vehicle* veh );
      void reset();
      
      bool fieldVisible( const MapCoordinate& pos );
      
   public:

      void operateField( GameMap* actmap, const MapCoordinate& pos );
      
      UnitWeaponRangeLayer();

      bool onLayer( int layer ) { return layer == 17; };
      
      void paintSingleField( const MapRenderer::FieldRenderInfo& fieldInfo,  int layer, const SPoint& pos );
};


#endif
