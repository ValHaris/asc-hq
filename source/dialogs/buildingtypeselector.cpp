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

#include "buildingtypeselector.h"
#include "selectionwindow.h"
#include "unitinfodialog.h"

#include "../buildingtype.h"
#include "../iconrepository.h"
#include "../spfst.h"


const int buildingHeight = 5 * fielddisty + fieldsizey;
const int buildingWidth = 4 * fielddistx;

int BuildingTypeBaseWidget :: getBuildingHeight( const BuildingType* type )
{
   int miny = maxint;
   int maxy = minint;
   for ( int y = 0; y < 6; ++y )
      for ( int x = 0; x < 4; ++x )
         if ( type->fieldExists( BuildingType::LocalCoordinate( x, y ))) {
            miny = min( miny, y );
            maxy = max( maxy, y );
         }

   return (maxy - miny) * fielddisty + fieldsizey;
}


BuildingTypeBaseWidget :: BuildingTypeBaseWidget( PG_Widget* parent, const PG_Point& pos, int width, const BuildingType* buildingType, int player ) : SelectionWidget( parent, PG_Rect( pos.x, pos.y, width, getBuildingHeight(buildingType)+10 )), vt( buildingType ), actplayer(player)
{

   new PG_Label( this, PG_Rect( buildingWidth, 20, Width() - buildingWidth - 10, 25 ), vt->name );

   SetTransparency( 255 );
};



ASCString BuildingTypeBaseWidget::getName() const
{
   return vt->getName();
};

void BuildingTypeBaseWidget::display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst )
{
   if ( !getClippingSurface().valid() )
      getClippingSurface() = Surface::createSurface( buildingWidth + 10, buildingHeight + 10, 32, 0 );

   getClippingSurface().Fill(0);

   int yoffs = maxint;
   int xoffs = maxint;
   for ( int y = 0; y < 6; ++y )
      for ( int x = 0; x < 4; ++x )
         if ( vt->fieldExists( BuildingType::LocalCoordinate( x, y ))) {
            yoffs = min( yoffs, y * fielddisty );
            xoffs = min( xoffs, x * fielddistx + (y&1)*fielddisthalfx);
         }

   for ( int y = 0; y < 6; ++y )
      for ( int x = 0; x < 4; ++x )
         if ( vt->fieldExists( BuildingType::LocalCoordinate( x, y )))
            vt->paintSingleField( getClippingSurface(), SPoint(5 - xoffs, 5 - yoffs), BuildingType::LocalCoordinate(x,y), actplayer );

   // vt->paint( getClippingSurface(), SPoint(5,5), actplayer, 0 );
   PG_Draw::BlitSurface( getClippingSurface().getBaseSurface(), src, surface, dst);
}

Surface BuildingTypeBaseWidget::clippingSurface;



BuildingTypeResourceWidget::BuildingTypeResourceWidget( PG_Widget* parent, const PG_Point& pos, int width, const BuildingType* BuildingType, int lackingResources, const Resources& cost, int player )
   : BuildingTypeBaseWidget( parent,pos, width, BuildingType, player )
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
      

BuildingTypeCountWidget::BuildingTypeCountWidget( PG_Widget* parent, const PG_Point& pos, int width, const BuildingType* BuildingType, int player, int number )
   : BuildingTypeBaseWidget( parent,pos, width, BuildingType, player )
{
   int col1 = 50;
   int lineheight  = 20;

   int sw = (width - col1 - 10) / 6;

   PG_Label* lbl = new PG_Label( this, PG_Rect( col1 + 4 * sw, 0, sw * 2, lineheight*2 ), ASCString::toString(number) );
   lbl->SetAlignment( PG_Label::RIGHT );
   lbl->SetFontSize( lbl->GetFontSize() + 5 );
}






BuildingTypeSelectionItemFactory :: BuildingTypeSelectionItemFactory( Resources plantResources, const Container& types, int player ) : actplayer(player), original_items( types )
{
   restart();
   setAvailableResource( plantResources );
};


bool BuildingComp ( const BuildingType* v1, const BuildingType* v2 )
{
   int id1 = getUnitSetID(v1);
   int id2 = getUnitSetID(v2);
   return (id1 <  id2) ||
          (id1 == id2 && v1->name < v2->name);
};


void BuildingTypeSelectionItemFactory::restart()
{
   items = original_items;
   sort( items.begin(), items.end(), BuildingComp );
   it = items.begin();
};


SelectionWidget* BuildingTypeSelectionItemFactory::spawnNextItem( PG_Widget* parent, const PG_Point& pos )
{
   if ( it != items.end() ) {
      const BuildingType* v = *(it++);
      Resources cost  = getCost(v);

      int lackingResources = 0;
      for ( int r = 0; r < 3; ++r )
         if ( plantResources.resource(r) < cost.resource(r))
            lackingResources |= 1 << r;
      return new BuildingTypeResourceWidget( parent, pos, parent->Width() - 15, v, lackingResources, cost, actplayer );
   } else
      return NULL;
};

Resources BuildingTypeSelectionItemFactory::getCost( const BuildingType* type )
{
   return type->productionCost;
}



void BuildingTypeSelectionItemFactory::itemSelected( const SelectionWidget* widget, bool mouse )
{
   if ( !widget )
      return;

   const BuildingTypeResourceWidget* fw = dynamic_cast<const BuildingTypeResourceWidget*>(widget);
   assert( fw );

   // showBuildingInfo( fw->getBuildingType() );
   
   BuildingTypeSelected( fw->getBuildingType() );
}

