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
#include "messaginghub.h"


SigC::Signal0<void> dataLoaderTicker;


ObjectType* eisbrecherobject = NULL;
ObjectType* fahrspurobject = NULL;

const char* cacheFileName = "asc2.cache";

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
void ItemRepositoryLoader<T>::readTextFiles( PropertyReadingContainer& prc, const ASCString& fileName, const ASCString& location )
{
   T* t = new T;
   t->runTextIO ( prc );

   t->filename = fileName;
   t->location = location;
   add ( t );
}


template<class T>
void ItemRepositoryLoader<T>::read( tnstream& stream )
{
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion( stream.getLocation(), 1, version );
   int num = stream.readInt();
   for ( int i = 0; i< num; ++i ) {
      dataLoaderTicker();

      T* t = new T;
      t->read( stream );

      t->filename = stream.readString();
      t->location = stream.readString();
      dataLoaderTicker();

      add ( t );
      // add ( T::newFromStream(stream ));
   }
}


template<class T>
void ItemRepositoryLoader<T>::write( tnstream& stream )
{
   stream.writeInt( 1 );
   stream.writeInt( ItemRepository<T>::container.size() );
   for ( typename vector<T*>::iterator i = ItemRepository<T>::container.begin(); i != ItemRepository<T>::container.end(); ++i ) {
       (*i)->write( stream );
       stream.writeString ( (*i)->filename );
       stream.writeString ( (*i)->location );
   }
}


template<class T>
void ItemRepository<T>::addIdTranslation( int from, int to )
{
    idTranslation[from] = to;
}

MineTypeRepository  mineTypeRepository;


MineTypeRepository::MineTypeRepository() : ItemRepository<MineType>("Mines")
{
   add( new MineType( cmantipersonnelmine ) );
   add( new MineType( cmantitankmine ) );
   add( new MineType( cmmooredmine ) );
   add( new MineType( cmfloatmine ) );
}


ItemRepositoryLoader<Vehicletype>  vehicleTypeRepository( "vehicletype" );
ItemRepositoryLoader<TerrainType>  terrainTypeRepository( "terraintype" );
ItemRepositoryLoader<ObjectType>   objectTypeRepository ( "objecttype" );
ItemRepositoryLoader<BuildingType> buildingTypeRepository ("buildingtype");
ItemRepositoryLoader<Technology>   technologyRepository ( "technology");

TechAdapterContainer techAdapterContainer;

namespace {
   class Foo {
   public:
      Foo() { objectTypeRepository.addIdTranslation( 12998, 1 ); };
   } foo;
};

class TechAdapterLoader : public TextFileDataLoader {
      void readTextFiles(PropertyReadingContainer& prc, const ASCString& fileName, const ASCString& location ) {
           TechAdapter* ta = new TechAdapter;
           ta->runTextIO ( prc );

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

const int cacheVersion = 13;

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

   if ( exist ( cacheFileName )) {
      stream = new tnfilestream ( cacheFileName, tnstream::reading );
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

   stream = new tn_file_buf_stream ( cacheFileName, tnstream::writing );

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
   loadguipictures();
   
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
         fatalError("the cache seems to have been generated with a newer version of ASC than this one.\nPlease upgrade to that version, or delete " + ASCString(cacheFileName) + " and try again");
      }
      displayLogMessage ( 4, "loading of cache completed\n");
   } else {
      MessagingHub::Instance().statusInformation("rebuilding data cache, please be patient");
      
      loadalltextfiles();

      for ( DataLoaders::iterator dl = dataLoaders.begin(); dl != dataLoaders.end(); ++dl) {
         TextPropertyList& tpl = textFileRepository[ (*dl)->getTypeName() ];
         for ( TextPropertyList::iterator i = tpl.begin(); i != tpl.end(); i++ ) {
            dataLoaderTicker();

            if ( !(*i)->isAbstract() ) {
               PropertyReadingContainer pc ( (*dl)->getTypeName(), *i );

               displayLogMessage ( 5, "loading " + (*i)->location );
               (*dl)->readTextFiles( pc, (*i)->fileName, (*i)->location );
               displayLogMessage ( 5, " done\n");
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
      dataLoaderTicker();

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
          if ( MessagingHub::Instance().getVerbosity() >= 10 )
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


bool ItemFiltrationSystem::isFiltered( const Vehicletype* item )
{
   return isFiltered( Vehicle, item->id );
}

bool ItemFiltrationSystem::isFiltered( const BuildingType* item )
{
   return isFiltered( Building, item->id );
}

bool ItemFiltrationSystem::isFiltered( const ObjectType* item )
{
   return isFiltered( Object, item->id );
}

bool ItemFiltrationSystem::isFiltered( const TerrainType* item )
{
   return isFiltered( Terrain, item->id );
}

bool ItemFiltrationSystem::isFiltered( const MineType* item )
{
   return false;
}


void ItemFiltrationSystem::DataLoader::readTextFiles( PropertyReadingContainer& prc, const ASCString& fileName, const ASCString& location )
{
   ItemFilter* itf = new ItemFilter;
   itf->runTextIO ( prc );

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

