/*
 * SDLmm - a C++ wrapper for SDL and related libraries
 * Copyright © 2001 David Hedbor <david@hedbor.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

// Description

#ifndef SDLMM_TIMER_H
#define SDLMM_TIMER_H

namespace SDLmm {
  class DECLSPEC Timer {
  public:
    typedef unsigned int Ticks;

    static bool Init();
    static void Quit();

    static Ticks GetTicks();

    Timer() : m_Tick(GetTicks()), m_Elapsed(0) {  }

    void Mark();

    unsigned int GetElapsed() const {
      return m_Elapsed;
    }

  private:
    Ticks  m_Tick;
    Ticks  m_Elapsed;
  };
}

#endif // SDLMM_TIMER_H
