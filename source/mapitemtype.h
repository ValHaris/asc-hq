/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef mapitemtypeH
 #define mapitemtypeH

 #include "ascstring.h"
 
//! The base class for everything that can be placed on a map field
class MapItemType {
   public:
       virtual ASCString getName() const = 0;
       virtual int getID() const = 0;      
       virtual ~MapItemType() {};
};

#endif
