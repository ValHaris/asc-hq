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
#include "sigc++/signal.h"

#ifdef SIGC_CXX_NAMESPACES
namespace SigC
{
#endif

class SignalConnectionNode;
class SignalIterator_;

// Signal_ comes from Slot_, but it lacks a proxy until added by
// the holding Signal<>, which does so on a request of the user.
// Thus you must use the conversion operator on a Signal to get a Slot.


SignalNode::SignalNode()
  : SlotNode(0), exec_count_(0), begin_(0), end_(0)
  {
    reference();
  }

SignalNode::~SignalNode()
  { clear(); }

void SignalNode::clear()
  { 
    if (!exec_count_)
      {
        SignalConnectionNode *i=begin_,*j;
        begin_=end_=0;
        while (i)
          {
            j=i->next_;
            i->parent_ = 0;
            i->unreference();
            i=j;
          }
      }
    else
      {
        SignalConnectionNode *i=begin_;
        while (i)
          { 
            i->defered_=true;
            i->blocked_=true;
            i=i->next_;
          }
        defered_=true;
      }
  }

bool SignalNode::empty()
  { return !begin_; }

/*
void Signal_::exec_reference()
  {
    reference();
    exec_count_+=1;
  }

void Signal_::exec_unreference()
  {
    if (!--exec_count_)
      cleanup();
    unreference();
  }
*/

void SignalNode::cleanup()
  {
    if(!defered_)
      return;

    defered_ = false;

    SignalConnectionNode* i = begin_;

    while(begin_ && begin_->defered_)
      begin_ = begin_->next_;

    while(end_ && end_->defered_)
      end_ = end_->prev_;

    // Hmm, this looks a bit tricky to me.  The code above removes all
    // elements from the front and the back that have defered_ = true set.
    // But the following loop unreferences *all* defered elements, including
    // those that are still in the list.
    //
    // I don't know whether this is a bug -- it's possible that defered nodes
    // are always at the front or back.  If not, a node could be unreferenced
    // twice which is bad.
    //
    // --Daniel

    while(i)
    {
      SignalConnectionNode* next = i->next_;

      if(i->defered_)
      {
        i->parent_ = 0;
        i->unreference();
      }

      i = next;
    }
  }

SlotNode* SignalNode::create_slot(FuncPtr proxy) // nothrow
{
    proxy_=proxy;
    return this;
}

ConnectionNode* SignalNode::push_front(const SlotBase& s)
  { 
    SignalConnectionNode* c=new SignalConnectionNode((SlotNode*)(s.impl())); 
    c->reference();
    c->parent_=this;
    c->next_=begin_;
    if (begin_)
      begin_->prev_=c;
    else
      end_=c;
    end_=c;
    return c;
  }

ConnectionNode* SignalNode::push_back(const SlotBase& s)
  { 
    SignalConnectionNode* c=new SignalConnectionNode((SlotNode*)(s.impl())); 
    c->reference();
    c->parent_=this;
    c->prev_=end_;
    if (end_)
      end_->next_=c;
    else
      begin_=c;
    end_=c;
    return c;
  }

void SignalNode::remove(SignalConnectionNode* c)
  {
    if (!exec_count_)
      {
        if (c->prev_)
          c->prev_->next_=c->next_;
        else
          begin_=c->next_;
       
        if (c->next_)
          c->next_->prev_=c->prev_;
        else
          end_=c->prev_;

        c->parent_ = 0;
        c->unreference();
      }
    else
      {
        c->defered_=true;
        c->blocked_=true;
        defered_=true;
      }
  }



/**********************************************************/

SignalBase::SignalBase()
  : impl_(0)
  {}

SignalBase::SignalBase(const SignalBase& s)
  : impl_(s.impl())
  { 
    impl_->reference(); 
  }

SignalBase::SignalBase(SignalNode* s)
  : impl_(s)
  { 
    if (s) 
      impl_->reference(); 
  }

SignalBase::~SignalBase()
  { 
    if (impl_) 
      impl_->unreference(); 
  }

SignalNode* SignalBase::impl() const 
  {
    if (!impl_) 
      impl_=new SignalNode();
    return impl_;
  }

/**********************************************************/

void SignalConnectionNode::notify(bool from_child)
  {
    Node n (this);
    ConnectionNode::notify(from_child);

    if(parent_) // this node might have been removed already
      parent_->remove(this); 
  }

SignalConnectionNode::~SignalConnectionNode()
  { }
SignalConnectionNode::SignalConnectionNode(SlotNode* s)
  : ConnectionNode(s), parent_(0), next_(0), prev_(0)
  { }

#ifdef SIGC_CXX_NAMESPACES
}
#endif

