/*! \file edmain.cpp
    \brief The map editor's main program 
*/

//     $Id: edmain.cpp,v 1.67.2.1 2004-10-26 16:35:04 mbickel Exp $

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

#include <algorithm>
#include <memory>
#include <SDL_image.h>


#include "edmisc.h"
#include "loadpcx.h"
#include "loadbi3.h"
#include "edselfnt.h"
#include "edglobal.h"
#include "errors.h"
#include "gameoptions.h"
#include "mapdisplay.h"
#include "itemrepository.h"
#include "loadimage.h"
#include "graphicset.h"

#include <signal.h>

#ifdef _DOS_
 #include "dos\memory.h"
#endif

// #define backgroundpict1 "BKGR2.PCX"  
#define menutime 35

//* õS Load-Functions

pprogressbar actprogressbar = NULL;


const char* progressbarfilename = "progress.6me";


// #define MEMCHK
#include "memorycheck.cpp"



void         loadcursor(void)
{ 
  int          w,i;

  {
      tnfilestream stream ("hexfeld.raw", tnstream::reading);
      stream.readrlepict ( &icons.fieldshape, false, &w );
  }

  {
      tnfilestream stream ("mapbkgrb.raw", tnstream::reading);
      stream.readrlepict ( &icons.mapbackground, false, &w );
  }

  {
      tnfilestream stream ("hexfld_a.raw", tnstream::reading);
    stream.readrlepict( &icons.stellplatz, false, &w);
  }

  {
    tnfilestream stream ("x.raw", tnstream::reading);
    stream.readrlepict( &icons.X, false, &w);
  }

  {
     tnfilestream stream ("pfeil-a0.raw", tnstream::reading);
     for (i=0;i<8 ;i++ ) stream.readrlepict( &icons.pfeil2[i], false, &w);
  }

   loadpalette();
   for (w=0;w<256 ;w++ ) {
      palette16[w][0] = pal[w][0];
      palette16[w][1] = pal[w][1];
      palette16[w][2] = pal[w][2];
      xlattables.nochange[w] = w;
   } /* endfor */

   loadicons(); 
   loadmessages(); 

} 





