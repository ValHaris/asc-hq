/*! \file dialog.h
    \brief Interface for all the dialog boxes used by the game and the mapeditor
*/

//     $Id: dialog.h,v 1.22 2002-03-03 14:13:48 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.21  2002/02/21 17:06:50  mbickel
//      Completed Paragui integration
//      Moved mail functions to own file (messages)
//
//     Revision 1.20  2001/10/11 10:41:06  mbickel
//      Restructured platform fileio handling
//      Added map archival information to mapeditor
//
//     Revision 1.19  2001/10/02 14:06:28  mbickel
//      Some cleanup and documentation
//      Bi3 import tables now stored in .asctxt files
//      Added ability to choose amoung different BI3 import tables
//      Added map transformation tables
//
//     Revision 1.18  2001/02/01 22:48:36  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.17  2001/01/31 14:52:35  mbickel
//      Fixed crashes in BI3 map importing routines
//      Rewrote memory consistency checking
//      Fileselect dialog now uses ASCStrings
//
//     Revision 1.16  2001/01/28 14:04:11  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.15  2001/01/21 12:48:35  mbickel
//      Some cleanup and documentation
//
//     Revision 1.14  2000/11/29 09:40:18  mbickel
//      The mapeditor has now two maps simultaneously active
//      Moved memorychecking functions to its own file: memorycheck.cpp
//      Rewrote password handling in ASC
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

#ifndef dialog_h
#define dialog_h

#include "ascstring.h"
#include "dlg_box.h"
#include "loaders.h"
#include "password.h"

const int dbluedark = 248;

#define maxstringlength 50  

/*! Selects a file

  \param ext the wildcard to search ( *.map for example )
  \param filename A string which will contain the selected filename. If it is empty, the dialog was canceled.
  \param load  true for selecting an existing file for loading; false for entering a filename to save to
*/
extern void   fileselectsvga( const ASCString& ext, ASCString& filename, bool load );

extern void  startnextcampaignmap(word         id);

extern void  setupalliances( int supervisor = 0 );




   
extern void  vehicle_information ( const Vehicletype* type = NULL);
   
//! displays a message in the message line
extern int   dispmessage2(int          id,
                          char *       st = NULL );
   
//! loads all messages from the files message?.txt
extern void  loadmessages(void);
   
extern void  statisticarmies(void);
   
extern void  statisticbuildings(void);
   
/*! displays a dialog with two buttons, to select one of them

    \param title: the message text; printf style arguments allowed
    \param s1 the text on the left button
    \param s2 the text on the right button
    \returns 1 if the left button has been pressed; 2 if the right button has been pressed
*/
extern int   choice_dlg(char *       title,
                        char *       s1,
                        char *       s2,
                        ... );
   


//! shows a small overview map in a dialog box
extern void showmap ( void );


extern char mix3colors ( int p1, int p2, int p3 );
extern char mix2colors ( int a, int b );
extern char mix4colors ( int a, int b, int c, int d );

//! returns a pointer to the message id. The messages must have been loaded with loadmessages
extern char*        getmessage(word         id);


  class   tviewanytext : public tdialogbox, public tviewtextwithscrolling {
               public:
                   const char                 *txt;
                   char              ok;
                    
                   char              scrollbarvisible;
                   char                 action;
                   
                   // int                  textsizey, textsizeycomplete;
                   int                  textstart;
                   int                  rightspace;

                   void                 init( char* title, const char* text , int xx1 = 50, int yy1 = 50 , int xxsize = 360, int yysize = 360 );
                   virtual void         run ( void );
                   virtual void         buttonpressed( int id);
                   void                 redraw ( void );
                   int                  getcapabilities ( void ) { return 1; };
                   void                 repaintscrollbar ( void );
                };

//! the dialog box for setting up how to load bi3 graphics and maps. Since ASC now uses its own graphics, this dialog is not used any more.
extern void bi3preferences  ( void );

typedef class tprogressbar* pprogressbar;
class tprogressbar {
       public:
         void start ( int _color, int _x1, int _y1, int _x2, int _y2, pnstream stream );
         void end ( void );
         void point ( void );
         void startgroup ( void );
         void writetostream ( pnstream stream );
       private:
         int x1, y1, x2, y2, color;
         int starttick;
         int time;
         int first;

            struct tgroup {
               int num;
               int orgnum;
               int time;
               int newtime;
               int timefromstart;
               dynamic_array<int> point;
            };
   
