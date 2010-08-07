/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "luacommandwriter.h"

#include <boost/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

void LuaCommandWriter :: splitString( const ASCString& string ) 
{
   typedef vector< ASCString > Split_vector_type;

   Split_vector_type splitVec; // #2: Search for tokens
   boost::algorithm::split( splitVec, string, boost::algorithm::is_any_of("\n") ); // SplitVec == { "hello abc","ABC","aBc goodbye" }
      
   for ( Split_vector_type::iterator i = splitVec.begin(); i != splitVec.end(); ++i ) {
      printCommand( *i );  
   }
}
      
void LuaCommandWriter :: writeHeader()  {
   outputLine("-- get handle to active map \n");
   outputLine("map = asc.getActiveMap() \n");
}
      
LuaCommandWriter :: LuaCommandWriter ( ) : commandCounter(0) {
}

void LuaCommandWriter :: printCommand( const ASCString& command ) {
   if ( command.find('\n') != ASCString::npos ) {
      splitString(command);
   } else {
      outputLine("r = asc." + command + "\n" );
      outputLine("if r:successful()==false then asc.displayActionError(r, \"command " + ASCString::toString(++commandCounter) + "\") end \n" );
   }
};

void LuaCommandWriter :: printComment( const ASCString& comment ) {
   outputLine("\n--" + comment + "\n" );
};



LuaCommandFileWriter :: LuaCommandFileWriter ( const ASCString& filename ) : stream ( filename, tnstream::writing ) 
{
   writeHeader();
}

void LuaCommandFileWriter :: outputLine( const ASCString& line )
{
   stream.writeString( line, false );
}

