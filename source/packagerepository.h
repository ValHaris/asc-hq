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

#ifndef packagerepositoryH
#define packagerepositoryH

#include "typen.h"
#include "itemrepository.h"

class PackageRepository : public TextFileDataLoader, public deallocating_vector<Package*>  {
      void addProgramPackage();
      Package* _getPackage( const ASCString& name );
   public:
      void readTextFiles( PropertyReadingContainer& prc, const ASCString& fileName, const ASCString& location );
      void read ( tnstream& stream );
      void write ( tnstream& stream );
      virtual void postChecks();
      ASCString getTypeName() { return "package"; };
      
      static SigC::Signal0<void> packgeDescriptionLoaded;
      
      void checkPackageDependency( const Package* pack );
      
      const Package* getPackage( const ASCString& name ) const;
      
};

extern PackageRepository packageRepository;



#endif
