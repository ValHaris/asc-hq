//     $Id: artint.cpp,v 1.16 2000-08-07 21:10:17 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.15  2000/08/07 16:29:18  mbickel
//      orbiting units don't consume fuel any more
//      Fixed bug in attack formula; improved attack formula
//      Rewrote reactionfire
//
//     Revision 1.14  2000/08/04 15:10:46  mbickel
//      Moving transports costs movement for units inside
//      refuelled vehicles now have full movement in the same turn
//      terrain: negative attack / defensebonus allowed
//      new mapparameters that affect damaging and repairing of building
//
//     Revision 1.13  2000/08/03 13:11:47  mbickel
//      Fixed: on/off switching of generator vehicle produced endless amounts of energy
//      Repairing units now reduces their experience
//      negative attack- and defenseboni possible
//      changed attackformula
//
//     Revision 1.12  2000/08/02 15:52:37  mbickel
//      New unit set definition files
//      demount accepts now more than one container file
//      Unitset information dialog added
//
//     Revision 1.11  2000/07/29 14:54:07  mbickel
//      plain text configuration file implemented
//
//     Revision 1.10  2000/07/23 17:59:50  mbickel
//      various AI improvements
//      new terrain information window
//
//     Revision 1.9  2000/07/16 14:19:58  mbickel
//      AI has now some primitive tactics implemented
//      Some clean up
//        moved weapon functions to attack.cpp
//      Mount doesn't modify PCX files any more.
//
//     Revision 1.8  2000/07/06 11:07:24  mbickel
//      More AI work
//      Started modularizing the attack formula
//
//     Revision 1.7  2000/07/05 13:26:06  mbickel
//      AI
//
//     Revision 1.6  2000/07/05 10:49:35  mbickel
//      Fixed AI bugs
//      setbuildingdamage event now updates the screen
//
//     Revision 1.5  2000/06/28 18:30:56  mbickel
//      Started working on AI
//      Started making loaders independent of memory layout
//      Destroyed buildings can now leave objects behind.
//
//     Revision 1.4  2000/06/19 20:05:01  mbickel
//      Fixed crash when transfering ammo to vehicle with > 8 weapons
//
//     Revision 1.3  1999/11/22 18:26:43  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:41:00  tmwilson
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



#include <stdio.h>

#ifdef _DOS_
 #include <i86.h>
 #include <conio.h>
 #include <dos.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <malloc.h>
// #include <vector>

#include "artint.h"

#include "tpascal.inc"
#include "basegfx.h"
#include "misc.h"
#include "newfont.h"
#include "mousehnd.h"
#include "typen.h"
#include "keybp.h"
#include "spfst.h"
#include "dlg_box.h"
#include "stack.h"
#include "missions.h"
#include "controls.h"
#include "dialog.h"
#include "timer.h"
#include "gamedlg.h"
#include "building.h"
#include "attack.h"
#include "gameoptions.h"




