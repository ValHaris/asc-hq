//     $Id: edglobal.h,v 1.3 1999-12-27 12:59:53 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  1999/11/16 03:41:35  tmwilson
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

//*Actions fÅr Editor

class mc_check {
   public :
      void on(void);
      void off(void);
   protected :
      signed char mstatus,cstatus;
   };


extern mc_check mc;

extern int infomessage( char* formatstring, ... );

#define execactionscount 65

extern const char*  execactionnames[execactionscount];

enum tuseractions {
     act_end,
     act_help,
     
     act_seteditmode,
     act_selbodentyp,
     act_selunit,
     act_selcolor,
     act_selbuilding,
     act_selobject,
     act_selmine,
     act_selweather,
     
     act_setupalliances,
     act_toggleresourcemode,
     act_changeunitdir,
     act_asc_resource,
     act_maptopcx,
     act_loadmap,
     act_changeplayers,
     act_newmap,
     act_polymode,
     act_repaintdisplay,
     act_unitinfo,
     act_viewmap,
     act_about,
     act_changeglobaldir,
     act_createresources,
     act_changecargo,
     act_changeresources,
     act_changeterraindir,
     act_events,
     act_fillmode,
     act_mapgenerator,
     act_setactivefieldvals,
     act_deletething,
     act_showpalette,
     act_changeminestrength,
     act_changemapvals,
     act_changeproduction,
     act_savemap,
     act_changeunitvals,
     act_mirrorcursorx,
     act_mirrorcursory,
     act_placebodentyp,
     act_placeunit,
     act_placebuilding,
     act_placeobject,
     act_placemine,
     act_placething,
     act_deleteunit,
     act_deletebuilding,
     act_deleteobject,
     act_deletemine,
     act_aboutbox,
     act_savemapas,
     act_endpolyfieldmode,
     act_smoothcoasts,
     act_import_bi_map,
     act_seperator,
     act_bi_resource,
     act_resizemap,
     act_insert_bi_map,
     act_setzoom,
     act_movebuilding,
     act_setactweatherglobal,
     act_setmapparameters, 
     act_terraininfo,
     act_setunitfilter };

extern void         execaction(int code);


struct IdRange {
        int from;
        int to;
       };

class SingeUnitSet {
      public:
         int active;
         char* name;
         dynamic_array<IdRange> ids;

         SingeUnitSet ( void ) 
         { 
            name = NULL;
            active = 1;
         }

         void init ( char* _name )
         {
            name = (char*)asc_malloc ( strlen ( _name) + 2); 
            strcpy ( name, _name );
         };

         ~SingeUnitSet () 
         {
            if ( name ) {
               asc_free ( name );
               name = NULL;
            }
         };
     };

class UnitSet {
       public:
        dynamic_array<SingeUnitSet> set;
      } ;

extern UnitSet unitSet;


