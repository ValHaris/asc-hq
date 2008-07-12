/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2008  Martin Bickel  and  Marc Schellenberger
 
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


#include "actionresult.h"

#include "../vehicle.h"

ActionResult::ActionResult( int code )
{
   this->code = code;  
}

ActionResult::ActionResult( int code, const ASCString& message )
{
   this->code = code;
   userMessage = message;  
}

ActionResult::ActionResult( int code, const Vehicle* veh )
{
   this->code = code;
   userMessage = "Unit is " + veh->getName() + " (ID: " + ASCString::toString(code) + ")";  
}

ActionResult::ActionResult( int code, const Vehicle* veh, const ASCString& message )
{
   this->code = code;
   userMessage = "Unit is " + veh->getName() + " (ID: " + ASCString::toString(code) + ")\n" + message;  
}


ActionResult::ActionResult( int code, const MapCoordinate& pos)
{
   this->code = code;
   userMessage = "Position is " + pos.toString();  
}


bool ActionResult::successful()
{
   return code == 0;
}

ASCString ActionResult::getMessage() const
{
   return "";
}


