//     $Id: vesa.cpp,v 1.3 1999-11-16 17:04:19 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  1999/11/16 03:42:47  tmwilson
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


#include "tpascal.inc"
#include "vesa.h"
#include "misc.h"

#ifndef minimal

#include "loadpcx.h"
#include "newfont.h"
#else
/*
void* asc_malloc ( size_t t )
{
   return malloc ( t );
}
void asc_free ( void* p)
{
   free (p);
}
*/
#endif

// *******************************************************************************************
//*******globabe variablen ********
//*******************************************************************************************


int                    vesaerrorrecovery = 0;
tvesamodeinfo activevesamodeinfo;

/*
		hgmp = (tgraphmodeparameters *) & hardwaregraphmodeparameters;
		agmp = (tgraphmodeparameters *) & activegraphmodeparameters;
  */

tgraphmodeparameters *agmp = (tgraphmodeparameters *) & activegraphmodeparameters;
tgraphmodeparameters *hgmp = (tgraphmodeparameters *) & hardwaregraphmodeparameters;
tgraphmodeparameters lastmodecheckgeneralparams;
tvesamodeinfo   lastmodecheckvesaparams;
tvesainfoblock  vesainfo;
dacpalette256  *activepalette256;
word           *modelist;

boolean         generalparamsread = false;

int dont_use_linear_framebuffer = 0;



static struct rminfo {
	long            edi;
	long            esi;
	long            ebp;
	long            reserved_by_system;
	long            ebx;
	long            edx;
	long            ecx;
	long            eax;
	short           flags;
	short           es, ds, fs, gs, ip, cs, sp, ss;
}               rmi;

#define realptr2protptr(p) (void*)((((int)p & 0xffff0000) >> 12) + ((int)p & 0x0000ffff))




boolean 
getgeneralparameters(void)
{
	///////////////////////////
	//variablendeklaration
	//////////////////////////

	REGS reg;   
	unsigned short int vesainfosegment;
	unsigned short int vesainfoselector;
	SREGS           segmentreg;
	char            info[5];
	word           *modeptr;
	int             numberofmodes;

	/*
	 * ////////////////////////// //code beginn ///////////////////////// 
	 */

	if (generalparamsread == false) {

		//reservierung des speichers f Å r interruptaufruf

			reg.w.ax = 0x0100;
		reg.w.bx = sizeof(tvesainfoblock) / 16;

#ifdef showstatus
		printf("allocating low memory\n");
#endif

		int386(0x31, &reg, &reg);

		if (reg.w.cflag) {
#ifdef showstatus
			printf("memory allocation for vesainfoblock failed");
			printf("error-code %d\n", reg.w.ax);
#endif

			return (false);
		} else {
#ifdef showstatus
			printf("memory allocated\n");
#endif
		}


		vesainfosegment = reg.w.ax;
		vesainfoselector = reg.w.dx;

		/* set up real-mode call structure */

#ifdef showstatus
		printf("fill rmi structure with 0\n");
#endif

		memset(&rmi, 0, sizeof(rmi));
		segread(&segmentreg);


#ifdef showstatus
		printf("rmi filled\n");
#endif

		strcpy((char *) (vesainfosegment * 0x10), "vbe2");

		rmi.eax = 0x00004f00;
		rmi.es = vesainfosegment;
		rmi.edi = 0;

		/* use dmpi call 300h to issue the dos interrupt */
		reg.w.ax = 0x0300;
		reg.h.bl = 0x10;
		reg.h.bh = 0;
		reg.w.cx = 0;
		segmentreg.es = FP_SEG( &rmi ); 

		reg.x.edi = FP_OFF( &rmi );

#ifdef showstatus
		printf("calling real mode interrupt\n");
#endif

		int386x(0x31, &reg, &reg, &segmentreg);


		if ((rmi.eax & 0xffff) != 0x004f) {
#ifdef showstatus
			printf("could not get vesainfoblock\n");
			printf("result: %x", rmi.eax);
#endif

			return (false);
		} else {
#ifdef showstatus
			printf("got vesainfoblock\n");
#endif
		}



		tvesainfoblock *vesadospuffern;
		vesadospuffern = (tvesainfoblock *) (vesainfosegment * 0x10);


		memcpy(&vesainfo, (void *) (vesainfosegment * 0x10), sizeof(vesainfo));
		vesainfo.hersteller = (char *) realptr2protptr(vesainfo.hersteller);
		vesainfo.modelist = (word *) realptr2protptr(vesainfo.modelist);

#ifdef showstatus
		printf("capabilities: %x\n", vesainfo.capabilities);
#endif


		info[0] = vesadospuffern->signature[0];
		info[1] = vesadospuffern->signature[1];
		info[2] = vesadospuffern->signature[2];
		info[3] = vesadospuffern->signature[3];
		info[4] = 0;


#ifdef showstatus
		printf("sizeof vesainfo: %d\n", sizeof(vesainfo));
		printf("identification: %s\n", info);

		printf("version: %d.%d\n", vesainfo.vesaversion >> 8, vesainfo.vesaversion & 0xff);

		printf("manufactorer: %s\n", vesainfo.hersteller);
#endif

		/*
		 * liste der verf Å gbaren graphikmodi abklappern; endet bei
		 * 0xffff 
		 */

		numberofmodes = 1;
		modeptr = vesainfo.modelist;
		while (*modeptr != 0xffff) {

#ifdef showstatus
			printf("%x ", *modeptr);
#endif

			numberofmodes++;
			modeptr++;
		}

		modelist = (word *) asc_malloc(numberofmodes * 2);
		memcpy(modelist, vesainfo.modelist, numberofmodes * 2);
		vesainfo.modelist = modelist;

#ifdef showstatus
		printf("\n");
#endif



		reg.w.ax = 0x0101;
		reg.w.dx = vesainfoselector;

		int386(0x31, &reg, &reg);

		if (reg.w.cflag) {
#ifdef showstatus
			printf("dos-memory deallocation failed\n");
			printf("error-code %d\n", reg.w.ax);
#endif
			return (false);
		}
		generalparamsread = true;
		return true;

	} else {
		return true;
	}			/* endif */

}




