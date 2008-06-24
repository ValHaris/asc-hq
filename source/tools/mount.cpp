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

// #include <config.h>

#include <fstream>
#include <cstring>
#include <cerrno>
#include <cstdio>

#include <SDL_types.h>
#include <SDL_endian.h>

#include "../global.h"
#include "../basestrm.h"
#include "../misc.h"
#include "../strtmesg.h"

/* this could be eliminated by a @OS_TYPE@ include in Makefile.am */
#ifdef _WIN32_
#  include "../win32/fileio.h"
#else
#  ifdef _UNIX_
#    include "../unix/fileio.h"
#  endif
#endif

/* some really generic stuff - perhaps to put into a really generic header? */

#ifndef MAX
 #define MAX(a,b)		((a) >= (b) ? (a) : (b))
#endif
#ifndef MIN
 #define MIN(a,b)		((a) <= (b) ? (a) : (b))
#endif
#define MAX3(a,b,c)		((a) >= (b) ? MAX(a,c) : MAX(b,c))
#define MIN3(a,b,c)		((a) <= (b) ? MIN(a,c) : MIN(b,c))

#define Sentinel    0xF0   /* the sentinel flag */
#define BUFFER_SIZE 30000

#define WriteCode(a,b,c) fprintf( outfile, "%c%c%c", a, b, c )



void fatalError ( const ASCString& s )
{
   fprintf( stderr, "%s \n", s.c_str() );
   exit(1);
}

            
int writeInt  ( FILE* f, int i )
{
   i = SDL_SwapLE32(i);
   return fwrite ( &i, 1, 4, f );
}


FILE* out;
int verbose = 1;

const char* containerName = NULL;

/*--- rle1.c ------------------------------ Listing 9-1 ---------
 *  Run-Length Encoding for files of all types
 *
 *-------------------------------------------------------------*/

FILE *infile,
*outfile;

int rlemain ( const char* argv1, const char* argv2  )
{
   unsigned char *buffer;
   unsigned char prev_char = 0 ;
   int  bytes_read,
   count,
   eof,
   first_time,
   i;

   if (( infile = fopen ( argv1, "rb" )) == NULL )  {
      fatalError ( "Error opening " + ASCString( argv1 ));
      return ( EXIT_FAILURE );
   }

   if (( outfile = fopen ( argv2, "wb" )) == NULL )  {
      fatalError ( "Error opening " + ASCString( argv2 ));
      return ( EXIT_FAILURE );
   }

   /* write the header to the output file */

   fprintf ( outfile, "%c%c%c%c%c%c%c%c%c%c", 0, 'M', 'B', 'R', 'L', 'E', '1', 0, 0, Sentinel );

   /* initialize the necessary variables */

   eof        = 0;
   first_time = 1;
   count = 0;

   buffer = (unsigned char *) malloc ( BUFFER_SIZE );
   if ( buffer == NULL ) {
      fatalError ( "Unable to allocate " + ASCString(strrr ( BUFFER_SIZE)) + " bytes" );
      return ( EXIT_FAILURE );
   }

   /* process the input file */

   while ( ! eof ) {
      bytes_read = fread ( buffer, 1, BUFFER_SIZE, infile );
      if ( bytes_read == 0 ) {
         eof = 1;
         break;
      }

      for ( i = 0; i < bytes_read; i++ ) {
         /* first time through is a special case */

         if ( first_time ) {
            prev_char = buffer[i];
            count = 1;
            first_time = 0;
            i++;
         }

         if ( buffer[i] == prev_char )   /* repeated char */
         {
            count += 1;
            if ( count == 255 ) {
               WriteCode ( Sentinel, count, prev_char );
               first_time = 1;
            }
            continue;
         }
         else    /* a new char, so write out all known data */
         {
            if ( count < 3 ) {
               if ( prev_char == Sentinel ) {
                  fprintf ( outfile, "%c%c",
                            Sentinel, count );
               } else
                  do {
                     fputc ( prev_char, outfile );
                  } while ( --count );
            }
            else
               WriteCode ( Sentinel, count, prev_char );

            prev_char = buffer[i];
            count = 1;
         }
      }

      /* we're at end of bytes_read, is it EOF? */

      if ( bytes_read < BUFFER_SIZE )
         eof = 1;
   }

   /* at EOF, so flush out any remaining bytes to be written */

   if ( count > 0 ) {
      if ( count < 3 )  {
         if ( prev_char == Sentinel )  {
            fprintf ( outfile, "%c%c",  Sentinel, count );
         } else
            do {
               fputc ( prev_char, outfile );
            } while ( --count );
      } else
         WriteCode ( Sentinel, count, prev_char );
   }      

   fclose ( infile );
   fclose ( outfile );

   return ( EXIT_SUCCESS );
}



