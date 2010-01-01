/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.org
     Copyright (C) 1994-2009  Martin Bickel
 
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

#include "streamencoding.h"
#include "basestrm.h"
            

void testStreamEncoding1()
{
   const int maxsize = 100000;
   char buffer[maxsize];

   int size;

   {
      tnfilestream stream ( "asc2_dlg.zip", tnstream::reading );
      size = stream.readdata(buffer, maxsize, false );
   }

   ASCString encodedData;

   {
      ASCIIEncodingStream stream;
      stream.writedata(buffer, size);
      encodedData = stream.getResult();
   }

   {
      char buffer2[maxsize];

      ASCIIDecodingStream stream ( encodedData );

      int size2 = stream.readdata( buffer2, size, true);
      assertOrThrow( size2 == size );

      for ( int i = 0; i < size; ++i )
         if ( buffer[i] != buffer2[i] )
            throw ASCmsgException( "ASCII Encoding/Decoding failed at offset " + ASCString::toString(i));
   }
}

void testStreamEncoding2()
{
   const int maxsize = 100000;
   char buffer[maxsize];

   int size;

   {
      tnfilestream stream ( "asc2_dlg.zip", tnstream::reading );
      size = stream.readdata(buffer, maxsize, false );
   }

   ASCString encodedData;

   {
      ASCIIEncodingStream outerStream;
      StreamCompressionFilter stream( &outerStream );
      stream.writedata(buffer, size);
      stream.close();

      encodedData = outerStream.getResult();
   }

   {
      char buffer2[maxsize];

      ASCIIDecodingStream outerStream ( encodedData );
      StreamDecompressionFilter stream( &outerStream );

      int size2 = stream.readdata( buffer2, size, true);
      assertOrThrow( size2 == size );

      for ( int i = 0; i < size; ++i )
         if ( buffer[i] != buffer2[i] )
            throw ASCmsgException( "ASCII Encoding/Decoding failed at offset " + ASCString::toString(i));
   }

}

void testStreamEncoding()
{
   testStreamEncoding1();
   testStreamEncoding2();
}