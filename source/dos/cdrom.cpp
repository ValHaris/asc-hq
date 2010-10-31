//     $Id: cdrom.cpp,v 1.3 2009-04-18 13:48:38 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  2000/08/12 12:52:56  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.1  2000/05/30 18:39:28  mbickel
//      Added support for multiple directories
//      Moved DOS specific files to a separate directory
//
//     Revision 1.5  2000/05/23 20:40:37  mbickel
//      Removed boolean type
//
//     Revision 1.4  2000/04/27 16:25:16  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.3  2000/03/29 09:58:42  mbickel
//      Improved memory handling for DOS version
//      Many small changes I can't remember ;-)
//
//     Revision 1.2  1999/11/16 03:41:13  tmwilson
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
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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

#include "cdrom.h"

#define xshowstatus

   call_real_int ri(cdromintmemsize);

   const char* ttdef[5]   =  {"2 audio-channels without pre-emphasis","2 audio-channels with pre-emphasis",
                                       "4 audio-channels without pre-emphasis", "4 audio-channels with pre-emphasis","Data Track"};
   const char* cerror[17]  = {"No Error","Write-protect violation","Unknown unit","Drive not ready","Unknown command","CRC error",
                                      "Bad drive request structure length","Seek error","Unknown media","Sector not found",
                                      "Printer out of paper","Write fault","Read fault","General failure","Reserved","Reserved",
                                      "Invalid disk change"};

tcdrom::tcdrom( void )
{
   if ( testcdromavailable() == 1 ) {
      getcdrominfo();
      activecdrom = driveletter[0];
      getcdromdrives();
   }
   else { 
       printf("No CD-Rom detected !\n");
       numberofdrives = 0;
   } 
}


void tcdrom::readcdinfo( void )
{  pascal_byte lt,ht,m,s,f,t,e;

   int i = 0;
   lt = 0;
   ht = 0;
   while ( ( (lt > ht) || ( lt > 99 ) || ( ht > 99 ) || ( ht < 1 ) || ( lt < 1 ) || ( e > 0 ) ) && ( i < 20 )) {
      lt = 0;
      ht = 0;
      getcdinfo(&lt,&ht,&m,&s,&f);
      e = geterror();
      i++;
   } /* endwhile */
   cdinfo.firsttrack= 0;
   cdinfo.lasttrack= 0;
   if (i == 20 ) return;
   cdinfo.firsttrack= lt;
   cdinfo.lasttrack= ht;
   cdinfo.smin = m;
   cdinfo.ssec = s;
   cdinfo.sframe = f;
   cdinfo.start = getnormalsector(m,s,f);
   getcdlength(&m,&s,&f);
   cdinfo.min = m;
   cdinfo.sec = s;
   cdinfo.frame = f;
   cdinfo.size = getnormalsector(m,s,f);

   for (i=0 ;i < ht ;i++ ) {
      cdinfo.track[i] = new( ttrackinf );
      gettrackinfo(i,&m,&s,&f,&t);
      cdinfo.track[i]->smin = m;
      cdinfo.track[i]->ssec = s;
      cdinfo.track[i]->sframe = f;
      cdinfo.track[i]->type = t;
      cdinfo.track[i]->start = getnormalsector(m,s,f);
      gettracklength(i,&m,&s,&f);
      cdinfo.track[i]->min = m;
      cdinfo.track[i]->sec = s;
      cdinfo.track[i]->frame = f;
      cdinfo.track[i]->size = getnormalsector(m,s,f);
   } /* endfor */

}

void tcdrom::playtrack(pascal_byte nr)
{
   if (cdinfo.track[nr]->type > 3) return;
   stopaudio();
   playaudio(cdinfo.track[nr]->start,cdinfo.track[nr]->size);
}

void tcdrom::playtrackuntilend(pascal_byte nr)
{
   if (cdinfo.track[nr]->type > 3) return;
   stopaudio();
   playaudio(cdinfo.track[nr]->start,cdinfo.size-cdinfo.track[nr]->start);
}


tcdrom::~tcdrom( void )
{
   for (int i=0 ;i < cdinfo.lasttrack ;i++ ) if (cdinfo.track[i] != NULL) free ( cdinfo.track[i] );
}