pavailablemodes 
searchformode(int x, int y, int col)
{
	pavailablemodes avm;
	word           *pw;


	avm = new tavailablemodes;
	avm->num = 0;

	if (getgeneralparameters()) {

		pw = vesainfo.modelist;
		while (*pw != 0xffff) {
			if (getvesamodeavail(*pw)) {
				if (((lastmodecheckgeneralparams.resolutionx == x) || (x == 0)) &&
				    ((lastmodecheckgeneralparams.resolutiony == y) || (y == 0)) &&
				    ((lastmodecheckgeneralparams.bitperpix == col) || (col == 0)) &&
                                    ((lastmodecheckgeneralparams.memorymodel == 6) || (lastmodecheckgeneralparams.memorymodel == 4)     )) {   /* direct color || packed pixel */
					avm->mode[avm->num].num = *pw;
					avm->mode[avm->num].x = lastmodecheckgeneralparams.resolutionx;
					avm->mode[avm->num].y = lastmodecheckgeneralparams.resolutiony;
					avm->mode[avm->num].col = lastmodecheckgeneralparams.bitperpix;
					avm->num++;

				}	/* endif */
			}	/* endif */
			pw++;
		}
	}
	return avm;
}



boolean
getvesamodeavail(int modenum)
{
	///////////////////////////
		//variablendeklaration
		//////////////////////////

		char            vesamodefound;
	REGS            reg;   
	unsigned short int vesainfosegment;
	unsigned short int vesainfoselector;
	SREGS           segmentreg;
	word           *modeptr;


	/*
	 * ////////////////////////// //code beginn /////////////////////////  
	 */

	memset(&lastmodecheckgeneralparams, 0, sizeof(lastmodecheckgeneralparams));

	//reservierung des speichers f Å r interruptaufruf

	reg.w.ax = 0x0100;
	reg.w.bx = sizeof(tvesainfoblock) / 16;

#ifdef showstatus
	printf("allocating low memory\n");
#endif
	int386(0x31, &reg, &reg);

	if (reg.w.cflag) {
#ifdef showstatus
		printf("memory allocation for vesainfoblock failed");
		printf("error-code %d\n", reg.w.ax);
#endif
		return (false);
	} else {
#ifdef showstatus
		printf("memory allocated\n");
#endif
	}


	vesainfosegment = reg.w.ax;
	vesainfoselector = reg.w.dx;
	segread(&segmentreg);


	if (getgeneralparameters() == false)
		return (false);

	/*
	 * liste der verf Å gbaren graphikmodi abklappern; endet bei 0xffff 
	 */
	vesamodefound = 0;
	modeptr = vesainfo.modelist;
	while (*modeptr != 0xffff) {
#ifdef showstatus
		printf("%x ", *modeptr);
#endif
		if (*modeptr == modenum)
			vesamodefound = 1;
		modeptr++;
	}

#ifdef showstatus
	printf("\n");
#endif

	if (vesamodefound != 1) {
#ifdef showstatus
		printf("mode not available\n");
#endif
		return (false);
	}
	/* vesa mode info anfordern  */

	/* set up real-mode call structure */
	memset(&rmi, 0, sizeof(rmi));
	strcpy((char *) (vesainfosegment * 0x10), "vesa");
	rmi.eax = 0x00004f01;
	rmi.es = vesainfosegment;
	rmi.edi = 0;
	rmi.ecx = modenum;


	/* use dmpi call 300h to issue the dos interrupt */
	reg.w.ax = 0x0300;
	reg.h.bl = 0x10;
	reg.h.bh = 0;
	reg.w.cx = 0;
	segmentreg.es = FP_SEG( &rmi );
	reg.x.edi = FP_OFF( &rmi );

	int386x(0x31, &reg, &reg, &segmentreg);


	if ((rmi.eax & 0xffff) != 0x004f) {
#ifdef showstatus
		printf("\ncould not get vesamodeinfoblock\n");
		printf("result: %x\n", rmi.eax);
#endif
		return (false);
	};

	memcpy(&lastmodecheckvesaparams, (void *) (vesainfosegment * 0x10), sizeof(lastmodecheckvesaparams));

#ifdef showstatus
	if (lastmodecheckvesaparams.modeattributes & 128) {
		printf("linear framebuffer supported at %x\n", lastmodecheckvesaparams.linearframebuffer);
	} else {
		printf("linear framebuffer not supported !\n");
	}			/* endif */
#endif

	if ( (lastmodecheckvesaparams.modeattributes & 128) && !dont_use_linear_framebuffer ) {
		lastmodecheckgeneralparams.windowstatus = 100;
	} else {
		lastmodecheckgeneralparams.windowstatus = 00;
		if ((lastmodecheckvesaparams.winaattributes & 7) != 7) {
			if ((((lastmodecheckvesaparams.winaattributes & 7) == 3) && ((lastmodecheckvesaparams.winbattributes & 7) == 5)) ||
			    (((lastmodecheckvesaparams.winaattributes & 7) == 5) && ((lastmodecheckvesaparams.winbattributes & 7) == 3))) {
				lastmodecheckgeneralparams.windowstatus = 1;
			} else {
#ifdef showstatus
				printf("unsupported settings of windowparameter !\n");
#endif
				return (false);
			}
		};
	}			/* endif */

	if (vesainfo.capabilities & 128) {

	}			/* endif */
	/* 
  freigeben des speichers f Å r interruptaufruf */


	reg.w.ax = 0x0101;
	reg.w.dx = vesainfoselector;

	int386(0x31, &reg, &reg);

	if (reg.w.cflag) {
#ifdef showstatus
		printf("dos-memory deallocation failed\n");
		printf("error-code %d\n", reg.w.ax);
#endif
		return (false);
	}
	if (lastmodecheckvesaparams.modeattributes & 1 == 0) {
#ifdef showstatus
		printf("mode not supportet !\n");
#endif
		return (false);
	}
	if (lastmodecheckvesaparams.winsize != 64) {
#ifdef showstatus
		printf("unsupported size of video-window size. 64 kb required\n");
#endif
		return (false);
	}
	lastmodecheckgeneralparams.granularity = (char) (6 - loga2(lastmodecheckvesaparams.wingranularity));
	lastmodecheckgeneralparams.resolutionx = lastmodecheckvesaparams.xresolution;
	lastmodecheckgeneralparams.resolutiony = lastmodecheckvesaparams.yresolution;
	lastmodecheckgeneralparams.actsetpage = 0;
	lastmodecheckgeneralparams.pagetoset  = 0;
	lastmodecheckgeneralparams.byteperpix = (char) (lastmodecheckvesaparams.bitsperpixel / 8);
	lastmodecheckgeneralparams.bitperpix = (char) (lastmodecheckvesaparams.bitsperpixel);
	lastmodecheckgeneralparams.videomemory = (word) (vesainfo.totalmemory * 64);
	lastmodecheckgeneralparams.redmasksize = lastmodecheckvesaparams.redmasksize;
	lastmodecheckgeneralparams.redfieldposition = lastmodecheckvesaparams.redfieldposition;
	lastmodecheckgeneralparams.greenmasksize = lastmodecheckvesaparams.greenmasksize;
	lastmodecheckgeneralparams.greenfieldposition = lastmodecheckvesaparams.greenfieldposition;
	lastmodecheckgeneralparams.bluemasksize = lastmodecheckvesaparams.bluemasksize;
	lastmodecheckgeneralparams.bluefieldposition = lastmodecheckvesaparams.bluefieldposition;
	lastmodecheckgeneralparams.activegraphmode = (word) modenum;
	lastmodecheckgeneralparams.scanlinelength = lastmodecheckvesaparams.bytesperscanline;
	lastmodecheckgeneralparams.bytesperscanline = lastmodecheckvesaparams.bytesperscanline;
	lastmodecheckgeneralparams.memorymodel = lastmodecheckvesaparams.memorymodel;

	if (vesamodefound == 1) {
		return (true);
	} else {
		return (false);
	}			/* endif */

}


