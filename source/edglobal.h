//     $Id: edglobal.h,v 1.16 2004-01-21 14:43:00 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.15  2003/04/23 18:31:09  mbickel
//      Fixed: AI problems
//      Improved cheating detection in replay
//
//     Revision 1.14  2003/03/20 10:08:29  mbickel
//      KI speed up
//      mapeditor: added clipboard
//      Fixed movement issues
//
//     Revision 1.13  2002/11/01 20:44:53  mbickel
//      Added function to specify which units can be build by other units
//
//     Revision 1.12  2002/03/17 21:25:18  mbickel
//      Fixed: View unit movement revealed the reaction fire status of enemy units
//      Mapeditor: new function "resource comparison"
//
//     Revision 1.11  2001/10/11 10:41:06  mbickel
//      Restructured platform fileio handling
//      Added map archival information to mapeditor
//
//     Revision 1.10  2001/10/08 14:12:20  mbickel
//      Fixed crash in AI
//      Speedup of AI
//      Map2PCX improvements
//      Mapeditor usability improvements
//
//     Revision 1.9  2001/10/02 14:06:28  mbickel
//      Some cleanup and documentation
//      Bi3 import tables now stored in .asctxt files
//      Added ability to choose amoung different BI3 import tables
//      Added map transformation tables
//
//     Revision 1.8  2001/08/02 15:33:01  mbickel
//      Completed text based file formats
//
//     Revision 1.7  2000/11/29 09:40:19  mbickel
//      The mapeditor has now two maps simultaneously active
//      Moved memorychecking functions to its own file: memorycheck.cpp
//      Rewrote password handling in ASC
//
//     Revision 1.6  2000/08/02 15:52:56  mbickel
//      New unit set definition files
//      demount accepts now more than one container file
//      Unitset information dialog added
//
//     Revision 1.5  2000/03/16 14:06:54  mbickel
//      Added unitset transformation to the mapeditor
//
//     Revision 1.4  2000/03/11 18:22:04  mbickel
//      Added support for multiple graphic sets
//
//     Revision 1.3  1999/12/27 12:59:53  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
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

const int execactionscount = 84;

extern const char*  execactionnames[execactionscount];

enum tuseractions {
     act_end,
     act_help,
     act_seteditmode,
     act_selbodentyp,
     act_selbodentypAll,
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
     act_deletetopmostobject,
     act_deleteallobjects,
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
     act_setunitfilter,
     act_selectgraphicset,
     act_unitsettransformation,
     act_unitSetInformation,
     act_switchmaps,
     act_transformMap,
     act_editArchivalInformation,
     act_displayResourceComparison,
     act_specifyunitproduction,
     act_pasteFromClipboard,
     act_copyToClipboard,
     act_cutToClipboard,
     act_saveClipboard,
     act_readClipBoard,
     act_setTurnNumber,
     act_showPipeNet};

extern void         execaction(int code);
