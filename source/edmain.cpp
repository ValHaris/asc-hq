//     $Id: edmain.cpp,v 1.8 2000-03-16 14:06:54 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.7  2000/02/24 10:54:08  mbickel
//      Some cleanup and bugfixes
//
//     Revision 1.6  2000/02/03 20:54:39  mbickel
//      Some cleanup
//      getfiletime now works under Linux too
//
//     Revision 1.5  1999/12/27 12:59:54  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.4  1999/12/07 22:05:08  mbickel
//      Added password verification for loading maps
//
//     Revision 1.3  1999/11/22 18:27:16  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:41:36  tmwilson
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
#include "loadpcx.h"
#include "timer.h"
#include "loadbi3.h"
#include "edselfnt.h"
#include "edglobal.h"
#include <signal.h>
#include <new.h>

#ifdef _DOS_
 #include "dos\memory.h"
#endif

// #define backgroundpict1 "BKGR2.PCX"  
#define menutime 35

//* õS Load-Functions

pprogressbar actprogressbar = NULL;


#ifdef HEXAGON
const char* progressbarfilename = "progress.6me";
#else
const char* progressbarfilename = "progress.8me";
#endif


// #define MEMCHK


#ifdef MEMCHK

  int blocknum = 0;
  int blocklist[1000000];


  void addblock ( void* p )
  {
     blocklist[blocknum++] = (int) p;
  }

  int removeblock ( void* p )
  {
     if ( blocknum== 0 )
        return 0;

     int error = 0;
     int found = 0;
     int pos = 0;
     while ( !found && pos < blocknum) {
        if ( blocklist[pos] == (int) p )
           found++;
        else
           pos++;
     } /* endwhile */
     if ( found ) {
        for ( int i = pos+1; i < blocknum; i++ )
           blocklist[i-1] = blocklist[i];
        blocknum--;
     } 

     return found;
  }

  int blockavail( void* p )
  {
     int found = 0;
     int pos = 0;
     while ( !found && pos < blocknum) {
        if ( blocklist[pos] == (int) p )
           found++;
        else
           pos++;
     } /* endwhile */
     return found;
  }


  void* memchkAlloc ( int tp, size_t amt )
  {
     int error;
     void* tmp = malloc ( amt + 53 * 4 );
     int* tmpi = (int*) tmp;
     /*
     if ( (int) tmpi == 0x1bb2138 || (int) tmpi == 0x1bcf178 ) 
        error++;
        */
     tmpi[0] = tp;
     tmpi[1] = (int) tmp;
     tmpi[2] = amt;
     for ( int i = 0; i < 25; i++ ) {
        tmpi[3 + i] = 0x12345678;
        tmpi[3 + i + (amt+3)/4 + 25] = 0x87654321;
     }
     void* p = &tmpi[28];

     addblock ( p );
     return p;
  }


  void* verifyblock ( int tp, void* p )
  {
     int error = 0;
     int* tmpi = (int*) p;
     tmpi -= 28;

     if ( tp != -1 )
        if ( tmpi[0] != tp )
           error++;

     if ( tmpi[1] != (int) tmpi)
        error++;

     int amt = tmpi[2];

     for ( int i = 0; i < 25; i++ ) {

        if ( tmpi[3 + i] != 0x12345678)
           error++;

        if ( tmpi[3 + i + (amt+3)/4 + 25] != 0x87654321 )
           error++;
     }
     return tmpi;
  }

  void verifyallblocks ( void )
  {
     for ( int i = 0; i < blocknum; i++ )
        verifyblock ( -1, (void*) blocklist[i] );
  }

  void memchkFree ( int tp, void* buf )
  {
     if ( removeblock ( buf )) {
        void* tmpi = verifyblock ( tp, buf );
        free ( tmpi );
     } else
       free ( buf );
  }

  void *operator new( size_t amt )
  {
      return( memchkAlloc( 100, amt ) );
  }
  
  void operator delete( void *p )
  {
     if ( p )
      memchkFree( 100, p );
  }
  
  void *operator new []( size_t amt )
  {
      return( memchkAlloc( 102, amt ) );
  }
  
  void operator delete []( void *p )
  {
     if ( p )
      memchkFree( 102, p );
  }

  void* asc_malloc ( size_t size )
  {
     void* tmp = memchkAlloc ( 104, size );
     if ( tmp == NULL ) 
        new_new_handler();
     return tmp;
  }

  void asc_free ( void* p )
  {
     memchkFree ( 104, p );
  }

