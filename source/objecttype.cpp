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

#include "objecttype.h"
#include "gamemap.h"
#include "basegfx.h"
#include "graphicset.h"
#include "textfileparser.h"
#include "textfiletags.h"
#include "sgstream.h"


ObjectType :: ObjectType ( void )
{
   terrain_and.flip();
   buildicon = NULL;
   removeicon = NULL;
   dirlist = NULL;
   dirlistnum = 0;

}


int  ObjectType :: buildable ( pfield fld )
{
   #ifndef converter
   if ( fld->building )
      return 0;

   if ( terrainaccess.accessible ( fld->bdt ) <= 0 )
       return 0;

   #endif
   return 1;
}


void* ObjectType :: getpic ( int i, int w )
{
   if ( !weather.test(w) )
      w = 0;

   if ( weatherPicture[w].images.size() <= i )
      i = 0;

   return weatherPicture[w].images[i];
}




void ObjectType :: display ( int x, int y, int dir, int weather )
{
   #ifndef converter
  if ( id == 1 || id == 2 ) {
     putspriteimage ( x, y,  getpic( dir, weather ) );
  } else
#ifdef HEXAGON
  if ( id == 4 ) {
#else
  if ( id == 3 ||  id == 4 ) {
#endif

     if ( dir == 68 )
        putspriteimage ( x, y,  getpic ( 9, weather ) );
     else
     if ( dir == 34 )
        putspriteimage ( x, y,  getpic ( 10, weather ) );
     else
     if ( dir == 17 )
        putspriteimage ( x, y,  getpic ( 11, weather ) );
     else
     if ( dir == 136)
        putspriteimage ( x, y,  getpic ( 12, weather ) );
     else
     if ( dir == 0)
        putspriteimage ( x, y,  getpic ( 0, weather ) );
     else
        for (int i = 0; i <= 7; i++)
           if ( dir & (1 << i))
              putspriteimage( x, y,  getpic ( i + 1, weather ) );

  } else
  if (  id == 5 ) {
     putspriteimage  ( x, y,  getpic ( 0, weather ) );
  } else
  #ifndef HEXAGON
  if (  id == 6  ||  id == 7 || id == 8 ) {
     for (int i = 0; i <= 7; i++)
        if ( dir & (1 << i))
           putspriteimage( x, y,  getpic ( i, weather ) );
  } else
  #endif

      if ( dirlistnum ) {
         for ( int i = 0; i < dirlistnum; i++ )
            if ( dirlist [ i ] == dir ) {
               putspriteimage ( x, y, getpic ( i, weather ) );
               return;
            }

         for ( int j = 0; j < dirlistnum; j++ )
            if ( dirlist [ j ] == 0 ) {
               putspriteimage ( x, y, getpic ( j, weather ) );
               return;
            }

         putspriteimage ( x, y, getpic ( 0, weather ) );

      } else
        if ( dir < weatherPicture[weather].images.size() )
           putspriteimage ( x, y, getpic ( dir, weather ) );
        else
           putspriteimage ( x, y, getpic ( 0, weather ) );

  #endif
}



void ObjectType :: display ( int x, int y )
{
   display ( x, y, 34, 0 );
}


const int object_version = 1;

void ObjectType :: read ( tnstream& stream )
{
   int version = stream.readInt();

   if ( version <= object_version && version >= 1 ) {

       id = stream.readInt();

       int ___weather = stream.readInt();
       weather.reset();
       for ( int i = 0; i < cwettertypennum; i++ )
          if ( ___weather & ( 1 << i ))
             weather.set ( i );

       visibleago = stream.readInt();
       int  ___objectslinkablenum = stream.readInt();
       stream.readInt(); // ___objectsLinkable
       stream.readInt(); // ___oldpicture 

       int ___pictnum = stream.readInt();
       armor = stream.readInt();

       int ___movemalus_plus_count = stream.readChar();
       stream.readInt();

       int ___movemalus_abs_count = stream.readChar();
       stream.readInt();

       attackbonus_plus = stream.readInt();
       attackbonus_abs  = stream.readInt();

       defensebonus_plus = stream.readInt();
       defensebonus_abs =  stream.readInt();

       basicjamming_plus = stream.readInt();
       basicjamming_abs = stream.readInt();

       height = stream.readInt();

       buildcost.read( stream );
       removecost.read ( stream );
       build_movecost = stream.readInt();
       remove_movecost = stream.readInt();

       bool ___name = stream.readInt();
       name.erase();

       no_autonet = stream.readInt();

       terrainaccess.read( stream );
       terrain_and.read ( stream );
       terrain_or.read ( stream );

       stream.readInt(); // ___buildicon
       stream.readInt(); // ___removeicon

       stream.readInt(); // ___dirlist
       dirlistnum = stream.readInt();

       bool _picture[cwettertypennum];
       for ( int aa = 0; aa < cwettertypennum; aa++ )
          _picture[aa] = stream.readInt();


      int w;
      for ( int ww = 0; ww < cwettertypennum; ww++ )
         if ( weather.test ( ww ) ) {
            weatherPicture[ww].bi3pic.resize(___pictnum);
            weatherPicture[ww].flip.resize(___pictnum);
            weatherPicture[ww].images.resize(___pictnum);

            for ( int n = 0; n < ___pictnum; n++ ) {
               stream.readInt(); // dummy
               weatherPicture[ww].bi3pic[n] = stream.readInt();
               weatherPicture[ww].flip[n] = stream.readInt();

               if ( weatherPicture[ww].bi3pic[n] != -1 )
                  loadbi3pict_double ( weatherPicture[ww].bi3pic[n],
                                      &weatherPicture[ww].images[n],
                                      1,  // CGameOptions::Instance()->bi3.interpolate.objects
                                      0 );
               else
                  stream.readrlepict ( &weatherPicture[ww].images[n], false, &w);

            }
         }

       setupImages();

       #ifndef converter
       int mmcount = cmovemalitypenum;

       if (mmcount < ___movemalus_plus_count )
          mmcount = ___movemalus_plus_count;
       #else
       int mmcount = ___movemalus_plus_count ;
       #endif

       movemalus_plus.clear();
       for ( int j=0; j < mmcount ; j++ ) {
          if ( j < ___movemalus_plus_count )
             movemalus_plus.push_back(  stream.readChar() );
          else
             if( j > 0 )
               movemalus_plus.push_back ( movemalus_plus[0] );
             else
               movemalus_plus.push_back ( 0 );
       }


       #ifndef converter
       mmcount = cmovemalitypenum;

       if (mmcount < ___movemalus_abs_count )
          mmcount = ___movemalus_abs_count;
       #else
       mmcount = ___movemalus_abs_count ;
       #endif

       movemalus_abs.clear();

       for ( int j=0; j< mmcount ; j++ ) {
          if (j < ___movemalus_abs_count )
             movemalus_abs.push_back( stream.readChar() );
          else
             if( j > 0 )
                movemalus_abs.push_back( movemalus_abs[0] );
             else
                movemalus_abs.push_back( 0 );
       }

       if ( ___name )
          name = stream.readString ( );


      if ( dirlistnum ) {
         dirlist = new int[ dirlistnum ];
         for ( int i = 0; i < dirlistnum; i++ )
            dirlist[i] = stream.readInt();
      }


      if ( ___objectslinkablenum ) {
         linkableObjects.clear();
         for ( int i = 0; i < ___objectslinkablenum; i++ )
             linkableObjects.push_back ( stream.readInt() );
      }

   #ifndef converter
    buildicon = generate_object_gui_build_icon ( this, 0 );
    removeicon = generate_object_gui_build_icon ( this, 1 );
   #endif


   } else
       throw ASCmsgException ( "invalid object file format version");
}


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
               delete[] buf;
               copycount++;
            }

            if ( weatherPicture[ww].bi3pic[n] == -1 )
               weatherPicture[ww].flip[n] = 0;
         }
   #endif

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

}


