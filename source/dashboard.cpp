/***************************************************************************
                          dashboard.cpp  -  description
                             -------------------
    begin                : Sat Jan 27 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/
/*! \file dashboard.cpp
    \brief The box displaying unit information
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "mapdisplay.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "spfst.h"
#include "typen.h"
#include "loaders.h"
#include "gameoptions.h"
#include "dialog.h"
#include "stack.h"
#include "loadbi3.h"
#include "mapalgorithms.h"
#include "controls.h"
#include "dashboard.h"
#include "weather.h"

         tdashboard  dashboard;

tdashboard::tdashboard ( void )
{
   fuelbkgr  = NULL;
   imagebkgr = NULL;
   movedisp  = 0;
   windheight = 0;
   windheightshown = 0;
   for ( int i = 0; i< 8; i++ )
      weaps[i].displayed = 0;
   repainthard = 1;
   materialdisplayed = 1;
}

void tdashboard::paint ( const pfield ffield, int playerview )
{
   if ( playerview >= 0 ) {
      if (fieldvisiblenow(ffield, playerview ))
         paintvehicleinfo ( ffield->vehicle, ffield->building, ffield->object, NULL );
      else
         paintvehicleinfo( NULL, NULL, NULL, NULL );
   }
}

void         tdashboard::putheight(integer      i, integer      sel)
                                      //          h”he           m”glichk.
{
   putrotspriteimage ( agmp->resolutionx - ( 640 - 589), 92  + i * 13, icons.height2[sel][i], actmap->actplayer * 8);
}






void         tdashboard::paintheight(void)
{
   if ( vehicle )
          for ( int i = 0; i <= 7; i++) {
             if (vehicle->typ->height & (1 << (7 - i)))
                if (vehicle->height & (1 << (7 - i)))
                  putheight(i,1);
                else
                  putheight(i,2);
             else
               putheight(i,0);
          }

    else
       if ( vehicletype ) {
          for ( int i = 0; i <= 7; i++) {
             if (vehicletype->height & (1 << (7 - i)))
               putheight(i,2);
             else
               putheight(i,0);
          }
       } else
          for ( int i = 0; i <= 7; i++)
              putheight(i,0);
}



void         tdashboard::painttank(void)
{
    word         w;
    int         c;

    int x1 = agmp->resolutionx - ( 640 - 520);
    int x2 = agmp->resolutionx - ( 640 - 573);
    int y1 = 59;
    int y2 = 67;

    if ( vehicle )
       if ( vehicle->typ->tank.fuel )
          w = ( x2 - x1 + 1) * vehicle->tank.fuel / vehicle->typ->tank.fuel;
       else
          w = 0;
    else
      w = 0;

    if (w < ( x2 - x1 + 1) )
       bar( x1 + w , y1 , x2, y2 , 172);

    c = vgcol;
    if (w < 25)
       c = 14;
    if (w < 15)
       c = red;
    if ( w )
       bar(x1, y1, x1 + w - 1 , y2 ,c);

    putspriteimage ( x1, y1, fuelbkgr );
}


char*         tdashboard:: str_2 ( int num )
{
      char* tmp;

      if ( num >= 1000000 ) {
         tmp = strrr ( num / 1000000 );
         strcat ( tmp, "M");
         return tmp;
      } else
         if ( num >= 10000 ) {
            tmp = strrr ( num / 1000 );
            strcat ( tmp, "k");
            return tmp;
         } else
            return strrr ( num );
}


void         tdashboard::paintweaponammount(int h, int num, int max )
{
      word         w;

      w = 20 * num / max;
      if (w > 0)
         bar( agmp->resolutionx - ( 640 - 552),     93 + h * 13, agmp->resolutionx - ( 640 - 551 ) + w, 101 + h * 13, 168 );
      if (w < 20)
         bar( agmp->resolutionx - ( 640 - 552) + w, 93 + h * 13, agmp->resolutionx - ( 640 - 571 ),     101 + h * 13, 172 );

      activefontsettings.justify = righttext;
      activefontsettings.font = schriften.guifont;
      activefontsettings.height = 9;
      activefontsettings.background = 255;
      activefontsettings.length = 19;

      showtext2c( str_2( num ), agmp->resolutionx - ( 640 - 552), 93 + h * 13);
}


void         tdashboard::paintweapon(int         h, int num, int strength,  const SingleWeapon  *weap )
{
      if ( weap->getScalarWeaponType() >= 0 )
         if ( weap->canRefuel() )
            putimage ( agmp->resolutionx - ( 640 - 465), 93 + h * 13, xlatpict ( &xlattables.light, icons.unitinfoguiweapons[ weap->getScalarWeaponType() ] ));
         else
            putimage ( agmp->resolutionx - ( 640 - 465), 93 + h * 13, icons.unitinfoguiweapons[ weap->getScalarWeaponType() ] );
      else
         if ( weap->service() )
            putimage ( agmp->resolutionx - ( 640 - 465), 93 + h * 13, icons.unitinfoguiweapons[ cwservicen ] );


      paintweaponammount( h, num, weap->count );


      activefontsettings.background = 172;
      activefontsettings.justify = righttext;
      activefontsettings.font = schriften.guifont;
      activefontsettings.height = 9;
      activefontsettings.length = 27;
      if ( weap->shootable() ) {
         showtext2c( strrr(strength), agmp->resolutionx - ( 640 - 503), 93 + h * 13);

         weaps[h].displayed = 1;
         weaps[h].maxstrength = int(strength * weapDist.getWeapStrength(weap, weap->mindistance, -1, -1 ));
         weaps[h].minstrength = int(strength * weapDist.getWeapStrength(weap, weap->maxdistance, -1, -1 ));
         weaps[h].mindist = weap->mindistance;
         weaps[h].maxdist = weap->maxdistance;

      } else {
         bar( agmp->resolutionx - ( 640 - 503),  93 + h * 13 ,agmp->resolutionx - ( 640 - 530), 101 + h * 13, 172 );
         weaps[h].displayed = 0;
      }



}




void         tdashboard::paintweapons(void)
{
   memset ( weaps, 0, sizeof ( weaps ));

   int i, j;

   activefontsettings.color = black;
   activefontsettings.background = 255;
   activefontsettings.justify = lefttext;
   activefontsettings.font = font;

   int serv = 0;


   int xp = agmp->resolutionx - ( 640 - 465);

   activefontsettings.justify = righttext;
   i = 0;
   int k = 7;

   const Vehicletype* vt;
   if ( vehicle )
      vt = vehicle->typ;
   else
      vt = vehicletype;

    if ( vt ) {
       if ( vt->weapons->count )
          for (j = 0; j < vt->weapons->count && j < 8; j++) {
             if ( vt->weapons->weapon[j].count ) {
                paintweapon(i, ( vehicle ? vehicle->ammo[j] : vt->weapons->weapon[j].count ), ( vehicle ? vehicle-> weapstrength[j] : vt->weapons->weapon[j].maxstrength ), &vt->weapons->weapon[j] );
                i++;
             }
             else {
                if ( vt->weapons->weapon[j].service() ) {
                   serv = 1;
                   if ( materialdisplayed )
                      if ( vt->tank.fuel ) {
                         putimage ( xp, 93 + k * 13, xlatpict ( &xlattables.light, icons.unitinfoguiweapons[ 8 ] ));
                         paintweaponammount ( k, ( vehicle ? vehicle->tank.fuel : vt->tank.fuel ), vt->tank.fuel );
                         k--;
                      }
                }
             }
          }

       if ( materialdisplayed ) {
          if ( vt->tank.material ) {
             if ( serv )
                putimage ( xp, 93 + k * 13, xlatpict ( &xlattables.light, icons.unitinfoguiweapons[ 11 ] ));
             else
                putimage ( xp, 93 + k * 13, icons.unitinfoguiweapons[ 11 ] );
              paintweaponammount ( k, ( vehicle ? vehicle->tank.material : vt->tank.material ), vt->tank.material );
              k--;
          }
          if ( vt->tank.energy ) {
             if ( serv )
                putimage ( xp, 93 + k * 13, xlatpict ( &xlattables.light, icons.unitinfoguiweapons[ 9 ] ));
             else
                putimage ( xp, 93 + k * 13, icons.unitinfoguiweapons[ 9 ] );
              paintweaponammount ( k, ( vehicle ? vehicle->tank.energy : vt->tank.energy ), vt->tank.energy );
              k--;
          }
       }
    }

    for (j = i; j <= k; j++) {
       putimage( xp, 93 + j * 13, icons.unitinfoguiweapons[12]);
       bar( agmp->resolutionx - ( 640 - 552),  93 + j * 13 ,agmp->resolutionx - ( 640 - 571), 101 + j * 13, 172 );
       bar( agmp->resolutionx - ( 640 - 503),  93 + j * 13 ,agmp->resolutionx - ( 640 - 530), 101 + j * 13, 172 );
    }
}

void         tdashboard :: paintlweaponinfo ( void )
{
   paintlargeweaponinfo();
}

void         tdashboard :: paintlargeweaponinfo ( void )
{
   int i = 0;
   for ( int lw = 0; lw < 16; lw++ )
      largeWeaponsDisplayPos[lw] = -1;

   int serv = -1;
   const Vehicletype* vt;
   if ( vehicle )
      vt = vehicle->typ;
   else
      vt = vehicletype;
   if ( vt ) {
       npush ( activefontsettings );

       int x1 = (agmp->resolutionx - 640) / 2;
       int y1 = 150;

       int count = 0;
       if ( vt->weapons->count )
          for ( int j = 0; j < vt->weapons->count ; j++)
             if ( vt->weapons->weapon[j].getScalarWeaponType() >= 0 )
                count++;
             else
                if (vt->weapons->weapon[j].service() )
                   serv = count;


       if ( serv >= 0 )
          count++;

       if ( vt->tank.energy )
          count++;

       int funcs;
       if ( vehicle )
          funcs = vehicle->functions;
       else
          funcs  = vt->functions;


       if ( (serv>= 0 || (funcs & cfmaterialref)) && vt->tank.material )
          count++;

       if ( (serv>= 0 || (funcs & cffuelref)) && vt->tank.fuel )
          count++;

       count++;

       setinvisiblemouserectanglestk ( x1, y1, x1 + 640, y1 + count * 25 + 110 );
       void* imgbuf = asc_malloc ( imagesize ( x1, y1, x1 + 640, y1 + count * 25 + 110 ));
       getimage ( x1, y1, x1 + 640, y1 + count * 25 + 110, imgbuf );

       putimage ( x1, y1, icons.weaponinfo[0] );

       getinvisiblemouserectanglestk ();


       if ( vt->weapons->count )
          for ( int j = 0; j < vt->weapons->count ; j++) {
             if ( vt->weapons->weapon[j].getScalarWeaponType() >= 0 ) {
                int maxstrength = vt->weapons->weapon[j].maxstrength;
                int minstrength = vt->weapons->weapon[j].minstrength;
                if ( vehicle && maxstrength ) {
                   minstrength = minstrength * vehicle->weapstrength[j] / maxstrength;
                   maxstrength = vehicle->weapstrength[j];
                }

                paintlargeweapon(i, cwaffentypen[ vt->weapons->weapon[j].getScalarWeaponType() ],
                               ( vehicle ? vehicle->ammo[j] : vt->weapons->weapon[j].count ) , vt->weapons->weapon[j].count,
                               vt->weapons->weapon[j].shootable(), vt->weapons->weapon[j].canRefuel(),
                               maxstrength, minstrength,
                               vt->weapons->weapon[j].maxdistance, vt->weapons->weapon[j].mindistance,
                               vt->weapons->weapon[j].sourceheight, vt->weapons->weapon[j].targ );
                largeWeaponsDisplayPos[i] = j;
                i++;
             }
          }

       if ( serv >= 0 ) {
          paintlargeweapon(i, cwaffentypen[ cwservicen ], -1, -1, -1, -1, -1, -1,
                         vt->weapons->weapon[serv].maxdistance, vt->weapons->weapon[serv].mindistance,
                         vt->weapons->weapon[serv].sourceheight, vt->weapons->weapon[serv].targ );
          largeWeaponsDisplayPos[i] = serv;
          i++;
       }
       if ( vt->tank.energy ) {
          paintlargeweapon(i, resourceNames[ 0 ], ( vehicle ? vehicle->tank.energy : vt->tank.energy ), vt->tank.energy, -1, -1, -1, -1, -1, -1, -1, -1 );
          largeWeaponsDisplayPos[i] = -1;
          i++;
       }

       if ( (serv>= 0 || (funcs & cfmaterialref)) && vt->tank.material ) {
          paintlargeweapon(i, resourceNames[ 1 ], ( vehicle ? vehicle->tank.material : vt->tank.material ), vt->tank.material, -1, -1, -1, -1, -1, -1, -1, -1 );
          largeWeaponsDisplayPos[i] = -1;
          i++;
       }
       if ( (serv>= 0 || (funcs & cffuelref)) && vt->tank.fuel ) {
          paintlargeweapon(i, resourceNames[ 2 ], ( vehicle ? vehicle->tank.fuel : vt->tank.fuel ), vt->tank.fuel, -1, -1, -1, -1, -1, -1, -1, -1 );
          largeWeaponsDisplayPos[i] = -1;
          i++;
       }

      {
         int x = x1;
         int y = y1 + i * 14 + 28;

         int height, width;
         getpicsize ( icons.weaponinfo[4], width, height );

         setinvisiblemouserectanglestk ( x, y, x + width, y + height );

         putspriteimage ( x, y, icons.weaponinfo[4] );

         activefontsettings.justify = centertext;
         activefontsettings.font = schriften.guifont;
         activefontsettings.height = 11;
         activefontsettings.length = 80;
         activefontsettings.background = 255;
         if ( vt->wait )
            showtext2c ( "no", x + 140, y +  2 );
         else
            showtext2c ( "yes", x + 140, y +  2 );

         if ( funcs & cf_moveafterattack )
            showtext2c ( "yes", x + 364, y +  2 );
         else
            showtext2c ( "no", x + 364, y +  2 );

         getinvisiblemouserectanglestk ();
         i++;
      }



      int lastpainted = -1;
      int first = 1;
      while ( mouseparams.taste == 2) {
         int topaint  = -1;
         int serv = 0;
         for ( int j = 0; j < vt->weapons->count ; j++) {
            int x = (agmp->resolutionx - 640) / 2;
            int y = 150 + 28 + (j - serv) * 14;
            if ( mouseinrect ( x, y, x + 640, y+ 14 ))
               if ( largeWeaponsDisplayPos[j] != -1 )
                  topaint = largeWeaponsDisplayPos[j];

         }
         if ( topaint != lastpainted ) {
            if ( topaint == -1 )
               paintlargeweaponefficiency ( i, NULL, first, 0 );
            else {
               int effic[13];
               for ( int k = 0; k < 13; k++ )
                  effic[k] = vt->weapons->weapon[topaint].efficiency[k];
               int mindelta = 1000;
               int maxdelta = -1000;
               for ( int h1 = 0; h1 < 8; h1++ )
                  for ( int h2 = 0; h2 < 8; h2++ )
                     if ( vt->weapons->weapon[topaint].sourceheight & ( 1 << h1 ))
                        if ( vt->weapons->weapon[topaint].targ & ( 1 << h2 )) {
                           int delta = getheightdelta ( h1, h2);
                           if ( delta > maxdelta )
                              maxdelta = delta;
                           if ( delta < mindelta )
                              mindelta = delta;
                        }
               for ( int a = -6; a < mindelta; a++ )
                  effic[6+a] = -1;
               for ( int b = maxdelta+1; b < 7; b++ )
                  effic[6+b] = -1;

               paintlargeweaponefficiency ( i, effic, first, vt->weapons->weapon[topaint].targets_not_hittable );
            }
            lastpainted = topaint;
            first = 0;
         }
         releasetimeslice();
      }

      setinvisiblemouserectanglestk ( x1, y1, x1 + 640, y1 + count * 25 + 40 );
      putimage ( x1, y1, imgbuf );
      getinvisiblemouserectanglestk ();

      asc_free  ( imgbuf );

      npop ( activefontsettings );
   }

}

void         tdashboard::paintlargeweaponefficiency ( int pos, int* e, int first, int nohit )
{
   int x = (agmp->resolutionx - 640) / 2;
   int y = 150 + 28 + pos * 14;

   int height, width;
   getpicsize ( icons.weaponinfo[3], width, height );

   setinvisiblemouserectanglestk ( x, y, x + width, y + height );

   if ( first )
      putspriteimage ( x, y, icons.weaponinfo[3] );

   static int bk1 = -1;
   static int bk2 = -1;
   if ( bk1 == -1 )
      bk1 = getpixel ( x + 100, y + 5 );
   if ( bk2 == -1 )
      bk2 = getpixel ( x + 100, y + 19);

   activefontsettings.justify = centertext;
   activefontsettings.font = schriften.guifont;
   activefontsettings.height = 10;
   activefontsettings.length = 36;
   for ( int i = 0; i < 13; i++ )
      if ( e && e[i] != -1 ) {
         activefontsettings.background = bk1;
         showtext2c ( strrr ( i - 6 ), x + 88 + i * 42, y +  2 );
         activefontsettings.background = bk2;
         showtext4c ( "%s%%", x + 88 + i * 42, y + 15, strrr ( e[i] ) );
      } else {
         activefontsettings.background = bk1;
         showtext2c ( "", x + 88 + i * 42, y +  2 );
         activefontsettings.background = bk2;
         showtext2c ( "",  x + 88 + i * 42, y + 15 );
     }

   activefontsettings.length = 179;
   // activefontsettings.background = white;
   activefontsettings.color = 86;
   activefontsettings.justify = lefttext;
   // activefontsettings.color = black;
   for ( int j = 0; j < cmovemalitypenum; j++ ) {
      int pnt;
      if ( dataVersion >= 2 )
         pnt = !(nohit & (1 << j ));
      else
         pnt = nohit & (1 << j );
      if ( pnt ) {
         activefontsettings.font = schriften.guifont;
         showtext2c ( cmovemalitypes[j],   x + 88 + (j % 3) * 180, y + 15 + 16 + (j / 3) * 12 );
      } else {
         activefontsettings.font = schriften.monogui;
         showtext2  ( cmovemalitypes[j],   x + 88 + (j % 3) * 180, y + 15 + 16 + (j / 3) * 12 );
      }
   }
   getinvisiblemouserectanglestk ();
}


void         tdashboard::paintlargeweapon ( int pos, const char* name, int ammoact, int ammomax, int shoot, int refuel, int strengthmax, int strengthmin, int distmax, int distmin, int from, int to )
{
   int height, width;
   getpicsize ( icons.weaponinfo[1], width, height );



   int x = (agmp->resolutionx - 640) / 2;
   int y = 150 + 28 + pos * 14;

   setinvisiblemouserectanglestk ( x, y, x + width, y + height );

   putspriteimage ( x, y, icons.weaponinfo[1] );
   y += 4;

   activefontsettings.background = 255;
   activefontsettings.font = schriften.guifont;
   activefontsettings.height = 11;
   activefontsettings.length = 75;

   if ( name ) {
      activefontsettings.justify = lefttext;
      activefontsettings.length = 75;
      showtext2c ( name, x + 2, y );
   }

   if ( ammoact >= 0 ) {
      activefontsettings.length = 20;
      char buf[100];
      char buf2[100];
      sprintf(buf, "%s / %s", int2string ( ammoact, buf ), int2string ( ammomax, buf2 ) );
      activefontsettings.length = 50;
      activefontsettings.justify = centertext;
      showtext2c ( buf, x + 77, y );
   }

   if ( shoot >= 0 ) {
      activefontsettings.justify = centertext;
      activefontsettings.length = 25;
      if ( shoot )
         showtext2c ( "yes", x + 130, y );
      else
         showtext2c ( "no", x + 130, y );
   }

   if ( refuel >= 0 ) {
      activefontsettings.justify = centertext;
      activefontsettings.length = 25;
      if ( refuel )
         showtext2c ( "yes", x + 158, y );
      else
         showtext2c ( "no", x + 158, y );
   }

   if ( strengthmax >= 0 ) {
      activefontsettings.justify = lefttext;
      activefontsettings.length = 38;
      showtext2c ( strrr( strengthmax ), x + 190, y );
   }

   if ( strengthmin >= 0 ) {
      activefontsettings.length = 38;
      activefontsettings.justify = righttext;
      showtext2c ( strrr( strengthmin ), x + 190, y );
   }

   if ( distmin >= 0 ) {
      activefontsettings.length = 36;
      activefontsettings.justify = lefttext;
      showtext2c ( strrrd8u( distmin ), x + 237, y );
   }

   if ( distmax >= 0 ) {
      activefontsettings.length = 36;
      activefontsettings.justify = righttext;
      showtext2c ( strrrd8d( distmax ), x + 237, y );
   }


   if ( from > 0 )
      for ( int i = 0; i < 8; i++ )
         if ( from & ( 1 << i ))
            putimage ( x + 285 + i * 22, y-2, icons.weaponinfo[2] );

   if ( to > 0 )
      for ( int i = 0; i < 8; i++ )
         if ( to & ( 1 << i ))
            putimage ( x + 465 + i * 22, y-2, icons.weaponinfo[2] );

   activefontsettings.justify = lefttext;

   getinvisiblemouserectanglestk ();
}




void         tdashboard::allocmem ( void )
{
    int x1 = 520;
    int x2 = 573;
    int y1 = 71;
    int y2 = 79;

    fuelbkgrread = 0;
    fuelbkgr = new char[  imagesize ( x1, y1, x2, y2 ) ] ;

    x1 = 460;
    y1 = 31;

    imagebkgr = new char[  imagesize ( x1, y1, x1 + 30, y1 + 30 ) ];
    imageshown = 10;

}


void         tdashboard::paintdamage(void)
{
    int x1 = agmp->resolutionx - ( 640 - 520);
    int x2 = agmp->resolutionx - ( 640 - 573);
    int y1 = 71;
    int y2 = 79;

    if ( fuelbkgrread == 0 ) {
       getimage ( x1, y1, x2, y2, fuelbkgr );
       int sze = imagesize ( x1, y1, x2, y2 );
       char *pc = (char*) fuelbkgr;
       int cl  = getpixel ( agmp->resolutionx - 81, 63 );
       for ( int m = 4; m < sze; m++ )
          if ( pc[m] == cl )
             pc[m] = 255;
       fuelbkgrread = 1;
    }


    int w = 0;
    int         c;



    if ( vehicle ) {
       w = (x2 - x1 + 1) * ( 100 - vehicle->damage ) / 100;
       if ( w > 23 )       // container :: subwin :: buildinginfo :: damage verwendet die selben Farben
          c = vgcol;
       else
          if ( w > 15 )
             c = yellow;
          else
             if ( w > 7 )
                c = lightred;
             else
                c = red;
    } else
       if ( building ) {
          w = (x2 - x1 + 1) * ( 100 - building->damage ) / 100;
          if (building->damage >= mingebaeudeeroberungsbeschaedigung)
             c = red;
          else
             c = vgcol;

       }
       else
          if ( object ) {
             c = darkgray;
             for ( int i = object->objnum-1; i >= 0; i-- )
               if ( object->object[ i ] -> typ->armor > 0 )
                  w = (x2 - x1 + 1) * ( 100 - object->object[ i ] -> damage ) / 100;

          } else
             w = 0;


    if (w < (x2 - x1 + 1) )
       bar( x1 + w , y1 , x2, y2 , 172);

    if ( w )
       bar(x1, y1, x1 + w - 1, y2 ,c);

    putspriteimage ( x1, y1, fuelbkgr );
}




void         tdashboard::paintexperience(void)
{
    if (vehicle)
       putimage( agmp->resolutionx - ( 640 - 587),  27, icons.experience[vehicle->experience]);
    else
       bar( agmp->resolutionx - ( 640 - 587), 27,agmp->resolutionx - ( 640 - 611), 50, 171);
}




void         tdashboard::paintmovement(void)
{
    if ( vehicle ) {
       activefontsettings.justify = centertext;
       activefontsettings.color = white;
       activefontsettings.background = 172;
       activefontsettings.font = schriften.guifont;
       activefontsettings.length = 17;
       activefontsettings.height = 9;
       if ( vehicle->typ->fuelConsumption ) {
          if ( (movedisp  &&  vehicle->typ->fuelConsumption ) || (minmalq*vehicle->tank.fuel / vehicle->typ->fuelConsumption  < vehicle->getMovement() ))
             showtext2c( strrrd8d( minmalq*vehicle->tank.fuel / vehicle->typ->fuelConsumption ), agmp->resolutionx - ( 640 - 591), 59);
          else
             showtext2c( strrrd8d(vehicle->getMovement() ), agmp->resolutionx - ( 640 - 591), 59);
       } else
          showtext2c( strrrd8d(0), agmp->resolutionx - ( 640 - 591), 59);
    } else
       bar( agmp->resolutionx - ( 640 - 591), 59,agmp->resolutionx - ( 640 - 608), 67, 172);
}

void         tdashboard::paintarmor(void)
{
    if ( vehicle || vehicletype ) {
       activefontsettings.justify = centertext;
       activefontsettings.color = white;
       activefontsettings.background = 172;
       activefontsettings.font = schriften.guifont;
       activefontsettings.length = 18;
       activefontsettings.height = 9;
       int arm;
       if ( vehicle )
          arm = vehicle->armor;
       else
          arm = vehicletype->armor;

       showtext2c( strrr(arm),agmp->resolutionx - ( 640 - 591), 71);
    } else
       bar(agmp->resolutionx - ( 640 - 591), 71,agmp->resolutionx - ( 640 - 608), 79, 172);
}

void         tdashboard::paintwind( int repaint )
{
  int j, i;

/*   void *p;
   if (actmap->weather.wind.direction & 1)
      p = icons.wind.southwest[actmap->weather.wind.speed >> 6];
   else
      p = icons.wind.south[actmap->weather.wind.speed >> 6];

   switch (actmap->weather.wind.direction >> 1) {
      case 0: putimage(430,320,p);
         break;
      case 1: putrotspriteimage90(430,320,p,0);
         break;
      case 2: putrotspriteimage180(430,320,p,0);
         break;
      case 3: putrotspriteimage270(430,320,p,0);
         break;
   }

   activefontsettings.justify = centertext;
   activefontsettings.color = black;
   activefontsettings.background = white;
   activefontsettings.font = schriften.smallarial;
   activefontsettings.length = 30;
   showtext2( strrr(actmap->weather.wind.speed),430,354);

   */

   if ( !CGameOptions::Instance()->smallmapactive ) {
      setinvisiblemouserectanglestk ( agmp->resolutionx - ( 800 - 612), 213, agmp->resolutionx - ( 800 - 781), 305 );

      static int lastdir = -1;

      if ( repaint ) {
         putimage ( agmp->resolutionx - ( 640 - 450), 211, icons.windbackground );
         lastdir = -1;
      }


      if ( !windheightshown ) {
         int x1 = agmp->resolutionx - ( 640 - 489 );
         int x2 = agmp->resolutionx - ( 640 - 509 );
         int y1 = 284;
         int y2 = 294;
         windheightbackground = new char [imagesize ( x1, y1, x2, y2 )];
         getimage ( x1, y1, x2, y2, windheightbackground );
         windheightshown = 1;
      }


      int unitspeed;
      int height = windheight;

      if ( vehicle ) {
          unitspeed = getmaxwindspeedforunit ( vehicle );
          if ( unitspeed < 255*256 )
             height = getwindheightforunit ( vehicle );
      }

      if ( actmap->weather.wind[height].speed ) {
        #ifdef HEXAGON
          if ( lastdir != actmap->weather.wind[height].direction ) {
             putimage ( agmp->resolutionx - ( 640 - 506), 227, icons.wind[ 8 ] );
             char* pic = rotatepict ( icons.windarrow, directionangle[ actmap->weather.wind[height].direction ] );
             int h1,w1, h2, w2;
             getpicsize ( pic, w2, h2 );
             getpicsize ( icons.wind[ 8 ], w1, h1 );
             putspriteimage ( agmp->resolutionx - ( 640 - (506 + w1/2 - w2/2)), 227 + h1/2- h2/2, pic );
             delete[] pic;
             lastdir = actmap->weather.wind[height].direction;
          }
        #else
         putimage ( agmp->resolutionx - ( 640 - 506), 227, icons.wind[ actmap->weather.wind[height].direction ] );
        #endif
      } else
         putimage ( agmp->resolutionx - ( 640 - 506), 227, icons.wind[ 8 ] );

      if ( (actmap->weather.wind[0] == actmap->weather.wind[1]) && (actmap->weather.wind[1] == actmap->weather.wind[2]) ) {
         if ( windheightshown == 2 ) {
            putimage ( agmp->resolutionx - ( 640 - 489), 284, windheightbackground );
            windheightshown = 1;
         }
      } else {
         windheightshown = 2;
         putimage ( agmp->resolutionx - ( 640 - 489), 284, icons.height2[0][ 3 - height] );
      }

      windheight = height;

      for (i = 0; i < (actmap->weather.wind[height].speed+31) / 32 ; i++ ) {
         int color = green;

         if ( vehicle == NULL ) {
            /*
            if ( i >= 6 )
               color = red;
            else
               if ( i >= 4 )
                  color = yellow;
             */
         } else {
             int windspeed = actmap->weather.wind[ height ].speed*maxwindspeed ;
             if ( unitspeed < 255*256 )
                if ( windspeed > unitspeed*9/10 )
                   color = red;
                else
                   if ( windspeed > unitspeed*66/100 )
                     color = yellow;
         }
         bar ( agmp->resolutionx - ( 640 - 597), 282-i*7, agmp->resolutionx - ( 640 - 601), 284-i*7, color );
      } /* endfor */
      for (j = i; j < 8; j++ )
         bar ( agmp->resolutionx - ( 640 - 597), 282-j*7, agmp->resolutionx - ( 640 - 601), 284-j*7, black );

      getinvisiblemouserectanglestk (  );
   }
}


