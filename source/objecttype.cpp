/***************************************************************************
                          objecttype.cpp  -  description
                             -------------------
    begin                : Fri Jul 27 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>

#include "objecttype.h"
#include "basegfx.h"
#include "graphicset.h"
#include "textfileparser.h"
#include "textfiletags.h"
#include "sgstream.h"
#include "textfile_evaluation.h"
#include "mapalgorithms.h"
#include "graphics/blitter.h"

#ifndef converter
#include "gamemap.h"
#endif


ObjectType :: FieldModification::FieldModification()
{
   terrain_and.flip();
}

ObjectType :: ObjectType ( void )
{
   groupID = -1;

   displayMethod = 0;
   canExistBeneathBuildings = false;
   netBehaviour = 0;
   viewbonus_abs = -1;
   viewbonus_plus = 0;
   imageHeight = 0;
   physicalHeight = 0;
}

ObjectType::FieldModification&  ObjectType::getFieldModification ( int weather )
{
   if ( this->weather.test( weather ))
      return fieldModification[weather];
   else
      return fieldModification[0];
}

bool  ObjectType :: buildable ( pfield fld )
{
   #ifndef converter
   if ( fld->building )
      return false;

   if ( getFieldModification( fld->getweather() ).terrainaccess.accessible ( fld->bdt ) <= 0 )
       return false;

   #endif
   return true;
}

int ObjectType :: getEffectiveHeight()
{
  return 1 << physicalHeight;
}


Surface& ObjectType :: getPicture ( int i, int w )
{
   if ( !weather.test(w) )
      w = 0;

   if ( weatherPicture[w].images.size() <= i )
      i = 0;

   if ( weatherPicture[w].bi3pic[i] > 0 )
      return GraphicSetManager::Instance().getPic(weatherPicture[w].bi3pic[i]);
   else
      return weatherPicture[w].images[i];
}




void ObjectType :: display ( Surface& surface, SPoint pos, int dir, int weather )
{
   vector<int> dirsToDisplay;
   
   int actualDir = dir;

   if ( id == 4 ) {
     switch ( dir ) {
        case  68 : dirsToDisplay.push_back(  9 ); break;
        case  34 : dirsToDisplay.push_back( 10 ); break;
        case  17 : dirsToDisplay.push_back( 11 ); break;
        case 136 : dirsToDisplay.push_back( 12 ); break;
        case   0 : dirsToDisplay.push_back(  0 ); break;
        default  : {
           for (int i = 0; i <= 7; i++)
              if ( dir & (1 << i))
                 dirsToDisplay.push_back( i+1 );
        
        }
     }
   } else
      dirsToDisplay.push_back( dir );
 
   for ( vector<int>::iterator i = dirsToDisplay.begin(); i != dirsToDisplay.end(); ++i ) {
      int flip = 0;
      if ( *i < weatherPicture[weather].flip.size() )
         flip = weatherPicture[weather].flip[*i];
         
      if ( id == 7 || id == 30 || displayMethod==1 ) { // buried pipeline,
         megaBlitter<ColorTransform_None, ColorMerger_AlphaShadow, SourcePixelSelector_Flip>(getPicture( dir, weather), surface, pos, nullParam,nullParam, flip); 
      } else
         if ( displayMethod == 2 ) {  // translation
            megaBlitter<ColorTransform_None, ColorMerger_Alpha_XLAT_TableShifter, SourcePixelSelector_Flip>(getPicture( dir, weather), surface, pos, nullParam, xlattables.nochange, flip); 
         } else
            if ( displayMethod == 4 ) {
               megaBlitter<ColorTransform_None, ColorMerger_AlphaMixer, SourcePixelSelector_Flip>(getPicture( dir, weather), surface, pos, nullParam,nullParam, flip); 
            } else {
               bool disp = true;
               #ifndef karteneditor
               if ( displayMethod == 3 ) // mapeditorOnly
                  disp = false;
               #endif
               if ( disp ) {
                  if ( flip )
                     megaBlitter<ColorTransform_None, ColorMerger_AlphaOverwrite, SourcePixelSelector_Flip>(getPicture( dir, weather), surface, pos, nullParam,nullParam, flip); 
                  else   
                     megaBlitter<ColorTransform_None, ColorMerger_AlphaOverwrite, SourcePixelSelector_Plain>(getPicture( dir, weather), surface, pos, nullParam,nullParam,nullParam); 
                     // surface.Blit(getPicture(dir,weather),pos );
               }
            }   
   }
}



void ObjectType :: display ( Surface& surface, SPoint pos )
{
   display ( surface, pos, 34, 0 );
}




#ifndef converter

namespace ForestCalculation {

/*
   These smoothing functions are straight conversions from Joerg Richter's routines
   he made for his Battle Isle Map editor
   Many thanks for giving them to me !
*/

const int woodformnum = 28;
int woodform[ woodformnum ] = { 63,30,60,39,51,28,35,48,6,57,15,14,56,7,49,47,59,31,61,60,55, -1,-1,-1,-1,-1,-1,-1 };


