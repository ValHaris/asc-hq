//     $Id: gui.h,v 1.3 1999-12-28 21:02:58 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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


#ifndef gui_h
#define gui_h

#include "basestrm.h"
#include "keybp.h"
#include "basegui.h"
#include "spfst.h"

   #define guismalliconsizex 24
   #define guismalliconsizey 17  
   #define guismallicongap 3


typedef class tguihost* pguihost;
typedef class tnguiicon* pnguiicon;

typedef class tnputbuildingguiicon* pnputbuildingguiicon; 
typedef class tnputobjectcontainerguiicon* pnputobjectcontainerguiicon; 
typedef class tnweapselguiicon* pnweapselguiicon;



class tguihost {
        pnguiicon first_icon;
      protected:
        int columncount;
        int iconspaintable;
        int  firstshownline;
        pnguiicon actshownicons[3][30];
        static void*     background[30][30];

        static int    numpainted;
        int       firstpaint;
        pnguiicon infotextshown;
        virtual void      bi2control (  );
        virtual pnguiicon getfirsticon();
        virtual void      setfirsticon( pnguiicon ic );
        void putbackground ( int xx , int yy );

        struct {
                  int x, y;
                  void *buf;
                  int xsize;
              } smalliconpos;

      public:
        int paintsize;  // 0 : normale grî·e; 1: klein

        tguihost ( void );
        virtual ~tguihost ( ) {};

        void   starticonload ( void );
        void   returncoordinates ( pnguiicon icon, int* x, int * y );

        virtual void   checkforkey ( tkey key );
        virtual void   checkformouse ( void );
        void   savebackground ( void );
        virtual int    painticons ( void );
        virtual void   paintsmallicons ( int taste, int up );
        void   chainiconstohost ( pnguiicon icn );
        void   restorebackground ( void );
        void   runpressedmouse ( int taste );

        void   reset ( void );
        void   cleanup ( void );    // wird zum entfernen der kleinen guiicons aufgerufen, bevor das icon ausgefÅhrt wird
        
        virtual void init ( int resolutionx, int resolutiony );
   };




typedef class tselectbuildingguihost* pselectbuildingguihost;

class tselectbuildingguihost : public tguihost {
         pnguiicon               first_icon;
         pnputbuildingguiicon*   icons;

       protected:
         virtual pnguiicon getfirsticon();
         virtual void      setfirsticon( pnguiicon ic );

       public:
         tselectbuildingguihost( void );

         void              init ( int resolutionx, int resolutiony );

         pbuildingtype selectedbuilding;
         pvehicle vehicle;
         int cancel;
         void reset ( pvehicle v );
   };





typedef class tselectobjectcontainerguihost *pselectobjectcontainerguihost;

class tselectobjectcontainerguihost : public tguihost {
         pnguiicon               first_icon;
         pnputobjectcontainerguiicon*   icons;

       protected:
         virtual pnguiicon getfirsticon();
         virtual void      setfirsticon( pnguiicon ic );

       public:
         void              init ( int resolutionx, int resolutiony );

         int cancel;
         void reset ( void );
   };



typedef class tselectvehiclecontainerguihost *pselectvehiclecontainerguihost;

typedef class tnputvehiclecontainerguiicon *pnputvehiclecontainerguiicon;


class tselectvehiclecontainerguihost : public tguihost {
         pnguiicon               first_icon;
         pnputvehiclecontainerguiicon*   icons;

       protected:
         virtual pnguiicon getfirsticon();
         virtual void      setfirsticon( pnguiicon ic );

       public:
         pvehicle constructingvehicle;
         void              init ( int resolutionx, int resolutiony );

         int cancel;
         void reset ( pvehicle _constructingvehicle );
   };






class tnguiicon {
          pnguiicon next;
          static pnguiicon first;

        protected:
          void*     picture[8];
          void*     picturepressed[8];
          char*     infotext;
          tkey      keys[6][6];
          int       priority;
          int       lasticonsize;


          pguihost  host;
          char      filename[9];
          int       x,y;

          char      infotextbuf[300];


          virtual pnguiicon nxt      ( void )          = 0;
          virtual void      setnxt   ( pnguiicon ts )  = 0;
          virtual pnguiicon frst     ( void )          = 0;
          virtual void      setfrst  ( pnguiicon ts )  = 0;
          
          friend void tguihost::chainiconstohost ( pnguiicon icn  );

          void putpict ( void* buf );
                                                    

        public:
          virtual int   available    ( void ) = 0;
          virtual void  exec         ( void ) = 0;
          virtual void  display      ( void );
          virtual char* getinfotext  ( void );

          void          loaddata     ( void );
          void          paintifavail ( void );
          void          seticonsize  ( int size );
          int           count        ( void );
          virtual void  loadspecifics( pnstream stream );
          virtual void  sethost      ( pguihost hst );

          virtual void  checkforkey  ( tkey key );
          virtual int   pressedbymouse ( void );
          virtual void  iconpressed  ( void );
          void          sort         ( pnguiicon last );
	  //          friend void tnguiicon::sort( pnguiicon last );

          tnguiicon ( void );
          virtual ~tnguiicon ( );
          static pnguiicon firstguiicon;
        };


class tnweapselguiicon : public tnguiicon {
          pnweapselguiicon        next;
          static pnweapselguiicon first;
        protected:
          virtual pnweapselguiicon nxt      ( void );
          virtual void      setnxt   ( pnguiicon ts );
          virtual pnweapselguiicon frst     ( void );
          virtual void      setfrst  ( pnguiicon ts );

          int iconnum;
          int weapnum;
          int typ;
          int strength;
       public:
          virtual int   available    ( void );
          virtual void  exec         ( void );
          virtual char* getinfotext  ( void );
          virtual void  checkforkey  ( tkey key );
          virtual void  setup        ( pattackweap atw, int n );
                                                                                       
