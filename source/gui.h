//     $Id: gui.h,v 1.22 2002-09-19 20:20:05 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.21  2001/07/13 12:53:01  mbickel
//      Fixed duplicate icons in replay
//      Fixed crash in tooltip help
//
//     Revision 1.20  2001/02/04 21:26:56  mbickel
//      The AI status is written to savegames -> new savegame revision
//      Lots of bug fixes
//
//     Revision 1.19  2001/01/28 14:04:14  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.18  2001/01/24 11:53:13  mbickel
//      Fixed some compilation problems with gcc
//
//     Revision 1.17  2000/12/21 11:00:50  mbickel
//      Added some code documentation
//
//     Revision 1.16  2000/10/18 14:14:13  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//
//     Revision 1.15  2000/10/11 14:26:40  mbickel
//      Modernized the internal structure of ASC:
//       - vehicles and buildings now derived from a common base class
//       - new resource class
//       - reorganized exceptions (errors.h)
//      Split some files:
//        typen -> typen, vehicletype, buildingtype, basecontainer
//        controls -> controls, viewcalculation
//        spfst -> spfst, mapalgorithm
//      bzlib is now statically linked and sources integrated
//
//     Revision 1.14  2000/08/30 16:26:59  mbickel
//     *** empty log message ***
//
//     Revision 1.13  2000/08/30 14:45:08  mbickel
//      ASC compiles and links with GCC again...
//
//     Revision 1.12  2000/08/29 20:21:07  mbickel
//      Tried to make source GCC compliant, but some problems still remain
//
//     Revision 1.11  2000/08/29 17:42:44  mbickel
//      Restructured GUI to make it compilable with VisualC.
//
//     Revision 1.10  2000/08/12 15:01:42  mbickel
//      Restored old versions of GUI ; new ones were broken
//
//     Revision 1.8  2000/06/08 21:03:41  mbickel
//      New vehicle action: attack
//      wrote documentation for vehicle actions
//
//     Revision 1.7  2000/05/02 16:20:54  mbickel
//      Fixed bug with several simultaneous vehicle actions running
//      Fixed graphic error at ammo transfer in buildings
//      Fixed ammo loss at ammo transfer
//      Movecost is now displayed for mines and repairs
//      Weapon info now shows unhittable units
//
//     Revision 1.6  2000/04/27 16:25:24  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.5  2000/03/11 18:22:06  mbickel
//      Added support for multiple graphic sets
//
//     Revision 1.4  2000/01/02 19:47:07  mbickel
//      Continued Linux port
//      Fixed crash at program exit
//
//     Revision 1.3  1999/12/28 21:02:58  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.2  1999/11/16 03:41:49  tmwilson
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

#ifdef karteneditor          
#error eigentlich sollte der Karteneditor ohne die Gui auskommen kînnen !
#endif


#ifndef guiH
#define guiH

#include "basestrm.h"
#include "spfst.h"
#include "unitctrl.h"
#include "ascstring.h"

const int guismalliconsizex = 24;
const int guismalliconsizey = 17;
const int guismallicongap = 3;

struct tnguiiconfiledata {
          void*     picture[8];
          void*     picturepressed[8];
          char*     infotext;
          tkey      keys[6][6];
          int       priority;
       };


typedef class tnguiicon* pnguiicon;

typedef class tnputbuildingguiicon* pnputbuildingguiicon; 
typedef class tnputobjectcontainerguiicon* pnputobjectcontainerguiicon; 
typedef class tnweapselguiicon* pnweapselguiicon;

class BasicGuiHost {
      protected:
        static void*     background[30][30];
        static int    numpainted;

