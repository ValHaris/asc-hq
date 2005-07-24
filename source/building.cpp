/*! \file building.cpp
    \brief The implementation of basic logic and the UI of buildings&transports
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2005  Martin Bickel  and  Marc Schellenberger
 
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
#include <stdlib.h>
#include <math.h>


#include "typen.h"
#include "vehicletype.h"
#include "buildingtype.h"

#include "misc.h"
#include "basegfx.h"
#include "newfont.h"
#include "dlg_box.h"
#include "spfst.h"
#include "stdio.h"
#include "dialog.h"
#include "events.h"
#include "building.h"
#include "building_controls.h"
#include "controls.h"
#include "loadpcx.h"
#include "sg.h"
#include "stack.h"
#include "gamedlg.h"
#include "basestrm.h"
#include "unitctrl.h"
#include "gameoptions.h"
#include "replay.h"
#include "dashboard.h"
#include "resourcenet.h"
#include "research.h"
#include "itemrepository.h"







//typedef class ccontainer*   pcontainer;
typedef class ccontainer_b* pcontainer_b;
typedef class ccontainer_t* pcontainer_t;




//-------------------------------------------------------------------------subwindows
typedef class csubwindow* psubwindow;

class csubwindow
{
   protected:
      int lastclickpos;
      int subwinid;
      int active;          // 0: šberhauptnicht vorhanden ;
      // 1: existent, aber nicht aktiv;
      // 2: existent und aktiv;


      psubwindow next;
      psubwindow nextavailable;
      pcontainer hostcontainer;
      char name[30];
      psubwindow *first;
      psubwindow *firstavailable;

      int laschx1, laschx2, laschcol;
      void* laschpic1;
      void* laschpic2;
      void paintlasche ( void );
      int  objpressedbymouse ( int num );

      struct
      {
         int x1, y1, x2, y2;
         int t1, t2;
         int type;
      }
      objcoordinates[30];
      int objnum;

      virtual void paintobj ( int num, int stat );
      virtual void checkformouse ( void ) = 0;


   public:
      tonlinehelplist helplist;
      void sethostcontainerchain ( pcontainer cntn );
      void setlaschenpos ( int x1, int maxwidth );
      virtual int subwin_available ( void ) ;
      virtual void checkforkey( tkey ch ) = 0;
      virtual void display ( void );
      void mousecheck ( int checkall );
      void setactive ( void );
      void paintalllaschen ( int i = 0 );
      void sethostcontainer ( pcontainer cntn );
      void init ( void );
      virtual void unitchanged ( void );
      void checklaschkey ( tkey taste );

      // virtual void setinactive( void );

      virtual void resetammo ( int mode );
      virtual void resetresources ( int mode );

      csubwindow ( void );
      virtual ~csubwindow ( );
};







class tcontaineronlinemousehelp : public tonlinemousehelp
{
      pcontainer hostcontainer;
   public:
      tonlinehelplist* helplist2;
      virtual void checkforhelp ( void );
      tcontaineronlinemousehelp ( pcontainer host );
};



class   hosticons_c: public ContainerBaseGuiHost
{ // basis fuer icons ->struct mit allen icons
   public:
      void seticonmains ( pcontainer maintemp );
};





class    ccontainer : public virtual ccontainercontrols
{
      void*  containerpicture;
      ASCString  name1;
      ASCString  name2;
      int mousestat;
   public:
      //-------------------------------------------------------------------------icons


      class repairicon_c : public generalicon_c
      {
         public:
            virtual int   available    ( void ) ;
            virtual void  exec         ( void ) ;
            virtual const char* getinfotext  ( void );
            repairicon_c ( void );
      };


      class moveicon_c : public generalicon_c
      {
         public:
            virtual int   available    ( void ) ;
            virtual void  exec         ( void ) ;
            moveicon_c ( void );
      };


      class fill_dialog_icon_c  : public generalicon_c
      {
         public:
            virtual int   available    ( void ) ;
            fill_dialog_icon_c ( void );
      };

      class fill_icon_c  : public generalicon_c , public ccontainercontrols::crefill
      {
         public:
            virtual void  exec         ( void ) ;
            fill_icon_c ( void );
      };

      class exit_icon_c  : public generalicon_c
      {
         public:
            virtual int   available    ( void ) ;
            virtual void  exec         ( void ) ;
            exit_icon_c ( void );
      };


      class container_icon_c : public generalicon_c
      {
         public:
            virtual int   available    ( void ) ;
            virtual void  exec         ( void ) ;
            container_icon_c ( void );
      };

      class cmoveup_icon_c : public generalicon_c, public ccontainercontrols::cmove_unit_in_container
      {
         public:
            virtual int   available    ( void ) ;
            virtual void  exec         ( void ) ;
            cmoveup_icon_c ( void );
      };

      class cunitinformation_icon : public generalicon_c
      {
         public:
            virtual int   available    ( void ) ;
            virtual void  exec         ( void ) ;
            cunitinformation_icon ( void );
      };

      class cmovedown_icon_c
               :	public generalicon_c,
               public ccontainercontrols::cmove_unit_in_container
      {
         public:
            virtual int   available    ( void ) ;
            virtual void  exec         ( void ) ;
            cmovedown_icon_c ( void );
      };

      // auch f?r "move in"
      class productioncancelicon_cb : public generalicon_c
      {
         public:
            virtual int   available    ( void ) ;
            virtual void  exec         ( void ) ;
            productioncancelicon_cb    ( void );
      };


      //-------------------------------------------------------------------------subwindows

      class  cammunitiontransfer_subwindow
               :	public csubwindow, public ccontainercontrols::crefill
      {
         protected:
            struct
            {
               int maxnum;
               int orgnum;
               int actnum;
               int type;
               int pos;
               int buildnum;
               const char* name;
            }
            weaps[20];
            int schieblength;
            int schiebpos[20];
            void checkformouse ( void );
            int num;
            pvehicle eht;
            void reset ( pvehicle veh = NULL );
            int  gpres ( int i );
            void check ( int i );
            void transfer ( void );
            void paintobj ( int num, int stat );
            char* txtptr;
            int actschieber;
            int externalloadingactive;
            virtual int externalloadavailable ( void );
            virtual void execexternalload ( void );
            int page;
            int pagenum;
         public:
            cammunitiontransfer_subwindow ( void );
            int  subwin_available ( void );
            void display ( void ) ;
            void displayvariables ( void ) ;
            void checkforkey ( tkey taste );
            void unitchanged ( void );
            void resetammo ( int mode );
            void resetresources ( int mode );
      };


      psubwindow allsubwindows[10];

      struct
      {
         int x;
         int y;
      }
      mark;

   protected:

      void*    picture[32];
      char     pictgray[32];

      void*    activefield;
      void*    inactivefield;

      void putFieldImage( int x, int y, void* image );

      void     checkformouse();
      int      getfieldundermouse ( int* x, int* y );

      void     showresources  ( void );
      void     showammo ( void );

      tcontaineronlinemousehelp* containeronlinemousehelp;

   public :
      virtual void paintvehicleinfo ( void );
      virtual void unitchanged( void );
      int      keymode;   // mit tab wird umgeschaltet. 0 : Laderaum ; 1 : Subwin ; 2 : Laschen
      int      repaintammo;
      int      repaintresources;
      void repaintResources() { repaintresources = 1;};

      void     setactunittogray ( void );
      void     buildgraphics();


      psubwindow actsubwindow;
      int  allsubwindownum;
      int end;
      BasicGuiHost*    oldguihost;
      virtual void     setpictures ( void );

      tunitmode unitmode;  // wird erst im Building-Container ben”tigt, aber damit die Icons darauf zugreifen k”nnen ist das teil schon hier ...

      void     init ( void *pict, int col, const ASCString& name, const ASCString& descr);
      void     registersubwindow ( psubwindow subwin );
      void     run (void);
      void     done (void);
      void     movemark (int direction);
      virtual pvehicle getmarkedunit (void) = 0;
      virtual pvehicletype getmarkedunittype ( void );
      void     displayloading ( int x, int y, int dx = 0, int dy = 0 );
      void     displayloading (void);
      ccontainer (void);
      virtual ~ccontainer (void);
};







typedef class cbuildingsubwindow* pbuildingsubwindow;

class cbuildingsubwindow : public virtual csubwindow
{
      void sethostcontainerchain ( pcontainer_b cntn );
   protected:
      pbuildingsubwindow         nextb;
      pcontainer_b               hostcontainerb;
      pbuildingsubwindow         *firstb;
   public:
      void sethostcontainer ( pcontainer_b cntn );
      cbuildingsubwindow ( void );
      ~cbuildingsubwindow ( );
};


class    ccontainer_b : public cbuildingcontrols , public ccontainer
{

   protected :

      class fill_dialog_icon_cb  : public ccontainer::fill_dialog_icon_c
      {
         public:
            virtual void  exec         ( void ) ;
      };

      class fill_icon_cb  : public ccontainer::fill_icon_c
      {
         public:
            virtual int   available    ( void ) ;
      };


      class recyclingicon_cb
               :	public generalicon_c ,
               public cbuildingcontrols::crecycling
      {
         public:
            virtual int   available    ( void ) ;
            virtual void  exec         ( void ) ;
            virtual const char* getinfotext  ( void );
            recyclingicon_cb ( void );
      };


      class trainuniticon_cb
               :	public generalicon_c ,
               public cbuildingcontrols::ctrainunit
      {
         public:
            virtual int   available    ( void ) ;
            virtual void  exec         ( void ) ;
            trainuniticon_cb ( void );
      };

      class BuildProductionLineIcon : public generalicon_c
      {
         public:
            virtual int   available    ( void ) ;
            virtual void  exec         ( void ) ;
            void loadspecifics( pnstream stream );

            BuildProductionLineIcon ( void );
      };

      class RemoveProductionLine : public generalicon_c
      {
         public:
            virtual int   available    ( void ) ;
            virtual void  exec         ( void ) ;
            void loadspecifics( pnstream stream );

            RemoveProductionLine ( void );
      };


      class dissectuniticon_cb
               :	public generalicon_c,
               public cbuildingcontrols::cdissectunit
      {
         public:
            virtual int   available    ( void ) ;
            virtual void  exec         ( void ) ;
            dissectuniticon_cb ( void );
      };

      class produceuniticon_cb
               :	public generalicon_c,
               public cbuildingcontrols::cproduceunit
      {
         public:
            virtual int   available    ( void ) ;
            virtual void  exec         ( void ) ;
            virtual const char* getinfotext  ( void );
            produceuniticon_cb ( void );
      };

      class takeofficon_cb : public generalicon_c
      {
         public:
            virtual int   available    ( void ) ;
            virtual void  exec         ( void ) ;
            takeofficon_cb             ( void ) ;
      };

      class   chosticons_cb : public hosticons_c
      {
         public:
            void init ( int resolutionx, int resolutiony );
            struct I1
            {
               moveicon_c          movement;
               recyclingicon_cb    recycling;
               repairicon_c        repair;
               trainuniticon_cb    training;
               dissectuniticon_cb  dissect;
               BuildProductionLineIcon buildProductionLine;
               RemoveProductionLine removeProductionLine;
               //fill_dialog_icon_cb filldialog;
               fill_icon_cb        fill;
               produceuniticon_cb  produceunit;
               productioncancelicon_cb cancel;
               // takeofficon_cb      takeofficon;
               exit_icon_c         exit;
               container_icon_c    contain;
               cmovedown_icon_c    movedown;
               cmoveup_icon_c      moveup;
               cunitinformation_icon unitinformation;
            }
            icons;
      }
      hosticons_cb;



      //-------------------------------------------------------------------------subwindows

      class  crepairbuilding_subwindow
               :	public cbuildingsubwindow
      {
         public:
            crepairbuilding_subwindow ( void );
            int  subwin_available ( void );
            void display ( void ) ;
            void checkforkey ( tkey taste );
         protected:
            void paintobj ( int num, int stat );
            void checkformouse ( void );
            int  ndamag;
            void paintvariables ( void );
      };


      class  cnetcontrol_subwindow
               :	public cbuildingsubwindow ,
               public cbuildingcontrols::cnetcontrol
      {
         protected:
            void checkformouse ( void );
            void objpressed ( int num );
            void paintobj ( int num, int stat );
         public:
            cnetcontrol_subwindow ( void );
            int  subwin_available ( void );
            void display ( void ) ;
            void checkforkey ( tkey taste );
            int getstatus ( int num );
      };

      class  cconventionelpowerplant_subwindow : public cbuildingsubwindow
      {
         protected:
            static int allbuildings;
            void checkformouse ( void );
            void setnewpower ( int pwr );
            void displayvariables ( void );
            void paintobj ( int num, int stat );
            void dispresources ( Resources* res, int ypos, int sign );

            int resourcecolor[3];
            int power;
            int gx1, gy1, gx2, gy2;
         public:
            cconventionelpowerplant_subwindow ( void );
            int  subwin_available ( void );
            void display ( void ) ;
            void checkforkey ( tkey taste );
      };

      class  cwindpowerplant_subwindow : public cbuildingsubwindow
      {
         protected:
            void checkformouse ( void );
         public:
            cwindpowerplant_subwindow ( void );
            int  subwin_available ( void );
            void display ( void ) ;
            void checkforkey ( tkey taste );
            void resetresources ( int mode );
      };

      class  csolarpowerplant_subwindow : public cbuildingsubwindow /*, public crepairbuilding */
      {
         protected:
            void checkformouse ( void );
         public:
            csolarpowerplant_subwindow ( void );
            int  subwin_available ( void );
            void display ( void ) ;
            void checkforkey ( tkey taste );
            void resetresources ( int mode );
      };

      class  cammunitionproduction_subwindow
               :	public cbuildingsubwindow ,
               public cbuildingcontrols::cproduceammunition
      {
            int toproduce[waffenanzahl];
            double maxproduceablenum;
            double grad;
         protected:
            int schieblength;
            void checkformouse ( void );
            int  pos2num ( int pos );
            int  num2pos ( int num );
            void displayusage( void );
            void checknewval ( int weaptype, int num );
            void setnewamount (  int weaptype, int num  );
            void paintobj ( int num, int stat );
            void produce( void );
            char* txtptr;
            int actschieber;

         public:
            cammunitionproduction_subwindow ( void );
            int  subwin_available ( void );
            void display ( void ) ;
            void checkforkey ( tkey taste );
      };


      class  cresourceinfo_subwindow : public cbuildingsubwindow /*, public crepairbuilding */
      {
         protected:
            void checkformouse ( void );
            struct
            {
               int r[4][3];
            }
            resource;
            int recalc;

         public:
            cresourceinfo_subwindow ( void );
            int  subwin_available ( void );
            int  getvalue ( int x, int y, int mode  );
            void displayvariables( void );
            void display ( void ) ;
            void checkforkey ( tkey taste );
            void resetammo ( int mode );
            void resetresources ( int mode );
      };

      class  cresearch_subwindow : public cbuildingsubwindow /*, public crepairbuilding */
      {
         protected:
            static int allbuildings;
            int research;    // 1024 ist maximale Forschung
            void checkformouse ( void );
            int gx1, gy1, gx2, gy2;
            int materialcolor, energycolor,fuelcolor;
         public:
            cresearch_subwindow ( void );
            int  subwin_available ( void );
            void display ( void ) ;
            void displayvariables ( void );
            void checkforkey ( tkey taste );
            void setnewresearch ( int res );
            void paintobj ( int num, int stat );
      };

      class  cminingstation_subwindow : public cbuildingsubwindow
      {
         protected:
            static int allbuildings;
            int extraction;    // 1024 ist maximale Production
            void checkformouse ( void );
            int gx1, gy1, gx2, gy2;
            int materialcolor, energycolor, fuelcolor;
            int resourcecolor[3];
            void dispresources ( Resources* res, int ypos, int sign );
            
            GetMiningInfo::MiningInfo mininginfo;
            bool mininginfoLoaded;

         public:
            cminingstation_subwindow ( void );
            int  subwin_available ( void );
            void display ( void ) ;
            void displayvariables ( void );
            void checkforkey ( tkey taste );
            void setnewextraction ( int res );
            void paintobj ( int num, int stat );
      };
      class  cmineralresources_subwindow : public cbuildingsubwindow
      {
         protected:
            static int allbuildings;
            int extraction;    // 1024 ist maximale Production
            void checkformouse ( void );
            int gx1, gy1, gx2, gy2;
            int hx1, hy1, hx2, hy2;
            int materialcolor, energycolor, fuelcolor;
            int resourcecolor[3];
            void dispresources ( Resources* res, int ypos, int sign );

         public:
            GetMiningInfo::MiningInfo mininginfo;
            bool mininginfoLoaded;
            cmineralresources_subwindow ( void );
            int  subwin_available ( void );
            void display ( void ) ;
            void displayvariables ( void );
            void checkforkey ( tkey taste );
            void setnewextraction ( int res );
            void paintobj ( int num, int stat );
      };
      class cammunitiontransferb_subwindow
               :	public ccontainer::cammunitiontransfer_subwindow
      {
         protected:
            virtual int externalloadavailable ( void );
            virtual void execexternalload ( void );
      };

      int    putammunition (int  weapontype, int  ammunition, int abbuchen);
      int    getammunition ( int weapontype, int num, int abbuchen, int produceifrequired = 0 );
      pvehicle getloadedunit (int num);
      virtual void     setpictures ( void );

      pvehicletype getmarkedunittype ( void );
      pvehicletype produceableunits[32];


      struct I2
      {
         crepairbuilding_subwindow             repairbuilding_subwindow;
         cnetcontrol_subwindow                 netcontrol_subwindow;
         cconventionelpowerplant_subwindow     conventionelpowerplant_subwindow;
         cwindpowerplant_subwindow             windpowerplant_subwindow ;
         csolarpowerplant_subwindow            solarpowerplant_subwindow;
         cammunitionproduction_subwindow       ammunitionproduction_subwindow;
         cammunitiontransferb_subwindow        ammunitiontransfer_subwindow;
         cresourceinfo_subwindow               resourceinfo_subwindow;
         cresearch_subwindow                   research_subwindow;
         cminingstation_subwindow              miningstation_subwindow;
         cmineralresources_subwindow           mineralresources_subwindow;
      }
      subwindows;


   public :
      virtual void paintvehicleinfo ( void );
      virtual void unitchanged( void );
      void     init (pbuilding bld);

      pvehicle getmarkedunit (void);
      ccontainer_b ( void );
      ~ccontainer_b ( void );

};










class    ccontainer_t : public ctransportcontrols , public ccontainer
{

   protected :

      class fill_dialog_icon_ct  : public ccontainer::fill_dialog_icon_c
      {
         public:
            virtual void  exec         ( void ) ;
      };

      class fill_icon_ct
               :	public ccontainer::fill_icon_c
      {
         public:
            virtual int   available    ( void ) ;
      };

      class   chosticons_ct :	public hosticons_c
      {
         public:
            void init ( int resolutionx, int resolutiony );
            struct I3
            {
               moveicon_c          movement;
               repairicon_c        repair;
               //fill_dialog_icon_ct filldialog;
               fill_icon_ct        fill;
               exit_icon_c         exit;
               container_icon_c    contain;
               cmovedown_icon_c    movedown;
               cmoveup_icon_c      moveup;
               productioncancelicon_cb cancel;
               cunitinformation_icon unitinformation;
            }
            icons;
      }
      hosticons_ct;

      pvehicle getloadedunit (int num);

      int    putammunition (int  weapontype, int  ammunition, int abbuchen);
      int    getammunition ( int weapontype, int num, int abbuchen, int produceifrequired = 0 );

      class    ctransportinfo_subwindow : public csubwindow
      {
         public:
            ctransportinfo_subwindow ( void );
            int  subwin_available ( void );
            void display ( void ) ;
            void unitchanged ( void );
            void checkforkey ( tkey taste );
         protected:
            void checkformouse ( void );
            void paintvariables ( void );
      };

      struct I4
      {
         ctransportinfo_subwindow            transportinfo;
         cammunitiontransfer_subwindow       ammunitiontransfer;
      }
      subwindows;

   public :
      void     init ( pvehicle eht );

      pvehicle getmarkedunit (void);
      ccontainer_t ( void );
      ~ccontainer_t ( void );

};







struct tbuildingparamstack
{
   pgeneralicon_c generalicon_c__first;
   pgeneralicon_c generalicon_c__firstguiicon;
   psubwindow csubwindow_first;
   psubwindow csubwindow_firstavailable;
   pbuildingsubwindow cbuildingsubwindow_firstb;

   char* name;
   pbuilding bld;
   pvehicle eht;
} ;




pcontainercontrols         cc;
pbuildingcontrols          cc_b;
ptransportcontrols         cc_t;

#define maxrecursiondepth 10


int recursiondepth = -1;
tbuildingparamstack buildingparamstack[maxrecursiondepth];


#define containerxpos 100
#define containerypos 60


const int repaint=0;
const int repaintall=0;
const int cursor_up=1;
const int cursor_down=2;
const int cursor_left=3;
const int cursor_right=4;

#define unitsshownx 6

#define unitsshowny 3

int unitposx[ unitsshownx + 1 ] = { containerxpos + 20,
                                    containerxpos + 76,
                                    containerxpos + 135,
                                    containerxpos + 190,
                                    containerxpos + 250,
                                    containerxpos + 304,
                                    containerxpos + 304 + fieldxsize };

int unitposy[ unitsshowny + 1 ] = { containerypos + 105,
                                    containerypos + 159,
                                    containerypos + 213,
                                    containerypos + 213 + fieldysize };




 #define subwinx1 (containerxpos + 13)
 #define subwiny1 (containerypos + 276 )
 #define subwinx2 ( subwinx1 + 345 )
 #define subwiny2 ( subwiny1 + 110 )
 #define laschxpos subwinx1
 #define laschypos (subwiny2+1)
 #define laschdist 50
 #define nameposx containerxpos + 149
 #define nameposy containerypos + 17


#define laschheight 12
#define laschstepwidth 3
#define bkgrcol 171
#define bkgrdarkcol 172


 int tabmarkpos[3] = { containerypos + 154-42, containerypos + 273-42+54, containerypos + 388-42+54 };
 int tabmarkposx = containerxpos+ 4;

const char* resourceusagestring = "; need: ~%d~ energy, ~%d~ material, ~%d~ fuel";


int windowwidth = -1;
int windowheight = -1;


int getstepwidth ( int max )
{
   double ep = log10 ( double(max) );
   if ( ep > 2.0 )
      ep -= 2.0;
   else
      ep = 0.0;

   return int(pow ( double(10), ep ));
}



void  container ( pvehicle eht, pbuilding bld )
{
   recursiondepth++;
   memset ( &buildingparamstack[recursiondepth], 0, sizeof ( buildingparamstack[recursiondepth] ));
   npush ( cc );
   npush ( cc_b );
   npush ( cc_t );

   if ( eht && eht->color == actmap->actplayer * 8  ) {
      buildingparamstack[recursiondepth].eht = eht;
      ccontainer_t  containert;
      containert.init ( eht );
      containert.run ();
      containert.done ();
   } else
      if (bld && ( bld->color == actmap->actplayer * 8  || bld->color == 8*8 )) {
         buildingparamstack[recursiondepth].bld = bld;
         ccontainer_b  containerb;
         containerb.init ( bld );
         containerb.run ();
         containerb.done ();
      };

   npop ( cc_t );
   npop ( cc_b );
   npop ( cc );
   recursiondepth--;
}




/***********************************************************************************/
/*                                                                                 */
/*   class    ccontainercontrols                                                   */
/*                                                                                 */
/***********************************************************************************/
ccontainercontrols :: ccontainercontrols (void)
{
   cc = this;
}



void  ccontainercontrols :: crefill :: resource (pvehicle eht, int resourcetype, int newamount)
{
   int oldamount = eht->getResource(maxint, resourcetype, true);
   int storable = eht->putResource(newamount - oldamount, resourcetype, true);

   eht->putResource( cc->getResource ( storable, resourcetype, false ), resourcetype, false );

   logtoreplayinfo ( rpl_refuel2, eht->xpos, eht->ypos, eht->networkid, int(1000+resourcetype), eht->getTank().resource(resourcetype), oldamount );
   logtoreplayinfo ( rpl_refuel3, cc->baseContainer->getIdentification(), int(1000+resourcetype), eht->getTank().resource(resourcetype) - oldamount );
};



void  ccontainercontrols :: crefill :: ammunition (pvehicle eht, char weapon, int newa )
{
   if ( eht->typ->weapons.weapon[ weapon ].requiresAmmo() ) {
      if ( newa > eht->typ->weapons.weapon[ weapon ].count )
         newa = eht->typ->weapons.weapon[ weapon ].count;

      if ( newa > eht->ammo[weapon] )
         eht->ammo[weapon]  +=  cc->getammunition ( eht->typ->weapons.weapon[ weapon ].getScalarWeaponType() , newa - eht->ammo[weapon], 1, CGameOptions::Instance()->container.autoproduceammunition );
      else {
         cc->putammunition ( eht->typ->weapons.weapon[ weapon ].getScalarWeaponType() , eht->ammo[weapon]  - newa, 1 );
         eht->ammo[weapon] = newa;
      }
   }
   logtoreplayinfo ( rpl_refuel, eht->xpos, eht->ypos, eht->networkid, int(weapon), newa );

};



void  ccontainercontrols :: crefill :: filleverything ( pvehicle eht )
{
   resource ( eht, 1, maxint );
   resource ( eht, 2, maxint );
   for (int i = 0; i < eht->typ->weapons.count; i++)
      if ( eht->typ->weapons.weapon[ i ].requiresAmmo() )
         ammunition ( eht, i, maxint );

}

void  ccontainercontrols :: crefill :: emptyeverything ( pvehicle eht )
{
   resource ( eht, 1, 0 );
   resource ( eht, 2, 0 );
   for (int i = 0; i < eht->typ->weapons.count; i++)
      if ( eht->typ->weapons.weapon[ i ].requiresAmmo() )
         ammunition ( eht, i, 0 );

   for ( int i = 0; i < 32; i++ )
      if ( eht->loading[i] )
         emptyeverything ( eht->loading[i] );
}


