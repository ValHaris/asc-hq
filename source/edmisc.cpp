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

#include <math.h>

#include "edmisc.h";
#include "loadbi3.h"
#include "edevents.h"
#include "edgen.h"
#include "edselfnt.h"
#include "edglobal.h"


// #define smallestweight 8 //gewicht der kleinsten vehicle fr selunitcargo

   tkey		ch;
   pfield		pf;

   pterraintype	auswahl;
   pvehicletype	auswahlf;
   pobjecttype  actobject;
   pbuildingtype	auswahlb;
   int		auswahls;
   byte		auswahlm;
   byte		auswahlw;
   byte		auswahld;
   byte		farbwahl;
   int			altefarbwahl;

   tfontsettings	rsavefont;
   int                  lastselectiontype;
   selectrec		sr[10];

   ppolygon		pfpoly;
   boolean		tfill,polyfieldmode;
   word		fillx1, filly1;

   int			i;
   pbuilding		gbde;
   boolean		mapsaved;
   tmycursor		mycursor;

   tpolygon_management	polymanage;
   tpulldown		pd;
   tcdrom		cdrom;

// õS Checkobject

boolean checkobject(pfield pf)
{
   if ( pf->object->objnum > 0 ) return true;
   else return false;
}

// õS MouseButtonBox

#define maxmouseboxitems 20

class   tmousebuttonbox {
           public :
                 boolean holdbutton; //true : menu only opens if button is held
                 int actcode;
                 void init(void);
                 void additem(int code);
                 int getboxundermouse(void); 
                 void checkfields(void);
                 void run(void);
                 void done(void);
           protected :
                 pfont savefont;
                 void *background;
                 int lastpos,actpos;
                 int mousestat;
                 int maxslots;
                 int itemcount,boxxsize,boxysize,slotsize,boxx,boxy;
                 int item[maxmouseboxitems];
                 };

void tmousebuttonbox::init(void)
{
   ch = ct_invvalue;
   mousestat = mouseparams.taste;
   boxx = mouseparams.x;
   boxy = mouseparams.y;
   itemcount = 0;
   holdbutton=false;
   boxxsize = 0;
   boxysize = 0;
   savefont = activefontsettings.font;
   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = black;
   slotsize = activefontsettings.font->height + 4;

   if (maxmouseboxitems * slotsize > agmp->resolutiony ) maxslots = ( agmp->resolutiony - 20 ) / slotsize;
   else maxslots = maxmouseboxitems;
   
}

void tmousebuttonbox::additem(int code)
{  int txtwidth;

   if (itemcount < maxslots ) {
      item[itemcount] = code;
      txtwidth = gettextwdth((char * ) execactionnames[code], activefontsettings.font);
      if (txtwidth > boxxsize) boxxsize = txtwidth;
      itemcount++;
   } 
}

int tmousebuttonbox::getboxundermouse(void)
{  int mx,my;

   mx=mouseparams.x;
   my=mouseparams.y;
   if ( (mx > boxx ) && (mx < boxx+boxxsize ) ) 
      if ( (my > boxy ) && (my < boxy + boxysize -5 ) ) {
         return ( my - boxy - 4 ) / slotsize;
      } 
   return -1;
}

void tmousebuttonbox::checkfields(void)
{
   if (keypress()) ch = r_key();
   actpos = getboxundermouse();
   if (actpos != lastpos) {
      mousevisible(false);
      if ( ( lastpos != -1 ) && ( item[lastpos] != act_seperator) ) rectangle(boxx + 3, boxy + 3 + lastpos * slotsize, boxx + boxxsize -3, boxy + 3 + ( lastpos +1 ) * slotsize,lightgray);
      if ( ( actpos != -1 ) && ( item[actpos] != act_seperator) ) lines(boxx + 3, boxy + 3 + actpos * slotsize, boxx + boxxsize -3, boxy + 3 + ( actpos +1 ) * slotsize);
      mousevisible(true);
      lastpos =actpos;
      if (actpos != -1 ) {
         if (item[actcode] != act_seperator) actcode = item[actpos];
         else actcode = -1;
      } else actcode = -1;
   }
}

void tmousebuttonbox::run(void)
{  
   lastpos = -1;
   actcode = -1;
   boxxsize+=15;
   boxysize= slotsize * itemcount + 5;

   if (boxy + boxysize > agmp->resolutiony -5 ) boxy = agmp->resolutiony - 5 - boxysize;
   if (boxx + boxxsize > agmp->resolutionx -5 ) boxx = agmp->resolutionx - 5 - boxxsize;

   background = malloc(imagesize(boxx,boxy,boxx+boxxsize,boxy+boxysize));

   mousevisible(false);
   getimage(boxx,boxy,boxx+boxxsize,boxy+boxysize,background);

   bar(boxx,boxy,boxx+boxxsize,boxy+boxysize,lightgray);
   lines(boxx,boxy,boxx+boxxsize,boxy+boxysize);

   activefontsettings.length = boxxsize - 10;
   activefontsettings.background = lightgray;
   activefontsettings.color = black;
   activefontsettings.justify = lefttext;

   for (int i=0;i < itemcount ;i++ ) {
      if (item[i] != act_seperator) showtext2((char*) execactionnames[item[i]],boxx+5,boxy + 5 + slotsize * i);
      else line( boxx + 5, boxy + 5 + slotsize / 2 + slotsize * i, boxx + boxxsize - 5, boxy + 5 + slotsize / 2 + slotsize * i,black) ;
   } /* endfor */
   mousevisible(true);
   while (mouseparams.taste == mousestat) {
      checkfields();
   } /* endwhile */
   if ( (actpos == -1) && (holdbutton == false ) ) {
      while ( (mouseparams.taste == 0) && (ch != ct_esc) ) {
         checkfields();
      } /* endwhile */
   }
}


void tmousebuttonbox::done(void)
{
   mousevisible(false);
   putimage(boxx,boxy,background);
   mousevisible(true);
   asc_free(background);
   activefontsettings.font = savefont;
   while (keypress()) ch = r_key();
   while (mouseparams.taste != 0);
   ch = ct_invvalue;
}


// õS Rightmousebox

int rightmousebox(void)
{  tmousebuttonbox tmb;

   tmb.init();
   pf = getactfield();
   if (pf != NULL) {

      if (pf->vehicle != NULL) tmb.additem(act_changeunitvals);
      if ( (pf->vehicle != NULL) && (pf->vehicle->typ->loadcapacity > 0) ) tmb.additem(act_changecargo);
      if ( (pf->building != NULL) && (pf->building->typ->loadcapacity > 0 ) ) tmb.additem(act_changecargo);
      if ( (pf->building != NULL) && (pf->building->typ->special & cgvehicleproductionb) ) tmb.additem(act_changeproduction);
      if ( (pf->object != NULL ) && (pf->object->mine != 0) ) tmb.additem(act_changeminestrength);
      tmb.additem(act_changeresources);

      tmb.additem(act_seperator);

      tmb.additem(act_changeterraindir);
      if (pf->vehicle != NULL) tmb.additem(act_changeunitdir);
      tmb.additem(act_changeglobaldir);

      tmb.additem(act_seperator);

      if (pf->vehicle != NULL) tmb.additem(act_deleteunit);
      if (pf->building != NULL) tmb.additem(act_deletebuilding);
      if ( (pf->object != NULL ) && (pf->object->objnum > 0 ) ) tmb.additem(act_deleteobject);
      if ( (pf->object != NULL ) && (pf->object->mine != 0) ) tmb.additem(act_deletemine);

      tmb.additem(act_seperator);

      if (pf->vehicle != NULL) tmb.additem(act_unitinfo);
   }
   tmb.additem(act_help);

   tmb.run();
   tmb.done();
   return tmb.actcode;
}

// õS Leftmousebox

int leftmousebox(void)
{  tmousebuttonbox tmb;

   tmb.init();
   tmb.holdbutton=true;
   tmb.additem(act_placebodentyp);
   tmb.additem(act_placeunit);
   tmb.additem(act_placebuilding);
   tmb.additem(act_placeobject);
   tmb.additem(act_placemine);

   tmb.additem(act_seperator);

   //tmb.additem(act_selbodentyp);
   //tmb.additem(act_selunit);
   //tmb.additem(act_selbuilding);
   //tmb.additem(act_selobject);
   //tmb.additem(act_selmine);
   //tmb.additem(act_selweather);
   tmb.additem(act_setactivefieldvals);

   //tmb.additem(act_seperator);

   tmb.additem(act_viewmap);
   tmb.additem(act_repaintdisplay);

   tmb.additem(act_seperator);

   tmb.additem(act_help);

   tmb.run();
   tmb.done();
   return tmb.actcode;
}

// õS PutResource

void tputresources :: init ( int sx, int sy, int dst, int restype, int resmax, int resmin )
{
   initsuche(sx,sy,dst,0); 
   resourcetype = restype;
   maxresource = resmax;
   minresource = resmin;
   dist = 0;
   xp = sx;
   yp = sy;
   testfield();
   startsuche();
}

void tputresources :: testfield ( void )
{
   int m = maxresource - dist * ( maxresource - minresource ) / maxdistance;
   pfield fld = getfield(xp,yp);
   if ( resourcetype == 1 )
      fld->material = m;
   else
      fld->fuel = m;
}

void tputresourcesdlg :: init ( void )
{
   resourcetype = 1;
   restp2 = 0;
   status = 0;
   tdialogbox::init();
   title = "set resources";
   xsize = 400;
   ysize = 300;
   maxresource = 200;
   minresource = 0;
   dist = 10;

   addbutton ( "~O~k", 10, ysize - 30, xsize/2 - 5, ysize - 10 , 0, 1, 1, true );
   addkey ( 1, ct_enter );
   addbutton ( "~C~ancel", xsize/2 + 5, ysize - 30, xsize-10 - 5, ysize - 10 , 0, 1, 2, true );
   addkey ( 2, ct_esc );

   addbutton ( "ma~x~ resources ( at center )", xsize/2 + 5, 80, xsize-20 - 5, 100 , 2, 1, 3, true );
   addeingabe ( 3, &maxresource, 0, 255 );

   addbutton ( "mi~n~ resources ", xsize/2 + 5, 120, xsize-20 - 5, 140 , 2, 1, 4, true );
   addeingabe ( 4, &minresource, 0, 255 );

   addbutton ( "max ~d~istance", 20, 120, xsize/2-5, 140 , 2, 1, 5, true );
   addeingabe ( 5, &dist, 1, 20 );

   addbutton ( "~m~aterial", 20, 160, xsize/2-5, 180, 3, 10, 6, true );
   addeingabe ( 6, &resourcetype, black, dblue );

   addbutton ( "~f~uel", xsize/2 + 5, 160, xsize - 20, 180, 3, 10, 7, true );
   addeingabe ( 7, &restp2, black, dblue );

   buildgraphics(); 

}

void tputresourcesdlg :: buttonpressed ( char id )
{
   tdialogbox :: buttonpressed ( id );

   switch ( id ) {
      case 1: status = 11;
         break;
      case 2: status = 10;
         break;
      case 6: restp2 = 0;
              enablebutton ( 7 );
         break;
      case 7: resourcetype = 0;
              enablebutton ( 6 );
         break;
   } /* endswitch */

}

void tputresourcesdlg :: run ( void )
{
   mousevisible ( true );
   do {
      tdialogbox :: run ( );
   } while ( status < 10 ); /* enddo */
   if ( status == 11 ) {
      tputresources pr;
      pr.init ( getxpos(), getypos(), dist, resourcetype ? 1 : 2, maxresource, minresource );
   }
}


//* õS Place-Functions

void placebodentyp(void)
{
   word         fillx2, filly2;
   integer       i, j;

   cursor.hide(); 
   mousevisible(false); 
   mapsaved = false;
   if (polyfieldmode) {
      tfillpolygonbodentyp fillpoly;

      fillpoly.tempvalue = 0;

      if (fillpoly.paint_polygon ( pfpoly ) == 0) 
         displaymessage("Invalid Polygon !",1 );
   
      polyfieldmode = false;
      pdbaroff();
      displaymap();
   } else {
      lastselectiontype = cselbodentyp;
      pf = getactfield(); 
      if (tfill) { 
         filly2 = cursor.posy + actmap->ypos; 
         fillx2 = cursor.posx + actmap->xpos;
         if (fillx1 > fillx2) exchg(&fillx1,&fillx2);
         if (filly1 > filly2) exchg(&filly1,&filly2);
         for (i = filly1; i <= filly2; i++) 
            for (j = fillx1; j <= fillx2; j++) { 
               pf = getfield(j,i); 
               if ( pf ) {
                  if ( auswahl->weather[auswahlw] ) 
                     pf->typ = auswahl->weather[auswahlw]; 
                  else
                     pf->typ = auswahl->weather[0]; 
   
                  pf->direction = auswahld;
                  pf->setparams();
                  if (pf->vehicle != NULL) 
                     if ( terrainaccessible(pf,pf->vehicle) == false ) removevehicle(&pf->vehicle); 
               }
            } 
         displaymap(); 
         tfill = false; 
         pdbaroff(); 
      } 
      else { 
         if ( auswahl->weather[auswahlw] ) 
            pf->typ = auswahl->weather[auswahlw]; 
         else
            pf->typ = auswahl->weather[0]; 
         pf->direction = auswahld;
         pf->setparams();
         if (pf->vehicle != NULL) 
            if ( terrainaccessible(pf,pf->vehicle) == false ) removevehicle(&pf->vehicle); 
      } 
   } /* endif */
   displaymap(); 
   mousevisible(true); 
   cursor.show(); 
}