char  tcdrom::testcdromavailable(void)
{ 
   ri.setuprmi();
   rmi.eax = 0x1500; 
   rmi.ebx = 0x0;
   ri.real_call(0x2f);
   if (rmi.ebx == 0) return false;
   else return true;
} 

pascal_byte tcdrom::geterror( void )

{ word       st;

   if ((ioctl->requestheader.status & 0x8000) > 0) { 
      st = ioctl->requestheader.status & 255;
      return ( st <255 ) ? ++st : 255;
   } 
   else return 0;
} 

pascal_byte tcdrom::checkerror( void )
{  int e;

  if (ioctl == NULL) printf("\nError : IOCTL is NULL !!!\n\n");
  else {
     e = geterror();
     if (e > 0 ) 
        if ( e <= 16 ) printf("\nError : %s\n\n",cerror[e]);
        else printf("\nError : Unknown Error\n\n");
  } 
  return e;
}


char tcdrom::openclosecdrom(void)  
{ 
   ioctl = ( tioctlo *) ri.protectedsegment;
   ds = ( tdevicestatus * ) &ioctl[1];
   memset(ioctl,0,sizeof(*ioctl));
   ioctl->requestheader.len = sizeof(tioctlo);
   ioctl->requestheader.commandcode = 3;
   ioctl->buffer = ( ( ri.realsegment << 16 ) + 28 );
   ioctl->buffersize = sizeof(*ds);
   ds->controlblockcode = 6; 
   ds->status = 0;
   
   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);

   memset(ioctl,0,sizeof(*ioctl));
   ioctl->requestheader.len = sizeof(tioctlo);
   ioctl->requestheader.commandcode = 12;
   ioctl->buffer = ( ( ri.realsegment << 16 ) + 28 );
   ioctl->buffersize = 1;
   if (((ds->status & 0x1) == 0)) { 
      ds->controlblockcode = 0;
      //return true;
   } 
   else { 
      ds->controlblockcode = 5;
      //return false;
   } 

   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   call_real_int(0x2f);
   return true;
} 

char tcdrom::testcdromopen(void)
{ 
   ioctl = ( tioctlo *) ri.protectedsegment;
   ds = ( tdevicestatus * ) &ioctl[1];
   memset(ioctl,0,sizeof(*ioctl));
   ioctl->requestheader.len = sizeof(tioctlo);
   ioctl->requestheader.commandcode = 3;
   ioctl->buffer = ( ( ri.realsegment << 16 ) + 28 );
   ioctl->buffersize = sizeof(*ds);
   ds->controlblockcode = 6; 
   ds->status = 0;
   
   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   call_real_int(0x2f);

   memset(ioctl,0,sizeof(*ioctl));
   ioctl->requestheader.len = sizeof(tioctlo);
   ioctl->requestheader.commandcode = 12;
   ioctl->buffer = ( ( ri.realsegment << 16 ) + 28 );
   ioctl->buffersize = 1;
   if (((ds->status & 0x1) == 0)) { 
      return true;
   } 
   else { 
      return false;
   } 
} 



void tcdrom::playaudio(int ss,   int numbersectors)
                            

{ tplayaudio    *pa; 

   pa = ( tplayaudio *) ri.protectedsegment;
   memset(pa,0,sizeof(*pa)); 

   ioctl->requestheader.len = sizeof(pa);
   ioctl->requestheader.commandcode = 132;
   pa->adressingmode = 0; 
   pa->startsector = ss;
   pa->numberofsectors = numbersectors;
   
   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);
} 


void tcdrom::stopaudio(void)  

{ tstopresume   *sr; 

   sr = ( tstopresume *) ri.protectedsegment;
   memset(sr,0,sizeof(*sr));
   
   ioctl->requestheader.len = sizeof(sr);
   ioctl->requestheader.commandcode = 133;
   
   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);
}

char tcdrom::checkbusy(void)
{ 
   ioctl = ( tioctlo *) ri.protectedsegment;
   ds = ( tdevicestatus * ) &ioctl[1];
   memset(ioctl,0,sizeof(*ioctl));
   ioctl->requestheader.len = sizeof(tioctlo);
   ioctl->requestheader.commandcode = 3;
   ioctl->buffer = ( ( ri.realsegment << 16 ) + 28 );
   ioctl->buffersize = sizeof(*ds);
   ds->controlblockcode = 6; 
   ds->status = 0;
   
   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);

   if ((ioctl->requestheader.status & 512) > 0) return true; 
   else return false; 
} 


