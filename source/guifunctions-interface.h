//     $Id: guifunctions-interface.h,v 1.5 2010-03-07 19:52:44 mbickel Exp $
//
/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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

#if defined(karteneditor) && !defined(pbpeditor)
#error the mapeditor is not supposed the gui icons  !
#endif


#ifndef guifunctionsinterfaceH
#define guifunctionsinterfaceH

#include "guiiconhandler.h"

class AStar3D;

namespace GuiFunctions {

class MovementBase : public GuiFunction {
   protected:
      virtual void parametrizePathFinder( AStar3D& pathFinder ) {};
      virtual int getVerticalDirection() = 0;
   public:
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
};
   
class Movement : public MovementBase 
{
   protected:
      virtual void parametrizePathFinder( AStar3D& pathFinder );
   public:
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
      int getVerticalDirection() { return 0; };
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
};


class Cancel : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );
      Surface& getImage( const MapCoordinate& po, ContainerBase* subject, int nums );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
};

}

#endif

