//     $Id: dialog.h,v 1.15 2001-01-21 12:48:35 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.14  2000/11/29 09:40:18  mbickel
//      The mapeditor has now two maps simultaneously active
//      Moved memorychecking functions to its own file: memorycheck.cpp
//      Rewrote password handling in ASC
//
//     Revision 1.13  2000/09/16 11:47:26  mbickel
//      Some cleanup and documentation again
//
//     Revision 1.12  2000/08/26 15:33:41  mbickel
//      Warning message displayed if empty password is entered
//      pcxtank now displays error messages
//
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

/*! \file dialog.h
   A lot of dialog boxes, used by both ASC and the mapeditor.
*/


#include "dlg_box.h"
#include "loaders.h"
#include "password.h"

const int dbluedark = 248;

#define maxstringlength 50  

/*! Selects a file

  \param ext the wildcard to search ( *.map for example )
  \param filename A pointer to an array where the selected filename will be written to. If string is empty, the dialog was canceled.
  \param mode 1: open file for loading   0: select filename to write to
*/

extern void  fileselectsvga(const char *       ext, 
                             char *       filename,
                             char         mode );



extern void  startnextcampaignmap(word         id);

extern void  setupalliances( int supervisor = 0 );




   
extern void  vehicle_information ( pvehicletype type = NULL);
   
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
   


/*
class tenterfiledescription : public tdialogbox {
                       public:
                         char* description;
                         void init ( char* descrip = NULL );
                         void buttonpressed ( int id );
                         void run ( void );
                       private:
                         char status;
                      };
*/


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



#endif
