/***************************************************************************
                          itemrepository.cpp   -  description
                             -------------------
    begin                : Thu Jul 28 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/
/*! \file itemrepository.cpp
    \brief Storage facility for all buildingtypes, vehicletypes etc 
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <map>
#include <vector>
#include "typen.h"
#include "itemrepository.h"
#include "textfileparser.h"
#include "sgstream.h"
#include "textfile_evaluation.h"

#ifndef converter
#  include "dialog.h"
#  include "sg.h"
#else
 class ProgressBar {
    public:
       void point() {};
 };
ProgressBar* actprogressbar = NULL;
#endif

pobjecttype eisbrecherobject = NULL;
pobjecttype fahrspurobject = NULL;


typedef vector<TextFileDataLoader*> DataLoaders;
DataLoaders dataLoaders;
DataLoaders dataLoadersToDelete;


typedef map<ASCString,TextPropertyList> TextFileRepository;
TextFileRepository textFileRepository;


void duplicateIDError ( const ASCString& itemtype, int id, const ASCString& file1, const ASCString& name1, const ASCString&  file2, const ASCString& name2 )
{
   fatalError ( "Conflicting IDs !\n"
                "These two " + itemtype + "s use both the ID of " + strrr ( id ) + "\n" +
                " " + name1 + " from file " + file1 + "\n" +
                " " + name2 + " from file " + file2 + "\n\n" +
                "This is NOT a bug of ASC, it is conflict between two data files.");
}



template<class T>
void ItemRepository<T>::add( T* obj )
{
   typename ObjectMap::iterator i = hash.find ( obj->id );
   if ( i != hash.end() && i->second )
     duplicateIDError ( typeName, obj->id, obj->location, obj->name, i->second->location, i->second->name );

   container.push_back( obj );
   hash[ obj->id ] = obj;
}


template<class T>
void ItemRepository<T>::readTextFiles( PropertyReadingContainer& prc, const ASCString& fileName, const ASCString& location )
{
   T* t = new T;
   t->runTextIO ( prc );
   prc.run();

   t->filename = fileName;
   t->location = location;
   add ( t );
}


template<class T>
void ItemRepository<T>::read( tnstream& stream )
{
   int version = stream.readInt();
   int num = stream.readInt();
   for ( int i = 0; i< num; ++i ) {
      if ( actprogressbar )
        actprogressbar->point();

      T* t = new T;
      t->read( stream );

      t->filename = stream.readString();
      t->location = stream.readString();

      add ( t );
      // add ( T::newFromStream(stream ));
   }
}


template<class T>
void ItemRepository<T>::write( tnstream& stream )
{
   stream.writeInt( 1 );
   stream.writeInt( container.size() );
   for ( typename vector<T*>::iterator i = container.begin(); i != container.end(); ++i ) {
       (*i)->write( stream );
       stream.writeString ( (*i)->filename );
       stream.writeString ( (*i)->location );
   }
}


ItemRepository<Vehicletype>  vehicleTypeRepository( "vehicletype" );
ItemRepository<TerrainType>  terrainTypeRepository( "terraintype" );
ItemRepository<ObjectType>   objectTypeRepository ( "objecttype" );
ItemRepository<BuildingType> buildingTypeRepository ("buildingtype");
ItemRepository<Technology>   technologyRepository ( "technology");

TechAdapterContainer techAdapterContainer;


class TechAdapterLoader : public TextFileDataLoader {
      void readTextFiles(PropertyReadingContainer& prc, const ASCString& fileName, const ASCString& location ) {
           TechAdapter* ta = new TechAdapter;
           ta->runTextIO ( prc );
           prc.run();

           ta->filename = fileName;
           ta->location = location;
           techAdapterContainer.push_back ( ta );
      };

      void read ( tnstream& stream ){
         readPointerContainer( techAdapterContainer, stream );
      };

      void write ( tnstream& stream ){
         writePointerContainer( techAdapterContainer, stream );
      };
      ASCString getTypeName() {
         return "techadapter";
      };
};


void  loadalltextfiles ( );

const int cacheVersion = 8;

class FileCache {
      vector<tfindfile::FileInfo> actualFileInfo;
      tnstream* stream;
      bool current;
      bool checkForModification ();
   public:
      FileCache( );
      bool isCurrent() { return current; };
      void load();
      void write();

      ~FileCache();
};

FileCache::FileCache( )
          :stream(NULL)
{
   tfindfile::FileInfo fi;
   {
      tfindfile f ( "*.con", tfindfile::AllDirs, tfindfile::OutsideContainer);
      while ( f.getnextname( fi ))
         actualFileInfo.push_back ( fi );
   }
   {
      tfindfile f ( "*.asctxt", tfindfile::AllDirs, tfindfile::OutsideContainer);
      while ( f.getnextname( fi ))
         actualFileInfo.push_back ( fi );
   }

   if ( exist ( "asc.cache" )) {
      stream = new tnfilestream ( "asc.cache", tnstream::reading );
      int version = stream->readInt();

      if ( version == cacheVersion )
         current = checkForModification();
      else
         current = false;
   } else
      current = false;
}

bool FileCache::checkForModification (  )
{
   vector<tfindfile::FileInfo> cacheFileInfo;
   readClassContainer ( cacheFileInfo, *stream );

   if ( cacheFileInfo.size() != actualFileInfo.size() )
      return false;

   for ( vector<tfindfile::FileInfo>::iterator i = actualFileInfo.begin(); i != actualFileInfo.end(); ++i ) {
      bool found = false;
      for ( vector<tfindfile::FileInfo>::iterator j = cacheFileInfo.begin(); j != cacheFileInfo.end(); ++j )
         if ( i->name == j->name ) {
            found = true;
            if ( i->size != j->size )
               return false;

            if ( i->date != j->date )
               return false;

            break;
         }

      if ( !found)
         return false;
   }


   return true;
}


void FileCache::load()
{
   for ( DataLoaders::iterator i = dataLoaders.begin(); i != dataLoaders.end(); ++i) {
      displayLogMessage ( 5, "loading all of " + (*i)->getTypeName() + " from cache ... ");
      (*i)->read ( *stream );
      displayLogMessage ( 5, "completed \n ");
   }
}


void FileCache::write()
{
   if ( stream )
      delete stream;

   stream = new tn_file_buf_stream ( "asc.cache", tnstream::writing );

   stream->writeInt ( cacheVersion );
   writeClassContainer ( actualFileInfo, *stream );

   for ( DataLoaders::iterator i = dataLoaders.begin(); i != dataLoaders.end(); ++i)
      (*i)->write ( *stream );

}



FileCache::~FileCache()
{
   if ( stream ) {
      delete stream;
      stream = NULL;
   }
   for ( DataLoaders::iterator i = dataLoadersToDelete.begin(); i != dataLoadersToDelete.end(); ++i)
      delete *i;
}



void registerDataLoader( TextFileDataLoader* dataLoader )
{
   dataLoaders.push_back ( dataLoader );
   dataLoadersToDelete.push_back ( dataLoader );
}

void registerDataLoader( TextFileDataLoader& dataLoader )
{
   dataLoaders.push_back ( &dataLoader );
}


void  loadAllData( bool useCache )
{
   FileCache cache;

   registerDataLoader( vehicleTypeRepository );
   registerDataLoader( terrainTypeRepository );
   registerDataLoader( objectTypeRepository );
   registerDataLoader( buildingTypeRepository );
   registerDataLoader( technologyRepository );
   registerDataLoader( new TechAdapterLoader() );
   registerDataLoader( new ItemFiltrationSystem::DataLoader() );


   if ( cache.isCurrent() && useCache ) {
      try {
         cache.load();
      }
      catch ( tinvalidversion err ) {
         fatalError("the cache seems to have been generated with a newer version of ASC than this one.\nPlease upgrade to that version, or delete asc.cache and try again");
      }
      displayLogMessage ( 4, "loading of cache completed\n");
   } else {
      loadalltextfiles();

      for ( DataLoaders::iterator dl = dataLoaders.begin(); dl != dataLoaders.end(); ++dl) {
         TextPropertyList& tpl = textFileRepository[ (*dl)->getTypeName() ];
         for ( TextPropertyList::iterator i = tpl.begin(); i != tpl.end(); i++ ) {
            if ( actprogressbar )
              actprogressbar->point();

            if ( !(*i)->isAbstract() ) {
               PropertyReadingContainer pc ( (*dl)->getTypeName(), *i );

               (*dl)->readTextFiles( pc, (*i)->fileName, (*i)->location );
            }
         }

         displayLogMessage ( 4, "loading all of " + (*dl)->getTypeName() + "  completed\n");

      }

      if ( useCache ) {
         try {
            cache.write();
         }
         catch ( tfileerror err ) {
            fatalError("error writing cache file");
         }
      }

      textFileRepository.clear();
   }

   eisbrecherobject = objectTypeRepository.getObject_byID( 6 );
   fahrspurobject   = objectTypeRepository.getObject_byID( 7 );
}





void  loadalltextfiles ( )
{
   tfindfile ff ( "*.asctxt" );
   ASCString c = ff.getnextname();

   while( !c.empty() ) {
      if ( actprogressbar )
         actprogressbar->point();

      tnfilestream s ( c, tnstream::reading );

      displayLogMessage ( 6, "loadalltextfiles :: loading " + s.getLocation() + ", " );

      TextFormatParser tfp ( &s );

      displayLogMessage ( 5, "TFP running... " );

      TextPropertyGroup* tpg = tfp.run();

      textFileRepository[tpg->typeName].push_back ( tpg );

      displayLogMessage ( 5, "done\n" );

      c = ff.getnextname();
   }
   displayLogMessage ( 4, "loadalltextfiles completed\n");

   displayLogMessage ( 4, "Building inheritance...");

   displayLogMessage ( 10, " Printing all .ASCTXT files after inheritance and aliases have been resolved\n");
   for ( TextFileRepository::iterator i = textFileRepository.begin(); i != textFileRepository.end(); i++ ) {
      i->second.buildIDs();
      for ( TextPropertyList::iterator j = i->second.begin(); j != i->second.end(); j++ ) {
          displayLogMessage( 9, "Building inheritance: " + (*j)->location + "\n");
          (*j)->buildInheritance( i->second );
          if ( verbosity >= 10 )
             (*j)->print();
      }
   }
   displayLogMessage ( 4, "done\n");
}







vector<ItemFiltrationSystem::ItemFilter*> ItemFiltrationSystem::itemFilters;


ItemFiltrationSystem::ItemFilter::ItemFilter( const ASCString& _name, const IntRangeArray& unitsetIDs, bool _active )
{
   name = "UnitSet: " + _name;
   units = unitsetIDs;
   active = _active;
}


void ItemFiltrationSystem::ItemFilter::runTextIO ( PropertyContainer& pc )
{
    if ( pc.find ( "Buildings"))
      pc.addIntRangeArray ( "Buildings", buildings );
    if ( pc.find ( "Vehicles"))
      pc.addIntRangeArray ( "Vehicles", units );
    if ( pc.find ( "Objects"))
       pc.addIntRangeArray ( "Objects", objects );
    if ( pc.find ( "Terrain"))
      pc.addIntRangeArray ( "Terrain", terrain );
    if ( pc.find ( "Technologies"))
      pc.addIntRangeArray ( "Technologies", technologies );
    pc.addBool ( "activated", active, false );
    pc.addString ( "name", name );
}

void ItemFiltrationSystem::ItemFilter::read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version > 2 )
      throw tinvalidversion( stream.getDeviceName(), 2, version );
   readClassContainer ( buildings, stream );
   readClassContainer ( objects, stream );
   readClassContainer ( units, stream );
   readClassContainer ( terrain, stream );
   if ( version >= 2 )
      readClassContainer ( technologies, stream );

   active = stream.readInt();
   name = stream.readString();
}

void ItemFiltrationSystem::ItemFilter::write ( tnstream& stream )
{
   stream.writeInt ( 2 );
   writeClassContainer ( buildings, stream );
   writeClassContainer ( objects, stream );
   writeClassContainer ( units, stream );
   writeClassContainer ( terrain, stream );
   writeClassContainer ( technologies, stream );
   stream.writeInt ( active );
   stream.writeString ( name );
}



bool ItemFiltrationSystem::ItemFilter::isContained ( IntRangeArray& arr, int id )
{
   for ( IntRangeArray::iterator i = arr.begin(); i != arr.end(); i++ )
      if ( id >= i->from && id <= i->to )
         return true;
   return false;
}



bool ItemFiltrationSystem::ItemFilter::isContained ( ItemFiltrationSystem::Category cat, int id )
{
   switch ( cat ) {
      case Building: return isContained ( buildings, id );
      case Vehicle: return isContained ( units, id );
      case Object: return isContained ( objects, id );
      case Terrain: return isContained ( terrain, id );
      case Technology: return isContained ( technologies, id );
   };
   return false;
}

bool ItemFiltrationSystem::isFiltered ( ItemFiltrationSystem::Category cat, int id )
{
   for ( vector<ItemFilter*>::iterator i = itemFilters.begin(); i != itemFilters.end(); i++ )
      if ( (*i)->isContained ( cat, id ) )
         if ( (*i)->isActive() )
            return true;

   return false;
}


void ItemFiltrationSystem::DataLoader::readTextFiles( PropertyReadingContainer& prc, const ASCString& fileName, const ASCString& location )
{
   ItemFilter* itf = new ItemFilter;
   itf->runTextIO ( prc );
   prc.run();

   // bmtt->filename = (*i)->fileName;
   // bmtt->location = (*i)->location;
   ItemFiltrationSystem::itemFilters.push_back ( itf );
}

void ItemFiltrationSystem::DataLoader::read ( tnstream& stream )
{
   stream.readInt();
   readPointerContainer( ItemFiltrationSystem::itemFilters, stream );
}

void ItemFiltrationSystem::DataLoader::write ( tnstream& stream )
{
   stream.writeInt ( 1 );
   writePointerContainer( ItemFiltrationSystem::itemFilters, stream );
}

