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
#include "sigc++/node.h"

#ifdef SIGC_CXX_NAMESPACES
namespace SigC
{
#endif

NodeBase::Link* NodeBase::link()
  {
    return 0;
  }


void NodeBase::notify(bool /*from_child*/)
  {
    notified_=1;
  }

NodeBase::NodeBase()
  : count_(0), notified_(0), blocked_(0), defered_(0)
  {}

NodeBase::~NodeBase()
  {}

/**************************************************************/

bool Node::valid() const
  {
    if (node_)
      if (!node_->notified_)
        return true;
      else
        clear();
    return false;
  }

void Node::assign(NodeBase* node)
  {
    node_=node;
    if (node_) 
      node_->reference();
  }

void Node::clear() const
  {
    if (node_) 
      node_->unreference();
    node_=0;
  }

Node& Node::operator =(const Node& node)
  {
    node.valid();
    if (node_==node.node_) 
      return *this;
    clear();
    assign(node.node_);
    return *this;
  }

#ifdef SIGC_CXX_NAMESPACES
}
#endif


