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

/*--- lzwcomp.c --------------------------- Listing 9-9 --------------
 * Compress an input file, using the LZW encoding technique
 *
 * Entry point:     int LZWEncode (FILE *infile, FILE *outfile)
 *
 *                  Will compress infile into outfile. Both
 *                  files should already be open in binary mode.
 *                  They will be closed by LZWEncode().
 *
 *                  LZWEncode() returns:
 *                      0: success
 *                      1: error while reading in
 *                      2: malloc error
 *                      4: error while writing out
 *
 * Switches:        DRIVER    - compiles a test driver
 *                  HASHSTATS - reports on hashing efficiency
 *                  HASHHITS  - show table of hash hits
 *                  SHOWSTATS - provides compression statistics
 *-------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*--- the data storage and output routines ---*/


template<class T> void tnlzwstream<T> :: LZWOut ( CodeType code )
{
    T::writedata ( &code, sizeof( CodeType )) ;

    LZWTotBytesOut += 2;
    LZWCurrBytesOut += 2;
}




/* our hashing lookup function */
template<class T> IndexType tnlzwstream<T> :: LZWFind ( CodeType currcode, int in )
{
    IndexType ndx;
    int step = 11, pastzero = 0;

    ndx = ( ( IndexType ) currcode << 8 ) | in;
    ndx = ndx % DICTIONARY_SIZE;

    for ( ;; )
    {
        if ( wdictionary[ ndx ].code == UNUSED_CODE )
            break;
        if ( wdictionary[ ndx ].parent == currcode &&
             wdictionary[ ndx ].c == in )
            break;
        ndx += step;
        if ( ndx >= DICTIONARY_SIZE)
        {
            ndx -= DICTIONARY_SIZE;
            pastzero += 1;

            /*
             * Next is a safety check. If the step
             * value and the dictionary size are
             * relatively prime, there should never
             * be a problem. However, let's not loop
             * too many times.
             */

            if ( pastzero > 5 )
                step = 1;
        }

    }
    return ( ndx );
}


template<class T> void tnlzwstream<T> :: initreading ( void )
{
  if ( mode == none ) {    // 1: read, 2: write
     char tempreadbuf[10];
     int tempreadbufsize = 0;

     int lzw = 1;

     for (int i = 0; i < strlen ( LZ_SIGNATURE ) + 3; i++) {
        try {
           T::readdata ( &tempreadbuf[tempreadbufsize], 1 );
           tempreadbufsize++;
        } /* endtry */

        catch ( treadafterend ) {
           tempreadbufsize--;
           lzw = 0;
        } /* endcatch */
     }

     if ( lzw ) {
        if ( tempreadbuf[0] )
           lzw = 0;
        if ( strcmp ( &tempreadbuf[1], LZ_SIGNATURE ) ) 
           lzw = 0;
        if ( tempreadbuf[strlen ( LZ_SIGNATURE ) + 2] )
           lzw = 0;
     }
     if ( lzw == 1 ) {
        mode = reading;

        rdictionary = new Rdictionary [DICTIONARY_SIZE];
    
        if ( rdictionary == NULL )
            throw OutOfMemoryError ( DICTIONARY_SIZE * sizeof( struct Rdictionary ) );
     } else {
        lzw = 2;
        if ( tempreadbuf[0] )
           lzw = 0;
        if ( strcmp ( &tempreadbuf[1], RLE_SIGNATURE ) ) 
           lzw = 0;
        if ( tempreadbuf[strlen ( RLE_SIGNATURE ) + 2] )
           lzw = 0;

        if ( lzw == 2 ) {
           rlestartbyte = tempreadbuf[strlen ( RLE_SIGNATURE ) + 3];

           rlenum = 0;
           mode = readingrle;

        } else {
           for ( i = 0; i < tempreadbufsize; i++ )
              tempbuf.putval ( tempreadbuf[i] );
   
           mode = readingdirect;
        }
     }
     initread = 1;

  } else
    if ( initwrite )
       throw tinvalidmode ( "tnlzwstream<T>", mode , reading );
}



