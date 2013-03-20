/*! \file controls.cpp
   Controlling units (which is graudally moved to vehicletype.cpp and unitctrl.cpp );
   Things that are run when starting and ending someones turn
*/


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


#include "turncontrol.h"
#include "gamemap.h"
#include "mapdisplay.h"
#include "gameoptions.h"
#include "dialogs/pwd_dlg.h"
#include "controls.h"
#include "spfst.h"
#include "dlg_box.h"
#include "ai/ai.h"
#include "dialog.h"
#include "strtmesg.h"
#include "loaders.h"
#include "cannedmessages.h"
#include "viewcalculation.h"
#include "network/simple_file_transfer.h"
#include "dialogs/fileselector.h"
#include "researchexecution.h"
#include "gameeventsystem.h"

bool authenticateUser ( GameMap* actmap, bool allowCancel = true, bool lockView = true, bool throwOnFailure = false  )
{
   for ( int p = 0; p < 8; p++ )
      actmap->player[p].existanceAtBeginOfTurn = actmap->player[p].exist() && actmap->player[p].stat != Player::off;

   int humannum = 0;
   for ( int i = 0; i < 8; i++ )
      if ( actmap->player[i].exist() )
         if ( actmap->player[i].isHuman() )
            humannum++;

   if ( humannum > 1  ) {
      MapDisplayPG::LockDisplay ld ( !lockView );

      bool firstRound = actmap->time.turn() == 1;
      bool specifyPassword = firstRound && actmap->player[actmap->actplayer].passwordcrc.empty();
      // bool askForPassword = false;

      if ( (!actmap->player[actmap->actplayer].passwordcrc.empty() && actmap->player[actmap->actplayer].passwordcrc != CGameOptions::Instance()->getDefaultPassword() )
         || firstRound  ) {
            bool stat;
            actmap->setPlayerView ( actmap->actplayer );  // the idle handler of enterpassword starts generating the overview map, so we need to have the correct view prior to enterpassword
            do {
               stat = enterpassword ( actmap->player[actmap->actplayer].passwordcrc, specifyPassword, allowCancel, true, actmap->player[actmap->actplayer].getName() );
               if ( !stat ) {
                  if ( throwOnFailure ) {
                     delete actmap;
                     throw NoMapLoaded();
                  } else
                     return false;
               }
            } while ( actmap->player[actmap->actplayer].passwordcrc.empty() && stat && viewtextquery ( 910, "warning", "~e~nter password", "~c~ontinue without password" ) == 0 ); /* enddo */
      } else {
         infoMessage("next player is " + actmap->player[actmap->actplayer].getName() );
         actmap->setPlayerView ( actmap->actplayer );
      }
      actmap->overviewMapHolder.clear( true );
   } else
      actmap->overviewMapHolder.clear( true );

   actmap->setPlayerView ( actmap->actplayer );

   return true;
   
}




void runai( GameMap* actmap, int playerView, MapDisplayInterface* display )
{
   MapDisplayPG::CursorHiding cusorHiding;
   actmap->setPlayerView ( playerView );

   computeview( actmap );

   if ( !actmap->player[ actmap->actplayer ].ai )
      actmap->player[ actmap->actplayer ].ai = new AI ( actmap, actmap->actplayer );

   actmap->player[ actmap->actplayer ].ai->run( display );
   updateFieldInfo();
}


int findNextPlayer( GameMap* actmap, AbstractPlayerProcessing* playerProcessor )
{
   int p = actmap->actplayer;
   bool found = false;
   int loop = 0;
   do {
      ++p;
      if ( p >= actmap->getPlayerCount())  {
         p = 0;
         ++loop;
         if ( loop >= 3 )
            throw ShutDownMap();
      }

      
      if ( actmap->player[p].exist() ) 
         if ( actmap->player[p].stat != Player::off )
            found = true;
      
      if ( !found && playerProcessor )
         playerProcessor->playerSkipped( actmap->player[p] );
      
   } while ( !found );
   return p;
}


class ReplayClearer : public AbstractPlayerProcessing {
   
   virtual void playerSkipped( Player& player ) {
      if ( !player.exist() || player.stat == Player::off || player.stat == Player::suspended ) {
         int i = player.getPosition();
         if ( player.getParentMap()->replayinfo )
            if ( player.getParentMap()->replayinfo->map[i] && player.getParentMap()->replayinfo->guidata[i] ) {
               delete player.getParentMap()->replayinfo->map[i];
               player.getParentMap()->replayinfo->map[i] = NULL;
      
               delete player.getParentMap()->replayinfo->guidata[i];
               player.getParentMap()->replayinfo->guidata[i] = NULL;
            }
      }
   };
   
};

