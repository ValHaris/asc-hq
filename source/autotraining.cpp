/***************************************************************************
                          autotraining.cpp  -  description
                             -------------------
    copyright            : (C) 2009 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file autotraining.cpp
    \brief Trains units in training centers automatically at each turn's end
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "player.h"
#include "gamemap.h"

#include "actions/trainunitcommand.h"
#include "actions/servicecommand.h"

static void autoTrainer( ContainerBase* container )
{
   for ( ContainerBase::Cargo::const_iterator i = container->getCargo().begin(); i != container->getCargo().end(); ++i ) {
      if ( TrainUnitCommand::avail( container, *i )) {
         Context context;
         context.gamemap = container->getMap();
         context.actingPlayer = & (container->getMap()->getPlayer( container ));
         context.parentAction = NULL;
         context.display = NULL;
         context.viewingPlayer = container->getOwner(); 
         context.actionContainer = &container->getMap()->actions;
                  
         auto_ptr<TrainUnitCommand> tuc ( new TrainUnitCommand( container ));
         tuc->setUnit( *i );
         ActionResult res = tuc->execute( context );
                  
         if ( res.successful())
            tuc.release();
                  
                  
         if ( ServiceCommand::avail( container, *i )) {
            auto_ptr<ServiceCommand> sc ( new ServiceCommand( container ));
            sc->setDestination( *i );
            TransferHandler& tc = sc->getTransferHandler();
            tc.fillDestAmmo();
            sc->saveTransfers();
            res = sc->execute( context );
            if ( res.successful())
               sc.release();
         }
      }
   }
}

void automaticTrainig( Player& player )
{
   for ( Player::BuildingList::iterator b = player.buildingList.begin(); b != player.buildingList.end(); ++b ) 
      autoTrainer( *b );
      
   for ( Player::VehicleList::iterator v= player.vehicleList.begin(); v != player.vehicleList.end(); ++v ) 
      autoTrainer( *v );
   
}
