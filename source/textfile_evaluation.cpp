/***************************************************************************
                          textfile_evaluation.cpp  -  description
                             -------------------
    begin                : Thu Oct 06 2002
    copyright            : (C) 2002 by Martin Bickel
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
#include <algorithm>
#include <iostream>
#include "global.h"
#include "ascstring.h"
#include "textfileparser.h"
#include "textfile_evaluation.h"
#include "stringtokenizer.h"
#ifdef ParserLoadImages
 #include <SDL_image.h>
 // #include "basegfx.h"
 #include "typen.h"
 #include "graphics/blitter.h"
#endif

#ifdef WIN32
#include "Windows.h"
#endif


const char* fileNameDelimitter = " =*/+<>,";


         template <class T>
         class PropertyTemplate: public PropertyContainer::Property  {
               void operation_not_defined( const TextPropertyGroup::Entry& entry ) const;
            protected:

               T& property;
               T defaultValue;

               virtual T parse ( const TextPropertyGroup::Entry& entry ) const;
               virtual T operation_mult ( const TextPropertyGroup::Entry& entry ) const;
               virtual T operation_add ( const TextPropertyGroup::Entry& entry ) const;
               virtual T operation_sub ( const TextPropertyGroup::Entry& entry ) const;
               virtual T operation_eq ( const TextPropertyGroup::Entry& entry ) const;


            public:
               void evaluate ( );

               PropertyTemplate ( T& property_ ) : PropertyContainer::Property ( false ), property ( property_ ) {};
               PropertyTemplate ( T& property_, const T& defaultValue_ ) : PropertyContainer::Property ( true ), property ( property_ ), defaultValue ( defaultValue_)  {};
         };


         typedef PropertyTemplate<int> PTI;
         class IntegerProperty : public PTI {
            protected:
              ASCString toString ( ) const ;
              int operation_eq  ( const TextPropertyGroup::Entry& entry ) const;
              int operation_add ( const TextPropertyGroup::Entry& entry ) const;
              int operation_sub ( const TextPropertyGroup::Entry& entry ) const;
              int operation_mult ( const TextPropertyGroup::Entry& entry ) const;
            public:
               IntegerProperty ( int& property_ ) : PTI ( property_ ) {};
               IntegerProperty ( int& property_, int defaultValue_ ) : PTI ( property_, defaultValue_ ) {};
         };

         typedef PropertyTemplate<double> PTD;
         class FloatProperty : public PTD {
            protected:
              ASCString toString ( ) const ;
              double operation_eq  ( const TextPropertyGroup::Entry& entry ) const;
              double operation_add ( const TextPropertyGroup::Entry& entry ) const;
              double operation_mult ( const TextPropertyGroup::Entry& entry ) const;
            public:
               FloatProperty ( double& property_ ) : PTD ( property_ ) {};
               FloatProperty ( double& property_, double defaultValue_ ) : PTD ( property_, defaultValue_ ) {};
         };


         typedef PropertyTemplate<bool> PTB;
         class BoolProperty : public PTB {
            protected:
              ASCString toString ( ) const;
              bool operation_eq ( const TextPropertyGroup::Entry& entry ) const;
            public:
               BoolProperty ( bool& property_ ) : PTB ( property_ ) {};
               BoolProperty ( bool& property_, bool defaultValue_ ) : PTB ( property_, defaultValue_ ) {};
         };

         typedef PropertyTemplate<ASCString> PTS;
         class StringProperty : public PTS {
            protected:
              ASCString toString ( ) const;
              ASCString operation_eq  ( const TextPropertyGroup::Entry& entry ) const;
              ASCString operation_add ( const TextPropertyGroup::Entry& entry ) const;
            public:
               StringProperty ( ASCString& property_ ) : PTS ( property_ ) {};
               StringProperty ( ASCString& property_, const ASCString& defaultValue ) : PTS ( property_, defaultValue ) {};
         };

         typedef PropertyTemplate< vector<ASCString> > PTSA;
         class StringArrayProperty : public PTSA {
              typedef vector<ASCString> PropertyType;
            protected:
              ASCString toString ( ) const;
              PropertyType operation_eq ( const TextPropertyGroup::Entry& entry ) const ;
            public:
               StringArrayProperty ( vector<ASCString>& property_ ) : PTSA ( property_ ) {};
         };


         template <class T>
         class ValArrayProperty: public PropertyTemplate< vector<T> > {
            protected:
              typedef vector<T> PropertyType;
              PropertyType operation_add ( const TextPropertyGroup::Entry& entry ) const;
              PropertyType operation_sub ( const TextPropertyGroup::Entry& entry ) const;
              PropertyType operation_mult ( const TextPropertyGroup::Entry& entry ) const;
              ValArrayProperty ( PropertyType& property_ ) : PropertyTemplate<vector<T> > ( property_ ) {};

         };

         typedef ValArrayProperty<int> PTIA;
         class IntegerArrayProperty : public PTIA {
            protected:
              PropertyType operation_eq ( const TextPropertyGroup::Entry& entry ) const ;
              ASCString toString ( ) const;
            public:
               IntegerArrayProperty ( vector<int>& property_ ) : PTIA ( property_ ) {};
         };

         typedef ValArrayProperty<double> PTDA;
         class DoubleArrayProperty : public PTDA {
            protected:
              PropertyType operation_eq ( const TextPropertyGroup::Entry& entry ) const ;
              ASCString toString ( ) const;
            public:
               DoubleArrayProperty ( vector<double>& property_ ) : PTDA ( property_ ) {};
         };


         typedef PropertyTemplate< vector<IntRange> > PTIRA;
         class IntRangeArrayProperty : public PTIRA {
              typedef vector<IntRange> PropertyType;
              bool required;
              bool hasDefault() {return !required; };
            protected:
              PropertyType operation_eq ( const TextPropertyGroup::Entry& entry ) const ;
              ASCString toString ( ) const;
            public:
               IntRangeArrayProperty ( vector<IntRange>& property_, bool _required ) : PTIRA ( property_ ), required(_required) {};
         };

         typedef PropertyTemplate< BitSet > PTTA;
         class TagArrayProperty : public PTTA {
              int tagNum;
              const char** tags;
              bool inverted;
            protected:
              BitSet operation_eq ( const TextPropertyGroup::Entry& entry ) const;
              BitSet operation_add ( const TextPropertyGroup::Entry& entry ) const;
              BitSet operation_sub ( const TextPropertyGroup::Entry& entry ) const;
              ASCString toString ( ) const;
            public:
               TagArrayProperty ( BitSet& property_, int tagNum_, const char** tags_, bool inverted_  ) : PTTA ( property_ ), tagNum (tagNum_), tags ( tags_ ), inverted ( inverted_ ) {};
         };

         typedef PropertyTemplate<int> PTTI;
         class TagIntProperty : public PTTI {
              int tagNum;
              const char** tags;
              bool inverted;
            protected:
              int operation_eq ( const TextPropertyGroup::Entry& entry ) const ;
              int operation_add ( const TextPropertyGroup::Entry& entry ) const ;
              int operation_sub ( const TextPropertyGroup::Entry& entry ) const ;
              ASCString toString ( ) const;
            public:
               TagIntProperty ( int& property_, int tagNum_, const char** tags_, bool inverted_  ) : PTTI ( property_ ), tagNum (tagNum_), tags ( tags_ ), inverted ( inverted_ ) {};
               TagIntProperty ( int& property_, int tagNum_, const char** tags_, int defaultValue_, bool inverted_  ) : PTTI ( property_, defaultValue_ ), tagNum (tagNum_), tags ( tags_ ), inverted ( inverted_ ) {};
         };

         typedef PropertyTemplate<int> PTNI;
         class NamedIntProperty : public PTNI {
              int tagNum;
              const char** tags;
            protected:
              int operation_eq ( const TextPropertyGroup::Entry& entry ) const ;
              ASCString toString ( ) const;
            public:
               NamedIntProperty ( int& property_, int tagNum_, const char** tags_ ) : PTNI ( property_ ), tagNum (tagNum_), tags ( tags_ ) {};
               NamedIntProperty ( int& property_, int tagNum_, const char** tags_, int defaultValue_ ) : PTNI ( property_, defaultValue_ ), tagNum (tagNum_), tags ( tags_ ) {};
         };