int SmoothTreesData0[] = {
     4, 7, 10,101,
     1,0x0001,243,
     1,0x0115,243,          // die die garnicht gehen
    30,0x3F, 30,243,0x3F, 60,243,0x3F, 39,243,0x3F, 51,243,
       0x3F, 28,243,0x3F, 35,243,0x3F, 48,243,0x3F,  6,243,
       0x3F, 57,243,0x3F, 15,243,0x3F, 14,243,0x3F, 56,243,
       0x3F,  7,243,0x3F, 49,243,0x3F, 47,243,0x3F, 59,243,
       0x3F, 31,243,0x3F, 61,243,0x3F, 62,243,0x3F, 55,243,
       0x3F, 23,243,0x3F, 46,243,0x3F, 29,243,0x3F, 58,243,
       0x3F, 53,243,0x3F, 43,243,0x3F, 22,243,0x3F, 38,243,
       0x3F, 50,243,0x3F, 52,243,
     7,264,265,266,267,268,269,270
  };

int SmoothTreesData[] = {
     4, 7, 10,101,
     1,0x0001,243,
     1,0x0115,243,
    30,0x3F, 30,244,0x3F, 60,245,0x3F, 39,246,0x3F, 51,247,
       0x3F, 28,248,0x3F, 29,248,0x3F, 35,249,0x3F, 43,249,
       0x3F, 48,250,0x3F, 50,250,0x3F, 52,250,0x3F,  6,251,
       0x3F, 22,251,0x3F, 38,251,0x3F, 57,252,0x3F, 15,253,
       0x3F, 14,254,0x3F, 46,254,0x3F, 56,255,0x3F, 58,255,
       0x3F,  7,256,0x3F, 23,256,0x3F, 49,257,0x3F, 53,257,
       0x3F, 47,258,0x3F, 59,259,0x3F, 31,260,0x3F, 61,261,
       0x3F, 62,262,0x3F, 55,263,
     7,264,265,266,267,268,269,270
  };

int UnSmoothTreesData[] = {
     4, 7, 8, 9,
     1,0x011C,243,
     0,
     0,
     1,243
  };




int  SmoothBanksData [] = {
     4, 7, 16, 77,
     1, 0x0103, 95,    // was zu ersetzen ist         - blaues wasser }
     4, 0x010F, 95,    // was als 1 zu betrachten ist - wasser und strand }
        0x010E,  0,                                // - Hafen }
        0x0109,110,                                // - Steine und Schilf }
        0x0107,121,                                // - Schilf }
    20,0x3F,59, 98, 0x3F,51, 98, 0x3F,47, 99, 0x3F,39, 99, // durch was ersetzen }
       0x3F,31,100, 0x3F,30,100, 0x3F,61,101, 0x3F,60,101,
       0x3F,55,102, 0x3F,62,103, 0x3F,35,104, 0x3F,28,105,
       0x3F,56,106, 0x3F,48,106, 0x3F,49,106, 0x3F,57,106,
       0x3F,14,107, 0x3F,15,107, 0x3F, 7,107, 0x3F, 6,107,
     0                                            // wenn nicht gefunden }
  };

int  UnSmoothBanksData [] = {
     4, 7, 8, 9,
     1, 0x010C, 98,          // { alle str"nder ersetzen }
     0,
     0,
     1, 95                  // durch flaches wasser ersetzen }
  };


int  SmoothDarkBanksData [] = {
     4, 7, 16, 77,
     1,0x0103,385,                       // dunkels wasser }
     4,0x0103,385,                       // dunkles wasser }
       0x010A,373,
       0x0104,449,
       0x0104,463,
    20,0x3F,59,373, 0x3F,51,373, 0x3F,47,374, 0x3F,39,374,          // durch was ersetzen }
       0x3F,31,375, 0x3F,30,375, 0x3F,61,376, 0x3F,60,376,
       0x3F,55,377, 0x3F,62,378, 0x3F,35,379, 0x3F,28,380,
       0x3F,56,381, 0x3F,48,381, 0x3F,49,381, 0x3F,57,381,
       0x3F,14,382, 0x3F,15,382, 0x3F, 7,382, 0x3F, 6,382,
     0                                       // wenn nicht gefunden }
  };

int  UnSmoothDarkBanksData [] = {
     4, 7, 8, 9,
     1,0x010A,373,
     0,
     0,
     1,385
  };



class Smoothing {
         pmap actmap;
       public:
         Smoothing ( pmap gamemap ) : actmap ( gamemap ) {};
         pfield getfield ( int x, int y )
         {
            return actmap->getField ( x, y );
         }

         int IsInSetOfWord( int Wert, int* A )
         {
           int Pos = 0;
           int Anz1 = A[Pos];
           Pos++;
           int res = 0;
           int Anz2;
           while ( Anz1 > 0 ) {
             int W = A[Pos];
             Pos++;
             Anz2 = W & 0xff;

             if ( W & 0x100 ) {
                if (( Wert>= A[Pos]) && (Wert< A[Pos]+Anz2))
                     res = 1;
                   Pos++;
             } else {
                while ( Anz2 > 0) {
                  if ( Wert == A[Pos] )
                     res = 1;
                  Pos++;
                  Anz2--;
                }
             }

             Anz1--;
           }

           return res;
         };


