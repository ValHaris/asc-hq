/******************************************************************************
**
** weaponguide.cpp
**
** Sat Aug 14 10:57:18 2004
** Linux 2.4.21-198-default (#1 Thu Mar 11 17:43:56 UTC 2004) i686
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
    {"imgsize", 1, 0, 'z'},
    {"framename", 1, 0, 'f'},
    {"style", 1, 0, 't'},
    {"menustyle", 1, 0, 'm'},
    {"allbuildings", 0, 0, 'b'},
    {"roottech", 1, 0, 256},
    {"writeall", 0, 0, 257},
    {"help", 0, 0, 'h'},
    {"version", 0, 0, 'v'},
    {0, 0, 0, 0}
  };

  _executable += argv[0];

  /* default values */
  _r = 0;
  _s = 0;
  _i = false;
  _z = 48;
  _f = "main";
  _t = "asc.css";
  _m = "asc.css";
  _b = false;
  _writeall = false;
  _h = false;
  _v = false;

  while ((c = getopt_long(argc, argv, "c:r:d:l:s:iz:f:t:m:bhv", long_options, &option_index)) != EOF)
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

        case 'z': 
          _z = atoi(optarg);
          if (_z < 0)
            {
              string s;
              s += "parameter range error: z must be >= 0";
              throw(s);
            }
          break;

        case 'f': 
          _f = optarg;
          break;

        case 't': 
          _t = optarg;
          break;

        case 'm': 
          _m = optarg;
          break;

        case 'b': 
          _b = true;
          break;

        case 256: 
          _roottech = optarg;
          break;

        case 257: 
          _writeall = true;
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
  cout << "generates html files that document ASCs units and buildings " << endl;
  cout << "usage: " << _executable << " [ -crdlsizftmbhv ]  vehicleFiles" << endl;
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
  cout << "  [ -z ] ";
  cout << "[ --imgsize ]  ";
  cout << "(";
  cout << "type=";
  cout << "INTEGER,";
  cout << " range=0...,";
  cout << " default=48";
  cout << ")\n";
  cout << "         size of unit images\n";
  cout << "  [ -f ] ";
  cout << "[ --framename ]  ";
  cout << "(";
  cout << "type=";
  cout << "STRING,";
  cout << " default=main";
  cout << ")\n";
  cout << "         name of the main frame\n";
  cout << "  [ -t ] ";
  cout << "[ --style ]  ";
  cout << "(";
  cout << "type=";
  cout << "STRING,";
  cout << " default=asc.css";
  cout << ")\n";
  cout << "         name of the main pages' stylesheet\n";
  cout << "  [ -m ] ";
  cout << "[ --menustyle ]  ";
  cout << "(";
  cout << "type=";
  cout << "STRING,";
  cout << " default=asc.css";
  cout << ")\n";
  cout << "         name of the menu stylesheet\n";
  cout << "  [ -b ] ";
  cout << "[ --allbuildings ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         generate doc for all buildings instead of uniquely named ones\n";
  cout << "  [ --roottech ]  ";
  cout << "(";
  cout << "type=";
  cout << "STRING";
  cout << ")\n";
  cout << "         specify root technologies for tech dependency\n";
  cout << "  [ --writeall ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         skip the check for changed files, which speeds up operation, but touches all files\n";
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