void loaddata( void ) 
{

   actprogressbar = new tprogressbar; 
   {
      tfindfile ff ( progressbarfilename );
      if ( !ff.getnextname().empty() ) {
         tnfilestream strm ( progressbarfilename, tnstream::reading );
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

   // ItemFiltrationSystem::read();

   if ( actprogressbar )
      actprogressbar->startgroup();

   GraphicSetManager::Instance().loadData();
      
   registerDataLoader ( new PlayListLoader() );
   registerDataLoader ( new BI3TranslationTableLoader() );
   
   loadAllData();

   if ( actprogressbar )
      actprogressbar->startgroup();

   loadUnitSets();

   if ( actprogressbar ) {
      actprogressbar->end();
      tnfilestream strm ( progressbarfilename, tnstream::writing );
      actprogressbar->writetostream( &strm );
      delete actprogressbar;
      actprogressbar = NULL;
   }
}

void buildemptymap ( void )
{
   if ( terrainTypeRepository.getObject_byID(30) )
      generatemap(terrainTypeRepository.getObject_byID(30)->weather[0], idisplaymap.getscreenxsize(1), idisplaymap.getscreenysize());
   else
      generatemap(terrainTypeRepository.getObject_byPos(0)->weather[0], idisplaymap.getscreenxsize(1), idisplaymap.getscreenysize());
}


void checkLeftMouseButton ( )
{
   static int buttonStat = 0;

   int x,y;
   if ( getfieldundermouse ( &x, &y ) ) {
      x += actmap->xpos;
      y += actmap->ypos;

      if ( mouseparams.taste )
         cursor.gotoxy ( x,y );

      if ( mapSwitcher.getDefaultAction() == MapSwitcher::select ) {
         if ( mouseparams.taste == 1 ) {
            execaction(act_setactivefieldvals);
            while ( mouseparams.taste == 1 )
               releasetimeslice();
         }

      } else {

         if ( mouseparams.taste == 1 ) {
            if ( buttonStat < 2 ) {
               execaction(act_placething);
               if ( lastselectiontype == cselunit || lastselectiontype == cselcolor || lastselectiontype == cselbuilding )
                  buttonStat = 2;
               else {
                 bool moved = false;
                 do {
                    int x1,y1;
                    if ( getfieldundermouse ( &x1, &y1 ) ) {
                       x1 += actmap->xpos;
                       y1 += actmap->ypos;
                       if ( x1 != x || y1 != y )
                          moved = true;
                    } else
                       moved = true;

                    if ( mouseparams.taste != 1 )
                       moved = true;
                 } while ( !moved );
               }
            }
         } else
            if ( buttonStat )
               buttonStat = 0;
      }
   } 

/*
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
                  releasetimeslice();
               }
               if (getfieldundermouse ( &mx, &my ) )
                  if ( ! time_elapsed(menutime)) {
                       if ( ! curposchanged ) {
                          execaction(act_placething);
                          while ( mouseparams.taste == 1 )
                             releasetimeslice();
                       }
                  }
            }
*/

}


void         editor(void)
{  int execcode;

   cursor.show();
   do {
      try {
         do { 
            if ( keypress() ) {
               ch = r_key();

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
                                                    showStatusBar();
                                                    mousevisible(true);
                                                 }
                     break;
                  case ct_f1:   execaction(act_help);
                     break;
                  case ct_f7 :
                  case ct_f3 : execaction(act_selbodentyp);
                     break;
                  case ct_f3 + ct_shp :
                  case ct_f3 + ct_altp :
                  case ct_f3 + ct_stp : execaction(act_selbodentypAll);
                     break;
                  case ct_f4 : execaction(act_selunit);
                     break;
                  case ct_f5 : execaction(act_selcolor);
                     break;
                  case ct_f6 : execaction(act_selbuilding);
                     break;
                  case ct_f7 + ct_shp :
                  case ct_f7 + ct_altp :
                  case ct_f7 + ct_stp : execaction(act_selobject);
                     break;
                  case ct_f8 : execaction(act_selmine);
                     break;
                  case ct_f9 : execaction(act_selweather);
                     break;
                  case ct_a + ct_stp :  execaction(act_setupalliances);
                     break;
                  case ct_b + ct_stp:  execaction(act_toggleresourcemode);
                     break;
                  case ct_c + ct_stp:  execaction(act_copyToClipboard);
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
                  case ct_p + ct_stp: execaction(act_changeproduction);
                     break;
                  case ct_r + ct_stp: execaction(act_repaintdisplay);
                     break;
                  case ct_u + ct_stp : execaction(act_unitinfo);
                     break;
                  case ct_v + ct_stp: execaction(act_pasteFromClipboard);
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
                  case ct_p: execaction(act_changeunitvals);
                     break;
                  case ct_r: execaction(act_resizemap);
                     break;
                  case ct_s : execaction(act_savemap);
                     break;
                  case ct_v:   execaction(act_viewmap);
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
                  case ct_tab: execaction(act_switchmaps );
                     break;
                  case ct_enter :
                  case ct_space : if ( mapSwitcher.getDefaultAction() == MapSwitcher::select ) 
                                    execaction(act_setactivefieldvals);
                                  else
                                    execaction(act_placething);
                     break;
                  case ct_esc : {
                        if ( polyfieldmode )
                           execaction(act_endpolyfieldmode);
                        else
                           execaction(act_end);
                     }
                     break;
                  }
            } 
            pulldown();
            checkselfontbuttons();
            checkLeftMouseButton();
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

                    execcode = rightmousebox();
                    if (execcode != -1) execaction(execcode);

                    while ( mouseparams.taste != 0 )
                       releasetimeslice();
                  }
                  releasetimeslice();
               }
            }
      
            checkformousescrolling();
            releasetimeslice();
      
         }  while (! (ch == ct_esc) || (ch == ct_altp+ct_x ) );
      } /* endtry */
      catch ( NoMapLoaded ) {
         buildemptymap();
         repaintdisplay();
      } /* endcatch */
   }  while (! (ch == ct_esc) || (ch == ct_altp+ct_x ) );
}


//* õS Diverse

void closesvgamode( void )
{
   closegraphics();
}    


void showmemory ( void )
{
  #ifdef _DOS_
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
   // showtext2( strrr ( _memmax() ), 10,410 );
   showtext2( strrr ( b  ), 110,480 );
   showtext2( strrr ( stackfree ()  ), 410,410 );
   if (mss == 2)
      setinvisiblemouserectangle ( -1, -1, -1, -1 );

   npop  ( activefontsettings );
  #endif
}

//* õS Main-Program


pfont load_font(char* name)
{
   tnfilestream stream ( name, tnstream::reading );
   return loadfont ( &stream );
}




int mapeditorMainThread ( void* _mapname )
{
   const char* mapname = (const char*) _mapname;
   initMapDisplay( );

   cursor.init();

   try {
      loaddata();

      if ( mapname && mapname[0] ) {
         if( patimat ( savegameextension, mapname )) {
            if( validatesavfile( mapname ) == 0 )
               fatalError ( "The savegame %s is invalid. Aborting.", mapname );

            try {
               loadgame( mapname );
            } catch ( tfileerror ) {
               fatalError ( "%s is not a legal savegame. ", mapname );
            }
         } else
            if( patimat ( mapextension, mapname )) 
               loadmap ( mapname );
            else
               fatalError ( "%s is not an accepted file format", mapname );
      } else
         buildemptymap();

      mapSwitcher.toggle();
      if ( exist ( "palette.map" ))
         loadmap ( "palette.map");
      else
         buildemptymap();

      actmap->preferredFileNames.mapname[0] = "";

      mapSwitcher.toggle();

   } /* end try */
   catch ( ParsingError err ) {
      displaymessage ( "Error parsing text file " + err.getMessage(), 2 );
   }
   catch ( tfileerror err ) {
      displaymessage ( " error loading file %s ",2, err.getFileName().c_str() );
   } /* end catch */

   pulldownfont = schriften.smallarial;

   activefontsettings.font = schriften.arial8;
   activefontsettings.color =lightblue ;
   activefontsettings.background =3;
   activefontsettings.length =100;
   activefontsettings.justify =lefttext;


   setstartvariables();

   addmouseproc ( &mousescrollproc );

   bar( 0, 0, hgmp->resolutionx-1, hgmp->resolutiony-1, 0 );
   setvgapalette256(pal);

   displaymap();
   showallchoices();
   pdsetup();
   pdbaroff();

   mousevisible(true);
   cursor.show();

   gameStartupComplete = true;
   editor();
   return 0;
}