void placeunit(void)
{
   if (polyfieldmode) {
      cursor.hide(); 
      mousevisible(false); 
      mapsaved = false;

      tfillpolygonunit fillpoly;

      fillpoly.tempvalue = 0;

      if (fillpoly.paint_polygon ( pfpoly ) == 0) 
         displaymessage("Invalid Polygon !",1 );
   
      polyfieldmode = false;
      pdbaroff();

      displaymap(); 
      mousevisible(true); 
      cursor.show(); 
   } else {
      lastselectiontype = cselunit;
      if (farbwahl < 8) {
         cursor.hide(); 
         mousevisible(false); 
         mapsaved = false;
         pf = getactfield(); 
         if (pf != NULL) 
            if ( terrainaccessible(pf,pf->vehicle) || actmap->gameparameter[ cgp_movefrominvalidfields] ) 
               { 
                  int set = 1;
                  if ( pf->vehicle ) {
                     if ( pf->vehicle->typ != auswahlf )
                        removevehicle(&pf->vehicle); 
                      else {
                         set = 0;
                         pf->vehicle->color = farbwahl * 8;
                      }
                  }
                  if ((auswahlf != NULL) && set ) { 
                     generatevehicle_ka(auswahlf,farbwahl ,pf->vehicle);
                     pf->vehicle->height=1;
                     while ( ! ( ( ( ( pf->vehicle->height & pf->vehicle->typ->height ) > 0) && (terrainaccessible(pf,pf->vehicle) == 2) ) ) && (pf->vehicle->height != 0) )
                        pf->vehicle->height = pf->vehicle->height * 2;
                     if (pf->vehicle->height == 0 ) {
                        if ( actmap->gameparameter[ cgp_movefrominvalidfields] ) {
                           pf->vehicle->height=1;
                           while ( !(pf->vehicle->height & pf->vehicle->typ->height) && pf->vehicle->height )
                              pf->vehicle->height = pf->vehicle->height * 2;
                        }
                        if (pf->vehicle->height == 0 ) 
                           removevehicle(&pf->vehicle); 
                     } 
                     if ( pf->vehicle ) {
                        pf->vehicle->movement = pf->vehicle->typ->movement[log2(pf->vehicle->height)];
                        pf->vehicle->direction = auswahld;
                     }
                  } 
            } 
            else if (auswahlf == NULL) if (pf->vehicle != NULL) removevehicle(&pf->vehicle); 
         displaymap(); 
         mousevisible(true); 
         cursor.show(); 
      }
   } /* endif */
}


void placeobject(void)
{
   cursor.hide();
   mousevisible(false); 
   mapsaved = false;
   pf = getactfield(); 
   if (tfill) { 
      putstreets2(fillx1,filly1,cursor.posx + actmap->xpos,cursor.posy + actmap->ypos, actobject ); 
      // 5. parameter zeiger auf object
      displaymap(); 
      tfill = false; 
      pdbaroff(); 
   } 
   else pf->addobject( actobject ); 
   lastselectiontype = cselobject;
   displaymap(); 
   mousevisible(true); 
   cursor.show(); 
}

void placemine(void)
{
   cursor.hide();
   mousevisible(false); 
   mapsaved = false;
   getactfield()->putmine(farbwahl,auswahlm+1,cminestrength[auswahlm]); 
   lastselectiontype = cselmine;
   displaymap();
   mousevisible(true); 
   cursor.show(); 
}


void putactthing ( void )
{
    cursor.hide(); 
    mousevisible(false); 
    mapsaved = false;
    pf = getactfield();
    switch (lastselectiontype) {
       case cselbodentyp :
       case cselweather : placebodentyp();
          break;
       case cselunit:
       case cselcolor:  placeunit();
          break;
       case cselbuilding:   placebuilding(farbwahl,auswahlb,true);
          break;
       case cselobject:   {
                              pf = getactfield();
                              pvehicle eht = pf->vehicle;
                              pf->vehicle = NULL;
                              pf->addobject( actobject );
                              pf->vehicle = eht;
                           }
          break;
       case cselmine:   if ( farbwahl < 8 ) getactfield()->putmine(farbwahl,auswahlm+1,cminestrength[auswahlm]);
          break;
    } 
    displaymap(); 
    mousevisible(true); 
    cursor.show(); 
}


// õS ShowPalette

void         showpalette(void)
{ 
   bar ( 0, 0, 639, 479, black );
   int x=7;

   for ( char a = 0; a <= 15; a++) 
      for ( char b = 0; b <= 15; b++) { 
         bar(     a * 40, b * 20,a * 40 +  x,b * 20 + 20, xlattables.light    [a * 16 + b]); 
         bar( x + a * 40, b * 20,a * 40 + 2*x,b * 20 + 20, xlattables.nochange [a * 16 + b]); 
         bar(2*x + a * 40, b * 20,a * 40 + 3*x,b * 20 + 20, xlattables.a.dark1    [a * 16 + b]); 
         bar(3*x + a * 40, b * 20,a * 40 + 4*x,b * 20 + 20, xlattables.a.dark2    [a * 16 + b]); 
         bar(4*x + a * 40, b * 20,a * 40 + 5*x,b * 20 + 20, xlattables.a.dark3    [a * 16 + b]); 
      }      
   wait();
   repaintdisplay();
} 

// õS Lines

void lines(int x1,int y1,int x2,int y2)
{ 
   line(x1,y1,x2,y1,white);
   line(x2,y1,x2,y2,darkgray);
   line(x1,y2,x2,y2,darkgray);
   line(x1,y1,x1,y2,white);
} 



// õS PDSetup

void         pdsetup(void)
{ 
  pd.addfield ( "~F~ile" );
   pd.addbutton ( "~N~ew mapctrl+N" , act_newmap    );
   pd.addbutton ( "~L~oad mapctrl+L", act_loadmap   ); 
   pd.addbutton ( "~S~ave mapS",      act_savemap   ); 
   pd.addbutton ( "Save map ~a~s",     act_savemapas ); 
   pd.addbutton ( "seperator",         -1            ); 
   pd.addbutton ( "~W~rite map to PCX-Filectrl+G", act_maptopcx); 
   #ifdef HEXAGON
    pd.addbutton ( "~I~mport BI map", act_import_bi_map );
    pd.addbutton ( "Insert ~B~I map", act_insert_bi_map );
   #endif
   #ifdef FREEMAPZOOM
    pd.addbutton ( "seperator", -1 ); 
    pd.addbutton ( "set zoom level", act_setzoom ); 
   #endif
   pd.addbutton ( "seperator", -1 ); 
   pd.addbutton ( "E~x~itEsc", act_end); 
      
  pd.addfield ("~T~ools"); 
   pd.addbutton ( "~V~iew mapctrl+V",          act_viewmap );
   pd.addbutton ( "~S~how paletteL",           act_showpalette ); 
   pd.addbutton ( "~R~ebuild displayctrl+R",   act_repaintdisplay ); 
   pd.addbutton ( "seperator",                  -1 ); 
   pd.addbutton ( "~C~reate ressourcesctrl+F", act_createresources ); 
   pd.addbutton ( "~M~ap generatorG",          act_mapgenerator ); 
   pd.addbutton ( "Resi~z~e mapR",             act_resizemap ); 
   pd.addbutton ( "set global ~w~eatherõctrl-W", act_setactweatherglobal );
   #ifdef HEXAGON
    pd.addbutton ( "Sm~o~oth coasts",          act_smoothcoasts ); 
   #endif

   pd.addfield ("~O~ptions"); 
    pd.addbutton ( "~M~ap valuesctrl+M",          act_changemapvals );
    pd.addbutton ( "~C~hange playersO",           act_changeplayers); 
    pd.addbutton ( "~E~dit eventsE",              act_events ); 
    pd.addbutton ( "~S~etup Alliancesctrl+A",     act_setupalliances );
    pd.addbutton ( "seperator",                    -1); 
    pd.addbutton ( "~T~oggle ResourceViewõctrl+B", act_toggleresourcemode); 
    pd.addbutton ( "~B~I ResourceMode",            act_bi_resource ); 
    pd.addbutton ( "~A~sc ResourceMode",           act_asc_resource ); 
    pd.addbutton ( "edit map ~P~arameters",        act_setmapparameters );

   pd.addfield ("~H~elp"); 
    pd.addbutton ( "~U~nit Informationctrl+U",    act_unitinfo );
    pd.addbutton ( "~T~errain Information",        act_terraininfo );
    pd.addbutton ( "seperator",                    -1 ); 
    pd.addbutton ( "~H~elp SystemF1",             act_help );
    pd.addbutton ( "~A~bout",                      act_about ); 
      


/*
   pd.pdb.count = 4; 
   for (int i=0;i < pd.pdb.count ;i++ ) pd.pdb.pdfield[i] = new( tpdfield );

   strcpy(pd.pdb.pdfield[0]->name,"~F~ile");
   strcpy(pd.pdb.pdfield[1]->name,"~T~ools"); 
   strcpy(pd.pdb.pdfield[2]->name,"~O~ptions"); 
   strcpy(pd.pdb.pdfield[3]->name,"~H~elp"); 
   //strcpy(pd.pdb.pdfield[5]->name,"");
   //strcpy(pd.pdb.pdfield[6]->name,"");
   //strcpy(pd.pdb.pdfield[7]->name,"");
   //strcpy(pd.pdb.pdfield[8]->name,"");
      
//File
      strcpy(pd.pdb.pdfield[0]->button[0].name,"~N~ew mapStrg+N");
      strcpy(pd.pdb.pdfield[0]->button[1].name,"~L~oad mapStrg+L"); 
      strcpy(pd.pdb.pdfield[0]->button[2].name,"~S~ave mapS"); 
      strcpy(pd.pdb.pdfield[0]->button[3].name,"Save map ~a~s"); 
      strcpy(pd.pdb.pdfield[0]->button[4].name,"seperator"); 
      strcpy(pd.pdb.pdfield[0]->button[5].name,"~W~rite map to PCX-FileStrg+G"); 
      #ifdef HEXAGON
         strcpy(pd.pdb.pdfield[0]->button[6].name,"~I~mport BI map");
         strcpy(pd.pdb.pdfield[0]->button[7].name,"Insert ~B~I map");
         strcpy(pd.pdb.pdfield[0]->button[8].name,"seperator"); 
         strcpy(pd.pdb.pdfield[0]->button[9].name,"E~x~itEsc"); 
         pd.pdb.pdfield[0]->count = 10;
      #else
         strcpy(pd.pdb.pdfield[0]->button[6].name,"seperator"); 
         strcpy(pd.pdb.pdfield[0]->button[7].name,"E~x~itEsc"); 
         pd.pdb.pdfield[0]->count = 8;
      #endif
      
      
//Tools
      strcpy(pd.pdb.pdfield[1]->button[0].name,"~V~iew mapStrg+V");
      strcpy(pd.pdb.pdfield[1]->button[1].name,"~S~how paletteL"); 
      strcpy(pd.pdb.pdfield[1]->button[2].name,"~R~ebuild displayStrg+R"); 
      strcpy(pd.pdb.pdfield[1]->button[3].name,"seperator"); 
      strcpy(pd.pdb.pdfield[1]->button[4].name,"~C~reate ressourcesStrg+F"); 
      strcpy(pd.pdb.pdfield[1]->button[5].name,"~M~ap generatorG"); 
      strcpy(pd.pdb.pdfield[1]->button[6].name,"Resi~z~e mapR"); 
      #ifdef HEXAGON
         strcpy(pd.pdb.pdfield[1]->button[7].name,"Sm~o~oth coasts"); 
         pd.pdb.pdfield[1]->count = 8;
      #else
         pd.pdb.pdfield[1]->count = 7;
      #endif
      
      
//Options
      strcpy(pd.pdb.pdfield[2]->button[0].name,"~M~ap valuesStrg+M");
      strcpy(pd.pdb.pdfield[2]->button[1].name,"~C~hange playersO"); 
      strcpy(pd.pdb.pdfield[2]->button[2].name,"~E~dit eventsE"); 
      strcpy(pd.pdb.pdfield[2]->button[3].name,"~S~etup AlliancesStrg+A");
      strcpy(pd.pdb.pdfield[2]->button[4].name,"seperator"); 
      strcpy(pd.pdb.pdfield[2]->button[5].name,"~T~oggle ResourceViewõStrg+B"); 
      strcpy(pd.pdb.pdfield[2]->button[6].name,"~B~I ResourceMode"); 
      strcpy(pd.pdb.pdfield[2]->button[7].name,"~A~sc ResourceMode"); 
      strcpy(pd.pdb.pdfield[2]->button[8].name,""); 
      pd.pdb.pdfield[2]->count = 8;
      
//Help
      strcpy(pd.pdb.pdfield[3]->button[0].name,"~U~nit InformationStrg+U");
      strcpy(pd.pdb.pdfield[3]->button[1].name,"seperator"); 
      strcpy(pd.pdb.pdfield[3]->button[2].name,"~H~elp SystemF1");
      strcpy(pd.pdb.pdfield[3]->button[3].name,"~A~bout"); 
      strcpy(pd.pdb.pdfield[3]->button[4].name,""); 
      strcpy(pd.pdb.pdfield[3]->button[5].name,""); 
      strcpy(pd.pdb.pdfield[3]->button[6].name,""); 
      pd.pdb.pdfield[3]->count = 4;
      
     /* strcpy(pd.pdb.pdfield[4]->button[0].name,"Fill");
      strcpy(pd.pdb.pdfield[4]->button[1].name,""); 
      strcpy(pd.pdb.pdfield[4]->button[2].name,""); 
      strcpy(pd.pdb.pdfield[4]->button[3].name,""); 
      strcpy(pd.pdb.pdfield[4]->button[4].name,""); 
      pd.pdb.pdfield[4]->count = 1;
*/

      pd.init();
      pd.setshortkeys();
}

