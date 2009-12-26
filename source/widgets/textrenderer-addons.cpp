/***************************************************************************
                          paradialog.cpp  -  description
                             -------------------
    begin                : Thu Feb 21 2002
    copyright            : (C) 2002 by Martin Bickel
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



#include <boost/regex.hpp>
#include <pglabel.h>
#include <pgimage.h>

#include "../global.h"

#include "textrenderer.h"
#include "../graphics/surface.h"
#include "../iconrepository.h"
#include "targetcoordinatelocator.h"

#include "../vehicletype.h"
#include "../itemrepository.h"


class VehicleTypeRenderer: public TextRenderer::TagRenderer
{
      boost::regex vehicletypetag;
   public:
      VehicleTypeRenderer() : vehicletypetag("#vehicletype=(\\d+)#")  {};

      bool renderWidget( const ASCString& tag, TextRenderer::Widgets& widgets, TextRenderer* parent  )
      {
         boost::smatch what;
         if( boost::regex_match( tag, what, vehicletypetag)) {
            ASCString s ( what[1] );
            int id = atoi ( s.c_str() );
            VehicleType* vt = vehicleTypeRepository.getObject_byID( id );
            if ( vt ) {
               Surface& surf = vt->getImage();
               widgets.push_back( new PG_Image( parent, PG_Point(0,0), surf.getBaseSurface(), false ));
            } else {
               widgets.push_back ( parent->parsingError ( "VehicleType " + ASCString::toString(id) + " not found" ) );
            }
            return true;
         }
         return false;
      }
};


class TargetCoordinateRenderer: public TextRenderer::TagRenderer
{
      boost::regex coordinates;
   public:
      TargetCoordinateRenderer() : coordinates( "#coord\\((.*)\\)#")  {};

      bool renderWidget( const ASCString& tag, TextRenderer::Widgets& widgets, TextRenderer* parent  )
      {
         boost::smatch what;
         if( boost::regex_match( tag, what, coordinates)) {
            ASCString s ( what[1] );

            static boost::regex coordinates2( ";?(\\d+)/(\\d+)(.*)");
            while ( boost::regex_match( s, what, coordinates2)) {
               ASCString s2 ( what[1] );
               int x = strtol(s2.c_str(), NULL, 0 );
               int y = strtol( ASCString(what[2]).c_str(), NULL, 0 );

               SelectFromMap::CoordinateList positions;
               positions.push_back( MapCoordinate(x,y));
               
               s.assign( what[3].first, what[3].second );

               widgets.push_back( new TargetCoordinateLocator( parent, PG_Point(0,0), positions ) );
            }
            return true;
         }
         return false;
      }
};


namespace {
   const bool r1 = TextRenderer::registerTagRenderer( new VehicleTypeRenderer );
   const bool r2 = TextRenderer::registerTagRenderer( new TargetCoordinateRenderer );
}

void uselessCallToTextRenderAddons()
{
   
}