int tcdrom::getcdsize(void) 

{ tcdsize       *cds;

   ioctl = ( tioctlo *) ri.protectedsegment;
   cds = ( tcdsize * ) &ioctl[1];
   memset(ioctl,0,sizeof(*ioctl));
   ioctl->requestheader.len = sizeof(tioctlo);
   ioctl->requestheader.commandcode = 3;
   ioctl->buffer = ( ( ri.realsegment << 16 ) + 28 );
   ioctl->buffersize = sizeof(*cds);
   cds->controlblockcode = 8;
   
   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);
   return cds->size;
} 

void tcdrom::getcdinfo(pascal_byte *l,pascal_byte *h,pascal_byte *min,pascal_byte *sec,pascal_byte *frame) 

{ tcdinfo       *cdi;
               
   ioctl = ( tioctlo *) ri.protectedsegment;
   cdi = ( tcdinfo * ) &ioctl[1];
   memset(ioctl,0,sizeof(*ioctl));
   ioctl->requestheader.len = sizeof(tioctlo);
   ioctl->requestheader.commandcode = 3;
   ioctl->buffer = ( ( ri.realsegment << 16 ) + 28 );
   ioctl->buffersize = sizeof(*cdi);
   cdi->controlblockcode = 10;
   
   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);

   *l = cdi->lowesttrack; 
   *h = cdi->highesttrack;
   *min = (cdi->leadout & 0x00FF0000) >> 16;
   *sec = (cdi->leadout & 0x0000FF00) >> 8;
   *frame = cdi->leadout & 0x000000FF;
} 

void tcdrom::getcdlength(pascal_byte *min,pascal_byte *sec,pascal_byte *frame)

{   pascal_byte         m, s, f, m1, s1, f1,ht,lt,t;

   getcdinfo(&lt,&ht,&m1,&s1,&f1);
   gettrackinfo(0,&m,&s,&f,&t);
            
   *min = m1 - m; 
   if (s > s1) { 
      if (*min > 0) *min = *min - 1; 
      else *min = 59; 
      *sec = 60 - s + s1; 
   } 
   else *sec = s1 - s; 
   if (f > f1) { 
      if (*sec > 0) *sec = *sec - 1; 
      else *sec = 59; 
      *frame = 74 - f + f1; 
   } 
   else *frame = f1 - f; 
} 

void tcdrom::gettracklength(pascal_byte tracknr,pascal_byte *min,pascal_byte *sec,pascal_byte *frame)

{ pascal_byte         m, m1, s, s1, f, f1, l, h, m2, s2, f2,t;

   getcdinfo(&l,&h,&m2,&s2,&f2); 
   gettrackinfo(tracknr,&m,&s,&f,&t); 
   if (tracknr < h-1) gettrackinfo(tracknr + 1,&m1,&s1,&f1,&t);
   else { 
      m1 = m2; 
      s1 = s2; 
      f1 = f2; 
   } 
   *min = m1 - m; 
   if (s > s1) { 
      if (*min > 0) *min = *min - 1; 
      else *min = 59; 
      *sec = 60 - s + s1; 
   } 
   else *sec = s1 - s; 
   if (f > f1) { 
      if (*sec > 0) *sec = *sec - 1; 
      else { 
         *sec = 59; 
         *min = *min - 1; 
      } 
      *frame = 74 - f + f1; 
   } 
   else *frame = f1 - f; 
} 

void tcdrom::getactivetimes(pascal_byte *min,pascal_byte *sec,pascal_byte *frame,pascal_byte *amin,pascal_byte *asec,pascal_byte *aframe)
{ tqinfo        *qi;

   ioctl = ( tioctlo *) ri.protectedsegment;
   qi = ( tqinfo * ) &ioctl[1];
   memset(ioctl,0,sizeof(*ioctl));
   ioctl->requestheader.len = sizeof(tioctlo);
   ioctl->requestheader.commandcode = 3;
   ioctl->buffer = ( ( ri.realsegment << 16 ) + 28 );
   ioctl->buffersize = sizeof(*qi);
   qi->controlblockcode = 12;
   
   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);

   *amin = qi->amin;
   *asec = qi->asec;
   *aframe = qi->aframe;
   *min = qi->min;
   *sec = qi->sec;
   *frame = qi->frame;
} 



