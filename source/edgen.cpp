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
#include "edgen.h"

#define layercount 5
#define centerlayer 65000
#define maxoverwrite 5

  class tmapgenerator : public tdialogbox {
        public :
               byte action;
               void init(void);
               void showmap(void);
               void generatefinishmap(void); // Bodensch„tze werden nicht draufmontiert, damit keine Farbverf„lschung
               void montlayer(byte layer);
               void showmontlayer(word sx, word sy, word barsize);
               void setpfield(int number);
               void setmaterial(int number);
               void setfuel(int number);
               void resetmapfield(void);
               void addcoast(void);
               void setmap(void);
               boolean checkland(int x, int y);
               virtual void run(void);
               virtual void buttonpressed(byte  id);
       protected :
               boolean flip,showland,showdesert,showforest,showmaterial,showfuel,correctvalues,calculatevalues;
               boolean initialized[ layercount ];
               byte *constructionlayer;
               byte layer,actlayer;
               byte barsize,maxbarsize;
               byte whereland;
               word tileval,res,pres,overwritecolorcount;
               tplasma plasma;
               pmemoryblock mblocks[ layercount ];
               pterraintype btyp[numofbdts];
               int overwritecolor[maxoverwrite];
               pfield pf;
               };

const char* clayer[layercount]  = {"Land", "Forest", "Desert","Material","Fuel"};


void         tmapgenerator::init(void)
{ 
  word         w;
  char          s1[100],s2[100];

   tdialogbox::init();
   action = 0;
   title = "Map generator";
   x1 = 10;
   xsize = 620;
   y1 = 10;
   ysize = 460;

   w = (xsize - 160) / 2;
   windowstyle = windowstyle ^ dlg_in3d; 

   for (int i=0;i < layercount ;i++ ) initialized[i] =false;

   plasma.maxx = actmap->xsize * 2;
   plasma.maxy = actmap->ysize;
   flip = plasma.flip;
   res = 10;
   if (res > plasma.maxx / 5) res = plasma.maxx / 5;
   pres = 300;
   tileval = 0;
   actlayer = layer = 0;
 

   byte xmax = 430 / plasma.maxx;
   byte ymax = 340 / plasma.maxy;

   if (xmax > ymax ) maxbarsize = ymax;
   else maxbarsize = xmax;

   barsize = maxbarsize;

   constructionlayer = new ( byte[plasma.maxx * plasma.maxy ]); // =blockcount

   addbutton("",480,80,510,100,0,1,7,true);
   addkey(7,ct_down);
   addbutton("",530,80,560,100,0,1,8,true);
   addkey(8,ct_up);

   addbutton("~G~enerate new",460,115,580,135,0,1,1,true);

   addbutton("~R~esolution",460,155,580,175,2,1,3,true);
   addeingabe(3,&res,1,plasma.maxx / 5);

   addbutton("~P~recission",460,195,580,215,2,1,4,true);
   addeingabe(4,&pres,0,30000);
 
   addbutton("~F~lip map",480,225,580,240,3,1,9,true);
   addeingabe(9,&flip,0,lightgray);

   addbutton("",460,295,580,315,2,1,2,true);
   addeingabe(2,&tileval,0,pres);

   addbutton("-",480,330,510,350,0,1,5,true);
   addkey(5,ct_minus);
   addbutton("+",530,330,560,350,0,1,6,true);
   addkey(6,ct_plus);
   
   addbutton("",480,370,510,390,0,1,11,true);
   addkey(11,ct_left);
   addbutton("",530,370,560,390,0,1,12,true);
   addkey(12,ct_right);

  correctvalues = true;
  addbutton("Co~r~rect",520,410,600,420,3,1,50,true);
  addeingabe(50,&correctvalues,0,lightgray);

  calculatevalues = true;
  addbutton("Ca~l~culate",520,430,600,440,3,1,51,true);
  addeingabe(51,&calculatevalues,0,lightgray);

   if ( maxbarsize >=1 ) addbutton("1",345,395,360,410,0,1,13,true);
   addkey(13,ct_1);
   if ( maxbarsize >=2 ) addbutton("2",365,395,380,410,0,1,14,true);
   addkey(14,ct_2);
   if ( maxbarsize >=3 ) addbutton("3",385,395,400,410,0,1,15,true);
   addkey(15,ct_3);
   if ( maxbarsize >=4 ) addbutton("4",405,395,420,410,0,1,16,true);
   addkey(16,ct_4);
   if ( maxbarsize >=5 ) addbutton("5",425,395,440,410,0,1,17,true);
   addkey(17,ct_5);

   addbutton ("~x~sym", 455,20,500,30, 3, 0, 31, true );
   addeingabe ( 31, &plasma.xsymm, 0, lightgray );

   addbutton ("~y~sym", 510,20,560,30, 3, 0, 32, true );
   addeingabe ( 32, &plasma.ysymm, 0, lightgray );

   addbutton("~A~ccept Map",20,ysize - 40,20 + w,ysize - 10,0,1,19,true);
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,20,true);

   buildgraphics(); 

   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = red;
   activefontsettings.background = lightgray;
   activefontsettings.length = 70;
   activefontsettings.justify = lefttext;
   
   showtext2("Layer",x1+460,y1+52);
   rahmen(true,x1 + 500,y1 + 50,x1 + 580,y1 + 70);
   showtext2(clayer[layer],x1+505,y1+52);

   showtext2("Limit",x1+460,y1+267);
   rahmen(true,x1 + 500,y1 + 265,x1 + 580,y1 + 285);

   strcpy(s1,"XSize : ");
   strcpy(s2,"YSize : ");
   showtext2(strcat(s1,strrr(plasma.maxx)),x1+10,y1+10);
   showtext2(strcat(s2,strrr(plasma.maxy)),x1+120,y1+10);

   putspriteimage(x1 + 487,y1 + 83,icons.pfeil2[6]);
   putspriteimage(x1 + 537,y1 + 83,icons.pfeil2[2]);

   putspriteimage(x1 + 487,y1 + 373,icons.pfeil2[6]);
   putspriteimage(x1 + 537,y1 + 373,icons.pfeil2[2]);

   rahmen(true,x1 + 450,y1 + 40,x1 + 590,y1 + 400);
   rahmen(true,x1 + 10,y1 + 40,x1 + 440,y1 + 390);

   //activefontsettings.length = 170;
   //showtext2("Press F1 for Help !",200,190);

   //showmemory(); 

   mousevisible(true); 
} 


