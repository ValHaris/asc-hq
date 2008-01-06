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


#include "selfdestruct.h"
#include "../gamemap.h"
#include "../vehicle.h"
#include "../spfst.h"
#include "../viewcalculation.h"
#include "../replay.h"
#include "../soundList.h"
#include "../reactionfire.h"

bool SelfDestruct::available( const ContainerBase* subject )
{
   if ( subject )
     if ( subject->baseType->hasFunction( ContainerBaseType::ManualSelfDestruct ) ) {
        Player& player = subject->getMap()->getPlayer(subject);
        if ( player.diplomacy.isAllied( subject->getMap()->actplayer)  ) 
           return true;
     }

   return false;
};


bool SelfDestruct::destruct( ContainerBase* subject )
{
   if ( !available(subject ))
      return false;

   GameMap* map = subject->getMap();

   logtoreplayinfo( rpl_selfdestruct, subject->getIdentification() );
   
   Vehicle* v = dynamic_cast<Vehicle*>(subject);
   if ( v )
      SoundList::getInstance().playSound( SoundList::unitExplodes , 0, false, v->typ->killSoundLabel );
   else 
      SoundList::getInstance().playSound( SoundList::buildingCollapses );
     
   delete subject;
      
   evaluateviewcalculation( map );
  
   return true;
}

