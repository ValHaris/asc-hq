/***************************************************************************
                          cargodialog.cpp  -  description
                             -------------------
    begin                : Tue Oct 24 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <pgimage.h>
#include <pgtooltiphelp.h>
#include "unitcounting.h"
#include "vehicletypeselector.h"
#include "../vehicle.h"
#include "../containerbase.h"
#include "../gamemap.h"



class VehicleCounterFactory: public SelectionItemFactory, public SigC::Object  {
   public:
      typedef vector<const Vehicletype*> Container;
   protected:
      Container::iterator it;
      Container items;
      
      typedef map<const Vehicletype*,int> Counter;
      Counter counter;
      tmap* gamemap;

      void calcCargoSummary( const ContainerBase* cb, Counter& summary )
      {
         for ( ContainerBase::Cargo::const_iterator i = cb->cargo.begin(); i != cb->cargo.end(); ++i )
            if ( *i ) {
               calcCargoSummary( *i, summary );
               summary[ (*i)->typ] += 1;
            }
      }

      
   public:
      VehicleCounterFactory( tmap* actmap );
      VehicleCounterFactory( const ContainerBase* container );
        
      
      void restart();
      SelectionWidget* spawnNextItem( PG_Widget* parent, const PG_Point& pos );
      void itemSelected( const SelectionWidget* widget, bool mouse ) {}
};

VehicleCounterFactory :: VehicleCounterFactory( tmap* actmap ) : gamemap ( actmap )
{
   sort( items.begin(), items.end(), VehicleComp );
   restart();
};


VehicleCounterFactory::VehicleCounterFactory( const ContainerBase* container ) : gamemap( container->getMap() )
{
   calcCargoSummary( container, counter );
   for ( Counter::iterator i = counter.begin(); i != counter.end(); ++i )
      items.push_back( i->first );
      
   sort( items.begin(), items.end(), VehicleComp );
   restart();
}



void VehicleCounterFactory::restart()
{
   it = items.begin();
};


SelectionWidget* VehicleCounterFactory::spawnNextItem( PG_Widget* parent, const PG_Point& pos )
{
   if ( it != items.end() ) {
      const Vehicletype* v = *(it++);
      return new VehicleTypeCountWidget( parent, pos, parent->Width() - 15, v, gamemap->actplayer, counter[v] );
   } else
      return NULL;
};

/*
void VehicleCounterFactory::itemSelected( const SelectionWidget* widget, bool mouse )
{
   if ( !widget )
      return;

   const VehicleTypeResourceWidget* fw = dynamic_cast<const VehicleTypeResourceWidget*>(widget);
   assert( fw );
}
*/

void showUnitCargoSummary( ContainerBase* cb )
{
   ItemSelectorWindow isw( NULL, PG_Rect( 100, 150, 400, 400 ),  "cargo summary", new VehicleCounterFactory( cb ));
   isw.Show();
   isw.RunModal();
}


void showUnitCount( tmap* actmap )
{

}
