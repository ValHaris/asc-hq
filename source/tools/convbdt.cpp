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

#include <string.h>
#include <stdio.h>
#include <dos.h>
#include <malloc.h>
#include <conio.h>

#include "..\tpascal.inc"
#include "..\typen.h"
#include "..\sgstream.h"
#include "..\misc.h"
#include "..\vesa.h"
#include "..\stack.h"



main(int argc, char *argv[] )
{ 
   if ( argc < 2 ) {
      printf("syntax: convbodn filename/wildcard\n");
      return 1;
   }

  find_t  fileinfo;
  unsigned rc;        /* return code */
  pterraintype bdt;
  int i;

   loadpalette();


   tterrainbits bts ( 0, 1 );


   tterrainbits packeis ( 1 << 30, 0 );


  rc = _dos_findfirst( argv[1], _A_NORMAL, &fileinfo );
  while( rc == 0 ) { 
      bdt = loadterraintype ( fileinfo.name );
      int modi =0;
      for ( int w = 0 ; w < cwettertypennum; w++ )
         if ( bdt->weather[w] )
            if ( bdt->weather[w]->art & packeis ) {
               bdt->weather[w]->art |= bts;
               bdt->weather[w]->art = bdt->weather[w]->art ^ packeis;
               printf(" %s weather %d modified \n", fileinfo.name, w );
               modi = 1;
            }

      if ( modi ) {
         tn_file_buf_stream mainstream ( fileinfo.name ,2);
         writeterrain ( bdt, &mainstream );                
      }

      rc = _dos_findnext( &fileinfo );
   }
   return 0;
}
