#include <iostream.h>

int main ( void )
{
   cout << "Dieses Programm wurde auf Codepage 850 geschrieben." << endl ;
   cout << "H„ll” Welt" << endl;  // Ein String mit umlauten; 'Haelloe Welt';
   cout << "Angriffsformel siehe asc.html õ9.16" << endl;    // Ein String mit einem Paragraphen-Zeichen, damit hatte ich mal probleme unter Linux 
   if ( 'õ' != 245  || '„' != 132 ) {    // ASCII Werte von paragraph und ae
      cout << "Sabotage !!" << endl;
      cout << int('õ') << " " << int('„') << endl; // Ausgabe der ASCII Werte von paragraph und ae
   }
   return 0;
}   
