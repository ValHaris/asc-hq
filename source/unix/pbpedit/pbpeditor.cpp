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
         
      if ( mp->getgameparameter(cgp_superVisorCanSaveMap) == 0 ) {
         delete mp;
         throw ASCmsgException("The function to manipulate a game has not been enabled when it was started");
      }
      
      bool participatingSV = false;
      Password pwd;
      for ( int i = 0; i < mp->getPlayerCount(); ++i )
         if ( mp->getPlayer(i).stat == Player::supervisor ) 
            if ( !mp->getPlayer(i).passwordcrc.empty() ) {
               pwd = mp->getPlayer(i).passwordcrc;
               participatingSV = true;
               break;
            }
     
      if ( pwd.empty() ) 
         pwd = mp->supervisorpasswordcrc;

      if ( pwd.empty() ) {
         delete mp;
         
         if ( participatingSV ) 
            throw ASCmsgException("You can't load this file. While a supervisor was defined in the game, he doesn't use a password");
         else
            throw ASCmsgException("Sorry, you can't load this file because no supervisor was defined ");
      }

      tlockdispspfld ldsf;
      if ( !enterpassword ( pwd )) {
         delete mp;
         throw  NoMapLoaded();
      }
      ASCfilename = s1;
      
      delete actmap;
      actmap =  mp;
      
      displaymap();
      mapsaved = true;
      actmap->preferredFileNames.mapname[0] = s1;
   }
}

