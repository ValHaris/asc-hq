/***************************************************************************
                          buildingtype.h  -  description
                             -------------------
    begin                : Fri Sep 29 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file buildingtype.h
    \brief The interface for buildings and buildingtypes
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
        };


        void*        w_picture [ cwettertypennum ][ maxbuildingpicnum ][4][6];
        int          bi_picture [ cwettertypennum ][ maxbuildingpicnum ][4][6];
        int          destruction_objects [4][6];
        LocalCoordinate entry;
        LocalCoordinate powerlineconnect;
        LocalCoordinate pipelineconnect;
        int          id;
        char*        name;
        int          _armor;
        int          jamming;
        int          view;
        int          loadcapacity;
        char         loadcapability;   /*  BM => CHoehenstufen; aktuelle Hoehe der reinzufahrenden vehicle
                                                                muss hier enthalten sein  */
        char         unitheightreq;   /*   "       , es d?rfen nur Fahrzeuge ,
                                                     die in eine dieser Hoehenstufen koennen , geladen werden  */
        int          special;   /*  HQ, Trainingslager, ...  */

        unsigned char         technologylevel;
        unsigned char         researchid;

        tterrainaccess terrainaccess;

        int          construction_steps;  // 1 .. 8
        int          maxresearchpoints;

        Resources   _tank;
        Resources   maxplus;

        int          efficiencyfuel;       // Basis 1024
        int          efficiencymaterial;   // dito

        void*        guibuildicon;

        pterrainaccess terrain_access;
        Resources    _bi_maxstorage;
        int          buildingheight;
        int          unitheight_forbidden;
        int          externalloadheight;
        int          vehicleCategoriesLoadable;
        void*        getpicture ( const LocalCoordinate& localCoordinate );

        BuildingType ( void ) {
           terrain_access = &terrainaccess;
           vehicleCategoriesLoadable = -1;
        };

        int          vehicleloadable ( pvehicletype fzt ) const ;
        MapCoordinate getFieldCoordinate( const MapCoordinate& entryOnMap, const LocalCoordinate& localCoordinate );
};


class  Building : public ContainerBase {
    int lastenergyavail;
    int lastmaterialavail;
    int lastfuelavail;

    int  processmining ( int res, int abbuchen );

    MapCoordinate entryPosition;

  public:
    const pbuildingtype typ;
    int               munitionsautoproduction[waffenanzahl];
    pvehicletype      production[32];

    Resources   plus;
    Resources   maxplus;

    Resources   actstorage;

    int         munition[waffenanzahl];

    word         maxresearchpoints;
    word         researchpoints;

    ASCString    name;
    char         _completion;
    int          netcontrol;
    int          connection;
    char         visible;
    pvehicletype  productionbuyable[32];

    Resources    bi_resourceplus;

    int           repairedThisTurn;

    AiValue*      aiparam[8];

    Building( pmap map, const MapCoordinate& entryPosition, const pbuildingtype type , int player, bool setupImages = true );

    int lastmineddist;
                                      /*
                                       int  getenergyplus( int mode );  // mode ( bitmapped ) : 1 : maximale energieproduktion ( ansonsten das, was gerade ins netz reingeht )
                                       //                      2 : windkraftwerk
                                       //                      4 : solarkraftwerk
                                       //                      8 : konventionelles Kraftwerk
                                       //                     16 : mining station
                                       int  getmaterialplus( int mode );  // mode ( bitmapped ) : 1 : maximale energieproduktion ( ansonsten das, was gerade ins netz reingeht )
                                       int  getfuelplus( int mode );  // mode ( bitmapped )     : 1 : maximale energieproduktion ( ansonsten das, was gerade ins netz reingeht )
                                      */
    int  getresourceplus ( int mode, Resources* plus, int queryonly ); // returns a positive value when the building did actually something
    void initwork ( void );
    int  worktodo ( void );
    int  processwork ( void );    // returns a positive value when the building did actually something

    bool canRepair ( void );

    struct Work {
       struct Mining {
          Resources touse;
          int did_something_atall;
          int did_something_lastpass;
          int finished;
       } mining;
       struct Resource_production {
          Resources toproduce;
          int did_something_atall;
          int did_something_lastpass;
          int finished;
       } resource_production;
       int wind_done;
       int solar_done;
       int bimode_done;
    } work;

    static Building* newFromStream ( pmap gamemap, tnstream& stream );
    void write ( tnstream& stream, bool includeLoadedUnits = true );
    void read ( tnstream& stream );
  private:
    void readData ( tnstream& stream, int version );
  public:

    void execnetcontrol ( void );
    int  getmininginfo ( int res );

    int  putResource ( int amount,    int resourcetype, int queryonly, int scope = 1 );
    int  getResource ( int amount,    int resourcetype, int queryonly, int scope = 1 );
    Resources putResource ( const Resources& res, int queryonly, int scope = 1 ) { return ContainerBase::putResource ( res, queryonly, scope ); };
    Resources getResource ( const Resources& res, int queryonly, int scope = 1 ) { return ContainerBase::getResource ( res, queryonly, scope ); };

    void getresourceusage ( Resources* usage );
    int vehicleloadable ( pvehicle eht, int uheight = -1 ) const;
    void* getpicture ( const BuildingType::LocalCoordinate& localCoordinate );
    void convert ( int col );
    void addview ( void );
    void removeview ( void );
    int  gettank ( int resource );
    int  getArmor( void );

    pfield getEntryField ( );
    MapCoordinate getEntry ( );

    pfield getField( const BuildingType::LocalCoordinate& localCoordinates );
    MapCoordinate getFieldCoordinates( const BuildingType::LocalCoordinate& localCoordinates );

    void produceAmmo ( int type, int num );

    void resetPicturePointers ( void );
    MapCoordinate getPosition ( ) { return getEntry(); };
    int  chainbuildingtofield ( const MapCoordinate& entryPos, bool setupImages = true );
    int  unchainbuildingfromfield ( void );

    void getpowerplantefficiency ( int* material, int* fuel );

    int getCompletion() const { return _completion; };
    void setCompletion ( int d );

    ~Building();

  protected:
     ResourceMatrix repairEfficiency;
     const ResourceMatrix& getRepairEfficiency ( void ) { return repairEfficiency; };
};


#endif