#else

  void* asc_malloc ( size_t size )
  {
     void* tmp = malloc ( size );
     if ( tmp == NULL ) 
        new_new_handler();
     return tmp;
  }

  void asc_free ( void* p )
  {
     free ( p );
  }


#endif




void         loadcursor(void)
{ 
  int          w,i;

  {
     #ifdef HEXAGON
      tnfilestream stream ("hexfeld.raw", 1);
     #else
      tnfilestream stream ("oktfld2.raw", 1);
     #endif
      stream.readrlepict ( &icons.fieldshape, false, &w );
  }

   #ifdef FREEMAPZOOM 
   {
      tnfilestream stream ("mapbkgrb.raw", 1);
      stream.readrlepict ( &icons.mapbackground, false, &w );
   }
   #endif

  {
     tnfilestream stream ("mausi.raw",1);
     stream.readrlepict( &icons.mousepointer, false, &w);
  }

  {
    #ifdef HEXAGON
      tnfilestream stream ("hexfld_a.raw",1); 
    #else
      tnfilestream stream ("markacti.raw",1); 
    #endif
    stream.readrlepict( &icons.stellplatz, false, &w);
  }

  {
    tnfilestream stream ("x.raw",1); 
    stream.readrlepict( &icons.X, false, &w);
  }

  {
     tnfilestream stream ("pfeil-a0.raw",1);
     for (i=0;i<8 ;i++ ) stream.readrlepict( &icons.pfeil2[i], false, &w);
  }

   loadpalette();
   for (w=0;w<256 ;w++ ) {
      palette16[w][0] = pal[w][0];
      palette16[w][1] = pal[w][1];
      palette16[w][2] = pal[w][2];
      xlattables.nochange[w] = w;
   } /* endfor */

   weapdist = new tweapdist;
   loadicons(); 
   loadmessages(); 

} 



void loadUnitSets ( void )
{
   tfindfile ff ( "*.set" );
   char* n = ff.getnextname();
   int setnum = 0;
   while ( n ) {
      tnfilestream s ( n, 1 );
      char buf[10000];
      char buf2[10000];
      int read = s.readdata ( buf, 10000, 0 );
      buf[read] = 0;

      if ( buf[0] ) {
         strcpy ( buf2, buf );
         int rangenum = 0;

         char* filename = strtok ( buf, ";\r\n");
         unitSet.set[setnum].init ( filename );

         char* piclist = strtok ( NULL, ";\r\n" );

         char* pic = strtok ( piclist, "," );
         while ( pic ) {
            int from, to;
            if ( strchr ( pic, '-' )) {
               char* a = strchr ( pic, '-' );
               *a = 0;
               from = atoi ( pic );
               to = atoi ( ++a );
            } else 
               from = to = atoi ( pic );

            unitSet.set[setnum].ids[rangenum].from = from;
            unitSet.set[setnum].ids[rangenum].to   = to;

            rangenum ++;
            pic = strtok ( NULL, "," );
         }

         strcpy ( buf, buf2 );

         dynamic_array<char*> transtable;
         int transtablenum = 0;
         const char* sectionlabel = "#";
         char* transstart  = strstr ( buf, sectionlabel );
         if ( transstart ) {
            char* pc = strtok ( transstart, "#\n\r" );
            while ( pc ) {
               transtable[transtablenum++] = pc;
               pc = strtok ( NULL, "#\n\r" );
            }
         }
         for ( int t = 0; t < transtablenum; t++ ) {
            char* tname = strtok ( transtable[t], ";" );
            char* trans = strtok ( NULL, ";" );
            int entrynum = 0;
            if ( trans ) 
               strcpy ( unitSet.set[setnum].transtab[t].name , tname );
            
            while ( trans ) {
               char* pc = strchr ( trans, ',' );
               unitSet.set[setnum].transtab[t].translation[entrynum].to = atoi ( pc+1 );
               *pc = 0;
               unitSet.set[setnum].transtab[t].translation[entrynum].from = atoi ( trans );
               entrynum++;

               trans = strtok ( NULL, ";" );
            } /* endwhile */

         }

         setnum++;
      }

      n = ff.getnextname();
   } /* endwhile */
}


