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

#include "realint.h";

rminfo rmi;

call_real_int::call_real_int(short int msize)
{
   memsize = msize;

   reg.w.ax = 0x0100;
   reg.w.bx = (memsize / 16 ) +1;

   #ifdef showstatus
      printf("allocating low memory\n");
   #endif

   int386(0x31, &reg, &reg);

   if (reg.w.cflag) {
      #ifdef showstatus
         printf("memory allocation failed");
         printf("error-code %d\n", reg.w.ax);
      #endif
   } else {
      #ifdef showstatus
         printf("memory allocated\n");
      #endif
   }
   realsegment = reg.w.ax;
   infoselector = reg.w.dx;
   protectedsegment = realsegment * 0x10;
}



boolean call_real_int::real_call(char intnr)
{

   /* use dmpi call 300h to issue the dos interrupt */
   reg.w.ax = 0x0300;
   reg.h.bl = intnr;
   reg.h.bh = 0;
   reg.w.cx = 0;
   segmentreg.es = FP_SEG( &rmi ); 
   
   reg.x.edi = FP_OFF( &rmi );
   
   #ifdef showstatus
      printf("calling real mode interrupt\n");
   #endif
   
   int386x(0x31, &reg, &reg, &segmentreg);
   
   
   /*if ((rmi.eax & 0xffff) != 0x004f) {
      #ifdef showstatus
         printf("could not get infoblock\n");
         printf("result: %x", rmi.eax);
      #endif
      
      return (false);
   } else {
      #ifdef showstatus
         printf("got infoblock\n");
      #endif
   } */
   return (true);
}

call_real_int::~call_real_int(void)
{
   reg.w.ax = 0x0101;
   reg.w.dx = infoselector;
   
   int386(0x31, &reg, &reg);
   
   if (reg.w.cflag) {
      #ifdef showstatus
         printf("dos-memory deallocation failed\n");
         printf("error-code %d\n", reg.w.ax);
      #endif
   }
   else {
      #ifdef showstatus
         printf("dos-memory deallocation successful\n");
      #endif
   }
}

void call_real_int::setuprmi(void)
{
   /* set up real-mode call structure */
   
   #ifdef showstatus
      printf("fill rmi structure with 0\n");
   #endif
   
   memset(&rmi, 0, sizeof(rmi));
   segread(&segmentreg);
}