         int  GetNeighbourMask( int x, int y, int* Arr, pobjecttype o )
         {
            int res = 0;
            for ( int d = 0; d < sidenum; d++ ) {
               int x1 = x;
               int y1 = y;
               getnextfield ( x1, y1, d );
               pfield fld = getfield ( x1, y1 );
               if ( fld ) {

                  pobject obj = fld->checkforobject ( o );
                  if ( obj )
                     if ( obj->typ->weather.test(0) )
                        if ( IsInSetOfWord ( obj->typ->weatherPicture[0].bi3pic[ obj->dir ], Arr ))
                           res += 1 << d;

                  // if ( fld->checkforobject ( o ) )
                  //    res += 1 << d;
               } else
                  res += 1 << d;

            }
            return res;
         };

         int  GetNeighbourMask( int x, int y, int* Arr )
         {
            int res = 0;
            for ( int d = 0; d < sidenum; d++ ) {
               int x1 = x;
               int y1 = y;
               getnextfield ( x1, y1, d );
               pfield fld = getfield ( x1, y1 );
               if ( fld ) {

                  if ( IsInSetOfWord ( fld->typ->bi_pict, Arr ))
                     res += 1 << d;

               } else
                  res += 1 << d;

            }
            return res;
         };


         /*
               Res:= 0;
               for I:= Oben to LOben do begin
                 GetNear(P, I, R);
                 if ValidEck(R) then
                   Res:= Res or
          (Byte(IsInSetOfWord(  TRawArrEck(   Mission.ACTN[R.Y, R.X] )[TerObj], Arr)) shl Ord(I))
                 else
                   Res:= Res or 1 shl Ord(I);
               end;
               GetNeighbourMask:= Res;
             end;
         */


         int SearchAndGetInt( int Wert, int* Arr, int* Res )
         {
            int Anz = Arr[0];
            int Pos = 1;
            while ( Anz> 0 ) {
              if (( Wert & Arr[Pos]) == Arr[Pos+1] ) {
                 *Res = Arr[Pos+2];
                 return  Anz > 0;
              }
              Pos += 3;
              Anz--;
            }
            return  Anz> 0;
         };


         int SmoothIt( pobjecttype TerObj, int* SmoothData )
         {
           int P0 = SmoothData[0];
           int P1 = SmoothData[1];
           int P2 = SmoothData[2];
           int P3 = SmoothData[3];
           int Res = 0;
           for ( int Y = 0 ; Y < actmap->ysize; Y++ )
             for ( int X = 0; X < actmap->xsize; X++ ) {
                 if ( TerObj ) {
                    pobject obj = getfield ( X, Y )-> checkforobject ( TerObj );
                    if ( obj  && obj->typ->weather.test(0) ) {
                       int Old = obj->dir; // bipicnum
                                           //    Old:= TRawArrEck(Mission.ACTN[Y, X])[TerObj];  // bisherige Form / oder Bildnummer ?

                       if ( IsInSetOfWord( obj->typ->weatherPicture[0].bi3pic[ obj->dir ], &SmoothData[P0] )) {    // Nur die "allesWald"-fielder werden gesmootht
                          int Mask = GetNeighbourMask( X, Y, &SmoothData[P1], TerObj );
                          if ( Mask < 63 ) {
                             int nw;
                             if ( !SearchAndGetInt(Mask, &SmoothData[P2], &nw) ) {  // Wenn kein passendes field gefunden wurde
                                if ( SmoothData[P3] == 0  ||  SmoothData[P3] == 1 )
                                   nw = SmoothData[P3+ 1];
                                else
                                   nw = SmoothData[P3+ 1 ]; // + (ticker % SmoothData[P3] )
                             }
                             for ( int i = 0; i < TerObj->weatherPicture[0].bi3pic.size(); i++ )
                                if ( TerObj->weatherPicture[0].bi3pic[ i ] == nw )
                                   obj->dir = i;
                          }
                       }
                       if ( Old != obj->dir )
                          Res = 1;
                    }
                 } else {
                    pfield fld = getfield ( X, Y );
                    TerrainType::Weather* old = fld->typ;
                    int odir = fld->direction;

                    if ( IsInSetOfWord( fld->typ->bi_pict, &SmoothData[P0] )) {    // Nur die "allesWald"-fielder werden gesmootht
                       int Mask = GetNeighbourMask( X, Y, &SmoothData[P1] );
                       if ( Mask < 63 ) {
                          int nw;
                          if ( !SearchAndGetInt(Mask, &SmoothData[P2], &nw) ) {  // Wenn kein passendes field gefunden wurde
                             if ( SmoothData[P3] == 0  ||  SmoothData[P3] == 1 )
                                nw = SmoothData[P3+ 1];
                             else
                                nw = SmoothData[P3+ 1 ]; // + (ticker % SmoothData[P3] )
                          }
                          /*
                          for ( int i = 0; i < terrainTypeRepository.getNum; i++ ) {
                             pterraintype trrn = terrainTypeRepository.getObject_byPos( i );
                             if ( trrn )
                                for ( int j = 0; j < cwettertypennum; j++ )
                                   if ( trrn->weather[j] )
                                      for ( int k = 0; k < sidenum; k++ )
                                         if ( trrn->weather[j]->pict )
                                            if ( trrn->weather[j]->bi_pict == nw ) {
                                               fld->typ = trrn->weather[j];
                                               fld->direction = k;
                                               fld->setparams();
                                            }
                          }
                          */
                       }
                    }
                    if ( old != fld->typ  ||  odir != fld->direction )
                       Res = 1;

                 }
              }
           return Res;
         };