// õS PlayerChange

/* class   tcolorsel : public tstringselect {
           public :
                 virtual void setup(void);
                 virtual void buttonpressed(byte id);
                 virtual void run(void);
                 virtual void gettext(word nr);
                 };

void         tcolorsel::setup(void)
{ 

   action = 0;
   title = "Select Color"; 
   numberoflines = 9;
   xsize = 250;
   ex = xsize - 100;
   ey = ysize - 60; 
   addbutton("~D~one",20,ysize - 40,120,ysize - 20,0,1,2,true); 
   addkey(2,ct_enter); 
   addbutton("~C~ancel",130,ysize - 40,230,ysize - 20,0,1,3,true); 
} 


void         tcolorsel::buttonpressed(byte         id)
{ 
   tstringselect::buttonpressed(id);
   switch (id) {
      
      case 2:   
      case 3:   action = id; 
   break; 
   } 
} 


void         tcolorsel::gettext(word nr)
{ 
   if (nr == 8) strcpy(txt,"Neutral");
   else {
      if ((nr>=0) && (nr<=7)) {
         strcpy(txt,"Color ");
         strcat(txt,strrr(nr+1));
      } else {
         strcpy(txt,"");
      } 
   }
} 


void         tcolorsel::run(void)
{ 
   mousevisible(false); 
   for (i=0;i<=8 ;i++ ) bar(x1 + 160,y1 + 52 + i*21 ,x1 + 190 ,y1 + 62 + i * 21,20 + i * 8);
   mousevisible(true); 
   do { 
      tstringselect::run(); 
   }  while ( ! ( (taste == ct_esc) || ( (action == 2) || (action == 3) ) || (msel == 1)) );
   if ( (action == 3) || (taste == ct_esc) ) redline = 255;
} 


byte         colorselect(void)
{ 
  tcolorsel  sm; 

   sm.init(); 
   sm.run(); 
   sm.done(); 
   return sm.redline; 
} */


class  tplayerchange : public tdialogbox {
          public :
              byte action;
              byte sel1,sel2;
              byte bkgcolor;
              void init(void);
              virtual void run(void);
              virtual void buttonpressed(byte id);
              void anzeige(void);
              };




void         tplayerchange::init(void)
{ 
   char *s1;

   tdialogbox::init(); 
   title = "Player Change";
   x1 = 50;
   xsize = 370; 
   y1 = 50; 
   ysize = 410; 
   action = 0; 
   sel1 = 255;
   sel2 = 255;
   bkgcolor = lightgray;

   windowstyle = windowstyle ^ dlg_in3d; 

   for (i=0;i<=8 ;i++ ) {
      s1 = new(char[12]);
      if (i == 8) {
         strcpy(s1,"~N~eutral");
      } else {
         strcpy(s1,"Player ~");
         strcat(s1,strrr(i+1));
         strcat(s1,"~");
      } 
      addbutton(s1,20,55+i*30,150,75+i*30,0,1,6+i,true); 
      addkey(1,ct_1+i); 
   }  

   //addbutton("~C~hange",20,ysize - 70,80,ysize - 50,0,1,3,true); 
   addbutton("X~c~hange",20,ysize - 70,170,ysize - 50,0,1,4,true); 
   addbutton("~M~erge",200,ysize - 70,350,ysize - 50,0,1,5,true); 

   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,1,true); 
   addkey(1,ct_enter); 
   addbutton("E~x~it",200,ysize - 40,350,ysize - 20,0,1,2,true); 

   buildgraphics(); 

   for (i=0;i<=8 ;i++ ) bar(x1 + 170,y1 + 60 + i*30 ,x1 + 190 ,y1 + 70 + i * 30,20 + ( i << 3 ));

   anzeige();

   mousevisible(true); 
} 

void         tplayerchange::anzeige(void)
{
   int e,b,m[9];
   for (i=0;i<=8 ;i++ ) m[i] =0;
   for (int i =0;i < actmap->xsize * actmap->ysize ;i++ ) {
      int color;
      pfield fld = &actmap->field[i];
      if  ( ( fld->object ) && (fld->object && fld->object->mine ) ) {
         color = ( fld->object->mine >> 1 ) & 7; // typ l”schen;
         m[color]++;
      }
   }
   activefontsettings.length = 40;
   activefontsettings.background = lightgray;
   activefontsettings.color = red;
   activefontsettings.justify = lefttext;
   activefontsettings.font = schriften.smallarial;
   mousevisible(false); 
   showtext2("Units",x1+210,y1+35);
   showtext2("Build.",x1+260,y1+35);
   showtext2("Mines",x1+310,y1+35);
   for (i=0;i<=8 ;i++ ) {
      if (i == sel1 ) rectangle (x1 + 16,y1+51+i*30,x1+154,y1+79+i*30, 20 );
      else if ( i == sel2 ) rectangle (x1 + 16,y1+51+i*30,x1+154,y1+79+i*30, 28 );
      else rectangle (x1 + 16,y1+51+i*30,x1+154,y1+79+i*30, bkgcolor );
      pvehicle fe = actmap->player[i].firstvehicle; 
      pbuilding fb = actmap->player[i].firstbuilding; 
      b = 0;
      e = 0;
      while (fe != NULL) {
         e++;
         fe = fe->next;
      } /* endwhile */
      while (fb != NULL) {
         b++;
         fb = fb->next;
      } /* endwhile */
      activefontsettings.justify = righttext;
      showtext2(strrr(e),x1+200,y1+56+i*30);
      showtext2(strrr(b),x1+255,y1+56+i*30);
      showtext2(strrr(m[i]),x1+310,y1+56+i*30);
   } /* endfor */
   mousevisible(true); 
}


void         tplayerchange::run(void)
{ 

   do { 
      tdialogbox::run(); 
      if (taste == ct_f1) help ( 1050 );
   }  while (!((taste == ct_esc) || ((action == 1) || (action ==2)))); 
   if ((action == 2) || (taste == ct_esc)) ; 
} 


void         tplayerchange::buttonpressed(byte         id)
{ 
   tdialogbox::buttonpressed(id); 
   switch (id) {
      
      case 1:   
      case 2:   action = id; 
        break;
/*     case 3: {
           int sel = colorselect();
           if ( ( sel != 255) && ( sel != sel1 ) && ( sel1 != 255 ) && 
              ( actmap->player[sel].firstvehicle == NULL ) && ( actmap->player[sel].firstbuilding == NULL )   ) {

           } 
        }
        break; */
     case 4: {
           if ( ( sel1 != 255) && ( sel2 != sel1 ) && ( sel2 != 255 ) ) {
              pvehicle fe = actmap->player[sel1].firstvehicle; 
              pbuilding fb = actmap->player[sel1].firstbuilding; 
              while (fe != NULL) {
                 fe->color = sel2 * 8;
                 fe = fe->next;
              } /* endwhile */
              while (fb != NULL) {
                 fb->color = sel2 * 8;
                 fb = fb->next;
              } /* endwhile */
              fe = actmap->player[sel2].firstvehicle; 
              fb = actmap->player[sel2].firstbuilding; 
              while (fe != NULL) {
                 fe->color = sel1 * 8;
                 fe = fe->next;
              } /* endwhile */
              while (fb != NULL) {
                 fb->color = sel1 * 8;
                 fb = fb->next;
              } /* endwhile */
              fe = actmap->player[sel1].firstvehicle; 
              fb = actmap->player[sel1].firstbuilding; 
              actmap->player[sel1].firstvehicle = actmap->player[sel2].firstvehicle; 
              actmap->player[sel1].firstbuilding = actmap->player[sel2].firstbuilding; ; 
              actmap->player[sel2].firstvehicle = fe; 
              actmap->player[sel2].firstbuilding = fb; 
              for (int i =0;i < actmap->xsize * actmap->ysize ;i++ ) {
                 int color,typ;
                 pfield fld = &actmap->field[i];
                 if  ( ( fld->object ) && (fld->object && fld->object->mine ) ) {
                    typ = fld->object->mine >> 4;
                    color = ( fld->object->mine >> 1 ) & 7; // typ l”schen;
                    if (color == sel1) { 
                       color = sel2;
                       fld->object->mine = 1 | (color << 1) | (typ << 4);
                    }
                    else if ( color == sel2 ) {
                       color = sel1;
                       fld->object->mine = 1 | (color << 1) | (typ << 4);
                    }
                 }
              } /* endfor */
              anzeige();
           }
        }
        break;
     case 5: {
           if ( ( sel1 != 255) && ( sel2 != sel1 ) && ( sel2 != 255 ) ) {
              pvehicle fe = actmap->player[sel1].firstvehicle; 
              pbuilding fb = actmap->player[sel1].firstbuilding; 
              if (fe == NULL) {
                 actmap->player[sel1].firstvehicle = actmap->player[sel2].firstvehicle; 
                 actmap->player[sel2].firstvehicle = NULL; 
              } else {
                 while (fe->next != NULL) fe = fe->next;
                 fe->next = actmap->player[sel2].firstvehicle; 
                 actmap->player[sel2].firstvehicle = NULL; 
              } /* endif */
              if (fb == NULL) {
                 actmap->player[sel1].firstbuilding = actmap->player[sel2].firstbuilding; 
                 actmap->player[sel2].firstbuilding = NULL; 
              } else {
                 while (fb->next != NULL) fb = fb->next;
                 fb->next = actmap->player[sel2].firstbuilding; 
                 actmap->player[sel2].firstbuilding = NULL; 
              } /* endif */
              while (fe != NULL) {
                 fe->color = sel1 * 8;
                 fe = fe->next;
              } /* endwhile */
              while (fb != NULL) {
                 fb->color = sel1 * 8;
                 fb = fb->next;
              } /* endwhile */
              for (int i =0;i < actmap->xsize * actmap->ysize ;i++ ) {
                 int color,typ;
                 pfield fld = &actmap->field[i];
                 if  ( ( fld->object ) && (fld->object && fld->object->mine ) ) {
                    typ = fld->object->mine >> 4;
                    color = ( fld->object->mine >> 1 ) & 7; // typ l”schen;
                    if (color == sel2) { 
                       color = sel1;
                       fld->object->mine = 1 | (color << 1) | (typ << 4);
                    }
                 }
              } /* endfor */
              anzeige();
           }
        }
        break;
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:  
      case 14: if ( id-6 != sel1 ) {
         sel2 = sel1;
         sel1 = id-6;
         anzeige();
      }
   break; 
   } 
} 


void playerchange(void)  
{ tplayerchange       sc; 

   sc.init(); 
   sc.run(); 
   sc.done(); 
} 


//* õS Mycursor

void       tmycursor::getimg ( void )
{
     int xp, yp;

     xp = sx + posx * ix;
     yp = sy + posy * iy;

     if (agmp-> linearaddress != hgmp-> linearaddress ) {
        npush(*agmp);
        *agmp = *hgmp;
        getimage(xp, yp, xp + fieldsizex, yp + fieldsizey, backgrnd );
        npop (*agmp);
     } else {
        getimage(xp, yp, xp + fieldsizex, yp + fieldsizey, backgrnd );
     } 
}

void       tmycursor::putbkgr ( void )
{
     int xp, yp;

     xp = sx + posx * ix;
     yp = sy + posy * iy;

     if (agmp-> linearaddress != hgmp-> linearaddress ) {
        npush( *agmp );
        *agmp = *hgmp;
        putimage(xp, yp, backgrnd );
        npop ( *agmp );
     } else {
        putimage(xp, yp, backgrnd );
     }


}

void       tmycursor::putimg ( void )
{
     int xp, yp;

     xp = sx + posx * ix;
     yp = sy + posy * iy;

     if (agmp-> linearaddress != hgmp-> linearaddress ) {
        npush( *agmp );
        *agmp = *hgmp;
        putrotspriteimage(xp, yp, picture,color );
        npop ( *agmp );
     } else {
        putrotspriteimage(xp, yp, picture,color );
     } 
}


