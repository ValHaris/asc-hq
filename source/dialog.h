/*! \file dialog.h
    \brief Interface for all the dialog boxes used by the game and the mapeditor
*/

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

#ifndef dialogH
#define dialogH


#include <sigc++/sigc++.h>
#include "ascstring.h"
#include "dlg_box.h"
#include "loaders.h"
#include "password.h"


const int dbluedark = 248;

ASCString  selectFile( const ASCString& ext, bool load );

extern void  startnextcampaignmap( int id);


   
extern void displaymessage2( const char* formatstring, ... );
   
//! displays a message in the message line
extern void  dispmessage2(int          id,
                          char *       st = NULL );
   
  
  
// extern void  statisticarmies(void);
   
// extern void  statisticbuildings(void);
   


extern char mix3colors ( int p1, int p2, int p3 );
extern char mix2colors ( int a, int b );
extern char mix4colors ( int a, int b, int c, int d );


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


//! a dialog box that lets a user resize the active map. Should only be used in the mapeditor
extern void resizemap ( void );

extern void choosezoomlevel ( void );

extern void viewterraininfo ( void );

extern void viewUnitSetinfo ( void );

/*! displays a dialog with two buttons, to select one of them

    \param title: the message text; printf style arguments allowed
    \param leftButton the text on the left button
    \param rightButton the text on the right button
    \returns 1 if the left button has been pressed; 2 if the right button has been pressed
 */
extern int   choice_dlg(const ASCString& title,
                        const ASCString& leftButton,
                        const ASCString& rightButton );



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

extern int editInt( const ASCString& title, int defaultValue, int minValue = 0, int maxValue = maxint );


#endif
