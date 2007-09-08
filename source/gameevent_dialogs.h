/*! \file gameevent_dialogs.h 
    \brief Interface to game event dialogs
*/

//     $Id: gameevent_dialogs.h,v 1.6 2007-09-08 14:13:51 mbickel Exp $

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


#ifndef gameevent_dialogsH
#define gameevent_dialogsH

#include "mappolygons.h"
#include "gameevents.h"
#include "dlg_box.h"
#include "research.h"

#include "dialogs/fieldmarker.h"

    //! checks, which vehicle types are newly available
    class NewVehicleTypeDetection  {
                    bool*  buf;
                public:
                    NewVehicleTypeDetection( );

                    //! checks, which vehicle are now available that where not available when initbuffer was called. The new ones are displayed by calling #tshownewtanks
                    void evalbuffer( void );

                    ~NewVehicleTypeDetection();
               };

     //! displays a list of vehicles that are newly available
     class   tshownewtanks : public tdialogbox {
                         public:
                           void          init ( bool*      buf2 );
                           virtual void  run  ( void );
                           virtual void  buttonpressed ( int id );
                         protected:
                           bool*      buf;
                           int           status;
                        };


extern void showtechnology( const Technology* t);
extern void selectFields( FieldAddressing::Fields& fields );
extern bool chooseWeather( int& weather );
extern bool chooseTerrain( int& terrainID );
extern bool chooseObject ( int& objectID );
extern void editpolygon (Poly_gon& poly);
extern void getxy_building(int *x,int *y);
extern int selectunit ( int unitnetworkid );
extern void playerselall( int *playerbitmap);
extern bool chooseVehicleType( int& vehicleTypeID );

class ReinforcementSelector : public SelectFromMap {
   private:
      tmemorystreambuf& buf;
      int& objectNum;
   protected:
      bool mark();
   public:
      ReinforcementSelector( CoordinateList& list, GameMap* map, tmemorystreambuf& buffer, int& objNum ) : SelectFromMap( list, map ), buf( buffer), objectNum(objNum) {};
};


class BitMapEditor : public ASC_PG_Dialog {
   public:
      typedef int BitType;
   private:
      BitType& reference;
      PG_PropertyEditor* propertyEditor;
      bool values[64];
      int bitCount;

      bool ok();

   public:
      BitMapEditor( BitType& value, const ASCString& title, const vector<ASCString>& names );
};


#endif
