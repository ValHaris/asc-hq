// Copyright 2000, Karl Einar Nelson
#include "sigc++/method_slot.h"

#ifdef SIGC_CXX_NAMESPACES
namespace SigC
{
#endif

void MethodSlotNode::init(Method method)
  { method_=method; }

MethodSlotNode::~MethodSlotNode()
  {}

void ConstMethodSlotNode::init(Method method)
  { method_=method; }

ConstMethodSlotNode::~ConstMethodSlotNode()
  {}

#ifdef SIGC_CXX_NAMESPACES
}
#endif