void         tdashboard::paintimage(void)
 {

    int x1 = agmp->resolutionx - ( 640 - 460);
    int y1 = 31;
    if ( imageshown == 10 ) {
       getimage ( x1, y1, x1 + 30, y1 + 30 , imagebkgr );
       imageshown = 0;
    }

    if ( imageshown )
       putimage ( x1, y1, imagebkgr );

    if ( vehicle ) {
       #ifdef HEXAGON

       TrueColorImage* zimg = zoomimage ( vehicle->typ->picture[0], fieldsizex/2, fieldsizey/2, pal, 0 );
       char* pic = convertimage ( zimg, pal ) ;
       putrotspriteimage ( x1+3, y1+3, pic, vehicle->color );
       delete[] pic;
       delete zimg;

       /*
       {
          tvirtualdisplay vi ( 50, 50, 255 );
          putrotspriteimage ( 0, 0, vehicle->typ->picture[0], vehicle->color );
          putmask ( 0, 0, icons.hex2octmask, 0 );
          getimage ( (fieldsizex-30) / 2, (fieldsizey-30) / 2, (fieldsizex-30) / 2 + 30, (fieldsizey-30) / 2 + 30, xlatbuffer );
       }
       putspriteimage( x1, y1, xlatbuffer );
       */

       #else
       putrotspriteimage ( x1, y1, vehicle->typ->picture[0], vehicle->color );
       #endif
       imageshown = 1;
    } else
       imageshown = 0;

}


