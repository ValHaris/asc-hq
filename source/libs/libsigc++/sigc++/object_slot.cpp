// Copyright 2000, Karl Einar Nelson
#include "sigc++/object_slot.h"

#ifdef SIGC_CXX_NAMESPACES
namespace SigC
{
#endif

NodeBase::Link* ObjectSlotNode::link()
  { return &link_; }

void ObjectSlotNode::init(Object* control, void* object, Method method)
  {
    control_=control->control(); 
    object_=object;
    method_=method;
    control_->add_dependency(this);
  }

void ObjectSlotNode::notify(bool from_child)
  {
    if (!from_child)
      control_->remove_dependency(this);
    SlotNode::notify(from_child);
  }

ObjectSlotNode::~ObjectSlotNode()
  {
    if (!notified_)
      control_->remove_dependency(this);
  }

#ifdef SIGC_CXX_NAMESPACES
}
#endif


