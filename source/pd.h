//     $Id: pd.h,v 1.4 2000-05-23 20:40:48 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.3  1999/12/28 21:03:18  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.2  1999/11/16 03:42:22  tmwilson
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


   #define rz 21
   #define rz2 245
   #define maxnamelength 30
   #define textdifference 20
   #define pdfieldenlargement 7

   typedef struct tpdbutton {
          char         name[maxnamelength];
          tkey         shortkey;
          char      tswitch;
          int          actionid;
       } tpdbutton;

   typedef struct tpdfield {
           char            name[maxnamelength];
           byte            count;
           dynamic_array<tpdbutton>  button;
           tkey            shortkey;
           word            rtextstart,xstart,xwidth,height;
        } tpdfield;
   typedef tpdfield *ppdfield;

   typedef struct pdbar {
           word    pdbreite;
           dynamic_array<tpdfield>   field;
           byte    count;
   } pdbar;

   class tpulldown {
         int pdfieldnr, buttonnr;
      public :
         char barstatus;
         byte action,mousestat;
         byte bkgcolor,textcolor,shortkeycolor,rcolor1,rcolor2;
         int pdfieldtextdistance,textstart,righttextdifference,anf,ende;
         void *backgrnd,*barbackgrnd;
         char lt[100];
         char rt[100];
         pdbar pdb;
         tkey key;
         int action2execute;

         virtual void init(void);
         virtual void setvars(void);
         virtual void baron(void);
         virtual void baroff(void);
         virtual void openpdfield(void);
         virtual void closepdfield(void);
         virtual void showbutton(void);
         virtual void hidebutton(void);
         virtual void checkpulldown(void);
         virtual void checkkeys(void);
         virtual void run(void);
         virtual void setshortkeys(void);
         virtual void lines(int x1,int y1,int x2,int y2);
         virtual void nolines(int x1,int y1,int x2,int y2);
         virtual void tpulldown::getleftrighttext(char *qtext, char *ltext, char *rtext);
         virtual int getpdfieldheight(byte pdfieldnr,byte pos);
         virtual void done(void);
         void addbutton ( char* name, int id );
         void addfield ( char* name );
         tpulldown ( void );
         virtual ~tpulldown() {};
   };




   extern pfont        pulldownfont;
