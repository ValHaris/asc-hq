
/*! \file map2pcx.cpp
  map2pcx : A program to generate pcx images of maps. This is a separate 
   program and not a module of the ASC main program or the mapeditor.
   This program is only used by Martin Bickel to create the images
   for the map archive.
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

#include <memory>

#include "typen.h"
#include "loaders.h"
#include "loadpcx.h"
#include "loadbi3.h"
#include "errors.h"
#include "gameoptions.h"
#include "mapdisplay.h"
#include "dialog.h"
#include "strtmesg.h"
#include "itemrepository.h"
#include "memorycheck.cpp"
#include "graphicset.h"


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
      tnfilestream stream ("hexfld_a.raw",tnstream::reading);
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
   loadcursor();

   loadalltextfiles();

   loadallobjecttypes();

   loadallvehicletypes();

   loadallbuildingtypes();

   loadalltechnologies();

   loadstreets();

   loadallterraintypes();

   readBI3translationTable();

   loadUnitSets();
}


typedef vector<string> FilesToLoad;
FilesToLoad filesToLoad;


pfont load_font(char* name)
{
   tnfilestream stream ( name, tnstream::reading );
   return loadfont ( &stream );
}

string outputDir;


void process_BI_map ( const ASCString& filename )
{
   string fn;
   if ( filename.find ( "mis/" ) == filename.npos )
      fn = "mis/"+filename;
   else
      fn = filename;

   tfindfile ff ( fn );
   string s = ff.getnextname();
   while ( !s.empty () ) {
      string errormsg;
      importbattleislemap ( outputDir.c_str(), s.c_str(), terrainTypeRepository.getObject_byPos(0)->weather[0], &errormsg, true );
      if ( !errormsg.empty() && 0 )
         fprintf(stderr, "map %s : %s \n", s.c_str(), errormsg.c_str() );
      string fn = outputDir+"images/pcx/"+s;
      fn.replace ( fn.find (".dat"), 4, ".pcx");

      int width, height;

      string t = outputDir+"mis/"+s;
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


      printf("%sµ%sµ%dµ%dµ%dµ%dµ%dµ%d\n", s.c_str(), actmap->maptitle.c_str(), actmap->xsize*2, actmap->ysize/2, width, height, playernum, ainum );

      s = ff.getnextname();

      delete actmap;
      actmap = NULL;
   }
}


void process_ASC_map ( const ASCString& filename )
{
   string fn;
   if ( filename.find ( "mis/" ) == filename.npos )
      fn = "mis/"+filename;
   else
      fn = filename;

   tfindfile ff ( fn );
   string s = ff.getnextname();
   while ( !s.empty () ) {
      try {
         tmaploaders gl;
         gl.loadmap ( s.c_str() );
      }
      catch ( ASCexception ) {
         fprintf(stderr, "skipping map %s \n", s.c_str() );
         s = ff.getnextname();
         continue;
      }

      activateGraphicSet ( actmap->graphicset );
      string fn = outputDir+"images/pcx/"+s;
      fn.replace ( fn.find (".map"), 4, ".pcx");

      int width, height;

      string t = outputDir+"mis/"+s;
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

      char datebuf[1000];
      tm* tmbuf = gmtime ( &actmap->archivalInformation.modifytime );
      strftime ( datebuf, 1000, "%Y-%m-%d", tmbuf );

      printf("%sµ%sµ%dµ%dµ%dµ%dµ%dµ%dµ%sµ%sµ%sµ%sµ%dµ%s\n", s.c_str(), actmap->maptitle.c_str(), actmap->xsize, actmap->ysize, width, height, playernum, ainum, actmap->archivalInformation.author.c_str(), actmap->archivalInformation.description.c_str(),actmap->archivalInformation.requirements.c_str(),actmap->archivalInformation.tags.c_str(),actmap->_resourcemode,datebuf);

      s = ff.getnextname();

      delete actmap;
      actmap = NULL;
   }
}



int mapeditorMainThread (  void* _mapname )
{
   tvirtualdisplay buf ( 800, 600 );
   initMapDisplay( );

   cursor.init();

   try {
      loaddata();

      for ( FilesToLoad::iterator i = filesToLoad.begin(); i != filesToLoad.end(); i++ )
          if ( patimat ( "*.dat", i->c_str() ))
             process_BI_map( *i );
          else
             process_ASC_map ( *i );

   } /* end try */
   catch ( tfileerror err ) {
      displaymessage ( " error loading file %s ",2,err.getFileName().c_str() );
   } /* end catch */

   return 0;
}


// including the command line parser, which is generated by genparse
#include "clparser/map2pcx.cpp"

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

   verbosity = cl->r();

   if ( cl->v() ) {
      cout << argv[0] << " " << getVersionString() << endl;
      exit(0);
   }

   outputDir = cl->d();

   for ( int i = cl->next_param(); i < argc; i++ )
       filesToLoad.push_back ( argv[i] );


   if ( filesToLoad.empty() ) {
      cerr << "no maps to load!\n";
      return 1;
   }

   addSearchPath ( "./") ;
   initFileIO( cl->c().c_str() );

   check_bi3_dir ();

   setDefaultBI3ImportTranslationTable ("bi-bi.asctxt");

   schriften.smallarial = load_font("smalaril.FNT");
   schriften.large = load_font("USABLACK.FNT");
   schriften.arial8 = load_font("ARIAL8.FNT");
   schriften.smallsystem = load_font("msystem.fnt");
   schriften.monogui = load_font("monogui.fnt");

   virtualscreenbuf.init();

   initializeEventHandling ( mapeditorMainThread, NULL );

   return 0;
}