      public:
        virtual void   returncoordinates ( void* icn, int* x, int * y ) = 0;
        int paintsize;  // 0 : normale grî·e; 1: klein
        virtual void   cleanup ( void ) = 0;    // wird zum entfernen der kleinen guiicons aufgerufen, bevor das icon ausgefÅhrt wird
        virtual int    painticons ( void ) = 0;
        virtual void   paintsmallicons ( int taste, int up ) = 0;
        virtual void   restorebackground ( void ) = 0;
        virtual void   checkforkey ( tkey key ) = 0;
        virtual void   checkformouse ( void ) = 0;
        virtual void   runpressedmouse ( int taste ) = 0;
        virtual void   savebackground ( void ) = 0;
        virtual void   starticonload ( void ) = 0;
        virtual void   init ( int resolutionx, int resolutiony ) = 0;
        virtual ~BasicGuiHost ( ) {};
   };

template <class T>
class GuiHost : public BasicGuiHost {
      protected:
        T first_icon;

        int columncount;
        int iconspaintable;
        int  firstshownline;
        T actshownicons[3][30];

        int       firstpaint;
        pnguiicon infotextshown;
        virtual void      bi2control (  );
        virtual T getfirsticon();
        virtual void      setfirsticon( T ic );
        void putbackground ( int xx , int yy );

        struct {
                  int x, y;
                  void *buf;
                  int xsize;
              } smalliconpos;

      public:

        virtual void   returncoordinates ( void* icn, int* x, int * y );
        GuiHost ( void );

        void   starticonload ( void );

        virtual void   checkforkey ( tkey key );
        virtual void   checkformouse ( void );
        void   savebackground ( void );
        virtual int    painticons ( void );
        virtual void   paintsmallicons ( int taste, int up );
        void   chainiconstohost ( T icn );
        void   runpressedmouse ( int taste );
        void   cleanup ( void ) ;    // wird zum entfernen der kleinen guiicons aufgerufen, bevor das icon ausgefÅhrt wird
        virtual void   restorebackground ( void );

        void   reset ( void );
        
        virtual void init ( int resolutionx, int resolutiony );
   };




typedef class tselectbuildingguihost* pselectbuildingguihost;
extern tselectbuildingguihost    selectbuildinggui;

class SelectBuildingBaseGuiHost: public GuiHost<pnputbuildingguiicon> { int foo; } ;
class tselectbuildingguihost : public SelectBuildingBaseGuiHost {

       protected:
         pnputbuildingguiicon*   icons;

       public:
         tselectbuildingguihost( void );

         void              init ( int resolutionx, int resolutiony );

         pbuildingtype selectedbuilding;
         pvehicle vehicle;
         int cancel;
         void reset ( pvehicle v );
   };





typedef class tselectobjectcontainerguihost *pselectobjectcontainerguihost;

class SelectObjectBaseGuiHost : public GuiHost<pnputobjectcontainerguiicon> {int foo;} ;
class tselectobjectcontainerguihost : public SelectObjectBaseGuiHost {
         pnputobjectcontainerguiicon*   icons;

       public:
         void              init ( int resolutionx, int resolutiony );

         int cancel;
         void reset ( void );
   };



typedef class tselectvehiclecontainerguihost *pselectvehiclecontainerguihost;

typedef class tnputvehiclecontainerguiicon *pnputvehiclecontainerguiicon;


class SelectVehicleBaseGuiHost: public GuiHost<pnputvehiclecontainerguiicon> {int foo;} ;

class tselectvehiclecontainerguihost : public SelectVehicleBaseGuiHost {
         pnputvehiclecontainerguiicon*   icons;

       public:
         pvehicle constructingvehicle;
         void              init ( int resolutionx, int resolutiony );

         int cancel;
         void reset ( pvehicle _constructingvehicle );
   };






class tnguiicon {
          pnguiicon next;
        protected:

          void*     picture[8];
          void*     picturepressed[8];
          ASCString    infotext;
          tkey      keys[6][6];
          int       priority;
          int       lasticonsize;


          BasicGuiHost*  host;
          std::string    filename;
          int       x,y;

          virtual pnguiicon nxt      ( void )          ;
          virtual void      setnxt   ( pnguiicon ts )  ;
          virtual void      setfrst  ( pnguiicon ts )  = 0;
          