/**********************************************************************************
      The first AI attempt, originally written in Pascal and never run after
      being converted to C . It is here to be cannibalized.
 **********************************************************************************

    typedef struct tattackableunit* pattackableunit;
    struct tattackableunit { 
                        pvehicle     attackingunit, attackedunit; 
                        int          xps, yps; 
                        int          xstart, ystart; 
                        int          distance; 
                        tattackresult result; 
                        int          weapon; 
                        int          priority; 
                        pattackableunit next; 
                     } ;


   typedef struct trequireunit* prequireunit;
   struct trequireunit {
                     int       id;
                     pvehicle  units;  // unit that is in need of something
                     int       need;
                     int       priority;
                     pvehicle  unitneeded;
                     prequireunit next;
                  } ;

   class tcmpsearchattackablevehicles : public tsearchattackablevehicles
                                 {             
                                      typedef tsearchattackablevehicles inherited;
                                    public:
                                      pattackableunit                   firstattackableunit;
                                      pattackableunit                   bestattack;    
                                      int                               completethreat,maxthreat;
                                      void             init ( pvehicle eht );
                                      void             testfield ( void ); 
                                      virtual void     run ( void );
                                      pattackableunit  getbestattack ( void );
                                      int              getpositionthreat ( void );
                                      int              getmaxthreat ( void );
                                      void             initattacksearch ( void );
                                      void             done ( void ); 
                                  };

   class tcmpgreifean : public tcmpsearchattackablevehicles {
                  typedef tcmpsearchattackablevehicles inherited;
                  public:
                     void run ( void );
                  };

  //  basis 256  
  #define cca_transport 512  
  #define cca_attackpriority 5    //  nenner 8  
  #define cca_killunit 12    //  basis 8  
  #define cca_cursordelay 20  
  #define cca_movecursor true  

  #define ccn_repair 1  
  #define ccn_refuel 2  
  #define ccn_remunitionier 4  


  #define ccm_waiting 10  
  #define ccm_secondattack 5  
  #define ccm_servicewait 8  


  #define cc_offensiv 30    //  0: eigene vehicle genausoviel wert wie gegnerische  
                            //  - 100: eigenen vehicle doppelt so viel wert  
                            //  100: eigene vehicle garnichts wert  

  #define cc_maxshortmovedist 30  

  #define ccbt_repairfacility 200    //  basic threatvalues for buildings  
  #define ccbt_hq 10000  
  #define ccbt_recycling 50  
  #define ccbt_training 150  

  #define ccs_refuel 6           //  basis 16; ab dieser grenze wird ein tankwagen angeheuert  
  #define ccs_remunitionier 5    //  basis 16; ab dieser grenze wird ein  munitionstransporter geordert  
  #define ccs_damageleave 5      //  basis 16; sofortige flucht zwecks reparatur;  
  #define ccs_damageord 8        //  basis 16; reperaturwagen wird angeheuert  

  #define unitstatusnum 4
  byte         unitstatus[4]  = {0, ccm_secondattack, ccm_servicewait, ccm_waiting}; 
  #define prnt false  

  #define ccat_reconquerbuilding 1  
  #define ccat_conquerbuilding 2  

  #define ccam_attackprirfak 26    // nenner 16 
     //  wenn vehicle^.typ^.wait, um wieviel muá attack-priorit„t nach movement gr”áer
     //  sein als sofortiger angriff  
  #define ccam_moveableunitthreatlossdecrease 8    // nenenr 16 
     //  falls sich eine vehicle vor dem angriff noch bewegen kann, um wieviel wird die
     // bedrohung vermindert. dadurch wird die bedrohung eines panzers verringert, da man
     // sich aus dessen schussreichweite entfernen kann, dieser vor einem angriff aber wieder
     // an die vehicle ranfahren kann.  





  boolean      threatvaluesgenerated; 
  byte         maxfusstruppenmove; 
  byte         maxtransportmove; 
  byte         maxweapdist; 
  byte         calcpass; 
  prequireunit orders[8]; 




void         beep2(void)
{ 
  integer      i; 

   for (i = 1; i <= 10; i++) { 
     //      sound(500 + i * 200); 
      ndelay(1); 
   } 
   //   nosound(); 
} 


void         beep3(void)
{ 
  integer      i; 

   for (i = 10; i >= 0; i--) { 
     //      sound(500 + i * 200); 
      ndelay(1); 
   } 
   //   nosound(); 
} 





void         showthreats(char *       s)
{ 

   displaymessage2(s); 
   displaymap(); 

   tkey         ch; 
   do { 
      ch = r_key(); 
      movecursor(ch); 
   }  while ( ch != ct_esc ); 
} 



void         generatethreatvalueunit(pvehicle     eht);


// boolean      fieldreachablelongdist(pvehicle     eht, integer      x2, integer      y2);

// void         movelongdist(pvehicle     eht, integer      x2, integer      y2);



void         initcomputerturn(void);



  struct tunits { 
              word         alliedunits[8]; 
              word         enemyunits[8]; 

              word         ownbuildings[8]; 
              word         enemybuildings[8]; 

              int      alliedthreats[8]; 
              int      enemythreats[8]; 

              int      alliedthreatpos[8]; 
              int      enemythreatpos[8]; 
           }; 
  typedef tunits *punits; 

  class tsearchsurroundingunits : public tsearchfields {
                               typedef tsearchfields inherited;
                         public:      
                               punits        units;
                               int           pass;
                               int           maxpasses;
                               void          testfield ( void );
                               void          init( punits p, int mxp );
                               int           unitposition( pvehicle eht, int  height );
                          };
  class tjugdesituationspfd : public tsearchsurroundingunits {
                           typedef tsearchsurroundingunits inherited;
                        public:
                           void  startsuche ( void );
                        };
  class tjugdesituationrect : public tsearchsurroundingunits {
                           typedef tsearchsurroundingunits inherited;
                         protected:
                           int  x1,y1,x2,y2;
                         public:
                           void init( punits p, int mxp, int x, int y, int rad );
                           void startsuche ( void );
                        };
  struct tposition { 
                 int          xpos, ypos; 
                 int          distance; 
                 int          threat; 
              }; 
  typedef tposition* pposition; 

  class tmoveshortdistance : public tsearchfields {
                          typedef tsearchfields inherited;
                      public:
                          pvehicle               vehicle;
                          int                    ehtoxp,ehtoyp;  //  original unit position 
                          pattackableunit        bestattack;
                          pattackableunit        firstattackableunit;
                          pposition              bestposition;
                          pattackableunit        pa,pb,pc;
                          int                    prirb;

                          void      init( pvehicle eht );
                          void      run ( void );
                          void      exec ( void );
                          void      testfield ( void );
                          void      done ( void );
                      };


void         changethreatvalue(pvehicle     eht,
                               int      value)
{ 


   eht->completethreatvalue += value; 
   eht->completethreatvaluesurr += value; 


 //  punits units = new tunits;
 //  tjugdesituationrect jsr; 
 //  jsr.init(units,1,eht->xpos,eht->ypos,10); 
 //  jsr.startsuche(); 
 //  jsr.done(); 

 //  delete units; 

} 


void         tjugdesituationrect :: init(punits       p,
                                      int             mxp,
                                      int             x,
                                      int             y,
                                      int             rad)
{ 
   inherited :: init( p, mxp );
   x1 = x - rad; 
   y1 = y - 2 * rad; 
   x2 = x + rad; 
   y2 = y + 2 * rad; 
   if (x1 < 0) 
      x1 = 0; 
   if (y1 < 0) 
      y1 = 0; 
   if (x2 >= actmap->xsize) 
      x2 = actmap->xsize - 1; 
   if (y2 >= actmap->ysize) 
      y2 = actmap->ysize - 1; 
} 


void         tjugdesituationrect :: startsuche(void)
{ 
   for (int p = 0; p <= maxpasses; p++) { 
      pass = p; 
      for (int xa = x1; xa <= x2; xa++) { 
         xp = xa; 
         for ( int ya = y1; ya <= y2; ya++) { 
            yp = ya; 
            testfield(); 
         } 
      } 
   } 
} 


void         tjugdesituationspfd :: startsuche(void)
{ 

   for ( int p = 0; p <= maxpasses; p++) { 
      pass = p; 
      for ( int x = 0; x <= actmap->xsize - 1; x++) { 
         xp = x; 
         for ( int y = 0; y <= actmap->ysize - 1; y++) { 
            yp = y; 
            testfield(); 
         } 
      } 
   } 
} 


void         tsearchsurroundingunits :: init( punits p, int mxp )
{ 
   memset (p, 0, sizeof(*p) ); 
   units = p; 
   pass = 0; 
   maxpasses = mxp; 
} 


void         tsearchsurroundingunits :: testfield(void)
{ 
  int      l; 
  pattackableunit pa; 

   if ((xp < actmap->xsize) && (xp >= 0) && (yp < actmap->ysize) && (yp >= 0)) { 
      pfield     fld = getfield(xp,yp); 
      pvehicle  eht = fld->vehicle; 
      pbuilding bld = fld->building; 
      if (pass == 0) { 
         if ( eht ) { 
            if (eht->completethreatvalue == 0) 
               generatethreatvalueunit( eht ); 
            if (getdiplomaticstatus(eht->color) == capeace) { 
               units->alliedunits[log2(eht->height)] ++; 
               units->alliedthreatpos[log2(eht->height)] += eht->completethreatvalue; 
               for ( int b = 0; b <= 7; b++) 
                  units->alliedthreats[b] += eht->threatvalue[b]; 
            } 
            else { 
               units->enemyunits[log2(eht->height)]++; 
               units->enemythreatpos[log2(eht->height)] += eht->completethreatvalue; 
               for ( int b = 0; b <= 7; b++) 
                  units->enemythreats[b] += eht->threatvalue[b]; 
            } 
         } 
         if ( bld ) 
            if (bld->color == actmap->actplayer * 8) 
               units->ownbuildings[log2(bld->typ->buildingheight)] ++; 
            else 
               if (getdiplomaticstatus(bld->color) == cawar) 
                  units->enemybuildings[log2(bld->typ->buildingheight)]++; 
      } 
      if (pass > 0) { 
         if ( eht ) { 
            tcmpsearchattackablevehicles csae;
            csae.init( eht ); 
            csae.run(); 
            pa = csae.getbestattack(); 
            if (pa == NULL) 
               l = 0; 
            else 
               l = pa->priority; 
            csae.done(); 
            if (l < 0) 
               l = 0; 
            l = l * cca_attackpriority / 8; 
            eht->completethreatvaluesurr = eht->completethreatvalue + l; 

         } 
      } 

   } 
} 

int      tsearchsurroundingunits :: unitposition(pvehicle     eht,
                                                     int          height)
{ 
  int      l, m, n, o; 
  double       ex1, ex2, ex3, ex4, ex5; 
  int b;

   char sh = 1 << height; 
   l = -units->enemythreats[height]; 

   o = 0; 
   for (b = 0; b <= 7; b++) { 
      if (units->alliedthreatpos[b] == 0) 
         units->alliedthreatpos[b] = 1; 
      n = units->enemythreats[b] * units->alliedthreatpos[b]; 
      o += n; 
   } 
   l += o; 

   o = 0; 
   m = 0; 
   for (b = 0; b <= 7; b++) { 
      //   if units^.alliedthreats[b] = 0 then
      //      units^.alliedthreats[b] :=1; 
      m += units->enemythreatpos[b]; 
      n += units->enemythreatpos[b] * eht->threatvalue[b]; 
      if (n > 0) 
         o += units->enemythreatpos[b]; 
   } 
   if (m != 0) { 
      ex4 = l; 
      ex2 = n; 
      ex3 = o; 
      ex5 = m; 
      ex1 = ex4 + ex2 * ex3 / ex5; 
      if (ex1 > maxint ) 
         l = maxint ; 
      else 
         l = (int)ex1; 
   } 
   else 
      if (l > 0) 
         l = 0; 

   m = 0; 
   for (b = 0; b <= 7; b++) { 
      n = units->enemythreats[height] * eht->completethreatvalue; 
      m += n; 
   } 

   l -= m; 
   return l; 
} 


int      getposval(void)
{ 
  tjugdesituationrect   ssu; 
  tunits                units; 

   if (moveparams.movestatus == 0) { 
      moveparams.vehicletomove = getactfield()->vehicle; 
      moveparams.movestatus = 222; 
      initcomputerturn(); 
      return -11; 
   } 
   else { 
      ssu.init( &units, 1, getxpos(), getypos(), 5); 
      ssu.startsuche(); 
      int temp = ssu.unitposition(moveparams.vehicletomove,log2(moveparams.vehicletomove->height)); 
      ssu.done(); 
      return temp;
   } 
} 


  class tgeneratethreatvaluefzt {
                         protected:
                              pvehicletype         fzt;
                              tthreatvar           weapthreatvalue[8];
                              tthreatvar           threatvalue2;
                              tthreatvar           generalthreatvalue;

                              virtual int       getdamage ( void );
                              virtual int       getexpirience ( void );
                              virtual int       getammunition( int i );
                              virtual int       getheight ( void );
                          public:
                              void              generatethreatvalue ( void );
                              void              gtvfzt ( void );
                       };

  class tgeneratethreatvalueunit : public tgeneratethreatvaluefzt {
                                typedef tgeneratethreatvaluefzt inherited;
                           protected:
                                pvehicle          eht;
                                virtual int       getdamage ( void );
                                virtual int       getexpirience ( void );
                                virtual int       getammunition( int i );
                                virtual int       getheight ( void );
                           public:
                                void              gtveht( pvehicle vehicle );
                       };



int          tgeneratethreatvaluefzt :: getammunition( int      i)
{ 
   return 1; 
} 


void         tgeneratethreatvaluefzt :: gtvfzt(void)
{ 

   for ( int w = 0; w < vehicletypenum; w++) { 
      fzt = getvehicletype_forpos(w); 
      if ( fzt ) {
         generatethreatvalue(); 
   
   
      //   if (prnt) { 
      //      fprintf(fzt->id,""); 
      //      fprintf(fzt->description,""); 
      //   } 
   
         for ( int b = 0; b <= 7; b++) { 
            fzt->threatvalue[b] = weapthreatvalue[b]; 
      //      if (prnt) 
      //         fprintf(weapthreatvalue[b],""); 
         } 
         fzt->generalthreatvalue = threatvalue2; 
      //   if (prnt) 
      //      fprintf(threatvalue2,"\n"); 
   
      }
   } 

} 


int          tgeneratethreatvaluefzt :: getheight(void)
{ 
   return 255; 
} 


int          tgeneratethreatvaluefzt :: getdamage(void)
{ 
   return 0; 
} 


int          tgeneratethreatvaluefzt :: getexpirience(void)
{ 
   return 0; 
} 



void         tgeneratethreatvaluefzt :: generatethreatvalue(void)
{ 

   int a = fzt->weapons->count;
   memset ( &weapthreatvalue, 0, sizeof(weapthreatvalue) ); 

   int m = 0; 
   if (a > 0) { 
      for ( int i = 0; i <= a - 1; i++) 
         if (fzt->weapons->weapon[i].typ & cwshootableb > 0) 
            if (fzt->weapons->weapon[i].typ && cwweapon > 0) 
               for ( int j = 0; j <= 7; j++) 
                  if (fzt->weapons->weapon[i].targ & (1 << j) > 0) { 
                     int d = 0; 
                     m = 0; 
                     int e = fzt->weapons->weapon[i].mindistance; 
                     while (e < fzt->weapons->weapon[i].maxdistance) { 
                        d++; 
                        int n = weapdist->getweapstrength( &fzt->weapons->weapon[i], e ) * fzt->weapons->weapon[i].maxstrength * attackstrength(getdamage()) * (getexpirience() + 8) / (256 * 100 * 8);
                        m += n; 
                        e += 8; 
                     } 
                     if (getammunition(i) == 0) 
                        m /= 2; 
                     if ( (fzt->weapons->weapon[i].sourceheight & getheight()) == 0) 
                        m /= 2; 
                     if (d > 1) 
                        m = 2 * m / d; 
                     if (m > weapthreatvalue[j]) 
                        weapthreatvalue[j] = m; 
                  } 
   } 

   threatvalue2 = fzt->armor * (100 - getdamage()) / 100; 

} 


int          tgeneratethreatvalueunit :: getammunition( int         i)
{ 
   return eht->munition[i]; 
} 


int          tgeneratethreatvalueunit :: getheight(void)
{ 
   return eht->height; 
} 


int          tgeneratethreatvalueunit :: getdamage(void)
{ 
   return eht->damage; 
} 


int          tgeneratethreatvalueunit :: getexpirience(void)
{ 
   return eht->experience; 
} 


void         tgeneratethreatvalueunit :: gtveht(pvehicle     vehicle)
{ 

   eht = vehicle; 
   fzt = eht->typ; 
   generatethreatvalue(); 
   int l = 0; 
   for ( int b = 0; b <= 7; b++) { 
      eht->threatvalue[b] = weapthreatvalue[b]; 
      if (weapthreatvalue[b] > l) 
         l = weapthreatvalue[b]; 
   } 
   eht->completethreatvalue = threatvalue2 + l; 
   eht->completethreatvaluesurr = threatvalue2 + l; 
   eht->threats = 0; 
} 




void         generatethreatvalueunit(pvehicle     eht)
{ 
   tgeneratethreatvalueunit gtvu; 

   gtvu.init(); 
   gtvu.gtveht(eht); 
   gtvu.done(); 
} 


void         generatethreatvaluebuilding(pbuilding    bld)
{ 
   int b; 

   bld->threatvalue = (bld->plus.a.energy / 10) + (bld->plus.a.fuel / 10) + (bld->plus.a.material / 10) + (bld->actstorage.a.energy / 20) + (bld->actstorage.a.fuel / 20) + (bld->actstorage.a.material / 20) + (bld->maxresearchpoints / 10); 
   for (b = 0; b <= 31; b++) 
      if ( bld->loading[b] )
         bld->threatvalue += bld->loading[b]->completethreatvalue; 
   for (b = 0; b <= 31; b++) 
      if ( bld->production[b] ) 
         bld->threatvalue += bld->production[b]->generalthreatvalue / 10; 
   if (bld->typ->special & cgrepairfacilityb ) 
      bld->threatvalue += ccbt_repairfacility; 
   if (bld->typ->special & cghqb ) 
      bld->threatvalue += ccbt_hq; 
   if (bld->typ->special & cgtrainingb ) 
      bld->threatvalue += ccbt_training; 
   if (bld->typ->special & cgrecyclingplantb ) 
      bld->threatvalue += ccbt_recycling; 
} 



void         generatethreatvalues( void )
{ 
   int v;

   for (v = 0; v <= 8; v++) 
      if (actmap->player[v].existent) { 
         pvehicle eht = actmap->player[v].firstvehicle; 
         while ( eht ) { 
            memset( &eht->threatvalue, 0, sizeof(eht->threatvalue) ); 
            eht->completethreatvalue = 0; 
            eht = eht->next; 
         } 
      } 

   if ( !threatvaluesgenerated ) { 
      tgeneratethreatvaluefzt gtvf; 
      gtvf.init(); 
      gtvf.gtvfzt(); 
      gtvf.done(); 
      threatvaluesgenerated = true; 
   } 
   if (maxfusstruppenmove == 0) { 
      for (v = 0; v < vehicletypenum; v++) {
         pvehicletype fzt = getvehicletype_forpos( v );
         if ( fzt )
            if ( fzt->functions & ( cf_conquer ) ) 
               if ( fzt->movement[chfahrend] > maxfusstruppenmove )
                  maxfusstruppenmove = fzt->movement[3]; 
      }
   } 
   if ( maxtransportmove == 0 ) { 
      for (v = 0; v < vehicletypenum; v++) {
         pvehicletype fzt = getvehicletype_forpos( v );
         if ( fzt )
            for ( int w = 0; w <= 7; w++) 
               if (fzt->movement[w] > maxtransportmove) 
                  maxtransportmove = fzt->movement[w]; 
      }
   } 
   if (maxweapdist == 0) { 
      for (v = 0; v < vehicletypenum; v++) {
         pvehicletype fzt = getvehicletype_forpos( v );
         if ( fzt )
            if ( fzt->weapons->count > 0) 
               for ( int w = 0; w < fzt->weapons->count ; w++) 
                  if (fzt->weapons->weapon[w].maxdistance > maxweapdist) 
                     maxweapdist = fzt->weapons->weapon[w].maxdistance; 
      }
   } 
} 

void         tcmpsearchattackablevehicles :: init(  pvehicle eht  )
{ 
   bestattack = NULL; 
   firstattackableunit = NULL; 
   inherited :: init(eht);
   messages = false; 
   maxthreat = 0; 
   completethreat = 0; 
} 


pattackableunit tcmpsearchattackablevehicles :: getbestattack ( void )
{ 
   return bestattack; 
} 


int          tcmpsearchattackablevehicles :: getmaxthreat ( void )
{ 
   return maxthreat; 
} 


int          tcmpsearchattackablevehicles :: getpositionthreat (void)
{ 
   if ( bestattack ) 
      return bestattack->priority; 
   else 
      return maxthreat; 
} 


void         tcmpsearchattackablevehicles :: testfield(void)
{ 

   byte         lastweapnum; 
   byte         lastweapstrength; 

   byte         killfactor; 
   int      prira, prirb; 

  pvehicle     attackingunit; 
  int      threatloss; 




   pfield fld2 = getfield(xp,yp); 
   if ( fieldvisiblenow( fld2 ) ) { 
      npush( actmap->actplayer ); 
      actmap->actplayer = angreifer->color / 8; 
      pattackweap  atw = attackpossible(angreifer,xp,yp); 
      npop( actmap->actplayer ); 
      if (atw->count > 0) { 
         if ( fld2->vehicle ) { 
            pfield fld1 = getfield(startx,starty); 
              // ****************************************************************
            ÿ //                          eigenen angriff testen
            ÿ // ****************************************************************
            
            battle.calc(fld1, xp, yp, atw->num[0] ); 
            tattackresult attackresult = battle.attackresult; 
            lastweapnum = 0; 
            if (attackresult.ddamage >= 100) 
               if (atw->count > 1) { 
                  lastweapstrength = atw->strength[0]; 
                  for (int i = 1; i < atw->count ; i++) { 
                     battle.calc(fld1,xp,yp,atw->num[i]); 
                     tattackresult attackresult2 = battle.attackresult; 
                     if (attackresult2.ddamage >= 100) 
                        if (atw->strength[i] < lastweapstrength) { 
                           lastweapstrength = atw->strength[i]; 
                           lastweapnum = i; 
                        } 
                  } 
                  battle.calc( fld1, xp, yp, atw->num[lastweapnum] ); 
                  attackresult = battle.attackresult; 
               } 
            pattackableunit pau = firstattackableunit; 
            firstattackableunit = new tattackableunit;
            firstattackableunit->weapon = atw->num[lastweapnum]; 
            attackingunit = angreifer; 
            firstattackableunit->attackedunit = fld2->vehicle; 
            firstattackableunit->distance = beeline(startx,starty,xp,yp); 
            if (attackresult.ddamage >= 100) 
               killfactor = cca_killunit; 
            else 
               killfactor = 8; 

            if ( attackresult.ddifferenz > 0 ) 
               prira = firstattackableunit->attackedunit->completethreatvaluesurr * attackresult.ddifferenz * killfactor / 800; 
            else 
               prira = 0; 

            if ( attackresult.adifferenz > 0 ) 
               prirb = (attackingunit->completethreatvaluesurr * attackresult.adifferenz / 100) * (100 - cc_offensiv) / 100; 
            else 
               prirb = 0; 

            firstattackableunit->priority = prira - prirb; 

            firstattackableunit->xps = xp; 
            firstattackableunit->yps = yp; 
            firstattackableunit->xstart = angreifer->xpos; 
            firstattackableunit->ystart = angreifer->ypos; 

            firstattackableunit->result = attackresult; 

            firstattackableunit->next = pau; 
             
            
         } 
      } 
      delete atw;


      // ****************************************************************
      //ÿ                   gegnerischen angriff testen
      // ****************************************************************
      attackingunit = fld2->vehicle; 
      if ( attackingunit ) { 
         
         npush( actmap->actplayer ); 
         actmap->actplayer = attackingunit->color / 8; 
         pattackweap  atw = attackpossible(attackingunit,startx,starty); 
         npop( actmap->actplayer ); 
         if (atw->count > 0) { 
            battle.calc( fld2, startx, starty, getstrongestweapon(attackingunit,angreifer->height,beeline(xp,yp,startx,starty))); 
            threatloss = battle.attackresult.ddifferenz * angreifer->completethreatvalue / 100 - battle.attackresult.adifferenz * attackingunit->completethreatvalue / 100; 
            if (threatloss < 0) 
               threatloss = 0; 
            if ( (attackingunit->movement > 0) && (attackingunit->typ->wait == false)) 
               threatloss *= ccam_moveableunitthreatlossdecrease / 16; 
            if ( threatloss > maxthreat ) 
               maxthreat = threatloss; 
            completethreat += threatloss; 
         } 
         delete atw; 
         
      } 
   } 
} 


void         tcmpsearchattackablevehicles :: initattacksearch ( void )
{ 
   anzahlgegner = 0; 

   if (angreifer == NULL) 
      return;

   moveparams.movesx = angreifer->xpos; 
   moveparams.movesy = angreifer->ypos; 

   attackposs = true; 
   initsuche(angreifer->xpos,angreifer->ypos,maxweapdist / 8 + 1,0); 
} 


void         tcmpsearchattackablevehicles :: run(void)
{ 
   int      prir = 0; 

   inherited :: run();

   pattackableunit pau2 = firstattackableunit; 
   while ( pau2 ) { 
      if (prir < pau2->priority) { 
         prir = pau2->priority; 
         bestattack = pau2; 
      } 
      pau2 = pau2->next; 
   } 

   maxthreat *= (100 - cc_offensiv) / 100; 

   if ( bestattack ) 
      bestattack->priority = bestattack->priority - maxthreat; 
} 


int      getposthreat(pvehicle     eht,
                      word         x,
                      word         y)
{ 

   npush( eht->xpos ); 
   npush( eht->ypos ); 


   tcmpsearchattackablevehicles csae; 
   csae.init(eht); 
   csae.run(); 
   int temp = csae.getpositionthreat(); 
   csae.done(); 

   npop( eht->ypos ); 
   npop( eht->xpos ); 

   return temp;
} 


void         attackunit(pattackableunit bestattack)
{ 
   if ( bestattack ) 
      if ( bestattack->attackingunit  &&  bestattack->attackedunit ) { 
         pvehicle eht = bestattack->attackingunit; 
         if ( eht->typ->wait ) 
            if (eht->movement != eht->typ->movement[log2(eht->height)]) 
               return;
         if ( eht->attacked ) 
            return;

         pfield fld = getfield(eht->xpos,eht->ypos); 
         if ( cca_movecursor ) { 
            cursor.gotoxy(eht->xpos,eht->ypos); 
            ndelay(cca_cursordelay); 
            cursor.setcolor(8); 
            cursor.gotoxy(bestattack->xps,bestattack->yps); 
            cursor.setcolor(0); 
            ndelay(cca_cursordelay); 
         } 
         // battle.fight( fld, bestattack->xps, bestattack->yps, bestattack->weapon ); 
         eht->attacked = true; 
         eht->movement = 0; 
         eht->cmpchecked = 255; 
      } 
} 


void         tcmpgreifean :: run(void)
{ 
   inherited :: run();
   attackunit( bestattack ); 
} 



void         tcmpsearchattackablevehicles :: done(void)
{ 

   while ( firstattackableunit ) { 
      pattackableunit pau = firstattackableunit; 
      firstattackableunit = firstattackableunit->next; 
      delete pau; 
   } 
   inherited :: done(); 
} 



   

  class  tcmpcheckreconquerbuilding : public tsearchfields {
                                   typedef tsearchfields inherited;
                                protected:
                                   int mode;        // (1): nur fusstruppen; (2): 1 und transporter; (3): 2 und geb„ude
                                   int unitnumber;
                                   int buildingthreatvalue;
                                public:
                                   void init( int md );
                                   void testfield ( void );
                                   void startsuche ( void );
                                   void returnresult ( int* a );
                                   void unitfound ( pvehicle eht );
                                };
        

void         tcmpcheckreconquerbuilding :: startsuche(void)
{ 
   buildingthreatvalue = getfield(startx,starty)->building->threatvalue; 
   inherited :: startsuche();
} 


void         tcmpcheckreconquerbuilding :: unitfound(pvehicle     eht)
{ 
   if (( eht->threats & ccat_reconquerbuilding) == 0) {
      changethreatvalue ( eht, buildingthreatvalue );
      eht->threats |= ccat_reconquerbuilding;
   } 
   unitnumber++;
} 


void         tcmpcheckreconquerbuilding :: testfield(void)
{ 
   if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) { 
      pvehicle eht = getfield(xp,yp)->vehicle; 
      pbuilding bld = getfield(xp,yp)->building; 
      if ( eht )
         if (getdiplomaticstatus(eht->color) != capeace) { 
            if ( eht->functions & ( cf_conquer ) ) { 
               fieldreachablerek(startx,starty,eht,2); 
               if ( moveparams.movedist != 0 )
                  unitfound(eht); 
            } 
            else 
               if (mode >= 2) { 
                  if (eht->typ->loadcapacity > 0) { 
                     if (artintconfig.lookintotransports) { 
                        for ( int w = 0; w <= 31; w++) {
                           if ( eht->loading[w] )
                              if ( eht->loading[w]->functions & ( cf_conquer ) )
                                 if (eht->typ->movement[log2(eht->height)] + eht->loading[w]->typ->movement[3] >= beeline(xp,yp,startx,starty)) 
                                    unitfound(eht); 
                        } 
                     } 
                     else 
                        if (eht->typ->movement[log2(eht->height)] + maxfusstruppenmove <= beeline(xp,yp,startx,starty)) 
                           unitfound(eht); 
                  } 
               } 
         } 
      if ( bld )
         if ((mode >= 3) && artintconfig.lookintobuildings) 
            if (getdiplomaticstatus(bld->color) != capeace) 
               for ( int w = 0; w <= 31; w++) {
                  if ( bld->loading[w] )
                     if ( bld->loading[w]->functions & ( cf_conquer ) )
                        if (bld->loading[w]->typ->movement[3] <= beeline(xp,yp,startx,starty)) { 
                           unitnumber++;
                        } 
               } 
   } 
} 



void         tcmpcheckreconquerbuilding :: init( int md )
{ 
   mode = md; 
   unitnumber = 0; 
} 


void         tcmpcheckreconquerbuilding :: returnresult( int*       a )
{ 
   *a = unitnumber; 
} 


void         checkconquerbuilding(pvehicle     eht)
{ 
  int i;
   for (i = 0; i <= 8; i++)
      if ( actmap->player[i].existent )
         if (getdiplomaticstatus(i * 8) != capeace) { 
            pbuilding bld = actmap->player[i].firstbuilding; 
            if ( eht->cmpchecked != 255 )
               while ( bld ) {
                  if (beeline(bld->xpos,bld->ypos,eht->xpos,eht->ypos) <= eht->movement) 
                     if (fieldaccessible(getfield(bld->xpos,bld->ypos),eht) == 2) { 
                        fieldreachablerek(bld->xpos,bld->ypos,eht,2); 
                        if (moveparams.movedist != 0) { 
                           moveparams.movesx = eht->xpos; 
                           moveparams.movesy = eht->ypos; 
                           moveparams.vehicletomove = eht; 
                           moveparams.uheight = -1;

                           pfield fld = getfield( eht->xpos, eht->ypos );
                           npush( fld->vehicle );
                           fld->vehicle = NULL;
                           npush( moveparams );

                           tcmpcheckreconquerbuilding ccrcb;
                           int j;
                           ccrcb.init(3); 
                           ccrcb.initsuche(bld->xpos,bld->ypos,(maxfusstruppenmove + maxtransportmove) / 8 + 1,0); 
                           ccrcb.startsuche(); 
                           ccrcb.returnresult( &j );
                           ccrcb.done(); 

                           npop( moveparams );
                           npop( fld->vehicle );



                           if (j > 0) 
                              if ((bld->color == 64) || artintconfig.lookintobuildings) { 
                                 int loadval = 0;
                                 for ( int k = 0; k <= 31; k++)
                                    if ( bld->loading[k] ) {
                                       if ( bld->loading[k]->completethreatvalue == 0)
                                          generatethreatvalueunit( bld->loading[k] );
                                       loadval += bld->loading[k]->completethreatvalue;
                                    } 
                                 if (eht->completethreatvalue < loadval) 
                                    j = 0; 
                              } 
                           if (j == 0) { 
                              moveparams.movestatus = 2; 
                              cursor.gotoxy(bld->xpos,bld->ypos); 
                              getfield(bld->xpos,bld->ypos)->a.temp = 1; 
                              movement(moveparams.vehicletomove); 
                              eht->cmpchecked = 255; 
                           } 
                           else 
                              eht->cmpchecked = ccm_waiting; 

                        } 
                     } 
                  bld = bld->next; 
               } 
         } 

   if ((eht->cmpchecked != 255) && (eht->cmpchecked != ccm_waiting)) { 

     struct {
                   pbuilding    bld; 
                   int      prir; 
                } bestbuilding; 

      bestbuilding.prir = 0; 
      for (i = 0; i <= 8; i++) 
         if (actmap->player[i].existent) 
            if (getdiplomaticstatus(i * 8) != capeace) { 
               pbuilding bld = actmap->player[i].firstbuilding; 
               while (bld != NULL) { 
                  if (bld->threatvalue == 0) 
                     generatethreatvaluebuilding(bld); 
                   
                  if (fieldreachablelongdist(eht,bld->xpos,bld->ypos) && 0) { 
                     int p = bld->threatvalue * 16 / beeline(bld->xpos,bld->ypos,eht->xpos,eht->ypos);
                     if ( p > bestbuilding.prir ) {
                        bestbuilding.prir = p; 
                        bestbuilding.bld = bld; 
                     } 
                  } 
                  
                  bld = bld->next; 
               } 
            } 
      if (bestbuilding.prir > 0) { 
         //movelongdist(eht,bestbuilding.bld->xpos,bestbuilding.bld->ypos); 
      } 

   } 
} 



void         checkattack( pvehicle     eht)
{ 

   if (eht == getfield(eht->xpos,eht->ypos)->vehicle) { 
      if ((eht->attacked == false) && ((eht->typ->wait == false) || (eht->movement == eht->typ->movement[log2(eht->height)]))) { 
         cursor.gotoxy(eht->xpos,eht->ypos); 
         tcmpgreifean cmpattack;
         cmpattack.init(eht); 
         cmpattack.run(); 
         cmpattack.done(); 
      } 
   } 
} 



      typedef byte trichtungen[8]; 


      trichtungen  directions[3][3]  = {{{7, 6, 0, 5, 1, 4, 2, 3}, {0, 1, 7, 2, 6, 3, 5, 4}, {1, 0, 2, 7, 3, 6, 4, 5}}, 
                                                    {{6, 7, 5, 0, 4, 1, 3, 2}, {0, 1, 2, 3, 4, 5, 6, 7}, {2, 1, 3, 0, 4, 7, 5, 6}}, 
                                                    {{5, 6, 4, 7, 3, 0, 2, 1}, {4, 3, 5, 2, 6, 1, 7, 0}, {3, 2, 4, 1, 5, 0, 6, 7}}}; 

trichtungen  dir2  = {0, 1, 7, 2, 6, 3, 5, 4}; 


  typedef struct tmovepathpoint { 
                       integer      posx, posy; 
                       int      dist; 
                    } tmovepathpoint; 
  /* 
  tlongdistmove : public tsucheerreichbarefielder)
                     field:pfield;
                     destx,desty:integer;
                     suchtiefe:byte;
                     zielerreicht:boolean;
                     actdir:byte;
                     x,y:integer;
                     status:byte;
                     show:boolean;
                     delaytime:byte;
                     fieldpasses:int;
                     
                     bestx,besty,bestnum:integer;
                     procedure init;
                     procedure teilestrecke(x1,y1,x2,y2:integer);
                     function passmarkedline:boolean;
                     procedure markline(xx1,yy1,xx2,yy2:integer; mark:int);
                     procedure run(eht:pvehicle; x2,y2:integer);
                     procedure move;
                     procedure testfield(x1,y1:integer); virtual;
                     procedure showresult;
                  end;
   

void         tlongdistmove :: init(void)
{ 
   suchtiefe = 0; 
   show = false; 
   delaytime = 20; 
   fieldpasses = 0; 
   bestx = -1; 
   besty = -1; 
   bestnum = 0; 
} 


void         tlongdistmove :: testfield(integer      x1,
                                    integer      y1)
{ 
  pfield        fld; 

   fld = getfield(x1,y1); 
   if (fld != NULL) 
      if (fld->temp2 > bestnum) { 
         bestnum = fld->temp2; 
         bestx = x1; 
         besty = y1; 
      } 
} 


void         tlongdistmove :: run(void)
{ 
   vehicle = eht; 
   startx = eht->xpos; 
   starty = eht->ypos; 
   destx = x2; 
   desty = y2; 
   teilestrecke(startx,starty,destx,desty); 
} 


void         tlongdistmove :: move(void)
{ 
  pfield        fld; 

   cleartemps(1); 
   if (status == 12) { 
        //* beep; 
      start(); 
      if (bestnum > 0) { 
           //* beep; 
         moveparams.movestatus = 1; 
         moveparams.movesx = startx; 
         moveparams.movesy = starty; 
         moveparams.vehicletomove = vehicle; 
         moveparams.uheight = -1;
         cursor.gotoxy(bestx,besty); 
         movement(vehicle); 
         fld = getfield(moveparams.movepath.field[moveparams.movepath.tiefe].x,moveparams.movepath.field[moveparams.movepath.tiefe].y); 
         while (((fld->vehicle != NULL) || (fld->building != NULL)) && (moveparams.movepath.tiefe > 1)) { 
            dec(moveparams.movepath.tiefe); 
            fld = getfield(moveparams.movepath.field[moveparams.movepath.tiefe].x,moveparams.movepath.field[moveparams.movepath.tiefe].y); 
         } 
         cursor.gotoxy(moveparams.movepath.field[moveparams.movepath.tiefe].x,moveparams.movepath.field[moveparams.movepath.tiefe].y); 
         movement(vehicle); 
         if (moveparams.movestatus == 2) 
            movement(vehicle); 
      } 
   } 
   else { 
   } 
} 



trichtungen  dir3[3]  = {{4, 5, 11, 6, 10, 7, 9, 8}, {1, 2, 3, 4, 5, 6, 7, 0}, {7, 6, 5, 4, 3, 2, 1, 0}}; 


void         tlongdistmove :: markline(int *        xx1,
                                    int *        yy1,
                                    int *        xx2,
                                    int *        yy2,
                                    long *       mark)
{ 
  integer      xx3, yy3; 
  byte         a; 
  pfield        field2; 

   xx3 = xx1; 
   yy3 = yy1; 
   do { 
      a = getdirection(xx3,yy3,xx2,yy2); 
      getnextfield(xx3,yy3,a); 
      field2 = getfield(xx3,yy3); 
      field2->special = field2->special | mark; 
   }  while (!((xx3 == xx2) && (yy3 == yy2))); 
} 


boolean      tlongdistmove :: passmarkedline(void)
{ 
  byte         rightdir, t; 
  shortint     leftdir; 
  integer      x3, y3; 
  pfield        fld; 
  boolean      result; 
  byte         d; 

   result = false; 
     //*  berprfen, ob die mit cbmovetempv gezogene beeline berquert wird  
   if (field->special && (1 << cbmovetempv) > 0) 
      result = true; 
   else 
      if (actdir && 1 == 0) { 
         d = 0; 
         leftdir = -1 + actdir; 
         if (leftdir < 0) 
            leftdir = leftdir + 8; 
         rightdir = 1 + actdir; 
         if (rightdir > 7) 
            rightdir = rightdir - 8; 
         x3 = x; 
         y3 = y; 
         getnextfield(x3,y3,leftdir); 
         fld = getfield(x3,y3); 
         if (fld != NULL) 
            if (fld->special & cbmovetempb > 0) 
               inc(d); 
         x3 = x; 
         y3 = y; 
         getnextfield(x3,y3,rightdir); 
         fld = getfield(x3,y3); 
         if (fld != NULL) 
            if (fld->special & (1 << cbmovetempv) > 0) 
               inc(d); 

         if (d == 2) 
            result = true; 
      } 
   passmarkedline = result; 
} 


void         tlongdistmove :: teilestrecke(void)
{ 
  integer      xt, yt; 
  byte         a, d, e; 
  shortint     c; 
  byte         movedir; 
  byte         lastdir; 
  byte         pass; 
  byte         destdir; 
  tkey         tk; 
  shortint     cstart; 


   cstart = 0; 
   if (fieldaccessible(getfield(x2,y2),vehicle) < 2) 
      status = 11; 
   else 
      status = 0; 
   { void *with1 = &activefontsettings; 
      font = schriften.smallarial; 
      background = 0; 
      justify = lefttext; 
      color = 1; 
      return 130; 
      } 
   
   markline(x1,y1,x2,y2,1 << cbmovetempv); 

   pass = 10; 
   x = x1; 
   y = y1; 
   movedir = 0; 
   lastdir = getdirection(x,y,x2,y2); 
   while (status <= 10) { 
      xt = x; 
      yt = y; 
      destdir = getdirection(x,y,x2,y2); 

      inc(fieldpasses); 

      getnextfield(xt,yt,destdir); 
      field = getfield(xt,yt); 
      if (field == NULL) { 
         inc(pass); 
         d = 0; 
      } 
      else { 
         d = fieldaccessible(field,vehicle); 
      } 

      if ((d > 0) && (movedir == 0)) { 
         x = xt; 
         y = yt; 
         inc(field->temp); 
         if (fieldpasses > high(field->temp2)) 
            field->temp2 = high(field->temp2); 
         else 
            field->temp2 = fieldpasses; 

         if (show) { 
            cursor.gotoxy(xt,yt); 
            displaymap(); 
            showtext2(scat("movedir: %s",strr(movedir)),500,100); 
            showtext2(scat("field^.temp: %s",strr(field->temp)),500,120); 
            showtext2(scat("pass: %s",strr(pass)),500,140); 
            ndelay(delaytime); 
            if (keypress) { 
               tk = r_key; 
               if (tk == ct_plus) 
                  delaytime+=10; 
               if (tk == ct_minus) 
                  if (delaytime >= 10) 
                     delaytime-=10; 
               if (tk == ct_esc) 
                  status = 11; 
            } 
         } 
         lastdir = destdir; 
         movedir = 0; 
      } 
      else { 
         if (movedir == 0) { 
            c = -1; 
            lastdir = destdir; 
         } 
         else 
            c = 0; 
         do { 
            inc(c); 
            actdir = dir3[movedir][c] + lastdir + 4;   //* destdir 
            while (actdir > 7) 
               actdir = actdir - 8; 

            xt = x; 
            yt = y; 

            getnextfield(xt,yt,actdir); 
            field = getfield(xt,yt); 
            d = fieldaccessible(field,vehicle); 
         }  while (!((c > 7) || (d > 0))); 

         if (d > 0) { 
            if (movedir == 0) { 
               c = lastdir; 
               e = 0; 
               while (c != actdir) { 
                  inc(c); 
                  inc(e); 
                  if (c > 7) 
                     c = c - 8; 
               } 
               if (e > 4) 
                    //*  linkskurve  
                  movedir = 2; 
               else 
                  movedir = 1;   //*  rechtskurve  
            } 
            else { 
                 //* (actdir  = destdir) or 
                  if (passmarkedline) 
                  if (field->temp < 2) { 
                     movedir = 0; 
                     if (show) 
                        beep(); 
                  } 
            } 
            x = xt; 
            y = yt; 
            inc(field->temp); 
            if (fieldpasses > high(field->temp2)) 
               field->temp2 = high(field->temp2); 
            else 
               field->temp2 = fieldpasses; 
            if (show) { 
               cursor.gotoxy(xt,yt); 
               displaymap(); 
               showtext2(scat("movedir: %s",strr(movedir)),500,100); 
               showtext2(scat("field^.temp: %s",strr(field->temp)),500,120); 
               showtext2(scat("pass: %s",strr(pass)),500,140); 
               ndelay(delaytime); 
               if (keypress) { 
                  tk = r_key; 
                  if (tk == ct_plus) 
                     delaytime+=10; 
                  if (tk == ct_minus) 
                     if (delaytime >= 10) 
                        delaytime-=10; 
                  if (tk == ct_esc) 
                     status = 11; 
               } 

            } 
            lastdir = actdir; 
         } 
      } 

      if (field->temp > 6) 
         status = 11; 

      if ((x == x2) && (y == y2)) 
         status = 12; 

   } 
} 


void         tlongdistmove :: showresult(void)
{ 
   if (status == 12) 
      displaymessage("ziel erreichtbar",scat("%s fields passed",strr(fieldpasses)),1); 
   else 
      displaymessage("ziel nicht erreichbar",scat("%s fields passed",strr(fieldpasses)),1); 
   showthreats("longdistmove"); 
} 


void         longdistmove(void)
{ 
      tlongdistmove ldm; 

   if (moveparams.movestatus == 0) { 
      moveparams.uheight = -1;
      moveparams.vehicletomove = getactfield->vehicle; 
      moveparams.movestatus = 112; 
   } 
   else 
      if (moveparams.movestatus == 112) { 
         cleartemps(7); 
         ldm.init(); 
         ldm.run(moveparams.vehicletomove,getxpos,getypos); 
         ldm.showresult(); 
         if (ldm.status == 12) { 
            ldm.move(); 
            showthreats(""); 
         } 
         ldm.done(); 
         cleartemps(7); 
         moveparams.movestatus = 0; 
         displaymap(); 
      } 
} 


boolean      fieldreachablelongdist(pvehicle     eht,
                                    integer      x2,
                                    integer      y2)
{ 
      tlongdistmove ldm; 

   cleartemps(7); 
   ldm.init(); 
   ldm.run(eht,x2,y2); 
   return ldm.status == 12; 
   ldm.done(); 
} 


void         movelongdist(pvehicle     eht,
                          integer      x2,
                          integer      y2)
{ 
      tlongdistmove ldm; 

   cleartemps(7); 
   ldm.init(); 
   ldm.run(eht,x2,y2); 
   if (ldm.status == 12) 
      ldm.move(); 
   ldm.done(); 
   displaymap(); 
} 

                          /* 
type tcompletemove : public tsearchfields)
                        destx,desty:word;
                        vehicle:pvehicle;
                        bestpos:record
                                   x,y:word;
                                   threat:int;
                                end;
                        procedure init(eht:pvehicle);
                        procedure testfield; virtual;
                        procedure run(x,y:word; var result:byte);
                     end;


void         tcompletemove :: init(void)
{ 
   inherited(); init(); 
   vehicle = eht; 
} 


void         tcompletemove :: testfield(void)
{ 
  int      l; 

   if ((xp < actmap->xsize) && (xp >= 0) && (yp < actmap->ysize) && (yp >= 0)) { 
      if (fieldreachablelongdist(vehicle,xp,yp)) { 
           //* l:=getposthreat(vehicle,xp,yp);
         // if (l > 0) and (l > bestpos.threat) then begin 
         bestpos.x = xp; 
         bestpos.y = yp; 
         bestpos.threat = 0; 
         abbruch = true; 
           //*  bestpos.threat:=l;
         //end;  
      } 
   } 
} 


   #define longmovesearchdist 12  



void         tcompletemove :: run(void)
{ 
  tmoveshortdistance msd; 

   bestpos.threat = low(bestpos.threat); 
   initsuche(x,y,longmovesearchdist,1); 
   startsuche(); 
   if (bestpos.threat > low(bestpos.threat)) { 
      movelongdist(vehicle,bestpos.x,bestpos.y); 
      if (vehicle->movement > 0) { 
         msd.init(vehicle); 
         msd.run(); 
         msd.exec(); 
         msd.done(); 
      } 
      else 
         checkattack(vehicle); 
      result = 0; 
   } 
   else 
      result = 1; 
} 

typedef struct tbestpos { 
                   byte         numx, numy; 
                   word         x, y; 
                   int      judg; 
                   byte         height; 
                } tbestpos; 


void         completemove(pvehicle     eht)
{ 
  #define stepnum 10  

  tjugdesituationrect jsr; 
  tcompletemove cm; 
  word         dst, rad; 
  tbestpos     bestpos[50]; 
  byte         i, j, k, m; 
  byte         x, y; 
  tunits       units; 
  int      l; 
  word         maxdst; 
  int      x1, y1; 

   displaymessage2(scat("%? / longmove",eht->typ->description)); 
   if (actmap->ysize * 2 > actmap->xsize) 
      maxdst = actmap->xsize / 2; 
   else 
      maxdst = actmap->ysize / 4; 
   rad = 10; 
   dst = longmovesearchdist; 
   do { 
      if (dst / 2 > rad) 
         rad = dst / 2; 
      for (i = low(bestpos); i <= high(bestpos); i++) 
         bestpos[i-1].judg = low(int); 

      for (x = 0; x <= stepnum; x++) 
         for (y = 0; y <= stepnum; y++) { 
            x1 = int(eht->xpos) - dst + (x * (2 * dst) / stepnum); 
            y1 = int(eht->ypos) - 2 * dst + (y * 4 * dst / stepnum); 
            jsr.init(addr(units),0,x1,y1,rad); 
            jsr.startsuche(); 
            l = jsr.unitposition(eht,log2(eht->height)); 
            j = high(bestpos); 
            k = 0; 
            do { 
               if (l > bestpos[j-1].judg) { 
                  if (j > 1) 
                     for (m = 2; m <= j; m++) 
                        bestpos[m - 1-1] = bestpos[m-1]; 
                  if (x1 < 0) x1 = 0; 
                  if (y1 < 0) y1 = 0; 
                  if (x1 >= actmap->xsize) x1 = actmap->xsize - 1; 
                  if (y1 >= actmap->ysize) y1 = actmap->ysize - 1; 
                  bestpos[j-1].judg = l; 
                  bestpos[j-1].x = x1; 
                  bestpos[j-1].y = y1; 
                  bestpos[j-1].height = eht->height; 
                  bestpos[j-1].numx = x; 
                  bestpos[j-1].numy = y; 
                  inc(k); 
               } 
               dec(j); 
            }  while (!((j == 0) || (k > 0))); 
         } 
      j = high(bestpos); 
        //* if bestpos[j].num <> 5 then begin 
      cm.init(eht); 
      do { 
         cm.run(bestpos[j-1].x,bestpos[j-1].y,k); 
         dec(j); 
      }  while (!((j == 0) || (k == 0))); 
      cm.done(); 
        //* end; *
      if (k != 0) { 
         dst = dst * 2; 
      } 
   }  while (!((k == 0) || (dst > maxdst))); 
} 






void         tmoveshortdistance :: init(pvehicle     eht)
{ 
   inherited(); init(); 
   bestattack = NULL; 
   firstattackableunit = NULL; 
   vehicle = eht; 
   ehtoxp = vehicle->xpos; 
   ehtoyp = vehicle->ypos; 
   bestposition = NULL; 
} 



void         tmoveshortdistance :: run(void)
{ 
      tcmpsearchattackablevehicles csae; 
      byte         m; 


   moveparams.movestatus = 0; 

   csae.init(vehicle); 
   csae.run(); 
   pb = csae.getbestattack; 
   if (pb != NULL) { 
      prirb = pb->priority; 
      pc = malloc(sizeof(*pc));
      *pc = *pb; 
   } 
   else { 
      prirb = -csae.getmaxthreat; 
      pc = NULL; 
   } 
   csae.done(); 

   moveparams.movestatus = 0; 

   movement(vehicle); 
   push(moveparams,sizeof(moveparams)); 
   m = vehicle->movement; 
   if (m > cc_maxshortmovedist) 
      m = cc_maxshortmovedist; 
   initsuche(ehtoxp,ehtoyp,m / 8 + 1,1); 
   startsuche(); 
   pa = firstattackableunit; 
   bestattack = firstattackableunit; 
   while (pa != NULL) { 
      if ((pa->priority > bestattack->priority) || ((pa->priority == bestattack->priority) && (beeline(pa->xstart,pa->ystart,ehtoxp,ehtoyp) < beeline(bestattack->xstart,bestattack->ystart,ehtoxp,ehtoyp)))) 
         bestattack = pa; 
      pa = pa->next; 
   } 

   if (pc != NULL) { 
      if (bestattack == NULL) 
         bestattack = pc; 
      else 
         if ((pc->priority > bestattack->priority) || ((pc->priority == bestattack->priority) && (beeline(pc->xstart,pc->ystart,ehtoxp,ehtoyp) < beeline(bestattack->xstart,bestattack->ystart,ehtoxp,ehtoyp)))) 
            bestattack = pc; 
   } 
   vehicle->xpos = ehtoxp; 
   vehicle->ypos = ehtoyp; 

   pop(moveparams,sizeof(moveparams)); 
} 


void         tmoveshortdistance :: exec(void)
{ 
   if (bestattack != NULL) { 
      if ((bestattack->result.adifferenz == 0) || (unitstatus[calcpass] == ccm_secondattack)) { 
         if ((((vehicle->typ->wait) && (bestattack->priority > int(prirb) * ccam_attackprirfak / 16)) || (vehicle->typ->wait == false)) && ((ehtoxp != bestattack->xstart) || (ehtoyp != bestattack->ystart))) { 
            cursor.gotoxy(bestattack->xstart,bestattack->ystart); 
            movement(vehicle); 
            movement(vehicle); 
            attackunit(bestattack); 
         } 
         else { 
            if (pc != NULL) 
               attackunit(bestattack); 
         } 
      } 
      else 
         vehicle->cmpchecked = ccm_secondattack; 
   } 
   else 
      if (bestposition != NULL) 
         if (bestposition->threat > prirb) { 
            cursor.gotoxy(bestposition->xpos,bestposition->ypos); 
            movement(vehicle); 
            movement(vehicle); 
         } 

   if (pc != NULL) 
      dispose(&pc); 
   cleartemps(7); 
   moveparams.movestatus = 0; 
} 


void         tmoveshortdistance_done(void)
{ 
   while (firstattackableunit != NULL) { 
      pa = firstattackableunit; 
      firstattackableunit = firstattackableunit->next; 
      dispose(&pa); 
   } 
   if (bestposition != NULL) 
      dispose(&bestposition); 
   inherited(); done(); 
} 


void         tmoveshortdistance :: testfield(void)
{ 
      pfield        fld; 
      tcmpsearchattackablevehicles csae; 
      pattackableunit pa2, pb2; 
      int      priorityb; 

   if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) { 
      fld = getfield(xp,yp); 
      if (fld->temp > 0) 
         if ((fld->building == NULL) && (fld->vehicle == NULL)) { 
            fld->vehicle = vehicle; 
            vehicle->xpos = xp; 
            vehicle->ypos = yp; 



            csae.init(vehicle); 
            csae.run(); 
            pa2 = csae.getbestattack; 
            if (pa2 != NULL) { 
               pb2 = firstattackableunit; 
               firstattackableunit = malloc(sizeof(*firstattackableunit));
               *firstattackableunit = *pa2; 
               firstattackableunit->next = pb2; 
            } 
            else { 
               priorityb = -csae.getmaxthreat; 

               if (bestposition == NULL) { 
                  bestposition = malloc(sizeof(*bestposition));
                  bestposition->threat = low(bestposition->threat); 
               } 

               if ((bestposition->threat < priorityb) || ((bestposition->threat == priorityb) && (beeline(startx,starty,xp,yp) < bestposition->distance))) { 
                  bestposition->threat = priorityb; 
                  bestposition->xpos = xp; 
                  bestposition->ypos = yp; 
                  bestposition->distance = beeline(startx,starty,xp,yp); 
               } 

            } 
            csae.done(); 

            fld->vehicle = NULL; 
         } 
   } 
} 




void         checkmovement(pvehicle     eht)
{ 
      tmoveshortdistance msd; 
      byte         b; 

   if (getfield(eht->xpos,eht->ypos)->vehicle == eht) { 
      if (weapexist(eht)) { 
         b = 0; 
         msd.init(eht); 
         msd.run(); 
//          if msd.bestattack <> NULL then
//            msd.exec;
//         else
//            b:=1;  
         msd.done(); 
         if (b == 1) 
            completemove(eht); 
      } 
   } 
} 


///////////////////////////////////////////////////////////
//          movementsroutinen Ende
/////////////////////////////////////////////////////////// 




  class tsearchforanything : public tsearchfields {
                        typedef tsearchfields inherited;
                       public:
                          int       destx,desty;
                          pvehicle vehicle;
                          int       status;
                          int       mode;
                          int       necheight;
                          int       objectsearched;   // { ccn_sprit, ...
                          void  init ( void );
                          void  exec ( void );
                     };


  class tsearchforrepairsite : public tsearchforanything {
                            typedef tsearchforanything inherited;
                         protected:
                            void testfield ( void );
                       };

  class tsearchforfuel       : public tsearchforanything {
                                 typedef tsearchforanything inherited;
                              protected:
                                 void testfield ( void );
                          };

  class tsearchforammunition : public tsearchforanything {
                                 typedef tsearchforanything inherited;
                              protected:
                                 void testfield ( void );
                          };



void         tsearchforanything :: init(void)
{ 
   status = 0; 
} 



int          checkrepair(pvehicle     dest,
                         pvehicle     source)
{ 
   if ( source->functions & cfrepair )
      if ( source->typ->weapons->count > 0 )
         for ( int i = 0; i < source->typ->weapons->count ; i++)
            if ( source->typ->weapons->weapon[i].sourceheight & source->typ->height )
               if ( source->typ->weapons->weapon[i].targ & dest->height )
                  if (source->typ->weapons->weapon[i].typ & cwserviceb ) 
                     return 1;
    return 0;
} 



boolean      checkrefuel(pvehicle     dest,
                         pvehicle     source)
{ 
   if (source->functions & cffuelref ) 
      if (source->fuel > dest->typ->tank - dest->fuel) 
         if (source->typ->weapons->count > 0) 
            for ( int i = 0; i < source->typ->weapons->count ; i++)
               if (source->typ->weapons->weapon[i].sourceheight & source->typ->height ) 
                  if (source->typ->weapons->weapon[i].targ & dest->height ) 
                     if (source->typ->weapons->weapon[i].typ & cwserviceb ) 
                        return 1;
   return 0;
} 



boolean      checkremunitionier(pvehicle     dest,
                                pvehicle     source)
{ 
   if ( dest->typ->weapons->count > 0)
      if (source->typ->weapons->count > 0) 
         for ( int j = 0; j < dest->typ->weapons->count ; j++)
            if ( dest->munition[j] <= dest->typ->weapons->weapon[j].count * ccs_remunitionier / 16)
               if (source->munition[j] > 0) 
                  for ( int i = 0; i < source->typ->weapons->count ; i++)
                     if (source->typ->weapons->weapon[i].sourceheight & source->typ->height ) 
                        if (source->typ->weapons->weapon[i].targ & dest->height ) 
                           if ((source->typ->weapons->weapon[i].typ & dest->typ->weapons->weapon[j].typ) & cwweapon ) 
                              return 1;
   return 0;
} 



void         tsearchforfuel :: testfield(void)
{ 
  int      x1, y1; 

   pfield fld = getfield(xp,yp);
   if ( fld ) {
      pvehicle eht = fld->vehicle; 
      if ( eht )
         if (eht->functions & cffuelref ) 
            if (eht->fuel > vehicle->typ->tank - vehicle->fuel) 
               if (eht->typ->weapons->count > 0) 
                  for ( int i = 0; i < eht->typ->weapons->count ; i++)
                     if (eht->typ->weapons->weapon[i].sourceheight & eht->typ->height ) 
                        if (eht->typ->weapons->weapon[i].targ & vehicle->height ) 
                           if (eht->typ->weapons->weapon[i].typ & cwserviceb ) { 
                              for ( int b = 0; b <= 7; b++) {
                                 x1 = eht->xpos; 
                                 y1 = eht->ypos; 
                                 getnextfield(x1,y1,b); 
                                 
                                 if (0 && fieldreachablelongdist(vehicle,x1,y1)) { 
                                    destx = x1; 
                                    desty = y1; 
                                    abbruch = true; 
                                    status = 2; 
                                    if ((eht->typ->weapons->weapon[i].targ & vehicle->height) == 0) {
                                       necheight = eht->typ->weapons->weapon[i].targ; 
                                    } 
                                    else 
                                       necheight = vehicle->height; 
                                 } 
                                 
                              } 
                              if (status != 2) { 
                                 if (0 && fieldreachablelongdist(vehicle,eht->xpos,eht->ypos)) { 
                                    destx = eht->xpos; 
                                    desty = eht->ypos; 
                                    status = 2; 
                                    abbruch = true; 
                                    necheight = vehicle->height; 
                                 } 
                              } 
                           } 
      if (mode > 0) { 
         pbuilding bld = fld->building;
         if (0 && bld != NULL) 
            if (bld->sprit + 5 * bld->plus.fuel >= vehicle->typ->tank - vehicle->fuel) 
               if (fieldreachablelongdist(vehicle,x1,y1)) { 
                  destx = x1; 
                  desty = y1; 
                  abbruch = true; 
                  status = 1; 
               } 
               
      } 
   } 
} 


void         tsearchforrepairsite :: testfield(void)
{ 
  int          x1, y1; 

   pfield fld = getfield(xp,yp);
   if (fld != NULL) { 
      pvehicle eht = fld->vehicle; 
      if (eht != NULL) 
         if (eht->functions & cfrepair ) 
            if (eht->typ->weapons->count > 0) 
               for ( int i = 0; i < eht->typ->weapons->count ; i++)
                  if (eht->typ->weapons->weapon[i].sourceheight & eht->typ->height ) 
                     if (eht->typ->weapons->weapon[i].targ & vehicle->height ) 
                        if (eht->typ->weapons->weapon[i].typ & cwserviceb ) { 
                           for ( int b = 0; b <= 7; b++) {
                              x1 = eht->xpos; 
                              y1 = eht->ypos; 
                              getnextfield(x1,y1,b); 
                              
                              if (0 && fieldreachablelongdist(vehicle,x1,y1)) { 
                                 destx = x1; 
                                 desty = y1; 
                                 abbruch = true; 
                                 status = 2; 
                                 if ((eht->typ->weapons->weapon[i].targ & vehicle->height) == 0) {
                                    necheight = eht->typ->weapons->weapon[i].targ; 
                                 } 
                                 else 
                                    necheight = vehicle->height; 
                              } 
                              
                           } 
                           if (status != 2) { 
                              if ( 0 && fieldreachablelongdist(vehicle,eht->xpos,eht->ypos)) { 
                                 destx = eht->xpos; 
                                 desty = eht->ypos; 
                                 status = 2; 
                                 abbruch = true; 
                                 necheight = vehicle->height; 
                              } 
                           } 
                        } 
      if (mode > 0) { 
         pbuilding bld = fld->building;
         if (bld != NULL) 
            if (bld->typ->special & cgrepairfacilityb )
               if (fieldreachablelongdist(vehicle,x1,y1)) { 
                  destx = x1; 
                  desty = y1; 
                  abbruch = true; 
                  status = 1; 
               } 
      } 
   } 
} 


void         tsearchforammunition :: testfield(void)
{ 
  int          x1, y1; 

   pfield fld = getfield(xp,yp);
   if (fld != NULL) { 
      pvehicle eht = fld->vehicle; 
      if (eht != NULL) 
         if (vehicle->typ->weapons->count > 0) 
            if (eht->typ->weapons->count > 0) 
               for ( int j = 0; j < vehicle->typ->weapons->count ; j++)
                  if (vehicle->munition[j] <= vehicle->typ->weapons->weapon[j].count * ccs_remunitionier / 16) 
                     if (eht->munition[j] > 0) 
                        for ( int i = 0; i < eht->typ->weapons->count ; i++)
                           if (eht->typ->weapons->weapon[i].sourceheight & eht->typ->height ) 
                              if (eht->typ->weapons->weapon[i].targ & vehicle->height ) 
                                 if ((eht->typ->weapons->weapon[i].typ & vehicle->typ->weapons->weapon[j].typ) & cwweapon ) { 
                                    for ( int b = 0; b <= 7; b++) {
                                       x1 = eht->xpos; 
                                       y1 = eht->ypos; 
                                       getnextfield(x1,y1,b); 
                                       
                                       if (0 && fieldreachablelongdist(vehicle,x1,y1)) { 
                                          destx = x1; 
                                          desty = y1; 
                                          abbruch = true; 
                                          status = 2; 
                                          if ((eht->typ->weapons->weapon[i].targ & vehicle->height) == 0) {
                                             necheight = eht->typ->weapons->weapon[i].targ; 
                                          } 
                                          else 
                                             necheight = vehicle->height; 
                                       } 
                                    } 
                                    if (status != 2) { 
                                       if (0 && fieldreachablelongdist(vehicle,eht->xpos,eht->ypos)) { 
                                          destx = eht->xpos; 
                                          desty = eht->ypos; 
                                          status = 2; 
                                          abbruch = true; 
                                          necheight = vehicle->height; 
                                       } 
                                    } 
                                 } 

      if (mode > 0) { 
         pbuilding bld = fld->building;
         if (0 && bld != NULL) 
            if (bld->sprit + 5 * bld->plus.fuel >= vehicle->typ->tank - vehicle->fuel) 
               if (fieldreachablelongdist(vehicle,x1,y1)) { 
                  destx = x1; 
                  desty = y1; 
                  abbruch = true; 
                  status = 1; 
               } 
               
      } 
   } 
} 






void         disposeorder(prequireunit ru)
{ 
  prequireunit ru1, ru2; 

   ru2 = NULL; 
   ru1 = orders[actmap->actplayer]; 
   while ((ru1 != NULL) && (ru != ru1)) { 
      ru2 = ru1; 
      ru1 = ru1->next; 
   } 
   if (ru1 == ru) { 
      if (ru2 == NULL) 
         orders[actmap->actplayer] = ru1->next; 
      else 
         ru2->next = ru1->next; 

      delete ru;
   } 
} 


void         addorder(pvehicle     eht,
                      byte         need,
                      int      prir)
{ 
  prequireunit ru; 
  word         id; 

   ru = orders[actmap->actplayer]; 
   id = 1; 
   while (ru != NULL) { 
      if (ru->units == eht) 
         return;
      if (ru->id == id) { 
         id++;
         ru = orders[actmap->actplayer]; 
      } 
      else 
         ru = ru->next; 
   } 
   ru = new ( trequireunit );
   ru->next = orders[actmap->actplayer]; 
   orders[actmap->actplayer] = ru; 
   ru->id = id; 
   ru->unitneeded = NULL; 
   ru->priority = prir; 
   ru->need = need; 
   ru->units = eht; 
} 

                                  
   class tcmprefuelunit : public trefuelvehicle {
                       typedef trefuelvehicle inherited;
                     public:
                       void testfield ( void );
                    };

void         tcmprefuelunit :: testfield(void)
{ 

   inherited :: testfield();
   pfield fld = getfield(xp,yp);
   if (fld->a.temp == 2) { 
      if (mode == 1) { 
         actvehicle->repairunit( fld->vehicle ); 
      } 
      if (mode == 2) { 
         verlademunition(fld->vehicle,actvehicle,NULL,3); 
      } 
   } 
} 


void         checkorders(pvehicle     eht)
{ 
  prequireunit ru; 
  int          x1, y1;
  int i;
  struct  { 
                              prequireunit order; 
                              int          prir;
                              int          effprir;
                              int          dist;
                           } bestorder[4]; 

   int b = 0;

   if (eht->functions & cffuelref )
      b |= ccn_refuel;

   if (eht->functions & cfrepair ) 
      b |= ccn_repair;

   if (eht->typ->weapons->count > 0) 
      for (i = 0; i < eht->typ->weapons->count ; i++)
         if (eht->munition[i] > 0) 
            if (eht->typ->weapons->count & cwammunitionb ) 
               b |= ccn_remunitionier;
   if (b > 0) { 
      memset(&bestorder, 0, sizeof(bestorder));
      for (i = 0; i < 4; i++) {
         bestorder[i].prir = minint;
         bestorder[i].effprir = minint;
      } 
      ru = orders[actmap->actplayer]; 
      while (ru != NULL) { 
         if (ru->need & b ) { 
            i = 0; 
            if (b & ccn_repair ) 
               if (checkrepair(ru->units,eht)) 
                  i++;
            if (b & ccn_refuel ) 
               if ( checkrefuel(ru->units,eht) )
                  i++;
            if (b & ccn_remunitionier ) 
               if (checkremunitionier(ru->units,eht)) 
                  i++;
            if (i > 0) { 
               int j = 0;
               i = 0; 
               do { 
                  if (ru->priority * 100 / beeline(ru->units->xpos,ru->units->ypos,eht->xpos,eht->ypos) > bestorder[j].effprir) { 
                     if ( j < 3)
                        for (i = 3; i > j ; i--)
                           bestorder[i] = bestorder[i - 1]; 
                     bestorder[j].order = ru; 
                     bestorder[j].prir = ru->priority; 
                     bestorder[j].dist = beeline(ru->units->xpos,ru->units->ypos,eht->xpos,eht->ypos); 
                     bestorder[j].effprir = ru->priority * 100 / bestorder[j].dist; 
                     i = 1; 
                  } 
                  j++;
               }  while (!((i == 1) || (j > 3))); 
            } 
         } 
         ru = ru->next; 
      } 
      if (bestorder[0].order == NULL) { 
         eht->cmpchecked = ccm_servicewait; 
      } 
      else { 
         int b = 0;
         int i = 0;
         do { 
            if (bestorder[b].order != NULL) { 
               pvehicle eht2 = bestorder[b].order->units; 
               if (eht2 == eht) 
                  i = 1; 
               int c = getdirection(eht2->xpos,eht2->ypos,eht->xpos,eht->ypos); 

               for ( int j = 0; j <= 7; j++) {
                  x1 = bestorder[b].order->units->xpos; 
                  y1 = bestorder[b].order->units->ypos; 
                  getnextfielddir(x1,y1,j,c); 
                  
                  if (0 && fieldreachablelongdist(eht,x1,y1)) { 
                     i++;
                     break;
                  } 
               } 

               if (i > 0) { 
                  
                  // if (beeline(x1,y1,eht->xpos,eht->ypos) > 15) 
                  //    movelongdist(eht,x1,y1); 
                     
                  if (beeline(x1,y1,eht->xpos,eht->ypos) <= 15) { 
                     if (bestorder[b].order->need & ccn_repair > 0) { 
                        tcmprefuelunit cru;
                        cru.initrefuelling(eht->xpos,eht->ypos,1); 
                        cru.actvehicle = eht; 
                        cru.startsuche(); 
                        cru.done(); 
                     } 
                     if (bestorder[b].order->need & (ccn_refuel | ccn_remunitionier) ) {
                        tcmprefuelunit cru;
                        cru.initrefuelling(eht->xpos,eht->ypos,2); 
                        cru.actvehicle = eht; 
                        cru.startsuche(); 
                        cru.done(); 
                     } 
                     disposeorder(bestorder[b].order); 
                     cleartemps(7); 
                     displaymap(); 
                     moveparams.movestatus = 0; 

                  } 
               } 
            } 
            b++;
         }  while ( (b <= 3)  &&  (i == 0 )) ;
      } 
   } 
} 




void         tsearchforanything :: exec(void)
{ 
  int          x1, y1;

   if (status > 0) { 
      if (necheight != vehicle->height) { 
         int b = vehicle->height; 
         if (necheight > b) { 
            changeheight(vehicle,'h',true); 
         } 
         else { 
            changeheight(vehicle,'t',true); 
         } 
         if (vehicle->typ->steigung > 0) { 
            int c = getdirection(vehicle->xpos,vehicle->ypos,destx,desty);
            int d = 0;
            int g = 0;
            do { 
               x1 = vehicle->xpos; 
               y1 = vehicle->ypos; 
               int f;
               for ( int e = 1; e <= vehicle->typ->steigung; e++)
                  getnextfielddir(x1,y1,f,c); 
               pfield fld = getfield(x1,y1);
               if ( fld )
                  if (fld->a.temp > 0) 
                     g = 1; 
               d++;
            }  while ( (d < 8) && (g != 1) );
            if (g == 1) { 
               cursor.gotoxy(x1,y1); 
               do { 
                  movement(NULL); 
               }  while ( moveparams.movestatus );
            } 
         } 
      } 
      // movelongdist(vehicle,destx,desty); 
   } 
   addorder(vehicle,objectsearched,vehicle->completethreatvalue); 
} 




void         checkservice(pvehicle     eht)
{
   int w;
   if (100 - eht->damage <= 100 * ccs_damageleave / 16) { 
      if (eht->typ->movement[log2(eht->typ->height)] == 0) 
         w = 0; 
      else 
         w = eht->fuel / eht->typ->fuelconsumption; 
      w = 20; 
      if (w > 0) {
         tsearchforrepairsite sfrs;
         sfrs.init(); 
         sfrs.vehicle = eht; 
         sfrs.objectsearched = ccn_repair; 
         sfrs.initsuche(eht->xpos,eht->ypos,w,1); 
         sfrs.startsuche(); 
         sfrs.exec(); 
         sfrs.done(); 
      } 
      else { 
         addorder(eht,ccn_repair,eht->completethreatvalue); 
      } 
   } 
   if (eht->fuel <= eht->typ->tank * ccs_refuel / 16) { 
      if (eht->typ->movement[log2(eht->typ->height)] == 0) 
         w = 0; 
      else 
         w = eht->fuel / eht->typ->fuelconsumption; 
      if (w > 0) { 
         tsearchforfuel sff;    
         sff.init(); 
         sff.vehicle = eht; 
         sff.objectsearched = ccn_refuel; 
         sff.initsuche(eht->xpos,eht->ypos,w,1); 
         sff.startsuche(); 
         sff.exec(); 
         sff.done(); 
      } 
      else { 
         addorder(eht,ccn_refuel,eht->completethreatvalue); 
      } 
   } 
   if (eht->typ->weapons->count > 0) 
      for ( int i = 0; i < eht->typ->weapons->count ; i++)
         if (eht->typ->weapons->weapon[i].count > 0) 
            if (eht->typ->weapons->weapon[i].typ & cwweapon ) 
               if (eht->munition[i] <= eht->typ->weapons->weapon[i].count * ccs_remunitionier / 16) { 
                  int w;
                  if (eht->typ->movement[log2(eht->typ->height)] == 0) 
                     w = 0; 
                  else 
                     w = eht->fuel / eht->typ->fuelconsumption; 
                  if (w > 0) { 
                     tsearchforammunition sfa;
                     sfa.init(); 
                     sfa.vehicle = eht; 
                     sfa.objectsearched = ccn_remunitionier; 
                     sfa.initsuche(eht->xpos,eht->ypos,w,1); 
                     sfa.startsuche(); 
                     sfa.exec(); 
                     sfa.done(); 
                  } 
                  else { 
                     addorder(eht,ccn_remunitionier,eht->completethreatvalue); 
                  } 
               } 
} 



void         initcomputerturn(void)
{ 
  int i;

   tempsvisible = false; 
   displaymessage2("generating threatvalues ... "); 
   generatethreatvalues(); 
   for (i = 0; i < 8; i++)
      if (actmap->player[i].existent) { 
         pvehicle vehicle = actmap->player[i].firstvehicle;
         if (i == actmap->actplayer) { 
            while (vehicle != NULL) { 
               vehicle->cmpchecked = 0; 
               generatethreatvalueunit(vehicle); 
               vehicle = vehicle->next; 
            } 
         } 
         else { 
            while (vehicle != NULL) { 
               generatethreatvalueunit(vehicle); 
               vehicle = vehicle->next; 
            } 
         } 
      } 

   for ( i = 0; i <= 8; i++) {
      pbuilding building = actmap->player[i].firstbuilding;
      while (building != NULL) { 
         generatethreatvaluebuilding(building); 
         building = building->next; 
      } 
      if (i == actmap->actplayer) { 
         building = actmap->player[i].firstbuilding; 
         while (building != NULL) { 
            tcmpcheckreconquerbuilding ccrcb;
            ccrcb.init(3); 
            ccrcb.initsuche(building->xpos,building->ypos,(maxfusstruppenmove + maxtransportmove) / 8 + 1,0); 
            ccrcb.startsuche(); 
            int j;
            ccrcb.returnresult( &j ); 
            ccrcb.done(); 
            building = building->next; 
         } 
      } 
   }     

   // punits units = new tunits;
   // tjugdesituationspfd jugdesituationspfd; 
   // jugdesituationspfd.init(units,1); 
   // jugdesituationspfd.startsuche(); 
   // jugdesituationspfd.done(); 
   // delete units;

   showthreats("init: threatvals generated"); 

} 


void         closecomputerturn(void)
{ 
   tempsvisible = true; 
   cleartemps(7); 
   displaymap(); 
   displaymessage2("ready"); 
   beep(); 
} 



void         computerturn(void)
{ 
   initcomputerturn(); 
                                                
   for ( int calcpass = 0; calcpass < unitstatusnum; calcpass++) { 
      pvehicle vehicle = actmap->player[actmap->actplayer].firstvehicle;
      while (vehicle != NULL) { 
         if (vehicle->cmpchecked == unitstatus[calcpass]) 
            if (vehicle->functions & ( cf_conquer )) 
               checkconquerbuilding(vehicle); 
         vehicle = vehicle->next; 
      } 

      // showthreats(scat("pass %s",strr(calcpass)));

      vehicle = actmap->player[actmap->actplayer].firstvehicle; 
      while (vehicle != NULL) { 
         if (vehicle->cmpchecked == unitstatus[calcpass]) { 
            displaymessage2(vehicle->typ->description); 
            checkorders(vehicle); 
            checkservice(vehicle); 
            // if (vehicle->movement > 0) 
            //   checkmovement(vehicle); 
            // else  
               checkattack(vehicle); 
         } 

         vehicle = vehicle->next; 
      } 
   } 
   closecomputerturn(); 
} 


class tartintinit {
          public:
            tartintinit ( void ) {
                   threatvaluesgenerated = false; 
                   maxfusstruppenmove = 0; 
                   artintconfig.movesearchshortestway = false;
                   artintconfig.lookintotransports = true; 
                   artintconfig.lookintobuildings = true; 
                   memset( &orders, 0, sizeof(orders) );
             };
        } artintinit;

tartintconfig artintconfig;

   
 **********************************************************************************
 **********************************************************************************
      The new AI starts here !
 **********************************************************************************
 **********************************************************************************/