void tmapgenerator::montlayer(byte layer)
{
   register i,j;
   boolean found;
 
   if ( overwritecolorcount == 0 ){
      for (i=0;i<plasma.blockcount;i++ ) {
         found = false;
         j=0;
         do {
            if (mblocks[layer]->mempointer[i] <= mblocks[layer]->tileval[j] ) { 
               if ( mblocks[layer]->color[j] != ctransparent ) constructionlayer[i] =  mblocks[layer]->color[j];
               found = true;
            }
            else j++;
         } while ( ( found == false ) && (j < mblocks[layer]->tilevalcount ) ); /* enddo */
         if ( (j == mblocks[layer]->tilevalcount ) && ( mblocks[layer]->color[mblocks[layer]->tilevalcount] != ctransparent ) )
               constructionlayer[i] =  mblocks[layer]->color[mblocks[layer]->tilevalcount];
      } /* endfor */
   } else {
      for (i=0;i<plasma.blockcount;i++ ) {
         for (int k=0;k<overwritecolorcount;k++ ) {
            if (constructionlayer[i] == overwritecolor[k]) {
               found = false;
               j=0;
               do {
                  if (mblocks[layer]->mempointer[i] <= mblocks[layer]->tileval[j] ) { 
                     constructionlayer[i] =  mblocks[layer]->color[j];
                     found = true;
                  }
                  else j++;
               } while ( ( found == false ) && (j < mblocks[layer]->tilevalcount ) ); /* enddo */
               if (j == mblocks[layer]->tilevalcount ) constructionlayer[i] =  mblocks[layer]->color[mblocks[layer]->tilevalcount];
            }
         } /* endfor */
      } /* endfor */
   } /* endif */
}

/*void setmem(word x, word y,word color)
{
   [ x + (y * maxx ) ] = color;
}

void membar(word x1 ,word y1 ,word x2 ,word y2, word color )
{  word exc;
   if (x1 >x2) {
      exc = x1;
      x1 = x2;
      x2 = exc;
   } 

   if (y1 >y2) {
      exc = y1 ;
      y1 = y2 ;
      y2 = exc ;
   } 

   for (int j=y1;j<=y2;j++ ) {
      for (int i=x1;i<=x2;i++) {
          //memblock->mempointer[ i + (j * maxx ) ] = color;
          setmem(i,j,color);
      } 
   } 
}*/


void tmapgenerator::showmontlayer(word sx, word sy, word barsize)
{
   register i;

   word prevx,prevy;
 
   //prevx = x1 + 15;
   //prevy = y1 + 45;
 
   prevx = x1 + 15 + (430 - plasma.maxx * barsize )  / 2;
   prevy = y1 + 45 + (340 - plasma.maxy * barsize )  / 2;

   if (sx == centerlayer) sx = prevx;
   if (sy == centerlayer) sy = prevy;

   /*byte *constlayer2 = new ( byte[plasma.maxx * plasma.maxy + 4]); // =blockcount + 2 * word fr x/y-size
   (word *) constlayer2[0] = plasma.maxx;
   (word *) constlayer2[1] = plasma.maxy;

   if (flip == true) {
      for (i=plasma.blockcount-1;i>=0 ;i-- ) {
         if ( barsize <= 1 ) putpixel(x,y, constructionlayer[i]);
         else bar(x,y,x+barsize-1,y+barsize-1, constructionlayer[i]);
         x += barsize;
         if (x >= sx + (plasma.maxx) * barsize ) {
            x = sx;
            y += barsize;
         } 
      } 
   } else {
      for (i=0;i<plasma.blockcount;i++ ) {
         if ( barsize <= 1 ) putpixel(x,y, constructionlayer[i]);
         else bar(x,y,x+barsize-1,y+barsize-1, constructionlayer[i]);
         x += barsize;
         if (x >= sx + (plasma.maxx) * barsize ) {
            x = sx;
            y += barsize;
         } 
      } 
   } */

   
   register x,y;

   x = sx;
   y = sy;

   if (flip == true) {
      for (i=plasma.blockcount-1;i>=0 ;i-- ) {
         if ( barsize <= 1 ) putpixel(x,y, constructionlayer[i]);
         else bar(x,y,x+barsize-1,y+barsize-1, constructionlayer[i]);
         x += barsize;
         if (x >= sx + (plasma.maxx) * barsize ) {
            x = sx;
            y += barsize;
         } 
      } 
   } else {
      for (i=0;i<plasma.blockcount;i++ ) {
         if ( barsize <= 1 ) putpixel(x,y, constructionlayer[i]);
         else bar(x,y,x+barsize-1,y+barsize-1, constructionlayer[i]);
         x += barsize;
         if (x >= sx + (plasma.maxx) * barsize ) {
            x = sx;
            y += barsize;
         } 
      } 
   } 
}

