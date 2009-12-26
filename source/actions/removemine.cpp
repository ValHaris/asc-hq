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


#include "removemine.h"
#include "action-registry.h"

#include "../vehicle.h"
#include "../gamemap.h"
     
RemoveMine::RemoveMine( GameMap* gamemap, const MapCoordinate& position, int mineID )
   : GameAction( gamemap ), pos(position), layer(-1), mineBuffer(NULL)
{
   this->mineID = mineID;
}
      
      
ASCString RemoveMine::getDescription() const
{
   ASCString res = "Remove mine at " + pos.toString(false) + " with id " + ASCString::toString(mineID);
   return  res;
}
      
      
void RemoveMine::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "RemoveMine", 1, version );
   
   mineID = stream.readInt();
   layer = stream.readInt();
   pos.read( stream );
   
   if ( stream.readInt() ) {
      mineBuffer = new tmemorystreambuf();
      mineBuffer->readfromstream( &stream );  
   } else
      mineBuffer = NULL;
   
};
      
      
void RemoveMine::writeData ( tnstream& stream ) const
{
   stream.writeInt( 1 );
   stream.writeInt( mineID );
   stream.writeInt( layer );
   pos.write( stream );
   
   if ( mineBuffer ) {
      stream.writeInt( 1 );
      mineBuffer->writetostream( &stream );
   } else
      stream.writeInt( 0 );
   
};


GameActionID RemoveMine::getID() const
{
   return ActionRegistry::RemoveMine;
}

ActionResult RemoveMine::runAction( const Context& context )
{
   layer = 0;
   MapField* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   if ( mineID > 0 ) {
      for ( MapField::MineContainer::iterator i = fld->mines.begin(); i != fld->mines.end(); ++i ) {
         if ( i->identifier == mineID ) {
            mineBuffer = new tmemorystreambuf();
            tmemorystream memstream( mineBuffer, tnstream::writing );
            i->write( memstream );
            
            fld->mines.erase( i );
            return ActionResult(0);
         }
         ++layer;
      }
   } else {
      layer = fld->mines.size();
      mineBuffer = new tmemorystreambuf();
      tmemorystream memstream( mineBuffer, tnstream::writing );
      for ( MapField::MineContainer::iterator i = fld->mines.begin(); i != fld->mines.end(); ++i ) 
         i->write( memstream );
      fld->mines.clear();
      return ActionResult(0);
   }
      
  return ActionResult( 21401 );
}


ActionResult RemoveMine::undoAction( const Context& context )
{
   MapField* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   if ( layer < 0 )
      throw ActionResult( 21401 );
   
   if ( mineID > 0 ) {
      MapField::MineContainer::iterator i = fld->mines.begin();
      
      int l = layer;
      while ( l-- )
         ++i;
      
      tmemorystream memstream( mineBuffer, tnstream::reading );
      fld->mines.insert( i, Mine::newFromStream( memstream ));
   } else {
      tmemorystream memstream( mineBuffer, tnstream::reading );
      for ( int i = 0; i < layer; ++i )
         fld->mines.push_back( Mine::newFromStream( memstream ));
   }
   return ActionResult(0);
}

ActionResult RemoveMine::verify()
{
   return ActionResult(0);
}

RemoveMine::~RemoveMine()
{
   delete mineBuffer;  
}


namespace {
   const bool r1 = registerAction<RemoveMine> ( ActionRegistry::RemoveMine );
}

