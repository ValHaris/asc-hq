/*! \file weather.h
    \brief Interface for changing the weather and the terrain on the map
*/

//     $Id: weather.h,v 1.5 2001-01-28 14:04:22 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.4  2000/04/27 16:25:31  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.3  1999/12/28 21:03:29  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.2  1999/11/16 03:42:52  tmwilson
//     	Added CVS keywords to most of the files.
//     	Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//     	Wrote replacement routines for kbhit/getch for Linux
//     	Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//     	Added autoconf/automake capabilities
//     	Added files used by 'automake --gnu'
//
//
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

#ifndef weather_h
 #define weather_h

#include "libs/triangul/interfac.h"
#include "libs/triangul/triangul.h"

#include "typen.h"

#pragma pack(1)

struct tpunkt {
                int x,y;
              };

typedef struct tpolygon* ppolygon;
struct tpolygon {
                   int     vertexnum;
                   tpunkt* vertex;
                };



class  tfillpolygon {
         public:
             virtual int    paint_polygon   ( ppolygon poly );
         protected:
             virtual void   sortpolygon    ( ppolygon a );
             virtual void   painttriangle  ( tpunkt p[] );
             virtual void   painthorizline ( int x1, int x2, int y );
             virtual void   paintline      ( int x1, int y1, int x2, int y2 );
             virtual void   setpoint       ( int x,  int y  );
             virtual int    getpolydir     ( ppolygon a);
             virtual double getsegmentdir  ( int dx, int dy );
             virtual int    checkpolygon   ( ppolygon poly );
             virtual int    checkline      ( tpunkt a, tpunkt b, tpunkt d, tpunkt e );
          };

class  tfillpolygonsquarecoord : public tfillpolygon {
        protected:
             virtual void setpoint    ( int x,  int y  );
             virtual void setpointabs ( int x,  int y  )=0;
        public:
             void setspecificpoint ( int x, int y ) { setpointabs ( x, y ); };
             virtual int    paint_polygon   ( ppolygon poly );
          };

class  tfillpolygonweather : public tfillpolygonsquarecoord {
        public:
             virtual void initweather ( int wt, int it );
        protected:
             virtual void setpointabs ( int x,  int y  );
             int weather;
             int intensity;
           };

class  tfillpolygonbdt : public tfillpolygonsquarecoord {
        public:
             virtual void initbdt ( int id );
        protected:
             virtual void setpointabs ( int x,  int y  );
             pterraintype bdt;
           };

void setweatherpoly ( int weather, int intensity, ppolygon poly );
void setweatherall ( int weather, int intensity );
void setweatherfields ( int weather, int intensity, int fieldnum, int* coordinates );
void setbdtfields ( int id, int fieldnum, int* coordinates );
void setbdtall ( int id );
void setbdtpoly ( int id, ppolygon poly1 );
void displaywindspeed(void);

#pragma pack()

#endif //weather_h