void tmapgenerator::generatefinishmap(void)
{ // layers eintragen, die terrain beeinflussen !
   overwritecolorcount = 0;
   if ( (showland == true ) && (initialized[0] == true ) ) montlayer(0);
   else memset(constructionlayer,lightgray,plasma.blockcount);

   overwritecolorcount = 2;
   overwritecolor[0] = mblocks[clland]->color[4];
   overwritecolor[1] = lightgray;
   if ( (showforest == true ) && (initialized[1] == true ) ) montlayer(1);

   overwritecolorcount = 2;
   overwritecolor[0] = mblocks[clland]->color[4];
   overwritecolor[1] = lightgray;
   if ( (showdesert == true ) && (initialized[2] == true ) ) montlayer(2);
}

// Karte wird aufgrund von Farben bernommen !!!
// Wenn bodensch„tze in der fertigen karte eingezeichnet werden, dann gibt das Fehler,
// weil dann die Farben fr die terrain nicht mehr stimmen !!!



void tmapgenerator::showmap(void)
{ // layers, die den bodentyp nicht beeinflussen
    generatefinishmap();

   overwritecolorcount = 0;
   if ( (showmaterial == true ) && (initialized[3] == true ) ) montlayer(3);

   overwritecolorcount = 0;
   if ( (showfuel == true ) && (initialized[4] == true ) ) montlayer(4);
 
   showmontlayer(centerlayer,centerlayer,barsize);
}

boolean tmapgenerator::checkland(int x, int y)
{
   if ( ! ( getfield(x,y)->typ->art & cbwater ) ) return true;
   else return false;
}

void tmapgenerator::addcoast(void)
{
   pfield pf;
   const int id[2]={42,83};
 
   for (int i=0;i<2;i++ ) {
      if (id[i] != 0 ) {
         btyp[i] = getterraintype_forid(id[i]);
         if (btyp[i] == NULL) btyp[i] = getterraintype_forpos(0);
      } 
   } /* endfor */



   for (int y=0;y<actmap->ysize ;y++ ) {
      for (int x=0;x<actmap->xsize ;x++) {
         pf = getfield(x,y);
         whereland = 0;
         if ( checkland(x,y) == false ){
            if ( (x >1 ) && (y >1 ) && (y < actmap->ysize-2 ) && ( x < actmap->xsize -2) ){ // nicht am Rand
               if ( checkland(x+1, y) ) whereland |= 4; // 2
               if ( checkland(x-1, y) ) whereland |= 64; // 6
               if ( checkland(x, y+2) ) whereland |= 16; // 4
               if ( checkland(x, y-2) ) whereland |= 1; // 0
               if (y & 1) {
                  if ( checkland(x-1, y-1) ) whereland |= 128; // 7
                  if ( checkland(x, y-1) ) whereland |= 2; // 1
                  if ( checkland(x, y+1) ) whereland |= 8; // 3
                  if ( checkland(x-1, y+1) ) whereland |= 32; // 5
               } else {
                  if ( checkland(x, y-1) ) whereland |= 128; // 7
                  if ( checkland(x+1,y-1) ) whereland |= 2; // 1
                  if ( checkland(x+1, y+1) ) whereland |= 8; // 3
                  if ( checkland(x, y+1) ) whereland |= 32; // 5
               } /* endif */
               if (whereland & 1) {
                  pf->typ = btyp[0]->weather[0];
                  pf->direction = 0;
               } 
               if (whereland & 2) {
                  pf->typ = btyp[1]->weather[0];
                  pf->direction = 0;
               } 
               if (whereland & 4) {
                  pf->typ = btyp[0]->weather[0];
                  pf->direction = 1;
               } 
               if (whereland & 8) {
                  pf->typ = btyp[1]->weather[0];
                  pf->direction = 1;
               } 
               if (whereland & 16) {
                  pf->typ = btyp[0]->weather[0];
                  pf->direction = 2;
               } 
               if (whereland & 32) {
                  pf->typ = btyp[1]->weather[0];
                  pf->direction = 2;
               } 
               if (whereland & 64) {
                  pf->typ = btyp[0]->weather[0];
                  pf->direction = 3;
               } 
               if (whereland & 128) {
                  pf->typ = btyp[1]->weather[0];
                  pf->direction = 3;
               } 
            } 
         } /* endfor */
      } /* endfor */
   } /* endfor */
}

