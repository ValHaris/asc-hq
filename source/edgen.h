//     $Id: edgen.h,v 1.6 2000-12-23 13:19:46 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.5  2000/08/12 12:52:46  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.4  2000/05/23 20:40:44  mbickel
//      Removed boolean type
//
//     Revision 1.3  2000/03/29 09:58:45  mbickel
//      Improved memory handling for DOS version
//      Many small changes I can't remember ;-)
//
//     Revision 1.2  1999/11/16 03:41:32  tmwilson
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

#define maxtilevals 7

#define cverydeepwater 30
#define cdeepwater 29
#define cwater 28
#define cshallowwater 27
#define cland 43
#define cmount darkgray

#define cforest 44

#define cdesert 34

#define cfewmaterial 21
#define cmediummaterial 22
#define cmuchmaterial 23

#define cfewfuel 181
#define cmediumfuel 182
#define cmuchfuel 183

#define numofbdts 12

#define ctransparent 255

#define clland 0
#define clforest 1
#define cldesert 2
#define clmaterial 3
#define clfuel 4


struct tmemoryblock {
   word *mempointer;
   word maxset,actpres;
   word res;
   word color[maxtilevals+1];
   word tileval[maxtilevals];
   char bordername[maxtilevals][20];
   char generated;
   word startblocksize;
   pascal_byte tilevalcount,acttile;
   };

typedef tmemoryblock *pmemoryblock;

class tplasma {
    public :
        word maxx,maxy,maxvalue;
        char flip;
        int blockcount;
        pmemoryblock memblock;
        tplasma(void);
        void membar(word x1 ,word y1 ,word x2 ,word y2, word color );
        word getmem(word x, word y);
        int creatememblock(void);
        void setmemory(word x, word y, word color);
        /*void preview(word sx, word sy,word barsize);
        void process(word sx, word sy,word barsize);*/
        void generateplasma(char resettile);

        int xsymm, ysymm;
   };


extern word random(word max);
extern int mapgenerator(void);