/*
         typedef PropertyTemplate<void*> PTIMG;
         class ImageProperty : public PTIMG {
               ASCString fileName;
            protected:
               void* operation_eq ( const TextPropertyGroup::Entry& entry ) const ;
               ASCString toString ( ) const;
            public:
               ImageProperty ( void* &property_, const ASCString& fileName_ ) : PTIMG ( property_ ), fileName ( fileName_ ) {};
         };
*/
         typedef PropertyTemplate<Surface> PTIMG2;
         class ASCImageProperty : public PTIMG2 {
               typedef Surface PropertyType;
               ASCString fileName;
            protected:
               PropertyType operation_eq ( const TextPropertyGroup::Entry& entry ) const ;
               ASCString toString ( ) const;
            public:
               ASCImageProperty ( Surface &property_, const ASCString& fileName_ ) : PTIMG2 ( property_ ), fileName ( fileName_ ) {};
         };

         typedef PropertyTemplate< vector<void*> > PTIMGA;
         class ImageArrayProperty : public PTIMGA {
               typedef vector<void*> PropertyType;
               ASCString fileName;
            protected:
               PropertyType operation_eq ( const TextPropertyGroup::Entry& entry ) const ;
               ASCString toString ( ) const;
            public:
               ImageArrayProperty ( PropertyType &property_, const ASCString& fileName_ ) : PTIMGA ( property_ ), fileName ( fileName_ ) {};
         };

         typedef PropertyTemplate< vector<Surface> > PTIMGA2;
         class ASCImageArrayProperty : public PTIMGA2 {
               typedef vector<Surface> PropertyType;
               ASCString fileName;
            protected:
               PropertyType operation_eq ( const TextPropertyGroup::Entry& entry ) const ;
               ASCString toString ( ) const;
            public:
               ASCImageArrayProperty ( PropertyType &property_, const ASCString& fileName_ ) : PTIMGA2 ( property_ ), fileName ( fileName_ ) {};
         };



/*
void PropertyContainer :: run ( )
{
   if ( isReading() )
      for ( Properties::iterator i = properties.begin(); i != properties.end(); i++ )
         if ( !(*i)->evaluated )
            (*i)->evaluate();

   closeBracket();
   if ( levelDepth )
      error ( "PropertyReadingContainer :: ~PropertyReadingContainer - still brackets open" );
}
*/

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

   size_t indent = output.length();
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


ASCString PropertyContainer :: getNameStack()
{
   ASCString s;
   for ( Level::iterator i = level.begin(); i != level.end(); ++i ) {
      if ( s.length() )
         s += ".";
      s += *i;
   }
   return s;
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

   p->evaluate();
}


