//     $Id: basestrm.cpp,v 1.43 2000-10-12 20:21:40 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.42  2000/10/12 19:51:43  mbickel
//      Added a stub program for generating a weapon guide
//      Added makefiles to compile this weaponguide with the free borland C++
//        compiler
//      Made some adjustments to basic IO file for compiling them with borland
//        C++
//
//     Revision 1.41  2000/10/12 19:00:20  mbickel
//      Fixed crash in building placement
//      Replaced multi-character character constants by strings (there where
//        problems with the byte order)
//      Building ID and name are now correctly displayed in mapeditor
//
//     Revision 1.40  2000/10/11 14:26:16  mbickel
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
//     Revision 1.39  2000/09/26 18:05:13  mbickel
//      Upgraded to bzlib 1.0.0 (which is incompatible to older versions)
//
//     Revision 1.38  2000/09/05 19:55:57  gulliver
//     namespace std added on string vars
//
//     Revision 1.37  2000/08/21 17:50:57  mbickel
//      Fixed: crash when unit reaching max experience
//      Fixed: crash when displaying research image
//      Fixed: crash when events referenced a unit that has been shot down
//      Fixed: screenshot being written to wrong directory
//
//     Revision 1.36  2000/08/13 12:55:57  mbickel
//      Fixed IO errors with very small files
//
//     Revision 1.35  2000/08/12 15:03:18  mbickel
//      Fixed bug in unit movement
//      ASC compiles and runs under Linux again...
//
//     Revision 1.34  2000/08/12 09:17:15  gulliver
//     *** empty log message ***
//
//     Revision 1.33  2000/08/03 19:45:13  mbickel
//      Fixed some bugs in DOS code
//      Removed submarine.ascent != 0 hack
//
//     Revision 1.32  2000/08/03 19:21:15  mbickel
//      Fixed: units had invalid height when produced in some buildings
//      Fixed: units could not enter building if unitheightreq==0
//      Started adding SDL_image support
//      Upgraded to SDL1.1.3 (for SDL_image)
//
//     Revision 1.31  2000/08/02 15:52:38  mbickel
//      New unit set definition files
//      demount accepts now more than one container file
//      Unitset information dialog added
//
//     Revision 1.30  2000/08/02 10:28:23  mbickel
//      Fixed: generator vehicle not working
//      Streams can now report their name
//      Field information shows units filename
//
//     Revision 1.29  2000/08/01 10:39:08  mbickel
//      Updated documentation
//      Refined configuration file handling
//
//     Revision 1.28  2000/07/31 19:16:31  mbickel
//      Improved handing of multiple directories
//      Fixed: wind direction not displayed when cycling through wind heights
//      Fixed: oil rig not working
//      Fixed: resources becomming visible when checking mining station status
//      Fixed: division by zero when moving unit without fuel consumption
//
//     Revision 1.27  2000/07/31 18:02:52  mbickel
//      New configuration file handling
//      ASC searches its data files in all directories specified in ascrc
//      Renamed all tools so they begin with asc
//
//     Revision 1.26  2000/07/28 10:15:26  mbickel
//      Fixed broken movement
//      Fixed graphical artefacts when moving some airplanes
//
//     Revision 1.25  2000/06/28 18:30:57  mbickel
//      Started working on AI
//      Started making loaders independent of memory layout
//      Destroyed buildings can now leave objects behind.
//
//     Revision 1.24  2000/06/05 18:21:21  mbickel
//      Fixed a security hole which was opened with the new method of loading
//        mail games by command line parameters
//
//     Revision 1.23  2000/05/30 19:59:18  mbickel
//      Fixed bug in basestrm: container having higher priority than single file
//
//     Revision 1.22  2000/05/30 18:39:20  mbickel
//      Added support for multiple directories
//      Moved DOS specific files to a separate directory
//
//     Revision 1.21  2000/05/22 15:40:30  mbickel
//      Included patches for Win32 version
//
//     Revision 1.20  2000/05/06 19:57:08  mbickel
//      Mapeditor/linux is now running
//
//     Revision 1.19  2000/04/27 16:25:14  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.18  2000/03/29 09:58:41  mbickel
//      Improved memory handling for DOS version
//      Many small changes I can't remember ;-)
//
//     Revision 1.17  2000/02/05 12:13:44  steb
//     Sundry tidying up to get a clean compile and run.  Presently tending to SEGV on
//     startup due to actmap being null when trying to report errors.
//
//     Revision 1.16  2000/02/03 21:15:32  mbickel
//      Fixed a crash in the new file date routines
//
//     Revision 1.15  2000/02/03 20:54:38  mbickel
//      Some cleanup
//      getfiletime now works under Linux too
//
//     Revision 1.14  2000/01/25 19:28:06  mbickel
//      Fixed bugs:
//        invalid mouse buttons reported when moving the mouse
//        missing service weapon in weapon information
//        invalid text displayed in ammo production
//        undamaged units selected in repair vehicle function
//
//      speed up when playing weapon sound
//
//     Revision 1.13  2000/01/06 11:19:11  mbickel
//      Worked on the Linux-port again...
//
//     Revision 1.12  2000/01/01 19:04:13  mbickel
//     /tmp/cvsVhJ4Z3
//
//     Revision 1.11  1999/12/29 17:38:05  mbickel
//      Continued Linux port
//
//     Revision 1.10  1999/12/29 12:50:40  mbickel
//      Removed a fatal error message in GUI.CPP
//      Made some modifications to allow platform dependant path delimitters
//
//     Revision 1.9  1999/12/28 21:02:37  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.8  1999/12/27 12:59:38  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.7  1999/12/14 20:23:45  mbickel
//      getfiletime now works on containerfiles too
//      improved BI3 map import tables
//      various bugfixes
//
//     Revision 1.6  1999/11/25 21:59:57  mbickel
//      Added weapon information window
//      Added support for primary offscreen frame buffers to graphics engine
//      Restored file time handling for DOS version
//
//     Revision 1.5  1999/11/22 18:26:50  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.4  1999/11/18 17:30:59  mbickel
//      Improved BI-map import translation tables
//      Moved macros to substitute Watcom specific routines into global.h
//
//     Revision 1.3  1999/11/16 17:03:54  mbickel
//     Made ASC compilable for DOS again :-)
//     Merged all the bug fixes in that I did last week
//
//     Revision 1.2  1999/11/16 03:41:06  tmwilson
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

