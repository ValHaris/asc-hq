//     $Id: cdrom.h,v 1.2 1999-11-16 03:41:14 tmwilson Exp $
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

#ifdef _DOS_
#include <i86.h>
#include <conio.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tpascal.inc"
#include "realint.h"

     typedef char tdevname[8]; 
     struct trequestheader { 
                       byte         len; 
                       byte         subunit; 
                       byte         commandcode; 
                       word         status; 
                       tdevname  devname; 
                    } ;


     struct tgetdevheader {
                     byte         controlblockcode; 
                     pointer      adress; 
                  } ;

     struct tioctlo { 
                trequestheader requestheader; 
                byte         mediadescriptor; 
                int            buffer;
                word         buffersize; 
                int      startsector; 
                int      volumeptr; 
             };

     struct tlocatehead {
                byte         controlblockcode;   /* 1 */ 
                byte         adressingmode; 
                int      headlocation; 
             } ; 

     struct taudiochannel {   /* 4 */
                      byte         controlblockcode; 
                      byte         inputchannel0; 
                      byte         volumechannel0; 
                      byte         inputchannel1; 
                      byte         volumechannel1; 
                      byte         inputchannel2; 
                      byte         volumechannel2; 
                      byte         inputchannel3; 
                      byte         volumechannel3; 
                   } ;

      struct tdevicestatus {
                      byte         controlblockcode;   /* 6 */ 
                      int      status; 
                   };

      struct tcdsize {
                byte         controlblockcode;   /* 8 */ 
                int      size; 
             } ;

      struct tcdinfo {
                byte         controlblockcode;   /* 10 */ 
                byte         lowesttrack; 
                byte         highesttrack; 
                int      leadout; 
             };

      struct ttrackinfo {
                   byte         controlblockcode;   /* 11 */ 
                   byte         tracknumber; 
                   int      startpoint; 
                   byte         controlinfo;
                } ;

      struct tqinfo {
               byte         controlblockcode;   /* 12 */ 
               byte         caab; 
               byte         poi; 
               byte         tracknumber; 
               byte         min; 
               byte         sec; 
               byte         frame; 
               byte         zero; 
               byte         amin; 
               byte         asec; 
               byte         aframe; 
            };

      struct tlockdoor {
                  byte         controlblockcode;   /* 1 */ 
                  byte         lockfunction; 
               } ; 

      struct tseek {   /* 131 */
              trequestheader requestheader; 
              byte         adressingmode; 
              int      transferadress; 
              word         numberofsectors; 
              int      startsector; 
           } ; 

      struct tplayaudio {   /* 132 */
                   trequestheader requestheader; 
                   byte         adressingmode; 
                   int      startsector; 
                   int      numberofsectors; 
                } ; 

     struct tstopresume {   /* 133,134 */
                   trequestheader requestheader; 
                } ; 

     struct ttrackinf {
            char name[30];
            byte min,sec,frame;
            int start;
            byte smin,ssec,sframe;
            int size;
            byte type;
        };

     typedef ttrackinf *ptrackinfo;
     struct tcdinf {
            byte min,sec,frame;
            int size;
            byte smin,ssec,sframe;
            int start;
            byte firsttrack,lasttrack;
            char name[50];
            ptrackinfo track[99];
        } ;

     struct taudioinfo {
        byte volume[3];
        byte channel[3];
     };

#define tt2c 0
#define tt2cp 1
#define tt4c 2
#define tt4cp 3
#define ttd 4

const short int cdromintmemsize=60;

class tcdrom {
      tioctlo *ioctl;
      taudiochannel *ac;
      tdevicestatus *ds;
   public :
      char activecdrom;
      byte error;
      char numberofdrives;
      char driveletter[16];
      taudioinfo ta;
      tcdinf cdinfo;

      tcdrom(void);
      ~tcdrom(void);

      pointer getdevheaderadress(void);
      boolean testcdromavailable(void);
      byte geterror( void );
      byte checkerror( void );
      boolean testcdromopen(void);
      void getcdrominfo(void);
      void getcdromdrives(void);
      void changecdromdrive(char nr);
      float get_mscdex_version(void);
      boolean openclosecdrom(void);
      boolean lockunlockcdrom(void);
      void getsectortime(int sector,byte *m,byte *s,byte *f);
      int getnormalsector(byte m,byte s,byte f);
      boolean checkbusy(void);
      int getheadlocation(void);
      void seeksector(int ss);
      void getactivetimes(byte *min,byte *sec,byte *frame,byte *amin,byte *asec,byte *aframe);
      int getcdsize(void);
      void getcdlength(byte *min,byte *sec,byte *frame);
      void getcdinfo(byte *l,byte *h,byte *min,byte *sec,byte *frame);
      void gettracklength(byte tracknr,byte *min,byte *sec,byte *frame);
      void gettrackinfo(byte tracknr,byte * min,byte * sec,byte * frame,byte * type);
      void getaudioinfo( void );
      void setaudiochannel( void );
      void playaudio(int ss,   int numbersectors);
      void stopaudio(void);
      void resumeaudio(void);

      void readcdinfo( void );
      void playtrack(byte nr);
      void playtrackuntilend(byte nr);
};

