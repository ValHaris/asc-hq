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

 #pragma pack(1)


class  Building : public ContainerBase {

    int  processmining ( int res, int abbuchen );

    MapCoordinate entryPosition;

    char         _completion;

    //! a structure to hold temporary values when the building is executing its work at the end of a turn
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

    friend class tprocessminingfields;

  public:
    const pbuildingtype typ;

    pvehicletype      production[32];

    //! the Resources that are produced each turn
    Resources   plus;

    //! the maximum amount of Resources that the building can produce each turn in the ASC resource mode ; see also #bi_resourceplus
    Resources   maxplus;

    //! the current storage of Resources
    Resources   actstorage;

    //! the ammo that is stored in the building
    int         ammo[waffenanzahl];

    //! the maximum amount of research that the building can conduct every turn
    word         maxresearchpoints;

    //! the current amount of research that the building conducts every turn
    word         researchpoints;

    //! the building's name
    ASCString    name;

    //! a bitmapped variable containing the status of the resource-net connection
    int          netcontrol;

    //! bitmapped: are there events that are triggered by actions affecting this building
    int          connection;

    //! is the building visible? Building can be made invisible, but this feature should be used only in some very special cases
    bool         visible;

    //! the vehicletype that the building can produce
    pvehicletype  productionbuyable[32];

    //! the maximum amount of Resources that the building can produce each turn in the BI resource mode ; see also #maxplus
    Resources    bi_resourceplus;

    //! the percantage that this build has already been repaired this turn. The maximum percentage may be limited by a gameparameter
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

    pfield getEntryField ( ) const;
    MapCoordinate3D getEntry ( ) const;

    pfield getField( const BuildingType::LocalCoordinate& localCoordinates ) const;
    MapCoordinate getFieldCoordinates( const BuildingType::LocalCoordinate& localCoordinates ) const;

    //! produces ammunition and stores it in #ammo
    void produceAmmo ( int type, int num );

    //! updates the pointers to the pictures , which are part of tfield (to speed up displaying)
    void resetPicturePointers ( void );
    MapCoordinate3D getPosition ( ) { return getEntry(); };
    int  chainbuildingtofield ( const MapCoordinate& entryPos, bool setupImages = true );
    int  unchainbuildingfromfield ( void );

    void getpowerplantefficiency ( int* material, int* fuel );

    int getCompletion() const { return _completion; };
    void setCompletion ( int completion, bool setupImages = true  );

    //! returns a name for the building. If the building itself has a name, it will be returned. If it doesn't, the name of the building type will be returned.
    const ASCString& getName ( ) const;

    //! returns the amount of resources that the net which the building is connected to produces each turn
    Resources netResourcePlus( ) const;

    ~Building();

  protected:
     ResourceMatrix repairEfficiency;
     const ResourceMatrix& getRepairEfficiency ( void ) { return repairEfficiency; };
     virtual void postRepair ( int oldDamage ) {};
};

 #pragma pack()

#endif
