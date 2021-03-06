/***************************************************************************
                          textfileparser.h  -  description
                             -------------------
    begin                : Thu Jul 26 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file textfileparser.h
    \brief Functions to parse the *.asctxt files
*/


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef textfileparserH
#define textfileparserH

#include <vector>
#include <map>
#include "basictypes.h"
#include "ascstring.h"
#include "errors.h"
#include "basestreaminterface.h"


class ParsingError : public ASCmsgException {
    public:
       ParsingError ( const ASCString& msg ) : ASCmsgException ( msg ) {};
};

class TextPropertyGroup;

class TextPropertyList : public PointerList<TextPropertyGroup*> {
                  typedef map<int,TextPropertyGroup*> IdentCache;
                  IdentCache identCache;
                public:
                   void buildIDs();
                   TextPropertyGroup* get ( int id );
};

/** Class that stores all the (preparsed) entries of an .ASCTXT file.
    The entries consist of a PropertyName, an operator and a value, but don't have any type information
*/
class TextPropertyGroup {
         bool inheritanceBuild;
         bool abstract;
         int id;
      public:
          TextPropertyGroup() : inheritanceBuild ( false ), abstract ( false ), id ( -1 ) {};

          class Entry {
            public:
               ASCString propertyName;
               enum Operator { eq, mult_eq, add_eq, alias, alias_all, sub_eq, alias_all_resolved } op;
               ASCString value;
               Entry* parent;
               Entry ( const ASCString& propertyName_, Operator op_, const ASCString& value_ ) : propertyName ( propertyName_ ), op ( op_ ), value ( value_ ), parent ( NULL ) { propertyName.toLower(); };
               ASCString toString() const;
         };

      private:
         typedef map<ASCString, Entry*> EntryCache;
         EntryCache entryCache;
         typedef list<Entry> Entries;
         Entries entries;

         typedef list<Entry*> EntryPointerList;

      protected:
         void error ( const ASCString& msg, bool printInheritance = true );
         bool processAlias( Entry& e, Entries& entriesToAdd, EntryPointerList& markAsResolved );
         int findGeneration ( Entry* e );
         ASCString listInheritanceFilenames();
      public:
         void addEntry( const Entry& entry );
         Entry* find( const ASCString& n );
         typedef vector<Entry*> Matches;
         void findMatches( const ASCString& name, const ASCString& name_without_dot, Matches& matches );

         typedef list<TextPropertyGroup*> Parents;
         Parents parents;

         ASCString fileName;
         ASCString location;
         ASCString archive;

         //! the name of the structure. For example "VehicleType"
         ASCString typeName;

         int evalID();

         void buildInheritance( TextPropertyList& tpl );
         void resolveAllAlias( );
         bool isAbstract() { return abstract; };
         void print( int indent = 0 );
};


//! Parses a .ASCTXT file and returns a TextPropertyGroup
class TextFormatParser {
         tnstream *stream;
         typedef list<ASCString> Level;
         Level level;
         ASCString s1, s2, s3;
         int levelDepth;
         ASCString primaryName;

         TextPropertyGroup* textPropertyGroup;

     public:
        TextFormatParser( tnstream* stream_, const ASCString& primaryName_ = "" ) : stream ( stream_ ), levelDepth ( 0 ), primaryName ( primaryName_ ), textPropertyGroup ( NULL ) {};
        TextPropertyGroup* run (  );
        ASCString readLine ( );

        static const int operationsNum;
        static const char* operations[];
        static const char* whiteSpace;

     protected:
        void startLevel ( const ASCString& levelName );
        void parseLine ( const ASCString& line );
        void error ( const ASCString& errmsg );
};


#endif
