//     $Id: basestrm.h,v 1.24 2000-10-12 19:00:21 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.23  2000/10/11 14:26:17  mbickel
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
//     Revision 1.22  2000/09/26 18:05:14  mbickel
//      Upgraded to bzlib 1.0.0 (which is incompatible to older versions)
//
//     Revision 1.21  2000/09/24 19:57:03  mbickel
//      ChangeUnitHeight functions are now more powerful since they use
//        UnitMovement on their own.
//
//     Revision 1.20  2000/08/12 09:17:16  gulliver
//     *** empty log message ***
//
//     Revision 1.19  2000/08/03 19:21:15  mbickel
//      Fixed: units had invalid height when produced in some buildings
//      Fixed: units could not enter building if unitheightreq==0
//      Started adding SDL_image support
//      Upgraded to SDL1.1.3 (for SDL_image)
//
//     Revision 1.18  2000/08/02 15:52:40  mbickel
//      New unit set definition files
//      demount accepts now more than one container file
//      Unitset information dialog added
//
//     Revision 1.17  2000/08/02 10:28:23  mbickel
//      Fixed: generator vehicle not working
//      Streams can now report their name
//      Field information shows units filename
//
//     Revision 1.16  2000/08/01 10:39:08  mbickel
//      Updated documentation
//      Refined configuration file handling
//
//     Revision 1.15  2000/07/31 19:16:32  mbickel
//      Improved handing of multiple directories
//      Fixed: wind direction not displayed when cycling through wind heights
//      Fixed: oil rig not working
//      Fixed: resources becomming visible when checking mining station status
//      Fixed: division by zero when moving unit without fuel consumption
//
//     Revision 1.14  2000/07/31 18:02:53  mbickel
//      New configuration file handling
//      ASC searches its data files in all directories specified in ascrc
//      Renamed all tools so they begin with asc
//
//     Revision 1.13  2000/07/28 10:15:27  mbickel
//      Fixed broken movement
//      Fixed graphical artefacts when moving some airplanes
//
//     Revision 1.12  2000/06/28 18:30:57  mbickel
//      Started working on AI
//      Started making loaders independent of memory layout
//      Destroyed buildings can now leave objects behind.
//
//     Revision 1.11  2000/05/30 18:39:20  mbickel
//      Added support for multiple directories
//      Moved DOS specific files to a separate directory
//
//     Revision 1.10  2000/04/27 16:25:15  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.9  2000/03/11 19:51:12  mbickel
//      Removed file name length limitation under linux
//      No weapon sound for attacked units any more (only attacker)
//
//     Revision 1.8  2000/02/05 12:13:44  steb
//     Sundry tidying up to get a clean compile and run.  Presently tending to SEGV on
//     startup due to actmap being null when trying to report errors.
//
//     Revision 1.7  2000/02/03 20:54:38  mbickel
//      Some cleanup
//      getfiletime now works under Linux too
//
//     Revision 1.6  1999/12/29 12:50:42  mbickel
//      Removed a fatal error message in GUI.CPP
//      Made some modifications to allow platform dependant path delimitters
//
//     Revision 1.5  1999/12/28 21:02:38  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.4  1999/12/27 12:59:40  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.3  1999/11/16 17:03:55  mbickel
//     Made ASC compilable for DOS again :-)
//     Merged all the bug fixes in that I did last week
//
//     Revision 1.2  1999/11/16 03:41:07  tmwilson
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

#ifndef basestream_h
#define basestream_h

#include <string>
#include <stdio.h>
#include <time.h>

#include "global.h"
#include "lzw.h"
#include "errors.h"

#include "libs/bzlib/bzlib.h"



#pragma pack(1)


#ifdef _DOS_
 #define maxfilenamelength 8
#else
 #define maxfilenamelength 255
#endif



enum FS_StreamMode { fs_undefined, fs_read, fs_write };

