/*! \file dlgraph.cpp
    \brief This file is included by dlg_box.cpp !

    It was an attempt to move the graphical dialog functions to their own file, 
     which was never finished.
*/

//     $Id: dlgraph.cpp,v 1.11 2001-02-11 11:39:31 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.10  2001/01/28 14:04:12  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.9  2000/10/16 14:34:12  mbickel
//      Win32 port is now running fine.
//      Removed MSVC project files and put them into a zip file in
//        asc/source/win32/msvc/
//
//     Revision 1.8  2000/08/12 09:17:26  gulliver
//     *** empty log message ***
//
//     Revision 1.7  2000/08/08 09:48:12  mbickel
//
//      speed up of dialog boxes in linux
//      fixed graphical errors in attack
//      fixed graphical error in ammo transfer
//      fixed reaction fire not allowing manual attack
//
//     Revision 1.6  2000/05/23 20:40:43  mbickel
//      Removed boolean type
//
//     Revision 1.5  2000/04/27 16:25:20  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.4  1999/12/30 20:30:32  mbickel
//      Improved Linux port again.
//
//     Revision 1.3  1999/12/28 21:02:52  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.2  1999/11/16 03:41:27  tmwilson
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

void         tdialogbox::rahmen(bool      invers,
                    int          x1,
                    int          y1,
                    int          x2,
                    int          y2)
{
     collategraphicoperations cgs ( x1, y1, x2, y2 );
     
	 char col	=	(invers == false)	?	white	:	darkgray;
     
	 line(x1,y1,x1,y2,col);
     line(x1,y1,x2,y1,col); 
     
	 col	=	(invers == true)	?	white	:	darkgray;
    
	 line(x2,y1,x2,y2,col); 
     line(x1,y2,x2,y2,col); 
} 

void                 tdialogbox::rahmen(	bool      invers,
											tmouserect   rect )
{
   rahmen ( invers, rect.x1, rect.y1, rect.x2,rect.y2 );
}

void         tdialogbox::knopf(integer      xx1,
                   integer      yy1,
                   integer      xx2,
                   integer      yy2)
{ 
     collategraphicoperations cgs ( xx1, yy1, xx2, yy2 );

     paintsurface2(xx1,yy1,xx2,yy2);
     rahmen(false,xx1,yy1,xx2,yy2); 
} 
                      

void         tdialogbox::knopfdruck(int      xx1,
                        int      yy1,
                        int      xx2,
                        int      yy2)
{

     collategraphicoperations cgs ( xx1, yy1, xx2, yy2 );

    void*      p;
    char      kn, kn2; 
    integer      mt; 

     mt = mouseparams.taste; 
     mousevisible(false); 
     kn2 = true; 

     void*    pq = asc_malloc ( imagesize ( xx1 , yy1 , xx2 , yy2 ));
     getimage( xx1 , yy1 , xx2 , yy2 , pq );

     p = asc_malloc ( imagesize ( xx1 + 1,yy1 + 1,xx2 - 2,yy2 - 2 ));
     getimage(xx1 + 1,yy1 + 1,xx2 - 2,yy2 - 2, p);
     putimage(xx1 + 3,yy1 + 3, p );
     rahmen(true, xx1, yy1, xx2, yy2); 
     mousevisible(true); 
     knopfsuccessful = true; 
     kn = true; 
     do { 
           if ((mouseparams.x > xx2) || (mouseparams.x < xx1) || (mouseparams.y > yy2) || (mouseparams.y < yy1)) { 
              knopfsuccessful = false; 
              kn = false; 
           } 
           if (mouseparams.taste != mt) 
              kn = false; 
           releasetimeslice();
     }  while (kn == true);
     mousevisible(false); 

     putimage(xx1 + 1,yy1 + 1, p);
     asc_free ( p );

     putimage( xx1 , yy1 , pq );
     asc_free ( pq );

     mousevisible(true); 
} 



void         tdialogbox::newknopf(integer      xx1,
                      integer      yy1,
                      integer      xx2,
                      integer      yy2)
{ 
     collategraphicoperations cgs ( xx1-1, yy1-1, xx2+1, yy2+1 );

     paintsurface2(xx1,yy1,xx2,yy2);

     rahmen(true,  xx1 - 1, yy1 - 1,xx2 + 1,yy2 + 1);
     rahmen(false, xx1,yy1,  xx2,yy2);
} 


