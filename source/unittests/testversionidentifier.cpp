/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "versionidentifier.h"

#include "unittestutil.h"


void testVersionIdentifier() 
{
   assertOrThrow( VersionIdentifier("1.2.0") ==  VersionIdentifier("1.2.0") );
   assertOrThrow( VersionIdentifier("1.2.0") ==  VersionIdentifier("1.2") );
   assertOrThrow( VersionIdentifier("1.2.0") !=  VersionIdentifier("1.2.1") );
   assertOrThrow( VersionIdentifier("1.2.0") !=  VersionIdentifier("1.2.0.1") );
   assertOrThrow( VersionIdentifier("1.2.0") !=  VersionIdentifier("11.2.0") );

   assertOrThrow( VersionIdentifier("1.2.0") <=  VersionIdentifier("1.2.0") );
   assertOrThrow( VersionIdentifier("1.2.0") <=  VersionIdentifier("1.2") );
   assertOrThrow( VersionIdentifier("1.2.0") >=  VersionIdentifier("1.2") );
   assertOrThrow( VersionIdentifier("1.2.0") >=  VersionIdentifier("1.2.0") );
   assertOrThrow( VersionIdentifier("1.3") >=  VersionIdentifier("1.2.0.1") );
   assertOrThrow( VersionIdentifier("1.3.0.0.2.0.1.0") >=  VersionIdentifier("1.2.0.1") );
   
   
   
   assertOrThrow( VersionIdentifier("1.2.0") <  VersionIdentifier("1.3") );
   assertOrThrow( VersionIdentifier("1.2.0") <  VersionIdentifier("1.2.1") );
   assertOrThrow( VersionIdentifier("1.2.0") <  VersionIdentifier("1.2.0.1") );
   
   assertOrThrow( !(VersionIdentifier("1.3") <  VersionIdentifier("1.2.0") ));
   assertOrThrow( !(VersionIdentifier("1.2.1") <  VersionIdentifier("1.2.0") ));
   assertOrThrow( !(VersionIdentifier("1.2.0.1") <  VersionIdentifier("1.2.0") ));
   
  
}
