//     $Id: flview.cpp,v 1.3 2000-03-29 09:58:45 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  1999/11/16 03:41:42  tmwilson
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

#include "edmisc.h";
#include "flview.h"
#include "timer.h"

byte tflicview::loadflic(char *name)
{
   int flc_size;
   tfliheader flc_head;

   tfindfile tf(name);       
   if (tf.getnextname() != NULL) {
      {
         tnfilestream stream (name,1);
         stream.readdata( &flc_head , sizeof(tfliheader) );
      }
      flc_size = flc_head.flicsize;
      flcmem = new ( byte[flc_size]);
      {
         tnfilestream stream (name,1);
         stream.readdata( flcmem , flc_size);
      }
   } 
   else return 1; //File not found
   return 0;
}

byte tflicview::loadconvtable(char *name)
{
   {
      tnfilestream stream (name,1);
      stream.readdata( flcpal, sizeof(flcpal));
      stream.readdata( convtable, sizeof(convtable));
   }
 
  return 0;
}

void tflicview::init(void)
{
   endatkey = false;
}

void tflicview::waitnextframe(void)
{
   do {
   } while (ticker < lasttimer+timerdiff  ); /* enddo */
   lasttimer = ticker;
}

byte tflicview::viewflc(void)
{ 
   //FILE *flc_test;

   //flc_test = fopen("brun.txt","wt");

     endit = false; 
     byte *chunkpart;
     long int chunksize;
     word chunktype;

     int unusedchunks =0;
 
     playspeed = 5;

     memcpy(&fliheader,flcmem ,sizeof(tfliheader));
     
     if ((fliheader.magic != 0xAF11) && (fliheader.magic != 0xAF12)) return 1;
     if ((fliheader.height > 400) || (fliheader.width > 640)) return 1;

     lasttimer = ticker;
     playspeed = 7;

     timerdiff = 80 / playspeed;

     do {
        long int pos = sizeof(tfliheader);
        for (int a = 0; a <= fliheader.frames - 1; a++) {
           memcpy(&frameheader, &flcmem[pos] ,sizeof(tframeheader));
           pos+= sizeof(tframeheader);

           chunkpart = (byte *) new( byte[frameheader.framesize - sizeof(tframeheader)] );

           memcpy(chunkpart, &flcmem[pos] , frameheader.framesize - sizeof(tframeheader) );
           pos+= frameheader.framesize - sizeof(tframeheader);

           if (frameheader.chunkcount > 0) { 
              int ccnt = 0;
              for (int b = 0; b <= frameheader.chunkcount - 1; b++) {
                 chunksize = ( (int *) &chunkpart[ccnt]) [0];
                 chunktype = ( (word *) &chunkpart[ccnt] ) [2];
                 int c=6 + ccnt;
                 //r_key();
                 switch (chunktype) {
                 case 11 :{ //FLI-COLOR
                        p1 = ( ( word *) &chunkpart[c])[0];
                        c += 2;
                        p6 = 0;
                        for (p2=0 ;p2 < p1 ;p2++ ) {
                           p3= chunkpart[c]; //skip
                           p4=chunkpart[c+1]; //change
                           if (p4 == 0) p4 = 256;
                           c+=2;
                           p6 += p3;
                           outp(0x3c8,p6);
                           for (p5 = 0; p5 < p4 ; p5++) {
                              outp(0x3c9,chunkpart[c] / 4);
                              outp(0x3c9,chunkpart[c+1] / 4);
                              outp(0x3c9,chunkpart[c+2] / 4);
                              c+=3;
                           } /* endfor */
                           p6+=p4;
                        } /* endfor */
                    }
                    break;
                 case 4 : {//FLC-COLOR
                                p1 = ( ( word *) &chunkpart[c])[0];
                                c += 2;
                                for (p2 = 0; p2 < p1; p2++) {
                                    p3= chunkpart[c]; //skip
                                    p4=chunkpart[c+1]; //change
                                    if (p4 == 0) p4 = 256; 
                                    c+=2;
                                    outp(0x3c8,p3);
                                    for (p5 = 0; p5 < p4 ; p5++) { 
                                        outp(0x3c9,chunkpart[c] / 4);
                                        outp(0x3c9,chunkpart[c+1] / 4);
                                        outp(0x3c9,chunkpart[c+2] / 4);
                                        c+=3;
                                    } 
                                } 
                              }
                    break;
                 case 7 :   { // FLC-DELTA
                       p1 = ( ( word *) &chunkpart[c])[0];
                       //fprintf(flc_test,"Chunkcount : %5.0d\n", p1);
                       c += 2;
                       y = flc_ypos;
                       waitnextframe();
                       for (int d = 0; d < p1; d++) { 
                           x = flc_xpos;
                           skipcnt = ( ( word *) &chunkpart[c])[0];
                           c += 2;
                           //fprintf(flc_test,"Check : C %2.0d X %2.0d Y %2.0d Ch %5.0i Cnt %5.0d\n",d,x,y,skipcnt,c);
                           if (skipcnt < 0) { 
                              y+= -skipcnt;
                              d--;
                           } 
                           else { 
                              for (p2 = 0; p2 < skipcnt; p2++) {
                                  p4 = chunkpart[c]; // Skip-Count
                                  szcnt = chunkpart[c+1]; // Size-Count
                                  //fprintf(flc_test,"P %5.0d SB %5.0i ",p4,szcnt);
                                  c+=2;
                                  x+=p4;
                                  if ( szcnt > 0 ) {
                                     //fprintf(flc_test,"V1 %5.0d ",c);
                                     //Ausgabe von (szcnt ) mal Farb-Werten die im Speicher folgen von position x ab
                                     for (pc=0;pc < szcnt * 2 ;pc+=2 ) {
                                           putpixel(x+pc,y,chunkpart[c+pc]);
                                           putpixel(x+pc+1,y,chunkpart[c+pc+1]);
                                     }
                                     x+=szcnt * 2;
                                     c+=szcnt * 2;
                                     //fprintf(flc_test,"V2 %5.0d\n",c);
                                  } 
                                  else { 
                                     szcnt = -szcnt; 
                                     p4 = chunkpart[c]; 
                                     p5 = chunkpart[c+1];
                                     c+=2;
                                     //fprintf(flc_test,"V3 %5.0d ",c);
                                     //Ausgabe von (szcnt ) mal p4&p5(Farbe)-Werten von position x ab
                                     for (pc=0;pc <szcnt *2 ;pc+=2 ) {
                                        putpixel(x+pc,y,p4);
                                        putpixel(x+pc+1,y,p5);
                                     }
                                     //line(x,y,x+(szcnt-1)*2,y,p4);
                                     x+=szcnt * 2; 
                                     //fprintf(flc_test,"V4 %5.0d\n",c);
                                  } 
                              } 
                              y++;
                           } 
                       } 
                    } 
                    break;
                 case 15 : {   // FLI_BRUN
                       y = flc_ypos;
                       waitnextframe();
                       do { 
                         p1 = chunkpart[c]; 
                         c++;
                         x = flc_xpos;
                          do { 
                             szcnt = chunkpart[c]; 
                             c++;
                             if (szcnt >= 0) { 
                                p3 = chunkpart[c]; 
                                c++;
                                //for (pc = x; pc < x + szcnt ;pc++ ) putpixel(pc,y,p3);
                                line(x, y, x+szcnt-1,y ,p3);
                                x+=szcnt;
                             } 
                             else { 
                                szcnt = -szcnt; 
                                for (pc=x;pc < x+ szcnt ;pc++ ) putpixel(pc,y,chunkpart[c+pc-x]);
                                x+=szcnt;
                                c+=szcnt;
                             } 
                          }  while (x < fliheader.width+flc_xpos);
                         y++;
                       }  while (y < fliheader.height+flc_ypos);
                    } 
                    break;
                 case 13 :  {
                       waitnextframe();
                       bar(x,y,x+fliheader.width-1,y+fliheader.height-1,0);
                    }
                    break;
                 default : unusedchunks++;
                   break;
                 } /* endswitch */
                 ccnt+=chunksize;
              }
           }
           free(chunkpart);
           if (keypress()) {
              if (endatkey) return 2;
              flkey = r_key();
              switch (flkey) {
              case ct_plus : {
                   playspeed++;
                   timerdiff = 80 / playspeed;
                 }
                 break;
              case ct_minus : {
                    if ( playspeed > 1 ) playspeed--;
                    timerdiff = 80 / playspeed;
                 }
                 break;
              default : return 2;
              } /* endswitch */
           } 
        }
     }
     while ( endit == false);
     //fclose(flc_test);
     return 0;
}