void         tdashboard::paintclasses ( void )
{
        if ( CGameOptions::Instance()->showUnitOwner ) {
      const char* owner = NULL;
      if ( vehicle )
         owner = actmap->getPlayerName(vehicle->color / 8);
      else
         if ( building )
            owner = actmap->getPlayerName(building->color / 8);

      if ( owner ) {
         activefontsettings.justify = lefttext;
         activefontsettings.color = white;
         activefontsettings.background = 171;
         activefontsettings.font = schriften.guifont;
         activefontsettings.length = 75;
         activefontsettings.height = 0;
         showtext2c( owner, agmp->resolutionx - ( 640 - 500), 42);
         activefontsettings.height = 9;
      } else
         bar ( agmp->resolutionx - ( 640 - 499), 42, agmp->resolutionx - ( 640 - 575), 50, 171 );
   } else
      if (vehicle)
         if (vehicle->typ->classnum) {
            activefontsettings.justify = lefttext;
            activefontsettings.color = white;
            activefontsettings.background = 171;
            activefontsettings.font = schriften.guifont;
            activefontsettings.length = 75;
            activefontsettings.height = 0;
            showtext2c( vehicle->typ->classnames[vehicle->klasse] ,agmp->resolutionx - ( 640 - 500), 42);
            activefontsettings.height = 9;
         } else
            bar ( agmp->resolutionx - ( 640 - 499), 42, agmp->resolutionx - ( 640 - 575), 50, 171 );
      else
         bar ( agmp->resolutionx - ( 640 - 499), 42, agmp->resolutionx - ( 640 - 575), 50, 171 );
}

