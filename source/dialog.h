//     $Id: dialog.h,v 1.3 1999-12-07 22:13:18 mbickel Exp $

//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  1999/11/16 03:41:21  tmwilson
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

#ifndef dialog_h
#define dialog_h

#include "dlg_box.h"
#include "loaders.h"

#define   dbluedark 248

#define maxstringlength 50  


extern void  fileselectsvga(const char *       ext, 
                            char *       filename,
                            char         mode,
                            char *       description = NULL );






extern void  startnextcampaignmap(word         id);

extern void  setupalliances( int supervisor = 0 );


/*
class    tscrollbarn {
             public:
                   integer               x1,x2,y1,y2,length,pos;
                   integer               numberofitems,visibleitems;
                   virtual void          scrollbutton( char    status, integer _pos );
                   void                  init( integer x1_,     integer y1_,    integer x2_, integer y2_,
                                               integer max_sel, integer ges_anz );
                   void          buttonpressed( byte number );
                   void          calcnewposition( integer mousexpos, integer mouseypos );
                   void          button( byte number );
                   void          setnewcoordinates( integer x, integer y );
                   void          run(  int* new_sel, int* new_pos );
                   void          done ( void );
                };

*/




   
extern void  vehicle(void);
   
extern int   dispmessage2(int          id,
                          char *       st);
   
extern void  loadmessages(void);
   
extern void  statisticarmies(void);
   
extern void  statisticbuildings(void);
   
extern byte  choice_dlg(char *       title,
                        char *       s1,
                        char *       s2,
                        ... );
   

extern void  checkscreensaver() ;


class tenterfiledescription : public tdialogbox {
                       public:
                         char* description;
                         void init ( char* descrip = NULL );
                         void buttonpressed ( char id );
                         void run ( void );
                       private:
                         char status;
                      };




extern void testdisptext ( void );
extern void showmap ( void );

extern char mix3colors ( int p1, int p2, int p3 );
extern char mix2colors ( int a, int b );
extern char mix4colors ( int a, int b, int c, int d );

extern char*        getmessage(word         id);

/*

class tviewanytext : public tdialogbox, public tviewtextwithscrolling {
               public:
                   char                 *txt;
                   boolean              ok;
                    
                   boolean              scrollbarvisible;
                   char                 action;
                   int                  textstart;

                   void                 init( char* titlet, char* texttoview );
                   virtual void         setup();
                   void                 buildgraphics ( void );
                   virtual void         run ( void );
                   virtual void         buttonpressed( char id);
                   void                 repaintscrollbar( void );
                };
*/

  class   tviewanytext : public tdialogbox, public tviewtextwithscrolling {
               public:
                   char                 *txt;
                   boolean              ok;
                    
                   boolean              scrollbarvisible;
                   char                 action;
                   
                   // int                  textsizey, textsizeycomplete;
                   int                  textstart;
                   int                  rightspace;

                   void                 init( char* ttl, char* text , int xx1 = 50, int yy1 = 50 , int xxsize = 360, int yysize = 360 );
                   virtual void         run ( void );
                   virtual void         buttonpressed( char id);
                   void                 redraw ( void );
                   int                  getcapabilities ( void ) { return 1; };
                   void                 repaintscrollbar ( void );
                };
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
extern void resizemap ( void );

#ifdef FREEMAPZOOM
extern void choosezoomlevel ( void );
#endif

extern void  verlademunition(pvehicle     vehicle,
                             pvehicle     vehicle2,
                             pbuilding    building,
                             byte         fast);
              /*  entweder muá vehicle2 oder building = NULL sein  */ 
     /* fast: */
     /*  0: innen dialog  */ 
     /*  1: innen fast  */ 
     /*  2: auáen dialog  */ 
     /*  3: auáen fast  */ 

extern void viewterraininfo ( void );



class tenterpassword : public tdialogbox {
             protected:
               char strng1[40];
               char strng2[40];
               
               int status;
               int *cr;
               int reask;
               int confirm;

               void dispeditstring ( char* st , int   x1, int   y1 );
                                           
               void lne(int          x1, int          y1, char *       s, int          position, boolean      einfuegen);

               virtual int checkforreask ( int crc );

               virtual int    gettextwdth_stredit ( char* txt, pfont font );             
            public:
               int  getcapabilities ( void );
               void init ( int* crc, int mode, char* ttl = NULL );  // mode : 0 = es muá unbedingt ein passwort eingegeben werden; 1 = Eingabe kann abgebrochen werden
               void  run ( int* result );
               void buttonpressed ( char id );
           };


#ifndef karteneditor
#include "network.h"
class taskforsupervisorpassword : public tenterpassword {
                           virtual int checkforreask ( int crc );
                         public:
                           void init ( int* crc, int mode );
                        };
#endif


extern void enterpassword ( int* cr );
extern int encodepassword ( char* pw );


#endif
