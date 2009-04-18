/************************************************************************
 * $Id: ystring.cpp,v 1.3 2009-04-18 13:48:40 mbickel Exp $
 *
 * ------------
 * Description:
 * ------------
 * ystring.cpp
 *
 * A unicode string class
 *
 * (C) Copyright 2004 Arabeyes, Mohammed Yousif
 *
 * -----------------
 * Revision Details:    (Updated by Revision Control System)
 * -----------------
 *  $Date: 2009-04-18 13:48:40 $
 *  $Author: mbickel $
 *  $Revision: 1.3 $
 *  $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/ystring/ystring.cpp,v $
 *
 *  (www.arabeyes.org - under GPL License)
 *
 ************************************************************************/

#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

#include "paragui.h"

#ifdef ENABLE_UNICODE

#include "ychar.h"
#include "ystring.h"

YString::YString() {}

YString::YString(const std::string & s, YString::size_type pos, YString::size_type n) {
	*this = fromUtf8(std::string(s, pos, n));
}

YString::YString(const char * str) {
	*this = fromUtf8(str);
}

YString::YString(const char * str, size_type n) {
	*this = fromUtf8(std::string(str, n));
}

YString::YString(size_type n, char c) {
	unicode.insert(unicode.begin(), n, YChar(c));
}

template <class InputIterator>
YString::YString(InputIterator first, InputIterator last) {
	unicode.insert(unicode.begin(), first, last);
}

YString::YString(const YString & s, YString::size_type pos, YString::size_type n) throw(std::out_of_range) {
	if (n == YString::npos)
		n = s.length();

	if (pos >= n) {
		PG_THROW(std::out_of_range("index out of range"));
		return;
	}

	unicode.reserve(n);
	for (YString::size_type i=pos; i<n; i++)
		unicode.push_back(s.unicode[i]);
}

YString::YString(const std::vector<YChar> arr) {
	unicode = arr;
}

YString::YString(size_type n, YChar c) {
	unicode.insert(unicode.begin(), n, c);
}

YString::YString(const YChar c) {
	unicode.push_back(c);
}

YString::iterator YString::begin() {
	return unicode.begin();
}

YString::iterator YString::end() {
	return unicode.end();
}

YString::const_iterator YString::begin() const {
	return unicode.begin();
}

YString::const_iterator YString::end() const {
	return unicode.end();
}

YString::reverse_iterator YString::rbegin() {
	return unicode.rbegin();
}

YString::reverse_iterator YString::rend() {
	return unicode.rend();
}

YString::const_reverse_iterator YString::rbegin() const {
	return unicode.rbegin();
}

YString::const_reverse_iterator YString::rend() const {
	return unicode.rend();
}

YString::size_type YString::size() const {
	return unicode.size();
}

YString::size_type YString::max_size() const {
	return unicode.max_size();
}

YString::size_type YString::capacity() const {
	return unicode.capacity();
}

bool YString::empty() const {
	return (unicode.size() == 0);
}

YString::reference YString::operator[](YString::size_type n) {
	return unicode[n];
}

YString::const_reference YString::operator[](YString::size_type n) const {
	return unicode[n];
}

const char * YString::c_str() const {
	return utf8().c_str();
}

const char * YString::data() const {
	return utf8().data();
}

YString & YString::operator=(const YString & rhs) {
	unicode = rhs.unicode;
	return *this;
}

YString & YString::operator=(const std::string & rhs) {
	return *this = fromUtf8(rhs);
}

YString & YString::operator=(const char * rhs) {
	return *this = fromUtf8(rhs);
}

YString & YString::operator=(char rhs) {
	this->clear();
	this->push_back(YChar(rhs));
	return *this;
}

YString & YString::operator=(YChar rhs) {
	this->clear();
	this->push_back(rhs);
	return *this;
}

YString & YString::operator=(uint32 rhs) {
	this->clear();
	this->push_back(YChar(rhs));
	return *this;
}

void YString::reserve(YString::size_type n) {
	unicode.reserve(n);
}

void YString::swap(YString & s) {
	unicode.swap(s.unicode);
}

YString::iterator YString::insert(YString::iterator pos, const YChar & c) {
	return unicode.insert(pos, c);
}

template <class InputIterator>
void YString::insert(YString::iterator pos, InputIterator first, InputIterator last) {
	unicode.insert(pos, first, last);
}

void YString::insert(YString::iterator pos, YString::size_type n, const YChar & c) {
	unicode.insert(pos, n, c);
}

YString & YString::insert(YString::size_type pos, const YString & s) {
	insert(begin() + pos, s.begin(), s.end());
	return *this;
}

