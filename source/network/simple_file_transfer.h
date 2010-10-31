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



#ifndef network_h
#define network_h

#include "../networkinterface.h"


class FileTransfer : public GameTransferMechanism {
      ASCString filename;
   protected:   
      void readChildData ( tnstream& stream );
      void writeChildData ( tnstream& stream ) const;
      bool enterfilename();
      ASCString constructFileName( const GameMap* actmap, int lastPlayer, int lastturn ) const;
   public:
      void setup();
      void setup( const ASCString& filename );
      
      void send( const GameMap* map, int lastPlayer, int lastturn  );
      GameMap* receive();
      GameMap* loadPBEMFile( const ASCString& filename );
      ASCString getMechanismID() const { return mechanismID(); };
      static ASCString mechanismID() { return "FileTransfer"; };
};

extern void networksupervisor ( void );

#endif