bool ccontainercontrols :: cmove_unit_in_container :: moveupavail ( pvehicle eht )
{
   if ( eht ) {
      if ( recursiondepth > 0 ) {
         if ( buildingparamstack[recursiondepth-1].bld )
            return buildingparamstack[recursiondepth-1].bld->vehicleFit ( eht );
         else
            if ( buildingparamstack[recursiondepth-1].eht )
               return buildingparamstack[recursiondepth-1].eht->vehicleFit ( eht );
      }
   }
   return 0;
}

bool ccontainercontrols :: cmove_unit_in_container :: movedownavail ( pvehicle eht, pvehicle into )
{
   if ( eht )
      if ( into )
         if ( into->vehicleFit ( eht ))
            return 1;
   return 0;
}


void ccontainercontrols :: cmove_unit_in_container :: moveup ( pvehicle eht )
{
   if ( moveupavail( eht ) )
      if ( eht ) {
         pvehicle targe = buildingparamstack[recursiondepth-1].eht;
         pbuilding targb = buildingparamstack[recursiondepth-1].bld;

         if ( targe ) {
            int i = 0;
            while ( i < maxloadableunits   &&  targe->loading[i] )
               i++;

            if ( !targe->loading[i] ) {
               targe->loading[i] = eht;
               i = 0;
               while ( cc_t->vehicle->loading[i] != eht)
                  i++;

               cc_t->vehicle->loading[i] = 0;

            }
            logtoreplayinfo ( rpl_moveUnitUpDown, targe->getPosition().x, targe->getPosition().y, cc_t->vehicle->networkid, targe->networkid, eht->networkid );
         } else
            if ( targb ) {
               int i = 0;
               while ( i < maxloadableunits   &&  targb->loading[i] )
                  i++;

               if ( !targb->loading[i] ) {
                  targb->loading[i] = eht;
                  i = 0;
                  while ( cc_t->vehicle->loading[i] != eht)
                     i++;

                  cc_t->vehicle->loading[i] = 0;
               }
               logtoreplayinfo ( rpl_moveUnitUpDown, targb->getPosition().x, targb->getPosition().y, cc_t->vehicle->networkid, -1, eht->networkid );
            }
      }
}


void ccontainercontrols :: cmove_unit_in_container :: movedown ( pvehicle eht, pvehicle into )
{
   if ( movedownavail(eht, into) ) {

      int i = 0;
      while ( i < maxloadableunits   &&  into->loading[i] )
         i++;

      if ( !into->loading[i] ) {
         into->loading[i] = eht;

         i = 0;
         if ( cc_t ) {
            while ( cc_t->vehicle->loading[i] != eht)
               i++;

            cc_t->vehicle->loading[i] = 0;
            logtoreplayinfo ( rpl_moveUnitUpDown, cc_t->vehicle->getPosition().x, cc_t->vehicle->getPosition().y, cc_t->vehicle->networkid, into->networkid, eht->networkid );
         } else
            if ( cc_b ) {
               while ( cc_b->building->loading[i] != eht)
                  i++;

               cc_b->building->loading[i] = 0;
               logtoreplayinfo ( rpl_moveUnitUpDown, cc_b->building->getPosition().x, cc_b->building->getPosition().y, -1, into->networkid, eht->networkid );
            }
      }
   }
}

VehicleMovement*   ccontainercontrols :: movement (  pvehicle eht, bool simpleMode )
{
   VehicleMovement* vehicleMovement = new VehicleMovement ( &defaultMapDisplay, NULL );
   int mode = 0;
   if ( simpleMode )
      mode |= VehicleMovement::DisableHeightChange;

   int status = vehicleMovement->execute ( eht, -1, -1, 0, -1, mode );

   if ( status > 0 )
      return vehicleMovement;
   else {
      delete vehicleMovement;
      return NULL;
   }
}



/***********************************************************************************/
/*                                                                                 */
/*   class    cbuildingcontrols : Geb„ude-Innereien                                */
/*                                                                                 */
/***********************************************************************************/

cbuildingcontrols :: cbuildingcontrols (void)
{
   cc_b = this;
   cc_t = NULL;
}

void  cbuildingcontrols :: init (pbuilding bldng)
{
   building = bldng;
   baseContainer = bldng;
};



char  cbuildingcontrols :: getactplayer (void)
{
   return   building->color / 8;
};



int   cbuildingcontrols :: putammunition ( int weapontype, int ammunition, int abbuchen)
{
   if ( abbuchen )
      building->ammo[weapontype] += ammunition;
   return ammunition;
};




int    cbuildingcontrols :: getammunition ( int weapontype, int num, int abbuchen, int produceifrequired )
{
   if ( building->ammo[ weapontype ] > num  ) {

      if ( abbuchen )
         building->ammo[ weapontype ] -= num;
      return num;

   } else {
      int toprod = num - building->ammo[ weapontype ];
      int prd;
      if ( produceifrequired )
         prd = weaponpackagesize * cc_b->produceammunition.checkavail ( weapontype, (toprod + weaponpackagesize - 1) / weaponpackagesize );
      else
         prd = 0;

      if ( abbuchen ) {
         if ( produceifrequired )
            cc_b->produceammunition.produce ( weapontype, (toprod + weaponpackagesize - 1) / weaponpackagesize );

         if ( building->ammo[ weapontype ] > num ) {
            building->ammo[ weapontype ] -= num;
            return num;
         } else {
            int i = building->ammo[ weapontype ];
            building->ammo[ weapontype ] = 0;
            return i;
         }
      } else {
         if ( building->ammo[ weapontype ] + prd > num )
            return num;
         else
            return building->ammo[ weapontype ] + prd;
      }

   }
}

int    cbuildingcontrols :: ammotypeavail ( int type )
{
   return 1;
}


int   cbuildingcontrols :: getxpos (void)
{
   return   building->getEntry().x;
};



int   cbuildingcontrols :: getypos (void)
{
   return   building->getEntry().y;
};



int   cbuildingcontrols :: getspecfunc ( tcontainermode mode )
{
   if ( mode == mbuilding )
      return building->typ->special;
   else
      return 0;
};



int    cbuildingcontrols :: getHeight ( void )
{
   return building->typ->buildingheight;
}




void  cbuildingcontrols :: removevehicle ( pvehicle *peht )
{
   for (int i=0; i<=31; i++) {
      if ( *peht == building->loading[i] )
         building->loading[i]=NULL;
   }
   building->regroupUnits();
   logtoreplayinfo ( rpl_removeunit, building->getEntry().x, building->getEntry().y, (*peht)->networkid );
   delete *peht;
   *peht = NULL;
}


Resources  cbuildingcontrols :: crecycling :: resourceuse (pvehicle eht)
{
   int   output;
   if ( cc->getspecfunc( mbuilding ) & cgrecyclingplantb)
      output = recyclingoutput;
   else
      output = destructoutput;

   Resources res;

   for (int i=0; i<=31; i++)
      if ( eht->loading[i] )
         res += resourceuse ( eht->loading[i] );


   res.material += eht->typ->productionCost.material * (100 - eht->damage/2 ) / 100 / output;
   return res;
}


void  cbuildingcontrols :: crecycling :: recycle (pvehicle eht)
{
      Resources res = resourceuse ( eht );

      // if ( CGameOptions::Instance()->container.emptyeverything )
      cc->refill.emptyeverything ( eht );


      cc->getResource ( -res.material, Resources::Material, false );

      cc_b->removevehicle (&eht);
};



void   cbuildingcontrols :: cnetcontrol :: setnetmode ( int category, int stat )
{
   if ( stat )
      cc_b->building->netcontrol |= category;
   else
      cc_b->building->netcontrol &= ~category;


};

int    cbuildingcontrols :: cnetcontrol :: getnetmode ( int mode )
{
   return  cc_b->building->netcontrol & mode;
};


void   cbuildingcontrols :: cnetcontrol :: emptyeverything ( void )
{
   setnetmode ( cnet_moveenergyout   + cnet_movematerialout   + cnet_movefuelout +
                cnet_stopenergyinput + cnet_stopmaterialinput + cnet_stopfuelinput       , 1 );

   setnetmode ( cnet_stopenergyoutput + cnet_stopmaterialoutput + cnet_stopfueloutput +
                cnet_storeenergy      + cnet_storematerial      + cnet_storefuel         , 0 );
};

void    cbuildingcontrols :: cnetcontrol :: reset ( void )
{
   setnetmode ( 0, -1 );
};







cbuildingcontrols :: cproduceammunition :: cproduceammunition ( void )
{
   baseenergyusage    = 0;
   basematerialusage  = 0;
   basefuelusage      = 0;
}




int  cbuildingcontrols :: cproduceammunition :: checkavail ( int weaptype, int num )
{
   if ( cc_b->building->typ->special & cgammunitionproductionb ) {
      int needede = cwaffenproduktionskosten[weaptype][0] * num;
      int neededm = cwaffenproduktionskosten[weaptype][1] * num;
      int neededf = cwaffenproduktionskosten[weaptype][2] * num;

      int   availae = cc->getResource ( baseenergyusage   + needede, Resources::Energy, true ) - baseenergyusage;
      int   availam = cc->getResource ( basematerialusage + neededm, Resources::Material, true ) - basematerialusage;
      int   availaf = cc->getResource ( basefuelusage     + neededf, Resources::Fuel, true ) - basefuelusage;

      int   eperc;
      if ( needede )
         eperc = 100 * availae / needede;
      else
         eperc = 100;

      int   mperc;
      if ( neededm )
         mperc = 100 * availam / neededm;
      else
         mperc = 100;

      int   fperc;
      if ( neededf )
         fperc = 100 * availaf / neededf;
      else
         fperc = 100;


      int perc;
      if ( mperc < eperc )
         perc = mperc;
      else
         perc = eperc;

      if ( fperc < perc )
         perc = fperc;

      if ( perc < 0 )
         perc = 0;
      if ( perc > 100 )
         perc = 100;

      int prod = num * perc / 100;

      energyneeded   = cwaffenproduktionskosten[weaptype][0] * prod;
      materialneeded = cwaffenproduktionskosten[weaptype][1] * prod;
      fuelneeded     = cwaffenproduktionskosten[weaptype][2] * prod;

      return prod;
   } else {
      energyneeded   = 0;
      materialneeded = 0;
      fuelneeded     = 0;
      return 0;
   }
}


void cbuildingcontrols :: cproduceammunition :: produce ( int weaptype, int num )
{
   int n = checkavail( weaptype, num );
   cc->getResource ( Resources( energyneeded, materialneeded, fuelneeded ), false );
   cc->putammunition ( weaptype, n*weaponpackagesize, 1 );

   logtoreplayinfo ( rpl_produceAmmo, cc_b->building->getPosition().x, cc_b->building->getPosition().y, weaptype, n );
}






int   cbuildingcontrols :: cproduceunit :: available (pvehicletype fzt, int* lack )
{
   int l = 0;
   if ( actmap->player[ cc->getactplayer() ].research.vehicletypeavailable ( fzt ) ) {
      for ( int r = 0; r < resourceTypeNum; r++ )
         if ( cc->getResource( fzt->productionCost.resource(r), r, true ) < fzt->productionCost.resource(r) )
            l |= 1 << r;
   } else
      l |= 1 << 10;

   if ( cc->baseContainer->vehicleUnloadable(fzt) || (cc_b->building->typ->special & cgproduceAllUnitsB )) {
      if ( lack )
         *lack = l;

      if ( l == 0) {
         for ( int i = 0; i < 32; i++ )
            if ( !cc_b->building->loading[ i ] )
               return 1;
      }
   }

   return 0;
}



pvehicle cbuildingcontrols :: cproduceunit :: produce (pvehicletype fzt, bool forceRefill)
{
   pvehicle    eht;
   generatevehicle_cl ( fzt, cc->getactplayer() , eht, cc->getxpos(), cc->getypos() );

   /* ####TRANS
   for ( int h2 = 0; h2<8; h2++ )
      if ( eht->typ->height & ( 1 << h2 ))
         if ( cc_b->building->typ->loadcapability & ( 1 << h2))
            eht->height = 1 << h2;

   for ( int h1 = 0; h1<8; h1++ )
      if ( eht->typ->height & ( 1 << h1 ))
         if ( cc_b->building->typ->buildingheight & ( 1 << h1))
            eht->height = 1 << h1;

   eht->setMovement ( eht->typ->movement[log2( eht->height )]);

   */

   cc->getResource( Resources ( fzt->productionCost.energy, fzt->productionCost.material, 0 ), false );

//   logtoreplayinfo( rpl_productionResourceUsage, fzt->id, cc_b->building->getPosition().x, cc_b->building->getPosition().y );

   int i = 0;
   int n = 1;
   while ( n ) {
      if ( cc_b->building->loading[ i ] == NULL ) {
         cc_b->building->loading[ i ] = eht;
         n = 0;
      } else
         i++;
   } /* endwhile */

   Resources res;
   for ( int i = 0; i < fzt->weapons.count; ++i )
      if ( fzt->weapons.weapon[i].count )
         if ( fzt->weapons.weapon[i].getScalarWeaponType() >= 0 )
            for ( int r = 0; r < 3; ++r )
               res.resource(r) += cwaffenproduktionskosten[fzt->weapons.weapon[i].getScalarWeaponType()][r] * fzt->weapons.weapon[i].count;

   bool autoFill = false;
   if ( CGameOptions::Instance()->container.filleverything == 1 )
      autoFill = true;

   if ( CGameOptions::Instance()->container.filleverything == 2 )
      if ( res.material < fzt->productionCost.material/2 && res.energy < fzt->productionCost.energy/2 )
         autoFill = true;

   if ( autoFill || forceRefill )
      cc->refill.filleverything ( eht );

   return eht;
};

pvehicle cbuildingcontrols :: cproduceunit :: produce_hypothetically (pvehicletype fzt)
{
   pvehicle    eht;
   generatevehicle_cl ( fzt, cc->getactplayer() , eht, cc->getxpos (), cc->getypos () );


   return eht;
};




int   cbuildingcontrols :: ctrainunit :: available ( pvehicle eht )
{
   if ( actmap->getgameparameter( cgp_bi3_training ) )
      return 0;

   if ( eht->experience < actmap->getgameparameter ( cgp_maxtrainingexperience ) )
      if ( !eht->attacked ) {
         if (  cc->getspecfunc ( mbuilding ) & cgtrainingb ) {
            int num = 0;
            int numsh = 0;
            for (int i = 0; i < eht->typ->weapons.count; i++ )
               if ( eht->typ->weapons.weapon[i].shootable() )
                  if ( eht->ammo[i] )
                     numsh++;
                  else
                     num++;

            if ( num == 0  &&  numsh > 0 )
               return 1;
         }
      }

   return 0;
}



void  cbuildingcontrols :: ctrainunit :: trainunit ( pvehicle eht )
{
   if ( available ( eht ) ) {
      eht->experience+= actmap->getgameparameter( cgp_trainingIncrement );
      for (int i = 0; i < eht->typ->weapons.count; i++ )
         if ( eht->typ->weapons.weapon[i].shootable() )
            eht->ammo[i]--;

      if ( eht->experience > actmap->getgameparameter ( cgp_maxtrainingexperience ) )
         eht->experience = actmap->getgameparameter ( cgp_maxtrainingexperience );

      eht->attacked = 1;
      eht->setMovement ( 0 );
      logtoreplayinfo ( rpl_trainunit, cc->getxpos (), cc->getypos (), eht->experience, eht->networkid );

   }
};



int   cbuildingcontrols :: cdissectunit :: available ( pvehicle eht )
{
   if ( eht )
      if (  cc->getspecfunc ( mbuilding ) & cgresearchb )
         return !actmap->player[ cc->getactplayer() ].research.vehicletypeavailable ( eht->typ );

   return 0;
}

void   cbuildingcontrols :: cdissectunit :: dissectunit ( pvehicle eht )
{
   if ( available ( eht ) ) {
      cc->refill.emptyeverything ( eht );
      dissectvehicle ( eht );
      cc_b->removevehicle ( &eht );
   }
}



Resources cbuildingcontrols :: BuildProductionLine :: resourcesNeeded( Vehicletype* veh )
{
   return veh->productionCost * productionLineConstructionCostFactor;
}


int cbuildingcontrols :: BuildProductionLine :: build( Vehicletype* veh  )
{
   for ( int i = 0; i < 18; ++i ) {
      if ( !cc_b->building->production[i] ) {
         if ( cc_b->building->getResource( resourcesNeeded(veh), 1 ) < resourcesNeeded(veh))
            return -500;

         cc_b->building->getResource( resourcesNeeded(veh), 0 );
         cc_b->building->production[i] = veh;
         return 0;
      }
   }
   return -501;
}


Resources cbuildingcontrols :: RemoveProductionLine :: resourcesNeeded( Vehicletype* veh )
{
   return veh->productionCost * productionLineRemovalCostFactor;
}


int cbuildingcontrols :: RemoveProductionLine :: remove( Vehicletype* veh  )
{
   for ( int i = 0; i < 18; ++i ) {
      if ( cc_b->building->production[i] == veh ) {
         if ( cc_b->building->getResource( resourcesNeeded(veh), 1 ) < resourcesNeeded(veh))
            return -500;

         cc_b->building->getResource( resourcesNeeded(veh), 0 );
         cc_b->building->production[i] = NULL;
         return 0;
      }
   }
   return -502;
}


/***********************************************************************************/
/*                                                                                 */
/*   class    ctransportcontrols : Geb„ude-Innereien                                */
/*                                                                                 */
/***********************************************************************************/

ctransportcontrols :: ctransportcontrols (void)
{
   cc_t = this;
   cc_b = NULL;
}

void  ctransportcontrols :: init (pvehicle eht)
{
   vehicle = eht;
   baseContainer = eht;
};

int    ctransportcontrols :: getHeight ( void )
{
   return vehicle->height;
}


char  ctransportcontrols :: getactplayer (void)
{
   return   vehicle->color / 8;
};




int   ctransportcontrols :: putammunition ( int weapontype, int ammunition, int abbuchen)
{
   int ammo = ammunition;
   for ( int i = 0; i < vehicle->typ->weapons.count; i++ )
      if ( ammo )
         if ( vehicle->typ->weapons.weapon[i].getScalarWeaponType() == weapontype ) {
            int dif = vehicle->typ->weapons.weapon[i].count - vehicle->ammo[i];
            if ( dif > ammo )
               dif = ammo;
            if ( abbuchen )
               vehicle->ammo[i] += dif;
            ammo -= dif;
         }
   return ammunition - ammo;
};




int    ctransportcontrols :: getammunition ( int weapontype, int num, int abbuchen, int produceifrequired  )
{
   int ammo = 0;
   for ( int i = 0; i < vehicle->typ->weapons.count ; i++)
      if ( ammo < num )
         if ( vehicle->typ->weapons.weapon[i].getScalarWeaponType() == weapontype ) {
            int dif = num - ammo;
            if ( dif > vehicle->ammo[i] )
               dif = vehicle->ammo[i];

            if ( abbuchen )
               vehicle->ammo[i] -= dif;
            ammo += dif;
         }
   return ammo;
};


int    ctransportcontrols :: ammotypeavail ( int type )
{
   for ( int i = 0; i < vehicle->typ->weapons.count ; i++)
      if ( vehicle->typ->weapons.weapon[i].getScalarWeaponType() == type )
         return 1;
   return 0;
}



int   ctransportcontrols :: getxpos (void)
{
   return   vehicle->xpos;
};



int   ctransportcontrols :: getypos (void)
{
   return   vehicle->ypos;
};



int   ctransportcontrols :: getspecfunc ( tcontainermode mode )
{
   if ( mode == mtransport )
      return vehicle->typ->functions;
   else
      return 0;
};




void  ctransportcontrols :: removevehicle ( pvehicle *peht )
{
   for (int i=0; i<=31; i++) {
      if ( *peht == vehicle->loading[i] )
         vehicle->loading[i]=NULL;
   };
   logtoreplayinfo ( rpl_removeunit, vehicle->xpos, vehicle->ypos, (*peht)->networkid );
   delete *peht;
   *peht = NULL;
}










/***********************************************************************************/
/*                                                                                 */
/*   class    ccontainer : Geb„ude- / Transporter-Innereien                        */
/*                                                                                 */
/***********************************************************************************/

#define  eoh   55

ccontainer :: ccontainer (void)
{
   mousestat = 1;
   keymode = 0;

   pushallmouseprocs ( );

   // removemouseproc ( (void*) mousescrollproc );
   // npush ( mouseproc );
   // mouseproc = NULL;

   if ( mouseparams.pictpointer != icons.mousepointer )
      setnewmousepointer ( icons.mousepointer, 0,0 );


   allsubwindownum = 0;
   mark.x=0;
   mark.y=0;
   repaintresources = 1;
   repaintammo = 0;
   memset ( &picture[0], 0, sizeof ( picture ));
   memset ( &pictgray[0], 0, sizeof ( pictgray ));
   actsubwindow = NULL;
   inactivefield = icons.container.mark.inactive;
   activefield   = icons.container.mark.active;

   containeronlinemousehelp = new tcontaineronlinemousehelp ( this );

};





void   ccontainer :: registersubwindow ( psubwindow subwin )
{
   allsubwindows[ allsubwindownum ] = subwin;
   allsubwindownum ++;
}


ccontainer :: ~ccontainer (void)
{
   delete containeronlinemousehelp;

   allsubwindownum = 0;
   // addmouseproc ( (void*) mousescrollproc );

   popallmouseprocs ( );



   //     npop ( mouseproc );
}
;

void  ccontainer :: buildgraphics( void )
{

   getpicsize (icons.container.container_window, windowwidth, windowheight);
   collategraphicoperations cgo ( containerxpos, containerypos, containerxpos+windowwidth, containerypos+windowheight );
   putspriteimage ( containerxpos, containerypos, icons.container.container_window );


   activefontsettings.color = 15;
   activefontsettings.background = 201;
   activefontsettings.length = 80;
   activefontsettings.justify = centertext;
   activefontsettings.font = schriften.guifont;

   if ( !name1.empty() )
      showtext2c (name1.c_str(), nameposx, nameposy );

   if ( !name2.empty() )
      showtext2c (name2.c_str(), nameposx+112, nameposy );

   repaintresources = 1;
   showresources ();
   showammo();

   for (int i = 0; i < 3; i++)
      putrotspriteimage( tabmarkposx, tabmarkpos[i], icons.container.tabmark[i == keymode], actmap->actplayer*8 );

   for ( int x = 0; x < 6; x++ )
      for ( int y = 0; y < 3; y++ )
         putFieldImage(x,y,inactivefield);

   if ( actsubwindow )  {
      actsubwindow->display();
      actsubwindow->paintalllaschen( 1 );
   }

}


void  ccontainer :: init (void *pict, int col, const ASCString& name, const ASCString& descr)
{
   containerpicture = pict;
   name1 = name;
   name2 = descr;
   buildgraphics();
}

int ammoorderxlat[8] = { 2, 3, 4, 5, 6, 7, 0, 1 };

void  ccontainer :: showammo ( void )
{
   activefontsettings.background = 201;
   activefontsettings.length = 19;
   activefontsettings.justify = righttext;
   activefontsettings.font = schriften.guifont;
   collategraphicoperations cgo ( nameposx - 12 , nameposy + 59, nameposx - 12 + 8 * 28, nameposy + 75 );
   for (int i = 0; i < 8; i++)
      showtext2c ( strrr ( getammunition ( ammoorderxlat[i], maxint, 0 )), nameposx - 12 + i * 28, nameposy + 59 );

}

void  ccontainer :: showresources ( void )
{
   if ( repaintresources ) {
      activefontsettings.color = 15;
      activefontsettings.background = 201;
      activefontsettings.length = 29;
      activefontsettings.justify = righttext;
      activefontsettings.font = schriften.guifont;

      collategraphicoperations cgo ( nameposx + 18 , nameposy + 27, nameposx + 164 + activefontsettings.length, nameposy + 40 );
      char buf[1000];
      for ( int r = 0; r < 3; ++r )
         showtext2c ( int2string ( getResource ( maxint, r, true ), buf ), nameposx + 18 + r * (91-18), nameposy + 27 );

      repaintresources = 0;
   }
}

int    ccontainer :: getfieldundermouse ( int* x, int* y )
{
   for ( int i = 0; i < unitsshowny; i++ )
      for ( int j = 0; j < unitsshownx; j++ ) {
         int xp = unitposx[j];
         int yp = unitposy[i];
         if ( mouseparams.x >= xp && mouseparams.x < xp+fieldxsize && mouseparams.y >= yp && mouseparams.y < yp + fieldysize) {
            int k=0;

            int xd = mouseparams.x - xp;
            int yd = mouseparams.y - yp;
            Uint16* pw = (Uint16*) icons.fieldshape;
            unsigned char* pc = (unsigned char*) icons.fieldshape;
            pc+=4;

            if ( pw[0] >= xd  && pw[1] >= yd )
               if ( pc[ yd * ( pw[0] + 1) + xd] != 255 )
                  k++;

            if ( k ) {
               *x = j;
               *y = i;
               return 1;
            }
         }
      }
   return 0;
}

void   ccontainer :: checkformouse( void )
{
   if ( CGameOptions::Instance()->mouse.fieldmarkbutton )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.fieldmarkbutton ) {
         int i,j;
         if ( getfieldundermouse ( &i, &j ))
            if ( i != mark.x  ||  j != mark.y ) {
               putFieldImage ( mark.x, mark.y, inactivefield);
               displayloading ( mark.x, mark.y );
               mark.x = i;
               mark.y = j;
               putFieldImage ( mark.x, mark.y, activefield);
               displayloading ( mark.x, mark.y, 0, 1 );
               mousestat = 1;
            }
      }

   if ( CGameOptions::Instance()->mouse.smallguibutton )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.smallguibutton ) {
         int x;
         int y;
         int r = getfieldundermouse ( &x, &y );

         if ( r )
            if ( (mark.x != x || mark.y != y) ) { // && ( moveparams.movestatus == 0   ||  getfield(actmap->xpos + x , actmap->ypos + y)->temp == 0) ) {
               putFieldImage ( mark.x, mark.y, inactivefield);
               displayloading ( mark.x, mark.y );
               mark.x = x;
               mark.y = y;
               putFieldImage ( mark.x, mark.y, activefield);
               displayloading ( mark.x, mark.y, 0, 1 );
               mousestat = 1;
            } else
               if ( mousestat == 2 ) { //  ||  mousestat == 0 ||  (moveparams.movestatus && getfield( actmap->xpos + x, actmap->ypos + y)->temp )  ) {
                  if ( mark.x == x && mark.y == y ) {
                     int num = actgui->painticons();
                     actgui->paintsmallicons( CGameOptions::Instance()->mouse.smallguibutton, num <= 1 );
                  }

                  mousestat = 1;
               }
      } else
         if ( mousestat == 1 )
            mousestat = 2;

   dashboard.checkformouse( 1 );

}

