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

#include "vehicletypeselector.h"
#include "selectionwindow.h"
#include "unitinfodialog.h"

#include "../vehicletype.h"
#include "../iconrepository.h"
#include "../spfst.h"
#include "../unitset.h"

VehicleTypeBaseWidget :: VehicleTypeBaseWidget( PG_Widget* parent, const PG_Point& pos, int width, const VehicleType* vehicletype, const Player& player ) : SelectionWidget( parent, PG_Rect( pos.x, pos.y, width, fieldsizey+10 )), vt( vehicletype ), actplayer(player)
{
   int col1 = 50;
   int lineheight  = 20;

   int sw = (width - col1 - 10) / 6;

   PG_Label* lbl1 = new PG_Label( this, PG_Rect( col1, 0, 3 * sw, lineheight ), vt->name );
   lbl1->SetFontSize( lbl1->GetFontSize() -2 );

   PG_Label* lbl2 = new PG_Label( this, PG_Rect( col1, lineheight, 3 * sw, lineheight ), vt->description );
   lbl2->SetFontSize( lbl2->GetFontSize() -2 );


   PG_Button* b = new PG_Button( this, PG_Rect( buttonXPos(width, 0 ), Height()/2-lineheight, 2*lineheight, 2*lineheight ));
   b->SetIcon( IconRepository::getIcon( "blue-i.png").getBaseSurface() );
   b->sigClick.connect( SigC::slot( *this, &VehicleTypeBaseWidget::info ));
   
   SetTransparency( 255 );
};

int VehicleTypeBaseWidget::buttonXPos( int width, int num )
{
   int col1 = 50;
   int sw = (width - col1 - 10) / 6;
   return col1 + 3 * sw + (10 + sw) * num;
}



bool VehicleTypeBaseWidget::info()
{
   unitInfoDialog( vt );
   return true;
}


ASCString VehicleTypeBaseWidget::getName() const
{
   return vt->getName();
};

void VehicleTypeBaseWidget::display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst )
{
   if ( !getClippingSurface().valid() )
      getClippingSurface() = Surface::createSurface( fieldsizex + 10, fieldsizey + 10, 32, 0 );

   getClippingSurface().Fill(0);

   vt->paint( getClippingSurface(), SPoint(5,5), actplayer.getPlayerColor(), 0 );
   PG_Draw::BlitSurface( getClippingSurface().getBaseSurface(), src, surface, dst);
}

Surface VehicleTypeBaseWidget::clippingSurface;



VehicleTypeResourceWidget::VehicleTypeResourceWidget( PG_Widget* parent, const PG_Point& pos, int width, const VehicleType* vehicletype, int lackingResources, const Resources& cost, const Player& player )
   : VehicleTypeBaseWidget( parent,pos, width, vehicletype, player )
{
   int col1 = 50;
   int lineheight  = 20;

   int sw = (width - col1 - 10) / 6;

   static const char* filenames[3] = { "energy.png", "material.png", "fuel.png" };

   for ( int i = 0; i < 3; ++i ) {
      new PG_Image ( this, PG_Point( col1 + 2 + 5 * sw, i * 12 + 5), IconRepository::getIcon( filenames[i] ).getBaseSurface(), false  );
      PG_Label* lbl = new PG_Label( this, PG_Rect( col1 + 3 * sw, i * 12, sw * 2, lineheight ), ASCString::toString(cost.resource(i)) );
      lbl->SetAlignment( PG_Label::RIGHT );
      lbl->SetFontSize( lbl->GetFontSize() - 3 );
      if ( lackingResources & (1<<i) )
         lbl->SetFontColor( 0xff0000);
   }
}
      

VehicleTypeCountWidget::VehicleTypeCountWidget( PG_Widget* parent, const PG_Point& pos, int width, const VehicleType* vehicletype, const Player& player, int number )
   : VehicleTypeBaseWidget( parent,pos, width, vehicletype, player )
{
   int col1 = 50;
   int lineheight  = 20;

   int sw = (width - col1 - 10) / 6;

   PG_Label* lbl = new PG_Label( this, PG_Rect( col1 + 4 * sw, 0, sw * 2, lineheight*2 ), ASCString::toString(number) );
   lbl->SetAlignment( PG_Label::RIGHT );
   lbl->SetFontSize( lbl->GetFontSize() + 5 );
}

VehicleTypeCountLocateWidget::VehicleTypeCountLocateWidget( PG_Widget* parent, const PG_Point& pos, int width, const VehicleType* vehicletype, const Player& player, int number )
   : VehicleTypeCountWidget( parent, pos, width, vehicletype, player, number )
{
   int lineheight  = 20;
   
   PG_Button* b = new PG_Button( this, PG_Rect( buttonXPos(width, 1 ), Height()/2-lineheight, 2*lineheight, 2*lineheight ));
   b->SetIcon( IconRepository::getIcon( "magnifier.png").getBaseSurface() );
   b->sigClick.connect( SigC::slot( *this, &VehicleTypeCountLocateWidget::locate ));
         
}

bool VehicleTypeCountLocateWidget::locate()
{
   locateVehicles( getVehicletype() );
   return true;
}


VehicleTypeSelectionItemFactory :: VehicleTypeSelectionItemFactory( Resources plantResources, const Container& types, const Player& player ) 
   : actplayer(player), showResourcesForUnit(true), original_items( types )
{
   restart();
   setAvailableResource( plantResources );
};

VehicleTypeSelectionItemFactory :: VehicleTypeSelectionItemFactory( const Container& types, const Player& player ) 
   : actplayer(player), showResourcesForUnit(false), original_items( types )
{
   restart();
};



void VehicleTypeSelectionItemFactory::restart()
{
   items = original_items;
   sort( items.begin(), items.end(), vehicleComp );
   it = items.begin();
};


SelectionWidget* VehicleTypeSelectionItemFactory::spawnNextItem( PG_Widget* parent, const PG_Point& pos )
{
   if ( it != items.end() ) {
      const VehicleType* v = *(it++);
      if ( showResourcesForUnit ) {
         Resources cost  = getCost(v);

         int lackingResources = 0;
         for ( int r = 0; r < 3; ++r )
            if ( plantResources.resource(r) < cost.resource(r))
               lackingResources |= 1 << r;
         return new VehicleTypeResourceWidget( parent, pos, parent->Width() - 15, v, lackingResources, cost, actplayer );
      } else
         return new VehicleTypeBaseWidget( parent, pos, parent->Width() - 15, v, actplayer );
   } else
      return NULL;
};

SigC::Signal1<void,const VehicleType*> VehicleTypeSelectionItemFactory::showVehicleInfo;


void VehicleTypeSelectionItemFactory::itemSelected( const SelectionWidget* widget, bool mouse )
{
   if ( !widget )
      return;

   const VehicleTypeBaseWidget* fw = dynamic_cast<const VehicleTypeBaseWidget*>(widget);
   assert( fw );

   showVehicleInfo( fw->getVehicletype() );
   
   vehicleTypeSelected( fw->getVehicletype(), mouse );
}

