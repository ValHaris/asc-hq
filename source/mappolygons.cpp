/*! \file weather.cpp
    \brief Operating with polygons on the map
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2005  Martin Bickel  and  Marc Schellenberger

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

#include <stdio.h>
#include "typen.h"
#include "misc.h"
#include "basegfx.h"
#include "newfont.h"
#include "loaders.h"
#include "spfst.h"
#include "mappolygons.h"
#include "dlg_box.h"
#include "itemrepository.h"



void  PolygonPainerSquareCoordinate::setpoint    ( int x,  int y  )
{
    if ( (x & 1) == (y & 1) )
       setpointabs ( x / 2, y);
}
          
bool    PolygonPainerSquareCoordinate::paintPolygon   ( const Poly_gon& poly )
{
   Poly_gon poly1;
   for ( int i=0; i< poly.vertex.size() ; i++ )
      poly1.vertex.push_back ( MapCoordinate( poly.vertex[i].x * 2 + (poly.vertex[i].y & 1), poly.vertex[i].y ));

   return PolygonPainter<Poly_gon>::paintPolygon ( poly1 );
}



void Poly_gon::read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version != 100 )
      throw tinvalidversion( stream.getLocation(), 100, version );
   readClassContainer ( vertex, stream );
}


void Poly_gon::write ( tnstream& stream ) const
{
   stream.writeInt( 100 );
   writeClassContainer ( vertex, stream );
}