void ccontainer :: putFieldImage( int x, int y, void* image )
{
   if ( y * 6 + x >= baseContainer->baseType->maxLoadableUnits )
      putspriteimage ( unitposx[x], unitposy[y], xlatpict ( xlatpictgraytable, image));
   else
      putspriteimage ( unitposx[x], unitposy[y], image);
}


void ccontainer :: unitchanged( void )
{}


void  ccontainer :: paintvehicleinfo ( void )
{
   dashboard.paintvehicleinfo ( getmarkedunit(), NULL, NULL, NULL );
}

void  ccontainer :: run ()
{
   end = 0;
   dashboard.x = 0xffff;
   mousevisible(true);
   do {
      if (keypress () ) {
         int keyprn;
         tkey input;
         getkeysyms( &input, &keyprn );
         containeronlinemousehelp->removehelp ();

         if ( input == ct_tab ) {
            {
               collategraphicoperations cgo ( tabmarkposx, tabmarkpos[keymode], tabmarkposx + 10, tabmarkpos[keymode] + 10 );
               putrotspriteimage( tabmarkposx, tabmarkpos[keymode], icons.container.tabmark[0], actmap->actplayer*8 );
            }

            keymode++;
            if ( keymode >= 3 )
               keymode = 0; {
               collategraphicoperations cgo ( tabmarkposx, tabmarkpos[keymode], tabmarkposx + 10, tabmarkpos[keymode] + 10 );
               putrotspriteimage( tabmarkposx, tabmarkpos[keymode], icons.container.tabmark[1], actmap->actplayer*8 );
            }
         }

         if ( keymode == 2 )
            if ( actsubwindow ) {
               actsubwindow->checklaschkey( input );
            }

         if ( actsubwindow && keymode == 1)
            if ( actsubwindow )
               actsubwindow-> checkforkey ( input );

         /*        if ( input == ct_esc )
                    if ( unitmode == mnormal )
                       end=1; */

         if ( keymode == 0 ) {
            actgui->checkforkey (input, keyprn);

            if ( input==ct_up || input==ct_8k)    movemark(cursor_up);
            if ( input==ct_down || input==ct_2k)  movemark(cursor_down);
            if ( input==ct_left || input==ct_4k)  movemark(cursor_left);
            if ( input==ct_right || input==ct_6k) movemark(cursor_right);

         }
         if ( input == ct_0 )  {
            char* nm = getnextfilenumname ( "screen", "pcx", 0 );
            writepcx ( nm, 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, pal );
            displaymessage2( "screen saved to %s", nm );
         }

      }

      if ( repaintresources ) {
         showresources();
         if ( actsubwindow )
            actsubwindow-> resetresources( 0 );

         repaintresources = 0;
      }

      if ( repaintammo ) {
         showammo ();
         if ( actsubwindow )
            actsubwindow-> resetammo ( 0 );

         repaintammo = 0;
      }


      actgui->checkformouse ();
      if ( actsubwindow )
         actsubwindow-> mousecheck ( 0 );

      checkformouse();

      if ((dashboard.x != mark.x ) || ( dashboard.y != mark.y )) {
         paintvehicleinfo ( );
         dashboard.x = mark.x ;
         dashboard.y = mark.y ;
         actgui->painticons();
         if ( actsubwindow )
            actsubwindow->unitchanged();
         unitchanged();
      }

      containeronlinemousehelp->checkforhelp();
      releasetimeslice();
   } while (end == 0);
}


void  ccontainer :: done ()
{
   if ( recursiondepth == 0 )
      displaymap ();

   actgui->restorebackground();
}




void  ccontainer :: movemark (int direction)
{
   collategraphicoperations cgo ( containerxpos, containerypos, containerxpos+windowwidth, containerypos+windowheight );
   if (direction != repaint  &&  direction != repaintall ) {

      putFieldImage ( mark.x, mark.y, inactivefield);
      displayloading ( mark.x, mark.y );

   } else {
      if ( direction == repaintall ) {
         setpictures();

         for ( int y = 0; y < unitsshowny; y++ )
            for ( int x = 0; x < unitsshownx; x++ )
               putFieldImage ( x, y, inactivefield);
         displayloading();
      }
   }


   switch (direction) {
      case cursor_up :
         mark.y--;
         if (mark.y < 0) mark.y=unitsshowny-1;
         break;
      case cursor_down :
         mark.y++;
         if (mark.y >= unitsshowny ) mark.y=0;
         break;
      case cursor_left :
         mark.x--;
         if (mark.x < 0) mark.x=unitsshownx-1;
         break;
      case cursor_right :
         mark.x++;
         if (mark.x >= unitsshownx ) mark.x=0;
         break;
   };


   putFieldImage ( mark.x, mark.y, activefield);
   displayloading ( mark.x, mark.y, 0, 1 );

   cgo.off();

   actgui->painticons();

};

pvehicletype ccontainer :: getmarkedunittype ( void )
{
   return NULL;
}

void  ccontainer :: setpictures ( void )
{
   if ( unitmode == mnormal ) {
      inactivefield = icons.container.mark.inactive;
      activefield   = icons.container.mark.active;
   } else
      if ( unitmode == mloadintocontainer ) {
         inactivefield = icons.container.mark.movein_inactive;
         activefield   = icons.container.mark.movein_active;
      }

   for ( int i = 0; i < 32; i++ ) {
      pvehicle unit = getloadedunit ( i );
      if ( unit ) {
         picture[i] = unit->typ->picture[0] ;
         if ( unit->getMovement() > 0 )
            pictgray[i] = 0;
         else
            pictgray[i] = 1;
      } else
         picture[i] = NULL;
   }
}


void  ccontainer :: displayloading ( int x, int y, int dx, int dy )
{
   void* pict = picture [ x+y*unitsshownx ];
   if ( pict ) {
      int w,h;
      getpicsize ( pict, w, h);
      collategraphicoperations cgo ( unitposx[x] - dx, unitposy[y] - dy, unitposx[x] - dx + w, unitposy[y] - dy + h );
      if ( pictgray[ x+y*unitsshownx ] )
         putspriteimage (unitposx[x] - dx, unitposy[y] - dy, xlatpict(xlatpictgraytable, pict )  );
      else
         putrotspriteimage (unitposx[x] - dx, unitposy[y] - dy, pict, getactplayer()*8 );
   }
}

void  ccontainer :: displayloading (void)
{
   collategraphicoperations cgo ( unitposx[0], unitposy[0], unitposx[unitsshownx-1]+fieldsizex, unitposy[unitsshowny-1] + fieldsizey );
   for (int x=0; x < unitsshownx; x++)
      for (int y=0; y < unitsshowny ; y++)
         displayloading ( x, y );
}



void    ccontainer ::  setactunittogray ( void )
{
   pictgray[mark.x + mark.y * unitsshownx] = 1;
   movemark ( repaint );
}




// -------------------------------- ----------------------------------------------------------



ccontainer :: cammunitiontransfer_subwindow :: cammunitiontransfer_subwindow ( void )
{
   strcpy ( name, "ammunition transfer" );
   laschpic1 = icons.container.lasche.a.ammotransfer[0];
   laschpic2 = icons.container.lasche.a.ammotransfer[1];
   eht = NULL;
   txtptr = "external";
   num = 0;

   for (int i = 0; i < waffenanzahl; i++) {
      objcoordinates[i].x1 = subwinx1 + 48  + 40 * i ;
      objcoordinates[i].x2 = subwinx1 + 58  + 40 * i ;
      objcoordinates[i].y1 = subwiny1 + 41;
      objcoordinates[i].y2 = subwiny1 + 90;
      objcoordinates[i].type = 3;
      objcoordinates[i].t1 = 0;
      objcoordinates[i].t2 = 8;
   } /* endfor */
   for ( int z = 0; z < 20; z++ )
      schiebpos[z] = 0;

   objcoordinates[19].x1 = subwinx1 + 217;
   objcoordinates[19].x2 = subwinx1 + 303;
   objcoordinates[19].y1 = subwiny1 + 4;
   objcoordinates[19].y2 = subwiny1 + 14;
   objcoordinates[19].type = 5;


   if ( dataVersion < 2 ) {
      objcoordinates[20].x1 = subwinx1 + 193;
      objcoordinates[20].x2 = subwinx1 + 215;
      objcoordinates[20].y1 = subwiny1 + 4;
      objcoordinates[20].y2 = subwiny1 + 14;
      objcoordinates[20].type = 6;

      objcoordinates[21].x1 = subwinx1 + 305;
      objcoordinates[21].x2 = subwinx1 + 327;
      objcoordinates[21].y1 = subwiny1 + 4;
      objcoordinates[21].y2 = subwiny1 + 14;
      objcoordinates[21].type = 6;
   } else {
      objcoordinates[20].x1 = subwinx1 + 192;
      objcoordinates[20].x2 = subwinx1 + 216;
      objcoordinates[20].y1 = subwiny1 + 3;
      objcoordinates[20].y2 = subwiny1 + 14;
      objcoordinates[20].type = 6;

      objcoordinates[21].x1 = subwinx1 + 306;
      objcoordinates[21].x2 = subwinx1 + 327;
      objcoordinates[21].y1 = subwiny1 + 3;
      objcoordinates[21].y2 = subwiny1 + 14;
      objcoordinates[21].type = 6;
   }
   actschieber = 0 ;
   externalloadingactive = 0;
   page = 0;
   pagenum = 0;
}

int  ccontainer :: cammunitiontransfer_subwindow :: subwin_available ( void )
{
   csubwindow :: subwin_available ( );
   if ( next )
      next->subwin_available ();

   return 0;
}

int  ccontainer :: cammunitiontransfer_subwindow :: externalloadavailable ( void )
{
   return 0;
}

void  ccontainer :: cammunitiontransfer_subwindow :: paintobj ( int numm, int stat )
{
   csubwindow :: paintobj ( numm, stat );

   if ( objcoordinates[numm].type == 3 ) {
      int nnumm = numm + page * 8;
      collategraphicoperations cgo  ( subwinx1 + 31 + numm * 40,   subwiny1 + 25,   subwinx1 + 31 + numm * 40 + 30,   subwiny1 + 96 + 10 );
      if ( nnumm < num ) {
         putimage ( objcoordinates[numm].x1-1,   objcoordinates[numm].y1-1,  icons.container.subwin.ammotransfer.schiene );

         int offs = 0;
         if ( numm == actschieber )
            offs = 1;

         if ( objcoordinates[numm].y2 - schiebpos[nnumm] - objcoordinates[numm].t2 == objcoordinates[numm].y1 )
            putimage ( objcoordinates[numm].x1,   objcoordinates[numm].y2 - schiebpos[nnumm] - objcoordinates[numm].t2,  icons.container.subwin.ammotransfer.schieber[offs + 2] );
         else
            putimage ( objcoordinates[numm].x1,   objcoordinates[numm].y2 - schiebpos[nnumm] - objcoordinates[numm].t2,  icons.container.subwin.ammotransfer.schieber[offs] );

         activefontsettings.color = white;
         activefontsettings.font = schriften.guifont;
         activefontsettings.length = 29;
         activefontsettings.justify = centertext;
         activefontsettings.background = bkgrcol;
         showtext2c ( strrr ( weaps[nnumm].actnum  ), subwinx1 + 31 + numm * 40, subwiny1 + 25 );
         showtext2c ( strrr ( weaps[nnumm].buildnum + weaps[nnumm].orgnum - weaps[nnumm].actnum  ), subwinx1 + 31 + numm * 40, subwiny1 + 96 );
      } else {
         putimage ( objcoordinates[numm].x1-1,   objcoordinates[numm].y1-1,  icons.container.subwin.ammotransfer.schieneinactive );
         bar ( subwinx1 + 31 + numm * 40, subwiny1 + 25, subwinx1 + 61 + numm * 40, subwiny1 + 35, bkgrcol );
         bar ( subwinx1 + 31 + numm * 40, subwiny1 + 96, subwinx1 + 61 + numm * 40, subwiny1 +106, bkgrcol );
         bar ( subwinx1 + 31 + numm * 40, subwiny1 + 38, subwinx1 + 45 + numm * 40, subwiny1 + 93, bkgrcol );
      }
   }
   if ( objcoordinates[numm].type == 5  && externalloadavailable() ) {
      collategraphicoperations cgo  ( objcoordinates[numm].x1,   objcoordinates[numm].y1,   objcoordinates[numm].x2+10,   objcoordinates[numm].y2 );
      if ( externalloadingactive )
         activefontsettings.font = schriften.guicolfont;
      else
         activefontsettings.font = schriften.guifont;
      activefontsettings.background = 255;
      activefontsettings.length = objcoordinates[numm].x2 - objcoordinates[numm].x1;
      activefontsettings.height = 0;
      activefontsettings.justify = centertext;
      if ( stat == 0 ) {
         putimage ( objcoordinates[numm].x1, objcoordinates[numm].y1, icons.container.subwin.ammotransfer.button );
         showtext2c ( txtptr, objcoordinates[numm].x1 + 1, objcoordinates[numm].y1 + 1 );
      } else {
         putimage ( objcoordinates[numm].x1, objcoordinates[numm].y1, icons.container.subwin.ammotransfer.buttonpressed );
         showtext2c ( txtptr, objcoordinates[numm].x1 + 2, objcoordinates[numm].y1 + 2 );
      }
   }
   if ( objcoordinates[numm].type == 6 ) {
      collategraphicoperations cgo  ( objcoordinates[numm].x1,   objcoordinates[numm].y1,   objcoordinates[numm].x2+10,   objcoordinates[numm].y2 );
      if ( dataVersion < 2 ) {
         activefontsettings.font = schriften.guifont;
         activefontsettings.background = 255;
         activefontsettings.length = objcoordinates[numm].x2 - objcoordinates[numm].x1;
         activefontsettings.height = 0;
         activefontsettings.justify = centertext;
         char* text;
         if ( numm == 20 )
            text = "-";
         else
            if ( numm == 21 )
               text = "+";

         if ( stat == 0 ) {
            bar ( objcoordinates[numm].x1, objcoordinates[numm].y1, objcoordinates[numm].x2, objcoordinates[numm].y2, bkgrcol);
            showtext2c ( text, objcoordinates[numm].x1 + 1, objcoordinates[numm].y1 + 1 );
         } else {
            bar ( objcoordinates[numm].x1, objcoordinates[numm].y1, objcoordinates[numm].x2, objcoordinates[numm].y2, bkgrdarkcol);
            showtext2c ( text, objcoordinates[numm].x1 + 2, objcoordinates[numm].y1 + 2 );
         }
      } else {
         if ( pagenum == 0 ) {
            if ( numm == 20 )
               putimage ( objcoordinates[numm].x1, objcoordinates[numm].y1, icons.container.subwin.ammotransfer.singlepage[0] );
            else
               putimage ( objcoordinates[numm].x1, objcoordinates[numm].y1, icons.container.subwin.ammotransfer.singlepage[1] );
         } else {
            if ( numm == 20 )
               putimage ( objcoordinates[numm].x1, objcoordinates[numm].y1, icons.container.subwin.ammotransfer.minus[stat] );
            else
               putimage ( objcoordinates[numm].x1, objcoordinates[numm].y1, icons.container.subwin.ammotransfer.plus[stat] );
         }
      }
   }

}

void  ccontainer :: cammunitiontransfer_subwindow :: reset ( pvehicle veh )
{
   int i;

   num = 0;
   if ( veh )
      eht = veh;
   else {
      if ( !externalloadingactive )
         eht = hostcontainer->getmarkedunit();
   }
   if ( eht ) {
      for (i = 0; i < eht->typ->weapons.count; i++) {
         if ( eht->typ->weapons.weapon[i].requiresAmmo() ) {
            int typ = eht->typ->weapons.weapon[i].getScalarWeaponType() ;
            if ( cc->ammotypeavail ( typ )) {
               weaps[num].name = cwaffentypen [ typ ];
               weaps[num].maxnum = eht->typ->weapons.weapon[i].count;
               weaps[num].orgnum = eht->ammo[i];
               weaps[num].actnum = weaps[num].orgnum;
               weaps[num].buildnum = cc->getammunition ( typ, maxint, 0 );
               weaps[num].pos = i;
               weaps[num].type = typ;
               num++;
            }
         }
      } /* endfor */
      if ( eht->typ->tank.material ) {
         weaps[num].name = resourceNames [ 1 ];
         weaps[num].maxnum = eht->typ->tank.material;
         weaps[num].orgnum = eht->getTank().material;
         weaps[num].actnum = weaps[num].orgnum;
         weaps[num].buildnum = cc->getResource ( maxint, Resources::Material, true );
         weaps[num].pos  = 101;
         weaps[num].type = 101;
         num++;
      }
      if ( eht->typ->tank.fuel ) {
         weaps[num].name = resourceNames [ 2 ];
         weaps[num].maxnum = eht->typ->tank.fuel;
         weaps[num].orgnum = eht->getTank().fuel;
         weaps[num].actnum = weaps[num].orgnum;
         weaps[num].buildnum = cc->getResource ( maxint, Resources::Fuel, true );
         weaps[num].pos  = 102;
         weaps[num].type = 102;
         num++;
      }
   }
   pagenum = (num-1) / 8;
   if ( page > pagenum )
      page = 0;

   if ( page < 0 )
      page = 0;

  for ( i = 0; i < num; i++ ) {
      check(i);
      objcoordinates[i].type = 3;
   }

}


int   ccontainer :: cammunitiontransfer_subwindow :: gpres ( int i )
{
   int n;
   int diff = weaps[i].actnum - weaps[i].orgnum;
   if ( weaps[i].pos < 100 ) {
      if ( diff > 0 )
         n = cc->getammunition ( weaps[i].type, diff, 0 );
      else
         n = -cc->putammunition ( weaps[i].type, -diff, 0 );
   } else
      if ( weaps[i].pos == 101 ) {
         n = cc->getResource ( diff, Resources::Material, true );
      } else
         if ( weaps[i].pos == 102 ) {
            n = cc->getResource ( diff, Resources::Fuel, true  );

            // hostcontainer->setpictures();
            // hostcontainer->displayloading();
         };

   return n;
}

void  ccontainer :: cammunitiontransfer_subwindow :: check ( int i )
{
   weaps[i].actnum = weaps[i].orgnum + gpres(i);
   int ii = i - page*8;
   if ( ii >= 0 && ii < 8 ) {
      int length = objcoordinates[ii].y2 - objcoordinates[ii].y1 - objcoordinates[ii].t2;
      if ( weaps[i].maxnum )
         schiebpos[i] = length * weaps[i].actnum / weaps[i].maxnum;
      else
         schiebpos[i] = 0;
   }
}


void  ccontainer :: cammunitiontransfer_subwindow :: display ( void )
{
   if ( hostcontainer->getmarkedunit() != eht )
      reset();

   collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );
   npush ( activefontsettings );
   putimage ( subwinx1, subwiny1, icons.container.subwin.ammotransfer.start );

   csubwindow :: display ();

   displayvariables();

   paintobj ( 19, 0 );
   paintobj ( 20, 0 );
   paintobj ( 21, 0 );


   npop ( activefontsettings );
}

void  ccontainer :: cammunitiontransfer_subwindow :: displayvariables ( void )
{
   paintobj ( 20, 0 );
   paintobj ( 21, 0 );

   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.justify = lefttext;
   for (int i = 0; i < 8; i++) {

      if ( i+page*8 < num ) {
         char* buf;
         paintobj ( i, 0 ); {
            tvirtualdisplay vdp ( 100, 100, bkgrcol );

            int x1 = 0;
            int y1 = 0;
            int x2 = 55;
            int y2 = 14;

            buf = new char [ imagesize ( x1, y1, x2, y2 ) ];

            activefontsettings.length = 0;
            activefontsettings.justify = lefttext;
            activefontsettings.background = 255;
            showtext2c ( weaps[i+page*8].name, 10, 10 );
            getimage ( 10, 10, 10 + x2, 10 + y2, buf );
         }
         putrotspriteimage90 ( subwinx1 + 31 + 40 * i , subwiny1 + 38, buf, 0 );

         delete[] buf;
      } else
         paintobj ( i, 0 );
   }

}

void  ccontainer :: cammunitiontransfer_subwindow :: transfer ( void )
{
   for ( int i = 0; i < num; i++ ) {
      if ( weaps[i].pos < 100 )
         ammunition ( eht, weaps[i].pos, weaps[i].actnum );

      if ( weaps[i].pos == 101 )
         resource ( eht, Resources::Material, weaps[i].actnum );

      if ( weaps[i].pos == 102 ) {
         resource ( eht, Resources::Fuel, weaps[i].actnum );
         hostcontainer->setpictures();
         hostcontainer->displayloading();
      }
   }
   dashboard.x = 0xffff;
}

void  ccontainer :: cammunitiontransfer_subwindow :: unitchanged ( void )
{
   if ( !externalloadingactive ) {
      if ( hostcontainer->getmarkedunit() != eht )
         reset();
      // display();
      displayvariables();
   }
}


void  ccontainer :: cammunitiontransfer_subwindow :: execexternalload ( void )
{}


void  ccontainer :: cammunitiontransfer_subwindow :: checkformouse ( void )
{
   if ( mouseparams.taste == 1 ) {
      for (int i = page*8; i < num && i < (page+1)*8; i++) {
         int ii = i - page*8;
         if ( mouseparams.x >= objcoordinates[ii].x1    && mouseparams.x <= objcoordinates[ii].x2  &&
               mouseparams.y >= objcoordinates[ii].y1    && mouseparams.y <= objcoordinates[ii].y2 ) {

            int repnt = 0;
            int tp = weaps[i].actnum;
            if ( ii != actschieber ) {
               int old = actschieber;
               actschieber = ii;
               paintobj ( old, 0 );
               repnt = 1;
            }

            int relpos = objcoordinates[ii].y2 -  mouseparams.y  /* - objcoordinates[ii].t2 / 2 */ ;
            if ( relpos < 0 )
               relpos = 0;

            int maxlen = ( objcoordinates[ii].y2 - objcoordinates[ii].y1 - objcoordinates[ii].t2 + 1 );
            if ( relpos > maxlen )
               relpos = maxlen;

            if ( relpos != schiebpos[i] ) {
               int n = relpos * weaps[i].maxnum / maxlen;
               int oldnum = weaps[i].actnum;
               if ( n != oldnum ) {
                  weaps[i].actnum = n;
                  check ( i );
                  if ( weaps[i].actnum != oldnum ) {
                     paintobj ( ii, 0 );
                     transfer();
                  }
               }

            }

            if (  tp == weaps[i].actnum && repnt )
               paintobj ( ii, 0 );


         }
      } /* endfor */

      if ( externalloadavailable () && objpressedbymouse ( 19 ) )
         execexternalload();

      if ( objpressedbymouse ( 20 ) && page > 0 ) {
         page--;
         // display();
         displayvariables();
      }

      if ( objpressedbymouse ( 21 ) && page < pagenum ) {
         page++;
         actschieber = 0;
         // display();
         displayvariables();
      }

   }
}

void  ccontainer :: cammunitiontransfer_subwindow :: checkforkey ( tkey taste )
{
   if ( num ) {
      int snum;
      if ( num < 8 )
         snum = num;
      else {
         if ( page == num/8 )
            snum = num%8;
         else
            snum = 8;
      }

      if ( taste == ct_right  || taste==ct_6k ) {
         int olds = actschieber;
         if ( actschieber+1 < snum  )
            actschieber++;
         else
            actschieber = 0;

         paintobj ( olds, 0 );
         paintobj ( actschieber, 0 );
      }
      if ( taste == ct_left || taste == ct_4k ) {
         int olds = actschieber;
         if ( actschieber > 0 )
            actschieber--;
         else
            actschieber = snum-1;

         paintobj ( olds, 0 );
         paintobj ( actschieber, 0 );
      }

      if ( taste == ct_up  ||  taste == ct_down || taste==ct_8k  || taste==ct_2k ) {
         int st = getstepwidth ( weaps[actschieber+page*8].maxnum );
         int n ;
         if ( taste == ct_up  || taste==ct_8k ) {
            if ( weaps[actschieber+page*8].actnum + st <= weaps[actschieber+page*8].maxnum  )
               n = weaps[actschieber+page*8].actnum + st;
            else
               n = weaps[actschieber+page*8].maxnum;
         }
         if ( taste == ct_down  || taste==ct_2k ) {
            if ( weaps[actschieber+page*8].actnum > st   )
               n = weaps[actschieber+page*8].actnum - st;
            else
               n = 0;
         }
         int oldnum = weaps[actschieber+page*8].actnum;
         weaps[actschieber+page*8].actnum = n;
         check ( actschieber+page*8 );
         if ( weaps[actschieber+page*8].actnum != oldnum ) {
            paintobj ( actschieber, 0 );
            transfer();
         }

      }
   }
}

void ccontainer :: cammunitiontransfer_subwindow :: resetammo ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetammo(1);
   else {
      reset ();
      if ( hostcontainer->actsubwindow == this )
         displayvariables();

      if ( nextavailable )
         nextavailable->resetammo ( mode );
   }
}

void ccontainer :: cammunitiontransfer_subwindow :: resetresources ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetresources(1);
   else {
      reset ();
      if ( hostcontainer->actsubwindow == this )
         displayvariables();

      if ( nextavailable )
         nextavailable->resetresources ( mode );
   }
}




