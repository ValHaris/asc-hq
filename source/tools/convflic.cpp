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

#include "ran.h"

#define freecolors 5 //color for free setting in palette

#define sqr(a) (a)*(a)

int maxdist = 0;
int farcolor = -1;
int actcolor = 0;
int shortdist = 200000;
byte used[256];

class nflicview : public tflicview {
                public :
                    byte getflicpal(void);
                    int getnearestflccolor(byte b);
                    void calcnewpalette(void);
};

nflicview flcv;

int calcdiff ( int r1, int r2, int g1, int g2, int b1, int b2 )
{
 //return cub( r1  - r2 ) + cub( g1 -  g2 ) + cub( b1 - b2 );
 return sqr( r1  - r2 ) + sqr( g1 -  g2 ) + sqr( b1 - b2 );
 //return abs( r1  - r2 ) + abs( g1 -  g2 ) + abs( b1 - b2 );
}


int nflicview::getnearestflccolor(byte b)
{
   int actdist;
   shortdist = 200000;
   int nextcol = -1;

   for (int j=0;j<256-freecolors+actcolor ;j++ ){
      actdist = calcdiff(pal[j][0],flcpal[b][0],pal[j][1],flcpal[b][1],pal[j][2],flcpal[b][2]);
      if (actdist < shortdist ) {
         shortdist = actdist;
         nextcol = j;
      } 
   } /* endfor */

   return nextcol;
}

void nflicview::calcnewpalette(void)
{
   byte ncolor;

   for (actcolor=0;actcolor < freecolors ;actcolor++ ) {
      maxdist = 0;
      farcolor = -1;
      for (int j=0 ;j < 256 ;j++ ) if ( used[j] == 1 ) {
         ncolor = getnearestflccolor(j);
         if (ncolor == -1 ) { 
            printf("No Color found !!!");
         } 
         convtable[j] = ncolor;
         if (shortdist > maxdist ) {
            maxdist = shortdist;
            farcolor = j;
         } 
      } /* endfor */
      //printf("Color %3.0d Distance : %6.0d \n",256-freecolors+actcolor,maxdist);
      pal[256-freecolors+actcolor][0] = flcpal[farcolor][0];
      pal[256-freecolors+actcolor][1] = flcpal[farcolor][1];
      pal[256-freecolors+actcolor][2] = flcpal[farcolor][2];
      convtable[farcolor] = 256-freecolors+actcolor;
   } /* endfor */
   for (int j=0 ;j < 256 ;j++ ) {
      ncolor = getnearestflccolor(j);
      if (ncolor == -1 ) { 
         printf("No Color found !!!");
      } 
      convtable[j] = ncolor;
      if (shortdist > maxdist ) {
         maxdist = shortdist;
         farcolor = j;
      } 
   } /* endfor */
}

byte nflicview::getflicpal(void)
{ 
     endit = false; 
     for (int t=0;t < 256 ;t++ ) 
          used[t]=0;
     byte *chunkpart;
     long int chunksize;
     word chunktype;

     int unusedchunks =0;
 
     flc_xpos=100;
     flc_ypos=100;
     playspeed = 5;

     memcpy(&fliheader,flcmem ,sizeof(tfliheader));
     
     if ((fliheader.magic != 0xAF11) && (fliheader.magic != 0xAF12)) return 1;
     if ((fliheader.height > 400) || (fliheader.width > 640)) return 1;

     lasttimer = ticker;
     playspeed = 7;

     timerdiff = 80 / playspeed;

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
              switch (chunktype) {
              case 11 :  { //FLI-COLOR
                     return 3;
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
                             if (endit) return 3;
                             p1 = ( ( word *) &chunkpart[c])[0];
                             c += 2;
                             for (p2 = 0; p2 < p1; p2++) {
                                 p3= chunkpart[c]; //skip
                                 p4=chunkpart[c+1]; //change
                                 if (p4 == 0) p4 = 256; 
                                 c+=2;
                                 for (p5 = 0; p5 < p4 ; p5++) { 
                                     //putc(getnearestcolor(chunkpart[c],chunkpart[c+1],chunkpart[c+2]),flcconv);
                                     flcpal[p5][0] = chunkpart[c] / 4;
                                     flcpal[p5][1] = chunkpart[c+1] / 4;
                                     flcpal[p5][2] = chunkpart[c+2] / 4;
                                     c+=3;
                                 } 
                             }
                             endit = true;
                           }
                 break;
                 case 7 :   { // FLC-DELTA
                       p1 = ( ( word *) &chunkpart[c])[0];
                       //fprintf(flc_test,"Chunkcount : %5.0d\n", p1);
                       c += 2;
                       y = flc_ypos;
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
                                           used[( chunkpart[c+pc] % 256 ) ]=1;
                                           used[ ( chunkpart[c+pc+1] % 256 ) ]=1;
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
                                     used[ ( p4 % 256 ) ]=1;
                                     used[ ( p5 % 256 ) ]=1;
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
                                used[ ( p3 % 256 ) ]=1;
                                x+=szcnt;
                             } 
                             else { 
                                szcnt = -szcnt; 
                                for (pc=x;pc < x+ szcnt ;pc++ ) used[ ( chunkpart[c+pc-x] % 256 ) ]=1;
                                x+=szcnt;
                                c+=szcnt;
                             } 
                          }  while (x < fliheader.width+flc_xpos);
                         y++;
                       }  while (y < fliheader.height+flc_ypos);
                    } 
                    break;
                 } /* endswitch */
                 ccnt+=chunksize;
             }
        }
        free(chunkpart);
     }
     return 0;
}


