
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <map> 
 
#include "global.h"
#include "cannedmessages.h"
#include "basestrm.h"

map<int,ASCString> messageStrings;


void   loadsinglemessagefile ( const char* name )
{
   try {
      tnfilestream stream ( name, tnstream::reading );

      ASCString s1, s2;
      s1 = stream.readString();
      s2 = stream.readString();

      while ( !s1.empty() && !s2.empty() ) {
         int w = atoi ( s1.c_str() );

         messageStrings[w] = s2;

         s1 = stream.readString();
         s2 = stream.readString();
      }
   }
   catch ( treadafterend ) {
   }
}


void         loadmessages(void)
{
   tfindfile ff ( "message?.txt" );
   ASCString filename = ff.getnextname();

   while( !filename.empty() ) {
      loadsinglemessagefile ( filename.c_str() );
      filename = ff.getnextname();
   }
}


const char*        getmessage( int id)
{
   if ( messageStrings.find ( id ) != messageStrings.end() )
         return messageStrings[id].c_str();

   static const char* notfound = "message not found";
   return notfound;
}

