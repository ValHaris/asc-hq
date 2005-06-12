/***************************************************************************
                          itemrepository.h  -  description
                             -------------------
    begin                : Thu Jul 28 2001
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

#ifndef itemrepositoryH
#define itemrepositoryH

#include <vector>
#include <sigc++/sigc++.h>
#include "ascstring.h"
#include "typen.h"
#include "terraintype.h"
#include "vehicletype.h"
#include "objecttype.h"
#include "buildingtype.h"
#include "research.h"
#include "textfile_evaluation.h"


class TextFileDataLoader {
   public:
      virtual void readTextFiles( PropertyReadingContainer& prc, const ASCString& fileName, const ASCString& location ) = 0;
      virtual void read ( tnstream& stream ) = 0;
      virtual void write ( tnstream& stream ) = 0;
      virtual ASCString getTypeName() = 0;
      virtual ~TextFileDataLoader() {};
};


//! registers a dataLoader . The object is delete after use, so use: registerDataLoader( new MyDataLoader() )
extern void registerDataLoader( TextFileDataLoader* dataLoader );

//! registers a dataLoader .
extern void registerDataLoader( TextFileDataLoader& dataLoader );


template<class T>
class ItemRepository: public TextFileDataLoader {
      ASCString typeName;
      typedef vector<T*> ItemContainerType;
      ItemContainerType   container;
      typedef map<int,T*>  ObjectMap;
      ObjectMap hash;

      void add( T* obj );

      map<int,int> idTranslation;

   public:
      ItemRepository( const ASCString& typeName_ ) : typeName( typeName_ ) {};
      T* getObject_byPos( int pos ) const { return container[pos]; };

      T* getObject_byID( int id ) { 
         typename ObjectMap::iterator i = hash.find( id );
         if ( i != hash.end() )
            return i->second;

         map<int,int>::iterator j = idTranslation.find( id );
         if ( j != idTranslation.end())
            return getObject_byID( j->second );

         return NULL;
      };

      size_t getNum() const { return container.size(); };
      void readTextFiles( PropertyReadingContainer& prc, const ASCString& fileName, const ASCString& location );
      void read( tnstream& stream );
      void write( tnstream& stream );
      ASCString getTypeName() { return typeName; };

      vector<T*>& getVector() { return container; };
      virtual ~ItemRepository();

      void addIdTranslation( int from, int to );
};

extern SigC::Signal0<void> dataLoaderTicker;


extern ItemRepository<Vehicletype>  vehicleTypeRepository;
extern ItemRepository<TerrainType>  terrainTypeRepository;
extern ItemRepository<ObjectType>   objectTypeRepository;
extern ItemRepository<BuildingType> buildingTypeRepository;
extern ItemRepository<Technology>   technologyRepository;

extern void  loadAllData( bool useCache = true );

extern pobjecttype eisbrecherobject;
extern pobjecttype fahrspurobject;

typedef  vector<TechAdapter*> TechAdapterContainer;
extern TechAdapterContainer techAdapterContainer;


class ItemFiltrationSystem {
      public:
         typedef enum { Building, Vehicle, Object, Terrain, Technology } Category;

         class ItemFilter {
               public:
                 typedef vector<IntRange> IntRangeArray;
               private:
                 IntRangeArray buildings;
                 IntRangeArray objects;
                 IntRangeArray units;
                 IntRangeArray terrain;
                 IntRangeArray technologies;
                 bool isContained (IntRangeArray& arr, int id );
                 bool active;
               public:
                 ItemFilter() { active = false; };
                 ItemFilter( const ASCString& _name, const IntRangeArray& unitsetIDs, bool _active );
                 ASCString name;
                 bool isActive() { return active; };
                 void setActive( bool _active ) { active = _active; };
                 void runTextIO ( PropertyContainer& pc );
                 void read ( tnstream& stream ) ;
                 void write ( tnstream& stream ) ;
                 bool isContained ( ItemFiltrationSystem::Category cat, int id );
         };
         static vector<ItemFilter*> itemFilters;

         class DataLoader : public TextFileDataLoader {
            public:
               void readTextFiles( PropertyReadingContainer& prc, const ASCString& fileName, const ASCString& location );
               void read ( tnstream& stream ) ;
               void write ( tnstream& stream ) ;
               ASCString getTypeName() { return "itemfilter"; };
         };


         static bool isFiltered ( Category cat, int id );
         static bool isFiltered( const Vehicletype* item );
         static bool isFiltered( const BuildingType* item );
         static bool isFiltered( const ObjectType* item );
         static bool isFiltered( const TerrainType* item );
       
};

#endif
