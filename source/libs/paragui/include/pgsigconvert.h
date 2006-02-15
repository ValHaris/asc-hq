/*
	libSigCX -libSigC++ Extras 
 
 
Distribution of library and components is under the LGPL as listed in the
file COPYING.  Examples and tests are Public Domain.
 
Contact info:
  Maintainer:   mailto: rottmann@users.sourceforge.net
  Mailing List: mailto: libsigx-main@lists.sourceforge.net
  Homepage:     http://libsigx.sourceforge.net
  Download:     http://sourceforge.net/project/showfiles.php?group_id=58659
*/

#ifndef   SIGC_CONVERT_H
#define   SIGC_CONVERT_H
#include <sigc++/adaptor.h>
#include "SDL.h"

/*
  SigC::convert
  -------------
  convert() alters a Slot by assigning a conversion function 
  which can completely alter the parameter types of a slot. 
 
  Only convert functions for changing with same number of
  arguments is compiled by default.
 
  Sample usage:
    int my_string_to_char(Slot2<int,const char*> &d,const string &s)
    int f(const char*);
    string s=hello;
 
 
    Slot1<int,const string &>  s2=convert(slot(&f),my_string_to_char);
    s2(s);  
 
*/

#ifdef SIGC_CXX_NAMESPACES
namespace SigCX {
#endif

// (internal)
struct DECLSPEC AdaptorConvertSlotNode : public SigC::AdaptorSlotNode {
	SigC::FuncPtr convert_func_;

	AdaptorConvertSlotNode(SigC::FuncPtr proxy, const SigC::Node& s, SigC::FuncPtr dtor);

