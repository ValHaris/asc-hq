//     $Id: view.cpp,v 1.2 1999-11-16 03:42:49 tmwilson Exp $
//
//     $Log: not supported by cvs2svn $
//
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

// included from CONTROLS.CPP

                                    
class tdrawgettempline : public tdrawline8 {
               public:
                  int tempsum;
                  tdrawgettempline ( void ) 
                    { 
                       tempsum = 0; 
                    };

                  virtual void putpix8 ( int x, int y )
                     {
                        tempsum += getfield ( x, y )->typ->basicjamming;
                     };
              };

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////      
//////                               S I C H T                                               ///////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////



//////////// tcomputeview          ////////////////////////////////


void         tcomputeview::initviewcalculation(  word maxviewweite, word sx, word sy )
{ 
   viewweite = maxviewweite; 

   initsuche(sx,sy,( (viewweite >> 2) + 5),1);
} 


char*        tcomputeview::getvar ( tvisibilitytempbuf num )
{
   if ( num == vsight )
      return &getfield(xp,yp)->temp;
   else
      if ( num == vjamming )
         return &getfield(xp,yp)->temp2;
      else
         if ( num == vfeatures )
            return &getfield(xp,yp)->temp3;

   return NULL;
}


void         tcomputeview::testfield(void)
{ 

   if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) { 
      int f = beeline(startx,starty,xp,yp);
      pfield efield = getfield(xp,yp); 

      if ( viewweite && ( f <= 15 ))
         *getvar ( vfeatures )  |= ( 1 << tempcompleteshr ) | ( sonar << tempsubshr ) | ( satellitenview << tempsatshr ) | ( minenview << tempmineshr );


      int str = viewweite;

      if ( gameoptions.visibility_calc_algo == 1 ) {
         int x = startx ;
         int y = starty ;
         do {
            int d = getdirection ( x, y, xp, yp );
            getnextfield ( x, y, d );
            str -= getfield(x,y)->typ->basicjamming + actmap->weather.fog ;
            if ( d & 1 )
               str-=8;
            else
               str-=12;
         } while ( x != xp  || y != yp );
      } else {
        tdrawgettempline lne;
        lne.start ( startx, starty, xp, yp );
        str -= f;
        str -= lne.tempsum;
      }

      if ( str > 0 ) { 
          char* c = getvar ( vsight );

         if ( *c > ( tfieldtemp2max - str ) )
            *c = tfieldtemp2max;
         else
            *c += str;

         *getvar ( vfeatures ) |= ( sonar << tempsubshr ) | ( satellitenview << tempsatshr ) | ( minenview << tempmineshr );
      } 
   } 
} 


//////////// tcomputevehicleview          ////////////////////////////////


void         tcomputevehicleview::initviewcalculation(word maxviewweite, word sx, word sy )
{ 
   pfield efield = getfield(sx,sy); 
   xp = sx;
   yp = sy;
   if ( efield->object && efield->object->mine  && ((((efield->object->mine >> 1) & 7) == player) || minenview) )
       *getvar ( vfeatures ) |=   ( 1 << tempcompleteshr ) | ( minenview << tempmineshr );
   else 
       *getvar ( vfeatures ) |=   ( 1 << tempcompleteshr ) ;
                     
   tcomputeview::initviewcalculation(maxviewweite,sx,sy);

} 


void         tcomputevehicleview::init( const pvehicle eht )
{ 
   player = eht->color / 8;
   
   satellitenview = !!(eht->functions & cfsatellitenview);
   sonar =           !!(eht->functions & cfsonar);
   minenview =      !!(eht->functions & cfmineview);
   tcomputeview::init();

   if ( eht->functions & cfautodigger )
      searchforminablefields ( eht );
} 








char* tcomputevehicleviewmove_forstaticunit::getvar ( tvisibilitytempbuf num )
{
   if ( actmap->shareview && actmap->shareview->mode[color][actview]==sv_shareview )
     return visibuf->value ( xp, yp, actview, num );

   else
     return visibuf->value ( xp, yp, color, num );
}




void         tcomputevehicleviewmove_formovingunit::testfield(void)
{ 
      if ((xp >= visibuf->outerrect.x1) && (yp >= visibuf->outerrect.y1) && (xp <= visibuf->outerrect.x2) && (yp <= visibuf->outerrect.y2)) 
         tcomputevehicleview::testfield();
} 















//////////// tcomputebuildingview          ////////////////////////////////


void         tcomputebuildingview::initbsb( const pbuilding    bld,
                                           integer      sx,
                                           integer      sy)
{ 
   player = bld->color / 8;

  integer      a, b, c; 

   if (bld->completion == bld->typ->construction_steps - 1)
      c = bld->typ->view; 
   else 
      c = 15; 

   initviewcalculation(c,sx,sy); 
   sonar = (boolean) (bld->typ->special & cgsonarb); 

   minenview = true;
   satellitenview = true;

   building = bld; 
   c = 0; 

   orgx = sx - building->typ->entry.x; 
   orgy = sy - building->typ->entry.y; 

   dx = orgy & 1; 

   orgx = orgx + (dx & (~bld->typ->entry.y));



   for (a = orgx; a <= orgx + 3; a++) 
      for (b = orgy; b <= orgy + 5; b++) 
         if ( building->getpicture ( a - orgx, b - orgy )) {
            xp = a - (dx & b);
            yp = b;
            *getvar ( vfeatures )  |= ( 1 << tempcompleteshr ) | ( minenview << tempmineshr );
         } 
} 






