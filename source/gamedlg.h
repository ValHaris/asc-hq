//     $Id: gamedlg.h,v 1.6 2000-08-07 16:29:21 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.5  2000/05/23 20:40:47  mbickel
//      Removed boolean type
//
//     Revision 1.4  2000/05/07 12:12:17  mbickel
//      New mouse option dialog
//      weapon info can now be displayed by clicking on a unit
//
//     Revision 1.3  1999/11/22 18:27:27  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:41:46  tmwilson
//     	Added CVS keywords to most of the files.
//     	Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//     	Wrote replacement routines for kbhit/getch for Linux
//     	Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//     	Added autoconf/automake capabilities
//     	Added files used by 'automake --gnu'
//
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

#include "tpascal.inc"
#include "typen.h"
#include "keybp.h"
#include "basegfx.h"
#include "newfont.h"
#include "spfst.h"
#include "loaders.h"
#include "misc.h"
#include "mousehnd.h"


#include "dialog.h"
#include "dlg_box.h"



typedef struct tstringlist* pstringlist ;

 struct tstringlist {
           tstringlist*    next;
           tstringlist*    priv;
           char*           text;
           int             linenum;
        };


class   tnewcampaignlevel : public tdialogbox, public tmaploaders {
                      public:
                         byte           status;
                         char           dateiinfo[100];
                         char           mapname [100];
                         char           mapinfo [100];
                         char           maptitle[100];
                         char           message1[100];
                         char           message2[100];
                         char           password[10];

                         void           init ( void );
                         void           searchmapinfo ( void );
                         virtual void   evaluatemapinfo( char* srname ) = 0;
                         void           loadcampaignmap ( void );
                         void           showmapinfo( word ypos );
                         void           done ( void );
                     };

class  tcontinuecampaign : public tnewcampaignlevel {
                         word           idsearched;
                         peventstore    oldevent;
                         pdevelopedtechnologies  tech[8];
                         pdissectedunit dissectedunits[8]; 
                    public:
                         void           init ( void );
                         virtual void   evaluatemapinfo( char* srname );
                         void           showmapinfo( word ypos );
                         virtual void   run ( void );
                         void           setid( word id );
                         virtual void   buttonpressed( byte id );
                         void           regroupevents ( pmap map );
                      };

class  tchoosenewmap    : public tnewcampaignlevel {
                     public:
                          void          init( char* ptitle );
                          virtual void  buttonpressed( byte id );
                          void          readmapinfo ( void );
                          virtual void  checkforcampaign( void ) = 0;
                      };

class  tchoosenewcampaign : public tchoosenewmap {
                     public:
                          void          init ( void );
                          virtual void  run ( void );
                          virtual void  checkforcampaign( void );
                          virtual void  evaluatemapinfo( char* srname );
                      };

class  tchoosenewsinglelevel : public tchoosenewmap {
                     public:
                          void          init ( void );
                          virtual void  run ( void );
                          virtual void  checkforcampaign( void );
                          virtual void  evaluatemapinfo( char* srname );
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

/*
extern void  producemunition( pmunition    munition,
                             int *    energy,
                             int *    material,
                             int *    sprit,
                             pmunition    autoprod);
*/

class   tshownewtanks : public tdialogbox {
                         public:
                           void          init ( char*      buf2 );
                           virtual void  run  ( void );
                           virtual void  buttonpressed ( char id );
                         protected:
                           char*      buf;
                           int           status;
                        };

extern void  showtechnology(ptechnology  t);
extern void researchinfo ( void );
extern void choosetechlevel ( void );
extern void editmessage ( pmessage msg );
extern void newmessage ( void );
extern void viewmessages ( char* title, pmessagelist strt, int editable, int md  ) ;   // mode : 0 verschickte ; 1 empfangene
extern void viewmessage ( pmessage message );
extern void editmessage ( pmessage msg );
extern void editjournal ( void );
extern void viewjournal ( void );



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

extern void settechlevel ( int techlevel, int playerBM );
extern void showGameParameters ( void );


#endif


