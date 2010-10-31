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

#ifndef versionidentifierH
#define versionidentifierH

#include <vector>

#include "ascstring.h"
#include "basestreaminterface.h"


class VersionIdentifier {
   private:
      typedef std::vector<int> Versions;
      Versions versions;
        
      bool trailZero( int index ) const;
      
      public:
      VersionIdentifier();
      VersionIdentifier( const ASCString& version );
      VersionIdentifier( const VersionIdentifier& version );
      
      bool empty();
      
      void read ( tnstream& stream );
      void write ( tnstream& stream ) const;
   
      bool operator< ( const VersionIdentifier& v ) const;
      bool operator<= ( const VersionIdentifier& v ) const;
      bool operator> ( const VersionIdentifier& v ) const;
      bool operator>= ( const VersionIdentifier& v ) const;
      bool operator== ( const VersionIdentifier& v ) const;
      bool operator!= ( const VersionIdentifier& v ) const;
      
      ASCString toString() const ;
      void fromString( const ASCString& v );
      
};


#endif
