/***************************************************************************
                          basestreaminterface.h  -  description
                             -------------------
    begin                : Sat Oct 14 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef basestreaminterface_h_included
 #define basestreaminterface_h_included

 #include "global.h"
 #include "tpascal.inc"


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

#endif // basestreaminterface_h_included