int ccontainer_b :: cammunitiontransferb_subwindow :: externalloadavailable ( void )
{
   return cc_b->building->typ->special & (cgexternalloadingb | cgexternalresourceloadingb | cgexternalammoloadingb );
}


void ccontainer_b :: cammunitiontransferb_subwindow :: execexternalload ( void )
{
   if ( !externalloadingactive ) {
      int f = searchexternaltransferfields ( cc_b->building );
      if ( f ) {
         int ms = getmousestatus();
         if (ms == 2)
            mousevisible(false);

         actgui->restorebackground();
         npush (actgui);
         actgui = hostcontainer->oldguihost;

         int cursorx = cursor.posx;
         int cursory = cursor.posy;

         displaymap ();
         dashboard.x = 0xffff;
         mousevisible( true );
         addmouseproc ( &mousescrollproc );

         do {

            tkey input;
            if (keypress ()) {
               mainloopgeneralkeycheck (input);
            }

            mainloopgeneralmousecheck ();

         } while ( moveparams.movestatus==130 ) ;
         removemouseproc ( &mousescrollproc );
         if ( mouseparams.pictpointer != icons.mousepointer )
            setnewmousepointer ( icons.mousepointer, 0,0 );

         mousevisible ( false );
         pvehicle markedvehicle = NULL;
         if ( moveparams.movestatus == 131 ) {
            markedvehicle = getactfield()->vehicle ;
            externalloadingactive = 1;
         }
         moveparams.movestatus = 0;

         cursor.hide ();
         cursor.posx = cursorx;
         cursor.posy = cursory;
         cursor.show ();
         actmap->cleartemps(7);
         hostcontainer->buildgraphics();
         hostcontainer->displayloading ();
         hostcontainer->movemark (repaint);
         actgui->restorebackground();
         npop (actgui);
         actgui->restorebackground();
         dashboard.x = 0xffff;

         if (ms == 2)
            mousevisible(true);

         reset ( markedvehicle );
         displayvariables();
         actmap->cleartemps ( 7 );
      } else
         dispmessage2 ( 401, NULL );
   } else {
      externalloadingactive = 0;
      paintobj ( 19, 0 );
      unitchanged();
   }
}

// -------------------------------- ----------------------------------------------------------


void  hosticons_c :: seticonmains ( pcontainer maintemp )
{
   pgeneralicon_c t = (pgeneralicon_c) getfirsticon ();
   t->setmain ( maintemp );
}








ccontainer :: moveicon_c :: moveicon_c ( void )
{
   filename = "movement";
};

int   ccontainer :: moveicon_c :: available    ( void )
{
   if ( main->unitmode != mnormal )
      return 0;

   pvehicle eht = main->getmarkedunit();

   if ( eht && eht->color == actmap->actplayer * 8 )
      return eht->canMove();

   return 0;
}

void  ccontainer :: moveicon_c :: exec         ( void )
{
   bool simpleMode = false;
   if (  skeypress( ct_lshift ) ||  skeypress ( ct_rshift ))
      simpleMode = true;

   VehicleMovement* vehicleMovement = main->movement ( main->getmarkedunit(), simpleMode );
   if ( vehicleMovement ) {
      int ms = getmousestatus();
      if (ms == 2)
         mousevisible(false);

      actgui->restorebackground();
      npush (actgui);
      actgui = main->oldguihost;

      int cursorx = cursor.posx;
      int cursory = cursor.posy;

      vehicleMovement->registerPVA ( vat_move, &pendingVehicleActions );
      for ( int i = 0; i < vehicleMovement->reachableFields.getFieldNum(); i++ )
         vehicleMovement->reachableFields.getField( i ) ->a.temp = 1;

      //if ( !CGameOptions::Instance()->dontMarkFieldsNotAccessible_movement )
      for ( int j = 0; j < vehicleMovement->reachableFieldsIndirect.getFieldNum(); j++ )
         vehicleMovement->reachableFieldsIndirect.getField( j ) ->a.temp2 = 2;

      displaymap();

      dashboard.x = 0xffff;
      mousevisible( true );
      addmouseproc ( &mousescrollproc );

      do {

         tkey input;
         if (keypress ()) {
            mainloopgeneralkeycheck (input);
         }

         mainloopgeneralmousecheck ();
         releasetimeslice();

      } while ( pendingVehicleActions.actionType == vat_move ) ;
      removemouseproc ( &mousescrollproc );
      if ( mouseparams.pictpointer != icons.mousepointer )
         setnewmousepointer ( icons.mousepointer, 0,0 );

      mousevisible ( false );
      cursor.hide ();
      cursor.posx = cursorx;
      cursor.posy = cursory;
      cursor.show ();
      actmap->cleartemps(7);
      main->buildgraphics();
      main->displayloading ();
      main->movemark (repaint);
      npop (actgui);
      actgui->restorebackground();
      dashboard.x = 0xffff;

      if ( pendingVehicleActions.move )
         delete pendingVehicleActions.move;

      if (ms == 2)
         mousevisible(true);
   } else
      dispmessage2 ( 107, NULL );

}



ccontainer :: repairicon_c :: repairicon_c ( void )
{
   filename = "repair";
};

int   ccontainer :: repairicon_c :: available    ( void )
{
   if ( main->unitmode != mnormal )
      return 0;

   pvehicle eht = main->getmarkedunit();
   if ( eht && eht->color == actmap->actplayer * 8)
      if ( eht->damage > 0 )
         return cc->baseContainer->canRepair( eht );

   return 0;
}

void  ccontainer :: repairicon_c :: exec         ( void )
{
   cc->baseContainer->repairItem ( main->getmarkedunit() , 0 );
   Vehicle* v = dynamic_cast<Vehicle*>(cc->baseContainer);
   if ( v )
      logtoreplayinfo ( rpl_repairUnit, v->networkid, main->getmarkedunit()->networkid, 0, v->getTank().material, v->getTank().fuel );
   Building* b = dynamic_cast<Building*>(cc->baseContainer);
   if ( b )
      logtoreplayinfo ( rpl_repairUnit2, b->getPosition().x, b->getPosition().y, main->getmarkedunit()->networkid, 0 );
   dashboard.x = 0xffff;
   main->repaintresources = 1;
}

const char* ccontainer :: repairicon_c :: getinfotext  ( void )
{
   Resources cost;
   static char buf[200];
   cc->baseContainer->getMaxRepair ( main->getmarkedunit() , 0, cost );
   strcpy ( buf, "re~p~air" );
   sprintf ( &buf[strlen(buf)], resourceusagestring, cost.energy, cost.material, cost.fuel );
   return buf;
}



ccontainer :: fill_dialog_icon_c :: fill_dialog_icon_c ( void )
{
   filename = "refueld";
};

int   ccontainer :: fill_dialog_icon_c :: available    ( void )
{
   if ( main->unitmode != mnormal )
      return 0;

   pvehicle eht = main->getmarkedunit();
   if ( eht && eht->color == actmap->actplayer * 8)
      return 1;

   return 0;
}




ccontainer :: fill_icon_c :: fill_icon_c ( void )
{
   filename =  "refuel";
};

void  ccontainer :: fill_icon_c :: exec         ( void )
{
   filleverything ( main->getmarkedunit() );
   main->setpictures();
   main->displayloading();
   main->repaintresources = 1;
   dashboard.x = 0xffff;
}




ccontainer :: exit_icon_c :: exit_icon_c ( void )
{
   filename = "exit";
};

void  ccontainer :: exit_icon_c :: exec         ( void )
{
   main->end = 1;
}


int   ccontainer :: exit_icon_c :: available    ( void )
{
   return   main->unitmode == mnormal ;
}



ccontainer :: container_icon_c :: container_icon_c ( void )
{
   filename = "loadinga";
};

void  ccontainer :: container_icon_c :: exec         ( void )
{
   pvehicle eht = main->getmarkedunit();
   container ( eht, NULL );
   main->buildgraphics();
   main->displayloading ();
   main->movemark (repaint);
   main->paintvehicleinfo();
}


int   ccontainer :: container_icon_c :: available    ( void )
{
   if ( main->unitmode != mnormal )
      return 0;

   pvehicle eht = main->getmarkedunit();
   if ( eht && eht->color == actmap->actplayer * 8)
      if ( eht->typ->maxLoadableUnits > 0 )
         if ( recursiondepth +1 < maxrecursiondepth )
            return 1;

   return 0;
}




int ccontainer :: cmoveup_icon_c :: available ( void )
{
   if ( main->unitmode != mnormal )
      return 0;

   return moveupavail ( main->getmarkedunit () );
}

void  ccontainer :: cmoveup_icon_c :: exec ( void )
{
   moveup( main->getmarkedunit () );
   main->displayloading ();
   main->movemark (repaint);
}

ccontainer :: cmoveup_icon_c :: cmoveup_icon_c ( void )
{
   filename = "contnup" ;
}



int ccontainer :: cunitinformation_icon :: available ( void )
{
   if ( main->getmarkedunittype() || main->getmarkedunit() )
      return 1;
   return 0;
}

void  ccontainer :: cunitinformation_icon :: exec ( void )
{
   if ( main->getmarkedunittype() )
      vehicle_information ( main->getmarkedunittype() );
   else
      if ( main->getmarkedunit() )
         vehicle_information ( main->getmarkedunit()->typ );
}

ccontainer :: cunitinformation_icon :: cunitinformation_icon ( void )
{
   filename = "informat";
}


int ccontainer :: cmovedown_icon_c :: available ( void )
{
   if ( main->unitmode == mnormal ) {
      for ( int i = 0; i < maxloadableunits; i++ ) {
         pvehicle eht = main->getloadedunit ( i );
         if ( eht )
            if ( eht != main->getmarkedunit ())
               if ( movedownavail ( main->getmarkedunit () , eht ))
                  return 1;
      }
   } else if ( main->unitmode == mloadintocontainer ) {
      if ( movedownavail ( unittomove, main->getmarkedunit()))
         return 1;
   }
   return 0;
}

void  ccontainer :: cmovedown_icon_c :: exec ( void )
{
   if ( main->unitmode == mnormal ) {
      unittomove = main->getmarkedunit();
      dashboard.x = 0xffff;
      main->unitmode = mloadintocontainer;
      main->setpictures();
      main->movemark ( repaintall );
   } else if ( main->unitmode == mloadintocontainer ) {
      movedown( unittomove, main->getmarkedunit());
      dashboard.x = 0xffff;
      main->unitmode = mnormal;
      main->setpictures();
      main->movemark ( repaintall );
   }
}

ccontainer :: cmovedown_icon_c :: cmovedown_icon_c ( void )
{
   filename = "contndwn";
}













csubwindow :: csubwindow ( void )
{
   first = &buildingparamstack[recursiondepth].csubwindow_first;
   firstavailable = &buildingparamstack[recursiondepth].csubwindow_firstavailable;

   *firstavailable = NULL;
   lastclickpos = 0;

   next = *first;
   *first = this;

   hostcontainer = NULL;
   laschcol = 22 + actmap->actplayer * 8;
   objnum = 0;
}


csubwindow :: ~csubwindow ( )
{
   *first = NULL;
   *firstavailable = NULL;

}


void  csubwindow :: sethostcontainer ( pcontainer cntn )
{
   (*first)->sethostcontainerchain ( cntn );
}

void csubwindow :: sethostcontainerchain ( pcontainer cntn )
{
   hostcontainer = cntn;
   if ( next )
      next->sethostcontainerchain ( cntn );
}



int csubwindow :: subwin_available ( void )
{
   if ( cc->getactplayer() == actmap->actplayer ) {
      hostcontainer->registersubwindow ( this );
      nextavailable = (*firstavailable);
      (*firstavailable) = this;
   }
   return 0;
}


void csubwindow :: display ( void )
{
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 147;
   activefontsettings.justify = lefttext;
   activefontsettings.background = 201;
   showtext2c ( name, subwinx1 + 15, subwiny1 + 4 );
}


void csubwindow :: unitchanged ( void )
{}





void csubwindow :: paintlasche ( void )
{
   /*
   setinvisiblemouserectanglestk ( laschx1, subwiny2, laschx2, subwiny2 + laschheight );

   int color = laschcol;
   if ( hostcontainer->actsubwindow == this )
      color -= 3;


   int steps = laschheight / laschstepwidth;
   if ( laschheight % laschstepwidth )
      steps++;

   for ( int i = 0; i < steps ; i++ ) {
      int laschy2 = (i+1) * laschstepwidth;
      if ( laschy2 > laschheight )
         laschy2 = laschheight;

      line ( laschx1+i, subwiny2, laschx1+i, subwiny2 + laschy2 , color );
}

   bar ( laschx1 + steps, subwiny2, laschx2 - steps, subwiny2 + laschheight, color );

   for ( i = 0; i < steps ; i++ ) {
      int laschy2 = ( steps - i ) * laschstepwidth;
      if ( laschy2 > laschheight )
         laschy2 = laschheight;

      line ( laschx2-steps+i+1, subwiny2, laschx2-steps+i+1, subwiny2 + laschy2 , color );
}
   npush ( activefontsettings );
   activefontsettings.color = white;
   activefontsettings.length = laschx2 - laschx1 - 2*steps;
   activefontsettings.justify = centertext;
   activefontsettings.background = 255;
   activefontsettings.height = 0;
   activefontsettings.font = schriften.guifont;
   showtext2 ( name, laschx1 + steps, subwiny2 + ( laschheight - activefontsettings.font->height ) / 2 );
   npop ( activefontsettings );

   getinvisiblemouserectanglestk (  );

   */
   collategraphicoperations cgo ( laschxpos + laschx1 * laschdist, laschypos, laschxpos + laschx1 * laschdist + 45, laschypos + 22 );
   if ( hostcontainer->actsubwindow == this )
      putimage ( laschxpos + laschx1 * laschdist, laschypos, laschpic1 );
   else
      putimage ( laschxpos + laschx1 * laschdist, laschypos, laschpic2 );
}


void csubwindow :: paintalllaschen ( int i )
{
   if ( i == 1 )
      (*firstavailable)->paintalllaschen();
   else {
      paintlasche( );
      if ( nextavailable )
         nextavailable->paintalllaschen();
   }
}


void csubwindow :: setlaschenpos ( int x1, int maxwidth )
{
   /*
   int steps = laschheight / laschstepwidth;
   if ( laschheight % laschstepwidth )
      steps++;

   int textwidth = gettextwdth ( name, schriften.guifont ) + 2 * steps;
   if ( textwidth > maxwidth )
      textwidth = maxwidth;
   laschx1 = x1 + subwinx1;
   laschx2 = x1 + subwinx1 + textwidth;
   if ( nextavailable )
      nextavailable->setlaschenpos ( x1 + textwidth + 2, maxwidth );
      */

   laschx1 = x1;
   if ( nextavailable )
      nextavailable->setlaschenpos ( x1 + 1, maxwidth );
}


void csubwindow :: init ( void )
{
   (*first) -> subwin_available ();

   if ( hostcontainer->allsubwindownum ) {
      if ( (*firstavailable) ) {
         (*firstavailable)->setlaschenpos ( 0, (subwinx2 - subwinx1) / hostcontainer->allsubwindownum - 2 );
         (*firstavailable)->setactive();
      }
   } else {
      npush ( activefontsettings );
      activefontsettings.color = white;
      activefontsettings.font = schriften.arial8;
      activefontsettings.length = 0;
      activefontsettings.justify = lefttext;
      showtext2 ( " no subwindows available ", subwinx1 + 20, subwiny1 + 20 );
      npop ( activefontsettings );
   }
}

void csubwindow :: checklaschkey ( tkey taste )
{
   if ( taste == ct_right  || taste==ct_6k )
      if ( nextavailable )
         nextavailable->setactive();
      else
         if ( (*firstavailable) != this )
            (*firstavailable)->setactive();

   if ( taste == ct_left || taste==ct_4k ) {
      psubwindow sw = (*firstavailable);
      if ( this != (*firstavailable) ) {
         while ( sw->nextavailable && sw->nextavailable != this )
            sw = sw->nextavailable;

         if ( sw->nextavailable == this )
            sw->setactive();

      } else {
         while ( sw->nextavailable )
            sw = sw->nextavailable;

         if ( sw != this )
            sw->setactive();

      }
   }
}


void  csubwindow :: mousecheck ( int checkall )
{
   if ( mouseparams.taste == 0 )
      lastclickpos = 0;

   if ( checkall ) {
      if ( mouseparams.x >= subwinx1  &&  mouseparams.x <= subwinx2  &&
            mouseparams.y >= subwiny2  &&  mouseparams.x <= subwiny2+laschheight &&
            mouseparams.taste == 1  &&  (lastclickpos == 0  ||  lastclickpos == 1)) {
         lastclickpos = 1;
         if ( mouseparams.x >= laschxpos + laschx1*laschdist  &&  mouseparams.x <= laschxpos + (laschx1+1)*laschdist ) {
            setactive(  );
         } else
            if ( nextavailable )
               nextavailable->mousecheck ( checkall );
      }

   } else {
      if ( mouseparams.x >= subwinx1  &&  mouseparams.x <= subwinx2  &&
            mouseparams.y >= subwiny1  &&  mouseparams.y <= subwiny2 &&
            mouseparams.taste == 1  &&  (lastclickpos == 0  ||  lastclickpos == 2)) {
         checkformouse();
         lastclickpos = 2;

      }
      if ( mouseparams.y >= subwiny2  &&  mouseparams.x <= subwiny2+laschheight &&
            mouseparams.taste == 1  &&  (lastclickpos == 0  ||  lastclickpos == 1))
         if ( (*firstavailable) )
            (*firstavailable)->mousecheck ( 1 );
   }
}


void csubwindow :: setactive ( void )
{
   if ( hostcontainer->actsubwindow != this ) {
      //hostcontainer->actsubwindow->setinactive();
      hostcontainer->actsubwindow = this;
      display();
      if ( (*firstavailable) )
         (*firstavailable)->paintalllaschen();

   }
}


void csubwindow :: paintobj ( int num, int stat )
{
   collategraphicoperations cgo  ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2,   objcoordinates[num].y2 );
   if ( objcoordinates[num].type == 1 ) {
      if ( stat != 2 ) {
         rectangle ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2,   objcoordinates[num].y2,   black );
         rectangle ( objcoordinates[num].x1+1, objcoordinates[num].y1+1, objcoordinates[num].x2-1, objcoordinates[num].y2-1, black );
      } else {
         rahmen ( true, objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2,   objcoordinates[num].y2  );
         rahmen ( true, objcoordinates[num].x1+1, objcoordinates[num].y1+1, objcoordinates[num].x2-1, objcoordinates[num].y2-1 );
      }
      if ( stat == 0 )
         bar ( objcoordinates[num].x1+2, objcoordinates[num].y1+2, objcoordinates[num].x2-2, objcoordinates[num].y2-2, bkgrcol );
      if ( stat == 1 )
         bar ( objcoordinates[num].x1+2, objcoordinates[num].y1+2, objcoordinates[num].x2-2, objcoordinates[num].y2-2, laschcol-1 );
      if ( stat == 2 )
         bar ( objcoordinates[num].x1+2, objcoordinates[num].y1+2, objcoordinates[num].x2-2, objcoordinates[num].y2-2, laschcol-4 );
   }
   if ( objcoordinates[num].type == 2 ) {
      if ( stat == 0 )
         rectangle ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2,   objcoordinates[num].y2,   black );
      else
         rahmen ( true, objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2,   objcoordinates[num].y2  );
   }
}




int  csubwindow :: objpressedbymouse ( int num )
{
   if ( mouseparams.x >= objcoordinates[num].x1  &&  mouseparams.y >= objcoordinates[num].y1  &&  mouseparams.x <= objcoordinates[num].x2  &&  mouseparams.y <= objcoordinates[num].y2  &&  mouseparams.taste == 1 ) {
      paintobj ( num, 1 );
      while ( mouseparams.x >= objcoordinates[num].x1  &&
              mouseparams.y >= objcoordinates[num].y1  &&
              mouseparams.x <= objcoordinates[num].x2  &&
              mouseparams.y <= objcoordinates[num].y2  &&
              mouseparams.taste == 1 ) releasetimeslice();
      paintobj ( num, 0 );
      if ( mouseparams.x >= objcoordinates[num].x1  &&  mouseparams.y >= objcoordinates[num].y1  &&  mouseparams.x <= objcoordinates[num].x2  &&  mouseparams.y <= objcoordinates[num].y2 )
         return 1;
   }
   return 0;
}


void csubwindow :: resetammo ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetammo(1);
   else
      if ( nextavailable )
         nextavailable->resetammo ( mode );
}

void csubwindow :: resetresources ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetresources(1);
   else
      if ( nextavailable )
         nextavailable->resetresources ( mode );
}














/***********************************************************************************/
/*                                                                                 */
/*   class    ccontainer_b : Geb„ude-Innereien                                     */
/*                                                                                 */
/***********************************************************************************/




cbuildingsubwindow :: cbuildingsubwindow ( void )
{
   firstb = &buildingparamstack[recursiondepth].cbuildingsubwindow_firstb;

   nextb = *firstb;
   *firstb = this;

   hostcontainerb = NULL;
}

cbuildingsubwindow :: ~cbuildingsubwindow (  )
{
   *firstb = NULL;
}

void cbuildingsubwindow :: sethostcontainerchain ( pcontainer_b cntn )
{
   hostcontainerb = cntn;
   if ( nextb )
      nextb->sethostcontainerchain ( cntn );
}

void  cbuildingsubwindow :: sethostcontainer ( pcontainer_b cntn )
{
   (*first)  ->sethostcontainerchain ( cntn );
   (*firstb) ->sethostcontainerchain ( cntn );
}






ccontainer_b :: ccontainer_b ( void )
{
   actgui->restorebackground();
   oldguihost = actgui;
   actgui = &hosticons_cb;
   unitmode = mnormal;
   memset ( &produceableunits, 0, sizeof ( produceableunits ));
}

void  ccontainer_b :: init ( pbuilding bld )
{
   hosticons_cb.init ( hgmp->resolutionx, hgmp->resolutiony );
   hosticons_cb.seticonmains ( this );
   hosticons_cb.starticonload();

   if ( bld ) {
      int mss = getmousestatus();
      if ( mss == 2 )
         mousevisible ( false );

      cbuildingcontrols :: init ( bld );

      setpictures();

      // setzen des mapcursors auf den Geb„udeeingang
      MapCoordinate mc = building->getEntry();
      cursor.gotoxy ( mc.x , mc.y );

      ccontainer :: init ( building->getpicture ( building->typ->entry ),
                           building->color, building->name.c_str(), building->typ->name.c_str());
      ccontainer :: displayloading ();
      ccontainer :: movemark (repaint);

      subwindows.repairbuilding_subwindow.sethostcontainer ( this );
      subwindows.repairbuilding_subwindow.init();
      if ( mss == 2 )
         mousevisible ( true );

   };
}

void ccontainer_b :: unitchanged( void )
{
   if ( unitmode == mproduction ) {
      pvehicletype fzt = getmarkedunittype();
      if ( fzt  && actmap->player[ cc->getactplayer() ].research.vehicletypeavailable ( fzt ) ) {
         int en = fzt->productionCost.energy;
         int ma = fzt->productionCost.material;
         int fu = 0;

         if ( CGameOptions::Instance()->container.filleverything )
            displaymessage2(" production costs ~%d~ energy, ~%d~ material and ~%d~ fuel ( + %d energy, %d material, %d fuel for filling the tanks)", en, ma, fu, 0, fzt->tank.material, fzt->tank.fuel  );
         else
            displaymessage2(" production costs ~%d~ energy, ~%d~ material and ~%d~ fuel ", en, ma, fu );


      } else
         displaymessage2(" ");
   }
}

void  ccontainer_b :: setpictures ( void )
{
   int i;
   if ( unitmode == mnormal  || unitmode == mloadintocontainer ) {
      ccontainer::setpictures();
   } else

      if ( unitmode == mproduction ) {
         int num = 0;
         for (i = 0; i < 32; i++ )
            if ( building->production[i]  &&
                  actmap->player[ cc->getactplayer() ].research.vehicletypeavailable ( building->production[i] )  &&
                  building->typ->vehicleFit( building->production[i] ) ) {
               produceableunits[num] = building->production[i];
               picture[num] = building->production[i]->picture[0] ;
               int en = building->production[i]->productionCost.energy;
               int ma = building->production[i]->productionCost.material;
               if ( getResource ( en, Resources::Energy, true ) < en  ||  getResource(  ma, Resources::Material, true ) < ma )
                  pictgray[num] = 1;
               else
                  pictgray[num] = 0;
               num++;
            };
         for ( i = num; i < 32; i++ ) {
            picture[i] = NULL;
            pictgray[i] = 0;
            produceableunits[i] = NULL;
         }

         inactivefield = icons.container.mark.repairinactive;
         activefield   = icons.container.mark.repairactive;
      }
}



pvehicle    ccontainer_b :: getmarkedunit (void)
{
   if ( unitmode == mnormal || unitmode == mloadintocontainer)
      return building->loading[mark.y*unitsshownx + mark.x];
   else
      return NULL;
}

pvehicletype ccontainer_b :: getmarkedunittype ( void )
{
   if ( unitmode == mproduction )
      return produceableunits[mark.y*unitsshownx + mark.x];
   else
      return NULL;
}



pvehicle cbuildingcontrols :: getloadedunit (int num)
{
   return building->loading[num];
};

pvehicle ccontainer_b :: getloadedunit (int num)
{
   if ( unitmode == mnormal || unitmode == mloadintocontainer )
      return building->loading[num];
   else
      return NULL;
};

void  ccontainer_b :: paintvehicleinfo ( void )
{
   if ( unitmode == mnormal )
      dashboard.paintvehicleinfo ( getmarkedunit(), NULL, NULL, NULL );
   else
      :: dashboard.paintvehicleinfo ( NULL, NULL, NULL, getmarkedunittype () );
}



ccontainer_b :: ~ccontainer_b ( )
{
   actgui = oldguihost ;
}



