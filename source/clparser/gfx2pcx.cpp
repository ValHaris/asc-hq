/******************************************************************************
**
** gfx2pcx.cpp
**
** Thu May 17 15:54:29 2001
** Linux 2.4.4 (#1 SMP Sam Apr 28 13:21:30 CEST 2001) i686
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
#include "gfx2pcx.h"

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
    {"legend", 0, 0, 256},
    {"wide", 0, 0, 257},
    {"orgpal", 0, 0, 258},
    {"nousage", 0, 0, 259},
    {"doublen", 0, 0, 260},
    {"doublei", 0, 0, 261},
    {"id", 1, 0, 262},
    {"help", 0, 0, 'h'},
    {"version", 0, 0, 'v'},
    {0, 0, 0, 0}
  };

  _executable += argv[0];

  /* default values */
  _r = 0;
  _legend = false;
  _wide = false;
  _orgpal = false;
  _nousage = false;
  _doublen = false;
  _doublei = false;
  _id = 0;
  _h = false;
  _v = false;

  while ((c = getopt_long(argc, argv, "c:r:hv", long_options, &option_index)) != EOF)
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

        case 256: 
          _legend = true;
          break;

        case 257: 
          _wide = true;
          break;

        case 258: 
          _orgpal = true;
          break;

        case 259: 
          _nousage = true;
          break;

        case 260: 
          _doublen = true;
          break;

        case 261: 
          _doublei = true;
          break;

        case 262: 
          _id = atoi(optarg);
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
  cout << "output index picture containing all ASC graphics and their usage " << endl;
  cout << "usage: " << _executable << " [ -crhv ] " << endl;
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
  cout << "  [ --legend ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         Output legend\n";
  cout << "  [ --wide ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         Use wide output ( 10 columns )\n";
  cout << "  [ --orgpal ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         keep Battle Isle palette\n";
  cout << "  [ --nousage ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         output just the pictures\n";
  cout << "  [ --doublen ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         use scaled pictures without interpolation\n";
  cout << "  [ --doublei ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         use scaled pictures with interpolation\n";
  cout << "  [ --id ]  ";
  cout << "(";
  cout << "type=";
  cout << "INTEGER,";
  cout << " default=0";
  cout << ")\n";
  cout << "         use graphic set X\n";
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