#define value_armorfactor 100
#define value_weaponfactor 70

#define ccbt_repairfacility 200    //  basic threatvalues for buildings  
#define ccbt_hq 10000  
#define ccbt_recycling 50  
#define ccbt_training 150  
#define attack_unitdestroyed_bonus 1.4


void nop ( void )
{
}

int compareinteger ( const void* op1, const void* op2 )
{
   const int* a = (const int*) op1;
   const int* b = (const int*) op2;
   return *a > *b;
}

void AiThreat :: reset ( void )
{
   for ( int i = 0; i < 8; i++ )
      threat[i] = 0;
}

void AiParameter :: reset ( void )
{
   AiThreat :: reset ( );

   value = 0;
   task = tsk_nothing;
}

AI :: AI ( pmap _map ) 
{ 
   fieldThreats = NULL;

   reset(); 
   activemap = _map; 

   static ReplayMapDisplay* rmd = new ReplayMapDisplay ( &defaultMapDisplay );
   mapDisplay = rmd;
   rmd->setCursorDelay ( gameoptions.replayspeed + 30 );

}

void AI :: reset ( void )
{
   maxTrooperMove = 0;  
   maxTransportMove = 0; 
   for ( int i= 0; i < 8; i++ )
      maxWeapDist[i] = -1;  
   baseThreatsCalculated = 0;

   if ( fieldThreats )
      delete[] fieldThreats;

   fieldThreats = NULL;
   fieldNum = 0;

   config.wholeMapVisible = 1;
   config.lookIntoTransports = 1;
   config.lookIntoBuildings = 1;
   config.aggressiveness  = 1;

}






  class CalculateThreat_VehicleType {
                         protected:
                              AI*               ai;

                              pvehicletype      fzt;
                              int               weapthreat[8];
                              int               value;

                              virtual int       getdamage ( void )      { return 0;   };
                              virtual int       getexpirience ( void )  { return 0;   };
                              virtual int       getammunition( int i )  { return 1;   };
                              virtual int       getheight ( void )      { return 255; };
                          public:
                              void              calc_threat_vehicletype ( pvehicletype _fzt );
                              CalculateThreat_VehicleType ( AI* _ai ) { ai = _ai; };
                       };

  class CalculateThreat_Vehicle : public CalculateThreat_VehicleType {
                           protected:
                                pvehicle          eht;
                                virtual int       getdamage ( void );
                                virtual int       getexpirience ( void );
                                virtual int       getammunition( int i );
                                virtual int       getheight ( void );
                           public:
                              void              calc_threat_vehicle ( pvehicle _eht );
                              CalculateThreat_Vehicle ( AI* _ai ) : CalculateThreat_VehicleType ( _ai ) {};
                       };


