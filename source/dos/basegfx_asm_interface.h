// this file is included by basegfx.h

 /*
 The interface for the same functions as above, but written in assembler instead.
 The pragma are for Watcom C/C++. Since we don't use Watcom C/C++ any more,
 this code is derelict.
 But since the asm code is working and quite a bit faster than the C code, I think we should
 keep all this stuff, at least until the graphics engine is rewritten to use high/truecolor and
 gets a nice object oriented structure
 */

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
 // !
 
 
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