int
imagesize(int x1, int y1, int x2, int y2)
{
	return ((x2 - x1 + 1) * (y2 - y1 + 1) + 4);
}


int xlatbuffersize = 66000;

boolean
initsvga(int modenum)
{
	if (getvesamodeavail(modenum & 0x3fff)) {
		REGS            regs;         

		regs.w.ax = 0x4f02;
		if (lastmodecheckgeneralparams.windowstatus == 100) {
			modenum |= 0x4000;
		}		/* endif */
		regs.w.bx = (word) modenum;

		int386(0x10, &regs, &regs);

		if (regs.w.ax != 0x4f) {
			printf("video initialisation failed !\n");
			return (false);
		}
		/*
		 * scanf("%d",i); vidptr = (char*)0x4000010; vidptr = 14;
		 * scanf("%d",i); 
		 *
		 * vidptr = (char*)0x4000010; for (j=0;j<10000 ;j++ ) {
		 * vidptr=14; vidptr++; } 
		 */
		hgmp = (tgraphmodeparameters *) & hardwaregraphmodeparameters;
		agmp = (tgraphmodeparameters *) & activegraphmodeparameters;
		*agmp = lastmodecheckgeneralparams;

                regs.w.ax = 0x4f06;
                regs.w.bx = 1;
                regs.w.cx = 0;
                int386(0x10, &regs, &regs);
                if ( regs.w.dx < agmp->resolutiony   ||   regs.w.bx < agmp->resolutionx  ) {
                   if ( vesaerrorrecovery == 0 ) {
                      settextmode ( 3 );
                      printf ( " The VESA-bios returned invalid parameters ! This is a bug in your graphic hard- or software !\n Please update your bios or driver or use a 3rd party vesa driver !\n Use command line switch /v1 to use default values !\n" );
                      getch();
                      exit(1);
                   } else
                      printf ( " The VESA-bios returned invalid parameters ! This is a bug in your graphic hard- or software !\n Please update your bios or driver or use a 3rd party vesa driver !\n using default values !\n" );
                   if ( agmp-> scanlinelength == 0 ) {
                      if ( agmp-> bytesperscanline == 0 ) 
                          agmp-> scanlinelength = agmp->resolutionx * agmp->byteperpix;
                      else
                          agmp-> scanlinelength =  agmp-> bytesperscanline;
                   }
                } else
                   agmp-> scanlinelength = regs.w.bx;



		agmp->actsetpage = 0;
		*hgmp = *agmp;

		if (lastmodecheckgeneralparams.windowstatus == 100) {
			regs.w.ax = 0x0800;	/* map physical to linear */
			regs.w.bx = (short) (lastmodecheckvesaparams.linearframebuffer >> 16);              
			regs.w.cx = (short) (lastmodecheckvesaparams.linearframebuffer & 0xffff);	/* bx:cx = address */
			regs.w.si = vesainfo.totalmemory;	/* si:di = size */
			regs.w.di = 0;
			int386(0x31, &regs, &regs);
			lastmodecheckvesaparams.linearframebuffer = ((regs.w.bx << 16) + regs.w.cx);
			agmp->linearaddress = lastmodecheckvesaparams.linearframebuffer;
                        hgmp->linearaddress = lastmodecheckvesaparams.linearframebuffer;
		} else {
                        agmp->linearaddress = 0xa0000;
                        hgmp->linearaddress = 0xa0000;
		}		/* endif */

                if ( !xlatbuffer )
                   xlatbuffer = asc_malloc ( xlatbuffersize );
                graphicinitialized = 1;
	} else {                                      
		printf("vesa-mode not available !\n");
	}
	return (false);

}


void  closesvga ( void ) {
   REGS regs;
   regs.w.ax = 0x0801;	/* map physical to linear */
   regs.w.bx = (short) (lastmodecheckvesaparams.linearframebuffer >> 16);
   regs.w.cx = (short) (lastmodecheckvesaparams.linearframebuffer & 0xffff);	/* bx:cx = address */
   int386(0x31, &regs, &regs);
/*
   asc_free ( xlatbuffer );
   xlatbuffer = NULL;
   */
   graphicinitialized = 0;
}


void getpicsize( void* hd2, int &width, int &height)
{
   trleheader* hd = (trleheader*) hd2;
   if (hd->id == 16973) {
      width = hd->x;
      height = hd->y;
   } else {
     width = hd->id + 1;
     height = hd->size + 1;
   }
}

int getpicsize2( void* hd2 )
{
   trleheader* hd = (trleheader*) hd2;
   int width; 
   int height;
   if (hd->id == 16973) 
      return hd->size + sizeof(*hd);
   else {
     width = hd->id + 1;
     height = hd->size + 1;
     return height*width+4;
   }
}