void         CalculateThreat_VehicleType :: calc_threat_vehicletype ( pvehicletype _fzt )
{ 
   fzt = _fzt;

   for ( int j = 0; j < 8; j++ )
      weapthreat[j] = 0;

   for ( int i = 0; i < fzt->weapons->count; i++) 
      if ( fzt->weapons->weapon[i].shootable() ) 
         if ( fzt->weapons->weapon[i].offensive() ) 
            for ( int j = 0; j < 8; j++) 
               if ( fzt->weapons->weapon[i].targ & (1 << j) ) { 
                  int d = 0; 
                  int m = 0; 
                  AttackFormula af;
                  for ( int e = (fzt->weapons->weapon[i].mindistance + maxmalq - 1)/ maxmalq; e <= fzt->weapons->weapon[i].maxdistance / maxmalq; e++ ) {    // the distance between two fields is maxmalq
                     d++; 
                     int n = weapDist.getWeapStrength( &fzt->weapons->weapon[i], e*maxmalq ) * fzt->weapons->weapon[i].maxstrength * af.strength_damage(getdamage()) * ( 1 + af.strength_experience(getexpirience()));
                     m += n / (2*(1+d)); 
                  } 
                  if (getammunition(i) == 0) 
                     m /= 2; 

                  if ( (fzt->weapons->weapon[i].sourceheight & getheight()) == 0) 
                     m /= 2; 

                  if ( !(getheight() & ( 1 << j )))
                     m /= 2;

                  if (m > weapthreat[j]) 
                     weapthreat[j] = m; 
               } 


   if ( !fzt->aiparam[ai->getplayer()] )
      fzt->aiparam[ ai->getplayer() ] = new AiParameter;

   for ( int l = 0; l < 8; l++ ) 
      fzt->aiparam[ ai->getplayer() ]->threat[l] = weapthreat[l];

   value = fzt->armor * value_armorfactor * (100 - getdamage()) / 100 ; 
   qsort ( weapthreat, 8, sizeof(int), compareinteger );

   for ( int k = 0; k < 8; k++ ) 
      value += weapthreat[k] * value_weaponfactor / (k+1);

   fzt->aiparam[ ai->getplayer() ]->value = value;
} 