void         tdialogbox::newknopfdruck2(integer      xx1,
                            integer      yy1,
                            integer      xx2,
                            integer      yy2)
{

    void*      p;
    char      kn; 
    integer      mt; 
    {
       collategraphicoperations cgs ( xx1 -1, yy1 -1, xx2 + 1, yy2 + 1 );
       mt = mouseparams.taste;
       mousevisible(false);
       p = asc_malloc ( imagesize ( xx1 + 1,yy1 + 1,xx2 - 2,yy2 - 2 ) );
       getimage(xx1 + 1,yy1 + 1,xx2 - 2,yy2 - 2, p);
       putimage(xx1 + 3,yy1 + 3, p);

       line(xx1 - 1,yy1 - 1,xx2 + 1,yy1 - 1, 8);
       line(xx1 - 1,yy1 - 1,xx1 - 1,yy2 + 1, 8);
       line(xx2 + 1,yy1 - 1,xx2 + 1,yy2 + 1, 15);
       line(xx1 - 1,yy2 + 1,xx2 + 1,yy2 + 1, 15);

       line(xx1,yy1,xx2,yy1, 8);
       line(xx1,yy1,xx1,yy2, 8);
       line(xx2,yy1,xx2,yy2, 15);
       line(xx1,yy2,xx2,yy2, 15);

       mousevisible(true);
       knopfsuccessful = true;
       kn = true;
    }
    do {
           if ((mouseparams.x > xx2) || (mouseparams.x < xx1) || (mouseparams.y > yy2) || (mouseparams.y < yy1)) { 
              knopfsuccessful = false; 
              kn = false; 
           } 
           if (mouseparams.taste != mt) 
              kn = false; 
           releasetimeslice();
    }  while (kn == true);
    {
       collategraphicoperations cgo ( xx1 - 1, yy1 - 1, xx2 + 1, yy2 + 1 );

       mousevisible(false);

       if (knopfsuccessful == false)
          putimage (xx1 + 1,yy1 + 1,p);
       asc_free ( p );

       if (knopfsuccessful == false) {
          line(xx1 - 1,yy1 - 1,xx2 + 1,yy1 - 1, 8);
          line(xx1 - 1,yy1 - 1,xx1 - 1,yy2 + 1, 8);
          line(xx2 + 1,yy1 - 1,xx2 + 1,yy2 + 1, 15);
          line(xx1 - 1,yy2 + 1,xx2 + 1,yy2 + 1, 15);

          line(xx1,yy1,xx2,yy1, 15);
          line(xx1,yy1,xx1,yy2, 15);
          line(xx2,yy1,xx2,yy2, 8);
          line(xx1,yy2,xx2,yy2, 8);
       }
       mousevisible(true);
    }

} 


void         tdialogbox::newknopfdruck3(integer      xx1,
                            integer      yy1,
                            integer      xx2,
                            integer      yy2)
{
     collategraphicoperations cgs ( xx1, yy1, xx2, yy2 );

   void*      p;

     // vom reingedr?ckten in den Normalzustand

     p = asc_malloc ( imagesize ( xx1 + 4,yy1 + 4,xx2 - 2,yy2 - 2 ));
     getimage(xx1 + 4,yy1 + 4,xx2 - 2,yy2 - 2, p );

     putimage(xx1 + 2,yy1 + 2,p);
     asc_free ( p );              

     rahmen(false,xx1,yy1,xx2,yy2); 
} 




void         tdialogbox::newknopfdruck4(integer      xx1,
                            integer      yy1,
                            integer      xx2,
                            integer      yy2)
{
     collategraphicoperations cgs ( xx1, yy1, xx2, yy2 );

   void*      p;
    // reindr?cken

     p = asc_malloc ( imagesize ( xx1 + 2,yy1 + 2,xx2 - 4,yy2 - 4 )) ;
     getimage(xx1 + 2,yy1 + 2,xx2 - 4,yy2 - 4, p);
     putimage(xx1 + 4,yy1 + 4, p);
     asc_free ( p );

     rahmen(true,xx1,yy1,xx2,yy2); 
} 


void         tdialogbox::newknopfdruck(integer      xx1,
                           integer      yy1,
                           integer      xx2,
                           integer      yy2)
{

   void*      p;
   char      kn;
   integer      mt;
   void*    pq ;
   {
     collategraphicoperations cgs( xx1-1, yy1-1, xx2+1, yy2+1 );
     mt = mouseparams.taste;
     mousevisible(false); 

     pq = asc_malloc ( imagesize ( xx1 , yy1 , xx2 , yy2 ));
     getimage( xx1 , yy1 , xx2 , yy2 , pq );


     p = asc_malloc ( imagesize ( xx1 + 1,yy1 + 1,xx2 - 2,yy2 - 2 ));
     getimage(xx1 + 1,yy1 + 1,xx2 - 2,yy2 - 2, p);
     putimage(xx1 + 3,yy1 + 3, p);

     line(xx1 - 1,yy1 - 1,xx2 + 1,yy1 - 1, 8);
     line(xx1 - 1,yy1 - 1,xx1 - 1,yy2 + 1, 8);

     line(xx2 + 1,yy1 - 1,xx2 + 1,yy2 + 1, 15);
     line(xx1 - 1,yy2 + 1,xx2 + 1,yy2 + 1, 15);

     line(xx1,yy1,xx2,yy1, 8);
     line(xx1,yy1,xx1,yy2, 8);

     line(xx2,yy1,xx2,yy2, 15);
     line(xx1,yy2,xx2,yy2, 15);
     mousevisible(true); 

     knopfsuccessful = true; 
     kn = true;
   }
   do {
           if ((mouseparams.x > xx2) || (mouseparams.x < xx1) || (mouseparams.y > yy2) || (mouseparams.y < yy1)) { 
              knopfsuccessful = false; 
              kn = false; 
           } 
           if (mouseparams.taste != mt) 
              kn = false; 
           releasetimeslice();
   }  while (kn == true);
   {
     collategraphicoperations cgo( xx1-1, yy1-1, xx2+1, yy2+1 );
     mousevisible(false); 
     putimage(xx1 + 1,yy1 + 1,p);
     asc_free ( p ) ;

     line(xx1 - 1,yy1 - 1,xx2 + 1,yy1 - 1, 8);
     line(xx1 - 1,yy1 - 1,xx1 - 1,yy2 + 1, 8);

     line(xx2 + 1,yy1 - 1,xx2 + 1,yy2 + 1, 15);
     line(xx1 - 1,yy2 + 1,xx2 + 1,yy2 + 1, 15);

     line(xx1,yy1,xx2,yy1, 15);
     line(xx1,yy1,xx1,yy2, 15);

     line(xx2,yy1,xx2,yy2, 8);
     line(xx1,yy2,xx2,yy2, 8);

     putimage( xx1 , yy1 , pq );
     asc_free ( pq );

     mousevisible(true);
   }
} 
