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

#ifndef textfileparser_h_included
#define textfileparser_h_included

#include <vector>
#include "ascstring.h"
#include "errors.h"
#include "typen.h"


class ParsingError : public ASCmsgException {
    public:
       ParsingError ( const ASCString& msg ) : ASCmsgException ( msg ) {};
};

class TextPropertyGroup;
typedef PointerList<TextPropertyGroup*> TextPropertyList;

/** Class that stores all the (preparsed) entries of an .ASCTXT file. 
    The entries consist of a PropertyName, an operator and a value, but don't have any type information 
*/
class TextPropertyGroup {
         bool inheritanceBuild;
      public:
          TextPropertyGroup() : inheritanceBuild ( false ) {};
          
          class Entry {
            public:
               ASCString propertyName;
               enum Operator { eq, mult_eq } op;
               ASCString value;
               Entry ( const ASCString& propertyName_, Operator op_, const ASCString& value_ ) : propertyName ( propertyName_ ), op ( op_ ), value ( value_ ) { propertyName.toLower(); };
         };

         typedef list<Entry> Entries;
         Entries entries;

         ASCString fileName;
         ASCString location;

         //! the name of the structure. For example "VehicleType"
         ASCString typeName;

         void buildInheritance( TextPropertyList& tpl );
         Entries::iterator find( const ASCString& n );
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
     protected:
        void startLevel ( const ASCString& levelName );
        void parseLine ( const ASCString& line );
        void error ( const ASCString& errmsg );
};

class PropertyContainer {
         bool reading;
         // ASCString filename;
      protected:
         int levelDepth;
         typedef list<ASCString> Level;
         Level level;

         TextPropertyGroup* textPropertyGroup;
      public:


         class Property {
            protected:
               ASCString name;
               ASCString lastName;
               PropertyContainer* propertyContainer;
               TextPropertyGroup::Entry* entry;
               virtual void evaluate_rw ( ) = 0;
               ASCString valueToWrite;
               virtual bool hasDefault() { return false; };
            public:
               Property () : propertyContainer ( NULL ), entry ( NULL ), evaluated(false) {};
               void evaluate ( );
               const ASCString& getName() { return name; };
               const ASCString& getLastName() { return lastName; };
               void setName ( const ASCString& name_, const ASCString& lastName_ );
               void setPropertyContainer ( PropertyContainer* propertyContainer_ ) {  propertyContainer = propertyContainer_; };
               virtual ~Property() {};
               void findEntry ();
               bool evaluated;
         };
         friend class Property;

         template <class T> class PropertyTemplate : public Property {
            protected:
               T& property;
               T defaultValue;
               bool defaultValueAvail;
               virtual bool hasDefault() { return defaultValueAvail; };
            public:
               PropertyTemplate ( T& property_ ) : property ( property_ ), defaultValueAvail ( false ) {};
               PropertyTemplate ( T& property_, const T& defaultValue_ ) : property ( property_ ), defaultValueAvail ( true ), defaultValue ( defaultValue_) {};
         };

         class IntProperty : public PropertyTemplate<int> {
            protected:
              void evaluate_rw ( );
            public:
               IntProperty ( int& property_ ) : PropertyTemplate<int> ( property_ ) {};
               IntProperty ( int& property_, int defaultValue_ ) : PropertyTemplate<int> ( property_, defaultValue_ ) {};
         };

         class BoolProperty : public Property {
              bool& property;
            protected:
              void evaluate_rw ( );
            public:
               BoolProperty ( bool& property_ ) : property ( property_ ) {};
         };

         class StringProperty : public Property {
              ASCString& property;
            protected:
              void evaluate_rw ( );
            public:
               StringProperty ( ASCString& property_ ) : property ( property_ ) {};
         };
         class StringArrayProperty : public Property {
              typedef vector<ASCString> PropertyType;
              PropertyType& property;
            protected:
              void evaluate_rw ( );
            public:
               StringArrayProperty ( vector<ASCString>& property_ ) : property ( property_ ) {};
         };
         class IntegerArrayProperty : public Property {
              typedef vector<int> PropertyType;
              PropertyType& property;
            protected:
              void evaluate_rw ( );
            public:
               IntegerArrayProperty ( vector<int>& property_ ) : property ( property_ ) {};
         };
         class IntRangeArrayProperty : public Property {
              typedef vector<IntRange> PropertyType;
              PropertyType& property;
            protected:
              void evaluate_rw ( );
            public:
               IntRangeArrayProperty ( vector<IntRange>& property_ ) : property ( property_ ) {};
         };
         class TagArrayProperty : public Property {
              BitSet& property;
              int tagNum;
              const char** tags;
              bool inverted;
            protected:
              void evaluate_rw ( );
            public:
               TagArrayProperty ( BitSet& property_, int tagNum_, const char** tags_, bool inverted_  ) : property ( property_ ), tagNum (tagNum_), tags ( tags_ ), inverted ( inverted_ ) {};
         };
         class TagIntProperty : public Property {
              int& property;
              int tagNum;
              const char** tags;
              bool inverted;
            protected:
              void evaluate_rw ( );
            public:
               TagIntProperty ( int& property_, int tagNum_, const char** tags_, bool inverted_  ) : property ( property_ ), tagNum (tagNum_), tags ( tags_ ), inverted ( inverted_ ) {};
         };
         class NamedIntProperty : public Property {
              int& property;
              int tagNum;
              const char** tags;
            protected:
              void evaluate_rw ( );
            public:
               NamedIntProperty ( int& property_, int tagNum_, const char** tags_ ) : property ( property_ ), tagNum (tagNum_), tags ( tags_ ) {};
         };