void         exchg(word *       a1,
                   word *       a2)
{ word        * ex;

   *ex = *a1; 
   *a1 = *a2; 
   *a2 = *ex; 
} 


void         pdbaroff(void)
{ 
   char s[200];
 
   mousevisible(false); 
   pd.baroff();
   rsavefont = activefontsettings; 

   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = 1;
   activefontsettings.background = black;
   activefontsettings.length = 200;
   activefontsettings.justify = lefttext;
      
   int y = agmp->resolutiony - 25;

   if ( actmap ) {
      strcpy(s,"Title : ");
      showtext2(strcat(s,actmap->title),10,5);
      strcpy(s,"X-Size : ");
      showtext2(strcat(s,strrr(actmap->xsize)),10,y);
      strcpy(s,"Y-Size : ");
      showtext2(strcat(s,strrr(actmap->ysize)),100,y);
   }
   if (polyfieldmode) {
      strcpy(s,"Polyfield fill mode");
      showtext2(s,10,y-20);
   }
   if (tfill) {
      strcpy(s,"Fill ( ");
      strcat(s,strrr(fillx1));
      strcat(s," / ");
      strcat(s,strrr(filly1));
      strcat(s," ) ");
      strcat(s,auswahl->name);
      showtext2(s,10,y-20);
   }
   activefontsettings = rsavefont; 
   mousevisible(true); 
} 


// õS TCDPlayer


class   tcdplayer : public tstringselect {
           public :
                 virtual void setup(void);
                 virtual void buttonpressed(byte id);
                 virtual void run(void);
                 virtual void gettext(word nr);
                 };

void         tcdplayer ::setup(void)
{ 

   action = 0;
   xsize = 400;
   ysize = 400;
   x1 = 100;
   title = "CD- Player";
   numberoflines = cdrom.cdinfo.lasttrack;
   ey = ysize - 60;
   ex = xsize - 120;
   addbutton("~R~CD",320,50,390,70,0,1,11,true);
   addbutton("~P~lay",320,90,390,110,0,1,12,true);
   addbutton("~S~top",320,130,390,150,0,1,13,true);
   addbutton("E~x~it",10,ysize - 40,290,ysize - 20,0,1,10,true);
} 


void         tcdplayer ::buttonpressed(byte         id)
{ 
   tstringselect::buttonpressed(id);
   switch (id) {
      
   case 10:   action = 3;
      break;
   case 11:  {
      cdrom.readcdinfo();
      redline = 0;
      firstvisibleline = 0;
      numberoflines = cdrom.cdinfo.lasttrack;
      viewtext();
      }
      break;
   case 12: {
         cdrom.playtrack(redline);
      }
      break;
   case  13: cdrom.stopaudio ();
      break;
   } 
} 


void         tcdplayer ::gettext(word nr)
{ 
    if (cdrom.cdinfo.track[nr]->type > 3) {
       strcpy(txt,"Data Track ");
       strcat(txt,strrr(nr+1));
    } else {
       strcpy(txt,"");
       strcat(txt,strrr(cdrom.cdinfo.track[nr]->min));
       strcat(txt," : ");
       strcat(txt,strrr(cdrom.cdinfo.track[nr]->sec));
       strcat(txt," : ");
       strcat(txt,strrr(cdrom.cdinfo.track[nr]->frame));
       strcat(txt,"   Track ");
       strcat(txt,strrr(nr+1));
    } /* endif */
} 


void         tcdplayer ::run(void)
{ 
   do { 
      tstringselect::run(); 
      if ( (msel == 1)  || ( taste == ct_enter ) ) cdrom.playtrack(redline);
   }  while ( ! ( (taste == ct_esc) || (action == 3) ) );
} 


void cdplayer( void )
{
   tcdplayer cd;

   cd.init();
   cd.run();
   cd.done();
}



void         repaintdisplay(void)
{ 
   mousevisible(false);
   cursor.hide();
   bar(0,0,agmp->resolutionx-1,agmp->resolutiony-1,0);
   displaymap(); 
   pdbaroff(); 
   showallchoices(); 
   cursor.show();
   mousevisible(true);
} 


void         k_savemap(boolean saveas)
{ 

   #ifdef logging
   logtofile ( "edmisc / k_savemap /  started" );
   #endif

  tenterfiledescription efd;

   char filename[300];

   int nameavail = 0;
   if ( actmap->preferredfilenames && actmap->preferredfilenames->mapname[0] ) {
      nameavail = 1;
      strcpy ( filename, actmap->preferredfilenames->mapname[0] );
   } else
      filename[0] = 0;


   mousevisible(false);
   if ( saveas || !nameavail ) {
      fileselectsvga(mapextension,filename,0);
   } 
   if ( filename[0] ) {


      #ifdef logging
      logtofile ( "edmisc / k_savemap /  filename entered" );
      #endif

      mapsaved = true;

      char filedescription[500];
      if ( actmap->preferredfilenames && actmap->preferredfilenames->mapdescription[0] )
         strcpy ( filedescription, actmap->preferredfilenames->mapdescription[0] );
      else
         filedescription[0] = 0;

      cursor.hide();
      efd.init( filedescription );
      efd.run();

      strcpy ( filedescription , efd.description );

      #ifdef logging
      logtofile ( "edmisc / k_savemap /  description entered" );
      #endif

      if ( !actmap->preferredfilenames ) {
         actmap->preferredfilenames = new PreferredFilenames;
         memset ( actmap->preferredfilenames, 0 , sizeof ( PreferredFilenames ));
      }
      if ( actmap->preferredfilenames->mapdescription[0] )
         asc_free ( actmap->preferredfilenames->mapdescription[0] );
      actmap->preferredfilenames->mapdescription[0] = strdup ( filedescription );

      if ( actmap->preferredfilenames->mapname[0] )
         asc_free ( actmap->preferredfilenames->mapname[0] );
      actmap->preferredfilenames->mapname[0] = strdup ( filename );


      efd.done();
      #ifdef logging
      logtofile ( "edmisc / k_savemap /  vor savemap" );
      #endif
      savemap(filename,filedescription);
      #ifdef logging
      logtofile ( "edmisc / k_savemap /  nach savemap " );
      #endif
      displaymap(); 
      cursor.show();
   } 
   mousevisible(true); 
}


void         k_loadmap(void)
{ 
  char         s1[300];
  char filedescription[500];

   mousevisible(false); 
   fileselectsvga(mapextension, s1, 1, filedescription );
   if ( s1[0] ) {
      cursor.hide(); 
      displaymessage("loading map %s",0, filedescription );
      loadmap(s1);
      initmap(); 
      removemessage(); 
      if (loaderror > 0) { 
         displaymessage("error nr %d",1, loaderror );
      } 

      displaymap(); 
      cursor.show(); 
      mapsaved = true;
   } 
   mousevisible(true); 
} 

void         placebuilding(byte               colorr,
                          pbuildingtype   buildingtyp,
                          boolean            choose)

{     
   #define bx   100
   #define by   100  
   #define sts  bx + 200  

   pbuilding    gbde; 

   mousevisible(false);
   cursor.hide();
   mapsaved = false;

   int f = 0;
   if (choose == true) {
      for ( int x = 0; x < 4; x++ )
         for ( int y = 0; y < 6; y++ )
            if ( buildingtyp->getpicture ( x, y )) {
               int xpos;
               int ypos;
               buildingtyp->getfieldcoordinates ( getxpos(), getypos(), x, y, &xpos, &ypos );
               pfield fld = getfield ( xpos, ypos );
               if ( buildingtyp->terrain_access->accessible ( fld->bdt ) <= 0 )
                  f++;
            }
      if ( f )
         if (choice_dlg("Invalid terrain for building !","~i~gnore","~c~ancel") == 2) 
            return;

      putbuilding(getxpos(),getypos(),colorr * 8,buildingtyp,buildingtyp->construction_steps); 
   }

   gbde = getactfield()->building;
   if (gbde == NULL) return;

   gbde->plus = gbde->maxplus;

   activefontsettings.color = 1;
   activefontsettings.background = lightgray;
   activefontsettings.length = 100;
   
   changebuildingvalues(&gbde);
   if (choose == true) 
      building_cargo( gbde );

   lastselectiontype = cselbuilding; 
   displaymap();
   cursor.show();
   mousevisible(true);
} 



void         freevariables(void)
{
   asc_free ( mycursor.backgrnd );
}


void         setstartvariables(void) 
{ 
   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = 1; 
   activefontsettings. background = 0;
   
   mapsaved = true;
   polyfieldmode = false;

   auswahl  = getterraintype_forpos(0);
   auswahlf = getvehicletype_forpos(0); 
   auswahlb = getbuildingtype_forpos(0); 
   actobject = getobjecttype_forpos(0);
   auswahls = 0;
   auswahlm = 1; 
   auswahlw = 0;
   auswahld = 0;
   farbwahl = 0; 
   
   sr[0].maxanz = terraintypenum;
   sr[0].showall = true;
   sr[1].maxanz = vehicletypenum;
   sr[1].showall = true;
   sr[2].maxanz = 9;
   sr[2].showall = true;
   sr[3].maxanz = buildingtypenum;
   sr[3].showall = true;
   sr[4].maxanz = vehicletypenum;
   sr[4].showall = false;
   sr[5].maxanz = objecttypenum;
   sr[5].showall = true;
   sr[6].maxanz = vehicletypenum;
   sr[6].showall = false;
   sr[7].maxanz = vehicletypenum;
   sr[7].showall = false;
   sr[8].maxanz = minecount;
   sr[8].showall = true;
   sr[9].maxanz = cwettertypennum;
   sr[9].showall = true;

   mycursor.picture = cursor.orgpicture;
   mycursor.backgrnd = malloc (10000);

   atexit( freevariables );
} 

void showcoordinates(void)
{
   char       s[200];
 
   int y = agmp->resolutiony - 45;

   activefontsettings.length = 95;
   waitretrace();
   bar(0, y, 639, y+20, black);
   bar(200, y+20, 400, y+40 ,black);
   strcpy(s,"X-Pos : ");
   showtext2(strcat(s,strrr(cursor.posx+actmap->xpos)),200,y+20);
   strcpy(s,"Y-Pos : ");
   showtext2(strcat(s,strrr(cursor.posy+actmap->ypos)),300,y+20);
}

int  selectfield(int * cx ,int  * cy)
{
   int oldposx = getxpos();
   int oldposy = getypos();
   cursor.gotoxy( *cx, *cy );
   displaymap();
   cursor.show();
   tkey ch = ct_invvalue;
   do {
      if (keypress()) {
            ch = r_key(); 
            switch (ch) {
               case ct_1k:
               case ct_2k:   
               case ct_3k:   
               case ct_4k:   
               case ct_5k:   
               case ct_6k:   
               case ct_7k:   
               case ct_8k:   
               case ct_9k:   
               case ct_1k + ct_stp:   
               case ct_2k + ct_stp:   
               case ct_3k + ct_stp:   
               case ct_4k + ct_stp:   
               case ct_5k + ct_stp:   
               case ct_6k + ct_stp:   
               case ct_7k + ct_stp:   
               case ct_8k + ct_stp:   
               case ct_9k + ct_stp:   { 
                                 mousevisible(false); 
                                 movecursor(ch); 
                                 cursor.show(); 
                                 showcoordinates();
                                 mousevisible(true); 
                              } 
                              break; 
                } /* endswitch */
      }
   } while ( ch!=ct_enter  &&  ch!=ct_space  && ch!=ct_esc); /* enddo */
   cursor.hide();
   *cx=cursor.posx+actmap->xpos;
   *cy=cursor.posy+actmap->ypos;
   // cursor.gotoxy( oldposx, oldposy );
   if ( ch == ct_enter )
      return 1;
   else
      if ( ch == ct_esc )
         return 0;
      else
         return 2;
}

//* õS FillPolygonevent

class  tfillpolygonwevent : public tfillpolygonsquarecoord {
        public:
             byte tempvalue;
             virtual void initevent ( void );
             virtual void setpointabs ( int x,  int y  );
           };

void tfillpolygonwevent::setpointabs    ( int x,  int y  )
{
       pfield ffield = getfield ( x , y );
       if (ffield)
           ffield->a.temp = tempvalue;
}



void tfillpolygonwevent::initevent ( void )
{
}

void createpolygon (ppolygon *poly, byte place, int id)
{
   polymanage.addpolygon(poly,place,id); 
   changepolygon(*poly);
}

//* õS FillPolygonbdt

void tfillpolygonbodentyp::setpointabs    ( int x,  int y  )
{
       pfield ffield = getfield ( x , y );
       if (ffield) {
           ffield->a.temp = tempvalue;
           if ( auswahl->weather[auswahlw] ) 
              ffield->typ = auswahl->weather[auswahlw]; 
           else
              ffield->typ = auswahl->weather[0]; 
           ffield->direction = auswahld;
           ffield->setparams();
           if (ffield->vehicle != NULL) 
              if ( terrainaccessible(ffield,ffield->vehicle) == false ) removevehicle(&ffield->vehicle); 
       }
}

