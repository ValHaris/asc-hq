/******************************************************************************
**
** mount.cpp
**
** Thu May 17 14:56:46 2001
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
#include "mount.h"

/*----------------------------------------------------------------------------
**
** Cmdline::Cmdline()
**
** Constructor method.
**
**--------------------------------------------------------------------------*/

Cmdline::Cmdline(int argc, char *argv[]) throw (string)
{
  extern int optind;
  int option_index = 0;
  int c;

  static struct option long_options[] =
  {
    {"quiet", 0, 0, 'q'},
    {"help", 0, 0, 'h'},
    {"version", 0, 0, 'v'},
    {0, 0, 0, 0}
  };

  _executable += argv[0];

  /* default values */
  _q = false;
  _h = false;
  _v = false;

  while ((c = getopt_long(argc, argv, "qhv", long_options, &option_index)) != EOF)
    {
      switch(c)
        {
        case 'q': 
          _q = true;
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
  cout << "constructs an ASC archive from the data files. " << endl;
  cout << "usage: " << _executable << " [ -qhv ]  DATAFILE [...] ARCHIVEFILE" << endl;
  cout << "  [ -q ] ";
  cout << "[ --quiet ]  ";
  cout << "(";
  cout << "type=";
  cout << "FLAG";
  cout << ")\n";
  cout << "         Suppress informational output\n";
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

