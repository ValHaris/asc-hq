//     $Id: gamedlg.h,v 1.19 2002-02-21 17:06:51 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.18  2001/07/18 18:15:52  mbickel
//      Fixed: invalid sender of mails
//      Fixed: unmoveable units are moved by AI
//      Some reformatting of source files
//
//     Revision 1.17  2001/07/18 16:05:47  mbickel
//      Fixed: infinitive loop in displaying "player exterminated" msg
//      Fixed: construction of units by units: wrong player
//      Fixed: loading bug of maps with mines
//      Fixed: invalid map parameter
//      Fixed bug in game param edit dialog
//      Fixed: cannot attack after declaring of war
//      New: ffading of sounds
//
//     Revision 1.16  2001/02/26 12:35:14  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.15  2001/01/23 21:05:17  mbickel
//      Speed up of AI
//      Lot of bugfixes in AI
//      Moved Research to own files (research.*)
//      Rewrote storing of developed technologies
//      Some cleanup and documentation
//
//     Revision 1.14  2001/01/04 15:13:52  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.13  2000/11/08 19:31:07  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
//     Revision 1.12  2000/10/18 14:14:11  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//

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
#ifndef  gamedlg_h
#define  gamedlg_h


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
                         char           password[10];

                         void           init ( void );
                         void           searchmapinfo ( void );
                         virtual void   evaluatemapinfo( const char* srname, tmap* spfld ) = 0;
                         void           loadcampaignmap ( void );
                         void           showmapinfo( word ypos );
                         void           done ( void );
                     };

class  tcontinuecampaign : public tnewcampaignlevel {
                         word           idsearched;
                         peventstore    oldevent;
                         tmemorystreambuf memoryStreamBuffer;
                         Player::DissectionContainer dissectedunits[8];
                    public:
                         void           init ( void );
                         virtual void   evaluatemapinfo( const char* srname, tmap* spfld );
                         void           showmapinfo( word ypos );
                         virtual void   run ( void );
                         void           setid( word id );
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

extern int   setupnetwork ( tnetwork* nw, int edt = 7, int player = -1 );
/* edt ist bitmappt:
      bit 0 :  receive einstellbar
          1 :  send einstellbar
          2 :  computer wechselbar
                  falls 0 dann:
           3:  prinzipiell computer 0 verwenden
*/

extern void  settributepayments ( void );


class   tshownewtanks : public tdialogbox {
                         public:
                           void          init ( char*      buf2 );
                           virtual void  run  ( void );
                           virtual void  buttonpressed ( int id );
                         protected:
                           char*      buf;
                           int           status;
                        };

extern void  showtechnology(ptechnology  t);
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

class tonlinemousehelp : public tsubmousehandler {
          protected:
              int active;
              void* image;
              struct {
                 int x1,y1,x2,y2;
              } pos;

              int lastmousemove;


              tonlinehelplist helplist;

           public:
              tonlinemousehelp ( void );
              void mouseaction ( void );
              void displayhelp ( int messagenum );
              void removehelp  ( void );
              virtual void checkforhelp ( void );
              virtual void checklist ( tonlinehelplist* list );
              void invisiblerect ( tmouserect newoffarea );
              int  rectinoffarea ( void );
              virtual ~tonlinemousehelp ();
      };

extern void gamepreferences  ( void );
extern void mousepreferences ( void );

extern void multiplayersettings ( void );

extern void giveunitaway ( void );

extern void showGameParameters ( void );

//! a dialog box to transfer resources from one vehicle to another
extern void  verlademunition( VehicleService* serv, int targetNWID );

#endif  // ifndef gamedlg_h