void loaddata( void ) 
{

   actprogressbar = new tprogressbar; 
   {
      tfindfile ff ( progressbarfilename );
      if ( ff.getnextname() ) {
         tnfilestream strm ( progressbarfilename, 1 );
         actprogressbar->start ( 255, 0, agmp->resolutiony-2, agmp->resolutionx-1, agmp->resolutiony-1, &strm );
      } else
         actprogressbar->start ( 255, 0, agmp->resolutiony-2, agmp->resolutionx-1, agmp->resolutiony-1, NULL );
   }

   if ( actprogressbar )
      actprogressbar->startgroup();
   loadcursor();

   if ( actprogressbar )
      actprogressbar->startgroup();

   loadguipictures();
   loadallobjecttypes();

   if ( actprogressbar )
      actprogressbar->startgroup();
   loadallvehicletypes();

   if ( actprogressbar )
      actprogressbar->startgroup();
   loadallbuildingtypes();

   if ( actprogressbar )
      actprogressbar->startgroup();
   loadalltechnologies();

   if ( actprogressbar )
      actprogressbar->startgroup();
   loadstreets();

   if ( actprogressbar )
      actprogressbar->startgroup();
   loadallterraintypes();

   if ( actprogressbar )
      actprogressbar->startgroup();

  #ifndef FREEMAPZOOM
   idisplaymap.setup_map_mask ( );
  #endif

   loadUnitSets();

   if ( actprogressbar ) {
      actprogressbar->end();
      tnfilestream strm ( progressbarfilename, 2 );
      actprogressbar->writetostream( &strm );
      delete actprogressbar;
      actprogressbar = NULL;
   }
}

void buildemptymap ( void )
{
  #ifdef HEXAGON
   if ( getterraintype_forid(30) )
      generatemap(getterraintype_forid(30)->weather[0], idisplaymap.getscreenxsize(1), idisplaymap.getscreenysize());
   else
      generatemap(getterraintype_forpos(0)->weather[0], idisplaymap.getscreenxsize(1), idisplaymap.getscreenysize());
  #else
   if ( getterraintype_forid(1) )
      generatemap(getterraintype_forid(1)->weather[0], idisplaymap.getscreenxsize(1), idisplaymap.getscreenysize());
   else
      generatemap(getterraintype_forpos(0)->weather[0], idisplaymap.getscreenxsize(1), idisplaymap.getscreenysize());
  #endif
}