void tfillpolygonbodentyp::initevent ( void )
{
}

//* õS FillPolygonunit

void tfillpolygonunit::setpointabs    ( int x,  int y  )
{
       pfield ffield = getfield ( x , y );
       if (ffield) {
          if ( terrainaccessible(ffield,ffield->vehicle) ) 
               { 
                  if (ffield->vehicle != NULL) removevehicle(&ffield->vehicle); 
                  if ((auswahlf != NULL)) { 
                     generatevehicle_ka(auswahlf,farbwahl ,ffield->vehicle);
                     ffield->vehicle->height=1;
                     while ( ! ( ( ( ( ffield->vehicle->height & ffield->vehicle->typ->height ) > 0) && (terrainaccessible(ffield,ffield->vehicle) == 2) ) ) && (ffield->vehicle->height != 0) )
                        ffield->vehicle->height = ffield->vehicle->height * 2;
                     for (i = 0; i <= 31; i++) ffield->vehicle->loading[i] = NULL;
                     if (ffield->vehicle->height == 0 ) removevehicle(&ffield->vehicle); 
                     else ffield->vehicle->movement = ffield->vehicle->typ->movement[log2(ffield->vehicle->height)];
                     ffield->vehicle->direction = auswahld;
                  } 
            } 
            else if (auswahlf == NULL) if (ffield->vehicle != NULL) removevehicle(&ffield->vehicle); 
            ffield->a.temp = tempvalue;
       }
}

void tfillpolygonunit::initevent ( void )
{
}


//* õS ChangePoly


void tchangepoly::setpolytemps (byte value)
{
   tfillpolygonwevent fillpoly;

   fillpoly.tempvalue = value ;

   if (fillpoly.paint_polygon ( poly ) == 0) {
      if ( value ) 
         displaymessage("Invalid Polygon !",1 );
      setpolypoints(0);
   }
}


void tchangepoly::setpolypoints(byte value)
{
   for (int i=0;i < poly->vertexnum ;i++ ) {
      pf = getfield(poly->vertex[i].x,poly->vertex[i].y);
      pf->a.temp = value;
   } /* endfor */
}

byte tchangepoly::checkpolypoint(int x, int y)
{
   for (int i=0 ;i < poly->vertexnum ;i++ ) if ( (poly->vertex[i].x == x ) && ( poly->vertex[i].y == y ) ) return 1;
   return 0;
}

void tchangepoly::deletepolypoint(int x, int y)
{
   if ( poly->vertexnum >= 3) setpolytemps(0);
   else setpolypoints(0);
   for (int i=0 ;i < poly->vertexnum ;i++ ) if ( (poly->vertex[i].x == x ) && ( poly->vertex[i].y == y ) ) {
      for (int j = i ;j < poly->vertexnum - 1 ;j++ ) {
         poly->vertex[j].x = poly->vertex[j+1].x;
         poly->vertex[j].y = poly->vertex[j+1].y;
      }
      poly->vertexnum--;
      if (poly->vertexnum > 1) ( poly->vertex ) = (tpunkt*) realloc (poly->vertex, poly->vertexnum * sizeof ( poly->vertex[0] ) );
      if ( poly->vertexnum >= 3) setpolytemps(1);
      setpolypoints(2);
      return;
   }
}


void  tchangepoly::run(void)
{
   int            x,y;

   x=0;
   y=0;
   if ( poly->vertexnum >= 3) setpolytemps(1);
   setpolypoints(2);
   int r;
   displaymessage("use space to select the vertices of the polygon\nfinish the selection by pressing enter",3);
   do {
      mousevisible(false);
      displaymap();
      mousevisible(true);
      r = selectfield(&x,&y);
      if ( r != 1   &&   (x != 50000) ) {
         if ( checkpolypoint(x,y) == 0 ) {
            if ( poly->vertexnum >= 3) setpolytemps(0);
            else setpolypoints(0);
            poly->vertexnum++;
            if (poly->vertexnum > 1) ( poly->vertex ) = (tpunkt*) realloc (poly->vertex, poly->vertexnum * sizeof ( poly->vertex[0] ) );
            poly->vertex[poly->vertexnum-1].x=x;
            poly->vertex[ poly->vertexnum-1].y=y;
            if ( poly->vertexnum >= 3) setpolytemps(1);
            setpolypoints(2);
         }
         else deletepolypoint(x,y);
      } 
   } while ( r != 1 ); /* enddo */
   if ( poly->vertexnum >= 3) setpolytemps(0);
   else setpolypoints(0);
}

void changepolygon(ppolygon poly)
{ 
  tchangepoly cp;

  cp.poly = poly;
  cp.run();

}


//* õS NewMap
   
  class tnewmap : public tdialogbox {
        public :
               byte action;
               char passwort[11], *sptitle;
               int sxsize,sysize;
               boolean valueflag,random,campaign;
               tcampaign cmpgn;
               pterraintype         tauswahl;
               word auswahlw;
               void init(void);
               virtual void run(void);
               virtual void buttonpressed(byte  id);
               void done(void);
               };


void         tnewmap::init(void)
{ 
  word         w;
  boolean      b;

   tdialogbox::init();
   action = 0;
   if (valueflag == true ) title = "New Map";
   else title = "Map Values";
   x1 = 70;
   xsize = 500;
   y1 = 70;
   ysize = 350;
   campaign = !!actmap->campaign;
   sxsize = actmap->xsize;
   sysize = actmap->ysize;
   sptitle = new ( char[100] );
   strcpy( sptitle, actmap->title );
   if (valueflag == true ) {
      strcpy(passwort,"");
      memset(&cmpgn,0,sizeof(cmpgn));
   }
   else {
      strcpy(passwort, actmap->codeword);
      if (actmap->campaign != NULL) {
         campaign = true;
         memcpy (&cmpgn , actmap->campaign, sizeof(cmpgn));
      }
      else memset(&cmpgn,0,sizeof(cmpgn));
   }
   random = false; 
   auswahlw = 0; /* !!! */

   w = (xsize - 60) / 2; 
   windowstyle = windowstyle ^ dlg_in3d; 

   addbutton("~T~itle",15,70,xsize - 30,90,1,1,1,true); 
   addeingabe(1,sptitle,0,100);

   if (valueflag == true ) {
      addbutton("~X~ Size",15,130,235,150,2,1,2,true);
      addeingabe(2,&sxsize,idisplaymap.getscreenxsize(1),65534);

      addbutton("~Y~ Size",250,130,470,150,2,1,3,true); 
      addeingabe(3,&sysize,idisplaymap.getscreenysize(1),65534);

      addbutton("~R~andom",250,190,310,210,3,1,11,true);
      addeingabe(11,&random,0,lightgray);

      if ( ! random) b = true;
      else b =false;

      addbutton("~B~dt",350,190,410,210,0,1,12,b);
   } 

   addbutton("~P~assword (10 letters)",15,190,235,210,1,1,9,true);
   addeingabe(9,passwort,10,10);

   addbutton("C~a~mpaign",15,230,235,245,3,1,5,true); 
   addeingabe(5,&campaign,0,lightgray);

   addbutton("~M~ap ID",15,270,235,290,2,1,6,campaign); 
   addeingabe(6,&cmpgn.id,0,65535);

   addbutton("Pr~e~vious Map ID",250,270,470,290,2,1,7,campaign); 
   addeingabe(7,&cmpgn.prevmap,0,65535);

   addbutton("~D~irect access to map",250,230,470,245,3,1,8,campaign); 
   addeingabe(8,&cmpgn.directaccess,0,lightgray);

   if (valueflag == true ) addbutton("~S~et Map",20,ysize - 40,20 + w,ysize - 10,0,1,10,true);
   else addbutton("~S~et Mapvalues",20,ysize - 40,20 + w,ysize - 10,0,1,10,true); 
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,4,true); 

   tauswahl = auswahl;

   buildgraphics(); 
   if (valueflag == true )
      if ( ! random ) {
         mousevisible(false); 
         if ( tauswahl->weather[auswahlw] ) 
            putspriteimage(x1 + 440,y1 + 182,tauswahl->weather[auswahlw]->picture[0]);
         else
            putspriteimage(x1 + 440,y1 + 182,tauswahl->weather[0]->picture[0]);
         mousevisible(true); 
      } 
   rahmen(true,x1 + 10,y1 + starty,x1 + xsize - 10,y1 + ysize - 45); 
   rahmen(true,x1 + 11,y1 + starty + 1,x1 + xsize - 11,y1 + ysize - 46); 
   mousevisible(true); 
} 


void         tnewmap::run(void)
{ 
   do { 
      tdialogbox::run();
      if (action == 3) {
         if ( sysize & 1 ) {
            displaymessage("YSize must be even !",1 ); 
            action = 0; 
         } 
         if (action != 4) 
            if ( sxsize * sysize * sizeof( tfield ) > maxavail() ) {
               displaymessage("Not enough memory for map.\nGenerate smaller map or free more memory",1); 
               action = 0; 
            } 
      } 
   }  while (!((taste == ct_esc) || (action >= 2))); 
   if (action == 3) { 
      actmap->xsize = sxsize;
      actmap->ysize = sysize;
      if (valueflag == true ) {
         if ( tauswahl->weather[auswahlw] )
            generatemap(tauswahl->weather[auswahlw], sxsize , sysize );
         else
            generatemap(tauswahl->weather[0], sxsize , sysize );
         if ( random) 
            mapgenerator();
      }   

      mapsaved = false;
      if ( actmap->title == NULL ) actmap->title = (char *) malloc( strlen(sptitle)+1 );
      else actmap->title = (char *) realloc( actmap->title, strlen(sptitle)+1 );
      
      strcpy(actmap->title,sptitle);
      strcpy(actmap->codeword,passwort);
      if (campaign == true ) {
         if (actmap->campaign == NULL) actmap->campaign = new (tcampaign);
         actmap->campaign->id = cmpgn.id; 
         actmap->campaign->prevmap = cmpgn.prevmap; 
         actmap->campaign->directaccess = cmpgn.directaccess; 
      } 
      else if (actmap->campaign != NULL) {
         asc_free(actmap->campaign);
         actmap->campaign = NULL; 
      } 
   } 
} 


void         tnewmap::buttonpressed(byte id)
{ 
   if (id == 4) 
      action = 2; 
   if (id == 10) 
      action = 3; 
   if (id == 5) 
      if (campaign) { 
         enablebutton(6); 
         enablebutton(7); 
         enablebutton(8); 
      } 
      else { 
         disablebutton(6); 
         disablebutton(7); 
         disablebutton(8); 
      } 
   if (id == 12) { 
      npush ( lastselectiontype );
      npush ( auswahl );

      void *p;
 
      mousevisible(false);
      p=malloc( imagesize(430,0,639,479) );
      getimage(430,0,639,479,p);
      mousevisible(true);
                  
      lastselectiontype = cselbodentyp;
      selterraintype( ct_invvalue );

      mousevisible(false);
      putimage(430,0,p);
      mousevisible(true);

      tauswahl = auswahl;

      npop ( auswahl );
      npop ( lastselectiontype );

      if ( tauswahl->weather[auswahlw] )
         putspriteimage(x1 + 440,y1 + 182,tauswahl->weather[auswahlw]->picture[0]); 
      else
         putspriteimage(x1 + 440,y1 + 182,tauswahl->weather[0]->picture[0]); 
   } 
   if (id == 11) 
      if ( ! random) {
         enablebutton(12); 
      } 
      else { 
         disablebutton(12); 
      } 
} 


void         tnewmap::done(void)
{ 
   tdialogbox::done();
   if (action == 3) displaymap();
   asc_free( sptitle );
}



void         newmap(void)
{ 
  tnewmap      nm; 

   nm.valueflag = true; 
   nm.init(); 
   nm.run(); 
   nm.done(); 
   pdbaroff(); 
} 


//* õS MapVals


void         changemapvalues(void)
{ 
  tnewmap      nm; 

   nm.valueflag = false; 
   nm.init(); 
   nm.run(); 
   nm.done(); 
//   if (actmap->campaign != NULL) setupalliances();
   pdbaroff(); 
}



     class tsel : public tdialogbox {
           public :
               byte action;
               pbuilding gbde;
               int rs,e,m,f,mrs;
               tresources plus,mplus, biplus;
               int col;
               boolean tvisible;
               char name[260];
               void init(void);
               virtual void run(void);
               virtual void buttonpressed(byte id);
               int dummy;
               };