void         showused(void)
{ 
   int x=17;

   for ( char a = 0; a <= 15; a++) 
      for ( char b = 0; b <= 15; b++) { 
         if ( used[a*16+b] == 1 ) bar( x + a * 40, b * 20+5,a * 40 + 2*x,b * 20 + 15,red);
      }      
   wait();
} 


void         showpalette(dacpalette256   pal)
{ 
   bar ( 0, 0, 639, 479, black );
   int x=7;
   setvgapalette256(pal);

   for ( char a = 0; a <= 15; a++) 
      for ( char b = 0; b <= 15; b++) { 
         bar( x + a * 40, b * 20,a * 40 + 2*x,b * 20 + 20, a * 16 + b); 
      }      
   wait();
} 


void closesvgamode( void )
{
    closesvga();
    settextmode(3);
}    


void writeorgpal(void)
{
   for (int j=0;j < 256;j++ ) flcv.convtable[j]=j;

   flcv.loadflic("bb.flc");
   flcv.getflicpal();

   {
      tn_file_buf_stream stream("bb.dat",2);
      stream.writedata(flcv.flcpal, sizeof(flcv.flcpal));
      stream.writedata(flcv.convtable, sizeof(flcv.convtable));
   }
}

class t_carefor_containerstream {
       public:
         t_carefor_containerstream ( void ) { 
            if ( exist ( "main.dta" ) )
               containerstream = new tncontainerstream;
         };
         ~t_carefor_containerstream () {
            if ( containerstream ) {
               delete containerstream;
               containerstream = NULL;
            }
          }
       };


pfont load_font(char* name)
{
   tnfilestream stream ( name, 1 );
   return loadfont ( &stream );
}



main(int argc, char *argv[])
{
   pavailablemodes  avm;
   int              modenum8;
   int              i;

   printf( kgetstartupmessage() );

   t_carefor_containerstream cfc;

   schriften.smallarial = load_font("smalaril.FNT");
   schriften.large = load_font("USABLACK.FNT");
   schriften.arial8 = load_font("ARIAL8.FNT");
   schriften.smallsystem = load_font("msystem.fnt");

   filename[0] = 0;
   
   initmisc ();

    avm = searchformode ( 640, 480, 8 );
    if (avm->num > 0) {
      for (i=0;i<avm->num ;i++ ) {
         printf("8 Bit mode available: %x        X: %d   Y: %d \n", avm->mode[i].num, avm->mode[i].x, avm->mode[i].y);
         modenum8 = avm->mode[i].num;
      } /* endfor */
 
      printf("Initializing ... \n");
      initsvga( modenum8 );
      atexit( closesvgamode );      
      initspfst();
     }
     else {
        printf("8 Bit mode nicht available");
        return 1;
     }

   free ( avm );

   virtualscreenbuf.init();

   flcv.loadflic(argv[1]);

   char param[20]; 
   strcpy(param,argv[1]);
   param[strlen(param)-3]=0;
   strcat(param,"dat");

   {
      tnfilestream stream("palette.pal",1);
      stream.readdata(pal, sizeof(pal)); 
   }

   showpalette(pal);

   showtext2("converting !",100,100);

   flcv.getflicpal();

   showpalette(flcv.flcpal);
   showused();

   flcv.calcnewpalette();

   showpalette(pal);

    {
      tn_file_buf_stream stream("bb.dat",2);
      stream.writedata(pal, sizeof(pal));
      stream.writedata(flcv.convtable, sizeof(flcv.convtable));
   }
 
   return 0;
}
