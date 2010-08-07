/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef luacommandwriterH
#define  luacommandwriterH


#include "actions/commandwriter.h"
#include "basestrm.h"

   class LuaCommandWriter : public AbstractCommandWriter {
         int commandCounter;
      protected:
         void splitString( const ASCString& string );
         virtual void outputLine( const ASCString& line ) = 0;
         void writeHeader();
      public:
         LuaCommandWriter();
               
         virtual void printCommand( const ASCString& command );
         virtual void printComment( const ASCString& comment );
   };

   class LuaCommandFileWriter : public LuaCommandWriter {
         virtual void outputLine( const ASCString& line );
      public:
         tn_file_buf_stream stream;
         LuaCommandFileWriter ( const ASCString& filename );
   };
   
   template <class C>
         class LuaCommandListWriter : public LuaCommandWriter {
         C& cont;
      protected:
         virtual void outputLine( const ASCString& line ) { cont.push_back( line ); }; 
      public:
         LuaCommandListWriter ( C& container ) : cont( container)  { LuaCommandWriter::writeHeader(); };
   };
   

#endif