void         tsel::init(void)
{ word         w; 
  boolean      b;

   tdialogbox::init();
   action = 0; 
   title = "Building Values";
   x1 = 90;
   xsize = 445;
   y1 = 10;
   ysize = 440;
   w = (xsize - 60) / 2; 
   action = 0; 

   windowstyle = windowstyle ^ dlg_in3d; 

   e = gbde->actstorage.a.energy; 
   m = gbde->actstorage.a.material; 
   f = gbde->actstorage.a.fuel;
   plus = gbde->plus;
   mplus = gbde->maxplus; 
   rs = gbde->researchpoints; 
   mrs = gbde->maxresearchpoints;
   tvisible = gbde->visible;
   biplus = gbde->bi_resourceplus;
   col = gbde->color / 8;

   if ( gbde->name != NULL ) strcpy(name,gbde->name);
   else strcpy(name,"");

   addbutton("~N~ame",15,50,215,70,1,1,10,true); 
   addeingabe(10,&name[0],0,25);

   addbutton("~E~nergy-Storage",15,90,215,110,2,1,1,true); 
   addeingabe(1,&e,0,gbde->typ->gettank(0));

   addbutton("~M~aterial-Storage",15,130,215,150,2,1,2,true); 
   addeingabe(2,&m,0,gbde->typ->gettank(1));

   addbutton("~F~uel-Storage",15,170,215,190,2,1,3,true); 
   addeingabe(3,&f,0,gbde->typ->gettank(2));

   if ( ( gbde->typ->special & cgenergyproductionb ) > 0) b = true;
   else b = false;

   addbutton("Energy-Max-Plus",230,50,430,70,2,1,13,b);
   addeingabe(13,&mplus.a.energy,0,gbde->typ->maxplus.a.energy);
   
//   addbutton("Energ~y~-Plus",230,90,430,110,2,1,4,b);
//   addeingabe(4,&plus.energy,0,mplus.energy);
   
   if ( ( gbde->typ->special & cgmaterialproductionb ) > 0 ) b = true;
   else b = false;

   addbutton("Material-Max-Plus",230,130,430,150,2,1,14,b);
   addeingabe(14,&mplus.a.material,0,gbde->typ->maxplus.a.material);

//   addbutton("M~a~terial-Plus",230,170,430,190,2,1,5,b);
//   addeingabe(5,&plus.material,0,mplus.material);

   if ( ( gbde->typ->special & cgfuelproductionb ) > 0) b = true;
   else b = false;

   addbutton("Fuel-Max-Plus",230,210,430,230,2,1,15,b);
   addeingabe(15,&mplus.a.fuel,0,gbde->typ->maxplus.a.fuel);
   dummy=2000;
//   addeingabe(15,&dummy,0,gbde->typ->maxplus.fuel);

//   addbutton("F~u~el-Plus",230,250,430,270,2,1,6,b);
//   addeingabe(6,&plus.fuel,0,mplus.fuel);

   if ( (  ( gbde->typ->special & cgresearchb ) > 0) && ( gbde->typ->maxresearchpoints > 0)) b = true;
   else b = false;

   addbutton("~R~esearch-Points",15,210,215,230,2,1,9,b);
   addeingabe(9,&rs,0,gbde->typ->maxresearchpoints);

   addbutton("Ma~x~research-Points",15,250,215,270,2,1,11,b);
   addeingabe(11,&mrs,0,gbde->typ->maxresearchpoints);


   addbutton("~V~isible",15,290,215,300,3,1,12,true);
   addeingabe(12,&tvisible,0,lightgray);

   addbutton("~C~olor",230,280,430,300,2,1,104,true);
   addeingabe(104,&col,0,8);


   addbutton("BI energy plus",15,330,215,350,2,1,101,1);
   addeingabe(101,&biplus.a.energy,0,maxint);

   addbutton("BI material plus",230,330,430,350,2,1,102,1);
   addeingabe(102,&biplus.a.material,0,maxint);

   addbutton("BI fuel plus",15,370,215,390,2,1,103,1);
   addeingabe(103,&biplus.a.fuel,0,maxint);


   addbutton("~S~et Values",20,ysize - 40,20 + w,ysize - 10,0,1,7,true); 
   addkey(7,ct_enter); 
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,8,true);

   buildgraphics(); 

   mousevisible(true); 
} 


void         tsel::run(void)
{ 
   do { 
      tdialogbox::run();
   }  while (!((taste == ct_esc) || (action == 1))); 
} 


void         tsel::buttonpressed(byte         id)
{
   switch(id) {
case 7: { 
      mapsaved = false;
      action = 1; 
      gbde->actstorage.a.energy = e; 
      gbde->actstorage.a.material = m; 
      gbde->actstorage.a.fuel = f;
      gbde->plus = plus;
      gbde->maxplus = mplus; 
      if ( col != gbde->color/8 )
         gbde->convert ( col );
   
      gbde->researchpoints = rs; 
      gbde->maxresearchpoints = mrs;
      gbde->visible = tvisible;
      gbde->bi_resourceplus = biplus;
      if ( strlen(name) > 0 ) {
         gbde->name = (char * ) realloc ( gbde->name ,strlen(name) +1 );
         strcpy(gbde->name,name);
      }
      else 
        if ( gbde->name ) {
           asc_free(gbde->name);
           gbde->name = NULL;
        }
   } 
   break;
case 8: action = 1; 
   break;
   
/*

case 13: {
                addeingabe(4,&plus.energy,0,mplus.energy);
                if (mplus.energy < plus.energy ) {
                   plus.energy = mplus.energy;
                   enablebutton(4);
                } 
             }
   break;
case 14: {
              addeingabe(5,&plus.material,0,mplus.material);
                 if (mplus.material < plus.material) {
                    plus.material =mplus.material;
                    enablebutton(5);
                 } 
              }

   break;
case 15: {
              addeingabe(6,&plus.fuel,0,mplus.fuel);
                 if (plus.fuel > mplus.fuel) {
                    plus.fuel = mplus.fuel;
                    enablebutton(6);
                 } 
              } 
   break;
*/   

  }
} 


void         changebuildingvalues(pbuilding *  b)
{ 
   tsel         sel;
   if (*b == NULL) return;
   sel.gbde = *b; 
   sel.init(); 
   sel.run(); 
   sel.done(); 
   *b = sel.gbde; 

   (*b) -> plus = (*b) ->maxplus;

   if (sel.tvisible == false) displaymap();
} 

// õS Class-Change


class   tclass_change: public tstringselect {
           public :
                 pvehicle unit;
                 word tklasse,tarmor,tfunktion;
                 word tweapstr[8];
                 virtual void setup(void);
                 virtual void buttonpressed(byte id);
                 virtual void run(void);
                 virtual void gettext(word nr);
                 };

void         tclass_change::setup(void)
{  int i,j,k;
   char s[200], *s2;

   action = 0;
   title = "Change Class";

   x1 = 50;
   y1 = 30;
   xsize = 540;
   ysize = 420;
   ex = 200;
   ey = 150;
   lnshown = 5;
   numberoflines = unit->typ->classnum;

   if (unit->klasse > unit->typ->classnum-1) unit->klasse = 0;
      redline = unit->klasse;
   tklasse = unit->klasse;
   for (j = 0 ; j < unit->typ->weapons->count  ; j++ ) {
       //tweapstr[j] = unit->weapstrength[j];
       tweapstr[j] = unit->typ->weapons->weapon[j].maxstrength * unit->typ->classbound[tklasse].weapstrength[ log2 ( unit->typ->weapons->weapon[j].typ & cwweapon) ] / 1024;
   }
   //tarmor = unit->armor;
   tarmor = unit->typ->armor * unit->typ->classbound[tklasse].armor / 1024;
   addbutton("~A~rmor",220,210,420,230,2,1,4,true);
   addeingabe(4,&tarmor,1,65535);
   for (i = 0 ; i < unit->typ->weapons->count  ; i++ ) {
      strcpy ( s, "" );
      for (k = 0; k < cwaffentypennum; k++)
         if (unit->typ->weapons->weapon[i].typ & (1 << k) ) {
            strcat( s, cwaffentypen[k] );
            strcat( s, " ");
         }
      strcat(s,"(~");
      strcat(s,strrr(i+1));
      strcat(s,"~)");
      s2 = (char * ) malloc( strlen(s) +1 );
      strcpy(s2,s);
      addbutton(s2,220,90+ i * 40 ,420,110 + i * 40,2,1,i+5,true);
      addeingabe(i+5,&tweapstr[i],1,65535);
   }
   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,20,true);
   addkey(20,ct_enter);
   addbutton("~C~ancel",190,ysize - 40,340,ysize - 20,0,1,21,true);
} 


void         tclass_change::buttonpressed(byte         id)
{ 
   tstringselect::buttonpressed(id);
   switch (id) {
      
      case 20:
      case 21:   action = id-18;
   break; 
   } 
} 


void         tclass_change::gettext(word nr)
{ 
   strcpy(txt,unit->typ->classnames[nr]);
} 


void         tclass_change::run(void)
{ 
   int j;

   do { 
      tstringselect::run(); 
      if ( (msel == 1 ) || ( taste == ct_enter ) ){
         if (tklasse != redline) {
            tklasse = redline;
            for (j = 0 ; j < unit->typ->weapons->count  ; j++ ) {
               tweapstr[j] = unit->typ->weapons->weapon[j].maxstrength * unit->typ->classbound[tklasse].weapstrength[ log2 ( unit->typ->weapons->weapon[j].typ & cwweapon) ] / 1024;
               showbutton(j+5);
            }
            tarmor = unit->typ->armor * unit->typ->classbound[tklasse].armor / 1024;
            tfunktion = unit->typ->functions  & unit->typ->classbound[tklasse].vehiclefunctions;
                              
            showbutton(4);
         } 
      } 
      switch (taste) {
      } /* endswitch */
   }  while ( ! ( (taste == ct_esc) || ( (action == 2) || (action == 3) ) ) );
   if (action == 2) {
      unit->klasse = tklasse;
      for (j = 0 ; j < unit->typ->weapons->count  ; j++ ) unit->weapstrength[j] = tweapstr[j];
      unit->armor = tarmor;
      unit->functions = tfunktion;
   }
} 


void         class_change(pvehicle p)
{ 
  tclass_change cc;

   cc.unit = p;
   cc.init();
   cc.run();
   cc.done();
} 


// õS Polygon-Management
                
class tpolygon_managementbox: public tstringselect {
              public:
                 ppolygon poly;
                 virtual void setup(void);
                 virtual void buttonpressed(byte id);
                 virtual void run(void);
                 virtual void gettext(word nr);
                 };
                 

tpolygon_management::tpolygon_management(void)
{
   polygonanz = 0;
   lastpolygon = firstpolygon;
}

void         tpolygon_managementbox::setup(void)
{ 
   action = 0;
   title = "Choose Polygon";

   x1 = 70;
   y1 = 40;
   xsize = 500;
   ysize = 400;
   lnshown = 10;
   numberoflines = polymanage.polygonanz;
   activefontsettings.length = xsize - 30;
   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,1,true);
   addkey(1,ct_enter);
   addbutton("~C~ancel",190,ysize - 40,340,ysize - 20,0,1,2,true);
} 


void         tpolygon_managementbox::buttonpressed(byte         id)
{ 
   tstringselect::buttonpressed(id);
   switch (id) {
      case 1:
      case 2:   action = id;
   break; 
   } 
} 


void         tpolygon_managementbox::gettext(word nr)
{ 
   char s[200];
   ppolystructure pps;
   int i,vn;

   const byte showmaxvertex = 5;

   pps = polymanage.firstpolygon;
   for (i=0 ;i<nr; i++ )
       pps =pps->next;
   strcpy(s,"");
   if ( pps->poly->vertexnum> showmaxvertex ) vn=5;
   else vn = pps->poly->vertexnum;
   switch (pps->place) {
   case 0: {
      strcat(s,"Events: ");
      strcat(s,ceventactions[pps->id]);
      }
      break;
   } 
   for (i=0;i <= vn-1;i++ ) {
       strcat(s,"(");
       strcat(s,strrr(pps->poly->vertex[i].x));
       strcat(s,"/");
       strcat(s,strrr(pps->poly->vertex[i].y));
       strcat(s,") ");
   } /* endfor */
   if (vn < pps->poly->vertexnum )  strcat(s,"...");
   strcpy(txt,s);
} 


void         tpolygon_managementbox::run(void)
{ 
   do { 
      tstringselect::run(); 
      if (taste ==ct_f1) help (1030);
   }  while ( ! ( (taste == ct_enter ) || (taste == ct_esc) || ( (action == 1) || (action == 2) ) ) );
   if ( ( ( taste == ct_enter ) || (action == 1 ) ) && (redline != 255 ) ) {
      ppolystructure pps;

      pps = polymanage.firstpolygon;
      for (i=0 ;i<redline; i++ ) pps =pps->next;
      poly = pps->poly;
   } 
} 

void tpolygon_management::addpolygon(ppolygon *poly, byte place, int id)
{
   (*poly) = new(tpolygon);

   (*poly)->vertexnum = 0;
   (*poly)->vertex    = (tpunkt*) malloc ( 1 * sizeof ( (*poly)->vertex[0] ) );
   if (polygonanz == 0 ) {
      lastpolygon = new ( tpolystructure );
      firstpolygon = lastpolygon;
   }
   else {
      lastpolygon->next = new ( tpolystructure );
      lastpolygon = lastpolygon->next;
   }
   lastpolygon->poly = (*poly);
   lastpolygon->id = id;
   lastpolygon->place = place;
   polygonanz++;
}

