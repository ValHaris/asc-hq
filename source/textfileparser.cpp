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
#include "sgstream.h"
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




ASCString TextPropertyGroup::Entry::toString() const
{
   Operator o = op;
   if ( o == alias_all_resolved )
     o = alias_all;

   ASCString s = propertyName + TextFormatParser::operations[o] + value ;
   return s;
}

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


void TextPropertyGroup :: error ( const ASCString& msg )
{
   fatalError ( "Error evaluating file " + location + "\n" + msg );
}

void TextPropertyGroup :: print( int indent )
{
   for ( Entries::iterator i = entries.begin(); i != entries.end(); i++ ) {
      for ( int m = 0; m<= indent; m++ )
         displayLogMessage(10, "  " );

      displayLogMessage(10,  i->toString());
   }

   for ( Parents::iterator i = parents.begin(); i != parents.end(); i++ ) {
      for ( int i = 0; i< indent; i++ )
         printf("  " );
      displayLogMessage(10, "  is inheriting from " + (*i)->location + "\n" );
      (*i)->print(indent+1 );
   }
}


void TextPropertyGroup :: buildInheritance(TextPropertyList& tpl )
{
   static list<TextPropertyGroup*> callStack;

   if ( !inheritanceBuild ) {
      if ( std::find ( callStack.begin(), callStack.end(), this ) != callStack.end() )
         error ( "endless inheritance loop detected: type " + typeName + "; ID " + strrr ( id ));

      callStack.push_back( this );

      PropertyReadingContainer prc ( typeName, this );
      prc.addBool ( "abstract", abstract, false );
      int iid;
      prc.addInteger("ID", iid, 0 );

      if ( find ( typeName+".parent") != NULL ) {
         typedef vector<int> ParentIDs;
         ParentIDs parentIDs;
         prc.addIntegerArray ( "parent", parentIDs );
         for ( ParentIDs::iterator i = parentIDs.begin(); i != parentIDs.end(); i++ ) {
            TextPropertyGroup* p = tpl.get ( *i );
            if ( p ) {
               parents.push_back ( p );
               displayLogMessage( 10, ASCString("  entering parent with ID ") + strrr(*i) + "\n" );
               p->buildInheritance( tpl );
               displayLogMessage( 10, ASCString("  leaving parent with ID ") + strrr(*i) + "\n" );
            } else
               error ( location + " : no parent with ID " + strrr(*i) + " of type " + typeName + " could be found !" );
         }
      }

      //! building inheritance
      /*
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
               error ( "could not find a parent entry for " + typeName + " :: " + i->propertyName  );
         }
      }
      */

      resolveAllAlias();

      callStack.pop_back();
      inheritanceBuild = true;
   }
}


void TextPropertyGroup :: resolveAllAlias( )
{
   int loop = 0;
   typedef list<Entry*> Unresolved;
   Unresolved unresolved;
   do {
      Entries additionalEntries;
      int resolvedCounter = 0;
      EntryPointerList toResolve;

      if ( loop == 0 ) {
         for ( Entries::iterator i = entries.begin(); i != entries.end(); i++ ) {
            bool resolved = processAlias ( *i , additionalEntries, toResolve );
            if ( !resolved )
                unresolved.push_back ( &(*i) );
            else
                resolvedCounter++;
         }
      } else {
         Unresolved newunresolved;
         for ( Unresolved::iterator i = unresolved.begin(); i != unresolved.end(); i++ ) {
            bool resolved = processAlias ( **i , additionalEntries, toResolve );
            if ( !resolved )
                newunresolved.push_back ( *i );
            else
                resolvedCounter++;
         }
         unresolved = newunresolved;
      }

      for ( Entries::iterator i = additionalEntries.begin(); i != additionalEntries.end(); i++ )
         // if ( find ( i->propertyName ) == NULL )
            addEntry ( *i );

      for ( EntryPointerList::iterator i = toResolve.begin(); i != toResolve.end(); i++ )
         (*i)->op = Entry::alias_all_resolved;

      if ( !resolvedCounter && !unresolved.empty() )
         for ( Unresolved::iterator i = unresolved.begin(); i != unresolved.end(); i++ )
            error ( "could not resolve the reference " + (*i)->value + " for " + typeName + " :: " + (*i)->propertyName  );

      loop++;
   } while ( !unresolved.empty() );
}

bool reallyVerbose = false;