void         editor(void)
{  int execcode;
   int lastx;
   int lasty;
   boolean curposchanged;

   cursor.show();
   do {
      try {
         do { 
            if ( ( keypress() ) || ( ( ch >=ct_f3) && (ch<=ct_f9) ) ) {
               if (! ( ( ch >=ct_f3) && (ch<=ct_f9) ) ) ch = r_key();
               pd.key = ch;
                  switch (ch) {
                  #ifdef NEWKEYB
                  case ct_up:
                  case ct_down:
                  case ct_left:
                  case ct_right:
                  case ct_up + ct_stp:
                  case ct_down + ct_stp:
                  case ct_left + ct_stp:
                  case ct_right + ct_stp:
                  #endif
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
                  case ct_9k + ct_stp:   if ( polyfieldmode == false ) { 
                                                    mousevisible(false); 
                                                    movecursor(ch); 
                                                    cursor.show(); 
                                                    showcoordinates();
                                                    mousevisible(true); 
                                                 }
                     break;
                  case ct_f1:   execaction(act_help);
                     break;
                  case ct_f3 : execaction(act_selbodentyp);
                     break;
                  case ct_f4 : execaction(act_selunit);
                     break;
                  case ct_f5 : execaction(act_selcolor);
                     break;
                  case ct_f6 : execaction(act_selbuilding);
                     break;
                  case ct_f7 : execaction(act_selobject);
                     break;
                  case ct_f8 : execaction(act_selmine);
                     break;
                  case ct_f9 : execaction(act_selweather);
                     break;
                  case ct_a + ct_stp :  execaction(act_setupalliances);
                     break;
                  case ct_b + ct_stp:  execaction(act_toggleresourcemode);
                     break;
                  case ct_d + ct_stp : execaction(act_changeglobaldir);
                     break;
                  case ct_f + ct_stp: execaction(act_createresources);
                     break;
                  case ct_g + ct_stp: execaction(act_maptopcx);  
                     break;
                  case ct_h + ct_stp : execaction(act_setunitfilter);
                     break;
                  case ct_i + ct_stp: execaction (act_import_bi_map );
                     break;
                  case ct_l + ct_stp : execaction(act_loadmap); 
                     break;
                  case ct_m + ct_stp: execaction(act_changemapvals);
                     break;
                  case ct_n + ct_stp: execaction(act_newmap);
                     break;
                  case ct_o + ct_stp: execaction(act_polymode);  
                     break;            
                  case ct_r + ct_stp: execaction(act_repaintdisplay);
                     break;
                  case ct_u + ct_stp : execaction(act_unitinfo);
                     break;
                  case ct_v + ct_stp: execaction(act_viewmap);
                     break;
                  case ct_w + ct_stp : execaction(act_setactweatherglobal);
                     break;
                  case ct_x + ct_stp: execaction(act_end);
                     break;
                  case ct_d + ct_shp: execaction(act_changeunitdir);
                     break;
                  case ct_a:   execaction(act_movebuilding);
                     break;
                  case ct_b:   execaction(act_changeresources);
                     break;
                  case ct_c:   execaction(act_changecargo); 
                     break;
                  case ct_d : execaction(act_changeterraindir);
                     break;
                  case ct_e:  execaction(act_events);
                     break;
                  case ct_f:  execaction(act_fillmode); 
                     break;
                  case ct_g: execaction(act_mapgenerator);
                     break;
                  case ct_h: execaction(act_setactivefieldvals);
                     break;
                  case ct_entf: execaction(act_deletething); 
                      break;
                  case ct_l : execaction(act_showpalette);
                     break;
                  case ct_m : execaction(act_changeminestrength);
                     break;
                  case ct_o: execaction(act_changeplayers);
                     break;
                  case ct_p: execaction(act_changeproduction);
                     break;
                  case ct_r: execaction(act_resizemap);
                     break;
                  case ct_s : execaction(act_savemap);
                     break;
                  case ct_v:   execaction(act_changeunitvals); 
                     break;
                  case ct_x:   execaction(act_mirrorcursorx);
                     break;
                  case ct_y:   execaction(act_mirrorcursory);
                     break;
                  case ct_z:   execaction(act_setzoom );
                     break;
                  case ct_3 : execaction(act_placebodentyp);
                     break;
                  case ct_4 : execaction(act_placeunit);
                     break;
                  case ct_6 : execaction(act_placebuilding);
                     break;
                  case ct_7 : execaction(act_placeobject);
                     break;
                  case ct_8 : execaction(act_placemine);
                     break;
                  case ct_space : execaction(act_placething);
                     break;
                  case ct_esc : {
                        if (polyfieldmode) execaction(act_endpolyfieldmode);
                        else execaction(act_end);
                     }
                     break;
                  }
            } 
            pulldown();
            checkselfontbuttons();
            if ( mouseparams.taste == 1 ) {
               int mx, my;
               starttimer();
               curposchanged = false;
               while ( mouseparams.taste == 1 ) {
                 if ( getfieldundermouse ( &mx, &my ) ) 
                    if ( ( mx != lastx ) || (my != lasty ) ) {
                       mousevisible(false);
                       cursor.hide();
                       cursor.posx = mx;
                       cursor.posy = my;
                       cursor.show();
                       mousevisible(true);
      
                       lastx = mx;
                       lasty = my;
                       curposchanged = true;
                       starttimer();
                    }
                    if (time_elapsed(menutime)) {
                       execcode = -1;
                       execcode = leftmousebox();
                       if (execcode != -1 ) execaction(execcode);
                       while ( mouseparams.taste != 0 );
                  } 
               }
               if (getfieldundermouse ( &mx, &my ) ) 
                  if ( ! time_elapsed(menutime)) {
                       if ( ! curposchanged ) { 
                          execaction(act_placething);
                          while ( mouseparams.taste == 1 );
                       }
                  }
            }
            if ( mouseparams.taste == 2 ) {
               int mx, my;
               while ( mouseparams.taste == 2 ) {
                 if ( getfieldundermouse ( &mx, &my ) )  {
                    mousevisible(false);
                    cursor.hide();
                    cursor.posx = mx;
                    cursor.posy = my;
                    cursor.show();
                    mousevisible(true);
      
                    lastx = mx;
                    lasty = my;
      
                    execcode = -1;
                    execcode = rightmousebox();
                    if (execcode != -1) execaction(execcode);
      
                    while ( mouseparams.taste != 0 );
                  }
               }
            }
      
            checkformousescrolling();
      
         }  while (! (ch == ct_esc) || (ch == ct_altp+ct_x ) );
      } /* endtry */
      catch ( tnomaploaded ) {
         buildemptymap();
         repaintdisplay();
      } /* endcatch */
   }  while (! (ch == ct_esc) || (ch == ct_altp+ct_x ) );
} 

