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
#include <string.h>

#include "..\tpascal.inc"
#include "..\typen.h"
#include "..\basegfx.h"
#include "..\loadpcx.h"
#include "..\sgstream.h"
#include "..\misc.h"
#include "krkr.h"

#ifdef HEXAGON
 #include "..\loadbi3.h"
#endif


char     richtung[2][11]  = {"0 ø", "45 ø"}; 
char     mode50 = 1;
#define  la   80

int i;

int usegraphics = 1;


const char* bildnr[8] = { " 0   ( unit facing up ) ",
                          " 1   ( unit facing up and right ) ",
                          " 2   ( unit facing right ) ",
                          " 3   ( unit facing right and down ) ",
                          " 4   ( unit facing down ) ",
                          " 5   ( unit facing down and left ) ",
                          " 6   ( unit facing left ) ",
                          " 7   ( unit facing left and up ) " };


void *       loadpcx2(char *       filestring, int autosize );



void readtextfile ( char* name, pchar &buf, int allocated )
{
   int size = filesize ( name );
   if (!allocated)
     buf = (char*) malloc ( size+1 );

   char *p1, p2;
   p1 = buf;

   tnfilestream mainstream ( name , 1);
   for (int i = 0; i < size ; i++ ) {
      mainstream.readdata ( &p2, 1);
      if (p2 == 0x0D) {
        mainstream.readdata ( &p2, 1);
        i++;
      }
      *p1 = p2;
      p1++;
   } /* endfor */
   *p1 = 0;
}