const int maxFileStringSize = 10000;    // is used for some character arrays


class CharBuf {
       public:
         int size;
         char* buf;
         CharBuf ( void );
         CharBuf ( int _size );
         void resize ( int newsize );
         ~CharBuf();
   };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////        Exceptions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






class tcompressionerror : public ASCmsgException {
   public:
     tcompressionerror ( string msg, int returncode ) : ASCmsgException ( msg )
     {
        message += "\n the returncode is ";
        message += strrr ( returncode );
     };
};

class tinternalerror: public ASCexception {
   int linenum;
   const char* sourcefilename;
  public:
   tinternalerror ( const char* filename, int l );
};

class toutofmem : public ASCexception {
  public:
   int required;
   toutofmem ( int m ) ;
};

class tbufferoverflow : public ASCexception {
};

class tfileerror : public ASCexception {
  public:
   char filename[2000];
   tfileerror ( const char* fn ) ;
   tfileerror ( void );
};

class tinvalidmode : public tfileerror {
  public:
   int orgmode, requestmode;
   tinvalidmode ( const char* fn, int org_mode, int requested_mode ) ;
};

class treadafterend : public tfileerror {
  public:
   treadafterend ( const char* fn );
};

class tinvalidversion : public tfileerror {
   public:
     tinvalidversion ( const char* fn, int ex, int fnd );
     int expected;
     int found;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////        dynamic data structures
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class dynamic_queue {
           protected:
              int blksize;
              int size;
              int first;
              int last;
              T* buf;
           public:
              dynamic_queue ( void );
              T getval ( void );
              void putval ( T a );
              int valavail ( void );
              ~dynamic_queue();
};



template<class T>
class dynamic_array {
           protected:
              int blksize;
              int size;
              T* buf;
              virtual void resize ( int newsize );
              int maxaccessed;

           public:
              dynamic_array ( void );
              dynamic_array ( int sze );
              virtual ~dynamic_array();
              T& operator[]( int a );
              int getlength( void );
              void reset ( void );
};

template<class T>
class dynamic_initialized_array : public dynamic_array<T> {
              T initval;
           protected: 
              virtual void resize ( int newsize );

           public: 
              dynamic_initialized_array ( T ival );
              dynamic_initialized_array ( T ival, int sze );
         };


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////        Streams
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef class tnstream* pnstream;

class tnstream {
        public:
           virtual void writedata ( const void* buf, int size ) = 0 ;
           virtual int  readdata  ( void* buf, int size, int excpt = 1 ) = 0 ;

           virtual void readpchar( char** pc, int maxlength = 0) ;
           virtual void readpnchar( char** pc, int maxlength = 0) ;
           virtual int readTextString ( std::string& s ); // return 0 if end of file is reached
           virtual void writepchar( const char* pc) ;

           virtual int  readInt  ( void );
           virtual word readWord ( void );
           virtual char readChar ( void );

           virtual void writeInt  ( int  i );
           virtual void writeWord ( word w );
           virtual void writeChar ( char c );

           virtual void writerlepict ( const void* pnter );
           virtual void readrlepict( void** pnter, int allocated, int* size);
           virtual ~tnstream() {};
           virtual const char* getDeviceName ( void );
           virtual void seek ( int newpos );
           tnstream ( );
         protected:
           std::string devicename; // will just contain "abstract" 

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MemoryStreamCopy : public tnstream {
               void* buf;
               int size;
               int pos;


             public:
               MemoryStreamCopy ( pnstream stream );
               ~MemoryStreamCopy ( );
               void writedata ( const void* buf, int size );
               int  readdata  ( void* buf, int size, int excpt = 1 );
               void seek ( int newpos );
               int getPosition ( void ) { return pos; };
               int getSize ( void ) { return size; };
         };


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  #ifndef pmemorystreambuf_defined
   #define pmemorystreambuf_defined
   typedef class tmemorystreambuf* pmemorystreambuf;
   typedef class tmemorystream* pmemorystream;
  #endif

class tmemorystreambuf {
        public: 
           int mode;         // 0: uninitialisiert; 
                             // 1: initialisiert
           int used;
           int allocated;
           int dummy[10];
           char* buf;
           tmemorystreambuf ( void );
           void writetostream ( pnstream stream );
           void readfromstream ( pnstream stram );
           ~tmemorystreambuf ( );
      };


class tmemorystream : public tnstream {
       protected:
           int   blocksize;
           char* zeiger;
           int   mode;
           int   actmempos;
           pmemorystreambuf buf;

