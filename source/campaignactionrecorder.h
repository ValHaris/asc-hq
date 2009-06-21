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

#ifndef campaignactionrecorderH
#define campaignactionrecorderH

#include <sigc++/sigc++.h>
#include "luacommandwriter.h"
#include "gamemap.h"

template <class C>
class LuaCommandListRecorder : public LuaCommandListWriter<C> {
      int counter;
   public:
      LuaCommandListRecorder ( C& container ) : LuaCommandListWriter<C>( container), counter(0) {};
      void printCommand( const ASCString& command ){
         if ( command.find('\n') != ASCString::npos ) {
            LuaCommandListWriter<C>::splitString(command);
         } else {
            ++counter;
            LuaCommandListWriter<C>::outputLine("r = asc." + command + "\n" );
            LuaCommandListWriter<C>::outputLine("if r:successful()==false then asc.displayActionError(r) \n  asc.errorMessage('Failure at command " + ASCString::toString(counter) + "') \n  return\nend \n" );
         }
      }
};


class CampaignActionLogger : public SigC::Object, public ActionRecorder {
   
      void endTurn( Player& player );
      
      GameMap* gamemap;
      
      typedef list<ASCString> CommandList;
      CommandList commands;
      
      void mapWon( Player& player ); 
      void commitCommand( GameMap* map, const Command& command );
      LuaCommandListRecorder<CommandList> writer;
      
   public:
      CampaignActionLogger ( GameMap* map );
      
      void readData ( tnstream& stream );
      
      void writeData ( tnstream& stream ) ;
};

#endif