void         tdashboard::paintname ( void )
{
   if ( vehicle || building || vehicletype) {
      activefontsettings.justify = lefttext;
      activefontsettings.color = white;
      activefontsettings.background = 171;
      activefontsettings.font = schriften.guifont;
      activefontsettings.length = 75;
      activefontsettings.height = 9;
      const Vehicletype* vt;
      if ( vehicle )
         vt = vehicle->typ;
      else
         vt = vehicletype;

      if ( vehicle || vt ) {
         if ( vehicle && !vehicle->name.empty() )
            showtext2c( vehicle->name.c_str() , agmp->resolutionx - ( 640 - 500 ), 27);
         else
            if ( vt->name && vt->name[0] )
               showtext2c( vt->name , agmp->resolutionx - ( 640 - 500 ), 27);
            else
               if ( vt->description  &&  vt->description[0] )
                  showtext2c( vt->description ,agmp->resolutionx - ( 640 - 500 ), 27);
               else
                  bar ( agmp->resolutionx - ( 640 - 499 ), 27, agmp->resolutionx - ( 640 - 575 ), 35, 171 );

      } else {
         if ( !building->name.empty() )
            showtext2c( building->name.c_str() , agmp->resolutionx - ( 640 - 500), 27);
         else
            showtext2c( building->typ->name , agmp->resolutionx - ( 640 - 500), 27);

      }
      activefontsettings.height = 0;
   } else
      bar ( agmp->resolutionx - ( 640 - 499), 27, agmp->resolutionx - ( 640 - 575), 35, 171 );
}

