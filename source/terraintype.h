/***************************************************************************
                          terraintype.h  -  description
                             -------------------
    begin                : Fri Jul 27 2001
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

#ifndef terraintype_h_included
 #define terraintype_h_included

 #include "typen.h"

  //! the properties of a terrain describing which units can move onto this field and which can't
 class TerrainBits : public BitSet {
  public:
   void setInt ( int terrain1 = 0, int terrain2 = 0 );
   void read ( tnstream& stream );
   void write ( tnstream& stream ) const;
 };

 //! This class is used by buildings, vehicles and objects to specify which terrain it can move to
 class TerrainAccess {
    public:
       TerrainAccess ( void ) ;

       //! at least one of these bits must match on of the terrain
       TerrainBits  terrain;

       //! ALL these bits must be set in the terrain
       TerrainBits  terrainreq;

       //! if one of these bits is set, the field will NOT be accessible
       TerrainBits  terrainnot;

       //! if a terrain is not accessible AND one of these bits is matched, the unit will be destroyed
       TerrainBits  terrainkill;

       /** checks whether a field with the given terrainbits is accessible.
            \returns 1 if the field is accessible;
                      0 if it is not accessible
                      -1 if it is not accessible and the unit is killed by it    */
       int accessible ( const TerrainBits& bts );

       void read ( tnstream& stream );
       void write ( tnstream& stream ) const;
       void runTextIO ( PropertyContainer& pc );

 };


 typedef class TerrainType* pterraintype;

 //! The type of a field
 class TerrainType {
    public:
      class  Weather {
        public:
          void*          pict;
          int            defensebonus;
          int            attackbonus;
          int            basicjamming;
          vector<int>    move_malus;
          void           paint ( int x1, int y1 );
          int            bi_pict;
          TerrainBits    art;

          TerrainType*   terraintype;
          pquickview     quickview;

          Weather ( TerrainType* base ) : terraintype ( base ), quickview ( NULL ), pict( NULL ) {};
          void runTextIO ( PropertyContainer& pc );
      };
     int                id;
     ASCString          name;
     Weather*           weather[cwettertypennum];

     //! The filename of the terrain
     ASCString          fileName;

     //! The filename and location on disk (including containerfiles) of the terrain. Can only be used for informational purposes
     ASCString          location;

     TerrainType();
     void runTextIO ( PropertyContainer& pc );
     void read ( tnstream& stream );
     void write ( tnstream& stream ) const;
 };


 extern const char*  cbodenarten[]  ;

 enum TerrainBitTypes { cbwater0 ,
                     cbwater1 ,
                     cbwater2 ,
                     cbwater3 ,
                     cbwater ,
                     cbstreet ,
                     cbrailroad ,
                     cbbuildingentry ,
                     cbharbour ,
                     cbrunway ,
                     cbpipeline ,
                     cbpowerline ,
                     cbfahrspur ,
                     cbfestland ,
                     cbsnow1 ,
                     cbsnow2 ,
                     cbhillside ,
                     cbsmallrocks ,
                     cblargerocks ,
                     cbfrozenwater,
                     cbicebreaking };

 TerrainBits getTerrainBitType ( TerrainBitTypes tbt );


#endif