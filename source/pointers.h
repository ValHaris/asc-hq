/***************************************************************************
                          pointers.h  -  description
                             -------------------
    begin                : Tue Jan 23 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file pointers.h
    \brief Some pointer definitions to central classes for usage as forward 
           declarations 
*/


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef pointers_h_included
 #define pointers_h_included

 typedef class TerrainAccess *pterrainaccess;
 typedef class ObjectType* pobjecttype;
 typedef class tmap*  pmap;
 typedef class tmap Map;
 typedef class tevent* pevent ;
 typedef class tresearch* presearch ;
 typedef char* pchar;
 typedef class tbasenetworkconnection* pbasenetworkconnection;
 typedef class tnetwork* pnetwork;
 typedef struct tquickview* pquickview;
 typedef class  tfield* pfield ;
 typedef class  Object* pobject;
 typedef class  BuildingType* pbuildingtype;
 class Vehicletype;
 class PropertyContainer;
 typedef class  Vehicletype*  pvehicletype ;

#endif
