/***************************************************************************
                          objecttype.h  -  description
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

#ifndef objecttypeH
 #define objecttypeH

 #include "typen.h"
 #include "terraintype.h"


 //! An object that can be placed on fields. Roads, pipelines and ditches are examples of objects. \sa Object
 class ObjectType : public LoadableItemType {
   public:
     //! the id of the object, used when referencing objects in files
     int id;

     //! the group ID allows units to specify whole groups of objects to be buildable / removable without specifying each individual object.
     int groupID;

     //! bitmapped variable containing the different weather types the objects exist for
     BitSet weather;

     //! is the object displayed under fog of war
     bool visibleago;

     //! some objects are graphically linked with others on neighbouring fields. \sa no_autonet linkableTerrain
     vector<int> linkableObjects;

     //! objects can be graphically linked to terrain, as if the terrain was an object of the same type. \sa no_autonet linkableObjects
     vector<int> linkableTerrain;

     //! if an object should not be attackable, set armor to 0
     int armor;

     //! the movemalus_plus is added to the current movemalus of the field to form the new movemalus. Negative values are ok.
     vector<int> movemalus_plus;

     //! The movemalus_abs replaces the current movemalus of the field by a new one. Values of 0 and -1 won't affect the movemalus of the field, and values ranging from 1 to 9 must not be used.
     vector<int> movemalus_abs;

     //! this is added to the current attackbonus of the field to form the new attackbonus.
     int attackbonus_plus;
     //! The attackbonus_abs replaces the current attackbonus of the field by a new one. A value of -1 won't affect the attackbonus of the field
     int attackbonus_abs;

     //! this is added to the current defensebonus of the field to form the new defensebonus.
     int defensebonus_plus;
     //! The defensebonus_abs replaces the current defensebonus of the field by a new one. A value of -1 won't affect the attackbonus of the field
     int defensebonus_abs;

     //! this is added to the current basicjamming of the field to form the new jamming.
     int basicjamming_plus;
     //! basicjamming_abs replaces the current basicjamming of the field by a new one. A value < 0 won't affect the jamming of the field
     int basicjamming_abs;

     //! the level of height the object is on. This is not the simple system of 8 levels used for units and building, but one with 255 levels which are documented in docs/biimport.html
     int height;

     //! The resources required to construct the object with a unit; Note that units usually don't have any energy available
     Resources buildcost;

     //! The resources required to remove the object with a unit; Note that units usually don't have any energy available
     Resources removecost;

     //! The movement points that are needed to build this object
     int build_movecost;

     //! The movement points that are needed to remove this object
     int remove_movecost;

     //! if a building is constructed on the field containing the object, will the object be removed or will it stay beneath the building.
     bool canExistBeneathBuildings;

     //! The name of the object
     ASCString name;

     static const int netBehaviourNum = 6;
     enum NetBehaviour { NetToBuildings = 1, NetToBuildingEntry = 2, NetToSelf = 4, NetToBorder = 8, SpecialForest = 0x10, AutoBorder = 0x20 };

     //! specifies how the object is going to connect to other things
     int netBehaviour;

     //! The terrain on which this object can be placed
     TerrainAccess terrainaccess;

     //! the terrain properties of the field will be AND-masked with this field and then OR-masked with terrain_or to form the new terrain properties
     TerrainBits terrain_and;
     TerrainBits terrain_or;

     ObjectType ( void );

     //! the icon used for selecting the object when executing the "build object" function of a unit. The image is automatically generated at load time
     void* buildicon;
     //! the icon used for selecting the object when executing the "remove object" function of a unit. The image is automatically generated at load time
     void* removeicon;

     //! direction lists were an attempt to allow the graphical connection of this object with neighbouring ones without having an image for each possible connection layout. The attempt failed. Don't use it any more.
     int* dirlist;
     int dirlistnum;

     //! the images of the objects
     struct WeatherPicture {
        vector<void*> images;
        vector<int>   bi3pic;
        vector<int>   flip;
        void resize(int i) { flip.resize(i); bi3pic.resize(i); images.resize(i); };
     } weatherPicture [cwettertypennum];

     //! displays the objecttype at x/y on the screen
     void display ( int x, int y );
     void display ( int x, int y, int dir, int weather = 0 );

     //! returns the pointer to the image i
     void* getpic ( int i, int weather = 0 );

     //! can the object be build on the field fld
     bool buildable ( pfield fld );

     //! reads the objecttype from a stream
     void read ( tnstream& stream );
     //! write the objecttype from a stream
     void write ( tnstream& stream ) const;

     //! reads or writes the objecttype to the text stream pc
     void runTextIO ( PropertyContainer& pc );

     //! some objects require special displaying methods, for example shading the terrain they are build on
     int displayMethod;

   private:
     //! the loading functions call this method to setup the objects images
     void setupImages();

 };

const int objectDisplayingMethodNum = 3;

namespace ForestCalculation {
  //! automatically adjusting the pictures of woods and coasts to form coherent structures
  extern void smooth ( int what, pmap gamemap, pobjecttype woodObj );
};


#endif
