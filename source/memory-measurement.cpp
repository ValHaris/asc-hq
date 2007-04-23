
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "memory-measurement.h"
#include "memsize_interface.h"
#include "iconrepository.h"
 


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
   ASCString result ( 2 * indent, ' ' );
   result += name + ": " + ASCString::toString(usage) + " / " + ASCString::toString( sum()) + "\n";
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
 
 
ASCString MemoryMeasurement::getResult()
{
   return root.prettyPrint(0);
}

 
 
