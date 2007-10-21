/*! \file basestrm.cpp
    \brief The various streams that ASC offers, like file and memory streams. 
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2005  Martin Bickel  and  Marc Schellenberger

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

#include <stdio.h> 
#include <ctype.h>
#include <cstdlib>
#include <stdlib.h>
#include <string>
#include <list>

#include "global.h"

#ifdef HAVE_LIMITS
 #include <limits> 
#endif


#include <sys/stat.h>

#include "global.h"
#include "basestrm.h"

#ifdef _DOS_
  #include "dos/fileio.h"
#else
 #ifdef _WIN32_
   #include "win32/fileio.h"
 #else
  #ifdef _UNIX_
    #include "unix/fileio.h"
  #endif
 #endif
#endif

//#include sdlheader
#include <SDL_endian.h>


#include "messaginghub.h"


 const int maxSearchDirNum = 30;
 int searchDirNum = 0;
 char* ascDirectory[maxSearchDirNum] = { NULL, NULL, NULL, NULL, NULL,
                                         NULL, NULL, NULL, NULL, NULL };




#pragma pack(1)
struct trleheader {
   unsigned short int id;
   unsigned short int size;
   char rle;
   unsigned short int x;
   unsigned short int y;
};

#pragma pack()

#define bzip_xor_byte 'M'

const char* containermagic = "NCBM";

const char* LZ_SIGNATURE  =  "MBLZW16";
const char* RLE_SIGNATURE =  "MBRLE1";
const char* BZIP_SIGNATURE = "MBZLB2X!";





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////        Watchpointer
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CharBuf :: CharBuf ( void )
         {
            size = 0;
            buf = NULL;
         }

CharBuf :: CharBuf ( int _size )

         {
            size = _size;
            buf = new char[ size ];
         }

void CharBuf :: resize ( int newsize )
         {
            char* nb = new char[newsize];
            for ( int i = 0; i < size; i++ )
               nb[i] = buf[i];

            delete[] buf;
            buf = nb;
         }

CharBuf :: ~CharBuf()
         {
            if ( buf )
               delete[] buf;
            buf = NULL;
         }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////        Exceptions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



toutofmem::toutofmem ( int m )
{
  required = m; 
}

tfileerror::tfileerror ( const ASCString& fileName )
{
   _filename = fileName ;
}


/*
tfileerror::~tfileerror()
{
   if ( filename ) {
      delete filename;
      filename = NULL;
   }
}
*/

tinvalidmode :: tinvalidmode ( const ASCString& _fileName, tnstream::IOMode org_mode, tnstream::IOMode requested_mode )
              : tfileerror ( _fileName )
{
   orgmode = org_mode;
   requestmode = requested_mode;
}



treadafterend :: treadafterend ( const ASCString& _fileName )
               : tfileerror ( _fileName )
{

}


tinternalerror::tinternalerror (  const char* filename, int l )
{
   linenum = l;
   sourcefilename = filename;
}



tinvalidversion :: tinvalidversion ( const ASCString& _fileName, int ex, int fnd )
                 : tfileerror ( _fileName ), expected ( ex ), found ( fnd )
{
}

