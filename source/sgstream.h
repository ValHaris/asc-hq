//     $Id: sgstream.h,v 1.2 1999-11-16 03:42:30 tmwilson Exp $
//
//     $Log: not supported by cvs2svn $
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
                           void readrlepict( char** pnter, boolean allocated, int* size);
                           void writerlepict ( void* buf );
                        };

extern pvehicletype loadvehicletype(char *       name);
extern pvehicletype loadvehicletype( pnstream stream );
extern void writevehicle( pvehicletype ft, pnstream stream );

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

extern int loaderror; 

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

extern char gamepath[200];


extern void logtofile ( char* strng );


extern void loadpalette ( void );
extern int readgameoptions ( void );
extern int writegameoptions ( void );
extern void loadguipictures( void );


#endif
