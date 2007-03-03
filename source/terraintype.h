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

#ifndef terraintypeH
 #define terraintypeH

 #include "graphics/surface.h"
 #include "typen.h"
 #include "mapitemtype.h"

 class OverviewMapImage;

 //! the number of bits that specify the terrain of a field
 const int terrainPropertyNum = 37;


  //! the properties of a terrain describing which units can move onto this field and which can't
 class TerrainBits : public BitSet {
  public:
   void setInt ( int terrain1 = 0, int terrain2 = 0 );
   void read ( tnstream& stream );
   void write ( tnstream& stream ) const;
 };

 //! This class is used by buildings, vehicles and objects to specify which terrain it can move to / be built on
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
       int accessible ( const TerrainBits& bts ) const;

       void read ( tnstream& stream );
       void write ( tnstream& stream ) const;
       void runTextIO ( PropertyContainer& pc );

 };


 typedef class TerrainType* pterraintype;

 //! The type of a field
 class TerrainType : public MapItemType, public LoadableItemType {
    public:
      int                id;
      int                getID() const { return id; };
      ASCString          name;
      ASCString          getName() const { return name; };
      
      class MoveMalus: public vector<int> {
         public: MoveMalus();
           #ifdef _vector_at_broken_
            int& at(size_t pos) { return operator[](pos); };
            const int& at(size_t pos) const { return operator[](pos); };
           #endif
            // void read ( tnstream& stream ) { read ( stream, -1 ); };
            void read ( tnstream& stream, int defaultValue, int moveMalusCount = -1 );
            void write ( tnstream& stream ) const;
      };

      class  Weather: public LoadableItemType {
          //! the color information for the small map
          OverviewMapImage* quickView;
        public:
          //! the image of the field
          Surface        image;

          //! the defense bonus for the unit standing on this field. \see AttackFormula::defense_defensebonus(int)
          int            defensebonus;

          //! the attack bonus for the unit standing on this field. \see AttackFormula::strength_attackbonus(int)
          int            attackbonus;

          //! the view obstraction of the field
          int            basicjamming;

          //! the movement cost for the various units to move across this field
          TerrainType::MoveMalus   move_malus;

          //! displays the image on the screen coordinates x1/y1
          void           paint ( Surface& s, SPoint pos );
          
          
          //! the image index from the graphic set. -1 if graphics is not from graphic set. \see  GraphicSet
          int            bi_pict;

          //! the properties defining which unit can move onto this field and which not
          TerrainBits    art;

          //! pointer to the outer structure
          TerrainType*   terraintype;

          const OverviewMapImage* getQuickView();

          Weather ( TerrainType* base ) : quickView ( NULL ), terraintype ( base ) {};
          ~Weather();
          void runTextIO ( PropertyContainer& pc );
          void read ( tnstream& stream );
          void read ( tnstream& stream, int version );
          void write ( tnstream& stream ) const;

      };
     Weather*           weather[cwettertypennum];

     TerrainType();
     void runTextIO ( PropertyContainer& pc );
     void read ( tnstream& stream );
     void write ( tnstream& stream ) const;
     ~TerrainType();
 };


 extern const char*  terrainProperty[]  ;

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