         void smooth ( int what, pobjecttype woodObj )
         {
           int ShowAgain = 0;
           if ( what & 2 ) {
             if ( SmoothIt( NULL, UnSmoothBanksData) )
                ShowAgain = 1;
             if ( SmoothIt( NULL, UnSmoothDarkBanksData) )
                ShowAgain = 1;
             if ( SmoothIt( NULL, SmoothBanksData) )
                ShowAgain = 1;
             if ( SmoothIt( NULL, SmoothDarkBanksData) )
                ShowAgain = 1;
           }

           if ( what & 1 ) {
              if ( woodObj  ) {
                int count = 0;
                while ( SmoothIt ( woodObj, SmoothTreesData0 ) && count < 20 ) {
                   ShowAgain = 1;
                   count++;
                }
                if  ( SmoothIt ( woodObj, SmoothTreesData) )
                   ShowAgain = 1;
              }
           }
         /*    while SmoothIt(1, SmoothDarkTreesData0) do ShowAgain:= true;
             if SmoothIt(1, SmoothDarkTreesData) then ShowAgain:= true;
             if ShowAgain then begin
               ShowAll;
               Repaint;
             end;    */
         };

};


/*
procedure TMissView.Smooth;
  var
    ShowAgain: Boolean;
  begin
    ShowAgain:= false;
    if SmoothIt(0, SmoothBanksData) then ShowAgain:= true;
    if SmoothIt(0, SmoothDarkBanksData) then ShowAgain:= true;

    while SmoothIt(1, SmoothTreesData0) do ShowAgain:= true;
    if SmoothIt(1, SmoothTreesData) then ShowAgain:= true;
{    while SmoothIt(1, SmoothDarkTreesData0) do ShowAgain:= true;
    if SmoothIt(1, SmoothDarkTreesData) then ShowAgain:= true;}
    if ShowAgain then begin
      ShowAll;
      Repaint;
    end;
  end;

function IsInSetofWord(Wert: Word; var A: array of Word): Boolean;
  var
    Anz1: Word;
    Anz2: Word;
    W: Word;
    Pos: Word;
  begin
    Pos:= 0;
    Anz1:= A[Pos];
    Inc(Pos);
    IsInSetOfWord:= false;
    while Anz1> 0 do begin
      W:= A[Pos];
      Inc(Pos);
      Anz2:= Lo(W);
      case Hi(W) of
        0: begin
            while Anz2> 0 do begin
              if Wert= A[Pos] then IsInSetOfWord:= true;
              Inc(Pos);
              Dec(Anz2);
            end;
          end;
        1: begin
            if (Wert>= A[Pos]) and (Wert< A[Pos]+Anz2) then
              IsInSetOfWord:= true;
            Inc(Pos);
          end;
      end;
      Dec(Anz1);
    end;
  end;



****************
Smoothdaten
****************

{
  Aufbau eines Set of Ints

  1. word anzahl der bl"cke
  dann folgen die bl"cke

  aufbau eines blocks:
    1. word:
       hibyte- optionsnummer
         0- nur rawdaten   lobyte ist anzahl der folgenden raws
         1- ab hier        lobyte ist anzahl der ab hier
    weitere ist entweder raw oder startzahl
}


  SmoothBanksData: array[0..77] of Word= (
     4, 7, 16, 77,
     1, $0103, 95,    { was zu ersetzen ist         - blaues wasser }
     4, $010F, 95,    { was als 1 zu betrachten ist - wasser und strand }
        $010E,  0,                                { - Hafen }
        $0109,110,                                { - Steine und Schilf }
        $0107,121,                                { - Schilf }
    20,$3F,59, 98, $3F,51, 98, $3F,47, 99, $3F,39, 99, { durch was ersetzen }
       $3F,31,100, $3F,30,100, $3F,61,101, $3F,60,101,
       $3F,55,102, $3F,62,103, $3F,35,104, $3F,28,105,
       $3F,56,106, $3F,48,106, $3F,49,106, $3F,57,106,
       $3F,14,107, $3F,15,107, $3F, 7,107, $3F, 6,107,
     0                                            { wenn nicht gefunden }
  );

  UnSmoothBanksData: array[0..10] of Word= (
     4, 7, 8, 9,
     1, $010C, 98,          { alle str"nder ersetzen }
     0,
     0,
     1, 95                  { durch flaches wasser ersetzen }
  );


  SmoothDarkBanksData: array[0..77] of Word= (
     4, 7, 16, 77,
     1,$0103,385,                       { dunkels wasser }
     4,$0103,385,                       { dunkles wasser }
       $010A,373,
       $0104,449,
       $0104,463,
    20,$3F,59,373, $3F,51,373, $3F,47,374, $3F,39,374,          { durch was ersetzen }
       $3F,31,375, $3F,30,375, $3F,61,376, $3F,60,376,
       $3F,55,377, $3F,62,378, $3F,35,379, $3F,28,380,
       $3F,56,381, $3F,48,381, $3F,49,381, $3F,57,381,
       $3F,14,382, $3F,15,382, $3F, 7,382, $3F, 6,382,
     0                                       { wenn nicht gefunden }
  );

  UnSmoothDarkBanksData: array[0..10] of Word= (
     4, 7, 8, 9,
     1,$010A,373,
     0,
     0,
     1,385
  );

*/