void PropertyContainer::addInteger ( const ASCString& name, int& property )
{
   IntegerProperty* ip = new IntegerProperty ( property );
   setup ( ip, name );
}

void PropertyContainer::addInteger ( const ASCString& name, int& property, int defaultValue )
{
   IntegerProperty* ip = new IntegerProperty ( property, defaultValue );
   setup ( ip, name );
}

void PropertyContainer::addDFloat ( const ASCString& name, double& property )
{
   FloatProperty* ip = new FloatProperty ( property );
   setup ( ip, name );
}

void PropertyContainer::addDFloat ( const ASCString& name, double& property, double defaultValue )
{
   FloatProperty* ip = new FloatProperty ( property, defaultValue );
   setup ( ip, name );
}



void PropertyContainer::addBool ( const ASCString& name, bool& property )
{
   BoolProperty* ip = new BoolProperty ( property );
   setup ( ip, name );
}

void PropertyContainer::addBool ( const ASCString& name, bool& property, bool defaultValue )
{
   BoolProperty* ip = new BoolProperty ( property, defaultValue );
   setup ( ip, name );
}

void PropertyContainer::addString ( const ASCString& name, ASCString& property )
{
   StringProperty* ip = new StringProperty ( property );
   setup ( ip, name );
}

void PropertyContainer::addString ( const ASCString& name, ASCString& property, const ASCString& defaultValue )
{
   StringProperty* ip = new StringProperty ( property, defaultValue );
   setup ( ip, name );
}


void PropertyContainer::addStringArray ( const ASCString& name, vector<ASCString>& property )
{
   StringArrayProperty* ip = new StringArrayProperty ( property );
   setup ( ip, name );
}


void PropertyContainer::addIntegerArray ( const ASCString& name, vector<int>& property )
{
   IntegerArrayProperty* ip = new IntegerArrayProperty ( property );
   setup ( ip, name );
}

void PropertyContainer::addDFloatArray ( const ASCString& name, vector<double>& property )
{
   DoubleArrayProperty* dp = new DoubleArrayProperty ( property );
   setup ( dp, name );
}

void PropertyContainer::addDFloatArray ( const ASCString& name, vector<int>& property )
{
   vector<double> dproperty;
   for ( vector<int>::iterator i = property.begin(); i != property.end(); i++ )
       dproperty.push_back (*i);
   DoubleArrayProperty* dp = new DoubleArrayProperty ( dproperty );
   setup ( dp, name );

   property.clear();
   for ( vector<double>::iterator i = dproperty.begin(); i != dproperty.end(); i++ )
       property.push_back ( int(*i) );
}


void PropertyContainer::addIntRangeArray ( const ASCString& name, vector<IntRange>& property, bool required )
{
   IntRangeArrayProperty* ip = new IntRangeArrayProperty ( property, required );
   setup ( ip, name );
}


void PropertyContainer::addTagArray ( const ASCString& name, BitSet& property, int tagNum, const char** tags, bool inverted )
{
   TagArrayProperty* ip = new TagArrayProperty ( property, tagNum, tags, inverted );
   setup ( ip, name );
}

void PropertyContainer::addTagInteger ( const ASCString& name, int& property, int tagNum, const char** tags, bool inverted )
{
   TagIntProperty* ip = new TagIntProperty ( property, tagNum, tags, inverted );
   setup ( ip, name );
}

void PropertyContainer::addTagInteger ( const ASCString& name, int& property, int tagNum, const char** tags, int defaultValue, bool inverted )
{
   TagIntProperty* ip = new TagIntProperty ( property, tagNum, tags, defaultValue, inverted );
   setup ( ip, name );
}

void PropertyContainer::addNamedInteger ( const ASCString& name, int& property, int tagNum, const char** tags )
{
   NamedIntProperty* ip = new NamedIntProperty ( property, tagNum, tags );
   setup ( ip, name );
}

void PropertyContainer::addNamedInteger ( const ASCString& name, int& property, int tagNum, const char** tags, int defaultValue )
{
   NamedIntProperty* ip = new NamedIntProperty ( property, tagNum, tags, defaultValue );
   setup ( ip, name );
}

void PropertyContainer::addBreakpoint ()
{
   bool breakpoint = false;
   if ( isReading() ) {
      addBool( "breakpoint", breakpoint, false);
      if ( breakpoint ) {
        #ifdef WIN32
        DebugBreak();
        #else
        cerr << "breakpoint hit";
        #endif
      }
   }
}


void PropertyContainer::storeContext( const ASCString& label )
{
   storedContext[label] = make_pair( levelDepth, level );
}

bool PropertyContainer::restoreContext( const ASCString& label )
{
   StoredContext::iterator pos = storedContext.find( label );
   if ( pos != storedContext.end() ) {
      levelDepth = pos->second.first;
      level = pos->second.second;
      return true;
   }
   return false;
}


#ifdef ParserLoadImages
#if 0
void PropertyContainer::addImageArray ( const ASCString& name, vector<void*> &property, const ASCString& filename )
{
   ImageArrayProperty* ip = new ImageArrayProperty ( property, filename );
   setup ( ip, name );
}
#endif

void PropertyContainer::addImageArray ( const ASCString& name, vector<Surface> &property, const ASCString& filename )
{
   ASCImageArrayProperty* ip = new ASCImageArrayProperty ( property, filename );
   setup ( ip, name );
}