          // friend void template class<T> GuiHost::chainiconstohost ( pnguiicon icn  );

          void putpict ( void* buf );
                                                    

        public:
          virtual pnguiicon frst     ( void )          = 0;

          virtual int   available    ( void ) = 0;
          virtual void  exec         ( void ) = 0;
          virtual void  display      ( void );
          virtual const char* getinfotext  ( void );

          void          loaddata     ( void );
          void          paintifavail ( void );
          void          seticonsize  ( int size );
          int           count        ( void );
          virtual void  loadspecifics( pnstream stream );
          virtual void  sethost      ( BasicGuiHost* hst );

          virtual void  checkforkey  ( tkey key );
          virtual int   pressedbymouse ( void );
          virtual void  iconpressed  ( void );
          void          sort         ( pnguiicon last );

          tnguiicon ( void );
          virtual ~tnguiicon ( );
};

class tnweapselguiicon : public tnguiicon {
        protected:
          static pnweapselguiicon first;

          virtual void      setfrst  ( pnguiicon ts );

          int iconnum;
          int weapnum;
          int typ;
          int strength;
       public:
          virtual pnguiicon frst     ( void );

          virtual int   available    ( void );
          virtual void  exec         ( void );
          virtual const char* getinfotext  ( void );
          virtual void  checkforkey  ( tkey key );
          virtual void  setup        ( pattackweap atw, int n );
                                                                                       
          tnweapselguiicon ( void );
};


class SelectWeaponBaseGuiHost : public GuiHost<pnweapselguiicon> {int foo;} ;

class tselectweaponguihost : public SelectWeaponBaseGuiHost {
         tnweapselguiicon   icon[20];
         pattackweap        atw;
         int                x,y;

       public:
         tselectweaponguihost( void );

         void              init ( int resolutionx, int resolutiony );
         virtual int       painticons ( void );
         virtual void      checkforkey ( tkey key );
         virtual void      checkformouse ( void );
         virtual void      checkcoordinates ( void ); 
   };


class tnputbuildingguiicon : public tnguiicon {
            static int             buildnum;
          protected:
            static pnputbuildingguiicon first;

            virtual void      setfrst  ( pnguiicon ts );

            pbuildingtype           building;

           friend void tselectbuildingguihost ::init( int resolutionx, int resolutiony );

          public:
            virtual pnguiicon frst     ( void );

            tnputbuildingguiicon ( pbuildingtype bld );
            int   available    ( void );
            void  exec         ( void );
         };




class tnputobjectcontainerguiicon : public tnguiicon {
            int build;
          protected:
            static pnputobjectcontainerguiicon first;
            static int             buildnum;

            virtual void      setfrst  ( pnguiicon ts );
            pobjecttype           object;

           friend void tselectobjectcontainerguihost ::init( int resolutionx, int resolutiony );

          public:
            virtual pnguiicon frst     ( void );

            tnputobjectcontainerguiicon ( pobjecttype obj, int bld );
            int   available    ( void );
            void  exec         ( void );
            int forcedeneable;
         };

class tnputvehiclecontainerguiicon : public tnguiicon {
            static pnputvehiclecontainerguiicon first;
            static int             buildnum;
          protected:
            virtual void      setfrst  ( pnguiicon ts );
            pvehicletype           vehicle;

           friend void tselectvehiclecontainerguihost ::init( int resolutionx, int resolutiony );

          public:
            virtual pnguiicon frst     ( void );

            tnputvehiclecontainerguiicon ( pvehicletype obj );
            int   available    ( void );
            void  exec         ( void );
            int forcedeneable;
         };



typedef class tnsguiicon* pnsguiicon; 
class tnsguiicon : public tnguiicon {
            static pnsguiicon first;
          protected:
            virtual void      setfrst  ( pnguiicon ts );

          public:
            virtual pnguiicon frst     ( void );

            tnsguiicon ( void );
         };




class tnsguiiconmove : public tnsguiicon {
          VehicleMovement vehicleMovement;
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;
          virtual void  display      ( void ) ;