int          CalculateThreat_Vehicle :: getammunition( int i )
{ 
   return eht->munition[i]; 
} 

int          CalculateThreat_Vehicle :: getheight(void)
{ 
   return eht->height; 
} 

int          CalculateThreat_Vehicle :: getdamage(void)
{ 
   return eht->damage; 
} 

int          CalculateThreat_Vehicle :: getexpirience(void)
{ 
   return eht->experience; 
} 


void         CalculateThreat_Vehicle :: calc_threat_vehicle ( pvehicle _eht )
{ 

   eht = _eht;     
   calc_threat_vehicletype ( eht->typ );

   if ( !eht->aiparam[ai->getplayer()] )
      eht->aiparam[ai->getplayer()] = new AiParameter;

   for ( int l = 0; l < 8; l++ ) 
      eht->aiparam[ai->getplayer()]->threat[l] = eht->typ->aiparam[ ai->getplayer() ]->threat[l];

   eht->aiparam[ai->getplayer()]->value = eht->typ->aiparam[ ai->getplayer() ]->value;

/*
   generatethreatvalue(); 
   int l = 0; 
   for ( int b = 0; b <= 7; b++) { 
      eht->threatvalue[b] = weapthreatvalue[b]; 
      if (weapthreatvalue[b] > l) 
         l = weapthreatvalue[b]; 
   } 
   eht->completethreatvalue = threatvalue2 + l; 
   eht->completethreatvaluesurr = threatvalue2 + l; 
   eht->threats = 0; 
*/
} 