int
bzmain( const char *argv1, const char* argv2  )
{
   int size = filesize ( argv1 );
   void* buf = malloc ( size );

   FILE* fp = fopen ( argv1, filereadmode );
   fread ( buf, size, 1, fp );
   fclose ( fp );

   // tnfilestream instream ( argv1, 1 );
   tnfilestream outstream ( argv2, tnstream::writing );
   // instream.readdata ( buf, size, 0 );
   outstream.writedata ( buf, size );

   free ( buf );
   return 0;
}


ASCString getTempPath()
{
#ifdef _UNIX_
  return "/tmp/";
#else
   ASCString name;
   if ( getenv ( "temp" ))
      name = getenv ( "temp" );
   else
      if ( getenv ( "tmp" ))
         name = getenv ( "tmp" );
      else {
         name = pathdelimitterstring + ASCString("tmp");
      }

      appendbackslash( name );
      return name;
#endif

}

ASCString getTempFileName( int type )
{
   if ( type == 0 )
      return getTempPath() + "temp.rle";
   else
      return getTempPath() + "temp.mzl";
}



dynamic_array<tcontainerindex> nindex;
Uint32 num = 0; /* number of files in the container ? */
Uint32 pos = 0; /* starting offset in container file ? */

int uncompsize = 0;
int   compsize = 0;


void copyfile ( const char* name, const char* orgname, int size )
{
   char* buf = new char[size+100];

   FILE* in = fopen ( name, "rb" );
   if ( fread ( buf, 1, size, in ) != size ) {
      fatalError ( "error reading file " + ASCString ( name ));
      exit(1);
   }
   fclose ( in );

   int s = fwrite ( buf, 1, size, out );
   if ( s != size ) {
      fatalError ( "error writing file " + ASCString ( name ));
      exit(1);
   }
   nindex[num].name = strdup ( orgname );
   nindex[num].start = pos;
   pos += s;
   nindex[num].end = pos-1;
   num++;
   if ( verbose )
      printf ( " ; written \n" );
   compsize += size;
}


char buf2[10000];

/* mikem: seems to test a file for asc compression */
void testcompress ( const char* name, int size )
{
   if ( strcmp ( name, "temp.mzl") == 0
     || strcmp ( name, "temp.rle") == 0
     || strcmp ( name, containerName ) == 0 )
      {
        printf ( " skipped\n");
        return;
      }
      
   
   ASCString newname;
   bool deleteNewName = false;
   
   const char* orgname = name;
   if ( patimat ( "*.pcx", name, true ) ) {

      FILE* infile = fopen ( name, filereadmode );

      newname = getTempPath() + name;

      FILE* outfile = fopen ( newname.c_str(), filewritemode );
      if ( !outfile )
         fatalError( "Unable to open " + newname + " for writing." );

      int size = filesize ( name );
      char* pc = (char*) malloc ( size );
      fread ( pc, size, 1, infile );
      fclose ( infile );

      int* ip = (int*) (pc + 124);
      *ip = SDL_SwapLE32(size);

      int w = fwrite ( pc, 1, size, outfile );
      fclose ( outfile );
      if ( w != size )
         fatalError( "Could not write file " + newname );

      name = newname.c_str();
      deleteNewName = true;
   }

   uncompsize += size;

   int comp = 0;
   {
      char buf[100];
      FILE* in = fopen ( name, "rb" );
      if ( !in )
         fatalError("cannot access file " + ASCString(name) );

      if ( fread ( buf, 1, 10, in ) == 10 ) {
         if ( strncmp ( &buf[1], "MBLZW16", 7) == 0 )
            comp = 1;
         if ( strncmp ( &buf[1], "MBRLE1", 6) == 0 )
            comp = 1;
         if ( strncmp ( &buf[0], "MBZLB2X!", 8) == 0 )
            comp = 1;
      }
      fclose ( in );
   }
   if ( comp ) {
      if ( verbose )
         printf ( "is already compressed ! " );
      copyfile ( name, extractFileName ( buf2, name ), filesize ( name ) );
   } else {

      int r;
      fflush ( stdout );

      r = rlemain( name, getTempFileName(0).c_str() );
      if ( r ) {
         fatalError ( "error executing rle1; errno is " + ASCString(strrr (errno)) );
         exit ( r );
      }
      int rl = filesize ( getTempFileName(0).c_str() );

      fflush ( stdout );
      r = bzmain ( name, getTempFileName(1).c_str() );
      if ( r ) {
         fatalError ( "error executing mbzip; errno is " + ASCString ( strrr ( errno)));
         exit ( r );
      }
      int mz = filesize ( getTempFileName(1).c_str() );

      if ( !size ) {
         fatalError(" encountered a file of size 0 !" );
      }

      if ( verbose )
         printf ( "; rle: %3d%%; mzl: %3d%%", 100 * rl / size, 100 * mz / size );
      int compr = MIN ( rl, mz );

      if ( compr * 120 / 100 > size ) {
         if ( verbose )
            printf ( "not compressed" );
         copyfile ( name, extractFileName ( buf2, name ), size );
      } else
         if ( compr == rl ) {
            if ( verbose )
               printf ( "rle compressed" );
            copyfile ( getTempFileName(0).c_str(), orgname, compr );
         } else
            if ( compr == mz ) {
               if ( verbose )
                  printf ( "mzl compressed" );
               copyfile ( getTempFileName(1).c_str(), orgname, compr );
            }

      unlink(getTempFileName(0).c_str());
      unlink(getTempFileName(1).c_str());
   }
   if ( deleteNewName )
      remove ( newname.c_str() );

}