byte tflicview::viewconvflc(void)
{ 
     endit = false; 
     byte *chunkpart;
     long int chunksize;
     word chunktype;

     int unusedchunks =0;
 
     playspeed = 5;

     memcpy(&fliheader,flcmem ,sizeof(tfliheader));
     
     if ((fliheader.magic != 0xAF11) && (fliheader.magic != 0xAF12)) return 1;
     if ((fliheader.height > 400) || (fliheader.width > 640)) return 1;

     lasttimer = ticker;
     playspeed = 7;

     timerdiff = 80 / playspeed;

     do {
        long int pos = sizeof(tfliheader);
        for (int a = 0; a <= fliheader.frames - 1; a++) {
           memcpy(&frameheader, &flcmem[pos] ,sizeof(tframeheader));
           pos+= sizeof(tframeheader);

           chunkpart = (byte *) new( byte[frameheader.framesize - sizeof(tframeheader)] );

           memcpy(chunkpart, &flcmem[pos] , frameheader.framesize - sizeof(tframeheader) );
           pos+= frameheader.framesize - sizeof(tframeheader);

           if (frameheader.chunkcount > 0) { 
              int ccnt = 0;
              for (int b = 0; b <= frameheader.chunkcount - 1; b++) {
                 chunksize = ( (int *) &chunkpart[ccnt]) [0];
                 chunktype = ( (word *) &chunkpart[ccnt] ) [2];
                 int c=6 + ccnt;
                 //r_key();
                 switch (chunktype) {
                 case 11 :{ //FLI-COLOR
                        p1 = ( ( word *) &chunkpart[c])[0];
                        c += 2;
                        p6 = 0;
                        for (p2=0 ;p2 < p1 ;p2++ ) {
                           p3= chunkpart[c]; //skip
                           p4=chunkpart[c+1]; //change
                           if (p4 == 0) p4 = 256;
                           c+=2;
                           p6 += p3;
                           for (p5 = 0; p5 < p4 ; p5++) {
                              c+=3;
                           } /* endfor */
                           p6+=p4;
                        } /* endfor */
                    }
                    break;
                 case 4 : {//FLC-COLOR
                                p1 = ( ( word *) &chunkpart[c])[0];
                                c += 2;
                                for (p2 = 0; p2 < p1; p2++) {
                                    p3= chunkpart[c]; //skip
                                    p4=chunkpart[c+1]; //change
                                    if (p4 == 0) p4 = 256; 
                                    c+=2;
                                    for (p5 = 0; p5 < p4 ; p5++) { 
                                        c+=3;
                                    } 
                                } 
                              }
                    break;
                 case 7 :   { // FLC-DELTA
                       p1 = ( ( word *) &chunkpart[c])[0];
                       //fprintf(flc_test,"Chunkcount : %5.0d\n", p1);
                       c += 2;
                       y = flc_ypos;
                       waitnextframe();
                       for (int d = 0; d < p1; d++) { 
                           x = flc_xpos;
                           skipcnt = ( ( word *) &chunkpart[c])[0];
                           c += 2;
                           //fprintf(flc_test,"Check : C %2.0d X %2.0d Y %2.0d Ch %5.0i Cnt %5.0d\n",d,x,y,skipcnt,c);
                           if (skipcnt < 0) { 
                              y+= -skipcnt;
                              d--;
                           } 
                           else { 
                              for (p2 = 0; p2 < skipcnt; p2++) {
                                  p4 = chunkpart[c]; // Skip-Count
                                  szcnt = chunkpart[c+1]; // Size-Count
                                  //fprintf(flc_test,"P %5.0d SB %5.0i ",p4,szcnt);
                                  c+=2;
                                  x+=p4;
                                  if ( szcnt > 0 ) {
                                     //fprintf(flc_test,"V1 %5.0d ",c);
                                     //Ausgabe von (szcnt ) mal Farb-Werten die im Speicher folgen von position x ab
                                     for (pc=0;pc < szcnt * 2 ;pc+=2 ) {
                                           putpixel(x+pc,y,convtable[chunkpart[c+pc]]);
                                           putpixel(x+pc+1,y,convtable[chunkpart[c+pc+1]]);
                                     }
                                     x+=szcnt * 2;
                                     c+=szcnt * 2;
                                     //fprintf(flc_test,"V2 %5.0d\n",c);
                                  } 
                                  else { 
                                     szcnt = -szcnt; 
                                     p4 = chunkpart[c]; 
                                     p5 = chunkpart[c+1];
                                     c+=2;
                                     //fprintf(flc_test,"V3 %5.0d ",c);
                                     //Ausgabe von (szcnt ) mal p4&p5(Farbe)-Werten von position x ab
                                     for (pc=0;pc <szcnt *2 ;pc+=2 ) {
                                        putpixel(x+pc,y,convtable[p4]);
                                        putpixel(x+pc+1,y,convtable[p5]);
                                     }
                                     //line(x,y,x+(szcnt-1)*2,y,p4);
                                     x+=szcnt * 2; 
                                     //fprintf(flc_test,"V4 %5.0d\n",c);
                                  } 
                              } 
                              y++;
                           } 
                       } 
                    } 
                    break;
                 case 15 : {   // FLI_BRUN
                       y = flc_ypos;
                       waitnextframe();
                       do { 
                         p1 = chunkpart[c]; 
                         c++;
                         x = flc_xpos;
                          do { 
                             szcnt = chunkpart[c]; 
                             c++;
                             if (szcnt >= 0) { 
                                p3 = chunkpart[c]; 
                                c++;
                                //for (pc = x; pc < x + szcnt ;pc++ ) putpixel(pc,y,convtable[p3]);
                                line(x, y, x+szcnt-1,y ,convtable[p3]);
                                x+=szcnt;
                             } 
                             else { 
                                szcnt = -szcnt; 
                                for (pc=x;pc < x+ szcnt ;pc++ ) putpixel(pc,y,convtable[chunkpart[c+pc-x]]);
                                x+=szcnt;
                                c+=szcnt;
                             } 
                          }  while (x < fliheader.width+flc_xpos);
                         y++;
                       }  while (y < fliheader.height+flc_ypos);
                    } 
                    break;
                 case 13 :  {
                       waitnextframe();
                       bar(x,y,x+fliheader.width-1,y+fliheader.height-1,0);
                    }
                    break;
                 default : unusedchunks++;
                   break;
                 } /* endswitch */
                 ccnt+=chunksize;
              }
           }
           free(chunkpart);
           if (keypress()) {
              if (endatkey) return 2;
              flkey = r_key();
              switch (flkey) {
              case ct_plus : {
                   playspeed++;
                   timerdiff = 80 / playspeed;
                 }
                 break;
              case ct_minus : {
                    if ( playspeed > 1 ) playspeed--;
                    timerdiff = 80 / playspeed;
                 }
                 break;
              default : return 2;
              } /* endswitch */
           } 
           if (mouseparams.taste != 0 ) endit = true;
        }
     }
     while ( endit == false);
     //fclose(flc_test);
     return 0;
}