void  AI :: calculateThreat ( pvehicletype vt)
{ 
   CalculateThreat_VehicleType ctvt ( this ); 
   ctvt.calc_threat_vehicletype( vt );
} 


void  AI :: calculateThreat ( pvehicle eht )
{ 
   CalculateThreat_Vehicle ctv ( this ); 
   ctv.calc_threat_vehicle( eht );
} 


void  AI :: calculateThreat ( pbuilding bld )
{ 
   if ( !bld->aiparam[ getplayer() ] )
      bld->aiparam[ getplayer() ] = new AiParameter;

   int b; 

   // Since we have two different resource modes now, this calculation should be rewritten....
   bld->aiparam[ getplayer() ]->value = (bld->plus.a.energy / 10) + (bld->plus.a.fuel / 10) + (bld->plus.a.material / 10) + (bld->actstorage.a.energy / 20) + (bld->actstorage.a.fuel / 20) + (bld->actstorage.a.material / 20) + (bld->maxresearchpoints / 10); 
   for (b = 0; b <= 31; b++) 
      if ( bld->loading[b]  ) {
         if ( !bld->loading[b]->aiparam[ getplayer() ] )
            calculateThreat ( bld->loading[b] );
         bld->aiparam[ getplayer() ]->value += bld->loading[b]->aiparam[ getplayer() ]->value; 
      }

   for (b = 0; b <= 31; b++) 
      if ( bld->production[b] )  {
         if ( !bld->production[b]->aiparam[ getplayer() ] )
            calculateThreat ( bld->production[b] );
         bld->aiparam[ getplayer() ]->value += bld->production[b]->aiparam[ getplayer() ]->value / 10; 
      }

   if (bld->typ->special & cgrepairfacilityb ) 
      bld->aiparam[ getplayer() ]->value += ccbt_repairfacility; 
   if (bld->typ->special & cghqb ) 
      bld->aiparam[ getplayer() ]->value += ccbt_hq; 
   if (bld->typ->special & cgtrainingb ) 
      bld->aiparam[ getplayer() ]->value += ccbt_training; 
   if (bld->typ->special & cgrecyclingplantb ) 
      bld->aiparam[ getplayer() ]->value += ccbt_recycling; 
} 



