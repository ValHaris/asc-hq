static ASCString ASCfilename;

#include "network/simple_file_transfer.h"

void         k_savemap(char saveas)
{
   ASCString filename;

   int nameavail = 0;
   if ( !actmap->preferredFileNames.mapname[0].empty() ) {
      nameavail = 1;
      filename = actmap->preferredFileNames.mapname[0];;
   }

   if ( saveas || !nameavail ) {
      filename = selectFile(ASCString("*") + tournamentextension, false);
   }
   if ( !filename.empty() ) {

      FileTransfer ft;
      ft.setup( filename );
      ft.send ( actmap, -1, -1 );
      mapsaved = true;
   }
}


void         k_loadmap(void)
{
   ASCString s1 = selectFile(  ASCString("*") + tournamentextension + ";*.asc", true );
   if ( !s1.empty() ) {
      StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "loading map " + s1 );
      FileTransfer ft;
      GameMap* mp = mapLoadingExceptionChecker( s1, MapLoadingFunction( &ft, &FileTransfer::loadPBEMFile ));
      if ( !mp )
         return;
         
      delete actmap;
      actmap =  mp;

      actmap->getPlayer(0).passwordcrc.setUnencoded("merphistologie");
      Password pwd;
      for ( int i = 0; i < actmap->getPlayerCount(); ++i )
         if ( actmap->getPlayer(i).stat == Player::supervisor ) 
            if ( !actmap->getPlayer(i).passwordcrc.empty() ) {
               pwd = actmap->getPlayer(i).passwordcrc;
               break;
            }
     
      if ( pwd.empty() ) 
         pwd = actmap->supervisorpasswordcrc;

      if ( !pwd.empty()  && (actmap->getgameparameter(cgp_superVisorCanSaveMap) > 0) ) {
         tlockdispspfld ldsf;
         if ( !enterpassword ( pwd )) {
            delete actmap;
            actmap = NULL;
            throw  NoMapLoaded();
         }
         ASCfilename = s1;
      } else {
         displaymessage("Sorry, you can't load this file because no supervisor was defined ",1);
         delete actmap;
         actmap = NULL;
         throw  NoMapLoaded();
      }

      displaymap();
      mapsaved = true;
      actmap->preferredFileNames.mapname[0] = s1;
   }
}

