//     $Id: sgstream.h,v 1.12 2000-11-21 20:27:07 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.11  2000/10/17 12:12:23  mbickel
//      Improved vehicletype loading/saving routines
//      documented some global variables
//
//     Revision 1.10  2000/09/01 15:47:50  mbickel
//      Added Astar path finding code
//      Fixed: viewid crashed
//      Fixed display error in ynswitch ( krkr.cpp )
//
//     Revision 1.9  2000/08/12 09:17:34  gulliver
//     *** empty log message ***
//
//     Revision 1.8  2000/08/02 15:53:04  mbickel
//      New unit set definition files
//      demount accepts now more than one container file
//      Unitset information dialog added
//
//     Revision 1.7  2000/08/01 10:39:17  mbickel
//      Updated documentation
//      Refined configuration file handling
//
//     Revision 1.6  2000/07/31 18:02:54  mbickel
//      New configuration file handling
//      ASC searches its data files in all directories specified in ascrc
//      Renamed all tools so they begin with asc
//
//     Revision 1.5  2000/07/29 14:54:46  mbickel
//      plain text configuration file implemented
//
//     Revision 1.4  2000/05/23 20:40:49  mbickel
//      Removed boolean type
//
//     Revision 1.3  2000/03/29 09:58:49  mbickel
//      Improved memory handling for DOS version
//      Many small changes I can't remember ;-)
//
//     Revision 1.2  1999/11/16 03:42:30  tmwilson
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


#ifndef sgstream_h
#define sgstream_h

#include <vector>

#include "basestrm.h"
#include "typen.h"



class tstream {
        public:
           virtual void writedata ( char* buf, int size ) = 0 ;
           virtual void readdata  ( char* buf, int size ) = 0 ;
           void writedata ( void* buf, int size );
           void readdata ( void* buf, int size );
           void writedata ( char buf );
           void readdata ( char* buf );
           void writedata ( word buf );
           void readdata ( word* buf );
           void writedata ( int buf );
           void readdata ( int* buf );

};



class tbufstream  : public tstream {
       public:
           char* zeiger;
           int   memsize;
           int   actmempos;
           int   datasize;
           char  modus;
           int   status;
           char* devicename;
           FILE* logfile;

           void init ( void );
           virtual void writedata ( char* buf, int size );
           virtual void readdata  ( char* buf, int size );

           virtual void readpchar(char** pc);
           virtual void readbuffer( void ) = 0;
           virtual void writepchar( char* pc);
           virtual void writebuffer( void ) = 0;
           virtual char getstatus( void );
           void done ( void );
      };

#define pfilestream  *tfilestream

class tfilestream : public tbufstream {
        public:
            void init ( void );
            void openstream(char* name, char mode);            /*   1 : readdata; 2 : writedata   */
            virtual void resetstream ( void );
            void closestream ( void );
            virtual void seekstream ( int newpos, int seekpos = SEEK_SET );
            virtual void readbuffer( void );
            virtual void writebuffer( void );
            virtual int getstreamsize ( void );
            void done ( void );

        private:
            /* fstream datei; */
            FILE* fp;
            int actfilepos;
  };

class tsfilestream : public tfilestream {
               /*         virtual void seekstream ( int newpos );
                        void openstream ( char* name; char mode );
                        virtual void readbuffer( void* p, int size, char* result);
                        virtual int getstreamsize( void );
                     private:
                        int startpos,stoppos;
                 */
                    };

class tsrlefilestream : public tsfilestream {
                        public:
                           void readrlepict( char** pnter, char allocated, int* size);
                           void writerlepict ( void* buf );
                        };

extern pvehicletype loadvehicletype(char *       name);
extern pvehicletype loadvehicletype( tnstream& stream );
extern void writevehicle( pvehicletype ft, tnstream& stream );

extern pterraintype loadterraintype(char *       name);
extern pterraintype loadterraintype( pnstream stream );
extern void writeterrain ( pterraintype bdt, pnstream stream );

extern pbuildingtype loadbuildingtype(char *       name);
extern pbuildingtype loadbuildingtype( pnstream stream );
extern void writebuildingtype ( pbuildingtype bld, pnstream stream );

extern ptechnology loadtechnology(char *       name);
extern ptechnology loadtechnology( pnstream stream );
extern void writetechnology ( ptechnology tech, pnstream stream );

extern pobjecttype   loadobjecttype(char *       name);
extern pobjecttype   loadobjecttype( pnstream stream );
void writeobject ( pobjecttype object, pnstream stream, int compressed = 0 );

#ifndef converter
extern pquickview generateaveragecol ( pwterraintype bdn );
#endif

#define streamwritedata(a,b) stream.writedata ( (char*) &(a), b )
#define streamwritedata2(a) stream.writedata ( (char*) &(a), sizeof(a) )
#define streamreaddata(a,b) stream.readdata( (char*) &(a), b )
#define streamreaddata2(a) stream.readdata( (char*) &(a), sizeof(a) )










extern void generatedirecpict ( void* orgpict, void* direcpict );
extern pquickview generateaveragecol ( pwterraintype bdn );


extern pobjecttype streetobjectcontainer ;
extern pobjecttype pathobject;
extern pobjecttype railroadobject ;
extern pobjecttype eisbrecherobject;
extern pobjecttype fahrspurobject;

class t_carefor_containerstream {       // not necessary any more, now that we have the containercollector
       public:
         t_carefor_containerstream ( void );
       };

extern void logtofile ( char* strng, ... );


extern void loadpalette ( void );
extern int readgameoptions ( const char* filename = NULL );
extern int writegameoptions ( void );
extern char* getConfigFileName ( char* buffer );
extern void loadguipictures( void );
extern void checkFileLoadability ( const char* filename );
extern void initFileIO ( const char* configFileName );





class SingleUnitSet {
      public:
         struct IdRange {
                 int from;
                 int to;
                };


         class TranslationTable {
                  public:
                     std::vector<IdRange> translation;
                     std::string name;
                     void parseString ( const char* s );
               };


         int active;
          std::string name;
          std::string maintainer;
          std::string information;

         std::vector<IdRange> ids;
         std::vector<TranslationTable*> transtab;
         bool filterBuildings;

         SingleUnitSet ( void ) : active ( 1 ), filterBuildings ( true ) {};
         int isMember ( int id );
         void read ( pnstream stream );
         void parseIDs ( const char* s );

     };

extern std::vector<SingleUnitSet*> unitSets;

extern void loadUnitSets ( void );


#endif
