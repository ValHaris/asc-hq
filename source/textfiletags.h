/***************************************************************************
                          textfiletags.h  -  description
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

#ifndef textfiletagsH
 #define textfiletagsH

 extern const char* weatherTags[];
 extern const char* terrainProperties[];
 extern const char* weatherAbbrev[];
 extern const char* heightTags[];
 extern const char* vehicleAbilities[]; //!< deprecated
 extern const char* weaponTags[];
 extern const char* unitCategoryTags[];
 extern const char* buildingFunctionTags[]; //!< deprecated
 extern const char* containerFunctionTags[];
 extern const char* objectNetMethod[];
 extern const char* entranceModes[];

 const int productionCostCalculationMethodNum = 3;
 extern const char* productionCostCalculationMethod[];

 const int graphicOperationNum = 3;
 extern const char* graphicOperations[];
 

 extern const char* objectDisplayingMethodTags[];

 extern const char* AItasks[];
 extern const char* AIjobs[];

#endif