void ObjectType :: write ( tnstream& stream ) const
{
    stream.writeInt ( object_version );

    stream.writeInt ( id);
    int ___weather = 0;
    for ( int i = 0; i < cwettertypennum; i++ )
       if ( weather.test ( i ))
          ___weather |= 1 << i;
    stream.writeInt ( ___weather );
    stream.writeInt ( visibleago );
    stream.writeInt ( linkableObjects.size() );
    stream.writeInt ( -1 ); // was: objectslinkable
    stream.writeInt ( -1 ); // was: oldpicture
    stream.writeInt ( weatherPicture[0].images.size() );
    stream.writeInt ( armor );

    stream.writeChar ( movemalus_plus.size() );
    stream.writeInt ( -1 ); // was movemalus_plus

    stream.writeChar ( movemalus_abs.size() );
    stream.writeInt ( -1 ); // was movemalus_abs

    stream.writeInt ( attackbonus_plus );
    stream.writeInt ( attackbonus_abs );
    stream.writeInt ( defensebonus_plus );
    stream.writeInt ( defensebonus_abs );

    stream.writeInt ( basicjamming_plus );
    stream.writeInt ( basicjamming_abs );

    stream.writeInt ( height );

    buildcost.write( stream );
    removecost.write ( stream );
    stream.writeInt( build_movecost );
    stream.writeInt( remove_movecost );

    stream.writeInt ( -1 ); // was name
    stream.writeInt ( no_autonet );

    terrainaccess.write( stream );
    terrain_and.write ( stream );
    terrain_or.write ( stream );

    stream.writeInt( -1 ); // was buildicon
    stream.writeInt( -1 ); // was removeicon

    stream.writeInt( -1 ); // was dirlist

    stream.writeInt ( dirlistnum );
    for ( int aa = 0; aa < cwettertypennum; aa++ )
       stream.writeInt( -1 ); // was picture

    for ( int ww = 0; ww < cwettertypennum; ww++ )
       if ( weather.test( ww ) )
          for ( int l = 0; l < weatherPicture[ww].images.size(); l++ ) {
             stream.writeInt ( -1 );
             stream.writeInt ( weatherPicture[ww].bi3pic[l] );
             stream.writeInt ( weatherPicture[ww].flip[l] );
             if ( weatherPicture[ww].bi3pic[l] == -1 )
                stream.writedata( weatherPicture[ww].images[l], getpicsize2 ( weatherPicture[ww].images[l] ) );
          }

    for ( int i = 0; i < movemalus_plus.size(); i++ )
        stream.writeInt ( movemalus_plus[i] );

    for ( int i = 0; i < movemalus_abs.size(); i++ )
        stream.writeInt ( movemalus_abs[i] );

    stream.writeString ( name );

    if ( dirlistnum )
       for ( int i = 0; i < dirlistnum; i++ )
          stream.writeInt ( dirlist[i] );

    for ( int i = 0; i < linkableObjects.size(); i++ )
        stream.writeInt( linkableObjects[i] );

}


