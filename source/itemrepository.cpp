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
   ObjectMap::iterator i = hash.find ( obj->id );
   if ( i != hash.end() && i->second )
     duplicateIDError ( typeName, obj->id, obj->location, obj->name, i->second->location, i->second->name );

   container.push_back( obj );
   hash[ obj->id ] = obj;
}


template<class T>
void ItemRepository<T>::load()
{
   TextPropertyList& tpl = textFileRepository[typeName];
   for ( TextPropertyList::iterator i = tpl.begin(); i != tpl.end(); i++ ) {
      if ( actprogressbar )
        actprogressbar->point();

      if ( !(*i)->isAbstract() ) {
         PropertyReadingContainer pc ( typeName, *i );

         T* t = new T;
         t->runTextIO ( pc );
         pc.run();

         t->filename = (*i)->fileName;
         t->location = (*i)->location;
         add ( t );
    }
  }
  displayLogMessage ( 4, "loading all of " + typeName + "  completed\n");
}


ItemRepository<Vehicletype>  vehicleTypeRepository( "vehicletype" );
ItemRepository<TerrainType>  terrainTypeRepository( "terraintype" );
ItemRepository<ObjectType>   objectTypeRepository ( "objecttype" );
ItemRepository<BuildingType> buildingTypeRepository ("buildingtype");
ItemRepository<Technology>   technologyRepository ( "technology");

void         loadalltechadapter()
{
   TextPropertyList& tpl = textFileRepository["techadapter"];
   for ( TextPropertyList::iterator i = tpl.begin(); i != tpl.end(); i++ ) {
      if ( actprogressbar )
        actprogressbar->point();

      if ( !(*i)->isAbstract() ) {
        PropertyReadingContainer pc ( "techadapter", *i );

        TechAdapter* ta = new TechAdapter;
        ta->runTextIO ( pc );
        pc.run();

        ta->filename = (*i)->fileName;
        ta->location = (*i)->location;
        techAdapterContainer.push_back ( ta );
      }
   }
   displayLogMessage ( 4, "loadallTechAdapter completed\n");
}


void  loadAllData()
{
   vehicleTypeRepository.load();
   terrainTypeRepository.load();
   objectTypeRepository.load();
   buildingTypeRepository.load();
   technologyRepository.load();
   loadalltechadapter();

   eisbrecherobject = objectTypeRepository.getObject_byID( 6 );
   fahrspurobject   = objectTypeRepository.getObject_byID( 7 );
}

TechAdapterContainer techAdapterContainer;



TextFileRepository textFileRepository;

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


void  freetextdata()
{
   textFileRepository.clear();
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
    pc.addIntRangeArray ( "Buildings", buildings );
    pc.addIntRangeArray ( "Vehicles", units );
    pc.addIntRangeArray ( "Objects", objects );
    pc.addIntRangeArray ( "Terrain", terrain );
    pc.addBool ( "activated", active, false );
    pc.addString ( "name", name );
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


void ItemFiltrationSystem::read ( )
{
    TextPropertyList& tpl = textFileRepository["itemfilter"];
    for ( TextPropertyList::iterator i = tpl.begin(); i != tpl.end(); i++ ) {

      PropertyReadingContainer pc ( "itemfilter", *i );

      ItemFilter* itf = new ItemFilter;
      itf->runTextIO ( pc );
      pc.run();

      // bmtt->filename = (*i)->fileName;
      // bmtt->location = (*i)->location;
      itemFilters.push_back ( itf );
   }
}