void         tdashboard::paintplayer( void )
{
   putspriteimage ( agmp->resolutionx - ( 640 - 540), 127, icons.player[actmap->actplayer] );
}


void         tdashboard::paintalliances ( void )
{
   int j = 0;
   for (int i = 0; i< 8 ; i++ ) {
      if ( i != actmap->actplayer ) {
         if ( actmap->player[i].exist() ) {
            if ( getdiplomaticstatus ( i*8 ) == capeace )
               putimage ( agmp->resolutionx - ( 640 - 476) + j * 19, agmp->resolutiony - ( 480 - 452), icons.allianz[i][0] );
            else
               putimage ( agmp->resolutionx - ( 640 - 476) + j * 19, agmp->resolutiony - ( 480 - 452), icons.allianz[i][1] );
         } else
             putimage ( agmp->resolutionx - ( 640 - 476) + j * 19, agmp->resolutiony - ( 480 - 452), icons.allianz[i][2] );
         j++;
      }
   } /* endfor */
}

#ifdef FREEMAPZOOM
void         tdashboard::paintzoom( void )
{
   int h;
   getpicsize ( zoom.pic, zoom.picwidth, h );
   zoom.x1 = agmp->resolutionx - ( 640 - 464);
   zoom.x2 = agmp->resolutionx - ( 640 - 609);
   zoom.y1 = agmp->resolutiony - ( 480 - 444);
   zoom.y2 = agmp->resolutiony - ( 480 - 464);

   setinvisiblemouserectanglestk ( zoom.x1, zoom.y1, zoom.x2, zoom.y2 );

   static void* background = NULL;
   if ( !background ) {
      background = asc_malloc ( imagesize ( zoom.x1, zoom.y1, zoom.x2, zoom.y2 ));
      getimage ( zoom.x1, zoom.y1, zoom.x2, zoom.y2, background );
   } else
      putimage ( zoom.x1, zoom.y1, background );

   int actzoom = zoomlevel.getzoomlevel() - zoomlevel.getminzoom();
   int maxzoom = zoomlevel.getmaxzoom() - zoomlevel.getminzoom();
   int dist = zoom.x2 - zoom.picwidth - zoom.x1;
   putimage ( zoom.x1 + dist - dist * actzoom / maxzoom, zoom.y1, zoom.pic );

   getinvisiblemouserectanglestk ();
}
#endif



