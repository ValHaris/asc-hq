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

/*
const int cbuildingfunctionnum = 24;
extern const char*  cbuildingfunctions[];
 #define cghqn 0
 #define cghqb ( 1 << cghqn  )
 #define cgtrainingn 1
 #define cgtrainingb ( 1 << cgtrainingn  )
 #define cgvehicleproductionn 3
 #define cgvehicleproductionb ( 1 << cgvehicleproductionn  )
 #define cgammunitionproductionn 4
 #define cgammunitionproductionb ( 1 << cgammunitionproductionn  )
 #define cgrepairfacilityn 8
 #define cgrepairfacilityb ( 1 << cgrepairfacilityn  )
 #define cgrecyclingplantn 9
 #define cgrecyclingplantb ( 1 << cgrecyclingplantn  )
 #define cgresearchn 10
 #define cgresearchb ( 1 << cgresearchn  )
 #define cgsonarn 11
 #define cgsonarb ( 1 << cgsonarn )
 #define cgwindkraftwerkn 12
 #define cgwindkraftwerkb ( 1 << cgwindkraftwerkn )
 #define cgsolarkraftwerkn 13
 #define cgsolarkraftwerkb ( 1 << cgsolarkraftwerkn )
 #define cgconventionelpowerplantn 14
 #define cgconventionelpowerplantb ( 1 << cgconventionelpowerplantn )
 #define cgminingstationn 15
 #define cgminingstationb ( 1 << cgminingstationn )
 #define cgexternalloadingn 16
 #define cgexternalloadingb ( 1 << cgexternalloadingn )
 #define cgproduceAllUnitsN 17
 #define cgproduceAllUnitsB ( 1 << cgproduceAllUnitsN )
 #define cgresourceSinkN 18
 #define cgresourceSinkB ( 1 << cgresourceSinkN )
 #define cgexternalresourceloadingn 19
 #define cgexternalresourceloadingb ( 1 << cgexternalresourceloadingn )
 #define cgexternalammoloadingn 20
 #define cgexternalammoloadingb ( 1 << cgexternalammoloadingn )
 #define cgnoobjectchainingn 21
 #define cgnoobjectchainingb ( 1 << cgnoobjectchainingn )
 #define cgselfdestruct_at_conquern 22
 #define cgselfdestruct_at_conquerb ( 1 << cgselfdestruct_at_conquern )
 #define cgsatviewn 23
 #define cgsatviewb ( 1 << cgsatviewn )
*/

 //! The class describing properties that are common to all buildings of a certain kind. \sa Building
 class  BuildingType : public ContainerBaseType {
        bool             field_Exists[4][6];
        Surface          w_picture [ cwettertypennum ][ maxbuildingpicnum ][4][6];
        int              bi_picture [ cwettertypennum ][ maxbuildingpicnum ][4][6];

        BitSet weatherBits; // for which weather are images available

        void convertOldFunctions( int abilities, const ASCString& location );
        
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
        };


        //! when the building is destroyed, it can leave rubble objects behind. If set to 0 no objects are being created
        int          destruction_objects [4][6];

        LocalCoordinate entry;

        /** the armor of the buildingtype.
            This does not necessarily be equal to the armor of a specific building during the
            game, since the map may modify the armor of buildings with a map parameter.
            Use Building::getArmor() to query the effective armor. */
        int          _armor;

        //! not used at the moment
        int          technologylevel;

        //! not used at the moment
        int          researchid;

        //! the terrain properties which are necessary for the building to be constructed there
        TerrainAccess terrainaccess;

        //! the number of stages that are required to construct a building using a construction unit. Each stage has a separate picture. Range is 1 to 8
        int          construction_steps;

        //! bitmapped: the level of height that this building will reside on.
        int          buildingheight;

        int getMoveMalusType() const {
           return 11;
        };


        //! bitmapped: units on these levels of height may be refuelled when standing next to the buildings entry
        int          externalloadheight;

        const Surface& getPicture ( const LocalCoordinate& localCoordinate, int weather = 0, int constructionStep = 0 ) const;
        void         paint ( Surface& s, SPoint pos, int player = 0, int weather = 0, int constructionStep = 0 ) const;
        void         paintSingleField ( Surface& s, SPoint pos, const LocalCoordinate& localCoordinate, int player = 0, int weather = 0, int constructionStep = 0 ) const;

        int          getBIPicture( const LocalCoordinate& localCoordinate, int weather = 0, int constructionStep = 0) const;
        
        
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

        bool buildingNotRemovable;

        
};


#endif
