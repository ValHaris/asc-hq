/***************************************************************************
                          itemrepository.h  -  description
                             -------------------
    begin                : Thu Jul 28 2001
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

#ifndef itemrepository_h_included
#define itemrepository_h_included

#include <vector>
#include "ascstring.h"
#include "typen.h"
#include "terraintype.h"
#include "vehicletype.h"
#include "objecttype.h"
#include "buildingtype.h"

extern pterraintype getterraintype_forid ( int id );
extern pobjecttype getobjecttype_forid ( int id );
extern pvehicletype getvehicletype_forid ( int id );
extern pbuildingtype getbuildingtype_forid ( int id );
extern ptechnology gettechnology_forid ( int id );

extern pterraintype getterraintype_forpos ( int pos );
extern pobjecttype getobjecttype_forpos ( int pos );
extern pvehicletype getvehicletype_forpos ( int pos );
extern pbuildingtype getbuildingtype_forpos ( int pos );
extern ptechnology gettechnology_forpos ( int pos );

extern void addterraintype ( pterraintype bdt );
extern void addobjecttype ( pobjecttype obj );
extern void addvehicletype ( pvehicletype vhcl );
extern void addbuildingtype ( pbuildingtype bld );
extern void addtechnology ( ptechnology tech );


typedef dynamic_array<pvehicletype> VehicleTypeVector;
extern VehicleTypeVector& getvehicletypevector ( void );

typedef dynamic_array<pterraintype> TerrainTypeVector;
extern TerrainTypeVector& getterraintypevector ( void );

typedef dynamic_array<pbuildingtype> BuildingTypeVector;
extern BuildingTypeVector& getbuildingtypevector ( void );

typedef dynamic_array<pobjecttype> ObjectTypeVector;
extern ObjectTypeVector& getobjecttypevector ( void );

extern void  loadalltextfiles();
extern void  loadallbuildingtypes(void);
extern void  loadallvehicletypes(void);
extern void  loadallterraintypes(void);
extern void  loadalltechnologies(void);
extern void  loadallobjecttypes ( void );
extern void  freetextdata();

extern pobjecttype streetobjectcontainer ;
extern pobjecttype pathobject;
extern pobjecttype railroadobject ;
extern pobjecttype eisbrecherobject;
extern pobjecttype fahrspurobject;

extern int  terraintypenum;
extern int  vehicletypenum;
extern int  buildingtypenum;
extern int  technologynum;
extern int  objecttypenum;

typedef map<ASCString,TextPropertyList> TextFileRepository;
extern TextFileRepository textFileRepository;


#endif
