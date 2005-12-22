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

class MapDisplayInterface;


struct treactionfire_replayinfo {
         int x1 ;
         int y1 ;
         int x2 ;
         int y2 ;
         int ad1 ;
         int ad2 ;
         int dd1 ;
         int dd2 ;
         int wpnum;
};
typedef treactionfire_replayinfo* preactionfire_replayinfo;





class treactionfire {
          public:
             virtual int  checkfield ( const MapCoordinate3D& pos, Vehicle* &eht, MapDisplayInterface* md ) = 0;
             virtual void init ( Vehicle* eht, const AStar3D::Path&  fieldlist ) = 0;
             virtual ~treactionfire() {};
        };

class treactionfirereplay : public treactionfire {
          protected:
             int num;
             dynamic_array<preactionfire_replayinfo> replay;
             Vehicle* unit;
          public:
             treactionfirereplay ( void );
             ~treactionfirereplay ( );
             virtual int checkfield ( const MapCoordinate3D& pos, Vehicle* &eht, MapDisplayInterface* md );
             virtual void init ( Vehicle* eht, const AStar3D::Path& fieldlist );
   };

class tsearchreactionfireingunits : public treactionfire {
           protected:


                static int maxshootdist[8];     // f?r jede H”henstufe eine
                void addunit ( Vehicle* vehicle );
                void removeunit ( Vehicle* vehicle );
           public:

                tsearchreactionfireingunits( void );
                void init ( Vehicle* eht, const AStar3D::Path& fieldlist );
                int  checkfield ( const MapCoordinate3D& pos, Vehicle* &eht, MapDisplayInterface* md );
                ~tsearchreactionfireingunits();
      };

  
    

#endif
