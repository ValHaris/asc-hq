/***************************************************************************
                           sg.cpp  -  description
                             -------------------
    begin                : a long time ago...
    copyright            : (C) 1994-2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file sg.cpp
    \brief THE main program: ASC
*/


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


/*!
   \mainpage 
   
   \section a short walk through the source
 
   THE central class of ASC is tmap in gamemap.h . 
   It is the anchor where nearly all elements of ASC are chained to. The global 
   variable #actmap is a pointer to the active map. There can be a maximum of
   8 players on a map, plus neutral units (which are handled like a 9th player). 
   Hence the array of 9 tmap::Player classes in tmap. 
   
   Each player has units and buildings, which are stored in the lists 
   tmap::Player::vehicleList and tmap::Player::buildingList . 
   The terms units and vehicles are used synonymously in ASC. Since unit was a 
   reserved word in Borland Pascal, we decided to use the term vehicle instead. 
   But now, with ASC written in C++,  'unit' is also used.
   
   Every building and unit is of a certain 'type': Vehicletype and BuildingType .
   These are stored in the data files which are loaded on startup and are globally 
   available. They are not modified during runtime in any way and are referenced 
   by the instances of Vehicle and Building. The Vehicletype has information that are shared
   by all vehicles of this 'type', like speed, weapon systems, accessable 
   terrain etc, while the vehicle stores things like remaining movement for this
   turn, ammo, fuel and cargo.
   
   The primary contents of a map are its fields ( tfield). Each field has again a pointer 
   to a certain weather of a TerrainType. Each TerrainType has up to 5 
   different weathers ("dry (standard)","light rain", "heavy rain", "few snow", 
   "lot of snow"). If there is a unit or a building standing on a field, the field
   has a pointer to it: tfield::vehicle and tfield::building . 
   
   On the field can be several instances of Object. Objects are another central class of 
   ASC. Roads, pipleines, trenches and woods are examples of objects.
 
*/

#include "global_os.h"

#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <new>
#include <malloc.h>
#include <ctype.h>
#include <signal.h>
#include <algorithm>
#include <memory>
#include <SDL_image.h>
#include <pgthemewidget.h>
#include <pgeventobject.h>
#include <pgapplication.h>

#include "SDL_mixer.h"

#include "paradialog.h"

#include "vehicletype.h"
#include "buildingtype.h"
#include "ai/ai.h"
#include "basegfx.h"
#include "misc.h"
#include "loadpcx.h"
#include "newfont.h"
#include "events.h"
#include "typen.h"
#include "spfst.h"
#include "loaders.h"
#include "dlg_box.h"
#include "stack.h"
#include "missions.h"
#include "controls.h"
#include "dlg_box.h"
#include "dialog.h"
#include "gui.h"
#include "pd.h"
#include "strtmesg.h"
#include "weather.h"
#include "gamedlg.h"
#include "network.h"
#include "building.h"
#include "sg.h"
#include "soundList.h"
#include "gameoptions.h"
#include "loadimage.h"
#include "astar2.h"
#include "errors.h"
#include "password.h"
#include "password_dialog.h"
#include "viewcalculation.h"
#include "replay.h"
#include "dashboard.h"
#include "graphicset.h"
#include "loadbi3.h"
#include "itemrepository.h"
#include "music.h"




// #define MEMCHK

#include "memorycheck.cpp"



class tsgonlinemousehelp : public tonlinemousehelp
{
   public:
      tsgonlinemousehelp ( void );
};

tsgonlinemousehelp :: tsgonlinemousehelp ( void )
{
   helplist.num = 12;

   static tonlinehelpitem sghelpitems[12]  = {{{ 498, 26, 576, 36}, 20001 },
         {{ 498, 41, 576, 51}, 20002 },
         {{ 586, 26, 612, 51}, 20003 },
         {{ 499, 57, 575, 69}, 20004 },
         {{ 499, 70, 575, 81}, 20005 },
         {{ 577, 58, 610, 68}, 20006 },
         {{ 577, 70, 610, 80}, 20007 },
         {{ 502, 92, 531,193}, 20008 },
         {{ 465, 92, 485,194}, 20009 },
         {{ 551, 92, 572,193}, 20010 },
         {{ 586, 90, 612,195}, 20011 },
         {{ 473,agmp->resolutiony - ( 480 - 449 ), 601,agmp->resolutiony - ( 480 - 460 )}, 20016 }};

   for ( int i = 0; i< helplist.num; i++ ) {
      sghelpitems[i].rect.x1 = agmp->resolutionx - ( 640 - sghelpitems[i].rect.x1 );
      sghelpitems[i].rect.x2 = agmp->resolutionx - ( 640 - sghelpitems[i].rect.x2 );
   }

   helplist.item = sghelpitems;
}

tsgonlinemousehelp* onlinehelp = NULL;




class tsgonlinemousehelpwind : public tonlinemousehelp
{
   public:
      tsgonlinemousehelpwind ( void );
} ;

tsgonlinemousehelpwind :: tsgonlinemousehelpwind ( void )
{
   helplist.num = 3;

   static tonlinehelpitem sghelpitemswind[3]  = { {{ 501,224, 569,290}, 20013 },
         {{ 589,228, 609,289}, 20014 },
         {{ 489,284, 509,294}, 20015 }};

   for ( int i = 0; i< helplist.num; i++ ) {
      sghelpitemswind[i].rect.x1 = agmp->resolutionx - ( 640 - sghelpitemswind[i].rect.x1 );
      sghelpitemswind[i].rect.x2 = agmp->resolutionx - ( 640 - sghelpitemswind[i].rect.x2 );
   }

   helplist.item = sghelpitemswind;
}

tsgonlinemousehelpwind* onlinehelpwind = NULL;



int  abortgame;

pprogressbar actprogressbar = NULL;
cmousecontrol* mousecontrol = NULL;

#define messagedisplaytime 300



#define maintainence2

int maintainencecheck( void )
{
#ifdef maintainence
   int num = 0;

   for ( int i = 0; i < 8; i++ )
      if ( actmap->player[i].stat == Player::human )
         num++;

   if ( actmap->campaign )
      num++;

   if ( actmap->network )
      num++;

   if ( num <= 1 )
      return 1;
   else
      return 0;

#else
   return 0;
#endif
}


