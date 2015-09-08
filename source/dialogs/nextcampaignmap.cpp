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


#include "../paradialog.h"
#include "../loaders.h"
#include "../windowing.h"

class NextCampaignMap : public ConfigurableWindow
{
   private:
      ASCString file;
   public:
      NextCampaignMap(PG_Widget* parent, const PG_Rect& r );
      GameMap* getMap();
      bool search( int id );
};

const char* waiting = "-- searching, please wait -- ";

NextCampaignMap :: NextCampaignMap(PG_Widget* parent, const PG_Rect& r ) : ConfigurableWindow( parent, r, "continuecampaign" )
{
   setup();
   setLabelText( "Title", waiting);
   setLabelText( "CodeWord", waiting);
   setLabelText( "Filename", waiting);

   PG_Button* b = dynamic_cast<PG_Button*>( FindChild( "GO", true ) );
   if ( b )
      b->sigClick.connect( sigc::mem_fun( *this, &NextCampaignMap::QuitModal ));
   
}

bool NextCampaignMap :: search( int id )
{
   MapConinuationInfo mci = findNextCampaignMap( id );
   if ( mci.filename.length() ) {
      setLabelText( "Title", mci.title );
      setLabelText( "CodeWord", mci.codeword );
      setLabelText( "Filename", mci.filename );
      file = mci.filename;
      return true;
   } else {
      errorMessage("The next campaign map is not available");
      return false;
   }
}

GameMap* NextCampaignMap :: getMap()
{
   return mapLoadingExceptionChecker( file, MapLoadingFunction( tmaploaders::loadmap ));
};


GameMap* nextCampaignMap( int id )
{
   NextCampaignMap ncm( NULL, PG_Rect(50,50,500,300) );
   ncm.Show();
   if ( ncm.search( id ) ) {
      ncm.RunModal();
      return ncm.getMap();
   } else
      return NULL;
}