          tnsguiiconmove ( void );
       };


class tnsguiiconattack : public tnsguiicon {
          VehicleAttack vehicleAttack;
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiiconattack ( void );
       };

class tnsguiiconascent : public tnsguiicon {
          IncreaseVehicleHeight increaseVehicleHeight;
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;
          virtual void  display      ( void ) ;

          tnsguiiconascent ( void );
       };

class tnsguiicondescent : public tnsguiicon {
          DecreaseVehicleHeight decreaseVehicleHeight;
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;
          virtual void  display      ( void ) ;

          tnsguiicondescent ( void );
       };

class tnsguiiconinformation : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;
          virtual void  display      ( void ) ;

          tnsguiiconinformation ( void );
       };

class tnsguiiconendturn : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiiconendturn ( void );
       };

class tnsguiiconexternalloading : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiiconexternalloading ( void );
       };

class tnsguiiconpoweroff : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiiconpoweroff ( void );
       };
class tnsguiiconpoweron : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiiconpoweron ( void );
       };


class tnsguiiconconstructvehicle : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiiconconstructvehicle ( void );
       };

class tnsguiiconputmine : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;
          virtual void  loadspecifics( pnstream stream );
          tnsguiiconputmine ( void );
       };

class tnsguiiconputgroundmine  : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
};
class tnsguiiconputantitankmine : public tnsguiiconputgroundmine {
        public:
          virtual void  exec         ( void ) ;
          tnsguiiconputantitankmine ( void );
       };
class tnsguiiconputantipersonalmine : public tnsguiiconputgroundmine {
        public:
          virtual void  exec         ( void ) ;
          tnsguiiconputantipersonalmine ( void );
       };



class tnsguiiconputseamine : public tnsguiicon {
           public:
             virtual int   available    ( void ) ;
      };

class tnsguiiconputfloatingmine : public tnsguiiconputseamine {
        public:
          virtual void  exec         ( void ) ;
          tnsguiiconputfloatingmine ( void );
       };
class tnsguiiconputmooredmine : public tnsguiiconputseamine {
        public:
          virtual void  exec         ( void ) ;
          tnsguiiconputmooredmine ( void );
       };


class tnsguiiconremovemine : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiiconremovemine       ( void );
       };




class tnsguiiconbuildany : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiiconbuildany ( void );
       };


class tnsguiiconrepair : public tnsguiicon {
          VehicleService service;
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;
          virtual void  loadspecifics( pnstream stream );

          tnsguiiconrepair ( void );
       };


class tnsguiiconrefuel : public tnsguiicon {
          VehicleService service;
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;
          virtual void  display      ( void ) ;

          tnsguiiconrefuel ( void );
       };

class tnsguiiconrefueldialog : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiiconrefueldialog ( void );
       };


class tnsguiiconputbuilding : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiiconputbuilding ( void );
       };

class tnsguiicondestructbuilding : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiicondestructbuilding ( void );
       };


class tnsguiicondig : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiicondig ( void );
       };

class tnsguiiconviewmap : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiiconviewmap ( void );
       };

class tnsguiiconenablereactionfire : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiiconenablereactionfire ( void );
       };

class tnsguiicondisablereactionfire : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiicondisablereactionfire ( void );
       };


class tnsguiiconcontainer : public tnsguiicon {
        static int containeractive;
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiiconcontainer ( void );
       };

class tnsguiiconcancel : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;
          int forcedeneable;

          tnsguiiconcancel ( void );
       };






class StandardBaseGuiHost : public GuiHost<pnsguiicon> {int foo;} ;

class tguihoststandard : public StandardBaseGuiHost {
   protected:
        virtual void      bi2control (  );

   public: 
     void init ( int resolutionx, int resolutiony );

