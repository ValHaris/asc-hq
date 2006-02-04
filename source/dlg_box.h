/*! \file dlg_box.h
    \brief Interface for some basic classes from which all of ASC's dialogs are derived
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

#ifndef dlg_boxH
#define dlg_boxH

#include <cstring>

#include "newfont.h"
#include "events.h"
#include "basegfx.h"

  #define dlg_wintitle 1  
  #define dlg_in3d 2  
  #define dlg_notitle 4  
  #define dlg_3dtitle 8


  class   tdlgengine {
                 protected:
                    int              x1, y1, xsize, ysize;

                 public:

                    struct tbutton {
                               int  art;                   /*  art:  0: normaler button
                                                                                  style:
                                                                                    þ 1:   standard
                                                                                    þ 2:   mit "dauerfeuer"  */

                               integer      x1, y1, x2, y2;              /*  1: texteingabefield  */
                               int id;                          /*  2: zahleingabefield  */
                               int style;                       /*  3: checkbox
                                                                                  style:
                                                                                     þ 10 : nur einschaltbar */
                               int status;                      /*  4: schieberegler    */
                               tbutton*      next;                        /*  5: scrollbar        */
                               const char*  text;
                               void*      data;
                               void*      data2;
                               int      min, max;   // max = itemsvisible bei scrollbar
                               char      active;

                               int          keynum;
                               tkey         key[6];

                               int          markedkeynum;
                               tkey         markedkey[6];
                               int          scrollspeed;
                               char         pressed;
                               int          newpressed;    // nur f?r Maus-"Dauerfeuer"
                               int          lasttick;      //  nur f?r Maus-"Dauerfeuer"
                               const char** entries;       // only for drop down
                               int          entrynum;      // only for drop down
                            };

                    typedef tbutton* pbutton;



                  struct ttaborder {
                     int         id;
                     tbutton*      button;
                     integer      x1, y1, x2, y2;
                  };

                     pbutton      firstbutton;

                     tkey         taste;
                     int          prntkey;

                     void         addbutton( const char *       ltxt,
                       int          lx1,
                       int          ly1,
                       int          lx2,
                       int          ly2,
                       int         lart,
                       int         lstyle,
                       int         lid,
                       char      enabled);
                     void         addbutton( const char *       ltxt,
                       tmouserect   rect1,
                       int         lart,
                       int         lstyle,
                       int         lid,
                       char      enabled);

                                                       /********************************/
                                                       /* art:  0: Normaler Button     */
                                                       /*       1: Texteingabefield     */
                                                       /*       2: ZahlEingabefield     */
                                                       /*       3: CheckBox            */
                                                       /*       4: Schieberegler       */
                                                       /*       5: Scrollbar           */
                                                       /*       6: Drop Down           */
                                                       /********************************/


                     void         addeingabe(int         lid,
                        void*      data,
                        int      min,
                        int      max);

                     void         addscrollbar(int          lx1,
                          int          ly1,
                          int          lx2,
                          int          ly2,
                          int*         numberofitems,
                          int          itemsvisible,
                          int*         actitem,
                          int         lid,
                          int         keys);  /*   0 = keine tasten-, 1 = immer tasten-, 2= markiert tasten  */
                                   /*   !! a scrollbar needs the two ids following lid for internal usage !!   */ 
                     void         addscrollbar( tmouserect rec,
                          int*         numberofitems,
                          int          itemsvisible,
                          int*         actitem,
                          int         lid,
                          int         keys);

                     void addDropDown( int x1, int y1, int x2, int y2, int ID, const char** entries, int entrynum, int* pos );


                     virtual void         buttonpressed(int         id) ;
                     virtual void         showbutton(int         id) = 0;
                      void                 clearkey ( char id );
                      void                 addkey(int         id, tkey         key);
                      void                 addmarkedkey( int id, tkey         key);
                     pbutton getbutton ( int id );

                     virtual ~tdlgengine() {};
                  };
