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
#include "ascstring.h"
#include "typen.h"
#include "terraintype.h"
#include "objecttype.h"
#include "textfileparser.h"
#include "dlg_box.h"

#include "loadpcx.h"

/*
const int operationsNum = 5;
const char* operations[operationsNum] =  { "=", "*=", "/=", "+=", "-=" };
*/


const int operationsNum = 1;
const char* operations[operationsNum] =  { "=" };






///////////////////// PropertyContainer //////////////////////////


void PropertyContainer :: run ( )
{
   if ( isReading() )
      for ( Properties::iterator i = properties.begin(); i != properties.end(); i++ )
         if ( !(*i)->evaluated )
            (*i)->evaluate();
}

void PropertyReadingContainer :: writeProperty ( Property& p, const ASCString& value )
{
   fatalError ( "Attempt to use PropertyReadingContainer :: writeProperty !");
}

void PropertyWritingContainer :: writeProperty ( Property& p, const ASCString& value )
{
   for ( int i = 0; i < levelDepth; i++ )
      stream.writeString ( "   ", false );

   stream.writeString ( p.getLastName(), false );
   stream.writeString ( " = " , false);
   stream.writeString ( value, false );
   stream.writeString ( "\n", false );
}


void PropertyContainer :: openBracket( const ASCString& name )
{
   level.push_back ( name );
   levelDepth++;
}

void PropertyContainer :: closeBracket(  )
{
   level.pop_back();
   levelDepth--;
}


void PropertyWritingContainer :: openBracket( const ASCString& name )
{
   for ( int i = 0; i < levelDepth; i++ )
      stream.writeString ( "   ", false );
   stream.writeString ( name + " { \n", false );

   PropertyContainer :: openBracket ( name );
}

void PropertyWritingContainer :: closeBracket(  )
{
   ASCString name = level.back();

   PropertyContainer :: closeBracket ( );

   for ( int i = 0; i < levelDepth; i++ )
      stream.writeString ( "   ", false );
   stream.writeString ( "} " + name + "\n", false );
}


void PropertyContainer::setup ( Property* p, const ASCString& name_ )
{
   ASCString name;
   for ( Level::iterator i = level.begin(); i != level.end(); i++ )
      name += *i + ".";
   name += name_;
   p->setName( name, name_ );
   p->setPropertyContainer ( this );
   properties.push_back ( p );

   if ( !isReading() )
      p->evaluate();
}


PropertyContainer::IntProperty& PropertyContainer::addInteger ( const ASCString& name, int& property )
{
   IntProperty* ip = new IntProperty ( property );
   setup ( ip, name );
   return *ip;
}

PropertyContainer::BoolProperty& PropertyContainer::addBool ( const ASCString& name, bool& property )
{
   BoolProperty* ip = new BoolProperty ( property );
   setup ( ip, name );
   return *ip;
}


PropertyContainer::StringProperty& PropertyContainer::addString ( const ASCString& name, ASCString& property )
{
   StringProperty* ip = new StringProperty ( property );
   setup ( ip, name );
   return *ip;
}


PropertyContainer::IntegerArrayProperty&  PropertyContainer::addIntegerArray ( const ASCString& name, vector<int>& property )
{
   IntegerArrayProperty* ip = new IntegerArrayProperty ( property );
   setup ( ip, name );
   return *ip;
}


PropertyContainer::TagArrayProperty&  PropertyContainer::addTagArray ( const ASCString& name, BitSet& property, int tagNum, const char** tags, bool inverted )
{
   TagArrayProperty* ip = new TagArrayProperty ( property, tagNum, tags, inverted );
   setup ( ip, name );
   return *ip;
}

PropertyContainer::ImageArrayProperty&   PropertyContainer::addImageArray ( const ASCString& name, vector<void*> &property, const ASCString& filename )
{
   ImageArrayProperty* ip = new ImageArrayProperty ( property, filename );
   setup ( ip, name );
   return *ip;
}


PropertyContainer::ImageProperty&   PropertyContainer::addImage ( const ASCString& name, void* &property, const ASCString& filename )
{
   ImageProperty* ip = new ImageProperty ( property, filename );
   setup ( ip, name );
   return *ip;
}



void PropertyContainer::warning ( const ASCString& errmsg )
{
   #ifdef converter
   fatalError ( errmsg.c_str() );
   #else
   displaymessage( "file " + textPropertyGroup->fileName+ ": " + errmsg, 1 );
   #endif
}

void PropertyContainer::error ( const ASCString& errmsg )
{
   warning( errmsg );
}




PropertyReadingContainer :: PropertyReadingContainer ( const ASCString& baseName, TextPropertyGroup* tpg ) : PropertyContainer ( baseName, tpg, true )
{
   openBracket ( baseName );
}

PropertyReadingContainer :: ~PropertyReadingContainer (  )
{
   closeBracket();
   if ( levelDepth )
      error ( "PropertyWritingContainer :: ~PropertyWritingContainer - still brackets open" );
}