ASCString tinvalidversion :: getMessage() const
{
   ASCString s;
   if ( expected < found )
      s.format( "File/module %s has invalid version.\nExpected file version %d\nFound file version %d\nThe file/module is newer than your application\nPlease install the latest version of ASC from www.asc-hq.org", getFileName().c_str(), expected, found );
   else
      s.format ( "File/module %s has invalid version.\nExpected file version %d\nFound file version %d\nThis is a bug, please report it!", getFileName().c_str(), expected, found );

   return s;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////        Streams
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


tnstream :: tnstream ( void ) 
          : devicename ( "-abstract tnstream-" ) {}

void tnstream::seek ( int pos )
{
   throw tfileerror ( "Seeking not supported for stream " + getDeviceName() );
}

void         tnstream::readrlepict( void** pnter, bool allocated, int* size)
{ 
  trleheader   hd; 
  int          w;
  char*        q;

  hd.id = readWord();
  hd.size = readWord();
  hd.rle = readChar();
  hd.x = readWord();
  hd.y = readWord();

   if (hd.id == 16973) {
      if (!allocated)
        *pnter = new char [ hd.size + sizeof(hd) ];
      memcpy( *pnter, &hd, sizeof(hd));
      q = (char*) (*pnter) + sizeof(hd);

      readdata( q, hd.size);  // endian ok ?
      *size = hd.size + sizeof(hd);
   }
   else {
      w =  (hd.id + 1) * (hd.size + 1) + 4 ;
      if (!allocated)
        *pnter = new char [ w ];
      memcpy ( *pnter, &hd, sizeof ( hd ));
      q = (char*) (*pnter) + sizeof(hd);
      readdata ( q, w - sizeof(hd) ); // endian ok ?
      *size = w;
   }
}



void tnstream :: writerlepict ( const void* buf )
{
   writeImage( buf, true );
}

void tnstream :: writeImage ( const void* buf, bool compress )
{
   if ( compress ) {
      char* tempbuf = new char [ 0xffff ];
      if ( tempbuf ) {
         int   size    = compressrle ( buf, tempbuf );
	 trleheader* hd = (trleheader*) tempbuf;
	 writeWord( hd->id );
	 writeWord( hd->size );
	 writeChar( hd->rle );
	 writeWord( hd->x );
	 writeWord( hd->y );
	 
         writedata ( hd+1, size - sizeof(*hd) );
         delete[] tempbuf;
      } else
         compress = false;
   }

   if ( !compress )  {
      Uint16* pw = (Uint16*) buf;
      writeWord(pw[0] );
      writeWord(pw[1] );
      writedata ( pw+2, ( pw[0] + 1 ) * ( pw[1] + 1 ) );
   }
}


ASCString tnstream::getDeviceName ( void )
{
   return devicename;
}

ASCString tnstream::getLocation ( void )
{
   return devicename;
}

int  tnstream::readInt  ( void )
{
   int i;
   readdata2 ( i );
   return SDL_SwapLE32( i );
}

int tnstream::readWord ( void )
{
   Uint16 w;
   readdata2 ( w );
   return SDL_SwapLE16( w );
}

char tnstream::readChar ( void )
{
   char c;
   readdata2 ( c );
   return c;
}

float SwapFloat( float f )
{
  union
  {
    float f;
    unsigned char b[4];
  } dat1, dat2;

  dat1.f = f;
  dat2.b[0] = dat1.b[3];
  dat2.b[1] = dat1.b[2];
  dat2.b[2] = dat1.b[1];
  dat2.b[3] = dat1.b[0];
  return dat2.f;
}

float tnstream::readFloat ( void )
{
   float c;
   readdata2 ( c );
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
   c = SwapFloat(c);
#endif
   return c;
}

#if SIZE_T_not_identical_to_INT

void tnstream::writeInt  ( size_t i )
{
#ifdef HAVE_LIMITS
   // assert( i <=  numeric_limits<int>::max());
#endif
   writeInt( int(i) );
}

#endif

void tnstream::writeInt  ( unsigned int i )
{
   i = SDL_SwapLE32(i);
   writedata2 ( i );
}

void tnstream::writeInt  ( bool b )
{
   int i = b;
   i = SDL_SwapLE32(i);
   writedata2 ( i );
}

void tnstream::writeInt  ( int i )
{
   i = SDL_SwapLE32(i);
   writedata2 ( i );
}

void tnstream::writeWord ( int w )
{
   Uint16 w2 = SDL_SwapLE16( Uint16(w) );
   writedata2 ( w2 );
}

void tnstream::writeChar ( char c )
{
   writedata2 ( c );
}

void tnstream::writeFloat ( float f )
{
   writedata2 ( f );
}

void         tnstream::readpchar(char** pc, int maxlength )
{ 
   int actpos2 = 0; 

   int maxav = 100000;
                            
   if ( maxlength )
      if ( maxav > maxlength )
         maxav = maxlength;

   CharBuf charbuf ( maxav );

   maxav--;


   char* pch2 = charbuf.buf;

   int loop = 0;

   do {
     actpos2++;
     if ( loop )
        pch2++;
        
     loop++;

     readdata( pch2, 1 ); // endian ok !

   } while (*pch2 != 0   &&   actpos2 < maxav ); /* enddo */

   if ( actpos2 >= maxav ) {
      pch2[1] = 0;
      actpos2++;

      if ( pch2[0] ) {
         char temp;
         do {
            readdata( &temp, 1 ); // endian ok !
         } while ( temp ); /* enddo */
      }
   }
                      
   pch2 = new char [ actpos2 ];

   memcpy ( pch2, charbuf.buf, actpos2 );

   *pc = pch2;
} 


void         tnstream::readpnchar(char** pc, int maxlength )
{ 
   int actpos2 = 0; 

   int maxav = 10000;
                            
   if ( maxlength )
      if ( maxav > maxlength )
         maxav = maxlength;


   CharBuf charbuf ( maxav );

   maxav--;


   char* pch2 = charbuf.buf;

   int ende = 0;

   do {
     char bt;

     int red = readdata( &bt, 1, 0 );

     if ( red < 1 ) {
        ende = 2;
        *pch2 = 0;
     } else
       if ( bt == '\n' || bt == 0 ) {
          *pch2 = 0;
          ende = 1;
       } else
          if ( bt != '\r' ) {
            *pch2 = bt;
            pch2++;
            actpos2++;
          }

   } while ( !ende  && actpos2 < maxav ); /* enddo */


   if ( !ende ) {
      if ( actpos2 >= maxav ) {
         *pch2 = 0;

         if ( pch2[0] ) {
            char temp;
            do {

               int red = readdata( &temp, 1, 0 );
               if ( red < 1 )  {
                  temp = 0;
               }  else
                 if ( temp == '\n' )
                    temp = 0;

            } while ( temp ); /* enddo */
         }
      }
   }
   if ( ende == 2 ) {
      if ( !actpos2 )
         *pc = NULL;
      else
         *pc = strdup ( charbuf.buf );
   } else
      *pc = strdup ( charbuf.buf );
}


bool  tnstream::readTextString ( ASCString& s, bool includeCR  )
{
  s = "";
  char c;
  int red;
  int end = 0;
  do {
     red = readdata( &c, 1, 0 ); // endian ok !
     if ( red < 1 ) {
        end = 2;
     } else
       if ( (c == '\n' && !includeCR) || c == 0 ) {
          end = 1;
       } else
          if ( c != '\r' )
             s += c;
  } while ( red && !end );
  if ( end == 2)
     return false;
  else
     return true;
}


ASCString  tnstream::readString ( bool includeCR )
{
  ASCString s;
  bool data = readTextString ( s, includeCR );
  if ( !data && s.empty() )
     throw treadafterend ( getLocation() );
  return s;
}


void         tnstream::writeString(const string& pc, bool binary )
{
   if ( binary )
      writepchar ( pc.c_str() );
   else
      writedata ( pc.data(), pc.length() );
}


void         tnstream::writepchar(const char* pc)
{ 
   if ( pc ) {
      const char *pch1 = pc;
      int loop = 0;

      do {
         if ( loop )
            pch1++;
         writedata( pch1, 1 );
         loop++;
      } while ( *pch1 > 0  ); /* enddo */
   } else {
       char         pch1 = 0;
       writedata ( &pch1, 1 );
   }

} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




MemoryStreamCopy :: MemoryStreamCopy ( pnstream stream )
{
  buf = NULL;
  int bufused = 0;
  int memreserved = 0;
  int blocksize = 500000;
  int red;
  do {
    if ( bufused + blocksize > memreserved ) {
       int newsize = memreserved + blocksize;
       void* newbuf = malloc (newsize);
       if ( buf ) {
          memcpy ( newbuf, buf, bufused );
          free ( buf );
       }
       buf = newbuf;
       memreserved = newsize;
    }
    char* cp = (char*) buf;
    red = stream->readdata ( cp + bufused, blocksize, 0 ); // endian ok !
    bufused += red;

  } while ( red == blocksize );
  size = bufused;
  pos = 0;

  devicename = stream->getDeviceName();
}

ASCString MemoryStreamCopy::getLocation()
{
  return devicename + " (memory bufferd)";
}

MemoryStreamCopy :: ~MemoryStreamCopy ( )
{
   if ( buf )
      free ( buf );
}


void MemoryStreamCopy :: writedata ( const void* buf, int size )
{
   throw  tinvalidmode ( getDeviceName(), reading, writing );
}

int MemoryStreamCopy :: readdata  ( void* buffer, int _size, bool excpt )
{
   char* cp = (char*) buf;
   if ( pos + _size > size ) {
      if ( excpt )
          throw treadafterend ( getDeviceName() );
      else {
         int tr = size-pos;
         memcpy ( buffer, cp+pos, tr );
         pos += tr;
         return tr;
      }

   } else {
      memcpy ( buffer, cp+pos, _size );
      pos += _size;
      return _size;
   }
}

void MemoryStreamCopy :: seek ( int newpos )
{
   if ( newpos > size || newpos < 0 )
     throw treadafterend ( getDeviceName() );

   pos = newpos;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _SDL_

static int stream_seek( struct SDL_RWops *context, int offset, int whence)
{
	MemoryStreamCopy* stream = (MemoryStreamCopy*) context->hidden.unknown.data1;
	if ( whence == SEEK_SET )
	   stream->seek ( offset );
	else
   	if ( whence == SEEK_CUR )
	      stream->seek ( offset + stream->getPosition() );
	   else
         if ( whence == SEEK_END )
            stream->seek ( offset + stream->getSize() );
  return stream->getPosition();
}


static int stream_read(SDL_RWops *context, void *ptr, int size, int maxnum)

{
	MemoryStreamCopy* stream = (MemoryStreamCopy*) context->hidden.unknown.data1;
	size_t nread = stream->readdata ( ptr, size * maxnum, 0 );

	return(nread / size);
}

static int stream_close(SDL_RWops *context)
{
	if ( context ) {
		if ( context->hidden.unknown.data1 ) {
			MemoryStreamCopy* stream = (MemoryStreamCopy*) context->hidden.unknown.data1;
			delete stream;
		}
		SDL_FreeRW(context);
	}
	return(0);
}


SDL_RWops *SDL_RWFromStream( pnstream stream )
{
   MemoryStreamCopy* msb = new MemoryStreamCopy ( stream );

	SDL_RWops *rwops;

	rwops = SDL_AllocRW();
	if ( rwops != NULL ) {
	   rwops->seek = stream_seek;
	   rwops->read = stream_read;
	   rwops->write = NULL;
	   rwops->close = stream_close;
	   rwops->hidden.unknown.data1 = msb;
	}
	return(rwops);
}

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




tncontainerstream :: tncontainerstream ( const char* containerfilename, ContainerIndexer* indexer, int dirLevel )
        : tn_file_buf_stream ( containerfilename, reading ), index(NULL)
{
   num = 0;
   char magic[4];
   readdata ( &magic, 4 ); // endian ok !
   if ( strncmp ( magic, containermagic, 4 ) == 0) {
      int pos = readInt();
      seek ( pos );
      num = readInt();
      index = new tcontainerindex[num];
      for ( int i = 0; i < num; i++ ) {

         bool __loadName = readInt();
         index[i].start = readInt();
         index[i].end = readInt();
         if ( __loadName ) {
            readpchar ( &index[i].name );

           #if CASE_SENSITIVE_FILE_NAMES == 1
            // quick hack to be able to use existing CON files.
            char *c = index[i].name;
            while ( *c ) {
               *c = tolower( *c );
               c++;
            }
           #endif

            indexer->addfile ( index[i].name, this, dirLevel );
         } else
            index[i].name = NULL;
      }
   }
   actfile = NULL;
   containerfilepos = 0;
}


int  tncontainerstream :: getcontainerfilesize ( const char* name )
{
   int i = 0;
   while ( i < num  &&  stricmp ( index[i].name, name ) )
      i++;

   if ( i >= num )
      return -1;
   else
      return index[i].end - index[i].start + 1;

}


void tncontainerstream :: opencontainerfile ( const char* name )
{
   if ( actfile ) {
      ASCString err = ASCString("two files simultaneously: ") + actfile->name + " and " + name;
      throw tfileerror (  err );
   }

   containerfilepos = 0;
   int i = 0;
   while ( i < num  && stricmp ( index[i].name, name ) )
      i++;

   if ( i >= num )
      throw tfileerror ( name );

   actfile = &index[i];
   containerfilepos = 0;
   seek ( actfile->start );
}

int tncontainerstream :: readcontainerdata ( void* buf, int size, bool excpt  )
{
   if ( actfile->start + containerfilepos + size > actfile->end+1 ) {
      if ( excpt ) 
         throw treadafterend ( actfile->name );
      else {
         int got = readdata ( buf, (actfile->end+1 - actfile->start) - containerfilepos  , excpt );
         containerfilepos+=got;
         return got;

      }
   }

   readdata ( buf, size );
   containerfilepos+=size;
   return size;
}

void tncontainerstream :: closecontainerfile ( void )
{
   actfile = NULL;
}

char* tncontainerstream :: getfirstname ( void )
{
   actname = 0;
   return getnextname();
}

char* tncontainerstream :: getnextname ( void )
{
   if ( actname < num )
      return index[actname++].name;
   else
      return NULL;

}



tncontainerstream  :: ~tncontainerstream  ()
{
   for ( int i = 0; i < num; i++ ) 
      if ( index[i].name )
         delete[] index[i].name;
   delete[] index;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ContainerCollector : public ContainerIndexer {
   public:
      struct FileIndex {
         ASCString name;
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
      FileIndex* getfile ( const ASCString& filename );
      FileIndex* getfirstname ( void );
      FileIndex* getnextname ( void );
      ASCString listContainer();
      virtual ~ContainerCollector();
};


ContainerCollector containercollector;

char* constructFileName( char* buf, int directoryLevel, const char* path, const char* filename )
{
  if ( buf ) {

     const char* filename2 = filename;
     buf[0] = 0;

     // filenames beginning with / or ~/ have an absolute path ; ignore variable path for them
     if ( ! (filename && (filename[0] == pathdelimitter || (filename[0]=='~' && filename[1] == pathdelimitter)) )) {
        if ( path )
           strcpy ( buf, path);
        else
           if ( directoryLevel >= 0 && ascDirectory[ directoryLevel ] )
              strcpy ( buf, ascDirectory[ directoryLevel ]);
     }

     appendbackslash ( buf );

     if ( filename && strchr ( filename, pathdelimitter )) {
     
        char name2[ maxFileStringSize ];
        // filename contains directories
        strcpy ( name2, filename );
        int i = strlen ( name2 )-1;
        while ( name2[i] != pathdelimitter )
           i--;

        name2[i+1] = 0;

        filename2 = &filename[i+1];

        // filename2 is now the pure filename without directory
        // name2 is the directory
        
        if ( buf[0] && name2[0]==pathdelimitter )
           strcpy ( buf, name2+1);
        else
           strcpy ( buf, name2);
     }

     if ( buf[0] == '~' && buf[1] == pathdelimitter ) {
        char* home = getenv ( "HOME" );
        if ( home ) {
           char temp[ maxFileStringSize ];
           strcpy ( temp, buf );
           strcpy ( buf, home );
           appendbackslash ( buf );
           strcat ( buf, &temp[2]);
        }
     }


     appendbackslash ( buf );

     if ( filename2 )
        strcat ( buf, filename2 );
  }

  return buf;
}

bool isPathRelative( const ASCString& path )
{
  if ( path.length() < 2 )
     return true;
     
  if ( path[0] == '~' && path[1] == pathdelimitter )
     return false;

#ifdef WIN32
  if ( path[1] == ':' && path[2] == pathdelimitter )
     return false;

#endif
     
  if ( path[0] == pathdelimitter )
     return false;

  return true;     
}

ASCString constructFileName( int directoryLevel, const ASCString& path, ASCString filename )
{
     ASCString result;
   
     // filenames beginning with / or ~/ have an absolute path ; ignore variable path for them
     if ( isPathRelative( filename )) {
        if ( !path.empty() )
           result += path;
        else
           if ( directoryLevel >= 0 && ascDirectory[ directoryLevel ] )
              result += ascDirectory[ directoryLevel ];
     }

     appendbackslash ( result );

     if ( !filename.empty() && filename.find( pathdelimitter )!= ASCString::npos ) {
        ASCString dir = filename;
        
        dir.erase( dir.rfind( pathdelimitter ) + 1);

        filename.erase( 0, filename.find( pathdelimitter ) + 1 );
        
        if ( dir.find( pathdelimitter ) == 0 )
           dir.erase( 0, 1 );
           
        result = dir;   
     }

     if ( result.length() > 2 && result[0] == '~' && result[1] == pathdelimitter ) {
        char* home = getenv ( "HOME" );
        if ( home ) {
           ASCString temp = result;
           result = home;
           appendbackslash ( result );
           result += temp.substr( 2 );
        }
     }


     appendbackslash ( result );

     result += filename;

     return result;
}



struct FileLocation {
        int directoryLevel;
        pncontainerstream container;
        int found;
     };

void locateFile ( const ASCString& filename, FileLocation* loc )
{
   loc->found = 0;
   ContainerCollector::FileIndex* idx = containercollector.getfile ( filename );
   int maxnum;
   if ( idx ) {
      maxnum = idx->directoryLevel+1;
      loc->directoryLevel = idx->directoryLevel;
      loc->found = 1;
      loc->container = idx->container;
   } else {
      maxnum = searchDirNum;
      loc->container = NULL;
      loc->directoryLevel = -1;
   }

   if ( maxnum ) {
      int localfound = 0;
      for ( int i = 0; i < maxnum && !localfound; i++ ) {
         char buf[2000];
         FILE* fp = fopen ( constructFileName ( buf, i, NULL, filename.c_str()), "r" );
         if ( fp ) {
            localfound = loc->found = 1;
            fclose ( fp );
            loc->container = NULL;
            loc->directoryLevel = i;
         }
      }
   } else {
      char buf[2000];
      FILE* fp = fopen ( constructFileName ( buf, -1, ".", filename.c_str()), "r" );
      if ( fp ) {
         loc->found = 1;
         fclose ( fp );
         loc->container = NULL;
         loc->directoryLevel = -2;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



ASCString listContainer()
{
   return containercollector.listContainer();
}


ContainerCollector :: ContainerCollector ( void )
{
  containernum = 0;
}

void ContainerCollector :: init ( const char* wildcard )
{
   for ( int i = 0; i < searchDirNum; i++ ) {
      DIR *dirp; 
      struct ASC_direct *direntp;

      char buf[ maxFileStringSize ];
      char buf2[ maxFileStringSize ];
      char buf3 [ maxFileStringSize ];
      dirp = opendir( extractPath ( buf2, constructFileName ( buf, i, NULL, wildcard )));
      extractFileName ( buf3, buf );
      if( dirp != NULL ) { 
         for(;;) { 
            direntp = readdir( dirp ); 
            if ( direntp == NULL ) {
               break; 
            }
            if ( patimat ( buf3, direntp->d_name )) {
               container[containernum++] = new tncontainerstream( constructFileName ( buf, i, buf2, direntp->d_name), this, i);
               if ( MessagingHub::Instance().getVerbosity() >= 2 )
                  printf("container %s mounted\n", buf );
            }
         } 
         closedir( dirp ); 
      } 
   }
}

void ContainerCollector :: addfile ( const char* filename, const pncontainerstream stream, int directoryLevel )
{
   int found = 0;
   FileIndex* cci = NULL;

   int i1 = toupper ( filename[0] );
   for ( int i = 0; i <= index[i1].getlength(); i++ )
      if ( index[i1][i].name.compare_ci ( filename ) == 0 ) 
         if ( index[i1][i].directoryLevel <= directoryLevel ) 
            return;
         else {
            cci = &(index[i1][i]);
            found = 1;
         }

   if ( !found )
      cci = &( index[i1][ index[i1].getlength()+1 ] );

   cci->name = filename;
   cci->container = stream;
   cci->directoryLevel = directoryLevel;
}

ContainerCollector::FileIndex* ContainerCollector :: getfile ( const ASCString& filename )
{
   int i1 = toupper ( filename[0] );
   for ( int i = 0; i <= index[i1].getlength(); i++ )
      if ( index[i1][i].name.compare_ci ( filename) == 0 )
         return &index[i1][i];

   return NULL;
}


ContainerCollector::FileIndex* ContainerCollector :: getfirstname ( void )
{
   namesearch.alpha = 0;
   namesearch.index = 0;
   return getnextname();
}

ContainerCollector::FileIndex* ContainerCollector :: getnextname ( void )
{
   while ( index[namesearch.alpha].getlength() < namesearch.index) {
      if ( namesearch.alpha == 255 )
         return NULL;
      namesearch.alpha++;
      namesearch.index = 0;
   } /* endwhile */
   return &index[namesearch.alpha][namesearch.index++];
}

ASCString ContainerCollector :: listContainer()
{
   ASCString s;
   for ( int i = 0; i < containernum; i++ )
      s += container[i]->getLocation() + "\n";

   return s;
   
}



ContainerCollector :: ~ContainerCollector()
{
  int i;
   for (i = 0; i < containernum; i++ )
      delete container[i];
   containernum = 0;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



libbzip_compression :: libbzip_compression ( p_compressor_stream_interface strm  )
{
   bzs.bzalloc = NULL;
   bzs.bzfree = NULL;
   bzs.opaque = NULL;

   BZ2_bzCompressInit ( &bzs, 5, 0, 0 );

   outputbufsize = 100000;
   outputbuf = new char [ outputbufsize ];

   stream = strm;
}


void libbzip_compression :: writedata ( const void* buf, int size )
{
   char* cbuf = (char*) buf;

   bzs.next_in = cbuf ;
   bzs.avail_in = size ;
   bzs.total_in_lo32 = 0 ;
   bzs.total_in_hi32 = 0 ;

   while ( bzs.total_in_lo32 < size ) {

     bzs.next_out = outputbuf;
     bzs.avail_out = outputbufsize;
     bzs.total_out_lo32 = 0;
     bzs.total_out_hi32 = 0;

     int res = BZ2_bzCompress ( &bzs, BZ_RUN );
     if ( res < 0 )
        throw tcompressionerror ( "MBZLB2 compression :: writedata", res );

     for ( int i = 0; i < bzs.total_out_lo32; i++ )
        outputbuf[i] ^= bzip_xor_byte;

     stream->writecmpdata ( outputbuf, bzs.total_out_lo32 );
   }
}


void libbzip_compression :: close_compression ( void )
{
   int res;
   do {
     bzs.next_in = outputbuf;
     bzs.avail_in = 0;

     bzs.next_out = outputbuf;
     bzs.avail_out = outputbufsize;
     bzs.total_out_lo32 = 0;
     bzs.total_out_hi32 = 0;

     res = BZ2_bzCompress ( &bzs, BZ_FINISH );
     if ( res < 0 )
        throw tcompressionerror ( "MBZLB2 compression :: closecompression", res );

     for ( int i = 0; i < bzs.total_out_lo32; i++ )
        outputbuf[i] ^= bzip_xor_byte;
     stream->writecmpdata ( outputbuf, bzs.total_out_lo32 );

   } while ( res != BZ_STREAM_END );


   BZ2_bzCompressEnd ( &bzs );
}

libbzip_compression :: ~libbzip_compression ( )
{
   if ( outputbuf ) {
      delete[] outputbuf;
      outputbuf = NULL;
   }
}




libbzip_decompression :: libbzip_decompression ( p_compressor_stream_interface strm  )
{
   bzs.bzalloc = NULL;
   bzs.bzfree = NULL;
   bzs.opaque = NULL;

   BZ2_bzDecompressInit ( &bzs, 0, 0 );

   inputbufsize = 100000;
   inputbuf = new char [ inputbufsize ];
   inputbufused = 0;
   inputbufread = 0;

   stream = strm;
}


int libbzip_decompression :: readdata ( void* buf, int size, bool excpt )
{
   int decompressed = 0;
   char* cbuf = (char*) buf;

   bzs.next_in = cbuf ;
   bzs.avail_in = size ;
   bzs.total_in_lo32 = 0 ;
   bzs.total_in_hi32 = 0 ;

   int abrt = 0;

   while ( decompressed < size  && !abrt ) {
     if ( inputbufread >= inputbufused ) {
        inputbufused = stream->readcmpdata ( inputbuf, inputbufsize, 0 );

        if ( !inputbufused && excpt )
           throw tcompressionerror ( "Decompressor :: out of data", 0 );


        for ( int i = 0; i < inputbufused; i++ )
           inputbuf[i] ^= bzip_xor_byte;

        inputbufread = 0;
     }
     bzs.next_in = inputbuf + inputbufread;
     bzs.avail_in = inputbufused - inputbufread;
     bzs.total_in_lo32 = 0;
     bzs.total_in_hi32 = 0;

     bzs.next_out = cbuf + decompressed;
     bzs.avail_out = size - decompressed;
     bzs.total_out_lo32 = 0;
     bzs.total_out_hi32 = 0;

     int res = BZ2_bzDecompress ( &bzs );
     decompressed += bzs.total_out_lo32;
     inputbufread += bzs.total_in_lo32;

     if ( decompressed < size  ) {
        if ( res == BZ_STREAM_END ) {
           if ( excpt )
              throw treadafterend ( "BZ_decompress_stream" );
           abrt = 1;
        } else {
           if ( res != BZ_OK ) {
              if ( excpt ) {
                 if ( res == BZ_MEM_ERROR )
                    throw toutofmem ( -1 );
                 else
                    throw tcompressionerror ( "MBZLB2 decompression :: readdata", res );
              }
              abrt = 1;
           }
        }
     }
   }
   return decompressed;
}


libbzip_decompression :: ~libbzip_decompression ( )
{
   if ( inputbuf ) {
      BZ2_bzDecompressEnd ( &bzs );
      delete[] inputbuf;
      inputbuf = NULL;
   }
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
t_compressor_2ndbuf_filter :: t_compressor_2ndbuf_filter ( t_compressor_stream_interface* strm )
{
   stream = strm;
}

void t_compressor_2ndbuf_filter :: writecmpdata ( const void* buf, int size )
{
   stream->writecmpdata ( buf, size );
}

int t_compressor_2ndbuf_filter :: readcmpdata ( void* buf, int size, bool excpt )
{
   int got = 0;

   char* pc = (char*) buf;

   while ( size && _queue.size() ) {
       *pc = _queue.front();
       _queue.pop();
       pc++;
       size--;
       got++;
   }

   if ( size )
      got += stream->readcmpdata ( pc, size, excpt );

   return got;
}

void t_compressor_2ndbuf_filter :: insert_data_into_queue ( const void* buf, int size )
{
   char* pc = (char*) buf;
   for (int i = 0; i < size; i++) {
      _queue.push ( *pc );
      pc++;
   }
}
*/



tanycompression :: tanycompression ( int md )
{
   mmd = md;
   bzip_compress = NULL;
   bzip_decompress = NULL;
}

void tanycompression :: init ( void )
{
   if ( mmd == 1 ) {
      char buf[10];

      int maxlen = strlen ( BZIP_SIGNATURE ) + 1;
      int bufdatanum = readcmpdata ( buf, maxlen, 0 );
      int siglen = 0;

      if ( bufdatanum  == maxlen ) {

         if ( strncmp ( &buf[1], LZ_SIGNATURE, 9 ) == 0  && !buf[0]) {
            status = 110;
            siglen = 0;
         } else
         if ( strncmp ( &buf[1], RLE_SIGNATURE, 9 ) == 0 && !buf[0]) {
            status = 111;
            siglen = 0;
         } else
         if ( strncmp ( buf, BZIP_SIGNATURE, 9 ) == 0 ) {
            status = 112;
            siglen = strlen ( BZIP_SIGNATURE ) + 1;
            bzip_decompress = new libbzip_decompression ( this );
         } else
            status= 109;

      } else
         status = 109;

      for ( int i = siglen; i < bufdatanum; i++ )
          _queue.push ( buf[i] );


   } else {
       status = 201;
       bzip_compress = new libbzip_compression ( this );
       writecmpdata ( BZIP_SIGNATURE, strlen ( BZIP_SIGNATURE ) + 1 );
   }
}


int  tanycompression :: readdata ( void* rbuf, int size, bool excpt )
{
   int red = 0;
   if ( size ) {
      switch ( status ) {
      case 109: red += readlzwdata ( rbuf, size, excpt );
         break;
      case 110:
      case 111: red += tlzwstreamcompression :: readdata ( rbuf, size, excpt );
         break;
      case 112: red += bzip_decompress -> readdata ( rbuf, size, excpt );
         break;
      } /* endswitch */
   }
   return red;   
}                          



void tanycompression :: writedata ( const void* buf, int size )
{
  bzip_compress -> writedata ( buf, size );
  // writecmpdata ( buf, size );
}


int tanycompression :: readlzwdata ( void* buf, int size, bool excpt )
{
   if ( _queue.size() ) {
      int got = 0;

      char* pc = (char*) buf;

      while ( size && _queue.size() ) {
          *pc = _queue.front();
          _queue.pop();
          pc++;
          size--;
          got++;
      } /* endwhile */

      if ( size )
         got += readcmpdata ( pc, size, excpt );

      return got;
   } else
      return readcmpdata ( buf, size, excpt );
}

void tanycompression :: writelzwdata ( const void* buf, int size )
{
   writecmpdata ( buf, size );
}


void tanycompression :: close_compression ( void )
{
   if ( bzip_compress ) {
      bzip_compress->close_compression ( );
      delete bzip_compress;
      bzip_compress = NULL;
   }
}


tanycompression :: ~tanycompression ( )
{
   if ( bzip_decompress ) {
      delete bzip_decompress;
      bzip_decompress = NULL;
   }

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
void tn_lzw_bufstream :: writedata ( const void* buf, int size )
{
   tlzwstreamcompression :: writedata ( buf, size );
}

int tn_lzw_bufstream :: readdata  ( void* buf, int size, bool excpt  )
{
   return tlzwstreamcompression :: readdata ( buf, size, excpt );
}

int  tn_lzw_bufstream :: readlzwdata ( void* buf, int size, bool excpt  )
{
   return tnbufstream :: readdata ( buf, size, excpt );
}

void tn_lzw_bufstream :: writelzwdata ( const void* buf, int size )
{
   tnbufstream :: writedata ( buf, size );
}

tn_lzw_bufstream :: ~tn_lzw_bufstream ()
{
   tlzwstreamcompression :: close();
   tnbufstream :: close();
}

*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tn_lzw_file_buf_stream :: writedata ( const void* buf, int size )
{
   tanycompression :: writedata ( buf, size );
}

int  tn_lzw_file_buf_stream :: readdata  ( void* buf, int size, bool excpt  )
{
   return tanycompression :: readdata ( buf, size, excpt );
}

int  tn_lzw_file_buf_stream:: readcmpdata ( void* buf, int size, bool excpt  )
{
   return tn_file_buf_stream :: readdata ( buf, size, excpt  );
}

void tn_lzw_file_buf_stream :: writecmpdata ( const void* buf, int size )
{
   tn_file_buf_stream :: writedata ( buf, size );
}

tn_lzw_file_buf_stream :: ~tn_lzw_file_buf_stream()
{
   close_compression ();
   tn_file_buf_stream :: close();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


tn_c_lzw_filestream :: tn_c_lzw_filestream ( const ASCString& name, IOMode mode ) : tanycompression ( mode )
{
   #ifdef logfiles
    FILE* fp = fopen ( "files.lst", "at" );
    fprintf ( fp, "%s\n", name );
    fclose ( fp );
   #endif

   strm = NULL;
   inp = 0;
   containerstream = NULL;

   FileLocation fl;
   if ( mode == tnstream::reading ) {
      locateFile ( name, &fl );

      if ( !fl.found )
         throw tfileerror ( name );

   } else {
      fl.directoryLevel = 0;
      fl.container = NULL;
   }


   if ( fl.container == NULL ) {
      char string[2000];

      ASCString fileNameComplete = constructFileName ( string, fl.directoryLevel, NULL, name.c_str());
      strm = new tn_file_buf_stream ( fileNameComplete, mode );
      inp = 1;
      devicename = fileNameComplete;
      location = fileNameComplete;

   } else {
      containerstream = fl.container;
      if ( containerstream ) {
         containerstream->opencontainerfile ( name.c_str() );
         devicename = name;
         location = name + " located inside " + containerstream->getDeviceName();
         inp = 2;
      } else
         throw tfileerror ( name );
   }
   fname = name;

   tanycompression :: init (  );
}

ASCString tn_c_lzw_filestream::getLocation()
{
   return location;
}

int tn_c_lzw_filestream :: getSize ( void )
{
   if ( inp == 2 )
      return containerstream->getSize();
   else
      return strm->getSize();
}

void tn_c_lzw_filestream :: writecmpdata ( const void* buf, int size )
{
   if ( inp == 2 )
      throw tinvalidmode ( fname, tnstream::reading, tnstream::writing );
   else
      strm->writedata ( buf, size );
}

int tn_c_lzw_filestream :: readcmpdata  ( void* buf, int size, bool excpt  )
{
   if ( inp == 2 )
      return containerstream->readcontainerdata ( buf, size, excpt );
   else
      return strm->readdata ( buf, size, excpt  );
};


void tn_c_lzw_filestream :: writedata ( const void* buf, int size )
{
   tanycompression :: writedata ( buf, size );
}

int tn_c_lzw_filestream :: readdata  ( void* buf, int size, bool excpt  )
{
   if ( tanycompression :: mode == readingdirect  && !tempbuf.size() )
      if ( inp == 2 )
         return containerstream->readcontainerdata ( buf, size, excpt  );
      else
         return strm->readdata ( buf, size, excpt  );
   else
      return tanycompression :: readdata ( buf, size, excpt );
};


time_t tn_c_lzw_filestream :: get_time ( void )
{
   if ( inp == 2 )
      return containerstream->get_time();
   else
      return strm->get_time();
}

tn_c_lzw_filestream :: ~tn_c_lzw_filestream()
{
   close_compression ();
   close();
   if ( inp == 1 ) {
      delete strm;
      strm = NULL;
   } else
      containerstream->closecontainerfile();
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int    compressrle ( const void* p, void* q)
{
   trleheader* sourcehead = (trleheader*) p;
   if ( sourcehead->id == 16973 ) {
      memcpy ( q, p, sourcehead->size + sizeof ( trleheader ) );
      return sourcehead->size + sizeof ( trleheader );
   }


   char* s = (char*) p;
   char* d = (char*) q;

   trleheader* header = (trleheader*) q;

   Uint16 x,y;
   int size;
   {
      Uint16* pw = (Uint16*) s;
      x = pw[0];
      y = pw[1];

      header->x = x;
      header->y = y;

      x++;
      y++;

      size = x * y;

      header->id = 16973;
   }

   {
      int bts[256];
      memset ( bts, 0, sizeof ( bts ));
      for ( int i = 0; i < size ;i++ )
         bts[int(s[i+4])]++;

      int min = 70000;
      for ( int i = 0; i < 256; i++ )
         if ( bts[i] < min ) {
            min = bts[i];
            header->rle = i;
         }

   }

   s+=4;
   d+=sizeof ( trleheader );

   {
      char* startpos = d;
      int xp;

      for (int j = 0; j < y ; j++ ) {
         xp = 0;
         unsigned char num;
         unsigned char actbyte ;

         do {
            num = 1;
            actbyte = *s;
   
            while ( xp+num < x   &&   num < 255 && s[num] == actbyte ) 
               num++;
   
            if ( num > 2  ||  actbyte == header->rle ) {
               *(d++) = header->rle;
               *(d++) = num;
               *(d++) = actbyte;
            } else {
               *(d++) = actbyte;
               if ( num > 1 )
                  *(d++) = actbyte;
            }

            s  += num;   
            xp += num;
         } while ( xp < x );


      } /* endfor */

      header->size = d - startpos;
   }
   return header->size + sizeof ( trleheader );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




bool patimat (const char *pat, const char *str, bool forceCaseInsensitivity )
{
   switch (*pat)
   {
      case '\0':
         return !*str;

      case '*' :
         return patimat(pat+1, str, forceCaseInsensitivity) || *str && patimat(pat, str+1, forceCaseInsensitivity);

      case '?' :
         return *str && patimat(pat+1, str+1, forceCaseInsensitivity);

      default  :
         if ( forceCaseInsensitivity ||
#if CASE_SENSITIVE_FILE_NAMES == 0
              true
#else
              false
#endif
           )
            return (toupper(*pat) == toupper(*str)) && patimat(pat+1, str+1, forceCaseInsensitivity);
         else
            return (*pat == *str) && patimat(pat+1, str+1, forceCaseInsensitivity );
      }
}

bool patimat (const ASCString& pat, const ASCString& str, bool forceCaseInsensitivity)
{
   return patimat( pat.c_str(), str.c_str(), forceCaseInsensitivity );
}


tfindfile :: tfindfile ( ASCString name, SearchPosition searchPosition, SearchTypes searchTypes )
{
   convertPathDelimitters ( name );

   if ( searchPosition == DefaultDir )
      searchPosition = AllDirs;

   
   if ( searchDirNum == 0 )
      searchPosition = CurrentDir;
   
   found = 0;
   act = 0;
   if ( name.empty() )
      return;

   ASCString directory[maxSearchDirNum];
   int dirNum;
   ASCString wildcard;

   int ppos = name.rfind ( pathdelimitterstring );
   if ( ppos != name.npos ) {
      // name contains a directory entry


      // checking if absolute or relative path
      bool absolute = false;
      if ( name[0] == pathdelimitter )
         absolute = true;

      if ( has_drive_letters && name.length() > 3 && name.find ( ":\\", 1 ) != name.npos )
         absolute = true;

      if ( absolute || searchPosition == CurrentDir ) {
         directory[0].assign ( name, 0, ppos );
         dirNum = 1;
      } else {
         ASCString strippedPath;
         strippedPath.assign ( name, 0, ppos );
         if ( name.find ( ASCString(".") + pathdelimitterstring ) == 0 )
            strippedPath.erase( 0, 2);

         int upDir = 0;
         while ( name.find ( ASCString("..") + pathdelimitterstring ) == 0 ) {
            upDir++;
            strippedPath.erase ( 0, 3 );
         }

         int dirsToProcess;
         if ( searchPosition == AllDirs ) {
            dirsToProcess = searchDirNum;
         } else
            dirsToProcess = 1;

         dirNum = 0;
         for ( int i = 0; i < dirsToProcess; i++ ) {
            ASCString dir = ascDirectory[i];

            // removing the trailing pathdelimitterstring
            dir.erase ( dir.length() -1 );

            for ( int j = 0; j < upDir; j++ ) {
               int pos = dir.rfind ( pathdelimitterstring );
               if ( pos > 0 && pos == dir.npos )
                  dir.erase ( pos );
            }

            // append the trailing pathdelimitterstring again
            dir += pathdelimitterstring;

            directory[dirNum++] = dir + strippedPath;
         }

         if ( !dirNum  ) {
            directory[0] = ".";
            dirNum = 1;
         }

      }

      wildcard.assign ( name, ppos+1, name.npos );

   } else {
      if ( searchDirNum ) {
         for (int i = 0; i < searchDirNum; i++ )
            directory[i] = ascDirectory[i];
         dirNum = searchDirNum;
      } else {
         directory[0] = ".";
         dirNum = 1;
      }
      wildcard = name;
   }

   if ( searchTypes == All || searchTypes == OutsideContainer )
      for ( int i = 0; i < dirNum; i++ ) {
         DIR *dirp;
         struct ASC_direct *direntp;

         dirp = opendir( directory[i].c_str() );
         if( dirp != NULL ) {
           for(;;) {
             direntp = readdir( dirp );
             if ( direntp == NULL )
                break;
 
             if ( patimat ( wildcard.c_str(), direntp->d_name )) {
                int localfound = 0;
                for ( int j = 0; j < found; j++ )
                   if ( strcmpi ( fileInfo[j].name.c_str(), direntp->d_name ) == 0 )
                      localfound++;

                if ( !localfound ) {
                   FileInfo fi;
                   fi.name = direntp->d_name;
                   fi.directoryLevel = i ;
                   fi.isInContainer = false ;
                   fi.location = directory[i];

                   char buf[1000];
                   ASCString fullName = constructFileName( buf, i, NULL, direntp->d_name );

                   struct stat statbuf;
                   stat( fullName.c_str(), &statbuf);

                   fi.size = statbuf.st_size ;
                   fi.date = statbuf.st_mtime;

                   fileInfo.push_back ( fi );

                   found++;
                }
             }
           }
           closedir( dirp );
         }
      }



   if ( searchTypes == All || searchTypes == InsideContainer ) {
      const ContainerCollector::FileIndex* c = containercollector.getfirstname();
      while ( c ) {
          if ( patimat ( name.c_str(), c->name ) ) {
             int f = 0;
             for ( int i = 0; i < found; i++ )
                if ( stricmp ( c->name.c_str(), fileInfo[i].name.c_str() ) == 0 ) {
                   if ( fileInfo[i].directoryLevel <= c->directoryLevel )
                      f = 1;
                   else {
                      FileInfo& fi = fileInfo[i];
                      fi.name = c->name ;
                      fi.isInContainer = true;
                      fi.directoryLevel = c->directoryLevel;
                      fi.location = c->container->getDeviceName();

                      fi.size = c->container->getstreamsize();
                      fi.date = c->container->get_time();
                      f = 1;
                   }
                }

             if ( !f ) {
                FileInfo fi;
                fi.name = c->name ;
                fi.directoryLevel = c->directoryLevel ;
                fi.isInContainer = true ;
                fi.location = c->container->getDeviceName() ;

                fi.size = c->container->getstreamsize() ;
                fi.date = c->container->get_time() ;
                fileInfo.push_back ( fi );
                found++;
             }
          }
          c = containercollector.getnextname();
      }
   }
}


ASCString tfindfile :: getnextname ( int* loc, bool* inContainer, ASCString* location )
{
   if ( act < found ) {
      if ( loc )
         *loc = fileInfo[act].directoryLevel;

      if ( inContainer )
         *inContainer = fileInfo[act].isInContainer;

      if ( location )
         *location = fileInfo[act].location;

      /*
      if ( directoryLevel[act] >= 0 && this->location[act] != ascDirectory[directoryLevel[act]] ) {
         ASCString s = this->location[act];
         appendbackslash ( s );
         s += names[act++];
         return s;
      } else */
         return fileInfo[act++].name;
   } else {
      if ( loc )
         *loc = -1;

      return "";
   }
}

bool tfindfile :: getnextname ( FileInfo& fi )
{
   if ( act < found ) {
      fi = fileInfo[act++];
      return true;
   } else
      return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// tncontainerstream* containerstream = NULL;



int checkforvaliddirectory ( char* dir )
{
   int stat = 0;

      DIR *dirp; 
      struct ASC_direct *direntp; 
  
/*      char temp[200];
      int l = strlen(dir) - 1;
      for (int i = 0; i < l; i++) {
         temp[i] = dir[i];
      } 
      temp[i] = 0;
*/
      dirp = opendir( dir ); 
      if( dirp != NULL ) { 
        for(;;) { 
          direntp = readdir( dirp ); 
          if ( direntp == NULL ) 
             break;

          if ( strcmp ( direntp -> d_name, ".") == 0 )
             stat = 1;
        } 
        closedir( dirp ); 
      } 

   return stat;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  tmemorystreambuf :: tmemorystreambuf ( void )
  {
     initialized = false;
     used = 0;
     allocated = 0;
     buf = 0;
     memset ( dummy, 0, sizeof ( dummy ));
  }

  tmemorystreambuf :: ~tmemorystreambuf ( void )
  {
     if ( buf ) {
        delete[] buf;
        buf = NULL;
     }
  }

  void tmemorystreambuf :: writetostream ( pnstream stream )
  {
     if ( stream ) {
        stream->writeInt ( initialized );
        stream->writeInt ( used );
        stream->writeInt ( allocated );
        for ( int i = 0; i < 10; i++ )
           stream->writeInt ( dummy[i] );
        if ( used > 0 )
           stream->writedata ( buf, used );
     }
  }

  void tmemorystreambuf :: readfromstream ( pnstream stream )
  {
     if ( stream ) {
        initialized = stream->readInt();
        used        = stream->readInt();
        allocated   = stream->readInt();
        for ( int i = 0; i< 10; i++ )
           dummy[i] = stream->readInt();

        if ( buf ) {
           delete[] buf;
           buf = NULL;
        }
        if ( used > 0 || allocated > 0 ) {
           allocated = max(allocated,used);
           buf = new char[allocated];
           stream->readdata ( buf, used );
        }
     }
  }


tmemorystream :: tmemorystream ( tmemorystreambuf* lbuf, IOMode lmode )
{

   blocksize = 1024;
   buf = lbuf;
   _mode = lmode;

   if ( !buf )
      throw tfileerror ( "memorystream" );


   if ( _mode == reading ) {
      zeiger = buf->buf;
      actmempos = 0;
   } else
   if ( _mode == writing ) {     // neuen Puffer anlegen
      if ( buf->buf ) {
         delete[] buf->buf;
         buf->buf = NULL;
      }
      buf->buf = new char[blocksize];
      buf->allocated = blocksize;
      buf->used = 0;
      zeiger = buf->buf;
      actmempos = 0;
   }
   if ( _mode == appending ) {
      zeiger = buf->buf;
      actmempos = buf->used;
      _mode = writing;
   }
}

void tmemorystream :: writedata ( const void* nbuf, int size )
{
   if ( _mode != writing )
      throw tinvalidmode ( "memorystream", _mode, writing );

   if ( buf->used + size > buf->allocated ) {
      int newsize = ((buf->used + size + blocksize - 1) / blocksize);
      newsize *= blocksize;
      char* tmp = new char[newsize];
      memcpy ( tmp, buf->buf, buf->used );
      delete[] buf->buf;
      buf->buf = tmp;
      buf->allocated = newsize;
   }

   memcpy ( &buf->buf[buf->used], nbuf, size );
   buf->used += size;
}   


int  tmemorystream :: readdata ( void* nbuf, int size, bool excpt  )
{
   if (_mode != reading )
      throw  tinvalidmode ( "memorystream", _mode, reading );
      
   if ( actmempos + size > buf->used )
      if ( excpt )
         throw treadafterend ( "memory stream" );
      else
         size = buf->used-actmempos;

   memcpy ( nbuf, &buf->buf[actmempos], size );
   actmempos += size;
   return size;
}

int tmemorystream :: dataavail ( void )
{
   if ( _mode == writing )
      return 1;
   else
      return actmempos < buf->used;
}



ASCString getnextfilenumname ( const ASCString& first, const ASCString& suffix, int num )
{
   ASCString name;
   
   if ( num < 0 )
     num = 0;

   do {
      name = first;
      while ( name.length() - first.length() + ASCString::toString(num).length() < 3 )
         name += "0";

      name += ASCString::toString(num) + "." + suffix;

      tfindfile ff ( name );
      ASCString c = ff.getnextname();
      if ( c.empty() )
         return name;
      
      num++;
   } while ( true ); 

   return "";
}


bool exist ( const ASCString& s )
{
   tfindfile ff ( s );
   return !ff.getnextname().empty();
}



#include "oldlzw.cpp"

void  tnbufstream :: writebuffer( void ) {
}

void opencontainer ( const char* wildcard )
{
   if ( !searchDirNum )
      addSearchPath(".");

   containercollector.init ( wildcard );
}




time_t get_filetime ( const char* fileName )
{
   FileLocation fl;
   locateFile ( fileName, &fl );

   if ( fl.found ) {
     if ( fl.container )
        return fl.container->get_time();
     else {
        struct stat stbuf;
        char buf[ maxFileStringSize ];
        if ( !stat ( constructFileName ( buf, fl.directoryLevel, NULL, fileName), &stbuf) )
           return ( stbuf.st_mtime);
        else
           return -1;
     }
   } else
      return -1;
}


int filesize( const char *name)
{
  struct stat buf;

  if ( !stat (name, &buf))
     return (buf.st_size);
  else
     return -1;
}


bool directoryExist ( const ASCString& path )
{
   bool existence = false;

   DIR *dirp = opendir( path.c_str() );

   if( dirp ) {
      if ( readdir( dirp ) )
         existence = true;
      else
         existence = false;

      closedir( dirp );
   }
   return existence;
}

void addSearchPath ( const ASCString& path )
{
   if ( !path.empty() ) {
      ASCString s = constructFileName ( -3, path, "" );

      if ( directoryExist( s.c_str() )) {
         bool found = false;
         for ( int i = 0; i < searchDirNum; i++ )
            if ( s == ascDirectory[i] )
               found = true;
   
         if ( !found )
            ascDirectory[ searchDirNum++ ] = strdup ( s.c_str() );
      }
   }
}

int getSearchPathNum()
{
   return searchDirNum;
}

ASCString getSearchPath ( int i )
{
   if ( i < searchDirNum )
      return ascDirectory[i];
   else
      return "";
}


char* extractPath ( char* buf, const char* filename )
{
   if ( buf && filename ) {
      if ( strchr ( filename, pathdelimitter )) {
         strcpy ( buf, filename );
         int i = strlen ( buf )-1;
         while ( buf[i] != pathdelimitter )
            i--;

         buf[i+1] = 0;
      } else
         strcpy ( buf, "./" );
   }
   return buf;
}

char* extractFileName ( char* buf, const char* filename )
{
   if ( buf && filename ) {
      if ( strchr ( filename, pathdelimitter )) {
         int i = strlen ( filename )-1;
         while ( filename[i] != pathdelimitter )
            i--;

         strcpy ( buf, filename +i+1);
      } else
         strcpy ( buf, filename );
   }
   return buf;
}

ASCString extractFileName ( const ASCString& filename )
{
   char buf[10000];
   return extractFileName( buf, filename.c_str() );
}

ASCString extractFileName_withoutSuffix ( const ASCString& filename )
{
   char buf[10000];
   extractFileName( buf, filename.c_str() );
   char* c = strchr ( buf, '.' );
   if ( c )
      *c = 0;
   return ASCString(buf);
}


void appendbackslash ( char* string )
{
   if ( strlen ( string ) && string[strlen ( string ) -1] != pathdelimitter )
      strcat ( string, pathdelimitterstring );
}

void appendbackslash ( ASCString& string )
{
   if ( !string.empty()  && string[ string.length() -1] != pathdelimitter )
      string += pathdelimitterstring ;
}


int createDirectory ( const char* name )
{
   #ifdef _UNIX_
    char *nname;
    int i;

    if (name == NULL || (nname=strdup(name)) == NULL)
	return -1;
    i = strlen(nname);
    /* leave one '/' */
    while (i>1 && nname[i-1] == '/')
		nname[--i] = '\0';
    i = mkdir ( nname, 0700 );
    free(nname);

    return i;
   #else
    return mkdir ( name );
   #endif
}


ASCString FileName::suffix ( )
{
   size_type slash = find_last_of ( pathdelimitterstring );
   size_type point = find_last_of ( "." );
   if ( point == npos )
      return "";
   else
      if ( slash == npos || slash < point )
         return substr(point+1);
      else
         return "";
}



void convertPathDelimitters ( ASCString& path )
{
   int pos;
   while ( (pos = path.find ( foreignPathDelimitterString )) != path.npos )
      path.replace ( pos, 1, pathdelimitterstring );
}