typedef class tdialogbox* pdialogbox;

  class   tdialogbox : public tdlgengine  {
                     bool eventQueue;
                 public:
                     int             textcolor;
                     integer          starty;
                     int           ms;
                     void*      tp;
                     char      imagesaved; 
                     const char*        title;
                     int         windowstyle;
                     ttaborder    taborder[100]; 
                     int         tabcount; 
                     int         markedtab; 
                     int         disablecolor; 
                     int          boxstatus;
                     static collategraphicoperations* pcgo;

                     tdialogbox();

                     void         init(void);
                     void         buildgraphics(void);       
                     virtual void   changecoordinates(void)  ;
                     virtual void   run(void) ;
                     void         done(void);


                      virtual char      checkvalue(int         id, void*      p)  ;
                      void                 editfield(pbutton      pb);
                      void                 editfield( int id );
                      void                 toggleswitch(pbutton      pb);
                      virtual void         enablebutton(int         id)  ;
                      virtual void         disablebutton(int         id)  ;
                      virtual void         execbutton(pbutton      pb, char      mouse) ;
                      void         showbutton(int         id);
                      void         hidebutton(int         id);
                      void         setscrollspeed(char        id , int  speed);
                      void         rebuildtaborder(void);
                      void         showtabmark(int         b);


                      virtual void setclipping ( void );
                      virtual void paint     ( void );    // komplettes zeichnen, mit framebuf, redraw und copy ...
                      virtual void redrawall ( void );    // l„át die aktuelle und alle darunterliegenden Boxen sich neu zeichnen
                      virtual void redrawall2 ( int xx1, int yy1, int xx2, int yy2 );    // l„át die aktuelle und bei Bedarf alle darunterliegenden Boxen sich neu zeichnen
                      virtual int  getcapabilities ( void );  // Dann muá REDRAW ?berladen werden
                      void repaintdisplay( void );
              protected:
                      int virtualbufoffset;
                      pdialogbox next;
                      pdialogbox prev;
                      static pdialogbox first;

                      int dlg_mode;           /*            Bit 1 :  redraw funktionalit„t 
                                                                2 :  zugrunde liegende Dialogboxen unterst?tzen redraw  */

                      virtual void redraw ( void );       // Zeichner die dialogbox in den aktuellen Puffer

                      virtual void setvirtualframebuf ( void );
                      virtual void copyvirtualframebuf ( void );

                      char knopfsuccessful;
                      void         newknopf(integer      xx1,
                                            integer      yy1,
                                            integer      xx2,
                                            integer      yy2);
                      void         knopfdruck(int      xx1,
                                              int      yy1,
                                              int      xx2,
                                              int      yy2);
                      void         knopf(integer      xx1,
                                         integer      yy1,
                                         integer      xx2,
                                         integer      yy2);
                      void         newknopfdruck4(integer      xx1,
                                                  integer      yy1,
                                                  integer      xx2,
                                                  integer      yy2);
                      void         newknopfdruck3(integer      xx1,
                                                  integer      yy1,
                                                  integer      xx2,
                                                  integer      yy2);
                      void         newknopfdruck2(integer      xx1,
                                                  integer      yy1,
                                                  integer      xx2,
                                                  integer      yy2);

                      void         newknopfdruck(integer      xx1,
                                                 integer      yy1,
                                                 integer      xx2,
                                                 integer      yy2);
                      void         rahmen(bool      invers,
                                          int          x1,
                                          int          y1,
                                          int          x2,
                                          int          y2);
                      void         rahmen(bool      invers,
                                          tmouserect   rect );

                      void         rahmen3(char *       txt,
                                           integer      x1,
                                           integer      y1,
                                           integer      x2,
                                           integer      y2,
                                           int         style);
                      void         bar ( tmouserect rect, int color );
                      void         bar ( int x1, int y1, int x2, int y2, int color );

                      virtual void dispeditstring ( char* st ,
                                                    int   x1, 
                                                    int   y1 );
                                           
                      void         stredit(char *       s,
                                           int          x1,
                                           int          y1,
                                           int          wdth,
                                           int          max);
                                           
                      virtual int    gettextwdth_stredit ( char* txt, 
                                                           pfont font );
                                           
                      virtual void         lne(int          x1,
                                               int          y1,
                                               char *       s,
                                               int          position,
                                               char      einfuegen);
                                               
                      void         intedit(int     *    st,
                                           int          x1,
                                           int          y1,
                                           int          wdth,
                                           int          min,
                                           int          max);
                      void         paintsurface2 ( int xx1, int yy1, int xx2, int yy2 );
                      void         paintsurface  ( int xx1, int yy1, int xx2, int yy2 );
                      virtual ~tdialogbox();
                   };



/** displays a dialog box with a message
   \param formatstring the text, which may contain the same format arguments as sprintf
   \param num 0 normal text, the dialog box will stay visible until removemessage() is called
              1 red text for error message
              2 fatal error, the program will be closed
              3 normal text
*/
extern void displaymessage( const char* formatstring, int num, ... );


