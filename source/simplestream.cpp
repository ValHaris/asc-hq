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

#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "simplestream.h"
#include "errors.h"

/* simplestream was intended to provide streams without any dependency on
   basestrm, but unknotting the two was never completed */
#include "basestrm.h"

#ifdef _DOS_
  #include "dos/fileio.h"
  #include "dos/fileio.cpp"
#else
 #ifdef _WIN32_
   #include "win32/fileio.h"
   #include "win32/fileio.cpp"
 #else
  #ifdef _UNIX_
    #include "unix/fileio.h"
    #include "unix/fileio.cpp"
  #endif
 #endif
#endif


tnbufstream::tnbufstream (  )
{
   datalen = 0;
   _mode = uninitialized;

  zeiger = NULL;
  int maxav = 0x10000;

  do {
      memsize = maxav;
      zeiger = new char [ memsize ];

      maxav /= 0x10;
     
  } while ( !zeiger && maxav ); /* enddo */

   datasize = 0;
   actmempos = 0;
}


int          tnbufstream::readdata( void* buf, int size, bool excpt  )
{
   char*        cpbuf = (char*) buf;
   int          s, actpos2;

   actpos2 = 0;

   if (_mode != reading)
      throw  tinvalidmode ( getDeviceName(), _mode, reading );
   

   while (actpos2 < size) {
      if (datasize == 0) {
          if ( excpt ) 
             throw treadafterend ( getDeviceName() );
          else
             return actpos2;
      }
       
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

   if (_mode != writing )
      throw  tinvalidmode ( getDeviceName(), _mode, writing );

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
   if ( memsize > 1)
      delete []  zeiger ;
} 




int tn_file_buf_stream::getstreamsize(void)
{
   if ( !sizeCached ) {
      struct stat buf;
      if ( stat ( getDeviceName().c_str(), &buf) )
         sizeValue = -1;
      else
         sizeValue = buf.st_size ;

      sizeCached = true;
   }
   return sizeValue;
}

time_t tn_file_buf_stream::get_time ( void )
{
   if ( !timeCached ) {
      struct stat buf;
      if ( stat ( getDeviceName().c_str(), &buf) )
         timeValue = -1;
      else
         timeValue = buf.st_mtime;

      timeCached = true;

   }
   return timeValue;
}


tn_file_buf_stream::tn_file_buf_stream( const ASCString& _fileName, IOMode mode)
   : sizeCached ( false ), sizeValue ( -1 ), timeCached( false), timeValue( -1 )

{
   char buf[10000];
   ASCString s;
   if ( strchr ( _fileName.c_str(), pathdelimitter ) == NULL )
      s = constructFileName ( buf, 0, NULL, _fileName.c_str() );
   else
      s = _fileName;

   _mode = mode;

   if (_mode == reading) {
      fp = fopen ( s.c_str(), filereadmode );
   } else {
      fp = fopen ( s.c_str(), filewritemode );
   }

   if (fp != NULL && ferror ( fp ) == 0 ) {

     actfilepos = 0;

     if (_mode == reading)
       readbuffer();

     devicename = s;

   } else {
      throw tfileerror( s + " : " + strerror(errno) );
   }

}


void tn_file_buf_stream::seek( int newpos )
{ 
   if ( _mode == writing ) {
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
            throw  tfileerror ( getDeviceName() + " : " + strerror(errno) );
      
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
   size_t written = fwrite( zeiger, 1, actmempos, fp );
   if ( ferror ( fp ) || actmempos != written )
      throw  tfileerror ( getDeviceName() );

   actmempos = 0;
}


tn_file_buf_stream::~tn_file_buf_stream()
{
   close();

   if (_mode == writing)
      writebuffer();

   int res = fclose( fp );
   if ( res != 0 ) 
      throw  tfileerror ( getDeviceName() + " : " + strerror(errno));
      
   _mode = uninitialized;

}


