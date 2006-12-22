/*! \file controls.cpp
   Controlling units (which is graudally moved to vehicletype.cpp and unitctrl.cpp );
   Things that are run when starting and ending someones turn
*/


/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2005  Martin Bickel  and  Marc Schellenberger

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
#include "network.h"
#include "cannedmessages.h"
#include "viewcalculation.h"

bool authenticateUser ( GameMap* actmap, int forcepasswordchecking = 0, bool allowCancel = true, bool lockView = true, bool throwOnFailure = false  )
{
   for ( int p = 0; p < 8; p++ )
      actmap->player[p].existanceAtBeginOfTurn = actmap->player[p].exist() && actmap->player[p].stat != Player::off;

   int humannum = 0;
   for ( int i = 0; i < 8; i++ )
      if ( actmap->player[i].exist() )
         if ( actmap->player[i].stat == Player::human )
            humannum++;

   if ( humannum > 1  ||  forcepasswordchecking > 0 ) {
      MapDisplayPG::LockDisplay ld ( !lockView );

      bool firstRound = actmap->time.turn() == 1;
      bool specifyPassword = firstRound && actmap->player[actmap->actplayer].passwordcrc.empty();
      // bool askForPassword = false;

      if ( (!actmap->player[actmap->actplayer].passwordcrc.empty() && actmap->player[actmap->actplayer].passwordcrc != CGameOptions::Instance()->getDefaultPassword() )
         || firstRound  ) {
            if ( forcepasswordchecking < 0 ) {
               delete actmap;
               actmap = NULL;
               throw NoMapLoaded();
            } else {
               bool stat;
               actmap->setPlayerView ( actmap->actplayer );  // the idle handler of enterpassword starts generating the overview map, so we need to have the correct view prior to enterpassword
               do {
                  stat = enterpassword ( actmap->player[actmap->actplayer].passwordcrc, specifyPassword, allowCancel );
                  if ( !stat ) {
                     if ( throwOnFailure ) {
                        delete actmap;
                        throw NoMapLoaded();
                     } else
                        return false;
                  }
               } while ( actmap->player[actmap->actplayer].passwordcrc.empty() && stat && viewtextquery ( 910, "warning", "~e~nter password", "~c~ontinue without password" ) == 0 ); /* enddo */
            }
      } else
         infoMessage("next player is " + actmap->player[actmap->actplayer].getName() );
   }

   moveparams.movestatus = 0;

   actmap->setPlayerView ( actmap->actplayer );

   return true;
   
}




void runai( GameMap* actmap, int playerView )
{
   MapDisplayPG::CursorHiding cusorHiding;
   actmap->setPlayerView ( playerView );

   computeview( actmap );

   if ( !actmap->player[ actmap->actplayer ].ai )
      actmap->player[ actmap->actplayer ].ai = new AI ( actmap, actmap->actplayer );

   actmap->player[ actmap->actplayer ].ai->run();
   updateFieldInfo();
}


int findNextPlayer( const GameMap* actmap )
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
      
   } while ( !found );
   return p;
}


//! deactivates the replays of non-active or deleted players
void clearReplays( GameMap* actmap )
{
   if ( actmap->replayinfo )
      for ( int i = 0; i < actmap->getPlayerCount(); ++i )
         if ( !actmap->player[i].exist() )
            if ( actmap->replayinfo->map[i] && actmap->replayinfo->guidata[i] ) {
               delete actmap->replayinfo->map[i];
               actmap->replayinfo->map[i] = NULL;

               delete actmap->replayinfo->guidata[i];
               actmap->replayinfo->guidata[i] = NULL;
            }
}


