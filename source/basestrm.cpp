//     $Id: basestrm.cpp,v 1.25 2000-06-28 18:30:57 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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

#include "config.h"
#include <stdio.h> 
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

#include "basestrm.h"

#ifdef _DOS_
#include <direct.h> 
#else

#ifdef HAVE_SYS_DIRENT_H
#include <sys/dirent.h>
#endif

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#endif





#if defined(_DOS_) | defined(WIN32)
 const char* filereadmode = "rb";
 const char* filewritemode = "wb";
 const char pathdelimitter = '\\';
 const char* pathdelimitterstring = "\\";
#else
 const char* filereadmode = "r";
 const char* filewritemode = "w";
 const char pathdelimitter = '/';
 const char* pathdelimitterstring = "/";
#endif

 const int maxSearchDirNum = 10;
 int searchDirNum = 0;
 char* ascDirectory[maxSearchDirNum] = { NULL, NULL, NULL, NULL, NULL, 
                                         NULL, NULL, NULL, NULL, NULL };


#ifdef _DOS_
 int verbosity = 0;
#else
 int verbosity = 0;
#endif

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

const int containermagic = 'MBCN';

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



tcompressionerror :: tcompressionerror ( const char* msg, int returncode )
{
   strcpy ( strng, msg );
   code = returncode;
};



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


int  tnstream::readint  ( void )
{
   int i;
   readdata2 ( i );
   return i;
}

word tnstream::readword ( void )
{
   word w;
   readdata2 ( w );
   return w;
}

char tnstream::readchar ( void )
{
   char c;
   readdata2 ( c );
   return c;
}


void tnstream::writeint  ( int i )
{
   writedata2 ( i );
}

void tnstream::writeword ( word w )
{
   writedata2 ( w );
}

