/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING. If not, write to the
    Free Software Foundation, Inc., 59 Temple Place, Suite 330,
    Boston, MA  02111-1307  USA
*/


#ifndef factoryWithNamesH
#define factoryWithNamesH

#include <sigc++/sigc++.h>
#include "loki/Singleton.h"
#include "loki/Functor.h"

#include "factory.h"


template < class AbstractProduct, typename IdentifierType, typename ObjectCreatorCallBack = AbstractProduct*(*)(), typename NameType = ASCString >
class FactoryWithNames : protected Factory<AbstractProduct, IdentifierType>
{
   private:
      typedef std::map<NameType, IdentifierType> NameMap;
      NameMap names;
   public:
      vector<NameType> getNames(){
         vector<NameType> nameList;
         
         for ( typename NameMap::iterator i = names.begin(); i != names.end(); ++i )
            nameList.push_back( i->first );
         return nameList;
      }

      IdentifierType getID( const ASCString& name )
      {
         return names[name];
      }

      bool registerClass( IdentifierType id, typename FactoryWithNames<AbstractProduct, IdentifierType, typename FactoryWithNames::ObjectCreatorCallBack, NameType>::ObjectCreatorCallBack createFn, Loki::Functor<NameType, LOKI_TYPELIST_1(const IdentifierType&)> nameProvider )
      {
         return registerClass( id, createFn, nameProvider(id) );
      }

      bool registerClass( IdentifierType id, typename FactoryWithNames<AbstractProduct, IdentifierType, typename FactoryWithNames::ObjectCreatorCallBack, NameType>::ObjectCreatorCallBack createFn, NameType name )
      {
         if ( Factory<AbstractProduct, IdentifierType>::registerClass ( id, createFn )) {
            names[name] = id;
            return true;
         } else
            return false;
      }

      AbstractProduct* createObject( IdentifierType id ) {
         return Factory<AbstractProduct, IdentifierType>::createObject( id );
      }

};

#endif