void AI :: WeaponThreatRange :: run ( pvehicle _veh, int x, int y, AiThreat* _threat, AI* _ai )
{
   ai = _ai;
   threat = _threat;
   veh = _veh;
   for ( height = 0; height < 8; height++ )
      for ( weap = 0; weap < veh->typ->weapons->count; weap++ ) 
         if ( veh->height & veh->typ->weapons->weapon[weap].sourceheight ) 
            if ( (1 << height) & veh->typ->weapons->weapon[weap].targ ) 
                if ( veh->typ->weapons->weapon[weap].shootable()  && veh->typ->weapons->weapon[weap].offensive() ) {
                   initsuche ( x, y, veh->typ->weapons->weapon[weap].maxdistance/maxmalq, veh->typ->weapons->weapon[weap].mindistance/maxmalq );
                   startsuche();
                }
}

void AI :: WeaponThreatRange :: testfield ( void ) 
{ 
   if ( getfield ( xp, yp ))
      if ( dist*maxmalq <= veh->typ->weapons->weapon[weap].maxdistance ) 
         if ( dist*maxmalq >= veh->typ->weapons->weapon[weap].mindistance ) {
            AttackFormula af;
            int strength = weapDist.getWeapStrength( &veh->typ->weapons->weapon[weap], dist*maxmalq, veh->height, 1 << height ) 
                         * veh->typ->weapons->weapon[weap].maxstrength              
                         * (1 + af.strength_experience ( veh->experience ) + af.strength_attackbonus ( getfield(startx,starty)->getattackbonus() ))
                         * af.strength_damage ( veh->damage );

            if ( strength ) {
               int pos = xp + yp * ai->getmap()->xsize;
               if ( strength > threat[pos].threat[height] )
                  threat[pos].threat[height] = strength;
            }
         }
}


