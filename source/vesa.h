//     $Id: vesa.h,v 1.2 1999-11-16 03:42:48 tmwilson Exp $
//
//     $Log: not supported by cvs2svn $
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

#ifndef vesa_h
#define vesa_h

#include "tpascal.inc"
#include "palette.h"

struct tvesainfoblock {
          char       signature[4];                    
          word       vesaversion;
          char       *hersteller;
          int    capabilities;
          word*      modelist;
          word       totalmemory;
          word       oemsoftwareversion;
          char       *vendorname;
          char       *productname;
          char       *productrevision;
          char       dummy[222];
          char       oemdummy[256];
       };


struct tvesamodeinfo {
            word          modeattributes;
            char          winaattributes;
            char          winbattributes;
            word          wingranularity;
            word          winsize;
            word          winasegment;
            word          winbsegment;
            void          *winfuncptr;
            word          bytesperscanline;

            word          xresolution;
            word          yresolution;
            char          xcharsize  ;
            char          ycharsize  ;
            char          numberofplanes;
            char          bitsperpixel  ;
            char          numberofbanks ;
            char          memorymodel   ;
            char          banksize      ;
            char          numberofimagepages;
            char          reserved          ;

            char          redmasksize       ;
            char          redfieldposition  ;
            char          greenmasksize     ;
            char          greenfieldposition;
            char          bluemasksize      ;
            char          bluefieldposition ;
            char          rsvdmasksize      ;
            char          rsvdmasposition   ;
            char          directcolormodeinfo;
            int           linearframebuffer;
            char          *offscreenmemoryaddress;
            word          offscreenmemory;
            char          dummy[206];
    };


struct  tgraphmodeparameters {
            int           resolutionx      ;
            int           resolutiony      ;
            int           actsetpage       ;
            int           windowstatus     ;
            int           granularity      ;
            int           scanlinelength   ;
            int           scanlinenumber   ;
            int           bytesperscanline ;
            int           activegraphmode  ;
            int           videomemory      ;
            int           byteperpix       ;
            int           linearaddress    ;
            int           pagetoset        ;
            char          redmasksize       ;
            char          redfieldposition  ;
            char          greenmasksize     ;
            char          greenfieldposition;
            char          bluemasksize      ;
            char          bluefieldposition ;
            char          bitperpix         ;
            char          memorymodel;
    };


struct trleheader {
   word id;
   word size;
   byte rle;
   word x;
   word y;
};


struct tavailablemodes {
          int   num;
          struct {  
                     int x,y;
                     int col;
                     int num;
                 } mode[256];
 
       };
typedef tavailablemodes* pavailablemodes;


#define    black        0
#define    blue         1
#define    green        2
#define    cyan         3
#define    red          4
#define    magenta      5 
#define    brown        6  
#define    lightgray    7   
#define    darkgray     8    
#define    lightblue    9     
#define    lightgreen  42   // 10      
#define    lightcyan   119  // 11      
#define    lightred    52   // 12        
#define    lightmagenta 5   // 13         
#define    yellow      103  // 14          
#define    white       160  // 15           


extern tgraphmodeparameters   *agmp;
extern tgraphmodeparameters   *hgmp;
extern int                    vesaerrorrecovery;

extern boolean initsvga(int modenum);
extern   void  closesvga ( void );

extern boolean getvesamodeavail( int modenum );


extern tvesamodeinfo activevesamodeinfo;
extern tgraphmodeparameters lastmodecheckgeneralparams;
extern tvesamodeinfo   lastmodecheckvesaparams;
extern tvesainfoblock  vesainfo;
extern "C" char      activepalette;
extern "C" int       palette16[256][4];
extern "C" void*     xlatbuffer;


extern dacpalette256* activepalette256;

extern "C" word activegraphmodeparameters;
extern "C" word hardwaregraphmodeparameters;
extern "C" int dpmscapabilities;
extern "C" int actdpmsmode;

extern pavailablemodes searchformode( int x, int y, int col );

extern "C" void setdisplaystart( int x, int y);
#pragma aux setdisplaystart parm [ ecx ][ edx ]  modify [eax ebx];

extern "C" int setscanlinelength( int length );
#pragma aux setscanlinelength parm [ ecx ] modify [ ebx edx ];

