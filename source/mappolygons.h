/*! \file mappolygons.h
    \brief operating with polygons on a map
*/

//     $Id: mappolygons.h,v 1.1 2004-01-16 19:14:55 mbickel Exp $

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

#ifndef mappolygonsH
 #define mappolygonsH


#include "polygontriangulation.h"
#include "typen.h"
#include "terraintype.h"

class Poly_gon {
   public:
      typedef MapCoordinate Point;
      typedef vector<Point>::iterator VertexIterator;
      vector<Point> vertex;
      void read ( tnstream& stream );
      void write ( tnstream& stream ) const;
};



class  PolygonPainerSquareCoordinate : protected PolygonPainter<Poly_gon> {
        protected:
             virtual void setpoint    ( int x,  int y  );
             virtual void setpointabs ( int x,  int y  )=0;
        public:
             void setspecificpoint ( int x, int y ) { setpointabs ( x, y ); };
             bool paintPolygon   (  const Poly_gon& poly );
          };


#endif //weather_h
