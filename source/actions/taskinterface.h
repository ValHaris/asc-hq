/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.org
     Copyright (C) 1994-2010  Martin Bickel 
 
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

#ifndef taskInterfaceH
#define taskInterfaceH

class Context;

class TaskInterface {
   public:
      virtual int getCompletion() = 0;
      
      /** checks if the task can still be operated.
          If the task's unit was shot down for example, this would be the method to detect it and return false */
      virtual bool operatable() = 0;
      virtual void rearm() = 0;
   
      virtual ~TaskInterface () {};
};

#endif