class tmainshowmap : public tbasicshowmap {
          public:
            void checkformouse ( void );
       };

void tmainshowmap :: checkformouse ( void )
{
   if ( mouseparams.taste == 1 ) {
       int oldx = dispxpos;
       int oldy = dispypos;
       tbasicshowmap :: checkformouse();
       if ( oldx != dispxpos  ||  oldy != dispypos ) {
          setmapposition();
          displaymap();
       }
   }
}

tmainshowmap* smallmap = NULL;

void         tdashboard::paintsmallmap ( int repaint )
{
   setinvisiblemouserectanglestk ( agmp->resolutionx - ( 800 - 612), 213, agmp->resolutionx - ( 800 - 781), 305 );
   if ( !smallmap ) {
      smallmap = new tmainshowmap;
      CGameOptions::Instance()->smallmapactive = 1;
      CGameOptions::Instance()->setChanged();
      repaint = 1;
   }

   if ( repaint )
      bar ( agmp->resolutionx - ( 800 - 612), 213, agmp->resolutionx - ( 800 - 781), 305, greenbackgroundcol );

   smallmap->init ( agmp->resolutionx - ( 800 - 612 ) , 213, 781-612, 305-213 );
   smallmap->generatemap ( 1 );
   smallmap->dispimage ( );

   getinvisiblemouserectanglestk (  );

}