int    ccontainer_b :: putammunition (int  weapontype, int  ammunition, int abbuchen)
{
   if ( abbuchen )
      repaintammo = 1;

   return cbuildingcontrols :: putammunition ( weapontype, ammunition, abbuchen );
}


int    ccontainer_b :: getammunition ( int weapontype, int num, int abbuchen, int produceifrequired  )
{
   if ( abbuchen )
      repaintammo = 1;

   return cbuildingcontrols :: getammunition ( weapontype, num, abbuchen, produceifrequired  );
}


Resources ccontainercontrols :: getResource ( Resources res, bool queryOnly )
{

   Resources r =  baseContainer->getResource(res, queryOnly );
   if ( !queryOnly )
      repaintResources();
   return r;
}

int  ccontainercontrols :: getResource ( int amount, int resourceType, bool queryOnly )
{
   int r = baseContainer->getResource(amount, resourceType, queryOnly );
   if ( !queryOnly )
      repaintResources();
   return r;
}



// Gui




void  ccontainer_b :: chosticons_cb :: init ( int resolutionx, int resolutiony )
{
   chainiconstohost ( &icons.movement );     //   muá erst eingesetzt werden !
   // GuiHost<generalicon_c*>::init ( resolutionx, resolutiony );
   ContainerBaseGuiHost::init ( resolutionx, resolutiony );
}








// --------------- Subwindows ----------------------------------------------------------------------------------







ccontainer_b :: crepairbuilding_subwindow :: crepairbuilding_subwindow ( void )
{
   strcpy ( name, "building info / repair" );
   laschpic1 = icons.container.lasche.a.buildinginfo[0];
   laschpic2 = icons.container.lasche.a.buildinginfo[1];

   objcoordinates[0].x1 = subwinx1 + 164;
   objcoordinates[0].y1 = subwiny1 +  34;
   objcoordinates[0].x2 = subwinx1 + 200;
   objcoordinates[0].y2 = subwiny1 + 50;
   objcoordinates[0].type = 5;

   ndamag = -1000;

   helplist.num = 10;

   static tonlinehelpitem repairbuildinghelpitems[10]  =
        {{{ 53 + subwinx1 , 26 + subwiny1 , 153 + subwinx1, 35 + subwiny1}, 20100 },
         {{ 52 + subwinx1 , 49 + subwiny1 ,  92 + subwinx1, 60 + subwiny1}, 20101 },
         {{113 + subwinx1 , 49 + subwiny1 , 153 + subwinx1, 60 + subwiny1}, 20102 },
         {{164 + subwinx1 , 34 + subwiny1 , 200 + subwinx1, 50 + subwiny1}, 20103 },
         {{ 26 + subwinx1 , 71 + subwiny1 ,  92 + subwinx1, 81 + subwiny1}, 20104 },
         {{ 26 + subwinx1 , 83 + subwiny1 ,  92 + subwinx1, 93 + subwiny1}, 20105 },
         {{ 26 + subwinx1 , 95 + subwiny1 ,  92 + subwinx1,105 + subwiny1}, 20106 },
         {{113 + subwinx1 , 71 + subwiny1 , 165 + subwinx1, 81 + subwiny1}, 20107 },
         {{277 + subwinx1 , 22 + subwiny1 , 297 + subwinx1,108 + subwiny1}, 20108 },
         {{308 + subwinx1 , 22 + subwiny1 , 328 + subwinx1,108 + subwiny1}, 20109 }};

   helplist.item = repairbuildinghelpitems;

}

int  ccontainer_b :: crepairbuilding_subwindow :: subwin_available ( void )
{
   cbuildingsubwindow :: subwin_available ( );
   if ( next )
      next->subwin_available ();

   return 0;
}

void  ccontainer_b :: crepairbuilding_subwindow :: display ( void )
{
   if ( ndamag == -1000 ) {
      if ( cc_b->building->repairedThisTurn )
         ndamag = cc_b->building->damage;
      else
         ndamag = cc_b->building->damage - actmap->getgameparameter ( cgp_maxbuildingrepair );

      if ( ndamag < 0 )
         ndamag = 0;
      // checkto ( ndamag );
   }

   collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );
   npush ( activefontsettings );
   putimage ( subwinx1, subwiny1, icons.container.subwin.buildinginfo.start );

   csubwindow :: display();
/* ####TRANS
   for ( int i = 0; i < 8; i++ ) {
      if ( cc_b->building->typ->loadcapability & ( 1 << ( 7 - i) ))
         putimage ( subwinx1 + 277, subwiny1 + 22 + i * 11, icons.container.subwin.buildinginfo.height1[i] );
      else
         putimage ( subwinx1 + 277, subwiny1 + 22 + i * 11, icons.container.subwin.buildinginfo.height2[i] );

      if ( cc_b->building->typ->unitheightreq & ( 1 << ( 7 - i ) ))
         putimage ( subwinx1 + 308, subwiny1 + 22 + i * 11, icons.container.subwin.buildinginfo.height1[i] );
      else
         putimage ( subwinx1 + 308, subwiny1 + 22 + i * 11, icons.container.subwin.buildinginfo.height2[i] );
   }
*/   
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 36;
   activefontsettings.justify = righttext;
   activefontsettings.background = 255;
   showtext2c ( strrr ( cc_b->building->getArmor() ), subwinx1 + 53,  subwiny1 + 72 );
   showtext2c ( strrr ( cc_b->building->typ->jamming ),  subwinx1 + 53,  subwiny1 + 84 );
   showtext2c ( strrr ( cc_b->building->typ->view ),     subwinx1 + 53,  subwiny1 + 96 );
   /* ####TRANS
   showtext2c ( strrr ( cc_b->building->typ->loadcapacity ),  subwinx1 + 140, subwiny1 + 72 );
   */

   paintvariables();

   /*   activefontsettings.color = white;
      activefontsettings.font = schriften.guifont;
      activefontsettings.length = 60;
      activefontsettings.justify = lefttext;
      activefontsettings.background = bkgrdarkcol;


      showtext2c ( "damage:",       subwinx1 + 20, subwiny1 + 10 );
      showtext2c ( "repairable:",   subwinx1 + 20, subwiny1 + 27 );
      showtext2c ( "repair cost: ", subwinx1 + 20, subwiny1 + 44 );


      objcoordinates[0].x1 = subwinx1 + 182;
      objcoordinates[0].x2 = subwinx2 -  20;
      objcoordinates[0].y1 = subwiny1 +  44;
      objcoordinates[0].y2 = subwiny1 +  58 + activefontsettings.font->height;
      objcoordinates[0].type = 2;
      objnum = 1;
      paintobj ( 0, 0 );

      activefontsettings.length = objcoordinates[0].x2 - objcoordinates[0].x1 - 2;
      activefontsettings.height = 0;
      activefontsettings.justify = centertext;
      showtext2c ( "repair !", objcoordinates[0].x1 + 1 , ( objcoordinates[0].y1 + objcoordinates[0].y2 - activefontsettings.font->height ) / 2 );
   */
   npop ( activefontsettings );
}

void ccontainer_b :: crepairbuilding_subwindow :: paintvariables ( void )
{
   npush ( activefontsettings );
   // int newdamage = checkto ( ndamag );

   int w = ( 100 - cc_b->building->damage);
   int c;
   if ( w > 23 )
      c = green;
   else
      if ( w > 15 )
         c = yellow;
      else
         if ( w > 7 )
            c = lightred;
         else
            c = red;

   bar ( subwinx1 + 53, subwiny1 + 26, subwinx1 + 53 + w, subwiny1 + 34, c );

   if( cc_b->building->damage ) {
      bar ( subwinx1 + 53 + ( 100 - cc_b->building->damage)+1, subwiny1 + 26, subwinx1 + 152, subwiny1 + 34, 244 );
      int maxr = actmap->getgameparameter ( cgp_maxbuildingrepair ) + w;
      if ( maxr > 100 )
         maxr = 100;

      if ( w < 100 && !cc_b->building->repairedThisTurn) {
         line ( subwinx1 + 53 + w, subwiny1 + 26+3, subwinx1 + 53 + maxr, subwiny1 + 26+3, c );
         line ( subwinx1 + 53 + w, subwiny1 + 26+5, subwinx1 + 53 + maxr, subwiny1 + 26+5, c );
         line ( subwinx1 + 53 + maxr, subwiny1 + 26, subwinx1 + 53 + maxr, subwiny1 + 34, c );
      }

   }

   putimage ( subwinx1 + 68, subwiny1 + 26, icons.container.subwin.buildinginfo.block );
   putimage ( subwinx1 + 86, subwiny1 + 26, icons.container.subwin.buildinginfo.block );
   putimage ( subwinx1 +104, subwiny1 + 26, icons.container.subwin.buildinginfo.block );

   if ( cc_b->building->damage ) {
      activefontsettings.font = schriften.guifont;
      activefontsettings.length = 37;
      activefontsettings.justify = righttext;
      activefontsettings.background = 244;
      Resources cost;
      cc_b->building->getMaxRepair ( cc_b->building, ndamag, cost );
      showtext2c ( strrr ( cost.energy ), subwinx1 + 53, subwiny1 + 51 );
      showtext2c ( strrr ( cost.material ), subwinx1 + 114, subwiny1 + 51 );
   } else {
      bar ( subwinx1 + 53, subwiny1 + 50 , subwinx1 + 53 + 38, subwiny1 + 59 , 244 );
      bar ( subwinx1 + 114, subwiny1 + 50, subwinx1 + 114 + 38, subwiny1 + 59 , 244 );
   }
   npop ( activefontsettings );
}

void  ccontainer_b :: crepairbuilding_subwindow :: paintobj ( int num, int stat )
{
   csubwindow::paintobj( num, stat );
   if ( objcoordinates[num].type == 5 ) {
      collategraphicoperations cgo  ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2,   objcoordinates[num].y2 );
      if ( stat == 0 )
         putrotspriteimage ( objcoordinates[num].x1, objcoordinates[num].y1, icons.container.subwin.buildinginfo.repair, actmap->actplayer * 8  );
      else
         putrotspriteimage ( objcoordinates[num].x1, objcoordinates[num].y1, icons.container.subwin.buildinginfo.repairpressed, actmap->actplayer * 8  );
   }
}

void  ccontainer_b :: crepairbuilding_subwindow :: checkformouse ( void )
{
   if ( cc_b->building->damage )
      if ( objpressedbymouse ( 0 ) ) {
         cc_b->building->repairItem ( cc_b->building, ndamag );
         cc_b->building->repairedThisTurn = 1;
         hostcontainer->repaintresources = 1;
         paintvariables();
      }
}

void  ccontainer_b :: crepairbuilding_subwindow :: checkforkey ( tkey taste )
{
}


//............................................................................................



ccontainer_b :: cnetcontrol_subwindow :: cnetcontrol_subwindow ( void )
{
   strcpy ( name, "net control" );
   laschpic1 = icons.container.lasche.a.netcontrol[0];
   laschpic2 = icons.container.lasche.a.netcontrol[1];
}

int  ccontainer_b :: cnetcontrol_subwindow :: subwin_available ( void )
{
   // if ( actmap->_resourcemode != 1 )
   cbuildingsubwindow :: subwin_available ( );

   if ( next )
      next->subwin_available ();

   return 0;
}

void  ccontainer_b :: cnetcontrol_subwindow :: paintobj ( int num, int stat )
{
   csubwindow::paintobj( num, stat );
   if ( objcoordinates[num].type == 10 ) {
      collategraphicoperations cgo  ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2+10,   objcoordinates[num].y2 );
      if ( stat == 1 )
         putrotspriteimage ( objcoordinates[num].x1, objcoordinates[num].y1, icons.container.subwin.netcontrol.active, actmap->actplayer * 8  );
      else
         putrotspriteimage ( objcoordinates[num].x1, objcoordinates[num].y1, icons.container.subwin.netcontrol.inactive, actmap->actplayer * 8  );
   }
}


void  ccontainer_b :: cnetcontrol_subwindow :: display ( void )
{
   collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );
   putimage ( subwinx1, subwiny1, icons.container.subwin.netcontrol.start );

   npush ( activefontsettings );
   csubwindow :: display();
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 0;
   activefontsettings.justify = lefttext;
   activefontsettings.background = 255;
   objnum = 0;
   for ( int y = 0; y < 4; y++ ) {
      showtext2c ( cgeneralnetcontrol[y], subwinx1 + 12, subwiny1 + 32 + 20 * y );
      for ( int x = 0; x < 3 ; x++) {
         objcoordinates[objnum].x1 = subwinx1 + 210 + 44 * x;
         objcoordinates[objnum].y1 = subwiny1 +  29 + 20 * y;
         objcoordinates[objnum].x2 = objcoordinates[objnum].x1 + 20;
         objcoordinates[objnum].y2 = objcoordinates[objnum].y1 + 14;
         objcoordinates[objnum].type = 10;

         if ( getstatus ( objnum ) )
            paintobj( objnum ,1 ) ;
         else
            paintobj( objnum ,0 ) ;

         objnum++;
      } /* endfor */
   }

   npop ( activefontsettings );
}

int ccontainer_b :: cnetcontrol_subwindow :: getstatus ( int num )
{
   switch ( num ) {
      case 0:
      case 1:
      case 2:
         return getnetmode ( cnet_storeenergy << num );
      case 3:
      case 4:
      case 5:
         return getnetmode ( cnet_moveenergyout << (num-3) );
      case 6:
      case 7:
      case 8:
         return getnetmode ( cnet_stopenergyinput << (num-6) );
      case 9:
      case 10:
      case 11:
         return getnetmode ( cnet_stopenergyoutput << (num-9) );
   } /* endswitch */
   return 0;
}

void ccontainer_b :: cnetcontrol_subwindow :: objpressed ( int num )
{
   switch ( num ) {
      case 0:
      case 1:
      case 2:
         setnetmode ( cnet_storeenergy << (num % 3), !getnetmode ( cnet_storeenergy << (num % 3) ) );
         break;
      case 3:
      case 4:
      case 5:
         setnetmode ( cnet_moveenergyout << (num % 3), !getnetmode ( cnet_moveenergyout << (num % 3) ) );
         break;
      case 6:
      case 7:
      case 8:
         setnetmode ( cnet_stopenergyinput << (num % 3), !getnetmode ( cnet_stopenergyinput << (num % 3) ) );
         break;
      case 9:
      case 10:
      case 11:
         setnetmode ( cnet_stopenergyoutput << (num % 3), !getnetmode ( cnet_stopenergyoutput << (num % 3) ) );
         break;
   } /* endswitch */


   if ( getstatus ( num ) )
      paintobj ( num, 1 );
   else
      paintobj ( num, 0 );

   if ( getnetmode ( cnet_storeenergy << (num % 3) ) && getnetmode ( cnet_moveenergyout << (num % 3) ) ) {
      if ( num < 3 ) {
         setnetmode ( cnet_moveenergyout << (num % 3), 0 );
         paintobj ( num + 3, 0 );
      } else {
         setnetmode ( cnet_storeenergy << (num % 3), 0 );
         paintobj ( (num % 3), 0 );
      }
   }
   hostcontainerb->repaintresources = 1;
   hostcontainerb->building->execnetcontrol();
}



void  ccontainer_b :: cnetcontrol_subwindow :: checkformouse ( void )
{
   for (int i = 0; i < 12; i++ ) {
      if ( mouseparams.x >= objcoordinates[i].x1  &&  mouseparams.y >= objcoordinates[i].y1  &&  mouseparams.x <= objcoordinates[i].x2  &&  mouseparams.y <= objcoordinates[i].y2  &&  mouseparams.taste == 1 ) {
         objpressed ( i );
         while ( mouseparams.x >= objcoordinates[i].x1  &&
                 mouseparams.y >= objcoordinates[i].y1  &&
                 mouseparams.x <= objcoordinates[i].x2  &&
                 mouseparams.y <= objcoordinates[i].y2  &&
                 mouseparams.taste == 1 ) releasetimeslice();
         //         objpressed ( i );
      }

   } /* endfor */
}

void  ccontainer_b :: cnetcontrol_subwindow :: checkforkey ( tkey taste )
{
   switch ( taste ) {
      case ct_1:
         objpressed ( 0 );
         break;
      case ct_2:
         objpressed ( 1 );
         break;
      case ct_3:
         objpressed ( 2 );
         break;
      case ct_q:
         objpressed ( 3 );
         break;
      case ct_w:
         objpressed ( 4 );
         break;
      case ct_e:
         objpressed ( 5 );
         break;
      case ct_a:
         objpressed ( 6 );
         break;
      case ct_s:
         objpressed ( 7 );
         break;
      case ct_d:
         objpressed ( 8 );
         break;
      case ct_y:
         objpressed ( 9 );
         break;
      case ct_x:
         objpressed ( 10 );
         break;
      case ct_c:
         objpressed ( 11 );
         break;
   } /* endswitch */
}


//............................................................................................


ccontainer_b :: cconventionelpowerplant_subwindow :: cconventionelpowerplant_subwindow ( void )
{
   strcpy ( name, "power plant" );
   laschpic1 = icons.container.lasche.a.powerplant[0];
   laschpic2 = icons.container.lasche.a.powerplant[1];

   helplist.num = 5;

   static tonlinehelpitem powerplanthelpitems[5]   =
        {{{ 11 + subwinx1 , 48 + subwiny1 , 164 + subwinx1, 58 + subwiny1}, 20120 },
         {{ 11 + subwinx1 , 61 + subwiny1 , 164 + subwinx1, 71 + subwiny1}, 20121 },
         {{ 11 + subwinx1 , 74 + subwiny1 , 164 + subwinx1, 84 + subwiny1}, 20122 },
         {{ 11 + subwinx1 , 87 + subwiny1 , 164 + subwinx1, 97 + subwiny1}, 20123 },
         {{ 178+ subwinx1 , 23 + subwiny1 , 343 + subwinx1, 108 + subwiny1}, 20125 }};

   helplist.item = powerplanthelpitems;


   resourcecolor[0] = 16+4;
   resourcecolor[1] = 232;
   resourcecolor[2] = 27; // 125;
   objcoordinates[0].x1 = subwinx1 + 316;
   objcoordinates[0].y1 = subwiny1 +   3;
   objcoordinates[0].x2 = subwinx1 + 343;
   objcoordinates[0].y2 = subwiny1 +  15;
   objcoordinates[0].type = 17;
}

int  ccontainer_b :: cconventionelpowerplant_subwindow :: subwin_available ( void )
{
   if ( actmap->_resourcemode != 1 )
      if ( ( hostcontainer->getspecfunc ( mbuilding ) & cgconventionelpowerplantb ) && ( cc_b->building->maxplus.energy )) {
         bool avail = false;
         for ( int r = 0; r < 3; r++ )
            if ( cc_b->building->maxplus.resource(r) < 0 )
               avail = true;

         if ( avail )
            cbuildingsubwindow :: subwin_available ( );
      }

   if ( next )
      next->subwin_available ();

   return 0;
}

void  ccontainer_b :: cconventionelpowerplant_subwindow :: display ( void )
{

   if ( cc_b->building->maxplus.energy )
      power = 1024 * cc_b->building->plus.energy / cc_b->building->maxplus.energy;
   else
      power = 0;

   collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );

   npush ( activefontsettings );

   putimage ( subwinx1, subwiny1, icons.container.subwin.conventionelpowerplant.start );

   csubwindow :: display();
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 0;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;
   showtext2c ( "max plus",     subwinx1 + 12, subwiny1 + 56 -7);
   showtext2c ( "max usage",     subwinx1 + 12, subwiny1 + 69 -7);
   showtext2c ( "plus",     subwinx1 + 12, subwiny1 + 82 -7);
   showtext2c ( "usage",     subwinx1 + 12, subwiny1 + 95 -7);


   /*   showtext2c ( "energy plus:", subwinx1 + 8, subwiny1 + 25 );

      showtext2c ( "fuel     cost:",     subwinx1 + 8, subwiny1 + 43 );
      showtext2c ( "material cost:",     subwinx1 + 8, subwiny1 + 61 );*/

   //   showtext2c ( "avail in:",        subwinx1 + 8, subwiny1 + 79 );

   //   showtext2 ( "act. technology:", subwinx1 + 195, subwiny1 + 4 );
   //   showtext2c ( "change all buildings:",subwinx1+179,subwiny1 + 5 );



   gx1 = subwinx1 + 181;
   gy1 = subwiny1 + 105 - 70;
   gx2 = subwinx1 + 181 + 150;
   gy2 = subwiny1 + 105;


   npop ( activefontsettings );

   displayvariables();
}

int ccontainer_b :: cconventionelpowerplant_subwindow :: allbuildings = 0;

void ccontainer_b :: cconventionelpowerplant_subwindow :: setnewpower ( int pwr )
{
   int x = gx1 + ( gx2 - gx1 ) * power / 1024;

   int cl ;
   if ( x == gx1 )
      cl = 247;
   else
      if ( x == gx1+1 )
         cl = 244;
      else
         cl = 201;

   line( x, gy1, x, gy2-1, cl );

   power = pwr;
   if ( power > 1024 )
      power = 1024;

   if ( allbuildings ) {
      for ( tmap::Player::BuildingList::iterator bi = actmap->player[actmap->actplayer].buildingList.begin(); bi != actmap->player[actmap->actplayer].buildingList.end(); bi++ ) {
         pbuilding bld = *bi;
         if ( bld->typ->special & cgconventionelpowerplantb )
            for ( int r = 0; r < 3; r++ )
               bld->plus.resource(r) = bld->maxplus.resource(r) * power/1024;

         logtoreplayinfo( rpl_setResourceProcessingAmount, bld->getPosition().x, bld->getPosition().y, bld->plus.energy, bld->plus.material, bld->plus.fuel );      
      }
   } else {
      pbuilding bld = cc_b->building;
      for ( int r = 0; r < 3; r++ )
         bld->plus.resource(r) = bld->maxplus.resource(r) * power/1024;

      logtoreplayinfo( rpl_setResourceProcessingAmount, bld->getPosition().x, bld->getPosition().y, bld->plus.energy, bld->plus.material, bld->plus.fuel );
   }

}

void  ccontainer_b :: cconventionelpowerplant_subwindow :: dispresources ( Resources* res, int ypos, int sign )
{
   npush ( activefontsettings );
   activefontsettings.font = schriften.monogui;
   activefontsettings.length = 29;
   activefontsettings.justify = righttext;
   activefontsettings.height = 0;
   activefontsettings.background = bkgrdarkcol;

   char buf[100];

   int r;
   for ( r = 0; r < 3; r++ )
      if( res->resource(r) * sign > 0 ) {
         activefontsettings.color = resourcecolor[r];
         showtext2 ( int2string ( res->resource(r) * sign, buf ), subwinx1 + 68 + r * 33, subwiny1 + 48 + ypos * 13 );
      } else
         bar ( subwinx1 + 68 + r * 33, subwiny1 + 48 + ypos * 13, subwinx1 + 68 + r * 33 + activefontsettings.length, subwiny1 + 48 + ypos * 13 + activefontsettings.font->height - 1, activefontsettings.background );

   npop ( activefontsettings );
}

void  ccontainer_b :: cconventionelpowerplant_subwindow :: displayvariables ( void )
{
   int x;

   npush ( activefontsettings );
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 30;
   activefontsettings.justify = righttext;
   activefontsettings.height = 0;
   activefontsettings.background = 201;

   dispresources ( &cc_b->building->maxplus, 0, 1 );
   dispresources ( &cc_b->building->maxplus, 1, -1 );
   dispresources ( &cc_b->building->plus, 2, 1 );


   Building::MatterConverter matConv( cc_b->building );
   Resources usage = matConv.getUsage();

   dispresources ( &usage, 3, 1 );

   int max = 0;
   for ( int r = 0; r < 3; r++ )
      if ( abs ( cc_b->building->maxplus.resource(r) ) > max )
         max = abs ( cc_b->building->maxplus.resource(r) );

   max = max * 17 / 16;

   int dist = gx2-gx1;
   for (x = dist; x >0 ; x--) {
      // returnresourcenuseforpowerplant ( cc_b->building, 100 * x / dist, &usage, 1 );

      if ( max )
         for ( int r = 0; r < 3; r++ )
            if ( cc_b->building->maxplus.resource(r) != 0 )
               if ( !(x % 3) || cc_b->building->maxplus.resource(r) >= 0 )
                  putpixel ( gx1 + x, gy2 - ( gy2-gy1 ) * abs ( cc_b->building->maxplus.resource(r) * x / dist ) / max, resourcecolor[r] );

   } /* endfor */
   x = gx1 + ( gx2 - gx1 ) * power / 1024;

   line( x, gy1, x, gy2-1, white );

   npop ( activefontsettings );
}


void  ccontainer_b :: cconventionelpowerplant_subwindow :: checkformouse ( void )
{
   if ( mouseparams.taste == 1 ) {
      if ( mouseinrect ( gx1, gy1, gx2, gy2 ) ) {
         int newpower = 1024 * (mouseparams.x-gx1) / (gx2-gx1);

         if ( newpower < 0 )
            newpower = 0;
         if ( newpower > 1024 )
            newpower = 1024;

         if ( newpower != power ) {
            collategraphicoperations cgo  ( subwinx1, subwiny1, subwinx2, subwiny2 );

            setnewpower( newpower );

            displayvariables();

         }
      }
      if ( objpressedbymouse(0) ) {
         allbuildings = !allbuildings;
         collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );
         displayvariables();
      }
   }
}


void  ccontainer_b :: cconventionelpowerplant_subwindow :: paintobj ( int num, int stat )
{
   if ( objcoordinates[0].type == 17 ) {

      collategraphicoperations cgo  ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2+10,   objcoordinates[num].y2 );

      activefontsettings.font = schriften.guifont;
      activefontsettings.height = 0;
      activefontsettings.justify = centertext;
      activefontsettings.length = 22;
      activefontsettings.background = 255;
      /*
           if ( stat == 0 ) {
              putimage ( objcoordinates[0].x1, objcoordinates[0].y1, icons.container.subwin.conventionelpowerplant.button[0] );
              if ( allbuildings )
                 showtext2c ( "yes", subwinx1+318, subwiny1 +  5 );
              else
                 showtext2c ( "no",  subwinx1+318, subwiny1 +  5 );
           } else {
              putimage ( objcoordinates[0].x1, objcoordinates[0].y1, icons.container.subwin.conventionelpowerplant.button[1] );
              if ( allbuildings )
                 showtext2c ( "yes", subwinx1+319, subwiny1 +  6 );
              else
                 showtext2c ( "no",  subwinx1+319, subwiny1 +  6 );
           }
      */
   }
}


