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

/*! \file basegfx.h
   \brief basegfx.h is the interface for the graphic routines (of which some are platform dependent).

   basegfx.h additionally includes the definitions of routines, 
    whose implementation is platform dependant, but whose interface
    is the same on all platforms. Their implementation is in the various
    subdirectories for the platforms.
    The graphic routines are older than ASC itself, first written in 1993 as a protected
    mode replacement for Borlands BGI. (That's why the uncompressed image structure
    is exactly the same as in the BGI).
    This is the most badly designed and ugly part of ASC, which hould be completely rewritten,
    to make use of high- and truecolor, hardware acceleration and to provide a nice, object
    oriented C++ interface.

    */


#include "tpascal.inc"
#include "global.h"
#include "palette.h"
#ifdef _DOS_
 #include "dos/vesa.h"
#else
 #include "sdl/graphics.h"
#endif

#pragma pack(1)

//! a graphical surface. 
struct  tgraphmodeparameters {
            int           resolutionx      ;       //!< the number of visible pixels in x direction
            int           resolutiony      ;       //!< the number of visible pixels in y direction
            int           actsetpage       ;       //!< only used in DOS where it may be necessary to access the graphic memory in 64 kB pages
            int           windowstatus     ;      //!< determines whether the memory is a linear (windowstatus==100) or paged. When using SDL, the memory is always linear addressable.
            int           granularity      ;        //!< the offset in graphics memory between two pages. Can be ignored nowadays
            int           scanlinelength   ;       //!< the size of a line in pixel (may be larger than resolutionx due to offscreen memory)
            int           scanlinenumber   ;      //!< the number of lines (may be larger than resolutiony due to offscreen memory)
            int           bytesperscanline ;       //!< the size of a line in byte
            int           activegraphmode  ;      //!< the number of the active graphic mode. Can be ignored nowadays
            int           videomemory      ;      //!< the amount of memory in the video card. Not used any more.
            int           byteperpix       ;        //!< the distance between two pixel
            int           linearaddress    ;        //!< the pointer to the actual memory (typecast to char* )
            int           pagetoset        ;        //!< only used in DOS with paged graphic memory
            char          redmasksize       ;      //!< RGB only: the number of bits in the red component
            char          redfieldposition  ;       //!< RGB only: the position of the first red bit relative to the start of the pixel
            char          greenmasksize     ;      //!< RGB only: the number of bits in the green component
            char          greenfieldposition;       //!< RGB only: the position of the first green bit relative to the start of the pixel
            char          bluemasksize      ;      //!< RGB only: the number of bits in the blue component
            char          bluefieldposition ;      //!< RGB only: the position of the first blue bit relative to the start of the pixel
            char          bitperpix         ;        //!< the size of a pixel(?) in bits
            char          memorymodel;            //!< unused
            int           directscreenaccess;      //!< if 0 no call to an update function (which copies the buffer to the screen) is performed
            int           reserved[9];
    };


struct trleheader {
   word id;
   word size;
   pascal_byte rle;
   word x;
   word y;
};

