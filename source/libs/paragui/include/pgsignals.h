/*
    ParaGUI - crossplatform widgetset
    Copyright (C) 2000,2001,2002  Alexander Pipelka
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
 
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
    Alexander Pipelka
    pipelka@teleweb.at
 
    Last Update:      $Author: mbickel $
    Update Date:      $Date: 2009-04-18 13:48:39 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgsignals.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

/** \file pgsignals.h
	Header file for the PG_Signal0, PG_Signal1 and PG_Signal2 classes.
*/

#ifndef PG_SIGNALS_H
#define PG_SIGNALS_H

#include <sigc++/sigc++.h>
#include "pgsigconvert.h"


typedef void* PG_Pointer;

template<class datatype = PG_Pointer>
class PG_Signal0 : public SigC::Signal0<bool> {
public:

	SigC::Connection connect(const SigC::Slot1<bool, datatype>& s, datatype data) {
		return SigC::Signal0<bool>::connect(bind(s, data));
	};

};

template<class P1, class datatype = PG_Pointer>
class PG_Signal1 : public SigC::Signal1<bool, P1> {
	static bool sig_convert0(SigC::Slot0<bool>& s, P1 p1) {
		return s();
	}

public:

	SigC::Connection connect(const SigC::Slot2<bool, P1, datatype>& s, datatype data) {
		return SigC::Signal1<bool, P1>::connect(bind(s, data));
	};

	SigC::Connection connect(const SigC::Slot1<bool, datatype>& s, datatype data) {
		return connect(bind(s, data));
	}

	SigC::Connection connect(const SigC::Slot1<bool, P1>& s) {
		return SigC::Signal1<bool, P1>::connect(s);
	}

	SigC::Connection connect(const SigC::Slot0<bool>& s) {
		return SigC::Signal1<bool, P1>::connect( SigCX::convert(s, sig_convert0));
	}

	PG_Signal1& operator=(const PG_Signal1&);
};


template<class P1, class P2, class datatype = PG_Pointer>
class PG_Signal2 : public SigC::Signal2<bool, P1, P2> {

	static bool sig_convert_p2( SigC::Slot1<bool, P2>& s, P1 p1, P2 p2) {
		return s(p2);
	}

	static bool sig_convert_p1( SigC::Slot1<bool, P1>& s, P1 p1, P2 p2) {
		return s(p1);
	}

	static bool sig_convert0( SigC::Slot0<bool>& s, P1 p1, P2 p2) {
		return s();
	}

public:

	SigC::Connection connect(const SigC::Slot3<bool, P1, P2, datatype>& s, datatype data) {
		return SigC::Signal2<bool, P1, P2>::connect(bind(s, data));
	}

	SigC::Connection connect(const SigC::Slot2<bool, P1, datatype>& s, datatype data) {
		return SigC::Signal2<bool, P1, P2>::connect(bind(s, data));
	};

	SigC::Connection connect(const SigC::Slot2<bool, P1, P2>& s) {
		return SigC::Signal2<bool, P1, P2>::connect(s);
	}

	SigC::Connection connect(const SigC::Slot1<bool, P2>& s) {
		return SigC::Signal2<bool, P1, P2>::connect( SigCX::convert(s, sig_convert_p2));
	}

	SigC::Connection connect(const SigC::Slot1<bool, P1>& s) {
		return SigC::Signal2<bool, P1, P2>::connect( SigCX::convert(s, sig_convert_p2));
	}

	SigC::Connection connect(const SigC::Slot0<bool>& s) {
		return SigC::Signal2<bool, P1, P2>::connect( SigCX::convert(s, sig_convert0));
	}

private:

	PG_Signal2& operator=(const PG_Signal2&);

};

#endif // PG_SIGNALS_H
