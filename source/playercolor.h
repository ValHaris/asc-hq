/***************************************************************************
                          gamemap.h  -  description
                             -------------------
    begin                : Tue Feb 17 2001
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


#ifndef playercolorH
 #define playercolorH
 
#include "graphics/surface.h"
 
 class PlayerColor {
      int playernumber;
      DI_Color color;
    public:
       PlayerColor(int number, const DI_Color& col );
       int getNum() const { return playernumber; };
       DI_Color getColor() const { return color; };
 };
 
#endif 