void tnstream::writechar ( char c )
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
/*

class StreamReadBuffer : public tnstream {
               pnstream stream;
               char  minbuf;
               int datalen;

               char* zeiger;
               int   actmempos;
               int   memsize;
               int   datasize;
            public:
               StreamReadBuffer ( pnstream s );
               int readdata( void* buf, int size, int excpt  );
          };




StreamReadBuffer::StreamReadBuffer ( pnstream s )
{
   stream = s;

   datalen = 0;

   memsize = 0x10000;
   zeiger = new char [ memsize ];

   datasize = 0;
   actmempos = 0;
}



int          StreamReadBuffer::readdata( void* buf, int size, int excpt  )
{
  char*        cpbuf = (char*) buf;
  int          s, actpos2;

   actpos2 = 0;

   while (actpos2 < size) {
      if (datasize == 0)
          if ( excpt )
             throw treadafterend ( devicename );
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

*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





tnbufstream::tnbufstream (  )
{ 
   datalen = 0;
   modus = 0; 

   strcpy ( devicename , "abstract" ); 

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
      throw  tinvalidmode ( devicename, modus, 1 );
	}
      
   while (actpos2 < size) { 
      if (datasize == 0) 
          if ( excpt ) {
             throw treadafterend ( devicename );
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
      throw  tinvalidmode ( devicename, modus, 2 );

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
          
   int size = -1;
   {
      DIR *dirp; 
      struct dirent *direntp; 
  
      dirp = opendir( devicename );  
      if( dirp != NULL ) { 
        for(;;) { 
          direntp = readdir( dirp ); 
          if ( direntp == NULL ) 
             break; 
#ifdef NAMLEN
          size = NAMLEN(direntp);
#else
	  size = direntp->d_size;
#endif
        } 
        closedir( dirp ); 
      } 
    }
   return size;

}                 

time_t tn_file_buf_stream::get_time ( void )
{
   struct stat buf;
   if ( stat (devicename, &buf) )
      return -1;
   else
      return (buf.st_mtime);
}


tn_file_buf_stream::tn_file_buf_stream( const char* name, char mode)
{ 

   modus = mode; 
   
   if (mode == 1) {
      fp = fopen ( name, filereadmode );
   } else {
      fp = fopen ( name, filewritemode );
   }

   if (fp != NULL && ferror ( fp ) == 0 ) {

     actfilepos = 0;

     if (mode == 1)
       readbuffer();
                
     strcpy ( devicename , name );

   } else 
     throw tfileerror( name );

} 


void tn_file_buf_stream::seekstream( int newpos )
{ 
   if ( modus == 2 ) {
      writebuffer();

      fseek( fp, newpos, SEEK_SET );
      if ( ferror ( fp ) )
         throw  tfileerror ( devicename );
   
      actmempos = 0; 
      actfilepos = newpos; 
   } else {
      if ( newpos >= actfilepos-datasize   &&  newpos < actfilepos )
         actmempos = newpos - ( actfilepos - datasize );
      else {
         fseek( fp, newpos, SEEK_SET );
         if ( ferror ( fp ) )
            throw  tfileerror ( devicename );
      
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
      throw  tfileerror ( devicename );

   actfilepos += datasize;
} 




void tn_file_buf_stream::writebuffer()
{ 
   fwrite( zeiger, 1, actmempos, fp );
   if ( ferror ( fp ) )
      throw  tfileerror ( devicename );

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
   int magic;
   readdata ( &magic, sizeof(pos) );
   if ( magic == containermagic ) {
      readdata ( &pos, sizeof(pos) );
      seekstream ( pos );
      readdata ( &num, sizeof (num) );
      index = new tcontainerindex[num];
      for ( int i = 0; i < num; i++ ) {
         readdata ( &index[i], sizeof ( index[i] ) );
         if ( index[i].name ) {
            readpchar ( &index[i].name );

           #if !(defined ( _DOS_ ) | defined ( WIN32 ))
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
   seekstream ( actfile->start );
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

     // this system could be extended to allow a relative path

     if ( path ) 
        strcpy ( buf, path);
     else
        strcpy ( buf, ascDirectory[ directoryLevel ]);

     if ( strlen ( buf ) && buf[strlen ( buf ) -1] != pathdelimitter )
        strcat ( buf, pathdelimitterstring );

     strcat ( buf, filename );
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
      struct dirent *direntp; 

      dirp = opendir( ascDirectory[i] );
      if( dirp != NULL ) { 
         for(;;) { 
            direntp = readdir( dirp ); 
            if ( direntp == NULL ) {
               break; 
            }
            if ( patimat ( wildcard, direntp->d_name )) {
               char buf[2000];
               container[containernum++] = new tncontainerstream( constructFileName ( buf, i, NULL, direntp->d_name), this, i);
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

   bzCompressInit ( &bzs, 5, 0, 0 );

   outputbufsize = 100000;
   outputbuf = new char [ outputbufsize ];

   stream = strm;
}


void libbzip_compression :: writedata ( const void* buf, int size )
{
   char* cbuf = (char*) buf;

   bzs.next_in = cbuf ;
   bzs.avail_in = size ;
   bzs.total_in = 0 ;

   while ( bzs.total_in < size ) {

     bzs.next_out = outputbuf;
     bzs.avail_out = outputbufsize;
     bzs.total_out = 0;

     int res = bzCompress ( &bzs, BZ_RUN );
     if ( res < 0 )
        throw tcompressionerror ( "MBZLB2 compression :: writedata", res );

     for ( int i = 0; i < bzs.total_out; i++ )
        outputbuf[i] ^= bzip_xor_byte;

     stream->writecmpdata ( outputbuf, bzs.total_out );
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
     bzs.total_out = 0;

     res = bzCompress ( &bzs, BZ_FINISH );
     if ( res < 0 )
        throw tcompressionerror ( "MBZLB2 compression :: closecompression", res );

     for ( int i = 0; i < bzs.total_out; i++ )
        outputbuf[i] ^= bzip_xor_byte;
     stream->writecmpdata ( outputbuf, bzs.total_out );

   } while ( res != BZ_STREAM_END ); 


   bzCompressEnd ( &bzs );
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

   bzDecompressInit ( &bzs, 0, 0 );

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
   bzs.total_in = 0 ;

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
     bzs.total_in = 0;

     bzs.next_out = cbuf + decompressed;
     bzs.avail_out = size - decompressed;
     bzs.total_out = 0;

     int res = bzDecompress ( &bzs );
     decompressed += bzs.total_out;
     inputbufread += bzs.total_in;

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
      bzDecompressEnd ( &bzs );
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

         for ( int i = siglen; i < bufdatanum; i++ )
            queue.putval ( buf[i] );
      } else 
         status = 108;

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
   } else {
      containerstream = fl.container;
      if ( containerstream ) {
         containerstream->opencontainerfile ( name );
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
#if defined ( _DOS_ ) | defined ( WIN32 )   // DOS filenames are not case sensitive
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
   char wildcard[1000];

   if ( strchr ( name, pathdelimitter )) {
      char name2[1000];
      strcpy ( name2, name );
      int i = strlen ( name2 )-1;
      while ( name2[i] != pathdelimitter )
         i--;
      name2[i+1] = 0;
      directory[0] = name2;
      dirNum = 1;

      strcpy ( wildcard, &name[i+1] );

   } else {
      for (int i = 0; i < searchDirNum; i++ ) 
         directory[i] = ascDirectory[i];
      dirNum = searchDirNum;
      strcpy ( wildcard, name );
   }


   for ( int i = 0; i < dirNum; i++ ) {
      DIR *dirp; 
      struct dirent *direntp; 
  
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
      struct dirent *direntp; 
  
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


char tempstringbuf[260];

char* getnextfilenumname ( const char* first, const char* suffix, int num )
{
   int found = 0;

   if ( num < 0 )
     num = 0;

   // int sl = strlen ( first );
   char tmp[260];
   do {
      strcpy ( tmp, first );
      itoa ( num, tempstringbuf, 10 );
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
      char buf[2000];

     if ( fl.container )
        return fl.container->get_time();
     else {
        struct stat stbuf;
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


void addSearchPath ( const char* path )
{
   char* string = new char[ strlen(path) + 10 ];
   strcpy ( string, path );
   if ( strlen ( string ) && string[strlen ( string ) -1] != pathdelimitter )
      strcat ( string, pathdelimitterstring );

   ascDirectory[ searchDirNum ] = string;
   searchDirNum++;
}


