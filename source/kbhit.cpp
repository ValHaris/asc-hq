//     $Id: kbhit.cpp,v 1.1 1999-11-16 03:41:51 tmwilson Exp $
//
//     $Log: not supported by cvs2svn $
//
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

#include "config.h"
#ifndef HAVE_KBHIT

#include <stdlib.h>
#include <stdio.h>

#include <termios.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
static struct termios stored_settings;

void set_keypress(void)
{
  struct termios new_settings;
  
  tcgetattr(0,&stored_settings);
  
  new_settings = stored_settings;
  
  /* Disable canonical mode, and set buffer size to 1 byte */
  new_settings.c_lflag &= (~ICANON);
  new_settings.c_cc[VTIME] = 0;
  new_settings.c_cc[VMIN] = 1;
  
  tcsetattr(0,TCSANOW,&new_settings);
  return;
}

void reset_keypress(void)
{
  tcsetattr(0,TCSANOW,&stored_settings);
  return;
}

int kbhit (void)
{
  fd_set rfds;
  struct timeval tv;
  int retval;

  FD_ZERO(&rfds);
  FD_SET(0, &rfds);

  tv.tv_sec = 2;
  tv.tv_usec = 0;

  retval = select (1, &rfds, NULL, NULL, &tv);
  return (FD_ISSET(0, &rfds));
}

int getch (void)
{
  int status, key;

  status = read (0, &key, 1);
  if (status == -1)
    perror ("in getch:");
  return (key);
}

#endif
