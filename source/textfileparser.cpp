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
#include "ascstring.h"
#include "basegfx.h"
#include "typen.h"
#include "terraintype.h"
#include "objecttype.h"
#include "textfileparser.h"
#include "dlg_box.h"
#include "stringtokenizer.h"
#include "loadpcx.h"

/*
const int operationsNum = 5;
const char* operations[operationsNum] =  { "=", "*=", "/=", "+=", "-=" };
*/


const int operationsNum = 1;
const char* operations[operationsNum] =  { "=" };


const char* whiteSpace = " \t";




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
   ASCString output;
   for ( int i = 0; i < levelDepth; i++ )
      output += "   ";

   output += p.getLastName();
   output +=  " = ";

   int indent = output.length();
   ASCString::size_type pos = value.find ( "\n" );
   if ( pos != ASCString::npos ) {
      output += "[";
      ASCString::size_type oldpos = 0;
      do {
         output += value.substr( oldpos, pos-oldpos+1 );
         oldpos = pos+1;
         for ( int i = 0; i < indent; i++ )
            output += " ";
         if ( pos+1 < value.length() )
            pos = value.find ( "\n", pos+1 );
         else
            pos = ASCString::npos;
      } while ( pos != ASCString::npos );
      output += value.substr( oldpos );
      output += "]";
   } else
      output += value;

   output += "\n";

   stream.writeString ( output, false );
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

PropertyContainer::StringArrayProperty&  PropertyContainer::addStringArray ( const ASCString& name, vector<ASCString>& property )
{
   StringArrayProperty* ip = new StringArrayProperty ( property );
   setup ( ip, name );
   return *ip;
}


PropertyContainer::IntegerArrayProperty&  PropertyContainer::addIntegerArray ( const ASCString& name, vector<int>& property )
{
   IntegerArrayProperty* ip = new IntegerArrayProperty ( property );
   setup ( ip, name );
   return *ip;
}

PropertyContainer::IntRangeArrayProperty&  PropertyContainer::addIntRangeArray ( const ASCString& name, vector<IntRange>& property )
{
   IntRangeArrayProperty* ip = new IntRangeArrayProperty ( property );
   setup ( ip, name );
   return *ip;
}


PropertyContainer::TagArrayProperty&  PropertyContainer::addTagArray ( const ASCString& name, BitSet& property, int tagNum, const char** tags, bool inverted )
{
   TagArrayProperty* ip = new TagArrayProperty ( property, tagNum, tags, inverted );
   setup ( ip, name );
   return *ip;
}

PropertyContainer::TagIntProperty&  PropertyContainer::addTagInteger ( const ASCString& name, int& property, int tagNum, const char** tags, bool inverted )
{
   TagIntProperty* ip = new TagIntProperty ( property, tagNum, tags, inverted );
   setup ( ip, name );
   return *ip;
}

PropertyContainer::NamedIntProperty&  PropertyContainer::addNamedInteger ( const ASCString& name, int& property, int tagNum, const char** tags )
{
   NamedIntProperty* ip = new NamedIntProperty ( property, tagNum, tags );
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
   fatalError ( errmsg );
   #else
   displaymessage( "file " + textPropertyGroup->fileName+ ": " + errmsg, 1 );
   #endif
}

void PropertyContainer::error ( const ASCString& errmsg )
{
   throw ParsingError ( getLocation() + " : " + errmsg );
}

bool PropertyContainer::find ( const ASCString& name )
{
   ASCString n;
   for ( Level::iterator i = level.begin(); i != level.end(); i++ )
      n += *i + ".";
   n += name;
   n.toLower();

   return textPropertyGroup->find ( n ) != textPropertyGroup->entries.end();
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
   textPropertyGroup = new TextPropertyGroup();
   textPropertyGroup->fileName = stream.getDeviceName();
   textPropertyGroup->location = stream.getLocation();
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
      if ( s.compare_ci ( "true" )==0 || s.compare_ci ( "1" )==0 )
         property = true;
      else
         if ( s.compare_ci ( "false" )==0 || s.compare_ci ( "0" )==0 )
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
   if ( propertyContainer->isReading() ) {
      property = entry->value;
      ASCString::size_type pos = property.find_first_not_of ( whiteSpace );
      if ( pos == ASCString::npos )
         property.erase();
      else
         property.erase ( 0, pos );
   } else {
      valueToWrite = property ;

      ASCString::size_type pos = 0;
      static const int linewidth = 60;
      do {
         if ( pos + linewidth < valueToWrite.length() ) {
            pos = valueToWrite.find_first_of ( whiteSpace, pos + linewidth );
            if ( pos != ASCString::npos )
               valueToWrite[pos] = '\n';
         } else
            pos = ASCString::npos;
      } while ( pos != ASCString::npos );
   }
}