// including the command line parser, which is generated by genparse
#include "../clparser/mount.cpp"



void processFiles( const char* files, const char* orgWorkingDir )
{
   int compress = 1;


   char buf[10000];
   strncpy( buf, files, 10000 );

   char* filename = buf;
   const char* dirname = ".";

   while ( strchr( buf, foreignPathDelimitter ))
      *strchr( buf, foreignPathDelimitter ) = pathdelimitter;

   if ( strchr( filename , pathdelimitter )) {
      char* c = filename + strlen( filename );
      while ( *c != pathdelimitter ) 
         c -= 1;
      
      dirname = buf;
      filename = c+1;
      *c = 0;
   }

   

   DIR * dirp = opendir( dirname );
   if( dirp != NULL ) {
      for(;;) {
         struct ASC_direct *direntp = readdir( dirp );
         if ( direntp == NULL )
            break;

         if ( patimat ( filename , direntp->d_name, true ) &&
               strcmp ( direntp->d_name, "." ) != 0 &&
               strcmp ( direntp->d_name, ".." ) != 0 ) {
            int    fnd = 0;
            int j   = 0;

            while( !fnd && ( j < num ) ) {
               //                for ( int j = 0; j < num; j++ )
               if ( strcmpi ( nindex[j++].name, direntp->d_name ) == 0 )
                  fnd = 1;
            }

            if ( !fnd ) {
               if ( chdir(dirname)  != 0 )
                  fatalError( "could not change directory (1)");

               if ( verbose )
                  printf( direntp->d_name );
               if ( compress )
                  testcompress ( direntp->d_name,  filesize(direntp->d_name) );
               else {
                  if ( verbose )
                     printf ( " is not compressed, " );
                  copyfile ( direntp->d_name, direntp->d_name,  filesize(direntp->d_name)  );
               }
               if ( chdir(orgWorkingDir)  != 0 )
                  fatalError( "could not change directory (2)");
            }
         }
      }
      closedir( dirp );
   }
   
}

int main(int argc, char *argv[] )
{
   assert( sizeof(int) == 4 ); 

   int i = 0;      /* loop var */
   Cmdline cl ( argc, argv );

   if ( cl.v() ) {
      cout << argv[0] << " " << getVersionString() << endl;
      exit(0);
   }

   if ( argc - cl.next_param() < 2 ) {
      cl.usage();
      exit(1);
   }

   if (cl.q())
      verbose = 0;

   addSearchPath(".");

   containerName = argv[argc-1];
   out = fopen ( argv[argc-1], filewritemode );
   if ( !out ) {
      fatalError( "cannot open file " + ASCString( argv[argc-1] ) + "  for writing " );
      exit(1);
   }

   pos += fwrite ( containermagic, 1, 4, out );
   pos += writeInt( out, i );


   char orgWorkingDir[10000];
   if ( !getcwd( orgWorkingDir, 10000 ))
      fatalError( "could not obtain working directory");


   for ( int df = cl.next_param(); df < argc-1; df++ ) {
      if ( argv[df][0] == '@' ) {
         std::ifstream list( argv[df]+1 );
         if ( !list )
            fatalError( "could not open file " + ASCString(argv[df]) );
         
         while (! list.eof() ) {
            ASCString filename;
            getline (list, filename);
            processFiles( filename.c_str(), orgWorkingDir  );
         }
      } else 
         processFiles( argv[df], orgWorkingDir );
      
   } /* endwhile */

   if ( verbose )
      printf ( "ftell: %d ; pos : %d ; num : %d \n ", int( ftell ( out )), pos, num );

   writeInt ( out, num );

   for ( i = 0; i < num; i++) {
      writeInt( out, nindex[i].name ? 1 : 0 );
      writeInt( out, nindex[i].start );
      writeInt( out, nindex[i].end );

      if ( nindex[i].name ) {
         for ( int j = 0; j < strlen ( nindex[i].name ); ++j )
            nindex[i].name[j] = tolower( nindex[i].name[j] );
         fwrite ( nindex[i].name, 1, strlen ( nindex[i].name ) + 1, out );
      }

   } /* endfor */

   fseek ( out, 4, SEEK_SET );
   writeInt ( out, pos );
   fclose ( out );

   if ( verbose ) {
      // printf(" average compression is %d%% \noverhead is %d byte \n", 100 * compsize / uncompsize, filesize ( argv[2] ) - compsize );

      printf ( "completed \n" );
   }
   remove ( "temp.rle" );
   remove ( "temp.mzl" );

   return 0;
}