void generategrayxlattable( ppixelxlattable tab, byte offset, byte size)
{
	for ( int b = 0; b <= 255; b++) {
//		(*tab)[b] = (char) (offset + size - 1 - ((*activepalette256)[b][0] + (*activepalette256)[b][1] + (*activepalette256)[b][2]) * size / 192);
		(*tab)[b] = (char) (offset + size - 1 - ( 0.299 * (*activepalette256)[b][0] + 0.587 * (*activepalette256)[b][1] + 0.114 * (*activepalette256)[b][2]) * size / 64 );
	}
}


void 
rahmen(boolean invers,
       integer x1,
       integer y1,
       integer x2,
       integer y2)
{
	byte            col;

	if (invers == false)
		col = white;
	else
		col = darkgray;
	line(x1, y1, x1, y2, col);
	line(x1, y1, x2, y1, col);
	if (invers == true)
		col = white;
	else
		col = darkgray;
	line(x2, y1, x2, y2, col);
	line(x1, y2, x2, y2, col);
}


/*
void tdrawline :: start ( int x1, int y1, int x2, int y2 )
{
	float           m, b;
	int             w;
	float           yy1, yy2, xx1, xx2;



    if ( x1 == x2) {
        for (w=y1;w<=y2 ;w++ ) 
           putpixel(x1, w, actcol );

    } else {
       if ( y1 == y2) {
          for (w=x1;w<=x2 ;w++ ) 
               putpix( w, y1 );
       } else {
        	yy1 = y1;
        	yy2 = y2;
        	xx1 = x1;
        	xx2 = x2;
        	m = (yy2 - yy1) / (xx2 - xx1);
        	b = y1 - m * x1;
        	if ((m <= 1) && (m >= -1)) {
        		if (x2 < x1) {
        			w = x2;
        			x2 = x1;
        			x1 = w;
        			w = y2;
        			y2 = y1;
        			y1 = w;
        		}
        		for (w = x1; w <= x2; w++) 
        			putpix( w, (int) (m * w + b) );
        		
        	} else {
        		if (y2 < y1) {
        			w = x2;
        			x2 = x1;
        			x1 = w;
        			w = y2;
        			y2 = y1;
        			y1 = w;
        		}
        		for (w = y1; w <= y2; w++) {
        			putpix((int) ((w - b) / m), w );
        		}
        
        	}
         } 
     }

}

*/




// Bresenham aus PCGPE 

void tdrawline :: start ( int x1, int y1, int x2, int y2 )
{ 
   int      i, deltax, deltay, numpixels, d, dinc1, dinc2, x, xinc1, xinc2, y, yinc1, yinc2; 


    /*  calculate deltax and deltay for initialisation  */ 
    
  deltax = x2 - x1; 
  if ( deltax < 0 )
     deltax = -deltax;

  deltay = y2 - y1; 
  if ( deltay < 0 )
     deltay = -deltay;

    /*  initialize all vars based on which is the independent variable  */ 
  if (deltax >= deltay) 
    { 

        /*  x is independent variable  */ 
      numpixels = deltax + 1; 
      d = (2 * deltay) - deltax; 
      dinc1 = deltay << 1; 
      dinc2 = (deltay - deltax) << 1; 
      xinc1 = 1; 
      xinc2 = 1; 
      yinc1 = 0; 
      yinc2 = 1; 
    } 
  else 
    { 

        /*  y is independent variable  */ 
      numpixels = deltay + 1; 
      d = (2 * deltax) - deltay; 
      dinc1 = deltax << 1; 
      dinc2 = (deltax - deltay) << 1; 
      xinc1 = 0; 
      xinc2 = 1; 
      yinc1 = 1; 
      yinc2 = 1; 
    } 

    /*  make sure x and y move in the right directions  */ 
  if (x1 > x2) 
    { 
      xinc1 = -xinc1; 
      xinc2 = -xinc2; 
    } 
  if (y1 > y2) 
    { 
      yinc1 = -yinc1; 
      yinc2 = -yinc2; 
    } 

    /*  start drawing at <x1, y1>  */ 
  x = x1; 
  y = y1; 

    /*  draw the pixels  */ 
  for (i = 1; i <= numpixels; i++) 
    { 
      putpix( x, y ); 


      if (d < 0) 
        { 
          d = d + dinc1; 
          x = x + xinc1; 
          y = y + yinc1; 
        } 
      else 
        { 
          d = d + dinc2; 
          x = x + xinc2; 
          y = y + yinc2; 
        } 
    } 
} 


void 
line(int  x1,
     int  y1,
     int  x2,
     int  y2,
     char actcol)
{
	float           m, b;
	int             w;
	float           yy1, yy2, xx1, xx2;



    if ( x1 == x2) {
        for (w=y1;w<=y2 ;w++ ) 
           putpixel(x1, w, actcol );

    } else {
       if ( y1 == y2) {
          for (w=x1;w<=x2 ;w++ ) 
               putpixel(w, y1, actcol );
       } else {
        	yy1 = y1;
        	yy2 = y2;
        	xx1 = x1;
        	xx2 = x2;
        	m = (yy2 - yy1) / (xx2 - xx1);
        	b = y1 - m * x1;
        	if ((m <= 1) && (m >= -1)) {
        		if (x2 < x1) {
        			w = x2;
        			x2 = x1;
        			x1 = w;
        			w = y2;
        			y2 = y1;
        			y1 = w;
        		}
        		for (w = x1; w <= x2; w++) 
        			putpixel(w, (int) (m * w + b), actcol);
        		
        	} else {
        		if (y2 < y1) {
        			w = x2;
        			x2 = x1;
        			x1 = w;
        			w = y2;
        			y2 = y1;
        			y1 = w;
        		}
        		for (w = y1; w <= y2; w++) {
        			putpixel((int) ((w - b) / m), w, actcol);
        		}
        
        	}
         } /* endif */
     }
}

