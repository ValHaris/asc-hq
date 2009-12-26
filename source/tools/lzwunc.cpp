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

/*--- lzwunc.c ---------------------------- Listing 9-10 -------
 * Decompress an LZW-compressed file
 *
 * Entry point:     int LZWDecode(FILE *infile, FILE *outfile)
 *
 *                  Will decompress infile into outfile. Both
 *                  files should already be open in binary mode.
 *                  They will be closed by LZWDecode().
 *
 *                  LZWDecode() returns:
 *                      0: success
 *                      1: invalid signature byte
 *                      2: bad malloc
 *                      3: read error
 *                      4: write error
 *
 *
 * Switches:        DRIVER - compiles a test driver
 *                  CODES  - displays input codes
 *-------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lzw.h"


static int LZWIn ( FILE *infile )
{
    if ( fread ( &incode, sizeof( CodeType ), 1, infile ) != 1 )
        return ( 4 );
    return ( 0 );
}




/* the active decompression routine */
static IndexType freecode;
static unsigned LZWLoadBuffer ( unsigned count, CodeType code )
{
    if ( code >= freecode )
    {
        printf( "LZWLoad: code %u out of range!", code );
        return ( 0 );
    }
    while ( code >= PRESET_CODE_MAX )
    {
        DecodeBuffer[ count++ ] = rdictionary[ code ].c;
        if ( count == DecodeBufferSize )
        {
            DecodeBuffer =
               realloc ( DecodeBuffer, DecodeBufferSize + 1000 );
            if ( ! DecodeBuffer )
            {
                /* out of memory */
                DecodeBufferSize = 0;
                return ( 0 );
            }
            else
                DecodeBufferSize += 1000;
        }
        code = rdictionary[ code ].parent;
    }
    DecodeBuffer[ count++ ] = code;
    return ( count );
}


int LZWDecode ( FILE *infile, FILE *outfile )
{
    char buffer[ 10 ];
    int retval = 0;
    unsigned int inchar;
    unsigned count;
    CodeType oldcode;

    /* check the signature */
    fgets ( buffer, strlen ( SIGNATURE ) + 2, infile );
    if ( strcmp ( buffer, SIGNATURE ))
    {
        retval = 1;
        goto done;
    }

    /* prime the pump */

    if (!DecodeBufferSize)
    {
        DecodeBufferSize = 1000;
        DecodeBuffer = malloc ( DecodeBufferSize );
        if ( DecodeBuffer == NULL )
            throw OutOfMemoryError ( DecodeBufferSize );
    }
    rdictionary = malloc( DICTIONARY_SIZE * sizeof( struct Rdictionary ) );

    if ( rdictionary == NULL )
        throw OutOfMemoryError ( DICTIONARY_SIZE * sizeof( struct Rdictionary ) );



 priming:
    freecode = STARTING_CODE;
    if ( retval = LZWIn ( infile ))
        goto done;
    if ( incode == END_OF_INPUT )
        goto done;

    /* the first character always is itself */
    oldcode = incode;
    inchar = incode;
    fputc( incode, outfile );

    while ( ! ( retval = LZWIn ( infile )))
    {
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
            return ( 2 ); /* had a memory problem */

        inchar = DecodeBuffer[ count - 1 ];
        while ( count )
        {
             fputc ( DecodeBuffer[--count], outfile);
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

 done:
    fclose ( infile );
    fclose ( outfile );
    return ( retval );
}

