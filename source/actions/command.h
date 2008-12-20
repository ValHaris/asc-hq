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


#ifndef CommandH
#define CommandH

#include "action.h"

/** A Command is an action that the user initiates. 
    It contains all the logic about when the command is available .
    Unlike the atomic actions, it is still be run in a changed environment compared 
    to the time it was initially issued */

class Command : public GameAction {
   public:
      enum State { Planned, Evaluated, SetUp, Completed, Failed };
   private:
      State state;
   public:
      State getState() const { return state; };
      
      virtual ASCString getCommandString() const = 0;
      
      ActionResult redo( const Context& context );
      
   protected:
      Command( GameMap* gamemap );
      
      void setState( State state );
      
      virtual ActionResult go ( const Context& context ) = 0; 
      
      virtual ActionResult runAction( const Context& context ) ;
      virtual ActionResult undoAction( const Context& context );
      
      virtual void readData ( tnstream& stream );
      virtual void writeData ( tnstream& stream ) const;
            
};

#endif