const int black = 0;
const int    blue        = 1;
const int    green       = 2;
const int    cyan        = 3;
const int    red         = 4;
const int    magenta     = 5 ;
const int    brown       = 6  ;
const int    lightgray   = 7   ;
const int    darkgray    = 8    ;
const int    lightblue   = 9     ;
const int    lightgreen  = 42  ; // 10      
const int    lightcyan   = 119 ; // 11      
const int    lightred    = 52  ; // 12        
const int    lightmagenta = 5  ; // 13         
const int    yellow       = 103;  // 14          
const int    white        = 160;  // 15           

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


 //! paints a colored bar
 extern void bar(int x1, int y1, int x2, int y2, char color);

 //! copy an image to a buffer. buffer must have been alloced before with malloc ( imagesize ( x1,y1,x2,y2 ))
 extern void getimage(int x1, int y1, int x2, int y2, void *buffer);

 //! puts the image pointed to by buffer to the screen
 extern void putimage(int x1, int y1, void *buffer);

 /** puts the image in pictbuffer on the screen performing a color translation. 
      xlattables points to an array of n*256 byte. n must be equal or greater than the highest 
      pixel in pictbuffer.
      Example: you want to paint the tracks on the screen which are left by heavy vehicles.
        There is an image of a track which contains just pixels of 0, 1 and two. You now provide
        a set of 3 tables each 256 bytes large. The first one just counts from 0 to 255
        The second one has color values that are a bit darker than the original: If color #2 was
        blue, table[2] would have the index of a dark blue color.
        The third table is like the second one, but points to even darker colors.
        If you now paint the track using this function:
           where there is color #0 in the track image, the pixel on the screen would be unchanged
           where there is color #1 in the track image, the pixel on the screen is made darker
           and where there is color #2, the pixel on the screen is becoming even more darker
      Ok, that's all. A very specialized function. I just checked and noticed that it is not used
      any more by ASC. Damn, why did I write this description instead of just deleting this
      procedure ;-)
 */
 extern void putxlatfilter ( int x1, int y1, void* pictbuffer, char* xlattables );

 //! puts the image pointed to by buffer on the screen. All pixels with color #255 are treated as transparent
 extern void putspriteimage(int x1, int y1, void *buffer);

 /** like #putspriteimage, but all color with an 16 <= colorIndex <= 23 are increased by 
      rotationvalue. This is used to display colored units. The mentioned color range are the
      red colors, which can be made blue by adding 8 to them, 16 makes them brown, etc.. */
 extern void putrotspriteimage(int x1, int y1, void *buffer, int rotationvalue);

 //! like #putspriteimage, but rotates the image by 90ø clock-wise
 extern void putrotspriteimage90(int x1, int y1, void *buffer, int rotationvalue);

 //! like #putspriteimage, but rotates the image by 180ø clock-wise
 extern void putrotspriteimage180(int x1, int y1, void *buffer, int rotationvalue);

 //! like #putspriteimage, but rotates the image by 270ø clock-wise
 extern void putrotspriteimage270(int x1, int y1, void *buffer, int rotationvalue);

 //! function not used any more and obsolete. Use #putimageprt instead
 extern void puttexture ( int x1, int y1, int x2, int y2, void *texture );

 /** Puts a part of texture on the screen. Texture must be the same size as the screen,
      so the coordinates x1/y1 and x2/y2 describe the same rectangle on texture and the screen.
      This rectangle is copied from texture to the screen. color #255 is treated as transparent */
 extern void putspritetexture ( int x1, int y1, int x2, int y2, void *texture );

 /** copies a part of texture to the screen. Texture and the screen may have different size.
      x1/y1 and x2/y2 is the rectangle of the screen that texture is copied to. 
      dx/dy is the offset between x1/y1 and the upper left corner of texture.
      If dx==0 and dy==1, the uppermost line of texture would not be displayed.
      The rectangle x1/y1 - x2/y2 may be larger than texture.                   */
 extern void putimageprt ( int x1, int y1, int x2, int y2, void *texture, int dx, int dy );

 /** Performs a color translation of the image pntr. The image is not modified. The new image
      is written to a static buffer. The address of the buffer is returned. The call to xlatpict
      will overwrite the buffer. The conversion itself will replace color by xl[color]    */
 extern void* xlatpict ( ppixelxlattable xl, void* pntr );

 //! returns the position of the lowest bit of a which is set. This equals an inter logarithm to the base of 2
 extern int loga2 ( int a );
 

 /** Converts a float to an int. Since Watcom C/C++ is awfully slow on this conversion, 
      I rewrote it myself in assembler. Function is now obsolete and will be removed sooner
      or later          */
 extern void float2int ( float* fp, int* ip );



#else
 #include "dos/basegfx_asm_interface.h"
#endif

 //! sets the color palette in 8 bit mode
 extern void setvgapalette256 ( dacpalette256 pal );

 //! puts a single pixel on the screen. This is one of the few functions that work in truecolor mode too
 extern void putpixel(int x1, int y1, int color);
 
 //! gets the color of a single pixel from the screen. This is one of the few functions that work in truecolor mode too
 extern int getpixel(int x1, int y1);

 //! returns the size for a buffer that can contain an image of the given size.
 extern int imagesize(int x1, int y1, int x2, int y2);
 
 //! write the dimension of the image p to width and height
 extern void getpicsize(void* p, int& width, int& height);

 //! returns the size that the picture p occupies in memory
 extern int  getpicsize2(void* p );

 /** uncompresses the RLE-compressed image pict. A buffer for the uncompressed image
      is allocated and returned, so it must be freed later                          */
 extern void* uncompress_rlepict ( void* pict );

 /** A translation table ( to be used by #xlatpict ) is generated to translate any color of palette
      pal to its gray value. The new pixels after the translation are still to be used with the 
      palette pal. offset and size specify a range of colors that are assumed to be a linear
      transition from white to black. So all colors are mapped to one of the colors of the
      offset-size range. */
 extern void  generategrayxlattable( ppixelxlattable tab, char offset, char size, dacpalette256* pal );

 /** puts a shadow of an image on the screen. This is done by replacing all pixels on the screen
      by xl[pixel] if the pixel of the image ptr is non-transparent. The actual color of the image
      does not matter. */
 extern void putshadow ( int x1, int y1, void* ptr, ppixelxlattable xl  );



/** paints a pseudo-3D rectangle on the screen. invers specifies wheather the rectangle
     seems to be pressed into the screen or coming out  */
extern void         rahmen( bool invers, integer x1, integer y1, integer x2, integer y2);

//! draws a simple line on the screen. Not very fast...
extern void         line(int x1, int y1, int x2, int y2, char color );

/** draws a simple line on the screen, but performs a XOR operation between the pixel already 
     on screen and color. Thus the line will always have a color different then the one
     that was previously there. And it can be undone by displaying it a second time. */
extern void xorline( int x1, int y1, int x2, int y2, char color );

//! draws a simple rectangle
extern void rectangle(int x1, int y1, int x2, int y2, char color );

/** draws a simple rectangle on the screen, but performs a XOR operation between the pixel already
     on screen and color. Thus the rectangle will always have a color different then the one
     that was previously there. And it can be undone by displaying it a second time. */