extern "C" int getscanlinelength();
#pragma aux getscanlinelength modify [ ebx ecx edx ];

extern "C" void bar(int x1, int y1, int x2, int y2, char color);
#pragma aux bar parm [ eax ] [ ebx ] [ ecx ] [ edx ]  modify [edi];

extern "C" void rotabar(int x1, int y1, int x2, int y2, char color);
#pragma aux rotabar parm [ eax ] [ ebx ] [ ecx ] [ edx ]  modify [edi];

extern "C" void rotabar2(int x1, int y1, int x2, int y2, char color, char min, char max);
#pragma aux rotabar2 parm [ eax ] [ ebx ] [ ecx ] [ edx ]  modify [edi];


extern "C" void settextmode(char mode);

extern "C" void getimage(int x1, int y1, int x2, int y2, void *buffer);
#pragma aux getimage parm [ eax ] [ ebx ] [ ecx ] [ edx ] [ edi ] modify [ esi ];

extern "C" void putimage(int x1, int y1, void *buffer);
#pragma aux putimage parm [ ebx ] [ ecx ] [ eax ] modify [edx edi esi];

extern "C" void putfilter(int x1, int y1, void *buffer);
#pragma aux putfilter parm [ ebx ] [ ecx ] [ eax ] modify [edx edi esi];

extern "C" void putxlatfilter ( int x1, int y1, void* pictbuffer, void* xlattables );
#pragma aux putxlatfilter parm [ ebx ] [ ecx ] [ esi ] [ edx ] modify [ eax edi ];

extern "C" void putspriteimage(int x1, int y1, void *buffer);
#pragma aux putspriteimage parm [ ebx ] [ ecx ] [ eax ] modify [edx edi esi];

extern "C" void putrotspriteimage(int x1, int y1, void *buffer, int rotationvalue);
#pragma aux putrotspriteimage parm [ ebx ] [ ecx ] [ eax ] [ edx ] modify [edi esi];

extern "C" void putrotspriteimage90(int x1, int y1, void *buffer, int rotationvalue);
#pragma aux putrotspriteimage90 parm [ ebx ] [ ecx ] [ eax ] [ edx ] modify [edi esi];

extern "C" void putrotspriteimage180(int x1, int y1, void *buffer, int rotationvalue);
#pragma aux putrotspriteimage180 parm [ ebx ] [ ecx ] [ eax ] [ edx ] modify [edi esi];

extern "C" void putrotspriteimage270(int x1, int y1, void *buffer, int rotationvalue);
#pragma aux putrotspriteimage270 parm [ ebx ] [ ecx ] [ eax ] [ edx ] modify [edi esi];

extern "C" void puttexture ( int x1, int y1, int x2, int y2, void *texture );
#pragma aux puttexture parm [ eax ] [ ebx ] [ ecx ] [ edx ] [ esi ] modify [ edi ];

extern "C" void putspritetexture ( int x1, int y1, int x2, int y2, void *texture );
#pragma aux putspritetexture parm [ eax ] [ ebx ] [ ecx ] [ edx ] [ esi ] modify [ edi ];

extern "C" void putimageprt ( int x1, int y1, int x2, int y2, void *texture, int dx, int dy );
#pragma aux putimageprt parm [ eax ] [ ebx ] [ ecx ] [ edx ] [ esi ] modify [ edi ];


extern void putpixel(int x1, int y1, int color);

extern int getpixel(int x1, int y1);

extern "C" void copybuf2displaymemory(int size, void *buf);
#pragma aux copybuf2displaymemory parm [ ebx ] [ eax ] modify [ eax edx ]

extern void setvgapalette256 ( dacpalette256 pal );
extern "C" void set_vgapalette256 ( dacpalette256 pal );
#pragma aux set_vgapalette256 parm [ eax ] modify [ ebx ecx edx esi ]

extern "C" void waitretrace ( void );
#pragma aux waitretrace modify [ al dx ]

extern "C" void getdpmscapabilities ( void );
#pragma aux getdpmscapabilities modify [ eax ebx ecx edx edi es ]

extern "C" void controldpms ( char mode );
#pragma aux controldpms parm [ bh ] modify [ eax ebx ecx edx ]