#if 0
void PropertyContainer::addImage ( const ASCString& name, void* &property, const ASCString& filename )
{
   ImageProperty* ip = new ImageProperty ( property, filename );
   setup ( ip, name );
}
#endif

void PropertyContainer::addImage ( const ASCString& name, Surface &property, const ASCString& filename )
{
   ASCImageProperty* ip = new ASCImageProperty ( property, filename );
   setup ( ip, name );
}


#endif

void PropertyContainer::warning ( const ASCString& errmsg )
{
   #ifdef converter
   fatalError ( errmsg );
   #else
   ::warning( "file " + textPropertyGroup->fileName+ ": " + errmsg );
   #endif
}

void PropertyContainer::error ( const ASCString& errmsg )
{
   displayLogMessage ( 0, getLocation() + " : " + errmsg  + "\n" );
   throw ParsingError ( getLocation() + " : " + errmsg );
}

bool PropertyContainer::find ( const ASCString& name )
{
   ASCString n;
   for ( Level::iterator i = level.begin(); i != level.end(); i++ )
      n += *i + ".";
   n += name;
   n.toLower();

   return textPropertyGroup->find ( n ) != NULL;
}



PropertyReadingContainer :: PropertyReadingContainer ( const ASCString& baseName, TextPropertyGroup* tpg ) : PropertyContainer ( baseName, tpg, true )
{
   openBracket ( baseName );
}


PropertyReadingContainer :: ~PropertyReadingContainer (  )
{
}

PropertyWritingContainer :: PropertyWritingContainer ( const ASCString& baseName, tnstream& stream ) : PropertyContainer ( baseName, NULL, false ), stream ( stream )
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

   name.toLower();

   if (!entry )
      entry = propertyContainer->textPropertyGroup->find ( name );

   if ( !entry && !hasDefault() )
      propertyContainer->error ( "entry " + name +" not found" );
}

void PropertyContainer::Property::writeProperty( )
{
   propertyContainer->writeProperty ( *this, toString() );
}


template <class T>
T PropertyTemplate<T>::parse ( const TextPropertyGroup::Entry& entry ) const
{
   if ( entry.op == TextPropertyGroup::Entry::eq )
      return operation_eq( entry );

   if ( entry.op == TextPropertyGroup::Entry::alias_all )
      operation_not_defined( entry );

   if ( !entry.parent )
      propertyContainer->error ( ASCString("PropertyContainer::PropertyTemplate::parse - no parent for operator ") + TextFormatParser::operations[entry.op] + " at entry " + entry.propertyName + " !");

   switch ( entry.op ) {
      case TextPropertyGroup::Entry::mult_eq : return operation_mult ( entry );
      case TextPropertyGroup::Entry::add_eq :  return operation_add ( entry );
      case TextPropertyGroup::Entry::sub_eq :  return operation_sub ( entry );
      default:;
   }

   propertyContainer->error ( "PropertyTemplate::parse - invalid operator !");
   return defaultValue;
}

template <class T>
void PropertyTemplate<T>::operation_not_defined(const TextPropertyGroup::Entry& entry) const
{
   propertyContainer->error ( ASCString("operator ") + TextFormatParser::operations[entry.op] + " not defined for this type !\nEntry " + entry.propertyName);
}


template <class T>
T PropertyTemplate<T>::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
   operation_not_defined( entry );
   return T();
}

template <class T>
T PropertyTemplate<T>::operation_mult ( const TextPropertyGroup::Entry& entry ) const
{
   operation_not_defined( entry );
   return T();
}

template <class T>
T PropertyTemplate<T>::operation_add ( const TextPropertyGroup::Entry& entry ) const
{
   operation_not_defined( entry );
   return T();
}

template <class T>
T PropertyTemplate<T>::operation_sub ( const TextPropertyGroup::Entry& entry ) const
{
   operation_not_defined( entry );
   return T();
}


template <class T>
void PropertyTemplate<T>::evaluate ()
{
   if ( evaluated )
      return;

   if ( propertyContainer->isReading() ) {
      findEntry();

      if ( entry ) {
         property = parse ( *entry );
      } else
         property = defaultValue;

      evaluated = true;
   } else {
      writeProperty();
      evaluated = true;
   }
}

int IntegerProperty::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
   char* p = NULL;

   ASCString value = entry.value;
   ASCString::size_type i;
   while ( (i = value.find_first_of( " \t\n\r" )) != ASCString::npos )
      value.erase( i, 1 );

   while ( value.find( "0") == 0 && value.find( "0x") != 0)  // removing leading zeroes
      value.erase(0,1);


   int res =  strtol ( value.c_str(), &p, 0  );    //   strtol(nptr, NULL, 10);
   if ( *p != 0 && *p != ';' ) {
      ASCString s = name + ": value "+ entry.value +" is no numerical value \n" ;
      // propertyContainer->error ( s );
      fprintf(stderr, s.c_str()  );
   }
   
   return res;
}


int IntegerProperty::operation_add ( const TextPropertyGroup::Entry& entry ) const
{
   return parse ( *entry.parent ) + operation_eq ( entry );
}

int IntegerProperty::operation_sub ( const TextPropertyGroup::Entry& entry ) const
{
   return parse ( *entry.parent ) - operation_eq ( entry );
}


int IntegerProperty::operation_mult ( const TextPropertyGroup::Entry& entry ) const
{
   return int ( double ( parse ( *entry.parent )) *  atof ( entry.value.c_str() ));
}


