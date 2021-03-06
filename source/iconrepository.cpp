/*! \file iconrepository.cpp
    \brief A central class to store all icons images.
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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
#include <cstring>
#include <stdlib.h>
#include <SDL_image.h>
#include <set>

#include "iconrepository.h"
#include "basestrm.h"
#include "util/messaginghub.h"


IconRepository::Repository IconRepository::repository;

set<ASCString> errorsShown;

Surface& IconRepository::getIcon( const ASCString& name )
{
  Repository::iterator i = repository.find( name );
  if ( i != repository.end() ) 
     return *i->second;
  else {
     try {
        tnfilestream fs ( name, tnstream::reading );
        if ( name.endswith(".raw") ) {
           repository[name] = new Surface();
           repository[name]->read( fs );
        } else {
           repository[name] = new Surface ( IMG_Load_RW ( SDL_RWFromStream( &fs ), 1));
        }
        return *repository[name];
     }
     catch ( tfileerror err ) {
        if ( errorsShown.find( name ) == errorsShown.end() ) {
            errorMessage("could not load " + err.getFileName() );
            errorsShown.insert( name );
        }
        if ( name != "dummy.png" )
           return getIcon( "dummy.png" );
        else
           throw;
     }
  }
}

IconRepository::Repository::~Repository()
{
   for (   Repository::iterator i = repository.begin(); i != repository.end(); ++i )
      delete i->second;
}

bool IconRepository::exists( const ASCString& name )
{
   return repository.find(name) != repository.end();
}

void IconRepository::insert( const ASCString& name, Surface* s )
{
   repository[name] = s;
}

int IconRepository::getMemoryFootprint()
{
   int size = 0;
   for ( Repository::iterator i = repository.begin(); i != repository.end(); ++i  )
      size += i->second->getMemoryFootprint();
   return size;
}