PropertyWritingContainer :: PropertyWritingContainer ( const ASCString& baseName, const ASCString& filename_ ) : PropertyContainer ( baseName, NULL, false ), stream ( filename_, tnstream::writing )
{
   setFilename ( filename_ );
   textPropertyGroup = new TextPropertyGroup();
   openBracket ( baseName );
}

PropertyWritingContainer :: ~PropertyWritingContainer()
{
   closeBracket();
   if ( levelDepth )
      error ( "PropertyWritingContainer :: ~PropertyWritingContainer - still brackets open" );
   delete textPropertyGroup;
}


///////////////////// PropertyContainer::Properties //////////////////////////


void PropertyContainer::Property::setName ( const ASCString& name_, const ASCString& lastName_ )
{
  name = name_ ;
  name.toLower();
  lastName = lastName_;
}

void PropertyContainer::Property::findEntry ()
{
   if ( !propertyContainer )
      fatalError ( "PropertyContainer::Property::evaluate - no propertyContainer ");
   if (!entry ) {
      for ( TextPropertyGroup::Entries::iterator i = propertyContainer->textPropertyGroup->entries.begin(); i != propertyContainer->textPropertyGroup->entries.end(); i++ )
          if ( i->propertyName == name ) {
             entry = &(*i);
             return;
          }
   }
   propertyContainer->error ( "entry " + name +" not found" );
}

void PropertyContainer::Property::evaluate ()
{
   if ( evaluated )
      return;

   if ( propertyContainer->isReading() ) {
      findEntry();
      evaluate_rw();
      evaluated = true;
   } else {
      evaluate_rw();
      propertyContainer->writeProperty ( *this, valueToWrite );
      evaluated = true;
   }
}



void PropertyContainer::IntProperty::evaluate_rw ( )
{
   if ( propertyContainer->isReading() )
      property = atoi ( entry->value.c_str() );    //   strtol(nptr, NULL, 10);
   else
      valueToWrite = strrr ( property );
}


void PropertyContainer::BoolProperty::evaluate_rw ( )
{
   if ( propertyContainer->isReading() ) {
      StringTokenizer st ( entry->value );
      ASCString s = st.getNextToken();
      if ( s.equal_ci ( "true" ) || s.equal_ci ( "1" ) )
         property = true;
      else
         if ( s.equal_ci ( "false" ) || s.equal_ci ( "0" ) )
            property = false;
         else
            propertyContainer->error ( name + ": token "+ s +" unknown" );
   } else
      if ( property )
         valueToWrite =  "true";
      else
         valueToWrite = "false";
}



void PropertyContainer::StringProperty::evaluate_rw ( )
{
   if ( propertyContainer->isReading() )
      property = entry->value;
   else
      valueToWrite = property ;
}


void PropertyContainer::IntegerArrayProperty::evaluate_rw ( )
{
   if ( propertyContainer->isReading() ) {
      property.clear();
      StringTokenizer st ( entry->value );
      ASCString s = st.getNextToken();
      while ( !s.empty() ) {
         property.push_back ( atoi ( s.c_str() ));
         s = st.getNextToken();
      }
   } else {
      for ( PropertyType::iterator i = property.begin(); i != property.end(); i++ ) {
         valueToWrite += strrr ( *i );
         valueToWrite += " ";
      }
   }
}

void PropertyContainer::TagArrayProperty::evaluate_rw ( )
{
   if ( propertyContainer->isReading() ) {
      if ( inverted )
         property.set();
      else
         property.reset();
      StringTokenizer st ( entry->value );
      ASCString s = st.getNextToken();
      while ( !s.empty() ) {
         bool found = false;
         for ( int i = 0; i < tagNum; i++ )
            if ( s.equal_ci ( tags[i] )  ) {
               property.flip ( i );
               found = true;
               break;
            }

         if ( !found )
            propertyContainer->error ( name + ": token "+ s +" unknown" );
         s = st.getNextToken();
      }
   } else
      for ( int i = 0; i < tagNum; i++ )
         if ( property.test(i) != inverted ) {
            valueToWrite += tags[i];
            valueToWrite += " ";
         }
}