ASCString IntegerProperty::toString ( ) const
{
   return strrr ( property );
}



double FloatProperty::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
   return atof ( entry.value.c_str() );    //   strtol(nptr, NULL, 10);
}


double FloatProperty::operation_add ( const TextPropertyGroup::Entry& entry ) const
{
   return parse ( *entry.parent ) + operation_eq ( entry );
}

double FloatProperty::operation_mult ( const TextPropertyGroup::Entry& entry ) const
{
   return double ( parse ( *entry.parent )) *  atof ( entry.value.c_str() );
}


ASCString FloatProperty::toString ( ) const
{
   ASCString s;
   s.format("%f", property );
   return s;
}


bool BoolProperty::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
    StringTokenizer st ( entry.value );
    ASCString s = st.getNextToken();
    if ( s.compare_ci ( "true" )==0 || s.compare_ci ( "1" )==0 )
       return true;
    else
       if ( s.compare_ci ( "false" )==0 || s.compare_ci ( "0" )==0 )
          return false;
       else {
          propertyContainer->error ( name + ": token "+ s +" unknown" );
          return false;
       }
}


ASCString BoolProperty::toString ( ) const
{
   if ( property )
      return "true";
   else
      return "false";
}



ASCString StringProperty::operation_add ( const TextPropertyGroup::Entry& entry ) const
{
/*   ASCString s = entry.value;
   while ( s.find ( "%s" ) != ASCString::npos )
      s.replace ( s.find ( "%s" ), 2, parse ( *entry.parent ));
   return s;
   */
   return parse( *entry.parent ) + entry.value;
}

ASCString StringProperty::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
    ASCString s = entry.value;
    ASCString::size_type pos = s.find_first_not_of ( TextFormatParser::whiteSpace );
    if ( pos == ASCString::npos )
       s.erase();                            
    else
       s.erase ( 0, pos );

    pos = s.find_last_not_of ( TextFormatParser::whiteSpace );
    if ( pos != ASCString::npos )
       s.erase ( pos+1 );

    return s;
}


ASCString StringProperty::toString ( ) const
{
   ASCString valueToWrite = property ;

   ASCString::size_type pos = 0;
   static const int linewidth = 60;
   do {
      if ( pos + linewidth < valueToWrite.length() ) {
         pos = valueToWrite.find_first_of ( TextFormatParser::whiteSpace, pos + linewidth );
         if ( pos != ASCString::npos )
            valueToWrite[pos] = '\n';
      } else
         pos = ASCString::npos;
   } while ( pos != ASCString::npos );

   return valueToWrite;
}


StringArrayProperty::PropertyType StringArrayProperty::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
   PropertyType sa;
   StringTokenizer st ( entry.value, true );
   ASCString s = st.getNextToken();
   while ( !s.empty() ) {
      sa.push_back ( s );
      s = st.getNextToken();
   }
   return sa;
}

ASCString StringArrayProperty::toString ( ) const
{
   ASCString valueToWrite;
   for ( PropertyType::iterator i = property.begin(); i != property.end(); i++ ) {
      valueToWrite += *i;
      valueToWrite += " ";
   }
   return valueToWrite;
}


template<class T>
typename ValArrayProperty<T>::PropertyType ValArrayProperty<T>::operation_add ( const TextPropertyGroup::Entry& entry ) const
{
   PropertyType child = this->operation_eq( entry );
   PropertyType parent = this->parse ( *entry.parent );

   if ( child.size() == parent.size() ) {
      PropertyType res;
      for ( int i = 0; i < child.size(); i++ )
         res.push_back ( parent[i] + child[i] );
      return res;
   }
   if ( child.size() == 1 ) {
      PropertyType res;
      for ( int i = 0; i < parent.size(); i++ )
         res.push_back ( parent[i] + child[0] );
      return res;
   }
   if ( parent.size() == 1 ) {
      PropertyType res;
      for ( int i = 0; i < child.size(); i++ )
         res.push_back ( parent[0] + child[i] );
      return res;
   }

   this->propertyContainer->error ( this->name + ": array dimensions do not agree" );
   return child;
}

template<class T>
typename ValArrayProperty<T>::PropertyType ValArrayProperty<T>::operation_sub ( const TextPropertyGroup::Entry& entry ) const
{
   PropertyType child = this->operation_eq( entry );
   PropertyType parent = this->parse ( *entry.parent );

   if ( child.size() == parent.size() ) {
      PropertyType res;
      for ( int i = 0; i < child.size(); i++ )
         res.push_back ( parent[i] - child[i] );
      return res;
   }
   if ( child.size() == 1 ) {
      PropertyType res;
      for ( int i = 0; i < parent.size(); i++ )
         res.push_back ( parent[i] - child[0] );
      return res;
   }
   if ( parent.size() == 1 ) {
      PropertyType res;
      for ( int i = 0; i < child.size(); i++ )
         res.push_back ( parent[0] - child[i] );
      return res;
   }

   this->propertyContainer->error ( this->name + ": array dimensions do not agree" );
   return child;
}


