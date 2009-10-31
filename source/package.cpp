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

#include "package.h"

#include "textfile_evaluation.h"


static const int packageStreamVersion  = 1;

void Package::read ( tnstream& stream )
{
   int v = stream.readInt();
   if ( v != packageStreamVersion  )
      throw tinvalidversion ( "Package", packageStreamVersion, v );
   
   name = stream.readString();
   description = stream.readString();
   updateUrl = stream.readString();
      
   version.read( stream );
      
   readClassContainer( dependencies, stream );
      
   archive = stream.readString();
}


void Package::write ( tnstream& stream ) const
{
   stream.writeInt( packageStreamVersion );
   stream.writeString( name );
   stream.writeString( description );
   stream.writeString( updateUrl );
      
   version.write( stream );
      
   writeClassContainer( dependencies, stream );
      
   stream.writeString ( archive );
}

void Package::runTextIO ( PropertyContainer& pc )
{
   pc.addString( "Name", name );
   pc.addString( "Description", description );
   pc.addString( "UpdateURL", updateUrl, "");
   
   
   ASCString s = version.toString();
   pc.addString( "Version", s );
   version.fromString( s );
   
   int depCount = dependencies.size();
   pc.addInteger( "DependencyCount", depCount, 0 );
   
   if ( pc.isReading() )
      dependencies.clear();
   
   for ( int i = 0; i < depCount; ++i ) {
      pc.openBracket( "Dependency" + ASCString::toString(i));
      
      while ( pc.isReading() && (dependencies.size() <= i )) 
         dependencies.push_back ( PackageDependency() );
      
      pc.addString( "package", dependencies[i].name );
      ASCString s = dependencies[i].version.toString();
      pc.addString( "version", s );
      dependencies[i].version.fromString(s);
      
      pc.closeBracket();
   }
   
   archive = pc.getArchive();
}


static const int packageDependencyStreamVersion  = 1;

void Package::PackageDependency::read ( tnstream& stream )
{
   int v = stream.readInt();
   if ( v != packageDependencyStreamVersion  )
      throw tinvalidversion ( "PackageDependency", packageDependencyStreamVersion, v );
   name = stream.readString();
   version.read( stream );
}

void Package::PackageDependency::write ( tnstream& stream ) const
{
   stream.writeInt( packageDependencyStreamVersion );
   stream.writeString( name );
   version.write( stream );
}

            
