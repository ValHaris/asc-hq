
/*! \file map2pcx.cpp
  map2pcx : A program to generate pcx images of maps. This is a separate 
   program and not a module of the ASC main program or the mapeditor
*/

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



#include "typen.h"
#include "loaders.h"
#include "loadpcx.h"
#include "loadbi3.h"
#include "errors.h"
#include "gameoptions.h"
#include "mapdisplay.h"
#include "dialog.h"

#include "memorycheck.cpp"

pprogressbar actprogressbar = NULL;

void repaintdisplay ( void ) {};

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

   loadicons(); 
   loadmessages(); 

} 





void loaddata( void ) 
{
   loadcursor();

   loadguipictures();
   loadallobjecttypes();

   loadallvehicletypes();

   loadallbuildingtypes();

   loadalltechnologies();

   loadstreets();

   loadallterraintypes();

   loadUnitSets();
}


typedef vector<string> FilesToLoad;
FilesToLoad filesToLoad;


pfont load_font(char* name)
{
   tnfilestream stream ( name, 1 );
   return loadfont ( &stream );
}




int mapeditorMainThread ( void* _mapname )
{
   tvirtualdisplay buf ( 800, 600 );
   initMapDisplay( );

   cursor.init();

   try {
      loaddata();

      for ( FilesToLoad::iterator i = filesToLoad.begin(); i != filesToLoad.end(); i++ ) {
          string fn;
          if ( i->find ( "mis/" ) == i->npos )
             fn = "mis/"+*i;
          else
             fn = *i;

          tfindfile ff ( fn );
          string s = ff.getnextname();
          while ( !s.empty () ) {
             string errormsg;
             importbattleislemap ( "data/", s.c_str(), getterraintype_forpos(0)->weather[0], &errormsg, true );
             if ( !errormsg.empty() && 0 )
                fprintf(stderr, "map %s : %s \n", s.c_str(), errormsg.c_str() );
             string fn = "data/images/pcx/"+s;
             fn.replace ( fn.find (".dat"), 4, ".pcx");

             int width, height;

             string t = "data/mis/"+s;
             int maptime = get_filetime ( t.c_str());
             int pcxtime = get_filetime ( fn.c_str() );
             if ( maptime > pcxtime || pcxtime < 0 )
                writemaptopcx ( false, fn, &width, &height );
             else {
                tdisplaywholemap wm ( fn );
                wm.init ( actmap->xsize, actmap->ysize );
                width = wm.getWidth();
                height = wm.getHeight();
             }

             int playernum = 0;
             int ainum = 0;
             for ( int i = 0; i < 8; i++ )
               if ( actmap->player[i].exist() ) {
                  if ( actmap->player[i].stat == 0 )
                     playernum++;
                  if ( actmap->player[i].stat == 1 )
                     ainum++;
                }

             printf("%sµ%sµ%dµ%dµ%dµ%dµ%dµ%d\n", s.c_str(), actmap->title, actmap->xsize*2, actmap->ysize/2, width, height, playernum, ainum );

             s = ff.getnextname();

             delete actmap;
             actmap = NULL;
          }


      }

   } /* end try */
   catch ( tfileerror err ) {
      displaymessage ( " error loading file %s ",2,err.filename );
   } /* end catch */

   return 0;
}


int main(int argc, char *argv[] )
{ 
   for (int i = 1; i<argc; i++ ) {
      if ( argv[i][0] == '/'  ||  argv[i][0] == '-' ) {
     	
        if ( strcmpi ( &argv[i][1], "-verbose" ) == 0 ||
             strcmpi ( &argv[i][1], "v" ) == 0 ) {
           verbosity = atoi ( argv[++i] ); continue;
        }
      } else {
         filesToLoad.push_back ( argv[i] );
      }

   }
   if ( filesToLoad.empty() ) {
      fprintf(stderr, "no maps to load!\n");
      return 1;
   }

   addSearchPath ( "./") ;
   initFileIO( NULL );

   check_bi3_dir ();

   schriften.smallarial = load_font("smalaril.FNT");
   schriften.large = load_font("USABLACK.FNT");
   schriften.arial8 = load_font("ARIAL8.FNT");
   schriften.smallsystem = load_font("msystem.fnt");
   schriften.monogui = load_font("monogui.fnt");

   virtualscreenbuf.init();

   initializeEventHandling ( mapeditorMainThread, NULL, NULL );

   return 0;
}

