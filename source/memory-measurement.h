
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef memorymeasurementH
 #define memorymeasurementH
 
#include <map>
#include "ascstring.h"
#include "itemrepository.h"

 
 
 class MemoryMeasurement {
   
     class Node {
        public:
           Node( const ASCString& name ) : usage(0) { this->name = name; };
           ASCString name;
           int usage;
           deallocating_vector<Node*> childs;
           void add( Node* n) { childs.push_back( n ); };
           
           int sum();
           ASCString prettyPrint( int indent );
     };
   
     Node root;
     
     template<class T>
     Node* measure(  ItemRepository<T>& repository );
     
     
    public:
       MemoryMeasurement() : root("root")  {};
       
       void measureTypes();
       void measure( GameMap* gamemap );
       void measureIcons( );
       ASCString getResult();
       
       
       
 };
 
 
#endif 
