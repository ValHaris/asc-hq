#include <stdio.h>
#include <new.h>
#include <malloc.h>
#include <ctype.h>
#include <conio.h>
#include <stdlib.h>
#include <i86.h>
#include <dos.h>

#include "..\dlg_box.h"
#include "..\loaders.h"
#include "memory.h"

 void* reservememory = NULL;
 void* emergencymemory = NULL;
 #define reservememorysize 300000
 #define emergencymemorysize 4000

 void emergency_new_handler ( void )
 {
    displaymessage("run out of memory while in new_new_handler.\n please contact authors.\n",2 );
 }

 void new_new_handler ( void )
 {
    delete  ( reservememory );
    set_new_handler ( emergency_new_handler );
    savegame("rescue.sav","game saved while exiting game due to a lack of memory ");
    displaymessage("Not enough memory. Saved game to emergncy.sav. ",2 );
 }

 void initmemory( void ) 
 {
     int memneeded = 20000000;
     int ma = maxavail() + _memavl();

     printf(" \n memory avaiable: %d \n ", ma );
     if ( ma < memneeded ) {
        printf(" Not enough momory available, at least %d MB recommended.\n"
               " You may try to run the game, but unpredictable results can happen.\n"
               " Press R to start the game anyway or any other key to quit\n", memneeded/1000000 );

        int ch = getch();
        if ( toupper ( ch ) != 'R' )
           exit(2);
     }

     reservememory = asc_malloc ( reservememorysize );
     emergencymemory = asc_malloc ( emergencymemorysize );
     set_new_handler ( new_new_handler );
 }


struct tmeminfo {
    unsigned LargestBlockAvail;
    unsigned MaxUnlockedPage;
    unsigned LargestLockablePage;
    unsigned LinAddrSpace;
    unsigned NumFreePagesAvail;
    unsigned NumPhysicalPagesFree;
    unsigned TotalPhysicalPages;
    unsigned FreeLinAddrSpace;
    unsigned SizeOfPageFile;
    unsigned Reserved[3];
} meminfo;

#define DPMI_INT	0x31



void getmeminfo( void )
{
    union REGS regs;
    struct SREGS sregs;

    regs.x.eax = 0x00000500;
    memset( &sregs, 0, sizeof(sregs) );
    sregs.es = FP_SEG( &meminfo );
    regs.x.edi = FP_OFF( &meminfo );

    int386x( DPMI_INT, &regs, &regs, &sregs );
}

int maxavail()
{
   getmeminfo();
   return meminfo. LargestBlockAvail;
}


int memavail()
{
   return maxavail();
}