void  ccontainer_b :: cconventionelpowerplant_subwindow :: checkforkey ( tkey taste )
{
   /*
   if ( taste == ct_space  ||  taste == ct_a ) {
     allbuildings = !allbuildings;
     collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );
     displayvariables();
}
   */
   int keyspeed = 50;
   if ( (taste == ct_left || taste==ct_4k) && power > 0 ) {
      collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );

      if ( power > keyspeed )
         setnewpower ( power - keyspeed );
      else
         setnewpower ( 0 );

      displayvariables();
   }
   if ( (taste == ct_right  || taste==ct_6k) && power < 1024 ) {
      collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );

      if ( power+keyspeed < 1024 )
         setnewpower ( power + keyspeed );
      else
         setnewpower ( 1024 );

      displayvariables();
   }
}

//............................................................................................


ccontainer_b :: cwindpowerplant_subwindow :: cwindpowerplant_subwindow ( void )
{
   strcpy ( name, "wind power" );
   laschpic1 = icons.container.lasche.a.wind[0];
   laschpic2 = icons.container.lasche.a.wind[1];
}

int  ccontainer_b :: cwindpowerplant_subwindow :: subwin_available ( void )
{
   if ( actmap->_resourcemode != 1 )
      if ( hostcontainer->getspecfunc ( mbuilding ) & cgwindkraftwerkb )
         cbuildingsubwindow :: subwin_available ( );
   if ( next )
      next->subwin_available ();

   return 0;
}

void  ccontainer_b :: cwindpowerplant_subwindow :: display ( void )
{
   npush ( activefontsettings );
   collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );
   putimage ( subwinx1, subwiny1, icons.container.subwin.windpower.start );

   csubwindow :: display();
   activefontsettings.length = 95;
   activefontsettings.justify = lefttext;
   activefontsettings.background = 201;
   activefontsettings.font = schriften.guifont;
   showtext2c ( " current power : ", subwinx1 + 9, subwiny1 + 38 );
   showtext2c ( " power storable: ", subwinx1 + 9, subwiny1 + 62 );
   showtext2c ( " max power : ", subwinx1 + 9, subwiny1 + 86 );

   activefontsettings.length = 53;
   activefontsettings.justify = righttext;

   Building::WindPowerplant windPowerPlant ( cc_b->building );
   Resources plus = windPowerPlant.getPlus();
   int prod = plus.energy;
   int storable = cc_b->building->putResource( plus.energy, 0, 1 );

   showtext2c ( strrr ( prod ), subwinx1 + 117, subwiny1 + 38 );

   char buf[100];
   if ( prod ) {
      strcpy ( buf, strrr ( storable / prod * 100));
      strcat ( buf, "%" );
   } else
      strcpy ( buf, "-" );

   showtext2c ( buf, subwinx1 + 117, subwiny1 + 62 );
   showtext2c ( strrr ( cc_b->building->maxplus.energy ), subwinx1 + 117, subwiny1 + 86 );

   npop ( activefontsettings );

}



void  ccontainer_b :: cwindpowerplant_subwindow :: checkformouse ( void )
{}


void  ccontainer_b :: cwindpowerplant_subwindow :: checkforkey ( tkey taste )
{
}


void ccontainer_b :: cwindpowerplant_subwindow :: resetresources ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetresources(1);
   else {
      if ( hostcontainer->actsubwindow == this )
         display();

      if ( nextavailable )
         nextavailable->resetresources ( mode );
   }
}

//............................................................................................


ccontainer_b :: csolarpowerplant_subwindow :: csolarpowerplant_subwindow ( void )
{
   strcpy ( name, "solar power" );
   laschpic1 = icons.container.lasche.a.solar[0];
   laschpic2 = icons.container.lasche.a.solar[1];
}

int  ccontainer_b :: csolarpowerplant_subwindow :: subwin_available ( void )
{
   if ( actmap->_resourcemode != 1 )
      if ( hostcontainer->getspecfunc ( mbuilding ) & cgsolarkraftwerkb )
         cbuildingsubwindow :: subwin_available ( );
   if ( next )
      next->subwin_available ();

   return 0;
}

void  ccontainer_b :: csolarpowerplant_subwindow :: display ( void )
{
   npush ( activefontsettings );
   collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );
   putimage ( subwinx1, subwiny1, icons.container.subwin.solarpower.start );
   csubwindow :: display();

   activefontsettings.length = 95;
   activefontsettings.justify = lefttext;
   activefontsettings.background = 201;
   activefontsettings.font = schriften.guifont;
   showtext2c ( " current power : ", subwinx1 + 9, subwiny1 + 38 );
   showtext2c ( " power storable: ", subwinx1 + 9, subwiny1 + 62 );
   showtext2c ( " max power : ", subwinx1 + 9, subwiny1 + 86 );

   activefontsettings.length = 53;
   activefontsettings.justify = righttext;

   Building::SolarPowerplant solarPowerPlant ( cc_b->building );
   Resources plus = solarPowerPlant.getPlus();
   int prod = plus.energy;
   int storable = cc_b->building->putResource( plus.energy, 0, 1 );

   showtext2c ( strrr ( prod ), subwinx1 + 117, subwiny1 + 38 );

   char buf[100];
   if ( prod ) {
      strcpy ( buf, strrr ( storable / prod * 100));
      strcat ( buf, "%" );
   } else
      strcpy ( buf, "-" );

   showtext2c ( buf, subwinx1 + 117, subwiny1 + 62 );

   if ( actmap->_resourcemode == 1 )
      showtext2c ( strrr ( cc_b->building->bi_resourceplus.energy ), subwinx1 + 117, subwiny1 + 86 );
   else
      showtext2c ( strrr ( cc_b->building->maxplus.energy ), subwinx1 + 117, subwiny1 + 86 );


   npop ( activefontsettings );
}



void  ccontainer_b :: csolarpowerplant_subwindow :: checkformouse ( void )
{}


void  ccontainer_b :: csolarpowerplant_subwindow :: checkforkey ( tkey taste )
{
}



void ccontainer_b :: csolarpowerplant_subwindow :: resetresources ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetresources(1);
   else {
      if ( hostcontainer->actsubwindow == this )
         display();

      if ( nextavailable )
         nextavailable->resetresources ( mode );
   }
}


//............................................................................................


ccontainer_b :: cammunitionproduction_subwindow :: cammunitionproduction_subwindow ( void )
{
   int i;

   strcpy ( name, "ammunition production" );

   for (i = 0; i < waffenanzahl; i++) {
      objcoordinates[i].x1 = subwinx1 + 23  + 37 * i ;
      objcoordinates[i].x2 = objcoordinates[i].x1 + 10;
      objcoordinates[i].y1 = subwiny1 + 28;
      objcoordinates[i].y2 = subwiny1 + 90;
      objcoordinates[i].type = 3;
      objcoordinates[i].t1 = 0;
      objcoordinates[i].t2 = 8;
   } /* endfor */

   txtptr = "produce";

   laschpic1 = icons.container.lasche.a.ammoproduction[0];
   laschpic2 = icons.container.lasche.a.ammoproduction[1];
   maxproduceablenum = 20;
   grad = 50;
   materialneeded = 0;
   energyneeded = 0;
   fuelneeded = 0;

   for ( i = 0; i < waffenanzahl; i++)
      toproduce[i] = 0;

   objcoordinates[9].x1 = subwinx1 + 217;
   objcoordinates[9].x2 = subwinx1 + 303;
   objcoordinates[9].y1 = subwiny1 + 4;
   objcoordinates[9].y2 = subwiny1 + 14;
   objcoordinates[9].type = 5;

   actschieber = 0 ;

}


int  ccontainer_b :: cammunitionproduction_subwindow :: subwin_available ( void )
{
   if ( hostcontainer->getspecfunc ( mbuilding ) & cgammunitionproductionb )
      cbuildingsubwindow :: subwin_available ( );
   if ( next )
      next->subwin_available ();

   return 0;
}

void  ccontainer_b :: cammunitionproduction_subwindow :: displayusage ( void )
{
   int e = 0;
   int m = 0;
   int f = 0;

   for ( int i = 0; i < waffenanzahl; i++ ) {
      e += cwaffenproduktionskosten[i][0] * toproduce[i];
      m += cwaffenproduktionskosten[i][1] * toproduce[i];
      f += cwaffenproduktionskosten[i][2] * toproduce[i];
   }

   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 35;
   activefontsettings.justify = righttext;
   activefontsettings.background = 201;
   showtext2c ( strrr ( e ), subwinx1 + 305, subwiny1 + 37 );
   showtext2c ( strrr ( m ), subwinx1 + 305, subwiny1 + 67 );
   showtext2c ( strrr ( f ), subwinx1 + 305, subwiny1 + 97 );

}

void  ccontainer_b :: cammunitionproduction_subwindow :: display ( void )
{
   int i;

   npush ( activefontsettings );
   collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );
   putimage ( subwinx1, subwiny1, icons.container.subwin.ammoproduction.start );
   csubwindow :: display();
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.justify = lefttext;



   int x1 = 0;
   int y1 = 0;
   int x2 = 66;
   int y2 = 14;
   char* buf = new char [ imagesize ( x1, y1, x2, y2 ) ];

   for (i = 0; i < waffenanzahl; i++) {
      {
         tvirtualdisplay vdp ( 100, 100, 255 );
         activefontsettings.length = 0;
         activefontsettings.justify = lefttext;
         activefontsettings.background = 255;
         showtext2c ( cwaffentypen[i], 10, 10 );
         getimage ( 10, 10, 10 + x2, 10 + y2, buf );
      }
      putrotspriteimage90 ( subwinx1 + 5 + 37 * i , subwiny1 + 27, buf, 0 );

      objcoordinates[i].t1 = num2pos ( toproduce[i] );
   }

   delete[] buf;

   for (i = 0; i < waffenanzahl; i++)
      paintobj ( i, 0 );

   activefontsettings.font = schriften.guifont;
   activefontsettings.background = 255;
   activefontsettings.length = 65;
   activefontsettings.height = 0;
   activefontsettings.justify = centertext;
   showtext2c ( txtptr, subwinx1 + 228, subwiny1 + 5 );

   displayusage();

   npop ( activefontsettings );
}

void ccontainer_b :: cammunitionproduction_subwindow :: produce( void )
{
   for (int i = 0; i < waffenanzahl; i++) {
      /*cbuildingcontrols :: */cproduceammunition :: produce ( i, toproduce[i] );
      toproduce[i] = 0;
   }
}


int  ccontainer_b :: cammunitionproduction_subwindow :: pos2num ( int pos )
{
   int ydiff = objcoordinates[0].y2 - objcoordinates[0].y1 - objcoordinates[0].t2;
   //  return ( (double) maxproduceablenum * pow ( pos, grad )  /  pow ( ydiff , grad ) );

   int result = (int)( maxproduceablenum * ( exp ( pos / grad ) - 1 ) / ( exp ( ydiff / grad ) - 1 ));
   if ( result < 0 )
      result = 0;
   if ( result > maxproduceablenum )
      result = (int)maxproduceablenum;
   return result;
}

int  ccontainer_b :: cammunitionproduction_subwindow :: num2pos ( int num )
{
   int ydiff = objcoordinates[0].y2 - objcoordinates[0].y1 - objcoordinates[0].t2;
   //  return sqrt ( (double) num * pow ( ydiff, grad ) / maxproduceablenum );

   int result = (int)( grad * log ( num / maxproduceablenum  * ( exp ( ydiff / grad ) - 1 ) + 1 ));
   if ( result < 0 )
      result = 0;
   if ( result > ydiff )
      result = ydiff;

   return result;
}


void ccontainer_b :: cammunitionproduction_subwindow :: checknewval ( int weaptype, int num )
{
   baseenergyusage    = 0;
   basematerialusage  = 0;
   basefuelusage      = 0;

   for (int i = 0; i < waffenanzahl; i++) {
      if ( i != weaptype ) {
         int n = checkavail ( i, toproduce[i] );
         if ( n != toproduce[i] )
            setnewamount( i, n );

         baseenergyusage    += energyneeded;
         basematerialusage  += materialneeded;
         basefuelusage      += fuelneeded;
      }
   } /* endfor */

   if ( weaptype != -1 ) {
      int n = checkavail ( weaptype, num );
      if ( n != toproduce[weaptype] )
         setnewamount( weaptype, n );
   }
   displayusage( );
}

void ccontainer_b :: cammunitionproduction_subwindow :: setnewamount ( int weaptype, int num )
{
   if ( weaptype > 8 )
      displaymessage(" 1 !!!!! ", 2);
   toproduce[weaptype] = num;
   objcoordinates[weaptype].t1 = num2pos ( num );
   paintobj ( weaptype, 0 );
}

void ccontainer_b :: cammunitionproduction_subwindow :: paintobj ( int num, int stat )
{
   csubwindow :: paintobj ( num, stat );
   if ( objcoordinates[num].type == 3 ) {
      {
         collategraphicoperations cgo  ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2+10,   objcoordinates[num].y2 );
         putimage ( objcoordinates[num].x1,   objcoordinates[num].y1,  icons.container.subwin.ammoproduction.schiene );

         int offs = 0;
         if ( num == actschieber )
            offs = 1;

         if ( objcoordinates[num].y2 - objcoordinates[num].t1 - objcoordinates[num].t2 == objcoordinates[num].y1 )
            putimage ( objcoordinates[num].x1,   objcoordinates[num].y2 - objcoordinates[num].t1 - objcoordinates[num].t2,  icons.container.subwin.ammoproduction.schieber[offs + 2] );
         else
            putimage ( objcoordinates[num].x1,   objcoordinates[num].y2 - objcoordinates[num].t1 - objcoordinates[num].t2,  icons.container.subwin.ammoproduction.schieber[offs] );

      }
      activefontsettings.color = white;
      activefontsettings.font = schriften.guifont;
      activefontsettings.length = 29;
      activefontsettings.justify = centertext;
      activefontsettings.background = bkgrcol;

      showtext2c ( strrr ( toproduce[num] * weaponpackagesize ), subwinx1 + 6 + num * 37, subwiny1 + 96 );
   }
   if ( objcoordinates[num].type == 5 ) {
      collategraphicoperations cgo  ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2+10,   objcoordinates[num].y2 );
      activefontsettings.font = schriften.guifont;
      activefontsettings.background = 255;
      activefontsettings.length = 65;
      activefontsettings.height = 0;
      activefontsettings.justify = centertext;
      if ( stat == 0 ) {
         putimage ( subwinx1 + 217, subwiny1 + 4, icons.container.subwin.ammoproduction.button );
         showtext2c ( txtptr, subwinx1 + 228, subwiny1 +5 );
      } else {
         putimage ( subwinx1 + 217, subwiny1 + 4, icons.container.subwin.ammoproduction.buttonpressed );
         showtext2c ( txtptr, subwinx1 + 229, subwiny1 + 6 );
      }
   }
}

void  ccontainer_b :: cammunitionproduction_subwindow :: checkformouse ( void )
{
   int i;

   if ( mouseparams.taste == 1 ) {
      for (i = 0; i < waffenanzahl; i++) {
         if ( mouseparams.x >= objcoordinates[i].x1    && mouseparams.x <= objcoordinates[i].x2  &&
               mouseparams.y >= objcoordinates[i].y1    && mouseparams.y <= objcoordinates[i].y2 ) {

            int repnt = 0;
            int tp = toproduce[i];
            if ( i != actschieber ) {
               int old = actschieber;
               actschieber = i;
               paintobj ( old, 0 );
               repnt = 1;
            }

            int relpos = objcoordinates[i].y2 -  mouseparams.y - objcoordinates[i].t2 / 2 ;
            if ( relpos < 0 )
               relpos = 0;
            if ( relpos > ( objcoordinates[i].y2 - objcoordinates[i].y1 - objcoordinates[i].t2 + 1 ) )
               relpos = objcoordinates[i].y2 - objcoordinates[i].y1 - objcoordinates[i].t2 + 1;

            if ( relpos != objcoordinates[i].t1  ) {
               int num = pos2num ( relpos );
               if ( num != toproduce[i]  )
                  checknewval ( i, num );
            }

            if (  tp == toproduce[i] && repnt )
               paintobj ( i, 0 );


         }
      } /* endfor */

      if ( objpressedbymouse ( 9 ) ) {
         produce();
         for ( i = 0; i < waffenanzahl; i++ ) {
            objcoordinates[i].t1 = 0;
            paintobj ( i, 0 );
         }
         displayusage();

      }

   }
}

void  ccontainer_b :: cammunitionproduction_subwindow :: checkforkey ( tkey taste )
{
   if ( taste == ct_right  || taste==ct_6k ) {
      int olds = actschieber;
      if ( actschieber < 7 )
         actschieber++;
      else
         actschieber = 0;

      paintobj ( olds, 0 );
      paintobj ( actschieber, 0 );

   }
   if ( taste == ct_left || taste==ct_4k) {
      int olds = actschieber;
      if ( actschieber > 0 )
         actschieber--;
      else
         actschieber = 7;

      paintobj ( olds, 0 );
      paintobj ( actschieber, 0 );
   }
   if ( taste == ct_up  || taste==ct_8k)
      if ( toproduce[actschieber] < maxproduceablenum )
         checknewval ( actschieber, toproduce[actschieber] + 1 );

   if ( taste == ct_down  || taste==ct_2k )
      if ( toproduce[actschieber] > 0 )
         checknewval ( actschieber, toproduce[actschieber] - 1 );

   if ( taste == ct_enter || taste == ct_enterk ) {
      produce();
      for ( int i = 0; i < waffenanzahl; i++ ) {
         objcoordinates[i].t1 = 0;
         paintobj ( i, 0 );
      }
      displayusage();
   }

}



//............................................................................................


ccontainer_b :: cresourceinfo_subwindow :: cresourceinfo_subwindow ( void )
{
   strcpy ( name, "resource info" );
   laschpic1 = icons.container.lasche.a.resourceinfo[0];
   laschpic2 = icons.container.lasche.a.resourceinfo[1];
   recalc = 1;
}

int  ccontainer_b :: cresourceinfo_subwindow :: subwin_available ( void )
{
   cbuildingsubwindow :: subwin_available ( );
   if ( next )
      next->subwin_available ();

   return 0;
}

const char* resourceinfotxt[4]  =
   {  "avail", "capacity", "plus", "usage"
   };
const char* resourceinfotxt2[3] =
   {  "local", "net",      "global"
   };

void  ccontainer_b :: cresourceinfo_subwindow :: display ( void )
{
   int y;

   npush ( activefontsettings );
   collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );
   putimage ( subwinx1, subwiny1, icons.container.subwin.resourceinfo.start );
   csubwindow :: display();
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 0;
   activefontsettings.justify = lefttext;
   activefontsettings.background = 255;

   for (y = 0; y < 4; y++ )
      showtext2c ( resourceinfotxt[y], subwinx1 + 5, subwiny1 + 58 + y * 13 );

   activefontsettings.length = 40;
   activefontsettings.justify = centertext;
   for ( y = 0; y < 3; y++ )
      showtext2c ( resourceinfotxt2[y], subwinx1 + 77 + y * 99, subwiny1 + 27 );


   displayvariables ();

   npop ( activefontsettings );

}

int  ccontainer_b :: cresourceinfo_subwindow :: getvalue ( int resourcetype, int y, int scope )
{
   switch ( y ) {
      case 0:
         {  // avail
            GetResource gr ( cc_b->building->getMap() );
            return gr.getresource ( cc_b->building->getEntry().x, cc_b->building->getEntry().y, resourcetype, maxint, 1, cc_b->building->color/8, scope );
         }
      case 1:
         {  // tank
            GetResourceCapacity grc ( cc_b->building->getMap() );
            return grc.getresource ( cc_b->building->getEntry().x, cc_b->building->getEntry().y, resourcetype, maxint, 1, cc_b->building->color/8, scope );
         }
      case 2:
         {  // plus
            GetResourcePlus grp ( cc_b->building->getMap() );
            return grp.getresource ( cc_b->building->getEntry().x, cc_b->building->getEntry().y, resourcetype, cc_b->building->color/8, scope );
         }
      case 3:
         {  // usage
            GetResourceUsage gru( cc_b->building->getMap() );
            return gru.getresource ( cc_b->building->getEntry().x, cc_b->building->getEntry().y, resourcetype, cc_b->building->color/8, scope );
         }
   } /* endswitch */
   return -1;
}


void ccontainer_b :: cresourceinfo_subwindow :: displayvariables( void )
{
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 29;
   activefontsettings.justify = righttext;
   activefontsettings.height = 0;
   activefontsettings.background = bkgrdarkcol;

   char buf[50];

   int value[3][3][4];
   int mx = 0;
   int c;

   for ( c = 0; c < 3; c++ )
      for ( int x = 0; x < 3; x++ )
         for ( int y = 0; y < 4; y++ ) {
            value[c][x][y] = getvalue ( x, y, c );
            if ( y != 1 )
               if ( value[c][x][y] > mx )
                  mx = value[c][x][y];
         }



   for ( c = 0; c < 3; c++ )
      for ( int x = 0; x < 3; x++ )
         for ( int y = 0; y < 4; y++ )
            if ( (y != 1 || value[c][x][y] < mx*10 || value[c][x][y] < 1000000000 ) && ( !actmap->isResourceGlobal(x) || y!=0 ||c ==2))   // don't show extremely high numbers
               showtext2c ( int2string ( value[c][x][y], buf ), subwinx1 + 49 + ( c * 3 + x ) * 33, subwiny1 + 57 + y * 13 );
            else
               showtext2c ( "-", subwinx1 + 49 + ( c * 3 + x ) * 33, subwiny1 + 57 + y * 13 );

   activefontsettings.length = 0;
   activefontsettings.length = 100;
   recalc = 1;
}



void  ccontainer_b :: cresourceinfo_subwindow :: checkformouse ( void )
{}


void  ccontainer_b :: cresourceinfo_subwindow :: checkforkey ( tkey taste )
{
}



void ccontainer_b :: cresourceinfo_subwindow :: resetammo ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetammo(1);
   else {
      if ( hostcontainer->actsubwindow == this )
         displayvariables();

      if ( nextavailable )
         nextavailable->resetammo ( mode );
   }
}

void ccontainer_b :: cresourceinfo_subwindow :: resetresources ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetresources(1);
   else {
      if ( hostcontainer->actsubwindow == this )
         displayvariables();

      if ( nextavailable )
         nextavailable->resetresources ( mode );
   }
}

//............................................................................................

int ccontainer_b :: cresearch_subwindow :: allbuildings = 0;

ccontainer_b :: cresearch_subwindow :: cresearch_subwindow ( void )
{
   strcpy ( name, "research" );
   laschpic1 = icons.container.lasche.a.research[0];
   laschpic2 = icons.container.lasche.a.research[1];
   materialcolor = 125;
   energycolor = 232;
   fuelcolor = red;
   objcoordinates[0].x1 = subwinx1 + 117;
   objcoordinates[0].y1 = subwiny1 +  95;
   objcoordinates[0].x2 = subwinx1 + 146;
   objcoordinates[0].y2 = subwiny1 + 108;
   objcoordinates[0].type = 17;
}

int  ccontainer_b :: cresearch_subwindow :: subwin_available ( void )
{
   if ( (hostcontainer->getspecfunc ( mbuilding ) & cgresearchb) && ( cc_b->building->maxresearchpoints ))
      cbuildingsubwindow :: subwin_available ( );
   if ( next )
      next->subwin_available ();

   return 0;
}

void  ccontainer_b :: cresearch_subwindow :: display ( void )
{
   if ( cc_b->building->maxresearchpoints )
      research = 1024 * cc_b->building->researchpoints / cc_b->building->maxresearchpoints;
   else
      research = 0;

   collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );

   npush ( activefontsettings );

   putimage ( subwinx1, subwiny1, icons.container.subwin.research.start );

   csubwindow :: display();
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 0;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;
   showtext2c ( "research points:", subwinx1 + 8, subwiny1 + 25 );

   showtext2c ( "energy cost:",     subwinx1 + 8, subwiny1 + 43 );
   showtext2c ( "material cost:",   subwinx1 + 8, subwiny1 + 61 );
   showtext2c ( "fuel cost:",       subwinx1 + 8, subwiny1 + 79 );

   showtext2c ( "avail in:",        subwinx1 + 8, subwiny1 + 97 );

   //   showtext2 ( "act. technology:", subwinx1 + 195, subwiny1 + 4 );
   // showtext2c ( "change all buildings:",subwinx1+8,subwiny1 + 98 );



   gx1 = subwinx1 + 181;
   gy1 = subwiny1 + 104 - 70;
   gx2 = subwinx1 + 181 + 150;
   gy2 = subwiny1 + 104;


   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;
   activefontsettings.background = 255;
   if ( actmap->player[actmap->actplayer].research.activetechnology ) {
      showtext2c(actmap->player[actmap->actplayer].research.activetechnology->name, subwinx1 + 195, subwiny1 + 4 );
      // showtext2 ( "avail in:", subwinx1 + 130, subwiny1 +        1 + abstand2 + abstand1 + abstand2 + abstand1 );
   } else
      showtext2c ( "none", subwinx1 + 195, subwiny1 + 4  );

   npop ( activefontsettings );

   displayvariables();
}