	virtual ~AdaptorConvertSlotNode();
};





template <class R,class T>
struct AdaptorConvertSlot0_ {
	typedef typename SigC::Trait<R>::type RType;
	typedef R (*ConvertFunc)(T&);
	static RType proxy(void *data) {
		AdaptorConvertSlotNode& node=*(AdaptorConvertSlotNode*)(data);
		T &slot_=(T&)(node.slot_);
		return ((ConvertFunc)(node.convert_func_))
		       (slot_);
	}
};

template <class R,class T>
SigC::Slot0<R>
convert(const T& slot_, R (*convert_func)(T&)) {
	return new AdaptorConvertSlotNode((SigC::FuncPtr)(&AdaptorConvertSlot0_<R,T>::proxy),
	                                  slot_,
	                                  (SigC::FuncPtr)(convert_func));
}


template <class R,class P1,class T>
struct AdaptorConvertSlot1_ {
	typedef typename SigC::Trait<R>::type RType;
	typedef R (*ConvertFunc)(T&,P1);
	static RType proxy(typename SigC::Trait<P1>::ref p1,void *data) {
		AdaptorConvertSlotNode& node=*(AdaptorConvertSlotNode*)(data);
		T &slot_=(T&)(node.slot_);
		return ((ConvertFunc)(node.convert_func_))
		       (slot_,p1);
	}
};

template <class R,class P1,class T>
SigC::Slot1<R,P1>
convert(const T& slot_, R (*convert_func)(T&,P1)) {
	return new AdaptorConvertSlotNode((SigC::FuncPtr)(&AdaptorConvertSlot1_<R,P1,T>::proxy),
	                                  slot_,
	                                  (SigC::FuncPtr)(convert_func));
}


template <class R,class P1,class P2,class T>
struct AdaptorConvertSlot2_ {
	typedef typename SigC::Trait<R>::type RType;
	typedef R (*ConvertFunc)(T&,P1,P2);
	static RType proxy(typename SigC::Trait<P1>::ref p1,typename SigC::Trait<P2>::ref p2,void *data) {
		AdaptorConvertSlotNode& node=*(AdaptorConvertSlotNode*)(data);
		T &slot_=(T&)(node.slot_);
		return ((ConvertFunc)(node.convert_func_))
		       (slot_,p1,p2);
	}
};

template <class R,class P1,class P2,class T>
SigC::Slot2<R,P1,P2>
convert(const T& slot_, R (*convert_func)(T&,P1,P2)) {
	return new AdaptorConvertSlotNode((SigC::FuncPtr)(&AdaptorConvertSlot2_<R,P1,P2,T>::proxy),
	                                  slot_,
	                                  (SigC::FuncPtr)(convert_func));
}


template <class R,class P1,class P2,class P3,class T>
struct AdaptorConvertSlot3_ {
	typedef typename SigC::Trait<R>::type RType;
	typedef R (*ConvertFunc)(T&,P1,P2,P3);
	static RType proxy(typename SigC::Trait<P1>::ref p1,typename SigC::Trait<P2>::ref p2,typename SigC::Trait<P3>::ref p3,void *data) {
		AdaptorConvertSlotNode& node=*(AdaptorConvertSlotNode*)(data);
		T &slot_=(T&)(node.slot_);
		return ((ConvertFunc)(node.convert_func_))
		       (slot_,p1,p2,p3);
	}
};

template <class R,class P1,class P2,class P3,class T>
SigC::Slot3<R,P1,P2,P3>
convert(const T& slot_, R (*convert_func)(T&,P1,P2,P3)) {
	return new AdaptorConvertSlotNode((SigC::FuncPtr)(&AdaptorConvertSlot3_<R,P1,P2,P3,T>::proxy),
	                                  slot_,
	                                  (SigC::FuncPtr)(convert_func));
}


template <class R,class P1,class P2,class P3,class P4,class T>
struct AdaptorConvertSlot4_ {
	typedef typename SigC::Trait<R>::type RType;
	typedef R (*ConvertFunc)(T&,P1,P2,P3,P4);
	static RType proxy(typename SigC::Trait<P1>::ref p1,typename SigC::Trait<P2>::ref p2,typename SigC::Trait<P3>::ref p3,typename SigC::Trait<P4>::ref p4,void *data) {
		AdaptorConvertSlotNode& node=*(AdaptorConvertSlotNode*)(data);
		T &slot_=(T&)(node.slot_);
		return ((ConvertFunc)(node.convert_func_))
		       (slot_,p1,p2,p3,p4);
	}
};

template <class R,class P1,class P2,class P3,class P4,class T>
SigC::Slot4<R,P1,P2,P3,P4>
convert(const T& slot_, R (*convert_func)(T&,P1,P2,P3,P4)) {
	return new AdaptorConvertSlotNode((SigC::FuncPtr)(&AdaptorConvertSlot4_<R,P1,P2,P3,P4,T>::proxy),
	                                  slot_,
	                                  (SigC::FuncPtr)(convert_func));
}


template <class R,class P1,class P2,class P3,class P4,class P5,class T>
struct AdaptorConvertSlot5_ {
	typedef typename SigC::Trait<R>::type RType;
	typedef R (*ConvertFunc)(T&,P1,P2,P3,P4,P5);
	static RType proxy(typename SigC::Trait<P1>::ref p1,typename SigC::Trait<P2>::ref p2,typename SigC::Trait<P3>::ref p3,typename SigC::Trait<P4>::ref p4,typename SigC::Trait<P5>::ref p5,void *data) {
		AdaptorConvertSlotNode& node=*(AdaptorConvertSlotNode*)(data);
		T &slot_=(T&)(node.slot_);
		return ((ConvertFunc)(node.convert_func_))
		       (slot_,p1,p2,p3,p4,p5);
	}
};

template <class R,class P1,class P2,class P3,class P4,class P5,class T>
SigC::Slot5<R,P1,P2,P3,P4,P5>
convert(const T& slot_, R (*convert_func)(T&,P1,P2,P3,P4,P5)) {
	return new AdaptorConvertSlotNode((SigC::FuncPtr)(&AdaptorConvertSlot5_<R,P1,P2,P3,P4,P5,T>::proxy),
	                                  slot_,
	                                  (SigC::FuncPtr)(convert_func));
}


template <class R,class P1,class P2,class P3,class P4,class P5,class P6,class T>
struct AdaptorConvertSlot6_ {
	typedef typename SigC::Trait<R>::type RType;
	typedef R (*ConvertFunc)(T&,P1,P2,P3,P4,P5,P6);
	static RType proxy(typename SigC::Trait<P1>::ref p1,typename SigC::Trait<P2>::ref p2,typename SigC::Trait<P3>::ref p3,typename SigC::Trait<P4>::ref p4,typename SigC::Trait<P5>::ref p5,typename SigC::Trait<P6>::ref p6,void *data) {
		AdaptorConvertSlotNode& node=*(AdaptorConvertSlotNode*)(data);
		T &slot_=(T&)(node.slot_);
		return ((ConvertFunc)(node.convert_func_))
		       (slot_,p1,p2,p3,p4,p5,p6);
	}
};

template <class R,class P1,class P2,class P3,class P4,class P5,class P6,class T>
SigC::Slot6<R,P1,P2,P3,P4,P5,P6>
convert(const T& slot_, R (*convert_func)(T&,P1,P2,P3,P4,P5,P6)) {
	return new AdaptorConvertSlotNode((SigC::FuncPtr)(&AdaptorConvertSlot6_<R,P1,P2,P3,P4,P5,P6,T>::proxy),
	                                  slot_,
	                                  (SigC::FuncPtr)(convert_func));
}



#ifdef SIGC_CXX_NAMESPACES
}
#endif



#endif // SIGC_CONVERT_H
