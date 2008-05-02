

#ifndef colorizerH
#define colorizerH

#include "surface.h"

class ColorSwitch {

      class HSV {
      public:
         int h;
         int s;
         int v;
         HSV( int hh, int ss, int vv ) : h(hh), s(ss), v(vv) {};
         HSV() : h(0), s(0), v(0) {};
      };

      struct Cache {
         DI_Color col[9][256][256];
      };

      Cache* cache;

      static const int playerAngles[9];

      static const bool sat[9];
      
      void generate(); 
	   HSV rgb2hsv( int r, int g);	
	   DI_Color hsv2rgb( HSV hsv);


   public:
      ColorSwitch();
      DI_Color switchC( int player, int r, int g, int b);
};

extern ColorSwitch colorSwitch;

#endif