template<class T> void tnlzwstream<T> :: initwriting ( void )
{
  if ( mode == none ) {    

      freecode = STARTING_CODE;
  
      LZWTotBytesIn = 0;
      LZWTotBytesOut = 0;
      LZWCurrBytesIn = 0;
      LZWCurrBytesOut = 0;
  
      char c = 0;
      T::writedata ( &c, 1 );
      T::writedata ( LZ_SIGNATURE, strlen ( LZ_SIGNATURE ) + 1 );
      T::writedata ( &c, 1 );
  
  
      wdictionary = new Wdictionary [ DICTIONARY_SIZE ];
                  
      if ( ! wdictionary )
          throw OutOfMemoryError ( DICTIONARY_SIZE * sizeof( struct Wdictionary ) );
  
      for ( i = 0; i < DICTIONARY_SIZE; i++ )
          wdictionary[ i ].code = UNUSED_CODE;
  
      currcodeloaded = 0;

      initwrite = 1;
      mode = writing;

  } else
    if ( initread )
       throw tinvalidmode ( "tnlzwstream<T>", mode , writing );
}

template<class T> tnlzwstream<T> :: tnlzwstream ( void )
{
    mode = none;
    initread = 0;
    initwrite = 0;
    DecodeBufferSize = 0;    
    rdictionary = NULL;
    readcnt = 0;
}


template<class T> void tnlzwstream<T> :: writedata ( const void* buf, int size )
{
   if ( !initwrite )
      initwriting();

   const char* buf2 = (char*) buf;
   int pos = 0;

   if ( size ) {
      if ( !currcodeloaded ) {
         currcode = buf2[pos++];

         LZWTotBytesIn += 1;
         LZWCurrBytesIn += 1;
         currcode &= 0xFF; /* make sure we don't sign extend */

         currcodeloaded = 1;
      }

      while ( pos < size )
      {
            in = buf2[pos++];

            LZWTotBytesIn += 1;
            LZWCurrBytesIn += 1;
            idx = LZWFind ( currcode, in );
            if ( wdictionary[ idx ].code == UNUSED_CODE )
            {
                /* not a match */
                LZWOut ( currcode );

                /* now, update the dictionary */
                if ( freecode < MAX_CODE )
                {
                    wdictionary[ idx ].c = in;
                    wdictionary[ idx ].code = freecode++;
                    wdictionary[ idx ].parent = currcode;
                }
                currcode = in;

                /* Had a miss; check compression efficiency */
                if ( LZWCurrBytesIn >= 10000 )
                {
                    unsigned ratio;
                    ratio = ( LZWCurrBytesOut * 100 ) /
                             LZWCurrBytesIn;

                    LZWCurrBytesIn = 0;
                    LZWCurrBytesOut = 0;
                    if ( ratio > LZWBestRatio )
                    {
                        if ( ratio > 50 &&
                             ( ratio > 90 ||
                             ratio > LZWLastRatio + 10 ))
                        {
                            LZWOut ( NEW_DICTIONARY );
                            for ( i = 0;
                                  i < DICTIONARY_SIZE; i++ )
                                wdictionary[ i ].code =
                                    UNUSED_CODE;
                            freecode = STARTING_CODE;
                        }
                    }
                    else
                        LZWBestRatio = ratio;
                    LZWLastRatio = ratio;
                }
            }
            else /* we match so far--keep going */
                currcode = wdictionary[ idx ].code;

      }
   }
}















template<class T> void tnlzwstream<T> :: LZWIn ( void )
{
    T::readdata ( &incode, sizeof( CodeType ) );
}


