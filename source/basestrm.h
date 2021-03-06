/*! \file basestrm.h
    \brief The interface for the various streams that ASC offers,
           like file and memory streams.
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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

#ifndef basestrmH
#define basestrmH


#include <string>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <list>
#include <queue>

#include <SDL.h>

#include "global.h"
#include "basestreaminterface.h"
#include "lzw.h"
#include "errors.h"

#include "simplestream.h"



extern SDL_RWops *SDL_RWFromStream( tnstream* stream );

class RWOPS_Handler {
      SDL_RWops *rwo;
   public:
      RWOPS_Handler( SDL_RWops *rw ) : rwo (rw) {};
      SDL_RWops* Get() { return rwo; };
      void Close() { if ( rwo ) { SDL_RWclose(rwo); rwo = NULL; } };
      ~RWOPS_Handler() { Close(); };
};




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






class StreamCompressionError : public ASCmsgException {
   public:
     StreamCompressionError ( const ASCString& msg, int returncode ) : ASCmsgException ( msg )
     {
        message += "\n the returncode is ";
        message += strrr ( returncode );
     };
};

class OutOfMemoryError : public ASCexception {
  public:
   int required;
   OutOfMemoryError ( int m ) ;
};

class tbufferoverflow : public ASCexception {
};


class tinvalidversion : public tfileerror {
   public:
     tinvalidversion ( const ASCString& fileName, int ex, int fnd );
     const int expected;
     const int found;
     ASCString getMessage() const;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////        dynamic data structures
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** DEPRECATED
    Use containers of the C++ Standard Template Library instead!    
    Container for managing objects of type T
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////        Streams
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Buffers the complete contents of a stream in memory.
 * May be useful when the original stream doesn't support seeking, but the
 * consumer needs it.
 */
class MemoryStreamCopy : public tnstream {
               void* buf;
               int size;
               int pos;


             public:
               MemoryStreamCopy ( tnstream* stream );
               ~MemoryStreamCopy ( );
               void writedata ( const void* buf, int size );
               int  readdata  ( void* buf, int size, bool excpt = true );
               void seek ( int newpos );
               int getPosition ( void ) { return pos; };
               int getSize ( void ) { return size; };
               ASCString getLocation();
         };


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * An in-memory storage of streamed data.
 * A MemoryStream can be used to read or write to it
 */
class MemoryStreamStorage {
           friend class MemoryStream;

           bool initialized;

           int used;
           int allocated;
           int dummy[10];
           Uint8* buf;
        public:
           MemoryStreamStorage();

           //! persist the contents in another stream
           void writetostream ( tnstream* stream );

           //! read persisted contents from another stream
           void readfromstream ( tnstream* stream );

           void clear() { used= 0; };

           int getMemoryFootprint() const { return allocated; };
           
           const Uint8* getBuffer() const { return buf; };
           int getSize() const { return used; };
           
           ~MemoryStreamStorage ( );
      };

/**
 * Reads data from or writes data to a MemoryStreamStorage
 * This allows a completely volatile storage of data.
 */
class MemoryStream : public tnstream {
       protected:
           int   blocksize;
           Uint8* pointer;
           IOMode _mode;
           int   actmempos;
           MemoryStreamStorage* buf;

        public:
           MemoryStream ( MemoryStreamStorage* lbuf, IOMode mode );
           virtual void writedata ( const void* nbuf, int size );
           virtual int  readdata  ( void* nbuf, int size, bool excpt = true );
           int dataavail ( void );
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
              unsigned char *decodeBuffer;
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
              Uint8 rlestartbyte;
              Uint8 rlenum;
              Uint8 rledata;

       // ******* general
              IndexType freecode;

              int initread;
              int initwrite;
              void initreading( void );
              void initwriting( void );

          protected:

              typedef deque<Uint8> CDQ;
              queue<Uint8,CDQ> tempbuf;

              enum tmode { none, reading, writing, readingdirect, readingrle };
              tmode mode;

          public:

             void writedata ( const void* buf, int size );
             int  readdata  ( void* buf, int size, bool excpt = true  );
             virtual int  readlzwdata ( void* buf, int size, bool excpt = true ) = 0;
             virtual void writelzwdata ( const void* buf, int size ) = 0;
             tlzwstreamcompression ( void );
             virtual ~tlzwstreamcompression ( void );
          protected:
             void close( void );

};


class CompressionStreamInterface {
           public:
             virtual void writecmpdata ( const void* buf, int size ) = 0;
             virtual int readcmpdata ( void* buf, int size, bool excpt = true ) = 0;
             virtual ~CompressionStreamInterface() {};
      };