extern void  xorrectangle(int x1, int y1, int x2, int y2, char color) ;

//! obsolete. not used any more. can be removed.
void putinterlacedrotimage ( int x1, int y1, void* ptr, int rotation );

/** rotates the image s by 90ø clockwise and writes it to d. d must point to a buffer
     with the size of imagesize2(s)                    */
void rotatepict90 ( void* s, void* d );

/** mirrors a picture horizontally (dir=1) or vertically (dir=0). d must point to a buffer
     with the size of imagesize2(s)                                  */
void flippict ( void* s, void* d, int dir = 1 );

/** reduces a pictures size. The new picture will have half the size in x and y direction, 
     resulting in a quarting of the area. The new image will be written to a static buffer that
     is overwritten the next time halfpict or #xlatpict is called. The address of the buffer is
     returned.                                                                 */
void* halfpict ( void* vbuf );


/** rotates the picture image by angle clockwise. The new image will be written to a static 
     buffer that is overwritten the next time halfpict or #xlatpict is called. The address of the 
     buffer is returned.                                                           */
char* rotatepict ( void* image, int angle );

//! returns the pixel at position x/y of the image buf or -1 if the pixel does not exist
extern int getpixelfromimage ( void* buf, int x, int y );

/** paints an ellipse on the screen that fills the rectangle x1/y1 - x2/y2.
     The thickness of the ellipse depends on tolerance. This function is just for highlighting
     elements on the screen, not for drawing nice and mathematically correct ellipse    */
extern void ellipse ( int x1, int y1, int x2, int y2, int color, float tolerance );

//! puts an image on the screen , with all pixels of newtransparence treated as transparent.
extern void putmask ( int x1, int y1, void* vbuf, int newtransparence );

/** puts the image ptr on the screen, mixing it with the colors there. Mixbuf must point to 
     256*256 byte large structure, where mixbuf[a][b] contains the colors that results from
     mixing a and b.                  */
extern void putpicturemix ( int x1, int y1, void* ptr, int rotation, char* mixbuf );


/** when using SDL, all operations are not performed on the displaymemory itself (as is the 
     case with DOS), but on a buffer. This function copies the buffer to the screen, performing 
     a color conversion if necessary */
extern void copySurface2screen( void );
extern void copySurface2screen( int x1, int y1, int x2, int y2 );



/** An abstract class that draws a line. This can not only be used for drawing a straight line
     on the screen, but also on the gamemap for example */
class tdrawline {
         public: 
           void start ( int x1, int y1, int x2, int y2 );
           virtual void putpix ( int x, int y ) = 0;
       };

/** A virtual screen is allocated and the agmp pointer set to it. All graphic operations will
     operate on this virtual display until it is deleted              */
class tvirtualdisplay {
           void* buf;
           tgraphmodeparameters oldparams;
           void init ( int x, int y, int col );
        public: 
           tvirtualdisplay ( int x, int y );
           tvirtualdisplay ( int x, int y, int color );
           ~tvirtualdisplay ();
        };



/**************************************************************
The following routines were an early attempt at rewriting the graphic engine. 
It was never continued and the routines are hardly used at all.
They can be completely scrapped should the graphic engine be rewritten 
sometime...
***************************************************************/

//! A class for a single pixel. 
struct trgbpixel {
       public:
         union {
            int rgb;
            struct { char r,g,b,a;  }channel;
          };
       //    mix ( const trgbpixel* pix );
        };

#define alphabase 64

//! A class for a RGB image. 
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

//! changes an images size. The source image (in buf) is 8-bit with palette pal .
extern TrueColorImage* zoomimage ( void* buf, int xsize, int ysize, dacpalette256 pal, int interpolate = 1, int proportional = 1 );

/** the truecolor image img is reduced to 8 bit color using the palette pal. A new buffer is 
     allocated and returned. It must be freed after use.          */
extern char* convertimage ( TrueColorImage* img, dacpalette256 pal );

//! converts a 8-bit image to a truecolor image
extern TrueColorImage* convertimage2tc ( void* buf, dacpalette256 pal );

/** a table to speed up conversion from truecolor to 8bit palette. The 6 most significant bits
     of each color component (RGB) form the the index.       */
extern char truecolor2pal_table[262144];


//! a class that is thrown as exception. Should be moved to error.h ...
class fatalgraphicserror { 
        char st[1000];
      public:
        fatalgraphicserror ( char* strng );
        fatalgraphicserror ( void );
      };

/** \brief Collects all graphic operations and updates the screen on destruction
     When porting ASC from DOS to SDL, the problem arose that under DOS all graphic operations
     directly modified the graphics memory and were directly visible without any overhead. Using
     SDL, the operations operated on a buffer that had to be copied to display memory in a 
     seperate operation, which was costly. Updating the screen after every basic graphic operation
     was too expensive, so I wrote this class that suspended all updates until it was destructed. */
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



#pragma pack()

//! this variable determines whether the next call to initgraphics will open a window or a fullscreen session
extern int fullscreen;

//! sets the caption of the main window
extern void setWindowCaption ( const char* s );


#endif