        public: 
           tmemorystream ( pmemorystreambuf lbuf, int lmode );
           virtual void writedata ( const void* nbuf, int size );
           virtual int  readdata  ( void* nbuf, int size, int excpt = 1 );
           int dataavail ( void );
      };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class tnbufstream  : public tnstream {
           char  minbuf;
           int datalen;

       protected:
           char* zeiger;
           char  modus;
           int   actmempos;
           int   memsize;
           int   datasize;

           virtual void readbuffer( void ) = 0;
           virtual void writebuffer( void ) = 0;
           virtual void close( void ) {};
       public:
           tnbufstream ( );
           virtual void writedata ( const void* buf, int size );
           virtual int  readdata  ( void* buf, int size, int excpt = 1  );

           virtual ~tnbufstream ( );

      };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



class tlzwstreamcompression {

       // ********** For writing
             CountType LZWTotBytesIn; /* no. data bytes in input file */
             CountType LZWTotBytesOut; /* no. data bytes in output file */
             CountType LZWCurrBytesIn; /* no. data bytes in current frame */
             CountType LZWCurrBytesOut; /* no. data bytes in current frame */
             unsigned  LZWBestRatio; /* best ratio we've had */
             unsigned  LZWLastRatio; /* last ratio we saw */



             /* structure for the data dictionary */
             struct Wdictionary {
                 unsigned char c;
                 CodeType code;
                 CodeType parent;
             };
             struct Wdictionary *wdictionary;

             void LZWOut ( CodeType code );
             IndexType LZWFind ( CodeType currcode, int in );

           // deklarationen, die in LZWencode drinwaren
              int in;
              CountType i;
              CodeType currcode;
              IndexType idx;

              int currcodeloaded;


       //*********** for reading

               /* structure for data dictionary */
               struct Rdictionary {
                   unsigned char c;
                   CodeType parent;
               };


              /* decode buffer */
              unsigned char *DecodeBuffer;
              unsigned DecodeBufferSize ;
              
              unsigned int inchar;
              unsigned count;
              CodeType oldcode;
              
              /* data rdictionary */
              struct Rdictionary *rdictionary;
              
              CodeType incode;

              void LZWIn ( void );
              unsigned LZWLoadBuffer ( unsigned count, CodeType code );
              int readcnt;

       // ******* rle uncompression
              char rlestartbyte;
              char rlenum;
              char rledata;

       // ******* general
              IndexType freecode;

              int initread;
              int initwrite;
              void initreading( void );
              void initwriting( void );

              dynamic_queue<char> tempbuf;

          public:
              int queuestat;
              enum tmode { none, reading, writing, readingdirect, readingrle };
              tmode mode;

