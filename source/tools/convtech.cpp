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

#include "tpascal.inc"
#include "typen.h"
#include "sgstream.h"
#include "misc.h"
#include "vesa.h"
#include "stack.h"


dacpalette256 pal;

   typedef struct totechnology* potechnology ;

   struct totechnology { 
                   pointer      icon; 
                   pchar        infotext; 
                   word         id; 
                   int      researchpoints; 
                   char         name[31]; 

                   tresearchdatachange unitimprovement; 

                   boolean      requireevent; 

                   union { 
                      ptechnology  requiretechnology[6]; 
                      int      requiretechnologyid[6]; 
                   };

                   /*
                   int          techlevelget;  // sobald dieser technologylevel erreicht ist, ist die Technologie automatisch verfgbar
                   int          settechlevel;  // sobald diese technology erforscht wird, ist dieser techlevel erreicht
                   */
                 };


potechnology       loadotechnology(char *       name)
{ 
  potechnology  pt; 
  int          w;


   mainstream.openstream(name,1); 
   if (mainstream.getstatus() != 0) {
      loaderror = 10;
      return NULL;           
   } 
   pt =  new totechnology ;
   mainstreamread(*pt, sizeof(*pt)); 
   passtring2cstring ( pt->name );
   if ( pt->icon != NULL )
      mainstream.readrlepict( (char**) &pt->icon,false,&w);

   if ( pt->infotext != NULL )
      mainstream.readpchar(&pt->infotext);
      


   mainstream.closestream(); 

   loaderror = mainstream.getstatus();
   return pt; 
} 



void main ( void ) {

  find_t  fileinfo;
  unsigned rc;        /* return code */
  potechnology oldtech;
  ttechnology tech;
  int i;

  mainstream.init();

  rc = _dos_findfirst( "*.tec", _A_NORMAL, &fileinfo );
  while( rc == 0 ) { 
      oldtech = loadotechnology ( fileinfo.name );

      memset ( &tech, 0, sizeof(tech));                
      tech.id = oldtech->id;
      tech.name = oldtech->name;
      if ( strcmpi ( fileinfo.name, "ekranopl.tec" ) == 0 ) {
         printf( "%s has a picture\n ",fileinfo.name );
         tech.icon = oldtech->icon;
      } else {
         printf( "%s has NOT a picture\n ",fileinfo.name );
         tech.icon = NULL;
      }

      tech.infotext = oldtech->infotext;
      tech.researchpoints = oldtech->researchpoints;
      tech.unitimprovement = oldtech->unitimprovement;
      tech.requireevent = oldtech->requireevent;
      for ( i = 0; i < 6; i++ )
         tech.requiretechnologyid[i] = oldtech->requiretechnologyid[i];

      tech.techlevelget = 256;

      mainstream.openstream(fileinfo.name ,2);
      if (mainstream.getstatus() != 0) { 
         printf(" Error writing file !\n\n");
         return ;
      } 
      mainstream.writedata ( (char*) &tech, sizeof ( tech ) );
      if ( tech.name )
         mainstream.writepchar( tech.name );
      if ( tech.infotext )
         mainstream.writepchar( tech.infotext );
      if ( tech.icon )
         mainstream.writerlepict ( tech.icon );

      mainstream.closestream();

      printf(" %s written \n",fileinfo.name);

      rc = _dos_findnext( &fileinfo );
   }

  mainstream.done();

}
