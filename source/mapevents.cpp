/***************************************************************************
                          mapevents.cpp  -  description
                             -------------------
    begin                : Tue Mar 13 2001
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


// this is work in progress and it will take some time until this code compiles or even works
// I'm working on this with quite low priority


Trigger_Time :: getDirectStatus ( )
{
   if ( gamemap->time.abs >= time.abs )
      return ultimatelyFulfilled;
   else
      return unFulfilled;
}

void Trigger_Time :: writeDerived ( tnstream& stream )
{
   stream.writeInt( time.abs );
}

void Trigger_Time :: readDerived  ( tnstream& stream )
{
   time.abs = stream.readInt();
}