#include <stdio.h> 
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <string>

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






 const int maxSearchDirNum = 10;
 int searchDirNum = 0;
 char* ascDirectory[maxSearchDirNum] = { NULL, NULL, NULL, NULL, NULL, 
                                         NULL, NULL, NULL, NULL, NULL };




 int verbosity = 0;

#pragma pack(1)
struct trleheader {
   unsigned short int id;
   unsigned short int size;
   char rle;
   unsigned short int x;
   unsigned short int y;
};

struct trleheader32 {
    int id;
    int size;
    int rle;
    int x;
    int y;
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

#ifndef word
typedef unsigned short int word;
#endif

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

tfileerror::tfileerror ( const char* fn ) 
{
   strcpy ( filename , fn );
}

tfileerror :: tfileerror ( void )
{
   filename[0] = 0;
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

tinvalidmode :: tinvalidmode ( const char* fn, int org_mode, int requested_mode )
              : tfileerror ( fn )
{
   orgmode = org_mode;
   requestmode = requested_mode;
}



treadafterend :: treadafterend ( const char* fn )
               : tfileerror ( fn )
{

}


tinternalerror::tinternalerror (  const char* filename, int l )
{
   linenum = l;
   sourcefilename = filename;
}



tinvalidversion :: tinvalidversion ( const char* fn, int ex, int fnd ) 
                 : tfileerror ( fn )
{
   expected = ex;
   found = fnd;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////        Streams
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


tnstream :: tnstream ( void ) 
          : devicename ( "-abstract tnstream-" ) {}

void tnstream::seek ( int pos )
{
   throw tfileerror ( getDeviceName() );
}

void         tnstream::readrlepict( void** pnter, int allocated, int* size)
{ 
  trleheader   hd; 
  int          w;
  char*        q;

   readdata( &hd, sizeof( hd ));

   if (hd.id == 16973) { 
      if (!allocated) 
        *pnter = new char [ hd.size + sizeof(hd) ];
      memcpy( *pnter, &hd, sizeof(hd)); 
      q = (char*) (*pnter) + sizeof(hd);

      readdata( q, hd.size);
      *size = hd.size + sizeof(hd); 
   } 
   else { 
      w =  (hd.id + 1) * (hd.size + 1) + 4 ;
      if (!allocated) 
        *pnter = new char [ w ];
      memcpy ( *pnter, &hd, sizeof ( hd ));
      q = (char*) (*pnter) + sizeof(hd);
      readdata ( q, w - sizeof(hd) );
      *size = w; 
   } 
} 

void tnstream :: writerlepict ( const void* buf )
{
   char* tempbuf = new char [ 0xffff ];
   if ( tempbuf ){
      int   size    = compressrle ( buf, tempbuf );
      writedata ( tempbuf, size );
      delete[] tempbuf;
   } else {
      word* pw = (word*) buf;
      writedata ( buf, ( pw[0] + 1 ) * ( pw[1] + 1 ) + 4 );
   }
}


const char* tnstream::getDeviceName ( void )
{
   return devicename.c_str();
}


int  tnstream::readInt  ( void )
{
   int i;
   readdata2 ( i );
   return i;
}

word tnstream::readWord ( void )
{
   word w;
   readdata2 ( w );
   return w;
}

char tnstream::readChar ( void )
{
   char c;
   readdata2 ( c );
   return c;
}


void tnstream::writeInt  ( int i )
{
   writedata2 ( i );
}

void tnstream::writeWord ( word w )
{
   writedata2 ( w );
}

void tnstream::writeChar ( char c )
{
   writedata2 ( c );
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
   pch2--;

   do {
     actpos2++;
     pch2++;

     readdata( pch2, 1 );

   } while (*pch2 != 0   &&   actpos2 < maxav ); /* enddo */

   if ( actpos2 >= maxav ) {
      pch2[1] = 0;
      actpos2++;

      if ( pch2[0] ) {
         char temp;
         do {
            readdata( &temp, 1 );
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


int  tnstream::readTextString ( std::string& s )
{
  char c;
  int red = 1;
  int end = 0;
  do {
     red = readdata( &c, 1, 0 );
     if ( red < 1 ) {
        end = 2;
     } else
       if ( c == '\n' || c == 0 ) {
          end = 1;
       } else
          if ( c != '\r' )
             s += c;
  } while ( red && !end );
  if ( end == 2)
     return 0;
  else
     return 1;
}



void         tnstream::writepchar(const char* pc)
{ 
   if ( pc ) {
      const char *pch1 = pc;
    
      pch1--;
      do {
         pch1++;
         writedata( pch1, 1 );
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
    red = stream->readdata ( cp + bufused, blocksize, 0 );
    bufused += red;

  } while ( red == blocksize );
  size = bufused;
  pos = 0;

  devicename = stream->getDeviceName();
  devicename += " (memory bufferd)";
}


MemoryStreamCopy :: ~MemoryStreamCopy ( )
{
   if ( buf )
      free ( buf );
}


void MemoryStreamCopy :: writedata ( const void* buf, int size )
{
   throw  tinvalidmode ( getDeviceName(), 1, 2 );
}

int MemoryStreamCopy :: readdata  ( void* buffer, int _size, int excpt )
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
         if ( whence == SEEK_SET )
            stream->seek ( offset + stream->getSize() );
  return 0;
}


static int stream_read(SDL_RWops *context, void *ptr, int size, int maxnum)
{
	MemoryStreamCopy* stream = (MemoryStreamCopy*) context->hidden.unknown.data1;
	size_t nread = stream->readdata ( ptr, size * maxnum, 0 );

	/*
	if ( nread < 0 ) {
		SDL_SetError("Error reading from datastream");
	}
	*/
	return(nread / size);
}

static int stream_close(SDL_RWops *context)
{
	if ( context ) {
		if ( context->hidden.unknown.data1 ) {
			MemoryStreamCopy* stream = (MemoryStreamCopy*) context->hidden.unknown.data1;
			delete stream;
		}
		free(context);
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


tnbufstream::tnbufstream (  )
{ 
   datalen = 0;
   modus = 0; 

  zeiger = NULL;
  int maxav = 0x10000;

  do {
      memsize = maxav;
      zeiger = new char [ memsize ];

      maxav /= 0x10;
     
  } while ( !zeiger && maxav ); /* enddo */

  if ( !zeiger )
      throw  toutofmem( memsize );

   datasize = 0; 
   actmempos = 0;
} 


int          tnbufstream::readdata( void* buf, int size, int excpt  )
{ 
   char*        cpbuf = (char*) buf;
   int          s, actpos2;

   actpos2 = 0; 

   if (modus == 2)  {
      throw  tinvalidmode ( getDeviceName(), modus, 1 );
	}
      
   while (actpos2 < size) { 
      if (datasize == 0) 
          if ( excpt ) {
             throw treadafterend ( getDeviceName() );
			}
          else
             return actpos2;
       
      s = datasize - actmempos; 
      if (s > size - actpos2) 
         s = size - actpos2; 

      memcpy ( cpbuf + actpos2, zeiger + actmempos, s );

      actmempos += s; 
      if (actmempos >= datasize) { 
         readbuffer();
         actmempos = 0; 
      } 
      actpos2 = actpos2 + s; 
   } 

   return actpos2;
} 





void         tnbufstream::writedata( const void* buf, int size )
{ 
   datalen += size;
   int          s, actpos2;
   char*        cpbuf = (char*) buf;

   if (modus == 1) 
      throw  tinvalidmode ( getDeviceName(), modus, 2 );

   actpos2 = 0; 

   while (actpos2 < size) { 
      s = memsize - actmempos; 
      if (s > size - actpos2) 
         s = size - actpos2; 

      memcpy(  zeiger + actmempos, cpbuf + actpos2, s );
      actmempos = actmempos + s; 
      if (actmempos == memsize) { 
         writebuffer();
         actmempos = 0; 
      } 
      else 
         if (actmempos > memsize) 
            throw tinternalerror ( __FILE__, __LINE__ );
                                   
      actpos2 += s;
   } 
} 


tnbufstream::~tnbufstream ()
{ 
   if (modus == 2) 
      writebuffer();

   if ( memsize > 1)
      delete []  zeiger ;
} 




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int tn_file_buf_stream::getstreamsize(void)
{ 
   struct stat buf;
   if ( stat ( getDeviceName(), &buf) )
      return -1;
   else
      return (buf.st_size );
}                 

time_t tn_file_buf_stream::get_time ( void )
{
   struct stat buf;
   if ( stat ( getDeviceName(), &buf) )
      return -1;
   else
      return (buf.st_mtime);
}


tn_file_buf_stream::tn_file_buf_stream( const char* name, char mode)
{
   char buf[10000];
   std::string s;
   if ( strchr ( name, pathdelimitter ) == NULL )
      s = constructFileName ( buf, 0, NULL, name);
   else
      s = name;

   modus = mode; 
   
   if (mode == 1) {
      fp = fopen ( s.c_str(), filereadmode );
   } else {
      fp = fopen ( s.c_str(), filewritemode );
   }

   if (fp != NULL && ferror ( fp ) == 0 ) {

     actfilepos = 0;

     if (mode == 1)
       readbuffer();
                
     devicename = s.c_str();

   } else 
     throw tfileerror( s.c_str() );

} 


void tn_file_buf_stream::seek( int newpos )
{ 
   if ( modus == 2 ) {
      writebuffer();

      fseek( fp, newpos, SEEK_SET );
      if ( ferror ( fp ) )
         throw  tfileerror ( getDeviceName() );
   
      actmempos = 0; 
      actfilepos = newpos; 
   } else {
      if ( newpos >= actfilepos-datasize   &&  newpos < actfilepos )
         actmempos = newpos - ( actfilepos - datasize );
      else {
         fseek( fp, newpos, SEEK_SET );
         if ( ferror ( fp ) )
            throw  tfileerror ( getDeviceName() );
      
         actmempos = 0; 
         actfilepos = newpos; 
         readbuffer(); 

      }
   }
}           


void tn_file_buf_stream::readbuffer( void )
{ 
   datasize = fread( zeiger, 1, memsize, fp);
   if ( ferror ( fp ) ) 
      throw  tfileerror ( getDeviceName() );

   actfilepos += datasize;
} 




void tn_file_buf_stream::writebuffer()
{ 
   fwrite( zeiger, 1, actmempos, fp );
   if ( ferror ( fp ) )
      throw  tfileerror ( getDeviceName() );

   actmempos = 0;
} 


tn_file_buf_stream::~tn_file_buf_stream()
{ 
   close();

   if (modus == 2) 
      writebuffer(); 
  
   fclose( fp );

   modus = 0; 
} 



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




tncontainerstream :: tncontainerstream ( const char* containerfilename, ContainerIndexer* indexer, int dirLevel )
        : tn_file_buf_stream ( containerfilename, 1 )
{
   int pos;
   num = 0;
   char magic[4];
   readdata ( &magic, sizeof(pos) );
   if ( strncmp ( magic, containermagic, 4 ) == 0) {
      readdata ( &pos, sizeof(pos) );
      seek ( pos );
      readdata ( &num, sizeof (num) );
      index = new tcontainerindex[num];
      for ( int i = 0; i < num; i++ ) {
         readdata ( &index[i], sizeof ( index[i] ) );
         if ( index[i].name ) {
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
         }
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
   if ( actfile )
      throw tfileerror ( name );

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

int tncontainerstream :: readcontainerdata ( void* buf, int size, int excpt  )
{
   if ( actfile->start + containerfilepos + size > actfile->end+1 ) {
      if ( excpt ) 
         throw treadafterend ( actfile->name );
      else {
         return readdata ( buf, (actfile->end+1 - actfile->start) - containerfilepos  , excpt );

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
           if ( directoryLevel >= 0 )
              strcpy ( buf, ascDirectory[ directoryLevel ]);
     }

     appendbackslash ( buf );

     char name2[ maxFileStringSize ];
     if ( filename && strchr ( filename, pathdelimitter )) {
        strcpy ( name2, filename );
        int i = strlen ( name2 )-1;
        while ( name2[i] != pathdelimitter )
           i--;

        name2[i+1] = 0;

        filename2 = &filename[i+1];

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

struct FileLocation {
        int directoryLevel;
        pncontainerstream container;
        int found;
     };

void locateFile ( const char* filename, FileLocation* loc )
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
         FILE* fp = fopen ( constructFileName ( buf, i, NULL, filename), "r" );
         if ( fp ) {
            localfound = loc->found = 1;
            fclose ( fp );
            loc->container = NULL;
            loc->directoryLevel = i;
         }
      }
   } else {
      char buf[2000];
      FILE* fp = fopen ( constructFileName ( buf, -1, ".", filename), "r" );
      if ( fp ) {
         loc->found = 1;
         fclose ( fp );
         loc->container = NULL;
         loc->directoryLevel = -2;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



ContainerCollector :: ContainerCollector ( void )
{
  containernum = 0;
}

void ContainerCollector :: init ( const char* wildcard )
{
   for ( int i = 0; i < searchDirNum; i++ ) {
      DIR *dirp; 
      struct direct *direntp; 

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
               if ( verbosity >= 2 )
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
      if ( stricmp ( index[i1][i].name, filename ) == 0 ) 
         if ( index[i1][i].directoryLevel <= directoryLevel ) 
            return;
         else {
            cci = &(index[i1][i]);
            free ( cci->name );
            found = 1;
         }

   if ( !found )
      cci = &( index[i1][ index[i1].getlength()+1 ] );

   cci->name = strdup ( filename );
   cci->container = stream;
   cci->directoryLevel = directoryLevel;
}

ContainerCollector::FileIndex* ContainerCollector :: getfile ( const char* filename )
{
   int i1 = toupper ( filename[0] );
   for ( int i = 0; i <= index[i1].getlength(); i++ )
      if ( stricmp ( index[i1][i].name, filename ) == 0 )
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


ContainerCollector :: ~ContainerCollector()
{
  int i;
   for (i = 0; i < containernum; i++ )
      delete container[i];
   for ( int j = 0; j < 255; j++ )
      for ( int k = 0; k <= index[i].getlength(); k++ )
         free ( index[i][k].name );
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


int libbzip_decompression :: readdata ( void* buf, int size, int excpt )
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


t_compressor_2ndbuf_filter :: t_compressor_2ndbuf_filter ( t_compressor_stream_interface* strm )
{
   stream = strm;
   queuesize = 0;
}

void t_compressor_2ndbuf_filter :: writecmpdata ( const void* buf, int size )
{
   stream->writecmpdata ( buf, size );
}

int t_compressor_2ndbuf_filter :: readcmpdata ( void* buf, int size, int excpt )
{
   int got = 0;

   char* pc = (char*) buf;

   while ( size && queuesize) {
       *pc = queue.getval();
       pc++;
       queuesize--;
       size--;
       got++;
   } /* endwhile */

   if ( size )
      got += stream->readcmpdata ( pc, size, excpt );

   return got;   
}

void t_compressor_2ndbuf_filter :: insert_data_into_queue ( const void* buf, int size )
{
   char* pc = (char*) buf;
   for (int i = 0; i < size; i++) {
      queue.putval ( *pc );
      *pc++;
      queuesize++;
   } /* endfor */
}




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

         if ( strcmp ( &buf[1], LZ_SIGNATURE ) == 0  && !buf[0]) {
            status = 110;
            siglen = 0;
         } else
         if ( strcmp ( &buf[1], RLE_SIGNATURE ) == 0 && !buf[0]) {
            status = 111;
            siglen = 0;
         } else
         if ( strcmp ( buf, BZIP_SIGNATURE ) == 0 ) {
            status = 112;
            siglen = strlen ( BZIP_SIGNATURE ) + 1;
            bzip_decompress = new libbzip_decompression ( this );
         } else
            status= 109;

      } else
         status = 109;

      for ( int i = siglen; i < bufdatanum; i++ )
          queue.putval ( buf[i] );


   } else {
       status = 201;
       bzip_compress = new libbzip_compression ( this );
       writecmpdata ( BZIP_SIGNATURE, strlen ( BZIP_SIGNATURE ) + 1 );
   }
}


int  tanycompression :: readdata ( void* rbuf, int size, int excpt )
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


int tanycompression :: readlzwdata ( void* buf, int size, int excpt )
{
   if ( queue.valavail() ) {
      int got = 0;
   
      char* pc = (char*) buf;
   
      while ( size && queue.valavail() ) {
          *pc = queue.getval();
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

int tn_lzw_bufstream :: readdata  ( void* buf, int size, int excpt  )
{
   return tlzwstreamcompression :: readdata ( buf, size, excpt );
}

int  tn_lzw_bufstream :: readlzwdata ( void* buf, int size, int excpt  )
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

int  tn_lzw_file_buf_stream :: readdata  ( void* buf, int size, int excpt  )
{
   return tanycompression :: readdata ( buf, size, excpt );
}

int  tn_lzw_file_buf_stream:: readcmpdata ( void* buf, int size, int excpt  )
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


tn_c_lzw_filestream :: tn_c_lzw_filestream ( const char* name, char mode ) : tanycompression ( mode )
{
   #ifdef logfiles
    FILE* fp = fopen ( "files.lst", "at" );
    fprintf ( fp, "%s\n", name );
    fclose ( fp );
   #endif

   strm = NULL;
   fname = 0;
   inp = 0;
   containerstream = NULL;

   FileLocation fl;
   if ( mode == 1 ) {
      locateFile ( name, &fl ); 

      if ( !fl.found )
         throw tfileerror ( name );
            
   } else {
      fl.directoryLevel = 0;
      fl.container = NULL;
   }
     

   if ( fl.container == NULL ) {
      char string[2000];

      strm = new tn_file_buf_stream ( constructFileName ( string, fl.directoryLevel, NULL, name), mode );
      inp = 1;
      devicename = name;

   } else {
      containerstream = fl.container;
      if ( containerstream ) {
         containerstream->opencontainerfile ( name );

         devicename = name;
         devicename += " located inside ";
         devicename += containerstream->getDeviceName() ;

         inp = 2;
      } else
         throw tfileerror ( name );
   }
   fname = strdup ( name );

   tanycompression :: init (  ); 
}


void tn_c_lzw_filestream :: writecmpdata ( const void* buf, int size )
{
   if ( inp == 2 )
      throw tinvalidmode ( fname, 1, 2 );
   else
      strm->writedata ( buf, size );
}

int tn_c_lzw_filestream :: readcmpdata  ( void* buf, int size, int excpt  )
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

int tn_c_lzw_filestream :: readdata  ( void* buf, int size, int excpt  )
{
   if ( tanycompression :: mode == readingdirect  && !queuestat )
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

   if ( fname ) {
      delete[] fname;
      fname = NULL;
   }
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

   word x,y;
   int size;
   {
      word* pw = (word*) s;
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
      int i;
      memset ( bts, 0, sizeof ( bts ));
      for (i = 0; i < size ;i++ ) 
        bts[s[i+4]]++;

      int min = 70000;
      for ( i = 0; i < 256; i++ )
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
   
            while ( s[num] == actbyte  &&  xp+num < x   &&   num < 255) 
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





int patimat (const char *pat, const char *str)
{
      switch (*pat)
      {
      case '\0':
            return !*str;

      case '*' :
            return patimat(pat+1, str) || *str && patimat(pat, str+1);

      case '?' :
            return *str && patimat(pat+1, str+1);

      default  :
#if CASE_SENSITIVE_FILE_NAMES == 0 
            return (toupper(*pat) == toupper(*str)) && patimat(pat+1, str+1);
#else
            return (*pat == *str) && patimat(pat+1, str+1);
#endif
      }
}



tfindfile :: tfindfile ( const char* name )
{
   found = 0;
   act = 0;
   if ( !name )
      return;

   char* directory[maxSearchDirNum];
   int dirNum;
   char wildcard[ maxFileStringSize ];

   if ( strchr ( name, pathdelimitter )) {
      char name2[ maxFileStringSize ];
      strcpy ( name2, name );
      int i = strlen ( name2 )-1;
      while ( name2[i] != pathdelimitter )
         i--;
      name2[i+1] = 0;
      directory[0] = name2;
      dirNum = 1;

      strcpy ( wildcard, &name[i+1] );

   } else {
      if ( searchDirNum ) {
         for (int i = 0; i < searchDirNum; i++ )
            directory[i] = ascDirectory[i];
         dirNum = searchDirNum;
      } else {
         directory[0] = ".";
         dirNum = 1;
      }
      strcpy ( wildcard, name );
   }


   for ( int i = 0; i < dirNum; i++ ) {
      DIR *dirp; 
      struct direct *direntp; 
  
      dirp = opendir( directory[i] );   
      if( dirp != NULL ) { 
        for(;;) { 
          direntp = readdir( dirp ); 
          if ( direntp == NULL ) 
             break; 
             
          if ( patimat ( wildcard, direntp->d_name )) {
             int localfound = 0;
             for ( int j = 0; j < found; j++ )
                if ( strcmpi ( names[j], direntp->d_name ) == 0 )
                   localfound++;

             if ( !localfound ) {
                names[found] = strdup ( direntp->d_name );
                namedupes[found] = 1;
                directoryLevel[found] = i;
                isInContainer[found] = 0;
                found++;
             }
          }
        } 
        closedir( dirp ); 
      } 
   }



   {
      const ContainerCollector::FileIndex* c = containercollector.getfirstname();
      while ( c ) {
          if ( patimat ( name, c->name ) ) {
             int f = 0;
             for ( int i = 0; i < found; i++ )
                if ( stricmp ( c->name, names[i] ) == 0 ) {
                   if ( directoryLevel[i] <= c->directoryLevel ) 
                      f = 1;
                   else {
                      if ( namedupes[i] )
                         delete[] names[i];
                      names[i] = strdup ( c->name );
                      namedupes[i] = 1;
                      isInContainer[i] = 1;
                      directoryLevel[i] = c->directoryLevel;
                      f = 1;
                   }
                }
                
             if ( !f ) {
                names[found] = c->name;
                namedupes[found] = 0;
                directoryLevel[found] = c->directoryLevel;
                isInContainer[found] = 1;
                found++;
             }
          }
          c = containercollector.getnextname();
      }
   }
}


char* tfindfile :: getnextname ( int* loc, int* inContainer )
{
   if ( act < found ) {
      if ( loc )
         *loc = directoryLevel[act];

      if ( inContainer )
         *inContainer = isInContainer[act];

      return names[act++];
   } else {
      if ( loc )
         *loc = -1;
      return NULL;
   }
}


tfindfile :: ~tfindfile()
{
   for ( int i = 0; i < found; i++ ) 
      if ( namedupes[i] ) {
         char* c = names[i];
         if ( c )
            delete[] c;
      }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// tncontainerstream* containerstream = NULL;



int checkforvaliddirectory ( char* dir )
{
   int stat = 0;

      DIR *dirp; 
      struct direct *direntp; 
  
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
     mode = 0;
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
        stream->writedata2 ( mode );
        stream->writedata2 ( used );
        stream->writedata2 ( allocated );
        stream->writedata2 ( dummy );
        if ( used > 0 )
           stream->writedata ( buf, used );
     }
  }

  void tmemorystreambuf :: readfromstream ( pnstream stream )
  {
     if ( stream ) {
        stream->readdata2 ( mode );
        stream->readdata2 ( used );
        stream->readdata2 ( allocated );
        stream->readdata2 ( dummy );
        if ( buf ) {
           delete[] buf;
           buf = NULL;
        }
        if ( used > 0 || allocated > 0 ) {
           buf = new char[allocated];
           stream->readdata ( buf, used );
        }
     }
  }


tmemorystream :: tmemorystream ( pmemorystreambuf lbuf, int lmode )
{
   blocksize = 1024;
   buf = lbuf;
   mode = lmode;

   if ( !buf )
      throw tfileerror ( "memorystream" );


   if ( mode == 1 ) {
      zeiger = buf->buf;
      actmempos = 0;
   } else
   if ( mode == 2 ) {     // neuen Puffer anlegen
      if ( buf->buf ) {
         delete[] buf->buf;
         buf->buf = NULL;
      }
      buf->buf = new char[blocksize];
      buf->allocated = blocksize;
      buf->used = 0;
      zeiger = buf->buf;
      actmempos = 0;
   } else
   if ( mode == 22 ) {
      zeiger = buf->buf;
      actmempos = buf->used;
      mode = 2;
   }
}

void tmemorystream :: writedata ( const void* nbuf, int size )
{
   if ( mode != 2 )
      throw tinvalidmode ( "memorystream", mode, 2 );

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


int  tmemorystream :: readdata ( void* nbuf, int size, int excpt  )
{
   if (mode == 2) 
      throw  tinvalidmode ( "memorystream", mode, 1 );
      
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
   if ( mode == 2 )
      return 1;
   else
      return actmempos < buf->used;
}


char tempstringbuf[ maxFileStringSize ];

char* getnextfilenumname ( const char* first, const char* suffix, int num )
{

   int found = 0;

   if ( num < 0 )
     num = 0;

   // int sl = strlen ( first );
   char tmp[260];
   do {
      strcpy ( tmp, first );
      std::itoa ( num, tempstringbuf, 10 );
      while ( strlen ( tmp ) + strlen ( tempstringbuf ) < 8 )
         strcat ( tmp, "0" );
      strcat ( tmp, tempstringbuf );
      strcat ( tmp, "." );
      strcat ( tmp, suffix );

      tfindfile ff ( tmp );
      char *c = ff.getnextname();
      if ( !c ) {
         strcpy ( tempstringbuf, tmp );
         found = 1;
      }
      num++;
   } while ( found == 0 ); /* enddo */

   return tempstringbuf;
}


int exist ( const char* s )
{
   tfindfile ff ( s );
   return ff.getnextname() != NULL;
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




time_t get_filetime ( char* fileName )
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


int filesize( char *name)
{
  struct stat buf;

  if ( !stat (name, &buf))
     return (buf.st_size);
  else
     return -1;
}


int directoryExist ( const char* path )
{
   int existence = 0;

   DIR *dirp = opendir( path );
   if( dirp ) {
      if ( readdir( dirp ) )
         existence = 1;
      else
         existence = 0;

      closedir( dirp );
   }
   return existence;
}

void addSearchPath ( const char* path )
{
   if ( path ) {
      char buf[ maxFileStringSize ];
      char* string = new char[ strlen(path) +  maxFileStringSize ];
      strcpy ( string, constructFileName ( buf, -3, path, NULL ) );

      if ( directoryExist( buf )) {
         int found = 0;
         for ( int i = 0; i < searchDirNum; i++ )
            if ( strcmp ( string, ascDirectory[i]) == 0 )
               found++;
   
         if ( !found ) 
            ascDirectory[ searchDirNum++ ] = string;
         else
            delete[] string;
      }
   }
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


void appendbackslash ( char* string )
{
   if ( strlen ( string ) && string[strlen ( string ) -1] != pathdelimitter )
      strcat ( string, pathdelimitterstring );
}
