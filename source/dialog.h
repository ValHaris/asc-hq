//     $Id: dialog.h,v 1.12 2000-08-26 15:33:41 mbickel Exp $

//
//     $Log: not supported by cvs2svn $
//     Revision 1.11  2000/08/13 09:54:00  mbickel
//      Refuelling is now logged for replays
//
//     Revision 1.10  2000/08/12 12:52:45  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.9  2000/08/06 11:38:48  mbickel
//      New map paramter: fuel globally available
//      Mapeditor can now filter buildings too
//      Fixed unfreed memory in fullscreen image loading
//      Fixed: wasted cpu cycles in building
//      map parameters can be specified when starting a map
//      map parameters are reported to all players in multiplayer games
//
//     Revision 1.8  2000/08/02 15:52:50  mbickel
//      New unit set definition files
//      demount accepts now more than one container file
//      Unitset information dialog added
//
//     Revision 1.7  2000/06/08 21:03:41  mbickel
//      New vehicle action: attack
//      wrote documentation for vehicle actions
//
//     Revision 1.6  2000/05/23 20:40:42  mbickel
//      Removed boolean type
//
//     Revision 1.5  2000/03/29 09:58:44  mbickel
//      Improved memory handling for DOS version
//      Many small changes I can't remember ;-)
//
//     Revision 1.4  1999/12/27 12:59:51  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.3  1999/12/07 22:13:18  mbickel
//      Fixed various bugs
//      Extended BI3 map import tables
//
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
                   void          buttonpressed( int id );
                   void          calcnewposition( integer mousexpos, integer mouseypos );
                   void          button( byte number );
                   void          setnewcoordinates( integer x, integer y );
                   void          run(  int* new_sel, int* new_pos );
                   void          done ( void );
                };

*/




   
extern void  vehicle_information ( pvehicletype type = NULL);
   
extern int   dispmessage2(int          id,
                          char *       st = NULL );
   
extern void  loadmessages(void);
   
extern void  statisticarmies(void);
   
extern void  statisticbuildings(void);
   
extern int   choice_dlg(char *       title,
                        char *       s1,
                        char *       s2,
                        ... );
   

extern void  checkscreensaver() ;


class tenterfiledescription : public tdialogbox {
                       public:
                         char* description;
                         void init ( char* descrip = NULL );
                         void buttonpressed ( int id );
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
                   char              ok;
                    
                   char              scrollbarvisible;
                   char                 action;
                   int                  textstart;

                   void                 init( char* titlet, char* texttoview );
                   virtual void         setup();
                   void                 buildgraphics ( void );
                   virtual void         run ( void );
                   virtual void         buttonpressed( int id);
                   void                 repaintscrollbar( void );
                };
*/

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
                                           
               void lne(int          x1, int          y1, char *       s, int          position, char      einfuegen);

               virtual int checkforreask ( int crc );

               virtual int    gettextwdth_stredit ( char* txt, pfont font );             
            public:
               int  getcapabilities ( void );
               void init ( int* crc, int mode, char* ttl = NULL );  // mode : 0 = es muá unbedingt ein passwort eingegeben werden; 1 = Eingabe kann abgebrochen werden
               void  run ( int* result );
               void buttonpressed ( int id );
           };


#ifndef karteneditor
#include "network.h"
class taskforsupervisorpassword : public tenterpassword {
                           virtual int checkforreask ( int crc );
                         public:
                           void init ( int* crc, int mode );
                        };
#endif


extern int enterpassword ( int* cr );
  /* returns: 1: OK
              2: default
              10: cancel
  */


extern int encodepassword ( char* pw );
extern void viewUnitSetinfo ( void );
extern int selectgameparameter( int lc );
extern void setmapparameters ( void );



#endif
