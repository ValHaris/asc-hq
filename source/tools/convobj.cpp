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


#define cpy(u) bldn-> u = bldo-> u

void main ( void ) 
{
  find_t  fileinfo;
  unsigned rc;        /* return code */
  pobjecttype nobj;
  int i;
  rc = _dos_findfirst( "*.obl", _A_NORMAL, &fileinfo );
  while( rc == 0 ) { 
      nobj = loadobjecttype ( fileinfo.name );
      nobj->objectslinkablenum = 0;
      nobj->objectslinkable = 0;
/*      nobj->xoffs = 0;
      nobj->yoffs = 0;*/
             
      {
          tn_file_buf_stream mainstream ( fileinfo.name ,2);
          writeobject ( nobj, &mainstream, 0 );
      }

      printf(" %s written \n",fileinfo.name);

      rc = _dos_findnext( &fileinfo );
  }

}