void ccontainer_b :: cresearch_subwindow :: setnewresearch ( int res )
{
   int x = gx1 + ( gx2 - gx1 ) * research / 1024;

   int cl ;
   if ( x == gx1 )
      cl = 247;
   else
      if ( x == gx1+1 )
         cl = 244;
      else
         cl = 201;

   line( x, gy1, x, gy2-1, cl );

   research = res;

   if ( allbuildings ) {
      for ( tmap::Player::BuildingList::iterator bi = actmap->player[actmap->actplayer].buildingList.begin(); bi != actmap->player[actmap->actplayer].buildingList.end(); bi++ ) {
         pbuilding bld = *bi;
         if ( bld->typ->special & cgresearchb ) {
            bld->researchpoints = bld->maxresearchpoints * research/1024;
            if ( bld->researchpoints > bld->maxresearchpoints )
               bld->researchpoints = bld->maxresearchpoints;

            logtoreplayinfo ( rpl_setResearch, bld->getIdentification(), bld->researchpoints );
         }
      }
   } else {
      pbuilding bld = cc_b->building;
      bld->researchpoints = bld->maxresearchpoints * research/1024;
      if ( bld->researchpoints > bld->maxresearchpoints )
         bld->researchpoints = bld->maxresearchpoints;

      logtoreplayinfo ( rpl_setResearch, bld->getIdentification(), bld->researchpoints );
   }
}


void  ccontainer_b :: cresearch_subwindow :: displayvariables ( void )
{
   int x;

   npush ( activefontsettings );
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 28;
   activefontsettings.justify = righttext;
   activefontsettings.height = 0;
   activefontsettings.background = 201;

   Resources res = returnResourcenUseForResearch ( cc_b->building, cc_b->building->researchpoints );

   showtext2c ( strrr( cc_b->building->researchpoints ), subwinx1 + 115, subwiny1 + 25 );

   showtext2c ( strrr( res.energy ), subwinx1 + 115, subwiny1 + 43 );
   showtext2c ( strrr( res.material ), subwinx1 + 115, subwiny1 + 61 );
   showtext2c ( strrr( res.fuel ), subwinx1 + 115, subwiny1 + 79 );


   showtext2c ( "avail in:",        subwinx1 + 8, subwiny1 + 97 );

   int rppt = 0;
   for ( tmap::Player::BuildingList::iterator bi = actmap->player[actmap->actplayer].buildingList.begin(); bi != actmap->player[actmap->actplayer].buildingList.end(); bi++ )
       rppt += (*bi)->researchpoints;

   if ( rppt  && actmap->player[actmap->actplayer].research.activetechnology ) {
      showtext2c ( strrr( (actmap->player[actmap->actplayer].research.activetechnology->researchpoints - actmap->player[actmap->actplayer].research.progress + rppt-1) / rppt ),  subwinx1 + 115, subwiny1 + 97 );
   } else
      bar ( subwinx1 + 115, subwiny1 + 97, subwinx1 + 115 + activefontsettings.length, subwiny1 + 97 + activefontsettings.font->height, activefontsettings.background );

   activefontsettings.justify = centertext;
   activefontsettings.length = 22;
   activefontsettings.background = 255;

/*
   putimage ( subwinx1 + 117, subwiny1 + 95, icons.container.subwin.research.button[0] );
   if ( allbuildings )
      showtext2c ( "yes", subwinx1+120, subwiny1 + 98 );
   else
      showtext2c ( "no",  subwinx1+120, subwiny1 + 98 );
*/


   // ->typ ??
   res = returnResourcenUseForResearch ( cc_b->building, cc_b->building->maxresearchpoints );
   int mx = max ( res.energy, max(res.material,res.fuel)) * 17/16;

   int dist = gx2-gx1;
   for (x = dist; x >0 ; x--) {
      int res = cc_b->building->maxresearchpoints * x / dist;
      Resources r = returnResourcenUseForResearch  ( cc_b->building, res );



      if ( mx ) {
         putpixel ( gx1 + x, gy2 - ( gy2-gy1 ) * r.energy / mx, energycolor );
         putpixel ( gx1 + x, gy2 - ( gy2-gy1 ) * r.material / mx, materialcolor );
         putpixel ( gx1 + x, gy2 - ( gy2-gy1 ) * r.fuel / mx, fuelcolor );
      }

   } /* endfor */
   x = gx1 + ( gx2 - gx1 ) * research / 1024;

   line( x, gy1, x, gy2-1, yellow );

   npop ( activefontsettings );
}


void  ccontainer_b :: cresearch_subwindow :: checkformouse ( void )
{
   if ( mouseparams.taste == 1 ) {
      if ( mouseinrect ( gx1, gy1, gx2, gy2 ) ) {
         int newresearch = 1024 * (mouseparams.x-gx1) / (gx2-gx1);
         if ( newresearch < 0 )
            newresearch = 0;
         if ( newresearch > 1024 )
            newresearch = 1024;
         if ( newresearch != research ) {
            collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );

            int x = gx1 + ( gx2 - gx1 ) * research/ 1024;

            int cl ;
            if ( x == gx1 )
               cl = 247;
            else
               if ( x == gx1+1 )
                  cl = 244;
               else
                  cl = 201;
            line( x, gy1, x, gy2-1, cl );

            setnewresearch( newresearch );

            displayvariables();
         }
      }
      if ( objpressedbymouse(0) ) {
         allbuildings = !allbuildings;
         collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );
         displayvariables();
      }
   }
}

void  ccontainer_b :: cresearch_subwindow :: paintobj ( int num, int stat )
{
   if ( objcoordinates[0].type == 17 ) {

      collategraphicoperations cgo  ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2+10,   objcoordinates[num].y2 );

      activefontsettings.font = schriften.guifont;
      activefontsettings.height = 0;
      activefontsettings.justify = centertext;
      activefontsettings.length = 22;
      activefontsettings.background = 255;

      if ( stat == 0 ) {
         putimage ( objcoordinates[0].x1, objcoordinates[0].y1, icons.container.subwin.research.button[0] );
         if ( allbuildings )
            showtext2c ( "yes", subwinx1+120, subwiny1 + 98 );
         else
            showtext2c ( "no",  subwinx1+120, subwiny1 + 98 );
      } else {
         putimage ( objcoordinates[0].x1, objcoordinates[0].y1, icons.container.subwin.research.button[1] );
         if ( allbuildings )
            showtext2c ( "yes", subwinx1+121, subwiny1 + 99 );
         else
            showtext2c ( "no",  subwinx1+121, subwiny1 + 99 );
      }
   }
}


void  ccontainer_b :: cresearch_subwindow :: checkforkey ( tkey taste )
{
   if ( taste == ct_space  ||  taste == ct_a ) {
      allbuildings = !allbuildings;
      collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );
      displayvariables();
   }
   int keyspeed = 50;
   if ( (taste == ct_left || taste==ct_4k ) && research > 0 ) {
      collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );
      if ( research > keyspeed )
         setnewresearch ( research - keyspeed );
      else
         setnewresearch ( 0 );

      displayvariables();
   }
   if ( (taste == ct_right  || taste==ct_6k) && research < 1024 ) {
      collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );
      if ( research+keyspeed < 1024 )
         setnewresearch ( research + keyspeed );
      else
         setnewresearch ( 1024 );

      displayvariables();
   }
}


//............................................................................................

int ccontainer_b :: cminingstation_subwindow :: allbuildings = 0;

ccontainer_b :: cminingstation_subwindow :: cminingstation_subwindow ( void )
{
   strcpy ( name, "mining" );
   laschpic1 = icons.container.lasche.a.miningstation[0];
   laschpic2 = icons.container.lasche.a.miningstation[1];
   materialcolor = 125;
   energycolor = 232;
   objcoordinates[0].x1 = subwinx1 + 316;
   objcoordinates[0].y1 = subwiny1 +   2;
   objcoordinates[0].x2 = subwinx1 + 344;
   objcoordinates[0].y2 = subwiny1 +  17;
   objcoordinates[0].type = 17;
   mininginfoLoaded = false;

   resourcecolor[0] = 16+3;
   resourcecolor[1] = 232;
   resourcecolor[2] = 27; // 125;



   helplist.num =  6;

   static tonlinehelpitem miningstationhelpitems[ 6]
      = {{{ 14 + subwinx1 , 41 + subwiny1 , 167 + subwinx1, 51 + subwiny1}, 20120 },
         {{ 14 + subwinx1 , 54 + subwiny1 , 167 + subwinx1, 64 + subwiny1}, 20121 },
         {{ 14 + subwinx1 , 67 + subwiny1 , 167 + subwinx1, 77 + subwiny1}, 20122 },
         {{ 14 + subwinx1 , 80 + subwiny1 , 167 + subwinx1, 90 + subwiny1}, 20123 },
         {{ 14 + subwinx1 , 93 + subwiny1 , 167 + subwinx1, 103 + subwiny1}, 20124 },
         {{ 178+ subwinx1 , 23 + subwiny1 , 343 + subwinx1, 108 + subwiny1}, 20125 }};


   helplist.item = miningstationhelpitems;
}

int  ccontainer_b :: cminingstation_subwindow :: subwin_available ( void )
{
   if ( actmap->_resourcemode != 1 )
      if ( hostcontainer->getspecfunc ( mbuilding ) & cgminingstationb )
         cbuildingsubwindow :: subwin_available ( );


   if ( next )
      next->subwin_available ();

   return 0;
}

void  ccontainer_b :: cminingstation_subwindow :: dispresources ( Resources* res, int ypos, int sign )
{
   npush ( activefontsettings );
   activefontsettings.font = schriften.monogui;
   activefontsettings.length = 29;
   activefontsettings.justify = righttext;
   activefontsettings.height = 0;
   activefontsettings.background = bkgrdarkcol;

   char buf[100];

   int r;
   for ( r = 0; r < 3; r++ )
      if( res->resource(r) * sign > 0 ) {
         activefontsettings.color = resourcecolor[r];
         showtext2 ( int2string ( res->resource(r) * sign, buf ), subwinx1 + 71 + r * 33, subwiny1 + 41 + ypos * 13 );
      } else
         bar ( subwinx1 + 71 + r * 33, subwiny1 + 41 + ypos * 13, subwinx1 + 71 + r * 33 + activefontsettings.length, subwiny1 + 41 + ypos * 13 + activefontsettings.font->height - 1, activefontsettings.background );

   npop ( activefontsettings );
}


void  ccontainer_b :: cminingstation_subwindow :: display ( void )
{
   extraction = 0;
   if ( cc_b->building->maxplus.material > 0 )
      extraction = 1024 * cc_b->building->plus.material / cc_b->building->maxplus.material;
   else
      if ( cc_b->building->maxplus.fuel > 0 )
         extraction = 1024 * cc_b->building->plus.fuel / cc_b->building->maxplus.fuel;


   collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );

   npush ( activefontsettings );

   putimage ( subwinx1, subwiny1, icons.container.subwin.miningstation.start );

   csubwindow :: display();

   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 53;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;
   showtext2c ( "max plus",       subwinx1 + 15, subwiny1 + 42 );
   showtext2c ( "max usage",      subwinx1 + 15, subwiny1 + 55 );
   showtext2c ( "plus",           subwinx1 + 15, subwiny1 + 68 );
   showtext2c ( "usage",          subwinx1 + 15, subwiny1 + 81 );
   showtext2c ( "efficiency",     subwinx1 + 15, subwiny1 + 94 );


   gx1 = subwinx1 + 181;
   gy1 = subwiny1 + 104 - 70;
   gx2 = subwinx1 + 181 + 150;
   gy2 = subwiny1 + 104;

   displayvariables();

   npop ( activefontsettings );
}


void ccontainer_b :: cminingstation_subwindow :: setnewextraction ( int res )
{
   int x = gx1 + ( gx2 - gx1 ) * extraction / 1024;

   int cl ;
   if ( x == gx1 )
      cl = 247;
   else
      if ( x == gx1+1 )
         cl = 244;
      else
         cl = 201;

   line( x, gy1, x, gy2-1, cl );

   extraction = res;

   if ( allbuildings ) {
      for ( tmap::Player::BuildingList::iterator bi = actmap->player[actmap->actplayer].buildingList.begin(); bi != actmap->player[actmap->actplayer].buildingList.end(); bi++ ) {
         pbuilding bld = *bi;
         if ( bld->typ->special & cgminingstationb ) {
            for ( int r = 0; r < 3; r++ )
               bld->plus.resource(r) = bld->maxplus.resource(r) * extraction/1024;
         }
         logtoreplayinfo( rpl_setResourceProcessingAmount, bld->getPosition().x, bld->getPosition().y, bld->plus.energy, bld->plus.material, bld->plus.fuel );      
      }
   } else {
      pbuilding bld = cc_b->building;
      for ( int r = 0; r < 3; r++ )
         bld->plus.resource(r) = bld->maxplus.resource(r) * extraction/1024;
         
      logtoreplayinfo( rpl_setResourceProcessingAmount, bld->getPosition().x, bld->getPosition().y, bld->plus.energy, bld->plus.material, bld->plus.fuel );
   }
}


void  ccontainer_b :: cminingstation_subwindow :: displayvariables ( void )
{
   int x;



   dispresources ( &cc_b->building->maxplus, 0, 1 );
   dispresources ( &cc_b->building->maxplus, 1, -1 );

   Building::MiningStation miningStation ( cc_b->building, true );

   Resources plus = miningStation.getPlus();
   dispresources ( &plus, 2, 1 );

   Resources usage = miningStation.getUsage();
   dispresources ( &usage, 3, 1 );

   Resources effic;
   effic.energy = 0;
   effic.material = cc_b->building->typ->efficiencymaterial;
   effic.fuel = cc_b->building->typ->efficiencyfuel;
   dispresources ( &effic, 4, 1 );


   npush ( activefontsettings );
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 30;
   activefontsettings.justify = righttext;
   activefontsettings.height = 0;
   activefontsettings.background = 201;

   int max = 0;
   for ( int r = 0; r < 3; r++ )
      if ( abs ( cc_b->building->maxplus.resource(r) ) > max )
         max = abs ( cc_b->building->maxplus.resource(r) );

   max = max * 17 / 16;

   int dist = gx2-gx1;
   if ( max )
      for (x = dist; x >0 ; x--)
         for ( int r = 0; r < 3; r++ )
            if ( cc_b->building->maxplus.resource(r) != 0 )
               if ( !(x % 3) || cc_b->building->maxplus.resource(r) >= 0 )
                  putpixel ( gx1 + x, gy2 - ( gy2-gy1 ) * abs ( cc_b->building->maxplus.resource(r) * x / dist ) / max, resourcecolor[r] );

   x = gx1 + ( gx2 - gx1 ) * extraction / 1024;

   line( x, gy1, x, gy2-1, white );

   npop ( activefontsettings );
}


void  ccontainer_b :: cminingstation_subwindow :: checkformouse ( void )
{
   if ( mouseparams.taste == 1 ) {
      if ( mouseinrect ( gx1, gy1, gx2, gy2 ) ) {
         int newresearch = 1024 * (mouseparams.x-gx1) / (gx2-gx1);
         if ( newresearch < 0 )
            newresearch = 0;
         if ( newresearch > 1024 )
            newresearch = 1024;
         if ( newresearch != extraction ) {
            int x = gx1 + ( gx2 - gx1 ) * extraction/ 1024;

            int cl ;
            if ( x == gx1 )
               cl = 247;
            else
               if ( x == gx1+1 )
                  cl = 244;
               else
                  cl = 201;
            line( x, gy1, x, gy2-1, cl );

            setnewextraction( newresearch );

            displayvariables();

         }
      }

      if ( objpressedbymouse(0) ) {
         if ( allbuildings < 2 )
            allbuildings++;
         else
            allbuildings = 0;

         paintobj( 0, 0 );
      }
   }
}

void  ccontainer_b :: cminingstation_subwindow :: paintobj ( int num, int stat )
{
   /*
   if ( objcoordinates[0].type == 17 ) {

     activefontsettings.font = schriften.guifont;
     activefontsettings.height = 0;
     activefontsettings.justify = centertext;
     activefontsettings.length = 22;
     activefontsettings.background = 255;

     int x;
     int y;
     if ( stat == 0 ) {
        putimage ( objcoordinates[0].x1, objcoordinates[0].y1, icons.container.subwin.miningstation.button[0] );
        x = 319;
        y =   5;
     } else {
        putimage ( objcoordinates[0].x1, objcoordinates[0].y1, icons.container.subwin.miningstation.button[1] );
        x = 320;
        y =   6;
     }

     if ( allbuildings == 0 )
        showtext2c ( "all", subwinx1+x, subwiny1 + y );
     else
        if ( allbuildings == 1 )
           if ( mode == 1 )
              showtext2c ( "mat.", subwinx1+x, subwiny1 + y );
           else
              showtext2c ( "fuel", subwinx1+x, subwiny1 + y );
        else
           showtext2c ( "this", subwinx1+x, subwiny1 + y );

}
   */
}


void  ccontainer_b :: cminingstation_subwindow :: checkforkey ( tkey taste )
{
   /*
   if ( taste == ct_space  ||  taste == ct_a ) {
     if ( allbuildings < 2 )
        allbuildings++;
     else
        allbuildings = 0;

     paintobj( 0, 0 );
} */

   int keyspeed = 50;
   if ( (taste == ct_left || taste==ct_4k)  && extraction > 0 ) {
      if ( extraction > keyspeed )
         setnewextraction ( extraction - keyspeed );
      else
         setnewextraction ( 0 );

      displayvariables();
   }
   if ( (taste == ct_right || taste==ct_6k) && extraction < 1024 ) {
      if ( extraction+keyspeed < 1024 )
         setnewextraction ( extraction + keyspeed );
      else
         setnewextraction ( 1024 );

      displayvariables();
   }

}


//............................................................................................

int ccontainer_b :: cmineralresources_subwindow :: allbuildings = 0;

ccontainer_b :: cmineralresources_subwindow :: cmineralresources_subwindow ( void )
{
   strcpy ( name, "mineral resources" );
   laschpic1 = icons.container.lasche.a.mineralresources[0];
   laschpic2 = icons.container.lasche.a.mineralresources[1];
   objcoordinates[0].x1 = subwinx1 + 316;
   objcoordinates[0].y1 = subwiny1 +   2;
   objcoordinates[0].x2 = subwinx1 + 344;
   objcoordinates[0].y2 = subwiny1 +  17;
   objcoordinates[0].type = 17;
   mininginfoLoaded = false;

   resourcecolor[0] = 16+4;
   resourcecolor[1] = 232;
   resourcecolor[2] = 27; // 125;



   helplist.num =  2;

   static tonlinehelpitem mineralresourceshelpitems[2] =
        {{{  6 + subwinx1 , 23 + subwiny1 , 171 + subwinx1, 108+ subwiny1}, 20128 },
         {{ 178+ subwinx1 , 23 + subwiny1 , 343 + subwinx1, 108+ subwiny1}, 20129 }};

   helplist.item = mineralresourceshelpitems;



}

int  ccontainer_b :: cmineralresources_subwindow :: subwin_available ( void )
{
   if ( actmap->_resourcemode != 1 )
      if ( hostcontainer->getspecfunc ( mbuilding ) & cgminingstationb )
         cbuildingsubwindow :: subwin_available ( );

   if ( next )
      next->subwin_available ();

   return 0;
}

void  ccontainer_b :: cmineralresources_subwindow :: dispresources ( Resources* res, int ypos, int sign )
{
   npush ( activefontsettings );
   activefontsettings.font = schriften.monogui;
   activefontsettings.length = 29;
   activefontsettings.justify = righttext;
   activefontsettings.height = 0;
   activefontsettings.background = 201;

   char buf[100];

   int r;
   for ( r = 0; r < 3; r++ )
      if( res->resource(r) * sign > 0 ) {
         activefontsettings.color = resourcecolor[r];
         showtext2 ( int2string ( res->resource(r) * sign, buf ), subwinx1 + 71 + r * 33, subwiny1 + 41 + ypos * 13 );
      } else
         bar ( subwinx1 + 71 + r * 33, subwiny1 + 41 + ypos * 13, subwinx1 + 71 + r * 33 + activefontsettings.length, subwiny1 + 41 + ypos * 13 + activefontsettings.font->height - 1, activefontsettings.background );

   npop ( activefontsettings );
}


void  ccontainer_b :: cmineralresources_subwindow :: display ( void )
{
   extraction = 0;
   if ( cc_b->building->maxplus.material > 0 )
      extraction = 1024 * cc_b->building->plus.material / cc_b->building->maxplus.material;
   else
      if ( cc_b->building->maxplus.fuel > 0 )
         extraction = 1024 * cc_b->building->plus.fuel / cc_b->building->maxplus.fuel;


   collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );

   npush ( activefontsettings );

   putimage ( subwinx1, subwiny1, icons.container.subwin.mineralresources.start );
   materialcolor = getpixel ( subwinx1 + 126, subwiny1 + 28 );
   fuelcolor = getpixel ( subwinx1 + 126, subwiny1 + 38 );

   csubwindow :: display();

   gx1 = subwinx1 + 9;
   gy1 = subwiny1 + 104 - 70;
   gx2 = subwinx1 + 9 + 150;
   gy2 = subwiny1 + 104;


   if ( !mininginfoLoaded ) {

      GetMiningInfo gmi ( actmap );
      gmi.run ( cc_b->building );

      mininginfo = gmi.getMiningInfo();
   }


   int max = mininginfo.efficiency[0] * 11 / 10;
   int i;
   for ( i = 0; i < maxminingrange; i++ ) {
      int y = gy2 - ( gy2 - gy1 ) * mininginfo.efficiency[i] / max;
      int xd = (gx2-gx1) / maxminingrange ;
      bar ( gx1 + i * xd , y, gx1 + i * xd + xd/2, gy2, materialcolor ); // 160 + 15 * mininginfo.avail[ i ].resource[ 1 ] / mininginfo.max[ i ].resource[ 1 ] );
      bar ( gx1 + i * xd + xd/2, y, gx1 + (i+1) * xd, gy2, fuelcolor ); // 160 + 15 * mininginfo.avail[ i ].resource( 2 ) / mininginfo.max[ i ].resource( 2 ) );
   }



   hx1 = subwinx1 + 181;
   hy1 = subwiny1 + 104 - 70;
   hx2 = subwinx1 + 181 + 150;
   hy2 = subwiny1 + 104;


   max = 0;
   for ( int r = 1; r < 3; r++ )
      for ( i = 0; i < maxminingrange; i++ )
         if ( mininginfo.max[ i ].resource( r ) > max )
            max = mininginfo.max[ i ].resource( r );

   max = max * 17 / 16;

   int maxa = 0;
   for ( int r = 1; r < 3; r++ )
      for ( i = 0; i < maxminingrange; i++ )
         if ( mininginfo.avail[ i ].resource( r ) > maxa )
            maxa = mininginfo.avail[ i ].resource( r );

   max = max * 17 / 16;
   maxa = maxa * 17 / 16;

   for ( i = 0; i < maxminingrange; i++ ) {
      int y1 = hy2 - ( hy2 - hy1 ) * mininginfo.avail[ i ].resource( 1 ) / mininginfo.max[ i ].resource( 1 );
      int y2 = hy2 - ( hy2 - hy1 ) * mininginfo.avail[ i ].resource( 2 ) / mininginfo.max[ i ].resource( 2 );

      int xd = (hx2-hx1) / maxminingrange ;
      bar ( hx1 + i * xd , y1, hx1 + i * xd + xd/2, hy2, materialcolor ); // 160 + 15 * mininginfo.avail[ i ].resource( 1 ) / mininginfo.max[ i ].resource( 1 ) );
      bar ( hx1 + i * xd + xd/2, y2, hx1 + (i+1) * xd, hy2, fuelcolor ); // 160 + 15 * mininginfo.avail[ i ].resource( 2 ) / mininginfo.max[ i ].resource( 2 ) );

      if ( maxa ) {
         y1 = hy2 - ( hy2 - hy1 ) * mininginfo.avail[ i ].resource( 1 ) / maxa;
         y2 = hy2 - ( hy2 - hy1 ) * mininginfo.avail[ i ].resource( 2 ) / maxa;

         // int ya = hy2 - ( hy2 - hy1 ) * mininginfo.max[ i ].resource( 2 ) / maxa;

         line ( hx1 + i * xd , y1, hx1 + i * xd + xd/2, y1, materialcolor-2 ); // 160 + 15 * mininginfo.avail[ i ].resource( 1 ) / mininginfo.max[ i ].resource( 1 ) );
         line ( hx1 + i * xd + xd/2, y2, hx1 + (i+1) * xd, y2, fuelcolor-2 ); // 160 + 15 * mininginfo.avail[ i ].resource( 2 ) / mininginfo.max[ i ].resource( 2 ) );
      }
      // line ( hx1 + i * xd , ya, hx1 + (i+1) * xd, ya, 16 + 8 * 16 + 4 ); // the maximum absolute amount

   }



   displayvariables();

   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;
   activefontsettings.background = 255;
   activefontsettings.font = schriften.guifont;

   showtext2c ( "dist", subwinx1 + 169-19, subwiny1 + 96 );
   showtext2c ( "dist", subwinx1 + 341-19, subwiny1 + 96 );

   npop ( activefontsettings );

}


