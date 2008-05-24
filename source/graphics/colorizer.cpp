
#include <cmath>

#include "colorizer.h"

// const int ColorSwitch::playerAngles[9] = { 0, 35, 67, 120, 170, 225, 270, 320, 0 };
const int ColorSwitch::playerAngles[9] = { 0, 240, 55, 120, 298, 225, 240, 36, 0 };

const bool ColorSwitch::sat[9] = { true, true, true, true, true, true, true, true, false };
   


void ColorSwitch::generate() {
   delete cache;
   cache = new Cache();

   for ( int r = 0; r < 256; ++r )
      for ( int g = 0; g < 256; ++g )
         if ( g < r ) {
            HSV hsv = rgb2hsv(r,g);
            for ( int player = 0; player < 9; ++player ) {
               if ( sat[player] == true )
                  cache->col[player][r][g] = hsv2rgb( HSV(playerAngles[player], hsv.s, hsv.v) );
               else
                  cache->col[player][r][g] = hsv2rgb( HSV(playerAngles[player], 0, hsv.v) );
            }
         }
}
			
ColorSwitch::HSV ColorSwitch::rgb2hsv( int r, int g)
{
	float max = float(r) / 255.0;
	float min = float(g) / 255.0;

	float delta = max - min;

   HSV hsv;
   hsv.s = int(ceil((delta / max) * 255));
   hsv.v = r;

   return hsv;
}
	
DI_Color ColorSwitch::hsv2rgb( HSV hsv)
{
   float h = float(hsv.h) / 360;
   float s = float(hsv.s) / 255;
   float v = float(hsv.v) / 255;
	

	if(hsv.s == 0) // grey
	{
      return DI_Color(hsv.v, hsv.v, hsv.v);
	}
	else
	{
      float h2 = h * 6;				
		if(h2 >= 6)
			h2 = 0;	//muss kleiner als 1 sein

		float var_i = floor(h2);            
		
		float var_1 = v * (1 - s);
		float var_2 = v * (1 - s * (h2 - var_i));
		float var_3 = v * (1 - s * (1 - (h2 - var_i)));
		
      float var_r, var_g, var_b;
		if( var_i == 0) 
		{ 
			var_r = v; 
			var_g = var_3; 
			var_b = var_1;
		}
		else if(var_i == 1)
		{ 
			var_r = var_2; 
			var_g = v;
			var_b = var_1;
		}
		else if(var_i == 2)
		{
			var_r = var_1;
			var_g = v;
			var_b = var_3; 
		}
		else if(var_i == 3)
		{
			var_r = var_1;
			var_g = var_2;
			var_b = v;
		}
		else if(var_i == 4)
		{
			var_r = var_3;
			var_g = var_1;
			var_b = v;
		}
		else
		{ 
			var_r = v;
			var_g = var_1;
			var_b = var_2;
		}
      return DI_Color( int( ceil( var_r * 255 )), 
                        int( ceil( var_g * 255 )),
                        int( ceil( var_b * 255 ))); 
	}

}

ColorSwitch::ColorSwitch() : cache(NULL) {
}

DI_Color ColorSwitch::switchC( int player, int r, int g, int b)
{
	if( g == b && r > g)
	{
	   if ( !cache )
	      generate();

		return cache->col[player][r][g];
	}
	else
	{
      return DI_Color(r,g,b);
	}
}

ColorSwitch colorSwitch;

