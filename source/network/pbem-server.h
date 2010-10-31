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



#ifndef pbemserver_h
#define pbemserver_h

#include "../networkinterface.h"

class ASC_PBEM;

class PBEMServer : public GameTransferMechanism {
      ASCString serverAddress;
      ASCString filename;
      int gameID;
      
   protected:   
      void readChildData ( tnstream& stream );
      void writeChildData ( tnstream& stream ) const;
      
      ASC_PBEM* getSession();
   public:
      
      PBEMServer();
      
      void setup();
      
      void send( const GameMap* map, int lastPlayer, int lastturn  );
      GameMap* receive();
      ASCString getMechanismID() const { return mechanismID(); };
      static ASCString mechanismID() { return "PBEM-Server"; };
      
      
      static ASCString getDefaultServerAddress();
      void setServerAddress( const ASCString& address );
};

#endif
