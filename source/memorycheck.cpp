/***************************************************************************
                          memorycheck.cpp  -  description
                             -------------------
    begin                : Wed Nov 29 2000
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


#ifdef MEMCHK

  int blocknum = 0;
  int blocklist[1000000];


  void addblock ( void* p )
  {
     blocklist[blocknum++] = (int) p;
  }

  int removeblock ( void* p )
  {
     if ( blocknum== 0 )
        return 0;

     int error = 0;
     int found = 0;
     int pos = 0;
     while ( !found && pos < blocknum) {
        if ( blocklist[pos] == (int) p )
           found++;
        else
           pos++;
     } /* endwhile */
     if ( found ) {
        for ( int i = pos+1; i < blocknum; i++ )
           blocklist[i-1] = blocklist[i];
        blocknum--;
     }

     return found;
  }

  int blockavail( void* p )
  {
     int found = 0;
     int pos = 0;
     while ( !found && pos < blocknum) {
        if ( blocklist[pos] == (int) p )
           found++;
        else
           pos++;
     } /* endwhile */
     return found;
  }

  void verifyallblocks( void );

  void* memchkAlloc ( int tp, size_t amt )
  {
     // verifyallblocks();
     int error;
     void* tmp = malloc ( amt + 53 * 4 );
    #ifdef _DOS_
     if ( !tmp )
        new_new_handler();
    #endif

     int* tmpi = (int*) tmp;
     /*
     if ( (int) tmpi == 0x1bb2138 || (int) tmpi == 0x1bcf178 )
        error++;
        */
     tmpi[0] = tp;
     tmpi[1] = (int) tmp;
     tmpi[2] = amt;
     for ( int i = 0; i < 25; i++ ) {
        tmpi[3 + i] = 0x12345678;
        tmpi[3 + i + (amt+3)/4 + 25] = 0x87654321;
     }
     void* p = &tmpi[28];

     addblock ( p );
     return p;
  }


  void* verifyblock ( int tp, void* p )
  {
     int error = 0;
     int* tmpi = (int*) p;
     tmpi -= 28;

     if ( tp != -1 )
        if ( tmpi[0] != tp )
           error++;

     if ( tmpi[1] != (int) tmpi) {
        error++;
        #ifdef logging
         logtofile ( "memory check: verifyblock : error A at address %x", p );
        #endif
     }

     int amt = tmpi[2];

     for ( int i = 0; i < 25; i++ ) {

        if ( tmpi[3 + i] != 0x12345678)
           if ( i == 1  &&  tmpi[3 + i] == -2) {
              error++;  // deallocated twice
              #ifdef logging
               logtofile ( "memory check: verifyblock : error B at address %x", p );
              #endif
           } else {
              error++;
              #ifdef logging
               logtofile ( "memory check: verifyblock : error C at address %x", p );
              #endif
           }

        if ( tmpi[3 + i + (amt+3)/4 + 25] != 0x87654321 ) {
           error++;
           #ifdef logging
            logtofile ( "memory check: verifyblock : error D at address %x", p );
           #endif
        }
     }
     return tmpi;
  }

  void verifyallblocks ( void )
  {
     for ( int i = 0; i < blocknum; i++ )
        verifyblock ( -1, (void*) blocklist[i] );
  }

  void memchkFree ( int tp, void* buf )
  {
     if ( removeblock ( buf )) {
        void* tmpi = verifyblock ( tp, buf );

        int* tmpi2 = (int*) buf;
        tmpi2 -= 28;
        tmpi2[4] = -2;


        free ( tmpi );
     } else
       free ( buf );
  }

  void *operator new( size_t amt )
  {
      return( memchkAlloc( 100, amt ) );
  }

  void operator delete( void *p )
  {
     if ( p )
      memchkFree( 100, p );
  }

  void *operator new []( size_t amt )
  {
      return( memchkAlloc( 102, amt ) );
  }

  void operator delete []( void *p )
  {
     if ( p )
      memchkFree( 102, p );
  }

  void* asc_malloc ( size_t size )
  {
     void* tmp = memchkAlloc ( 104, size );
    #ifdef _DOS_
     if ( tmp == NULL )
        new_new_handler();
    #endif
     return tmp;
  }

  void asc_free ( void* p )
  {
     memchkFree ( 104, p );
  }

#else

  void* asc_malloc ( size_t size )
  {
     void* tmp = malloc ( size );
    #ifdef _DOS_
     if ( tmp == NULL )
        new_new_handler();
    #endif
     return tmp;
  }

  void asc_free ( void* p )
  {
     free ( p );
  }


#endif