          tnweapselguiicon ( void );
};



class tselectweaponguihost : public tguihost {
       private:
         pnweapselguiicon   first_icon;
         tnweapselguiicon   icon[9];
         pattackweap        atw;
         int                x,y;
       protected:
         virtual pnweapselguiicon getfirsticon();
         virtual void      setfirsticon( pnguiicon ic );

       public:
         tselectweaponguihost( void );

         void              init ( int resolutionx, int resolutiony );
         virtual int       painticons ( void );
         virtual void      checkforkey ( tkey key );
         virtual void      checkformouse ( void );
         virtual void      checkcoordinates ( void ); 
   };


class tnputbuildingguiicon : public tnguiicon {
            pnputbuildingguiicon        next;
            static pnputbuildingguiicon first;
            static int             buildnum;
          protected:
            virtual pnguiicon nxt      ( void );
            virtual void      setnxt   ( pnguiicon ts );
            virtual pnguiicon frst     ( void );
            virtual void      setfrst  ( pnguiicon ts );
            pnputbuildingguiicon  bnxt      ( void );
            pnputbuildingguiicon  bfrst     ( void );
            pbuildingtype           building;
            pselectbuildingguihost     bldhost;

           friend void tselectbuildingguihost ::init( int resolutionx, int resolutiony );

          public:
            static pnputbuildingguiicon firstsguiicon;
            tnputbuildingguiicon ( pbuildingtype bld );
            ~tnputbuildingguiicon ( );
            void sethost      ( pselectbuildingguihost hst );
            int   available    ( void );
            void  exec         ( void );
         };




class tnputobjectcontainerguiicon : public tnguiicon {
            int build;
            pnputobjectcontainerguiicon        next;
            static pnputobjectcontainerguiicon first;
            static int             buildnum;
          protected:
            virtual pnguiicon nxt      ( void );
            virtual void      setnxt   ( pnguiicon ts );
            virtual pnguiicon frst     ( void );
            virtual void      setfrst  ( pnguiicon ts );
            pnputobjectcontainerguiicon  bnxt      ( void );
            pnputobjectcontainerguiicon  bfrst     ( void );
            pobjecttype           object;
            pselectobjectcontainerguihost     bldhost;

           friend void tselectobjectcontainerguihost ::init( int resolutionx, int resolutiony );

          public:
            static pnputobjectcontainerguiicon firstsguiicon;
            tnputobjectcontainerguiicon ( pobjecttype obj, int bld );
            ~tnputobjectcontainerguiicon ( );
            void sethost      ( pselectobjectcontainerguihost hst );
            int   available    ( void );
            void  exec         ( void );
            int forcedeneable;
         };

class tnputvehiclecontainerguiicon : public tnguiicon {
            pnputvehiclecontainerguiicon        next;
            static pnputvehiclecontainerguiicon first;
            static int             buildnum;
          protected:
            virtual pnguiicon nxt      ( void );
            virtual void      setnxt   ( pnguiicon ts );
            virtual pnguiicon frst     ( void );
            virtual void      setfrst  ( pnguiicon ts );
            pnputvehiclecontainerguiicon  bnxt      ( void );
            pnputvehiclecontainerguiicon  bfrst     ( void );
            pvehicletype           vehicle;
            pselectvehiclecontainerguihost     bldhost;

           friend void tselectvehiclecontainerguihost ::init( int resolutionx, int resolutiony );

          public:
            static pnputvehiclecontainerguiicon firstsguiicon;
            tnputvehiclecontainerguiicon ( pvehicletype obj );
            ~tnputvehiclecontainerguiicon ( );
            void sethost      ( pselectvehiclecontainerguihost hst );
            int   available    ( void );
            void  exec         ( void );
            int forcedeneable;
         };



typedef class tnsguiicon* pnsguiicon; 
class tnsguiicon : public tnguiicon {
            pnsguiicon next;
            static pnsguiicon first;
          protected:
            virtual pnguiicon nxt      ( void );
            virtual void      setnxt   ( pnguiicon ts );
            virtual pnguiicon frst     ( void );
            virtual void      setfrst  ( pnguiicon ts );

          public:
            static pnsguiicon firstsguiicon;
            tnsguiicon ( void );
         };




class tnsguiiconmove : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;
          virtual void  display      ( void ) ;

          tnsguiiconmove ( void );
       };


class tnsguiiconattack : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiiconattack ( void );
       };

class tnsguiiconascent : public tnsguiicon {
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;
          virtual void  display      ( void ) ;

          tnsguiiconascent ( void );
       };

class tnsguiicondescent : public tnsguiicon {
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
        public:
          virtual int   available    ( void ) ;
          virtual void  exec         ( void ) ;

          tnsguiiconrepair ( void );
       };


class tnsguiiconrefuel : public tnsguiicon {
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







class tguihoststandard : public tguihost {
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
            preplayguiicon next;
            static preplayguiicon first;
          protected:
            virtual pnguiicon nxt      ( void );
            virtual void      setnxt   ( pnguiicon ts );
            virtual pnguiicon frst     ( void );
            virtual void      setfrst  ( pnguiicon ts );

          public:
            static preplayguiicon firstsguiicon;
            treplayguiicon ( void );
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



class treplayguihost : public tguihost {
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
   };



extern void         setmouseongui( void );












extern tguihoststandard          gui;
extern tselectbuildingguihost    selectbuildinggui;
extern tselectobjectcontainerguihost      selectobjectcontainergui;
extern tselectvehiclecontainerguihost     selectvehiclecontainergui;
extern tselectweaponguihost      selectweaponguihost;
extern tguihost*                 actgui;
extern void setguiposy ( int y );

#endif