int TextPropertyGroup::findGeneration ( Entry* e )
{
   for ( Entries::iterator i = entries.begin(); i != entries.end(); i++ )
      if ( &(*i) == e )
         return 0;

   for ( Parents::iterator i = parents.begin(); i != parents.end(); i++ ) {
      int fg = (*i)->findGeneration( e );
      if ( fg >= 0 )
         return fg+1;
   }
   return -1;
}


bool TextPropertyGroup::processAlias( Entry& e, Entries& entriesToAdd, EntryPointerList& markAsResolved )
{
   if ( e.op == Entry::alias_all ) {
      ASCString ss = e.value;
      ss.toLower();
      ASCString::size_type pos;
      do {
         pos = ss.find ( " " );
         if ( pos != ss.npos )
            ss.erase(pos,1);
      } while ( pos != ss.npos );

      ASCString s_without_dot = ss;
      if ( ss.length() > 0 )
         if ( ss[ss.length()-1] != '.' )
            ss = ss + ".";

      if ( s_without_dot.length() > 0 )
         if ( s_without_dot[s_without_dot.length()-1] == '.' )
            s_without_dot.erase ( s_without_dot.length()-1 );

      ASCString newName = e.propertyName;
      if ( newName.length() > 0 )
         if ( newName[newName.length()-1] != '.' )
            newName = newName + ".";

      int counter = 0;
      Matches matches;


      findMatches ( ss, s_without_dot, matches );

      for ( Matches::iterator i = matches.begin(); i != matches.end(); i++ )
         if ( (*i)->op == Entry::alias_all ) {
            displayLogMessage ( 9, "  alias is pending " + ss + "* <- " + e.propertyName + " because " + (*i)->propertyName + " is unresolved\n" );
            return false;
         }

      for ( Matches::iterator i = matches.begin(); i != matches.end(); i++ )
         if ( (*i)->op != Entry::alias_all_resolved ) {
            Entry e = **i;
            e.propertyName.replace ( 0, ss.length(), newName );
            TextPropertyGroup::Entry* existent = find ( e.propertyName );
            // if ( existent == NULL || findGeneration(*i) <= findGeneration( existent ) ) {
            if ( existent == NULL || findGeneration( existent ) > 0 ) {
               /* if ( existent && findGeneration( existent ) == 0 ) {
                  displayLogMessage(10, "    overwriting entry " + existent->toString() + " because it belongs to an older generation\n" );
                  *existent = e;
               } else */
               entriesToAdd.push_back ( e );
               displayLogMessage(10, "   aliasing entry " + (*i)->toString() + " to " + e.propertyName + " \n" );
            }
            counter++;
         } else
            displayLogMessage(10, "   skipping aliasing entry " + (*i)->toString() + " to " + e.propertyName + " because it is already resolved\n" );

      if ( !counter ) {
         displayLogMessage ( 9, "  could not successfully resolve alias " + ss + "* <- " + e.propertyName + "\n" );
         return false;
      } else {
         markAsResolved.push_back ( &e );
         displayLogMessage ( 9, "  successfully resolved alias " + e.propertyName + " ->* " + ss + "\n" );
         return true;
      }
   }

   if ( e.op == Entry::alias ) {
      ASCString s = e.value;
      s.toLower();
      ASCString::size_type pos;
      do {
            pos = s.find ( " " );
            if ( pos != s.npos )
               s.erase(pos,1);
      } while ( pos != s.npos );
      Entry* alias = find ( s );
      if ( !alias )
         return false;
      else {
         e.value = alias->value;
         e.op = alias->op;
         e.parent = alias->parent;
         return true;
      }
   }

   //! building inheritance
   if ( e.op != Entry::eq && e.op != Entry::alias_all && e.op != Entry::alias && e.op != Entry::alias_all_resolved  ) {
      Parents::iterator p = parents.begin();
      while ( p != parents.end() ) {
         e.parent = (*p)->find( e.propertyName );
         if ( e.parent )
            break;
         p++;
      }
      if ( p == parents.end())
         return false;
      else
         return true;
   }


   return true;
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

void TextPropertyGroup::findMatches( const ASCString& name, const ASCString& name_without_dot, Matches& matches )
{
   for ( Entries::iterator i = entries.begin(); i != entries.end(); i++ ) 
      if ( i->propertyName.find ( name ) == 0 || i->propertyName == name_without_dot )
         matches.push_back ( &(*i) );

   for ( Parents::iterator p = parents.begin(); p != parents.end(); p++ )
      (*p)->findMatches ( name, name_without_dot, matches );
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






