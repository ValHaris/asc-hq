
#include <stdio.h>
#include <conio.h>
#include "..\vesa.h"


int main()
{
   initsvga ( 0x101 );
//   bar ( 20, 20, 200, 200, 1 );
   ellipse ( 100, 100, 140, 120, yellow, 0.2 );
   getch();
   closesvga();
   settextmode(3);
   return 0;
}

