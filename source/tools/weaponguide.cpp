/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel, Marc Schellenberger and
    Steffen Froehlich

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
// Version v1.0 , change also GENERAL


#include <stdio.h>

#include "../tpascal.inc"
#include "../typen.h"
#include "../basestrm.h"
#include "../misc.h"
#include "../sgstream.h"
#include "../loadbi3.h"
#include "../buildingtype.h"
#include "../vehicletype.h"
#include "../errors.h"
#include "../graphicset.h"
#include "../ascstring.h"

int main(int argc, char *argv[] )
{
   // mounting all container files
      opencontainer ( "*.con" );

   try {
                  
      loadpalette();
      loadbi3graphics();

   
      char* wildcard;
   
      if ( argc == 2 ) {
         wildcard = argv[1];
         // if a command line parameter is specified, use it as wildcard
         // for example: weaponguide s*.veh
      } else {
         wildcard =  "*.veh";
         // else use *.veh
      }
      
      FILE* overview = fopen ( "overview.html", "w" );
      FILE* overview1 = fopen ( "overview1.html", "w" );
      // opens a file for writing and assigns the file pointer overview to it
      
      // Beginn des HTML Files HEAD und BODY
      //  \n is the sequence to start a new line
      
      fprintf ( overview , "<html>\n"
                           "<HEAD>\n" 
                           "<TITLE>UNITGUIDE OVERVIEW LEFT</TITLE>\n"
                           "<base target=\"base\">\n"
                           "<LINK REL=\"stylesheet\" TYPE=\"text/css\" HREF=\"../../asc/asc.css\">\n"
                           "</HEAD>\n"
                           "\n"
                           "<BODY bgcolor=\"#447744\" text=\"#ffffff\" background=\"../../bilder/asc-hin3.gif\" leftmargin=\"5\" topmargin=\"2\">\n"
                           "<table width=\"100%\" border=\"1\" bordercolordark=\"#333333\" bordercolorlight=\"#408050\" cellpadding=\"1\" cellspacing=\"1\">\n"
                           "<tr><td><a href=\"overview1.html\">SEE PICTURES</a></td></tr><tr><td></td></tr>\n" );

      fprintf ( overview1 , "<html>\n"
                           "<HEAD>\n" 
                           "<TITLE>WEAPONGUIDE OVERVIEW PICTURES</TITLE>\n"
                           "<base target=\"base\">\n"
                           "<LINK REL=\"stylesheet\" TYPE=\"text/css\" HREF=\"../../asc/asc.css\">\n"
                           "</HEAD>\n"
                           "\n"
                           "<BODY bgcolor=\"#447744\" text=\"#ffffff\" background=\"../../bilder/asc-hin3.gif\" leftmargin=\"5\" topmargin=\"0\">\n" );
      
      tfindfile ff ( wildcard );
      ASCString cn = ff.getnextname();
      while( !cn.empty() ) {
         cn.toLower();
      	 // now we are cycling through all files that match wildcard

         string s, s1, s2, s3, s4, s5, b1, b2;
         s = s1 = s2 = s3 = s4 = s5 = b1 = b2 = cn;
         
         // this is a C++ string which is much more powerful than the standard C strings ( char* )

         s.replace ( s.find ("veh"), 3, "html");
         s1.replace ( s1.find (".veh"), 4, "1.html");
         s2.replace ( s2.find (".veh"), 4, "2.html");
         s3.replace ( s3.find (".veh"), 4, "3.html");
         s4.replace ( s4.find (".veh"), 4, "4.html");
         s5.replace ( s5.find (".veh"), 4, "5.html");
         b1.replace ( b1.find ("veh"), 3, "gif");
         b2.replace ( b2.find ("veh"), 3, "jpg");
         // we are replacing the 3 characters "veh" of the original string by "html"
         
         FILE* detailed = fopen ( s.c_str(), "w" );
         FILE* detailed1 = fopen ( s1.c_str(), "w" );
         FILE* detailed2 = fopen ( s2.c_str(), "w" );
         FILE* detailed3 = fopen ( s3.c_str(), "w" );
         FILE* detailed4 = fopen ( s4.c_str(), "w" );
         FILE* detailed5 = fopen ( s5.c_str(), "w" );
         // c_str() converts a c++ string back to a c string which fopen requires
         
         
         // Beginn Einzelfiles   
         // UNIT FRAME
         fprintf ( detailed, "<html>\n"
                             "<HEAD>\n" 
                             "<TITLE>UNITGUIDE FRAME</TITLE>\n"
                             "<frameset  rows=\"207,*\" border=0 >\n"
                             "<frame name=\"over\" src=\"%s\" marginheight=\"0\">\n"
                             "<frame name=\"under\" src=\"%s\" marginheight=\"2\">\n" 
                             "<noframes><body><p>Diese Seite verwendet Frames. Frames werden von Ihrem Browser aber nicht unterstützt.</p></body></noframes>\n"
                             "</frameset>\n"
                             "</html>\n", s1.c_str() , s5.c_str() );

         // UNIT GENERAL
         fprintf ( detailed1, "<html>\n"
                              "<HEAD>\n" 
                              "<TITLE>UNITGUIDE GENERAL</TITLE>\n"
                              "<base target=\"under\"> \n"
                              "<LINK REL=\"stylesheet\" TYPE=\"text/css\" HREF=\"../../asc/asc.css\">\n"
                              "</HEAD>\n"
                              "\n"
                              "<BODY bgcolor=\"#447744\" text=\"black\" background=\"../../bilder/asc-hin2.gif\">\n"
                              );

         // UNIT TERRAIN
         fprintf ( detailed2, "<html>\n"
                              "<HEAD>\n" 
                              "<TITLE>UNITGUIDE MOVEMENT</TITLE>\n"
                              "<base target=\"under\"> \n"
                              "<LINK REL=\"stylesheet\" TYPE=\"text/css\" HREF=\"../../asc/asc.css\">\n"
                              "</HEAD>\n"
                              "\n"
                              "<BODY bgcolor=\"#447744\" text=\"black\" background=\"../../bilder/asc-hin2.gif\">\n" );

         // UNIT WEAPONS
         fprintf ( detailed3, "<html>\n"
                              "<HEAD>\n" 
                              "<TITLE>UNITGUIDE TERRAIN</TITLE>\n"
                              "<base target=\"under\"> \n"
                              "<LINK REL=\"stylesheet\" TYPE=\"text/css\" HREF=\"../../asc/asc.css\">\n"
                              "</HEAD>\n"
                              "\n"
                              "<BODY bgcolor=\"#447744\" text=\"black\" background=\"../../bilder/asc-hin2.gif\">\n" );

         fprintf ( detailed4, "<html>\n"
                              "<HEAD>\n" 
                              "<TITLE>UNITGUIDE LOADING</TITLE>\n"
                              "<base target=\"under\"> \n"
                              "<LINK REL=\"stylesheet\" TYPE=\"text/css\" HREF=\"../../asc/asc.css\">\n"
                              "</HEAD>\n"
                              "\n"
                              "<BODY bgcolor=\"#447744\" text=\"black\" background=\"../../bilder/asc-hin2.gif\">\n" );

         fprintf ( detailed5, "<html>\n"
                              "<HEAD>\n" 
                              "<TITLE>UNITGUIDE DESCRIPTION</TITLE>\n"
                              "<base target=\"under\"> \n"
                              "<LINK REL=\"stylesheet\" TYPE=\"text/css\" HREF=\"../../asc/asc.css\">\n"
                              "</HEAD>\n"
                              "\n"
                              "<BODY bgcolor=\"#447744\" text=\"black\" background=\"../../bilder/asc-hin2.gif\">\n" );
        
         pvehicletype  ft = loadvehicletype( cn.c_str() );
         // we are loading the vehicletype with the name in cn

// OVERVIEW LEFT   
         fprintf ( overview, " <tr><td><A HREF=\"%s\">%s</A></td></tr>\n", s.c_str(), ft->name );
 
// OVERVIEW RIGHT 
         fprintf ( overview1, "<table align=\"center\" border=\"1\" bordercolordark=\"#333333\" bordercolorlight=\"#408050\" cellpadding=\"1\" cellspacing=\"1\">\n"
                              " <tr><td rowspan=\"2\" width=\"50\">" );
         if ( exist ( b1.c_str() ))
            fprintf ( overview1, "<img src=\"%s\" border=\"0\">", b1.c_str() ); 
         fprintf ( overview1, "</td><td width=\"100\"><A HREF=\"%s\">%s</A></td></tr><tr><td><a href=\"%s\">%s</a></td></tr></table>\n", s.c_str(), ft->name, s.c_str(), ft->description );

// END OVERVIEW RIGHT
         
         // we are adding a link to the overview file.
         // to put a singile " into a string we must use double quotes ( "" ), because a single quote is interpreted as the end of the string by C
         // %s tells C to insert a string there. The strings are appended at the end of the command
         // at the first %s the filename in s is inserted, at the second %s the unit variable 'description'
         
// UNIT GENERAL
         fprintf ( detailed1, "<table width=\"100%\" id=\"H2\" border=\"1\" bordercolordark=\"#333333\" bordercolorlight=\"#408050\" cellpadding=\"1\" cellspacing=\"1\"> \n"
                              "<tr><td colspan=\"2\"></td><td id=\"H9\" align=\"right\">UNIT GUIDE v1.0 </td></tr>"
                              "<tr><td width=\"50\">" );
         if ( exist ( b1.c_str() ))
             fprintf ( detailed1, "<img src=\"%s\">", b1.c_str() );
         fprintf ( detailed1, "</td>\n<td>" );
                 
         fprintf ( detailed1, "<table id=\"H2\" border=\"1\" bordercolordark=\"#333333\" bordercolorlight=\"#408050\" cellpadding=\"1\" cellspacing=\"1\"> \n" );
         fprintf ( detailed1, "<tr><td bgcolor=\"#20483f\">Name</td>         <td align=\"center\" colspan=\"4\">%s</td> </tr>\n", ft->name );
         fprintf ( detailed1, "<tr><td bgcolor=\"#20483f\">Description</td>  <td align=\"center\" colspan=\"4\">%s</td> </tr>\n", ft->description );
         fprintf ( detailed1, "<tr><td bgcolor=\"#20483f\">ID</td>           <td align=\"center\">%i</td> <td></td><td bgcolor=\"#20483f\">Weight</td> <td align=\"center\">%d</td> </tr>\n", ft->id, ft->weight );
         fprintf ( detailed1, "<tr><td bgcolor=\"#20483f\">Armor</td>        <td align=\"center\">%i</td> <td></td><td bgcolor=\"#20483f\">View</td>   <td align=\"center\">%d</td> </tr>\n", ft->armor, (ft->view/10) );
         fprintf ( detailed1, "<tr><td bgcolor=\"#20483f\">Cost energy</td>  <td align=\"center\">%i</td> <td></td><td bgcolor=\"#20483f\">Jamming</td><td align=\"center\">%d</td> </tr>\n", ft->productionCost.material, (ft->jamming/10) );
         fprintf ( detailed1, "<tr><td bgcolor=\"#20483f\">Cost material</td><td align=\"center\">%i</td> </tr>\n", ft->productionCost.energy );
         // Abfrage noch unbefriedigend !!
         fprintf ( detailed1, "<tr><td bgcolor=\"#20483f\" colspan=\"4\">Attack after move</td> <td>%d</td> </tr>\n", ft->wait );
         // Abfrage für MAM funktioniert nicht
         fprintf ( detailed1, "<tr><td bgcolor=\"#20483f\" colspan=\"4\">Move after Attack</td> <td>? ? ?</td></tr>\n" );
         fprintf ( detailed1, "</table>\n" );
         fprintf ( detailed1, "</td>\n\n<td width=\"150\">" );
         if ( exist ( b2.c_str() ))
            fprintf ( detailed1, "<img src=\"%s\">", b2.c_str() );            
         fprintf ( detailed1, "</td></tr></table>\n" );
         fprintf ( detailed1, "<table width=\"100%\" id=\"H2\" border=\"1\" bordercolordark=\"#333333\" bordercolorlight=\"#408050\" cellpadding=\"1\" cellspacing=\"1\">\n<tr align=\"center\">" );
         fprintf ( detailed1, "<td><a href=\"%s\">Movement</a></td>", s2.c_str() );
         fprintf ( detailed1, "<td><a href=\"%s\">Weapons</a></td>" , s3.c_str() );
         fprintf ( detailed1, "<td><a href=\"%s\">Functions</a></td>" , s4.c_str() );
         fprintf ( detailed1, "<td><a href=\"%s\">Loading</a></td>" , s4.c_str() );
         fprintf ( detailed1, "<td><a href=\"%s\">Description</a></td>" , s5.c_str() );
//         fprintf ( detailed1, "<td><a href=\"%s\">Research</a></td>" , s4.c_str() );                 
         fprintf ( detailed1, "</tr></table>\n" );
// END UNIT GENERAL
          
         // some details about the unit; %d tells C to insert a decimal number there
         // take a look at the vehicletype class in vehicletype.h for the names of all variables that make a vehicletype
         // be carefuel not to make a , at the end of the first lines, since this would seperate the string in to several independant strings

         // choehenstufen is a global array that contains the names of the height levels


// BEGIN MOVEMENT

  // Hoehenstufen
         int i,w;
    // Tabellenbeginn
         fprintf( detailed2, "<TABLE align=\"left\" rules=\"rows\" id=\"H9\" border=\"1\" bordercolordark=\"#333333\" bordercolorlight=\"#408050\" cellpadding=\"1\" cellspacing=\"1\">"
                             "<TR><td colspan=\"9\" bgcolor=\"#20483f\" id=\"H2\">Reachable levels of height:</td></tr>\n<tr>");
    // Spaltentitel
         fprintf ( detailed2, "<td></td>" );
         for ( i = 0; i < 8; i++ )
             fprintf ( detailed2, " <TD><IMG src=\"../hoehe%d.gif\" ></TD>", i);
         fprintf( detailed2, "</TR>\n<TR>\n");
    // Spaltenwerte Hacken
         fprintf ( detailed2, "<td></td>" );
         for ( i = 0; i < 8; i++ )
            if ( ft->height & ( 1 << i ))
                fprintf ( detailed2, " <TD><img src=\"../hacken.gif\"></TD>" );
            else
                fprintf ( detailed2, " <TD></TD>"  );
         fprintf( detailed2, "</TR>\n<TR>\n");
    // Spaltenwerte pro Runde
         fprintf ( detailed2, "<td>Round</td>" );
         for ( i = 0; i < 8; i++ ) 
            if ( ft->height & ( 1 << i ))
                fprintf ( detailed2, " <TD align=\"center\">%d</TD>", (ft->movement[i]/10) );
            else
                fprintf ( detailed2, " <TD></TD>"  );
         fprintf( detailed2, "</TR>\n<TR>\n");
    // Spaltenwerte Maximum
         fprintf ( detailed2, "<td>Max.</td>" );
         for ( i = 0; i < 8; i++ ) 
            if ( (ft->height & ( 1 << i )) && ft->fuelConsumption )
                fprintf ( detailed2, " <TD align=\"center\">%d</TD>", ft->tank.fuel/ft->fuelConsumption );
            else
                fprintf ( detailed2, " <TD></TD>"  );
         fprintf( detailed2, "\n</TR>\n</TABLE>\n");
         
  // Einzelne Werte
    // Tabellenbeginn 
         fprintf( detailed2, "<TABLE id=\"H2\" border=\"1\" bordercolordark=\"#333333\" bordercolorlight=\"#408050\" cellpadding=\"1\" cellspacing=\"1\">\n"
                             "<TR><td bgcolor=\"#20483f\">Fuel tank</td>    <td align=\"center\">%d</td></tr>\n" 
                             "<TR><td bgcolor=\"#20483f\">Material tank</td><td align=\"center\">%d</td></tr>\n"
                             "<TR><td bgcolor=\"#20483f\">Energy tank</td>  <td align=\"center\">%d</td></tr>\n"
                             "<TR><td bgcolor=\"#20483f\">Consumption</td>  <td align=\"center\">%d</td></tr>\n"
                             "<TR><td bgcolor=\"#20483f\">*Steigung*</td>   <td align=\"center\">%d</td></tr>\n", ft->tank.fuel, ft->tank.material, ft->tank.energy, ft->fuelConsumption, ft->steigung );
         fprintf( detailed2, "\n</TABLE>\n");


  // Bodentypen global
         fprintf( detailed2,"<br><br>\n<table rules=\"rows\" id=\"H2\" border=\"1\" bordercolordark=\"#333333\" bordercolorlight=\"#408050\" cellpadding=\"1\" cellspacing=\"1\"><tr><td colspan=\"34\" bgcolor=\"#20483f\">" );
         fprintf( detailed2,"Terraintypes for not flying units:</td></tr>\n<tr>" );
         
    // Spalten gfx
         fprintf ( detailed2, "<td></td>" );
         for ( i = 0; i < cbodenartennum ; i++ ) 
             fprintf ( detailed2, " <TD><IMG src=\"../gfx%d.gif\"></TD>", i);
         fprintf( detailed2, "</TR>\n<TR>\n");
                  
    // Spaltenwerte befahrbare Bodentypen        
         fprintf ( detailed2, "<td>can&nbsp;drive&nbsp;on</td>" );
         for ( i = 0; i < cbodenartennum ; i++) {
            tterrainbits bts;
            if ( i < 32 )
               bts.set ( 1 << i, 0 );
            else
               bts.set ( 0, 1 << ( i - 32));

            if ( ft->terrainaccess->terrain & bts) 
               fprintf ( detailed2, "<td><img src=\"../hacken.gif\"></td>" );
            else
               fprintf ( detailed2, "<td></td>" );
         } /* endfor */
         fprintf( detailed2, "</TR>\n<TR>\n");

    // Spaltenwerte stirbt auf        
         fprintf ( detailed2, "<td>dies on</td>" );
         for ( i = 0; i < cbodenartennum ; i++) {
            tterrainbits bts;
            if ( i < 32 )
               bts.set ( 1 << i, 0 );
            else
               bts.set ( 0, 1 << ( i - 32));

            if ( ft->terrainaccess->terrainkill & bts) 
               fprintf ( detailed2, "<td><img src=\"../hacken.gif\"></td>" );
            else
               fprintf ( detailed2, "<td></td>" );
         } /* endfor */
         fprintf( detailed2, "</TR>\n<TR>\n");

   // Spaltenwerte can not drive on
         fprintf ( detailed2, "<td>not drive on</td>" );
         for ( i = 0; i < cbodenartennum ; i++) {
            tterrainbits bts;
            if ( i < 32 )
               bts.set ( 1 << i, 0 );
            else
               bts.set ( 0, 1 << ( i - 32));

            if ( ft->terrainaccess->terrainnot & bts) 
               fprintf ( detailed2, "<td><img src=\"../hacken.gif\"></td>" );
            else
               fprintf ( detailed2, "<td></td>" );
         } /* endfor */

         fprintf ( detailed2, "</tr><tr>\n" );
   // Spaltenwerte can need terrain       
         fprintf ( detailed2, "<td>required</td>" );
         for ( i = 0; i < cbodenartennum ; i++) {
            tterrainbits bts;
            if ( i < 32 )
               bts.set ( 1 << i, 0 );
            else
               bts.set ( 0, 1 << ( i - 32));

            if ( ft->terrainaccess->terrainreq & bts) 
               fprintf ( detailed2, "<td><img src=\"../hacken.gif\"></td>" );
            else
               fprintf ( detailed2, "<td></td>" );
         } /* endfor */
         fprintf ( detailed2, "</tr></table>\n" );   
// ENDE MOVEMENT
   
// BEGIN WEAPONS
   // Waffen NR-TYP-AMMO-DISTANCE-STRENGS-
         fprintf ( detailed3, "<table nowrap id=\"H10\" border=\"1\" bordercolordark=\"#333333\" bordercolorlight=\"#408050\" cellpadding=\"1\" cellspacing=\"1\"> \n" );
         // Überschriften
         fprintf ( detailed3, "<tr><td colspan=\"2\"></td>" 
                              "    <td bgcolor=\"#20483f\" colspan=\"4\" align=\"center\">Distance</td>"
                              "    <td bgcolor=\"#20483f\" colspan=\"8\" align=\"center\"> can be shot from: </td>"
                              "    <td></td>"
                              "    <td bgcolor=\"#20483f\" colspan=\"8\" align=\"center\"> attack to: </td>"
                              "    <td>Type</td>"
                              "</tr>\n" );
         // GFX
         fprintf ( detailed3, "<tr><td></td>" 
                              "    <td><img src=\"../ammo.gif\"</td>"
                              "    <td><img src=\"../dis1.gif\"</td>"
                              "    <td><img src=\"../str1.gif\"</td>"
                              "    <td><img src=\"../dis2.gif\"</td>"
                              "    <td><img src=\"../str2.gif\"</td>" );
            // Höhenstufenzeichen einfügen für shoot from und target
            for ( i = 0; i < 8; i++ ) 
                fprintf ( detailed3, " <TD><IMG src=\"../hoehe%d.gif\" ></TD>", i);
            fprintf (detailed3, "<td></td> ");
            for ( i = 0; i < 8; i++ ) 
                fprintf ( detailed3, " <TD><IMG src=\"../hoehe%d.gif\" ></TD>", i);
         fprintf ( detailed3, "    <td></td>"
                              "</tr>\n" );

         // Werte der Waffen
         for ( w = 0; w < ft->weapons->count ; w++) {
            fprintf ( detailed3, "<tr><td>#%d</td>", w+1 );
            fprintf ( detailed3, "    <td align=\"center\">? ?</td>" ); //Abfrage für AMMO funktioniert noch nicht
            fprintf ( detailed3, "    <td align=\"center\">%d</td>", (ft->weapons->weapon[w].mindistance+9)/10 );
            fprintf ( detailed3, "    <td align=\"center\">%d</td>", ft->weapons->weapon[w].maxstrength ); 
            fprintf ( detailed3, "    <td align=\"center\">%d</td>", (ft->weapons->weapon[w].maxdistance)/10 );
            fprintf ( detailed3, "    <td align=\"center\">%d</td>", ft->weapons->weapon[w].minstrength ); 
            // Höhenstufenzeichen einfügen für shoot from und target
            for ( i = 0; i < 8; i++ ) 
               if ( ft->weapons->weapon[w].sourceheight & ( 1 << i ) )
                  fprintf ( detailed3, "<td><img src=\"../hacken.gif\"></td> " );
               else
                  fprintf ( detailed3, "<td></td>" );
            fprintf ( detailed3, "    <td></td>");
            for ( i = 0; i < 8; i++ ) 
               if ( ft->weapons->weapon[w].targ & ( 1 << i ) )
                  fprintf ( detailed3, "<td><img src=\"../hacken.gif\"></td>" );
               else
                  fprintf ( detailed3, "<td></td>" );
            fprintf ( detailed3, "    <td>" );            
            for ( int i = 0; i < cwaffentypennum; i++ ) 
               if ( ft->weapons->weapon[w].typ & ( 1 << i ) )
                  fprintf ( detailed3, "%s.", cwaffentypen[i] );
            fprintf ( detailed3, "</td></tr>\n" );
         }
         fprintf ( detailed3, "</table>\n" );

         fprintf ( detailed3, "<br><br>\n\n" ); 
            // Die entfernung wird durch 10 dividiert, um die Anzahl der Felder zu erhalten
            // Die normale Division rundet IMMER ab, also 1,9 / 2 = 0
            // Aber die minimale Entfernung muá aufgerundet werden, deshalb benutze ich einen kleinen Trick: Ich addiere vor der Division 9 (also Quotient-1) dazu

   // noch ungeordnet
   //fprintf ( detailed, "Number of weapons: %d <br>\n", ft->weapons->count );
         for ( w = 0; w < ft->weapons->count ; w++) {
            // looping through all weapons; w will be increased from 0 until it reaches ft->weapons->count; 
            fprintf ( detailed3, "<table id=\"H2\" border=\"1\" bordercolordark=\"#333333\" bordercolorlight=\"#408050\" cellpadding=\"1\" cellspacing=\"1\"> \n" );
            fprintf ( detailed3, "<tr><td id=\"h1\" colspan=\"3\"> Weapon #%d: </td></tr>\n", w+1 );
            fprintf ( detailed3, "</table> \n" );

            
            fprintf ( detailed3, "<table id=\"H2\" border=\"1\" bordercolordark=\"#333333\" bordercolorlight=\"#408050\" cellpadding=\"1\" cellspacing=\"1\"> \n" );
            fprintf ( detailed3, "<tr><td colspan=\"16\">The weapon can hit: </td></tr><tr>");
            for ( i = 0; i < cmovemalitypenum; i++ ) 
                fprintf ( detailed3, " <TD><IMG src=\"../typ%d.gif\" ></TD>", i);
            fprintf ( detailed3, "</TR>\n<TR>");
            for ( i = 0; i < cmovemalitypenum; i++ ) 
               if ( !(ft->weapons->weapon[w].targets_not_hittable & ( 1 << i )) )
                  fprintf ( detailed3, "<td><img src=\"../hacken.gif\"></td>" );
               else
                  fprintf ( detailed3, "<td></td>" );

            for ( i = 0; i < 13; i++ ) 
               fprintf(detailed3, "effizienz ber H”henstufendifferenz von %d ist %d%%", i-6, ft->weapons->weapon[w].efficiency[i] );

            fprintf ( detailed3, "</tr>\n");
            fprintf ( detailed3, "</table> \n" );

         } /* endfor */



         fprintf ( detailed1, "</body></html>\n");
         fprintf ( detailed2, "</body></html>\n");
         fprintf ( detailed3, "</body></html>\n");
         fprintf ( detailed4, "</body></html>\n");
         fprintf ( detailed5, "</body></html>\n");
         // Ende des Einheiten Dokuments

         fclose ( detailed );
         fclose ( detailed1 );
         fclose ( detailed2 );
         fclose ( detailed3 );
         fclose ( detailed4 );
         fclose ( detailed5 );
         // closing the file

         printf(" processed unit %s \n", ft->description );
         // we are writing this not to a file, but the screen
         
         cn = ff.getnextname();
      }
      // getting the name of the next file and closing the loop
        
      // Dokument Übersicht Ende
      fprintf( overview , "</table></body></html>\n" );   
      fprintf ( overview1, "</body></html>\n" );
         
      fclose ( overview );
      fclose ( overview1 );
   
   } /* endtry */
   catch ( tfileerror err ) {
      printf("\nfatal error accessing file %s \n", err.filename );
      return 1;
   } /* endcatch */
   catch ( ASCexception ) {
      printf("\na fatal exception occured\n" );
      return 2;
   } /* endcatch */

   return 0;
};




