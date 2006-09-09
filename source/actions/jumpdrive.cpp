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


#include "jumpdrive.h"
#include "../gamemap.h"
#include "../vehicle.h"
#include "../spfst.h"
#include "../viewcalculation.h"
#include "../replay.h"

bool JumpDrive::available( const Vehicle* subject )
{
   if ( subject )
      if ( !subject->hasMoved() )
         if ( subject->height & subject->typ->jumpDrive.height )
            if ( subject->getResource( subject->typ->jumpDrive.consumption ) == subject->typ->jumpDrive.consumption ) 
               return true;

   return false;
};

bool JumpDrive::fieldReachable( const Vehicle* subject, const MapCoordinate& dest )
{
   GameMap* gamemap = subject->getMap();
   tfield* fld = gamemap->getField( dest );
   if ( beeline( dest, subject->getPosition()) <= subject->typ->jumpDrive.maxDistance )
      if ( !fld->vehicle && !fld->building ) 
         if ( fieldvisiblenow( fld, subject->getOwner(), gamemap ))
            if ( subject->typ->jumpDrive.targetterrain.accessible( fld->bdt ) > 0 ) 
               return true;
   
   return false;
}


bool JumpDrive::getFields( const Vehicle* subject )
{
   if ( !available(subject ))
      return false;
     
   GameMap* gamemap = subject->getMap();
    
   for ( int y = 0; y < gamemap->ysize; ++y )
      for (int x = 0; x < gamemap->xsize; ++x ) {
         MapCoordinate dest (x,y); 
         if ( fieldReachable( subject, dest)) {
            fieldAvailable(gamemap,dest);
            destinations[dest] = true;
         }
      }
      
      
   return destinations.size() > 0;
}

bool JumpDrive::jump( Vehicle* subject, const MapCoordinate& destination )
{
   if ( !available(subject ))
      return false;
      
   if ( !fieldReachable( subject, destination ))
      return false;
   
   tfield* fld = subject->getMap()->getField(subject->getPosition() ); 
   if ( fld->vehicle != subject )
      return false;
   
   fld->vehicle = NULL;
   subject->getResource( subject->typ->jumpDrive.consumption, false );
   subject->removeview();
   subject->setnewposition ( destination.x, destination.y );
   subject->setMovement(0, 1);
   subject->addview();
   subject->getMap()->getField(destination)->vehicle = subject;
   evaluateviewcalculation( subject->getMap() );
   
   logtoreplayinfo( rpl_jump , subject->networkid, destination.x, destination.y );
   
   return true;
}