         class ImageProperty : public Property {
               void* &property;
               ASCString fileName;
            protected:
               void evaluate_rw ( );
            public:
               ImageProperty ( void* &property_, const ASCString& fileName_ ) : property ( property_ ), fileName ( fileName_ ) {};
         };

         class ImageArrayProperty : public Property {
               typedef vector<void*> PropertyType;
               PropertyType &property;
               ASCString fileName;
            protected:
               void evaluate_rw ( );
            public:
               ImageArrayProperty ( PropertyType &property_, const ASCString& fileName_ ) : property ( property_ ), fileName ( fileName_ ) {};
         };


         typedef PointerList<Property*> Properties;
         Properties properties;

         virtual void openBracket( const ASCString& name );
         virtual void closeBracket();

         StringProperty&        addString ( const ASCString& name, ASCString& property );
         StringArrayProperty&   addStringArray ( const ASCString& name, vector<ASCString>& property );
         IntProperty&           addInteger ( const ASCString& name, int& property );
         IntProperty&           addInteger ( const ASCString& name, int& property, int defaultValue );
         IntegerArrayProperty&  addIntegerArray ( const ASCString& name, vector<int>& property );
         IntRangeArrayProperty& addIntRangeArray ( const ASCString& name, vector<IntRange>& property );
         TagArrayProperty&      addTagArray ( const ASCString& name, BitSet& property, int tagNum, const char** tags, bool inverted = false );
         TagIntProperty&        addTagInteger ( const ASCString& name, int& property, int tagNum, const char** tags, bool inverted = false );
         NamedIntProperty&      addNamedInteger ( const ASCString& name, int& property, int tagNum, const char** tags );
         ImageProperty&         addImage ( const ASCString& name, void* &property, const ASCString& fileName );
         ImageArrayProperty&    addImageArray ( const ASCString& name, vector<void*> &property, const ASCString& fileName );
         BoolProperty&          addBool  ( const ASCString& name, bool &property );

         void run ( );
         bool isReading() { return reading; };
         void warning ( const ASCString& errmsg );
         void error ( const ASCString& errmsg );
         bool find ( const ASCString& name );
         virtual ASCString getFileName (  ) = 0;
         virtual ~PropertyContainer ( ) { };
      protected:
         PropertyContainer ( const ASCString& baseName, TextPropertyGroup* tpg, bool reading_ ) : reading( reading_ ), levelDepth ( 0 ), textPropertyGroup( tpg ) { };
         virtual ASCString getLocation() = 0;
      private:
         void setup ( Property* p, const ASCString& name );
         virtual void writeProperty ( Property& p, const ASCString& value ) = 0;

};

class PropertyReadingContainer : public PropertyContainer {
   protected:
         virtual ASCString getLocation() { return textPropertyGroup->location; };
   public:
         virtual ASCString getFileName (  ) { return textPropertyGroup->fileName; };
         PropertyReadingContainer ( const ASCString& baseName, TextPropertyGroup* tpg );
         ~PropertyReadingContainer (  );
         void writeProperty ( Property& p, const ASCString& value );
};

class PropertyWritingContainer : public PropertyContainer {
         tn_file_buf_stream stream;
   protected:
         virtual ASCString getLocation() { return stream.getLocation(); };
   public:
         virtual ASCString getFileName (  ) { return stream.getDeviceName(); };
         PropertyWritingContainer ( const ASCString& baseName, const ASCString& filename_ );
         ~PropertyWritingContainer();

         void writeProperty ( Property& p, const ASCString& value );
         virtual void openBracket( const ASCString& name );
         virtual void closeBracket();
};

extern void* getFieldMask();

#endif
