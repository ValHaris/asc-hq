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
#include <dos.h> 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream.h>


#include "tpascal.inc"
#include "typen.h"
#include "sgstream.h"
#include "misc.h"
#include "vesa.h"



  struct tovehicletype {   /*  vehicleart: z.B. Schwere Fu·truppe  */
                    char*        name;          /* z.B. Exterminator  */
                    char*        description;   /* z.B. Jagdbomber    */
                    char*        infotext;      /* optional, kann sehr ausfÅhrlich sein. Empfehlenswert Åber eine Textdatei einzulesen */
                    tweapons     attack;        /*  AngriffsstÑrke der einzelnen Waffen  */
                    tpk          production;    /*  Produktionskosten der vehicle  */
                    Word         armor; 
                    Pointer      picture[8];    /*  0¯  ,  45¯   */
                    Byte         height;        /*  BM  Besteht die Mîglichkeit zum Hîhenwechseln  */
                    word         researchid;
                    int          terrain;    /*  BM     befahrbare terrain: z.B. Schiene, Wasser, Wald, ...  */
                    int          terrainreq; /*  BM     diese Bits MöSSEN in ( field->typ->art & terrain ) gesetzt sein */
                    int          terrainkill;  /* falls das aktuelle field nicht befahrbar ist, und bei field->typ->art eine dieser Bits gesetzt ist, verschwindet die vehicle */
                    Byte         steigung;      /*  max. befahrbare Hîhendifferenz zwischen 2 fieldern  */
                    Byte         jamming;      /*  StÑrke der Stîrstrahlen  */
                    Word         view;         /*  viewweite  */
                    char         wait;        /*  Kann vehicle nach movement sofort schie·en ?  */
                    Byte         tiefgang;      /*  FÅr Schiffe: Tiefgang; bei vehicle: Gewicht  */
                    Word         loadcapacity;      /*  Transportmîglichkeiten  */
                    word         maxunitweight; /*  maximales Gewicht einer zu ladenden vehicle */
                    char         loadcapability;     /*  BM     CHoehenStufen   die zu ladende vehicle mu· sich auf einer dieser Hîhenstufen befinden */
                    char         loadcapabilityreq;  /*  eine vehicle, die geladen werden soll, mu· auf eine diese Hîhenstufen kommen kînnen */
                    char         loadcapabilitynot;  /*  eine vehicle, die auf eine dieser Hîhenstufen kann, darf NICHT geladen werden. Beispiel: Flugzeuge in Transportflieger */
                    Word         id; 
                    int      tank; 
                    Word         fuelconsumption; 
                    int      energy; 
                    int      material; 
                    int      functions;
                    char         movement[8];      /*  max. movementsstrecke  */
                    char         movemalustyp;     /*  wenn ein Bodentyp mehrere Movemali fÅr unterschiedliche vehiclearten, wird dieser genommen.  <= cmovemalitypes */
                    tthreatvar   generalthreatvalue;   /*  Wird von ArtInt benîtigt, au·erhalb keine Bedeutung  */ 
                    tthreatvar   threatvalue[8];       /*  dito                                                 */
                    char         classnum;         /* Anzahl der Klassen, max 8, min 0 ;  Der EINZIGE Unterschied zwischen 0 und 1 ist der NAME ! */
                    char*        classnames[8];    /* Name der einzelnen Klassen */
                    tclassbound  classbound[8];    /* untergrenze (minimum), die zum erreichen dieser Klasse notwendig ist, classbound[0] gilt fÅr vehicletype allgemein*/
                    char         maxwindspeedonwater;
                    //char         digrange;        // Radius, um den nach bodenschÑtzen gesucht wird. 0 bedeutet, es mu· manuell ein field ausgewÑhlt werden
                    //int          dummy[16];
                 }; 

dacpalette256 pal;

void main ( void ) {

  find_t  fileinfo;
  unsigned rc;        /* return code */
  tovehicletype     fzta;
  tvehicletype     fztn;

  int i,j;
  char* nm,*nn;
  char *pi,*pj;

  mainstream.init();

  pj = (char*) malloc ( 800 );
  nm = (char*) malloc ( 200 );

  rc = _dos_findfirst( "*.veh", _A_NORMAL, &fileinfo );
  while( rc == 0 ) { 

        memset ( &fztn, 0, sizeof ( fztn ));

        mainstream.openstream( fileinfo.name, 1);
        if (mainstream.getstatus() != 0) { 
           printf("unable to oepn %s \n\n",fileinfo.name);
           return ; 
        } 
     
        mainstreamread( fzta, sizeof(fzta)); 

        memcpy ( &fztn, &fzta, sizeof ( fzta ));

        if (fzta.name)
           mainstream.readpchar( &fztn.name );
        if (fzta.description)
           mainstream.readpchar( &fztn.description );
        if (fzta.infotext)
           mainstream.readpchar( &fztn.infotext );
        for (i=0;i<8  ;i++ ) 
           if ( fzta.classnames[i] )
              mainstream.readpchar( &(fztn.classnames[i]) );
     
        int w;
     
        for (i=0;i<8  ;i++ ) 
           if ( fzta.picture[i] ) 
              mainstream.readrlepict ( (char**) &(fztn.picture[i]), false, &w);


      mainstream.closestream(); 



      mainstream.openstream(fileinfo.name,2);
      if (mainstream.getstatus() != 0) { 
         printf(" Error writing file !\n\n");
         return ;
      } 
   
      mainstream.writedata ( (char*) &fztn, sizeof(fztn));
   
      if (fztn.name)
         mainstream.writepchar( fztn.name );
      if (fztn.description)
         mainstream.writepchar( fztn.description );
      if (fztn.infotext)
         mainstream.writepchar( fztn.infotext );
      for (i=0; i<8; i++)
         if ( fztn.classnames[i] )
            mainstream.writepchar( fztn.classnames[i] );
   
      for (i=0;i<8  ;i++ ) 
         if ( fztn.picture[i] ) 
            mainstream.writedata ( (char*) fztn.picture[i], tanksize );
   
      if (fztn.picture[1] == NULL)
          fztn.picture[1] = fztn.picture[0];
   
      loaderror = mainstream.getstatus (); 
      mainstream.closestream(); 



      printf( "%s written \n",fileinfo.name );

      
      rc = _dos_findnext( &fileinfo );
  }

  mainstream.done();

}
