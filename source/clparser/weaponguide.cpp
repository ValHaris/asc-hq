/******************************************************************************
**
** weaponguide.cpp
**
** Wed Feb 12 23:57:57 2003
** Linux 2.4.19-4GB (#1 Fri Sep 13 13:14:56 UTC 2002) i686
** martin@linux. (Martin Bickel)
**
** Definition of command line parser class
**
** Automatically created by genparse v0.5.2
**
** See http://genparse.sourceforge.net/ for details and updates
**
******************************************************************************/

#include <getopt.h>
#include <stdlib.h>
#include "weaponguide.h"

/*----------------------------------------------------------------------------
**
** Cmdline::Cmdline()
**
** Constructor method.
**
**--------------------------------------------------------------------------*/

Cmdline::Cmdline(int argc, char *argv[]) throw (string)
{
  extern char *optarg;
  extern int optind;
  int option_index = 0;
  int c;

  static struct option long_options[] =
  {
    {"configfile", 1, 0, 'c'},
    {"verbose", 1, 0, 'r'},
    {"directory", 1, 0, 'd'},
    {"linkdir", 1, 0, 'l'},
    {"set", 1, 0, 's'},
    {"image", 0, 0, 'i'},
    {"help", 0, 0, 'h'},
    {"version", 0, 0, 'v'},
    {0, 0, 0, 0}
  };

  _executable += argv[0];

  /* default values */
  _r = 0;
  _s = 0;
  _i = false;
  _h = false;
  _v = false;

  while ((c = getopt_long(argc, argv, "c:r:d:l:s:ihv", long_options, &option_index)) != EOF)
    {
      switch(c)
        {
        case 'c': 
          _c = optarg;
          break;

        case 'r': 
          _r = atoi(optarg);
          if (_r < 0)
            {
              string s;
              s += "parameter range error: r must be >= 0";
              throw(s);
            }
          if (_r > 10)
            {
              string s;
              s += "parameter range error: r must be <= 10";
              throw(s);
            }
          break;

        case 'd': 
          _d = optarg;
          break;

        case 'l': 
          _l = optarg;
          break;

        case 's': 
          _s = atoi(optarg);
          if (_s < 0)
            {
              string s;
              s += "parameter range error: s must be >= 0";
              throw(s);
            }
          break;

        case 'i': 
          _i = true;
          break;

        case 'h': 
          _h = true;
          this->usage();
          break;

        case 'v': 
          _v = true;
          break;

        default:
          this->usage();

        }
    } /* while */

  _optind = optind;
}

/*----------------------------------------------------------------------------
**
** Cmdline::usage()
**
** Usage function.
**
**--------------------------------------------------------------------------*/

void Cmdline::usage()
{
  cout << "generates html files that document ASCs units " << endl;
  cout << "usage: " << _executable << " [ -crdlsihv ]  vehicleFiles" << endl;
  cout << "  [ -c ] ";
  cout << "[ --configfile ]  ";
  cout << "(";
  cout << "type=";
  cout << "STRING";
  cout << ")\n";
  cout << "         Use given configuration file\n";
  cout << "  [ -r ] ";
  cout << "[ --verbose ]  ";
  cout << "(";
  cout << "type=";
  cout << "INTEGER,";
  cout << " range=0...10,";
  cout << " default=0";
  cout << ")\n";
  cout << "         Set verbosity level to x (0..10)\n";
  cout << "  [ -d ] ";
  cout << "[ --directory ]  ";
  cout << "(";
  cout << "type=";
  cout << "STRING";
  cout << ")\n";
  cout << "         place all output files in different directory\n";
  cout << "  [ -l ] ";
  cout << "[ --linkdir ]  ";
  cout << "(";
  cout << "type=";
  cout << "STRING";
  cout << ")\n";
  cout << "         relative directory for the menu links\n";
  cout << "  [ -s ] ";
  cout << "[ --set ]  ";
  cout << "(";
  cout << "type=";
  cout << "INTEGER,";
  cout << " range=0...,";
  cout << " default=0";
  cout << ")\n";
  cout << "         only use unitset with given ID\n";
  cout << "  [ -i ] ";
  cout << "[ --image ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         generate images for units (Linux only)\n";
  cout << "  [ -h ] ";
  cout << "[ --help ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         Display help information.\n";
  cout << "  [ -v ] ";
  cout << "[ --version ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         Output version.\n";
  exit(0);
}

