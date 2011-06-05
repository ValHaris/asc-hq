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


#ifndef ServiceCommandH
#define ServiceCommandH

#include "containercommand.h"

#include "../typen.h"
#include "../objects.h"
#include "../mapfield.h"
#include "servicing.h"

class ServiceCommand : public ContainerCommand {
   
   public:
      static bool availExternally ( ContainerBase* source );
      static bool avail ( ContainerBase* source, ContainerBase* target );
   private:
      
      ServiceCommand( GameMap* map ) : ContainerCommand( map ), targetSearcher(NULL), transferHandler(NULL), destinationSpecified(false),destinationContainerID(0) {};
      template<class Child> friend GameAction* GameActionCreator( GameMap* map);
      
      ServiceTargetSearcher* targetSearcher;
      TransferHandler* transferHandler;
      
      //! key is the id of the transferrable, value is the destination amount
      typedef map<int,int> Values;
      Values values;
      Values orgValues;
      
      bool destinationSpecified;
      int destinationContainerID;
      
      ContainerBase* getDestination();
      
   protected:
      void readData ( tnstream& stream );
      void writeData ( tnstream& stream ) const;
      
      GameActionID getID() const;
      ASCString getDescription() const;
      
   public:
      ServiceCommand ( ContainerBase* unit );
      
      /** each call to getTransferHandler will deallocate the previous handler and create a new one
          Make sure to not have any pointers to the old one around when calling this another time */
      TransferHandler& getTransferHandler( );
            
      const ServiceTargetSearcher::Targets& getDestinations();
      
      void setDestination( ContainerBase* destination );
      
      void saveTransfers();
      
      ActionResult go ( const Context& context ); 
      ASCString getCommandString() const;
      
      ContainerBase* getRefueller() { return ContainerCommand::getContainer(); };
      ActionResult checkExecutionPrecondition() const;
      
      ~ServiceCommand();
};

#endif

