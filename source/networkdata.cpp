/***************************************************************************
                          networkdata.cpp  -  description
                             -------------------
    begin                : Sun Feb 18 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cstring>
#include "misc.h"
#include "networkdata.h"

#ifdef sgmain
 #include "network.h"
#endif


tnetworkcomputer :: tnetworkcomputer ( void )
{
   #ifdef sgmain
   send.transfermethod = defaultnetworkconnection;
   send.transfermethodid = defaultnetworkconnection->getid();
   memset ( send.data, 0, sizeof ( send.data ));

   receive.transfermethod = defaultnetworkconnection;
   receive.transfermethodid = defaultnetworkconnection->getid();
   memset ( receive.data, 0, sizeof ( receive.data ));
   #endif

   name = NULL;
   existent = 0;
}

tnetworkcomputer :: ~tnetworkcomputer ( )
{
   #ifdef sgmain
   if ( send.transfermethod ) {
      if ( send.transfermethod->transferopen () )
         send.transfermethod->closetransfer();
      if ( send.transfermethod->connectionopen () )
         send.transfermethod->closeconnection();
   }

   if ( receive.transfermethod ) {
      if ( receive.transfermethod->transferopen () )
         receive.transfermethod->closetransfer();
      if ( receive.transfermethod->connectionopen () )
         receive.transfermethod->closeconnection();
   }
   #endif

}


tnetwork :: tnetwork ( void )
{
  for (int i = 0; i < 8; i++) {
     player[i].compposition = 0;
     player[i].codewordcrc = 0;
  } /* endfor */
  computernum = 0;
  turn = 0;
  globalparams.enablesaveloadofgames = 0;
  globalparams.reaskpasswords = 0;
}

