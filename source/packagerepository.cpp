/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.org
     Copyright (C) 1994-2010  Martin Bickel
 
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

#include "packagerepository.h"
#include "strtmesg.h"
#include "packagemanager.h"            
            

PackageRepository packageRepository;

void PackageRepository ::addProgramPackage()
{
   Package* prog = _getPackage( "ASC");
   if ( !prog ) {
      prog = new Package();
      packageRepository.push_back( prog );
      prog->name = "ASC";
   }
   prog->version.fromString( getVersionString() );
}

void PackageRepository ::readTextFiles( PropertyReadingContainer& prc, const ASCString& fileName, const ASCString& location )
{
   Package* p = new Package();
   p->runTextIO( prc );
   
   // p->filename = fileName;
   // p->location = location;
   
   packageRepository.push_back( p );
}

void PackageRepository :: read ( tnstream& stream )
{
   stream.readInt();
   readPointerContainer( packageRepository, stream );
}

void PackageRepository :: write ( tnstream& stream )
{
   stream.writeInt( 1 );
   writePointerContainer( packageRepository, stream );
}

SigC::Signal0<void> PackageRepository::packgeDescriptionLoaded;

void PackageRepository :: postChecks() 
{
   addProgramPackage();
   for ( PackageRepository::iterator i = packageRepository.begin(); i != packageRepository.end(); ++i ) {
      checkPackageDependency( *i );
   }
   packgeDescriptionLoaded();
}

void PackageRepository::checkPackageDependency( const Package* pack, const PackageData* packageData )
{
   for ( Package::Dependencies::const_iterator dep = pack->dependencies.begin(); dep != pack->dependencies.end(); ++dep ) {
      const Package* p = getPackage( dep->name );
      if ( p ) {
         if ( p->version < dep->version ) {
            ASCString s = "The package " +  dep->name + " on your system is outdated.\n"
                  + "Package " + pack->name + " requires at least " + dep->version.toString() + "\n";
            
            if ( !p->description.empty() )
                s += "\nPackage Info:\n" + p->description ;
            throw ASCmsgException( s);
         } 
      } else {
         ASCString s = "The package " +  dep->name + " is missing on your system!" ;
         if ( packageData ) {
            for ( PackageData::Packages::const_iterator ref = packageData->packages.begin(); ref != packageData->packages.end(); ++ref )
               if ( ref->second->name == dep->name )
                  if ( !ref->second->description.empty())
                     s += "\nPackage Info:\n" + ref->second->description ;
                        
         }
         throw ASCmsgException( s );
      }

   }
}


Package* PackageRepository::_getPackage( const ASCString& name ) 
{
   for ( PackageRepository::const_iterator p = packageRepository.begin(); p != packageRepository.end(); ++p ) 
      if ( (*p)->name.compare_ci( name ) == 0 ) 
         return *p;
   return NULL;
}

const Package* PackageRepository::getPackage( const ASCString& name ) const
{
   for ( PackageRepository::const_iterator p = packageRepository.begin(); p != packageRepository.end(); ++p ) 
      if ( (*p)->name.compare_ci( name ) == 0 ) 
         return *p;
   return NULL;
}


