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

#include <malloc.h>
#include <stdio.h>
#include <i86.h>
#include <graph.h>

#include "tpascal.inc"
#include "typen.h"
#include "basestrm.h"
#include "misc.h"
#include "dos.h"
#include "sgstream.h"
#include "vesa.h"

dacpalette256 pal;


main (void)
{ 
   int quantity=0;   

   struct find_t  fileinfo;
   unsigned rc;        /* return code */ 

   rc = _dos_findfirst( "s*.tnk", _A_NORMAL, &fileinfo );
          
   while (rc == 0) {
      quantity++;
      pvehicletype   ft;
      ft = loadvehicletypetype(fileinfo.name);

      if ( ft->terrainreq1 )
         printf (" converting <%15.15s>   ", fileinfo.name);


      rc = _dos_findnext( &fileinfo ); 
   }

   printf ("\n %i files converted ! \n\n", quantity);
   return 0;
};



