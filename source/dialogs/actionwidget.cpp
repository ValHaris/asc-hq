/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.org
     Copyright (C) 1994-2009  Martin Bickel

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

#include "actionwidget.h"
#include "../widgets/vehicletypeimage.h"

#include "../gamemap.h"
#include "../actions/unitcommand.h"
#include "../itemrepository.h"


ActionWidget::ActionWidget( PG_Widget* parent, const PG_Point& pos, int width, const Command& action, GameMap* map )
        : SelectionWidget( parent, PG_Rect( pos.x, pos.y, width, fieldsizey )), act( action )
{

    int offset = 0;

    const UnitCommand* uc = dynamic_cast<const UnitCommand*>(&action);
    if ( uc != NULL && uc->getUnitTypeID() > 0 ) {
        VehicleType* vt = vehicleTypeRepository.getObject_byID( uc->getUnitTypeID() );
        if ( vt ) {
            PG_Widget* w = new VehicleTypeImage( this, PG_Point( 25, 0), vt, map->getCurrentPlayer() );
            offset += w->Width() + 5;
        }
    }
    PG_Label* lbl1 = new PG_Label( this, PG_Rect( 25 + offset, 0, Width()-30, Height() ), action.getDescription() );
    lbl1->SetFontSize( lbl1->GetFontSize() -2 );

    SetTransparency( 255 );
};

ASCString ActionWidget::getName() const
{
    return act.getDescription();
};

vector<MapCoordinate> ActionWidget::getCoordinates() const
{
    return act.getCoordinates();
}


void ActionWidget::display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst )
{
}
;


