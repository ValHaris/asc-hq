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
#include "sigc++/connection.h"

#ifdef SIGC_CXX_NAMESPACES
namespace SigC
{
#endif

ConnectionNode::~ConnectionNode()
  {
    if (!notified_)
      slot()->remove_dependency(this);
  }

ConnectionNode::ConnectionNode(SlotNode* s)
  : NodeBase(), slot_(s)
  {
    if (s)
       slot()->add_dependency(this);
  }

NodeBase::Link* ConnectionNode::link()
  {
    return &link_;
  }

bool ConnectionNode::block(bool should_block) 
  { 
    bool old = bool(blocked_);
    blocked_ = should_block;
    return old;
  }

void ConnectionNode::notify(bool from_child)
  {
    blocked_ = true;

    if(!from_child && !notified_)
      slot()->remove_dependency(this);

    slot_.clear();
    NodeBase::notify(from_child);
  }

/*************************************************/

void Connection::disconnect()
  {
    if (node_)
      obj()->notify(false); //false = not from_child.

    clear();
  }

bool Connection::blocked() const
  {
    if (!valid())
      return false;

    return obj()->blocked();
  }

bool Connection::block(bool should_block)    
  {
    if (!valid())
      return false;

    return obj()->block(should_block);
  }

#ifdef SIGC_CXX_NAMESPACES
}
#endif