/* the active decompression routine */
template<class T> unsigned tnlzwstream<T> :: LZWLoadBuffer ( unsigned count, CodeType code )
{
    if ( code >= freecode )
       throw tbufferoverflow();
                   
    while ( code >= PRESET_CODE_MAX )
    {
        DecodeBuffer[ count++ ] = rdictionary[ code ].c;
        if ( count == DecodeBufferSize )
        {
            DecodeBuffer = (unsigned char *) realloc ( DecodeBuffer, DecodeBufferSize + 1000 );

            if ( ! DecodeBuffer )
                throw OutOfMemoryError ( DecodeBufferSize + 1000 );
            else      
                DecodeBufferSize += 1000;
        }
        code = rdictionary[ code ].parent;
    }
    DecodeBuffer[ count++ ] = code;
    return ( count );
}



template<class T> void tnlzwstream<T> :: readdata ( void* buf, int size )
{
   if ( !initread )
      initreading();

   int pos = 0;
   char* buf2 = (char*) buf ;


   if ( mode == readingdirect ) {
      while ( pos < size  &&  tempbuf.valavail() ) 
         buf2 [pos++] = tempbuf.getval();
       if ( size-pos > 0 ) 
          T::readdata ( &buf2[pos], size-pos );
   } else 



   if ( mode == readingrle ) {
      while ( pos < size ) {
          
          if ( rlenum ) {
             buf2[pos++] = rledata;
             rlenum--;
          } else {

             // getting byte

             T::readdata ( &rledata, 1 );

             if ( rledata == rlestartbyte ) {
                T::readdata ( &rlenum, 1 );
                if ( rlenum > 2 ) 
                   T::readdata ( &rledata, 1 );
             } else
                buf2[pos++] = rledata;
          }

      } /* endwhile */

   } else {
   
       while ( pos < size  &&  tempbuf.valavail() ) 
          buf2 [pos++] = tempbuf.getval();


       if ( pos < size ) {
   
          /* prime the pump */
      
          if (!DecodeBufferSize)
          {
              DecodeBufferSize = 1000;
              DecodeBuffer = (unsigned char *) malloc ( DecodeBufferSize );
              if ( DecodeBuffer == NULL )
                  throw OutOfMemoryError ( DecodeBufferSize );
          }
      
      
          if ( !readcnt ) {
   
          priming:
             freecode = STARTING_CODE;
             LZWIn (  );
         
             if ( incode == END_OF_INPUT )
                 goto done;
   
             /* the first character always is itself */
   
             oldcode = incode;
             inchar = incode;
             buf2[ pos++ ] = incode;
   
          }
      
          while ( pos < size ) {
              LZWIn ( );
   
              if ( incode == END_OF_INPUT )
                  break;
                  
              if ( incode == NEW_DICTIONARY )
                  goto priming;
                  
              if ( incode >= freecode )
              {
                  /* We have a code that's not in our rdictionary! */
                  /* This can happen only one way--see text */
      
                  count = LZWLoadBuffer ( 1, oldcode );
      
                  /* Make last char same as first. Can use either */
                  /* inchar or the DecodeBuffer[count-1] */
      
                  DecodeBuffer[ 0 ] = inchar;
              }
              else
                  count = LZWLoadBuffer ( 0, incode );
      
              if ( count == 0 )
                 throw terror();
                             
              inchar = DecodeBuffer[ count - 1 ];
              while ( count )
              {
                 if ( pos < size )
                    buf2 [pos++] = DecodeBuffer[--count];
                 else 
                    tempbuf.putval ( DecodeBuffer[--count] ); 
              }
      
              /* now, update the rdictionary */
              if ( freecode < MAX_CODE )
              {
                  rdictionary[ freecode ].parent = oldcode;
                  rdictionary[ freecode ].c = inchar;
                  freecode += 1;
      
              }
              oldcode = incode;
          }
       }
    done:
      readcnt++;
   }
}


template<class T> void tnlzwstream<T> :: close( void )
{
   if ( mode == writing ) {
      LZWOut ( currcode );
      LZWOut ( END_OF_INPUT );
      mode = none;
   }
   T::close();
}


template<class T> tnlzwstream<T> :: ~tnlzwstream ( )
{
   if ( mode == writing )
      close();
}


