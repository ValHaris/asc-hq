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
#include "sigc++/object_slot.h"

#ifdef SIGC_CXX_NAMESPACES
namespace SigC
{
#endif

int sigc_major_version = SIGC_MAJOR_VERSION;
int sigc_minor_version = SIGC_MINOR_VERSION;
int sigc_micro_version = SIGC_MICRO_VERSION;

Control_::Control_(const ObjectBase* object)
  : object_(object), dep_(0), count_(0), ccount_(1), manage_(0)
  {}

Control_::~Control_()
{}

void Control_::cref()
{
  ccount_ += 1;
}

void Control_::cunref()
{
  if (!--ccount_)
    delete this;
}

void Control_::ref()
{
  count_++;
  cref();
}

void Control_::unref()
{
  if ( !(--count_) && manage_ )
    delete object_;

  cunref();
}

void Control_::destroy()
  { 
    object_ = 0;
    manage_ = 0;
    NodeBase* n = dep_;
    while (n)
      {
        NodeBase* next = n->link()->next_;
        n->notify(false);
        n = next;
      }
    dep_ = 0;
    cunref(); 
  }

void Control_::add_dependency(NodeBase* node)
  {
    NodeBase::Link* link= node->link();
    if (!link) return;
    link->next_ =dep_;
    link->prev_ = 0;
    if (dep_)
      dep_->link()->prev_ = node;
    dep_ = node;
  }

void Control_::remove_dependency(NodeBase* node)
  {
    NodeBase::Link *link= node->link();
    NodeBase::Link *nlink,*plink;
    if (dep_ == node)
      dep_ = link->next_;

    if (link->next_)
      {
        nlink = link->next_->link();
        nlink->prev_ = link->prev_;
      }

    if (link->prev_)
      {
        plink = link->prev_->link();
        plink->next_ = link->next_;
      }
  }

Object::Object()
  {}
Object::~Object()
  {}

ObjectBase::~ObjectBase()
  {
    if (control_) 
      control_->destroy();
    control_ = 0;
  }

void ObjectBase::add_dependency(NodeBase* n)
  {
    if (!n)
      return;

    if (!control_) 
      control_ = new Control_(this);

    control_->add_dependency(n);
  }

void ObjectBase::remove_dependency(NodeBase* n)
  {
    if (!n)
     return;

    if (!control_) 
      control_ = new Control_(this);

    control_->remove_dependency(n);
  }

void ObjectBase::reference() const
  {
    if (!control_)
      control_ = new Control_(this);
    control_->ref();
  }

void ObjectBase::unreference() const
  {
    if (!control_) 
      control_ = new Control_(this);
    control_->unref();
  }

void ObjectBase::set_manage()
  {
    if (!control_) 
      control_ = new Control_(this);
    control_->manage_ = 1;
  }

#ifdef SIGC_CXX_NAMESPACES
}
#endif