void 
xorline(int  x1,
     int  y1,
     int  x2,
     int  y2,
     char actcol)
{
	float           m, b;
	int             w;
	float           yy1, yy2, xx1, xx2;

    if ( x1 == x2) {
        for (w=y1;w<=y2 ;w++ ) {
		putpixel( x1, w, getpixel ( x1, w ) ^ actcol);
        } /* endfor */
    } else {
	yy1 = y1;
	yy2 = y2;
	xx1 = x1;
	xx2 = x2;
	m = (yy2 - yy1) / (xx2 - xx1);
	b = y1 - m * x1;
	if ((m <= 1) && (m >= -1)) {
		if (x2 < x1) {
			w = x2;
			x2 = x1;
			x1 = w;
			w = y2;
			y2 = y1;
			y1 = w;
		}
		for (w = x1; w <= x2; w++) {
			putpixel(w, (int) (m * w + b), getpixel( w, (int) (m * w + b) ) ^ actcol);
		}
	} else {
		if (y2 < y1) {
			w = x2;
			x2 = x1;
			x1 = w;
			w = y2;                  
			y2 = y1;
			y1 = w;
		}
		for (w = y1; w <= y2; w++) {
			putpixel((int) ((w - b) / m), w, getpixel ( (int) ((w - b) / m), w ) ^ actcol);             
		}

	}
     }
}



void 
rectangle(int x1,
	  int y1,
	  int x2,
	  int y2,
	  byte color)
{
	line(x1, y1, x1, y2, color);
	line(x1, y1, x2, y1, color);
	line(x2, y1, x2, y2, color);
	line(x1, y2, x2, y2, color);
}


void xorrectangle(int x1,
	     int y1,
	     int x2,
	     int y2,
	     byte color)
{
          xorline(x1,y1,x1,y2,color); 
          xorline(x1,y1,x2,y1,color);
          xorline(x2,y1,x2,y2,color); 
          xorline(x1,y2,x2,y2,color);
}

/*
void         xorrectangle( tmouserect r, byte         color)
{
   xorrectangle ( r.x1, r.y1, r.x2, r.y2, color );
}

  */


void* halfpict ( void* vbuf )
{
   char* buf = (char*) vbuf;

   word* wp = (word*) xlatbuffer;
   char* dest = (char*) xlatbuffer;

   trleheader*   hd = (trleheader*) vbuf; 

   if ( hd->id == 16973 ) { 
      wp[0] = hd->x / 2;
      wp[1] = hd->y / 2;

      if ( ( wp[0] + 1 ) * ( wp[1] + 1 ) + 4 >= xlatbuffersize )
         throw fatalgraphicserror ( "halfpict : picture larger than buffer ! " );

      dest += 4;

      buf += sizeof ( *hd );


      int linecount = 0;
      int rowcount = 0;

      for ( int c = 0; c < hd->size; c++ ) {
         if ( *buf == hd->rle ) {
            for ( int i = buf[1]; i > 0; i-- ) {
               if ( !(linecount & 1)  &&  !(rowcount & 1)) {
                  *dest = buf[2];
                  dest++;
               }
               rowcount++;
               if ( rowcount > hd->x ) {
                  rowcount = 0;
                  linecount++;
               }
            }

            buf += 3;
            c += 2;

         } else {
            if ( !(linecount & 1)  &&  !(rowcount & 1)) {
               *dest = *buf;
               dest++;
            }
            buf++;
            rowcount++;
            if ( rowcount > hd->x ) {
               rowcount = 0;
               linecount++;
            }
         }
         if ( (int)dest - (int)xlatbuffer > xlatbuffersize )
            throw fatalgraphicserror ( "halfpict : picture larger than buffer ! " );

      }
   } else {
      int linecount = 0;
      int rowcount = 0;

      word* wp2 = (word*) vbuf;

      wp[0] = wp2[0] / 2;
      wp[1] = wp2[1] / 2;

      if ( ( wp[0] + 1 ) * ( wp[1] + 1 ) + 4 >= xlatbuffersize )
         throw fatalgraphicserror ( "halfpict : picture larger than buffer ! " );

      dest += 4;
      buf += 4;

      for ( int c = (wp2[0] + 1) * (wp2[1] + 1); c > 0; c-- ) {
         if ( !(linecount & 1)  &&  !(rowcount & 1)) {
            *dest = *buf;
            dest++;
         }
         buf++;
         rowcount++;
         if ( rowcount > wp2[0] ) {
            rowcount = 0;
            linecount++;
         }
      }

   }
   return xlatbuffer;
} 


void putshadow ( int x1, int y1, void* ptr, ppixelxlattable xl )
{
   word* w = (word*) ptr;
   char* c = (char*) ptr + 4;
   int spacelength = agmp->scanlinelength - *w - 1;

   if ( agmp->windowstatus == 100 ) {
      char* buf = (char*) (agmp->scanlinelength * y1 + x1 + agmp->linearaddress);
      for ( int y = w[1] + 1; y > 0; y-- ) {
         for ( int x = w[0]+1; x > 0; x-- ) {
            if ( *c != 255 )
               *buf = (*xl)[*buf];
            buf++;
            c++;
         }
         buf+=spacelength;
      }
   } else {
   }
}

void putpicturemix ( int x1, int y1, void* ptr, int rotation, char* mixbuf )
{
   word* w = (word*) ptr;
   char* c = (char*) ptr + 4;
   int spacelength = agmp->scanlinelength - *w - 1;

   if ( agmp->windowstatus == 100 ) {
      char* buf = (char*) (agmp->scanlinelength * y1 + x1 + agmp->linearaddress);
      for ( int y = w[1] + 1; y > 0; y-- ) {
         for ( int x = w[0]+1; x > 0; x-- ) {
            if ( *c != 255 ) {
               int o = *buf << 8;
               if ( *c >= 16  && *c < 24 )
                  *buf = mixbuf[o + *c + rotation];
               else
                  *buf = mixbuf[o + *c];
            }
            buf++;
            c++;
         }
         buf+=spacelength;
      }
   }
}


void putinterlacedrotimage ( int x1, int y1, void* ptr, int rotation )
{
   word* w = (word*) ptr;
   char* c = (char*) ptr + 4;
   int spacelength = agmp->scanlinelength - *w - 1;

   if ( agmp->windowstatus == 100 ) {
      char* buf = (char*) (agmp->scanlinelength * y1 + x1 + agmp->linearaddress);
      for ( int y = w[1] + 1; y > 0; y-- ) {
         for ( int x = w[0]+1; x > 0; x-- ) {
            if ( *c != 255 )
               if ( ((int)(buf+y)) & 1 )
                  if ( *c >= 16  && *c < 24 )
                     *buf = *c + rotation;
                  else
                     *buf = *c;
            buf++;
            c++;
         }
         buf+=spacelength;
      }
   } else {
   }
}


