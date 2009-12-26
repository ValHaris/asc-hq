
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <sstream>

#include "memory-measurement.h"
#include "memsize_interface.h"
#include "iconrepository.h"

#include "gamemap.h"



void MemoryMeasurement::measureTypes()
{
      
   Node* node = new Node( "Type Repositories" );
   root.add( node );
   
   node->add( measure( vehicleTypeRepository ));
   node->add( measure( terrainTypeRepository ));
   node->add( measure( objectTypeRepository ));
   node->add( measure( buildingTypeRepository )); 
   // node->add( measure( technologyRepository ));

}
 
 
void MemoryMeasurement::measure( GameMap* gamemap )
{
   Node* node = new Node( "Current Map" );
   root.add( node );
   node->usage = gamemap->getMemoryFootprint();

   Node* buildings = new Node("Buildings");
   node->add( buildings );
   Node* units = new Node("Units");
   node->add( units );

   for ( int i = 0; i < gamemap->getPlayerCount(); ++i ) {
      Player& p = gamemap->getPlayer(i);
      for ( Player::VehicleList::iterator i = p.vehicleList.begin(); i != p.vehicleList.end(); ++i )
         units->usage += (*i)->getMemoryFootprint();

      for ( Player::BuildingList::iterator i = p.buildingList.begin(); i != p.buildingList.end(); ++i )
         buildings->usage += (*i)->getMemoryFootprint();
   }

   Node* fields = new Node("Fields");
   node->add( fields);


   Node* objects = new Node("Objects");
   fields->add(objects);

   for ( int y = 0; y < gamemap->ysize; ++y )
      for ( int x = 0; x < gamemap->xsize; ++x ) {
         MapField* fld = gamemap->getField(x,y);
         fields->usage += fld->getMemoryFootprint();
         objects->usage += sizeof( Object ) * fld->objects.size();
      }
  
   
}
      
void MemoryMeasurement::measureIcons()
{
   Node* node = new Node( "Global Images" );
   root.add( node );
   node->usage = IconRepository::getMemoryFootprint();
}

 
 
int MemoryMeasurement::Node::sum()
{
   int sum = usage;
   for ( deallocating_vector<Node*>::iterator i = childs.begin(); i != childs.end(); ++i )
      sum += (*i)->sum();
   return sum;
}


ASCString MemoryMeasurement::Node::prettyPrint( int indent )
{
   ASCString result; // ( 2 * indent, ' ' );
   result += "#indent=" + ASCString::toString(indent*20) + "," + ASCString::toString(indent*20) + "#";
   result += name + ": " + ASCString::toString(usage/1024) + " / " + ASCString::toString( sum()/1024) + "\n";
   for ( deallocating_vector<Node*>::iterator i = childs.begin(); i != childs.end(); ++i )
      result += (*i)->prettyPrint( indent + 1);
   return result;
}


 
template<class T>
MemoryMeasurement::Node* MemoryMeasurement::measure( ItemRepository<T>& repository )
{
   
   Node* node = new Node( repository.getTypeName() );
   
   node->usage += for_each ( repository.getVector().begin(), repository.getVector().end(), MemorySum<T>() ).size;
   
   return node;
}
 

 
 template<class T> 
 struct ObjectStorage : public unary_function<T, void>
 {
    static bool compare( const pair<int,int>& a1, const pair<int,int>& a2 )
    {
       return a1.second > a2.second;
    }
       
    typedef vector< pair<int,int> > Storage;
    Storage storage;
    void operator() (const T& x) { storage.push_back( make_pair(x->id, getMemoryFootprint(*x))); };
    ASCString print()
    {
       sort( storage.begin(), storage.end(), &compare );

       ASCString s;
       for ( Storage::iterator i = storage.begin(); i != storage.end(); ++i ) {
          ObjectType* o = objectTypeRepository.getObject_byID(i->first);
          s += ASCString::toString(i->second/1024) + " " + o->name  + "(ID: " + ASCString::toString(i->first) + "; " + o->filename + ")\n";
       }
          
       return s;
    }
 };



ASCString MemoryMeasurement::getResult()
{
   ASCString result = "#fontsize=16#Memory Footprint\n#fontsize=12#all sizes in kB\n\n";
   result += root.prettyPrint(0);

   result += "#indent=0,0#";

   result += "\n\n#fontsize=16#Objects#fontsize=12#\n\n";

   // for_each ( objectTypeRepository.getVector().begin(), objectTypeRepository.getVector().end(), ObjectStorage<ObjectType*>() );
   result += for_each ( objectTypeRepository.getVector().begin(), objectTypeRepository.getVector().end(), ObjectStorage<ObjectType*>() ).print();
   return result;
}




 
