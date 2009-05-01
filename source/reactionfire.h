/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING. If not, write to the 
    Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
    Boston, MA  02111-1307  USA
*/

#if defined(karteneditor) 
 #error the mapeditor should not need to use reactionfire.h !
#endif


#ifndef reactionfireH
#define reactionfireH

#include "typen.h"
#include "astar2.h"
#include "actions/context.h"

class MapDisplayInterface;


class treactionfire {
          public:
             virtual int  checkfield ( const MapCoordinate3D& pos, Vehicle* &eht, const Context& context ) = 0;
             virtual void init ( Vehicle* eht, const AStar3D::Path&  fieldlist ) = 0;
             virtual int  finalCheck ( int currentPlayer, const Context& context ) = 0;
             virtual ~treactionfire() {};
        };

        
class tsearchreactionfireingunits : public treactionfire {
           private: 
                void initLimits();
                void findOffensiveUnits( Vehicle* vehicle, int height, int x1, int y1, int x2, int y2 );

                GameMap* gamemap;
           protected:
                int attack( Vehicle* attacker, Vehicle* target, const Context& context );

                static int maxshootdist[8];     // f?r jede Hhenstufe eine
                void addunit ( Vehicle* vehicle );
                void removeunit ( Vehicle* vehicle );
                
                typedef map<const Vehicle*,int> VisibleUnits;
                VisibleUnits visibleUnits;
                
           public:

                tsearchreactionfireingunits( GameMap* gamemap );
                void init ( Vehicle* eht, const AStar3D::Path& fieldlist );
                void init ( Vehicle* eht, const MapCoordinate3D& pos );
                int  checkfield ( const MapCoordinate3D& pos, Vehicle* &eht, const Context& context  );
                virtual int  finalCheck ( int currentPlayer, const Context& context );
                ~tsearchreactionfireingunits();
      };

  
    

#endif