vector<void*> loadImage ( const ASCString& file, int num )
{
   vector<void*> images;

   static void* mask = NULL;
   if ( !mask ) {
      int i ;
      tnfilestream s ( "largehex.raw", tnstream::reading );
      s.readrlepict ( &mask, false, & i );
   }

   int xsize;
   if ( num <= 10)
      xsize = (num+1)* 100;
   else
      xsize = 1100;

   int depth = pcxGetColorDepth ( file.c_str() );
   if ( depth > 8 ) {
      tvirtualdisplay vdp ( xsize, (num/10+1)*100, TCalpha, 32 );
      loadpcxxy ( file.c_str(), 0, 0, 0 );
      for ( int i = 0; i < num; i++ ) {
          int x1 = (i % 10) * 100;
          int y1 = (i / 10) * 100;
          TrueColorImage* tci = getimage ( x1, y1, x1 + fieldxsize-1, y1 + fieldysize-1 );

          tvirtualdisplay vdp ( 100, 100, 255 );
          void* img = convertimage ( tci, pal );
          putimage ( 0, 0, img );
          putmask ( 0, 0, mask, 0 );
          getimage ( 0, 0, fieldsizex-1, fieldsizey-1, img );
          images.push_back ( img );
      }
   } else {
      tvirtualdisplay vdp ( xsize, (num/10+1)*100, 255, 8 );
      loadpcxxy ( file.c_str(), 0, 0, 0 );
      for ( int i = 0; i < num; i++ ) {
          int x1 = (i % 10) * 100;
          int y1 = (i / 10) * 100;
          putmask ( x1, y1, mask, 0 );
          void* img = new char[imagesize (0, 0, fieldsizex-1, fieldsizey-1)];
          getimage ( x1, y1, x1+fieldsizex-1, y1+fieldsizey-1, img );
          images.push_back ( img );
      }
   }

   return images;
}

void PropertyContainer::ImageProperty::evaluate_rw ( )
{
   if ( propertyContainer->isReading() ) {
      try {
         StringTokenizer st ( entry->value );
         property = loadImage ( st.getNextToken(), 1 )[0];
      }
      catch ( ASCexception ){
         propertyContainer->error( "error accessing file " + entry->value );
         property = NULL;
      }
   } else {
      tvirtualdisplay vdp ( 100, 100, 255, 8 );
      putimage ( 0, 0, property );
      valueToWrite = fileName + ".pcx";
      writepcx ( valueToWrite, 0, 0, fieldsizex-1, fieldsizey-1, pal );
   }
}

void PropertyContainer::ImageArrayProperty::evaluate_rw ( )
{
   if ( propertyContainer->isReading() ) {
      try {
         StringTokenizer st ( entry->value );
         ASCString imgName = st.getNextToken();
         ASCString imgNumS = st.getNextToken();
         if ( imgNumS.empty() )
            propertyContainer->error( name + ": image number missing" );
         int imgNum = atoi ( imgNumS.c_str() );
         property = loadImage ( imgName, imgNum );
      }
      catch ( ASCexception ){
         propertyContainer->error( "error accessing file " + entry->value );
         property.clear();
      }
   } else {
      int num = property.size();
      tvirtualdisplay vdp ( 1100, 100 * (num / 10 + 1), 255, 8 );
      int cnt = 0;
      for ( PropertyType::iterator i = property.begin(); i != property.end(); i++ ) {
         putimage ( (cnt % 10) * 100, (cnt / 10) * 100, *i );
         cnt++;
      }
      valueToWrite = fileName + ".pcx" + " " + strrr( cnt );
      writepcx ( fileName + ".pcx", 0, 0, 1100 - 1, 100 * (num / 10 + 1) - 1, pal );
   }
}



///////////////////// TextFormatParser //////////////////////////



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
      if ( s3.empty() )
         error ( "missing data after operand");

      ASCString s;
      for ( Level::iterator i = level.begin(); i != level.end(); i++ )
         s += *i + ".";
      s += s1;
      textPropertyGroup->entries.push_back ( TextPropertyGroup::Entry (s, TextPropertyGroup::Entry::Operator(op), s3 ) );
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
   }
}


void TextFormatParser::startLevel ( const ASCString& levelName )
{

   if ( levelDepth == 0 )
      if ( ! levelName.equal_ci ( primaryName )  )
         error ( "expecting group " + primaryName + " , found " + levelName );

   int curlevel = ++levelDepth;
   level.push_back ( levelName );

   do {
       parseLine ( stream->readString() );
   } while ( levelDepth >= curlevel );
}


TextPropertyGroup* TextFormatParser::run (  )
{
   textPropertyGroup = new TextPropertyGroup ;
   textPropertyGroup->fileName = extractFileName_withoutSuffix ( stream->getDeviceName() );
   textPropertyGroup->location = stream->getDeviceName();
   parseLine ( stream->readString() );
   return textPropertyGroup;
}

void TextFormatParser::error ( const ASCString& errmsg )
{
   #ifdef converter
   fatalError ( errmsg.c_str() );
   #else
   displaymessage(errmsg, 1 );
   #endif
}


void testtext ( TerrainType* pt, ObjectType* po )
{
   {
      tnfilestream s ( "terraintest.txt", tnstream::reading );
      TextFormatParser tfp ( &s, "TerrainType" );

      TextPropertyGroup* tpg = tfp.run();
      PropertyReadingContainer pc ( "TerrainType", tpg );

      TerrainType t;
      t.runTextIO ( pc );
      pc.run();
      delete tpg;
   }


   {
   PropertyWritingContainer pc2 ( "TerrainType", "test.txtbdt" );

   pt->runTextIO ( pc2 );
   pc2.run();
   }
   {
   PropertyWritingContainer pc2 ( "ObjectType", "test.txtobj" );

   po->runTextIO ( pc2 );
   pc2.run();
   }

}