void iterateToNextPlayer( GameMap* actmap, bool saveNetwork, int lastPlayer, int lastTurn, MapDisplayInterface* display  )
{
   int loop = 0;
   bool closeLoop = false;

   ReplayClearer replayClearing;
   
   do {

      int currentPlayer= actmap->actplayer;
      
      int nextPlayer = findNextPlayer( actmap, &replayClearing );
      
      if ( nextPlayer <= currentPlayer ) {
         actmap->endRound();
         ++loop;
      }
      
      if ( loop > 2 ) {
         displaymessage("no human players found !", 1 );
         delete actmap;
         actmap = NULL;
         throw NoMapLoaded();
      }

               
      actmap->actplayer = nextPlayer;
      
      if ( actmap->player[nextPlayer].stat == Player::computer ) {
         actmap->beginTurn();
         runai( actmap, lastPlayer, display );
         actmap->endTurn();
      }
      
      if ( actmap->player[nextPlayer].stat == Player::suspended ) {
         actmap->beginTurn();
         actmap->endTurn();
      }

      if ( actmap->player[nextPlayer].stat == Player::human || actmap->player[nextPlayer].stat == Player::supervisor ) {
         if ( actmap->network && lastPlayer >= 0 && saveNetwork ) {
            actmap->network->send( actmap, lastPlayer, lastTurn );
            delete actmap;
            actmap = NULL;
            throw NoMapLoaded();
         } else
            closeLoop = true;
         }
   } while ( !closeLoop ); /* enddo */

}

bool NextTurnStrategy_AskUser::continueWhenLastPlayer() const
{
   viewtext2(904);
   if ( choice_dlg("Do you want to continue playing ?","~y~es","~n~o") == 2) 
      return false;
   else
      return true;
}

bool NextTurnStrategy_AskUser::authenticate( GameMap* actmap ) const
{
   return authenticateUser( actmap );
}

bool NextTurnStrategy_Abort::continueWhenLastPlayer() const { 
   return false;
};  

bool NextTurnStrategy_Abort::authenticate( GameMap* actmap) const
{
   return true;  
}


void next_turn ( GameMap* gamemap, const NextTurnStrategy& nextTurnStrategy, MapDisplayInterface* display, int playerView  )
{
   int lastPlayer = gamemap->actplayer;
   int lastTurn = gamemap->time.turn();

   if  ( lastPlayer >= 0 )
      gamemap->endTurn();
   
   /*
   int pv;
   if ( playerView == -2 ) {
      if ( gamemap->time.turn() <= 0 || gamemap->actplayer < 0 )
         pv = -1;
      else
         if ( gamemap->player[gamemap->actplayer].stat != Player::human )
            pv = -1;
         else
            pv = gamemap->actplayer;
   } else
      pv = playerView;
   */

      
   if ( findNextPlayer( gamemap ) == lastPlayer ) {
      if ( !gamemap->continueplaying ) {
         if ( !nextTurnStrategy.continueWhenLastPlayer() ) {
            delete gamemap;
            gamemap = NULL;
            throw NoMapLoaded();
         } else {
            gamemap->continueplaying = true;
            if ( gamemap->replayinfo ) {
               delete gamemap->replayinfo;
               gamemap->replayinfo = NULL;
            }
         }
      }   
   }
      
 
   iterateToNextPlayer( gamemap, true, lastPlayer, lastTurn, display );
   
   gamemap->setPlayerView ( -1 );
   
   if ( ! nextTurnStrategy.authenticate(gamemap) ) {
      delete gamemap;
      throw NoMapLoaded();
   }
   
   gamemap->beginTurn();
   gamemap->setPlayerView ( gamemap->actplayer );
   gamemap->overviewMapHolder.clear();
   
   gamemap->sigPlayerUserInteractionBegins( gamemap->player[gamemap->actplayer] );
   
   checktimedevents( gamemap, display );
   checkevents( gamemap, display );
}


void skipTurn( GameMap* gamemap )
{
   if ( gamemap->actplayer >= 0 ) {
      SuppressTechPresentation stp;
            
      gamemap->beginTurn();
      gamemap->endTurn();
   }
   iterateToNextPlayer( gamemap, false, -1, -1, NULL );
}

