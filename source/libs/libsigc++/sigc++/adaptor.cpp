// -*- c++ -*-
/*  
  Copyright 2000, Karl Einar Nelson

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/
#include "sigc++/adaptor.h"

#ifdef SIGC_CXX_NAMESPACES
namespace SigC
{
#endif

AdaptorSlotNode::AdaptorSlotNode(FuncPtr proxy,const Node& s)
  : SlotNode(proxy), slot_(s)
  {
    static_cast<SlotNode*>(slot_.impl())->add_dependency(this);
  }


void AdaptorSlotNode::notify(bool from_child)
  {
    if (from_child)
      slot_.clear();
    else
      static_cast<SlotNode*>(slot_.impl())->remove_dependency(this);
    SlotNode::notify(from_child);
  }

NodeBase::Link* AdaptorSlotNode::link()
  { return &link_; }

AdaptorSlotNode::~AdaptorSlotNode()
  {
    if (!notified_)
      static_cast<SlotNode*>(slot_.impl())->remove_dependency(this);
    slot_.clear();
  }

#ifdef SIGC_CXX_NAMESPACES
}
#endif