template<class T>
typename ValArrayProperty<T>::PropertyType ValArrayProperty<T>::operation_mult ( const TextPropertyGroup::Entry& entry ) const
{
   PropertyType child = this->operation_eq( entry );
   PropertyType parent = this->parse ( *entry.parent );

   if ( child.size() == parent.size() ) {
      PropertyType res;
      for ( int i = 0; i < child.size(); i++ )
         res.push_back ( int( double(parent[i]) * child[i]) );
      return res;
   }
   if ( child.size() == 1 ) {
      PropertyType res;
      for ( int i = 0; i < parent.size(); i++ )
         res.push_back ( int( double(parent[i]) * child[0]) );
      return res;
   }
   if ( parent.size() == 1 ) {
      PropertyType res;
      for ( int i = 0; i < child.size(); i++ )
         res.push_back ( int( double(parent[0]) * child[i]) );
      return res;
   }

   this->propertyContainer->error ( this->name + ": array dimensions do not agree" );
   return parent;
}


IntegerArrayProperty::PropertyType IntegerArrayProperty::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
   PropertyType ia;
   StringTokenizer st ( entry.value, true );
   ASCString s = st.getNextToken();
   while ( !s.empty() ) {
      ia.push_back ( atoi ( s.c_str() ));
      s = st.getNextToken();
   }
   return ia;
}



ASCString DoubleArrayProperty::toString ( ) const
{
   ASCString valueToWrite;
   for ( PropertyType::iterator i = property.begin(); i != property.end(); i++ ) {
      ASCString s;
      s.format ( "%f", *i );
      valueToWrite += s;
      valueToWrite += " ";
   }
   return valueToWrite;
}


DoubleArrayProperty::PropertyType DoubleArrayProperty::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
   PropertyType ia;
   StringTokenizer st ( entry.value, true );
   ASCString s = st.getNextToken();
   while ( !s.empty() ) {
      ia.push_back ( atof ( s.c_str() ));
      s = st.getNextToken();
   }
   return ia;
}



ASCString IntegerArrayProperty::toString ( ) const
{
   ASCString valueToWrite;
   for ( PropertyType::iterator i = property.begin(); i != property.end(); i++ ) {
      valueToWrite += strrr ( *i );
      valueToWrite += " ";
   }
   return valueToWrite;
}




IntRangeArrayProperty::PropertyType IntRangeArrayProperty::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
   return String2IntRangeVector( entry.value );
}

ASCString IntRangeArrayProperty::toString() const
{
   ASCString valueToWrite;
   for ( PropertyType::iterator i = property.begin(); i != property.end(); i++ ) {
      if ( i->from != i->to ) {
         valueToWrite += strrr ( i->from );
         valueToWrite += "-";
         valueToWrite += strrr ( i->to );
      } else
         valueToWrite += strrr ( i->from );
      valueToWrite += " ";
   }
   return valueToWrite;
}


BitSet TagArrayProperty::operation_add ( const TextPropertyGroup::Entry& entry ) const
{
   return parse ( *entry.parent ) | operation_eq ( entry );
}

BitSet TagArrayProperty::operation_sub ( const TextPropertyGroup::Entry& entry ) const
{
   return parse ( *entry.parent ) & ~operation_eq ( entry );
}


BitSet TagArrayProperty::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
   BitSet bs;

   if ( inverted )
      bs.set();
   else
      bs.reset();
   StringTokenizer st ( entry.value );
   ASCString s = st.getNextToken();
   while ( !s.empty() ) {
      bool found = false;
      for ( int i = 0; i < tagNum; i++ )
         if ( s.compare_ci ( tags[i] )==0  ) {
            if ( inverted )
               bs.reset ( i );
            else
               bs.set ( i );
            found = true;
            break;
         }

      if ( !found )
         propertyContainer->error ( name + ": token "+ s +" unknown" );
      s = st.getNextToken();
   }

   return bs;
}

ASCString TagArrayProperty::toString() const
{
   ASCString valueToWrite;
   for ( int i = 0; i < tagNum; i++ )
      if ( property.test(i) != inverted ) {
         valueToWrite += tags[i];
         valueToWrite += " ";
      }
   return valueToWrite;
}


int TagIntProperty::operation_add ( const TextPropertyGroup::Entry& entry ) const
{
   return parse ( *entry.parent ) | operation_eq ( entry );
}

int TagIntProperty::operation_sub ( const TextPropertyGroup::Entry& entry ) const
{
   return parse ( *entry.parent ) & ~operation_eq ( entry );
}


int TagIntProperty::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
   int i;

   if ( inverted )
      i = -1;
   else
      i = 0;

   StringTokenizer st ( entry.value );
   ASCString s = st.getNextToken();
   while ( !s.empty() ) {
      bool found = false;
      for ( int j = 0; j < tagNum; j++ )
         if ( s.compare_ci ( tags[j] )==0  ) {
            i ^= 1 << j;
            found = true;
            break;
         }

      if ( !found )
         propertyContainer->error ( name + ": token "+ s +" unknown" );
      s = st.getNextToken();
   }
   return i;
}


ASCString TagIntProperty::toString() const
{
   ASCString valueToWrite;
   for ( int i = 0; i < tagNum; i++ )
      if ( !!(property & (1 << i)) != inverted ) {
         valueToWrite += tags[i];
         valueToWrite += " ";
      }
   return valueToWrite;
}


int NamedIntProperty::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
   int i;

   StringTokenizer st ( entry.value );
   ASCString s = st.getNextToken();
   if ( !s.empty() ) {
      bool found = false;
      for ( int j = 0; j < tagNum; j++ )
         if ( s.compare_ci ( tags[j] )==0  ) {
            i = j;
            found = true;
            break;
         }

      if ( !found )
         propertyContainer->error ( name + ": token "+ s +" unknown" );
   }

   return i;
}

ASCString NamedIntProperty::toString() const
{
   return tags[property];
}

