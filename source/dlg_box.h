//     $Id: dlg_box.h,v 1.17 2000-11-21 20:27:01 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.16  2000/10/18 14:14:04  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//
//     Revision 1.15  2000/08/26 15:33:42  mbickel
//      Warning message displayed if empty password is entered
//      pcxtank now displays error messages
//
//     Revision 1.14  2000/08/12 12:52:45  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.13  2000/08/12 09:17:25  gulliver
//     *** empty log message ***
//
//     Revision 1.12  2000/08/08 09:48:08  mbickel
//
//      speed up of dialog boxes in linux
//      fixed graphical errors in attack
//      fixed graphical error in ammo transfer
//      fixed reaction fire not allowing manual attack
//
//     Revision 1.11  2000/08/06 11:38:58  mbickel
//      New map paramter: fuel globally available
//      Mapeditor can now filter buildings too
//      Fixed unfreed memory in fullscreen image loading
//      Fixed: wasted cpu cycles in building
//      map parameters can be specified when starting a map
//      map parameters are reported to all players in multiplayer games
//
//     Revision 1.10  2000/08/03 13:12:09  mbickel
//      Fixed: on/off switching of generator vehicle produced endless amounts of energy
//      Repairing units now reduces their experience
//      negative attack- and defenseboni possible
//      changed attackformula
//
//     Revision 1.9  2000/05/23 20:40:42  mbickel
//      Removed boolean type
//
//     Revision 1.8  2000/05/06 20:25:23  mbickel
//      Fixed: -recognition of a second mouse click when selection a pd menu item
//             -movement: fields the unit can only pass, but not stand on them,
//                        are marked darker
//             -intedit/stredit: mouseclick outside is like hitting enter
//
//     Revision 1.7  2000/04/27 16:25:20  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.6  2000/03/29 09:58:45  mbickel
//      Improved memory handling for DOS version
//      Many small changes I can't remember ;-)
//
//     Revision 1.5  2000/01/04 19:43:51  mbickel
//      Continued Linux port
//
//     Revision 1.4  2000/01/01 19:04:17  mbickel
//     /tmp/cvsVhJ4Z3
//
//     Revision 1.3  1999/11/22 18:27:14  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:41:25  tmwilson
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

#ifndef dlg_box_h
#define dlg_box_h

#include <string.h>

#include "newfont.h"
#include "events.h"
#include "basegfx.h"

  #define dlg_wintitle 1  
  #define dlg_in3d 2  
  #define dlg_notitle 4  
  #define dlg_3dtitle 8




  typedef word* pword ;
  typedef struct tbutton* pbutton;

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
                pbutton      next;                        /*  5: scrollbar        */
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
                int          newpressed;     // nur f?r Maus-"Dauerfeuer"
                int          lasttick;      //  nur f?r Maus-"Dauerfeuer"
             };


   struct ttaborder { 
      int         id; 
      pbutton      button; 
      integer      x1, y1, x2, y2; 
   }; 




  class   tdlgengine {
                 protected:
                    int              x1, y1, xsize, ysize;
                     pbutton      firstbutton; 

                 public:
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


                     virtual void         buttonpressed(int         id) ;
                     virtual void         showbutton(int         id) = 0;
                      void                 clearkey ( char id );
                      void                 addkey(int         id, tkey         key);
                      void                 addmarkedkey(word         id, tkey         key);
                     pbutton getbutton ( int id );

                     virtual ~tdlgengine() {};
                  };
typedef class tdialogbox* pdialogbox;

  class   tdialogbox : public tdlgengine  {
                 public:
                     int             textcolor;
                     integer          starty;
                     int           ms;
                     void*      tp;
                     char      imagesaved; 
                     char*        title;
                     word         windowstyle; 
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

typedef char* tstringa[30];
typedef tstringa* pstringa;

class tdisplaymessage : public tdialogbox {
                        char status;
                        int  mode;
                    public:
                        void init ( tstringa a, int md, int linenum );
                        virtual void buttonpressed ( int id );
                        virtual void run ( void );
                   };

extern int  displaymessage2( const char* formatstring, ... );
extern void displaymessage( const char* formatstring, int num, ... );   // num   0: Box bleibt aufgeklappt, 1 box wird geschlossen , text rot (Fehler), 2 : Programm wird beendet; 3 : normaler text ( OK)
extern void removemessage( void );

extern tdisplaymessage* messagebox;
extern char* exitmessage[20];


extern void  help(word         id);

extern void         viewtext2 ( word         id);

extern int  viewtextquery(word         id,
                           char *       title,
                           char *       s1,
                           char *       s2);

extern void  loadtexture ( void );

         typedef struct tstartpoint* pstartpoint;
         struct tstartpoint {
                     int ypos;
                     int textcolor;
                     int background;
                     pfont font;
                     int eeinzug;
                     int aeinzug;
                     int height;
                     const char* textpointer;
                     pstartpoint next;
                     int xpos;
                     int maxlineheight;
                };

class tviewtext {

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

extern char*  readtextmessage( int id );


class   tstringselect : public tdialogbox {
                public :
                     char txt[2000];
                     char  ok;
                     int sy,ey,sx,ex,action,dx;
                     int dk;
                     int msel,mouseselect,redline,lnshown,numberoflines,firstvisibleline,startpos;
                     char scrollbarvisible;
                     void init(void);
                     virtual void setup(void);
                     virtual void run(void);
                     virtual void buttonpressed(int id);
                     void scrollbar_on(void);
                     void viewtext(void);
                     virtual void gettext(word nr);
                     virtual void resettextfield(void);
                     void done(void);
                     };

extern int getid( char* title, int lval, int min, int max );


#endif