void smooth ( int what, pmap gamemap, pobjecttype woodObj )
{
  Smoothing s ( gamemap );
  s.smooth ( what, woodObj );
}


void calculateforest( pmap actmap, pobjecttype woodObj )
{
   for ( int y = 0; y < actmap->ysize ; y++)
     for ( int x = 0; x < actmap->xsize ; x++) {
        pfield fld = actmap->getField(x,y);

        for ( tfield::ObjectContainer::iterator i = fld->objects.begin(); i != fld->objects.end(); i++ )
           if ( i->typ == woodObj )
              i->dir = 0;
     }

   Smoothing s ( actmap );
   s.smooth( 1, woodObj );
   return;

   int run = 0;
   int changed ;
   do {
      changed = 0;
      for ( int y = 0; y < actmap->ysize ; y++)
         for ( int x = 0; x < actmap->xsize ; x++) { 
            pfield fld = actmap->getField(x,y);
   
            for ( tfield::ObjectContainer::iterator o = fld->objects.begin(); o != fld->objects.end(); o++ )
               if ( o->typ == woodObj ) {
                  int c = 0;
                  for ( int i = 0; i < sidenum; i++) {
                     int a = x;
                     int b = y;
                     getnextfield( a, b, i );
                     pfield fld2 = actmap->getField(a,b);

                     if ( fld2 ) {
                        pobject oi = fld2->checkforobject ( o->typ );
                        if ( oi )
                           if ( oi->dir <= 20  ||  run == 0 )
                              c |=  1 << i ;
                      }
                  }

                  // int found = 0;
                  int dr;
                  for ( int j = 0; j < woodformnum; j++ )
                     if ( woodform[j] == c ) {
                        dr = j;
                        // found = 1;
                     }

//                  if ( !found )
//                     dr = 21 + ticker % 7;

                  if ( o->dir != dr  && !(o->dir >= 21  && dr >= 21)) {
                     o->dir = dr;
                     fld->setparams();
                     changed = 1;
                  }
               }
         } 
      run++;
   } while ( changed );
}

} // namespace forestcalculation


void         calculateobject( int       x,
                              int       y,
                              bool      mof,
                              pobjecttype obj,
                              pmap actmap )
{
   if ( obj->netBehaviour & ObjectType::SpecialForest ) {
      ForestCalculation::calculateforest( actmap, obj );
      return;
   }

   pfield fld = actmap->getField(x,y) ;
   pobject oi2 = fld-> checkforobject (  obj  );

   int c = 0;
   for ( int dir = 0; dir < sidenum; dir++) {
      int a = x;
      int b = y;
      getnextfield( a, b, dir );
      pfield fld2 = actmap->getField(a,b);

      if ( fld2 ) {
         for ( int oj = -1; oj < int(obj->linkableObjects.size()); oj++ ) {
            pobject oi;
            if ( oj == -1 )
               oi = fld2->checkforobject ( obj );
            else
               oi = fld2->checkforobject ( actmap->getobjecttype_byid ( obj->linkableObjects[oj] ) );

            if ( oi ) {
               c |=  1 << dir ;
               if ( mof )
                  calculateobject ( a, b, false, oi->typ, actmap );

            }
         }
         for ( unsigned int t = 0; t < obj->linkableTerrain.size(); t++ )
            if ( fld2->typ->terraintype->id == obj->linkableTerrain[t] )
               c |=  1 << dir ;

         if ( fld2->building && !(fld2->building->typ->special & cgnoobjectchainingb) ) {
            if ( (obj->netBehaviour & ObjectType::NetToBuildingEntry)  &&  (fld2->bdt & getTerrainBitType(cbbuildingentry) ).any() )
               c |= 1 << dir;

            if ( obj->netBehaviour & ObjectType::NetToBuildings )
               c |= 1 << dir;
         }

      }
      else {
         if ( obj->netBehaviour & ObjectType::NetToBorder )
            c |= 1 << dir;
      }
   }

   if ( obj->netBehaviour & ObjectType::AutoBorder ) {
      int autoborder = 0;
      int count = 0;
      for ( int dir = 0; dir < sidenum; dir++) {
         int a = x;
         int b = y;
         getnextfield( a, b, dir );
         pfield fld2 = actmap->getField(a,b);
         if ( !fld2 ) {
            // if the field opposite of the border field is connected to, make a straight line out of the map.
            if ( c & (1 << ((dir+sidenum/2) % sidenum ))) {
               autoborder |= 1 << dir;
               count++;
            }
         }
      }
      if ( count == 1 )
         c |= autoborder;
   }

   if ( oi2 ) {
     oi2->setdir ( c );
     fld->setparams();
   }

}




