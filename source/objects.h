/*! \file objects.h
    \brief Mine and Objects which can be placed on a map field
*/

/***************************************************************************
                          gamemap.h  -  description
                             -------------------
    begin                : Tue Feb 17 2001
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


#ifndef objectsH
 #define objectsH

 #include <vector>

 #include "typen.h"
 #include "mapitemtype.h"
 #include "overviewmapimage.h"
 #include "graphics/surface.h"
 
 class ObjectType;

class AgeableItem {
    protected:
       AgeableItem() : lifetimer(-1) {};
    public:
       int lifetimer;

       //! ages the object by one turn. Returns true if the object shall be removed
       static bool age( AgeableItem& obj );
};

//! an instance of an object type (#ObjectType) on the map
class Object : public AgeableItem {
    public:
       const ObjectType* typ;
       int damage;
       int dir;
       int remainingGrowthTime;
       
       Object ();
       Object ( const ObjectType* o );
       void display ( Surface& surface, const SPoint& pos, int weather = 0 ) const;
       const OverviewMapImage* getOverviewMapImage( int weather );
       void setdir ( int dir );
       int  getdir ( void );

       void write ( tnstream& stream );
       void read ( tnstream& stream );
       
};

const int cminenum = 4;
extern const char* MineNames[cminenum] ;
extern const int MineBasePunch[cminenum]  ;

enum MineTypes { cmantipersonnelmine = 1 , cmantitankmine, cmmooredmine, cmfloatmine  };

class MineType : public MapItemType {
      MineTypes type;
   public:
      int id;

      //! not really used for minetypes.
      vector<int> secondaryIDs;

      ASCString name;
      ASCString location;
      
      int getID() const { return id; }; 
      
      MineType( MineTypes t ) : type ( t ), id( int(t)) {};
      MineType( const MineType& w ) : type ( w.type ) {};
      ASCString getName() const {
         return MineNames[int(type)-1];
      };
      
      void paint ( Surface& surface, const SPoint& pos ) const ;
      static void paint( MineTypes type, int player, Surface& surf, const SPoint& pos );
};

#endif