void tmapgenerator::setpfield(int number)
{
   switch ( constructionlayer[ number ] ) {
   case cwater : pf->typ = btyp[0]->weather[0];
      break;
   case cland   : pf->typ = btyp[1]->weather[0];
      break;
   case cmount : pf->typ = btyp[2]->weather[0];
      break;
   case cforest : pf->typ = btyp[3]->weather[0];
      break;
   case cdesert : pf->typ = btyp[4]->weather[0];
      break;
   case cshallowwater : pf->typ = btyp[5]->weather[0];
      break;
   case cdeepwater : pf->typ = btyp[6]->weather[0];
      break;
   case cverydeepwater : pf->typ = btyp[7]->weather[0];
      break;
   default: pf->typ = btyp[1]->weather[0];
     break;
   } /* endswitch */
}

int matdiv[3];
int fueldiv[3];

#define enhance 1000

void tmapgenerator::setmaterial(int number)
{  
   for (int i =mblocks[clmaterial]->tilevalcount-1;i >=0 ;i-- ) {
      if (mblocks[clmaterial]->mempointer[number] > mblocks[clmaterial]->tileval[i] )  {
         int test = ( ( (mblocks[clmaterial]->mempointer[number] - mblocks[clmaterial]->tileval[i] ) * matdiv[i] / enhance ) + (i * 80)) * 14 / 10;
         if ( test > 255 ) pf->material = 255;
         else pf->material = test;
         return;// Korrektur um x 1,4 => *14 / 10
      }
   } /* endfor */
}

void tmapgenerator::setfuel(int number)
{
   for (int i =mblocks[clfuel]->tilevalcount-1;i >=0   ;i-- ) {
      if (mblocks[clfuel]->mempointer[number] > mblocks[clfuel]->tileval[i] ){
         int test = ( ( (mblocks[clfuel]->mempointer[number] - mblocks[clfuel]->tileval[i] ) * fueldiv[i] / enhance ) + (i * 80) ) * 14 / 10;
         if ( test>255 ) pf->fuel =255;
         else pf->fuel = test;
         return;// Korrektur um x 1,4 => *14 / 10
      }
   } /* endfor */
}


int terrain_object_ids[numofbdts][2][2] = {{{ 299, -1 }, { 307, -1 }},  // water
                                           {{ 28 , -1 }, { 38 , -1 }},  // land
                                           {{ 30 , -1 }, { 40 , -1 }},  // mountain
                                           {{ 30 , -1 }, { 40 , -1 }},  // forest
                                           {{ 34 , -1 }, { 35 , -1 }},  // desert
                                           {{ 299, -1 }, { 307, -1 }},  // shallow water
                                           {{ 301, -1 }, { 308, -1 }},  // deep water
                                           {{ 302, -1 }, { 309, -1 }},  // very deep water
                                           {{ 30 , -1 }, { 40 , -1 }},  // ??
                                           {{ 30 , -1 }, { 40 , -1 }},  // ??
                                           {{ 30 , -1 }, { 40 , -1 }},  // ??
                                           {{ 30 , -1 }, { 40 , -1 }}};   // ??
   
void tmapgenerator::setmap(void)
{
   const int id[numofbdts]={60,18,82,20,100,61,63,71,0,0,0};
   int i,j;

   generatefinishmap();
 
   #ifdef HEXAGON
   int set = 2 - choice_dlg("Choose terrain set","dark","light") ;
   for (i=0;i<numofbdts ;i++ ) {
       btyp[i] = getterraintype_forid( terrain_object_ids[i][set][0] );
       if ( !btyp[i] ) 
          btyp[i] = getterraintype_forpos(0);
   }

   #else
      for (i=0;i<numofbdts ;i++ ) {
         if (id[i] != 0 ) {
            btyp[i] = getterraintype_forid(id[i]);
            if (btyp[i] == NULL) btyp[i] = getterraintype_forpos(0);
         } 
      } /* endfor */
   #endif

   if (initialized[clmaterial] == true ) {
      matdiv[0] = 81 * enhance / ( mblocks[clmaterial]->tileval[1] -mblocks[clmaterial]->tileval[0] +1 ) ;
      matdiv[1] = 80 * enhance / ( mblocks[clmaterial]->tileval[2] - mblocks[clmaterial]->tileval[1] +1 );
      matdiv[2] = 95 * enhance / ( mblocks[clmaterial]->actpres - mblocks[clmaterial]->tileval[1] +1);
   } // +1 damit keine 0 vorkommen kann

   if (initialized[clfuel] == true ) {
      fueldiv[0] = 81 * enhance / ( mblocks[clfuel]->tileval[1] -mblocks[clfuel]->tileval[0] +1);
      fueldiv[1] = 80 * enhance / ( mblocks[clfuel]->tileval[2] - mblocks[clfuel]->tileval[1] +1);
      fueldiv[2] = 95 * enhance / ( mblocks[clfuel]->actpres - mblocks[clfuel]->tileval[1] +1);
   }

   if ( ( actmap->xsize * 2 == plasma.maxx ) && (actmap->ysize == plasma.maxy ) ){
      mapsaved = false;
      if (flip == true) {
         for (i=0;i< ( plasma.maxx / 2 ) ;i++ ) {
            for (j=0;j<plasma.maxy;j++ ) {
               pf = &actmap->field[ ( plasma.maxx / 2 - i - 1 ) + ( ( plasma.maxy - j -1) * plasma.maxx / 2 ) ];
               pf->direction = 0;
               int plasmalayernr = ( i * 2 + ( j & 1 ) ) + ( j * plasma.maxx );
               setpfield ( plasmalayernr ) ;
               pf->setparams();
               if (initialized[clmaterial] == true ) setmaterial ( plasmalayernr );
               if (initialized[clfuel] ==true ) setfuel ( plasmalayernr );
               if (pf->vehicle != NULL)
                  if ( terrainaccessible(pf,pf->vehicle) == false ) removevehicle(&pf->vehicle); 
            } /* endfor */
         } /* endfor */
      } else {
         for (i=0;i< ( plasma.maxx / 2 ) ;i++ ) {
            for (j=0;j<plasma.maxy;j++ ) {
               pf = &actmap->field[i + ( j * plasma.maxx / 2 ) ];
               pf->direction = 0;
               int plasmalayernr = ( i * 2 + ( j & 1 ) ) + ( j * plasma.maxx );
               setpfield ( plasmalayernr ) ;
               pf->setparams();
               if (initialized[clmaterial] == true ) setmaterial ( plasmalayernr );
               if (initialized[clfuel] ==true ) setfuel ( plasmalayernr );
               if (pf->vehicle != NULL)
                  if ( terrainaccessible(pf,pf->vehicle) == false ) removevehicle(&pf->vehicle); 
            } /* endfor */
         } /* endfor */
      } /* endif */
   }/* else {
      displaymessage2("Map genaration failure !!! Sizes do not match !");
      r_key();
      removemessage();
   }*/

   //addcoast();
}

