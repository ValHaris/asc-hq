// This file is included by loadbi3.cpp and bi2pcx.cpp

// The first entry is the picture number that is going to be replaced by the picture number of the second entry.


int terraintranslatenum = 0;
int terraintranslate[100][2];


int terrain2idTransNum = 0;
int terrain2idTranslation[100][3];


// This is a special translation for the fields that must be translated to a terrain AND an additional object

int terraincombixlatnum = 0;
struct Terraincombixlat {
           int bigraph;
           int terrainid;
           int terrainweather;
           int objectid;
       };
Terraincombixlat terraincombixlat[100];




// These BI object pictures can be translated to up to four ASC objects. The first entry is again the BI picture number, 
//  the following four the pictures for the ASC objects. A -1 is used if the entry is not used. It does not matter if you 
//  use " -1, 1500, -1  , -1 " or " 1500, -1  , -1,  -1" or any other order.


int objecttranslatenum = 0;
int objecttranslate[100][5];

int object2IDtranslatenum = 0;
int object2IDtranslate[200][2];


int getterraintranslatenum ( void )
{
   return terraintranslatenum;
}

int getobjectcontainertranslatenum ( void )
{
   return objecttranslatenum;
}

const int* getterraintranslate ( int pos )
{
   return terraintranslate[pos];
}

const int* getobjectcontainertranslate ( int pos )
{
   return objecttranslate[pos];
}



void readBI3translationTable ( )
{
   tnfilestream s ( "bi3maptranslation.txt", tnstream::reading );

   TextFormatParser tfp ( &s );
   TextPropertyGroup* tpg = tfp.run();

   PropertyReadingContainer pc ( "bi3maptranslation", tpg );

   vector<int> terraintranslation;
   pc.addIntegerArray ( "TerrainTranslation", terraintranslation );

   vector<int> terrain2idtrans;
   pc.addIntegerArray ( "Terrain2IDTranslation", terrain2idtrans );

   vector<int> terrainobjtranslation;
   pc.addIntegerArray ( "TerrainObjTranslation", terrainobjtranslation );

   vector<int> objecttranslation;
   pc.addIntegerArray ( "ObjectTranslation", objecttranslation );

   vector<int> object2IDtranslation;
   pc.addIntegerArray ( "Object2IDTranslation", object2IDtranslation );

   pc.run();

   delete tpg;



   if ( terraintranslation.size() % 2 )
      fatalError ( "Bi3 map translation : terraintranslation - Invalid number of entries ");
   terraintranslatenum = terraintranslation.size()/2;
   for ( int i = 0; i < terraintranslation.size(); i++ )
      terraintranslate[i/2][i%2] = terraintranslation[i];

   if ( terrain2idtrans.size() % 3 )
      fatalError ( "Bi3 map translation : terrain2idTranslation - Invalid number of entries ");
   terrain2idTransNum = terrain2idtrans.size()/3;
   for ( int i = 0; i < terrain2idtrans.size(); i++ )
      terrain2idTranslation[i/3][i%3] = terrain2idtrans[i];


   if ( terrainobjtranslation.size() % 4 )
      fatalError ( "Bi3 map translation : terrainobjtranslation - Invalid number of entries ");
   terraincombixlatnum = terrainobjtranslation.size()/4;
   for ( int i = 0; i < terrainobjtranslation.size()/4; i++ ) {
       terraincombixlat[i].bigraph = terrainobjtranslation[i*4];
       terraincombixlat[i].terrainid = terrainobjtranslation[i*4+1];
       terraincombixlat[i].terrainweather = terrainobjtranslation[i*4+2];
       terraincombixlat[i].objectid = terrainobjtranslation[i*4+3];
   }

   if ( objecttranslation.size() % 5 )
      fatalError ( "Bi3 map translation : objecttranslation - Invalid number of entries ");
   objecttranslatenum = objecttranslation.size()/5;
   for ( int i = 0; i < objecttranslation.size(); i++ )
      objecttranslate[i/5][i%5] = objecttranslation[i];

   if ( object2IDtranslation.size() % 2 )
      fatalError ( "Bi3 map translation : object2IDtranslation - Invalid number of entries ");
   object2IDtranslatenum = object2IDtranslation.size()/2;
   for ( int i = 0; i < object2IDtranslation.size(); i++ )
      object2IDtranslate[i/2][i%2] = object2IDtranslation[i];

}
