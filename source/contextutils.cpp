/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.org
     Copyright (C) 1994-2010  Martin Bickel
 
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

#include "contextutils.h"
#include "gamemap.h"

            
/** creates a context that will move the map so that any action can be seen by the user */
ReplayContext createFollowerContext( GameMap* gamemap )
{
   ReplayContext context;
   // Context context;
   // context.display = new ReplayMapDisplay( &getDefaultMapDisplay() );
   
   context.gamemap = gamemap;
   context.actingPlayer = &gamemap->getPlayer( gamemap->actplayer );
   context.parentAction = NULL;
   context.viewingPlayer = gamemap->getPlayerView(); 
   context.actionContainer = &gamemap->actions;
   return context;   
}

Context createContext( GameMap* gamemap )
{
   Context context;
   
   context.gamemap = gamemap;
   context.actingPlayer = &gamemap->getPlayer( gamemap->actplayer );
   context.parentAction = NULL;
   context.display = &getDefaultMapDisplay();
   context.viewingPlayer = gamemap->getPlayerView(); 
   context.actionContainer = &gamemap->actions;
   return context;   
}

ReplayContext::ReplayContext() 
   : Context(), repDisplay( &getDefaultMapDisplay() )
{
   display= &repDisplay;
}