//* õS Close-Functions

void closemouse ( void )
{
   removemousehandler();
}


//* õS Diverse

void dispmessageonexit ( void ) {
   int i;
   printf("\n");
   if (exitmessage[0] != NULL) {
      for (i=0;i<20 ;i++ ) {
          if (exitmessage[i] != NULL) {
             printf(exitmessage[i]);
             printf("\n");
          } /* endif */
      }
   } else {
      printf("exiting ... \n \n");
   } /* endif */
}


void closesvgamode( void )
{
   closegraphics();
}    


void showmemory ( void )
{
   npush ( activefontsettings );
   activefontsettings.length = 99;
   activefontsettings.background = 0;
   activefontsettings.color = 14;
   activefontsettings.justify = lefttext;
   activefontsettings.font = schriften.smallarial;
   int mss = getmousestatus();

   int a = maxavail();
   int b = _memavl();
   showtext2( strrr ( a ), 210,410 );
   showtext2( strrr ( b+a ), 310,410 );
   showtext2( strrr ( _memmax() ), 10,410 );
   showtext2( strrr ( b  ), 110,480 );
   showtext2( strrr ( stackfree ()  ), 410,410 );
   if (mss == 2)
      setinvisiblemouserectangle ( -1, -1, -1, -1 );

   npop  ( activefontsettings );
};

//* õS Main-Program


pfont load_font(char* name)
{
   tnfilestream stream ( name, 1 );
   return loadfont ( &stream );
}





