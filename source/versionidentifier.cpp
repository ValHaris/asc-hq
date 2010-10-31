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

#include "versionidentifier.h"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "errors.h"
#include "basestrm.h"

VersionIdentifier::VersionIdentifier()
{
}

VersionIdentifier :: VersionIdentifier( const ASCString& version )
{
   fromString( version );
}

bool VersionIdentifier :: empty()
{
   return versions.empty();
}


ASCString VersionIdentifier :: toString() const 
{
   ASCString s;
   for ( int i = 0; i < versions.size(); ++i ) {
      if ( i >= 1 )
         s += ".";
      s += ASCString::toString( versions[i] );
   }
   return s;
}

void VersionIdentifier :: fromString( const ASCString& v )
{
   versions.clear();
   
   boost::regex versionformat( "\\s*(\\d+)(\\.\\d+)*\\s*");
   boost::smatch what;

   if( !boost::regex_match( v, what, versionformat)) 
      throw ASCmsgException( v + " is not a valid version identifier");
   
   typedef std::vector< ASCString > Split_vector_type;
   Split_vector_type splitVector;
   split( splitVector, v, boost::is_any_of(".") );
   
   for ( vector<ASCString>::const_iterator i = splitVector.begin(); i != splitVector.end(); ++i ) {
      int num = atoi( i->c_str() );
      versions.push_back( num );
   }
}


VersionIdentifier :: VersionIdentifier( const VersionIdentifier& version )
{
   versions = version.versions;
}
      
static const int versionIdentifierStreamVersion = 1;      
      
void VersionIdentifier :: read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version != versionIdentifierStreamVersion  )
      throw tinvalidversion ( "VersionIdentifier", versionIdentifierStreamVersion, version );

   readClassContainer( versions, stream );
}


void VersionIdentifier :: write ( tnstream& stream ) const
{
   stream.writeInt( versionIdentifierStreamVersion  );
   writeClassContainer( versions, stream );
}

bool VersionIdentifier:: trailZero( int index ) const 
{
   for ( int i = index; i < versions.size(); ++i )
      if ( versions[i] != 0 )
         return false;
   return true;
}


bool VersionIdentifier::operator<= ( const VersionIdentifier& v ) const 
{ 
   int m = min ( v.versions.size(), versions.size() );
   for ( int i = 0; i < m; ++i ) {
      if ( versions[i] > v.versions[i] )
         return false;
   }
   if ( versions.size() < v.versions.size() ) {
      return true;
   } else {
      if ( trailZero( v.versions.size() ))
         return true;
      else
         return false;
   }
};


bool VersionIdentifier::operator< ( const VersionIdentifier& v ) const 
{ 
   int m = min ( v.versions.size(), versions.size() );
   for ( int i = 0; i < m; ++i ) {
       if ( versions[i] < v.versions[i] )
          return true;
       if ( versions[i] > v.versions[i] )
          return false;
   }
   if ( versions.size() < v.versions.size() ) {
      if ( v.trailZero( versions.size() ))
         return false;
      else
         return true;
   } else {
       return false;
   }
};

bool VersionIdentifier::operator> ( const VersionIdentifier& v ) const 
{
   return !operator<=(v);
}
   
bool VersionIdentifier::operator>= ( const VersionIdentifier& v ) const 
{
   return !operator<(v);
}
   


bool VersionIdentifier::operator== ( const VersionIdentifier& v ) const
{ 
   int m = min ( v.versions.size(), versions.size() );
   for ( int i = 0; i < m; ++i ) {
      if ( versions[i] != v.versions[i] )
         return false;
   }
   if ( versions.size() < v.versions.size() ) {
      if ( v.trailZero( versions.size() ))
         return true;
      else
         return false;
   } else {
      if ( trailZero( v.versions.size() ))
         return true;
      else
         return false;
   }
}

bool VersionIdentifier::operator!= ( const VersionIdentifier& v ) const 
{
   return !operator==(v);
};

