/***************************************************************************
                          buildings.h  -  description
                             -------------------
    begin                : Sat Feb 17 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file buildingtype.h
    \brief The interface for the Building class
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef buildings_h_included
 #define buildings_h_included

 #include "typen.h"
 #include "containerbase.h"
 #include "ascstring.h"
 #include "buildingtype.h"



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
    void setCompletion ( int completion, bool setupImages = true  );

    ~Building();

  protected:
     ResourceMatrix repairEfficiency;
     const ResourceMatrix& getRepairEfficiency ( void ) { return repairEfficiency; };
};


#endif
