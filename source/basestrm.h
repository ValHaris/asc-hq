//     $Id: basestrm.h,v 1.4 1999-12-27 12:59:40 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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

#ifdef MEMCHECK
#include "memchk.h2"
#endif

#include <stdio.h>

#include "global.h"
#include "lzw.h"
extern "C" {
#ifdef _DOS_
    #include "libs\bzlib\bzlib.h"
#else
#include "libs/bzlib/bzlib.h"
#endif

}



#ifndef MAX
 #define MAX(a,b)		((a) >= (b) ? (a) : (b))
#endif
#ifndef MIN
 #define MIN(a,b)		((a) <= (b) ? (a) : (b))
#endif
#define MAX3(a,b,c)		((a) >= (b) ? MAX(a,c) : MAX(b,c))
#define MIN3(a,b,c)		((a) <= (b) ? MIN(a,c) : MIN(b,c))


// #pragma library (basestrm)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////        Watchpointer
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
class twatchptr {
       public:
         void* ptr;
         void** pptr;
         twatchptr ( void );
         twatchptr ( void* pt );
         twatchptr ( void** pt );
         ~twatchptr ();
         void free ( void );
      };
*/



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



class terror { 
};



class tcompressionerror : public terror {
     char strng[100];
     int code;

   public:
     tcompressionerror ( const char* msg, int returncode );
};

class tinternalerror: public terror {
   int linenum;
   const char* sourcefilename;
  public:
   tinternalerror ( const char* filename, int l );
};

class toutofmem : public terror { 
  public:
   int required;
   toutofmem ( int m ) ;
};

class tbufferoverflow : public terror { 
};

class tfileerror : public terror {
  public:
   char filename[20];
   tfileerror ( const char* fn ) ;
   tfileerror ( void );
//   ~tfileerror();
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
           virtual void writepchar( const char* pc) ;

           virtual void writerlepict ( const void* pnter );
           virtual void readrlepict( void** pnter, int allocated, int* size);
           virtual ~tnstream() {};

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
           char devicename[100];
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
            virtual void seekstream ( int newpos );
            virtual int getstreamsize ( void );
            virtual ~tn_file_buf_stream  ( );
            virtual int gettime ( void );

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
            virtual int gettime ( void );
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
           virtual void addfile ( const char* filename, const pncontainerstream stream ) = 0;
     };


class tncontainerstream : public tn_file_buf_stream {
                 tcontainerindex* index;
                 tcontainerindex* actfile;
                 int containerfilepos;
                 int num;
                 
                 int actname;

               public:
                 tncontainerstream ( const char* containerfilename, ContainerIndexer* indexer );
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
           struct ContainerCollectorIndex {
              char* name;
              pncontainerstream container;
           };
           dynamic_array<ContainerCollectorIndex> index[256];    // not very efficient, but who cares :-)

           dynamic_array<pncontainerstream> container;
           int containernum;
           struct {
              int alpha;
              int index;
           } namesearch;       // next entry to return
         public:
           ContainerCollector ( void );
           void init ( const char* wildcard );
           void addfile ( const char* filename, const pncontainerstream stream );
           pncontainerstream getfile ( const char* filename );
           char* getfirstname ( void );
           char* getnextname ( void );
           ~ContainerCollector();
        };







///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class tfindfile {
        int found;
        int act;
        dynamic_array<char*> names;
        dynamic_array<char>  namedupes;

      public:
        tfindfile ( const char* name );
        char* getnextname ( void );
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

extern int exist ( char* s );
extern int getfiletime ( char* devicename );

extern void opencontainer ( const char* wildcard );

extern const int containermagic;

#endif