void PropertyContainer::StringArrayProperty::evaluate_rw ( )
{
   if ( propertyContainer->isReading() ) {
      property.clear();
      StringTokenizer st ( entry->value, true );
      ASCString s = st.getNextToken();
      while ( !s.empty() ) {
         property.push_back ( s.c_str() );
         s = st.getNextToken();
      }
   } else {
      for ( PropertyType::iterator i = property.begin(); i != property.end(); i++ ) {
         valueToWrite += *i;
         valueToWrite += " ";
      }
   }
}


void PropertyContainer::IntegerArrayProperty::evaluate_rw ( )
{
   if ( propertyContainer->isReading() ) {
      property.clear();
      StringTokenizer st ( entry->value, true );
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

void PropertyContainer::IntRangeArrayProperty::evaluate_rw ( )
{
   if ( propertyContainer->isReading() ) {
      property.clear();
      StringTokenizer st ( entry->value );
      ASCString s = st.getNextToken();
      while ( !s.empty() ) {
         if ( s.find ( "-" ) != ASCString::npos ) {
            ASCString from = s.substr ( 0, s.find ( "-" )-1 );
            ASCString to = s.substr ( s.find ( "-" )+1 );
            property.push_back ( IntRange ( atoi ( from.c_str() ), atoi ( to.c_str() )));
         } else {
            property.push_back ( IntRange ( atoi ( s.c_str() ), atoi ( s.c_str() )));
         }
         s = st.getNextToken();
      }
   } else {
      for ( PropertyType::iterator i = property.begin(); i != property.end(); i++ ) {
         if ( i->from != i->to ) {
            valueToWrite += strrr ( i->from );
            valueToWrite += "-";
            valueToWrite += strrr ( i->to );
         } else
            valueToWrite += strrr ( i->from );
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
            if ( s.compare_ci ( tags[i] )==0  ) {
               if ( inverted )
                  property.reset ( i );
               else
                  property.set ( i );
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

void PropertyContainer::TagIntProperty::evaluate_rw ( )
{
   if ( propertyContainer->isReading() ) {
      if ( inverted )
         property = -1;
      else
         property = 0;

      StringTokenizer st ( entry->value );
      ASCString s = st.getNextToken();
      while ( !s.empty() ) {
         bool found = false;
         for ( int i = 0; i < tagNum; i++ )
            if ( s.compare_ci ( tags[i] )==0  ) {
               property ^= 1 << i;
               found = true;
               break;
            }

         if ( !found )
            propertyContainer->error ( name + ": token "+ s +" unknown" );
         s = st.getNextToken();
      }
   } else
      for ( int i = 0; i < tagNum; i++ )
         if ( !!(property & (1 << i)) != inverted ) {
            valueToWrite += tags[i];
            valueToWrite += " ";
         }
}

void PropertyContainer::NamedIntProperty::evaluate_rw ( )
{
   if ( propertyContainer->isReading() ) {

      StringTokenizer st ( entry->value );
      ASCString s = st.getNextToken();
      if ( !s.empty() ) {
         bool found = false;
         for ( int i = 0; i < tagNum; i++ )
            if ( s.compare_ci ( tags[i] )==0  ) {
               property = i;
               found = true;
               break;
            }

         if ( !found )
            propertyContainer->error ( name + ": token "+ s +" unknown" );
      }
   } else
       valueToWrite += tags[property];
}

void* getFieldMask()
{
   static void* mask = NULL;
   if ( !mask ) {
      int i ;
      tnfilestream s ( "largehex.raw", tnstream::reading );
      s.readrlepict ( &mask, false, & i );
   }
   return mask;
}

vector<void*> loadImage ( const ASCString& file, int num )
{
   vector<void*> images;

   int imgwidth = fieldsizex;
   int imgheight = fieldsizey;

   int xsize;
   if ( num <= 10)
      xsize = (num+1)* 100;
   else
      xsize = 1100;

   int pcxwidth;
   int pcxheight;
   int depth = pcxGetColorDepth ( file, &pcxwidth, &pcxheight );
   if ( depth > 8 ) {
      tvirtualdisplay vdp ( xsize, (num/10+1)*100, TCalpha, 32 );
      if ( num == 1 )
         loadpcxxy ( file, 0, 0, 0, &imgwidth, &imgheight );
      else
         loadpcxxy ( file, 0, 0, 0 );

      for ( int i = 0; i < num; i++ ) {
          int x1 = (i % 10) * 100;
          int y1 = (i / 10) * 100;
          TrueColorImage* tci = getimage ( x1, y1, x1 + imgwidth-1, y1 + imgheight-1 );

          tvirtualdisplay vdp ( 100, 100, 255 );
          void* img = convertimage ( tci, pal );
          putimage ( 0, 0, img );
          putmask ( 0, 0, getFieldMask(), 0 );
          getimage ( 0, 0, imgwidth-1, imgheight-1, img );
          images.push_back ( img );
      }
   } else {
      tvirtualdisplay vdp ( max(xsize, pcxwidth), max( (num/10+1)*100, pcxheight), 255, 8 );

      if ( num == 1 )
         loadpcxxy ( file, 0, 0, 0, &imgwidth, &imgheight );
      else
         loadpcxxy ( file, 0, 0, 0 );

      for ( int i = 0; i < num; i++ ) {
          int x1 = (i % 10) * 100;
          int y1 = (i / 10) * 100;
          if ( num > 1 )
             putmask ( x1, y1, getFieldMask(), 0 );
          void* img = new char[imagesize (0, 0, imgheight-1, imgwidth-1)];
          getimage ( x1, y1, x1+imgwidth-1, y1+imgheight-1, img );
          images.push_back ( img );
      }
   }

   return images;
}

void PropertyContainer::ImageProperty::evaluate_rw ( )
{
   if ( propertyContainer->isReading() ) {
      try {
         StringTokenizer st ( entry->value, true );
         FileName fn = st.getNextToken();
         fn.toLower();
         if ( fn.suffix() == "png" ) {
            SDLmm::Surface* s = NULL;
            do {
               tnfilestream fs ( fn, tnstream::reading );
               SDLmm::Surface s2 ( IMG_LoadPNG_RW ( SDL_RWFromStream( &fs )));
               s2.SetAlpha ( SDL_SRCALPHA, SDL_ALPHA_OPAQUE );
               if ( !s )
                  s = new SDLmm::Surface ( s2 );
               else {
                  int res = s->Blit ( s2 );
                  if ( res < 0 )
                     propertyContainer->warning ( "PropertyContainer::ImageProperty::evaluate_rw - couldn't blit surface "+fn);
               }

               fn = st.getNextToken();
            } while ( !fn.empty() );
            if ( s )
               property = convertSurface ( *s );
            else
               property = NULL;
         } else
            if ( fn.suffix() == "pcx" ) {
               property = loadImage ( fn, 1 )[0];
            }
      }
      catch ( ASCexception ){
         propertyContainer->error( "error accessing file " + entry->value );
         property = NULL;
      }
   } else {
      int width, height;
      getpicsize( property, width, height) ;
      tvirtualdisplay vdp ( width+10, height+10, 255, 8 );
      putimage ( 0, 0, property );
      valueToWrite = extractFileName_withoutSuffix(fileName) + ".pcx";
      writepcx ( valueToWrite, 0, 0, width-1, height-1, pal );
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
      valueToWrite = extractFileName_withoutSuffix(fileName) + ".pcx" + " " + strrr( cnt );
      writepcx ( extractFileName_withoutSuffix(fileName) + ".pcx", 0, 0, 1100 - 1, 100 * (num / 10 + 1) - 1, pal );
   }
}


///////////////////// TextPropertyGroup //////////////////////////

void TextPropertyGroup :: buildInheritance(TextPropertyList& tpl )
{
   if ( !inheritanceBuild ) {
      if ( find ( "inherits") != entries.end()) {
         
      }
   }
}



TextPropertyGroup::Entries::iterator  TextPropertyGroup :: find( const ASCString& n )
{
   for ( Entries::iterator i = entries.begin(); i != entries.end(); i++ )
      if ( i->propertyName == n )
         return i;
   return entries.end();
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