void tmapgenerator::resetmapfield(void)
{
   bar(x1 + 12,y1 + 42,x1 + 438,y1 + 388,lightgray);
}

void         tmapgenerator::run(void)
{ 
   do { 
      tdialogbox::run();
      if (taste == ct_f1) help(1010);
   }  while (!((taste == ct_esc) || (action >= 2))); 
   if (action == 2) setmap();
   for (int i=0;i<layercount ;i++ ) {
      if (initialized[i]) { 
         asc_free(mblocks[i]->mempointer);
         asc_free(mblocks[i]);
      }
   } /* endfor */
} 


void         tmapgenerator::buttonpressed(byte id)
{ 

   switch (id ) {
   case 1: {
         if (initialized[layer] == false ) {
            plasma.creatememblock();
            mblocks[layer] = plasma.memblock;
            initialized[layer] = true;
            switch (layer) {
            case clland : {
                   plasma.memblock->color[0] = cverydeepwater;
                   plasma.memblock->color[1] = cdeepwater;
                   plasma.memblock->color[2] = cwater;
                   plasma.memblock->color[3] = cshallowwater;
                   plasma.memblock->color[4] = cland;
                   plasma.memblock->color[5] = cmount;
                   plasma.memblock->tilevalcount = 5; // {anzahl Values -1 ( immer h”chster Wert in der [-Klammer) Montlayer „ndern !!!}
                   strcpy(plasma.memblock->bordername[0],"Deep-water");
                   strcpy(plasma.memblock->bordername[1],"Medium-water");
                   strcpy(plasma.memblock->bordername[2],"Shallow-water");
                   strcpy(plasma.memblock->bordername[3],"Land");
                   strcpy(plasma.memblock->bordername[4],"Mountain");
                   showland=true;
                   addbutton("~M~ap",10,397,50,405,3,1,21,true);
                   addeingabe(21,&showland,0,lightgray);
                   enablebutton(21);
                   barsize = maxbarsize;
               }
               break;
            case clforest : {
                   plasma.memblock->color[0] = cland;
                   plasma.memblock->color[1] = cforest;
                   plasma.memblock->tilevalcount = 1;
                   strcpy(plasma.memblock->bordername[0],"Forest");
                   showforest = true;
                   addbutton("~F~orest",60,397,120,405,3,1,22,true); //maximaler x-wert 340 -> barsize-buttons
                   addeingabe(22,&showforest,0,lightgray);
                   enablebutton(22);
               }
               break;
             case cldesert : {
                   plasma.memblock->color[0] = cland;
                   plasma.memblock->color[1] = cdesert;
                   plasma.memblock->tilevalcount = 1;
                   strcpy(plasma.memblock->bordername[0],"Desert");
                   showdesert=true;
                   addbutton("~D~esert",130,397,190,405,3,1,23,true);
                   addeingabe(23,&showdesert,0,lightgray);
                   enablebutton(23);
               }
               break;
            case clmaterial: {
                   plasma.memblock->color[0] = ctransparent;
                   plasma.memblock->color[1] = cfewmaterial;
                   plasma.memblock->color[2] = cmediummaterial;
                   plasma.memblock->color[3] = cmuchmaterial;
                   plasma.memblock->tilevalcount = 3;
                   strcpy(plasma.memblock->bordername[0],"Few Material");
                   strcpy(plasma.memblock->bordername[1],"Medium Material");
                   strcpy(plasma.memblock->bordername[2],"Much Material");
                   showmaterial=true;
                   addbutton("~M~aterial",200,397,260,405,3,1,24,true);
                   addeingabe(24,&showmaterial,0,lightgray);
                   enablebutton(24);
               }
               break;
            case clfuel: {
                   plasma.memblock->color[0] = ctransparent;
                   plasma.memblock->color[1] = cfewfuel;
                   plasma.memblock->color[2] = cmediumfuel;
                   plasma.memblock->color[3] = cmuchfuel;
                   plasma.memblock->tilevalcount = 3;
                   strcpy(plasma.memblock->bordername[0],"Few Fuel");
                   strcpy(plasma.memblock->bordername[1],"Medium Fuel");
                   strcpy(plasma.memblock->bordername[2],"Much Fuel");
                   showfuel=true;
                   addbutton("~F~uel",270,397,330,405,3,1,25,true);
                   addeingabe(25,&showfuel,0,lightgray);
                   enablebutton(25);
               }
               break;
            } /* endswitch */

            plasma.memblock->actpres = 0; // Damit Tilevalues neu gesetzt werden
            activefontsettings.font = schriften.smallarial;
            activefontsettings.color = red;
            activefontsettings.background = lightgray;
            activefontsettings.length = 70;
            bar(x1 + 501,y1 + 266,x1 + 579,y1 + 284,lightgray);
            showtext2(plasma.memblock->bordername[plasma.memblock->acttile],x1+505,y1+267);
            tileval = plasma.memblock->tileval[plasma.memblock->acttile];
            enablebutton(2);
         }          
         plasma.memblock->maxset = pres;
         plasma.memblock->res = res;
         if (plasma.memblock->actpres != pres ) {
            plasma.generateplasma(true);
            tileval = plasma.memblock->tileval[plasma.memblock->acttile];
            enablebutton(2);
            addeingabe(2,&tileval,0,pres);
         } 
         else plasma.generateplasma(false);
         plasma.memblock->actpres = pres;
         showmap();
      }
      break;
   case 2 : if (initialized[layer] == true ) {
          plasma.memblock->tileval[plasma.memblock->acttile] = tileval;
          int i;
          if (correctvalues == true ) {
             for (i=0 ;i < plasma.memblock->acttile ;i++) {
                if (plasma.memblock->tileval[i]>plasma.memblock->tileval[plasma.memblock->acttile]) 
                   plasma.memblock->tileval[i] = plasma.memblock->tileval[plasma.memblock->acttile];
             } /* endfor */
             for (i=plasma.memblock->acttile+1;i < plasma.memblock->tilevalcount;i++ ) {
                if (plasma.memblock->tileval[i]<plasma.memblock->tileval[plasma.memblock->acttile])
                   plasma.memblock->tileval[i] = plasma.memblock->tileval[plasma.memblock->acttile];
             } /* endfor */
          } 
          if ( (calculatevalues == true ) && (plasma.memblock->acttile < plasma.memblock->tilevalcount) ) {
             int div =plasma.memblock->maxset - plasma.memblock->tileval[plasma.memblock->acttile];
             int count = plasma.memblock->tilevalcount - plasma.memblock->acttile;
             int add = div / count;
             if (add > 0) {
                for (i=plasma.memblock->acttile+1;i < plasma.memblock->tilevalcount;i++ ) {
                   plasma.memblock->tileval[i] = plasma.memblock->tileval[plasma.memblock->acttile] + add * ( i - plasma.memblock->acttile);
                } /* endfor */
             } 
          } 
          showmap();
          }
      break;
   case 6 : if (initialized[layer] == true ){
          tileval++;
          enablebutton(2);
          plasma.memblock->tileval[plasma.memblock->acttile] = tileval;
          showmap();
          }
      break;
   case 5 : if (initialized[layer] == true ){
          tileval--;
          enablebutton(2);
          plasma.memblock->tileval[plasma.memblock->acttile] = tileval;
          showmap();
      }
      break;
   case 11 :
   case 12 : if ( (initialized[layer] == true ) && ( plasma.memblock->tilevalcount >1 ) ) {
          if (id == 11) {
             if (plasma.memblock->acttile > 0) plasma.memblock->acttile--;
             else plasma.memblock->acttile = plasma.memblock->tilevalcount -1;
          } else {
             if (plasma.memblock->acttile < plasma.memblock->tilevalcount -1 ) plasma.memblock->acttile++;
             else plasma.memblock->acttile = 0;
          } /* endif */
          activefontsettings.font = schriften.smallarial;
          activefontsettings.color = red;
          activefontsettings.background = lightgray;
          activefontsettings.length = 70;
          bar(x1 + 501,y1 + 266,x1 + 579,y1 + 284,lightgray);
          showtext2(plasma.memblock->bordername[plasma.memblock->acttile],x1+505,y1+267);
          tileval = plasma.memblock->tileval[plasma.memblock->acttile];
          enablebutton(2);
      }
      break;
   case 7 : 
   case 8 : {
          if (id == 7) {
             if (layer > 0) layer--;
             else layer = layercount -1;
          } else {
             if (layer < layercount -1 ) layer++;
             else layer = 0;
          } /* endif */
          plasma.memblock = mblocks[layer];
          activefontsettings.font = schriften.smallarial;
          activefontsettings.color = red;
          activefontsettings.background = lightgray;
          activefontsettings.length = 70;
          bar(x1 + 501,y1 + 51,x1 + 579,y1 + 69,lightgray);
          showtext2(clayer[layer],x1+505,y1+52);

          bar(x1 + 501,y1 + 266,x1 + 579,y1 + 284,lightgray);

          if (initialized[layer] == true ) {
              pres = plasma.memblock->maxset;
              tileval = plasma.memblock->tileval[plasma.memblock->acttile];
              res = plasma.memblock->res;
              addeingabe(2,&tileval,0,pres);
              enablebutton(2);
              enablebutton(3);
              enablebutton(4);
 
              plasma.memblock->actpres = 0; // Damit Tilevalues neu gesetzt werden
              activefontsettings.font = schriften.smallarial;
              activefontsettings.color = red;
              activefontsettings.background = lightgray;
              activefontsettings.length = 70;
              showtext2(plasma.memblock->bordername[plasma.memblock->acttile],x1+505,y1+267);
              tileval = plasma.memblock->tileval[plasma.memblock->acttile];
              enablebutton(2);
          }
      }
      break;
   case 9 : if (initialized[layer] == true ){
         plasma.flip = flip;
         showmap();
      }
      break;
   case 13 :
   case 14 :
   case 15 :
   case 16 :
   case 17 : if (id -12 != barsize ) {
         barsize = id -12;
         resetmapfield();
         showmontlayer(centerlayer,centerlayer,barsize);
      }
      break;
   case 21 : 
   case 22 :
   case 23 : 
   case 24 :
   case 25 : {
          resetmapfield();
          showmap();
      }
      break;
   case 19: action = 2;
      break;
   case 20: action = 3;
      break;
   } /* endswitch */

      
} 