void         calculateallobjects( pmap actmap )
{
   vector<ObjectType*> forestObjects;
   for ( int y = 0; y < actmap->ysize ; y++)
      for ( int x = 0; x < actmap->xsize ; x++) {
         pfield fld = actmap->getField(x,y);

         for ( tfield::ObjectContainer::iterator i = fld->objects.begin(); i != fld->objects.end(); i++ )
             if ( !(i->typ->netBehaviour & ObjectType::SpecialForest) )
                calculateobject( x, y, false, i->typ, actmap );
             else
                if ( find ( forestObjects.begin(), forestObjects.end(), i->typ ) == forestObjects.end())
                   forestObjects.push_back ( i->typ );

         fld->setparams();
      }

   for ( vector<ObjectType*>::iterator i = forestObjects.begin(); i != forestObjects.end(); i++ )
      ForestCalculation::calculateforest( actmap, *i );
}




#else // ifdef converter



#endif





const int object_version = 13;

void ObjectType :: read ( tnstream& stream )
{
   int version = stream.readInt();

   if ( version < 9 )
      fatalError ( "sorry, the old file format for objects cannot be loaded any more" );

   if ( version <= object_version && version >= 9 ) {

       id = stream.readInt();
       groupID = stream.readInt();

       int ___weather = stream.readInt();
       weather.reset();
       for ( int i = 0; i < cwettertypennum; i++ )
          if ( ___weather & ( 1 << i ))
             weather.set ( i );

       visibleago = stream.readInt();

       readClassContainer( linkableObjects, stream );
       readClassContainer( linkableTerrain, stream );

       armor = stream.readInt();

       for ( int i = 0; i < cwettertypennum; ++i ) {
          fieldModification[i].terrainaccess.read( stream );
          fieldModification[i].terrain_and.read ( stream );
          fieldModification[i].terrain_or.read ( stream );
          fieldModification[i].movemalus_plus.read ( stream, 0 );
          fieldModification[i].movemalus_abs.read ( stream, -1 );
       }


       attackbonus_plus = stream.readInt();
       attackbonus_abs  = stream.readInt();

       defensebonus_plus = stream.readInt();
       defensebonus_abs =  stream.readInt();

       basicjamming_plus = stream.readInt();
       basicjamming_abs = stream.readInt();

       if ( version >= 10 ) {
          viewbonus_plus = stream.readInt();
          viewbonus_abs = stream.readInt();
       }

       if ( version <= 10 ) {
         imageHeight = stream.readInt();
         physicalHeight = imageHeight / 30;
       } else {
         imageHeight = stream.readInt();
         physicalHeight = stream.readInt();
       }


       buildcost.read( stream );
       removecost.read ( stream );
       build_movecost = stream.readInt();
       remove_movecost = stream.readInt();

       canExistBeneathBuildings = stream.readInt();

       name = stream.readString();

       netBehaviour = stream.readInt();

       displayMethod = stream.readInt();

       buildIcon.read( stream );
       removeIcon.read( stream );


       techDependency.read ( stream );

       for ( int ww = 0; ww < cwettertypennum; ww++ )
         if ( weather.test ( ww ) ) {

            int pictnum = stream.readInt();
            stream.readInt(  ); // weatherPicture[ww].gfxReference = 

            weatherPicture[ww].bi3pic.resize( pictnum );
            weatherPicture[ww].flip.resize( pictnum );
            weatherPicture[ww].images.resize( pictnum );

            for ( int n = 0; n < pictnum; n++ ) {
               int bi3 = stream.readInt();
               if ( bi3 == 1 ) {
                  weatherPicture[ww].bi3pic[n] = stream.readInt();
                  weatherPicture[ww].flip[n] = stream.readInt();
               } else {
                  weatherPicture[ww].bi3pic[n] = -1;
                  weatherPicture[ww].images[n].read ( stream );
                  if ( object_version >= 13 )
                     weatherPicture[ww].flip[n] = stream.readInt();
                  else   
                     weatherPicture[ww].flip[n] = 0;
               }
            }
         }

   } else
       throw tinvalidversion  ( stream.getLocation(), object_version, version );
}

#if 0
void ObjectType :: setupImages()
{
   int copycount = 0;
   #ifndef converter
   for ( int ww = 0; ww < cwettertypennum; ww++ )
      if ( weather.test( ww ) )
         for ( int n = 0; n < weatherPicture[ww].bi3pic.size(); n++ ) {
            if ( weatherPicture[ww].flip[n] == 1 ) {
               void* buf = new char [ imagesize ( 0, 0, fieldxsize, fieldysize ) ];
               flippict ( weatherPicture[ww].images[n], buf , 1 );
               asc_free ( weatherPicture[ww].images[n] );
               weatherPicture[ww].images[n] = buf;
               copycount++;
            }

            if ( weatherPicture[ww].flip[n] == 2 ) {
               void* buf = new char [ imagesize ( 0, 0, fieldxsize, fieldysize ) ];
               flippict ( weatherPicture[ww].images[n], buf , 2 );
               asc_free ( weatherPicture[ww].images[n] );
               weatherPicture[ww].images[n] = buf;
               copycount++;
            }

            if ( weatherPicture[ww].flip[n] == 3 ) {
               void* buf = new char [ imagesize ( 0, 0, fieldxsize, fieldysize ) ];
               flippict ( weatherPicture[ww].images[n], buf , 2 );
               flippict ( buf, weatherPicture[ww].images[n], 1 );
               asc_free( buf );
               copycount++;
            }


//            if ( weatherPicture[ww].bi3pic[n] == -1 )
//               weatherPicture[ww].flip[n] = 0;
         }
   #endif

   /*
   if ( copycount == 0 )
      for ( int ww = 0; ww < cwettertypennum; ww++ )
         if ( weather.test ( ww ) )
            for ( int n = 0; n < weatherPicture[ww].images.size(); n++ )
               if ( weatherPicture[ww].bi3pic[n] != -1 ) {
                  asc_free ( weatherPicture[ww].images[n] );
                  loadbi3pict_double ( weatherPicture[ww].bi3pic[n],
                                       &weatherPicture[ww].images[n],
                                       1 ); // CGameOptions::Instance()->bi3.interpolate.objects );
               }
   */
}
#endif