extern "C" void setvirtualpagepos ( int page );
#pragma aux setvirtualpagepos parm [ eax ]

extern "C" void* xlatpict ( ppixelxlattable xl, void* pntr );
#pragma aux xlatpict parm [ ebx ] [ esi ] modify [ eax ecx edx edi ]

extern "C" int loga2 ( int a );
#pragma aux loga2 parm [ ebx ] 

extern int imagesize(int x1, int y1, int x2, int y2);

extern void getpicsize(void* p, int& width, int& height);
extern int  getpicsize2(void* p );

extern "C" void generatemixbuf ( void* buf );
#pragma aux generatemixbuf parm [ esi ] modify [ eax ebx ecx edx edi ];

// extern "C" int mix3colors ( int a, int b, int c );
// #pragma aux mix3colors parm [ esi ][ edi ][ ebx ] modify [ eax ecx edx ];


extern void* uncompress_rlepict ( void* pict );


extern void         generategrayxlattable(ppixelxlattable tab,
                                   byte         offset,
                                   byte         size);

extern void         rahmen(boolean      invers,
                    integer      x1,
                    integer      y1,
                    integer      x2,
                    integer      y2);
extern void         line(int         x1,
                  int         y1,
                  int         x2,
                  int         y2,
                  char         actcol);
extern void xorline(int x1,
     int y1,
     int x2,
     int y2,
     char actcol);

extern void         rectangle(int         x1,
                        int         y1,
                        int         x2,
                        int         y2,
                        byte         color);
extern void         xorrectangle(int         x1,
                          int         y1,
                          int         x2,
                          int         y2,
                          byte         color);

// extern void         xorrectangle( tmouserect r, byte         color);

void putshadow ( int x1, int y1, void* ptr, ppixelxlattable xl  );
void putinterlacedrotimage ( int x1, int y1, void* ptr, int rotation );
void rotatepict90 ( void* s, void* d );
void flippict ( void* s, void* d, int dir = 1 );
void* halfpict ( void* vbuf );
char* rotatepict ( void* image, int organgle );

class tdrawline {
         public: 
           void start ( int x1, int y1, int x2, int y2 );
           virtual void putpix ( int x, int y ) = 0;
       };


class tvirtualdisplay {
           void* buf;
           tgraphmodeparameters oldparams;
           void init ( int x, int y, int col );
        public: 
           tvirtualdisplay ( int x, int y );
           tvirtualdisplay ( int x, int y, int color );
           ~tvirtualdisplay ();
        };

struct trgbpixel {
       public:
         union {
            int rgb;
            struct { char r,g,b,a;  }channel;
          };
       //    mix ( const trgbpixel* pix );
        };

extern int dont_use_linear_framebuffer;

#define alphabase 64

class TrueColorImage {
         int xsize;
         int ysize;
         trgbpixel* pix;
      public:
         TrueColorImage ( int x, int y );
         trgbpixel getpix ( int x, int y );
         trgbpixel* getpix ( void );
         void setpix ( int x, int y, int r, int g, int b, int alpha = alphabase );
         int getxsize( void );
         int getysize( void );

         ~TrueColorImage();

   };

extern TrueColorImage* zoomimage ( void* buf, int xsize, int ysize, dacpalette256 pal, int interpolate = 1, int proportional = 1 );
extern char* convertimage ( TrueColorImage* img, dacpalette256 pal );
extern TrueColorImage* convertimage2tc ( void* buf, dacpalette256 pal );

extern "C" int gettimestampcounter( void ); 
#pragma aux gettimestampcounter modify [edx]

extern "C" void float2int ( float* fp, int* ip );
#pragma aux float2int parm [ edi ] [ esi ]

extern char truecolor2pal_table[262144];
extern int getpixelfromimage ( void* buf, int x, int y );

extern void ellipse ( int x1, int y1, int x2, int y2, int color, float tolerance );

class fatalgraphicserror { 
        char st[1000];
      public:
        fatalgraphicserror ( char* strng );
        fatalgraphicserror ( void );
      };

extern void putmask ( int x1, int y1, void* vbuf, int newtransparence );
extern void putpicturemix ( int x1, int y1, void* ptr, int rotation, char* mixbuf );

extern int graphicinitialized;

#endif