void rotatepict90 ( void* s, void* d )
{
   word* sw = (word*) s;
   char* sc = (char*) s + 4;
   
   word* dw = (word*) d;
   char* dc = (char*) d + 4;

   dw[0] = sw[1];
   dw[1] = sw[0];

   int dl = dw[0]+1;
   int dh = dw[1]+1;

   int sl = sw[0]+1;
   int sh = sw[1]+1;

   for (int y = 0; y <= dw[1]; y++) 
     for (int x = 0; x <= dw[0]; x++) 
         dc[ y * dl + x] = sc[ ( sh - x - 1 ) * sl + y];
}

void flippict ( void* s, void* d, int dir )
{
   word* sw = (word*) s;
   char* sc = (char*) s + 4;
   
   word* dw = (word*) d;
   char* dc = (char*) d + 4;

   dw[1] = sw[1];
   dw[0] = sw[0];

   int sl = sw[0]+1;
   int sh = sw[1]+1;

   if ( dir == 1 ) {
      for (int y = 0; y <= dw[1]; y++) 
        for (int x = 0; x <= dw[0]; x++) 
            dc[ y * sl + x] = sc[ y * sl + ( sl - 1 - x ) ];
   } else {
      for (int y = 0; y <= dw[1]; y++) 
        for (int x = 0; x <= dw[0]; x++) 
            dc[ y * sl + x] = sc[ ( sh - 1 - y)  * sl + x ];
   }
}


char truecolor2pal_table[262144];




class tinitvesa {
        public:
          tinitvesa ( void );
       } initvesa;

tinitvesa::tinitvesa ( void )
{
   memset ( truecolor2pal_table, 255, sizeof ( truecolor2pal_table ));
   hgmp = (tgraphmodeparameters *) & hardwaregraphmodeparameters;
   agmp = (tgraphmodeparameters *) & activegraphmodeparameters;
   activepalette256 = (dacpalette256*) &activepalette;
   xlatbuffer = asc_malloc ( xlatbuffersize );
}




void tvirtualdisplay :: init ( int x, int y, int color )
{
   agmp = (tgraphmodeparameters *) & activegraphmodeparameters;
   oldparams = *agmp ;
   char* cbuf = (char*) asc_malloc ( x * y );
   if ( !cbuf )
      throw fatalgraphicserror ( "could not allocate memory !");

   buf = cbuf;
   for ( int i = 0; i < (x*y); i++ )
      cbuf[i] = color;

   agmp->resolutionx   = x   ;
   agmp->resolutiony   = y   ;
   agmp->windowstatus  = 100 ;
   agmp->scanlinelength  = x ;
   agmp->scanlinenumber  = y ;
   agmp->bytesperscanline  = x;
   agmp->byteperpix   = 1    ;
   agmp->linearaddress = (int) buf ;
   agmp->bitperpix          = 8;

}

tvirtualdisplay :: tvirtualdisplay ( int x, int y )
{
   init ( x, y, 0 );
}

tvirtualdisplay :: tvirtualdisplay ( int x, int y, int color )
{
   init ( x, y, color );
}


tvirtualdisplay :: ~tvirtualdisplay ( )
{
   asc_free (  buf ) ;
   *agmp = oldparams;
}



extern "C" void putpixel8(int x1, int y1, int color);
#pragma aux putpixel8 parm [ ebx ] [ eax ] [ edx ] modify [ ecx ]

extern "C" int getpixel8(int x1, int y1);
#pragma aux getpixel8 parm [ ebx ] [ eax ] modify [ ecx edx ]


void putpixel(int x1, int y1, int color)
{
   if ( agmp->byteperpix == 1 )
      putpixel8 ( x1, y1, color );
   else {
      if ( agmp->windowstatus == 100 ) {
         char* pc = (char*) ( agmp->linearaddress + x1 * agmp->byteperpix + y1 * agmp->scanlinelength );
         int alpha = color >> 24;
         if ( alpha == 0 ) {
            pc[ agmp->redfieldposition/8 ] = color & 0xff;
            pc[ agmp->greenfieldposition/8 ] = (color >> 8) & 0xff;
            pc[ agmp->bluefieldposition/8 ] = (color >> 16) & 0xff;
         } else {
            pc[ agmp->redfieldposition/8 ] = pc[ agmp->redfieldposition/8 ] * alpha / alphabase + (color & 0xff) * (alphabase - alpha ) / alphabase;
            pc[ agmp->greenfieldposition/8 ] = pc[ agmp->greenfieldposition/8 ] * alpha / alphabase + ((color >> 8 ) & 0xff) * (alphabase - alpha ) / alphabase;
            pc[ agmp->bluefieldposition/8 ] = pc[ agmp->bluefieldposition/8 ] * alpha / alphabase + ((color >> 16) & 0xff) * (alphabase - alpha ) / alphabase;
         }
      } else {
         int pos = x1 * agmp->byteperpix + y1 * agmp->scanlinelength;
         int page = pos >> 16;
         if ( hgmp->actsetpage != page )
            setvirtualpagepos ( page );

         char* pc = (char*) ( agmp->linearaddress + (pos & 0xffff) );

         int alpha = color >> 24;
         if ( alpha == 0 ) {
            pc[ agmp->redfieldposition/8 ] = color & 0xff;
            pc[ agmp->greenfieldposition/8 ] = (color >> 8) & 0xff;
            pc[ agmp->bluefieldposition/8 ] = (color >> 16) & 0xff;
         } else {
            pc[ agmp->redfieldposition/8 ] = pc[ agmp->redfieldposition/8 ] * alpha / alphabase + (color & 0xff) * (alphabase - alpha ) / alphabase;
            pc[ agmp->greenfieldposition/8 ] = pc[ agmp->greenfieldposition/8 ] * alpha / alphabase + ((color >> 8 ) & 0xff) * (alphabase - alpha ) / alphabase;
            pc[ agmp->bluefieldposition/8 ] = pc[ agmp->bluefieldposition/8 ] * alpha / alphabase + ((color >> 16) & 0xff) * (alphabase - alpha ) / alphabase;
         }
         
      }
   }
}

