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
   boolean generated;
   word startblocksize;
   byte tilevalcount,acttile;
   };

typedef tmemoryblock *pmemoryblock;

class tplasma {
    public :
        word maxx,maxy,maxvalue;
        boolean flip;
        int blockcount;
        pmemoryblock memblock;
        tplasma(void);
        void membar(word x1 ,word y1 ,word x2 ,word y2, word color );
        word getmem(word x, word y);
        int creatememblock(void);
        void setmem(word x, word y,word color);
        /*void preview(word sx, word sy,word barsize);
        void process(word sx, word sy,word barsize);*/
        void generateplasma(boolean resettile);

        int xsymm, ysymm;
   };


extern word random(word max);
extern int mapgenerator(void);
