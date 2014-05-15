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


#include "../global.h"

#include "../typen.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "eventinfo.h"
#include "../gameeventsystem.h"

#include "../widgets/textrenderer.h"


static const char* triggerStateNames[] = { "unfulfilled", "fulfilled", "finally_fulfilled", "finally_failed" };
static const char* eventStateNames[] = { "Untriggered", "Triggered", "Timed", "Executed"};


void viewEventInfo ( GameMap* gamemap )
{
   ASCString text;

   text += "#fontsize=18#Event Information\n";

   for (GameMap::Events::iterator e = gamemap->events.begin(); e != gamemap->events.end(); ++e ) {
       text += "\n#fontsize=14#" + (*e)->description + "#fontsize=11#\n";

       int counter = 0;
       for ( vector<EventTrigger*>::iterator t = (*e)->trigger.begin(); t != (*e)->trigger.end(); ++t ) {
           text += "Trigger " + ASCString::toString(counter++) + " ";
           text += triggerStateNames[(*t)->state(gamemap->actplayer)];
           if ( (*t)->invert )
               text += " (inverted)";
           text += "\n";
       }
       text += "Event ";
       text += eventStateNames[(*e)->status];
       text += "\n";

   }
   
   ViewFormattedText vft( "Field Information", text, PG_Rect( -1, -1, 420, 600 ));
   vft.Show();
   vft.RunModal();
}