void tpolygon_management::deletepolygon(ppolygon *poly)
{
   asc_free( (*poly)->vertex );
   asc_free(*poly);
}


byte        getpolygon(ppolygon *poly) //return Fehlerstatus
{
   tpolygon_managementbox polymanagebox;
    
   polymanagebox.poly = (*poly);
   polymanagebox.init();
   polymanagebox.run();
   polymanagebox.done();
   (*poly) = polymanagebox.poly;
   if ( (polymanagebox.action == 2) || (polymanagebox.taste == ct_esc ) ) return 1;
   else return 0;
} 

// õS Unit-Values





     class tunit: public tdialogbox {
                word        dirx,diry;
                byte        action;
                pvehicle    unit, orgunit;
                int         w2, heightxs;
              public:
               // boolean     checkvalue( char id, char* p );
                void        init( pvehicle v );
                void        run( void );
                void        buttonpressed( char id );
     };


void         tunit::init( pvehicle v )
{ 
   unit = new tvehicle ( v, NULL );
   orgunit = v;

   word         w;

   tdialogbox::init();
   action = 0; 
   title = "Unit-Values";

   windowstyle = windowstyle ^ dlg_in3d; 

   x1 = 70;
   xsize = 500;
   y1 = 40;
   ysize = 400;
   w = (xsize - 60) / 2; 
   w2 = (xsize - 40) / 8;
   dirx= x1 + 350;
   diry= y1 + 100;
   action = 0; 

   if ( !unit->name ) {
      unit->name = new  char[100] ;
      unit->name[0] = 0;
   }

   addbutton("~N~ame",50,80,250,100,1,1,29,true);
   addeingabe(29, unit->name, 0, 100);

   addbutton("E~x~p of Unit",50,120,250,140,2,1,1,true);
   addeingabe(1, &unit->experience, 0, maxunitexperience);

   addbutton("~D~amage of Unit",50,160,250,180,2,1,2,true);
   addeingabe(2, &unit->damage, 0, 100 );
 
   addbutton("~F~uel of Unit",50,200,250,220,2,1,3,true);
   addeingabe( 3, &unit->fuel, 0, unit->getmaxfuelforweight() );

   addbutton("~M~aterial",50,240,250,260,2,1,12,true);
   addeingabe(12,&unit->material, 0, unit->getmaxmaterialforweight() );


   int unitheights = 0;
   heightxs = 320;
   pfield fld = getfield ( orgunit->xpos, orgunit->ypos);
   if ( fld->vehicle == orgunit ) {
      npush ( unit->height );
      for (i=0;i<=7 ;i++) {
          unit->height = 1 << i;
          if (( ( unit->height & unit->typ->height ) > 0) && ( terrainaccessible( fld, unit ) == 2)) {
             addbutton("",20+( i * w2),heightxs,w2 * (i +1 ),heightxs+24,0,1,i+4,true);
             addkey(i+4,ct_0+i);
          }
          unitheights |= 1 << i;
      } /* endfor  Buttons 4-11*/
      npop ( unit->height );
   }

   // 8 im Kreis bis 7
/*
   addbutton("",dirx-22,diry-45,dirx-2,diry-17,0,1,14,true);
   addkey(14,ct_8k);

   addbutton("",dirx+22,diry-45,dirx+50,diry-17,0,1,15,true);
   addkey(15,ct_9k);

   addbutton("",dirx+22,diry+6,dirx+50,diry+26,0,1,16,true);
   addkey(16,ct_6k);

   addbutton("",dirx+22,diry+47,dirx+50,diry+75,0,1,17,true);
   addkey(17,ct_3k);
 
   addbutton("",dirx-22,diry+47,dirx-2,diry+75,0,1,18,true);
   addkey(18,ct_2k);

   addbutton("",dirx-70,diry+47,dirx-42,diry+75,0,1,19,true);
   addkey(19,ct_1k);
 
   addbutton("",dirx-70,diry+6,dirx-42,diry+26,0,1,20,true);
   addkey(20,ct_4k);

   addbutton("",dirx-70,diry-45,dirx-42,diry-17,0,1,21,true);
   addkey(21,ct_7k);
*/
   if ( unit->typ->classnum > 0 ) addbutton("C~h~ange Class",280,280,450,300,0,1,32,true);

   addbutton("~R~eactionfire",350,250,450,260,3,1,22,true);
   addeingabe(22,&unit->reactionfire_active, 0, lightgray);
   
   addbutton("~S~et Values",20,ysize - 40,20 + w,ysize - 10,0,1,30,true);
   addkey(30,ct_enter );
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,31,true);
   addkey(31,ct_esc );

   buildgraphics(); 
 
   mousevisible(false);

   if ( unitheights ) 
       for (i=0;i<=7 ;i++) {
           if ( unit->height == (1 << i) ) 
              bar(x1 + 25+( i * w2),y1 + heightxs-5,x1 + w2 * (i +1 ) - 5,y1 + heightxs-3,red);
    
           if ( unitheights & ( 1 << i ))
              putimage(x1 + 23+( i * w2), y1 + heightxs + 2 ,icons.height[i]);
       }

   // 8 im Kreis bis 7
   float radius = 50;
   float pi = 3.14159265;
   for ( int i = 0; i < sidenum; i++ ) {
                              
      int x = dirx + radius * sin ( 2 * pi * (float) i / (float) sidenum );
      int y = diry - radius * cos ( 2 * pi * (float) i / (float) sidenum );

      addbutton("", x-10, y - 10, x + 10, y + 10,0,1,14+i,true);
      enablebutton ( 14 + i );
      void* pic = rotatepict ( icons.pfeil2[0], directionangle[i] );
      int h,w;
      getpicsize ( pic, w, h );
      putspriteimage ( x1 + x - w/2, y1 + y - h/2, pic );
      delete pic;
  }

   /*
   putspriteimage(dirx + x1 - 20,diry + y1 -39,icons.pfeil2[0]);
   putspriteimage(dirx + x1 + 30,diry + y1 -39,icons.pfeil2[1]);
   putspriteimage(dirx + x1 + 30,diry + y1 + 8,icons.pfeil2[2]);
   putspriteimage(dirx + x1 + 30,diry + y1 + 53,icons.pfeil2[3]);
   putspriteimage(dirx + x1 - 20,diry + y1 + 53,icons.pfeil2[4]);
   putspriteimage(dirx + x1 - 63,diry + y1 + 53,icons.pfeil2[5]);
   putspriteimage(dirx + x1 - 63,diry + y1 + 8,icons.pfeil2[6]);                            
   putspriteimage(dirx + x1 - 63,diry + y1 -39,icons.pfeil2[7]);
   */

   putrotspriteimage(dirx + x1 - fieldsizex/2 ,diry + y1 - fieldsizey/2, unit->typ->picture[ unit->direction ],unit->color);
   mousevisible(true); 
} 

/*
boolean      tunit::checkvalue( char id, void* p)
{ 
   if ( id == 3 ) {
   }
   addbutton("~F~uel of Unit",50,200,250,220,2,1,3,true);
   addeingabe( 3, &unit->fuel, 0, unit->getmaxfuelforweight() );

   addbutton("~M~aterial",50,240,250,260,2,1,12,true);
   addeingabe(12,&unit->material, 0, unit->getmaxmaterialforweight() );
   return  true;
} 
*/

void         tunit::run(void)
{ 
   do { 
      tdialogbox::run();
   }  while ( !action ); 
} 


void         tunit::buttonpressed(byte         id)
{ 
   byte ht;
 
   switch (id) {
   case 3:  addeingabe(12,&unit->material, 0, unit->getmaxmaterialforweight() );
      break;
      
   case 4 : 
   case 5 :
   case 6 :
   case 7 :
   case 8 :
   case 9 :
   case 10 :
   case 11 :
   {
     int h = 1 << ( id - 4 );
     bar(x1 +20,y1 + heightxs-5,x1 + 480,y1 + heightxs-3,lightgray);
     for (i=0;i<=7 ;i++) {
        ht = 1 << i;
        if ( ht == h ) bar(x1 + 25+( i * w2),y1 + heightxs-5,x1 + w2 * (i +1 ) - 5,y1 + heightxs-3,red);
     } /* endfor */
     unit->height = h;
     unit->movement = unit->typ->movement[ log2 ( unit->height ) ];
   }
   break;
   case 12: addeingabe( 3, &unit->fuel, 0, unit->getmaxfuelforweight() );
      break;
   
   case 14 : 
   case 15 : 
   case 16 :
   case 17 :
   case 18 :                 
   case 19 :
   case 20 :
   case 21 :  {
                  unit->direction = id-14;
                  setinvisiblemouserectanglestk ( dirx + x1 -fieldsizex/2, diry + y1 - fieldsizey/2 ,dirx + x1 + fieldsizex/2 ,diry + y1 +fieldsizey/2 );
                  bar(dirx + x1 -fieldsizex/2, diry + y1 - fieldsizey/2 ,dirx + x1 + fieldsizex/2 ,diry + y1 +fieldsizey/2,lightgray);
                  putrotspriteimage(dirx + x1 - fieldsizex/2 ,diry + y1 - fieldsizey/2, unit->typ->picture[ unit->direction ],unit->color);
                  getinvisiblemouserectanglestk ();
               }
         break;
   case 30 : {
         mapsaved = false;
         action = 1; 
         if ( unit->reactionfire_active ) {
            unit->reactionfire_active = 3;
            unit->reactionfire = 0xff;
         }
         
         orgunit->clone ( unit, NULL );
         delete unit ;
        }
        break;
    case 31 : action = 1;
          delete unit;
        break;
    case 32: class_change( unit );
        break;
   } /* endswitch */
} 


void         changeunitvalues(pvehicle ae)
{ 
   if ( !ae ) 
      return;

   tunit units;
   units.init( ae ); 
   units.run(); 
   units.done(); 
} 


//* õS Resource

     class tres: public tdialogbox {
            public :
                byte action;
                byte fuel,material;
                void init(void);
                virtual void run(void);
                virtual void buttonpressed(byte id);
                };



void         tres::init(void)
{ word         w; 

   tdialogbox::init();
   action = 0; 
   title = "Resources";
   x1 = 170;
   xsize = 200; 
   y1 = 70; 
   ysize = 200; 
   w = (xsize - 60) / 2; 
   action = 0; 
   pf = getactfield();
   fuel = pf->fuel; 
   material = pf->material; 

   windowstyle = windowstyle ^ dlg_in3d; 

   activefontsettings.length = 200;
 
   addbutton("~F~uel (0-255)",50,70,150,90,2,1,1,true); 
   addeingabe(1,&fuel,0,255);
   addbutton("~M~aterial (0-255)",50,110,150,130,2,1,2,true); 
   addeingabe(2,&material,0,255);

   addbutton("~S~et Vals",20,ysize - 40,20 + w,ysize - 10,0,1,7,true); 
   addkey(7,ct_enter); 
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,8,true); 

   buildgraphics(); 

   mousevisible(true); 
} 


void         tres::run(void)
{ 
   do { 
      tdialogbox::run();
   }  while (!((taste == ct_esc) || (action == 1))); 
} 


void         tres::buttonpressed(byte         id)
{ 
   if (id == 7) { 
      mapsaved = false;
      action = 1; 
      pf->fuel = fuel; 
      pf->material = material; 
   } 
   if (id == 8) action = 1; 
} 


void         changeresource(void)
{ 
   tres         resource;

   resource.init(); 
   resource.run(); 
   resource.done(); 
} 

//* õS MineStrength

     class tminestrength: public tdialogbox {
            public :
                byte action;
                byte strength;
                void init(void);
                virtual void run(void);
                virtual void buttonpressed(byte id);
                };



void         tminestrength::init(void)
{ word         w; 

   tdialogbox::init();
   action = 0; 
   title = "Minestrength";
   x1 = 170;
   xsize = 200; 
   y1 = 70; 
   ysize = 160;
   w = (xsize - 60) / 2; 
   action = 0; 
   pf = getactfield();
   strength = pf->object->minestrength;

   windowstyle = windowstyle ^ dlg_in3d; 

   activefontsettings.length = 200;
 
   addbutton("~S~trength (0-255)",30,70,170,90,2,1,1,true);
   addeingabe(1,&strength,0,255);

   addbutton("~S~et Vals",20,ysize - 40,20 + w,ysize - 10,0,1,7,true); 
   addkey(7,ct_enter); 
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,8,true); 

   buildgraphics(); 

   mousevisible(true); 
} 


void         tminestrength::run(void)
{ 
   do { 
      tdialogbox::run();
   }  while (!((taste == ct_esc) || (action == 1))); 
} 


void         tminestrength::buttonpressed(byte         id)
{ 
   if (id == 7) { 
      mapsaved = false;
      action = 1; 
      pf->object->minestrength = strength;
   } 
   if (id == 8) action = 1; 
} 


void         changeminestrength(void)
{ 
   tminestrength  ms;

   pf =  getactfield();
   if ( (pf->object == NULL ) || (pf->object->mine == 0) ) return;
   ms.init();
   ms.run();
   ms.done();
} 


//* õS Pulldown