int         mapgenerator(void)
{ 
  tmapgenerator      mg;

   mg.init();
   mg.run();
   mg.done();
   return 0;
} 

word random(word max)
{  
   if (max <= 1 ) return 0;
   word div = RAND_MAX;
   div /= (max -1);
   word r = rand();
   return r / (div + 1);
}

tplasma::tplasma(void)
{
   maxvalue=300;
   maxx = 200;
   maxy = 150;
   flip=false;
   xsymm = 0;
   ysymm = 0;

}

int tplasma::creatememblock(void)
{
   memblock = new (tmemoryblock);
   memblock->startblocksize = 30000;
   memblock->res = 4;
   memblock->generated = false;
   memblock->color[0] = 0;
   memblock->color[1] = 1;
   memblock->color[2] = 2;
   memblock->maxset = maxvalue;
   memblock->acttile = 0;
   memblock->tilevalcount = 2;

   blockcount = maxx * maxy;
 
   memblock->mempointer = (word * ) new ( word [ blockcount ]);
   return 0;
}

/*void tplasma::preview(word sx, word sy, word barsize)
{
   register x,y;
   register i;
 
   x = sx;
   y = sy;
 
   if (flip == true) {
      for (i=blockcount-1;i>0 ;i-- ) {
         if ( barsize <= 1 ) {
            if (memblock->mempointer[i] > memblock->tilevalue1 ) {
               if (memblock->mempointer[i] <= memblock->tilevalue2 ) putpixel(x,y, memblock->col2);
               else putpixel(x,y, memblock->col3);
            }
            else putpixel(x,y, memblock->col1);
         }
         else {
            if (memblock->mempointer[i] > memblock->tilevalue1 ) {
               if (memblock->mempointer[i] <= memblock->tilevalue2 ) bar(x,y,x+barsize,y+barsize,memblock->col2);
               else bar(x,y,x+barsize,y+barsize,memblock->col3);
            }
            else bar(x,y,x+barsize,y+barsize,memblock->col1);
         }
         x += barsize;
         if (x >= sx + (maxx-1) * barsize ) {
            x = sx;
            y += barsize;
         } 
      } 
   } else {
      for (i=0;i<blockcount-1;i++ ) {
         if ( barsize <= 1 ) {
            if (memblock->mempointer[i] > memblock->tilevalue1 ) {
               if (memblock->mempointer[i] <= memblock->tilevalue2 ) putpixel(x,y, memblock->col2);
               else putpixel(x,y, memblock->col3);
            }
            else putpixel(x,y, memblock->col1);
         }
         else {
            if (memblock->mempointer[i] > memblock->tilevalue1 ) {
               if (memblock->mempointer[i] <= memblock->tilevalue2 ) bar(x,y,x+barsize-1,y+barsize-1,memblock->col2);
               else bar(x,y,x+barsize-1,y+barsize-1,memblock->col3);
            }
            else bar(x,y,x+barsize-1,y+barsize-1,memblock->col1);
         }
         x += barsize;
         if (x >= sx + (maxx-1) * barsize ) {
            x = sx;
            y += barsize;
         } 
      } 
   } 
}

void tplasma::process(word sx, word sy,word barsize)
{
   creatememblock();
   generateplasma(true);
   preview(sx,sy,barsize);
   showtext2(strrr(memblock->tilevalue1),500,100);
   do {                      
      ch = r_key();
      switch (ch) {
      case ct_p : {
             memblock->tilevalue1++;
             preview(sx,sy,barsize);
             showtext2(strrr(memblock->tilevalue1),500,100);
             }
         break;
      case ct_o : {
             memblock->tilevalue1--;
             preview(sx,sy,barsize);
             showtext2(strrr(memblock->tilevalue1),500,100);
             }
         break;
         case ct_l : {
             memblock->tilevalue2++;
             preview(sx,sy,barsize);
             showtext2(strrr(memblock->tilevalue2),500,200);
             }
         break;
      case ct_k : {
             memblock->tilevalue2--;
             preview(sx,sy,barsize);
             showtext2(strrr(memblock->tilevalue2),500,200);
             }
         break;
      case ct_f :
             if (flip == true) flip = false;
             else flip = true;
             preview(sx,sy,barsize);
         break;
      case ct_enter : {
            generateplasma(false);
            memblock->tilevalue1 = memblock->maxset / 2;
            preview(sx,sy,barsize);
            showtext2(strrr(memblock->tilevalue1),500,100);
         }
         break;
      } 
   } while (ch != ct_esc ); 
}*/