YString & YString::insert(YString::size_type pos, const YString & s,
                          YString::size_type pos1, YString::size_type n) {
	insert(begin() + pos, s.begin() + pos1, s.begin() + pos1 + n);
	return *this;
}

YString & YString::insert(YString::size_type pos, const char * s) {
	return insert(pos, YString(s));
}

YString & YString::insert(YString::size_type pos, const char * s, YString::size_type n) {
	return insert(pos, YString(s, n));
}

YString & YString::insert(YString::size_type pos, YString::size_type n, YChar c) {
	return insert(pos, YString(n, c));
}

YString & YString::insert(YString::size_type pos, YString::size_type n, char c) {
	return insert(pos, YString(n, c));
}

YString::iterator YString::erase(YString::iterator pos) {
	return unicode.erase(pos);
}

YString::iterator YString::erase(YString::iterator first, YString::iterator last) {
	return unicode.erase(first, last);
}

YString & YString::erase(YString::size_type pos, YString::size_type n) throw(std::out_of_range) {
	if (pos > size()) {
		PG_THROW(std::out_of_range("index out of range"));
		return *this;
	}
	if (n > size() - pos)
		n = size() - pos;
	erase(begin() + pos, begin() + pos + n);
	return *this;
}

YString::size_type YString::find(YChar c, YString::size_type pos) throw(std::out_of_range) {
	if (pos >= length()) {
		PG_THROW(std::out_of_range("index out of range"));
		return YString::npos;
	}

	YString::iterator result = std::find(unicode.begin() + pos, unicode.end() , c);
	if (result == end())
		return YString::npos;
	else
		return result - begin();
}

void YString::push_back(const YChar & c) {
	unicode.push_back(c);
}

void YString::pop_back() {
	unicode.pop_back();
}

void YString::clear() {
	unicode.clear();
}

YString::size_type YString::length() const {
	return unicode.size();
}

YString::size_type YString::bytes() const {
	YString::size_type len = 0;
	for (YString::size_type i = 0; i<length(); i++)
		len += unicode[i].bytes();
	return len;
}
/*
YString YString::lower() const
{
  // TODO
}
 
YString YString::upper() const
{
  // TODO
}
*/

YString YString::reverse() const {
	YString reversed(*this);
	std::reverse(reversed.unicode.begin(), reversed.unicode.end());
	return YString(reversed);
}

YChar YString::at(YString::size_type index) const throw(std::out_of_range) {
	if (index >=unicode.size() || index < 0) {
		PG_THROW(std::out_of_range("the provided character index is out of range"));
		return '\0';
	}
	return unicode[index];
}

void YString::setAt(YString::size_type index, YChar c) throw(std::out_of_range) {
	if (index >= unicode.size() || index < 0)
		PG_THROW(std::out_of_range("the provided character index is out of range"));
	else
		unicode[index] = c;
}

int YString::compareTo(const YString & rhs) const {
	return unicode == rhs.unicode;
}

YString YString::substr(YString::size_type index, YString::size_type len) const throw(std::out_of_range) {
	if (len == YString::npos)
		len = length() - index;
	if (len == 0)
		return "";
	YString::size_type end = len - 1;
	if (index >=length() || index < 0 ||
	        end >= length()  || end < 0) {
		PG_THROW(std::out_of_range("the provided character index/substring length is out of range"));
		return "";
	}

	YString::const_iterator start = begin() + index;
	return YString(start, start + len);
}

YString YString::fromUtf8(const char * utf8) throw(std::domain_error) {
	return fromUtf8(std::string(utf8));
}

YString YString::fromUtf8(const std::string& utf8) throw(std::domain_error) {
	YString buff;

	if(utf8.empty()) {
		return buff;
	}

	YString::size_type currentOctet = 0;
	do {
		YString::size_type noOctets = 1 + YChar::getNumberOfContinuingOctents(utf8[currentOctet]);
		if(noOctets == 0) {
			break;
		}
		YChar c;
		PG_TRY {
		    c = YChar::fromUtf8(utf8.substr(currentOctet, noOctets));
		} PG_CATCH (std::domain_error, e) {
			PG_THROW(e);
		}
		buff += c;
		currentOctet += noOctets;
	} while (currentOctet < utf8.length());
	return buff;
}

const std::string YString::utf8() const {
	std::string buff;
	for (YString::size_type i=0; i<unicode.size(); i++) {
		buff += unicode[i].utf8();
	}
	return buff;
}


YString::operator std::string() const {
	return utf8();
}
/*
YString::operator const char *() const
{
  // TODO
}
*/



YString & YString::operator+=(const YString & rhs) {
	unicode.insert(unicode.end(), rhs.unicode.begin(), rhs.unicode.end());
	return *this;
}