int tcdrom::getheadlocation(void)  

{ tlocatehead      *lh;

   ioctl = ( tioctlo *) ri.protectedsegment;
   lh = ( tlocatehead * ) &ioctl[1];
   memset(ioctl,0,sizeof(*ioctl));
   ioctl->requestheader.len = sizeof(tioctlo);
   ioctl->requestheader.commandcode = 3;
   ioctl->buffer = ( ( ri.realsegment << 16 ) + 28 );
   ioctl->buffersize = sizeof(*lh);
   lh->controlblockcode = 1;
   lh->adressingmode = 0;
   
   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);

   return lh->headlocation;
} 


void tcdrom::seeksector(int ss)  

{ tseek         *s;

   s = ( tseek *) ri.protectedsegment;
   memset(s,0,sizeof(*s));
   s->requestheader.len = sizeof(tseek);
   s->requestheader.commandcode = 131;
   s->adressingmode = 0;
   s->startsector = ss;
   
   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);
} 


void tcdrom::gettrackinfo(pascal_byte tracknr,pascal_byte * min,pascal_byte * sec,pascal_byte * frame,pascal_byte * type)  

{  ttrackinfo    *ti;

   tracknr++;
   ioctl = ( tioctlo *) ri.protectedsegment;
   ti = ( ttrackinfo * ) &ioctl[1];
   memset(ioctl,0,sizeof(*ioctl));
   ioctl->requestheader.len = sizeof(tioctlo);
   ioctl->requestheader.commandcode = 3;
   ioctl->buffer = ( ( ri.realsegment << 16 ) + 28 );
   ioctl->buffersize = sizeof(*ti);
   ti->controlblockcode = 11;
   ti->tracknumber = tracknr;
   
   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);

   *min = (ti->startpoint & 0x00FF0000) >> 16;
   *sec = (ti->startpoint & 0x0000FF00) >> 8;
   *frame = ti->startpoint & 0x000000FF;
   ti->controlinfo &= 0xF0;
   if ( ( ti->controlinfo & 0x80 ) > 0) *type = ( ( ti->controlinfo & 0x10 ) > 0 ) ? 3 : 2;
   else *type = ( ( ti->controlinfo & 0x10 ) > 0 ) ? 1 : 0;
   if ( ( ti->controlinfo & 0x40 ) > 0) *type = 4;
} 


void tcdrom::getaudioinfo()
{ 
   ioctl = ( tioctlo *) ri.protectedsegment;
   ac = ( taudiochannel * ) &ioctl[1];
   memset(ioctl,0,sizeof(*ioctl));
   ioctl->requestheader.len = sizeof(tioctlo);
   ioctl->requestheader.commandcode = 3;
   ioctl->buffer = ( ( ri.realsegment << 16 ) + 28 );
   ioctl->buffersize = sizeof(*ac);
   ac->controlblockcode = 4;

   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);

   ta.volume[0] = ac->volumechannel0;
   ta.volume[1] = ac->volumechannel1;
   ta.volume[2] = ac->volumechannel2;
   ta.volume[3] = ac->volumechannel3;
   ta.channel[0] = ac->inputchannel0;
   ta.channel[1] = ac->inputchannel1;
   ta.channel[2] = ac->inputchannel2;
   ta.channel[3] = ac->inputchannel3;
} 


void tcdrom::setaudiochannel() 
{ 
   ioctl = ( tioctlo *) ri.protectedsegment;
   ac = ( taudiochannel * ) &ioctl[1];
   memset(ioctl,0,sizeof(*ioctl));
   ioctl->requestheader.len = sizeof(tioctlo);
   ioctl->requestheader.commandcode = 12;
   ioctl->buffer = ( ( ri.realsegment << 16 ) + 28 );
   ioctl->buffersize = 9;
   ac->controlblockcode = 3;
   ac->volumechannel0 = ta.volume[0] ;
   ac->volumechannel1 = ta.volume[1] ;
   ac->volumechannel2 = ta.volume[2] ;
   ac->volumechannel3 = ta.volume[3] ;
   ac->inputchannel0 = ta.channel[0] ;
   ac->inputchannel1 = ta.channel[1] ;
   ac->inputchannel2 = ta.channel[2] ;
   ac->inputchannel3 = ta.channel[3] ;

   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);
} 

