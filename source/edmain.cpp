/*! \file edmain.cpp
    \brief The map editor's main program 
*/

//     $Id: edmain.cpp,v 1.67.2.9 2005-06-12 11:05:16 mbickel Exp $

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
#include "paradialog.h"
#include "soundList.h"
#include "maped-mainscreen.h"

#include <signal.h>

#ifdef _DOS_
 #include "dos\memory.h"
#endif

// #define backgroundpict1 "BKGR2.PCX"  
#define menutime 35

//* �S Load-Functions

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

   dataLoaderTicker.connect(SigC::slot( *actprogressbar, &tprogressbar::point ));
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
      generatemap(terrainTypeRepository.getObject_byID(30)->weather[0], 10, 20);
   else
      generatemap(terrainTypeRepository.getObject_byPos(0)->weather[0], 10, 20);
}


void checkLeftMouseButton ( )
{
/*
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
*/
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



//* �S Diverse

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

//* �S Main-Program


pfont load_font(char* name)
{
   tnfilestream stream ( name, tnstream::reading );
   return loadfont ( &stream );
}




int mapeditorMainThread ( void* _mapname )
{
   const char* mapname = (const char*) _mapname;
   loadpalette();

  
   try {
      GraphicSetManager::Instance().loadData();
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

//   addmouseproc ( &mousescrollproc );

   mainScreenWidget = new MainScreenWidget( getPGApplication());
   mainScreenWidget->Show();
   
   mousevisible(true);

   gameStartupComplete = true;
   getPGApplication().Run();
   return 0;
}

// including the command line parser, which is generated by genparse
#include "clparser/mapedit.cpp"

void setSaveNotification()
{
   mapsaved = false;
}

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

   fullscreen = !CGameOptions::Instance()->mapeditWindowedMode;
   if ( cl->f() )
      fullscreen = 1;
   if (  cl->w() )
      fullscreen = 0;

   checkDataVersion();

   SDLmm::Surface* icon = NULL;
   try {
      tnfilestream iconl ( "icon_mapeditor.gif", tnstream::reading );
      SDL_Surface *icn = IMG_LoadGIF_RW( SDL_RWFromStream ( &iconl ));
      SDL_SetColorKey(icn, SDL_SRCCOLORKEY, *((Uint8 *)icn->pixels));
      icon = new SDLmm::Surface ( icn );
   }
   catch ( ... ) {
   }



   // determining the graphics resolution
   int xr  = CGameOptions::Instance()->mapeditor_xresolution;
   if ( cl->x() != 800 )
      xr = cl->x();

   int yr  = CGameOptions::Instance()->mapeditor_yresolution;
   if ( cl->y() != 600 )
      yr = cl->y();

   ASC_PG_App app ( "asc2_dlg" );
   
   int flags = SDL_SWSURFACE;
   if ( fullscreen )
      flags |= SDL_FULLSCREEN;
   
   app.InitScreen( xr, yr, 32, flags);
      
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


   mapChanged.connect( SigC::hide<tmap*>( repaintMap.slot() ) );
   mapChanged.connect( SigC::hide<tmap*>( updateFieldInfo.slot() ) );
   mapChanged.connect( SigC::hide<tmap*>( SigC::slot( setSaveNotification) ));
   
   char* buf = new char[cl->l().length()+10];
   strcpy ( buf, cl->l().c_str() );
   initializeEventHandling ( mapeditorMainThread, buf, icons.mousepointer );
   delete[] buf;

   writegameoptions ();

  #ifdef MEMCHK
   verifyallblocks();
  #endif
   return 0;
}

