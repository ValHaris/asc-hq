/*! \file clipboard.h
    \brief The clipboard for ASC
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2003  Martin Bickel  and  Marc Schellenberger

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

#ifndef clipboardH
 #define clipboardH

 #include <map>

 #include "loki/Singleton.h"
 #include "basestrm.h"
 #include "vehicletype.h"
 #include "buildingtype.h"
 #include "typen.h"
 #include "textfile_evaluation.h"

class ClipBoardBase {
      map<ASCString,ASCString> properties;
   
      tmemorystreambuf buf;
      enum Type { ClipVehicle, ClipBuilding };
      int objectNum;

      friend struct Loki::CreateUsingNew<ClipBoardBase>;
      ClipBoardBase();
      ~ClipBoardBase() {};
      
      void setProperties( const ContainerBase* unit );
      
   public:
      void clear();
      void addUnit ( const Vehicle* unit );
      void addBuilding ( const Building* bld );
      Vehicle* pasteUnit();
      Vehicle* pasteUnit( tnstream& stream);
      void place ( const MapCoordinate& pos );

      void writeProperties( PropertyContainer& pc ) const;
      
      void write( tnstream& stream );
      void read( tnstream& stream );
};

typedef Loki::SingletonHolder< ClipBoardBase > ClipBoard;

extern const char* clipboardFileExtension;


#endif