int getpixel(int x1, int y1)
{
   if ( agmp->byteperpix == 1 )
      return getpixel8( x1, y1 );
   else {
      if ( agmp->windowstatus == 100 ) {
         char* pc = (char*) ( agmp->linearaddress + x1 * agmp->byteperpix + y1 * agmp->scanlinelength );
         return pc[ agmp->redfieldposition/8 ] + 
              ( pc[ agmp->greenfieldposition/8 ] << 8 ) + 
              ( pc[ agmp->bluefieldposition/8 ] << 16 );
      } else {
         int pos = x1 * agmp->byteperpix + y1 * agmp->scanlinelength;
         int page = pos >> 16;
         if ( hgmp->actsetpage != page )
            setvirtualpagepos ( page );

         char* pc = (char*) ( agmp->linearaddress + (pos & 0xffff) );
         return pc[ agmp->redfieldposition/8 ] + 
              ( pc[ agmp->greenfieldposition/8 ] << 8 ) + 
              ( pc[ agmp->bluefieldposition/8 ] << 16 );
      }
   }
}

TrueColorImage :: TrueColorImage ( int x, int y )
{
   pix = new trgbpixel[x*y];
   xsize = x;
   ysize = y;
}

TrueColorImage :: ~TrueColorImage (  )
{
   if ( pix ) {
      delete[] pix;
      pix = NULL;
   }
}


int getpixelfromimage ( void* buf, int x, int y )
{
   word* wp = (word*) buf;
   if ( x > wp[0]  ||  y > wp[1] || x < 0 || y < 0 )
      return -1;

   char* pc = (char*) buf;
   return pc[4 + x + y * (wp[0]+1) ];
}

void TrueColorImage :: setpix ( int x, int y, int r, int g, int b, int alpha )
{
   trgbpixel* p = & pix[ x + y * xsize ];
   p->channel.r = r;
   p->channel.g = g;
   p->channel.b = b;
   p->channel.a = alpha;
}

trgbpixel TrueColorImage :: getpix ( int x, int y )
{
   return pix[ x + y * xsize ];
}


int TrueColorImage :: getxsize( void )
{
   return xsize;
}

int TrueColorImage :: getysize( void )
{
   return ysize;
}



int f2i ( float f )
{
   
//   int b = f;

   int a; 
   f -= 0.4999;
   float2int ( &f, &a );
/*   if ( a != b )
      return -1; */
   return a;
}

int f2i2 ( float f )
{
   int a; 
   float2int ( &f, &a );
   return a;
}


int newpalgenerated = 0;
dacpalette256 ppal;




TrueColorImage* zoomimage ( void* buf, int xsize, int ysize, dacpalette256 pal, int interpolate, int proportional )
{
   if ( !newpalgenerated ) 
      for ( int i = 0; i < 256; i++ )
         for ( int j = 0; j<3; j++ )
            ppal[i][j] = pal[i][j] << 2;



   int orgx;
   int orgy;
   getpicsize ( buf, orgx, orgy );

   {
      float ratiox = (float)xsize / (float)orgx;
      float ratioy = (float)ysize / (float)orgy;
      if ( proportional ) {
         if ( ratiox > ratioy )
            xsize = ratioy * orgx;
         else
            ysize = ratiox * orgy;
      }
   }

   TrueColorImage* img = new TrueColorImage ( xsize, ysize );
   

   float dx = (float)orgx / (float)xsize;
   float dy = (float)orgy / (float)ysize;

   float oy = 0;
   for ( int y = 0; y < ysize; y++, oy+=dy ) {

      int oyi = f2i ( oy );
      float oyif = oyi;

      float ox = 0;
      for ( int x = 0; x < xsize; x++,ox+=dx ) {

         float r = 0;
         float g = 0;
         float b = 0;
         float alpha = 0;

         if ( interpolate ) {
            float d;

            int oxi = f2i ( ox );
            float oxif = oxi;
   
            float dist = 0;
            int alphanum = 0;

            // pixel links oben
            int px1 = getpixelfromimage ( buf, oxi, oyi );
            int px2 = getpixelfromimage ( buf, oxi+1, oyi );
            int px3 = getpixelfromimage ( buf, oxi, oyi+1 );
            int px4 = getpixelfromimage ( buf, oxi+1, oyi+1 );
            if ( px1 == 255 && px2 == 255 && px3 == 255 && px4 == 255 ) {
                r = 0;
                g = 0;
                b = 0;
                alpha = alphabase;
            } else {

               if ( px1 >= 0 ) {
                  d = ( 1-(ox - oxif)) * ( 1-( oy - oyif ));
      
                  if ( px1 == 255 ) {
                     alpha += ((float)alphabase) * d;
                     alphanum++;
                  }
                  else {
                     dist += d;
                     r += ppal[px1][0] * d ;
                     g += ppal[px1][1] * d ;
                     b += ppal[px1][2] * d ;
                  }
               }
                  
               // pixel rechts oben
               if ( px2 >= 0 ) {
                  d = (ox - oxif) * ( 1 -  ( oy - oyif ));
      
                  if ( px2 == 255 ) {
                     alpha += ((float)alphabase) * d;
                     alphanum++;
                  }
                  else {
                     dist += d;
                     r += ppal[px2][0] * d ;
                     g += ppal[px2][1] * d ;
                     b += ppal[px2][2] * d ;
                  }
               }
   
   
               // pixel links unten
               if ( px3 >= 0 ) {
                  d = ( 1- (ox - oxif)) *  (oy - oyif);
      
                  if ( px3 == 255 ) {
                     alpha += ((float)alphabase) * d;
                     alphanum++;
                  }
                  else {
                     dist += d;
                     r += ppal[px3][0] * d ;
                     g += ppal[px3][1] * d ;
                     b += ppal[px3][2] * d ;
                  }
               }
   
               // pixel rechts unten
               if ( px4 >= 0 ) {
                  d = (ox - oxif) *  (oy - oyif);
      
                  if ( px4 == 255 ) {
                     alpha += ((float)alphabase) * d;
                     alphanum++;
                  }
                  else {
                     dist += d;
                     r += ppal[px4][0] * d ;
                     g += ppal[px4][1] * d ;
                     b += ppal[px4][2] * d ;
                  }
               }
   
               if ( alphanum <= 2 ) 
                  alpha = 0;
   
               if ( dist ) {
                  r /= dist;
                  g /= dist;
                  b /= dist;
                  alpha /= dist;
               } else {
                  r = 0;
                  g = 0;
                  b = 0;
                  alpha = alphabase;
               }
            }
         } else {
             int px = getpixelfromimage ( buf, f2i2(ox), f2i2(oy) );
             if ( px == 255 ) 
                alpha += alphabase ;
             else {
                r += ppal[px][0] ;
                g += ppal[px][1] ;
                b += ppal[px][2] ;
             }

         }
         img->setpix ( x, y, f2i(r), f2i(g), f2i(b), f2i(alpha) );
      }
   }
   
   return img;
}

