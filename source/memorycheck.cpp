/** \file memorycheck.cpp
    \brief Includefile (!) for heap tracking
  
    The routines here can be included into the main programs for finding heap
    related problems.
*/

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

  int blockNum = 0;
  int operationID = 0;
  int errorNum = 0;
  int breakOnFree = 0;
  const int infoblocksize = 3;

  bool memchk_initialized = false;


  bool memchk_reallyComplete = false;  // this variable is intended to be set by the debugger
  int memchk_breakOnOp = -1;           // this variable is intended to be set by the debugger

  void memchkError ( )
  {
     errorNum++;
     fprintf(stderr, "memchkError !\n");
  }

  class Block {
     public:
        int start;
        int stop;
        int userStart;
        int size;
        int allocated;
        int freed;
        int allocatedOP;
        int freedOP;
  };


  Block blocks[1000000];

  void* verifyblock ( int tp, Block* b )
  {
     int error = 0;
     int* tmpi = (int*) b->start;

     if ( tp != -1 )
        if ( tmpi[0] != tp )
           error++;  // for example: allocated with new  ; freed with delete[]
/*
     if ( tmpi[1] != (int) tmpi) {
        memchkError();
        #ifdef logging
         logtofile ( "memory check: verifyblock : error A at address %x", p );
        #endif
     }
*/
     int amt = tmpi[2];

     for ( int i = 0; i < 25; i++ ) {
        if ( tmpi[infoblocksize + i] != 0x12345678) {
           memchkError();
           #ifdef logging
            logtofile ( "memory check: verifyblock : error C at address %x", p );
           #endif
        }

        if ( tmpi[infoblocksize + i + (amt+3)/4 + 25] != 0x87654321 ) {
           memchkError();
           #ifdef logging
            logtofile ( "memory check: verifyblock : error D at address %x", p );
           #endif
        }
     }
     return tmpi;
  }

  void verifyallblocks ( void )
  {
     for ( int i = 0; i < blockNum; i++ )
        if ( !blocks[i].freed )
           verifyblock ( -1, &blocks[i] );
  }

  void* memchkAlloc ( int tp, size_t amt )
  {
     if ( ! memchk_initialized ) {
        memset ( blocks, 0, sizeof ( blocks ));
        memchk_initialized = true;
     }

     if ( memchk_reallyComplete )
        verifyallblocks();

     Block& b = blocks[ blockNum++ ];
     if ( blockNum >= 1000000 )
        memchkError();

     b.size = amt + (50+infoblocksize) * 4;
     b.allocated++;

     if ( operationID == memchk_breakOnOp )
        memchkError();

     b.allocatedOP = operationID++;

     void* tmp = malloc ( b.size+4 );


     b.start = (int) tmp;
     b.stop = b.start + b.size;

     for ( int i = 0; i < blockNum-1; i++ ) {
        if ( !blocks[i].freed ) {
           if ( b.start >= blocks[i].start && b.start < blocks[i].stop )
              memchkError();
           if ( b.stop >= blocks[i].start  && b.stop < blocks[i].stop )
              memchkError();
           if ( blocks[i].start < b.start && blocks[i].stop > b.stop )
              memchkError();
        }
     }


     int* tmpi = (int*) tmp;
     /*
     if ( (int) tmpi == 0x1bb2138 || (int) tmpi == 0x1bcf178 )
        error++;
        */
     tmpi[0] = tp;

     if ( 4 < infoblocksize )
       tmpi[4] = (int) tmp;

     if ( 2 < infoblocksize )
       tmpi[2] = amt;

     if ( 3 < infoblocksize )
        tmpi[3] = b.allocatedOP;

     if ( 1 < infoblocksize )
        tmpi[1] = blockNum-1;

     for ( int i = 0; i < 25; i++ ) {
        tmpi[infoblocksize + i] = 0x12345678;
        tmpi[infoblocksize + i + (amt+3)/4 + 25] = 0x87654321;
     }
     void* p = &tmpi[25+infoblocksize];
     b.userStart = (int) p;
     return p;
  }

  void memchkFree ( int tp, void* buf )
  {
     if ( breakOnFree == (int) buf )
        memchkError();

     Block* b = NULL;
     for ( int i = blockNum-1; i>= 0 ; i-- )
        if ( blocks[i].userStart == (int) buf ) {
           b = &blocks[i];
           break;
        }

     if ( b ) {

        if ( memchk_reallyComplete )
           verifyallblocks();

        void* tmpi = verifyblock ( tp, b );

        if ( b->freed > 0 )
           memchkError();

        if ( b->allocated <= 0 )
           memchkError();

        if ( operationID == memchk_breakOnOp )
           memchkError();


        b->freed++;
        b->freedOP = operationID++;

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
