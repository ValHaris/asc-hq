/***************************************************************************
                          buildingtype.h  -  description
                             -------------------
    begin                : Fri Sep 29 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file buildingtype.h
    \brief The interface for the buildingtype class
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef buildingtypeH
 #define buildingtypeH

 #include "typen.h"
 #include "containerbasetype.h"
 #include "ascstring.h"
 #include "terraintype.h"
 #include "objecttype.h"
 #include "basestreaminterface.h"
 #include "textfileparser.h"
 #include "research.h"

 //! The class describing properties that are common to all buildings of a certain kind. \sa Building
 class  BuildingType : public ContainerBaseType {
    public:
       static const int xdimension = 4;
       static const int ydimension = 6;
        bool             field_Exists[xdimension][ydimension];
        Surface          w_picture [ cwettertypennum ][ maxbuildingpicnum ][4][6];
        int              bi_picture [ cwettertypennum ][ maxbuildingpicnum ][4][6];

        BitSet weatherBits; // for which weather are images available

        /** the method for specifying a building's abilility was change at some point in time.
            This method converts an old legacy ability specification to the new one 
            \see ContainerBaseType::ContainerFunctions
        */
        void convertOldFunctions( int abilities, const ASCString& location );
        
        //! the number of legacy abilities that a building could be equipped with
        static const int cbuildingfunctionnum = 24;
        
   public:
        //! A local coordinate referencing a single field that a building covers.
        class LocalCoordinate {
           public:
              int x,y;
              LocalCoordinate ( int _x, int _y ) : x(_x), y(_y) {};
              LocalCoordinate ( ) : x(-1), y(-1) {};
              LocalCoordinate ( const ASCString& s );
              bool valid() const  { return x>=0 && y>=0; };
              ASCString toString ( ) const;
              bool operator<(const LocalCoordinate& a) const { return y < a.y || (y == a.y && x < a.x);};
        };


        typedef multimap<LocalCoordinate,int> DestructionObjects;
        //! when the building is destroyed, it can leave rubble objects behind. If set to 0 no objects are being created
        DestructionObjects destructionObjects;

        //! the position of the entrance, which is the field of the building where units can enter and leave 
        LocalCoordinate entry;

        /** the armor of the buildingtype.
            This does not necessarily be equal to the armor of a specific building during the
            game, since the map may modify the armor of buildings with a map parameter.
            Use Building::getArmor() to query the effective armor. */
        int          _armor;

        //! not used at the moment
        int          technologylevel;

        //! the terrain properties which are necessary for the building to be constructed there
        TerrainAccess terrainaccess;

        //! the number of stages that are required to construct a building using a construction unit. Each stage has a separate picture. Range is 1 to 8
        int          construction_steps;

        int getMoveMalusType() const {
           return 11;
        };


        //! bitmapped: units on these levels of height may be refuelled when standing next to the buildings entry
        int          externalloadheight;

        const Surface& getPicture ( const LocalCoordinate& localCoordinate, int weather = 0, int constructionStep = 0 ) const;
        void         paint ( Surface& s, SPoint pos, const PlayerColor& player, int weather = 0, int constructionStep = 0 ) const;
        void         paint ( Surface& s, SPoint pos ) const;
        void         paintSingleField ( Surface& s, SPoint pos, const LocalCoordinate& localCoordinate, const PlayerColor& player , int weather = 0, int constructionStep = 0 ) const;
        void         paintSingleField ( Surface& s, SPoint pos, const LocalCoordinate& localCoordinate, int weather = 0, int constructionStep = 0 ) const;

        int          getBIPicture( const LocalCoordinate& localCoordinate, int weather = 0, int constructionStep = 0) const;
        
        //! returns whether this building covers the given field 
        bool         fieldExists(const LocalCoordinate& localCoordinate) const { return field_Exists[localCoordinate.x][localCoordinate.y]; } ;
        
        BuildingType ( void );

        /** returns the Mapcoordinate of a buildings field
            \param entryOnMap The location of the buildings entry on the map
            \param localCoordinate The relative coordinate of the buildings segment for which is global MapCoordinate is going to be calculated and returned.
        */
        MapCoordinate getFieldCoordinate( const MapCoordinate& entryOnMap, const LocalCoordinate& localCoordinate ) const;

        //! converts a global coordinate into a local coordinate.
        LocalCoordinate getLocalCoordinate( const MapCoordinate& entryOnMap, const MapCoordinate& field ) const;

        void read ( tnstream& stream ) ;
        void write ( tnstream& stream ) const ;
        void runTextIO ( PropertyContainer& pc );

        //! if true, this building can not be removed by the player with his construction vehicles 
        bool buildingNotRemovable;
        
        int getMemoryFootprint() const;

        
};


#endif