void tflicview::done(void)
{
   free( flcmem ); 
   setvgapalette256(pal);
}

void flicplayer(char *name,int x, int y)
{
   tflicview fv;

   fv.init();
   fv.loadflic(name);
   fv.flc_xpos = x;
   fv.flc_ypos = y;
   fv.viewflc();
   fv.done();
}


void showflic(char *name,int x, int y)
{
   tflicview fv;

   fv.init();
   fv.loadflic(name);
   fv.endatkey = true;
   fv.flc_xpos = x;
   fv.flc_ypos = y;
   fv.viewflc();
   fv.done();
}

void showconvflic(char *name,int x, int y)
{
   tflicview fv;

   fv.init();
   int loaderror = fv.loadflic(name);
   if (loaderror != 0 ) return;

   char param[20]; 

   strcpy(param,name);
   param[strlen(param)-3]=0;
   strcat(param,"dat");

   loaderror = fv.loadconvtable(param);
   if (loaderror != 0 ) return;
   setvgapalette256(fv.flcpal);
   fv.endatkey = true;
   fv.flc_xpos = x;
   fv.flc_ypos = y;
   fv.viewconvflc();
   fv.done();
}

void play_flc(int unitid,int prevx,int prevy)
{
   switch (unitid) {
      case 2 : showconvflic("art_prv.flc",prevx,prevy);
         break;
      case 32 : showconvflic("bb_prv.flc",prevx,prevy);
         break;
      case 7 : showconvflic("mars_prv.flc",prevx,prevy);
         break;
      case 3 : showconvflic("tnk1_prv.flc",prevx,prevy);
         break;
      case 14 : showconvflic("nim_prv.flc",prevx,prevy);
         break;
      default :  
        break;
      } /* endswitch */
}