int main(int argc, char *argv[] )
{ 
   signal ( SIGINT, SIG_IGN );
   int resolx = 800;
   int resoly = 600;
   int              modenum8;
   int              i;

   printf( kgetstartupmessage() );

   #ifdef logging
   logtofile ( kgetstartupmessage() );
   logtofile ( "\n new log started \n ");
   #endif


      for (i = 1; i<argc; i++ ) {
           if ( argv[i][0] == '/'  ||  argv[i][0] == '-' ) {
              if ( strcmpi ( &argv[i][1], "V1" ) == 0 ) 
                 vesaerrorrecovery = 1;
              else
              if ( strnicmp ( &argv[i][1], "x=", 2 ) == 0 ) {
                 resolx = atoi ( &argv[i][3] );
              } else
              if ( strnicmp ( &argv[i][1], "x:" ,2 ) == 0 ) {
                 resolx = atoi ( &argv[i][3] );
              } else
              if ( strnicmp ( &argv[i][1], "y=" ,2 ) == 0 ) {
                 resoly = atoi ( &argv[i][3] );
              } else
              if ( strnicmp ( &argv[i][1], "y:" ,2 ) == 0 ) {
                 resoly = atoi ( &argv[i][3] );
              }
              else 
                if ( ( strcmpi ( &argv[i][1], "?" ) == 0 ) || ( strcmpi ( &argv[i][1], "h" ) == 0 ) ){
                   printf( " Parameters: \n ");
                   printf( "    /h      This page\n ");
                   printf( "    /v1     Set vesa error recovery level to 1 \n");
                   printf( "    /x:X    Set horizontal resolution to X; default is 800 \n");
                   printf( "    /y:Y    Set verticalal resolution to Y; default is 600 \n\n");

                   exit (0);

                } else {
                    printf ( "\nInvalid command line parameter: %s \n", argv[i] );
                    exit(1);
                }
           } else {
               printf ( "\nInvalid command line parameter: %s \n", argv[i] );
               exit(1);
           }
        } /* endfor */
   
   initmisc ();

   t_carefor_containerstream cfc;

       #ifdef HEXAGON
        readgameoptions();
        check_bi3_dir ();
       #endif


   memset(exitmessage, 0, sizeof ( exitmessage ));
   atexit ( dispmessageonexit );
        
   #ifdef _DOS_
    initmemory();
   #endif
   
   modenum8 = initgraphics( resolx, resoly, 8 );
   if ( modenum8 < 0 )
      return 1;
   atexit ( closesvgamode ); 


   inittimer(100);
   atexit ( closetimer );

   schriften.smallarial = load_font("smalaril.FNT");
   schriften.large = load_font("USABLACK.FNT");
   schriften.arial8 = load_font("ARIAL8.FNT");
   schriften.smallsystem = load_font("msystem.fnt");
   schriften.monogui = load_font("monogui.fnt");
   

   virtualscreenbuf.init();

   {
       tnfilestream stream ( "logo640.pcx", 1 );
       loadpcxxy( &stream, (hgmp->resolutionx - 640)/2, (hgmp->resolutiony-35)/2, 1 ); 
       int whitecol = 251;
       activefontsettings.font = schriften.smallarial;
       activefontsettings.background = 255;
       activefontsettings.justify = centertext;
       activefontsettings.color = whitecol;
       activefontsettings.length = hgmp->resolutionx-20;
       showtext2 ("Map Editor", 10, hgmp->resolutiony - activefontsettings.font->height - 2 );

   }
   #ifdef HEXAGON
       initspfst( -1, -1 );
   #else
      if ( resolx == 640  && resoly == 480 )
         initspfst();
      else
         initspfst( -1, -1 );
   #endif

   try {
      loaddata();
   } /* end try */
   catch ( tfileerror err ) {
      displaymessage ( " error loading file %s ",2,err.filename );
   } /* end catch */

   if ( initmousehandler( icons.mousepointer )) 
      displaymessage("mouse required", 2 );

   pulldownfont = schriften.smallarial;

   activefontsettings.font = schriften.arial8;
   activefontsettings.color =lightblue ;
   activefontsettings.background =3;
   activefontsettings.length =100;
   activefontsettings.justify =lefttext;

   //tplasma plasma;
   //plasma.process(10,10,2);
 

//   showtext2("Loading ...",80,200); 

   cursor.init();
               
   atexit( closemouse );
      
   xlatpictgraytable = (ppixelxlattable) malloc( sizeof(*xlatpictgraytable) );
   generategrayxlattable(xlatpictgraytable,160,16); 


   buildemptymap();

   //loadmap("test.map");

   //showmemory();
   //r_key();

   ( * xlatpictgraytable)[255] = 255;

   
   //konst2();
   setstartvariables(); 
   
   addmouseproc ( &mousescrollproc );

   godview = true; 

   #ifdef NEWKEYB   
       initkeyb(); 
       lasttick = ticker; 
   #endif   

   bar( 0, 0, hgmp->resolutionx-1, hgmp->resolutiony-1, 0 );
   setvgapalette256(pal); 

   displaymap();
   showallchoices();
   //pulldownbaroff();
   pdsetup();
   pdbaroff();

   mousevisible(true); 
   cursor.show();
 
   editor();
   cursor.hide();
   writegameoptions ();
  #ifdef MEMCHK
   verifyallblocks();
  #endif
   return 0;
}

