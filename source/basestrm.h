/*! \file basestrm.h
    \brief The interface for the various streams that ASC offers, 
           like file and memory streams. 
*/


//     $Id: basestrm.h,v 1.38 2001-02-26 22:03:18 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.37  2001/02/26 21:14:30  mbickel
//      Added two small editors to the linux makefiles
//      Added some more truecolor hacks to the graphics engine
//
//     Revision 1.36  2001/02/18 15:37:02  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.35  2001/02/11 11:39:28  mbickel
//      Some cleanup and documentation
//
//     Revision 1.34  2001/01/28 14:04:03  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.33  2001/01/04 15:13:30  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.32  2000/12/21 11:00:44  mbickel
//      Added some code documentation
//
//     Revision 1.31  2000/11/09 17:48:46  mbickel
//      The size of a stream can now be queried
//      PCX loader (in C) can now load unpatched images provided they are not
//        compressed

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
#include <vector>
#include <queue>

#include "global.h"
#include "basestreaminterface.h"
#include "lzw.h"
#include "errors.h"
#include "tpascal.inc"

extern "C" {
#include "libs/bzlib/bzlib.h"
}



#pragma pack(1)




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
   ASCString _filename;
  public:
   tfileerror ( const ASCString& fileName ) ;
   const ASCString& getFileName() const { return _filename; };
   tfileerror ( void ) {};
};

class tinvalidmode : public tfileerror {
  public:
   int orgmode, requestmode;
   tinvalidmode ( const ASCString& fileName, tnstream::IOMode org_mode, tnstream::IOMode requested_mode ) ;
};

class treadafterend : public tfileerror {
  public:
   treadafterend ( const ASCString& fileName );
};

class tinvalidversion : public tfileerror {
   public:
     tinvalidversion ( const ASCString& fileName, int ex, int fnd );
     const int expected;
     const int found;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////        dynamic data structures
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
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
*/


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
           bool initialized;

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
           IOMode _mode;
           int   actmempos;
           pmemorystreambuf buf;

        public:
           tmemorystream ( pmemorystreambuf lbuf, IOMode mode );
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
           IOMode  _mode;
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

          protected:

              typedef deque<char> CDQ;
              queue<char,CDQ> tempbuf;

              enum tmode { none, reading, writing, readingdirect, readingrle };
              tmode mode;

          public:

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
             typedef deque<char> CDQ;
             queue<char, CDQ> _queue;
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
            tn_file_buf_stream ( const ASCString& _fileName, IOMode mode );
            virtual void seek ( int newpos );
            virtual int getstreamsize ( void );
            virtual int getSize ( void ) { return getstreamsize(); };
            virtual time_t get_time ( void );
            virtual ~tn_file_buf_stream  ( );

  };



class tanycompression : public t_compressor_stream_interface, protected tlzwstreamcompression {

                            typedef deque<char> CDQ;
                            queue<char, CDQ> _queue;

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
              tn_lzw_file_buf_stream ( const char* name, IOMode mode ) : tn_file_buf_stream ( name, mode ) , tanycompression ( mode )
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
            tn_c_lzw_filestream ( const char* name, IOMode mode );
            void writedata ( const void* buf, int size );
            int  readdata  ( void* buf, int size, int excpt = 1  );
            virtual ~tn_c_lzw_filestream  ( );
            virtual time_t get_time ( void );
            virtual int getSize( void );
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

        vector<string> names;
        vector<int> directoryLevel;
        vector<bool> isInContainer;
        int found;
        int act;

      public:
        tfindfile ( string name );
        string getnextname ( int* loc = NULL, bool* inContainer = NULL );
     };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "basetemp.h"

extern  const char* asc_configurationfile;
extern  const int maxfilenamelength;


extern int compressrle ( const void* p, void* q);
extern int patimat (const char *pat, const char *str);

extern int checkforvaliddirectory ( char* dir );

extern char* getnextfilenumname ( const char* first, const char* suffix, int num = -1  );


#define writedata2(a)  writedata ( &(a), sizeof(a) )
#define readdata2(a)   readdata  ( &(a), sizeof(a) )

extern int exist ( const char* s );
extern time_t get_filetime ( const char* devicename );

extern void opencontainer ( const char* wildcard );

extern const char* containermagic;

#pragma pack()

extern const char* filereadmode;
extern const char* filewritemode;
extern const char* filereadmodetext;
extern const char* filewritemodetext;

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
extern int createDirectory ( const char* name );

#ifdef _SDL_
 #include sdlheader
 extern SDL_RWops *SDL_RWFromStream( pnstream stream );
#endif

#endif

