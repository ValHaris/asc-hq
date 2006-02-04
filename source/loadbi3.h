/***************************************************************************
                          loadbi3.h  -  description
                             -------------------
    begin                :
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file loadbi3.h
   \brief Loading Battle Isle 3 maps.
   The central BI3 map loading class is a straight conversion of
   Joerg Richter's code from his BI3 map editor.
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef loadbi3H
#define loadbi3H

#include "typen.h"
#include "sgstream.h"
#include "palette.h"
#include "itemrepository.h"


#ifdef converter
 #error The small editors should not need to use LoadBi3
#endif



/** Imports a Battle Isle map 
  \param path the path to the BI3 installation
  \param filename The name of the map
  \param trrn The terrain that is used for all fields which are unknown to ASC
  \param errorOutput Pointer to a string which will store all warning and error messages from the import. May be NULL.
  \param __fakeMap If true ASC tries to make the map look like the Battle Isle map, generating new ObjectType and TerrainType instances if necessary.
                   This must not be used by any program other than map2pcx, since the maps can not be played. This is only used to generate
                   the images of the Battle Isle Map Archive.   
*/
extern void importbattleislemap ( const char* path, const char* filename, TerrainType::Weather* trrn, string* errorOutput = NULL, bool __fakeMap = false );

/** Imports a Battle Isle map and inserts it into the current ASC map
  \param x The x coordinate of field where the upper left field of the BI map will be placed.
  \param y The y coordinate of field where the upper left field of the BI map will be placed.
  \param path the path to the BI3 installation
  \param filename The name of the map
*/
extern void insertbattleislemap ( int x, int y, const char* path, const char* filename  );

//! returns the path to the BI3 installation
extern ASCString getbi3path();

//! to prevent the appearnce of the dialog "choose import translation table" when running in batch mode, a default translation table can be specified
extern void setDefaultBI3ImportTranslationTable( const ASCString& filename );

extern int getterraintranslatenum ( void );
extern int getobjectcontainertranslatenum ( void );

extern const int* getterraintranslate ( int pos );
extern const int* getobjectcontainertranslate ( int pos );

class BI3TranslationTableLoader: public TextFileDataLoader {
   public:
      virtual void readTextFiles( PropertyReadingContainer& prc, const ASCString& fileName, const ASCString& location ) ;
      virtual void read ( tnstream& stream ) ;
      virtual void write ( tnstream& stream );
      virtual ASCString getTypeName();
};


#endif