     struct I{
        tnsguiiconmove                 movement;
        tnsguiiconattack               attack;
        tnsguiicondescent              descent; 
        tnsguiiconascent               ascent;
        tnsguiiconinformation          information;
        tnsguiiconendturn              endturn;
        tnsguiiconputmine              putmine;
        tnsguiiconputantitankmine      putantitankmine;
        tnsguiiconputantipersonalmine  putantipersonalmine;
        tnsguiiconputfloatingmine      putfloatingmine;
        tnsguiiconputmooredmine        putmooredmine;
        tnsguiiconremovemine           removemine;
        tnsguiiconbuildany             buildany;
        tnsguiiconrepair               repair;
        tnsguiiconrefuel               refuel;
        tnsguiiconrefueldialog         refueldialog;
        tnsguiiconputbuilding          putbuilding;
        tnsguiicondestructbuilding     destructbuilding;
        tnsguiicondig                  dig;
        tnsguiiconviewmap              viewmap;
        tnsguiiconcontainer            container;
        tnsguiiconenablereactionfire   reactionfireon;
        tnsguiicondisablereactionfire  reactionfireoff;
        tnsguiiconconstructvehicle     constructvehicle;
        tnsguiiconexternalloading      externalloading;
        tnsguiiconpoweroff             poweroff;
        tnsguiiconpoweron              poweron;

        tnsguiiconcancel               cancel;
     } icons;

   };






typedef class treplayguiicon* preplayguiicon; 
class treplayguiicon : public tnguiicon {
            static preplayguiicon first;
          protected:
            virtual void      setfrst  ( pnguiicon ts );

          public:
            virtual pnguiicon frst     ( void );

            treplayguiicon ( void );
         };




typedef class ccontainer* pcontainer;
typedef class generalicon_c*   pgeneralicon_c;
class generalicon_c : public tnguiicon {       // f?r Container //grundlage f¸r jedes einzelne icon
                        pgeneralicon_c *first;
                    protected:
                        char infotextbuf[1000];
                        pcontainer main;
                        virtual void      setfrst  ( pnguiicon ts );

                    public:
                        virtual pnguiicon frst     ( void );
                        void setmain ( pcontainer maintemp );
                        generalicon_c ( void );
                        ~generalicon_c ();
                };




class trguiicon_play : public treplayguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          trguiicon_play ( void );
       };

class trguiicon_pause : public treplayguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          trguiicon_pause ( void );
       };

class trguiicon_faster : public treplayguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          trguiicon_faster ( void );
       };

class trguiicon_slower : public treplayguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          trguiicon_slower ( void );
       };

class trguiicon_back : public treplayguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          trguiicon_back ( void );
       };

class trguiicon_exit : public treplayguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          trguiicon_exit ( void );
       };

class trguiicon_cancel : public treplayguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          int forcedeneable;
          trguiicon_cancel ( void );
       };



class ReplayBaseGuiHost: public GuiHost<preplayguiicon> {int foo;} ;
class treplayguihost : public ReplayBaseGuiHost {
   protected:
        virtual void      bi2control (  );

   public: 
     void init ( int resolutionx, int resolutiony );

     struct I {
        trguiicon_play   play;
        trguiicon_pause  pause;
        trguiicon_slower slower;
        trguiicon_faster faster;
        trguiicon_back   back;
        trguiicon_exit   exit;
        trguiicon_cancel cancel;
     } icons;
     treplayguihost ( ) {};
   };

extern treplayguihost replayGuiHost;

class ContainerBaseGuiHost : public GuiHost<pgeneralicon_c> {
         public:
            ContainerBaseGuiHost ( void ) { chainiconstohost(NULL); }; // just a hack to avoid some gcc bugs; chainiconstohost wouldn't be instantiated else ...
    };


extern tguihoststandard          gui;
extern tselectbuildingguihost    selectbuildinggui;
extern tselectobjectcontainerguihost      selectobjectcontainergui;
extern tselectvehiclecontainerguihost     selectvehiclecontainergui;
extern tselectweaponguihost      selectweaponguihost;
extern BasicGuiHost*                 actgui;
extern void setguiposy ( int y );

#endif