#define sqr(a) (a)*(a)
#define cub(a) abs ((a)*(a)*(a))


#ifndef minimal
 #include "basestrm.h"
#endif


TrueColorImage* convertimage2tc ( void* buf, dacpalette256 pal )
{
   if ( !buf ) 
      return NULL;

   int x, y;
   getpicsize ( buf, x, y );
   return zoomimage ( buf, x,y , pal, 0, 0 );
}


char* convertimage ( TrueColorImage* img, dacpalette256 pal )
{
  #ifndef minimal
   if ( truecolor2pal_table[0] == 255 ) {
      tfindfile ff ( "tc2pal.dat" );
      if ( ff.getnextname() ) {
         tnfilestream stream ( "tc2pal.dat", 1 );
         stream.readdata ( truecolor2pal_table, sizeof ( truecolor2pal_table ));
      } else {
         for ( int r = 0; r < 64; r++ )
            for ( int g = 0; g < 64; g++ )
               for ( int b = 0; b < 64; b++ ) {
                     int sml = r  + ( g << 6) + ( b << 12 );
      
                     int diff = 0xFFFFFFF;
                     int pix1;
            
                     for ( int k=0;k<256 ;k++ ) {
                        int actdif = sqr( pal[k][0]  - r ) + sqr( pal[k][1]  - g ) + sqr( pal[k][2]  - b );
            
                        if (actdif < diff) {
                           diff = actdif;
                           pix1 = k;
                        }
                     } 
                    truecolor2pal_table[sml] = pix1;
               }
       /*
         tnfilestream stream ( "tc2pal.dat", 2 );
         stream.writedata ( truecolor2pal_table, sizeof ( truecolor2pal_table ));
       */
      }
   }
  #endif
   int size = imagesize ( 1, 1, img->getxsize(), img->getysize() );
   char* newimg = new char[ size ] ;
   char* start = newimg;
   word* wp = (word*) newimg;
   wp[0] = img->getxsize()-1;
   wp[1] = img->getysize()-1;

   newimg+= 4;

   for ( int y = 0; y <= wp[1]; y++ )
      for ( int x = 0; x <= wp[0]; x++ ) {
         trgbpixel p  = img->getpix ( x, y );


         if ( p.channel.a <= alphabase*2/3 ) {
            int sml = ( p.channel.r >> 2) + (( p.channel.g >> 2) << 6) + (( p.channel.b >> 2) << 12);

            *newimg = truecolor2pal_table[sml];

         } else {
           *newimg = 255;
         }
         newimg++;
      }

   if ( newimg - start > size ) 
      printf("\a");
   return (char*)wp;
}


fatalgraphicserror :: fatalgraphicserror ( char* strng ) {
   strcpy ( st, strng );
}

fatalgraphicserror :: fatalgraphicserror ( void ) {
   st[0] = 0;
}
                 
void putmask ( int x1, int y1, void* vbuf, int newtransparence )
{
      int linecount = 0;
      int rowcount = 0;

      word* wp = (word*) vbuf;

      char* basemembuf = (char*) (agmp->linearaddress + y1 * agmp->bytesperscanline + x1 * agmp->byteperpix );
      char* img = (char*) vbuf + 4;

      for ( int y = 0; y <= wp[1]; y++ ) {
         char* membuf = basemembuf + y * agmp->bytesperscanline;

         for ( int x = 0; x <= wp[0]; x++ ) {
            if ( *img != newtransparence )
               *membuf = *img;
            membuf += agmp->byteperpix;
            img++;
         }
      }
}

int graphicinitialized = 0;



void* uncompress_rlepict ( void* pict )
{
   trleheader* hd = (trleheader*) pict;
   if (hd->id == 16973) {

      int w, h;
      getpicsize( pict, w, h );
      w++;
      h++;
      void* newbuf = asc_malloc ( w * h + 4 );
      word* wp = (word*) newbuf;
      char* dest = (char*) newbuf;
      wp[0] = w-1;
      wp[1] = h-1;
      dest +=4;
   
      char* buf = (char*) pict;
      buf += sizeof ( *hd );
   
      for ( int c = 0; c < hd->size; c++ ) {
         if ( *buf == hd->rle ) {
            for ( int i = buf[1]; i > 0; i-- ) {
               *dest = buf[2];
               dest++;
            }
            buf += 3;
            c += 2;
   
         } else {
            *dest = *buf;
            dest++;
            
            buf++;
         }
      }
      return newbuf;
   } else
      return NULL;
}

void setvgapalette256 ( dacpalette256 pal )
{
   if ( graphicinitialized )
      set_vgapalette256( pal );
   else
      memcpy ( activepalette256, pal, sizeof ( *activepalette256 ));
}


/*
void* getlastpointer ( void )
{
    return (void*) uncompress_rlepict;
}    
*/

void ellipse ( int x1, int y1, int x2, int y2, int color, float tolerance )
{
   int midx = (x1 + x2) / 2;
   int midy = (y1 + y2) / 2;
   float xr = x2 - x1;
   float yr = y2 - y1;

   tolerance = tolerance / (xr+yr) *  80;

   xr= (xr/2)*(xr/2);
   yr= (yr/2)*(yr/2);

   for ( int y = y1; y <= y2; y++ )
      for ( int x = x1; x <= x2; x++ ) {
         float dx = x - midx;
         float dy = y - midy;
         float tmp = dx*dx/xr + dy*dy/yr;
//         float tmp = dx*dx*yr + dy*dy*xr;
         if (  tmp <= 1 + tolerance && tmp >= 1 - tolerance )
            putpixel ( x, y, color );
      }

}

#endif