// including the command line parser, which is generated by genparse
#include "clparser/mapedit.cpp"

int main(int argc, char *argv[] )
{ 

   Cmdline* cl = NULL;
   auto_ptr<Cmdline> apcl ( cl );
   try {
      cl = new Cmdline ( argc, argv );
   }
   catch ( string s ) {
      cerr << s.c_str();
      exit(1);
   }

   if ( cl->next_param() < argc ) {
      cerr << "invalid command line parameter\n";
      exit(1);
   }

   if ( cl->v() ) {
      ASCString msg = kgetstartupmessage();
      printf( msg.c_str() );
      exit(0);
   }

   verbosity = cl->r();

   int              modenum8;

   #ifdef logging
    logtofile ( kgetstartupmessage() );
    logtofile ( "\n new log started \n ");
   #endif

   #ifdef _DOS_
    if ( showmodes ) {
       showavailablemodes();
       return 0;
    }
   #endif

   signal ( SIGINT, SIG_IGN );

   initFileIO( cl->c().c_str() );

   fullscreen = CGameOptions::Instance()->mapeditForceFullscreenMode;
   if ( cl->f() )
      fullscreen = 1;
   if (  cl->w() )
      fullscreen = 0;

   checkDataVersion();
   GraphicSetManager::Instance().loadData();

   SDLmm::Surface* icon = NULL;
   try {
      tnfilestream iconl ( "icon_mapeditor.gif", tnstream::reading );
      SDL_Surface *icn = IMG_LoadGIF_RW( SDL_RWFromStream ( &iconl ));
      SDL_SetColorKey(icn, SDL_SRCCOLORKEY, *((Uint8 *)icn->pixels));
      icon = new SDLmm::Surface ( icn );
   }
   catch ( ... ) {
   }



   int xr = 800;
   int yr = 600;
   // determining the graphics resolution
   if ( CGameOptions::Instance()->mapeditor_xresolution != 800 )
      xr = CGameOptions::Instance()->mapeditor_xresolution;
   if ( cl->x() != 800 )
      xr = cl->x();

   if ( CGameOptions::Instance()->mapeditor_yresolution != 600 )
      yr = CGameOptions::Instance()->mapeditor_yresolution;
   if ( cl->y() != 600 )
      yr = cl->y();

   modenum8 = initgraphics ( xr, yr, 8, icon );

   if ( modenum8 < 0 )
      return 1;
   atexit ( closesvgamode );

   #ifdef pbpeditor
   setWindowCaption ( "Advanced Strategic Command : PBP Editor ");
   #else
   setWindowCaption ( "Advanced Strategic Command : Map Editor ");
   #endif

   schriften.smallarial = load_font("smalaril.fnt");
   schriften.large = load_font("usablack.fnt");
   schriften.arial8 = load_font("arial8.fnt");
   schriften.smallsystem = load_font("msystem.fnt");
   schriften.monogui = load_font("monogui.fnt");
   

   virtualscreenbuf.init();

   int fs = loadFullscreenImage ( "title_mapeditor.jpg" );
   if ( !fs ) {
      tnfilestream stream ( "logo640.pcx", tnstream::reading );
      loadpcxxy( &stream, (hgmp->resolutionx - 640)/2, (hgmp->resolutiony-35)/2, 1 );
      int whitecol = 251;
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background = 255;
      activefontsettings.justify = centertext;
      activefontsettings.color = whitecol;
      activefontsettings.length = hgmp->resolutionx-20;
      showtext2 ("Map Editor", 10, hgmp->resolutiony - activefontsettings.font->height - 2 );
   }

   {
      int w;
      tnfilestream stream ("mausi.raw", tnstream::reading);
      stream.readrlepict(   &icons.mousepointer, false, &w );
   }


   char* buf = new char[cl->l().length()+10];
   strcpy ( buf, cl->l().c_str() );
   initializeEventHandling ( mapeditorMainThread, buf, icons.mousepointer );
   delete[] buf;

   cursor.hide();
   writegameoptions ();

  #ifdef MEMCHK
   verifyallblocks();
  #endif
   return 0;
}