word tplasma::getmem(word x, word y)
{
   if (x >= maxx ) x -= maxx;
   if (y >= maxy ) y -= maxy;
   return memblock->mempointer[ x + (y * maxx ) ] ;
}


void tplasma::setmem(word x, word y,word color)
{
   if (x >= maxx ) x -= maxx;
   if (y >= maxy ) y -= maxy;
   memblock->mempointer[ x + (y * maxx ) ] = color;
}

void tplasma::membar(word x1 ,word y1 ,word x2 ,word y2, word color )
{  word exc;
   if (x1 >x2) {
      exc = x1;
      x1 = x2;
      x2 = exc;
   } 

   if (y1 >y2) {
      exc = y1 ;
      y1 = y2 ;
      y2 = exc ;
   } 

   for (int j=y1;j<=y2;j++ ) {
      for (int i=x1;i<=x2;i++) {
          //memblock->mempointer[ i + (j * maxx ) ] = color;
          setmem(i,j,color);
      } /* endfor */
   } /* endfor */
}



void tplasma::generateplasma(boolean resettile)
{
   word colour;
   word sblocksize;

   if (memblock->startblocksize / 2 >= maxx) memblock->startblocksize = maxx / 2;

   word blocksize = memblock->startblocksize;
 
   blocksize /= memblock->res;
 
   sblocksize = blocksize;

   //memset(memblock->mempointer,0,blockcount * 2);
   //memset(memblock->mempointer,0,sizeof(*memblock->mempointer));

   memblock->generated = true;

   if (resettile == true ) {
      for (word i=0;i<memblock->tilevalcount;i++ ) {
         memblock->tileval[i] = ( memblock->maxset / ( memblock->tilevalcount +1 ) ) * (i +1 );
      } /* endfor */
   } else {
      for (word i=0;i<memblock->tilevalcount;i++ ) {
         if (memblock->tileval[i] > memblock->maxset) memblock->tileval[i] = memblock->maxset;
      } /* endfor */
   } /* endif */


  // ## MB 
  int maxy_local = maxy;
  #ifdef cheatsymmetry
  maxy_local /= 2;
  #endif 

   do {
      for(register int y = 0; y < (maxy_local / blocksize) + 1; y++) {
         for(register int x = 0; x < (maxx / blocksize) + 1; x++) {
            /*if(keypress()) {
               r_key();
               return 1;
            }*/
            if(blocksize == sblocksize) {
               colour = random(memblock->maxset + 1);
               if(random(5000) > 3500) colour = 1 + memblock->maxset - random(memblock->maxset / (1 + random(8)));
               colour %= (memblock->maxset + 1);
            }
            else {
               colour = getmem(x * blocksize >= maxx ? 0 : x * blocksize + blocksize,y * blocksize);
               colour += getmem(x * blocksize,y * blocksize < 2 * blocksize ? maxy_local : y * blocksize - blocksize);
               colour += getmem(x * blocksize < 2 * blocksize ? maxx : x * blocksize - blocksize,y * blocksize);
               colour += getmem(x * blocksize,y * blocksize > maxy_local - blocksize ? 0 : y * blocksize + blocksize);

               //colour = memblock->mempointer[ (x * blocksize >= maxx ? 0 : x * blocksize + blocksize) + ( ( y * blocksize ) * maxx ) ];
               //colour += memblock->mempointer[ (x * blocksize ) + ((y * blocksize < 2 * blocksize ? maxy_local : y * blocksize - blocksize)  * maxx ) ];
               //colour += memblock->mempointer[ (x * blocksize < 2 * blocksize ? maxx : x * blocksize - blocksize) + ((y * blocksize) * maxx ) ];
               //colour += memblock->mempointer[ (x * blocksize) + ((y * blocksize < 2 * blocksize ? maxy_local : y * blocksize - blocksize) * maxx ) ];
               colour /= 4; 
            }
            if(blocksize != 1) membar(x * blocksize, y * blocksize, x * blocksize + blocksize - 1, y * blocksize + blocksize - 1,colour);
            else setmem(x,y,colour);
            //memblock->mempointer[ x + (y * maxx ) ] = colour;
         }
      }
      blocksize /= 2;
   }
   while(blocksize > 0);

  // ## MB 
   if ( ysymm )
      for(int y = maxy/2; y < maxy; y++) 
         for(int x = 0; x < maxx ; x++) 
            setmem(x , y, getmem ( x, maxy-y ));

   if ( xsymm )
      for(int x = maxx/2; x < maxx ; x++) 
         for(int y = 0; y < maxy; y++) 
            setmem(x , y, getmem ( maxx-x, y ));


   //showmemory();
}  


