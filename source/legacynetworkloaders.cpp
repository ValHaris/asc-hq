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

#include "messaginghub.h"

#pragma pack(1)
enum tnetworkchannel { TN_RECEIVE, TN_SEND };

 typedef class tbasenetworkconnection* pbasenetworkconnection;
 typedef class tnetwork* pnetwork;

#define tnetworkdatasize 100
typedef char tnetworkconnectionparameters[ tnetworkdatasize ];
typedef tnetworkconnectionparameters* pnetworkconnectionparameters;

class tnetworkcomputer {
  public:
    char*        name;
    struct {
      int          transfermethodid;
      pbasenetworkconnection transfermethod;
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
   #if SDL_BYTEORDER == SDL_LIL_ENDIAN
   if ( sizeof(tnetwork)==2040 ) {
      int i;

      // don't change anything for Big-Endian adaption,
      // this part is really ugly and I'll rewrite it

      tnetwork net; 
      stream.readdata2 ( net );
      for (i=0; i<8 ; i++ ) 
         if (net.computer[i].name != NULL )
            stream.readpchar ( &net.computer[i].name );
   } else 
   #endif
   {
      errorMessage("Unable to load map in old file format on this computer architecture\nPlease convert this file to the new file format on a 32 Bit little endian machine and try again");
      throw tfileerror();
   }
}


