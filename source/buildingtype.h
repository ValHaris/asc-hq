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

#ifndef buildingtype_h_included
 #define buildingtype_h_included

 #include "typen.h"
 #include "containerbase.h"
 #include "ascstring.h"
 #include "terraintype.h"
 #include "objecttype.h"
 #include "basestreaminterface.h"
 #include "textfileparser.h"


const int cbuildingfunctionnum = 18;
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



 class  BuildingType : public ContainerBaseType {
   public:
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
        int          destruction_objects [4][6];

        LocalCoordinate entry;

        int          id;


        ASCString    fileName;
        ASCString    location;

        int          _armor;

        int          jamming;
        int          view;

        //! the maximum size of a unit that may enter the building
        int          loadcapacity;

        //! bitmapped: only units on one of these levels of height may enter
        int          loadcapability;

        //! bitmapped: only units that are able to reach one of these levels of height may enter. For example, an airport may have "low level flight" set here. That does not mean that the units must FLY into the building!
        int          unitheightreq;

        //! bitmapped: units that are able to reach one of these levels of height may NEVER enter this building
        int          unitheight_forbidden;

        //! bitmapped: vehicle categories that may enter the building
        int          vehicleCategoriesLoadable;


        int          special;   /*  HQ, Trainingslager, ...  */

        int          technologylevel;
        int          researchid;

        TerrainAccess terrainaccess;

        int          construction_steps;  // 1 .. 8
        int          maxresearchpoints;

        Resources    _tank;
        Resources    maxplus;

        int          efficiencyfuel;       // Basis 1024
        int          efficiencymaterial;   // dito

        //! the picture for the GUI that is used for selecting a building that is going to be constructed by a unit
        void*        guibuildicon;

        Resources    _bi_maxstorage;

        //! bitmapped: the level of height that this building will reside on.
        int          buildingheight;

        //! bitmapped: units on these levels of height may be refuelled when standing next to the buildings entry
        int          externalloadheight;

        void*        getpicture ( const LocalCoordinate& localCoordinate );

        BuildingType ( void );

        bool          vehicleloadable ( pvehicletype fzt ) const ;
        MapCoordinate getFieldCoordinate( const MapCoordinate& entryOnMap, const LocalCoordinate& localCoordinate );

        void read ( tnstream& stream ) ;
        void write ( tnstream& stream ) const ;
        void runTextIO ( PropertyContainer& pc );

};


#endif
