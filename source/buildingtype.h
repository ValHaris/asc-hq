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



 class  Buildingtype : public ContainerBaseType {
   public:
        void*        w_picture [ cwettertypennum ][ maxbuildingpicnum ][4][6];
        int          bi_picture [ cwettertypennum ][ maxbuildingpicnum ][4][6];
        int          destruction_objects [4][6];
        struct {
          int     x, y;
        } entry, powerlineconnect, pipelineconnect;
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
        void*        getpicture ( int x, int y );

        Buildingtype ( void ) {
           terrain_access = &terrainaccess;
           vehicleCategoriesLoadable = -1;
        };

        int          vehicleloadable ( pvehicletype fzt ) const ;
        void getfieldcoordinates( int bldx, int bldy, int x, int y, int *xx, int *yy);
};


class  Building : public ContainerBase {
    int lastenergyavail;
    int lastmaterialavail;
    int lastfuelavail;
  public:
    pbuildingtype     typ;
    int               munitionsautoproduction[waffenanzahl];
    unsigned char     color;
    pvehicletype      production[32];

    Resources   plus;
    Resources   maxplus;

    Resources   actstorage;

    int         munition[waffenanzahl];

    word         maxresearchpoints;
    word         researchpoints;

    ASCString    name;
    Integer      xpos, ypos;
    pbuilding    next;
    pbuilding    prev;
    char         completion;
    int          netcontrol;
    int          connection;
    char         visible;
    pvehicletype  productionbuyable[32];

    Resources    bi_resourceplus;

    int           repairedThisTurn;

    AiValue*      aiparam[8];

    Building( pmap actmap = NULL );
    Building ( pbuilding src, pmap actmap );
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

  private:
    int  processmining ( int res, int abbuchen );

  public:
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

    void execnetcontrol ( void );
    int  getmininginfo ( int res );

    int  putResource ( int amount,    int resourcetype, int queryonly, int scope = 1 );
    int  getResource ( int amount,    int resourcetype, int queryonly, int scope = 1 );
    Resources putResource ( const Resources& res, int queryonly, int scope = 1 ) { return ContainerBase::putResource ( res, queryonly, scope ); };
    Resources getResource ( const Resources& res, int queryonly, int scope = 1 ) { return ContainerBase::getResource ( res, queryonly, scope ); };

    void getresourceusage ( Resources* usage );
    void changecompletion ( int d );
    int vehicleloadable ( pvehicle eht, int uheight = -1 ) const;
    void* getpicture ( int x, int y );
    void convert ( int col );
    void addview ( void );
    void removeview ( void );
    int  chainbuildingtofield ( int x, int y );
    int  unchainbuildingfromfield ( void );
    int  gettank ( int resource );
    int  getArmor( void );
    pfield getField( int  x, int y);
    pfield getEntryField ( );
    MapCoordinate getEntry ( );
    void  getFieldCoordinates( int x, int y, int &xx, int &yy);
    void produceAmmo ( int type, int num );

    void resetPicturePointers ( void );
    MapCoordinate getPosition ( ) { return getEntry(); };

    void getpowerplantefficiency ( int* material, int* fuel );


  protected:
     ResourceMatrix repairEfficiency;
     const ResourceMatrix& getRepairEfficiency ( void ) { return repairEfficiency; };
};

#define compensatebuildingcoordinateorgx (a) (dx & (~a))
#define compensatebuildingcoordinatex ( + (dx & ~b) )


#endif
