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

#include <cmath>

#include "util/messaginghub.h"
#include "statistics.h"
#include "vehicle.h"
#include "viewcalculation.h"
#include "attack.h"
#include "spfst.h"
#include "widgets/textrenderer.h"

StatisticsCalculator::StatisticsCalculator( const ContainerBase* unit )
{
   this->unit = unit;  
}

double StatisticsCalculator::strength()
{
   const Vehicle* veh = dynamic_cast<const Vehicle*>(unit);
   if ( veh ) {
      double s = veh->typ->productionCost.energy + veh->typ->productionCost.material;
      AttackFormula af;
      s *= (af.strength_experience( veh->experience) + af.defense_experience( veh->experience))/2 + 1.0 ;
      s *= af.strength_damage( veh->damage );
      return s/10000;
   } else {
      return 0;
   }
}

Resources StatisticsCalculator::resource()
{
   Resources res = unit->baseType->productionCost;
   for ( int r = 0; r < 3; ++r )
      res.resource(r) += unit->getAvailableResource( maxint, r, 0 );
   return res;
}



ASCString getVisibilityStatistics( GameMap* actmap )
{
   ASCString msg;

   computeview ( actmap, 0, true );

   for ( int i = 0; i < actmap->getPlayerCount(); i++ ) {
      if ( actmap->player[i].exist() ) {
         msg += ASCString("#fontsize=14#Player ") + ASCString::toString( i ) + ": "+  actmap->player[i].getName() +  "#fontsize=12#\n" ;
         int notVisible = 0;
         int fogOfWar = 0;
         int visible = 0;
         for ( int x = 0; x < actmap->xsize; x++ )
            for ( int y = 0; y < actmap->ysize; y++ ) {
                VisibilityStates vs = fieldVisibility  ( actmap->getField ( x, y ), i );
                switch ( vs ) {
                   case visible_not: ++notVisible;
                   break;
                   case visible_ago: ++fogOfWar;
                   break;
                   default: ++visible;
                }
            }
         msg += ASCString("  not visible: ") + ASCString::toString(notVisible ) + " fields\n";
         msg += ASCString("  fog of war: ")  + ASCString::toString(fogOfWar ) + " fields\n";
         msg += ASCString("  visible: ")     + ASCString::toString(visible ) + " fields\n\n";
      } 
   }

   computeview ( actmap, 0 , false );

   return msg;
}

ASCString getPlayerStrength( GameMap* gamemap )
{
   ASCString message;
   for ( int i = 0; i< gamemap->getPlayerCount(); ++i ) {
      double strength = 0;
      Resources r;
      Resources total;
      for ( Player::VehicleList::iterator j = actmap->player[i].vehicleList.begin(); j != actmap->player[i].vehicleList.end(); ++j ) {
         StatisticsCalculator sc ( *j );
         strength += sc.strength();
         r += (*j)->typ->productionCost;
         total += sc.resource();
      }

      for ( Player::BuildingList::iterator j = actmap->player[i].buildingList.begin(); j != actmap->player[i].buildingList.end(); ++j ) {
         StatisticsCalculator sc ( *j );
         total += sc.resource();
      }

      message += ASCString("#fontsize=14#Player ") + ASCString::toString( i ) + ": "+  actmap->player[i].getName() +  "#fontsize=12#\n" ;
      message += "strength: ";
      ASCString s;
      s.format("%9.0f", ceil(strength) );
      message += s + "\n";
      message += "Unit production cost ";
      for ( int k = 1; k < 2; ++k ) { // just material
         message += resourceNames[k];
         message += ": " + ASCString::toString(r.resource(k)/1000 ) + "k\n";
      }
      message += "Unit count: " + ASCString::toString( int( actmap->player[i].vehicleList.size())) + "\n";
      message += "Material index: " + ASCString::toString( total.material/1000 ) + "k\n";
      message += "\n\n";

   }
   return message;
}



void pbpplayerstatistics( GameMap* gamemap )
{
   ASCString msg;
   { 
      StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "calculating... " );

      msg = "#fontsize=18#Map Statistics for " + gamemap->maptitle + "#fontsize=12#\n\n"; 
      
      msg += "#fontsize=16#Visibility#fontsize=12#\n";
      msg += getVisibilityStatistics( gamemap );
      
      msg += "#fontsize=16#Strength#fontsize=12#\n";
      msg += getPlayerStrength( gamemap );
   }
   
   ViewFormattedText vft ( "Map Statistics", msg, PG_Rect(-1,-1,600,600));
   vft.Show();
   vft.RunModal();
}