void checkUsedASCVersions ( Player& currentPlayer )
{
   for ( int i = 0; i < 8; i++ )
      if  ( currentPlayer.getParentMap()->player[i].exist() )
         if ( currentPlayer.getParentMap()->actplayer != i )
            if ( currentPlayer.getParentMap()->player[i].ASCversion > 0 )
               if ( (currentPlayer.getParentMap()->player[i].ASCversion & 0xffffff00) > getNumericVersion() ) {
                  new Message ( ASCString("Player ") + currentPlayer.getParentMap()->player[i].getName()
                           + " is using a newer version of ASC. \n"
                           "Please check www.asc-hq.org for updates.\n\n"
                           "Please do NOT report any problems with this version of ASC until "
                           "you have confirmed that they are also present in the latest "
                           "version of ASC.", currentPlayer.getParentMap(), 1<<currentPlayer.getParentMap()->actplayer );
                  return;
               }

}




GameMap* continueNetworkGame ( bool mostRecent )
{
   ASCString filename;
   if ( !mostRecent ) {
      filename = selectFile( ASCString("*") + tournamentextension + ";*.asc", true );
   } else {
      int datefound = 0;

      for ( int w = 0; w < 2; ++w) {

         ASCString wildcard;
         if ( w == 0 )
            wildcard = ASCString("*") + tournamentextension;
         else
            wildcard = "*.asc";

         tfindfile ff ( wildcard );

         tfindfile::FileInfo fi;
         while ( ff.getnextname( fi ))
            if ( fi.date > datefound ) {
               datefound = fi.date;
               filename = fi.name;
            }
      }
   }

   if ( filename.empty() )
      return NULL;

   StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "loading " + filename );

   return continueNetworkGame( filename );
}

GameMap* continueNetworkGame ( const ASCString& filename )
{
   FileTransfer ft;
   auto_ptr<GameMap> newMap ( mapLoadingExceptionChecker( filename, MapLoadingFunction( &ft, &FileTransfer::loadPBEMFile )));
   if ( !newMap.get() )
      return NULL;

   if ( !authenticateUser( newMap.get(), true, false ))
      return NULL;
   
   computeview( newMap.get() );
   newMap->beginTurn();
   newMap->setPlayerView ( newMap->actplayer );
   return newMap.release();
}



void  checkforvictory ( GameMap* gamemap, bool hasTurnControl )
{
   if ( !gamemap->continueplaying ) {
      int plnum = 0;
      for ( int i = 0; i < 8; i++ )
         if ( !gamemap->player[i].exist() && gamemap->player[i].existanceAtBeginOfTurn ) {
            int to = 0;
            for ( int j = 0; j < 8; j++ )
               if ( j != i )
                  to |= 1 << j;


            if ( !gamemap->campaign.avail ) {
               const char* sp = getmessage( 10010 ); // Message "player has been terminated"
			   ASCString txt;
			   txt.format( sp, gamemap->player[i].getName().c_str() );
               new Message ( txt, gamemap, to  );
            } 

            gamemap->player[i].existanceAtBeginOfTurn = false;

            if ( i == gamemap->actplayer ) {
               if ( gamemap->getPlayerView() == i && gamemap->getPlayer(i).stat == Player::human )
                  displaymessage ( getmessage ( 10011 ),1 );

               int humannum=0;
               for ( int j = 0; j < 8; j++ )
                  if (gamemap->player[j].exist() && gamemap->player[j].stat == Player::human )
                     humannum++;

               if ( hasTurnControl ) {
                  if ( humannum )
                     next_turn(gamemap, NextTurnStrategy_AskUser(), &getDefaultMapDisplay() );
                  else {
                     delete gamemap;
                     gamemap = NULL;
                     throw NoMapLoaded();
                  }
               }
            }
         } else
            plnum++;

      if ( plnum <= 1 ) {
         if ( gamemap->player[gamemap->actplayer].ai &&  gamemap->player[gamemap->actplayer].ai->isRunning() ) {
            displaymessage("You lost!",1);
         } else {
            displaymessage("Congratulations!\nYou won!",1);
            if (choice_dlg("Do you want to continue playing ?","~y~es","~n~o") == 2) {
               delete gamemap;
               gamemap = NULL;
               throw NoMapLoaded();
            } else {
               gamemap->continueplaying = 1;
               if ( gamemap->replayinfo ) {
                  delete gamemap->replayinfo;
                  gamemap->replayinfo = 0;
               }
            }
         }
      }
   }
}

