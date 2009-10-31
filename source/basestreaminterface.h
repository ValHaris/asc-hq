/***************************************************************************
                          basestreaminterface.h  -  description
                             -------------------
    begin                : Sat Oct 14 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file basestreaminterface.h
    \brief The standard interface for ASC streams.
    This is the parent class for all kinds of streams:
    normal files, compressed files, files inside containers, memory buffer, ... 
*/

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

#include <bitset>
 #include "global.h"

 #include "ascstring.h"

 typedef class tnstream* pnstream;

 /** The interface for all kinds of IO stream. Because ASC was originally 
     written in Pascal, the streams don't have any relation to the C++ streams */
 class tnstream {
        public:
           tnstream ( );

           //! the modes in which a stream can operate
           enum IOMode { uninitialized, reading, writing, appending };
 
           /** writes data to the stream
               \param buf pointer to the buffer
               \param size the number of bytes which are going to be written
            */   
           virtual void writedata ( const void* buf, int size ) = 0 ;

           /** Reads data from the stream.  
                \param buf the buffer which the data is written to
                \param size the number of bytes which should be read 
                \param excpt If true the operation will throw an exception if the desired number of bytes
                       cannot be read. 
                \returns the number of bytes which were read.
           */
           virtual int  readdata  ( void* buf, int size, bool excpt = true ) = 0 ;

           /** Reads a string. All bytes of the files are stored in the allocated memory until either a 0 Byte
               is read or the maximum number of character have been reached. The string will always be 0 terminated 
               in both cases. 

               \param pc pointer to an uninitialized pointer which will contain the string. 
                         The memory will be allocated using new char[length of string] . 
               \param maxlength The maximum number of bytes that will be read. If zero, the maximum length 
                         is not limited.
           */
           virtual void readpchar( char** pc, int maxlength = 0) ;

           /** Reads a string. All bytes of the files are stored in the allocated memory until either a 0 Byte
               or a newline character is read or the maximum number of character have been reached. 
               The string will always be 0 terminated. Carriage return bytes will be filtered out.
                
               \param pc pointer to an uninitialized pointer which will contain the string. 
                         The memory will be allocated using new char[length of string] . 
               \param maxlength The maximum number of bytes that will be read. If zero, the maximum length 
                         is not limited.
           */
           virtual void readpnchar( char** pc, int maxlength = 0) ;

           /** Reads a string. Carriage Return character will be filtered.
               \param s The ASCString which will contain the data read.
               \param includeCR if true, the reading will stop only a 0 Bytes or the end of stream, 
                                so all newline character will be part of the text string. Useful for binary files.
                                If false reading will stop and 0 Bytes and newline characters. The newline character will not be part of the resulting string.
               \returns true if there is remaining data after the string, false if the reading was ended because the end of the stream was reached.
           */
           virtual bool readTextString ( ASCString& s, bool includeCR = false ); 

           /** Reads and returns a string. Carriage Return character will be filtered. If the very first read
               operation fails because the end of the stream was already reached, a readafterend exception will 
               be thrown. If some data could be read before the end was reached, this data will be returned.
               \param includeCR if true, the reading will stop only at 0 Bytes or the end of stream, 
                                so all newline character will be part of the text string. Useful for binary files.
                                If false reading will stop at 0 Bytes and newline characters. The newline character will not be part of the resulting string.
           */
           virtual ASCString readString ( bool includeCR = false );

           //! write the C-String pc to the stream
           virtual void writepchar( const char* pc) ;

           /** writes the C++ String pc to the stream. 
               \param binary If true, all bytes (including 0 bytes) of the string will be written to the stream. 
                             Note that a string containing one or more 0 bytes cannot be read with a single stream reading operation.
                             If false, writing will stop at the first 0 character of the string (or its end of course)
           */
           virtual void writeString( const string& pc, bool binary = true ) ;

           //! Reads a 32 bit signed Integer. In the stream little-endian byte order is used and a translation is performed, if necessary.
           virtual int  readInt  ( void );

           //! Reads a 16 bit unsigned Integer. In the stream little-endian byte order is used and a translation is performed, if necessary.
           virtual int readWord ( void );

           //! Reads a 8 bit unsigned Integer. 
           virtual char readChar ( void );

           template<size_t N>
           void writeBitset( const std::bitset<N>& bs ) {
              writeInt(1);
              writeInt(N);
              for( int i = 0; i < N;++i)
                 writeChar( bs.test(i));
           }

           template<size_t N>
           void readBitset( std::bitset<N>& bs ) {
              readInt(); // version
              int n = readInt();
              assert( n == N );
              bs.reset();
              for( int i = 0; i < N;++i)
                 if ( readChar() )
                  bs.set( i );
           }
           
           
           //! Reads a flaot variable.
           virtual float readFloat ( void );

           //! Writes a 32 bit signed Integer. In the stream little-endian byte order is used and a translation is performed, if necessary.
           virtual void writeInt  ( int  i );
           virtual void writeInt  ( unsigned int  i );
           void writeInt  ( bool b );

           #ifdef SIZE_T_not_identical_to_INT
           //! Writes a size_t as a 32 bit signed Integer. If the value is larger, an error is issued.
           void writeInt  ( size_t i );
           #endif


           //! Writes a 16 bit unsigned Integer. In the stream little-endian byte order is used and a translation is performed, if necessary.
           virtual void writeWord ( int w );

           //! Writes a 8 bit unsigned Integer. 
           virtual void writeChar ( char c );

           //! Write a floating point variable
           virtual void writeFloat ( float f );

           //! Writes an image to the stream and compresses it using RLE
           virtual void writerlepict ( const void* buf );

           //! Writes an image to the stream
           virtual void writeImage ( const void* buf, bool compress = true );


           /** Writes an RLE compressed or uncompressed image from the stream.
               \param pnter Pointer to a void* , which will contain the new image.
               \param allocated If false, a sufficient amount of memory will be allocated for the image.
                                If true it is assumed that a sufficiently large amount of memory has already been allocated.
               \param size Will contain the size of the image. Must not be NULL.
            */
           virtual void readrlepict( void** pnter, bool allocated, int* size);

           //! returns the name of the stream.
           virtual ASCString getDeviceName();

           /** returns the location of the stream. This may be a complete english sentence describing how the stream (usually a file) can be found. 
               Should only be used for informing the user. */
           virtual ASCString getLocation();
           
           //! returns the archive name if the stream is located inside an archive; or "" if not.
           virtual ASCString getArchive();

           //! Sets the stream pointer to a new location. An exception is thrown if the stream does not support seeking.
           virtual void seek ( int newpos );

           //! returns the size of the stream or -1 if the stream does not have a size
           virtual int getSize ( void ) { return -1; };

           virtual ~tnstream() {};
         protected:
           ASCString devicename; 
};



#endif // basestreaminterface_h_included
