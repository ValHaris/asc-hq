
#include "overviewmapimage.h"
#include "palette.h"


OverviewMapImage::OverviewMapImage() : initialized(false )
{
}

OverviewMapImage::OverviewMapImage( const Surface& image )
{
   create ( image );
}


void OverviewMapImage::create( const Surface& image )
{
   for ( int y = 0; y < height; ++y )
      for ( int x = 0; x < width; ++x )
         if ( (y == 0 || y == height-1) && (x == 0 || x == width-1)) {
            segment[x][y] = SDLmm::ColorRGBA( 0, 0, 0, 0xff );
         } else {
            int r = 0;
            int b = 0;
            int g = 0;
            int a = 0;
            int count = 0;

            for ( int iy = image.h() * y / height; iy < image.h() * (y+1) / height; ++iy)
               for ( int ix = image.w() * x / width; ix < image.w() * (x+1) / width; ++ix) {
                   if ( image.GetPixelFormat().BitsPerPixel() == 1 ) {
                      int col = image.GetPixel(ix,iy);
                      if ( col != 255 ) {
                         r += pal[col][0] * 4;
                         g += pal[col][0] * 4;
                         b += pal[col][0] * 4;
                      } else
                         a += 255;
                   } else {
                      SDLmm::ColorRGBA col = image.GetPixelFormat().GetRGBA( image.GetPixel(ix,iy));
                      /*
                      r += col.r * (255-col.a) / 255;
                      g += col.g * (255-col.a) / 255;;
                      b += col.b * (255-col.a) / 255;;
                      */
                      r += col.r ;
                      g += col.g ;
                      b += col.b ;
                      a += col.a;
                   }
                   ++count;
               }

            if ( count )
               segment[x][y] = SDLmm::ColorRGBA( r / count, g / count, b / count, a / count );
            else
               segment[x][y] = SDLmm::ColorRGBA( 0, 0, 0, 0xff );
         }
}

void OverviewMapImage::blit( Surface& s, int x, int y, int layer ) const
{
   assert( s.GetPixelFormat().BytesPerPixel() == 4 );
   for ( int ly = 0; ly < height; ++ly )
      for ( int lx = 0; lx < width; ++lx )
         if ( ! ((ly == 0 || ly == height-1) && (lx == 0 || lx == width-1)))
            s.SetPixel( x + lx, y + ly, s.GetPixelFormat().MapRGB( segment[lx][ly] ));

}

