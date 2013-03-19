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

#include "util/messaginghub.h"

#pragma pack(1)
enum tnetworkchannel { TN_RECEIVE, TN_SEND };

 typedef class tbasenetworkconnection* pbasenetworkconnection;
 typedef class tnetwork* pnetwork;

#define tnetworkdatasize 100
typedef char tnetworkconnectionparameters[ tnetworkdatasize ];
typedef tnetworkconnectionparameters* pnetworkconnectionparameters;

class tnetworkcomputer {
  public:
     int        name; // was: char*
    struct {
      int          transfermethodid;
      int transfermethod;  // was: pbasenetworkconnection
      tnetworkconnectionparameters         data;
    } send, receive;
    int          existent;
};

class  tnetwork {
  public:
    struct {
      char         compposition;   // Nr. des Computers, an dem der SPieler spielt    => network.computernames
      int          codewordcrc;
    } player[8];

    tnetworkcomputer computer[8];

    int computernum;
    int turn;
    struct tglobalparams {
      int enablesaveloadofgames;
      int reaskpasswords;
      int dummy[48];
    } globalparams;
};

#pragma pack()

#include "basestrm.h"

void        readLegacyNetworkData ( tnstream& stream )
{
   tnetwork network;

// #if SDL_BYTEORDER == SDL_LIL_ENDIAN

      // don't change anything for Big-Endian adaption,
      // this part is really ugly and is rewrite for ASC2

         for ( int i = 0; i < 8; ++i ) {
            network.player[i].compposition = stream.readUint8();
            network.player[i].codewordcrc = stream.readInt();
         }

         for ( int i = 0; i< 8; ++i ) {
            network.computer[i].name = stream.readInt();
            network.computer[i].send.transfermethodid = stream.readInt();
            stream.readInt(); // network.computer[i].send.transfermethod = (pbasenetworkconnection) 
            stream.readdata2( network.computer[i].send.data );
            network.computer[i].receive.transfermethodid = stream.readInt();
            stream.readInt(); // network.computer[i].receive.transfermethod = (pbasenetworkconnection) 
            stream.readdata2( network.computer[i].receive.data );
            network.computer[i].existent = stream.readInt();
         }
         network.computernum = stream.readInt();
         network.turn = stream.readInt();
         network.globalparams.enablesaveloadofgames = stream.readInt();
         network.globalparams.reaskpasswords = stream.readInt();
         for ( int i = 0; i< 48; ++i )
            network.globalparams.dummy[i] = stream.readInt();
         
         for ( int i=0; i<8 ; i++ )
            if (network.computer[i].name  )
               stream.readString();
// #else
//         errorMessage("Unable to load map in old file format on this computer architecture\nPlease convert this file to the new file format on a 32 Bit little endian machine and try again");
//         throw tfileerror();
// #endif
}