YString & YString::operator+=(const std::string & rhs) {
	YString rhsStr(rhs);
	return *this += rhsStr;
}

YString & YString::operator+=(const char * rhs) {
	YString rhsStr(rhs);
	return *this += rhsStr;
}

YString & YString::operator+=(const YChar rhs) {
	unicode.push_back(rhs);
	return *this;
}

YString & YString::operator+=(const char rhs) {
	unicode.push_back(YChar(rhs));
	return *this;
}

YString & YString::operator+=(const uint32 rhs) {
	unicode.push_back(YChar(rhs));
	return *this;
}

bool operator==(const YString & lhs, const YString & rhs) {
	return lhs.compareTo(rhs) == 0;
}

bool operator==(const YString & lhs, const char * rhs) {
	return lhs.compareTo(YString(rhs)) == 0;
}

bool operator==(const char * lhs, const YString & rhs) {
	return YString(lhs).compareTo(rhs) == 0;
}

bool operator!=(const YString & lhs, const YString & rhs) {
	return lhs.compareTo(rhs) != 0;
}

bool operator!=(const YString & lhs, const char * rhs) {
	return lhs.compareTo(YString(rhs)) != 0;
}

bool operator!=(const char * lhs, const YString & rhs) {
	return YString(lhs).compareTo(rhs) != 0;
}

bool operator<(const YString & lhs, const YString & rhs) {
	return lhs.compareTo(rhs) == -1;
}

bool operator<(const YString & lhs, const char * rhs) {
	return lhs.compareTo(YString(rhs)) == -1;
}

bool operator<(const char * lhs, const YString & rhs) {
	return YString(lhs).compareTo(rhs) == -1;
}

bool operator<=(const YString & lhs, const YString & rhs) {
	return lhs.compareTo(rhs) <= 0;
}

bool operator<=(const YString & lhs, const char * rhs) {
	return lhs.compareTo(YString(rhs)) <= 0;
}

bool operator<=(const char * lhs, const YString & rhs) {
	return YString(lhs).compareTo(rhs) <= 0;
}

bool operator>(const YString & lhs, const YString & rhs) {
	return lhs.compareTo(rhs) == 1;
}

bool operator>(const YString & lhs, const char * rhs) {
	return lhs.compareTo(YString(rhs)) == 1;
}

bool operator>(const char * lhs, const YString & rhs) {
	return YString(lhs).compareTo(rhs) == 1;
}

bool operator>=(const YString & lhs, const YString & rhs) {
	return lhs.compareTo(rhs) >= 0;
}

bool operator>=(const YString & lhs, const char * rhs) {
	return lhs.compareTo(YString(rhs)) >= 0;
}

bool operator>=(const char * lhs, const YString & rhs) {
	return YString(lhs).compareTo(rhs) >= 0;
}

YString operator+(const YString & lhs, const YString & rhs) {
	YString buff(lhs);
	buff.unicode.insert(buff.unicode.end(), rhs.unicode.begin(), rhs.unicode.end());
	return buff;
}

YString operator+(const YString & lhs, const char * rhs) {
	return lhs + YString(rhs);
}
YString operator+(const char * lhs, const YString & rhs) {
	return YString(lhs) + rhs;
}

YString operator+(const YString & lhs, const YChar rhs) {
	return lhs + YString(rhs.utf8());
}

YString operator+(const YChar lhs, const YString & rhs) {
	return YString(lhs.utf8()) + rhs;
}

YString operator+(const YString & lhs, const char rhs) {
	return lhs + YChar(rhs);
}

YString operator+(const char lhs, const YString & rhs) {
	return YChar(lhs) + rhs;
}

YString operator+(const YString & lhs, const uint32 rhs) {
	return lhs + YChar(rhs);
}

YString operator+(const uint32 lhs, const YString & rhs) {
	return YChar(lhs) + rhs;
}

YString operator+(const YChar lhs, const YChar rhs) {
	YString buff;
	buff.reserve(2);
	buff.push_back(lhs);
	buff.push_back(rhs);
	return buff;
}

YString operator+(const char * lhs, const YChar rhs) {
	return YString(lhs) + rhs;
}

YString operator+(const YChar lhs, const char * rhs) {
	return lhs + YString(rhs);
}

std::ostream & operator<<( std::ostream & out, YString & str) {
	for (YString::size_type i=0; i<str.length(); i++)
		out << str[i];
	return out;
}

std::istream & operator>>( std::istream & in, YString & str) {
	std::string buff;
	in >> buff;
	YString new_string(buff);
	str = new_string;
	return in;
}

#endif // ENABLE_UNICODE
