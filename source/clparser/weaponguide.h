/******************************************************************************
**
** weaponguide.h
**
** Wed Feb 12 23:57:57 2003
** Linux 2.4.19-4GB (#1 Fri Sep 13 13:14:56 UTC 2002) i686
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
  string _c;
  int _r;
  string _d;
  string _l;
  int _s;
  bool _i;
  bool _h;
  bool _v;

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

  string c() { return _c; }
  int r() { return _r; }
  string d() { return _d; }
  string l() { return _l; }
  int s() { return _s; }
  bool i() { return _i; }
  bool h() { return _h; }
  bool v() { return _v; }
};

#endif
