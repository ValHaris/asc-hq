/*! \file oldlzw.cpp
    \brief Some old LZW encoding and decoding stuff. Included by basestrm.cpp
   
    ASC does not use LZW for compressing new files, since the bzlib is the new
    compression library that replaces this one. But there are old files around
    which should still be loaded.
*/

//     $Id: oldlzw.cpp,v 1.7 2001-02-18 15:37:16 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.6  2001/01/28 14:04:15  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.5  2000/10/11 14:26:44  mbickel
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
//     Revision 1.4  2000/05/03 14:49:14  mbickel
//      Fixed a bug in LZW decompression
//
//     Revision 1.3  1999/11/22 18:27:47  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:42:17  tmwilson
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



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


tlzwstreamcompression  :: tlzwstreamcompression ( void )
{
    mode = none;
    initread = 0;
    initwrite = 0;
    DecodeBufferSize = 0;    
    DecodeBuffer = NULL;
    rdictionary = NULL;
    readcnt = 0;
    wdictionary = NULL;
}


void tlzwstreamcompression  :: LZWOut ( CodeType code )
{
    writelzwdata ( &code, sizeof( CodeType )) ;

    LZWTotBytesOut += 2;
    LZWCurrBytesOut += 2;
}




/* our hashing lookup function */
 IndexType tlzwstreamcompression  :: LZWFind ( CodeType currcode, int in )
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


 void tlzwstreamcompression  :: initreading ( void )
{
  if ( mode == none ) {    // 1: read, 2: write
     char tempreadbuf[10];
     int tempreadbufsize = 0;

     int lzw = 1;
     int i;

     for (i = 0; i < strlen ( LZ_SIGNATURE ) + 3; i++) {
        try {
           readlzwdata ( &tempreadbuf[tempreadbufsize], 1 );
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
            throw toutofmem ( DICTIONARY_SIZE * sizeof( struct Rdictionary ) );
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
              tempbuf.push ( tempreadbuf[i] );

           mode = readingdirect;
        }
     }
     initread = 1;

  } else
    if ( initwrite )
       throw tinvalidmode ( "tlzwstreamcompression ", tnstream::IOMode ( mode ), tnstream::reading );
}



 void tlzwstreamcompression  :: initwriting ( void )
{
  if ( mode == none ) {    

      freecode = STARTING_CODE;
  
      LZWTotBytesIn = 0;
      LZWTotBytesOut = 0;
      LZWCurrBytesIn = 0;
      LZWCurrBytesOut = 0;
  
      char c = 0;
      writelzwdata ( &c, 1 );
      writelzwdata ( LZ_SIGNATURE, strlen ( LZ_SIGNATURE ) + 1 );
      writelzwdata ( &c, 1 );
  
  
      wdictionary = new Wdictionary [ DICTIONARY_SIZE ];
                  
      if ( ! wdictionary )
          throw toutofmem ( DICTIONARY_SIZE * sizeof( struct Wdictionary ) );
  
      for ( i = 0; i < DICTIONARY_SIZE; i++ )
          wdictionary[ i ].code = UNUSED_CODE;
  
      currcodeloaded = 0;

      initwrite = 1;
      mode = writing;

  } else
    if ( initread )
       throw tinvalidmode ( "tlzwstreamcompression ", tnstream::IOMode ( mode ) , tnstream::writing );
}



 void tlzwstreamcompression  :: writedata ( const void* buf, int size )
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



 void tlzwstreamcompression  :: LZWIn ( void )
{
    readlzwdata ( &incode, sizeof( CodeType ) );
}


/* the active decompression routine */
 unsigned tlzwstreamcompression  :: LZWLoadBuffer ( unsigned count, CodeType code )
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
                throw toutofmem ( DecodeBufferSize + 1000 );
            else      
                DecodeBufferSize += 1000;
        }
        code = rdictionary[ code ].parent;
    }
    DecodeBuffer[ count++ ] = code;
    return ( count );
}



int tlzwstreamcompression  :: readdata ( void* buf, int size, int excpt  )
{
   if ( !initread )
      initreading();

   int pos = 0;
   char* buf2 = (char*) buf ;


   if ( mode == readingdirect ) {
      int tp = 0;
      while ( pos < size  &&  tempbuf.size() )  {
         buf2 [pos++] = tempbuf.front();
         tempbuf.pop();
         tp++;
       }
       if ( size-pos > 0 )
          tp += readlzwdata ( &buf2[pos], size-pos, excpt );

       return tp;

   } else



   if ( mode == readingrle ) {
      while ( pos < size ) {

          if ( rlenum ) {
             buf2[pos++] = rledata;
             rlenum--;
          } else {

             // getting byte

             if ( !readlzwdata ( &rledata, 1, excpt ))
                return pos;

             if ( rledata == rlestartbyte ) {
                readlzwdata ( &rlenum, 1 );
                if ( rlenum > 2 )
                   readlzwdata ( &rledata, 1 );
             } else
                buf2[pos++] = rledata;
          }

      } /* endwhile */
      return pos;

   } else {

       while ( pos < size  &&  tempbuf.size() ) {
          buf2 [pos++] = tempbuf.front();
          tempbuf.pop();
       }


       if ( pos < size ) {
   
          /* prime the pump */
      
          if (!DecodeBufferSize)
          {
              DecodeBufferSize = 1000;
              DecodeBuffer =  (unsigned char * ) new char [ DecodeBufferSize ];
              if ( DecodeBuffer == NULL )
                  throw toutofmem ( DecodeBufferSize );
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
                 throw ASCexception();
                             
              inchar = DecodeBuffer[ count - 1 ];
              while ( count )
              {
                 if ( pos < size )
                    buf2 [pos++] = DecodeBuffer[--count];
                 else 
                    tempbuf.push ( DecodeBuffer[--count] );
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

      return pos;
   }

   // return 0;
}


 void tlzwstreamcompression  :: close( void )
{
   if ( mode == writing ) {
      LZWOut ( currcode );
      LZWOut ( END_OF_INPUT );
      mode = none;
   }
}


 tlzwstreamcompression  :: ~tlzwstreamcompression ( )
{
   if ( mode == writing )
      close();

    if ( rdictionary ) {
       delete[] rdictionary;
       rdictionary = NULL;
    }
    if ( wdictionary ) {
       delete[] wdictionary;
       wdictionary = NULL;
    }

    if ( DecodeBuffer ) {
       delete[] DecodeBuffer;
       DecodeBuffer = NULL;
    }

}