             void writedata ( const void* buf, int size );
             int  readdata  ( void* buf, int size, int excpt = 1  );
             virtual int  readlzwdata ( void* buf, int size, int excpt = 1 ) = 0;
             virtual void writelzwdata ( const void* buf, int size ) = 0;
             tlzwstreamcompression ( void );
             virtual ~tlzwstreamcompression ( void );
          protected:
             void close( void );

};


class t_compressor_stream_interface {
           public:
             virtual void writecmpdata ( const void* buf, int size ) = 0;
             virtual int readcmpdata ( void* buf, int size, int excpt = 1 ) = 0;
      };

class t_compressor_2ndbuf_filter : public t_compressor_stream_interface {
             t_compressor_stream_interface *stream;
             dynamic_queue<char> queue;
             int queuesize;
           public:
             t_compressor_2ndbuf_filter ( t_compressor_stream_interface* strm );
             virtual void writecmpdata ( const void* buf, int size );
             virtual int readcmpdata ( void* buf, int size, int excpt = 1 );
             void insert_data_into_queue ( const void* buf, int size );
             virtual ~t_compressor_2ndbuf_filter() {};
};

typedef t_compressor_stream_interface *p_compressor_stream_interface;

class libbzip_compression {
            bz_stream bzs;
            char* outputbuf;
            int outputbufsize;
            p_compressor_stream_interface stream;
         public:
             void close_compression ( void );
             void writedata ( const void* buf, int size );
             libbzip_compression ( p_compressor_stream_interface strm );
             virtual ~libbzip_compression ( );

};

class libbzip_decompression {
            bz_stream bzs;
            char* inputbuf;
            int inputbufsize;
            int inputbufused;
            int inputbufread;
            p_compressor_stream_interface stream;

         public:
             int  readdata  ( void* buf, int size, int excpt = 1  );
             libbzip_decompression ( p_compressor_stream_interface strm );
             virtual ~libbzip_decompression ( );

};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



class tn_file_buf_stream : public tnbufstream {
            FILE* fp;
            int actfilepos;

        protected:
            void readbuffer( void );
            void writebuffer( void );


        public:
            tn_file_buf_stream ( const char* name, char mode );
            virtual void seek ( int newpos );
            virtual int getstreamsize ( void );
            virtual ~tn_file_buf_stream  ( );
            virtual time_t get_time ( void );

  };



class tanycompression : public t_compressor_stream_interface, protected tlzwstreamcompression {

                            dynamic_queue<char> queue;

                            libbzip_compression* bzip_compress;
                            libbzip_decompression* bzip_decompress;
                            int mmd;
                            int status;

                          protected:
                             virtual int readlzwdata ( void* buf, int size, int excpt = 1 );
                             virtual void writelzwdata ( const void* buf, int size );
                             void close_compression ( void );

                           public:
                             tanycompression ( int md );
                             void init ( void );

                             void writedata ( const void* buf, int size );
                             int  readdata  ( void* buf, int size, int excpt = 1  );
                             ~tanycompression ( );
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
class tn_lzw_bufstream : public tnbufstream, protected tlzwstreamcompression {
                            public:
                              void writedata ( const void* buf, int size );
                              int  readdata  ( void* buf, int size, int excpt = 1  );
                              int readlzwdata ( void* buf, int size, int excpt = 1 );
                              void writelzwdata ( const void* buf, int size );
                              ~tn_lzw_bufstream();
                        };
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class tn_lzw_file_buf_stream : public tn_file_buf_stream, protected tanycompression {
           public:
              tn_lzw_file_buf_stream ( const char* name, char mode ) : tn_file_buf_stream ( name, mode ) , tanycompression ( mode )
                 { 
                    tanycompression :: init ( ); 
                 } ;
                                                                     
              void writedata ( const void* buf, int size );
              int  readdata  ( void* buf, int size, int excpt = 1  );
              int  readcmpdata ( void* buf, int size, int excpt = 1 );
              void writecmpdata ( const void* buf, int size );
              ~tn_lzw_file_buf_stream();
           };


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef class tncontainerstream* pncontainerstream;


class tn_c_lzw_filestream : public tnstream, protected tanycompression {
            tn_file_buf_stream *strm;
            pncontainerstream containerstream;
            int inp;
            char* fname;
         protected:
            int  readcmpdata ( void* buf, int size, int excpt = 1 );
            void writecmpdata ( const void* buf, int size );
         public:
            tn_c_lzw_filestream ( const char* name, char mode );
            void writedata ( const void* buf, int size );
            int  readdata  ( void* buf, int size, int excpt = 1  );
            virtual ~tn_c_lzw_filestream  ( );
            virtual time_t get_time ( void );
     };

typedef tn_c_lzw_filestream tnfilestream ;
typedef tnfilestream*  pnfilestream ;




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                 struct tcontainerindex {
                    char* name;
                    int start;  // das erste Byte
                    int end;    // das letzte Byte
                 };


class ContainerIndexer {
       public:
           virtual void addfile ( const char* filename, const pncontainerstream stream, int directoryLevel ) = 0;
     };


class tncontainerstream : public tn_file_buf_stream {
                 tcontainerindex* index;
                 tcontainerindex* actfile;
                 int containerfilepos;
                 int num;
                 