char tcdrom::lockunlockcdrom(void)

{ tlockdoor     *ld;

   ioctl = ( tioctlo *) ri.protectedsegment;
   ds = ( tdevicestatus * ) &ioctl[1];
   memset(ioctl,0,sizeof(*ioctl));
   ioctl->requestheader.len = sizeof(tioctlo);
   ioctl->requestheader.commandcode = 3;
   ioctl->buffer = ( ( ri.realsegment << 16 ) + 28 );
   ioctl->buffersize = sizeof(*ds);
   ds->controlblockcode = 6; 
   ds->status = 0;
   
   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);

   ld = ( tlockdoor * ) &ioctl[1];

   memset(ioctl,0,sizeof(*ioctl));
   ioctl->requestheader.len = sizeof(tioctlo);
   ioctl->requestheader.commandcode = 12;
   ioctl->buffer = ( ( ri.realsegment << 16 ) + 28 );
   ioctl->buffersize = 2;
   ld->controlblockcode = 1;
   if (((ds->status & 0x2) == 0)) ld->lockfunction = 0;
   else ld->lockfunction = 1;

   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);

   if (((ds->status & 0x2) == 0)) return false;
   else return true;
} 




void tcdrom::resumeaudio(void)  

{ tstopresume   *sr;

   sr = ( tstopresume *) ri.protectedsegment;
   memset(sr,0,sizeof(*sr));
   
   ioctl->requestheader.len = sizeof(sr);
   ioctl->requestheader.commandcode = 136;
   
   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);
} 


float tcdrom::get_mscdex_version(void)
{ 
   ri.setuprmi();
   rmi.eax = 0x150C;
   ri.real_call(0x2f);

   return rmi.bh + (rmi.bl / 100);
} 

void tcdrom::getcdrominfo(void)  
{ 
   ri.setuprmi();
   rmi.eax = 0x1500;
   ri.real_call(0x2f);

   numberofdrives = rmi.bx;
   driveletter[0] = rmi.cx;
} 

void tcdrom::getcdromdrives(void)  
{ 
   char *drives;

   drives = ( char *) ri.protectedsegment;
   ri.setuprmi();
   rmi.eax = 0x150D;
   rmi.ebx = 0;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);
   for (int i=0;i < numberofdrives ;i++ ) driveletter[i+1]=drives[i];
} 

void tcdrom::changecdromdrive(char nr)
{
   if ( (nr < 0) || ( nr > numberofdrives-1 ) ) return;
   activecdrom = driveletter[nr];
}



int tcdrom::getnormalsector(pascal_byte m,pascal_byte s,pascal_byte f)
{ 
   return (int(m) * 60 * 75) + (int(s) * 75) + int(f) - 150; 
} 


void tcdrom::getsectortime(int sector,pascal_byte *m,pascal_byte *s,pascal_byte *f)

{ int      v; 

   sector+=150;
   *f = sector % 75; 
   v = sector / 75; 
   *m = v / 60; 
   *s = v % 60; 
} 

void* tcdrom::getdevheaderadress(void)
{ tgetdevheader *dh;

   ioctl = ( tioctlo *) ri.protectedsegment;
   dh = ( tgetdevheader * ) &ioctl[1];
   memset(ioctl,0,sizeof(*ioctl));
   ioctl->requestheader.len = sizeof(tioctlo);
   ioctl->requestheader.commandcode = 3;
   ioctl->buffer = ( ( ri.realsegment << 16 ) + 28 );
   ioctl->buffersize = sizeof(*dh);
   dh->controlblockcode = 0;
   dh->adress = 0;
   
   ri.setuprmi();
   rmi.eax = 0x1510;
   rmi.ebx = 0;
   rmi.ecx = activecdrom;
   rmi.es = ri.realsegment;
   ri.real_call(0x2f);

   return dh->adress;
} 