void         tdashboard::checkformouse ( int func )
{

    if ( mouseinrect ( agmp->resolutionx - ( 800 - 612), 213, agmp->resolutionx - ( 800 - 781), 305 ) && (mouseparams.taste == 2)) {
       CGameOptions::Instance()->smallmapactive = !CGameOptions::Instance()->smallmapactive;
       CGameOptions::Instance()->setChanged();

       if ( CGameOptions::Instance()->smallmapactive )
          dashboard.paintsmallmap( 1 );
       else
          dashboard.paintwind( 1 );

       while ( mouseparams.taste == 2 )
          releasetimeslice();
    }
    /*
    if ( mouseinrect ( agmp->resolutionx - ( 800 - 620),  90, agmp->resolutionx - ( 800 - 735), 196 ) && (mouseparams.taste == 2)) {
       npush ( activefontsettings );
       materialdisplayed = !materialdisplayed;
       setinvisiblemouserectanglestk ( agmp->resolutionx - ( 800 - 620),  90, agmp->resolutionx - ( 800 - 735), 196 );
       paintweapons();
       while ( mouseparams.taste == 2 );
       getinvisiblemouserectanglestk ();
       npop ( activefontsettings );
    }
    */

    if ( (func & 1) == 0 ) {
       if ( smallmap  &&  CGameOptions::Instance()->smallmapactive )
          smallmap->checkformouse();

       if ( !CGameOptions::Instance()->smallmapactive ) {
          if ( mouseparams.x >= agmp->resolutionx - ( 640 - 588 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 610 )  &&   mouseparams.y >= 227   &&   mouseparams.y <= 290  && (mouseparams.taste & 1) ) {
             displaywindspeed();
             while ( mouseparams.x >= agmp->resolutionx - ( 640 - 588 )  &&   mouseparams.x <= agmp->resolutionx - ( 640 - 610 )  &&   mouseparams.y >= 227   &&   mouseparams.y <= 290  && (mouseparams.taste & 1) )
                releasetimeslice();
          }
          if ( mouseinrect ( agmp->resolutionx - ( 640 - 489 ), 284, agmp->resolutionx - ( 640 - 509 ), 294 ) && (mouseparams.taste & 1)) {
             dashboard.windheight++;
             if ( dashboard.windheight > 2 )
                dashboard.windheight = 0;
             dashboard.x = 0xffff;
             paintwind(1);
             while ( mouseinrect ( agmp->resolutionx - ( 640 - 489 ), 284, agmp->resolutionx - ( 640 - 509 ), 294 ) && (mouseparams.taste & 1) )
                releasetimeslice();
          }
       }

      #ifdef FREEMAPZOOM
       if ( mouseparams.taste == 1 )
          if ( mouseinrect ( zoom.x1, zoom.y1, zoom.x2, zoom.y2 )) {
             int pos = mouseparams.x - zoom.x1;
             pos -= zoom.picwidth / 2;
             int w = zoom.x2 - zoom.x1 - zoom.picwidth;
             int perc = 1000 * pos / w;
             if ( perc < 0 )
                perc = 0;
             if ( perc > 1000 )
                perc = 1000;
             int newzoom = zoomlevel.getminzoom() + (zoomlevel.getmaxzoom() - zoomlevel.getminzoom()) * ( 1000 - perc ) / 1000;
             if ( newzoom != zoomlevel.getzoomlevel() ) {
                cursor.hide();
                zoomlevel.setzoomlevel( newzoom );
                paintzoom();
                cursor.show();
                displaymap();
                displaymessage2("new zoom level %d%%", newzoom );
                dashboard.x = 0xffff;
             }
          }
       #endif
    }

    if ( mouseparams.x >= agmp->resolutionx - ( 640 - 578 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 609 )  &&   mouseparams.y >=  59   &&   mouseparams.y <=  67  && (mouseparams.taste & 1) ) {
       dashboard.movedisp = !dashboard.movedisp;
       dashboard.x = 0xffff;
       while ( mouseparams.x >= agmp->resolutionx - ( 640 - 578 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 609 )  &&   mouseparams.y >=  59   &&   mouseparams.y <=  67  && (mouseparams.taste & 1) )
          releasetimeslice();
    }

    for ( int i = 0; i < 8; i++ ) {
       if ( dashboard.weaps[i].displayed )
          if ( mouseinrect ( agmp->resolutionx - ( 640 - 502 ), 92 + i * 13, agmp->resolutionx - ( 640 - 572 ), 102 + i * 13 ) && (mouseparams.taste == 1)) {
             char tmp1[100];
             char tmp2[100];
             strcpy ( tmp1, strrrd8d ( dashboard.weaps[i].maxdist ));
             strcpy ( tmp2, strrrd8u ( dashboard.weaps[i].mindist ));
             displaymessage2 ( "min strength is %d at %s fields, max strength is %d at %s fields", dashboard.weaps[i].minstrength, tmp1, dashboard.weaps[i].maxstrength, tmp2 );

             while ( mouseinrect ( agmp->resolutionx - ( 640 - 502 ), 92 + i * 13, agmp->resolutionx - ( 640 - 572 ), 102 + i * 13 ) && (mouseparams.taste == 1))
                releasetimeslice();
          }
   }

   if ( (vehicle || vehicletype  ) && mouseinrect ( agmp->resolutionx - ( 640 - 461 ), 89, agmp->resolutionx - ( 640 - 577 ), 196 ) && (mouseparams.taste == 2))
      paintlargeweaponinfo();


   if ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )   &&   mouseparams.y >= 71    &&   mouseparams.y <= 79   && (mouseparams.taste & 1) ) {
       pfield fld = getactfield();
       if ( fieldvisiblenow ( fld ) ) {
          if ( fld->vehicle )
             displaymessage2("damage is %d", fld->vehicle->damage );
          else
          if ( fld->building )
             displaymessage2("damage is %d", fld->building->damage );
          else
          if ( fld->object ) {
             char temp[1000];
             strcpy ( temp, "damage is " );
             for ( int i = fld->object->objnum-1; i >= 0; i-- )
                if ( fld->object->object[i]->typ->armor >= 0 ) {
                   strcat ( temp, strrr ( fld->object->object[i]->damage ));
                   strcat ( temp, " ");
                }

             displaymessage2( temp );
          }

       }
       while ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )  &&   mouseparams.y >= 71    &&   mouseparams.y <= 79   && (mouseparams.taste & 1) )
          releasetimeslice();
   }

   if ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )   &&   mouseparams.y >= 59    &&   mouseparams.y <= 67   && (mouseparams.taste & 1) ) {
       pfield fld = getactfield();
       if ( fieldvisiblenow ( fld ) ) {
          if ( fld->vehicle )
             displaymessage2("unit has %d fuel", fld->vehicle->tank.fuel );
       }
       while ( mouseparams.x >= agmp->resolutionx - ( 640 - 501 )   &&   mouseparams.x <= agmp->resolutionx - ( 640 - 573 )   &&   mouseparams.y >= 59    &&   mouseparams.y <= 67   && (mouseparams.taste & 1) )
          releasetimeslice();
   }
}



