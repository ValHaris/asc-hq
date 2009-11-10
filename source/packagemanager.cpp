/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2008  Martin Bickel  and  Marc Schellenberger
 
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

#include "packagemanager.h"
#include "package.h"
#include "packagerepository.h"


static const int packageDataStreamVersion = 1;

void PackageData::read ( tnstream& stream )
{
   int v = stream.readInt();
   if ( v != packageDataStreamVersion  )
      throw tinvalidversion ( "PackageData", packageDataStreamVersion, v );
   
   int size = stream.readInt();
   for ( int i = 0; i < size; ++i ) {
      ASCString s = stream.readString();
      Package* p = new Package();
      p->read( stream );
      packages[s] = p;
   }
}


void PackageData::write ( tnstream& stream ) const
{
   stream.writeInt( packageDataStreamVersion );
   stream.writeInt( packages.size() );
   for ( Packages::const_iterator i = packages.begin(); i != packages.end(); ++i ) {
      stream.writeString( i->first );  
      i->second->write( stream );
   }
}


void PackageManager::checkGame( GameMap* game )
{
   if( game == NULL || game->packageData == NULL )
      return;
   
   for ( PackageData::Packages::const_iterator i = game->packageData->packages.begin(); i != game->packageData->packages.end(); ++i ) 
      packageRepository.checkPackageDependency( i->second );
}

void PackageManager::processContainer( const ContainerBase* container, std::set<ASCString>& archives )
{
   archives.insert( container->baseType->archive );
   for ( ContainerBase::Production::const_iterator i = container->getProduction().begin(); i != container->getProduction().end(); ++i )
      if ( *i ) {
         archives.insert( (*i)->archive );
         if ( (*i)->id ==22301 )
            printf("Hello World");
      }
}


void PackageManager::storeData( const GameMap* game )
{
   if( game->packageData == NULL )
      game->packageData = new PackageData();
   
   std::set<ASCString> archives;
   
   for ( int y = 0; y < game->ysize; ++y )
      for ( int x = 0; x < game->xsize; ++x ) {
          const tfield* fld = game->getField( x,y );
          archives.insert( fld->typ->terraintype->archive );
          
          for ( tfield::ObjectContainer::const_iterator o = fld->objects.begin(); o != fld->objects.end(); ++o ) 
             archives.insert( o->typ->archive );
      }
   for ( int p = 0; p <= 8; ++p ) {
      const Player& pl = game->getPlayer( p );
      for ( Player::VehicleList::const_iterator i = pl.vehicleList.begin(); i != pl.vehicleList.end(); ++i )
         processContainer( *i , archives );
      
      for ( Player::BuildingList::const_iterator i = pl.buildingList.begin(); i != pl.buildingList.end(); ++i )
         processContainer( *i , archives );
   }
   
   game->packageData->packages.clear();
   for ( PackageRepository::const_iterator i = packageRepository.begin(); i != packageRepository.end(); ++i ) {
      if ( archives.find( (*i)->archive ) != archives.end() )
         game->packageData->packages[ (*i)->name ] = *i;
   }
   
}