/** displays a dialog box with a message
   \param text the text
   \param num 0 normal text, the dialog box will stay visible until removemessage() is called
              1 red text for error message
              2 fatal error, the program will be closed
              3 normal text
*/
extern void displaymessage( const ASCString& text, int num );

//! closes a message dialog box that has been opened by displaymessage()
extern void removemessage( void );

//! displays a dialogbog with the given help topic \sa viewtext2(int)
extern void  help( int id);

//! displays a dialogbog with the given message \sa help(int)
extern void viewtext2 ( int id);

extern int  viewtextquery( int        id,
                           char *       title,
                           char *       s1,
                           char *       s2);


class tviewtext {
      protected:
         struct tstartpoint {
                     int ypos;
                     int textcolor;
                     int background;
                     pfont font;
                     int eeinzug;
                     int aeinzug;
                     int height;
                     const char* textpointer;
                     tstartpoint* next;
                     int xpos;
                     int maxlineheight;
                };
         typedef tstartpoint* pstartpoint;

       public: 
         tviewtext();

         void setparams ( int xx1, int yy1, int xx2, int yy2, const char* ttxt, char clr, char bkgr);
         void setpos ( int xx1, int yy1, int xx2, int yy2 );

         void displaytext ( void );
         void evalcommand ( const char** s);
         void nextline ( int einzug, const char* txtptr );
         void displaysingleline ( char* t);
         void fillline ( int x1, int x2 );
         void setnewlineheight ( int h );

         int tvt_x1, tvt_y1, tvt_x2, tvt_y2, tvt_xp,  tvt_yp, tvt_starty;
         int tvt_color, tvt_background;
         int tvt_dispactive;
         int eeinzug, aeinzug;
         int tvt_maxlineheight;
         const char*  tvt_text;
         char*  actline;
         void*  tvt_firstlinebuf;
         tgraphmodeparameters tvt_firstlinebufparm;
         int   tvt_firstlinebufheight;
                     
         virtual ~tviewtext();
       protected:
         pstartpoint tvt_startpoint;
         int defaulttextcolor;
      };


class tviewtextwithscrolling : public tviewtext {
        public:
          void checkscrolling ( void );
          int textsizey, textsizeycomplete;
          virtual void repaintscrollbar ( void ) = 0;
      };

extern int actdisplayedmessage;
extern long lastdisplayedmessageticker ;


#define linesshown 13  

class tvirtualscreenbuf {
      public:
        int size; 
        tvirtualscreenbuf ( void );
        void init ( void );
        ~tvirtualscreenbuf ();
        void* buf;
      } ;

extern tvirtualscreenbuf virtualscreenbuf; 
extern int getplayercolor ( int i );

extern void   strrd8u(int  l, char* s);   // aufrunden
extern void   strrd8d(int  l, char* s);   // abrunden
extern void   strrd8n(int  l, char* s);   // mathematisch korrekt runden
extern char*  strrrd8u(int  l);           // aufrunden
extern char*  strrrd8d(int  l);           // abrunden
extern char*  strrrd8n(int  l);           // mathematisch korrekt runden


/** returns the message with the given ID from the message system. This system spans
    the helpfiles and the message files associated with the maps */
extern ASCString readtextmessage( int id );



class   tstringselect : public tdialogbox {
                public :
                     char txt[2000];
                     char  ok;
                     int sy,ey,sx,ex,action,dx;
                     int dk;
                     int msel,mouseselect,redline,lnshown,numberoflines,firstvisibleline,startpos;
                     char scrollbarvisible;
                     void init(void);
                     tstringselect ( );
                     virtual void setup(void);
                     virtual void run(void);
                     virtual void buttonpressed(int id);
                     void scrollbar_on(void);
                     void viewtext(void);
                     virtual void resettextfield(void);
                     virtual void get_text( int nr);
                     void done(void);
                 };

extern int getid( const char* title, int lval, int min, int max );
extern ASCString editString( const ASCString& title, const ASCString& defaultValue = "" );


/** displays a dialogbox which lets you chose one of a number of strings.
    \param title the title of the dialog box
    \param entries the list of strings
    \returns the selected index or -1 if nothing was selected
*/
extern int chooseString ( const ASCString& title, const vector<ASCString>& entries, int defaultEntry = -1 );

/** displays a dialogbox which lets you chose one of a number of strings.
    \param title the title of the dialog box
    \param entries the list of strings
    \returns button-number, selected index 
*/
extern pair<int,int> chooseString ( const ASCString& title, const vector<ASCString>& entries, const vector<ASCString>& buttons, int defaultEntry = -1 );


#endif

