/***************************************************************************
                          networkdata.h  -  description
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


#ifndef networkdata_h_included
#define networkdata_h_included

#include "pointers.h"

#pragma pack(1)


enum tnetworkchannel { TN_RECEIVE, TN_SEND };



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
    tnetworkcomputer ( void );
    ~tnetworkcomputer ( );
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
    tnetwork ( void );
};

#pragma pack()

#endif // networkdata_h_included