void         pulldown( void )

{ 
   pd.checkpulldown();
   if ( pd.action2execute >= 0 ) {
      execaction ( pd.action2execute );

      if (mouseparams.y <= pd.pdb.pdbreite ) 
         pd.baron();
      pd.action2execute = -1;
   } 
} 

//* õS Laderaum Unit-Cargo

class tladeraum : public tdialogbox {
               protected:
                    int maxusable;
                    int itemsperline;
                    int action;
                    virtual char* getinfotext ( int pos );
                    void displayinfotext ( void );

                    virtual void additem ( void ) = 0;
                    virtual void removeitem ( int pos ) = 0;
                    virtual void checkforadditionalkeys ( tkey ch );
                    void displayallitems ( void );
                    int mouseoverfield ( int pos );
                    virtual void displaysingleitem ( int pos, int x, int y ) = 0;
                    void displaysingleitem ( int pos );
                    int cursorpos;
                    virtual void finish ( int cancel ) = 0;
                    int getcapabilities ( void ) { return 1; };
                    virtual void redraw ( void );

               public :
                    void init( char* ttl );
                    virtual void run(void);
                    virtual void buttonpressed(byte id);
                    void done(void);
                 };

void tladeraum::redraw ( void )
{
   tdialogbox::redraw();
   displayallitems();
}
   

void         tladeraum::init( char* ttl )
{ 
   #ifdef HEXAGON
    maxusable = 18;
   #else
    maxusable = 27;
   #endif

   itemsperline = 8;

   npush ( farbwahl );
   cursorpos = 0;

   tdialogbox::init();
   title = ttl;
   x1 = 20;
   xsize = 600; 
   y1 = 50; 
   ysize = 400; 
   int w = (xsize - 60) / 2; 
   action = 0; 

   windowstyle = windowstyle ^ dlg_in3d; 

   addbutton("~N~ew",20,40,100,70,0,1,1,true);
   addbutton("~E~rase",120,40,220,70,0,1,2,true);

   addbutton("~O~K",20,ysize - 40,20 + w,ysize - 10,0,1,7,true);
   addkey(7,ct_enter); 
   addbutton("Cancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,8,true); 
   addkey(8,ct_esc );

   buildgraphics(); 

   displayallitems(); 
   displayinfotext();

} 

void tladeraum :: displayallitems ( void )
{
   for ( int i = 0; i < 32; i++ )
      displaysingleitem ( i );
}


void tladeraum :: displaysingleitem ( int pos )
{
   int x = x1 + 28 + (pos % itemsperline) * fielddistx;
   int y = y1 + 88 + (pos / itemsperline) * fielddisty * 3;
   setinvisiblemouserectanglestk ( x, y, x + fieldsizex, y + fieldsizey );
   putspriteimage( x, y, icons.stellplatz );
   if ( pos >= maxusable )
      putspriteimage( x, y, icons.X );

   displaysingleitem ( pos, x, y );
   if ( pos == cursorpos )
      putspriteimage ( x, y, mycursor.picture );
   getinvisiblemouserectanglestk ( );
}

int tladeraum :: mouseoverfield ( int pos )
{
   int x = x1 + 28 + (pos % itemsperline) * fielddistx;
   int y = y1 + 88 + (pos / itemsperline) * fielddisty * 3;
   return mouseinrect ( x, y, x+fieldsizex, y + fieldsizey );
}

char* tladeraum :: getinfotext( int pos ) 
{
   return NULL;
}

void tladeraum :: checkforadditionalkeys ( tkey ch )
{
}

void         tladeraum::displayinfotext ( void )
{
   npush ( activefontsettings );
   activefontsettings.background = lightgray;
   activefontsettings.color = red; 
   activefontsettings.length = 185;
   activefontsettings.font = schriften.smallarial;
   char* it = getinfotext( cursorpos );
   if ( it )
      showtext2(it, x1 + 240, y1 + 50);
   npop ( activefontsettings );
}

void         tladeraum::run(void)
{ 
   mousevisible( true );
   do { 
      tdialogbox::run();
      
      checkforadditionalkeys ( taste );
      int oldpos = cursorpos;
      if ((taste == ct_4k) || (taste == ct_6k) || (taste == ct_8k) || (taste == ct_2k)) {
         switch (taste) {
            case ct_8k:   cursorpos -= itemsperline;
               break;
            case ct_4k:   cursorpos--;
               break;
            case ct_6k:   cursorpos++;
               break;
            case ct_2k:   cursorpos += itemsperline;
               break;
         } 
         if ( cursorpos < 0 )
            cursorpos = 0;
         if ( cursorpos >= 32 )
            cursorpos = 31;
      } 
      if ( mouseparams.taste == 1) 
         for ( int i = 0; i < 32; i++ )
            if ( mouseoverfield ( i ))
               cursorpos = i;
       
      if ( cursorpos != oldpos ) {
         displaysingleitem ( oldpos );
         displaysingleitem ( cursorpos );
         displayinfotext();
      }                 

   }  while ( !action );
   finish ( action == 1 );

} 


void         tladeraum::buttonpressed( char id )
{ 
   if (id == 1) { 
      additem ();
      redraw();
   } 
   if (id == 2) { 
      removeitem ( cursorpos );
      displayallitems();
   } 
   if (id == 7) { 
      mapsaved = false;
      action = 2; 
   } 
   if (id == 8) 
      action = 1; 

} 


void         tladeraum::done(void)
{ 
   tdialogbox::done(); 
   npop ( farbwahl );
   ch = 0; 
} 




class tvehiclecargo : public tladeraum {
               protected:
                    pvehicle transport;
                    pvehicle orgvehicle;
                    virtual char* getinfotext ( int pos );
                    virtual void additem ( void );
                    virtual void removeitem ( int pos );
                    virtual void checkforadditionalkeys ( tkey ch );
                    void displaysingleitem ( int pos, int x, int y );
                    virtual void finish ( int cancel );

                public:
                    void init ( pvehicle unit );


};


char* tvehiclecargo :: getinfotext ( int pos )
{
   if ( transport->loading[ pos ] ) 
      if ( transport->loading[ pos ]->name && transport->loading[ pos ]->name[0] )
         return transport->loading[ pos ]->name;
      else
         if ( transport->loading[ pos ]->typ->name && transport->loading[ pos ]->typ->name[0] )
            return transport->loading[ pos ]->typ->name;
         else
            return transport->loading[ pos ]->typ->description;
   return NULL;
}


void tvehiclecargo :: init ( pvehicle unit )
{
   transport = new tvehicle ( unit, NULL ); 
   orgvehicle = unit;
   tladeraum::init ( "cargo" );
}

void tvehiclecargo :: displaysingleitem ( int pos, int x, int y )
{
   if ( transport->loading[ pos ] )
      putrotspriteimage ( x, y, transport->loading[ pos ]->typ->picture[0], farbwahl * 8 );
}

void tvehiclecargo :: additem  ( void )
{
   selunitcargo ( transport );
}

void tvehiclecargo :: removeitem ( int pos )
{
   if ( transport->loading[ pos ] )
      removevehicle ( &transport->loading[ pos ] );
}

void tvehiclecargo :: checkforadditionalkeys ( tkey ch )
{
   if ( transport->loading[ cursorpos ] ) {
       if ( ch == ct_v )  
          changeunitvalues( transport->loading[ cursorpos ] );
       if ( ch == ct_c )  
          unit_cargo( transport->loading[ cursorpos ] );
   }
} 


void tvehiclecargo :: finish ( int cancel )
{
   if ( !cancel ) 
      orgvehicle->clone ( transport, NULL );
   
   delete transport;
}


void         unit_cargo( pvehicle vh )
{ 
   if ( vh && vh->typ->loadcapacity ) {
      tvehiclecargo laderaum; 
      laderaum.init( vh ); 
      laderaum.run(); 
      laderaum.done(); 
   }
} 


//* õS Laderaum2 Building-Cargo

class tbuildingcargoprod : public tladeraum {
               protected:
                    pbuilding building;
                    pbuilding orgbuilding;
                    void finish ( int cancel );
                public:
                    void init ( pbuilding bld, char* title );
       };


void tbuildingcargoprod :: init ( pbuilding bld, char* title )
{
   building = bld; 
   orgbuilding = new tbuilding ( bld, actmap );
   tladeraum::init ( title );
}

void tbuildingcargoprod :: finish ( int cancel )
{
   if ( cancel ) {
      swapbuildings ( orgbuilding, building );
      removebuilding ( &orgbuilding );
   } else {
      removebuilding ( &orgbuilding );
   }
}

class tbuildingcargo : public tbuildingcargoprod {
               protected:
                    virtual char* getinfotext ( int pos );
                    virtual void additem ( void );
                    virtual void removeitem ( int pos );
                    virtual void checkforadditionalkeys ( tkey ch );
                    void displaysingleitem ( int pos, int x, int y );
   };



void tbuildingcargo :: displaysingleitem ( int pos, int x, int y )
{
   if ( building->loading[ pos ] )
      putrotspriteimage ( x, y, building->loading[ pos ]->typ->picture[0], farbwahl * 8 );
}

void tbuildingcargo :: additem  ( void )
{
   selbuildingcargo ( building );
}

void tbuildingcargo :: removeitem ( int pos )
{
   if ( building->loading[ pos ] )
      removevehicle ( &building->loading[ pos ] );
}

void tbuildingcargo :: checkforadditionalkeys ( tkey ch )
{
   if ( building->loading[ cursorpos ] ) {
       if ( ch == ct_v )  
          changeunitvalues( building->loading[ cursorpos ] );
       if ( ch == ct_c )  
          unit_cargo( building->loading[ cursorpos ] );
   }
} 

char* tbuildingcargo :: getinfotext ( int pos )
{
   if ( building->loading[ pos ] ) 
      if ( building->loading[ pos ]->name && building->loading[ pos ]->name[0] )
         return building->loading[ pos ]->name;
      else
         if ( building->loading[ pos ]->typ->name && building->loading[ pos ]->typ->name[0] )
            return building->loading[ pos ]->typ->name;
         else
            return building->loading[ pos ]->typ->description;
   return NULL;
}


void         building_cargo( pbuilding bld )
{
   if ( bld  ) {
      tbuildingcargo laderaum; 
      laderaum.init( bld, "cargo" ); 
      laderaum.run(); 
      laderaum.done(); 
   }
} 

//* õS Production Building-Production

class tbuildingproduction : public tbuildingcargoprod {
               protected:
                    virtual char* getinfotext ( int pos );
                    virtual void additem ( void );
                    virtual void removeitem ( int pos );
                    void displaysingleitem ( int pos, int x, int y );
   };

void tbuildingproduction :: displaysingleitem ( int pos, int x, int y )
{
   if ( building->production[ pos ] )
      putrotspriteimage ( x, y, building->production[ pos ]->picture[0], farbwahl * 8 );
}

void tbuildingproduction :: additem  ( void )
{
   selbuildingproduction ( building );
}

void tbuildingproduction :: removeitem ( int pos )
{
   building->production[ pos ] = NULL;
}

char* tbuildingproduction :: getinfotext ( int pos )
{
   if ( building->production[ pos ] ) 
      if ( building->production[ pos ]->name && building->production[ pos ]->name[0] )
         return building->production[ pos ]->name;
      else
         return building->production[ pos ]->description;
   return NULL;
}


void         building_production( pbuilding bld )
{
   if ( bld  && (bld->typ->special & cgvehicleproductionb ) ) {
      tbuildingproduction laderaum; 
      laderaum.init( bld, "production" ); 
      laderaum.run(); 
      laderaum.done(); 
   }
}


void movebuilding ( void )
{
   pfield fld = getactfield();
   if ( fld->vehicle ) {
      pvehicle v = fld->vehicle;
      fld->vehicle = NULL;
   
      int x = v->xpos;
      int y = v->ypos;
      int res2 = -1;
      do {
         res2 = selectfield ( &x, &y );
         if ( res2 > 0 ) {
            if ( getfield(x,y)->vehicle || getfield(x,y)->vehicle  || !terrainaccessible(getfield(x,y),v)) {
               displaymessage ( "you cannot place the vehicle here", 1 );
               res2 = -1;
            } else {
               getfield(x,y)->vehicle = v;
               v->setnewposition( x, y );
            }
         }
      } while ( res2 < 0  );
      displaymap();

   }
   if ( fld->building ) {
      pbuilding bld = fld->building;
 
      bld->unchainbuildingfromfield ();
   
      int x = bld->xpos;
      int y = bld->ypos;
      int res2 = -1;
      do {
         res2 = selectfield ( &x, &y );
         if ( res2 > 0 ) {
            int res = bld->chainbuildingtofield ( x, y );
            if ( res ) {
               displaymessage ( "you cannot place the building here", 1 );
               res2 = -1;
            }
         }
      } while ( res2 < 0  );
      if ( res2 == 0 ) {   // operation canceled
         if ( bld->chainbuildingtofield ( bld->xpos, bld->ypos ))
            displaymessage ( "severe inconsistency in movebuilding !", 1 );
      } 
      displaymap();
   }
}