main (int argc, char *argv[] )
{ 
   char cl_filename[1000];
   cl_filename[0] = 0;

   for ( int i = 1; i<argc; i++ ) 
      if ( argv[i][0] == '/'  ||  argv[i][0] == '-' ) {
         if ( strcmpi ( &argv[i][1], "nographics" ) == 0 ) 
            usegraphics = 0;
      } else
         strcpy ( cl_filename, argv[i] );

   t_carefor_containerstream cfcst;
   try {

      pvehicletype   ft;
      tfile          datfile;
      char           pict = YES;       // Bilder einlesen
      char           dif = 1;          // Anzahl der Bilder
   
      char           maxmovement = 0;
    
   
      loadpalette();
      loadbi3graphics();
   
      ft = new tvehicletype;

      strcpy (datfile.name, "");
   
      if (mode50) settxt50mode ();
      _settextcolor (7);
      _setbkcolor (0);
      clearscreen ();
      char   creat_edit ;
      if ( !cl_filename[0] ) {
         printf ("\n    Create a new unit or edit an existent one\n");
         creat_edit = 0;
         yn_switch (" Create ", " Edit ", 0, 1, creat_edit);
      } else
         creat_edit = 1;
   
      if (creat_edit==0) {                                            // creat new tank
         clearscreen ();
         printf ("\n    enter filename (without extension)\n");
         stredit2 (datfile.name, 9, 255,255);
         strcat (datfile.name, ".veh");
         if (exist(datfile.name)) {
            clearscreen(); 
            _settextcolor (15);
            _setbkcolor (red);
            _settextposition (4, 5);
            char s[100];
            sprintf ( s, " !! \a Attention :  Filename <%s> already exists !! \n", datfile.name );
            _outtext ( s );
            _wait ();
            // settextmode ( 3 );
            exit (0);
         } 
   
      } 
      else {                                                         // edit existing tank
   
         clearscreen ();
         if ( !cl_filename[0] ) 
            fileselect("*.veh", _A_NORMAL, datfile);
         else
            strcpy ( datfile.name, cl_filename );
   
         clearscreen ();
         ft = loadvehicletype(datfile.name);
         if (ft->picture[0] == ft->picture[1]) { 
            ft->picture[1] = NULL;
            dif = false; 
         } 
         else 
            dif = true; 
   
         printf ("\n    Change picture ? \n");
         yn_switch ("", "", YES, NO, pict);
         
      } 
   
   
      if (pict) {
         tfile    pictfile;
   
         char  ok=1;
         do { 
   
             char battleisle = 1;
             printf ("\n    use pictures of Battle Isle or own picture ?\n");
             yn_switch ("BI", "seperate picture" , 1, 0, battleisle);
             if ( battleisle ) {
                if ( usegraphics ) {
                   initgraphics (640, 480, 8);
                   getbi3pict_double ( &ft->bipicture, &ft->picture[0] ); 
                } else {
                   printf("\nSorry, you need graphics to select a BI picture !\n");
                   return 0;
                }
             } else {    
                char autosize = 0;
                printf ("\n    use standard image size, or detect the size automatically ?\n"
                        "If the unit has the standard size, because it was painted in hexagon.pcx\n"
                        "which is part of the ASC editor package, then use the standard size,\n"
                        "since it provides better image quality. The automatic size detection\n"
                        "has a tolerance of +- 1 pixel, so it may scale the image even if it\n"
                        "wasn't necessary.\n");


                yn_switch ("standard size", "automatic size detection" , 0, 1, autosize);

                fileselect ("*.PCX", _A_NORMAL, pictfile);
                                          
                if ( usegraphics ) {
                   initgraphics (640, 480, 8);
                   ft->picture[0] = loadpcx2(pictfile.name, autosize);
                } else {
                   tvirtualdisplay vd ( 640, 480, 255 );
                   ft->picture[0] = loadpcx2(pictfile.name, autosize);
                }

                ft->bipicture = -1;
                for ( int i = 1;  i < 8; i++ )
                   ft->picture[i] = NULL;
             }

             settxt50mode (); 
     
             printf ("\n    continue ?");
             yn_switch (" YES, Go on ", " NO, repeat pictureselection ", 1,0, ok);
         }  while (!ok);
        
      } else 
         if ( usegraphics ) { 
             void     *p, *q;
             initgraphics (640, 480, 8);
             setvgapalette256(pal);
             p = ft->picture[0]; 
             q = ft->picture[1]; 
       
             if (q == NULL)
                q = p; 
       
             for (int i = 0; i <= 8; i++) {
                putrotspriteimage(la + 50 * (i),50,p,i * 8); 
                putrotspriteimage(la + 50 * (i),100,q,i * 8); 
       
                putrotspriteimage90(la + 50 * (i),150,p,i * 8); 
                putrotspriteimage90(la + 50 * (i),200,q,i * 8); 
       
                putrotspriteimage180(la + 50 * (i),250,p,i * 8); 
                putrotspriteimage180(la + 50 * (i),300,q,i * 8); 
       
                putrotspriteimage270(la + 50 * (i),350,p,i * 8); 
                putrotspriteimage270(la + 50 * (i),400,q,i * 8); 
             } 
       
             _wait();
       
             settxt50mode (); 
             printf ("\n    Continue ? \n");
             char  c=1;
             yn_switch (" YES ", " NO, exit ", 1,0, c);
             if (c==0) 
                exit(0);
          } 
   
                   
   
      {
         printf ("\n    ID :  The ID has to be a unique number. For creating new units\n"
                 "            you should check with tankid.exe which IDs are not used\n");
         num_ed ( ft->id , 0, 65534);
   
         printf ("\n    Description    Example: Main Battle Tank ; transport plane ; .... \n");
         stredit (ft->description, 21, 255,255);
   
         printf ("\n    Name           Example: Samurai 3 ; Crux ; ... \n");
         stredit (ft->name, 21, 255,255);
   
         printf ("\n    Include an infotext for the unit ? \n"
                 "             This infotext may be in greater detail and can only be loaded\n"
                 "             from textfiles. Please read the section about textfile in the \n"
                 "             file editors.txt before including any texts here\n");
   
         char text = 0;
         yn_switch ("YES", "NO", 1,0, text);
         if ( text ) {
   
            tfile    pictfile;
            fileselect ("*.txt", _A_NORMAL, pictfile);
            readtextfile ( pictfile.name, ft->infotext, 0 );
            printf( ft->infotext );
            fflush ( stdout );
            _wait();
         }
   
   
         printf ("\n    production cost : \n");
         printf ("\n       material :\n");
         num_ed (ft->production.material, 0, 65535);
         printf ("\n       energy  (%i) :\n", ft->production.material / 10 * 8);
         num_ed (ft->production.energy, 0, 65535);
   
   
         printf ("\n    Armor  ");
   
         num_ed (ft->armor, 0, 65535);
   
         printf ("\n    Levels of height the unit can enter \n"
                 "\n           ( no gaps allowed !!) \n");
         int height = ft->height;
         bitselect ( height, choehenstufen, choehenstufennum);
         ft->height = height;
   
         printf ("\n    Movement  (note that one field with road has %d movement points)", minmalq);
         for (i = 0; i <= 7; i++) 
            if (ft->height & (1 << i) ) {
               printf ("%s   :", choehenstufen[i]);
               num_ed (ft->movement[i], 0, 255);
               if (maxmovement < ft->movement[i]) 
                  maxmovement = ft->movement[i]; 
            }
   
         printf ("\n    category : ");
         for (i=0; i<cmovemalitypenum; i++) {
            printf ("\n %i.)  %s ", i, cmovemalitypes[i]);
         } /* endfor */
         num_ed (ft->movemalustyp, 0, cmovemalitypenum-1);
   
   
         terrainaccess_ed ( ft->terrainaccess, "vehicle" );
   
   
         {
            int n = 0;
            for ( int i = 0; i < 8; i++ )
               if ( ft->height & ( 1 << i ))
                  n++;
   
             if ( n > 1 ) {
                printf ("\n    distance the unit needs to change height");
                int ascend = ft->steigung;
                num_ed (ascend, 0, 255);
                ft->steigung = ascend;
             }
         }
   
   
         printf ("\n    wait after movement (to attack) \n");
         yn_switch ("","", 1,0,  ft->wait);
   
         printf ("\n    weight  ");
         num_ed (ft->weight, 0, 32000);
                                                                                    
         printf ("\n    max load         all loaded units together; not the size of the largest\n"
                 "                       unit to load \n");
         num_ed (ft->loadcapacity, 0, 32000 );
   
         if (ft->loadcapacity > 0) {
            printf ("\n    maximum weight of a single loadable unit \n ");
   
            if ( ft->maxunitweight <= 0 )
               ft->maxunitweight = ft->loadcapacity;
   
            num_ed (ft->maxunitweight, 0, 32000 );
   
            printf ("\n    units that can be loaded have to be on this height: \n");
            bitselect (ft->loadcapability, choehenstufen, choehenstufennum);
   
            printf ("\n    units that can be loaded have to be ABLE to be on these levels of height :"
                    "           example: aircraft carrier can only transport aircraft, although\n "
                    "           they are landed on it (height=driving) \n");
            bitselect (ft->loadcapabilityreq, choehenstufen, choehenstufennum);
   
            printf ("\n    units that can be loaded MUST NOT to be able to be on these levels of height ,\n"
                    "          example: aircrafts must not be loaded into an transport plane\n");
            bitselect (ft->loadcapabilitynot, choehenstufen, choehenstufennum);
   
            printf ("\n    these vehicle categories can be loaded \n"
                    "          This vehicle property is not evaluated, but will replace the system\n"
                    "          checking the levels of height in some future version\n");
            bitselect (ft->vehicleCategoriesLoadable, cmovemalitypes, cmovemalitypenum );


         } 
   
   
         printf ("\n    fuel consumption \n");
         num_ed (ft->fuelConsumption, 0, 65534);
                                                                                     
         printf ("\n    fuel tank    \n ");
         num_ed (ft->tank, 0, maxint);
                                                                       
                                    
         printf ("\n    energy tank  ( ONLY for generators !!!!! Every other unit must have a energy tank of 0)  : ");
         num_ed (ft->energy, 0, maxint); 
   
         printf ("\n    material tank   : \n"); 
         num_ed (ft->material, 0, maxint );
   
         clearscreen(); 
         printf ("\n    edit weapons ? : \n");
         char  muned = creat_edit;
         yn_switch (" Edit ", " Skip ", 0, 1, muned);
   
         if (muned==0) {
            printf ("\n    number of weapons :\n");
            num_ed (ft->weapons->count,0,16);
            clearscreen(); 
   
            i = 0; 
            while (i != ft->weapons->count) { 
               clearscreen(); 
   
               printf ("\n    Select %i. weapon :\n", i+1);
               bitselect (ft->weapons->weapon[i].typ, cwaffentypen, cwaffentypennum);
   
               printf ("\n    aims :\n");
               bitselect (ft->weapons->weapon[i].targ, choehenstufen, choehenstufennum);
   
               do { 
                  printf ("\n    the weapon can be shot from ... :\n");
                  bitselect (ft->weapons->weapon[i].sourceheight, choehenstufen, choehenstufennum);
               }  while (! (ft->weapons->weapon[i].sourceheight & ft->height )); 
   
               printf ("\n    max distance : \n");
               num_ed (ft->weapons->weapon[i].maxdistance, 0, 255);
   
               printf ("\n    min distance : \n");
               if (creat_edit==0) ft->weapons->weapon[i].mindistance=8;
               num_ed (ft->weapons->weapon[i].mindistance, 0, ft->weapons->weapon[i].maxdistance);
   
               printf ("\n    strength at min distance " );
               num_ed (ft->weapons->weapon[i].maxstrength, 0, maxint);
   
               printf ("\n    strength at max distance  :\n");
               num_ed (ft->weapons->weapon[i].minstrength, 0, maxint);
   
   
               if (ft->weapons->weapon[i].minstrength==0)
                  ft->weapons->weapon[i].minstrength = ft->weapons->weapon[i].maxstrength /2;
   
               printf ("\n    ammo  \n" );
               num_ed (ft->weapons->weapon[i].count, 0, 255);


               printf ("\n   the current efficency settings are:");
               for ( int j = 0; j < 13; j++ )
                  printf(" %d%% ", ft->weapons->weapon[i].efficiency[j] );

                char change = 0;
                printf ("\n   change them ?\n");

                yn_switch ("yes", "no", 1, 0, change);

                if ( change ) {
                   printf("\n remember that floating and ground level count as same height !\nnegative height differences: shooting 'down', positive is shooting 'up'\n");
                   for ( int k = 0; k < 13; k++ ) {
                      printf(" \n height difference = %d \n", k - 6 );
                      num_ed ( ft->weapons->weapon[i].efficiency[k], 0, 100);
                   }
                }

               printf ("\n    The weapon can NOT hit :\n");
               bitselect (ft->weapons->weapon[i].targets_not_hittable, cmovemalitypes, cmovemalitypenum);

               i++;
            } 
         } 
         printf ("\n    initiative ( > 0 :better assault /  < 0 better defense ):\n");
         num_ed (ft->initiative,-127, 127);
   
         printf ("\n    functions (if the unit has different classes: max functions): \n");
         bitselect (ft->functions, cvehiclefunctions, cvehiclefunctionsnum);
   
         printf ("\n    autorepairrate \n " );
         num_ed (ft->autorepairrate, 0, 100);

         if (ft->functions & (cfautodigger | cfmanualdigger)) {
         	  printf ("\n    radius to check for resources : \n The radius is specified in 'number of fields', not distance !\n For a radius of 5 fields enter 5 here.\n");
   	  num_ed (ft->digrange, 0,255);
         } else ft->digrange=0;
   
         int radius;
         if (ft->weapons->count>0) 
   	 radius = 16200/ft->weapons->weapon[0].maxdistance/(ft->weapons->weapon[0].maxstrength+1);
         else
            radius =1;
   
         printf ("\n    view  \n" );
         num_ed (ft->view, 0, 255);
                   
         int jamm = ft->jamming;
         printf ("\n    jamming  \n" );
         num_ed (jamm, 0, 255);
         ft->jamming = jamm;
      } 
   
      clearscreen ();
   

      printf ("\n     number of classes   : ");
      num_ed (ft->classnum, 0, 8);
   
   
      char standrt = (creat_edit==0);
      if ( ft->classnum == 0 ) 
         printf ("\n     should the standard research values be used ? \n");
      else
         printf ("\n     should the standard research values be used for class 0 ? \n");
   
      yn_switch ("", "", 1, 0, standrt);
   
      int start = 0;
   
      if (standrt) {
         for ( i=0; i< waffenanzahl ;i++ ) 
            ft->classbound[0].weapstrength[i] = 1024;
         ft->classbound[0].armor = 1024;
         for ( i=0; i< 4; i++)
            ft->classbound[0].techrequired[i] = 0;
         ft->classbound[0].eventrequired = 0;
         ft->classbound[0].techlevel = 0;
         start = 1;
      } else
         start = 0;
   
      if ( ft->classnum || start == 0 ) {
         if (start == 1) {
            printf("\n      name of base class :\n");
            stredit ( ft->classnames[0], 60, -1, -1 );
   
            ft->classbound[0].vehiclefunctions = ft->functions;
            printf("\n      functions of %s  \n"
                   "               it is OK to set ALL bits here ...\n", ft->classnames[0]);
   
            bitselect(ft->classbound[0].vehiclefunctions, cvehiclefunctions, cvehiclefunctionsnum);
         }
         int end;
         if ( ft->classnum == 0 )
            end = 0;
         else
            end = ft->classnum-1;
   
         for (i=start; i<= end ; i++) {
            ft->classbound[i].vehiclefunctions = ft->functions;
            printf("\n      name of %. class :\n",i);
            stredit ( ft->classnames[i], 60, -1, -1 );
   
            printf("\n      parameters of  %s class  \n", ft->classnames[i] );
            for ( int j=0; j< waffenanzahl ;j++ ) {
               int num = 0;
               for ( int k = 0; k < ft->weapons->count; k++) 
                  if ( (ft->weapons->weapon[k].typ & cwweapon) == (1 << j) )
                     num++;
               if (num > 0) {
                  printf("\n      needed weapon improvement for %s \n     ( base is 1024; 2048 would be doubled strength requiered )\n", cwaffentypen[j] );
                  num_ed ( ft->classbound[i].weapstrength[j] , 1024, 65530 );
               } else
                  ft->classbound[i].weapstrength[j] = 1024;
            }
            printf("\n     needed armor improvement : \n ");
            num_ed ( ft->classbound[i].armor, 1024, 65530 );
   
            printf("\n     IDs of required technologies  ( 0 to continue ) : \n ");
            j = 0;
            do {
               num_ed ( ft->classbound[i].techrequired[j] , 0, 65500 );
               j++;
            } while ( (j < 4) && ft->classbound[i].techrequired[j-1]   ); /* enddo */
   
            {
               char c = ft->classbound[i].eventrequired;
               printf("\n    is an event required ? \n"
                      "         usually NO, because this event has to be set in EVERY map where\n"
                      "         the delopment of this class should be possible\n" );
                       
               yn_switch ("", "", 1, 0, c );
               ft->classbound[i].eventrequired = c;
            }
   
            printf("\n    Techlevel : \n");
            num_ed ( ft->classbound[i].techlevel , 0, 255 );
    
            printf("\n      functions \n ");
            bitselect(ft->classbound[i].vehiclefunctions, cvehiclefunctions, cvehiclefunctionsnum);
   
         } /* endfor */
   
      }           
   
      if ( ft->height & (chschwimmend | chfahrend) ) {
         printf("\n     max. windspeed on water that can be survived  \n" );
         num_ed ( ft->maxwindspeedonwater, 0, 255 );
      }
   
   
      {  
   
         dynamic_array<pobjecttype> obj;
         dynamic_array<pchar> name;
   
         int objectlayernum = 0; 
       
         tfindfile ff ( "*.obl" );
   
         char *c = ff.getnextname();
       
         while ( c ) {
             obj[objectlayernum] = loadobjecttype( c ); 
             name[objectlayernum] = strdup ( c );
             objectlayernum++;
             c = ff.getnextname();
          }
       
          printf("\n\n\n    the unit can build the following items of the object layers : \n\n");
          int* pi = new int[100];
          memset ( pi, 0, 400 );
          for ( i = 0; i < ft->objectsbuildablenum; i++ ) {
             for ( int j = 0; j < objectlayernum; j++ )
                if ( obj[j]->id == ft->objectsbuildableid[i] )
                   printf("           %s\n", name[j] );
   
             pi[i] = ft->objectsbuildableid[i];
          }
          pi[i] = 0;
   
          ft->objectsbuildableid = pi;
   
   
          char m = 0;
          printf("     Change ?\n");
   
          yn_switch ("no", "yes", 0, 1, m);
          if ( m ) {
             int num = 0;
             do {
               printf("\n\n\n\n");
               for ( int j = 0; j < objectlayernum; j++ ) {
                  printf("%3i %28s |    ", obj[j]->id, name[j]);
                  if (j % 2 ==0) printf("\n");
               }
   
               printf (" enter id of object  ( 0 to continue ): ");
               num_ed (ft->objectsbuildableid[num],minint,maxint);
               num++;
             } while ( ft->objectsbuildableid[num-1] ); /* enddo */
             ft->objectsbuildablenum = num-1;
   
          }
   
      }
      if ( ft->functions & cfvehicleconstruction ) {  
         loadguipictures();

         dynamic_array<pvehicletype> tnk;
         dynamic_array<pchar> name;
   
         int vehiclenum = 0; 

          {
             tfindfile ff ( "*.veh" );
       
             char *c = ff.getnextname();
           
             while ( c ) {
                 tnk[vehiclenum] = loadvehicletype( c ); 
                 name[vehiclenum] = strdup ( c );
                 vehiclenum++;
                 c = ff.getnextname();
             }
          }
       

          printf("\n\n\n    the unit can build the following vehicles : \n\n");
          int* pi = new int[100];
          memset ( pi, 0, 400 );
          for ( i = 0; i < ft->vehiclesbuildablenum; i++ ) {
             for ( int j = 0; j < vehiclenum; j++ )
                if ( tnk[j]->id == ft->vehiclesbuildableid[i] )
                   printf("           %s\n", name[j] );
   
             pi[i] = ft->vehiclesbuildableid[i];
          }
          pi[i] = 0;
   
          ft->objectsbuildableid = pi;
   
   
          char m = 0;
          printf("     Change ?\n");
   
          yn_switch ("no", "yes", 0, 1, m);
          if ( m ) {
             int num = 0;
             do {
               printf("\n\n\n\n");
               for ( int j = 0; j < vehiclenum; j++ ) {
                  printf("%3i %10s | ", tnk[j]->id, name[j]);
                  if (j % 4 ==3) printf("\n");
               }
   
               printf (" \n enter id of vehicle ( 0 to continue ): ");
               num_ed (ft->vehiclesbuildableid[num],0,100000);
               num++;
             } while ( ft->vehiclesbuildableid[num-1] ); /* enddo */
             ft->vehiclesbuildablenum = num-1;
   
          }
   
      }
   

      if ( ft->functions & cfspecificbuildingconstruction ) {
         loadguipictures();
         printf( "The buildings with to following IDs are buildable: \n\n");


         for ( int i = 0; i < ft->buildingsbuildablenum; i++ )
            if ( ft->buildingsbuildable[i].from == ft->buildingsbuildable[i].to )
               printf(" %d \n", ft->buildingsbuildable[i].from );
            else
               printf(" %d - %d \n", ft->buildingsbuildable[i].from, ft->buildingsbuildable[i].to );

         char m = 0;
         printf("     Change buildings buildable?\n");
   
         yn_switch ("no", "yes", 0, 1, m);

         clearscreen ();
         if ( m ) {
   
            dynamic_array<pbuildingtype> bld;
            dynamic_array<pchar> name;
      
            int buildingnum = 0; 
   
            {
                tfindfile ff ( "*.bld" );
          
                char *c = ff.getnextname();
              
                while ( c ) {
                    bld[buildingnum] = loadbuildingtype( c ); 
                    name[buildingnum] = strdup ( c );
                    buildingnum++;
                    c = ff.getnextname();
                 }
             }
   
             printf("\n\nNote: you may enter single ids ( '25' ) or ranges ('1 - 999');\n");
   
   
             pbuildrange ids = new tbuildrange[1000];
             for ( int i = 0; i < ft->buildingsbuildablenum; i++ )
                ids[i] = ft->buildingsbuildable[i];
             int num = 0;
   
             char s[100];
             char d;
             do {
                printf("\n\n\nbuildings available\n");
                for ( int j = 0; j < buildingnum; j++ ) {
                   printf("%3i %10s | ", bld[j]->id, name[j]);
                   if (j % 4 ==3) printf("\n");
                }
      
                if ( num < ft->buildingsbuildablenum ) {
                    if ( ids[num].from == ids[num].to )
                       sprintf ( s, "%d", ids[num].to );
                    else
                       sprintf ( s, "%d - %d", ids[num].from, ids[num].to );
                } else
                    s[0] = 0;
   
                stredit2 ( s, 21, 255,255);
                d = s[0];
   
                if ( s[0] ) {
                   int from;
                   int to;
                   char* m = strchr ( s, '-' );
                   if ( m ) {
                      char * d = strtok ( s, " -" );
                      from = atoi ( d );
                      d = strtok ( NULL, " -" );
                      to = atoi ( d );
                   } else {
                      char * d = strtok ( s, " " );
                      from = atoi ( d );
                      to = from;
                   }
                   ids[num].from = from;
                   ids[num].to   = to;
                   num++;
                }
   
             } while ( d ); /* enddo */
   
             ft->buildingsbuildable = ids;
             ft->buildingsbuildablenum = num;
          }
      }


      {
         tn_file_buf_stream mainstream ( datfile.name, 2 );
         writevehicle( ft, &mainstream );
      }

   } /* endtry */
   catch ( tfileerror err ) {
      printf("\nfatal error accessing file %s \n", err.filename );
      return 1;
   } /* endcatch */
   catch ( terror ) {
      printf("\na fatal exception occured\n" );
      return 2;
   } /* endcatch */
      
   clearscreen(); 

   if (mode50) _setvideomode (_TEXTC80);
   return 0;
}



