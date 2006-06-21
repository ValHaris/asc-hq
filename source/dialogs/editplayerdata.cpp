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

#include "../ascstring.h"
#include "../paradialog.h"
#include "../windowing.h"
#include <pgpropertyfield_string.h>
#include "../gamemap.h"



class EditPlayerData : public ASC_PG_Dialog
{
      ASC_PropertyEditor* editor;
      ASCString name;
      Player& myPlayer;
      bool ok();
   public:
      EditPlayerData( Player& player );
      GameMap* getMap();
      bool search( int id );
};

EditPlayerData :: EditPlayerData(Player& player  ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 500, 200), "Edit Player Data" ), editor(NULL), myPlayer( player )
{
   name = player.getName();
   editor = new ASC_PropertyEditor( this, PG_Rect( 10, 40, Width()-20, Height() - 50));
   new PG_PropertyField_String<ASCString>( editor , "Name", &name );
   new PG_PropertyField_String<ASCString>( editor , "Email Address", &player.email );

   StandardButtonDirection( Horizontal );
   AddStandardButton( "OK")->sigClick.connect( SigC::slot( *this, &EditPlayerData::ok ));
}

bool EditPlayerData  :: ok()
{
   if ( editor->Apply() ) {
      myPlayer.setName( name );
      QuitModal();
   }
   return true;
}



void editPlayerData( GameMap* gamemap )
{
   int humanNum = 0;
   for ( int i = 0; i < gamemap->getPlayerCount(); ++i )
      if ( gamemap->getPlayer(i).isHuman() )
         ++humanNum;
         
   if ( humanNum <= 1 ) 
      infoMessage("Only available for multiplayer games");
   else {
      EditPlayerData epd( gamemap->getPlayer( gamemap->actplayer) );
      epd.Show();
      epd.RunModal();
   }
     
}

