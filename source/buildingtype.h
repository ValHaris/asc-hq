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


const int cbuildingfunctionnum = 22;
extern const char*  cbuildingfunctions[cbuildingfunctionnum];
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


 //! The class describing properties that are common to all buildings of a certain kind. \sa Building
 class  BuildingType : public ContainerBaseType {
   public:
        //! A local coordinate referencing a single field that a building covers.
        class LocalCoordinate {
           public:
              int x,y;
              LocalCoordinate ( int _x, int _y ) : x(_x), y(_y) {};
              LocalCoordinate ( ) : x(-1), y(-1) {};
              LocalCoordinate ( const ASCString& s );
              ASCString toString ( ) const;
        };

        ASCString    name;

        void*        w_picture [ cwettertypennum ][ maxbuildingpicnum ][4][6];
        int          bi_picture [ cwettertypennum ][ maxbuildingpicnum ][4][6];

        //! when the building is destroyed, it can leave rubble objects behind. If set to 0 no objects are being created
        int          destruction_objects [4][6];

        LocalCoordinate entry;

        int          id;

        /** the armor of the buildingtype.
            This does not necessarily be equal to the armor of a specific building during the
            game, since the map may modify the armor of buildings with a map parameter.
            Use Building::getArmor() to query the effective armor. */
        int          _armor;

        int          jamming;
        int          view;

        //! bitmapped: functions the building can perfom. see #cbuildingfunctions
        int          special;

        //! not used at the moment
        int          technologylevel;

        //! not used at the moment
        int          researchid;

        //! the terrain properties which are necessary for the building to be constructed there
        TerrainAccess terrainaccess;

        //! the number of stages that are required to construct a building using a construction unit. Each stage has a separate picture. Range is 1 to 8
        int          construction_steps;

        //! the maximum number of research points a research center may produce
        int          maxresearchpoints;

        //! when a building of this type is placed on a map, its maxResearch property will be set to this value
        int          defaultMaxResearchpoints;

        //! the number of reseach points for which the plus settings apllies
        int          nominalresearchpoints;

        //! the amount of resources stored in the building. Use Building::getResource( int, int, int, int) to access this field, since depending on the map settings some resources may be globally available and stored in a global pool: #tmap::bi_resource
        Resources    _tank;
        Resources    maxplus;

        //! currently only used by mining stations: the efficiency of the resource extraction from the ground. Base is 1024
        int          efficiencyfuel;

        //! currently only used by mining stations: the efficiency of the resource extraction from the ground. Base is 1024
        int          efficiencymaterial;

        //! the picture for the GUI that is used for selecting a building that is going to be constructed by a unit
        void*        guibuildicon;

        //! the maximum resource storage in BI resource mode.
        Resources    _bi_maxstorage;

        //! if a new building is constructed, this will be the resource production of the building
        Resources    defaultProduction;

        //! bitmapped: the level of height that this building will reside on.
        int          buildingheight;

        //! bitmapped: units on these levels of height may be refuelled when standing next to the buildings entry
        int          externalloadheight;

        void*        getpicture ( const LocalCoordinate& localCoordinate );

        BuildingType ( void );

        /** returns the Mapcoordinate of a buildings field
            \param entryOnMap The location of the buildings entry on the map
            \param localCoordinate The relative coordinate of the buildings segment for which is global MapCoordinate is going to be calculated and returned.
        */
        MapCoordinate getFieldCoordinate( const MapCoordinate& entryOnMap, const LocalCoordinate& localCoordinate );


        void read ( tnstream& stream ) ;
        void write ( tnstream& stream ) const ;
        void runTextIO ( PropertyContainer& pc );

};


#endif
