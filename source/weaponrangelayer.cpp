
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "weaponrangelayer.h"
#include "gamemap.h"
#include "spfst.h"
#include "iconrepository.h"

void UnitWeaponRangeLayer::markField( const MapCoordinate& pos )
{
   fields[pos] |= 1;
}

bool UnitWeaponRangeLayer::fieldVisible( const MapCoordinate& pos )
{
#ifdef karteneditor
   return true;
#else
   return fieldvisiblenow ( gamemap->getField( pos ));
#endif
}


bool UnitWeaponRangeLayer::addUnit( Vehicle* veh )
{
   if ( fieldVisible( MapCoordinate(veh->xpos, veh->ypos ))) {
      int found = 0;
      for ( int i = 0; i < veh->typ->weapons.count; i++ ) {
         if ( veh->typ->weapons.weapon[i].shootable() ) {
            circularFieldIterator( gamemap,veh->getPosition(), veh->typ->weapons.weapon[i].maxdistance/minmalq, (veh->typ->weapons.weapon[i].mindistance+maxmalq-1)/maxmalq, FieldIterationFunctor( this, &UnitWeaponRangeLayer::markField )  );
            found++;
         }
      }
      if ( found )
         fields[veh->getPosition()] |= 2;
         
      return found;
   } else
      return false;
};

void UnitWeaponRangeLayer::reset()
{
   fields.clear();
}

void UnitWeaponRangeLayer::operateField( GameMap* actmap, const MapCoordinate& pos )
   {
      if ( !pos.valid() )
         return;
      
      if ( gamemap && gamemap != actmap ) 
         reset();

      gamemap = actmap;
      
      if ( fields.find( pos ) != fields.end() ) {
         if ( fields[pos] & 2 ) {
            reset();
            setActive(false);
            statusMessage("Weapon range layer disabled");
            repaintMap();
            return;
         }
      }
      
      if ( actmap->getField( pos )->vehicle ) {
         if ( addUnit( actmap->getField( pos )->vehicle ) ) {
            setActive(true);
            statusMessage("Weapon range layer enabled");
            repaintMap();
         }
      }
   }
   
   UnitWeaponRangeLayer::UnitWeaponRangeLayer() : icon1 ( IconRepository::getIcon( "markedfield-red.png")), icon2 ( IconRepository::getIcon( "markedfield-red2.png")), gamemap(NULL) {
      // cursorMoved.connect( sigc::mem_fun( *this, UnitWeaponRangeLayer::cursorMoved ));
   }

   void UnitWeaponRangeLayer::paintSingleField( const MapRenderer::FieldRenderInfo& fieldInfo,  int layer, const SPoint& pos )
   {
      if ( fieldInfo.gamemap != gamemap && gamemap) {
         reset();
         gamemap = NULL;
         return;
      }
      
      if ( fieldInfo.visibility >= visible_ago) {
         if ( fields.find( fieldInfo.pos ) != fields.end() ) {
            int p = fields[fieldInfo.pos];
            if ( p & 1 )
               fieldInfo.surface.Blit( icon1, pos );
            if ( p & 2 )
               fieldInfo.surface.Blit( icon2, pos );
         }
      }
   }

