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


double StatisticsCalculator::strength( const ContainerBase* c, bool recurse )
{
   double s = 0;
   const Vehicle* veh = dynamic_cast<const Vehicle*>(c);
   if ( veh ) {
      s = veh->typ->productionCost.energy + veh->typ->productionCost.material;
      AttackFormula af( c->getMap() );
      s *= (af.strength_experience( veh->experience) + af.defense_experience( veh->experience))/2 + 1.0 ;
      s *= af.strength_damage( veh->damage );
      s/=10000;
   }
   
   if ( recurse )
      for ( ContainerBase::Cargo::const_iterator i = c->getCargo().begin(); i != c->getCargo().end(); ++i )
         if ( *i )
            s += strength( *i, recurse );
   
   return s;
}

int StatisticsCalculator::unitCount( const ContainerBase* c, bool recurse )
{
   int counter = 1;
   if ( recurse )
      for ( ContainerBase::Cargo::const_iterator i = c->getCargo().begin(); i != c->getCargo().end(); ++i )
      if ( *i )
         counter += unitCount( *i, recurse );
   return counter;  
}


Resources StatisticsCalculator::resource(const ContainerBase* c, bool recurse )
{
   Resources res = c->baseType->productionCost;
   for ( int r = 0; r < 3; ++r )
      res.resource(r) += c->getAvailableResource( maxint, r, 0 );
   
   if ( recurse )
   for ( ContainerBase::Cargo::const_iterator i = c->getCargo().begin(); i != c->getCargo().end(); ++i )
      if ( *i )
         res += resource(*i, recurse);
      
   return res;
}

int StatisticsCalculator::unitCost( const ContainerBase* c, bool recurse )
{
   Resources res = c->baseType->productionCost;
   
   if ( recurse )
      for ( ContainerBase::Cargo::const_iterator i = c->getCargo().begin(); i != c->getCargo().end(); ++i )
      if ( *i )
         res.material += unitCost(*i, recurse);
      
   return res.material;
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
         int viewDominance = 0;
         for ( int x = 0; x < actmap->xsize; x++ )
            for ( int y = 0; y < actmap->ysize; y++ ) {
               tfield* fld = actmap->getField ( x, y );
               VisibilityStates vs = fieldVisibility  ( fld, i );
               switch ( vs ) {
                  case visible_not:
                     ++notVisible;
                     break;
                  case visible_ago:
                     ++fogOfWar;
                     break;
                  default:
                     ++visible;
               }

               if ( vs == visible_all || vs == visible_now ) {
                  int maxView = -1;
                  for ( int p = 0; p < actmap->getPlayerCount(); ++p )
                     if ( fld->view[p].view > maxView ) 
                        maxView = fld->view[p].view;
   
                  if ( fld->view[i].view == maxView )
                     ++viewDominance;
               }

            }
         msg += ASCString("  not visible: ")   + ASCString::toString(notVisible )    + " fields\n";
         msg += ASCString("  fog of war: ")    + ASCString::toString(fogOfWar )      + " fields\n";
         msg += ASCString("  visible: ")       + ASCString::toString(visible )       + " fields\n";
         msg += ASCString("  view dominant: ") + ASCString::toString(viewDominance ) + " fields\n\n";
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
      for ( Player::VehicleList::iterator j = gamemap->player[i].vehicleList.begin(); j != gamemap->player[i].vehicleList.end(); ++j ) {
         strength += StatisticsCalculator::strength( *j, false );
         r += (*j)->typ->productionCost;
         total += StatisticsCalculator::resource( *j, false );
      }

      for ( Player::BuildingList::iterator j = gamemap->player[i].buildingList.begin(); j != gamemap->player[i].buildingList.end(); ++j ) {
         total += StatisticsCalculator::resource( *j, false );
      }

      message += ASCString("#fontsize=14#Player ") + ASCString::toString( i ) + ": "+  gamemap->player[i].getName() +  "#fontsize=12#\n" ;
      message += "strength: ";
      ASCString s;
      s.format("%9.0f", ceil(strength) );
      message += s + "\n";
      message += "Unit production cost ";
      for ( int k = 1; k < 2; ++k ) { // just material
         message += resourceNames[k];
         message += ": " + ASCString::toString(r.resource(k)/1000 ) + "k\n";
      }
      message += "Unit count: " + ASCString::toString( int( gamemap->player[i].vehicleList.size())) + "\n";
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

