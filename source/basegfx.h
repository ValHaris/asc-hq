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

#ifndef basegfx_h
#define basegfx_h

#include "tpascal.inc"
#include "global.h"
#include "palette.h"
#ifdef _DOS_
 #include "vesa.h"
#else
 #include "sdl/graphics.h"
#endif

#pragma pack(1)

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
            int           directscreenaccess;
            int           reserved[9];
    };


struct trleheader {
   word id;
   word size;
   byte rle;
   word x;
   word y;
};

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

extern dacpalette256* activepalette256;


#ifdef _NOASM_
extern tgraphmodeparameters activegraphmodeparameters;
extern tgraphmodeparameters hardwaregraphmodeparameters;

extern dacpalette256 activepalette;
extern int       palette16[256][4];
extern void*     xlatbuffer;

#else

extern "C" word activegraphmodeparameters;
extern "C" word hardwaregraphmodeparameters;

extern "C" char      activepalette;
extern "C" int       palette16[256][4];
extern "C" void*     xlatbuffer;

#endif



#ifdef _NOASM_



 extern void bar(int x1, int y1, int x2, int y2, char color);
 extern void getimage(int x1, int y1, int x2, int y2, void *buffer);
 extern void putimage(int x1, int y1, void *buffer);
 extern void putxlatfilter ( int x1, int y1, void* pictbuffer, char* xlattables );
 extern void putspriteimage(int x1, int y1, void *buffer);
 extern void putrotspriteimage(int x1, int y1, void *buffer, int rotationvalue);
 extern void putrotspriteimage90(int x1, int y1, void *buffer, int rotationvalue);
 extern void putrotspriteimage180(int x1, int y1, void *buffer, int rotationvalue);
 extern void putrotspriteimage270(int x1, int y1, void *buffer, int rotationvalue);
 extern void puttexture ( int x1, int y1, int x2, int y2, void *texture );
 extern void putspritetexture ( int x1, int y1, int x2, int y2, void *texture );
 extern void putimageprt ( int x1, int y1, int x2, int y2, void *texture, int dx, int dy );
 extern void copybuf2displaymemory(int size, void *buf);
 
 extern void* xlatpict ( ppixelxlattable xl, void* pntr );
 extern int loga2 ( int a );
 
 extern void putpixel8(int x1, int y1, int color);
 extern int getpixel8(int x1, int y1);


 extern void float2int ( float* fp, int* ip );
 // Watcom is awfully slow on this conversion, that why I wrote it myself in ASM !


/*
 extern void generatemixbuf ( void* buf );
 // used only by external tools
*/