void         loadMoreData(void)
{
   int          w;
   char         i; {
      tnfilestream stream ( "height2.raw", tnstream::reading );
      for (i=0;i<3 ;i++ )
         for ( int j=0; j<8; j++)
            stream.readrlepict( &icons.height2[i][j], false, &w );
   }

   {
      tnfilestream stream ("windp.raw", tnstream::reading);
      for (i=0;i<9 ;i++ )
         stream.readrlepict( &icons.wind[i], false, &w );

   }

   {
      tnfilestream stream ("windpfei.raw",tnstream::reading);
      stream.readrlepict( &icons.windarrow, false, &w );
   }

   {
      tnfilestream stream ("farbe.raw",tnstream::reading);
      for (i=0;i<8 ;i++ )
         stream.readrlepict( &icons.player[i], false, &w );
   }

   {
      tnfilestream stream ("allianc.raw",tnstream::reading);
      for (i=0;i<8 ;i++ ) {
         stream.readrlepict(   &icons.allianz[i][0], false, &w );
         stream.readrlepict(   &icons.allianz[i][1], false, &w );
         stream.readrlepict(   &icons.allianz[i][2], false, &w );
      } /* endfor */
   }

   {
      tnfilestream stream ("weapicon.raw",tnstream::reading);
      for (i=0; i<13 ;i++ )
         stream.readrlepict(   &icons.unitinfoguiweapons[i], false, &w );
   }

   {
      tnfilestream stream ("expicons.raw",tnstream::reading);
      for (i=0; i<=maxunitexperience ;i++ )
         stream.readrlepict(   &icons.experience[i], false, &w );
   }

   {
      tnfilestream stream ("hexinvi2.raw",tnstream::reading);
      stream.readrlepict(   &icons.view.va8, false, &w);
   }

   {
      tnfilestream stream ("hexinvis.raw",tnstream::reading);
      stream.readrlepict(   &icons.view.nv8, false, &w);
      void* u = uncompress_rlepict ( icons.view.nv8 );
      if ( u ) {
         asc_free( icons.view.nv8 );
         icons.view.nv8 = u;
      }
   }

   {
      tnfilestream stream ("fg8.raw",tnstream::reading);
      stream.readrlepict(   &icons.view.fog8, false, &w);
   }

   {
      tnfilestream stream ("windrose.raw",tnstream::reading);
      stream.readrlepict(   &icons.windbackground, false, &w);
   }

   if ( actprogressbar )
      actprogressbar->point();
   {
      tnfilestream stream ("hexfld_a.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.mark.active, false, &w);
   }

   if ( actprogressbar )
      actprogressbar->point(); {
      tnfilestream stream ("hexfld.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.mark.inactive, false, &w);
   }

   {
      tnfilestream stream ("in_ach.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.mark.movein_active, false, &w);
   }

   {
      tnfilestream stream ("in_h.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.mark.movein_inactive, false, &w);
   }

   {
      tnfilestream stream ("build_ah.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.mark.repairactive, false, &w);
   }

   {
      tnfilestream stream ("build_h.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.mark.repairinactive, false, &w);
   }

   {
      tnfilestream stream ("hexbuild.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.container_window, false, &w);
   }

   if ( actprogressbar )
      actprogressbar->point();

   if ( !asc_paletteloaded )
      loadpalette();

   for (w=0;w<256 ;w++ ) {
      palette16[w][0] = pal[w][0];
      palette16[w][1] = pal[w][1];
      palette16[w][2] = pal[w][2];
      xlattables.nochange[w] = w;
   } /* endfor */

   if ( actprogressbar )
      actprogressbar->point();

   loadicons();

   if ( actprogressbar )
      actprogressbar->point();

   loadmessages();

   if ( actprogressbar )
      actprogressbar->point(); {
      tnfilestream stream ("waffen.raw",tnstream::reading);
      int num;

      static int xlatselectweaponguiicons[12] = { 2, 7, 6, 3, 4, 9, 0, 5, 10, 11, 11, 11 };

      stream.readdata ( (char*) &num, sizeof( num ));
      for ( i = 0; i < num; i++ )
         stream.readrlepict(   &icons.selectweapongui[xlatselectweaponguiicons[i]], false, &w );
      stream.readrlepict(   &icons.selectweaponguicancel, false, &w );
   }

   {
      tnfilestream stream ("knorein.raw",tnstream::reading);
      stream.readrlepict(   &icons.guiknopf, false, &w );
   }

   {
      tnfilestream stream ("compi2.raw",tnstream::reading);
      stream.readrlepict(   &icons.computer, false, &w );
   }

   {
      tnfilestream stream ("pfeil-a0.raw",tnstream::reading);
      for (i=0; i<8 ;i++ ) {
         stream.readrlepict(   &icons.pfeil2[i], false, &w );
      } /* endfor */
   }

   {
      tnfilestream stream ("gebasym2.raw",tnstream::reading);
      for ( i = 0; i < 12; i++ )
         for ( int j = 0; j < 2; j++ )
            stream.readrlepict(   &icons.container.lasche.sym[i][j], false, &w );
   }

   {
      tnfilestream stream ("netcontr.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.netcontrol.start, false, &w );
      stream.readrlepict(   &icons.container.subwin.netcontrol.inactive, false, &w );
      stream.readrlepict(   &icons.container.subwin.netcontrol.active, false, &w );
   }

   {
      tnfilestream stream ("ammoprod.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.ammoproduction.start, false, &w );
      stream.readrlepict(   &icons.container.subwin.ammoproduction.button, false, &w );
      stream.readrlepict(   &icons.container.subwin.ammoproduction.buttonpressed, false, &w );
      for ( i = 0; i < 4; i++ )
         stream.readrlepict(   &icons.container.subwin.ammoproduction.schieber[i], false, &w );
      stream.readrlepict(   &icons.container.subwin.ammoproduction.schiene, false, &w );
   }

   if ( actprogressbar )
      actprogressbar->point(); {
      tnfilestream stream ("resorinf.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.resourceinfo.start, false, &w );
   }

   {
      tnfilestream stream ("windpowr.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.windpower.start, false, &w );
   }

   {
      tnfilestream stream ("solarpwr.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.solarpower.start, false, &w );

   }

   {
      tnfilestream stream ("ammotran.raw", tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.ammotransfer.start, false, &w );
      stream.readrlepict(   &icons.container.subwin.ammotransfer.button, false, &w );
      stream.readrlepict(   &icons.container.subwin.ammotransfer.buttonpressed, false, &w );
      for ( i = 0; i < 4; i++ )
         stream.readrlepict(   &icons.container.subwin.ammotransfer.schieber[i], false, &w );
      stream.readrlepict(   &icons.container.subwin.ammotransfer.schieneinactive, false, &w );
      stream.readrlepict(   &icons.container.subwin.ammotransfer.schiene, false, &w );
      if ( dataVersion >= 2 ) {
         stream.readrlepict(   &icons.container.subwin.ammotransfer.singlepage[0], false, &w );
         stream.readrlepict(   &icons.container.subwin.ammotransfer.singlepage[1], false, &w );
         stream.readrlepict(   &icons.container.subwin.ammotransfer.plus[0], false, &w );
         stream.readrlepict(   &icons.container.subwin.ammotransfer.plus[1], false, &w );
         stream.readrlepict(   &icons.container.subwin.ammotransfer.minus[0], false, &w );
         stream.readrlepict(   &icons.container.subwin.ammotransfer.minus[1], false, &w );
      }

   }

   {
      tnfilestream stream ("research.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.research.start, false, &w );
      stream.readrlepict(   &icons.container.subwin.research.button[0], false, &w );
      stream.readrlepict(   &icons.container.subwin.research.button[1], false, &w );
      stream.readrlepict(   &icons.container.subwin.research.schieber, false, &w );
   }

   {
      tnfilestream stream ("pwrplnt2.raw",tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.conventionelpowerplant.start, false, &w );
      stream.readrlepict(   &icons.container.subwin.conventionelpowerplant.schieber, false, &w );
      //stream.readrlepict(   &icons.container.subwin.conventionelpowerplant.button[1], false, &w );
   }





   int m; {
      tnfilestream stream ( "bldinfo.raw", tnstream::reading );
      stream.readrlepict( &icons.container.subwin.buildinginfo.start, false, &m );
      for ( i = 0; i < 8; i++ )
         stream.readrlepict( &icons.container.subwin.buildinginfo.height1[i], false, &m );
      for ( i = 0; i < 8; i++ )
         stream.readrlepict( &icons.container.subwin.buildinginfo.height2[i], false, &m );
      stream.readrlepict( &icons.container.subwin.buildinginfo.repair, false, &m );
      stream.readrlepict( &icons.container.subwin.buildinginfo.repairpressed, false, &m );
      stream.readrlepict( &icons.container.subwin.buildinginfo.block, false, &m );
   }


   {
      tnfilestream stream ("mining2.raw", tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.miningstation.start, false, &w );
      stream.readrlepict(   &icons.container.subwin.miningstation.zeiger, false, &w );
      /*
      for ( i = 0; i < 2; i++ )
         stream.readrlepict(   &icons.container.subwin.miningstation.button[i], false, &w );
      for ( i = 0; i < 2; i++ )
         stream.readrlepict(   &icons.container.subwin.miningstation.resource[i], false, &w );
      for ( i = 0; i < 3; i++ )
         stream.readrlepict(   &icons.container.subwin.miningstation.axis[i], false, &w );
      for ( i = 0; i < 2; i++ )
         stream.readrlepict(   &icons.container.subwin.miningstation.pageturn[i], false, &w );
      stream.readrlepict(   &icons.container.subwin.miningstation.graph, false, &w );
      */
   }

   {
      tnfilestream stream ("mineral.raw", tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.mineralresources.start, false, &w );
      stream.readrlepict(   &icons.container.subwin.mineralresources.zeiger, false, &w );
   }

   {
      tnfilestream stream ("tabmark.raw", tnstream::reading);
      stream.readrlepict (   &icons.container.tabmark[0], false, &w );
      stream.readrlepict (   &icons.container.tabmark[1], false, &w );
   }


   {
      tnfilestream stream ("traninfo.raw", tnstream::reading);
      stream.readrlepict(   &icons.container.subwin.transportinfo.start, false, &w );
      for ( i = 0; i < 8; i++ )
         stream.readrlepict(   &icons.container.subwin.transportinfo.height1[i], false, &w );
      for ( i = 0; i < 8; i++ )
         stream.readrlepict(   &icons.container.subwin.transportinfo.height2[i], false, &w );
      stream.readrlepict(   &icons.container.subwin.transportinfo.sum, false, &w );
   }

   if ( actprogressbar )
      actprogressbar->point(); {
      tnfilestream stream ("attack.raw", tnstream::reading);
      stream.readrlepict (   &icons.attack.bkgr, false, &w );
      icons.attack.orgbkgr = NULL;
   }

   {
      tnfilestream stream ("hexfeld.raw", tnstream::reading);
      stream.readrlepict ( &icons.fieldshape, false, &w );
   }

#ifdef FREEMAPZOOM
   {
      tnfilestream stream ("mapbkgr.raw", tnstream::reading);
      stream.readrlepict ( &icons.mapbackground, false, &w );
   }
#endif

   {
      tnfilestream stream ("hex2oct.raw", tnstream::reading);
      stream.readrlepict (   &icons.hex2octmask, false, &w );
   }

   {
      tnfilestream stream ("weapinfo.raw", tnstream::reading);
      for ( int i = 0; i < 5; i++ )
         stream.readrlepict (   &icons.weaponinfo[i], false, &w );
   }

   backgroundpict.load();

}

enum tuseractions { ua_repainthard,     ua_repaint, ua_help, ua_showpalette, ua_dispvehicleimprovement, ua_mainmenu, ua_mntnc_morefog,
                    ua_mntnc_lessfog,   ua_mntnc_morewind,   ua_mntnc_lesswind, ua_mntnc_rotatewind, ua_changeresourceview,
                    ua_benchgamewv,     ua_benchgamewov,     ua_viewterraininfo, ua_unitweightinfo,  ua_writemaptopcx,  ua_writescreentopcx,
                    ua_startnewsinglelevel, ua_changepassword, ua_gamepreferences, ua_bi3preferences,
                    ua_exitgame,        ua_newcampaign,      ua_loadgame,  ua_savegame, ua_setupalliances, ua_settribute, ua_giveunitaway,
                    ua_vehicleinfo,     ua_researchinfo,     ua_unitstatistics, ua_buildingstatistics, ua_newmessage, ua_viewqueuedmessages,
                    ua_viewsentmessages, ua_viewreceivedmessages, ua_viewjournal, ua_editjournal, ua_viewaboutmessage, ua_continuenetworkgame,
                    ua_toggleunitshading, ua_computerturn, ua_setupnetwork, ua_howtostartpbem, ua_howtocontinuepbem, ua_mousepreferences,
                    ua_selectgraphicset, ua_UnitSetInfo, ua_GameParameterInfo, ua_GameStatus, ua_viewunitweaponrange, ua_viewunitmovementrange,
                    ua_aibench, ua_networksupervisor, ua_selectPlayList, ua_soundDialog, ua_reloadDlgTheme };


class tsgpulldown : public tpulldown
{
   public:
      void init ( void );
}
pd;

void         tsgpulldown :: init ( void )
{
   addfield ( "Glo~b~al" );
   addbutton ( "toggle ~R~esourceviewõ1", ua_changeresourceview );
   addbutton ( "toggle unit shadingõ2", ua_toggleunitshading );
   addbutton ( "seperator", -1);
   addbutton ( "~O~ptions", ua_gamepreferences );
   addbutton ( "~M~ouse options", ua_mousepreferences );
   addbutton ( "~S~ound options", ua_soundDialog );
   addbutton ( "seperator", -1);
   addbutton ( "E~x~itõctrl-x", ua_exitgame );


   addfield ("~G~ame");
   addbutton ( "New ~C~ampaign", ua_newcampaign);
   addbutton ( "~N~ew single Levelõctrl-n", ua_startnewsinglelevel );

   addbutton ( "seperator", -1);
   addbutton ( "~L~oad gameõctrl-l", ua_loadgame );
   addbutton ( "~S~ave game", ua_savegame );
   addbutton ( "seperator", -1 );
   addbutton ( "Continue network gameõF3", ua_continuenetworkgame);
   addbutton ( "setup Net~w~ork", ua_setupnetwork );
   addbutton ( "Change Passw~o~rd", ua_changepassword );
   addbutton ( "supervise network game", ua_networksupervisor );
   addbutton ( "seperator", -1 );
   addbutton ( "~P~layers + Alliances", ua_setupalliances);
   addbutton ( "transfer ~U~nit control", ua_giveunitaway );
   addbutton ( "~T~ransfer resources", ua_settribute);

   addfield ( "~I~nfo" );
   addbutton ( "~V~ehicle types", ua_vehicleinfo );
   addbutton ( "Unit ~w~eapon rangeõ3", ua_viewunitweaponrange );
   addbutton ( "Unit ~m~ovement rangeõ4", ua_viewunitmovementrange );
   addbutton ( "~G~ame Timeõ5", ua_GameStatus );
   addbutton ( "unit ~S~et informationõ6", ua_UnitSetInfo );
   addbutton ( "~T~errainõ7", ua_viewterraininfo );
   addbutton ( "~U~nit weightõ8", ua_unitweightinfo );
   // addbutton ( "seperator", -1 );
   // addbutton ( "~R~esearch", ua_researchinfo );

   // addbutton ( "vehicle ~I~mprovementõF7", ua_dispvehicleimprovement);
   // addbutton ( "show game ~P~arameters", ua_GameParameterInfo );


   addfield ( "~S~tatistics" );
   addbutton ( "~U~nits", ua_unitstatistics );
   addbutton ( "~B~uildings", ua_buildingstatistics );
   // addbutton ( "seperator");
   // addbutton ( "~H~istory");

   addfield ( "~M~essage");
   addbutton ( "~n~ew message", ua_newmessage );
   addbutton ( "view ~q~ueued messages", ua_viewqueuedmessages );
   addbutton ( "view ~s~end messages", ua_viewsentmessages );
   addbutton ( "view ~r~eceived messages", ua_viewreceivedmessages);
   addbutton ( "seperator", -1 );
   addbutton ( "view ~j~ournal", ua_viewjournal );
   addbutton ( "~a~ppend to journal", ua_editjournal );

   addfield ( "~T~ools" );
   addbutton ( "save ~M~ap as PCXõ9", ua_writemaptopcx );
   addbutton ( "save ~S~creen as PCXõ0", ua_writescreentopcx );
   addbutton ( "benchmark without view calc", ua_benchgamewov );
   addbutton ( "benchmark with view calc", ua_benchgamewv);
   addbutton ( "compiler benchmark (AI)", ua_aibench );
   // addbutton ( "test memory integrity", ua_heapcheck );
   addbutton ( "seperator", -1 );
   addbutton ( "select graphic set", ua_selectgraphicset );
   addbutton ( "reload dialog theme", ua_reloadDlgTheme );

   addfield ( "~H~elp" );
   addbutton ( "HowTo ~S~tart email games", ua_howtostartpbem );
   addbutton ( "HowTo ~C~ontinue email games", ua_howtocontinuepbem );
   addbutton ( "seperator", -1);
   addbutton ( "~K~eys", ua_help );

   addbutton ( "~A~bout", ua_viewaboutmessage );

   tpulldown :: init();
   setshortkeys();
}


class MainMenuPullDown : public tpulldown
{
   public:
      void init ( void );
} ;

void         MainMenuPullDown :: init ( void )
{
   alwaysOpen = true;

   addfield ( "Glo~b~al" );
   addbutton ( "~O~ptions", ua_gamepreferences );
   addbutton ( "~M~ouse options", ua_mousepreferences );
   addbutton ( "Select Music Play ~L~ist ", ua_selectPlayList );
   addbutton ( "~S~ound options", ua_soundDialog );
   addbutton ( "seperator", -1);
   addbutton ( "E~x~itõctrl-x", ua_exitgame );


   addfield ("~G~ame");
   addbutton ( "New ~C~ampaign", ua_newcampaign);
   addbutton ( "~N~ew single Levelõctrl-n", ua_startnewsinglelevel );

   addbutton ( "~L~oad gameõctrl-l", ua_loadgame );
   addbutton ( "Continue network gameõF3", ua_continuenetworkgame);
   addbutton ( "supervise network game", ua_networksupervisor );

   addfield ( "~H~elp" );
   addbutton ( "HowTo ~S~tart email games", ua_howtostartpbem );
   addbutton ( "HowTo ~C~ontinue email games", ua_howtocontinuepbem );
   addbutton ( "seperator", -1);
   addbutton ( "~K~eys", ua_help );

   addbutton ( "~A~bout", ua_viewaboutmessage );

   tpulldown :: init();
   setshortkeys();
}


void         repaintdisplay(void)
{
   collategraphicoperations cgo;
   int mapexist = actmap && (actmap->xsize > 0) && (actmap->ysize > 0);


   int ms = getmousestatus();
   if ( ms == 2 )
      mousevisible ( false );

   int cv = cursor.an;

   if ( mapexist && cv )
      cursor.hide();
   backgroundpict.paint();
   setvgapalette256(pal);

   if ( mapexist ) {
      displaymap();

      if ( cv )
         cursor.show();
   }

   pd.barstatus = false;

   if ( ms == 2 )
      mousevisible ( true );
   dashboard.x = 0xffff;
   dashboard.repainthard = 1;
   if ( actmap && actmap->ellipse )
      actmap->ellipse->paint();

   if ( actgui && actmap && actmap->xsize>0)
      actgui->painticons();

}


void loadMap()
{
   ASCString s1;

   mousevisible(false);
   fileselectsvga(mapextension, s1, true );

   if ( !s1.empty() ) {
      mousevisible(false);
      cursor.hide();
      displaymessage("loading map %s",0, s1.c_str() );
      loadmap(s1.c_str());
      actmap->startGame();

      next_turn();

      removemessage();
      if (actmap->campaign != NULL) {
         delete  ( actmap->campaign );
         actmap->campaign = NULL;
      }

      displaymap();
      dashboard.x = 0xffff;
      moveparams.movestatus = 0;
   }
   mousevisible(true);
}


void loadGame()
{
   mousevisible(false);

   ASCString s1;
   fileselectsvga(savegameextension, s1, true );

   if ( !s1.empty() ) {
      mousevisible(false);
      cursor.hide();
      displaymessage("loading %s ",0, s1.c_str());
      loadgame(s1.c_str() );
      removemessage();
      if ( !actmap || actmap->xsize == 0 || actmap->ysize == 0 )
         throw  NoMapLoaded();

      if ( actmap->network )
         setallnetworkpointers ( actmap->network );

      computeview( actmap );
      displaymap();
      dashboard.x = 0xffff;
      moveparams.movestatus = 0;
   }
   mousevisible(true);
}


void saveGame( bool as )
{
   ASCString s1;

   int nameavail = 0;
   if ( !actmap->preferredFileNames.savegame[actmap->actplayer].empty() )
      nameavail = 1;

   if ( as || !nameavail ) {
      mousevisible(false);
      fileselectsvga(savegameextension, s1, false );
   } else
      s1 = actmap->preferredFileNames.savegame[actmap->actplayer];

   if ( !s1.empty() ) {
      actmap->preferredFileNames.savegame[actmap->actplayer] = s1;

      mousevisible(false);
      cursor.hide();
      displaymessage("saving %s", 0, s1.c_str());
      savegame(s1.c_str());

      removemessage();
      displaymap();
      cursor.show();
   }
   mousevisible(true);
}


void         showpalette(void)
{
   bar ( 0, 0, 639, 479, black );
   int x=7;

   for ( char a = 0; a <= 15; a++)
      for ( char b = 0; b <= 15; b++) {
         bar(     a * 40, b * 20,a * 40 +  x,b * 20 + 20, xlattables.a.light    [a * 16 + b]);
         bar( x + a * 40, b * 20,a * 40 + 2*x,b * 20 + 20, xlattables.nochange [a * 16 + b]);
         bar(2*x + a * 40, b * 20,a * 40 + 3*x,b * 20 + 20, xlattables.a.dark1    [a * 16 + b]);
         bar(3*x + a * 40, b * 20,a * 40 + 4*x,b * 20 + 20, xlattables.a.dark2    [a * 16 + b]);
         bar(4*x + a * 40, b * 20,a * 40 + 5*x,b * 20 + 20, xlattables.a.dark3    [a * 16 + b]);
      }
   wait();
   repaintdisplay();
}





void         newcampaign(void)
{
   tchoosenewcampaign tnc;
   tnc.init();
   tnc.run();
   tnc.done();
}



void         newsinglelevel(void)
{
   tchoosenewsinglelevel tnc;

   tnc.init();
   tnc.run();
   tnc.done();
}

void         startnewsinglelevelfromgame(void)
{
   cursor.hide();
   newsinglelevel();
   if ( !actmap )
      throw NoMapLoaded();
   computeview( actmap );
   displaymap();
   cursor.show();
}



void loadStartupMap ( const char *gameToLoad=NULL )
{
   if ( gameToLoad && gameToLoad[0] ) {
      if ( patimat ( tournamentextension, gameToLoad )) {

         if( validateemlfile( gameToLoad ) == 0 ) {
            fprintf( stderr, "Email gamefile %s is invalid. Aborting.\n", gameToLoad );
            exit(-1);
         }

         try {
            tnfilestream gamefile ( gameToLoad, tnstream::reading );
            tnetworkloaders nwl;
            nwl.loadnwgame( &gamefile );
            if ( actmap->network )
               setallnetworkpointers ( actmap->network );
         } catch ( tfileerror ) {
            fprintf ( stderr, "%s is not a legal email game. \n", gameToLoad );
            exit(-1);
         }
      } else if( patimat ( savegameextension, gameToLoad )) {
         if( validatesavfile( gameToLoad ) == 0 ) {
            fprintf( stderr, "The savegame %s is invalid. Aborting.\n", gameToLoad );
            exit( -1 );
         }
         try {
            loadgame( gameToLoad );
         } catch ( tfileerror ) {
            fprintf ( stderr, "%s is not a legal savegame. \n", gameToLoad );
            exit(-1);
         }

      } else if( patimat ( mapextension, gameToLoad )) {
         if( validatemapfile( gameToLoad ) == 0 ) {
            fprintf( stderr, "Mapfile %s is invalid. Aborting.\n", gameToLoad );
            exit(-1);
         }

         try {
            loadmap( gameToLoad );
            if ( actmap->network )
               setallnetworkpointers ( actmap->network );
         } catch ( tfileerror ) {
            fprintf ( stderr, "%s is not a legal map. \n", gameToLoad );
            exit(-1);
         }
      } else {
         fprintf ( stderr, "Don't know how to handle the file %s \n", gameToLoad );
         exit(-1);
      }

   } else {  // resort to loading defaults

      ASCString s = "first.map";
      /*
      if ( CGameOptions::Instance()->startupcount < 4 ) {
         s = "tutor0" ;
   } else {
         s = "railstat";
   }
      s += &mapextension[1];
      */

      int maploadable; {
         tfindfile ff ( s );
         string filename = ff.getnextname();
         maploadable = validatemapfile ( filename.c_str() );
      }

      if ( !maploadable ) {

         tfindfile ff ( mapextension );
         string filename = ff.getnextname();

         if ( filename.empty() )
            displaymessage( "unable to load startup-map",2);

         while ( !validatemapfile ( filename.c_str() ) ) {
            filename = ff.getnextname();
            if ( filename.empty() )
               displaymessage( "unable to load startup-map",2);

         }
         s = filename;
      }

      loadmap(s.c_str());

      displayLogMessage ( 6, "initializing map..." );
      actmap->startGame();
      displayLogMessage ( 6, "done\n Setting up Resources..." );
      actmap->setupResources();
      displayLogMessage ( 6, "done\n" );
   }
}


void benchgame ( int mode )
{
   int t2;
   int t = ticker;
   int n = 0;
   do {
      if ( mode <= 1 ) {
         if ( mode == 1 )
            computeview( actmap );
         displaymap();
      } else
         copy2screen();

      n++;
      t2 = ticker;
   } while ( t + 1000 > t2 ); /* enddo */
   double d = 100 * n;
   d /= (t2-t);
   char buf[100];
   sprintf ( buf, "%3.1f", d );
   displaymessage2 ( " %s fps ", buf );
}

class WeaponRange : public SearchFields
{
   public:
      int run ( const pvehicle veh );
      void testfield ( const MapCoordinate& mc )
      {
         gamemap->getField( mc )->tempw = 1;
      };
      WeaponRange ( pmap _gamemap ) : SearchFields ( _gamemap )
      {}
      ;
};

int  WeaponRange :: run ( const pvehicle veh )
{
   int found = 0;
   if ( fieldvisiblenow ( getfield ( veh->xpos, veh->ypos )))
      for ( int i = 0; i < veh->typ->weapons.count; i++ ) {
         if ( veh->typ->weapons.weapon[i].shootable() ) {
            initsearch ( veh->getPosition(), veh->typ->weapons.weapon[i].maxdistance/minmalq, (veh->typ->weapons.weapon[i].mindistance+maxmalq-1)/maxmalq );
            startsearch();
            found++;
         }
      }
   return found;
}

void viewunitweaponrange ( const pvehicle veh, tkey taste )
{
   if ( veh && !moveparams.movestatus  ) {
      actmap->cleartemps ( 7 );
      WeaponRange wr ( actmap );
      int res = wr.run ( veh );
      if ( res ) {
         displaymap();

         if ( taste != ct_invvalue ) {
            while ( skeypress ( taste )) {

               while ( keypress() )
                  r_key();

               releasetimeslice();
            }
         } else {
            int mb = mouseparams.taste;
            while ( mouseparams.taste == mb && !keypress() )
               releasetimeslice();
            while ( keypress() )
               r_key();
         }

         actmap->cleartemps ( 7 );
         displaymap();
      }
   }
}

void viewunitmovementrange ( pvehicle veh, tkey taste )
{
   if ( veh && !moveparams.movestatus && fieldvisiblenow ( getfield ( veh->xpos, veh->ypos ))) {
      actmap->cleartemps ( 7 );
      TemporaryContainerStorage tcs ( veh, true );
      veh->reactionfire.disable();
      veh->setMovement ( veh->typ->movement[log2(veh->height)]);
      VehicleMovement vm ( NULL, NULL );
      if ( vm.available ( veh )) {
         vm.execute ( veh, -1, -1, 0, -1, -1 );
         if ( vm.reachableFields.getFieldNum()) {
            for  ( int i = 0; i < vm.reachableFields.getFieldNum(); i++ )
               if ( fieldvisiblenow ( vm.reachableFields.getField ( i ) ))
                  vm.reachableFields.getField ( i )->a.temp = 1;
            for  ( int j = 0; j < vm.reachableFieldsIndirect.getFieldNum(); j++ )
               if ( fieldvisiblenow ( vm.reachableFieldsIndirect.getField ( j )))
                  vm.reachableFieldsIndirect.getField ( j )->a.temp = 1;

            displaymap();

            if ( taste != ct_invvalue ) {
               while ( skeypress ( taste )) {
                  while ( keypress() )
                     r_key();

                  releasetimeslice();
               }
            } else {
               int mb = mouseparams.taste;
               while ( mouseparams.taste == mb && !keypress() )
                  releasetimeslice();

               while ( keypress() )
                  r_key();
            }
            actmap->cleartemps ( 7 );
            displaymap();
         }
      }
      tcs.restore();
   }
}



void execuseraction ( tuseractions action )
{
   switch ( action ) {
      case ua_repainthard  :
      case ua_repaint      :
         repaintdisplay();
         break;

      case ua_help         :
         help(20);
         break;

      case ua_howtostartpbem :
         help(21);
         break;

      case ua_howtocontinuepbem :
         help(22);
         break;

      case ua_showpalette  :
         showpalette();
         break;

      case ua_dispvehicleimprovement    :
         displaymessage("Research:\n%s %d \n%s %d \n%s %d \n"
                        "%s %d \n%s %d \n%s %d \n"
                        "%s %d \n%s %d \n%s %d \n"
                        "%s %d",1,
                        cwaffentypen[0], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[0]),
                        cwaffentypen[1], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[1]),
                        cwaffentypen[2], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[2]),
                        cwaffentypen[3], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[3]),
                        cwaffentypen[4], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[4]),
                        cwaffentypen[5], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[5]),
                        cwaffentypen[6], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[6]) ,
                        cwaffentypen[7], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[7])  ,
                        cwaffentypen[10], (actmap->player[actmap->actplayer].research.unitimprovement.weapons[10])     ,
                        "armor",         (actmap->player[actmap->actplayer].research.unitimprovement.armor));
         break;

      case ua_mainmenu:
         if (choice_dlg("do you really want to close the current game ?","~y~es","~n~o") == 1) {
            delete actmap;
            actmap = NULL;
            throw NoMapLoaded();
         }
         break;

      case ua_mntnc_morefog:
         if (actmap->weather.fog < 255   && maintainencecheck() ) {
            actmap->weather.fog++;
            computeview( actmap );
            displaymessage2("fog intensity set to %d ", actmap->weather.fog);
            displaymap();
         }
         break;

      case ua_mntnc_lessfog:
         if (actmap->weather.fog  && maintainencecheck()) {
            actmap->weather.fog--;
            computeview( actmap );
            displaymessage2("fog intensity set to %d ", actmap->weather.fog);
            displaymap();
         }
         break;

      case ua_mntnc_morewind:
         if ((actmap->weather.wind[0].speed < 254) &&  maintainencecheck()) {
            actmap->weather.wind[0].speed+=2;
            actmap->weather.wind[2].speed = actmap->weather.wind[1].speed = actmap->weather.wind[0].speed;
            displaywindspeed (  );
            dashboard.x = 0xffff;
         }
         break;

      case ua_mntnc_lesswind:
         if ((actmap->weather.wind[0].speed > 1)  && maintainencecheck() ) {
            actmap->weather.wind[0].speed-=2;
            actmap->weather.wind[2].speed = actmap->weather.wind[1].speed = actmap->weather.wind[0].speed;
            displaywindspeed (  );
            dashboard.x = 0xffff;
         }
         break;

      case ua_mntnc_rotatewind:
         if ( maintainencecheck() ) {
            if (actmap->weather.wind[0].direction < sidenum-1 )
               actmap->weather.wind[0].direction++;
            else
               actmap->weather.wind[0].direction = 0;
            actmap->weather.wind[2].direction = actmap->weather.wind[1].direction = actmap->weather.wind[0].direction;
            displaymessage2("wind dir set to %d ", actmap->weather.wind[0].direction);
            dashboard.x = 0xffff;
            resetallbuildingpicturepointers();
            displaymap();
         }
         break;

      case ua_changeresourceview:
         showresources++;
         if ( showresources >= 3 )
            showresources = 0;
         displaymap();
         break;

      case ua_benchgamewov:
         benchgame( 2 );
         break;

      case ua_benchgamewv :
         benchgame( 1 );
         break;

      case ua_viewterraininfo:
         viewterraininfo();
         break;

      case ua_unitweightinfo:
         if ( fieldvisiblenow  ( getactfield() )) {
            pvehicle eht = getactfield()->vehicle;
            if ( eht && getdiplomaticstatus ( eht->color ) == capeace )
               displaymessage(" weight of unit: \n basic: %d\n+fuel: %d\n+material:%d\n+cargo:%d\n= %d",1 ,eht->typ->weight, eht->tank.fuel * resourceWeight[Resources::Fuel] / 1000 , eht->tank.material * resourceWeight[Resources::Material] / 1000, eht->cargo(), eht->weight() );
         }
         break;

      case ua_writemaptopcx :
         writemaptopcx ();
         break;

      case ua_writescreentopcx:
         {
            char* nm = getnextfilenumname ( "screen", "pcx", 0 );
            writepcx ( nm, 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, pal );
            displaymessage2( "screen saved to %s", nm );
         }
         break;

      case ua_startnewsinglelevel:
         startnewsinglelevelfromgame();
         break;

      case ua_changepassword:
         {
            bool success;
            do {
               Password oldpwd = actmap->player[actmap->actplayer].passwordcrc;
               actmap->player[actmap->actplayer].passwordcrc.reset();
               success = enterpassword ( actmap->player[actmap->actplayer].passwordcrc, true, true );
               if ( !success )
                  actmap->player[actmap->actplayer].passwordcrc = oldpwd;
            } while ( actmap->player[actmap->actplayer].passwordcrc.empty() && success && viewtextquery ( 910, "warning", "~e~nter password", "~c~ontinue without password" ) == 0 ); /* enddo */
         }
         break;

      case ua_gamepreferences:
         gamepreferences();
         break;

      case ua_mousepreferences:
         mousepreferences();
         break;

      case ua_bi3preferences:
         bi3preferences();
         break;

      case ua_exitgame:
         if (choice_dlg("do you really want to quit ?","~y~es","~n~o") == 1)
            abortgame = 1;
         else
            exitprogram = 0;
         break;

      case ua_newcampaign:
         cursor.hide();
         newcampaign();
         computeview( actmap );
         displaymap();
         cursor.show();
         break;

      case ua_loadgame:
         loadGame();
         break;

      case ua_savegame:
         saveGame( true );
         break;

      case ua_setupalliances:
         setupalliances();
         logtoreplayinfo ( rpl_alliancechange );
         logtoreplayinfo ( rpl_shareviewchange );

         if ( actmap->shareview && actmap->shareview->recalculateview ) {
            logtoreplayinfo ( rpl_shareviewchange );
            computeview( actmap );
            actmap->shareview->recalculateview = 0;
            displaymap();
         }
         dashboard.x = 0xffff;
         break;

      case ua_settribute :
         settributepayments ();
         break;

      case ua_giveunitaway:
         giveunitaway ();
         break;

      case ua_vehicleinfo:
         activefontsettings.font = schriften.smallarial;
         vehicle_information();
         break;

      case ua_researchinfo:
         researchinfo ();
         break;

      case ua_unitstatistics:
         statisticarmies();
         break;

      case ua_buildingstatistics:
         statisticbuildings();
         break;

      case ua_newmessage:
         newmessage();
         break;

      case ua_viewqueuedmessages:
         viewmessages( "queued messages", actmap->unsentmessage, 1, 0 );
         break;

      case ua_viewsentmessages:
         viewmessages( "sent messages", actmap->player[ actmap->actplayer ].sentmessage, 0, 0 );
         break;

      case ua_viewreceivedmessages:
         viewmessages( "received messages", actmap->player[ actmap->actplayer ].oldmessage, 0, 1 );
         break;

      case ua_viewjournal:
         viewjournal();
         break;

      case ua_editjournal:
         editjournal();
         break;

      case ua_viewaboutmessage:
         {
            help(30);
            tviewanytext vat;
            ASCString s = getstartupmessage();

#ifdef _SDL_
            char buf[1000];
            SDL_version compiled;
            SDL_VERSION(&compiled);
            sprintf(buf, "\nCompiled with SDL version: %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
            s += buf;

            sprintf(buf, "Linked with SDL version: %d.%d.%d\n", SDL_Linked_Version()->major, SDL_Linked_Version()->minor, SDL_Linked_Version()->patch);
            s += buf;
#endif

            vat.init ( "about", s.c_str() );
            vat.run();
            vat.done();
         }
         break;

      case ua_continuenetworkgame:
         continuenetworkgame();
         displaymap();
         break;

      case ua_toggleunitshading:
         CGameOptions::Instance()->units_gray_after_move = !CGameOptions::Instance()->units_gray_after_move;
         CGameOptions::Instance()->setChanged();
         displaymap();
         while ( mouseparams.taste )
            releasetimeslice();

         if ( CGameOptions::Instance()->units_gray_after_move )
            displaymessage("units that can not move will now be displayed gray", 3);
         else
            displaymessage("units that can not move and cannot shoot will now be displayed gray", 3);
         break;

      case ua_computerturn:
         if ( maintainencecheck() || 1 ) {
            displaymessage("This function is under development and for programmers only\n"
                           "unpredictable things may happen ...",3 ) ;

            if (choice_dlg("do you really want to start the AI?","~y~es","~n~o") == 1) {

               if ( !actmap->player[ actmap->actplayer ].ai )
                  actmap->player[ actmap->actplayer ].ai = new AI ( actmap, actmap->actplayer );

               savegame ( "aistart.sav" );
               actmap->player[ actmap->actplayer ].ai->run();
            }
         }
         break;
      case ua_setupnetwork:
         if ( actmap->network )
            setupnetwork ( actmap->network );
         else
            displaymessage("This map is not played across a network",3 );
         break;
      case ua_selectgraphicset:
         selectgraphicset();
         break;
      case ua_UnitSetInfo:
         viewUnitSetinfo();
         break;
      case ua_GameParameterInfo:
         showGameParameters();
         break;
      case ua_GameStatus:
         displaymessage ( "Current game time is:\n turn %d , move %d ", 3, actmap->time.turn(), actmap->time.move() );
         break;
      case ua_viewunitweaponrange:
         viewunitweaponrange ( getactfield()->vehicle, ct_invvalue );
         break;

      case ua_viewunitmovementrange:
         viewunitmovementrange ( getactfield()->vehicle, ct_invvalue );
         break;

      case ua_aibench:
         if ( maintainencecheck() || 1 ) {
            if ( !actmap->player[ actmap->actplayer ].ai )
               actmap->player[ actmap->actplayer ].ai = new AI ( actmap, actmap->actplayer );

            if ( AI* ai = dynamic_cast<AI*>( actmap->player[ actmap->actplayer ].ai )) {
               savegame ( "ai-bench-start.sav" );
               ai->run( true );
            }
         }
         break;
      case ua_networksupervisor:
         networksupervisor();
         displaymap();
         break;

      case ua_selectPlayList:
         selectPlayList();
         break;

      case ua_soundDialog:
         soundSettings();
         break;
      case ua_reloadDlgTheme:
         if ( pgApp ) {
             pgApp->reloadTheme();
             soundSettings();
         }
         break;

   }


}

void checkpulldown( tkey ch )
{
   pd.key = ch;
   pd.checkpulldown();

   if (pd.action2execute >= 0 ) {
      tuseractions ua = (tuseractions) pd.action2execute;
      pd.action2execute = -1;
      execuseraction ( ua );
   }
}

void mainloopgeneralkeycheck ( tkey& ch )
{
   ch = r_key();
   checkpulldown( ch );

   movecursor(ch);
   actgui->checkforkey ( ch );
}




void mainloopgeneralmousecheck ( void )
{
   if ( exitprogram )
      execuseraction ( ua_exitgame );

   actgui->checkformouse();

   dashboard.checkformouse();

   if ((dashboard.x != getxpos()) || (dashboard.y != getypos())) {
      dashboard.paint ( getactfield(), actmap->playerView );
      actgui->painticons();
   }

   if ( lastdisplayedmessageticker + messagedisplaytime < ticker )
      displaymessage2("");

   if ( mousecontrol )
      mousecontrol->chkmouse(); {
      int oldx = actmap->xpos;
      int oldy = actmap->ypos;
      checkformousescrolling();
      if ( oldx != actmap->xpos || oldy != actmap->ypos )
         dashboard.x = 0xffff;
   }

   if ( onlinehelp )
      onlinehelp->checkforhelp();

   if ( onlinehelpwind && !CGameOptions::Instance()->smallmapactive )
      onlinehelpwind->checkforhelp();
}




void  mainloop ( void )
{
   tkey ch;
   abortgame = 0;

   do {
      viewunreadmessages();
      activefontsettings.background=0;
      activefontsettings.length=50;
      activefontsettings.color=14;
      if (keypress()) {

         mainloopgeneralkeycheck ( ch );

         switch (ch) {

#ifndef NEWKEYB
            case 'R':
               execuseraction ( ua_repainthard );
               break;
#else
            case ct_shp + ct_r:
               execuseraction ( ua_repainthard );
               break;
#endif

            case ct_stp + ct_l:
               execuseraction ( ua_loadgame );
               break;

            case ct_stp + ct_n:
               execuseraction ( ua_startnewsinglelevel );
               break;

            case ct_r:
               execuseraction ( ua_repaint );
               break;

            case ct_f1:
               execuseraction ( ua_help );
               break;

            case ct_f2:
               execuseraction ( ua_mainmenu );
               break;

            case ct_f3:
               execuseraction ( ua_continuenetworkgame );
               break;

            case ct_f4:
               execuseraction ( ua_computerturn );
               break;

            case ct_f8:
               {
                  int color;
                  if ( getactfield()->vehicle )
                     color = getactfield()->vehicle->color / 8 ;
                  else
                     color = actmap->actplayer;

                  if ( actmap->player[color].ai ) {
                     AI* ai = (AI*) actmap->player[color].ai;
                     ai->showFieldInformation ( getxpos(), getypos() );
                  }
               }
               break;

            case ct_1:
               execuseraction ( ua_changeresourceview );
               break;

            case ct_2:
               execuseraction ( ua_toggleunitshading );
               break;

            case ct_3:
               viewunitweaponrange ( getactfield()->vehicle, ct_3 );
               break;

            case ct_4:
               viewunitmovementrange ( getactfield()->vehicle, ct_4 );
               break;

            case ct_5:
               execuseraction ( ua_GameStatus );
               break;

            case ct_6:
               execuseraction ( ua_UnitSetInfo );
               break;

            case ct_7:
               execuseraction ( ua_viewterraininfo );
               break;

            case ct_8:
               execuseraction ( ua_unitweightinfo );
               break;

            case ct_9: {
                for ( int i = 0;i < 9; i++ )
                   for ( tmap::Player::VehicleList::iterator j = actmap->player[i].vehicleList.begin(); j != actmap->player[i].vehicleList.end(); j++ )
                      if ( (*j)->networkid == 29809 )
                         displaymessage ( actmap->getPlayerName(i ), 1);

            }
               {
                  // testtext ( getterraintype_forid ( 1011 ), getobjecttype_forid ( 1 ) );
                  /*
                  static pvehicle veh = 0;
                  if ( !veh ) {
                  veh = getactfield()->vehicle;
               } else {
                  actmap->cleartemps ( 15 );
                  std::vector<MapCoordinate3D> path;
                  AStar3D ast ( actmap, veh );
                  ast.findAllAccessibleFields ( veh->tank.fuel / veh->typ->fuelConsumption * maxmalq );

                  for ( int x = 0; x < actmap->xsize; x++ )
                  for ( int y = 0; y < actmap->ysize; y++ )
                  if ( !(getfield( x,y )->a.temp & chfahrend) )
                     getfield( x,y )->a.temp = 0;
                    */
                  /*
                                               int x = veh->xpos;
                                               int y = veh->ypos;
                                               actmap->cleartemps( 15 );
                                               for ( int i = 0; i < path.size(); i++ )
                                                  actmap->getField ( path[i] )->a.temp = 1;
                   
                                               AI ai ( actmap, actmap->actplayer );
                                               int res = ai.moveUnit ( veh, path );
                                               if ( res < 0 )
                                                  displaymessage("error", 1 );
                                                  */
                  /*
                     if ( !getfield ( x, y ))
                        break;

                     getfield ( x, y )->a.temp = 1;
               } */

                  /*
                  for ( int xp = 0; xp < actmap->xsize; xp++ )
                     for ( int yp = 0; yp < actmap->ysize; yp++ )
                        if ( ast.fieldVisited ( xp, yp ))
                           getfield ( xp, yp )->a.temp = 1;
                  */
                  /*
                  displaymap();
                  veh = NULL;

               }  */
                  /*
                  static AStar* ast = 0;
                  if ( ast ) {
                     delete ast;
                     ast =  NULL;
               } else {
                     ast = new AStar ( actmap, getactfield()->vehicle );
                     ast->findAllAccessibleFields ( );
                     displaymap();
               } */
                  // testland();
               }


               break;

            case ct_0:
               execuseraction ( ua_writescreentopcx );
               break;

            case ct_x + ct_stp:
               execuseraction ( ua_exitgame );
               break;
         }

      } else
         ch = ct_invvalue;


      mainloopgeneralmousecheck ( );

      /************************************************************************************************/
      /*        Pulldown Men?                                                                       . */
      /************************************************************************************************/

      checkpulldown( ch );

      while ( actmap->player[ actmap->actplayer ].queuedEvents )
         checkevents( &defaultMapDisplay );

      checktimedevents( &defaultMapDisplay );

      checkforvictory();

      releasetimeslice();

   }  while ( !abortgame );
}



pfont load_font ( const char* name )
{
   tnfilestream stream ( name , tnstream::reading );
   return loadfont ( &stream );
}

const char* progressbarfilename = "progress.6mn";



void loaddata( int resolx, int resoly, const char *gameToLoad=NULL )
{
   actprogressbar = new tprogressbar;
   if ( actprogressbar ) {
      tfindfile ff ( progressbarfilename );
      if ( !ff.getnextname().empty() ) {
         tnfilestream strm ( progressbarfilename, tnstream::reading );
         actprogressbar->start ( 255, 0, agmp->resolutiony-3, agmp->resolutionx-1, agmp->resolutiony-1, &strm );
      } else {
         actprogressbar->start ( 255, 0, agmp->resolutiony-3, agmp->resolutionx-1, agmp->resolutiony-1, NULL );
      }
   }

   schriften.smallarial = load_font("smalaril.fnt");
   schriften.large = load_font("usablack.fnt");
   schriften.arial8 = load_font("arial8.fnt");
   schriften.smallsystem = load_font("msystem.fnt");
   schriften.guifont = load_font("gui.fnt");
   schriften.guicolfont = load_font("guicol.fnt");
   schriften.monogui = load_font("monogui.fnt");
   activefontsettings.markfont = schriften.guicolfont;
   shrinkfont ( schriften.guifont, -1 );
   shrinkfont ( schriften.guicolfont, -1 );
   shrinkfont ( schriften.monogui, -1 );
   pulldownfont = schriften.smallarial ;

   if ( actprogressbar ) actprogressbar->startgroup();

   SoundList::init();

   if ( actprogressbar ) actprogressbar->startgroup();

   loadMoreData();

   if ( actprogressbar ) actprogressbar->startgroup();

   loadalltextfiles();

   if ( actprogressbar ) actprogressbar->startgroup();

   loadAllMusicPlayLists();

   if ( actprogressbar ) actprogressbar->startgroup();

   loadguipictures();
   loadallobjecttypes();

   if ( actprogressbar ) actprogressbar->startgroup();
   loadallvehicletypes();
   loadUnitSets();

   if ( actprogressbar ) actprogressbar->startgroup();
   loadallbuildingtypes();


   if ( actprogressbar ) actprogressbar->startgroup();
   loadalltechnologies();


   if ( actprogressbar ) actprogressbar->startgroup();
   loadstreets();

   if ( actprogressbar ) actprogressbar->startgroup();
   loadallterraintypes();

   if ( actprogressbar ) actprogressbar->startgroup();


   freetextdata();

   cursor.init();
   selectbuildinggui.init( resolx, resoly );
   selectobjectcontainergui.init( resolx, resoly );
   selectvehiclecontainergui.init( resolx, resoly );

   if ( actprogressbar ) actprogressbar->startgroup();

   loadStartupMap( gameToLoad );

   if ( actprogressbar ) actprogressbar->startgroup();

   displayLogMessage ( 6, "loading gui icons..." );
   gui.starticonload();
   displayLogMessage ( 6, "done\n" );

   if ( actprogressbar ) actprogressbar->startgroup();

   dashboard.allocmem ();

   mousecontrol = new cmousecontrol;

   if ( actprogressbar ) {
      actprogressbar->end();
      try {
         tnfilestream strm ( progressbarfilename, tnstream::writing );
         actprogressbar->writetostream( &strm );
      } /* endtry */
      catch ( tfileerror ) { } /* endcatch */

      delete actprogressbar;
      actprogressbar = NULL;
   }
}







void runmainmenu ( void )
{
   MainMenuPullDown pd;
   pd.init();
   backgroundpict.paint();
   pd.baron();
   // loadFullscreenImage ( "title.jpg" );

   do {
      tkey ch = ct_invvalue;
      if (keypress()) {
         ch = r_key();

         switch (ch) {
            case ct_f3:
               execuseraction ( ua_continuenetworkgame );
               break;
            case 'R':
               execuseraction ( ua_repainthard );
               break;
            case ct_stp + ct_l:
               execuseraction ( ua_loadgame );
               break;
            case ct_stp + ct_n:
               execuseraction ( ua_startnewsinglelevel );
               break;
            case ct_x + ct_stp:
               execuseraction ( ua_exitgame );
               break;
         };
      }

      pd.key = ch;
      pd.checkpulldown();

      if (pd.action2execute >= 0 ) {
         tuseractions ua = (tuseractions) pd.action2execute;
         pd.action2execute = -1;
         execuseraction ( ua );
         pd.redraw();
      }

      releasetimeslice();
   } while ( !actmap && !abortgame ); /* enddo */

}



struct GameThreadParams
{
   ASCString filename;
};

int gamethread ( void* data )
{
   GameThreadParams* gtp = (GameThreadParams*) data;

   initMapDisplay( );

   int resolx = agmp->resolutionx;
   int resoly = agmp->resolutiony;
   gui.init ( resolx, resoly );
   virtualscreenbuf.init();

   try {
      int fs = loadFullscreenImage ( "title.jpg" );
      if ( !fs ) {
         tnfilestream stream ( "logo640.pcx", tnstream::reading );
         loadpcxxy( &stream, (hgmp->resolutionx - 640)/2, (hgmp->resolutiony-35)/2, 1 );
      }
      loaddata( resolx, resoly, gtp->filename.c_str() );
      if ( fs )
         closeFullscreenImage ();

   } catch ( tfileerror err ) {
      displaymessage ( "unable to access file %s \n", 2, err.getFileName().c_str() );
   }
   catch ( ParsingError err ) {
      displaymessage ( "Error parsing text file " + err.getMessage(), 2 );
   }
   catch ( ASCexception ) {
      displaymessage ( "loading of game failed", 2 );
   }

   displayLogMessage ( 5, "loaddata completed successfully.\n" );
   setvgapalette256(pal);

   addmouseproc ( &mousescrollproc );

   startMusic();

   onlinehelp = new tsgonlinemousehelp;
   onlinehelpwind = new tsgonlinemousehelpwind;

   pd.init();

   abortgame = 0;
   gameStartupComplete = true;


   displayLogMessage ( 5, "entering outer main loop.\n" );
   do {
      try {
         if ( !actmap || actmap->xsize <= 0 || actmap->ysize <= 0 ) {
            displayLogMessage ( 8, "gamethread :: starting main menu.\n" );
            runmainmenu();
         } else {
            if ( actmap->actplayer == -1 ) {
               displayLogMessage ( 8, "gamethread :: performing next_turn..." );
               next_turn();
               displayLogMessage ( 8, "done.\n" );
            }

            displayLogMessage ( 8, "gamethread :: Painting background pict..." );
            backgroundpict.paint();

            if ( !gtp->filename.empty() && patimat ( tournamentextension, gtp->filename.c_str() ) ) {
               displayLogMessage ( 5, "Initializing network game..." );
               initNetworkGame ( );
               displayLogMessage ( 5, "done\n" );
            }

            displayLogMessage ( 8, "gamethread :: displaying map..." );
            displaymap();
            displayLogMessage ( 8, "done.\n" );
            cursor.show();

            moveparams.movestatus = 0;

            displayLogMessage ( 8, "gamethread :: painting gui icons..." );
            actgui->painticons();
            displayLogMessage ( 8, "done.\n" );
            mousevisible(true);

            dashboard.x = 0xffff;
            dashboard.y = 0xffff;

            setupMainScreenWidget();

            displayLogMessage ( 5, "entering inner main loop.\n" );
            mainloop();
            mousevisible ( false );
         }
      } /* endtry */
      catch ( NoMapLoaded ) { } /* endcatch */


   } while ( abortgame == 0);
   return 0;
}


// including the command line parser, which is generated by genparse
#include "clparser/asc.cpp"

int main(int argc, char *argv[] )
{
   Cmdline* cl = NULL;
   auto_ptr<Cmdline> apcl ( cl );
   try {
      cl = new Cmdline ( argc, argv );
   } catch ( string s ) {
      cerr << s.c_str();
      exit(1);
   }

   /*
   if ( cl->next_param() < argc ) {
      cerr << "invalid command line parameter\n";
      exit(1);
   }*/

   if ( cl->v() ) {
      ASCString msg = getstartupmessage();
      printf( msg.c_str() );
      exit(0);
   }

   if ( cl->w() )
      fullscreen = SDL_FALSE;

   if ( cl->f() )
      fullscreen = SDL_TRUE;

   verbosity = cl->r();

   mapborderpainter = &backgroundpict;

   initmissions();


   initFileIO( cl->c().c_str() );  // passing the filename from the command line options

   try {
      checkDataVersion();
      check_bi3_dir ();
   } catch ( tfileerror err ) {
      displaymessage ( "unable to access file %s \n", 2, err.getFileName().c_str() );
   }
   catch ( ... ) {
      displaymessage ( "loading of game failed during pre graphic initializing", 2 );
   }

   if ( CGameOptions::Instance()->forceWindowedMode && !cl->f() )  // cl->f == force fullscreen command line param
      fullscreen = SDL_FALSE;

   SDLmm::Surface* icon = NULL;
   try {
      tnfilestream iconl ( "icon_asc.gif", tnstream::reading );
      SDL_Surface *icn = IMG_LoadGIF_RW( SDL_RWFromStream ( &iconl ));
      SDL_SetColorKey(icn, SDL_SRCCOLORKEY, *((Uint8 *)icn->pixels));
      icon = new SDLmm::Surface ( icn );
   } catch ( ... ) {}


   int xr = 800;
   int yr = 600;
   // determining the graphics resolution
   if ( CGameOptions::Instance()->xresolution != 800 )
      xr = CGameOptions::Instance()->xresolution;
   if ( cl->x() != 800 )
      xr = cl->x();

   if ( CGameOptions::Instance()->yresolution != 600 )
      yr = CGameOptions::Instance()->yresolution;
   if ( cl->y() != 600 )
      yr = cl->y();


   SoundSystem soundSystem ( CGameOptions::Instance()->sound.muteEffects, CGameOptions::Instance()->sound.muteMusic, cl->q() || CGameOptions::Instance()->sound.off );

   soundSystem.setMusicVolume ( CGameOptions::Instance()->sound.musicVolume );
   soundSystem.setEffectVolume ( CGameOptions::Instance()->sound.soundVolume );


   ASC_PG_App app ( "asc_dlg" );
   pgApp = &app;

   int flags = SDL_SWSURFACE;
   if ( fullscreen )
      flags |= SDL_FULLSCREEN;

   setWindowCaption ( "Advanced Strategic Command" );
   SDL_WM_SetIcon( icon->GetSurface(), NULL );


   app.InitScreen( xr, yr, 8, flags);

   initASCGraphicSubsystem ( app.GetScreen(), icon );

   GameThreadParams gtp;
   gtp.filename = cl->l();

   if ( cl->next_param() < argc )
      for ( int i = cl->next_param(); i < argc; i++ )
         gtp.filename = argv[i];


   {
      int w;
      tnfilestream stream ("mausi.raw", tnstream::reading);
      stream.readrlepict(   &icons.mousepointer, false, &w );
   }


   // this starts the gamethread procedure, whichs will run the entire game
   initializeEventHandling ( gamethread, &gtp, icons.mousepointer );

   closegraphics();

   writegameoptions ( );

   delete onlinehelp;
   onlinehelp = NULL;

   delete onlinehelpwind;
   onlinehelpwind = NULL;

#ifdef MEMCHK
   verifyallblocks();
#endif
   return(0);
}

