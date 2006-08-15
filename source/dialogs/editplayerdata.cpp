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

#include <pgcolorselector.h>



class EditPlayerData : public ASC_PG_Dialog
{
      ASC_PropertyEditor* editor;
      ASCString name;
      Player& myPlayer;
      bool ok();

      PG_DropDown* playerMode; 
      map<int,Player::PlayerStatus> modes;
      
      PG_ColorSelector* colorSelector;

   public:
      EditPlayerData( Player& player );
      GameMap* getMap();
      bool search( int id );
};

EditPlayerData :: EditPlayerData(Player& player  ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 500, 400), "Edit Player Data" ), editor(NULL), myPlayer( player ), colorSelector(NULL)
{
   name = player.getName();
   editor = new ASC_PropertyEditor( this, PG_Rect( 10, 40, Width()-20, 70));
   new PG_PropertyField_String<ASCString>( editor , "Name", &name );
   new PG_PropertyField_String<ASCString>( editor , "Email Address", &player.email );


   playerMode = new PG_DropDown( this, PG_Rect( 10, 110, Width() - 30, 25 ));
   playerMode->AddItem( Player :: playerStatusNames[player.stat] );
   modes[0] = player.stat;

   int counter = 1;
   int pos = 0;
   while ( Player :: playerStatusNames[pos] ) {
      if( pos != Player::supervisor && pos != player.stat ) {
         playerMode->AddItem( Player :: playerStatusNames[pos] );
         modes[counter++] = Player::PlayerStatus(pos);
      }
      ++pos;
   }
            
   playerMode->SelectItem(0);

//   colorSelector = new PG_ColorSelector( this, PG_Rect( 10, 140, Width() - 30, 150 ));
//   colorSelector->SetColor( player.getColor() );

   StandardButtonDirection( Horizontal );
   AddStandardButton( "OK")->sigClick.connect( SigC::slot( *this, &EditPlayerData::ok ));
}

bool EditPlayerData  :: ok()
{
   if ( editor->Apply() ) {
      myPlayer.setName( name );
      myPlayer.stat = modes[playerMode->GetSelectedItemIndex()];
      // myPlayer.setColor( colorSelector->
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

