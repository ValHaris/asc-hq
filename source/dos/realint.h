//     $Id: realint.h,v 1.2 2009-04-18 13:48:38 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.1  2000/05/30 18:39:29  mbickel
//      Added support for multiple directories
//      Moved DOS specific files to a separate directory
//
//     Revision 1.3  2000/05/23 20:40:48  mbickel
//      Removed boolean type
//
//     Revision 1.2  1999/11/16 03:42:24  tmwilson
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
#include <stdio.h>
#include <i86.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>

#include "../tpascal.inc"

class call_real_int {
          short int memsize;
          REGS reg;
          unsigned short int infoselector;
          SREGS segmentreg;
       public :
          call_real_int(short int msize);
          ~call_real_int(void);
          char real_call(char intnr);
          void setuprmi(void);
          unsigned short int realsegment; // Segment Adresse im Realmode
          unsigned short int protectedsegment; // Segment Adresse im Protectedmode
};

// To call a real mode interrupt, first run setuprmi then fill the RMI-Registers with your Register-Values
// and then call real_call( Interrupt - Number )

struct rminfo {
        long            edi;
        long            esi;
        long            ebp;
        long            reserved_by_system;
        union {
            long            ebx;
            char            bl,bh,ebl,ebh;
            word           bx,hbx;
        };
        long            edx;
        union {
           long            ecx;
           word           cx,hcx;
        };
        long            eax;
        short           flags;
        short           es, ds, fs, gs, ip, cs, sp, ss;
     } ;

extern rminfo rmi;
#endif
