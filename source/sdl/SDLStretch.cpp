/***************************************************************************
                          SDLStretch.cpp  -  description
                             -------------------
    begin                : Thu Nov 11 1999
    copyright            : (C) 1999 by Alexander Pipelka
    email                : pipelka@teleweb.at
 ***************************************************************************/

/***************************************************************************

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

 ***************************************************************************/

#include "../global.h"
#include "SDLStretch.h"
#include <stdio.h>
#include <cstring>
#include <math.h>
#include <stdlib.h>

template<class ST, class DT>
inline void StretchTemplateLUT(int x1, int x2, int y1, int y2, int yr, int yw, ST src_pixels, DT dst_pixels, Uint32* lut)
{
        int dx,dy,e,d,dx2;

        register Uint32 p;
        
        dx=(x2-x1);
        dy=(y2-y1);

        dy<<=1;
        e=dy-dx;
        dx2=dx<<1;

        for(d=0;d<=dx;d++)
        {
                p = lut[*src_pixels];
                *dst_pixels++ = p;

                while(e>=0)
                {
                        src_pixels ++;
                        e-=dx2;
                }
                
                e+=dy;
        }
}

template<class ST, class DT>
inline void StretchTemplate(int x1, int x2, int y1, int y2, int yr, int yw, ST src_pixels, DT dst_pixels)
{
        int dx,dy,e,d,dx2;
        
        dx=(x2-x1);
        dy=(y2-y1);

        dy<<=1;
        e=dy-dx;
        dx2=dx<<1;

        for(d=0;d<=dx;d++)
        {
                *dst_pixels++ = *src_pixels;

                while(e>=0)
                {
                        src_pixels ++;
                        e-=dx2;
                }
                
                e+=dy;
        }
}

template<class ST, class DT>
inline void RectStretchTemplate(SDL_Surface* src_surface, ST src, int xs1, int ys1, int xs2, int ys2, SDL_Surface* dst_surface, DT dst, int xd1, int yd1, int xd2, int yd2, Uint32* lutVOI)
{
        int dx,dy,e,d,dx2;
        int sx,sy;
        dx=abs((int)(yd2-yd1));
        dy=abs((int)(ys2-ys1));
        sx=sign(yd2-yd1);
        sy=sign(ys2-ys1);
        e=(dy<<1)-dx;
        dx2=dx<<1;
        dy<<=1;

        Uint16 src_pitch = src_surface->pitch;
        Uint16 dst_pitch = dst_surface->pitch;
        
        int src_bpp = src_surface->format->BytesPerPixel;
        int dst_bpp = dst_surface->format->BytesPerPixel;

        register long src_pixels = ((long)src + ys1*src_pitch + xs1 * src_bpp);
        register long dst_pixels = ((long)dst + yd1*dst_pitch + xd1 * dst_bpp);
        register Uint32* lut = lutVOI;

        if(lut == NULL){                                        // Stretch without lookup table
                for(d=0; (d<=dx) && (yd1<dst_surface->h); d++)
                {
                        StretchTemplate(xd1,xd2,xs1,xs2,ys1,yd1, (ST)src_pixels, (DT)dst_pixels);

                        while(e>=0)
                        {
                                src_pixels += src_pitch;
                                ys1++;
                                e-=dx2;
                        }
                        dst_pixels += dst_pitch;
                        yd1++;
                        e+=dy;
                }
        }
        else {                                                                  // Stretch with lookup table
                for(d=0; (d<=dx) && (yd1<dst_surface->h); d++)
                {
                        StretchTemplateLUT(xd1,xd2,xs1,xs2,ys1,yd1, (ST)src_pixels, (DT)dst_pixels, lut);

                        while(e>=0)
                        {
                                src_pixels += src_pitch;
                                ys1++;
                                e-=dx2;
                        }
                        dst_pixels += dst_pitch;
                        yd1++;
                        e+=dy;
                }
        }

}

void SDL_StretchSurface(SDL_Surface* src_surface, int xs1, int ys1, int xs2, int ys2, SDL_Surface* dst_surface, int xd1, int yd1, int xd2, int yd2, Uint32* lutVOI)
{
        int src_bpp = src_surface->format->BytesPerPixel;
        int dst_bpp = dst_surface->format->BytesPerPixel;

        if(dst_surface->format->BytesPerPixel == 1){
            SDL_SetColors(dst_surface, src_surface->format->palette->colors, 0, 256);
        }

        switch(dst_bpp){

                case 1:
                        switch(src_bpp){

                                case 1:
                                        RectStretchTemplate(src_surface, (Uint8*)src_surface->pixels, xs1, ys1, xs2, ys2, dst_surface, (Uint8*)dst_surface->pixels, xd1, yd1, xd2, yd2, (Uint32*)NULL);
                                        break;

                                case 2:
                                        RectStretchTemplate(src_surface, (Uint16*)src_surface->pixels, xs1, ys1, xs2, ys2, dst_surface, (Uint8*)dst_surface->pixels, xd1, yd1, xd2, yd2, lutVOI);
                                        break;
                        }
                        break;

                case 2:
                        switch(src_bpp){

                                case 2:
                                        RectStretchTemplate(src_surface, (Uint16*)src_surface->pixels, xs1, ys1, xs2, ys2, dst_surface, (Uint16*)dst_surface->pixels, xd1, yd1, xd2, yd2, (Uint32*)NULL);
                                        break;
                        }
                        break;

                case 4:
                        switch(src_bpp){

                                case 1:
                                        RectStretchTemplate(src_surface, (Uint8*)src_surface->pixels, xs1, ys1, xs2, ys2, dst_surface, (Uint32*)dst_surface->pixels, xd1, yd1, xd2, yd2, lutVOI);
                                        break;

                                case 2:
                                        RectStretchTemplate(src_surface, (Uint16*)src_surface->pixels, xs1, ys1, xs2, ys2, dst_surface, (Uint32*)dst_surface->pixels, xd1, yd1, xd2, yd2, lutVOI);
                                        break;

                                case 4:
                                        RectStretchTemplate(src_surface, (Uint32*)src_surface->pixels, xs1, ys1, xs2, ys2, dst_surface, (Uint32*)dst_surface->pixels, xd1, yd1, xd2, yd2, (Uint32*)NULL);
                                        break;

                        }
                        break;


        }

}


void SDL_StretchSurface(SDL_Surface* src_surface, SDL_Rect* src_rect, SDL_Surface* dst_surface, SDL_Rect* dst_rect, Uint32* voiLUT){
        
        SDL_StretchSurface(
                                src_surface,
                                src_rect->x,
                                src_rect->y,
                                src_rect->w,
                                src_rect->h,
                                dst_surface,
                                dst_rect->x,
                                dst_rect->y,
                                dst_rect->w,
                                dst_rect->h,    
                                voiLUT);
}