#else

 extern "C" void bar(int x1, int y1, int x2, int y2, char color);
 #pragma aux bar parm [ eax ] [ ebx ] [ ecx ] [ edx ]  modify [edi];
 // !!
 
 /*
 extern "C" void rotabar(int x1, int y1, int x2, int y2, char color);
 #pragma aux rotabar parm [ eax ] [ ebx ] [ ecx ] [ edx ]  modify [edi];
 // unused
 
 extern "C" void rotabar2(int x1, int y1, int x2, int y2, char color, char min, char max);
 #pragma aux rotabar2 parm [ eax ] [ ebx ] [ ecx ] [ edx ]  modify [edi];
 // unused
 
 */
 
 
 extern "C" void getimage(int x1, int y1, int x2, int y2, void *buffer);
 #pragma aux getimage parm [ eax ] [ ebx ] [ ecx ] [ edx ] [ edi ] modify [ esi ];
 // !!
 
 
 extern "C" void putimage(int x1, int y1, void *buffer);
 #pragma aux putimage parm [ ebx ] [ ecx ] [ eax ] modify [edx edi esi];
 // !
 
 /*
 extern "C" void putfilter(int x1, int y1, void *buffer);
 #pragma aux putfilter parm [ ebx ] [ ecx ] [ eax ] modify [edx edi esi];
 // unused
 */
 
 extern "C" void putxlatfilter ( int x1, int y1, void* pictbuffer, char* xlattables );
 #pragma aux putxlatfilter parm [ ebx ] [ ecx ] [ esi ] [ edx ] modify [ eax edi ];
 // !
 
 extern "C" void putspriteimage(int x1, int y1, void *buffer);
 #pragma aux putspriteimage parm [ ebx ] [ ecx ] [ eax ] modify [edx edi esi];
 // !
 
 extern "C" void putrotspriteimage(int x1, int y1, void *buffer, int rotationvalue);
 #pragma aux putrotspriteimage parm [ ebx ] [ ecx ] [ eax ] [ edx ] modify [edi esi];
 // !
 
 
 extern "C" void putrotspriteimage90(int x1, int y1, void *buffer, int rotationvalue);
 #pragma aux putrotspriteimage90 parm [ ebx ] [ ecx ] [ eax ] [ edx ] modify [edi esi];
 // !
 
 extern "C" void putrotspriteimage180(int x1, int y1, void *buffer, int rotationvalue);
 #pragma aux putrotspriteimage180 parm [ ebx ] [ ecx ] [ eax ] [ edx ] modify [edi esi];
 // !
 
 extern "C" void putrotspriteimage270(int x1, int y1, void *buffer, int rotationvalue);
 #pragma aux putrotspriteimage270 parm [ ebx ] [ ecx ] [ eax ] [ edx ] modify [edi esi];
 // !
 
 extern "C" void puttexture ( int x1, int y1, int x2, int y2, void *texture );
 #pragma aux puttexture parm [ eax ] [ ebx ] [ ecx ] [ edx ] [ esi ] modify [ edi ];
 // !
 
 extern "C" void putspritetexture ( int x1, int y1, int x2, int y2, void *texture );
 #pragma aux putspritetexture parm [ eax ] [ ebx ] [ ecx ] [ edx ] [ esi ] modify [ edi ];
 // !
 
 extern "C" void putimageprt ( int x1, int y1, int x2, int y2, void *texture, int dx, int dy );
 #pragma aux putimageprt parm [ eax ] [ ebx ] [ ecx ] [ edx ] [ esi ] modify [ edi ];
 // !
 
 extern "C" void copybuf2displaymemory(int size, void *buf);
 #pragma aux copybuf2displaymemory parm [ ebx ] [ eax ] modify [ eax edx ]
 // !
 
 

 extern "C" void* xlatpict ( ppixelxlattable xl, void* pntr );
 #pragma aux xlatpict parm [ ebx ] [ esi ] modify [ eax ecx edx edi ]
 // !!
 
 extern "C" int loga2 ( int a );
 #pragma aux loga2 parm [ ebx ] 
 // !!
 
 extern "C" void generatemixbuf ( void* buf );
 #pragma aux generatemixbuf parm [ esi ] modify [ eax ebx ecx edx edi ];
 // used only by external tools

 extern "C" void float2int ( float* fp, int* ip );
 #pragma aux float2int parm [ edi ] [ esi ]
 // Watcom is awfully slow on this conversion, that why I wrote it myself
 // !!

 extern "C" void putpixel8(int x1, int y1, int color);
 #pragma aux putpixel8 parm [ ebx ] [ eax ] [ edx ] modify [ ecx ]
 
 extern "C" int getpixel8(int x1, int y1);
 #pragma aux getpixel8 parm [ ebx ] [ eax ] modify [ ecx edx ]


#endif

 extern void setvgapalette256 ( dacpalette256 pal );

 extern void putpixel(int x1, int y1, int color);
 
 extern int getpixel(int x1, int y1);
 extern int imagesize(int x1, int y1, int x2, int y2);
 
 extern void getpicsize(void* p, int& width, int& height);
 extern int  getpicsize2(void* p );


extern void* uncompress_rlepict ( void* pict );


extern void         generategrayxlattable(ppixelxlattable tab,
                                   byte         offset,
                                   byte         size);

extern void         rahmen(char      invers,
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

extern char truecolor2pal_table[262144];
extern int getpixelfromimage ( void* buf, int x, int y );

extern void ellipse ( int x1, int y1, int x2, int y2, int color, float tolerance );

extern void putmask ( int x1, int y1, void* vbuf, int newtransparence );
extern void putpicturemix ( int x1, int y1, void* ptr, int rotation, char* mixbuf );


class fatalgraphicserror { 
        char st[1000];
      public:
        fatalgraphicserror ( char* strng );
        fatalgraphicserror ( void );
      };

class collategraphicoperations {
         int olddirectscreenaccess;
         int x1, y1, x2, y2;
         int status;
       public:
         collategraphicoperations ( void );
         collategraphicoperations ( int _x1, int _y1, int _x2, int _y2 );
         ~collategraphicoperations ();
         void on ( void );
         void off ( void );
};

extern void copySurface2screen( void );
extern void copySurface2screen( int x1, int y1, int x2, int y2 );


#pragma pack()

extern int fullscreen;

#endif