#ifdef ParserLoadImages
void* getRawFieldMask()
{
   static void* mask = NULL;
   if ( !mask ) {
      int i ;
      tnfilestream s ( "largehex.raw", tnstream::reading );
      s.readrlepict ( &mask, false, & i );
   }
   return mask;
}

#if 0
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

   ASCString lowerFile = copytoLower( file );
   
   int pcxwidth;
   int pcxheight;
   int depth = pcxGetColorDepth ( lowerFile, &pcxwidth, &pcxheight );
   if ( depth > 8 ) {
      tvirtualdisplay vdp ( xsize, (num/10+1)*100, TCalpha, 32 );
      if ( num == 1 )
         loadpcxxy ( lowerFile, 0, 0, 0, &imgwidth, &imgheight );
      else
         loadpcxxy ( lowerFile, 0, 0, 0 );

      for ( int i = 0; i < num; i++ ) {
          int x1 = (i % 10) * 100;
          int y1 = (i / 10) * 100;
          TrueColorImage* tci = getimage ( x1, y1, x1 + imgwidth-1, y1 + imgheight-1 );

          tvirtualdisplay vdp ( 100, 100, 255 );
          void* img = convertimage ( tci, pal );
          putimage ( 0, 0, img );
          putmask ( 0, 0, getRawFieldMask(), 0 );
          getimage ( 0, 0, imgwidth-1, imgheight-1, img );
          images.push_back ( img );
      }
   } else {
      tvirtualdisplay vdp ( max(xsize, pcxwidth), max( (num/10+1)*100, pcxheight), 255, 8 );

      if ( num == 1 )
         loadpcxxy ( lowerFile, 0, 0, 0, &imgwidth, &imgheight );
      else
         loadpcxxy ( lowerFile, 0, 0, 0 );

      for ( int i = 0; i < num; i++ ) {
          int x1 = (i % 10) * 100;
          int y1 = (i / 10) * 100;
          if ( num > 1 )
             putmask ( x1, y1, getRawFieldMask(), 0 );
          void* img = new char[imagesize (0, 0, imgheight-1, imgwidth-1)];
          getimage ( x1, y1, x1+imgwidth-1, y1+imgheight-1, img );
          images.push_back ( img );
      }
   }

   return images;
}
#endif

vector<Surface> loadASCImage ( const ASCString& file, int num )
{
   vector<Surface> images;

   int xsize;
   if ( num <= 10)
      xsize = (num+1)* 100;
   else
      xsize = 1100;

   
   tnfilestream fs ( file, tnstream::reading );
   
   Surface s ( IMG_Load_RW ( SDL_RWFromStream( &fs ), 1));
   assert( s.valid());
   
   if ( s.GetPixelFormat().BitsPerPixel() == 8 )
      s.assignDefaultPalette();

      
   int depth = s.GetPixelFormat().BitsPerPixel();
   
   for ( int i = 0; i < num; i++ ) {
       int x1 = (i % 10) * 100;
       int y1 = (i / 10) * 100;
       if ( depth > 8 && depth < 32 )
          depth = 32;
          
       Surface s2 = Surface::createSurface(fieldsizex,fieldsizey, depth );
       
       if ( s2.GetPixelFormat().BitsPerPixel() != 8 || s.GetPixelFormat().BitsPerPixel() != 8 ) {
          if ( s.GetPixelFormat().BitsPerPixel() == 32 ) {
            MegaBlitter<4,4,ColorTransform_None,ColorMerger_PlainOverwrite,SourcePixelSelector_Rectangle > blitter;
            blitter.setSrcRectangle(SDLmm::SRect(SPoint(x1,y1),fieldsizex,fieldsizey));
            blitter.blit( s, s2, SPoint(0,0)  );
          } else {
            s2.Blit( s, SDLmm::SRect(SPoint(x1,y1),fieldsizex,fieldsizey), SPoint(0,0));
          }
          applyLegacyFieldMask(s2);
       } else {
          // we don't want any transformations from one palette to another; we just assume that all 8-Bit images use the same colorspace
          MegaBlitter<1,1,ColorTransform_None,ColorMerger_AlphaOverwrite,SourcePixelSelector_Rectangle > blitter;
          blitter.setSrcRectangle(SDLmm::SRect(SPoint(x1,y1),fieldsizex,fieldsizey));
          blitter.blit( s, s2, SPoint(0,0)  );
          applyFieldMask(s2);
       }
       s2.detectColorKey();
       images.push_back ( s2 );
   }
   return images;
}

#if 0
void* ImageProperty::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
   void* img;

   ASCString lstring = copytoLower( entry.value );
   
   try {
      StringTokenizer st ( lstring, fileNameDelimitter );
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
                  propertyContainer->warning ( "ImageProperty::operation_eq - couldn't blit surface "+fn);
            }

            fn = st.getNextToken();
         } while ( !fn.empty() );
         if ( s )
            img = convertSurface ( *s );
         else
            img = NULL;
      } else
         if ( fn.suffix() == "pcx" ) {
            return loadImage ( fn, 1 )[0];
         }
   }
   catch ( ASCexception ){
      propertyContainer->error( "error accessing file " + lstring );
      return NULL;
   }
   return img;
}


ASCString ImageProperty::toString() const
{
   int width, height;
   getpicsize( property, width, height) ;
   tvirtualdisplay vdp ( width+10, height+10, 255, 8 );
   putimage ( 0, 0, property );
   ASCString valueToWrite = extractFileName_withoutSuffix(fileName) + ".pcx";
   writepcx ( valueToWrite, 0, 0, width-1, height-1, pal );
   return valueToWrite;
}
#endif