char* tcomputebuildingviewmove::getvar ( tvisibilitytempbuf num )
{
   if ( actmap->shareview && actmap->shareview->mode[color][actview]==sv_shareview )
     return visibuf->value ( xp, yp, actview, num );

   else
     return visibuf->value ( xp, yp, color, num );
}


void         tcomputebuildingviewmove::testfield(void)
{ 
    if ((xp >= visibuf->outerrect.x1) && (yp >= visibuf->outerrect.y1) && (xp <= visibuf->outerrect.x2) && (yp <= visibuf->outerrect.y2)) 
       tcomputebuildingview::testfield();
} 





////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////      
//////                               S T ™ R U N G                                           ///////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


void         tcomputejamming::initstoercalculation(integer      maxjamming,
                                                  integer      sx,
                                                  integer      sy)
{ 
   stoerstaerke = maxjamming; 
   initsuche(sx,sy,stoerstaerke / minmalq,0); 
} 


char*        tcomputejamming::getvar ( tvisibilitytempbuf num )
{
   if ( num == vsight )
      return &getfield(xp,yp)->temp;
   else
      if ( num == vjamming )
         return &getfield(xp,yp)->temp2;
      else
         if ( num == vfeatures )
            return &getfield(xp,yp)->temp3;

   return NULL;
}



void         tcomputejamming::testfield(void)
{ 
   if ((xp >= 0) && (yp >= 0) && (xp < actmap->xsize) && (yp < actmap->ysize)) { 
      int w = stoerstaerke - beeline(startx,starty,xp,yp); 

      if ( w > 0 ) { 
         char* c = getvar ( vjamming );
         if ( *c > ( tfieldtemp2max - w ) )
            *c = tfieldtemp2max;
         else
            *c += w;
      } 
   } 
} 



void         tcomputejamming::startsuche(void)
{ 
   tsearchfields::startsuche();
   pfield efield = getfield(startx,starty); 
   xp = startx;
   yp = starty;

   
   char* c = getvar ( vjamming );
   if ( *c > ( tfieldtemp2max - stoerstaerke ) )
      *c = tfieldtemp2max;
   else
      *c += stoerstaerke;
} 














void         tcomputevehiclejamming::run( const pvehicle     pe)
{ 
   initstoercalculation(pe->typ->jamming,pe->xpos,pe->ypos); 
   startsuche(); 
} 



void         tcomputevehiclejammingmove_formovingunits::testfield(void)
{ 
      if ((xp >= visibuf->outerrect.x1) && (yp >= visibuf->outerrect.y1) && (xp <= visibuf->outerrect.x2) && (yp <= visibuf->outerrect.y2)) 
         tcomputevehiclejamming::testfield();
} 


void         tcomputevehiclejammingmove_formovingunits::run( const pvehicle     pe, int x, int y)
{ 
   initstoercalculation(pe->typ->jamming, x, y ); 
   startsuche(); 
} 

/*
void         tcomputevehiclejammingmove_formovingunits::testfield(void)
{ 
    if ((xp >= visibuf->outerrect.x1) && (yp >= visibuf->outerrect.y1) && (xp <= visibuf->outerrect.x2) && (yp <= visibuf->outerrect.y2)) {
      int w = stoerstaerke - beeline(startx,starty,xp,yp); 

      if ( w > 0 ) 
         getfield( xp, yp )->temp = w;

   } 
} 
*/

char* tcomputevehiclejammingmove_forstaticunits::getvar ( tvisibilitytempbuf num )
{
   if ( actmap->shareview && actmap->shareview->mode[color][actview]==sv_shareview )
     return visibuf->value ( xp, yp, actview, num );

   else
     return visibuf->value ( xp, yp, color, num );
}



/*
void         tcomputevehiclejammingmove_forstaticunits::testfield(void)
{ 
    if ((xp >= visibuf->outerrect.x1) && (yp >= visibuf->outerrect.y1) && (xp <= visibuf->outerrect.x2) && (yp <= visibuf->outerrect.y2)) 
       tcomputevehiclejamming :: testfield ();
} 
*/






void         tcomputebuildingjamming::run( const pbuilding    pe)
{ 
   initstoercalculation(pe->typ->jamming,pe->xpos,pe->ypos); 
   startsuche(); 
} 





char* tcomputebuildingjammingmove::getvar ( tvisibilitytempbuf num )
{
   if ( actmap->shareview && actmap->shareview->mode[color][actview]==sv_shareview )
     return visibuf->value ( xp, yp, actview, num );

   else
     return visibuf->value ( xp, yp, color, num );
}


void         tcomputebuildingjammingmove::testfield(void)
{ 
      if ((xp >= visibuf->outerrect.x1) && (yp >= visibuf->outerrect.y1) && (xp <= visibuf->outerrect.x2) && (yp <= visibuf->outerrect.y2)) 
         tcomputebuildingjamming::testfield();
} 