// #define OLDSIZE

int searchline ( int x1, int y1, int x2, int y2 )
{
   if ( x1 == x2 ) {
      for ( int y = y1; y <= y2; y++ )
         if ( getpixel ( x1, y ) != 255 )
            return 1;
         
      return 0;
   } else {
      for ( int x = x1; x <= x2; x++ )
         if ( getpixel ( x, y1 ) != 255 )
            return 1;
         
      return 0;
   }
}


void *       loadpcx2(char *       filestring, int autosize)
{      
  void         *p = NULL;
  int b; 


   bar ( 0, 0, 639, 479, 255 );
   b = loadpcxxy(filestring, 1, 0,0); 
   if (b == 0) { 
      if ( !autosize ) {
         p = malloc( 10000 ); 
         #ifndef OLDSIZE
         getimage(0,0,fieldsizex-1,fieldsizey-1,p); 
         rectangle ( 0, 0, fieldsizex-1,fieldsizey-1, blue );
         #else
         getimage(0,0,unitsizex,unitsizey,p); 
         rectangle ( 0, 0, unitsizex,unitsizey, blue );
         #endif
      } else {
         int maxsize = 100;
         
         int minx = 0;
         while ( !searchline ( minx, 0, minx, maxsize ))
            minx++;
      
         int miny = 0;
         while ( !searchline ( 0, miny, maxsize, miny ))
            miny++;
      
         int maxx = maxsize;
         while ( !searchline ( maxx, 0, maxx, maxsize ))
            maxx--;
      
      
         int maxy = maxsize;
         while ( !searchline ( 0, maxy, maxsize, maxy ))
            maxy--;

         int xsize = maxx+minx;
         int ysize = maxy+miny;

         p = malloc( imagesize ( 0, 0, xsize, ysize )); 
         getimage(0,0, xsize, ysize, p); 
         rectangle ( 0, 0, xsize, ysize, blue );

      }
      _wait(); 
   } 
   return p; 

} 