ASCImageProperty::PropertyType ASCImageProperty::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
   try {
      StringTokenizer st ( entry.value, fileNameDelimitter );
      FileName fn = st.getNextToken();
      fn.toLower();
      if ( fn.suffix() == "png" ) {
         SDLmm::Surface* s = NULL;
         do {
            tnfilestream fs ( fn, tnstream::reading );
            RWOPS_Handler rwo( SDL_RWFromStream( &fs ) );
            SDLmm::Surface s2 ( IMG_LoadPNG_RW ( rwo.Get() ));
            rwo.Close();
            // s2.SetAlpha ( SDL_SRCALPHA, SDL_ALPHA_OPAQUE );
            if ( !s )
               s = new SDLmm::Surface ( s2 );
            else {
               int res = s->Blit ( s2 );
               if ( res < 0 )
                  propertyContainer->warning ( "ImageProperty::operation_eq - couldn't blit surface "+fn);
            }

            fn = st.getNextToken();
         } while ( !fn.empty() );
         if ( s )  {
            Surface s3( *s );
            delete s;
            return s3;
         } else
            return Surface();

      } else
         if ( fn.suffix() == "pcx" ) {
            tnfilestream fs ( fn, tnstream::reading );

            RWOPS_Handler rwo ( SDL_RWFromStream( &fs ));
            SDL_Surface* surface = IMG_LoadPCX_RW ( rwo.Get() );
            rwo.Close();

            if ( !surface )
               propertyContainer->error( "error loading file " + fn );
               
            Surface s ( surface );
            if ( s.GetPixelFormat().BitsPerPixel() == 8)
               s.SetColorKey( SDL_SRCCOLORKEY, 255 );
            else 
               s.SetColorKey( SDL_SRCCOLORKEY, 0xfefefe );
            s.SetAlpha(0,255);
            return s;
         }
   }
   catch ( ASCexception ){
      propertyContainer->error( "error accessing file " + entry.value );
   }
   return Surface();
}

ASCString ASCImageProperty::toString() const
{
   warning( "writing of Images not supported yet");
   return "";
   /*
   int width, height;
   getpicsize( property, width, height) ;
   tvirtualdisplay vdp ( width+10, height+10, 255, 8 );
   putimage ( 0, 0, property );
   ASCString valueToWrite = extractFileName_withoutSuffix(fileName) + ".pcx";
   writepcx ( valueToWrite, 0, 0, width-1, height-1, pal );
   return valueToWrite;
   */
}


#if 0
ImageArrayProperty::PropertyType ImageArrayProperty::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
   try {
      StringTokenizer st ( entry.value, fileNameDelimitter );
      ASCString imgName = st.getNextToken();
      ASCString imgNumS = st.getNextToken();
      if ( imgNumS.empty() )
         propertyContainer->error( name + ": image number missing" );
      int imgNum = atoi ( imgNumS.c_str() );
      return loadImage ( imgName, imgNum );
   }
   catch ( ASCexception ){
      propertyContainer->error( "error accessing file " + entry.value );
   }
   return PropertyType();
}


ASCString ImageArrayProperty::toString() const
{
   size_t num = property.size();
   tvirtualdisplay vdp ( 1100, 100 * (num / 10 + 1), 255, 8 );
   int cnt = 0;
   for ( PropertyType::iterator i = property.begin(); i != property.end(); i++ ) {
      putimage ( (cnt % 10) * 100, (cnt / 10) * 100, *i );
      cnt++;
   }
   ASCString valueToWrite = extractFileName_withoutSuffix(fileName) + ".pcx" + " " + strrr( cnt );
   writepcx ( extractFileName_withoutSuffix(fileName) + ".pcx", 0, 0, 1100 - 1, 100 * (num / 10 + 1) - 1, pal );
   return valueToWrite;
}
#endif

ASCImageArrayProperty::PropertyType ASCImageArrayProperty::operation_eq ( const TextPropertyGroup::Entry& entry ) const
{
   try {
      StringTokenizer st ( entry.value, fileNameDelimitter );
      ASCString imgName = st.getNextToken();
      ASCString imgNumS = st.getNextToken();
      if ( imgNumS.empty() )
         propertyContainer->error( name + ": image number missing" );
      int imgNum = atoi ( imgNumS.c_str() );
      return loadASCImage ( imgName, imgNum );
   }
   catch ( ASCexception ){
      propertyContainer->error( "error accessing file " + entry.value );
   }
   return PropertyType();
}


ASCString ASCImageArrayProperty::toString() const
{
   warning( "writing of Images not supported yet");
   return "";
   /*
   int num = property.size();
   tvirtualdisplay vdp ( 1100, 100 * (num / 10 + 1), 255, 8 );
   int cnt = 0;
   for ( PropertyType::iterator i = property.begin(); i != property.end(); i++ ) {
      putimage ( (cnt % 10) * 100, (cnt / 10) * 100, *i );
      cnt++;
   }
   ASCString valueToWrite = extractFileName_withoutSuffix(fileName) + ".pcx" + " " + strrr( cnt );
   writepcx ( extractFileName_withoutSuffix(fileName) + ".pcx", 0, 0, 1100 - 1, 100 * (num / 10 + 1) - 1, pal );
   return valueToWrite;
   */
}


#endif