void   tdashboard :: paintvehicleinfo( const pvehicle     vehicle,
                                       const pbuilding    building,
                                       const pobjectcontainer      object,
                                       const pvehicletype vt )
{
   collategraphicoperations cgo ( agmp->resolutionx - 800 + 610, 15, agmp->resolutionx - 800 + 783, 307 );

   int         ms;

   npush( activefontsettings );
   ms = getmousestatus();
   if (ms == 2) mousevisible(false);
   dashboard.backgrndcol    = 24;
   dashboard.vgcol          = green;    /* 26 / 76  */
   dashboard.ymx            = 471;    /*  469 / 471  */
   dashboard.ymn            = 380;
   dashboard.ydl            = dashboard.ymx - dashboard.ymn;
   dashboard.munitnumberx   = 545;
   dashboard.vehicle        = vehicle;
   dashboard.building       = building;
   dashboard.object         = object;
   dashboard.vehicletype    = vt;

   dashboard.paintheight();
   dashboard.paintweapons();
   dashboard.paintdamage();
   dashboard.painttank();
   dashboard.paintexperience();
   dashboard.paintmovement();
   dashboard.paintarmor();

   if ( CGameOptions::Instance()->smallmapactive )
      dashboard.paintsmallmap( dashboard.repainthard );
   else
      dashboard.paintwind( dashboard.repainthard );

   dashboard.paintname();
   dashboard.paintclasses ();
   dashboard.paintimage();
   dashboard.paintplayer();
  #ifndef FREEMAPZOOM
   dashboard.paintalliances();
  #else
   dashboard.paintzoom();
  #endif
   dashboard.x = getxpos();
   dashboard.y = getypos();
   if (ms == 2) mousevisible(true);
   npop( activefontsettings );

   dashboard.repainthard = 0;

   if ( actmap && actmap->ellipse )
      actmap->ellipse->paint();

}   /*  paintvehicleinfo  */
