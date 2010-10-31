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



#include "networkinterface.h"

GameTransferMechanism* GameTransferMechanism :: read ( tnstream& stream )
{
   stream.readInt();
   ASCString id = stream.readString();
   GameTransferMechanism* m = networkTransferMechanismFactory::Instance().createObject( id );
   m->readChildData( stream );
   return m;
}

void GameTransferMechanism :: write ( tnstream& stream ) const
{
   stream.writeInt( 1 ); // version
   stream.writeString( getMechanismID() );
   writeChildData( stream );
}

