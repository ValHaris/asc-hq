/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef movementtestH
#define movementtestH

#include "../ascstring.h"
#include "../errors.h"

extern GameMap* startMap( const ASCString& filename );

class TestFailure : public ASCmsgException {
   public:
      TestFailure( const ASCString& message ) : ASCmsgException (message ){
      }
};

extern Context createTestingContext( GameMap* gamemap );

#endif