                 int actname;

               public:
                 tncontainerstream ( const char* containerfilename, ContainerIndexer* indexer, int directoryLevel );
                 void opencontainerfile ( const char* name );
                 int  getcontainerfilesize ( const char* name );
                 int readcontainerdata ( void* buf, int size, int excpt = 1 );
                 void closecontainerfile ( void );
                 char* getfirstname ( void );
                 char* getnextname ( void );
                 virtual ~tncontainerstream ();
             };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ContainerCollector : public ContainerIndexer {
         public:
           struct FileIndex {
              char* name;
              pncontainerstream container;
              int directoryLevel;
           };
         protected:

           dynamic_array<FileIndex> index[256];    // not very efficient, but who cares :-)

           dynamic_array<pncontainerstream> container;
           int containernum;
           struct {
              int alpha;
              int index;
           } namesearch;       // next entry to return
         public:
           ContainerCollector ( void );
           void init ( const char* wildcard );
           void addfile ( const char* filename, const pncontainerstream stream, int directoryLevel );
           // pncontainerstream getfile ( const char* filename );
           FileIndex* getfile ( const char* filename );
           FileIndex* getfirstname ( void );
           FileIndex* getnextname ( void );
           virtual ~ContainerCollector();
        };







///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class tfindfile {
       /*
        class Node {
                char* name;
                int deletename;
                int directoryLevel;
              public:
                Node ( char* _name, int _deletename, int _dirLevel );
               ~Node();
            };
       */
                   
        int found;
        int act;
        dynamic_array<char*> names;
        dynamic_array<char>  namedupes;
        dynamic_array<int>   directoryLevel;   
        dynamic_array<int>   isInContainer;   

      public:
        tfindfile ( const char* name );
        char* getnextname ( int* loc = NULL, int* inContainer = NULL );
        ~tfindfile();

     };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "basetemp.h"


extern int compressrle ( const void* p, void* q);
// extern tncontainerstream* containerstream;
extern int patimat (const char *pat, const char *str);

extern int checkforvaliddirectory ( char* dir );

extern char* getnextfilenumname ( const char* first, const char* suffix, int num = -1  );


#define writedata2(a)  writedata ( &(a), sizeof(a) )
#define readdata2(a)   readdata  ( &(a), sizeof(a) )

extern int exist ( const char* s );
extern time_t get_filetime ( char* devicename );

extern void opencontainer ( const char* wildcard );

extern const char* containermagic;

#pragma pack()

extern const char* filereadmode;
extern const char* filewritemode;

extern int verbosity;

extern const char pathdelimitter; 
extern const char* pathdelimitterstring; 
extern int filesize( char *name);

extern void addSearchPath ( const char* path );
extern void appendbackslash ( char* string );
extern char* constructFileName( char* buf, int directoryLevel, const char* path, const char* filename );
extern int directoryExist ( const char* path );
extern char* extractPath ( char* buf, const char* filename );
extern char* extractFileName ( char* buf, const char* filename );

#ifndef _DOS_
 #include sdlheader
 extern SDL_RWops *SDL_RWFromStream( pnstream stream );
#endif

#endif

