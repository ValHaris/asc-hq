

#include "../loadimage.h"
#include "../basegfx.h"
#include "../basestrm.h"
#include "loadjpg.h"
#include "../loadpcx.h"

int trueColor = 1;
int fullscreenStatus = 0;
int oldModeNum = -1;
int modenum24 = -1;

int loadFullscreenImage ( const char* filename )
{
   if ( !trueColor )
      return 0;

   if ( modenum24 == -1 )
      modenum24 = initgraphics ( 640, 480,32 );
   else
      reinitgraphics ( modenum24 );

   if ( modenum24 > 0 ) {
      if ( exist ( filename )) {
         tnfilestream stream ( filename, 1 );
         if ( strstr ( filename, ".PCX" ) || strstr ( filename, ".pcx" ))
            loadpcxxy ( &stream, 0, 0 );
         else
            read_JPEG_file ( &stream );
      } else {
         string s = filename;
         while ( s.find ( ".PCX" ) != string::npos )
            s.replace ( s.find ( ".PCX" ), 4, ".JPG"); 

         while ( s.find ( ".pcx" ) != string::npos )
            s.replace ( s.find ( ".pcx" ), 4, ".jpg"); 
   
         if ( exist ( s.c_str() )) {
            tnfilestream stream ( s.c_str(), 1 );
            read_JPEG_file ( &stream );
         }
      }

      fullscreenStatus = 1;
      return 1;
   } else 
      return 0;

}

void closeFullscreenImage ( void )
{
   if ( fullscreenStatus == 1 ) {
      reinitgraphics ( oldModeNum );
      fullscreenStatus = 0;
   }
}


void setFullscreenSetting ( FullscreenImageSettings setting, int value )
{
   if ( setting == FIS_noTrueColor )
      trueColor = value;
   if ( setting == FIS_oldModeNum )
      oldModeNum = value;
}