            dynamic_array<tgroup> group;
            int groupnum;
            int actgroupnum;

       int ended;
       int lastpaintedpos;
       void lineto ( float pos );


};

class tbasicshowmap {
         public:
            tbasicshowmap ( void );
            int  generatemap ( int autosize );
            void freebuf ( void );
            void dispimage ( void );
            void generatemap_var ( void );
            void init ( int x1, int y1, int xsize, int ysize );
            void checkformouse ( void );

         protected :
            unsigned char      *buffer, *buffer2;
            int       zoom;
            int       mxsize,mysize;
            int       lastmapxsize, lastmapysize;
            int       xofs, yofs;           // Anzahl der Pixel links und oben des virtuellen Bildes, die NICHT angezeigt werden
            int       dispxpos, dispypos;   // Koordinaten des linken oberen fieldes des Bildschirmouseschnitts
            int       border;               
            int       maxzoom;
            int txsize, tysize;   // Gr”áe des angezeigten Fensters
            int bufsizex, bufsizey;
            int xp1, yp1;

            int getbufpos( int x, int y ) { return (y * mxsize + x + 4);};
            int scrxsize ;
            int scrysize ;

            int lastmapposx;
            int lastmapposy;
            int lastmaptick;
            int maxmapscrollspeed;

            void saveimage ( void );

            tmouserect  mapwindow;   // Rechteck, in dem die Karte angezeigt wird. Relativ zu x1/y1 der DLG-Box
            int getfieldposx ( int c );
            int getfieldposy ( int c );
            int getposfieldx ( int c );
            int getposfieldy ( int c );

            void generatemap1 ( void );
            void generatemap2 ( void );
            void generatemap3 ( void );
            // void displaymap ( void );
            void interpolatemap ( void );

            void setmapposition ( void );
    };


//! a dialog box that lets a user resize the active map. Should only be used in the mapeditor
extern void resizemap ( void );

#ifdef FREEMAPZOOM
 extern void choosezoomlevel ( void );
#endif

extern void viewterraininfo ( void );

extern void viewUnitSetinfo ( void );

extern int selectgameparameter( int lc );

extern void setmapparameters ( void );




typedef class tparagraph* pparagraph;
class  tparagraph {
        public:
          tparagraph ( void );
          tparagraph ( pparagraph prv );   // f?gt einen neuen paragraph hinter prv an

          void join ( void );   // returnvalue : paragraph to delete;
          void changesize ( int newsize );

          void addchar ( char c );
          pparagraph erasechar ( int c );
          void checkcursor ( void );
          pparagraph movecursor ( int dx, int dy );
          pparagraph cut ( void );

          int  reflow( int all = 1 );
          void display ( void );

          void checkscrollup ( void );
          void checkscrolldown ( void );
          int  checkcursorpos ( void );

          void addtext ( const ASCString& txt );
          ~tparagraph ();

          void setpos ( int x1, int y1, int y2, int linepos, int linenum );

          void displaycursor ( void );
          int cursor;
          int cursorstat;
          int cursorx;
          int normcursorx;
          int cursory;
          int searchcursorpos;
          static int maxlinenum;

          int size;
          int allocated;
          char* text;

          static int winy1;
          static int winy2;
          static int winx1;
          struct {
             int line1num;
          } ps;

          dynamic_array<char*> linestart;
          dynamic_array<int>   linelength;

          int   linenum;

          pparagraph next;
          pparagraph prev;
      };


class tmessagedlg : public tdialogbox {
           protected:
               int to[8];

                pparagraph firstparagraph;
                pparagraph actparagraph;

                int tx1, ty1, tx2, ty2,ok;
                int lastcursortick;
                int blinkspeed;

            public:
                tmessagedlg ( void );
                virtual void setup ( void );
                void inserttext ( const ASCString& txt );
                void run ( void );
                ASCString extracttext ();
                ~tmessagedlg();
         };

class MultilineEdit : public tmessagedlg  {
               ASCString& text;
               ASCString dlg_title;
               bool textchanged;
            public:
               MultilineEdit ( ASCString& txt, const ASCString& title ) : text ( txt ), dlg_title ( title ), textchanged ( false ) {};
               void init ( void );
               void setup ( void );
               void buttonpressed ( int id );
               void run ( void );
               bool changed ( ) { return textchanged; };
       };

extern void selectgraphicset ( void );


#endif