void  ccontainer_b :: cmineralresources_subwindow :: displayvariables ( void )
{


   /*
     int i;

      npush ( activefontsettings );
      activefontsettings.color = white;
      activefontsettings.font = schriften.guifont;
      activefontsettings.length = 52;
      activefontsettings.justify = righttext;
      activefontsettings.height = 0;
      activefontsettings.background = 201;

      char c[100];


      if ( mode == 1 )
         strcpy ( c, strrr ( plus.a.material ));
      else
         strcpy ( c, strrr ( plus.a.fuel ));         // aktualle F”rderung

      strcat ( c , " / ");
      strcat ( c, strrr ( cc_b->building->plus.resource[mode] ));         // maximale F”rderung

      showtext2c ( c,               subwinx1 + 63, subwiny1 + 25 );



      int dist = 100;
      for (i = maxminingrange; i >= 0; i-- )
         if ( mininginfo.avail[ i ].resource[ mode ] )
            dist = i;

      strcpy ( c, strrr ( cc_b->building->lastmineddist ));
      strcat ( c , " / ");
      strcat ( c, strrr ( dist ));
      showtext2c ( c ,                                                 subwinx1 + 63, subwiny1 + 43 );


  //    if ( mode == 1 ) {
  //       if ( !cc_b->building->typ->efficiencymaterial )
  //          displaymessage(" the %s has a material efficiency of 0 !", 2, cc_b->building->typ->name );
  //       i = cc_b->building->getmininginfo ( mode ) * 1024 / cc_b->building->typ->efficiencymaterial;
  //    } else {
  //       if ( !cc_b->building->typ->efficiencyfuel )
  //          displaymessage(" the %s has a fuel efficiency of 0 !", 2, cc_b->building->typ->name );
  //       i = cc_b->building->getmininginfo ( mode ) * 1024 / cc_b->building->typ->efficiencyfuel;
  //    }
   */
   /*
      int t = 0;
      if ( i )
          t = mininginfo.avail [ dist ].resource[ mode ] / i;         // in wieviel Runden wird n„chste Entfernung erreicht

      showtext2c ( strrr ( t ),                                        subwinx1 + 63, subwiny1 + 61 );


      if ( mode == 1 )
         i =  cc_b->building->typ->efficiencymaterial;
      else
         i =  cc_b->building->typ->efficiencyfuel;

      showtext2c ( strrr ( i ),                                        subwinx1 + 63, subwiny1 + 79 );

   //   showtext2c ( "energy cost:",     subwinx1 + 8, subwiny1 + 43 );
   //   showtext2c ( "material cost:",   subwinx1 + 8, subwiny1 + 61 );

   */
   /*
      int energy;
      int material;
      returnresourcenuseforresearch ( cc_b->building, cc_b->building->researchpoints, &energy, &material );

      showtext2c ( strrr( cc_b->building->researchpoints ), subwinx1 + 115, subwiny1 + 25 );

      showtext2c ( strrr( energy ), subwinx1 + 115, subwiny1 + 43 );
      showtext2c ( strrr( material ), subwinx1 + 115, subwiny1 + 61 );


      showtext2c ( "avail in:",        subwinx1 + 8, subwiny1 + 79 );

      int rppt = 0;
      pbuilding bld = actmap->player[actmap->actplayer].firstbuilding;
      while ( bld ) {
         rppt += bld->researchpoints;
         bld=bld->next;
      }

      if ( rppt  && actmap->player[actmap->actplayer].research.activetechnology ) {
         showtext2c ( strrr( (actmap->player[actmap->actplayer].research.activetechnology->researchpoints - actmap->player[actmap->actplayer].research.progress + rppt-1) / rppt ),  subwinx1 + 115, subwiny1 + 79 );
      } else
         bar ( subwinx1 + 115, subwiny1 + 79, subwinx1 + 115 + activefontsettings.length, subwiny1 + 79 + activefontsettings.font->height, activefontsettings.background );

      activefontsettings.justify = centertext;
      activefontsettings.length = 22;
      activefontsettings.background = 255;



      paintobj ( 0, 0 );


      returnresourcenuseforresearch ( cc_b->building, cc_b->building->typ->maxresearchpoints, &energy, &material );
      int max;
      if ( energy > material )
         max = energy * 17/16;
      else
         max = material * 17/16;

      int dist = gx2-gx1;
      for (int x = dist; x >0 ; x--) {
          int res = cc_b->building->maxresearchpoints * x / dist;
          returnresourcenuseforresearch ( cc_b->building, res, &energy, &material );


          if ( max ) {
             putpixel ( gx1 + x, gy2 - ( gy2-gy1 ) * energy / max, energycolor );
             putpixel ( gx1 + x, gy2 - ( gy2-gy1 ) * material / max, materialcolor );
          }

      }
      x = gx1 + ( gx2 - gx1 ) * research / 1024;

      line( x, gy1, x, gy2-1, yellow );


      npop ( activefontsettings ); */
}


void  ccontainer_b :: cmineralresources_subwindow :: checkformouse ( void )
{}


void  ccontainer_b :: cmineralresources_subwindow :: paintobj ( int num, int stat )
{}



void  ccontainer_b :: cmineralresources_subwindow :: checkforkey ( tkey taste )
{}


// GUI



ccontainer_b :: BuildProductionLineIcon :: BuildProductionLineIcon ()
{
   filename = "buyprodline";
}

void ccontainer_b :: BuildProductionLineIcon :: loadspecifics( pnstream stream )
{
    infotext = "~B~uy production line";
    priority = 20;
    int w;
    stream->readrlepict ( &picture[0], false, &w );
    keys[0][0] = 'b' ;
}

int  ccontainer_b :: BuildProductionLineIcon :: available ( )
{
   if ( main->unitmode != mproduction )
      return 0;

   if ( main->getmarkedunittype() )
      return 0;

   int count = 0;
   for ( int i = 0; i < 32; ++i )
      if ( cc_b->building->production[i] )
         ++count;

   if ( count >= 18 )
      return 0;

   return 1;
}

void ccontainer_b :: BuildProductionLineIcon :: exec( )
{
   vector<ASCString> list;
   vector<int>       idList;

   Resources r = cc_b->building->getResource( Resources(maxint, maxint, maxint), 1 );
   for ( int i = 0; i < vehicleTypeRepository.getNum(); ++i ) {
      Vehicletype* veh = actmap->getvehicletype_bypos ( i );
      if ( veh ) {
         bool found = false;
         for ( int j = 0; j< 32; ++j )
            if ( cc_b->building->production[j] == veh )
               found = true;


         if ( cc_b->building->typ->vehicleFit ( veh ) && !found )
            if ( cc->baseContainer->vehicleUnloadable(veh) || (cc_b->building->typ->special & cgproduceAllUnitsB ))
               if ( veh->techDependency.available ( actmap->player[actmap->actplayer].research )) {
                  ASCString s;
                  if ( r < veh->productionCost * productionLineConstructionCostFactor  ) {
                     s += "* ";
                     idList.push_back ( -1 );
                  } else
                     idList.push_back ( veh->id );
                  s += veh->getName() + " (E: ";
                  s += strrr ( int(productionLineConstructionCostFactor * veh->productionCost.energy ));
                  s += " M: ";
                  s += strrr ( int(productionLineConstructionCostFactor * veh->productionCost.material ));
                  s += ")";
                  list.push_back ( s );
               }
      }
   }

   if ( idList.size() ) {
      vector<ASCString> buttons;
      buttons.push_back ( "Build" );
      buttons.push_back ( "Cancel" );

      pair<int,int> pos =  chooseString ( "Select Unit Type", list, buttons );
      if ( pos.first == 0 && pos.second >= 0 ) {
         int id = idList[pos.second];
         if ( id >= 0 ) {
            Vehicletype* veh = actmap->getvehicletype_byid ( id );
            int result = cc_b->buildProductionLine.build( veh );
            if ( result < 0 )
               dispmessage2( -result );
            else
               logtoreplayinfo ( rpl_buildProdLine, cc_b->building->getIdentification(), id );
               
            main->setpictures();
            main->movemark (repaint);
            main->repaintresources = 1;
         }
      }
   } else
      displaymessage ( "No Unit Types researched", 1);
}



ccontainer_b :: RemoveProductionLine :: RemoveProductionLine ()
{
   filename = "removeprodline";
}

void ccontainer_b :: RemoveProductionLine :: loadspecifics( pnstream stream )
{
    infotext = "~R~emove production line";
    priority = 10;
    int w;
    stream->readrlepict ( &picture[0], false, &w );
    for ( int i = 0; i< 6; ++i)
       for ( int j = 0; j < 6; ++j )
         keys[i][j] = ct_invvalue;
    keys[0][0] = 'r' ;
}

int  ccontainer_b :: RemoveProductionLine :: available ( )
{
   if ( main->unitmode != mproduction )
      return 0;

   if ( !main->getmarkedunittype() )
      return 0;

   return 1;
}

void ccontainer_b :: RemoveProductionLine :: exec( )
{
   Vehicletype* veh = main->getmarkedunittype();
   if ( veh )
      if (choice_dlg("do you really want to remove this production line ?","~y~es","~n~o") == 1) {
         int result = cc_b->removeProductionLine.remove ( veh );
         if ( result < 0 ) {
            dispmessage2( result );
            return;
         }

         main->setpictures();
         main->movemark (repaint);
         main->repaintresources = 1;
         dashboard.x = 0xffff;
         logtoreplayinfo ( rpl_removeProdLine, cc_b->building->getIdentification(), veh->id );
      }
}


ccontainer_b :: trainuniticon_cb :: trainuniticon_cb ( void )
{
   filename = "training";
};

int   ccontainer_b :: trainuniticon_cb :: available    ( void )
{
   if ( main->unitmode != mnormal )
      return 0;

   pvehicle eht = main->getmarkedunit();
   if ( eht && eht->color == actmap->actplayer * 8 )
      return /*cbuildingcontrols ::*/ ctrainunit :: available ( eht );

   return 0;
}

void  ccontainer_b :: trainuniticon_cb :: exec         ( void )
{
   trainunit ( main->getmarkedunit() );
   dashboard.x = 0xffff;
   main->setactunittogray();
   main->repaintammo = 1;
}



ccontainer_b :: dissectuniticon_cb :: dissectuniticon_cb ( void )
{
   filename = "sezieren";
};

int   ccontainer_b :: dissectuniticon_cb :: available    ( void )
{
   pvehicle eht = main->getmarkedunit();
   if ( eht && eht->color == actmap->actplayer * 8)
      return /*cbuildingcontrols ::*/ cdissectunit :: available ( eht );

   return 0;
}

void  ccontainer_b :: dissectuniticon_cb :: exec         ( void )
{
   pvehicle eht = main->getmarkedunit();
   dissectunit ( eht );
   main->movemark (repaint);
   dashboard.x = 0xffff;
}



void  ccontainer_b :: fill_dialog_icon_cb :: exec         ( void )
{
   // verlademunition ( main->getmarkedunit(), NULL, cc_b->building, 0 );
   dashboard.x = 0xffff;
   main->repaintresources = 1;
}



int   ccontainer_b :: fill_icon_cb :: available    ( void )
{
   pvehicle eht = main->getmarkedunit();
   if ( eht && eht->color == actmap->actplayer * 8) {
      if ( eht->getTank().material < eht->typ->tank.material )
         return 1;
      if ( eht->getTank().fuel < eht->typ->tank.fuel )
         return 1;
      for (int i = 0; i < eht->typ->weapons.count; i++)
         if ( eht->typ->weapons.weapon[ i ].requiresAmmo() )
            if ( eht->ammo[i] < eht->typ->weapons.weapon[ i ].count )
               return 1;
   }


   return 0;
}




ccontainer_b :: produceuniticon_cb :: produceuniticon_cb ( void )
{
   filename = "produnit";
};

int   ccontainer_b :: produceuniticon_cb :: available    ( void )
{
   if ( cc_b->building->color == actmap->actplayer * 8 ) {
      if ( main->unitmode == mnormal ) {
         if ( cc_b->building->vehiclesLoaded() >= min ( 32, cc_b->building->baseType->maxLoadableUnits ))
            return 0;

         if ( main->getspecfunc ( mbuilding ) & cgvehicleproductionb ) {
            pvehicle eht = main->getmarkedunit();
            if ( eht )
               return 0;
            else
               return 1;

         }
      } else
         if ( main->getmarkedunittype() )
            if (  cproduceunit :: available ( main->getmarkedunittype() ) )
               return 1;
   }
   return 0;
}

void  ccontainer_b :: produceuniticon_cb :: exec         ( void )
{
   if ( main->unitmode == mnormal ) {

      main->unitmode = mproduction;
      main->unitchanged();

   } else {

      produce  ( main->getmarkedunittype() );
      main->unitmode = mnormal;

   }
   main->setpictures();
   main->movemark ( repaintall );
   dashboard.x = 0xffff;
}


const char* ccontainer_b :: produceuniticon_cb :: getinfotext  ( void )
{
   pvehicletype fzt = main->getmarkedunittype();
   if ( fzt ) {
      int en = fzt->productionCost.energy;
      int ma = fzt->productionCost.material;
      int fu = 0;

      if ( CGameOptions::Instance()->container.filleverything ) {
         int en1 = en;
         int ma1 = ma;
         int fu1 = fu;

         fu += fzt->tank.fuel;
         ma += fzt->tank.material;

         strcpy ( infotextbuf, infotext.c_str() );
         sprintf ( &infotextbuf[strlen( infotextbuf)], resourceusagestring, en, ma, fu );
         sprintf ( &infotextbuf[strlen( infotextbuf)], "(empty: %d energy, %d material, %d fuel)", en1, ma1, fu1 );
      } else {
         strcpy ( infotextbuf, infotext.c_str() );
         sprintf ( &infotextbuf[strlen( infotextbuf)], resourceusagestring, en, ma, fu );
      }
      return infotextbuf;
   } else {
      return infotext.c_str();
   }
}





ccontainer :: productioncancelicon_cb :: productioncancelicon_cb ( void )
{
   filename = "cancel";
};

int   ccontainer :: productioncancelicon_cb :: available    ( void )
{
   if ( main->unitmode != mnormal )
      return 1;
   return 0;
}

void  ccontainer :: productioncancelicon_cb :: exec         ( void )
{
   main->unitmode = mnormal;
   main->setpictures();
   main->movemark ( repaintall );
   dashboard.x = 0xffff;
}




ccontainer_b :: recyclingicon_cb :: recyclingicon_cb ( void )
{
   filename = "c_recycl";
};

int  ccontainer_b :: recyclingicon_cb :: available ( void )
{
   pvehicle eht = main->getmarkedunit ();
   if ( eht && eht->color == actmap->actplayer * 8 )
      return 1;
   else
      return 0;
};

void    ccontainer_b :: recyclingicon_cb :: exec ( void )
{
   if (choice_dlg("do you really want to recycle this unit ?","~y~es","~n~o") == 1) {
      recycle ( main->getmarkedunit() );
      main->movemark (repaint);
      dashboard.x = 0xffff;
   }
};



const char* ccontainer_b :: recyclingicon_cb :: getinfotext  ( void )
{
   Resources res = resourceuse ( main->getmarkedunit() );
   strcpy ( &infotextbuf[100], infotext.c_str() );
   int n = 0;
   sprintf ( &infotextbuf[100+strlen( &infotextbuf[100])], resourceusagestring, res.energy, -res.material, n );
   return &infotextbuf[100];
}






ccontainer_b :: takeofficon_cb :: takeofficon_cb             ( void )
{
   filename = "takeoff";
}


int   ccontainer_b :: takeofficon_cb :: available    ( void )
{
   return true;
}

void  ccontainer_b :: takeofficon_cb :: exec         ( void )
{
   return;
}











/***********************************************************************************/
/*                                                                                 */
/*   class    ccontainer_t : Geb„ude-Innereien                                     */
/*                                                                                 */
/***********************************************************************************/








ccontainer_t :: ctransportinfo_subwindow :: ctransportinfo_subwindow ( void )
{
   strcpy ( name, "transport info" );
   laschpic1 = icons.container.lasche.a.transportinfo[0];
   laschpic2 = icons.container.lasche.a.transportinfo[1];

   helplist.num = 4;

   static tonlinehelpitem transportinfohelpitems[4]    =
        {{{246 + subwinx1 , 22 + subwiny1 , 266 + subwinx1,108 + subwiny1}, 20130 },
         {{277 + subwinx1 , 22 + subwiny1 , 297 + subwinx1,108 + subwiny1}, 20131 },
         {{308 + subwinx1 , 22 + subwiny1 , 328 + subwinx1,108 + subwiny1}, 20132 },
         {{subwinx1 + 70,  subwiny1 + 33, subwinx1 + 200,  subwiny1 + 41},  20133 }};


   helplist.item = transportinfohelpitems;

}

int  ccontainer_t :: ctransportinfo_subwindow :: subwin_available ( void )
{
   csubwindow :: subwin_available ( );
   if ( next )
      next->subwin_available ();

   return 0;
}

void  ccontainer_t :: ctransportinfo_subwindow :: display ( void )
{
   collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );

   npush ( activefontsettings );
   putimage ( subwinx1, subwiny1, icons.container.subwin.transportinfo.start );

   csubwindow :: display();

   /* ####TRANS
   for ( int i = 0; i < 8; i++ ) {

      if ( cc_t->vehicle->typ->loadcapability & ( 1 << ( 7 - i) ))
         putimage ( subwinx1 + 246, subwiny1 + 22 + i * 11, icons.container.subwin.transportinfo.height1[i] );
      else
         putimage ( subwinx1 + 246, subwiny1 + 22 + i * 11, icons.container.subwin.transportinfo.height2[i] );

      if ( cc_t->vehicle->typ->loadcapabilityreq & ( 1 << ( 7 - i ) ))
         putimage ( subwinx1 + 277, subwiny1 + 22 + i * 11, icons.container.subwin.transportinfo.height1[i] );
      else
         putimage ( subwinx1 + 277, subwiny1 + 22 + i * 11, icons.container.subwin.transportinfo.height2[i] );

      if ( cc_t->vehicle->typ->loadcapabilitynot & ( 1 << ( 7 - i ) ))
         putimage ( subwinx1 + 308, subwiny1 + 22 + i * 11, icons.container.subwin.transportinfo.height1[i] );
      else
         putimage ( subwinx1 + 308, subwiny1 + 22 + i * 11, icons.container.subwin.transportinfo.height2[i] );
   }
   */
   activefontsettings.font = schriften.guifont;
   activefontsettings.justify = lefttext;
   activefontsettings.length = 0;
   showtext3c ( "~Transport:~",  subwinx1 + 8,  subwiny1 + 25 );
   showtext2c ( "current load:",        subwinx1 + 70,  subwiny1 + 25 );
   showtext2c ( "max (single / total):",        subwinx1 + 70,  subwiny1 + 33 );
   showtext2c ( "free:",            subwinx1 + 70,  subwiny1 + 41 );


   showtext3c ( "~Loaded Unit:~",   subwinx1 + 8,   subwiny1 + 62 );

   showtext2c ( "unladen weight:",  subwinx1 + 70,  subwiny1 + 62 );
   showtext2c ( "material:",        subwinx1 + 70,  subwiny1 + 70 );
   showtext2c ( "fuel:",            subwinx1 + 70,  subwiny1 + 78 );
   showtext2c ( "loaded units:",    subwinx1 + 70,  subwiny1 + 86 );
   putimage ( subwinx1 + 70,  subwiny1 + 97, icons.container.subwin.transportinfo.sum );

   paintvariables();

   npop ( activefontsettings );
}

void ccontainer_t :: ctransportinfo_subwindow :: paintvariables ( void )
{
   npush ( activefontsettings );

   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 30;
   activefontsettings.justify = righttext;
   activefontsettings.background = 201;

   pvehicle eht = cc_t->vehicle;
   int mass = eht->cargo();
   int free = eht->typ->maxLoadableWeight - mass;

   activefontsettings.length = 40;

   char buf[50];
   sprintf(buf, "%d / %d", cc_t->vehicle->typ->maxLoadableUnitSize, cc_t->vehicle->typ->maxLoadableWeight );

   showtext2c ( strrr ( mass ),                          subwinx1 + 170,  subwiny1 + 25 );
   showtext2c ( buf,                                     subwinx1 + 170,  subwiny1 + 33 );
   showtext2c ( strrr ( free ),                          subwinx1 + 170,  subwiny1 + 41 );

   eht = hostcontainer->getmarkedunit();
   if ( eht ) {
      showtext2c ( strrr ( eht->typ->weight ),                       subwinx1 + 170,  subwiny1 +  62 );
      showtext2c ( "-",  subwinx1 + 170,  subwiny1 +  70 );
      showtext2c ( "-",  subwinx1 + 170,  subwiny1 +  78 );
      showtext2c ( strrr ( eht->cargo() ),                           subwinx1 + 170,  subwiny1 +  86 );
      showtext2c ( strrr ( eht->weight() ),                          subwinx1 + 170,  subwiny1 +  96 );
   } else {
      showtext2c ( "-",                       subwinx1 + 170,  subwiny1 +  62 );
      showtext2c ( "-",                       subwinx1 + 170,  subwiny1 +  70 );
      showtext2c ( "-",                       subwinx1 + 170,  subwiny1 +  78 );
      showtext2c ( "-",                       subwinx1 + 170,  subwiny1 +  86 );
      showtext2c ( "-",                       subwinx1 + 170,  subwiny1 +  96 );
   }
   line ( subwinx1 + 70,  subwiny1 +  95 , subwinx1 + 210,  subwiny1 +  95 , lightgray );


   npop ( activefontsettings );
}

void ccontainer_t :: ctransportinfo_subwindow :: unitchanged ( void )
{
   collategraphicoperations cgo ( subwinx1, subwiny1, subwinx2, subwiny2 );
   paintvariables();
}

void ccontainer_t :: ctransportinfo_subwindow :: checkformouse ( void )
{}


void ccontainer_t :: ctransportinfo_subwindow :: checkforkey ( tkey taste )
{}

//............................................................................................









ccontainer_t :: ccontainer_t ( void )
{
   oldguihost = actgui;
   actgui = &hosticons_ct;
   actgui->restorebackground();
   unitmode = mnormal;
}

void  ccontainer_t :: init (pvehicle eht)
{
   hosticons_ct.init ( hgmp->resolutionx, hgmp->resolutiony );
   hosticons_ct.seticonmains ( this );
   hosticons_ct.starticonload();


   if ( eht ) {
      int mss = getmousestatus();
      if ( mss == 2 )
         mousevisible ( false );

      ctransportcontrols :: init ( eht );

      setpictures();

      /*
            int x = vehicle->xpos;
            int y = vehicle->ypos;
      */
      /*
            cursor.hide ();
            cursor.posx = x - actmap->xpos;
            cursor.posy = y - actmap->ypos;
            cursor.show ();
      */

      ccontainer :: init ( vehicle->typ->picture[0], vehicle->color, vehicle->name.empty()? vehicle->typ->name: vehicle->name, vehicle->typ->description );
      ccontainer :: displayloading ();
      ccontainer :: movemark (repaint);

      subwindows.ammunitiontransfer.sethostcontainer ( this );
      subwindows.ammunitiontransfer.init();

      if ( mss == 2 )
         mousevisible ( true );
   };
};



pvehicle    ccontainer_t :: getmarkedunit (void)
{
   if ( unitmode == mnormal || unitmode == mloadintocontainer)
      return vehicle->loading[mark.y*unitsshownx + mark.x];
   else
      return NULL;
}



pvehicle ctransportcontrols :: getloadedunit (int num)
{
   return vehicle->loading[num];
};

pvehicle ccontainer_t :: getloadedunit (int num)
{
   if ( unitmode == mnormal || unitmode == mloadintocontainer )
      return vehicle->loading[num];
   else
      return NULL;
};



ccontainer_t :: ~ccontainer_t ( )
{
   actgui = oldguihost ;
}


int    ccontainer_t :: putammunition (int  weapontype, int  ammunition, int abbuchen)
{
   if ( abbuchen )
      repaintammo = 1;

   return ctransportcontrols :: putammunition ( weapontype, ammunition, abbuchen );
}


int    ccontainer_t :: getammunition ( int weapontype, int num, int abbuchen, int produceifrequired  )
{
   if ( abbuchen )
      repaintammo = 1;

   return ctransportcontrols :: getammunition ( weapontype, num, abbuchen, produceifrequired  );
}

// Gui


void  ccontainer_t :: chosticons_ct :: init ( int resolutionx, int resolutiony )
{
   chainiconstohost ( &icons.movement );     //   muá erst eingesetzt werden !
   GuiHost<generalicon_c*>::init ( resolutionx, resolutiony );
}








// --------------- GUI ----------------------------------------------------------------------------------







void  ccontainer_t :: fill_dialog_icon_ct :: exec         ( void )
{
   // verlademunition ( main->getmarkedunit(), cc_t->vehicle, NULL, 0 );
   dashboard.x = 0xffff;
   main->repaintresources = 1;
}




int   ccontainer_t :: fill_icon_ct :: available    ( void )
{
   if ( main->unitmode != mnormal )
      return 0;


   pvehicle eht = main->getmarkedunit();
   if ( eht && eht->color == actmap->actplayer * 8) {
      if ( eht->getTank().material < eht->typ->tank.material )
         return 1;
      if ( eht->getTank().fuel < eht->typ->tank.fuel )
         return 1;
      for (int i = 0; i < eht->typ->weapons.count; i++)
         if ( eht->typ->weapons.weapon[ i ].requiresAmmo() )
            if ( eht->ammo[i] < eht->typ->weapons.weapon[ i ].count )
               for (int j = 0; j < cc_t->vehicle->typ->weapons.count; j++)
                  if ( cc_t->vehicle->typ->weapons.weapon[ j ].requiresAmmo() )
                     if ( cc_t->vehicle->ammo[j] )
                        if ( eht->typ->weapons.weapon[ i ].getScalarWeaponType() ==  cc_t->vehicle->typ->weapons.weapon[ j ].getScalarWeaponType() )
                           return 1;
   }


   return 0;
}










//------------------------------------------------------------------------------- GENERAL_ICON


generalicon_c::generalicon_c ( void )
{
   first = &buildingparamstack[recursiondepth].generalicon_c__first;

   setnxt( *first );
   *first = this;
}


pnguiicon   generalicon_c::frst( void )
{
   return *first;
}

void        generalicon_c::setfrst  ( pnguiicon ts )
{
   *first = (pgeneralicon_c) ts;
}


void        generalicon_c::setmain ( pcontainer maintemp )
{
   main = maintemp;
   if ( nxt() ) {
      generalicon_c* ne = (generalicon_c*) nxt();
      ne->setmain ( maintemp );
   }
}

generalicon_c:: ~generalicon_c ( )
{
   first = NULL;
}

void tcontaineronlinemousehelp :: checkforhelp ( void )
{
   if ( CGameOptions::Instance()->onlinehelptime )
      if ( (ticker > lastmousemove+CGameOptions::Instance()->onlinehelptime  && mouseparams.taste == 0 ) || mouseparams.taste == 2 )
         if ( active == 1 )
            if ( hostcontainer->actsubwindow )
               if ( hostcontainer->actsubwindow->helplist.num )
                  checklist ( &hostcontainer->actsubwindow->helplist );


}

tcontaineronlinemousehelp :: tcontaineronlinemousehelp ( pcontainer host )
{
   hostcontainer = host;
}

