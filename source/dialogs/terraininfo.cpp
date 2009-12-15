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


#include "../global.h"

#include "../typen.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "terraininfo.h"

#include "../widgets/textrenderer.h"




void viewterraininfo ( GameMap* gamemap, const MapCoordinate& pos, bool fullVisibility )
{
   tfield* fld = gamemap->getField( pos );

   if ( !fld )
      return;

   ASCString text;

   text += "#fontsize=18#Field Information (";
   text += ASCString::toString(pos.x) + "/" + ASCString::toString(pos.y) + ")\n";

   
   text += "#fontsize=14#Terrain#aeinzug20##eeinzug20##fontsize=12##aeinzug30#\n";
   

   text += "ID: " + ASCString::toString( fld->typ->terraintype->id ) + "\n";

   text += "Weather: " + ASCString(cwettertypen[fld->getWeather() ]) + "\n";


   ASCString sub;
   float ab = fld->getattackbonus();
   float db = fld->getdefensebonus();
   sub.format( "Attack Bonus: %.1f\nDefense Bonus: %.1f\n", ab/8, db/8 );

   text += sub;

   text += "Base Jamming: " + ASCString::toString( fld->getjamming() ) + "\n";
   text += "terrain filename: " + fld->typ->terraintype->location  + "\n";
   

   fld->bdt.appendToString( text );

   text += "\n\n#eeinzug0##fontsize=14#Visibility#eeinzug20##fontsize=12#\n";

   int view = 0;
   int jamming = 0; 
   for ( int i = 0; i < gamemap->getPlayerCount(); ++i ) 
      if ( gamemap->getPlayer(i).diplomacy.sharesView( gamemap->getPlayerView() )) {
         view += fld->view[i].view;
         jamming += fld->view[i].jamming;
      }

   text += "View: " + ASCString::toString( view ) + "\n";
   text += "Jamming: " + ASCString::toString( jamming ) + "\n";
   text += "Terrain view obstruction (basejamming): " + ASCString::toString( fld->getjamming() ) + "\n";


   text += "\n#eeinzug0##fontsize=14#Movemali#eeinzug20##fontsize=12#\n";


   for ( int i = 0; i < cmovemalitypenum; i++ ) {
      if ( fld->vehicle && fld->vehicle->typ->movemalustyp == i )
         text +=  "#fontcolor=0xffd658#";
      text += cmovemalitypes[i] + ASCString(": ") + ASCString::toString( fld->getmovemalus(i) );
      
      if ( fld->vehicle && fld->vehicle->typ->movemalustyp == i )
         text += "#fontcolor=default#";

      text += "\n";
   }

   int mines[4] = { 0, 0, 0, 0 };
   int mineDissolve[4] = { maxint, maxint, maxint, maxint };

   for ( tfield::MineContainer::iterator m = fld->mines.begin(); m != fld->mines.end(); ++m )
      if ( m->player == gamemap->actplayer || fullVisibility ) {
         mines[m->type-1]++;
         int lifetime = gamemap->getgameparameter( GameParameter(cgp_antipersonnelmine_lifetime + m->type-1 ));
         if ( lifetime > 0)
            mineDissolve[m->type-1] = min( m->lifetimer, mineDissolve[m->type-1]);
      }

   if ( mines[0] || mines[1] || mines[2] || mines[3] ) {
      text += "#eeinzug0##fontsize=14#Mine Information#fontsize=12##eeinzug20##aeinzug30#\n";
      text += "On this field are #aeinzug50# \n" ;

      for ( int i = 0; i < 4; i++ ) {
         text += " " + ASCString::toString( mines[i] ) + " " + MineNames[i];
         if ( mines[i] > 1 )
            text += "s";

         if ( mineDissolve[i] >= 0 && mineDissolve[i] < maxint ) 
            text += ", next mine will dissolve in " + ASCString::toString ( mineDissolve[i]) + " turns.";
         
         text += "\n";
      }
   }
   
   if  ( fld->vehicle ) {
      text += "#aeinzug0##eeinzug0#\n#fontsize=14#Vehicle Information:#fontsize=12##aeinzug30##eeinzug20#\n" ;

      const Vehicletype* typ = fld->vehicle->typ;

      text += "Unit name: ";
      if ( !typ->name.empty() )
         text += typ->name.c_str() ;
      else
         text += typ->description;
      text += "\n";

      text += "Unit ID: " + ASCString::toString( typ->id ) + "\n";
      text += "Internal Unit Identification: " + ASCString::toString( fld->vehicle->networkid ) + "\n";

      if ( !typ->filename.empty() )
         text += "File Name: " + typ->location + "\n";

      
      text += "Terrain access:\n" + typ->terrainaccess.toString() + "\n";
   }

   if ( !fld->objects.empty() )
      text += "#aeinzug0##eeinzug0#\n#fontsize=14#Object Information:#fontsize=12##aeinzug30##eeinzug20#\n" ;

   for ( tfield::ObjectContainer::iterator i = fld->objects.begin(); i != fld->objects.end(); i++ ) 
      text += i->typ->location + "\n";


   if ( fld->building ) {
      text += "#aeinzug0##eeinzug0#\n#fontsize=14#Building Information:#fontsize=12##aeinzug30##eeinzug20#\n";
      text += fld->building->typ->location + "\n";
      text += "Owner: " + gamemap->player[fld->building->getOwner()].getName() + "\n";
   }


   if ( !fld->objects.empty() ) {
      text += "#aeinzug0##eeinzug0#\n#fontsize=14#Object Details:#aeinzug30##eeinzug20#";
      for ( tfield::ObjectContainer::iterator i = fld->objects.begin(); i != fld->objects.end(); i++ ) {
         const TerrainAccess* ta = &i->typ->getFieldModification( fld->getWeather() ).terrainaccess;
         text += "#aeinzug30##eeinzug20##fontsize=12#\n";
         text += i->typ->name + "#aeinzug50##eeinzug40##fontsize=10#\n";
         text += "Type ID: " + ASCString::toString(i->typ->id) + "\n";
         text += "Location: " + i->typ->location + "\n";

         text += "Terrain access:\n" + ta->toString() + "\n";

         text += "\nremaining lifetime:";
         text += ASCString::toString( i->lifetimer ) + "\n";

         text += "\nremaining child spawn counter:";
         text += ASCString::toString( i->remainingGrowthTime ) + "\n";

      }
   }

   ViewFormattedText vft( "Field Information", text, PG_Rect( -1, -1, 420, 600 ));
   vft.Show();
   vft.RunModal();
}