class CompressionStreamAdapter : public CompressionStreamInterface {
   private:
      tnstream* stream;
   public:
      CompressionStreamAdapter( tnstream* compressedStream );
      virtual void writecmpdata ( const void* buf, int size );
      virtual int readcmpdata ( void* buf, int size, bool excpt = true );
};

class PrivateCompressionData;

class libbzip_compression {
            CompressionStreamInterface* stream;
            PrivateCompressionData* data;
         public:
             void close_compression ( void );
             void writedata ( const void* buf, int size );
             libbzip_compression ( CompressionStreamInterface* strm );
             virtual ~libbzip_compression ( );

};

class PrivateDecompressionData;

class libbzip_decompression {
            CompressionStreamInterface* stream;
            PrivateDecompressionData* data;
         public:
             int  readdata  ( void* buf, int size, bool excpt = true  );
             libbzip_decompression ( CompressionStreamInterface* strm );
             virtual ~libbzip_decompression ( );

};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



class tanycompression : public CompressionStreamInterface, public tlzwstreamcompression {

                            typedef deque<Uint8> CDQ;
                            queue<Uint8, CDQ> _queue;

                            libbzip_compression* bzip_compress;
                            libbzip_decompression* bzip_decompress;
                            int mmd;
                            int status;

                          protected:
                             virtual int readlzwdata ( void* buf, int size, bool excpt = true );
                             virtual void writelzwdata ( const void* buf, int size );
                             void close_compression ( void );

                           public:
                             tanycompression ( int md );
                             void init ( void );

                             void writedata ( const void* buf, int size );
                             int  readdata  ( void* buf, int size, bool excpt = true  );
                             ~tanycompression ( );
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class tn_lzw_file_buf_stream : public tn_file_buf_stream, protected tanycompression {
           public:
              tn_lzw_file_buf_stream ( const char* name, IOMode mode ) : tn_file_buf_stream ( name, mode ) , tanycompression ( mode )
                 {
                    tanycompression :: init ( );
                 } ;

              void writedata ( const void* buf, int size );
              int  readdata  ( void* buf, int size, bool excpt = true  );
              int  readcmpdata ( void* buf, int size, bool excpt = true );
              void writecmpdata ( const void* buf, int size );
              ~tn_lzw_file_buf_stream();
           };


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef class tncontainerstream* pncontainerstream;


class tn_c_lzw_filestream : public tnstream, protected tanycompression {
            tn_file_buf_stream *strm;
            pncontainerstream containerstream;
            int inp;
            ASCString fname;
            ASCString location;
         protected:
            int  readcmpdata ( void* buf, int size, bool excpt = true );
            void writecmpdata ( const void* buf, int size );
         public:
            tn_c_lzw_filestream ( const ASCString& name, IOMode mode );
            void writedata ( const void* buf, int size );
            int  readdata  ( void* buf, int size, bool excpt = true  );
            virtual ~tn_c_lzw_filestream  ( );
            virtual time_t get_time ( void );
            virtual int getSize( void );
            ASCString getLocation();
            ASCString getArchive();
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
           virtual ~ContainerIndexer() {};
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
                 int readcontainerdata ( void* buf, int size, bool excpt = true );
                 void closecontainerfile ( void );
                 char* getfirstname ( void );
                 char* getnextname ( void );
                 virtual ~tncontainerstream ();
             };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



extern ASCString listContainer();





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! Searching for files
class tfindfile {


      public:
        struct FileInfo {
            ASCString name;
            int directoryLevel;
            bool isInContainer;
            ASCString location;
            int size;
            int date;

            void read ( tnstream& stream ) {
               int version = stream.readInt();
               if ( version != 1 )
                  throw tinvalidversion( stream.getDeviceName(), 1, version );

               name = stream.readString();
               directoryLevel = stream.readInt();
               isInContainer = stream.readInt() != 0;
               location = stream.readString();
               size = stream.readInt();
               date = stream.readInt();
            };

            void write ( tnstream& stream ) const {
               stream.writeInt(1 ); // version
               stream.writeString ( name );
               stream.writeInt( directoryLevel );
               stream.writeInt ( isInContainer );
               stream.writeString ( location );
               stream.writeInt ( size );
               stream.writeInt ( date );
            };

        };

      private:
        vector<FileInfo> fileInfo;
        int found;
        int act;

      public:
        enum SearchPosition { DefaultDir, CurrentDir, PrimaryDir, AllDirs };
        enum SearchTypes { InsideContainer, OutsideContainer, All };