void ObjectType :: runTextIO ( PropertyContainer& pc )
{

   pc.addInteger  ( "ID", id ).evaluate();
   pc.addTagArray ( "Weather", weather, cwettertypennum-1, weatherTags ).evaluate();
   pc.addBool     ( "visible_in_fogOfWar", visibleago );
   pc.addIntegerArray ( "LinkableObjects", linkableObjects );
   pc.addInteger  ( "Armor", armor );
   pc.addIntegerArray ( "Movemalus_plus", movemalus_plus ).evaluate();
   int mm = movemalus_plus.size();
   movemalus_plus.resize( cmovemalitypenum );
   for ( int i = mm; i < cmovemalitypenum; i++ ) {
      if ( i == 0 )
         movemalus_plus[i] = 0;
      else
         movemalus_plus[i] = movemalus_plus[0];
   }  
    

   pc.addIntegerArray ( "Movemalus_abs", movemalus_abs ).evaluate();
   mm = movemalus_abs.size();
   movemalus_abs.resize( cmovemalitypenum );
   for ( int i = mm; i < cmovemalitypenum; i++ ) {
      if ( i == 0 )
         movemalus_abs[i] = -1;
      else
         movemalus_abs[i] = movemalus_abs[0];
   }

   pc.addInteger  ( "AttackBonus_abs", attackbonus_abs );
   pc.addInteger  ( "AttackBonus_plus", attackbonus_plus );
   pc.addInteger  ( "DefenseBonus_abs", defensebonus_abs );
   pc.addInteger  ( "DefenseBonus_plus", defensebonus_plus );
   pc.addInteger  ( "Jamming_abs", basicjamming_abs );
   pc.addInteger  ( "Jammming_plus", basicjamming_plus );
   pc.addInteger  ( "Height", height );

   pc.openBracket ( "ConstructionCost" );
   buildcost.runTextIO ( pc );
   pc.addInteger  ( "Movement", build_movecost );
   pc.closeBracket ();

   pc.openBracket ( "RemovalCost" );
   removecost.runTextIO ( pc );
   pc.addInteger  ( "Movement", remove_movecost );
   pc.closeBracket ();

   pc.addString( "Name", name );
   pc.addBool  ( "NoSelfChaining", no_autonet );

   pc.openBracket ( "TerrainAccess" );
   terrainaccess.runTextIO ( pc );
   pc.closeBracket ();

   pc.addTagArray ( "TerrainProperties_Filter", terrain_and, cbodenartennum, bodenarten, true );
   pc.addTagArray ( "TerrainProperties_Add", terrain_or, cbodenartennum, bodenarten );

   for ( int i = 0; i < cwettertypennum; i++ )
      if ( weather.test(i) ) {

         pc.openBracket  ( weatherTags[i] );
         bool bi3pics = false;

         if ( !pc.isReading() )
            for ( int j = 0; j < weatherPicture[i].bi3pic.size(); j++ )
                if ( weatherPicture[i].bi3pic[j] >= 0 )
                   bi3pics = true;

         pc.addBool  ( "UseGFXpics", bi3pics ).evaluate();
         if ( bi3pics ) {
            pc.addIntegerArray ( "GFXpictures", weatherPicture[i].bi3pic ).evaluate();
            pc.addIntegerArray ( "FlipPictures", weatherPicture[i].flip ).evaluate();
            weatherPicture[i].images.resize( weatherPicture[i].bi3pic.size() );

            if ( pc.isReading() )
               for ( int j = 0; j < weatherPicture[i].bi3pic.size(); j++ )
                   loadbi3pict_double (  weatherPicture[i].bi3pic[j],
                                        &weatherPicture[i].images[j],
                                        1,
                                        0 );

         } else {
            ASCString s = extractFileName_withoutSuffix( filename );
            if ( s.empty() ) {
               s = "object";
               s += strrr(id);
            }
            pc.addImageArray ( "picture",   weatherPicture[i].images, s + weatherAbbrev[i] ).evaluate();

            if ( pc.find ( "FlipPictures" ) ) {
               vector<int>   imgReferences;
               weatherPicture[i].bi3pic.resize( weatherPicture[i].images.size() );
               weatherPicture[i].flip.resize( weatherPicture[i].images.size() );
               imgReferences.resize ( weatherPicture[i].images.size() );

               for ( int j = 0; j < weatherPicture[i].images.size(); j++ ) {
                  weatherPicture[i].bi3pic[j] = -1;
                  weatherPicture[i].flip[j] = 0;
                  imgReferences[j] = -1;
               }

               pc.addIntegerArray ( "FlipPictures", weatherPicture[i].flip ).evaluate();
               pc.addIntegerArray ( "ImageReference", imgReferences ).evaluate();

               for ( int j = 0; j < weatherPicture[i].images.size(); j++ )
                  if ( imgReferences[j] >= 0 && imgReferences[j] < weatherPicture[i].images.size() ) {
                     if ( weatherPicture[i].images[j] )
                        asc_free ( weatherPicture[i].images[j] );
                     int newimg = imgReferences[j];
                     int size = getpicsize2( weatherPicture[i].images[newimg] );
                     void* p = asc_malloc(size);
                     memcpy ( p, weatherPicture[i].images[newimg], size );
                     weatherPicture[i].images[j] = p;
                  }

            } else {
               weatherPicture[i].bi3pic.resize( weatherPicture[i].images.size() );
               weatherPicture[i].flip.resize( weatherPicture[i].images.size() );
               for ( int u = 0; u < weatherPicture[i].images.size(); u++ ) {
                  weatherPicture[i].bi3pic[u] = -1;
                  weatherPicture[i].flip[u] = 0;
               }
            }

         }

         pc.closeBracket (  );
      }

   if ( pc.isReading() )
      setupImages();


   #ifndef converter
    buildicon = generate_object_gui_build_icon ( this, 0 );
    removeicon = generate_object_gui_build_icon ( this, 1 );
   #endif

}
