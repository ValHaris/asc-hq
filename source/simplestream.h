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

#ifndef simplestreamH
#define simplestreamH

#include <time.h>
#include "ascstring.h"
#include "errors.h"
#include "basestreaminterface.h"

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
           virtual int  readdata  ( void* buf, int size, bool excpt = true  );

           virtual ~tnbufstream ( );

      };

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
            virtual ~tn_file_buf_stream( );
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

class tinternalerror: public ASCexception {
   int linenum;
   const char* sourcefilename;
  public:
   tinternalerror ( const char* filename, int l );
};


#endif