        /** Searches for files matching the wildcard name in all search paths specified for ASC and inside the ASC archive files.

            If name contains a relative directory entry ( like music / *.mp3 ), searchPosition specifies, which directories
            will be searched for the file. Note that "Current" may be problematic in Unix environments, because the program may be
            started from any directory.
        */
        tfindfile ( ASCString name, SearchPosition searchPosition = DefaultDir, SearchTypes searchTypes = All);

        /** Returns the next entry of the internal file list. Optionally, some additional information about the file can be returned:
           \param loc contains the number of the directory. ASC can search several directories for files. These directories are specified in the ASC configuration file and CGameOptions
           \param inContainer returns whether the file is inside a ASC archive file ( like main.con )
           \param location contains the directory this file resides in
        */
        ASCString getnextname ( int* loc = NULL, bool* inContainer = NULL, ASCString* location = NULL );
        bool getnextname ( FileInfo& fi );
        
        int getFoundFileNum() {
           return found;
        };

     };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Data written to this stream will be encoded similar to base64 and can be retrieved as a string
 *  \see ASCIIDecodingStream
 */
class ASCIIEncodingStream : public tnstream {
   private:
      int shift;
      int buf;

      ASCString result;
   public:
      ASCIIEncodingStream();

      virtual void writedata ( const void* buf, int size );
      virtual int  readdata  ( void* buf, int size, bool excpt = true );

       void put( Uint8 c );
       void flush();
       ASCString getResult();
};

/** Reading data from an ASCII encoded String
 *  \see  ASCIIEncodingStream
 */
class ASCIIDecodingStream : public tnstream {
   private:
      int shift;
      int buf;

      int length;
      ASCString data;

      int reverse[256];

      void generateTable();

      int get();
   public:
      ASCIIDecodingStream( const ASCString& data);
      virtual void writedata ( const void* buf, int size );
      virtual int  readdata  ( void* buffer, int size, bool excpt = true );
};

/**
 * Compresses data transparently into another stream (using bzip2 compression)
 * \see StreamDecompressionFilter
 */
class StreamCompressionFilter : public tnstream {
      CompressionStreamAdapter adapter;
      libbzip_compression compressor;
      bool closed;
   public:
      StreamCompressionFilter( tnstream* outputstream );
      virtual void writedata ( const void* buf, int size );
      virtual int  readdata  ( void* buf, int size, bool excpt = true );
      void close();
      ~StreamCompressionFilter();
};

/**
 * Deompresses data from a compressed stream (using bzip2 compression)
 * \see StreamCompressionFilter
 */
class StreamDecompressionFilter : public tnstream {
      CompressionStreamAdapter adapter;
      libbzip_decompression decompressor;

