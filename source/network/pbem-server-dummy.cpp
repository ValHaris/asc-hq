/*! \file network.cpp
    \brief Code for moving a multiplayer game data from one computer to another.
 
    The only method that is currently implemented is writing the data to a file
    and telling the user to send this file by email :-)
    But the interface for real networking is there...
*/
/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2005  Martin Bickel  and  Marc Schellenberger

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

#include "pbem-server.h"
#include "../util/messaginghub.h"

PBEMServer::PBEMServer() {};
ASCString PBEMServer::getDefaultServerAddress() { return "not supported"; };
void PBEMServer::setServerAddress( const ASCString& address ) 
{
   warningMessage("This version of ASC was compiled without support PBEM Server");
};

void PBEMServer::readChildData ( tnstream& stream ) 
{
   warningMessage("This version of ASC was compiled without support PBEM Server");
};

void PBEMServer::writeChildData ( tnstream& stream ) const 
{
   warningMessage("This version of ASC was compiled without support PBEM Server");
}

void PBEMServer::setup()
{
   warningMessage("This version of ASC was compiled without support PBEM Server");
}
      
void PBEMServer::send( const GameMap* map, int lastPlayer, int lastturn  )
{
   warningMessage("This version of ASC was compiled without support PBEM Server");
}


GameMap* PBEMServer::receive()
{
   warningMessage("This version of ASC was compiled without support PBEM Server");
   return NULL;  
}
