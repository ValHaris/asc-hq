//     $Id: vesa.cpp,v 1.8 2000-05-23 20:40:53 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.7  2000/01/07 13:20:07  mbickel
//      DGA fullscreen mode now working
//
//     Revision 1.6  2000/01/02 20:23:39  mbickel
//      Improved keyboard handling in dialog boxes under DOS
//
//     Revision 1.5  1999/12/27 13:00:18  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.4  1999/11/22 18:28:04  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.3  1999/11/16 17:04:19  mbickel
//     Made ASC compilable for DOS again :-)
//     Merged all the bug fixes in that I did last week
//
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
#include <conio.h>
#include <stdio.h>
#include <i86.h>
#include <string.h>
#include <stdlib.h>


#include "tpascal.inc"
#include "vesa.h"
#include "misc.h"
#include "basegfx.h"

#ifndef minimal
 #include "loadpcx.h"
 #include "newfont.h"
#endif


#ifdef _NOASM_
void setdisplaystart( int x, int y) {}
#endif

int fullscreen = 1;

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


struct tavailablemodes {
          int   num;
          struct {  
                     int x,y;
                     int col;
                     int num;
                 } mode[256];
 
       };
typedef tavailablemodes* pavailablemodes;


int vesaerrorrecovery = 0;
tvesamodeinfo activevesamodeinfo;


tgraphmodeparameters lastmodecheckgeneralparams;
tvesamodeinfo   lastmodecheckvesaparams;
tvesainfoblock  vesainfo;
word           *modelist;

char         generalparamsread = false;

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




char 
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


char getvesamodeavail(int modenum);


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



char
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
	lastmodecheckgeneralparams.directscreenaccess = 1;

	if (vesamodefound == 1) {
		return (true);
	} else {
		return (false);
	}			/* endif */

}



#ifdef _NOASM_
 void settextmode(char mode);
#else
 extern "C" void settextmode(char mode);
#endif


char
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


void showavailablemodes ( void ) 
{
   printf("\navailable graphic modes:\n");
   pavailablemodes avm = searchformode ( 0, 0, 8 );
   for ( int i = 0; i < avm->num; i++ )
      if ( avm->mode[i].x >= 640  &&  avm->mode[i].y >= 480 )
         printf("  %d * %d \n", avm->mode[i].x, avm->mode[i].y );
}



int initgraphics ( int x, int y, int depth )
{
   if ( graphicinitialized )
      closesvga();

   int pass = 0;
   do {
      pavailablemodes avm = searchformode ( x, y, depth );
      if (avm->num > 0) {
   
         for (int i=0;i<avm->num ;i++ ) 
            printf("%d Bit mode available: %x        X: %d   Y: %d \n", depth, avm->mode[i].num, avm->mode[i].x, avm->mode[i].y);
   
         printf("Initializing ... \n");
   
         initsvga( avm->mode[avm->num-1].num );
         int mode = avm->mode[avm->num-1].num;
         delete  ( avm );
         return mode;
      } else {
         delete  ( avm );
         if ( depth == 32 )
            depth = 24;
         else {
            printf("graphic-mode not available !\nAvailable modes are:\n");
            showavailablemodes();
            return -1;
         }
      }
      pass++;
   } while ( pass < 5 ); /* enddo */
   return -2;
}

int reinitgraphics(int modenum)
{
   return initsvga ( modenum );
}


void  closegraphics ( void )
{
   if ( graphicinitialized ) {
      closesvga();
      settextmode(3);
   }

}


#ifdef _NOASM_

void settextmode( char mode )
{
   REGS reg;   
   reg.w.ax = mode;
   int386(0x10, &reg, &reg);
}

void set_vgapalette256( dacpalette256 pal )
{
   outp( 0x3C8, 0 );
   for ( int i = 0; i < 256; i++ )
      for ( int c = 0; c < 3; c++ ) {
         int col;
         if ( pal[i][c] == 255 )
            col = activepalette[i][c];
         else {
            col = pal[i][c];
            activepalette[i][c] = col;
         }
         outp( 0x3C9, col );
      }
}
#endif

int copy2screen( void ) 
{
   return 0;
};

int copy2screen( int x1, int y1, int x2, int y2 ) 
{
   return 0;
};

int isfullscreen ( void )
{
   return graphicinitialized;
}
   
#endif