   public:
      StreamDecompressionFilter( tnstream* inputstream );
      virtual void writedata ( const void* buf, int size ) ;
      virtual int  readdata  ( void* buf, int size, bool excpt = true );
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "basetemp.h"

extern  const char* asc_configurationfile;
extern  const int maxfilenamelength;


extern int compressrle ( const void* p, void* q);

/** checks whether the regular expression pat matches the string str . This functions
    only understands dos/windows style wildcards: * and ?
*/
// extern bool patimat (const char *pat, const char *str);
extern bool patimat (const ASCString& pat, const ASCString& str, bool forceCaseInsensitivity = false) ;

extern int checkforvaliddirectory ( char* dir );

extern ASCString getnextfilenumname ( const ASCString& first, const ASCString& suffix, int num = -1  );


#define writedata2(a)  writedata ( &(a), sizeof(a) )
#define readdata2(a)   readdata  ( &(a), sizeof(a) )

//! does a file s exist (wildcards allowed)
extern bool exist ( const ASCString& s );
extern time_t get_filetime ( const char* devicename );

extern void opencontainer ( const char* wildcard );

extern const char* containermagic;


extern const char* filereadmode;
extern const char* filewritemode;
extern const char* filereadmodetext;
extern const char* filewritemodetext;


//! This class handles filenames. All operations that work on filenames will be added here
class FileName : public ASCString {
    public:
       /** return the suffix of the file or an empty string if the file doesn't have a suffix.
           The suffix is the part of the filename after its last period. */
       ASCString suffix();
       FileName& operator= ( const ASCString& s ) { ASCString::operator=(s); return *this;};
       FileName() {};
       FileName( const ASCString& s ) : ASCString ( s ) {};
};


extern const char pathdelimitter;
extern const char* pathdelimitterstring;
extern int filesize( const char *name);

extern void addSearchPath ( const ASCString& path );
extern void appendbackslash ( char* String );
extern void appendbackslash ( ASCString& String );
extern char* constructFileName( char* buf, int directoryLevel, const char* path, const char* filename );
extern ASCString constructFileName( int directoryLevel, const ASCString& path, ASCString filename );
extern bool directoryExist ( const ASCString& path );
extern char* extractPath ( char* buf, const char* filename );
extern char* extractFileName ( char* buf, const char* filename );
extern ASCString extractFileName ( const ASCString& filename );
extern ASCString extractFileName_withoutSuffix ( const ASCString& filename );
extern int createDirectory ( const char* name );
extern int getSearchPathNum();
extern ASCString getSearchPath ( int i );
extern bool isPathRelative( const ASCString& path );

//! converts path delimitters from foreign operating systems to the ones used by the current operating system. On Linux, this function converts backslashes to slashes, on Windows vice versa
extern void convertPathDelimitters ( ASCString& path );




template<typename C>
      void writePointerContainer ( const C& c, tnstream& stream  )
{
   stream.writeInt ( 1 );
   stream.writeInt ( c.size() );
   typedef typename C::const_iterator IT;
   for ( IT i = c.begin(); i != c.end(); ++i )
      (*i)->write ( stream );
}

template<typename BaseType>
      void readPointerContainer ( vector<BaseType*>& v, tnstream& stream  )
{
   stream.readInt(); // version
   int num = stream.readInt();
   v.clear();
   for ( int i = 0; i < num; ++i ) {
      BaseType* bt = new BaseType;
      bt->read( stream );
      v.push_back( bt );
   }
}

template<typename BaseType>
      void readPointerContainer ( list<BaseType*>& v, tnstream& stream  )
{
   stream.readInt(); // version
   int num = stream.readInt();
   v.clear();
   for ( int i = 0; i < num; ++i ) {
      BaseType* bt = new BaseType;
      bt->read( stream );
      v.push_back( bt );
   }
}



template<typename C>
      void writeClassContainer ( const C& c, tnstream& stream  )
{
   stream.writeInt ( 1 );
   stream.writeInt ( c.size() );
   typedef typename C::const_iterator IT;
   for ( IT i = c.begin(); i != c.end(); ++i )
      i->write ( stream );
}

template<typename C>
      void readClassContainer ( C& c, tnstream& stream  )
{
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion( stream.getLocation(), 1, version );
      
   int num = stream.readInt();
   c.clear();
   for ( int i = 0; i < num; ++i ) {
      typedef typename C::value_type VT;
      VT vt;
      vt.read( stream );
      c.push_back( vt );
   }
}

template<typename C>
      void readClassContainerStaticConstructor ( C& c, tnstream& stream  )
{
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion( stream.getLocation(), 1, version );
      
   int num = stream.readInt();
   c.clear();
   for ( int i = 0; i < num; ++i ) {
      typedef typename C::value_type VT;
      c.push_back( VT::newFromStream( stream ) );
   }
}


template<>
      inline void writeClassContainer<> ( const vector<ASCString>& c, tnstream& stream  )
{
   stream.writeInt ( 1 );
   stream.writeInt ( c.size() );
   typedef vector<ASCString>::const_iterator IT;
   for ( IT i = c.begin(); i != c.end(); ++i )
      stream.writeString(*i);
}


template<>
      inline void readClassContainer<> ( vector<ASCString>& c, tnstream& stream  )
{
   stream.readInt(); // version
   int num = stream.readInt();
   c.clear();
   for ( int i = 0; i < num; ++i )
      c.push_back( stream.readString() );
}

template<>
      inline void writeClassContainer<> ( const vector<int>& c, tnstream& stream  )
{
   stream.writeInt ( 1 );
   stream.writeInt ( c.size() );
   typedef vector<int>::const_iterator IT;
   for ( IT i = c.begin(); i != c.end(); ++i )
      stream.writeInt(*i);
}


template<>
      inline void readClassContainer<> ( vector<int>& c, tnstream& stream  )
{
   stream.readInt(); // version
   int num = stream.readInt();
   c.clear();
   for ( int i = 0; i < num; ++i )
      c.push_back ( stream.readInt() );
}

template<>
      inline void writeClassContainer<> ( const vector<pair<int,int> >& c, tnstream& stream  )
{
   stream.writeInt ( 1 );
   stream.writeInt ( c.size() );
   typedef vector<pair<int,int> >::const_iterator IT;
   for ( IT i = c.begin(); i != c.end(); ++i ) {
      stream.writeInt(i->first);
      stream.writeInt(i->second );
   }
}


template<>
      inline void readClassContainer<> ( vector<pair<int,int> >& c, tnstream& stream  )
{
   stream.readInt(); // version
   int num = stream.readInt();
   c.clear();
   for ( int i = 0; i < num; ++i ) {
      int first = stream.readInt();
      int second = stream.readInt();
      c.push_back ( make_pair(first,second) );
   }
}



#endif

