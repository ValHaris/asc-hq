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

#include "dataversioncheck.h"

#include "package.h"
#include "packagerepository.h"
            
void checkDataFileVersion()
{
   const Package* main = packageRepository.getPackage("ASCdata");
   if ( !main )
      throw ASCmsgException("No package description found for ASCdata");
   
   VersionIdentifier mainRequired("2.4.91.0");
   if( main->version < mainRequired )
      throw ASCmsgException("Package ASCmain is outdated.\nYou need at least version " + mainRequired.toString() + "\n" + main->location + "\n\n" + main->description) ;
      
   const Package* pbp = packageRepository.getPackage("pbp");
   ASCString pbpversion = "91";
   if ( pbp && pbp->version < VersionIdentifier(pbpversion) )
      throw ASCmsgException("Package PBP is outdated. You need at least version " +  pbpversion + "\n" + main->location +  "\n\n" + pbp->description) ;
   
}
            
            
void checkDataVersion()
{
   PackageRepository::packgeDescriptionLoaded.connect( SigC::slot( &checkDataFileVersion ));
}
            