void ObjectType :: write ( tnstream& stream ) const
{
    stream.writeInt ( object_version );

    stream.writeInt ( id );
    stream.writeInt ( groupID );

    int ___weather = 0;
    for ( int i = 0; i < cwettertypennum; i++ )
       if ( weather.test ( i ))
          ___weather |= 1 << i;
    stream.writeInt ( ___weather );
    stream.writeInt ( visibleago );

    writeClassContainer ( linkableObjects, stream );
    writeClassContainer ( linkableTerrain, stream );

    stream.writeInt ( armor );

    for ( int i = 0; i < cwettertypennum; i++ ) {
       fieldModification[i].terrainaccess.write( stream );
       fieldModification[i].terrain_and.write ( stream );
       fieldModification[i].terrain_or.write ( stream );
       fieldModification[i].movemalus_plus.write ( stream );
       fieldModification[i].movemalus_abs.write ( stream );
    }


    stream.writeInt ( attackbonus_plus );
    stream.writeInt ( attackbonus_abs );
    stream.writeInt ( defensebonus_plus );
    stream.writeInt ( defensebonus_abs );

    stream.writeInt ( basicjamming_plus );
    stream.writeInt ( basicjamming_abs );

    stream.writeInt ( viewbonus_plus );
    stream.writeInt ( viewbonus_abs );


    stream.writeInt ( imageHeight );
    stream.writeInt ( physicalHeight );

    buildcost.write( stream );
    removecost.write ( stream );
    stream.writeInt( build_movecost );
    stream.writeInt( remove_movecost );

    stream.writeInt( canExistBeneathBuildings );

    stream.writeString ( name );

    stream.writeInt ( netBehaviour );

    stream.writeInt ( displayMethod );

    buildIcon.write( stream );
    removeIcon.write( stream );

    techDependency.write ( stream );


    for ( int ww = 0; ww < cwettertypennum; ww++ )
       if ( weather.test( ww ) ) {
          stream.writeInt( weatherPicture[ww].images.size() );
          stream.writeInt( 0 ); // weatherPicture[ww].gfxReference

          for ( int l = 0; l < weatherPicture[ww].images.size(); l++ ) {
             if ( weatherPicture[ww].bi3pic[l] >= 0 ) {
                stream.writeInt ( 1 );
                stream.writeInt ( weatherPicture[ww].bi3pic[l] );
             } else {
                stream.writeInt ( 2 );
                weatherPicture[ww].images[l].write( stream );
             }
             stream.writeInt ( weatherPicture[ww].flip[l] );
          }
       }
}


void ObjectType :: FieldModification :: runTextIO ( PropertyContainer& pc )
{
   pc.addDFloatArray ( "Movemalus_plus", movemalus_plus );
   int mm = movemalus_plus.size();
   movemalus_plus.resize( cmovemalitypenum );
   for ( int i = mm; i < cmovemalitypenum; i++ ) {
      if ( i == 0 )
         movemalus_plus[i] = 0;
      else
         movemalus_plus[i] = movemalus_plus[0];
   }


   pc.addDFloatArray ( "Movemalus_abs", movemalus_abs );
   mm = movemalus_abs.size();
   movemalus_abs.resize( cmovemalitypenum );
   for ( int i = mm; i < cmovemalitypenum; i++ ) {
      if ( i == 0 )
         movemalus_abs[i] = -1;
      else
         movemalus_abs[i] = movemalus_abs[0];
   }

   pc.openBracket ( "TerrainAccess" );
   terrainaccess.runTextIO ( pc );
   pc.closeBracket ();

   pc.addTagArray ( "TerrainProperties_Filter", terrain_and, cbodenartennum, bodenarten, true );
   pc.addTagArray ( "TerrainProperties_Add", terrain_or, cbodenartennum, bodenarten );
}

