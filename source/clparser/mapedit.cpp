/******************************************************************************
**
** mapedit.cpp
**
** Thu May 17 01:05:09 2001
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
#include "mapedit.h"

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
    {"xresolution", 1, 0, 'x'},
    {"yresolution", 1, 0, 'y'},
    {"load", 1, 0, 'l'},
    {"configfile", 1, 0, 'c'},
    {"verbose", 1, 0, 'v'},
    {"window", 0, 0, 'w'},
    {"fullscreen", 0, 0, 'f'},
    {"version", 0, 0, 256},
    {"help", 0, 0, 'h'},
    {0, 0, 0, 0}
  };

  _executable += argv[0];

  /* default values */
  _x = 800;
  _y = 600;
  _v = 0;
  _w = false;
  _f = false;
  _version = false;
  _h = false;

  while ((c = getopt_long(argc, argv, "x:y:l:c:v:wfh", long_options, &option_index)) != EOF)
    {
      switch(c)
        {
        case 'x': 
          _x = atoi(optarg);
          if (_x < 640)
            {
              string s;
              s += "parameter range error: x must be >= 640";
              throw(s);
            }
          break;

        case 'y': 
          _y = atoi(optarg);
          if (_y < 480)
            {
              string s;
              s += "parameter range error: y must be >= 480";
              throw(s);
            }
          break;

        case 'l': 
          _l = optarg;
          break;

        case 'c': 
          _c = optarg;
          break;

        case 'v': 
          _v = atoi(optarg);
          if (_v < 0)
            {
              string s;
              s += "parameter range error: v must be >= 0";
              throw(s);
            }
          if (_v > 10)
            {
              string s;
              s += "parameter range error: v must be <= 10";
              throw(s);
            }
          break;

        case 'w': 
          _w = true;
          break;

        case 'f': 
          _f = true;
          break;

        case 256: 
          _version = true;
          break;

        case 'h': 
          _h = true;
          this->usage();
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
  cout << "usage: " << _executable << " [ -xylcvwfh ] " << endl;
  cout << "  [ -x ] ";
  cout << "[ --xresolution ]  ";
  cout << "(";
  cout << "type=";
  cout << "INTEGER,";
  cout << " range=640...,";
  cout << " default=800";
  cout << ")\n";
  cout << "         Set horizontal resolution to <X>\n";
  cout << "  [ -y ] ";
  cout << "[ --yresolution ]  ";
  cout << "(";
  cout << "type=";
  cout << "INTEGER,";
  cout << " range=480...,";
  cout << " default=600";
  cout << ")\n";
  cout << "         Set vertical resolution to <Y>\n";
  cout << "  [ -l ] ";
  cout << "[ --load ]  ";
  cout << "(";
  cout << "type=";
  cout << "STRING";
  cout << ")\n";
  cout << "         Load a map on startup\n";
  cout << "  [ -c ] ";
  cout << "[ --configfile ]  ";
  cout << "(";
  cout << "type=";
  cout << "STRING";
  cout << ")\n";
  cout << "         Use given configuration file\n";
  cout << "  [ -v ] ";
  cout << "[ --verbose ]  ";
  cout << "(";
  cout << "type=";
  cout << "INTEGER,";
  cout << " range=0...10,";
  cout << " default=0";
  cout << ")\n";
  cout << "         Set verbosity level to x (0..10)\n";
  cout << "  [ -w ] ";
  cout << "[ --window ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         Disable fullscreen mode (overriding config file)\n";
  cout << "  [ -f ] ";
  cout << "[ --fullscreen ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         Enable fullscreen mode\n";
  cout << "  [ --version ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         Display version information and exit\n";
  cout << "  [ -h ] ";
  cout << "[ --help ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         Display help information.\n";
  exit(0);
}