void iterateToNextPlayer( GameMap* actmap, bool saveNetwork, int lastPlayer, int lastTurn  )
{
   int loop = 0;
   bool closeLoop = false;

   clearReplays( actmap );

   do {

      
      int currentPlayer= actmap->actplayer;
      
      int nextPlayer = findNextPlayer( actmap );
      
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
         runai( actmap, lastPlayer );
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

void next_turn ( int playerView )
{
   int lastPlayer = actmap->actplayer;
   int lastTurn = actmap->time.turn();

   if  ( lastPlayer >= 0 )
      actmap->endTurn();
   
   int pv;
   if ( playerView == -2 ) {
      if ( actmap->time.turn() <= 0 || actmap->actplayer < 0 )
         pv = -1;
      else
         if ( actmap->player[actmap->actplayer].stat != Player::human )
            pv = -1;
         else
            pv = actmap->actplayer;
   } else
      pv = playerView;

      
   if ( findNextPlayer( actmap ) == lastPlayer ) {
      if ( !actmap->continueplaying ) {
         viewtext2(904);
         if (choice_dlg("Do you want to continue playing ?","~y~es","~n~o") == 2) {
            delete actmap;
            actmap = NULL;
            throw NoMapLoaded();
         } else {
            actmap->continueplaying = true;
            if ( actmap->replayinfo ) {
               delete actmap->replayinfo;
               actmap->replayinfo = NULL;
            }
         }
      }   
   }
      
 
   iterateToNextPlayer( actmap, true, lastPlayer, lastTurn );
   
   actmap->setPlayerView ( -1 );
   actmap->overviewMapHolder.clear();
   
   if ( !authenticateUser( actmap, 0, false, true, true )) {
      delete actmap;
      throw NoMapLoaded();
   }
   
   actmap->beginTurn();
   actmap->setPlayerView ( actmap->actplayer );
   actmap->sigPlayerUserInteractionBegins( actmap->player[actmap->actplayer] );
}


void skipTurn( GameMap* gamemap )
{
   iterateToNextPlayer( gamemap, false, -1, -1 );
}

void checkUsedASCVersions ( Player& currentPlayer )
{
   for ( int i = 0; i < 8; i++ )
      if  ( actmap->player[i].exist() )
         if ( actmap->actplayer != i )
            if ( actmap->player[i].ASCversion > 0 )
               if ( (actmap->player[i].ASCversion & 0xffffff00) > getNumericVersion() ) {
                  new Message ( ASCString("Player ") + actmap->player[i].getName()
                           + " is using a newer version of ASC. \n"
                           "Please check www.asc-hq.org for updates.\n\n"
                           "Please do NOT report any problems with this version of ASC until "
                           "you have confirmed that they are also present in the latest "
                           "version of ASC.", actmap, 1<<actmap->actplayer );
                  return;
               }

}




bool continuenetworkgame ( bool mostRecent )
{
   ASCString filename;
   if ( !mostRecent ) {
      filename = selectFile( ASCString("*") + tournamentextension + ";*.asc", true );
   } else {
      int datefound = 0;

      for ( int w = 0; w < 2; ++w) {

         ASCString wildcard;
         if ( w == 0 )
            ASCString("*") + tournamentextension;
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
      return false;

   StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "loading " + filename );
   FileTransfer ft;
   auto_ptr<GameMap> newMap ( mapLoadingExceptionChecker( filename, MapLoadingFunction( &ft, &FileTransfer::loadPBEMFile )));
   if ( !newMap.get() )
      return false;

   if ( !authenticateUser( newMap.get() , 0, true, false ))
      return false;
   
   delete actmap;
   actmap = newMap.release();

   computeview( actmap );
   actmap->beginTurn();
   actmap->setPlayerView ( actmap->actplayer );
   return true;
}



void  checkforvictory ( )
{
   if ( !actmap->continueplaying ) {
      int plnum = 0;
      for ( int i = 0; i < 8; i++ )
         if ( !actmap->player[i].exist() && actmap->player[i].existanceAtBeginOfTurn ) {
            int to = 0;
            for ( int j = 0; j < 8; j++ )
               if ( j != i )
                  to |= 1 << j;


            if ( !actmap->campaign.avail ) {
               char txt[1000];
               const char* sp = getmessage( 10010 ); // Message "player has been terminated"
   
               sprintf ( txt, sp, actmap->player[i].getName().c_str() );
               new Message ( txt, actmap, to  );
            } 

            actmap->player[i].existanceAtBeginOfTurn = false;

            if ( i == actmap->actplayer ) {
               displaymessage ( getmessage ( 10011 ),1 );

               int humannum=0;
               for ( int j = 0; j < 8; j++ )
                  if (actmap->player[j].exist() && actmap->player[j].stat == Player::human )
                     humannum++;
               if ( humannum )
                  next_turn();
               else {
                  delete actmap;
                  actmap = NULL;
                  throw NoMapLoaded();
               }
            }
         } else
            plnum++;

      if ( plnum <= 1 ) {
         if ( actmap->player[actmap->actplayer].ai &&  actmap->player[actmap->actplayer].ai->isRunning() ) {
            displaymessage("You lost!",1);
         } else {
            displaymessage("Congratulations!\nYou won!",1);
            if (choice_dlg("Do you want to continue playing ?","~y~es","~n~o") == 2) {
               delete actmap;
               actmap = NULL;
               throw NoMapLoaded();
            } else {
               actmap->continueplaying = 1;
               if ( actmap->replayinfo ) {
                  delete actmap->replayinfo;
                  actmap->replayinfo = 0;
               }
            }
         }
      }
   }
}

