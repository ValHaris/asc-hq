/***************************************************************************
                          textfileparser.cpp  -  description
                             -------------------
    begin                : Thu Jul 26 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <vector>
#include <SDL_image.h>
#include <algorithm>
#include "ascstring.h"
#include "basegfx.h"
#include "typen.h"
#include "terraintype.h"
#include "objecttype.h"
#include "textfileparser.h"
#include "dlg_box.h"
#include "stringtokenizer.h"
#include "loadpcx.h"
#include "textfile_evaluation.h"


const int TextFormatParser::operationsNum = 5;
const char* TextFormatParser::operations[5] =  { "=", "*=", "+=", "->", "->*" };
const char* TextFormatParser::whiteSpace = " \t";





void TextPropertyList::buildIDs()
{
   for ( iterator i = begin(); i != end(); i++ ) {
      int id = (*i)->evalID();
      if ( id > 0 )
         identCache[id] = *i;
   }
}

TextPropertyGroup* TextPropertyList::get ( int id )
{
   IdentCache::iterator f = identCache.find ( id );
   if ( f != identCache.end() )
      return &(*(f->second));
   else
      return NULL;
}


///////////////////// TextPropertyGroup //////////////////////////

void TextPropertyGroup :: buildInheritance(TextPropertyList& tpl )
{
   static list<TextPropertyGroup*> callStack;

   if ( !inheritanceBuild ) {
      if ( std::find ( callStack.begin(), callStack.end(), this ) != callStack.end() )
         fatalError ( "endless inheritance loop detected: type " + typeName + "; ID " + strrr ( id ));

      callStack.push_back( this );

      PropertyReadingContainer prc ( typeName, this );
      prc.addBool ( "abstract", abstract, false );

      if ( find ( typeName+".parent") != NULL ) {
         typedef vector<int> ParentIDs;
         ParentIDs parentIDs;
         prc.addIntegerArray ( "parent", parentIDs );
         for ( ParentIDs::iterator i = parentIDs.begin(); i != parentIDs.end(); i++ ) {
            TextPropertyGroup* p = tpl.get ( *i );
            if ( p ) {
               parents.push_back ( p );
               p->buildInheritance( tpl );
            } else
               fatalError ( location + " : no parent with ID " + strrr(*i) + " of type " + typeName + " could be found !" );
         }
      }

      //! building inheritance
      for ( Entries::iterator i = entries.begin(); i != entries.end(); i++ ) {
         if ( i->op != Entry::eq && i->op != Entry::alias_all && i->op != Entry::alias) {
            Parents::iterator p = parents.begin();
            while ( p != parents.end() ) {
               i->parent = (*p)->find( i->propertyName );
               if ( i->parent )
                  break;
               p++;
            }
            if ( p == parents.end())
               fatalError ( "could not find a parent entry for " + typeName + " :: " + i->propertyName  );
         }
      }

      // linking alias
      Entries additionalEntries;
      for ( Entries::iterator i = entries.begin(); i != entries.end(); i++ ) {
         if ( i->op == Entry::alias_all ) {
            ASCString s = i->value;
            s.toLower();
            ASCString::size_type pos;
            do {
               pos = s.find ( " " );
               if ( pos != s.npos )
                  s.erase(pos,1);
            } while ( pos != s.npos );
            if ( s.length() > 0 )
               if ( s[s.length()-1] != '.' )
                  s = s + ".";

            ASCString newName = i->propertyName;
            if ( newName.length() > 0 )
               if ( newName[newName.length()-1] != '.' )
                  newName = newName + ".";

            int counter = 0;
            for ( Entries::iterator i = entries.begin(); i != entries.end(); i++ ) {
               if ( i->propertyName.find ( s ) == 0 ) {
                  Entry e = *i;
                  e.propertyName.replace ( 0, s.length(), newName );
                  additionalEntries.push_back ( e );
                  counter++;
               }
            }
            if ( !counter )
               fatalError ( "could not find any alias matching " + i->value + " for " + typeName + " :: " + i->propertyName  );

         }
         if ( i->op == Entry::alias ) {
            ASCString s = i->value;
            s.toLower();
            ASCString::size_type pos;
            do {
                  pos = s.find ( " " );
                  if ( pos != s.npos )
                     s.erase(pos,1);
            } while ( pos != s.npos );
            Entry* alias = find ( s );
            if ( !alias )
               fatalError ( "could not find the alias " + i->value + " for " + typeName + " :: " + i->propertyName  );
            i->value = alias->value;
            i->op = alias->op;
            i->parent = alias->parent;
         }
      }

      for ( Entries::iterator i = additionalEntries.begin(); i != additionalEntries.end(); i++ )
         if ( find ( i->propertyName ) == NULL )
            addEntry ( *i );

      callStack.pop_back();

      inheritanceBuild = true;
   }
}

int TextPropertyGroup :: evalID()
{
   PropertyReadingContainer prc ( typeName, this );
   prc.addInteger ( "ID", id, 0 );
   return id;
}




TextPropertyGroup::Entry*  TextPropertyGroup :: find( const ASCString& n )
{
   EntryCache::iterator i = entryCache.find ( n );
   if ( i != entryCache.end() )
      return i->second;
   else {
      for ( Parents::iterator p = parents.begin(); p != parents.end(); p++ ) {
         TextPropertyGroup::Entry* ent = (*p)->find ( n );
         if ( ent )
            return ent;
      }
      return NULL;
   }
}

void TextPropertyGroup::addEntry( const Entry& entry )
{
   entries.push_back ( entry );
   entryCache[entry.propertyName] = &entries.back();
}



///////////////////// TextFormatParser //////////////////////////

ASCString TextFormatParser::readLine ( )
{
   ASCString s;
   int noCommentLines = 0;
   int bracketsOpen = 0;
   do {
      ASCString t = stream->readString();
      ASCString::size_type pos = t.find_first_not_of ( whiteSpace );
      if ( pos != ASCString::npos ) {
         if ( t[pos] != ';' ) {
            noCommentLines++;
            if ( !s.empty() )
               s += " ";

            s += t.substr ( pos );

            if ( t.find ( "[" ) != t.npos )
               bracketsOpen++;

            if ( t.find ( "]" ) != t.npos )
               bracketsOpen--;
         }
      } else {
         // empty line
         if ( bracketsOpen )
            s += "\n";
      }

   } while ( noCommentLines == 0 || bracketsOpen > 0 );

   ASCString::size_type pos = s.find ( "[" );
   if ( pos != s.npos )
      s.erase ( pos, 1 );

   pos = s.rfind ( "]" );
   if ( pos != s.npos )
      s.erase ( pos, 1 );

   return s;
}



void TextFormatParser::parseLine ( const ASCString& line )
{

   StringTokenizer st ( line );
   s1 = st.getNextToken();
   s2 = st.getNextToken();
   s3 = st.getRemaining();

   int op = -1;
   for ( int i = 0; i < operationsNum; i++ )
      if ( s2 == operations[i] )
         op = i;

   if ( op != -1 ) {
      /*
      if ( s3.empty() )
         error ( "missing data after operand");
      */

      ASCString s;
      for ( Level::iterator i = level.begin(); i != level.end(); i++ )
         s += *i + ".";
      s += s1;
      textPropertyGroup->addEntry ( TextPropertyGroup::Entry (s, TextPropertyGroup::Entry::Operator(op), s3 ) );
      return;
   }

   if ( !s1.empty() && s2 == "{" ) {
      s1.toLower();
      startLevel ( s1 );
      return;
   }

   if ( s1 == "}" ) {
      if ( level.empty() )
         error ("closing unopened bracket");

      s2.toLower();
      if ( s2 != level.back() )
         error ( "unmatching bracket closed");

      level.pop_back();
      levelDepth--;
      return;
   }
   error ( "unknown operator at entry " + s1  );
}


void TextFormatParser::startLevel ( const ASCString& levelName )
{

   if ( levelDepth == 0  ) {
      if ( !primaryName.empty() )
         if ( levelName.compare_ci ( primaryName )  )
            error ( "expecting group " + primaryName + " , found " + levelName );
      textPropertyGroup->typeName = levelName;
      textPropertyGroup->typeName.toLower();
   }

   int curlevel = ++levelDepth;
   level.push_back ( levelName );

   do {
       parseLine ( readLine() );
   } while ( levelDepth >= curlevel );
}


TextPropertyGroup* TextFormatParser::run (  )
{
   textPropertyGroup = new TextPropertyGroup ;
   textPropertyGroup->fileName = stream->getDeviceName();
   textPropertyGroup->location = stream->getLocation();
   parseLine ( readLine() );
   return textPropertyGroup;
}

void TextFormatParser::error ( const ASCString& errmsg )
{
   if ( stream )
      throw ParsingError ( stream->getLocation() + " : " + errmsg );
   else
      throw ParsingError ( " : " + errmsg );
}






