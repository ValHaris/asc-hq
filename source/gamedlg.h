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
#ifndef  gamedlgH
#define  gamedlgH


/*! \file gamedlg.h
   A lot of dialog boxes that are only used by both ASC.
*/


#include "typen.h"
#include "basegfx.h"
#include "newfont.h"
#include "spfst.h"
#include "loaders.h"
#include "misc.h"

#include "dialog.h"
#include "dlg_box.h"
#include "unitctrl.h"


class   tnewcampaignlevel : public tdialogbox {
                      protected:
                         tmaploaders  loader;
                      public:
                         int           status;
                         char           dateiinfo[100];
                         char           mapname [100];
                         char           mapinfo [100];
                         char           maptitle[100];
                         char           message1[100];
                         char           message2[100];
                         char           password[100];

                         void           init ( void );
                         void           searchmapinfo ( void );
                         virtual void   evaluatemapinfo( const char* srname, tmap* spfld ) = 0;
                         void           loadcampaignmap ( void );
                         void           showmapinfo( int ypos );
                         void           done ( void );
                     };

class  tcontinuecampaign : public tnewcampaignlevel {
                         int           idsearched;
                         tmemorystreambuf memoryStreamBuffer;
                         Player::DissectionContainer dissectedunits[8];
                    public:
                         void           init ( void );
                         virtual void   evaluatemapinfo( const char* srname, tmap* spfld );
                         void           showmapinfo( int ypos );
                         virtual void   run ( void );
                         void           setid( int id );
                         virtual void   buttonpressed( int id );
                         void           regroupevents ( pmap map );
                      };

class  tchoosenewmap    : public tnewcampaignlevel {
                     public:
                          void          init( char* ptitle );
                          virtual void  buttonpressed( int id );
                          void          readmapinfo ( void );
                          virtual void  checkforcampaign( tmap* spfld ) = 0;
                      };

class  tchoosenewcampaign : public tchoosenewmap {
                     public:
                          void          init ( void );
                          virtual void  run ( void );
                          virtual void  checkforcampaign( tmap* spfld );
                          virtual void  evaluatemapinfo( const char* srname, tmap* spfld );
                      };

class  tchoosenewsinglelevel : public tchoosenewmap {
                     public:
                          void          init ( void );
                          virtual void  run ( void );
                          virtual void  checkforcampaign( tmap* spfld );
                          virtual void  evaluatemapinfo( const char* srname, tmap* spfld );
                      };






extern void  choosetechnology(void);
extern void  settributepayments ( void );


extern void researchinfo ( void );
extern void choosetechlevel ( void );



struct tonlinehelpitem {
            tmouserect rect;
            int messagenum;
          };

class tonlinehelplist {
         public:
           int num;
           tonlinehelpitem* item;

           tonlinehelplist ( void ) {
              num = 0;
              item = NULL;
           }

       };


extern void gamepreferences  ( void );
extern void mousepreferences ( void );

extern void multiplayersettings ( void );

extern void giveunitaway ( pfield fld );

extern void showGameParameters ( void );

//! a dialog box to transfer resources from one vehicle to another
extern void  verlademunition( VehicleService* serv, int targetNWID );

extern void showPlayerTime();
extern void displaywindspeed();


#endif  // ifndef gamedlg_h

