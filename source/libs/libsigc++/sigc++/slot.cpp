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
#include "sigc++/slot.h"

#ifdef SIGC_CXX_NAMESPACES
namespace SigC
{
#endif

SlotNode::SlotNode(FuncPtr proxy)
  : NodeBase(), proxy_(proxy), dep_(0)
  {}

SlotNode::~SlotNode(void)
  {}

/*********************************************************/

void SlotNode::add_dependency(NodeBase* node)
  {
    if (!node) return;
    Link* link=node->link();
    if (!link) return;
    link->next_=dep_;
    link->prev_=0;
    if (dep_)
      dep_->link()->prev_=node;
    dep_=node;
  }

void SlotNode::remove_dependency(NodeBase* node)
  {
    if (!node) return;
    Link *link=node->link();
    Link *nlink,*plink;
    if (dep_==node)
      dep_=link->next_;
    if (link->next_)
      {
        nlink=link->next_->link();
        nlink->prev_=link->prev_;
      }
    if (link->prev_)
      {
        plink=link->prev_->link();
        plink->next_=link->next_;
      }
  }

void SlotNode::notify(bool from_child)
  {
    Node hold(this); 
    NodeBase::notify(from_child);
    NodeBase *n=dep_;
    while (n)
      {
        NodeBase* next = n->link()->next_;
        n->notify(true);
        n = next;
      } 
    dep_=0;
  }
   
/*********************************************************/

FuncSlotNode::FuncSlotNode(FuncPtr proxy,FuncPtr func)
  : SlotNode(proxy), func_(func)
  {}

FuncSlotNode::~FuncSlotNode() 
  {}

#ifdef SIGC_CXX_NAMESPACES
}
#endif


