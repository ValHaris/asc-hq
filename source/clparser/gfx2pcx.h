/******************************************************************************
**
** gfx2pcx.h
**
** Thu May 17 15:54:29 2001
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
  string _c;
  int _r;
  bool _legend;
  bool _wide;
  bool _orgpal;
  bool _nousage;
  bool _doublen;
  bool _doublei;
  int _id;
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
  bool legend() { return _legend; }
  bool wide() { return _wide; }
  bool orgpal() { return _orgpal; }
  bool nousage() { return _nousage; }
  bool doublen() { return _doublen; }
  bool doublei() { return _doublei; }
  int id() { return _id; }
  bool h() { return _h; }
  bool v() { return _v; }
};

#endif
