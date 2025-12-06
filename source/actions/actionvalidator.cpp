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


#include "actionvalidator.h"

#include <fstream>
#include <iostream>

#include "actioncontainer.h"
#include "../gamemap.h"

#include "../util/messaginghub.h"
#include "../basestrm.h"
#include "../gameoptions.h"
#include "../loaders.h"
#include "../researchexecution.h"
#include "../mapdisplayinterface.h"
#include "../cannedmessages.h"
#include "action.h"
#include "actioncontainer.h"
#include "../viewcalculation.h"

bool validateActionStack( GameMap* map, bool createSavegame, const ActionContainer::Actions& actions) {

   if ( !map->replayinfo )
       return true;


   if ( createSavegame ) {
       const ASCString filename("_actiondebug.sav");
       StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "saving " + filename);
       savegame(filename, map);
       std::ofstream myfile(constructFileName(0, "", "_actiondebug.sav.actions" ));
       int counter = 0;
       for ( ActionContainer::Actions::const_iterator i = actions.begin(); i != actions.end(); ++i) {
           myfile << counter++
                  << ";"
                  << map->actions.isActive_map( *i )
                  << ";"
                  << map->actions.isActive_req( *i )
                  << ";"
                  << (*i)->getDescription()
                  << "\n";
       }
   }

   auto_ptr<GameMap> validationMap ( loadreplay( map->replayinfo->map[map->getCurrentPlayer().getPosition()] ));
   SuppressTechPresentation stp;

   BlankMapDisplay blankDisplay;

   Context context;

   context.gamemap = validationMap.get();
   context.actingPlayer = &context.gamemap->getCurrentPlayer();
   context.parentAction = NULL;
   context.display = &blankDisplay;
   context.viewingPlayer = map->getPlayerView();

   computeview(validationMap.get());

   StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "Validating command");

   int counter = 0;
   for ( ActionContainer::Actions::const_iterator i = actions.begin(); i != actions.end(); ++i ) {
      counter++;

      smw.SetText("Validating command " + ASCString::toString(counter) + " of " + ASCString::toString(actions.size()));


       MemoryStreamStorage buffer;
       {
          MemoryStream writer( &buffer, tnstream::writing);
          (*i)->write(writer);
       }
       {
           MemoryStream reader( &buffer, tnstream::reading );
           auto_ptr<GameAction> readaction ( GameAction::readFromStream( reader, validationMap.get() ));
           Command* a = dynamic_cast<Command*> ( readaction.get() );
           if ( a ) {
              try {
                  ActionResult res = a->redo( context );
                  if ( !res.successful() ) {
                      errorMessage(ASCString("action ") + ASCString::toString(counter) + ": " + a->getDescription() + " failed\n" + getmessage(res.getCode()));
                      return false;
                  }
              } catch ( const ActionResult & res ) {
                  errorMessage(ASCString("action ") + ASCString::toString(counter) + ": " + a->getDescription() + " failed\n" + getmessage(res.getCode()));
                 throw res;
              }

           } else
               errorMessage("could not read Command action from replay stream" );
       }
   }
   return true;
}


void registerActionValidator() {
    ActionContainer::validateActionStack.connect( sigc::ptr_fun( &validateActionStack ));
}