void AI :: calculateFieldThreats ( void )
{
   if ( fieldNum && fieldNum != activemap->xsize * activemap->ysize ) {
      delete[] fieldThreats;
      fieldThreats = NULL;
      fieldNum = 0;
   }
   if ( !fieldThreats ) {
      fieldNum = activemap->xsize * activemap->ysize;
      fieldThreats = new AiThreat[ fieldNum ];
   } else
      for ( int a = 0; a < fieldNum; a++ )
         fieldThreats[ a ].reset();
      
   AiThreat*  singleUnitThreat = new AiThreat[fieldNum];

   // we now check the whoe map
   for ( int y = 0; y < activemap->ysize; y++ )
      for ( int x = 0; x < activemap->xsize; x++ ) {
         pfield fld = getfield ( x, y );
         if ( config.wholeMapVisible || fieldvisiblenow ( fld, getplayer() ) )
            if ( fld->vehicle && getdiplomaticstatus2 ( getplayer()*8, fld->vehicle->color) == cawar ) { 
               WeaponThreatRange wr;
               if ( !fld->vehicle->typ->wait ) {

                  // The unit may have already moved this turn. 
                  // So we give it the maximum movementrange
   
                  int move = fld->vehicle->getMovement() ;

                  fld->vehicle->setMovement ( fld->vehicle->typ->movement [ log2 ( fld->vehicle->height )] );
   
                  VehicleMovement vm ( NULL, NULL );
                  if ( vm.available ( fld->vehicle )) {
                     vm.execute ( fld->vehicle, -1, -1, 0, -1, -1 );

                     // Now we cycle through all fields that are reachable...
                     for ( int f = 0; f < vm.reachableFields.getFieldNum(); f++ ) {
                        int xp, yp;
                        vm.reachableFields.getFieldCoordinates ( f, &xp, &yp );
                        // ... and check for each which fields are threatened if the unit was standing there
                        wr.run ( fld->vehicle, xp, yp, singleUnitThreat, this );
                     }

                     for ( int g = 0; g < vm.reachableFieldsIndirect.getFieldNum(); g++ ) {
                        int xp, yp;
                        vm.reachableFieldsIndirect.getFieldCoordinates ( g, &xp, &yp );
                        wr.run ( fld->vehicle, xp, yp, singleUnitThreat, this );
                     }
                  }
                  fld->vehicle->setMovement ( move );
               } else
                  wr.run ( fld->vehicle, x, y, singleUnitThreat, this );


               for ( int a = 0; a < fieldNum; a++ ) {
                  for ( int b = 0; b < 8; b++ ) 
                     fieldThreats[a].threat[b] += singleUnitThreat[a].threat[b];
                  
                  singleUnitThreat[ a ].reset();
               }
            }
      }

}


void     AI :: calculateAllThreats( void )
{ 
   // Calculates the basethreats for all vehicle types
   if ( !baseThreatsCalculated ) { 
      for ( int w = 0; w < vehicletypenum; w++) { 
         pvehicletype fzt = getvehicletype_forpos(w); 
         if ( fzt ) 
            calculateThreat( fzt ); 
         
      }
      baseThreatsCalculated = 1; 
   }

   // Some further calculations that only need to be done once.
   if ( maxTrooperMove == 0) { 
      for ( int v = 0; v < vehicletypenum; v++) {
         pvehicletype fzt = getvehicletype_forpos( v );
         if ( fzt )
            if ( fzt->functions & ( cf_conquer ) ) 
               if ( fzt->movement[chfahrend] > maxTrooperMove )   // buildings can only be conquered on ground level, or by moving to adjecent field which is less
                  maxTrooperMove = fzt->movement[chfahrend]; 
      }
   } 
   if ( maxTransportMove == 0 ) {
      for (int v = 0; v < vehicletypenum; v++) {
         pvehicletype fzt = getvehicletype_forpos( v );
         if ( fzt )
            for ( int w = 0; w <= 7; w++) // cycle through all levels of height
               if (fzt->movement[w] > maxTransportMove) 
                  maxTransportMove = fzt->movement[w]; 
      }
   }
   for ( int height = 0; height < 8; height++ ) 
      if ( maxWeapDist[height] < 0 ) { 

         maxWeapDist[height] = 0; // It may be possible that there is no weapon to shoot to a specific height

         for ( int v = 0; v < vehicletypenum; v++) {
            pvehicletype fzt = getvehicletype_forpos( v );
            if ( fzt )
               for ( int w = 0; w < fzt->weapons->count ; w++)    
                  if ( fzt->weapons->weapon[w].maxdistance > maxWeapDist[height] ) 
                     if ( fzt->weapons->weapon[w].targ & ( 1 << height ))   // targ is a bitmap, each bit standing for a level of height
                         maxWeapDist[height] = fzt->weapons->weapon[w].maxdistance; 
         }
      } 



   // Resetting all previos AI parameters for the units. This should later be removed to allow long-time planning,
   // but at the moment it provides clean starting conditions

   // There are only 8 players in ASC, but there may be neutral units (player == 8)
   for ( int v = 0; v < 9; v++)      
      if (actmap->player[v].existent) { 

         // Now we cycle through all units of this player
         pvehicle eht = actmap->player[v].firstvehicle; 
         while ( eht ) { 
            if ( !eht->aiparam[ getplayer() ] )
               eht->aiparam[ getplayer() ] = new AiParameter;
            else
               eht->aiparam[ getplayer() ]->reset();

            calculateThreat ( eht );
            eht = eht->next; 
         } 
      } 
} 



void    AI :: setup (void)
{ 
   displaymessage2("calculating all threats ... "); 
   calculateAllThreats (); 
   displaymessage2("calculating field threats ... "); 
   calculateFieldThreats();

/*
   for ( i = 0; i <= 8; i++) {
      pbuilding building = actmap->player[i].firstbuilding;
      while (building != NULL) { 
         generatethreatvaluebuilding(building); 
         building = building->next; 
      } 
      if (i == actmap->actplayer) { 
         building = actmap->player[i].firstbuilding; 
         while (building != NULL) { 
            tcmpcheckreconquerbuilding ccrcb;
            ccrcb.init(3); 
            ccrcb.initsuche(building->xpos,building->ypos,(maxfusstruppenmove + maxtransportmove) / 8 + 1,0); 
            ccrcb.startsuche(); 
            int j;
            ccrcb.returnresult( &j ); 
            ccrcb.done(); 
            building = building->next; 
         } 
      } 
   }     
   */

   /*
    punits units = new tunits;
    tjugdesituationspfd jugdesituationspfd; 
    jugdesituationspfd.init(units,1); 
    jugdesituationspfd.startsuche(); 
    jugdesituationspfd.done(); 
    delete units;
   */

   // showthreats("init: threatvals generated"); 
   displaymessage2("setup completed ... "); 
} 

void AI :: searchTargets ( pvehicle veh, int x, int y, TargetList* tl, int moveDist )
{
   npush ( veh->xpos );
   npush ( veh->ypos );
   veh->xpos = x;
   veh->ypos = y;

   VehicleAttack va ( NULL, NULL );
   if ( va.available ( veh )) {
      va.execute ( veh, -1, -1, 0 , 0, -1 );
      for ( int g = 0; g < va.attackableVehicles.getFieldNum(); g++ ) {
         int xp, yp;
         va.attackableVehicles.getFieldCoordinates ( g, &xp, &yp );
         pattackweap aw = va.attackableVehicles.getData ( g );

         int bestweap = -1;  
         int targdamage = -1;
         int weapstrength = -1;
         for ( int w = 0; w < aw->count; w++ ) {
            tunitattacksunit uau ( veh, getfield ( xp, yp)->vehicle, 1, aw->num[w] );
            uau.calc();
            if ( uau.dv.damage > targdamage ) {
               bestweap = aw->num[w];
               targdamage = uau.dv.damage;
               weapstrength = aw->strength[w];
            } else
            if ( uau.dv.damage == 100 ) 
               if ( weapstrength == -1 || weapstrength > aw->strength[w] ) {
                  bestweap = aw->num[w];
                  targdamage = uau.dv.damage;
                  weapstrength = aw->strength[w];
               }
         }

         if ( bestweap == -1 )
            displaymessage ( "inconsistency in AI :: searchTarget", 1 );

         MoveVariant* mv = &(*tl)[tl->getlength()+1 ];  // sometime this should all be modified to use the STL...

         tunitattacksunit uau ( veh, getfield ( xp, yp)->vehicle, 1, bestweap );
         mv->orgDamage = uau.av.damage;
         mv->damageAfterMove = uau.av.damage;
         mv->enemyOrgDamage = uau.dv.damage;
         uau.calc();


         mv->damageAfterAttack = uau.av.damage;
         mv->enemyDamage = uau.dv.damage;
         mv->enemy = getfield ( xp, yp )->vehicle;
         mv->movex = x;
         mv->movey = y;
         mv->attackx = xp;
         mv->attacky = yp;
         mv->weapNum = bestweap;
         mv->moveDist = moveDist;

      }
   }

   npop ( veh->ypos );
   npop ( veh->xpos );
}


void AI::tactics( void )
{
   displaymessage2("starting tactics ... "); 

   pvehicle veh = getmap()->player[ getplayer() ].firstvehicle;
   while ( veh ) {
      if ( veh->weapexist() ) {
         int orgmovement = veh->getMovement();
         int orgxpos = veh->xpos ;
         int orgypos = veh->ypos ;

         VehicleMovement vm ( NULL, NULL );
         if ( vm.available ( veh )) {
            vm.execute ( veh, -1, -1, 0, -1, -1 );

            TargetList targetList;

            searchTargets ( veh, veh->xpos, veh->ypos, &targetList, 0 );

            // Now we cycle through all fields that are reachable...
            if ( !veh->typ->wait )
               for ( int f = 0; f < vm.reachableFields.getFieldNum(); f++ ) 
                  if ( !vm.reachableFields.getField( f )->vehicle ) {
                      int xp, yp;
                      vm.reachableFields.getFieldCoordinates ( f, &xp, &yp );
                      searchTargets ( veh, xp, yp, &targetList, beeline ( xp, yp, orgxpos, orgypos ) );
                   }

            int bestpos = -1;
            int bestres = -1;
            int bestmove = maxint;
            for ( int i = 0; i <= targetList.getlength(); i++ ) {
               MoveVariant* mv = &targetList[i];

               mv->result = int ((mv->enemyDamage - mv->enemyOrgDamage) * mv->enemy->aiparam[getplayer()]->value - config.aggressiveness * (mv->damageAfterAttack - mv->orgDamage) * veh->aiparam[getplayer()]->value );
               if ( mv->enemyDamage >= 100 )
                  mv->result *= attack_unitdestroyed_bonus;

               if ( mv->result > bestres || (mv->result == bestres && bestmove > mv->moveDist )) {
                  bestres = mv->result;
                  bestpos = i;
                  bestmove = mv->moveDist;
               }
            }
            if ( bestpos >= 0 ) {
               MoveVariant* mv = &targetList[bestpos];

               if ( mv->movex != veh->xpos || mv->movey != veh->ypos ) {
                  VehicleMovement vm2 ( mapDisplay, NULL );
                  vm2.execute ( veh, -1, -1, 0, -1, -1 );
                  if ( vm2.getStatus() != 2 )
                     displaymessage ( "AI :: tactics \n error in movement step 0 with unit %d", 1, veh->networkid );
   
                  vm2.execute ( NULL, mv->movex, mv->movey, 2, -1, -1 );
                  if ( vm2.getStatus() != 3 )
                     displaymessage ( "AI :: tactics \n error in movement step 2 with unit %d", 1, veh->networkid );
   
                  vm2.execute ( NULL, mv->movex, mv->movey, 3, -1,  1 );
                  if ( vm2.getStatus() != 1000 )
                     displaymessage ( "AI :: tactics \n error in movement step 3 with unit %d", 1, veh->networkid );
               }

               VehicleAttack va ( mapDisplay, NULL );
               va.execute ( veh, -1, -1, 0 , 0, -1 );
               if ( va.getStatus() != 2 )
                  displaymessage ( "AI :: tactics \n error in attack step 2 with unit %d", 1, veh->networkid );

               va.execute ( NULL, mv->attackx, mv->attacky, 2 , -1, mv->weapNum );
               if ( va.getStatus() != 1000 )
                  displaymessage ( "AI :: tactics \n error in attack step 3 with unit %d", 1, veh->networkid );

            }
         }
      }
      veh = veh->next;
   }
   displaymessage2("threats completed ... "); 
}


void AI:: run ( void )
{
   tempsvisible = false; 
   setup();
   tempsvisible = true; 

   tactics();
}

void AI :: showFieldInformation ( int x, int y )
{
   if ( fieldThreats ) {
      const char* fieldinfo = "#font02#Field Information#font01##aeinzug20##eeinzug10##crtp10#"
                              "threat orbit: %d\n"
                              "threat high-level flight: %d\n"
                              "threat flight: %d\n"
                              "threat low-level flight: %d\n"
                              "threat ground level: %d\n"
                              "threat floating: %d\n"
                              "threat submerged: %d\n"
                              "threat deep submerged: %d\n";
   
      char text[1000];
      int pos = x + y * activemap->xsize;
      sprintf(text, fieldinfo, fieldThreats[pos].threat[7], fieldThreats[pos].threat[6], fieldThreats[pos].threat[5], 
                               fieldThreats[pos].threat[4], fieldThreats[pos].threat[3], fieldThreats[pos].threat[2],
                               fieldThreats[pos].threat[1], fieldThreats[pos].threat[0] );

      pfield fld = getfield (x, y );
      if ( fld->vehicle && fieldvisiblenow ( fld )) {
         char text2[1000];
         sprintf(text2, "\nunit nwid: %d ; typeid: %d", fld->vehicle->networkid, fld->vehicle->typ->id );
         strcat ( text, text2 );
      }
      tviewanytext vat;
      vat.init ( "AI information", text );
      vat.run();
      vat.done();
   }
}


AI :: ~AI ( )
{
   if ( fieldThreats ) {
      delete[] fieldThreats;
      fieldThreats = NULL;
      fieldNum = 0;
   }
}