void ObjectType :: runTextIO ( PropertyContainer& pc )
{
   pc.addInteger  ( "ID", id );
   pc.addInteger  ( "GroupID", groupID, -1 );
   pc.addTagArray ( "Weather", weather, cwettertypennum, weatherTags );
   pc.addBool     ( "visible_in_fogOfWar", visibleago );
   pc.addIntegerArray ( "LinkableObjects", linkableObjects );
   if ( pc.find ( "LinkableTerrain" ) || !pc.isReading() )
      pc.addIntegerArray ( "LinkableTerrain", linkableTerrain );

   pc.addBool ( "canExistBeneathBuildings", canExistBeneathBuildings, false );

   pc.addInteger  ( "Armor", armor );

   bool oldWeatherSpecification;

   if ( pc.find ( "Movemalus_plus" )) {
      oldWeatherSpecification = true;

      fieldModification[0].runTextIO ( pc );

      for ( int i = 1; i < cwettertypennum; ++i )
         fieldModification[i] = fieldModification[0];

   } else
      oldWeatherSpecification= false;


   pc.addInteger  ( "AttackBonus_abs", attackbonus_abs );
   pc.addInteger  ( "AttackBonus_plus", attackbonus_plus );
   pc.addInteger  ( "DefenseBonus_abs", defensebonus_abs );
   pc.addInteger  ( "DefenseBonus_plus", defensebonus_plus );
   pc.addInteger  ( "Jamming_abs", basicjamming_abs );
   pc.addInteger  ( "Jammming_plus", basicjamming_plus );
   pc.addInteger  ( "Height", imageHeight );
   if ( pc.find ( "PhysicalHeight" ) || !pc.isReading() ) {
      pc.addInteger  ( "PhysicalHeight", physicalHeight );
   } else
      physicalHeight = imageHeight / 30;

   pc.addInteger  ( "ViewBonus_abs", viewbonus_abs, -1 );
   pc.addInteger  ( "ViewBonus_plus", viewbonus_plus, 0 );

   pc.openBracket ( "ConstructionCost" );
   buildcost.runTextIO ( pc );
   pc.addInteger  ( "Movement", build_movecost );
   pc.closeBracket ();

   pc.openBracket ( "RemovalCost" );
   removecost.runTextIO ( pc );
   pc.addInteger  ( "Movement", remove_movecost );
   pc.closeBracket ();

   pc.addString( "Name", name );

   pc.addTagInteger ( "NetBehaviour", netBehaviour, netBehaviourNum, objectNetMethod, int(0) );

   if ( pc.isReading() && pc.find ( "NoSelfChaining" )) {
      bool no_autonet;
      pc.addBool  ( "NoSelfChaining", no_autonet );
      if ( !no_autonet )
         netBehaviour |= NetToSelf;
   }


   for ( int i = 0; i < cwettertypennum; i++ )
      if ( weather.test(i) ) {

         pc.openBracket  ( weatherTags[i] );
         bool bi3pics = false;

         if ( !pc.isReading() )
            for ( int j = 0; j < weatherPicture[i].bi3pic.size(); j++ )
                if ( weatherPicture[i].bi3pic[j] >= 0 )
                   bi3pics = true;

         pc.addBool  ( "UseGFXpics", bi3pics );
         if ( bi3pics ) {
            pc.addIntegerArray ( "GFXpictures", weatherPicture[i].bi3pic );
            pc.addIntegerArray ( "FlipPictures", weatherPicture[i].flip );
            int oldsize = weatherPicture[i].flip.size();
            weatherPicture[i].flip.resize( weatherPicture[i].bi3pic.size() );
            weatherPicture[i].images.resize( weatherPicture[i].bi3pic.size() );
         } else {
            ASCString s = extractFileName_withoutSuffix( filename );
            if ( s.empty() ) {
               s = "object";
               s += strrr(id);
            }
            pc.addImageArray ( "picture",   weatherPicture[i].images, s + weatherAbbrev[i] );
            weatherPicture[i].bi3pic.resize( weatherPicture[i].images.size() );
            weatherPicture[i].flip.resize( weatherPicture[i].images.size() );

            if ( pc.find ( "FlipPictures" ) ) {
               vector<int>   imgReferences;
               imgReferences.resize ( weatherPicture[i].images.size() );

               for ( int j = 0; j < weatherPicture[i].images.size(); j++ ) {
                  weatherPicture[i].bi3pic[j] = -1;
                  weatherPicture[i].flip[j] = 0;
                  imgReferences[j] = -1;
               }

               pc.addIntegerArray ( "FlipPictures", weatherPicture[i].flip );
               pc.addIntegerArray ( "ImageReference", imgReferences );

               for ( int j = 0; j < weatherPicture[i].images.size(); j++ )
                  if ( imgReferences[j] >= 0 && imgReferences[j] < weatherPicture[i].images.size() ) 
                     weatherPicture[i].images[j] = weatherPicture[i].images[imgReferences[j]];

            } else {
               for ( int u = 0; u < weatherPicture[i].images.size(); u++ ) {
                  weatherPicture[i].bi3pic[u] = -1;
                  weatherPicture[i].flip[u] = 0;
               }
            }

            if ( pc.find ( "DisplayMethod" ) )
               pc.addNamedInteger( "DisplayMethod", displayMethod, objectDisplayingMethodNum, objectDisplayingMethodTags );
            else
               displayMethod = 0;

         }

         if ( !oldWeatherSpecification ) {
            fieldModification[i].runTextIO( pc );
         }
         pc.closeBracket (  );
      }

   techDependency.runTextIO( pc );

   #ifndef converter
    buildIcon  = generate_gui_build_icon ( this, false );
    removeIcon = generate_gui_build_icon ( this, true );
   #endif

}
