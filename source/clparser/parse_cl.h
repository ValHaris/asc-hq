/******************************************************************************
**
** parse_cl.h
**
** Thu May 17 12:48:59 2001
** Linux 2.4.4 (#1 SMP Sam Apr 28 13:21:30 CEST 2001) i686
** martin@linux. (Martin Bickel)
**
** Header file for command line parser
**
** Automatically created by genparse v0.5.2
**
** See http://genparse.sourceforge.net/ for details and updates
**
******************************************************************************/

#include <stdio.h>

#ifndef bool
typedef enum bool_t
{
  false = 0, true
} bool;
#endif

/* customized structure for command line parameters */
struct arg_t
{
  int v;
  bool h;
  int optind;
};

/* function prototypes */
struct arg_t * Cmdline(int, char **);
void usage(char *);
void free_args(struct arg_t *);

