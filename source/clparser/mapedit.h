/******************************************************************************
**
** mapedit.h
**
** Thu May 17 01:05:09 2001
** Linux 2.4.4 (#1 SMP Sam Apr 28 13:21:30 CEST 2001) i686
** martin@linux. (Martin Bickel)
**
** Header file for command line parser class
**
** Automatically created by genparse v0.5.2
**
** See http://genparse.sourceforge.net/ for details and updates
**
******************************************************************************/

#ifndef CMDLINE_H
#define CMDLINE_H

#include <iostream>
#include <string>

/*----------------------------------------------------------------------------
**
** class Cmdline
**
** command line parser class
**
**--------------------------------------------------------------------------*/

class Cmdline
{
private:
  /* parameters */
  int _x;
  int _y;
  string _l;
  string _c;
  int _v;
  bool _w;
  bool _f;
  bool _version;
  bool _h;

  /* other stuff to keep track of */
  string _executable;
  int _optind;

public:
  /* constructor and destructor */
  Cmdline(int, char **) throw(string);
  ~Cmdline(){}

  /* usage function */
  void usage();

  /* return next (non-option) parameter */
  int next_param() { return _optind; }

  /* callback functions */

  int x() { return _x; }
  int y() { return _y; }
  string l() { return _l; }
  string c() { return _c; }
  int v() { return _v; }
  bool w() { return _w; }
  bool f() { return _f; }
  bool version() { return _version; }
  bool h() { return _h; }
};

#endif
