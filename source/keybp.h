//     $Id: keybp.h,v 1.14 2000-10-14 15:31:54 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.13  2000/10/11 14:26:41  mbickel
//      Modernized the internal structure of ASC:
//       - vehicles and buildings now derived from a common base class
//       - new resource class
//       - reorganized exceptions (errors.h)
//      Split some files:
//        typen -> typen, vehicletype, buildingtype, basecontainer
//        controls -> controls, viewcalculation
//        spfst -> spfst, mapalgorithm
//      bzlib is now statically linked and sources integrated
//
//     Revision 1.12  2000/06/23 09:24:17  mbickel
//      Fixed crash in replay
//      enabled cursor movement in stredit
//
//     Revision 1.11  2000/06/01 15:27:46  mbickel
//      Some changes for the upcoming Win32 version of ASC
//      Fixed error at startup: unable to load smalaril.fnt
//
//     Revision 1.10  2000/02/05 12:13:44  steb
//     Sundry tidying up to get a clean compile and run.  Presently tending to SEGV on
//     startup due to actmap being null when trying to report errors.
//
//     Revision 1.9  2000/01/04 19:43:52  mbickel
//      Continued Linux port
//
//     Revision 1.8  2000/01/01 19:04:18  mbickel
//     /tmp/cvsVhJ4Z3
//
//     Revision 1.7  1999/12/30 21:04:46  mbickel
//      Restored DOS compatibility again.
//
//     Revision 1.6  1999/12/30 20:30:35  mbickel
//      Improved Linux port again.
//
//     Revision 1.5  1999/12/28 22:04:26  mbickel
//      Had to make some changes again to compile it for DOS...
//
//     Revision 1.4  1999/12/28 21:03:00  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.3  1999/12/27 12:51:38  mbickel
//      Cleanup for Linux port
//
//     Revision 1.2  1999/11/16 03:41:53  tmwilson
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

#ifdef _DOS_
 #include "dos/keysymbols.h"
#else
 #ifdef _SDL_
  #include "sdl/keysymbols.h"
 #endif
#endif

 typedef int tkey;

 extern char  skeypress( tkey keynr);
 extern char *get_key(tkey keynr);
 extern tkey r_key(void);
 extern int  rp_key(void);
 extern int keypress(void);
 extern void initkeyb(void);
 extern void closekeyb(void);
 extern void wait(void);
 extern tkey char2key (int ch);
 extern void getkeysyms ( tkey* keysym, int* keyprnt );

 extern int exitprogram;
